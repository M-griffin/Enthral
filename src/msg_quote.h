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

# ifndef MSG_QUOTE_H
# define MSG_QUOTE_H

# include "struct.h"
# include "mb_api.h"

# include <string>
# include <ctime>

typedef struct msgquote_ini {
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
    bool msg_exists();
    void msg_create();
    void msg_chkpar(std::string &data);
    void msg_check(std::string cfgdata);
    void msg_parse();

} msgquote_ini;


class msg_quote : private mbapi_jam, msgquote_ini
{
private:

    int tTop;
    int tBot;

public:
    msg_quote();
    void ParseMQuote(char *filename);
    void insert_lines(LineRec *orgLink, msg_readll *mLink);
    void setup_quoter();
    void par_qstring(char * rBuffer, msg_readll *mLink);
    void StartQuoter(LineRec *orgLink, std::string &retbuf);

};


# endif
