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

# ifndef MAIN_SYSTEM_H
# define MAIN_SYSTEM_H

# include "struct.h"
# include "conio.h"
# include "node.h"

class main_system :
    private ConsoleIO, node
{

public:

    void start(UserRec *user);

};


# endif
