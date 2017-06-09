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

// Enthral SVN: $Id: msg_newscan.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msg_newscan.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef MSG_NEWSCAN_H
# define MSG_NEWSCAN_H

# include "struct.h"

typedef struct msg_newscan {

    int  new_lockSet(int onoff);
    int  new_read(NewScan *ns, int idx, char *mbase);
    int  new_write(NewScan *ns, int idx, char *mbase);
    int  new_count(char *mbase);

    // Message NewScan Functions
    int  read_mbase(mb_list_rec *mr, int rec);
    int  check_mbase(UserRec *thisuser);


} msg_newscan;

# endif
