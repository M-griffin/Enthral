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

# ifndef MAIN_SYSTEM_H
# define MAIN_SYSTEM_H


# include "struct.h"
# include "conio.h"
# include "node.h"

/*
    This is the Main Global Entry Point for the BBS
    All System Information per Session will be kept
    and Passed along from this point .. .

    User Info, BBS Configuration Info.. etc..
    This is Repensent at the global where everything
    is init'd and falls back to.
*/

class main_system :
    private SESSION, node   // Input Output
{

public:
	
    //menu_func   _mnf;
    void start(UserRec *user);

};


# endif
