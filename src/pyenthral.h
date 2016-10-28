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

// Enthral SVN: $Id: pyenthral.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/pyenthral.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

#ifndef PY_ENTHRAL_H
#define PY_ENTHRAL_H

# include "struct.h"
# include <string>

using namespace std;


//------------------------------------------------------------------------------
// Name: main()
// Desc: Application's main entry point.
//------------------------------------------------------------------------------
//void main( void )

void pybbs_run(std::string scripts, UserRec *usr);


# endif

