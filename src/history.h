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

# ifndef HISTORY_H
# define HISTORY_H

# include "struct.h"


class history
{
public:
    int  daily_lockSet(int onoff);
    int  daily_write(History *hist);
    int  daily_read(History *hist);

    int  hist_lockSet(int onoff);
    int  hist_write(History *hist);
    int  hist_read(History *hist);
};


# endif
