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

# include "pyenthral.h"
# include "struct.h"
# include "menu_func.h"
# include "main_system.h"
# include "language.h"

# include <cstdio>
# include <cstdarg>
# include <cstring>
# include <cstdlib>
# include <ctime>
# include <unistd.h> // gcc 4.7

# include <iostream>
# include <string>


using namespace std;

char OSSYSTEM[1024]   = {0};
// Globals for MCI Codes..
// Should pass this but i hate passing everything!
int  CURRENT_MAREA    = 0;
int  CURRENT_FAREA    = 0;
int  CURRENT_MSGTITLE = 0;
int  CURRENT_ULIST    = 0;

int  UTF8Output = FALSE; // Add detection for UTF8

/**
 * String Control Chars from String
 */
void strip(char *ostr)
{
    int id1;
    std::string tstr = ostr;
    while ((id1=tstr.find("\r",0)) != -1) tstr.erase(id1,1);
    while ((id1=tstr.find("\n",0)) != -1) tstr.erase(id1,1);
    while ((id1=tstr.find("\b",0)) != -1) tstr.erase(id1,1);
    while ((id1=tstr.find("\t",0)) != -1) tstr.erase(id1,1);
    strcpy(ostr,(char *)tstr.c_str());
}

/**
 * Main System Startup Sequence before Menu System Loop
 * Pre-Login Sequence
 */
void main_system::start(UserRec *user)
{
    // Startup Friend Classes
    language    _lang;
    //users       _usr;
	UserRec     tmp;

	
	// Setup Default CP437 Encoding.  Add Detection from TelnetD here also!
	write(1, "\x1b" , 1); // Back to ISO Char Set
	write(1, "%@" , 2);   

	write(1, "\x1b" , 1); // Enable CP437
	write(1, "(U" , 2);
	
	UTF8Output = FALSE;
	
    // Start Control Sequence Resetting... Will be ignored by Non Ansi Systems.
    pipe2ansi((char *)"|CS");


    fflush(stdout);
    open_keyboard();

    int c    = 0;
    int done = FALSE;

    char sCmd[2048]  = {0};
    char text[1024]  = {0};
    char text2[1024] = {0};
    char text3[1024] = {0};

    FILE *uname;
    char os[255]     = {0};
    int lastchar     = 0;


	
	// Need to Add variable / Config item for pre-startup script.
	// Make Startup 1 py. Then Startup 2 py after detection.
	if (strlen(STARTUP_SCRIPT) > 0)
		pybbs_run(STARTUP_SCRIPT, &tmp);
	

	// Set Char Set in Console default CP437.	
	ansiPrintf((char *)"encode");
	ansiPrintf((char *)"en_bar1");

	// Start encoding loop.
	int encode_input = 0;
	int encode_done = FALSE;
	

	int rightLeft = TRUE;
	while(encode_done == FALSE)
	{
		encode_input = getkey(true);

		switch (toupper(encode_input))
		{

			case 'C':
				std::setlocale(LC_ALL, "en_US.utf8");
				write(1, "\x1b" , 1); // Back to ISO Char Set
				write(1, "%@" , 2);   
				cout << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << flush;
				
				write(1, "\x1b" , 1); // Enable CP437
				write(1, "(U" , 2);
				cout << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << flush;
				UTF8Output = FALSE;
				rightLeft = TRUE;
				ansiPrintf((char *)"encode");
				break;

			case 'U':						
				write(1, "\x1b" , 1); // UTF-8
				write(1, "%G" , 2);   
				cout << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << flush;
				UTF8Output = TRUE;
				rightLeft = FALSE;
				ansiPrintf((char *)"encode");
				break;

			case 10: // ENTER Done.
				encode_done = TRUE;
				if (rightLeft)
				{
					write(1, "\x1b" , 1); // Back to ISO Char Set
					write(1, "%@" , 2);
					cout << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << flush;
					
					write(1, "\x1b" , 1); // Enable CP437
					write(1, "(U" , 2);
					cout << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << flush;
					UTF8Output = FALSE;
				}
				else
				{
					write(1, "\x1b" , 1); // UTF-8
					write(1, "%G" , 2);
					cout << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << flush;
					UTF8Output = TRUE;
					rightLeft = FALSE;
					UTF8Output = TRUE;
				}
				break;

			case 27: // Get Arrow Key
				if (EscapeKey[0] == '[')
				{
					if (EscapeKey[1] == 'D' && rightLeft == FALSE)
					{
						write(1, "\x1b" , 1); // Back to ISO Char Set
						write(1, "%@" , 2);
						cout << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << flush;
		
						write(1, "\x1b" , 1); // Enable CP437
						write(1, "(U" , 2);
						cout << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << flush;
					
						UTF8Output = FALSE;
						ansiPrintf((char *)"encode");					
						rightLeft = TRUE;
						break;
					
					}
					if (EscapeKey[1] == 'C' && rightLeft == TRUE)
					{
						std::setlocale(LC_ALL, "en_US.utf8");
						write(1, "\x1b" , 1); // UTF-8
						write(1, "%G" , 2);
						cout << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << flush;
					
						UTF8Output = TRUE;
						ansiPrintf((char *)"encode");					
						rightLeft = FALSE;
						break;
					}
				}

				continue;

			default:  continue;
		}

		// Set back to CP437 For Light Bar Selections, 
		// This displays in both CP437 and UTF-8, However
		// If utf8 is not support, then this would be a mess
		// So we want to set this part so the user can see it.

		if (!encode_done)
		{
			write(1, "\x1b" , 1); // Back to ISO Char Set
			write(1, "%@" , 2);
			cout << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << flush;

			write(1, "\x1b" , 1); // Enable CP437
			write(1, "(U" , 2);
			cout << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << "\x1b[D \x1b[D" << flush;

	 
			if (rightLeft)
			{
				UTF8Output = FALSE;
				ansiPrintf((char *)"en_bar1");
			}
			else
			{
				UTF8Output = FALSE;
				ansiPrintf((char *)"en_bar2");
			}
		}
	}

	pipe2ansi((char *)"|CS");

	if (strlen(STARTUP_SCRIPT2) > 0)
		pybbs_run(STARTUP_SCRIPT2, &tmp);

	pipe2ansi((char *)"|CS");
	
	fflush(stdout);
    open_keyboard();
			
    // Thanks to Frank for this quickie! :)
    uname = popen("uname -sm", "r");

    lastchar = fread(os, 1, 80, uname);
    os[lastchar] = '\0';
    pclose(uname);

    strip(os);
    sprintf(OSSYSTEM,"(%s)",os);

    sprintf(sCmd,"%s %s Telnet Node %i",BBSVERSION, OSSYSTEM, NODE_NUM);
    cspacing(sCmd);

    // have to put newline in after center spacing, or it throws it all off.
    strcat(sCmd,(char *)"\r\n");
    putline(sCmd);

	// Add a config setting to display or omit this from displaying
	// Some people don't like to give system info!
/*
	sprintf(sCmd,"GCC: %s", __VERSION__);
    cspacing(sCmd);

    // have to put newline in after center spacing, or it throws it all off.
    strcat(sCmd,(char *)"\r\n");
    putline(sCmd);*/

    sprintf(sCmd,"Copyright (c) Michael Griffin 2004-2014, All Rights Reserved");
    cspacing(sCmd);
    strcat(sCmd,(char *)"\r\n");
    pipe2ansi(sCmd);

    // Holds for Sysop Name / Email String.
    _lang.lang_get(text,56);
    cspacing(text);
    strcat(text,(char *)"\r\n");
    pipe2ansi(text);

    sleep (1);

    putline((char *)"\r\n");
    _lang.lang_get(text,1);
    //cspacing(text);
    strcat(text,(char *)"\r\n");
    pipe2ansi(text);

    sleep (1);

    // Ansi detection screen
    ansiPrintf((char *)"ansi");

    // Detect if were compitable.
    if (!getxy())
    {
        _lang.lang_get(text,51);
        //cspacing(text);
        //sprintf(text,"%s\r\n\r\n",sCmd);

        strcat(text,(char *)"\r\n\r\n");
        putline(text);

        _lang.lang_get(text2,52);
        //sprintf(text2," .\b\b", text2);
        //strcat(text2," .\b\b");
        pipe2ansi(text2);
        do
        {
            c = getkey(true);
            switch(toupper(c))
            {
            case 10:
                //putkey('Y');
                isANSI = FALSE;
                done = TRUE;
                break;

            case 'Y':
                //putkey('Y');
                isANSI = FALSE;
                done = TRUE;
                break;

            case 'N':
                //putkey('N');
                isANSI = TRUE;
                done = TRUE;
                break;

            default:
                break;
            }
        }
        while(done == FALSE);

    }
    else
    {
        _lang.lang_get(text,2);
        //cspacing(text);
        sprintf(text,"%s",text);
        pipe2ansi(text);

        _lang.lang_get(text2,53);
        //sprintf(text2,"%s .\b\b",text2);
        pipe2ansi(text2);

        done = FALSE;
        do
        {
            c = getkey(true);
            switch(toupper(c))
            {
            case 10:
                //putkey('Y');
                isANSI = TRUE;
                done = TRUE;            //putkey(ch);
                break;

            case 'Y':
                //putkey('Y');
                isANSI = TRUE;
                done = TRUE;
                break;

            case 'N':
                //putkey('N');
                isANSI = FALSE;
                done = TRUE;
                break;

            default:
                break;
            }
        }
        while (done == FALSE);
    }

    if (isANSI)
        _lang.lang_get(text3,57);
    else
        _lang.lang_get(text3,58);

    sprintf(sCmd,"%s",text3);
    pipe2ansi(sCmd);

    sleep(2);

    if (isANSI == FALSE)
    {
        putline((char *)"\r\n\r\n|07|16Your terminal doesn't support an ESC Sequence query for the\r\n");
        putline((char *)"current (x/y) cursor position when the bbs asks. Due to this issue, some\r\n");
        putline((char *)"graphics or interfaces might appear incorrect. Telnet programs I reccomend\r\n");
        putline((char *)"are Mtelnet for Windows, and SyncTerm for Linux, BSD, OSX. for Full Support.\r\n\r\n");
        startpause();

		isANSI = TRUE;
    }

    // If were doing unit Testing, load unit test menu, where we can jump directly to interfaces.
    // If menu exists, then we run it!

	menu_func mnf;

	/*
	mnf._premenu.clear();
    mnf._gosub.clear();
    mnf._curmenu = "unittest";
    mnf._loadnew = true;

    // Start unit test menu, skip matrix and login.
    // Ask for System Password lateron for Sysop Quick Login!!

    if ( mnf.menu_exists() )  // Testing purposes only, maybe passworded features lateron
    {
//		errlog((char *)"Starting UnitTest Menu!");
        // Set default users to 0, sysop
        _usr.users_read(user,0);
//		errlog((char *)"Forced Login to: %s",user->handle);

        // When user logs in, always reset last area and msg to 0.
        user->lastmbarea = 0;
        user->lastmsg = 0;
        CURRENT_MAREA = 0;
        _usr.users_write(user,0);

        node _node;
        _node.node_write(user,NODE_NUM);
        UserLoggedIn = TRUE;
        isSysop = TRUE;

        mnf.menu_mainloop(user);
        return;
    }*/

	/*
//	errlog((char *)"Starting Matrix Menu");
    _mnf._premenu.clear();
    _mnf._gosub.clear();
    _mnf._curmenu = "matrix";
    _mnf._loadnew = true;

    // Display Welcome Screen
    ansiPrintf((char *)"welcome");
    _mnf.menu_mainloop(user);*/

	// Load Menu System sepeartly for init pre-system script.
	

	mnf._premenu.clear();
    mnf._gosub.clear();
    mnf._curmenu = "matrix";
    mnf._loadnew = true;

    // Display Welcome Screen
    ansiPrintf((char *)"welcome");
    mnf.menu_mainloop(user);	
	//delete pre_mnf;
}

