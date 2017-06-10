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

# include "msg_stats.h"
# include "struct.h"

# include <string>
# include <fstream>

# include <cstdio>
# include <ctime>
# include <cctype>
# include <unistd.h>

using namespace std;

/**
 *  Lock for Message History File
 */
int msg_stats::hist_lockSet(int onoff, char *area)
{
    std::string path = LOCKPATH;
    path   += area;
    path   += "_stats.lck";

    if (!onoff) {
        remove((char *)path.c_str());
        return TRUE;
    }

    //While lock file missing, create, or loop until it disapears.
    FILE *stream;
    while(1) {
        stream = fopen(path.c_str(),"rb+");
        if(stream == NULL) {
            stream = fopen(path.c_str(), "wb");
            if(stream == NULL) {
                return FALSE;
            } else {
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
    if(stream == NULL) {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL) {
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
    if(stream == NULL) {
        stream=fopen(path.c_str(), "wb");
        if(stream == NULL) {
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
