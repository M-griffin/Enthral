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

// Enthral SVN: $Id: conio.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/conio.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $


# ifndef CONIO_H
# define CONIO_H

# include "struct.h"
# include "history.h"

# include <string>
# include <stdarg.h>

using namespace std;


//input class
class SESSION : public history
{

public:

    History  hist;  // Struct
    UserRec  *user; // For MCI Codes

    char EscapeKey[10];
    SESSION();
    SESSION(UserRec *thisuser);

    PASSING   *p;  // For passing Menu command to new thread
    pthread_t ahThread;

private:
    char textline[100][81];
    char peekch;
    int  Col;
    int  MAX_WIDTH;
    std::string input;

    int  ansi_x;
    int  ansi_y;


public:


    char onekey(char *chlist);
    void repeatchar(char ch, int length);

    void start_session(UserRec *thisuser);
    void hist_display(); // Testing Func.
    void hist_update(int i, UserRec *user, int cnt = 0);
    void errlog(char *fmt, ...);
    void errlog2(char *fmt, ...); // Used for Debugging, commented out for easy removal.

    void open_keyboard();
    void close_keyboard();

    int  term_option(int option);
    void inputfield(char *text, int &len);
    void rspacing(char *str, int space);
    void lspacing(char *str, int space);
    void cspacing(char *str);
    void mask(char *str);
    void putline(char *str); //, ...);

    void putkey(char ch);

    // Working on MultiNode Messages.
    int  send_nmsg(int node, struct nodemessage *nmsg);
    int  processmsg(struct nodemessage *nmsg);

    void node_message(int display = FALSE);
    void lineolm(int node, const char *ms);
    void lineolm_login(int onoff);
    void read_input(); // Multi-node input.

    // Start of Normal input, hits MultiNode too.
    int  getkey(bool bWait);
    void getline(char   *line,
                 int     length,
                 char   *leadoff = 0,
                 int     hid     = FALSE,
                 char   *chlist  = 0);

    int  rt_arrow();
    int  lt_arrow();
    void home_cursor();
    void end_cursor();
    void ansi_fg(char *szEsc, int fg);
    void ansi_bg(char *szEsc, int bg);

    // LocalFunction
    void ParseLocalMCI(std::string &AnsiString, std::string mcicode, std::string outbuff);
    void afilecon(std::string &AnsiString, int buffer);

    void pipe2ansi(char* szString, int buffer=TRUE);
    std::string pipe2string(std::string szString);
    void setTermSpeed(int speed);

    BOOL ansiPrintf(char *filename, int delay=FALSE, int abort=FALSE);
    void readinAnsi(std::string FileName, std::string &buff);
    int  startpause(char *script='\0');

    int getxy();
    int ansi_getx();
    int ansi_gety();
};

# endif
