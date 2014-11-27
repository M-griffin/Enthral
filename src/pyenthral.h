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

void pybbs_run ( std::string scripts, UserRec *usr );


# endif
