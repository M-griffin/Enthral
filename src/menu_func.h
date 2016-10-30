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

// Enthral SVN: $Id: menu_func.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/menu_func.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef MENU_FUNC_H
# define MENU_FUNC_H

# include "struct.h"

# include "users.h"
# include "msgs.h"
# include "files.h"
# include "language.h"
# include "conio.h"

class menu_func : private SESSION,
    language,
    users,
    msgs,
    files
{


protected:
    MenuRec    *menur2;
    CommandRec *cmdr2;

    int  noc;           // Number of Commands in Loaded Menu
    int  System_Alive;  // System Exit


public:

    std::string _curmenu;  // Current Menu
    std::string _premenu;  // Previous Menu
    std::string _gosub;    // GoSub Menu.

    //CommandRec cmdr2[MAX_MENU_COMMANDS];

    UserRec *urec;
    unsigned long MsgsLeft;   // Used in Message Reader.
    unsigned long CurMsgs;
    unsigned long TotMsgs;

    // Menu Lightbar Variables
    short xx;//      = 1;        // Holds X Coord
    short yy;//      = 1;        // Holds Y Coord
    short iNoc;//    = 0;        // Holds Lightbar # of choices
    short sNoc;//    = 0;        // Holds Scroll Text # of choices
    short choice;//  = 0;        // Holds Currect Lightbar #

    short  *execnum;
    short  *execnum2;
    short  *execnum3;

    unsigned long cntEscCmds;  // = 0;

    int c, cc;
    bool  EscHit;//  = false; // Is Input key Escaped char, or Normal Key
    char  outBuff[1024];      // Holds Formatted Lightbar Data
    std::string output;       // Buffer for writing all lightbars at the same time
    short sRand;//   = 0;     // Randmise Text Scrollies
    PASSING   *p;//  = 0;     // For passing Menu command to new thread
    pthread_t ahThread;
    bool      tScroll;
    int       executed;       // Test's for hot keys commands excuted, if non pass through loop
    // This is so scrolling isn't killed with invalid menu input!
    int exe;// = 0;

    uint32_t CURRENT_AREA;    // Lightbar Starting Position for Interfaces.
    // This needs access from ie Title Scan Class.

    menu_func();
    menu_func(UserRec *tu);

    ~menu_func();

    //void start_menu_func(UserRec *user);

    // Below here are Menu Processing Functions
    bool _loadnew;

    // New Menu Text File Parsing
    void chkparse(std::string &temp);

    int  mnuparse(std::string cfgdata);
    int  menu_read(std::string MenuName);

    void cmdparse(std::string cfgdata, int idx);
    int  cmdexist(std::string MenuName, int idx);
    int  cnt_cmds(std::string MenuName);
    int  cmds_read(std::string MenuName, int idx);

    // Temp convert from old binary to new text format.
    int  convert_read(std::string menu, CommandRec *cmdr, int idx);
    void menu_convert(std::string menuname);


    // Read in / Load Menu and Commands
    void insert_cmdr();
    void menu_reset();
    int  menu_exists();
    void menu_readin();
    void menu_cmds(char *inPut);

    void menu_clear();

    void display_menu(BOOL forcelevel=FALSE);
    void menu_bars(char *inPut);
    void menu_proc(char *mString, uint32_t area=0);

    // Initalizes the User Logon / Application Process
    int  logon_system(unsigned char c);   // Init Logon Sequence / Application


    void fileedit_system();
    void filearea_change(int change);
    void filearea_list();

    void msgedit_system();                // Init Message Base Editor

    //int  msgread_system(unsigned char c, char *mString=0); // Init Message Reader System
    // For the Email Reader
//    int  emailread_system(unsigned char c, char *mString=0);
    int  msgscan_system(unsigned char c, char *mString=0);

    void msgemail_system();
    void msgpost_system(char *mString='\0');

    // WIP Not finished@@@
    void msgpack_system(int all);
    void msgtrunc_system(int all);
    // WIP Not finished@@@

    void msgarea_change(int change);
    void msgarea_list();

    int  usrlist_list();

    void write_email();
    void email_list();

    // Kick off External door!
    void start_external(char *mString);

    // Message Autosig.
    void msg_autosig();

    // Main System Loop
    void da_system(char *mString);

    // Menu Command Processing
    void menu_docmd(CommandRec *cmdr);

    // Menu System Loop.
    void menu_setuser(UserRec *user);

    // Main System Loop for menu system.
    void menu_mainloop(UserRec *user);

};


# endif
