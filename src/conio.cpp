/***************************************************************************
 *   Copyright (C) 2004-2017 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Console and basic I/O                                        *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

# include "struct.h"
# include "conio.h"
# include "msgs.h"
# include "files.h"
# include "users.h"

# include <errno.h>

# include "console.h"
# include "node.h"
# include "language.h"
# include "dtfunc.h"

# include <cstdlib>
# include <cstdarg>
# include <cstring>
# include <fstream>
# include <string>
# include <time.h>
# include <termios.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <sys/un.h>

# define ulong unsigned long

using namespace std;

# define echo_on()  term_option(0)
# define echo_off() term_option(1)

// Get this from config file lateron!
// |IN will overwrite this input length |FB will override color
char INPUT_COLOR[7] = {"|15|17"};
char HIDDEN_CHAR    = '*';
int  USE_CHAR       = TRUE;
int  CONT_INPUT = FALSE;
int  PAUSE_SCROLING = FALSE;

/**
 * Session Constructor Main I/O class
 */
ConsoleIO::ConsoleIO()
{
    peekch = -1;
    memset(&EscapeKey,0,sizeof(EscapeKey));
}

ConsoleIO::ConsoleIO(UserRec *thisuser)
{
    memset(&EscapeKey,0,sizeof(EscapeKey));
    peekch = -1;
    user = thisuser;
}

/**
 * History Display, Testing Only!
 */
void ConsoleIO::hist_display()
{
    History hist;
    memset(&hist,0,sizeof(History));

    hist_read(&hist);
}

/**
 * Initialize Passing of UserRec for MCI Codes.
 */
void ConsoleIO::start_session(UserRec *thisuser)
{
    user = thisuser;
}

/**
 * Add Date checking and multi File Write for Daily or Today's and Overall.
 */
void ConsoleIO::hist_update(int i, UserRec *user, int cnt)
{
    History  hist;
    History  daily;
    users    _usr;

    memset(&hist,0,sizeof(History));
    memset(&daily,0,sizeof(History));

    hist_read(&hist);
    daily_read(&daily);

    switch (i) {
    case HIST_CALLS :
        hist.LastCallTime = GetCurrentDTSec();
        strcpy((char *)hist.LastCaller,(char *)user->handle);

        daily.LastCallTime = GetCurrentDTSec();
        strcpy((char *)daily.LastCaller,(char *)user->handle);

        ++hist.Calls;
        ++daily.Calls;
        ++user->Calls;
        break;

    case HIST_EMAILS :
        ++hist.Emails;
        ++daily.Emails;
        ++user->Emails;
        break;

    case HIST_POSTS :
        ++hist.Posts;
        ++daily.Posts;
        ++user->Posts;
        break;

    case HIST_REPLIES :
        ++hist.Replies;
        ++daily.Replies;
        ++user->Replies;
        break;

    case HIST_EDITS :
        ++hist.Edits;
        ++daily.Edits;
        ++user->Edits;
        break;

    case HIST_DELETES :
        ++hist.Deletes;
        ++daily.Deletes;
        ++user->Deletes;
        break;

    case HIST_VIEWS :
        if (cnt != 0) {
            ++hist.Views;
            ++daily.Views;
            ++user->Views;
        } else {
            hist.Views  += cnt;
            daily.Views += cnt;
            user->Views += cnt;
        }
        break;

    case HIST_LOCAL :
        ++hist.Local;
        ++daily.Local;
        ++user->Local;
        break;

    case HIST_ECHOMAIL :
        ++hist.Echomail;
        ++daily.Echomail;
        ++user->Echomail;
        break;

    case HIST_UPLOADS :
        ++hist.Uploads;
        ++daily.Uploads;
        ++user->Uploads;
        break;

    case HIST_UPLOADKB :
        ++hist.UploadKb;
        ++daily.UploadKb;
        ++user->UploadKb;
        break;

    case HIST_DOWNLOADS :
        ++hist.Downloads;
        ++daily.Downloads;
        ++user->Downloads;
        break;

    case HIST_DOWNLAODSKB :
        ++hist.DownloadKb;
        ++daily.DownloadKb;
        ++user->DownloadKb;
        break;

    default :
        break;
    }

    // Write Out History Updates.
    hist_write(&hist);
    daily_write(&daily);
    _usr.users_write(user,user->idx);
}

void ConsoleIO::errlog (char *fmt, ...)
{
    char logfile[1024]= {0};
    sprintf(logfile,"%snode%i_errlog.txt",NODEPATH,NODE_NUM);

    va_list arg;
    time_t t;
    tm *tm;
    char buffer[2048]= {0},datestr[81]= {0},buf2[2100]= {0};

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
    if(fp != NULL) {
        fputs(buf2,fp);
        fclose(fp);
    } else {
        printf("\r\nUnable to open/create log file. . .");
    }
}

/**
 * Right String Padding
 */
void ConsoleIO::rspacing(char *str, int space)
{
    if (space == 0) return;

    std::string Line = str;
    int s = Line.size();

    if (s >= space) {
        Line.erase(space,(s-space));
        strcpy(str,Line.c_str());
        return;
    }
    Line = "";
    for(int i = 0; i < (space-s); i++)
        Line += ' ';
    strcat(str,Line.c_str());
}

/**
 * Left String Padding
 */
void ConsoleIO::lspacing(char *str, int space)
{
    if (space == 0) return;

    std::string Line = "";
    int s = strlen(str);

    if (s > space) {
        Line.erase(space,(s-space));
        strcpy(str,Line.c_str());
        return;
    }
    for(int i = 0; i < (space-s); i++)
        Line += ' ';
    Line += str;
    strcpy(str,Line.c_str());
}

/**
 * Center String Padding
 */
void ConsoleIO::cspacing(char *str)
{
    int iLen = strlen(str);
    int space = 79 - iLen;

    space /= 2;

    std::string Line = "";

    for(int i = 0; i < space; i++)
        Line += ' ';
    Line += str;
    strcpy(str,Line.c_str());
}

/**
 * Mask Input
 */
void ConsoleIO::mask(char *str)
{
    std::string Line = "";
    int s = strlen(str);

    for(int i = 0; i < s; i++)
        Line += '*';
    strcpy(str,Line.c_str());
}

/**
 * Send Raw Input to Screen, no MCI Codes
 */
void ConsoleIO::putline(char *str)
{
    console_putsn(str,strlen(str));
}

/**
 * Send Raw Single Character to Screen
 */
void ConsoleIO::putkey(char ch)
{
    std::string output;
    output = ch;
    console_putsn((char *)output.c_str(),output.size());
}

/**
 * Get a Single input key pass white list of
 * acceptable characters user can press
 */
char ConsoleIO::onekey(char *chlist)
{
    bool done = false;
    char ch   = 0,
         ch1  = 0;

    while(!done) {
        ch = (char)getkey(true);
        if(ch >= 'a' && ch <= 'z')
            ch -= 32;

        if(strchr(chlist,ch)) {
            ch1  = ch;
            done = true;
            if(ch1 != '\n')
                putkey(ch1);
        }
    }
    return(ch1);
}

/**
 * Repease a char in string per length
 *
 */
void ConsoleIO::repeatchar(char ch, int length)
{
    for(int i = 0; i < length; i++)
        putkey(ch);
}

/**
 * Sets up input field with background color
 *
 */
void ConsoleIO::inputfield(char *text, int &len)
{
    std::string repeat;
    std::string temp;
    char sTmp[3]  = {0};
    char sTmp2[3] = {0};

    temp = text;
    int tLen, id1;

    bool inc = false;

    // Overide Input Length for Ansi
    id1 = temp.find("|IN",0);
    if (id1 != -1) {
        sTmp[0] = temp[id1+3];
        sTmp[1] = temp[id1+4];
        temp.erase(id1,5);
        tLen = atoi( sTmp );
        if (tLen < len) len = tLen; // Set new Length
    }

    // Overide Foreground/Background Input Field Colors
    id1 = -1;
    id1 = temp.find("|FB",0);
    if (id1 != -1) {
        memset(&sTmp,0,3);
        // Foreground 00-15
        sTmp[0] = temp[id1+3];
        sTmp[1] = temp[id1+4];

        memset(&sTmp2,0,3);
        // Background 16-23
        sTmp2[0] = temp[id1+5];
        sTmp2[1] = temp[id1+6];
        temp.erase(id1,7);

        sprintf(INPUT_COLOR,"|%s|%s",sTmp,sTmp2);
        inc = true;
    }

    for(int i = 0; i != len; i++) {
        if (USE_CHAR) {
            repeat += "\xb0";
        } else {
            repeat += " ";
        }
    }

    if (!inc) {
        sprintf(INPUT_COLOR,"|15|17");
    }

    sprintf(text,"%s%s%s\x1b[%iD",(char *)temp.c_str(),INPUT_COLOR,repeat.c_str(),len);
}

/**
 * Set default terminal options
 *
 */
int ConsoleIO::term_option(int option)
{
    struct termios attributes;
    switch(option) {

    case 0: // Echo On
        if(tcgetattr(STDIN_FILENO,&attributes) != 0) return (-1);
        attributes.c_lflag |= ECHO;
        attributes.c_lflag |= ICANON;
        attributes.c_lflag |= ISIG;
        if(tcsetattr(STDIN_FILENO,TCSANOW,&attributes) != 0) return (-1);
        break;

    case 1: // Echo Off
        if(tcgetattr(STDIN_FILENO,&attributes) != 0) return (-1);
        attributes.c_lflag &= ~(ECHO);
        if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&attributes) != 0) return (-1);
        break;

    case 2: // Wait for the keyboard to be touched
        if(tcgetattr(STDIN_FILENO,&attributes) != 0) return (-1);
        attributes.c_lflag    &= ~(ICANON);
        attributes.c_cc[VMIN]  = 1;
        attributes.c_cc[VTIME] = 1;
        if(tcsetattr(STDIN_FILENO,TCSANOW,&attributes) != 0) return (-1);
        break;

    case 3:
        if(tcgetattr(STDIN_FILENO,&attributes) != 0) return (-1);
        attributes.c_lflag    &= ~(ICANON);
        attributes.c_lflag    &= ~(ISIG);
        attributes.c_cc[VMIN]  = 0;
        attributes.c_cc[VTIME] = 0;
        if(tcsetattr(STDIN_FILENO,TCSANOW,&attributes) != 0) return (-1);
        break;
    }
    return 0;
}

/**
 * Allow Input
 *
 */
void ConsoleIO::open_keyboard()
{
    term_option(1);
    term_option(3);
}

/**
 * Block Input
 *
 */
void ConsoleIO::close_keyboard()
{
    term_option(0);
}

/**
 * Send Node Packet
 *
 */
int ConsoleIO::send_nmsg(int node, struct nodemessage *nmsg)
{
    int sock, flow;
    struct sockaddr_un name;

    snprintf(name.sun_path, sizeof(name.sun_path),
             "%s/enthral_sock%d", ENTHRALTMP, node);

    name.sun_family = PF_UNIX;
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0)
        return FALSE;

    flow = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flow | O_NONBLOCK);

    sendto(sock, nmsg, sizeof(struct nodemessage), 0,(struct sockaddr *) &name,
           sizeof(struct sockaddr_un));
    close(sock);
    return TRUE;
}

/**
 * Send One Line Message to another Node.
 *
 */
void ConsoleIO::lineolm(int node, const char *ms)
{
    struct nodemessage nmsg;
    memset(&nmsg, 0, sizeof(struct nodemessage));
    nmsg.command = 1;
    snprintf(nmsg.str, sizeof nmsg.str,
             "\n[36mMessage from %s on node %d:\n[0m%-80.80s\n\n",
             user->handle, NODE_NUM, ms);
    send_nmsg(node, &nmsg);
    return;
}


/**
 * Node Message Loop
 */
void ConsoleIO::node_message(int display)
{
    node _node;
    char text[100]= {0};
    char t2[5]= {0};
    unsigned char c;

    int len;
    int nodenum;

    // Display only who's online
    if (display) {
        _node.whoisonline();
        pipe2ansi((char *)"|CR|PA");
        return;
    }

    while(TRUE) {
        _node.whoisonline();

        len = 3;
        sprintf(text,"|CR|11[|03ESC|15/|03Q|11] |15to Skip |CRSend a message to which node #|07? ");

        inputfield(text,len);
        pipe2ansi(text);
        while (1) {
            c = getkey(true);
            t2[0] = c;
            t2[1] = '\0';

            if (toupper(c) == 'Q' || c == 27) {
                return;
            } else if (c == '\n') {
                continue;
            }

            getline(text,len,t2);

            nodenum = atoi(text);
            if (_node.node_exists(nodenum) == TRUE && nodenum != NODE_NUM) {
                len = 60;
                sprintf(text,"|CR|15Message|07: ");
                inputfield(text,len);
                pipe2ansi(text);
                while(1) {
                    getline(text,len);
                    if (strcmp(text,"") != 0 && strcmp(text,"\n") != 0) {
                        lineolm(nodenum,text);
                        break;
                    }
                }
                continue;
            } else if (nodenum == NODE_NUM) {
                sprintf(text,"|CR|04Sorry, Can't send messages to yourself! ");
                pipe2ansi(text);
                startpause();
                break;
            } else {
                sprintf(text,"|CR|04Sorry, No Users on that node! ");
                pipe2ansi(text);
                startpause();
                break;
            }
        }
    }
}

/**
 * Send Node Message on User Login
 */
void ConsoleIO::lineolm_login(int onoff)
{
    node _node;
    struct nodemessage nmsg;
    memset(&nmsg, 0, sizeof(struct nodemessage));
    nmsg.command = 1;

    if (strcmp((char *)user->handle,(char *)"")==0)
        strcpy((char *)user->handle,"User Logging In");

    for(int i = 1; i != 11; i++) {
        if ((_node.node_exists(i) == TRUE) && (i != NODE_NUM)) {
            // Added these node messages to language.txt lateron.
            if (onoff) {
                snprintf(nmsg.str, sizeof nmsg.str,
                         " |12%s |07has logged in on node |12%d|07.",user->handle, NODE_NUM);
            } else {
                snprintf(nmsg.str, sizeof nmsg.str,
                         " |12%s |07has logged off from node |12%d|07.",user->handle, NODE_NUM);
            }
            send_nmsg(i, &nmsg);
        }
    }
    return;
}

/**
 * Grab Node Notifications
 */
int ConsoleIO::processmsg(struct nodemessage *nmsg)
{
    struct olm *myolm;

    PAUSE_SCROLING = TRUE; // Incase Screen Has Scrolling Text, Pause it!

    // Firs get a copy of scrren buffer so we don't write out
    // Node Messaeg after we refresh the screen.
    std::string localscreen = screen_buffer;
    std::string node_msg;

    myolm = new struct olm;
    strcpy(myolm->olm_msg, nmsg->str);
    myolm->olm_number = 0;

    ansiPrintf((char *)"nodemsg");
    node_msg = "|09";
    node_msg += myolm->olm_msg;
    pipe2ansi((char *)node_msg.c_str());

    pipe2ansi((char *)"|CR|CR|PA");

    screen_buffer.erase();
    console_putsn((char *)localscreen.c_str(),localscreen.size(),TRUE);

    PAUSE_SCROLING = FALSE; // Re-Enable Scrolling.

    return 1;
}

/**
 * Scan for Waiting Node Messages
 */
void ConsoleIO::read_input()
{
    struct timeval tv;
    fd_set rset;
    int maxfd;

    FD_ZERO(&rset);

    maxfd = serhandle;
    FD_SET(serhandle, &rset);

    maxfd = console_select_input(maxfd, &rset);

    if (sockfd) {
        if (maxfd < sockfd)
            maxfd = sockfd;
        FD_SET(sockfd, &rset);
    }

    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (select(maxfd + 1, &rset, NULL, NULL, &tv) == -1) {
        if (errno != EINTR)
            abort();

        return;
    }

    if (FD_ISSET(sockfd, &rset)) {
        struct nodemessage ddn;
        if (read(sockfd, &ddn, sizeof(struct nodemessage)) !=
            sizeof(struct nodemessage)) {
            return;
        }
        processmsg(&ddn);
    }
    return;
}

/**
 * Get Single Key and ESC Sequence Input
 */
int ConsoleIO::getkey(bool bWait)
{

    fd_set fds;
    struct timeval tv;
    char buffer[1];
    int  len  = 0;
    int  ch   = 0;

    std::time_t  theTime, callTime;

    double difference = 0;
    int mins = 0;
    int warning = FALSE;


    theTime = time(0);
    int gotesc = FALSE;

    std::string current_buffer;
    do {
        if (!UserLoggedIn) {
            callTime = time(0);

            if ( theTime != (std::time_t)(-1) && callTime != (std::time_t)(-1) ) {
                difference = std::difftime(callTime,theTime); /// (60 * 60 * 24);
                mins = (int)(difference / 60);
            }

            if (mins >= 3) {
                putline((char *)"\r\nConnection Timed out for pre-login inactivity (3) Minutes.\r\n");
                errlog((char *)"Connection Timed out for pre-login inactivity (3) Minutes.");
                exit(0);
            } else if (mins == 2 && warning == FALSE) {
                warning = TRUE;
                putline((char *)"\r\nWARNING * Connection will timeout in 1 minute due to pre-login inactivity.\r\n");
            }
        }

        if (feof(stdin) || ferror(stdin))
            clearerr(stdin);

        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        tv.tv_sec  = 0;
        // .4 second delay / Save CPU Usage
        tv.tv_usec = 400000;

        if (select(STDIN_FILENO+1, &fds, 0, 0, &tv)) {
            if (FD_ISSET(STDIN_FILENO, &fds)) {
                len = read(STDIN_FILENO, buffer, 1);

                if (len <= 0) {
                    if (feof(stdin) || ferror(stdin)) {
                        clearerr(stdin);
                    }
                    continue;
                }
            }

            ch = buffer[0];
            memset(&buffer,0,sizeof(buffer));

            // Don't process multiple esc sequences.
            // grab only the first in a sequence.
            if (ch == 27 && current_buffer.size() == 0) {

                if (gotesc == FALSE) {
                    gotesc = TRUE;
                    memset(&EscapeKey,0,sizeof(EscapeKey));

                    if (ch == '\0') {
                        continue;
                    }
                    current_buffer = ch;
                    continue; // Get next character
                } else {
                    // If we get here again and second char is another ESC, Return 27
                    memset(&EscapeKey,0,sizeof(EscapeKey));
                    current_buffer.erase();
                    return 27;
                }
            }

            // Handles if we got (2) ESC keys in a row.
            gotesc = FALSE;

            // Check if were appending to currnet buffer or returning
            if (current_buffer.size() > 0) {
                if (current_buffer.size() >= 8) {
                    // Drop and reloop for input.
                    current_buffer.erase();
                    continue;
                }

                // Check for Consective ESC sequences, if so exit on ESC
                if (ch == 27 && current_buffer.size() == 1) {
                    memset(&EscapeKey,0,sizeof(EscapeKey));
                    current_buffer.erase();
                    return 27;
                }

                // Translate Hardware Keys,  ESCOA etc.. to ESC[A
                if (ch == 0x4f && current_buffer.size() == 1)
                    ch = '[';

                // Parse ESC Sequence for Match
                if (ch == '[' && current_buffer.size() == 1) {
                    current_buffer = ch; // Overwrite ESC and continue Sequence
                    continue;
                } else {
                    switch (ch) {
                    case '[': // [[ Double Brackets F1 Keys.
                        current_buffer += ch;
                        continue;

                        // Handle [0F END
                    case 'O':
                        // Skip and don't add to buffer
                        // Translate OF to H for End Key
                        continue;

                    case 'A': // Up
                        current_buffer += ch;
                        strcpy(EscapeKey, (char *)current_buffer.c_str());
                        current_buffer.erase();
                        return 27; // Return ESC to Mark Sequence / Key
                        break;
                    case 'B': // Dn
                        current_buffer += ch;
                        strcpy(EscapeKey, (char *)current_buffer.c_str());
                        current_buffer.erase();
                        return 27; // Return ESC to Mark Sequence / Key
                        break;
                    case 'C': // Lt
                        current_buffer += ch;
                        strcpy(EscapeKey, (char *)current_buffer.c_str());
                        current_buffer.erase();
                        return 27; // Return ESC to Mark Sequence / Key
                        break;
                    case 'D': // Rt
                        current_buffer += ch;
                        strcpy(EscapeKey, (char *)current_buffer.c_str());
                        current_buffer.erase();
                        return 27; // Return ESC to Mark Sequence / Key
                        break;

                    case 'F': // [OF
                    case 'K': // End
                        current_buffer += 'K'; // ch;
                        strcpy(EscapeKey, (char *)current_buffer.c_str());
                        current_buffer.erase();
                        return 27; // Return ESC to Mark Sequence / Key
                        break;
                    case 'H': // Home
                        current_buffer += ch;
                        strcpy(EscapeKey, (char *)current_buffer.c_str());
                        current_buffer.erase();
                        return 27; // Return ESC to Mark Sequence / Key
                        break;
                    case 'V': // PageUP
                        current_buffer += ch;
                        strcpy(EscapeKey, (char *)current_buffer.c_str());
                        current_buffer.erase();
                        return 27; // Return ESC to Mark Sequence / Key
                    case 'U': // PageDn
                        current_buffer += ch;
                        strcpy(EscapeKey, (char *)current_buffer.c_str());
                        current_buffer.erase();
                        return 27; // Return ESC to Mark Sequence / Key

                    case '1': // Home
                    case '2': // Insert
                    case '3': // DEL
                    case '4': // ESC
                    case '5': // PG UP
                    case '6': // PG DN
                    case '7': // Function Keys.
                    case '8': // Function Keys.
                    case '9': // Function Keys.
                        current_buffer += ch;
                        continue;

                        // Usually End of Number Sequence.
                    case '~': // Tail
                        current_buffer += ch;
                        strcpy(EscapeKey, (char *)current_buffer.c_str());
                        current_buffer.erase();
                        return 27; // Return ESC to Mark Sequence / Key
                        break;

                    case '\0':
                        //current_buffer += ch;
                        strcpy(EscapeKey, (char *)current_buffer.c_str());
                        current_buffer.erase();
                        return 27; // Return ESC to Mark Sequence / Key
                        break;

                    default :
                        // Not ESC Sequence.
                        // Done with loop.
                        current_buffer += ' ';
                        strcpy(EscapeKey, (char *)current_buffer.c_str());
                        current_buffer.erase();
                        return 27; // Return ESC to Mark Sequence / Key
                        break;
                    }
                }
            }
            // Return normal character
            else {
                break;
            }

        } else {
            // Check if We got ESC key wtih no trailing ESC Sequence.
            // If so Push Back ESC Key was pressed.
            if (current_buffer.size() == 1 && current_buffer[0] == 27) {
                current_buffer.erase();
                return 27;
            }

            read_input();
        }

    } while (bWait);

    // Translater ENTER Key, Some Terms send CR
    if (ch == '\r') {
        ch = '\n';
    }

    return ch;
}

/**
 * Get Input up to <ENTER>
 */
void ConsoleIO::getline(char *line,     // Returns Input into Line
                      int   length,   // Max Input Length of String
                      char *leadoff,  // Data to Display in Default String {Optional}
                      int   hid,      // If input is Echomail as hidden    {Optional}
                      char *chlist)   // Valid Input Char List             {Optional}
{
    int c, cc;
    int  i = 0;
    Col    = 0;
    int EscHit;
    MAX_WIDTH = length;
    int scroll;//, tCol;
    std::string tLine;
    std::string output;
    char sLine[200]  = {0};
    char sList[200]  = {0};

    // If were starting Off Input with a String already in buffer!  display it!
    input.erase();
    if (leadoff != 0) {
        input = leadoff;
        i = input.size();
        Col = i;
        putline((char *)input.c_str());
    }

    // This is a list of Valid inPut Keys
    if (chlist != 0) {
        // Append ESC, CTRL Y, BACKSPACE, ENTER
        sprintf(sLine,"%c%c%c%c%c%c%c",(char)27,(char)25,(char)0x7f,(char)0x08,(char)10,char(8),char(207));
        sprintf(sList,"%s%s",chlist,sLine);
    }
    while (1) {
RESTART:
        c = getkey(true);
        if (c == '\r') c = '\n';

        if (chlist != 0) {
            if(c >= 'a' && c <= 'z') {
                c = toupper(c);
            }
            if(strchr(sList,c) || c == '\n' || c == '\r' || (int)c == 10)
            {  }
            else {
                goto RESTART;
            }
        }

        // Escape in this case, ignore, later add movement in string
        if ((int)c == 27) {
            cc = EscapeKey[1];
            EscHit = TRUE;
        } else {
            EscHit = FALSE;
        }

        // Catch all Escaped Keys for Cursor Movement
        if (EscHit) {
            switch (cc) {
            case '3' : // Delete
                if (i != 0 || Col != 0) {
                    if (USE_CHAR) {
                        console_putsn((char *)"\x1b[D\xB0\x1b[D",7);
                    } else {
                        console_putsn((char *)"\x1b[D \x1b[D",7);
                    }
                    input.erase(Col-1,1);
                    --i;
                    --Col;
                }
                break;

            default :
                break;
            }

        } else if ((int)c == 25) {
            // CTRL Y - Clear Line
            input.erase();
            while (1) {
                scroll = rt_arrow();
                if (!scroll) break;
            }
            i = Col;
            for (; i != 0; i--) {
                if (USE_CHAR) {
                    console_putsn((char *)"\x1b[D\xB0\x1b[D",7);
                } else {
                    console_putsn((char *)"\x1b[D \x1b[D",7);
                }
            }
            i = 0;
            Col = i;
        }

        // Do destructive backspace
        // on VT100 Terms 127 DEL == BS!
        else if ((int)c == 0x08 || (int)c == 127 || int(c) == 8 || int(c) == 207 || (int)c == 0x7f) {
            if (i != 0 || Col != 0) {
                if (USE_CHAR) {
                    console_putsn((char *)"\x1b[D\xB0\x1b[D",7);
                } else {
                    console_putsn((char *)"\x1b[D \x1b[D",7);
                }
                input.erase(Col-1,1);
                --i;
                --Col;
            }
        }
        // Normal Key Input, Letters & numbers
        else if((int)c > 31 && (int)c < 126) {
            if (i != length-1) {
                if (hid) {
                    output = HIDDEN_CHAR;
                    console_putsn((char *)output.c_str(),output.size());
                } else {
                    output = c;
                    console_putsn((char *)output.c_str(),output.size());
                }
                input += c;
                ++i;
                ++Col;
            }
        } else if (c == 10 || c == '\r' || c == '\n') {
            input += '\0';
            strncpy(line,(char *)input.c_str(),length);
            break;
        }
    }

    // Restore Background color after input received.
    Col = 0;
    char sReplace[15]= {0};
    ansi_bg(sReplace, 16);
}

/**
 * Move Cursor Right on Arror Key
 */
int ConsoleIO::rt_arrow()
{
    if (Col < MAX_WIDTH-1) {
        // Not at End of Line
        //if (Col >= strlen((char *)input.c_str())-1) {
        if (Col+1 < (signed)input.size()) {
            ++Col;
        }
        else {
            return FALSE;
        }

        console_putsn((char *)"\x1b[C",3);
        return TRUE;
    }

    return FALSE;
}

/**
 * Move Cursor Left on Arror Key
 */
int ConsoleIO::lt_arrow()
{
    if (Col != 0) {
        console_putsn((char *)"\x1b[D",3);
        --Col;
        return TRUE;
    }

    return FALSE;
}

/**
 * Move Cursor end of Line on END key
 */
void ConsoleIO::end_cursor()
{
    while (rt_arrow());
    Col = MAX_WIDTH-1;
}

/**
 * Move Cursor begining of Line on HOME key
 */
void ConsoleIO::home_cursor()
{
    while (lt_arrow());
    Col = 0;
}


/**
 * Set ANSI Foreground colors
 */
void ConsoleIO::ansi_fg(char *data, int fg)
{
    switch (fg) {
    case 0:
        strcat(data, "x[0;30m");
        break;
    case 1:
        strcat(data, "x[0;34m");
        break;
    case 2:
        strcat(data, "x[0;32m");
        break;
    case 3:
        strcat(data, "x[0;36m");
        break;
    case 4:
        strcat(data, "x[0;31m");
        break;
    case 5:
        strcat(data, "x[0;35m");
        break;
    case 6:
        strcat(data, "x[0;33m");
        break;
    case 7:
        strcat(data, "x[0;37m");
        break;
    case 8:
        strcat(data, "x[1;30m");
        break;
    case 9:
        strcat(data, "x[1;34m");
        break;
    case 10:
        strcat(data, "x[1;32m");
        break;
    case 11:
        strcat(data, "x[1;36m");
        break;
    case 12:
        strcat(data, "x[1;31m");
        break;
    case 13:
        strcat(data, "x[1;35m");
        break;
    case 14:
        strcat(data, "x[1;33m");
        break;
    case 15:
        strcat(data, "x[1;37m");
        break;
    default :
        break;
    }
    data[0] = '\x1b';
}

/**
 * Set ANSI Background colors
 */
void ConsoleIO::ansi_bg(char *data, int bg)
{
    switch (bg) {
    case 16:
        strcat(data, "x[40m");
        break;
    case 17:
        strcat(data, "x[44m");
        break;
    case 18:
        strcat(data, "x[42m");
        break;
    case 19:
        strcat(data, "x[46m");
        break;
    case 20:
        strcat(data, "x[41m");
        break;
    case 21:
        strcat(data, "x[45m");
        break;
    case 22:
        strcat(data, "x[43m");
        break;
    case 23:
        strcat(data, "x[47m");
        break;
        // Default to none.
    case 24:
        strcat(data, "x[0m");
        break;
    default :
        break;
    }
    data[0] = '\x1b';
}

/**
 * Parse String with MCI Code value, then return value in the MCI Codes Place.
 */
void ConsoleIO::ParseLocalMCI(std::string &AnsiString, std::string mcicode, std::string outbuff)
{
    std::string::size_type id1 = 0;
    do {
        // Parse New Message's MCI Code
        id1 = AnsiString.find(mcicode,0);
        if (id1 != std::string::npos) {
            AnsiString.replace(id1,3,outbuff);
            id1 = AnsiString.find(mcicode,0);
        }
    } while (id1 != std::string::npos);
}

/**
 * Takes care of parses for multiple (delays) |DF in a string
 * Ansi File Continues parsing after delay is hit.
 */
void ConsoleIO::afilecon(std::string &AnsiString, int buffer)
{
    int id1 = 0;
    int id2 = 0;
    std::string tmp = AnsiString;
    std::string tmp2;

    id1  = tmp.find("|DF",0);
    id2  = tmp.find(" ",id1+3);
    tmp2 = tmp.substr(id1+3,id2-(id1+3));
    ansiPrintf((char *)tmp2.c_str(), buffer);
    AnsiString = tmp.erase(id1,tmp2.size()+2);
}

/**
 * Main Output for All String Data, Parses PIPE color and MCI Codes.
 *
 */
void ConsoleIO::pipe2ansi(char* szString, int buffer)
{
    std::string::size_type  id1 = 0, test;    // Pipe Position
    char szTmp[3];         // Holds 1, 2nd digit of Pipe

    char szPos1[3];        // Hold XY Pos for Ansi Postion Codes
    char szPos2[3];        // Hold XY Pos for Ansi Postion Codes
    char szReplace[1024];  // Holds Converted Pipe 2 Ansi

    std::string fTmp;
    std::string AnsiString = szString;

    // Message Area List
    msgs mf;
    mb_list_rec mr;
    fb_list_rec fr;

    // Search for First Pipe
    id1 = 0;
    id1 = AnsiString.find("|",0);
    while (id1 != std::string::npos) {
        if (id1 != std::string::npos) {
            memset(&szTmp,0,sizeof(szTmp));
            memset(&szReplace,0,sizeof(szReplace));
            szTmp[0] = AnsiString[id1+1];  // Get First # after Pipe
            szTmp[1] = AnsiString[id1+2];  // Get Second Number After Pipe
            if (szTmp[0] == '\0' || szTmp[0] == '\r' || szTmp[0] == EOF) break;
            if (szTmp[1] == '\0' || szTmp[1] == '\r' || szTmp[0] == EOF) break;

            if (isdigit(szTmp[0]) && isdigit(szTmp[1])) {
                switch (szTmp[0]) {
                    // Parse from pipe to Foreground/back Color Ansi Sequences.
                case '0' :
                    switch (szTmp[1]) {
                    case '0' :
                        ansi_fg(szReplace, 0);
                        break;
                    case '1' :
                        ansi_fg(szReplace, 1);
                        break;
                    case '2' :
                        ansi_fg(szReplace, 2);
                        break;
                    case '3' :
                        ansi_fg(szReplace, 3);
                        break;
                    case '4' :
                        ansi_fg(szReplace, 4);
                        break;
                    case '5' :
                        ansi_fg(szReplace, 5);
                        break;
                    case '6' :
                        ansi_fg(szReplace, 6);
                        break;
                    case '7' :
                        ansi_fg(szReplace, 7);
                        break;
                    case '8' :
                        ansi_fg(szReplace, 8);
                        break;
                    case '9' :
                        ansi_fg(szReplace, 9);
                        break;
                    default :
                        ++id1;
                        break;
                    }
                    break;

                case '1' :
                    switch (szTmp[1]) {
                    case '0' :
                        ansi_fg(szReplace, 10);
                        break;
                    case '1' :
                        ansi_fg(szReplace, 11);
                        break;
                    case '2' :
                        ansi_fg(szReplace, 12);
                        break;
                    case '3' :
                        ansi_fg(szReplace, 13);
                        break;
                    case '4' :
                        ansi_fg(szReplace, 14);
                        break;
                    case '5' :
                        ansi_fg(szReplace, 15);
                        break;
                    case '6' :
                        ansi_bg(szReplace, 16);
                        break;
                    case '7' :
                        ansi_bg(szReplace, 17);
                        break;
                    case '8' :
                        ansi_bg(szReplace, 18);
                        break;
                    case '9' :
                        ansi_bg(szReplace, 19);
                        break;
                    default :
                        ++id1;
                        break;
                    }
                    break;

                case '2' :
                    switch (szTmp[1]) {
                    case '0' :
                        ansi_bg(szReplace, 20);
                        break;
                    case '1' :
                        ansi_bg(szReplace, 21);
                        break;
                    case '2' :
                        ansi_bg(szReplace, 22);
                        break;
                    case '3' :
                        ansi_bg(szReplace, 23);
                        break;
                    default :
                        ++id1;
                        break;
                    }
                    break;

                default :
                    ++id1;
                    break;
                }

                // Replace pipe code with Ansi Sequence
                if (strcmp(szReplace,"") != 0) {
                    AnsiString.replace(id1,3,szReplace);
                }
            }
            // Else not a Pipe Color / Parse for Screen Modification
            else if (szTmp[0] != 'U' && szTmp[0] != 'D') {
                // Carriage Return / New Line
                if (strcmp(szTmp,"CR") == 0) {
                    ansi_bg(szReplace, 16);
                    strcat(szReplace,"\r\n");
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
                // Clear Screen
                else if (strcmp(szTmp,"CS") == 0) {
                    ansi_bg(szReplace, 16);
                    // Set Scroll Region, Clear Background, Then Home Cursor.
                    strcat(szReplace,"\x1b[2J\x1b[1;1H");
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
                // Sexy XY Cursor Position
                else if (strcmp(szTmp,"XY") == 0) {
                    memset(&szPos1,0,sizeof(szPos1));
                    memset(&szPos2,0,sizeof(szPos2));
                    // X Pos
                    szPos1[0] = AnsiString[id1+3];
                    szPos1[1] = AnsiString[id1+4];
                    // Y Pos
                    szPos2[0] = AnsiString[id1+5];
                    szPos2[1] = AnsiString[id1+6];
                    ansi_bg(szReplace, 16);
                    sprintf(szReplace,"%s\x1b[%i;%iH",szReplace, atoi(szPos2),atoi(szPos1));
                    AnsiString.replace(id1,7,szReplace);
                }
                // Current BBS Version
                else if (strcmp(szTmp,"SI") == 0) {
                    strcat(szReplace,BBSVERSION);
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
                // Ip Address
                else if (strcmp(szTmp,"IP") == 0) {
                    strcat(szReplace,UsersIP);
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
                // Current Message Area # ie 1.  2.  3.
                else if (strcmp(szTmp,"M#") == 0) {
                    sprintf(szReplace,"%lu",(ulong)user->lastmbarea+1);
                    //sprintf(szReplace,"%i",CURRENT_MAREA+1);
                    AnsiString.replace(id1,3,szReplace);
                }
                // Message Area Name ie  General Messages
                else if (strcmp(szTmp,"MA") == 0) {
                    // Check this lateron, not getting descpt for #12 Message Area.
                    mf.read_mbaselist(&mr,user->lastmbarea);
                    //mf.read_mbaselist(&mr,CURRENT_MAREA);
                    strcat(szReplace,(char *)mr.mbdisplay);
                    AnsiString.replace(id1,3,szReplace);
                }

                else if (strcmp(szTmp,"F#") == 0) {
                    sprintf(szReplace,"%i",CURRENT_FAREA);
                    AnsiString.replace(id1,3,szReplace);
                }
                // File Area
                else if (strcmp(szTmp,"FA") == 0) {
                    strcat(szReplace,(char *)fr.fbdisplay);
                    AnsiString.replace(id1,3,szReplace);
                }
                // Node Number
                else if (strcmp(szTmp,"N#") == 0) {
                    sprintf(szReplace,"%i",NODE_NUM);
                    AnsiString.replace(id1,3,szReplace);
                }
                // Pause
                else if (strcmp(szTmp,"PA") == 0) {
                    test = AnsiString.size();
                    fTmp = AnsiString.substr(0,id1);

                    console_putsn((char*)fTmp.c_str(),fTmp.size());
                    fTmp.erase();

                    AnsiString.erase(id1,3);
                    AnsiString.erase(0,id1);

                    startpause();
                }
                else if (strcmp(szTmp,"WM") == 0) {
                    sprintf(szReplace,"%ld",(mf.CountAllNewMsgs(user)));
                    AnsiString.replace(id1,3,szReplace);
                }
                else if (strcmp(szTmp,"WE") == 0) {
                    sprintf(szReplace,"%ld",(mf.CountAllNewMsgs(user)));
                    AnsiString.replace(id1,3,szReplace);
                }
                else if (strcmp(szTmp,"LD") == 0) {
                    sprintf(szReplace,"%s",getLastCallDays(user->dtlaston).c_str());
                    AnsiString.replace(id1,3,szReplace);
                }
                else if (strcmp(szTmp,"EN") == 0) { // CP437 | UTF8 Encoding
                    if (UTF8Output) {
                        sprintf(szReplace,"UTF-8");
                    }
                    else {
                        sprintf(szReplace,"CP437");
                    }
                    AnsiString.replace(id1,3,szReplace);
                } else if (strcmp(szTmp,"TT") == 0) { // Client Terminal Type
                    // Read in Unique FileAreaScan
                    std::string instr;

                    ifstream in;
                    in.open(CLIENT_TERM);
                    if (in.is_open()) {
                        std::getline(in,instr);
                        in.close();
                    }

                    if (instr.size() == 0) instr = "UNKNOWN";
                    sprintf(szReplace,"%s",instr.c_str());
                    AnsiString.replace(id1,3,szReplace);
                } else if (strcmp(szTmp,"WS") == 0) { // Client Windows Size
                    sprintf(szReplace,"%ix%i",TERM_WIDTH, TERM_HEIGHT);
                    AnsiString.replace(id1,3,szReplace);
                }

                else ++id1;
            } else {
                switch (szTmp[0]) {
                case 'U' : // User MCI Codes
                    switch (szTmp[1]) {
                    case '#' :
                        sprintf(szReplace,"%ld",(ulong)user->idx+1);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'H' :
                        sprintf(szReplace,"%s",user->handle);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'N' :
                        sprintf(szReplace,"%s",user->name);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'P' :
                        sprintf(szReplace,"%s",user->password);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'G' :
                        sprintf(szReplace,"%s",(toupper(user->sex) == 'M') ? "Male" : "Female");
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'E' :
                        sprintf(szReplace,"%s",user->email);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'O' :
                        sprintf(szReplace,"%s",user->usernote);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'S' :
                        sprintf(szReplace,"%s",user->startmenu);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'Q' :
                        sprintf(szReplace,"%s",user->c_question);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'A' :
                        sprintf(szReplace,"%s",user->c_answer);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'B' :
                        sprintf(szReplace,"%s",Sec2Date(user->dtbday).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'F' :
                        sprintf(szReplace,"%s",Sec2DateTM(user->dtfirston,1).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'L' :
                        sprintf(szReplace,"%s",Sec2DateTM(user->dtlaston,1).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'X' :
                        sprintf(szReplace,"%s",getAge(user->dtbday).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'Z' :
                        sprintf(szReplace,"%s",(toupper(user->emprivate) == 'Y') ? "Yes" : "No");
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    default :
                        ++id1;
                        break;
                    }
                    break;

                case 'D' : // Date Time
                    switch (szTmp[1]) {
                    case '1' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),1).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case '2' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),2).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case '3' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),3).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case '4' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),4).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case '5' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),5).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case '6' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),6).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;
                        // Display a file
                    case 'F' :
                        afilecon(AnsiString,buffer);
                        id1 = 0;
                        break;
                        // Delay for 1 second
                    case 'E' :
                        AnsiString.erase(id1,3);
                        // If anymore string data after |DE, chop up
                        // and display everything before current |DE then move on to next
                        // After the pause.
                        test = AnsiString.size();
                        if (test > id1 && AnsiString.find("|",0) != std::string::npos) {
                            fTmp = AnsiString.substr(0,id1);
                            console_putsn((char*)fTmp.c_str(),fTmp.size());
                            fTmp.erase();
                            AnsiString.erase(0,id1);
                            id1 = 0;
                        } else {
                            console_putsn((char*)AnsiString.c_str(),AnsiString.size());
                            AnsiString.erase();
                            id1 = 0;
                        }
                        sleep(1);
                        break;

                    default :
                        ++id1;
                        break;

                    }
                    break;

                default :
                    ++id1;
                    break;
                }
            }
        } else break;
        id1 = AnsiString.find("|",id1);
    }

    console_putsn((char*)AnsiString.c_str(),AnsiString.size(),buffer);
}

/**
 * Parses and Retuns a String with Pipe and MCI Codes.
 *
 */
std::string ConsoleIO::pipe2string(std::string szString)
{
    std::string::size_type  id1 = 0;
    char szTmp[3];         // Holds 1, 2nd digit of Pipe

    char szPos1[3];        // Hold XY Pos for Ansi Postion Codes
    char szPos2[3];        // Hold XY Pos for Ansi Postion Codes
    char szReplace[1024];  // Holds Converted Pipe 2 Ansi

    std::string fTmp;
    std::string AnsiString = szString;

    // Message Area List
    msgs mf;
    mb_list_rec mr;
    fb_list_rec fr;

    // Search for First Pipe
    id1 = 0;
    id1 = AnsiString.find("|",0);
    while (id1 != std::string::npos) {
        if (id1 != std::string::npos) {
            memset(&szTmp,0,sizeof(szTmp));
            memset(&szReplace,0,sizeof(szReplace));
            szTmp[0] = AnsiString[id1+1];  // Get First # after Pipe
            szTmp[1] = AnsiString[id1+2];  // Get Second Number After Pipe
            if (szTmp[0] == '\0' || szTmp[0] == '\r' || szTmp[0] == EOF) break;
            if (szTmp[1] == '\0' || szTmp[1] == '\r' || szTmp[0] == EOF) break;

            if (isdigit(szTmp[0]) && isdigit(szTmp[1])) {
                switch (szTmp[0]) {
                    // Parse from pipe to Foreground/back Color Ansi Sequences.
                case '0' :
                    switch (szTmp[1]) {
                    case '0' :
                        ansi_fg(szReplace, 0);
                        break;
                    case '1' :
                        ansi_fg(szReplace, 1);
                        break;
                    case '2' :
                        ansi_fg(szReplace, 2);
                        break;
                    case '3' :
                        ansi_fg(szReplace, 3);
                        break;
                    case '4' :
                        ansi_fg(szReplace, 4);
                        break;
                    case '5' :
                        ansi_fg(szReplace, 5);
                        break;
                    case '6' :
                        ansi_fg(szReplace, 6);
                        break;
                    case '7' :
                        ansi_fg(szReplace, 7);
                        break;
                    case '8' :
                        ansi_fg(szReplace, 8);
                        break;
                    case '9' :
                        ansi_fg(szReplace, 9);
                        break;
                    default :
                        ++id1;
                        break;
                    }
                    break;

                case '1' :
                    switch (szTmp[1]) {
                    case '0' :
                        ansi_fg(szReplace, 10);
                        break;
                    case '1' :
                        ansi_fg(szReplace, 11);
                        break;
                    case '2' :
                        ansi_fg(szReplace, 12);
                        break;
                    case '3' :
                        ansi_fg(szReplace, 13);
                        break;
                    case '4' :
                        ansi_fg(szReplace, 14);
                        break;
                    case '5' :
                        ansi_fg(szReplace, 15);
                        break;
                    case '6' :
                        ansi_bg(szReplace, 16);
                        break;
                    case '7' :
                        ansi_bg(szReplace, 17);
                        break;
                    case '8' :
                        ansi_bg(szReplace, 18);
                        break;
                    case '9' :
                        ansi_bg(szReplace, 19);
                        break;
                    default :
                        ++id1;
                        break;
                    }
                    break;

                case '2' :
                    switch (szTmp[1]) {
                    case '0' :
                        ansi_bg(szReplace, 20);
                        break;
                    case '1' :
                        ansi_bg(szReplace, 21);
                        break;
                    case '2' :
                        ansi_bg(szReplace, 22);
                        break;
                    case '3' :
                        ansi_bg(szReplace, 23);
                        break;
                    default :
                        ++id1;
                        break;
                    }
                    break;

                default :
                    ++id1;
                    break;
                }

                // Replace pipe code with Ansi Sequence
                if (strcmp(szReplace,"") != 0)
                    AnsiString.replace(id1,3,szReplace);
            }
            // Else not a Pipe Color / Parse for Screen Modification
            else if (szTmp[0] != 'U' && szTmp[0] != 'D') {
                // Carriage Return / New Line
                if (strcmp(szTmp,"CR") == 0) {
                    ansi_bg(szReplace, 16);
                    strcat(szReplace,"\r\n");
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
                // Clear Screen
                else if (strcmp(szTmp,"CS") == 0) {
                    ansi_bg(szReplace, 16);
                    strcat(szReplace,"\x1b[2J\x1b[1;1H");
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
                // Sexy XY Cursor Position
                else if (strcmp(szTmp,"XY") == 0) {
                    memset(&szPos1,0,sizeof(szPos1));
                    memset(&szPos2,0,sizeof(szPos2));
                    // X Pos
                    szPos1[0] = AnsiString[id1+3];
                    szPos1[1] = AnsiString[id1+4];
                    // Y Pos
                    szPos2[0] = AnsiString[id1+5];
                    szPos2[1] = AnsiString[id1+6];
                    ansi_bg(szReplace, 16);  // Clear Background Attribute first
                    sprintf(szReplace,"%s\x1b[%i;%iH",szReplace, atoi(szPos2),atoi(szPos1));
                    AnsiString.replace(id1,7,szReplace);
                }
                // Current BBS Version
                else if (strcmp(szTmp,"SI") == 0) {
                    strcat(szReplace,BBSVERSION);
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
                // Ip Address
                else if (strcmp(szTmp,"IP") == 0) {
                    strcat(szReplace,UsersIP);
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
                // Current Message Area # ie 1.  2.  3.
                else if (strcmp(szTmp,"M#") == 0) {
                    sprintf(szReplace,"%lu",(ulong)user->lastmbarea+1);
                    AnsiString.replace(id1,3,szReplace);
                }
                // Message Area Name ie  General Messages
                else if (strcmp(szTmp,"MA") == 0) {
                    // Check this lateron, not getting descpt for #12 Message Area.
                    mf.read_mbaselist(&mr,user->lastmbarea);
                    strcat(szReplace,(char *)mr.mbdisplay);
                    AnsiString.replace(id1,3,szReplace);
                }

                else if (strcmp(szTmp,"F#") == 0) {
                    sprintf(szReplace,"%i",CURRENT_FAREA);
                    AnsiString.replace(id1,3,szReplace);
                }
                // File Area
                else if (strcmp(szTmp,"FA") == 0) {
                    strcat(szReplace,(char *)fr.fbdisplay);
                    AnsiString.replace(id1,3,szReplace);
                }
                // Node Number
                else if (strcmp(szTmp,"N#") == 0) {
                    sprintf(szReplace,"%i",NODE_NUM);
                    AnsiString.replace(id1,3,szReplace);
                }
                // Pause
                else if (strcmp(szTmp,"PA") == 0) {
                    fTmp = AnsiString.substr(0,id1);

                    console_putsn((char*)fTmp.c_str(),fTmp.size());
                    fTmp.erase();

                    AnsiString.erase(id1,3);
                    AnsiString.erase(0,id1);

                    startpause();
                }

                else if (strcmp(szTmp,"WM") == 0) {
                    sprintf(szReplace,"%ld",(mf.CountAllNewMsgs(user)));
                    AnsiString.replace(id1,3,szReplace);
                }
                else if (strcmp(szTmp,"WE") == 0) {
                    sprintf(szReplace,"%ld",(mf.CountAllNewMsgs(user)));
                    AnsiString.replace(id1,3,szReplace);
                } else if (strcmp(szTmp,"LD") == 0) {
                    sprintf(szReplace,"%s",getLastCallDays(user->dtlaston).c_str());
                    AnsiString.replace(id1,3,szReplace);
                } else if (strcmp(szTmp,"EN") == 0) { // CP437 | UTF8 Encoding
                    if (UTF8Output)
                        sprintf(szReplace,"UTF-8");
                    else
                        sprintf(szReplace,"CP437");
                    AnsiString.replace(id1,3,szReplace);
                } else if (strcmp(szTmp,"TT") == 0) { // Client Terminal Type
                    // Read in Unique FileAreaScan
                    std::string instr;

                    ifstream in;
                    in.open(CLIENT_TERM);
                    if (in.is_open()) {
                        std::getline(in,instr);
                        in.close();
                    }

                    if (instr.size() == 0) instr = "UNKNOWN";
                    sprintf(szReplace,"%s",instr.c_str());
                    AnsiString.replace(id1,3,szReplace);
                } else if (strcmp(szTmp,"WS") == 0) { // Client Windows Size
                    sprintf(szReplace,"%ix%i",TERM_WIDTH, TERM_HEIGHT);
                    AnsiString.replace(id1,3,szReplace);
                }

                else ++id1;
            } else {
                switch (szTmp[0]) {
                case 'U' : // User MCI Codes
                    switch (szTmp[1]) {
                    case '#' :
                        sprintf(szReplace,"%ld",(ulong)user->idx+1);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'H' :
                        sprintf(szReplace,"%s",user->handle);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'N' :
                        sprintf(szReplace,"%s",user->name);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'P' :
                        sprintf(szReplace,"%s",user->password);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'G' :
                        sprintf(szReplace,"%s",(toupper(user->sex) == 'M') ? "Male" : "Female");
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'E' :
                        sprintf(szReplace,"%s",user->email);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'O' :
                        sprintf(szReplace,"%s",user->usernote);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'S' :
                        sprintf(szReplace,"%s",user->startmenu);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'Q' :
                        sprintf(szReplace,"%s",user->c_question);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'A' :
                        sprintf(szReplace,"%s",user->c_answer);
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'B' :
                        sprintf(szReplace,"%s",Sec2Date(user->dtbday).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'F' :
                        sprintf(szReplace,"%s",Sec2DateTM(user->dtfirston,1).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'L' :
                        sprintf(szReplace,"%s",Sec2DateTM(user->dtlaston,1).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'X' :
                        sprintf(szReplace,"%s",getAge(user->dtbday).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case 'Z' :
                        sprintf(szReplace,"%s",(toupper(user->emprivate) == 'Y') ? "Yes" : "No");
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    default :
                        ++id1;
                        break;
                    }
                    break;

                case 'D' : // Date Time
                    switch (szTmp[1]) {
                    case '1' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),1).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case '2' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),2).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case '3' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),3).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case '4' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),4).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case '5' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),5).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    case '6' :
                        sprintf(szReplace,"%s",Sec2DateTM(GetCurrentDTSec(),6).c_str());
                        AnsiString.replace(id1,3,szReplace);
                        break;

                    default :
                        ++id1;
                        break;

                    }
                    break;

                default :
                    ++id1;
                    break;
                }
            }
        } else break;
        id1 = AnsiString.find("|",id1);
    }
    return(AnsiString);
}

/**
 * Display Ansi file, read from file, also parses pipe codes
 * And control ansi delay or aborting
 * (WIP) Need to move hard coded ansi filenames to MCI Code
 * for delayed ANSI screens
 */
BOOL ConsoleIO::ansiPrintf(char *filename, int delay, int abort)
{
    int ret = 0, c = 0;
    std::string AnsiBuf;
    std::string path = ANSIPATH;
    path.append(filename);

    if (isANSI == TRUE)
        path += ".ans";
    else
        path += ".asc";

    language _lang;
    char sCmd[255] = {0};
    char text[255] = {0};

    FILE *inStream;
    std::string line;
    std::string::size_type id1;

    //return false so we can generate menu commands.
    if (strcmp(filename,"") == 0)
        return FALSE;

    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL) {
        return FALSE;
    }

    // Add Custom Options for Which ansi to delay lateron
    // Maybe an MCI Code or soo...
    CONT_INPUT = FALSE;

    if (strcmp(filename,"welcome") == 0) {
        delay = TRUE;
        abort = TRUE;
    }

    else if (strcmp(filename,"logoff") == 0) {
        delay = TRUE;
        abort = TRUE;
    } else if (strcmp(filename,"aflogin") == 0) {
        delay = TRUE;
        abort = TRUE;
    } else if (strcmp(filename,"userwel2") == 0) {
        delay = TRUE;
        abort = TRUE;
    } else if (strcmp(filename,"enthral") == 0) {
        delay = TRUE;
        abort = TRUE;
    }

    fd_set fds;
    struct timeval tv;
    struct timespec ts;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    // Set Output Delay for NanoSleep()
    char buffer[255]= {0};
    int charCounter = 0;
    do {
        c = getc(inStream);
        ++charCounter;

        if (c == '\r') { //  || charCounter >= 160)
            charCounter = 0;
            // End of Line.
            if (delay == TRUE) {
                ts.tv_sec  = 0;
                ts.tv_nsec = 20000000;
                while (1) {
                    // Sleep for the time specified in tv. If interrupted by a
                    // signal, place the remaining time left to sleep back into tv.
                    int rval = nanosleep (&ts, &ts);
                    if (rval == 0)
                        // Completed the entire sleep time; all done.
                        break;
                    else if (errno == EINTR)
                        // Interrupted by a signal. Try again.
                        continue;
                    else
                        // Some other error; bail out.
                        break;
                }

                if (abort == TRUE) {

                    if (feof(stdin) || ferror(stdin))
                        clearerr(stdin);

                    FD_ZERO(&fds);
                    FD_SET(STDIN_FILENO, &fds);
                    tv.tv_sec  = 0;
                    tv.tv_usec = 0;
                    if (select(STDIN_FILENO+1, &fds, 0, 0, &tv)) {

                        if ((ret = read(STDIN_FILENO, buffer, 1)) > 0);
                        if (buffer[0] == SPACE || buffer[0] == ' ') {
                            CONT_INPUT = TRUE;
                            AnsiBuf.erase();
                            break;
                        } else {
                            memset(buffer,0,sizeof(buffer));
                        }
                    }
                }

            }

            // Displays 1 Line at a time and checks for Input / Abort
            id1 = AnsiBuf.find("\x1b[2J",0);
            if (id1 != std::string::npos) {
                AnsiBuf.replace(id1,4,"|CS");
            }

            // Ignore SAUSE Lines
            id1 = AnsiBuf.find("SAUCE00",1);
            if (id1 != std::string::npos) {
                AnsiBuf.erase(id1-1);
                pipe2ansi((char*)AnsiBuf.c_str());
                AnsiBuf.erase();
                break;
            }

            pipe2ansi((char*)AnsiBuf.c_str());
            AnsiBuf.erase();


        } else {
            // Ansi is 1 line, no newlines found!
            id1 = AnsiBuf.find("\x1b[2J",0);
            if (id1 != std::string::npos) {
                AnsiBuf.replace(id1,4,"|CS");
            }

            // Ignore SAUSE Lines
            id1 = AnsiBuf.find("SAUCE00",1);
            if (id1 != std::string::npos) {
                AnsiBuf.erase(id1-1);
                pipe2ansi((char*)AnsiBuf.c_str());
                AnsiBuf.erase();
                break;
            }
        }
        if (c != EOF) AnsiBuf += c;
        else {

            // Ignore SAUSE Lines
            id1 = AnsiBuf.find("SAUCE00",1);
            if (id1 != std::string::npos) {
                AnsiBuf.erase(id1-1);
                pipe2ansi((char*)AnsiBuf.c_str());
                AnsiBuf.erase();
                break;
            }

            pipe2ansi((char*)AnsiBuf.c_str());
            AnsiBuf.erase();
            break;
        }
    } while ( c != EOF );
    fclose(inStream);

    // throw this aborted sting into language file lateron...
    if (CONT_INPUT == TRUE) {
        if (isANSI) {
            _lang.lang_get(text,59);
            sprintf(sCmd,"%s",text);
            pipe2ansi(sCmd);
            startpause();
        } else {
            putline((char *)" >>> aborted ...");
            startpause();
        }
    }

    return TRUE;
}

/**
 * Reads in Ansi file into Buffer Only
 */
void ConsoleIO::readinAnsi(std::string FileName, std::string &buff)
{
    std::string path = ANSIPATH;
    path += FileName;

    if (isANSI == TRUE)
        path += ".ans";
    else
        path += ".asc";

    FILE *fp;
    std::string::size_type id1 = 0;

    int c = 0;
    if ((fp = fopen(path.c_str(), "r+")) ==  NULL) {
        return;
    }
    do {
        c = getc(fp);
        if (c != EOF) {
            buff += c;
        }
    } while (c != EOF);

    // If we have clear, replace with MCI code so it will add
    // Clear then Also goto [1,1] Home Cursor Position.
    do {
        id1 = buff.find("\x1b[2J",0);
        if (id1 != std::string::npos) {
            buff.replace(id1,4,"|CS");
        }
    } while(id1 != std::string::npos);

    fclose(fp);
}

/**
 * Start Animated Pause
 */
int ConsoleIO::startpause(char *script)
{
    std::time_t  theTime, callTime;
    int mins = 0;
    int difference = 0;

    int ret;
    std::string path = INIPATH;
    if (strlen(script) == 0)
        path += "pause.ini";
    else
        path += script;

    ifstream inStream;
    inStream.open( path.c_str() );
    if (!inStream.is_open()) {
        perror(" ini ***cannot open pause.ini, check if it exists and permissions!");
        return 0;
    }

    // count lines in files to loop through
    int lines = 0;
    std::string cfgdata;
    for (;;) {
        std::getline(inStream,cfgdata);
        if (cfgdata[0] != '#')
            ++lines;
        if(inStream.eof()) break;
    }
    inStream.close();


    std::string *sLine = new std::string[lines];
    if (!sLine) {
        return 0;
    }

    // Now Read in All Lines
    ifstream ins2;
    ins2.open( path.c_str() );
    if (!ins2.is_open()) {
        return 0;
    }

    int i1 = 0;
    while (1) {
        std::getline(ins2,cfgdata);
        if (cfgdata[0] != '#') {
            sLine[i1] = cfgdata;
            ++i1;
        }
        if(ins2.eof()) break;
    }
    ins2.close();


    fd_set fds;
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = 0;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);


    char buffer[20]      = {0};
    char outstring[1024] = {0};
    int  iCnt            =  0;

    theTime = time(0);

    bool done = false;
    while (!done) {
        callTime = time(0);
        if ( theTime != (std::time_t)(-1) && callTime != (std::time_t)(-1) ) {
            difference = std::difftime(callTime,theTime);
            if (difference > 0) {
                mins = (int)(difference / 60);
            }
        }

        // Time Out!  Break out of Pause.
        if (mins >= 5) {
            errlog((char *)"startpause() Timed out for inactivity (5) Minutes.");
            putline ((char *)"\r\x1b[K\x1b[1G");
            done = true;
        }

        sprintf(outstring,"\r\x1b[K\x1b[1G%s",(char *)sLine[iCnt].c_str());
        pipe2ansi(outstring, FALSE);
        ++iCnt;

        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);

        if (select(STDIN_FILENO+1, &fds, 0, 0, &tv)) {
            // After key is hit, make sure to clear out remaining data in buffer.
            if ((ret = read(STDIN_FILENO, buffer, sizeof(buffer)-1)) > 0);
            putline ((char *)"\r\x1b[K\x1b[1G"); // Restore, then delete eol for Redraw.
            done = true;
        }

        usleep(12000*10);
        if (iCnt == lines)
            iCnt = 0;
    }

    if (sLine)
        delete [] sLine;
    return (int)buffer[0];
}

/**
 * Uses ESC Sequence to Parse for Terminal X/Y Position.
 */
int ConsoleIO::getxy()
{
    fd_set fds;
    struct timeval tv;
    static char buffer[1024];
    static int e = 0;
    char ch = 0;

    char xy[5] = {0};
    int i      = 0;
    int ret    = 0;

    ansi_x = 0;
    ansi_y = 0;

    putline ((char *)"\x1b[s\x1b[6n"); // Save Cursor Position   Then get position.

    long starttime = 0, nowtime = 0;
    double timediff = 0;
    starttime = GetCurrentDTSec();

    // Main Loop for getting ESC Cursor Position Sequence from Telnet Client
LOOP:

    nowtime = GetCurrentDTSec();
    timediff=difftime(nowtime,starttime);

    // Greater then 5 Seconds
    if (timediff > 5) {
        putline ((char *)"\x1b[u"); // Restore
        return FALSE;
    }


    // If we have left over data, say from a copy / paste, roll through
    // and return each char seperately.
    if (e < (signed)strlen(buffer) &&
        buffer[e] != '\0'  &&
        e > 0) {
        ++e;
        ch = (buffer[e-1]);
        goto CONT;
    } else e = 0;

    memset(buffer,0,sizeof(buffer));
    do {
        usleep(1);
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0; // .5 seconds delay / Save CPU Usage
        if (select(STDIN_FILENO+1, &fds, 0, 0, &tv)) {
            if ((ret = read(STDIN_FILENO, buffer, sizeof(buffer)-1)) > 0)
                ch = buffer[0];
            break;
        }
        // Can setup timeout here later on for inactivity.. :)
        // after 30 seconds break and return false;
        nowtime = GetCurrentDTSec();
        timediff=difftime(nowtime,starttime);

        // Greater then 5 Seconds
        if (timediff > 5) {
            putline ((char *)"\x1b[u"); // Restore
            return FALSE;
        }
    } while (1);

    // If ESC hit, then Process Escape Sequence and chop up
    // Else return key, and check buffer for extract data like Copy/Paste
    e = 1;

CONT:

    while (1) {
        // Passthrough...
        if(ch == 27 || ch == '^' || ch == '[') {}

        // Get x[##;xx
        if(isdigit(ch)) {
            xy[i] = ch;
            ++i;
        }
        //now get x[xx;##
        if(ch==';') {
            i = 0;
            ansi_x = atoi(xy);
        }
        //now get end of sequence.
        if (toupper(ch) == 'R') {
            ansi_y = atoi(xy);
            break;
        }
        goto LOOP;
    }

    putline ((char *)"\x1b[u");
    return TRUE;
}

/**
 * Get Current Screen X Position
 */
int ConsoleIO::ansi_getx()
{
    return ansi_x;
}

/**
 * Get Current Screen X Position
 */
int ConsoleIO::ansi_gety()
{
    return ansi_y;
}
