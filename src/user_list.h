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

# ifndef USER_LIST_H
# define USER_LIST_H

//# include <time.h>

# include "struct.h"
# include "mb_api.h"       // Mainly for SESSION and i/o functions.
# include "msgs.h"

# include <string>
# include <vector>

//# define ulong unsigned long

typedef struct usrlist_ini
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
    bool usrl_exists();
    void usrl_create();
    void usrl_chkpar(std::string &data);
    void usrl_check(std::string cfgdata);
    int  usrl_parse(int idx=0);

} usrlist_ini;

class usr_list : private mbapi_jam, usrlist_ini
{

    //private:
    UserRec *thisuser;

public:
    int tTop;
    int tBot;

    // This holds the current user select in the list.
    int CURRENT_ULIST;

    usr_list();


    std::vector<UserRec> read_users();
    std::vector<list_bar> build_userlist(UserRec *usr, std::string &temp);//, Query &qry);

    void ParseHeader(char *filename);
    int  change_theme(int idx);
    void SetupList(UserRec *user);
    void SetupEmailList(UserRec *user);

    void UserList(std::string &tmp);

    BOOL StartUserEditor(ulong usernum);
    int  StartList(int email=TRUE);

};


# endif
