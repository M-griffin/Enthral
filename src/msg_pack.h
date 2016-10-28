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

// Enthral SVN: $Id: msg_pack.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msg_pack.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef MSG_PACK_H
# define MSG_PACK_H

/*
# include "struct.h"
# include "mb_api.h"
# include "msgs.h"

# include <string>


class msg_pack : private mbapi_jam {

    unsigned long lastmsg;
    unsigned long total;
    unsigned long mbnum;
    unsigned long msgProcessed; // # of Termed Messages.

    mb_list_rec   mb;
    msgs          _msgf;

    public:
    msg_pack();
    int  ReadMessages();

    // Read in only current are and truncate mesasge base.
    void TruncCurrentArea(int iArea, int MaxMsgs);

    // Read in only current are and pack mesasge base.
    void PackCurrentArea(int iArea);
    // Change to next message area, if doesn't exist return FALSE!
    int  ChangeNextArea();
    // Loop throguh each message area and pack bases.
    void PackAllAreas();

    // Loop through and Truncate All Message areas.
    void TruncAllAreas();

};
*/

# endif
