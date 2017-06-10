/***************************************************************************
 *   Copyright (C) 2004-2017 by Michael Griffin                            *
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

# ifndef MSG_EMAIL_H
# define MSG_EMAIL_H

# include "struct.h"
# include "mb_api.h"
# include "msgs.h"

# include <string>

typedef struct msgemail_ini {

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
    UserRec *thisuser;

public:
    int tTop;
    int tBot;

    msg_email();
    void ParseHeader(char *filename);
    void SetupList(UserRec *user);
    char *StartList();

};


# endif
