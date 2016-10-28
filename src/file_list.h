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

// Enthral SVN: $Id: file_list.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/file_list.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef FILE_LIST_H
# define FILE_LIST_H

# include "struct.h"
//# include "msg_api.h"


// pickup thisuser and session from msg_api

# include <time.h>
# include <string>

/*
typedef struct filelist_ini
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
    char sQUOTE_RANGE[20];

    // Message Read .ini File
    bool filelist_exists();
    void filelist_create();
    void filelist_chkpar(std::string &data);
    void filelist_check(std::string cfgdata);
    void filelist_parse();

} filelist_ini;


class filelist : private msg_api, filelist_ini
{
private:

    int tTop;
    int tBot;

public:
    filelist();
    void Parsefilelist(char *filename);
    void insert_lines(LineRec *orgLink, msg_readll *mLink);
    void setup_filelist();
    void par_qstring(char * rBuffer, msg_readll *mLink);
    void Startfilelist(LineRec *orgLink, std::string &retbuf);

};
*/


# endif
