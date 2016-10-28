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

// Enthral SVN: $Id: msg_stats.cpp 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msg_stats.cpp $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# include "msg_stats.h"
# include "struct.h"

# include <string>
# include <fstream>

# include <cstdio>
# include <ctime>
# include <cctype>
# include <unistd.h> // gcc 4.7

using namespace std;

/**
 *  Lock for Message History File
 */
int msg_stats::hist_lockSet(int onoff, char *area)
{

    std::string path = LOCKPATH;
    path   += area;
    path   += "_stats.lck";

    if (!onoff)
    {
        remove((char *)path.c_str());
        return TRUE;
    }

    //While lock file missing, create, or loop until it disapears.
    FILE *stream;
    while(1)
    {
        stream = fopen(path.c_str(),"rb+");
        if(stream == NULL)
        {
            stream = fopen(path.c_str(), "wb");
            if(stream == NULL)
            {
                //elog("Error history.lck!");
                return FALSE;
            }
            else
            {
                fclose(stream);
                return TRUE;
            }
        }
        fclose(stream);
        usleep(10*20000);
    }
}

/**
 *  Write Data
 */
int msg_stats::hist_write(MsgStats *stats, char *area)
{

    std::string path = MESGPATH;
    path   += area;
    path   += ".sts";
    int x   = 0;
    hist_lockSet(TRUE,area);

    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL)
        {
            //elog("Error hist_write!");
            hist_lockSet(FALSE,area);
            return x;
        }
    }
    if(fseek(stream,0,SEEK_SET)==0)
        x = fwrite(stats,sizeof(MsgStats),1,stream);
    fclose(stream);
    hist_lockSet(FALSE,area);
    return x;
}

/**
 *  Read Data
 */
int msg_stats::hist_read(MsgStats *stats, char *area)
{

    std::string path = MESGPATH;
    path   += area;
    path   += ".sts";
    int x   = 0;

    hist_lockSet(TRUE,area);
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        stream=fopen(path.c_str(), "wb");
        if(stream == NULL)
        {
            //elog("Error hist_read!");
            hist_lockSet(FALSE,area);
            return x;
        }
    }
    fclose(stream);

    stream = fopen(path.c_str(), "rb");
    if(fseek(stream,0,SEEK_SET)==0)
        x = fread(stats,sizeof(MsgStats),1,stream);
    fclose(stream);
    hist_lockSet(FALSE,area);
    return x;
}


