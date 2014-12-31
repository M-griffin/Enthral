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

# ifndef MSG_READLL_H
# define MSG_READLL_H

# include "struct.h"
# include "conio.h"

# include <vector>

using namespace std;

typedef struct msg_readll : private SESSION
{

public:
    // Main List for Holding All Data
    LineRec *head;
    LineRec *current_node;
    LineRec *last;
    AreaRec *node;
    // Passing Lightbars between Interfaces
    std::vector<list_bar> listing;

    int Row;
    int Top;
    int Bot;
    int Tot;
    int Num;
    int Lines;
    int Page;
    int TotPages;
    int reader;
    int quote;
    int arealist;
    int pgRows;

    int headerrows;
    int current_selection; // Current Line in Box of Selection.

    int PAGE_SCROLL;
    int CLEAR_BOX;

    // Quote and Title Scans
    char LoColor[10];
    char HiColor[10];

    // Message Reader
    char TEXT_COLOR[10];
    char QUOTE_COLOR[10];
    char SYS_COLOR[10];
    char ORIGIN_COLOR[10];

    msg_readll();
    ~msg_readll();

    void initQuote(char *sLow, char *sHigh,int q = FALSE);
    void initReader(char *sTEXT, char *sQUOTE, char *sSYS, char *sORIGIN);
    void initList(char *sLow, int alist = FALSE);
    void togglePageScroll(int on = TRUE);
    void clearBox();

    // Copy message from link lists to buffer
    void stripCRONLY(char *ostr);
    //char *GetBuffer();
    void GetBuffer(std::string &tbuf);
    void GetVector(std::vector<list_bar> listb);

    char *PopulateQuoter();

    // Copy From buffer into Link List (UserSig) Clean
    void PutBufferSig(char *mBuff, int Strip = FALSE);
    void PutBuffer(char *mBuff, int Strip = TRUE);

    void cleartags(char *tag, char *origin);

    void clear_all();
    void select_all();

    void display_list(void);
    void flag_line(int iNum);
    void delete_line_up();
    void add_to_list(std::string add_data);
    int  move_up();
    int  move_down();
    int  line_count();
    int  line_total();
    void Last_Line();
    void dispose();
    void dispose_list();
    // Line Reader - old school scrolling.
    char *parseReadColorScroller(char *cap,std::string data);
    char *parsereadcolors(char *cap,int row, int col, std::string data);
    // Line Reader - old school scrolling.
    void box_scroll_reader();
    void box_pgdn_scroll();
    // Normal Lightbar...
    void box_start(unsigned long CURRENT);    // CURRENT = ARea MAREA or ULIST etc...
    void box_scrolldn();
    void box_pgdn();
    void box_pgup();
    void box_refresh(unsigned long CURRENT);
    void box_overdraw(unsigned long CURRENT);

    // New std::vector list parsing.
    // Not in use - box_start_vector replaces all 3.
    void box_pgup_vector();
    void box_scrolldn_vector();
    void box_pgdn_vector();

    // Working Up and Down, need to add Scroll Down now.
    //void box_start_vector(unsigned long CURRENT); // Area Listing.
    void box_start_vector(unsigned long page, unsigned long list);

} msg_readll;

# endif
