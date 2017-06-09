/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose:                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// Enthral SVN: $Id: msg_read.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msg_read.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef MSG_READ_H
# define MSG_READ_H

# include <time.h>

#include <vector>

# include "struct.h"
# include "msgs.h"
# include "mb_api.h"
# include "mb_jam.h"
# include "users.h"

# include <string>

/******************************************************************************
 * Message Reader INI Class
 ******************************************************************************/
typedef struct msgread_ini {

    int iTop;
    int iBot;

    char sVIEW[80];

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

    char sTEXT_COLOR[10];
    char sQUOTE_COLOR[10];

    char sSYS_COLOR[10];
    char sORIGIN_COLOR[10];
    char sANSI_FILE[10];
    char sANSI_HELP[10];
    char sTHEME_NAME[20];
    char sMENU_PROMPT[20];
    char sMENU_PROMPT2[20];

    char sEOM_ON[20];
    char sEOM_WORD_ON[80];

    char sNXT_ON[20];
    char sNXT_WORD_ON[80];

    // Message Read .ini File
    int  msg_exists();
    void msg_create();
    void msg_chkpar(std::string &data);
    void msg_check(std::string cfgdata);
    int  msg_parse(int idx = 0);

} msgread_ini;


/******************************************************************************
 * Message Reader Class
 ******************************************************************************/
class msg_read : private
    mbapi_jam,
    msgread_ini,
    msgs,
    users
{
private:

    int  tTop,
         tBot,
         firstscan,    // Firstscan of current area
         gblNewScan;   // Global NewScan

    ulong Views;

    ulong MsgsLeft,
          CurMsgs,
          TotMsgs;

    std::vector<ulong> *msgidx_translation;
    ulong current_msgidx;

    int opscan; // Email SysOp Scan. Overwrite and view all messages.

public:
    msg_read();
    void start(UserRec *user);

    // Change Ansi Themes
    int change_theme(int idx=0);

    // Parsing the Message Header
    void ansi_file(char *filename);

    // Read in Message Only, for replying from title scan
    void ReadInMsgOnly(ulong marea);

    // Process both Normal and email/netmail message reading.
    int  ReadMsg(ulong mbnum, int showit, int newmsg, int private_area=FALSE);

    // Use this for quick message find lateron!
    int  ScanPosters(ulong marea);

    int  NewScanMsgs(int newmsg,ulong marea);
    int  ScanMessages(ulong marea);
    int  ReadMessages(ulong marea);

    void JumpToMessage();
    void IgnoreTheRest(ulong marea);
    int  DelCurMsg(ulong mbnum, ulong msgnum);
    int  NextAreaScan();

    int  verify_username(char *text, char *name);

    int  read_usersig(std::string &tmp);
    void write_usersig(std::string &tmp);
    int  SetupUserSig();

    void get_address(XMSG *xm);
//    void get_address(struct MemMessage *xm);

    void DoPostEmail(int Reply);
    void DoPost(int mbnum, int Reply);
    void SetupMsgPost();
    void DoEdit(int mbnum);
    void EditMessage();

    // Title Scan
    long title_scan(int newmsg, int multiscan = FALSE, char *mString=(char *)"\0");

    // Message Reader
    int  StartReader(int newmsg, ulong msgidx);


};


# endif
