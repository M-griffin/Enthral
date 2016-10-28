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

// Enthral SVN: $Id: file_edit.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/file_edit.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef FILE_EDIT_H
# define FILE_EDIT_H

# include "struct.h"
# include "files.h"
# include "conio.h"


class file_edit : private SESSION
{

private:
    PASSING   *pass;
    files      _ffunc;

public:

    char *faddr2char(char *s,fidoaddr *fa);
    void fbeditmenu();
    void kill_fbase(int fbnum);
    void delete_fbase();
    void poke_fbase(int pokenum);
    void init_fbase(fb_list_rec *fb);
    int  list_file_bases(int page);
    void insert_fbase();
    void edit_aka(fb_list_rec *fr);
    //void ACS(ACSrec *acs, char *fstr);
    void mod_fbase(int fbnum);
    void modify_fbase();
    void swap_fbase(int iFrom, int iTo);
    void move_fbase();

};


# endif

