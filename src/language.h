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

// Enthral SVN: $Id: language.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/language.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# ifndef LANGUAGE_H
# define LANGUAGE_H

# include <string>

using namespace std;

# include "struct.h"

class language
{
public:
    // Language File, Read and Compile new
    void lang_remove();
    int  lang_write(LangRec *lang, int idx);
    int  lang_read(LangRec *lang, int idx);
    int  lang_count();
    void lang_get(char *lang, int idx);
    void lang_check(std::string lang);
    void lang_compile();

};


# endif
