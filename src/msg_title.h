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

// Enthral SVN: $Id: msg_title.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msg_title.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef MSG_TITLE_H
# define MSG_TITLE_H


# include "struct.h"
# include "mb_api.h"       // Mainly for SESSION and i/o functions.
# include "msgs.h"

# include <string>
# include <vector>
# include <stdint.h>

//# define ulong unsigned long

typedef struct msgtitle_ini
{

    char sININAME[255];

    int  iTop;
    int  iBot;
    char sPAGENUM[80];
    char sPAGETOTAL[80];
    char sMOREUP[80];
    char sMOREUP_CHAR[80];
    char sMOREDOWN[80];
    char sMOREDOWN_CHAR[80];
    char sMOREMSG_ON[80];
    char sMOREMSG_WORD_ON[80];
    char sMOREMSG_OFF[80];
    char sMOREMSG_WORD_OFF[80];
    char sINPUT_BOX[20];
    char sTEXT_COLOR[10];
    char sTEXT_HILIGHT[10];
    char sMAX_AREAS[20];
    // Theme
    char sANSI_FILE[10];
    char sANSI_HELP[10];
    char sTHEME_NAME[20];
    char sMENU_PROMPT[20];
    char sMENU_PROMPT2[20];

    // Message Read .ini File
    bool mtitle_exists();
    void mtitle_create();
    void mtitle_chkpar(std::string &data);
    void mtitle_check(std::string cfgdata);
    int  mtitle_parse(int idx=0);

} msgtitle_ini;

class msg_title : private mbapi_jam, msgtitle_ini, msgs
{

    //private:
    UserRec *thisuser;

public:

    ulong CURRENT_BAR;

    int tTop;
    int tBot;

    msg_title();

    std::vector<list_bar>
    build_titlelist(vector<unsigned long> &elist);//, Query &qry);

    void  ParseHeader(char *filename);
    int   change_theme(int idx);
    void  SetupList(UserRec *user, int isPrivate);

    int   StartTitleList(int newscan,
                        unsigned long marea,
                        vector<unsigned long> &elist,
                        unsigned long currmsg);

};

#endif
