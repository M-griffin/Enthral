/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
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

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$


# ifdef HAVE_CONFIG_H
# include <config.h>
# endif

# include "configbbs.h"
# include "language.h"
# include "main_system.h"
//# include "menus.h"
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
# include <pty.h>     // Struct Winsize
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


// Testing and debugging only!
/*
void print_trace()
{

    char pid_buf[30]={0};
    sprintf(pid_buf, "%d", getpid());
    char name_buf[512]={0};
    //name_buf[readlink("/proc/self/exe", name_buf, 511)]=0;
	name_buf[readlink("/home/merc/Downloads/enthral/source/src/enthral", name_buf, 511)]=0;
    int child_pid = fork();
    if (!child_pid)
    {
        dup2(2,1); // redirect output to stderr
        fprintf(stdout,"stack trace for %s pid=%s\n",name_buf,pid_buf);
        execlp("gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt", name_buf, pid_buf, NULL);
        abort(); // If gdb failed to start
    }
    else
    {
        waitpid(child_pid,NULL,0);
    }
}
*/

// umask(S_IRWXG | SIRWXO);
//Set the umask such that any files created won't allow the
//group or the world to read, write, or execute.

void killazombie (int iSig)
{

	
	// User Hung up or Disconnected, Shutdown properly and End Program..
    // Add System Shutdown here.

    // Node Node is Setup, Add Process to Read User Data From Node Filename
    // Log User, Then Remove File to make sure node is clean.
    node    _node;
    users   _usr;
    SESSION _io;

	// Send Logoff Node notification.
	//_io.lineolm_login(FALSE);

    _node.node_remove_dropfiles(NODE_NUM);
    _node.node_remove(NODE_NUM);

    if (strcmp((char *)user.handle,"") != 0 && UserLoggedIn == TRUE)
    {
        user.dtlaston = GetCurrentDTSec();
        _usr.users_write(&user,user.idx);
    }

	UserLoggedIn = FALSE;

	_io.errlog2((char *)"killazombie System Shutdown (User Hung Up on the System!).");

	switch(iSig)
	{

		case SIGHUP:
			_io.errlog2((char *)"killazombie SIGHUP).");
			break;
		case SIGTERM:
			_io.errlog2((char *)"killazombie SIGTERM).");
			break;
		case SIGINT:
			_io.errlog2((char *)"killazombie SIGINT).");
			break;
		case SIGILL:
			_io.errlog2((char *)"killazombie SIGILL).");
			break;

		case SIGABRT:
			_io.errlog2((char *)"killazombie SIGABRT).");
			break;					
		case SIGQUIT:
			_io.errlog2((char *)"killazombie SIGQUIT).");
			break;
		case SIGKILL:
			_io.errlog2((char *)"killazombie SIGKILL).");
			break;

		// Masked Signals
		case SIGCHLD:
			_io.errlog2((char *)"killazombie SIGCHLD).");
			break;
		case SIGTTOU:
			_io.errlog2((char *)"killazombie SIGTTOU).");
			break;
			
		default:
			_io.errlog2((char *)"Unknown Signal).");
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
    SESSION _io;
    node    _node;
//    users   _usr;

	//_io.lineolm_login(FALSE);

	// Clear Node Data.

	// Send Logoff Node notification.
	// his is done when menu system exits, no need for this here!
	//_io.lineolm_login(FALSE);
	
    //UserLoggedIn = FALSE;
	_io.errlog2((char *)"At Exit!).");

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
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &wsz)>=0 && wsz.ws_row>0 && wsz.ws_col>0)
    {
        TERM_HEIGHT=wsz.ws_row;
        TERM_WIDTH=wsz.ws_col;
	}
}

int main(int argc, char *argv[])
{
    // Setup Signal Checking if Telnetd was killed.
    // Then kill this program, Add Save Current Users Settings...
    // NOTE This only killed if program kicked off directly from telnetd,
    // if you run a bash script, that becomes a zombie process becasue it creates a second pty!
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
	
	
    SESSION _io;
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
    for (int i = 0; ; i++)
    {
        if (parg[i] == '\0') break;
        if (parg[i] == '/') num = i;
    }
    if (num != 0)
    {
        for (int i = 0; i < num+1; i++)
        {
            BBSPATH[i] = parg[i];
        }
    }

    // get environment varliable, if exists override current folder.
    char *pPath;
    pPath = std::getenv((char *)"ENTHRAL");
    //printf((char *)" *** Environment 1: pPath: %s\r\n",pPath);
    if (pPath!=NULL)
    {
        memset(&BBSPATH,0L,sizeof(BBSPATH));
        strcpy(BBSPATH,pPath);
    }

	if (argc >= 6)
	{
		// Passed from Telnetd
        sprintf(CLIENT_TERM,"%s",argv[5]);
    }
	
	// Set windows size from readin variables of environment.
	struct winsize ws;

	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws)>=0 && ws.ws_row>0 && ws.ws_col>0)
    {
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

    sprintf(DATAPATH,"%sdata/",BBSPATH);  // Data Path
    sprintf(MENUPATH,"%smenu/",BBSPATH);  // Menu Files
    sprintf(ANSIPATH,"%sansi/",BBSPATH);  // Ansi Files
    sprintf(MESGPATH,"%smsgs/",BBSPATH);  // Message Files
    sprintf(INIPATH,"%sini/",BBSPATH);    // INI Files
    sprintf(DAPATH,"%sdareas/",BBSPATH);  // Data Area Data
    sprintf(FILEPATH,"%sfiles/",BBSPATH);  // Files Area Data
    sprintf(USRSIG,"%susersig/",BBSPATH);  // Files Area Data

    // Direcotries that don't always have data, have checks for creating them!
    sprintf(NODEPATH,"%snode/",BBSPATH);   // Nodes
    sprintf(LOCKPATH,"%slock/",BBSPATH);  // r/w Lock Files
    sprintf(SCRIPTS,"%sscripts/",BBSPATH);  // scripts path.  ie python
    sprintf(ENTHRALTMP,"/tmp/enthral");   // Tmp, Sockets, IPC

    // Do Error Checking if CONFIG.CFG File exists, if not creates it
    if(configdataexists() == false)
        createconfig();

    // Open and Read Config file
    parseconfig();


    // Get Users incomming IP Address
    // Unless being kicked off locally.
    if (argc >= 3)
    {
        sprintf(UsersIP,"%s",argv[1]);
        sprintf(UsersHOST,"%s",argv[2]);
    }
    else
    {
        sprintf(UsersIP,"Localhost");
        sprintf(UsersHOST,"Localhost");
    }

    // Figure out Node Number
    node _node;
    for (i = 1; ; i++)
    {
        if (_node.node_exists(i) == FALSE)
        {
            NODE_NUM = i;
            break;
        }
    }

	_io.errlog2((char *)" *** MAX_NODES: %i ",MAX_NODES);

	// Check if Node umber great then Max Node Number
	// Add this to config file.
	if (NODE_NUM <= MAX_NODES)
	{

	//    _io.errlog((char *)" *** BBS Loaded! Root Path: %s",BBSPATH);
	
		_io.errlog2((char *)"TERM FILE: %s ",CLIENT_TERM);
	
		// Go through and create basic directory structure if doesn't exist.
		_io.errlog2((char *)"User Logged in IP: %s / %s - Node: %i ",UsersIP, UsersHOST, NODE_NUM);

		// Create Data Directory
		sprintf(sCmd,"%s",DATAPATH);
		if (mkdir(sCmd, 0770) == -1 && errno != EEXIST)
		{
		    // Go through and create basic directory structure if doesn't exist.
	//        _io.errlog((char *)"Err: Unable to create data folder.");
		}

		// Create Node Directory
		sprintf(sCmd,"%s",MESGPATH);
		if (mkdir(sCmd, 0770) == -1 && errno != EEXIST)
		{
	//        _io.errlog((char *)"Err: Unable to create msgs folder.");
		}

		// Create Node Directory
		sprintf(sCmd,"%s",FILEPATH);
		if (mkdir(sCmd, 0770) == -1 && errno != EEXIST)
		{
	//        _io.errlog((char *)"Err: Unable to create files folder.");
		}

		// Create Node Directory
		sprintf(sCmd,"%s",NODEPATH);
		if (mkdir(sCmd, 0770) == -1 && errno != EEXIST)
		{
	//        _io.errlog((char *)"Err: Unable to create node folder.");
		}

		// Create Node Subdirectory NodeNum Directory!
		sprintf(NODEDIR,"node%i",NODE_NUM);
		sprintf(sCmd,"%s%s",NODEPATH,NODEDIR);
		if (mkdir(sCmd, 0770) == -1 && errno != EEXIST)
		{
	//        _io.errlog((char *)"Err: Unable to create NODE_NUM folder.");
		}

		// Create Lockfile directory.
		sprintf(sCmd,"%slock",BBSPATH);
		if (mkdir(sCmd, 0770) == -1 && errno != EEXIST)
		{
	//        _io.errlog((char *)"Err: Unable to create lock folder.");
		}

		// Create UserSig Directory
		sprintf(sCmd,"%susersig",BBSPATH);
		if (mkdir(sCmd, 0770) == -1 && errno != EEXIST)
		{
	//        _io.errlog((char *)"Err: Unable to create usersig folder.");
		}

		// Create UserSig Directory
		sprintf(sCmd,"%sscripts",BBSPATH);
		if (mkdir(sCmd, 0770) == -1 && errno != EEXIST)
		{
	//        _io.errlog((char *)"Err: Unable to create scripts folder.");
		}

		// Create TMP Directory!
		if (mkdir(ENTHRALTMP, 0770) == -1 && errno != EEXIST)
		{
	//        _io.errlog((char *)"Err: Unable to create /tmp/enthral folder.");
		}


		// Compile Language File on Startup {Data dir has to be present first!}
		language *lang = new language;
		if (!lang)
		{
	//        _io.errlog((char *)"Err: Unable to Allocate Memory for Language.txt Compile.");
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
		atexit(exit_system);  // Removed Node Files, and Saves Use On Proper Shutdown
	
		// New internode/process communication WIP!
		init_nodes();

		


		/*-------------------------------------------------------------------------
		    Main System Loop,
		 -------------------------------------------------------------------------*/

		main_system ms;
		ms.start(&user); // Main Loop.

	}
	else
	{
		// Go through and create basic directory structure if doesn't exist.
		_io.errlog2((char *)"User Logged in IP: %s / %s - Node: %i ",UsersIP, UsersHOST, NODE_NUM);
		_io.errlog2((char *)"Max Number of Nodes Reached, Please Try again sortly. ");
		_io.pipe2ansi ((char *)"|CR|CR|15Max Number of Nodes Reached, Please Try again sortly. |DE |DE");
	}
	/*-------------------------------------------------------------------------
        Logoff Sequence
     -------------------------------------------------------------------------*/
	
	if (strcmp((char *)user.handle,"") != 0 && UserLoggedIn == TRUE)
    {
        user.dtlaston = GetCurrentDTSec();
        _usr.users_write(&user,user.idx); // Save Stats
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

//    _io.errlog((char *)"System Shutdown Cleanly.");
    exit( 0 );
}

