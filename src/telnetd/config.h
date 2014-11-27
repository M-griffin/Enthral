/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: BBS Config INI I/O                                           *
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

# ifndef __CONFIG_H
# define __CONFIG_H

#include <string>
#include <limits.h>

using namespace std;

extern char PATH[PATH_MAX];
extern char USE_FILE_LOG;
extern char USE_HOSTS_DENY;
extern char BLOCK_NO_HOSTNAME;
extern char HOSTS_DENY_PATH[PATH_MAX];
extern char ENTHRAL_TEMP_PATH[PATH_MAX];

// Temp Folder for Passing Drop files to Enthral.
extern std::string term_passing;

bool configdataexists();
void createconfig();
void checkcfg ( std::string cfgdata );
void parseconfig();

# endif
