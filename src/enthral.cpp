/***************************************************************************
 *   Copyright (C) 2004-2017 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose:                                                              *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

# ifdef HAVE_CONFIG_H
# include <config.h>
# endif

# include "configbbs.h"
# include "language.h"
# include "main_system.h"
# include "conio.h"
# include "users.h"
# include "node.h"
# include "dtfunc.h"
# include "console.h"

# include <cstdio>
# include <csignal>
# include <cerrno>
# include <cstring>
# include <cstdlib>

# include <limits.h>
# include <unistd.h>
# include <pty.h>
# include <termios.h>
# include <sys/stat.h>

using namespace std;

char BBSPATH[PATH_MAX]     = {0};
char DATAPATH[PATH_MAX]    = {0};
char MENUPATH[PATH_MAX]    = {0};
char ANSIPATH[PATH_MAX]    = {0};
char MESGPATH[PATH_MAX]    = {0};
char FILEPATH[PATH_MAX]    = {0};
char LOCKPATH[PATH_MAX]    = {0};
char INIPATH[PATH_MAX]     = {0};
char DAPATH[PATH_MAX]      = {0};
char NODEPATH[PATH_MAX]    = {0};
char NODEDIR[PATH_MAX]     = {0};
char USRSIG[PATH_MAX]      = {0};
char SCRIPTS[PATH_MAX]     = {0};
char ENTHRALTMP[PATH_MAX]  = {0};

char UsersIP[1024]    = {0};
char UsersHOST[1024]  = {0};
char SYSOP_NAME[80]   = {0};
char SYSTEM_NAME[80]  = {0};
char CLIENT_TERM[255] = {0};

// Startup Python Scripts
char STARTUP_SCRIPT[255]  = {0};
char STARTUP_SCRIPT2[255] = {0};

int  NODE_NUM         = 0;
int  UserLoggedIn     = FALSE;
int  TERM_HEIGHT      = 25;
int  TERM_WIDTH       = 80;
int  MAX_NODES        = 5;

// Ansi Terminal Detected.
int isANSI = FALSE;

static UserRec user;

struct termios old_termios;

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void killazombie (int iSig)
{
    node    _node;
    users   _usr;
    ConsoleIO _io;

    _node.node_remove_dropfiles(NODE_NUM);
    _node.node_remove(NODE_NUM);

    if (strcmp((char *)user.handle,"") != 0 && UserLoggedIn == TRUE) {
        user.dtlaston = GetCurrentDTSec();
        _usr.users_write(&user,user.idx);
    }

    UserLoggedIn = FALSE;

    _io.errlog((char *)"killazombie System Shutdown (User Hung Up on the System!).");

    switch(iSig) {

    case SIGHUP:
        _io.errlog((char *)"killazombie SIGHUP).");
        break;
    case SIGTERM:
        _io.errlog((char *)"killazombie SIGTERM).");
        break;
    case SIGINT:
        _io.errlog((char *)"killazombie SIGINT).");
        break;
    case SIGILL:
        _io.errlog((char *)"killazombie SIGILL).");
        break;

    case SIGABRT:
        _io.errlog((char *)"killazombie SIGABRT).");
        break;
    case SIGQUIT:
        _io.errlog((char *)"killazombie SIGQUIT).");
        break;
    case SIGKILL:
        _io.errlog((char *)"killazombie SIGKILL).");
        break;

        // Masked Signals
    case SIGCHLD:
        _io.errlog((char *)"killazombie SIGCHLD).");
        break;
    case SIGTTOU:
        _io.errlog((char *)"killazombie SIGTTOU).");
        break;

    default:
        _io.errlog((char *)"Unknown Signal).");
        break;

    }

    // Test Setting the Terminal back to Original.
    tcflush( STDOUT_FILENO, TCIFLUSH);
    tcsetattr( STDOUT_FILENO, TCSANOW, &old_termios );

    clear_nodes();

    exit(iSig);
}

void exit_system(void)
{
    ConsoleIO _io;
    node    _node;

    _io.errlog((char *)"At Exit!).");

    _node.node_remove_dropfiles(NODE_NUM);
    _node.node_remove(NODE_NUM);
    clear_nodes();

    exit(0);
}

// Windows Resizing detected from Client Telnetd/Term.
void sigwinch(int sig)
{
    struct winsize wsz;

    (void) signal(SIGWINCH, sigwinch);
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &wsz)>=0 && wsz.ws_row>0 && wsz.ws_col>0) {
        TERM_HEIGHT=wsz.ws_row;
        TERM_WIDTH=wsz.ws_col;
    }
}

int main(int argc, char *argv[])
{
    (void) signal (SIGHUP,  killazombie);
    (void) signal (SIGTERM, killazombie);
    (void) signal (SIGINT,  killazombie);
    (void) signal (SIGILL,  killazombie);
    (void) signal (SIGKILL, killazombie);
    (void) signal (SIGABRT, killazombie);
    (void) signal (SIGQUIT, killazombie);

    // Detect Screen Size Changes.
    (void) signal (SIGWINCH, sigwinch);

    // Masked Signals.
    (void) signal (SIGTTOU, SIG_IGN);
    (void) signal (SIGCHLD, SIG_IGN);

    ConsoleIO _io;
    users   _usr;

    // Obtain a copy of the termios structure for stdout.
    if( tcgetattr( STDOUT_FILENO, &old_termios ) )
        return( 1 );

    // Setup Global Path in Structs.h
    char sCmd[255]= {0};
    char parg[255]= {0};

    strcpy(parg,argv[0]);

    int num = 0;
    int i   = 0;

    memset(&BBSPATH,0,sizeof(BBSPATH));
    // Get FULL PATH TO EXE, and Chop off Filename for PATH
    for (int i = 0; ; i++) {
        if (parg[i] == '\0') break;
        if (parg[i] == '/') num = i;
    }
    if (num != 0) {
        for (int i = 0; i < num+1; i++) {
            BBSPATH[i] = parg[i];
        }
    }

    // get environment varliable, if exists override current folder.
    char *pPath;
    pPath = std::getenv((char *)"ENTHRAL");
    if (pPath!=NULL) {
        memset(&BBSPATH,0L,sizeof(BBSPATH));
        strcpy(BBSPATH,pPath);
    }

    if (argc >= 6) {
        sprintf(CLIENT_TERM,"%s",argv[5]);
    }

    // Set windows size from readin variables of environment.
    struct winsize ws;

    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws)>=0 && ws.ws_row>0 && ws.ws_col>0) {
        TERM_HEIGHT = ws.ws_row;
        TERM_WIDTH  = ws.ws_col;
    }

    // set the path if it doesn't exists, or to what is detected
    // So apps, scripts and custom programs can use this variable
    // for bbs system files.
    setenv((char*)"ENTHRAL", BBSPATH, TRUE);

    // Change to BBS path as current working folder.
    int iRet = 0;
    if ((iRet = chdir(BBSPATH)) == 0) {}

    sprintf(DATAPATH,"%sdata/",BBSPATH);   // Data Path
    sprintf(MENUPATH,"%smenu/",BBSPATH);   // Menu Files
    sprintf(ANSIPATH,"%sansi/",BBSPATH);   // Ansi Files
    sprintf(MESGPATH,"%smsgs/",BBSPATH);   // Message Files
    sprintf(INIPATH,"%sini/",BBSPATH);     // INI Files
    sprintf(DAPATH,"%sdareas/",BBSPATH);   // Data Area Data
    sprintf(FILEPATH,"%sfiles/",BBSPATH);  // Files Area Data
    sprintf(USRSIG,"%susersig/",BBSPATH);  // Files Area Data

    // Direcotries that don't always have data, have checks for creating them!
    sprintf(NODEPATH,"%snode/",BBSPATH);   // Nodes
    sprintf(LOCKPATH,"%slock/",BBSPATH);   // r/w Lock Files
    sprintf(SCRIPTS,"%sscripts/",BBSPATH); // scripts path.  ie python
    sprintf(ENTHRALTMP,"/tmp/enthral");    // Tmp, Sockets, IPC

    // Do Error Checking if CONFIG.CFG File exists, if not creates it
    if(configdataexists() == false)
        createconfig();

    // Open and Read Config file
    parseconfig();

    // Get Users incomming IP Address
    // Unless being kicked off locally.
    if (argc >= 3) {
        sprintf(UsersIP,"%s",argv[1]);
        sprintf(UsersHOST,"%s",argv[2]);
    } else {
        sprintf(UsersIP,"Localhost");
        sprintf(UsersHOST,"Localhost");
    }

    // Figure out Node Number
    node _node;
    for (i = 1; ; i++) {
        if (_node.node_exists(i) == FALSE) {
            NODE_NUM = i;
            break;
        }
    }

    _io.errlog((char *)" *** MAX_NODES: %i ",MAX_NODES);

    // Check if Node umber great then Max Node Number
    // Add this to config file.
    if (NODE_NUM <= MAX_NODES) {
        _io.errlog((char *)"TERM FILE: %s ",CLIENT_TERM);

        // Go through and create basic directory structure if doesn't exist.
        _io.errlog((char *)"User Logged in IP: %s / %s - Node: %i ",UsersIP, UsersHOST, NODE_NUM);

        // Create Data Directory
        sprintf(sCmd,"%s",DATAPATH);
        if (mkdir(sCmd, 0770) == -1 && errno != EEXIST) {
        }

        // Create Node Directory
        sprintf(sCmd,"%s",MESGPATH);
        if (mkdir(sCmd, 0770) == -1 && errno != EEXIST) {
        }

        // Create Node Directory
        sprintf(sCmd,"%s",FILEPATH);
        if (mkdir(sCmd, 0770) == -1 && errno != EEXIST) {
        }

        // Create Node Directory
        sprintf(sCmd,"%s",NODEPATH);
        if (mkdir(sCmd, 0770) == -1 && errno != EEXIST) {
        }

        // Create Node Subdirectory NodeNum Directory!
        sprintf(NODEDIR,"node%i",NODE_NUM);
        sprintf(sCmd,"%s%s",NODEPATH,NODEDIR);
        if (mkdir(sCmd, 0770) == -1 && errno != EEXIST) {
        }

        // Create Lockfile directory.
        sprintf(sCmd,"%slock",BBSPATH);
        if (mkdir(sCmd, 0770) == -1 && errno != EEXIST) {
        }

        // Create UserSig Directory
        sprintf(sCmd,"%susersig",BBSPATH);
        if (mkdir(sCmd, 0770) == -1 && errno != EEXIST) {
        }

        // Create UserSig Directory
        sprintf(sCmd,"%sscripts",BBSPATH);
        if (mkdir(sCmd, 0770) == -1 && errno != EEXIST) {
        }

        // Create TMP Directory!
        if (mkdir(ENTHRALTMP, 0770) == -1 && errno != EEXIST) {
        }

        // Compile Language File on Startup {Data dir has to be present first!}
        language *lang = new language;
        if (!lang) {
        }

        isANSI = TRUE;
        lang->lang_compile();
        delete lang;

        // Set Node File with Blank Data Untill User Has Logged in.
        memset(&user,0,sizeof(UserRec));
        // Setup Default Node User as logging in...
        sprintf((char *)user.handle,"User Logging In");
        _node.node_write(&user,NODE_NUM);

        // Test Setting the Terminal back to Original.
        // If unable to, then error.
        tcflush( STDOUT_FILENO, TCIFLUSH);
        if ( tcsetattr( STDOUT_FILENO, TCSADRAIN ,&old_termios ) )
            return( 2 );

        // Procedures to run at shutdown.
        atexit(exit_system);

        // New internode/process communication WIP!
        init_nodes();

        main_system ms;
        ms.start(&user);

    } else {
        _io.errlog((char *)"User Logged in IP: %s / %s - Node: %i ",UsersIP, UsersHOST, NODE_NUM);
        _io.errlog((char *)"Max Number of Nodes Reached, Please Try again sortly. ");
        _io.pipe2ansi ((char *)"|CR|CR|15Max Number of Nodes Reached, Please Try again sortly. |DE |DE");
    }

    // Shutdown Sequence.
    if (strcmp((char *)user.handle,"") != 0 && UserLoggedIn == TRUE) {
        user.dtlaston = GetCurrentDTSec();
         // Save Stats
        _usr.users_write(&user,user.idx);
    }

    // Test Setting the Terminal back to Original.
    tcflush( STDOUT_FILENO, TCIFLUSH);
    if (tcsetattr( STDOUT_FILENO, TCSANOW,&old_termios ) )
        return ( 2 );

    write(0,(char *)"\x1b",1);
    write(0,(char *)"c",1);

    clear_nodes();

    _node.node_remove_dropfiles(NODE_NUM);
    _node.node_remove(NODE_NUM);

    exit( 0 );
}
