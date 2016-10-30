/* entelnetd.cpp
 *
 * enTelnetd - Simple telnet server for Enthral BBS
 * Added IP Address Forwarding and Hostname Resolving
 * Adding NAWS and TTYPE Detection for Window resizing and Client TERM detection.
 *
 * - Michael Griffin 2011-2014 <mrmisticismo@hotmail.com>
 * - 10/30/16 Added Pre Negoiation Loop to Catch Term and scripts
 *
 * g++ -o entelnetd entelnetd.cpp config.cpp -lutil -std=c++0x
 *
 *
 * Based on ddtelnetd, based on utelnetd, see credits below.
 * ddtelnetd by Bo Simonsen <bo@geekworld.dk>
 *
 * ---------------------------------------------------------------------------
 * (C) 2000, 2001, 2002, 2003 by the authors mentioned below
 * ---------------------------------------------------------------------------
 *
 * Artur Bajor, Centrum Informatyki ROW   Joerg Schmitz-Linneweber, Aston GmbH
 * <abajor@cirow.pl>                      <schmitz-linneweber@aston-technologie.de>
 *
 * Vladimir Oleynik                       Robert Schwebel, Pengutronix
 * <dzo@simtreas.ru>                      <r.schwebel@pengutronix.de>
 *
 * Bjorn Wesen, Axis Communications AB    Sepherosa Ziehau
 * <bjornw@axis.com>                      <sepherosa@myrealbox.com>
 *
 * This file is distributed under the GNU General Public License (GPL),
 * please see the file LICENSE for further information.
 *
 */

#include "config.h"

#include <unistd.h>
#include <pty.h>
#include <termios.h>

#include <iostream>       // std::cout

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <limits.h>

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>       // transform#include <string>

#include <uuid/uuid.h>

#include <syslog.h>        //  syslogd

#include <arpa/telnet.h>   // IAC WILL, DO, DONT etc.
#include <arpa/inet.h>     // inet_ntoa

#include <netinet/in.h>   // gethostbyaddr
#include <netdb.h>        // gethostbyaddr


#define BUFSIZE 4096

#define OUTFD 1           // Output to Client Terminal
#define INFD 0            // Input From Client Terminal

#define TRUE 1
#define FALSE 0

char PATH[PATH_MAX]              = {0};
char USE_FILE_LOG                = 'F';
char USE_HOSTS_DENY              = 'F';
char BLOCK_NO_HOSTNAME           = 'F';
char HOSTS_DENY_PATH[PATH_MAX]   = {0};

using namespace std;


// Default Terminal Size.
int term_hight = 25;
int term_width = 80;

int pid        = 0;      // Holds child process.
int unknowhost = FALSE;

std::string term_type    = "Unknown";
std::string term_passing = "/tmp/enthral/"; // Config can overide this!

int TTYPE_FINALIZE = FALSE;

int ptyfd      = 0;
struct termios termbuf, termbuf2;	// pty control structure
vector<string> client_term;         // holds client TERM's detected


/**
 * EnTelnet Class
 */
class enTelnet
{
public:
    std::string hostaddr_string;
    std::string hostname_string;

    std::string make_uuid();
    void errlog (char *fmt, ...);

    void load_hosts_deny(std::string ipaddress, std::string hostname);
    int send_iac(unsigned char command, int option);
    int send_iac_ttype();
    int remove_iacs(unsigned char *in, int len, unsigned char* out, int* out_count);
};



/**
 * Used To created Unique filename passed to BBS server
 * Holds detected terminal emulation for each node spawned.
 */
std::string enTelnet::make_uuid()
{
    uuid_t id;
    uuid_generate(id);

    char *string = new char[100];
    uuid_unparse(id, string);

    std::string tmp = string;
    delete string;
    return tmp;
}


/**
 * Errlog is used for Easy testing and debugging.
 * Writes Message to a File.
 */
void enTelnet::errlog (char *fmt, ...)
{
    char logfile[1024]= {0};
    va_list arg;
    time_t t;
    tm *tm;
    char buffer[2048]= {0},datestr[81]= {0},buf2[2100]= {0};

    if (toupper(USE_FILE_LOG) == 'T')
    {
        sprintf(logfile,"%sentelnetd.log",PATH);
        va_start(arg, fmt);
        vsprintf(buffer, fmt, arg);
        va_end(arg);

        t  = time(NULL);
        tm = localtime(&t);
        strftime(datestr,81,"%Y/%m/%d %H:%M:%S",tm);
        sprintf(buf2,"\n%s : %s",datestr,buffer);

        FILE *fp;
        fp = fopen(logfile,"at");
        if(fp == NULL) fp = fopen(logfile,"wt");
        if(fp != NULL)
        {
            fputs(buf2,fp);
            fclose(fp);
        }
        else
        {
            syslog(LOG_INFO, "Enthral in.telnetd: Unable to Open/create log file");
        }
    }
}


/**
 * Load hosts.deny block file for Offending IP and HostNames.
 * Can add stright IP, or HostName without Wildcard *,
 */
void enTelnet::load_hosts_deny(std::string ipaddress, std::string hostname)
{
    errlog((char *)"Loading Hosts.Deny");
    std::string badhost;
    std::ifstream in;
    std::string::size_type id1;
    in.open(HOSTS_DENY_PATH);
    if (in.is_open())
    {
        for (;;)
        {
            if(in.eof()) break;
            std::getline(in,badhost,'\n');

            if (badhost.size() != 0)
            {

                // First Check for Bad IP Address
                if( badhost == ipaddress)
                {
                    syslog(LOG_INFO, "Enthral in.telnetd [IP %s] Blocked IP Address Found.", ipaddress.c_str() );
                    errlog((char *)"Blocked IP Address Found, Dropping connection %s", ipaddress.c_str() );
                    write(OUTFD, "\r\nIP/Hostname Blocked due to connection abuse.\r\n", 52 );
                    closelog();
                    in.close();
                    exit(0);
                }
                else
                {
                    //Check if hostname WildCard is blocked.
                    id1 = hostname.find(badhost,0);
                    if (id1 != std::string::npos)
                    {
                        syslog(LOG_INFO, "Enthral in.telnetd [IP %s, HOST %s] Blocked by Hostname.", ipaddress.c_str(), hostname.c_str() );
                        errlog((char *)"Blocked IP Hostname Found, Dropping connection %s - %s", ipaddress.c_str(), hostname.c_str());
                        write(OUTFD, "\r\nIP/Hostname Blocked due to connection abuse.\r\n", 52);
                        closelog();
                        in.close();
                        exit(0);
                    }

                }
            }
            if(in.eof()) break;

        }
        in.close();
    }
}


/**
 * Sends IAC Sequence back to Users Client for Terminal Negoation.
 */
int enTelnet::send_iac(unsigned char command, int option)
{
    std::stringstream stm;
    std::string       buf;

    stm << static_cast<char>(IAC);
    stm << command;
    stm << static_cast<char>(option);
    buf = stm.str();

    errlog((char *)"Sending: IAC:%i CMD:%i OPT:%i",IAC,command,option);

    return (write(OUTFD, buf.c_str(), option == -1 ? 2 : 3));
}


/**
 * Send Specific IAC TTYPE Message to Users Client Terminal
 * Asks for Clients Terminal Types.
 */
int enTelnet::send_iac_ttype()
{
    std::stringstream stm;
    std::string       buf;

    //IAC SB TTYPE SEND IAC SE
    stm << static_cast<char>(IAC);
    stm << static_cast<char>(SB);
    stm << static_cast<char>(TELOPT_TTYPE);

    stm << static_cast<char>(TELQUAL_SEND);
    stm << static_cast<char>(IAC);
    stm << static_cast<char>(SE);
    buf = stm.str();

    return (write(OUTFD, buf.c_str(), buf.size() ));
}


/**
 * Method that reads in a Naws Subnegotiation that ends up with a IAC SE
 * Sets the Windows Size to the Detected Size, or Sets Defaults.
 */
void handleNAWS()
{
    struct winsize ws;

#ifdef TIOCSWINSZ
    if (term_width || term_hight)
    {
        memset(&ws, 0, sizeof(ws));
        ws.ws_col = term_width;
        ws.ws_row = term_hight;
        ioctl(ptyfd, TIOCSWINSZ, (char *)&ws);
    }
#endif
}


/**
 * Method that parses IAC responsed from Client Terminal
 * Also cleans out sequences for sending raw User Input to BBS System.
 */
int enTelnet::remove_iacs(unsigned char *in, int len, unsigned char* out, int* out_count)
{

    std::string parse_subnegoation;
    std::string::size_type id1;
    std::string::size_type id2;

    std::string tmp = (char *)in;
    std::string term;

    int termindex = -1;

    unsigned char *end = in + len;
    unsigned char* orig_in = in;
    static int ignore = FALSE;

    int done = FALSE;

    while (in < end)
    {
        if (*in != IAC)
        {
            if(ignore == FALSE)
            {
                *out = *in;
                in++;
                out++;
                *out_count = *out_count + 1;
            }
            else
            {
                in++;
            }
        }
        else
        {
            if((in+1) == end)
            {
                *orig_in = IAC;
                return 1;
            }

            if(ignore && *(in+1) != SE)
            {
                send_iac(*(in+1), -1);
                in += 2;
                continue;
            }

            syslog(LOG_INFO, "Enthral in.telnetd [IP %s] got real IAC, %d, %d", hostaddr_string.c_str(), *(in+1), *(in+2));
            errlog((char *)"Enthral in.telnetd [IP %s] got real IAC, %d, %d", hostaddr_string.c_str(), *(in+1), *(in+2));


            switch(*(in+1))
            {
            case IAC:
                *out = IAC;
                ++out;
                *out_count = *out_count + 1;
                in += 2;
                break;

            case WILL:

                // If we get response will, then ask to do it again.
                switch (*(in+2))
                {
                case TELOPT_TTYPE:
                    errlog((char *)"WILL TELOPT_TTYPE");

                    // Don't send Extra if Finalized.
                    if (TTYPE_FINALIZE == FALSE)
                    {
                        if (send_iac_ttype() < 0)
                        {
                            errlog((char *)"Enthral send_iac_ttype ERROR!");
                            closelog();
                            exit(1);
                        }
                        errlog((char *)"Enthral send_iac_ttype OK!");
                    }
                    break;

                default:
                    break;
                }

                if((in+2) == end)
                {
                    *orig_in = IAC;
                    *(orig_in+1) = *(end-1);
                    return 2;
                }
                in += 3;
                break;

            case WONT:

                // If we get response will, then ask to do it again.
                switch (*(in+2))
                {
                case TELOPT_TTYPE:
                    errlog((char *)"WON'T TELOPT_TTYPE");
                    write(OUTFD, "Warning, Unsupported Telnet Client Detected, disconnecting", 59 );

                    // If host is unknown, and Won't TELOPT then drop!  Catch scripts!
                    if (unknowhost)
                    {
                        errlog((char *)"Unknown Host & Unsupported Term, dropping connection on scripts.");
                        errlog((char *)"---------------------------------------------");
                        kill(pid, SIGHUP);
                        exit(1);
                    }

                    errlog((char *)"Unsupported Terminal, dropping connection on scripts.");
                    errlog((char *)"---------------------------------------------");
                    kill(pid, SIGHUP);
                    exit(1);

                    break;

                default:
                    break;
                }

                if((in+2) == end)
                {
                    *orig_in = IAC;
                    *(orig_in+1) = *(end-1);
                    return 2;
                }
                in += 3;
                break;

            case DO:
            case DONT:

                if((in+2) == end)
                {
                    *orig_in = IAC;
                    *(orig_in+1) = *(end-1);
                    return 2;
                }
                in += 3;
                break;

            case SB:
                ignore = TRUE;
                syslog(LOG_INFO, "Enthral in.telnetd [IP %s] Negotiation began", hostaddr_string.c_str());

                errlog((char *)"SB");

                in += 2;

                switch (*(in))
                {

                case TELOPT_NAWS:
                    //cout << "\r\nNAWS" << endl;
                    errlog((char *)"NAWS SB");
                    ++in; // Bypass first (255) Char
                    ++in;

                    // Never Set Width or Hight to 0
                    term_width = (unsigned char)*in;

                    ++in; // Bypass first (255) Char
                    ++in;

                    term_hight = (unsigned char)*in;

                    // Overide any 0 values to default.
                    if (term_width < 1) term_width = 80;
                    if (term_hight < 1) term_hight = 25;


                    errlog((char *)"Term Size: %ix%i",term_width,term_hight);
                    errlog((char *)"NAWS SE");

                    in-=4;
                    handleNAWS();
                    break;

                case TELOPT_TTYPE:
                    term.erase();
                    //cout << "\r\nTTYPE" << endl;
                    errlog((char *)"TTYPE SB");
                    ++in; // Bypass first (255) Char
                    ++in;

                    // Grab Term String
                    while(1)
                    {
                        if ((*in) == IAC || *in == '\0') break;
                        term += *in;
                        in += 1;
                    }

                    // Remove the SB
                    in -= 2;

                    // Reset back to parse out.
                    in -= term.size();


                    // Change Terminal String to Uppercase
                    std::transform(term.begin(), term.end(),term.begin(), ::toupper);
                    errlog((char *)term.c_str());

                    // In Detection Phase,  Continue to Parse and Iterate through.
                    if (TTYPE_FINALIZE == FALSE)
                    {

                        // Always loop through and get at least (2) Responses.
                        client_term.push_back(term);

                        // Check if This and previous match, detecting last terminal.
                        // Skip the very First Terminal Entry for this comparison.
                        if (client_term.size() > 1 && client_term[client_term.size()-2] == term)
                        {
                            done = TRUE;
                            errlog((char *)"Completed Terminal Detection Loop.");
                            errlog((char *)"TTYPE SE");
                        }

                        // Check for ANSI Terminals first.
                        // Main Supported Terms for ANSI -
                        // 1. ANSI       - SyncTerm and Mtelnet
                        // 2. ANSI-BBS
                        // 3. QNX        - Console
                        // 4. QANSI      - QNX ANSI
                        // 5. SCOANSI    - SCO ANSI
                        // 6. VT100      - NETRUNNER and some Normal Terms.
                        // 7. XTERM
                        // 8. LINUX      - Real Console
                        // cycle through and try for most compitable, if nothing, then take the first.
                        termindex = 0;
                        term_type.erase();

                        if (done == TRUE)
                        {
                            // Set Term to Last entry send by Client
                            term_type = term;
                            TTYPE_FINALIZE = TRUE;
                            errlog((char *)"Term Negoantion failed, Default to Client Term %s.", (char *)term.c_str());
                        }

                        if (TTYPE_FINALIZE == FALSE)
                        {

for(std::string ct : client_term)
                            {
                                // run first looking for ANSI Capabably,
                                if (ct == "ANSI"     ||
                                    ct == "ANSI-BBS" ||
                                    ct == "PC-ANSI"  ||
                                    ct == "QANSI"    ||
                                    ct == "SCOANSI")

                                {
                                    term_type = ct;
                                    errlog((char *)"Perferred TERM detected: %s.", (char *)term.c_str());
                                    done = TRUE;
                                    TTYPE_FINALIZE = TRUE;
                                    break;
                                }
                                ++termindex;
                            }

                            // We didn't get perferred Selecton,
                            // Now try secondary selection of terms.
                            if (term_type.size() == 0 && TTYPE_FINALIZE == FALSE)
                            {
                                termindex = 0;
                                for(std::string ct : client_term)
                                {
                                    // If no Ansi erminal, Then look for
                                    // Possiable UTF8 Terminal
                                    if (ct == "VT100"          ||
                                            ct == "XTERM"      ||
                                            ct == "LINUX"      ||
                                            ct == "QNX"        ||
                                            ct == "SCREEN")
                                    {
                                        term_type = ct;
                                        errlog((char *)"Perferred TERM detected: %s.", (char *)term.c_str());
                                        TTYPE_FINALIZE = TRUE;
                                        done = TRUE;
                                        break;
                                    }
                                    ++termindex;
                                }
                            }
                        }

                        // If were not Done, and Term size empty, query for next TERM in list.
                        if (done == FALSE)
                        {
                            // While not done, send request for next term type.
                            errlog((char *)"Perferred Terminal not Found, Query for Next TERM.");
                            if (send_iac_ttype() < 0)
                            {
                                errlog((char *)"Enthral send_iac_ttype ERROR!");
                                closelog();
                                exit(1);
                            }
                        }

                        // Were Done. Log the Finialization of the TERM Detected from the client.
                        if(TTYPE_FINALIZE == TRUE)
                        {
                            // We found perferred Terminal, and we marked done Above.
                            // Set Term and Done!  Only term response we have.
                            errlog((char *)"Finalized TERM (1): %s ",(char *)term_type.c_str());

                            // Write to unique File so the Forked process can read it in.
                            std::ofstream out;
                            out.open(term_passing.c_str(),ios::trunc);
                            if (out.is_open())
                            {
                                out << term_type << endl;
                                out.close();
                            }

                        }
                    }

                    break;
                case TELOPT_LINEMODE:
                    cout << "\r\nLINEMODE" << endl;

                    break;
                case TELOPT_NEW_ENVIRON:
                    cout << "\r\nENVIRON" << endl;

                    break;
                default:
                    break;
                }
                break;

                in+=2;

            case SE:
                ignore = FALSE;
                syslog(LOG_INFO, "Enthral in.telnetd [IP %s] Negotiation completed", hostaddr_string.c_str());
                in += 2;
                break;

            case NOP:
            case DM:
            case BREAK:
            case IP:
            case AO:
            case AYT:
            case GA:
                in += 2;
                break;
            case EC:
                in += 2;
                *out = '\b';
                ++out;
                *out_count = *out_count + 1;
                break;
            default:
                syslog(LOG_INFO, "Enthral in.telnetd [IP %s] Unknown IAC arg: %d", hostaddr_string.c_str(),  *(in+1));
                in += 2;
                break;
            }
        }
    }

    return 0;
}


/**
 * Container for Passing Command Line Arguments in Fork()
 */
static char *argv_init[] = {NULL, NULL, NULL, NULL, NULL, NULL};


/**
 * Show EnTelnetD Command Line Parameters.
 */
void show_usage(void)
{
    printf("Usage: entelnetd [-l loginprogram] [-u user]\n");
    printf("\n");
    printf("   -l loginprogram  program started by the server\n");
    printf("   -u user  logins user without password auth\n");
    printf("\n");
    exit(1);
}


/**
 * Startup the inital Terminal buffer
 */
void init_termbuf()
{
# ifdef  STREAMSPTY
    if (really_stream)
        tcgetattr(ttyfd, &termbuf);
    else
# endif
        tcgetattr(ptyfd, &termbuf);
    termbuf2 = termbuf;
}


/**
 * Setup the inital Terminal buffer
 */
void set_termbuf()
{
    /*
     * Only make the necessary changes.
     */
    if (memcmp(&termbuf, &termbuf2, sizeof(termbuf)))
# ifdef  STREAMSPTY
        if (really_stream)
            tcsetattr(ttyfd, TCSANOW, &termbuf);
        else
# endif
            tcsetattr(ptyfd, TCSANOW, &termbuf);
}


/**
 * Get Current Working Directory
 * Read the Full Executable w/ Path
 * xinetd leaves current working folder as /
 * and argv[0] is empty. Foo!
 */
std::string get_working_path()
{
    char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    return std::string( result, (count > 0) ? count : 0 );
}


/**
 * Main Program
 */
int main(int argc, char **argv)
{
    enTelnet _ent;

    std::stringstream ss;

    int new_offset = 0;
//	int shell_pid  = 0;

    unsigned char buf[BUFSIZE]= {0};
    unsigned char tmpbuf[BUFSIZE]= {0};

    std::string loginpath;
    std::string user;
    std::string login;

    std::string hostaddr_string;
    std::string hostname_string;


    struct sockaddr_in from;
    socklen_t fromlen;
    struct hostent *hp;

    _ent.errlog((char *)"=============================================");
    std::string cdw = get_working_path();

    int num = 0;
    for (int i = 0; ; i++)
    {
        if (cdw[i] == '\0') break;
        if (cdw[i] == '/') num = i;
    }
    if (num != 0)
    {
        for (int i = 0; i < num+1; i++)
        {
            PATH[i] = cdw[i];
        }
    }
    else
        strcpy(PATH,cdw.c_str());


    // If Config Exists Run
    if (configdataexists())
        parseconfig();


    //_ent.errlog((char *)"getuuid");

    // Unique filename to pass to BBS for
    // Reading in Detected Client Terminal.
    term_passing += _ent.make_uuid();


    // Parameter for Login Program and User Login ID.
    for (;;)
    {
        int c;

        c = getopt( argc, argv, "l:u:");

        if (c == EOF) break;
        switch (c)
        {
        case 'u':
            user = strdup(optarg);
            break;
        case 'l':
            login = strdup(optarg);
            break;

        default:
            printf("%c\n", c);
            show_usage();
            exit(1);
        }
    }

    if(login.size() > 0)
    {
        loginpath = login;
        login.erase();
    }
    else
    {
        loginpath = "/bin/login";
    }

    argv_init[0] = strdup(loginpath.c_str());

    // Resolve IP Address as 1st Passed Argument.
    fromlen = sizeof (from);
    if (getpeername(0, (struct sockaddr *)&from, &fromlen) < 0)
    {
        syslog(LOG_INFO, "Enthral in.telnetd [getpeername] error!");
        hostaddr_string = "localhost.unknown";
        unknowhost = TRUE;
    }
    else
    {
        hostaddr_string = inet_ntoa(from.sin_addr);
        _ent.errlog((char *)"Connection IP From: %s", (char *)hostaddr_string.c_str());
    }

    // Get hostname of conenction.
    hp = gethostbyaddr((char *)&from.sin_addr, sizeof (struct in_addr),from.sin_family);
    if (hp)
    {
        hostname_string = hp->h_name;
    }
    else
    {
        if (toupper(BLOCK_NO_HOSTNAME) == 'T')
        {
            syslog(LOG_INFO, "Enthral in.telnetd [gethostbyaddr] failed!");
            _ent.errlog((char *)"Enthral in.telnetd [gethostbyaddr] failed! IP: %s", (char *)hostaddr_string.c_str());
            write(OUTFD, "\r\nUnable to resolve hostname, connection dropped due to connection abuse.\r\n", 80 );
            exit(1);
        }
        else
        {
            hostname_string = "hostname.unknown";
            unknowhost = TRUE;
        }

    }

    // Check for Blocked IP Addresses, if found then exit!
    _ent.errlog((char *)"Connection Hostname From: %s", (char *)hostname_string.c_str());
    if (toupper(USE_HOSTS_DENY) == 'T')
        _ent.load_hosts_deny(hostaddr_string,hostname_string);

    argv_init[1] = (char *)hostaddr_string.c_str();
    argv_init[2] = (char *)hostname_string.c_str();

    _ent.errlog((char *)"enTelnetd argv_init[3-4]");

    // Pass Login user for External Process
    if(user.size() > 0)
    {
        argv_init[3] = strdup("-f");
        argv_init[4] = strdup(user.c_str());
    }

    _ent.errlog((char *)"enTelnetd argv_init[5]");

    if(term_passing.size() > 0)
        argv_init[5] = strdup(term_passing.c_str());

    _ent.errlog((char *)"enTelnetd");

    // Startup Syslogd.
    openlog("enTelnetd", LOG_PID, LOG_SYSLOG);

    syslog(LOG_INFO, "Enthral in.telnetd PATH: %s", PATH);

    // Log Connection
    syslog(LOG_INFO, "Enthral in.telnetd [%s] [Server Path]", loginpath.c_str());
    syslog(LOG_INFO, "Enthral in.telnetd [IP %s] [Host %s] [Connection Logged]",
           hostaddr_string.c_str(), hostname_string.c_str());


    /* These options are taken from maximus ipcomm.c (max 3.03b unix)
     * If <0 then lost connection.
     */
    if(_ent.send_iac(DONT, TELOPT_OLD_ENVIRON) < 0)
    {
        syslog(LOG_INFO, "Enthral in.telnetd [IP %s] TELOPT_OLD_ENVIRON Closed connection", hostaddr_string.c_str());
        closelog();
        exit(1);
    }

    if(_ent.send_iac(DONT, TELOPT_LINEMODE) < 0)
    {
        syslog(LOG_INFO, "Enthral in.telnetd [IP %s] TELOPT_LINEMODE Closed connection", hostaddr_string.c_str());
        closelog();
        exit(1);
    }

    if(_ent.send_iac(DO, TELOPT_SGA) < 0)
    {
        syslog(LOG_INFO, "Enthral in.telnetd [IP %s] TELOPT_SGA Closed connection", hostaddr_string.c_str());
        closelog();
        exit(1);
    }

    if(_ent.send_iac(WILL, TELOPT_ECHO) < 0)
    {
        syslog(LOG_INFO, "Enthral in.telnetd [IP %s] TELOPT_ECHO Closed connection", hostaddr_string.c_str());
        closelog();
        exit(1);
    }

    if(_ent.send_iac(DO, TELOPT_TTYPE) < 0)
    {
        syslog(LOG_INFO, "Enthral in.telnetd [IP %s] TELOPT_TTYPE Closed connection", hostaddr_string.c_str());
        closelog();
        exit(1);
    }

    if(_ent.send_iac(WILL, TELOPT_SGA) < 0)
    {
        syslog(LOG_INFO, "Enthral in.telnetd [IP %s] TELOPT_SGA Closed connection", hostaddr_string.c_str());
        closelog();
        exit(1);
    }
    if(_ent.send_iac(DO, TELOPT_NAWS) < 0)
    {
        syslog(LOG_INFO, "Enthral in.telnetd [IP %s] TELOPT_NAWS Closed connection", hostaddr_string.c_str());
        closelog();
        exit(1);
    }
    if(_ent.send_iac(WILL, TELOPT_BINARY) < 0)
    {
        syslog(LOG_INFO, "Enthral in.telnetd [IP %s] WILL TELOPT_BINARY Closed connection", hostaddr_string.c_str());
        closelog();
        exit(1);
    }
    if(_ent.send_iac(DO, TELOPT_BINARY) < 0)
    {
        syslog(LOG_INFO, "Enthral in.telnetd [IP %s] DO TELOPT_BINARY Closed connection", hostaddr_string.c_str());
        closelog();
        exit(1);
    }

    sleep(2);

    time_t start, end;
    time(&start);

    struct timeval value;
    value.tv_sec = 0;
    value.tv_usec = 10000;


    // This first Loop is to catch all Initial TelnetOPT negoiations
    // Were going to Detect the TERM and NAWS etc here and pass to
    // External Program
    bool isWaitingTerm = true;
    do
    {
        int selret;
        fd_set rdfdset;

        FD_ZERO(&rdfdset);
        FD_SET(INFD, &rdfdset);

        selret = select(INFD + 1, &rdfdset, NULL, NULL, &value);

        // Error / Lost Connection on
        if (selret <= 0)
        {
            isWaitingTerm = false;
            break;
        }


        if (FD_ISSET(INFD, &rdfdset))
        {
            int out_count = 0;
            int r;

            // Read incoming Socket
            r = read(INFD, buf + new_offset, BUFSIZE - new_offset);

            if (r <= 0)
            {
                isWaitingTerm = false;
                break;
            }

            // Parse Telnet Options
            new_offset = _ent.remove_iacs(buf, r + new_offset, tmpbuf, &out_count);
        }


        // Give (2) Seconds for negoiations to complete
        time(&end);
        double diff = start-end;
        if (diff > 2)
        {
            isWaitingTerm = false;
            break;
        }

    }
    while (isWaitingTerm);



    // Once Telnet Negoiations are complete, test for TERM, or Terminal Type
    // If non detected, most likely a script, so kill the connection.
    syslog(LOG_INFO, "Enthral in.telnetd Term Type: %s", term_type.c_str());
    _ent.errlog((char *)"Term Type: %s", term_type.c_str());

    if (term_type == "Unknown" || term_type == "VT220")
    {
        closelog();
        exit(1);
    }

    _ent.errlog((char *)"enTelnetd forkpty()");

    // If the Connection is good, fork and start the BBS
    pid = forkpty(&ptyfd, NULL, NULL, NULL);

    if (pid == 0)
    {
        setsid();
        tcsetpgrp(0, getpid());

        /* exec shell, with correct argv and env */
        execv(loginpath.c_str(), argv_init);
        syslog(LOG_INFO, "Enthral in.telnetd [IP %s] pid == 0", hostaddr_string.c_str());
        exit(1);
    }
    else if(pid == -1)
    {
        syslog(LOG_INFO, "Enthral in.telnetd [IP %s] pid == -1", hostaddr_string.c_str());
        closelog();
        exit(1);
    }
    /*
    	else
    	{
    	    shell_pid = pid;
    	}
    */

    // Setup Detected Screen Size.
    struct winsize ws;

    // setup Term
    init_termbuf();
#ifdef TIOCSWINSZ
    if (term_width || term_hight)
    {
        memset(&ws, 0, sizeof(ws));
        ws.ws_col = term_width;
        ws.ws_row = term_hight;
        ioctl(ptyfd, TIOCSWINSZ, (char *)&ws);
    }
#endif


    bool isConnected = true;
    do
    {
        int selret;
        fd_set rdfdset;

        FD_ZERO(&rdfdset);
        FD_SET(ptyfd, &rdfdset);
        FD_SET(INFD, &rdfdset);

        syslog(LOG_INFO, "WAiting new data");
        selret = select(ptyfd + 1, &rdfdset, NULL, NULL, NULL);

        // Error / Lost Connection on
        if (selret <= 0)
        {
            isConnected = false;
            break;
        }


        if (FD_ISSET(ptyfd, &rdfdset))
        {
            unsigned char* ptr1, *ptr2;
            int iacs = 0;
            int r;

            r = read(ptyfd, tmpbuf, BUFSIZE / 2);

            if (r <= 0)
            {
                isConnected = false;
                break;
            }

            ptr1 = tmpbuf;
            ptr2 = buf;

            while(ptr1 != tmpbuf + r && ptr2 != buf + BUFSIZE && isConnected)
            {
                if(*ptr1 == IAC)
                {
                    *ptr2 = IAC;
                    ptr2++;
                    *ptr2 = IAC;
                    ptr2++;
                    ptr1++;
                    iacs++;
                }
                else
                {
                    *ptr2 = *ptr1;
                    ptr1++;
                    ptr2++;
                }
            }

            if(write(OUTFD, buf, r + iacs) != (r + iacs))
            {
                _ent.errlog((char *)"2 write OUTFD break!");
                isConnected = false;
                break;
            }
        }

        if (FD_ISSET(INFD, &rdfdset))
        {

            int out_count = 0;
            int r;

            r = read(INFD, buf + new_offset, BUFSIZE - new_offset);

            if (r <= 0)
            {
                isConnected = false;
                break;
            }

            new_offset = _ent.remove_iacs(buf, r + new_offset, tmpbuf, &out_count);

            if(write(ptyfd, tmpbuf, out_count) != out_count)
            {
                _ent.errlog((char *)"2 write ptyfd break!");
                isConnected = false;
                break;
            }
        }

    }
    while (isConnected);


    raise(SIGHUP);
    sleep(2);

    kill(pid, SIGHUP);

    close(ptyfd);

    syslog(LOG_INFO, "Enthral in.telnetd [IP %s] [Closed Connection]", hostaddr_string.c_str());


    closelog();

    _exit(0);
}

