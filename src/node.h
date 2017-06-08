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

// Enthral SVN: $Id: node.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/node.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef NODE_H
# define NODE_H

# include "struct.h"

typedef struct node {

    int  node_lockSet(int onoff);
    int  node_remove(int nodenum);
    int  node_remove_dropfiles(int nodenum);
    int  node_exists(int nodenum);
    int  node_socket_exists(int nodenum);
    int  node_read(UserRec *user, int nodenum);
    int  node_write(UserRec *user, int nodenum);

    void whoisonline();

    /*  Not used in here, this is going on in conio now,
        only calling node to get who's online.

        void node_message();
        void node_global_announce(char *nmsg);
        void node_global_announce_login();
        void node_global_announce_logoff();
    */
} node;

# endif
