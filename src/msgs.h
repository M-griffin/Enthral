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

# ifndef MSGS_H
# define MSGS_H

# include "struct.h"

# include <string>

typedef struct msgs
{

	/*
    int  read_emaillist(EmailIdx *em, int recno);
    int  save_emaillist(EmailIdx *em, int recno);
	*/

    int  mbaselist_lockSet(int onoff);

    int  read_mbaselist(mb_list_rec *mr, int recno);

    int  save_mbasetemp(mb_list_rec *mr, int recno);

    int  save_mbaselist(mb_list_rec *mr, int recno);

    int  msg_count();
    void save_msgbase(mb_list_rec *mb);
    int  msg_find(char *tfile);

    int  jlr_lockSet(int onoff);
    int  readlr(LastRead *lr, int idx, mb_list_rec *mb);
    int  writelr(LastRead *lr, int idx, mb_list_rec *mb);

    unsigned
	long JamAreaGetLast(long usernum, mb_list_rec *mb);
    void JamAreaSetLast(long usernum, long msgnum, mb_list_rec *mb);

    unsigned
    long CountMsgs(unsigned long mbnum, UserRec *usr),
         CountNewMsgs(unsigned long mbnum, UserRec *usr),
         CountCurrentNewMsgs(UserRec *usr),
         CountAllNewMsgs(UserRec *usr);

    void resetlastread(UserRec *usr);

    unsigned long
    getlastread(unsigned long mbnum, UserRec *usr);


} msgs;

# endif
