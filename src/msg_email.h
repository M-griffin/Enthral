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

// Enthral SVN: $Id: msg_email.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msg_email.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef MSG_EMAIL_H
# define MSG_EMAIL_H

//# include <time.h>

# include "struct.h"
# include "mb_api.h"		// Mainly for SESSION and i/o functions.
# include "msgs.h"

// pickup thisuser and session from msg_api


# include <string>

typedef struct msgemail_ini
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

    // Message Read .ini File
    bool msga_exists();
    void msga_create();
    void msga_check(std::string cfgdata);
    void msga_parse();

} msgemail_ini;


class msg_email : private mbapi_jam, msgemail_ini, msgs
{

    //private:
    UserRec *thisuser;

public:
    int tTop;
    int tBot;

    msg_email();
    void ParseHeader(char *filename);
    //void SetupList();
    void SetupList(UserRec *user);
    char *StartList();

};


# endif
