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

// Enthral SVN: $Id: file_area.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/file_area.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef FILE_AREA_H
# define FILE_AREA_H

//# include <time.h>

# include "struct.h"
# include "mb_api.h"       // Mainly for SESSION and i/o functions.
# include "files.h"

// pickup thisuser and session from msg_api
# include <string>

typedef struct filearea_ini {

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
    bool filea_exists();
    void filea_create();
    void filea_chkpar(std::string &data);
    void filea_check(std::string cfgdata);
    int  filea_parse(int idx=0);

} filearea_ini;

class file_area : private mbapi_jam, filearea_ini, files
{

    //private:
    UserRec *thisuser;

public:
    int tTop;
    int tBot;

    file_area();
    void ParseHeader(char *filename);
    int  change_theme(int idx);
    void SetupList(UserRec *user);
    char *StartList(char *OrgMsg);

};


# endif
