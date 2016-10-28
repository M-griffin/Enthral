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

// Enthral SVN: $Id: msg_area.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msg_area.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef MSG_AREA_H
# define MSG_AREA_H

//# include <time.h>
# include <string>
# include <vector>

# include "struct.h"
# include "mb_api.h"
# include "msgs.h"


typedef struct msgarea_ini
{

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
    bool msga_exists();
    void msga_create();
    void msga_chkpar(std::string &data);
    void msga_check(std::string cfgdata);
    int  msga_parse(int idx=0);

} msgarea_ini;

class msg_area : private mbapi_jam, msgarea_ini, msgs
{

    //private:
    UserRec *thisuser;

public:
    int tTop;
    int tBot;

    msg_area();

    void init_db();

    std::vector<mb_list_rec> read_areas();
    std::vector<list_bar> build_arealist(UserRec *usr, std::string &temp);//, Query &qry);


    void ParseHeader(char *filename);
    int  change_theme(int idx);
    void SetupList(UserRec *user);
    //char *StartList(char *OrgMsg);

    char *StartList();
    //char *StartList(Query &qry);

};


# endif
