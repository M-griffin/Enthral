/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Basic User Data I/O                                          *
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

# ifndef USERS_H
# define USERS_H

# include "struct.h"

typedef struct users
{

    // User File
    int  users_lockSet ( int onoff );
    int  uidx_lockSet ( int onoff );
    int  users_writet ( UserRec *usr, int idx );
    int  users_write ( UserRec *usr, int idx );
    int  users_read ( UserRec *usr, int idx );

    // User Index File for Faster Read / Writes
    int  idx_writet ( UserIdx *usr, int idx );
    int  idx_write ( UserIdx *usr, int idx );
    int  idx_read ( UserIdx *usr, int idx );
    int  idx_count();
    int  idx_find ( char *name );
    bool idx_match ( char *name );
    void idx_new ( char *name, int idx );
    bool check_password ( char *name, char *pass );

} users;

# endif
