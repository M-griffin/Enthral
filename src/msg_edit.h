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

// Enthral SVN: $Id: msg_edit.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msg_edit.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef MSG_EDIT_H
# define MSG_EDIT_H

# include "struct.h"
# include "msgs.h"
# include "conio.h"

class msg_edit : private ConsoleIO
{

private:
    PASSING   *pass;
    msgs      _mfunc;

public:

    void kill_mbase(int mbnum);
    void delete_mbase();
    void poke_mbase(int pokenum);
    void init_mbase(mb_list_rec *mb);

    void insert_mbase();
    void edit_aka(mb_list_rec *mr);
    //void ACS(ACSrec *acs, char *fstr);
    void mod_mbase(int mbnum);
    void modify_mbase();
    void swap_mbase(int iFrom, int iTo);
    void move_mbase();

    char *faddr2char(char *s,fidoaddr *fa);

    int  list_message_bases(int page);
    void mbeditmenu();
};


# endif
