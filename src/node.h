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

} node;

# endif
