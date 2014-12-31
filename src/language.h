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

# ifndef LANGUAGE_H
# define LANGUAGE_H

# include "struct.h"

# include <string>

using namespace std;

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
