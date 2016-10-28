/***************************************************************************
 *   Copyright (C) 2004-2013 by Michael Griffin                            *
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

// Enthral SVN: $Id: msg_fse.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msg_fse.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef MSG_FSE_H
# define MSG_FSE_H

# include "struct.h"
//# include "msg_api.h"

# include "mb_api.h"
# include "msg_readll.h"

# include <string>
# include <vector>

using namespace std;

class msg_fse :
    //private msg_api
private mbapi_jam
{

private:
    // Main List for Holding All Data
    LineRec     *head;          // Pointer to Fist in List
    LineRec     *current_node;  // Current Pointer in List
    LineRec     *last;          // Pointer to Last Element in List.

    UserRec     *thisuser;
    msg_readll  *reader_list;
    MsgHead      mHLocal;

    // Editor Variables
    int Row,       // Y - Row    [Lines]
    Col,       // X - Colume [Char in Line]
    Len,       // Line Length

    Top,       // Top Margin
    Bot,       // Bottom Margin

    Tot,       // Total # of Lines in Link List
    Num;       // Current Line Number in Link List

    bool MSG_REPLY;

    // Holds String Data for Current Line in use
    std::string Line;   // Line Data
    std::string TLine;  // Temp Line Data

    bool OVERWRITE;     // Do Replace or Insert

    int  TRow;      // Temp Row, that were Line Wrapping On (garbage NOT IN USE!)
    bool cont;      // If continious wrappign is on or off (word wrap es, NOT IN USE!)
    int  bPush;     // If were pushing from the middle of the line

    char sRow[15];  // Row
    char sCol[15];  // Colume for FSE Ansi Display
    char sANSI_FILE[15];
    char sTHEME_NAME[20];
    char sTEXT_COLOR[15];
    char sMENU_PROMPT[15];

public:
    msg_fse(UserRec *urec);
    ~msg_fse();

    std::string buffer;

    // Link List Functions  -  Note can move this to reference msg_ll.cpp / h
    void add_to_list(std::string);
    void stripCRONLY(char *ostr);
    void PutBuffer(char *mBuff);
    void save_all();

    int  move_up();
    int  move_down();
    void move_first();
    void MoveAllDown();
    void move_last();
    int  line_count();
    int  line_total();
    void display_list();
    void delete_line_up();
    void delete_line();
    void dispose_list();
    void clear_all();
    void print_options();

    // FSE Functions
    void insert_line();
    void box_clear();
    void box_redraw();

    int  format_paragraph();
    void word_wrapex();
    void word_wrap();
    void add_char(unsigned char c);

    void up_arrow();
    void dn_arrow();
    void rt_arrow();
    void lt_arrow();
    void home_cursor();
    void end_cursor();
    void Last_Line();
    void box_scrolldn();

    int  parsere(char *qText);
    int  options_prompt(unsigned char c = '\0');
    void insert_lines();

	std::vector<std::string> inline split_string(const std::string &source, 
                                                 const char *delimiter = " ", 
                                                 bool keepEmpty = false);

	void delete_key();
		
    int  poll_chr(int reply, int msg_edit, MsgHead *mH, msg_readll *mL = 0);

private:
    // fse.ini
    int  change_theme(int idx=0);
    void ansi_file(char *filename);
    bool fse_exists();
    void fse_create();
    void fse_chkpar(std::string &temp);
    void fse_check(std::string cfgdata);
    int  fse_parse(int idx=0);

};


# endif
