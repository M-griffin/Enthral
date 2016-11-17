/**
 * Telnet Daemon
 * Originaly based off of ddtelnetd for Enthral BBS
 * Rewritten and based off Obv/2 XRM BBS Software
 * (c) Michael Griffin 2016 <mrmisticismo@hotmail.com>
 * https://github.com/M-griffin
 *
 * required uuid-dev
 *
 * compiled using code::blocks in linux mint.
 *
 *
 */

#include "config.hpp"
#include "telnet_daemon.hpp"
#include "telnet_decoder.hpp"

#include <unistd.h>
#include <pty.h>
#include <termios.h>

#include <iostream>
#include <csignal>
#include <cstring>
#include <string>
#include <fstream>



#include <syslog.h>        //  syslogd

char PATH[PATH_MAX]              = {0};
char USE_FILE_LOG                = 'F';
char USE_HOSTS_DENY              = 'F';
char BLOCK_NO_HOSTNAME           = 'F';
char HOSTS_DENY_PATH[PATH_MAX]   = {0};
char ENTHRAL_TEMP_PATH[PATH_MAX] = "/tmp/enthral/";

int  ptyfd      = 0;                // Holds Child File Descriptor
int  pid        = 0;                // Holds child process.
bool unknowhost = false;

struct termios termbuf, termbuf2;	// pty control structure



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
    TelnetDaemon daemon;

    std::string loginpath;
    std::string user;
    std::string login;
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
    {
        strcpy(PATH,cdw.c_str());
    }


    // If Config Exists Run
    if (configdataexists())
    {
        parseconfig();
    }


    // Unique filename to pass to BBS for
    // Reading in Detected Client Terminal.
    daemon.make_uuid(ENTHRAL_TEMP_PATH);

    // Lookup info for the passed socket descriptor
    daemon.get_host_info();


    // Parameter for Login Program and User Login ID.
    int c = '\0';
    for (;;)
    {
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
    argv_init[1] = (char *)daemon.m_hostaddr_string.c_str();
    argv_init[2] = (char *)daemon.m_hostname_string.c_str();

    // Pass Login user for External Process
    if(user.size() > 0)
    {
        argv_init[3] = strdup("-f");
        argv_init[4] = strdup(user.c_str());
    }

    // Filename for passing the TERM to the external program.
    if(daemon.m_term_passing.size() > 0)
    {
        argv_init[5] = strdup(daemon.m_term_passing.c_str());
    }

    // Startup Syslogd.
    openlog("telnet_daemon", LOG_PID, LOG_SYSLOG);

    syslog(LOG_INFO, "telnet_daemon in.telnetd PATH: %s", PATH);
    daemon.errlog((char *)"telnet_daemon in.telnetd PATH: %s", PATH);

    // Log Connection
    syslog(LOG_INFO, "telnet_daemon in.telnetd [%s] [Server Path]", loginpath.c_str());
    daemon.errlog((char *)"telnet_daemon in.telnetd [%s] [Server Path]", (char *)loginpath.c_str());

    syslog(LOG_INFO, "telnet_daemon in.telnetd [IP %s] [Host %s] [Connection Logged]",
           daemon.m_hostaddr_string.c_str(),
           daemon.m_hostname_string.c_str()
          );

    daemon.errlog((char *)"telnet_daemon in.telnetd [IP %s] [Host %s] [Connection Logged]",
                  (char *)daemon.m_hostaddr_string.c_str(),
                  (char *)daemon.m_hostname_string.c_str()
                 );

    // Send Startup Telnet Sequences to connecting client.
    // Ready for Terminal / NAWS Detection and setting defaults.
    daemon.errlog((char *)"send_startup_iac");
    daemon.send_startup_iac();

    // Look intitial TELOPT codes with (2) second pause then (2) second detection loop
    // To Catch Terminal Type and NAWS.
    daemon.errlog((char *)"loop_detection");
    daemon.loop_detection();
    daemon.errlog((char *)"loop_detection completed.");

    // Display Detected Term, Rows, Cols.
    daemon.errlog((char *)"Term Type: %s", (char *)daemon.getTermType().c_str());
    daemon.errlog((char *)"Term Cols: %i", daemon.getTermCols());
    daemon.errlog((char *)"Term Rows: %i", daemon.getTermRows());


    if (daemon.getTermType() == "undetected" || daemon.getTermType() == "VT220" || daemon.getTermType() == "")
    {
        daemon.errlog((char *)"No Term Detected, Disconnecting.");
        closelog();
        exit(1);
    }

    // Terminal Type Detection completed, write out to file for extrnal program
    // So the Forked process can read it in.
    std::ofstream out;
    out.open(daemon.m_term_passing.c_str(),ios::trunc);
    if (out.is_open())
    {
        out << daemon.getTermType() << endl;
        out.close();
    }


    daemon.errlog((char *)"Starting BBS Server forkpty()");

    // If the Connection is good, fork and start the BBS
    pid = forkpty(&ptyfd, NULL, NULL, NULL);

    daemon.errlog((char *)"ptyfd: %i", ptyfd);


    // Pid 0 Start Child Process.
    if (pid == 0)
    {
        setsid();
        tcsetpgrp(0, getpid());

        // exec shell, with correct argv and env
        execv(loginpath.c_str(), argv_init);
        syslog(LOG_INFO, "child process created for [IP %s] pid == 0", daemon.m_hostaddr_string.c_str());
        daemon.errlog((char *)"child process created for [IP %s] pid == 0", (char*)daemon.m_hostaddr_string.c_str());
        exit(1);

    }
    // Pid -1 is an error
    else if(pid == -1)
    {
        syslog(LOG_INFO, "fork() Error, exiting [IP %s] pid == -1", daemon.m_hostaddr_string.c_str());
        daemon.errlog((char *)"fork() Error, exiting [IP %s] pid == -1", (char*)daemon.m_hostaddr_string.c_str());
        closelog();
        exit(1);
    }
    else if(pid > 0)
    {
        // Parent process.
        syslog(LOG_INFO, "parent process for [IP %s] pid > 0", daemon.m_hostaddr_string.c_str());
        daemon.errlog((char *)"parent process for [IP %s] pid > 0", (char*)daemon.m_hostaddr_string.c_str());
    }

    // Setup Detected Screen Size.
    struct winsize ws;

    // setup Term
    init_termbuf();
#ifdef TIOCSWINSZ
    if (daemon.getTermCols() || daemon.getTermRows())
    {
        memset(&ws, 0, sizeof(ws));
        ws.ws_col = daemon.getTermCols();
        ws.ws_row = daemon.getTermRows();
        ioctl(ptyfd, TIOCSWINSZ, (char *)&ws);
    }
#endif


    // Start Main Parent Loop for communication with child (BBS)
    daemon.errlog((char *)"Starting Parent Loop");
    daemon.loop_parent_process(ptyfd);
    daemon.errlog((char *)"Closing Parent Loop");

    raise(SIGHUP);

    syslog(LOG_INFO, "in.telnetd [IP %s] [Closed Connection]", daemon.m_hostaddr_string.c_str());
    daemon.errlog((char *)"in.telnetd [IP %s] [Closed Connection]", (char*)daemon.m_hostaddr_string.c_str());

    closelog();

    remove(daemon.m_term_passing.c_str());

    _exit(0);


}

