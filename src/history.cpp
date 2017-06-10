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

# include "history.h"
# include "struct.h"

# include <stdio.h>
# include <time.h>
# include <ctype.h>
# include <string>
# include <fstream>
# include <unistd.h>

using namespace std;

/**
 * Lock History File before writting new data to it.
 */
int history::daily_lockSet(int onoff)
{
    std::string path = LOCKPATH;
    path += "daily.lck";

    if (!onoff) {
        remove((char *)path.c_str());
        return TRUE;
    }

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
 * Write Daily History
 */
int history::daily_write(History *hist)
{

    time_t t;
    tm *tm;
    char datestr[81]= {0};

    t  = time(NULL);
    tm = localtime(&t);
    strftime(datestr,81,"%Y%m%d",tm);

    std::string
    path    = DATAPATH;
    path   += "daily";
    path   += datestr;
    path   += ".dat";
    int x   = 0;
    daily_lockSet(TRUE);

    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL) {
            daily_lockSet(FALSE);
            return x;
        }
    }
    if(fseek(stream,0,SEEK_SET)==0)
        x = fwrite(hist,sizeof(History),1,stream);
    fclose(stream);
    daily_lockSet(FALSE);
    return x;
}

/**
 * Read Daily History
 */
int history::daily_read(History *hist)
{
    time_t t;
    tm *tm;
    char datestr[81]= {0};

    t  = time(NULL);
    tm = localtime(&t);
    strftime(datestr,81,"%Y%m%d",tm);

    std::string
    path    = DATAPATH;
    path   += "daily";
    path   += datestr;
    path   += ".dat";
    int x   = 0;

    daily_lockSet(TRUE);
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        stream=fopen(path.c_str(), "wb");
        if(stream == NULL) {
            daily_lockSet(FALSE);
            return x;
        }
    }
    fclose(stream);

    stream = fopen(path.c_str(), "rb");
    if(fseek(stream,0,SEEK_SET)==0)
        x = fread(hist,sizeof(History),1,stream);
    fclose(stream);
    daily_lockSet(FALSE);
    return x;
}

/**
 * History Lockfile
 */
int history::hist_lockSet(int onoff)
{
    std::string path = LOCKPATH;
    path += "history.lck";

    if (!onoff) {
        remove((char *)path.c_str());
        return TRUE;
    }

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
 * Write History
 */
int history::hist_write(History *hist)
{

    std::string path = DATAPATH;
    path   += "history.dat";
    int x   = 0;
    hist_lockSet(TRUE);

    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL) {
            hist_lockSet(FALSE);
            return x;
        }
    }
    if(fseek(stream,0,SEEK_SET)==0)
        x = fwrite(hist,sizeof(History),1,stream);
    fclose(stream);
    hist_lockSet(FALSE);
    return x;
}

/**
 * Read History
 */
int history::hist_read(History *hist)
{

    std::string path = DATAPATH;
    path   += "history.dat";
    int x   = 0;

    hist_lockSet(TRUE);
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        stream=fopen(path.c_str(), "wb");
        if(stream == NULL) {
            hist_lockSet(FALSE);
            return x;
        }
    }
    fclose(stream);

    stream = fopen(path.c_str(), "rb");
    if(fseek(stream,0,SEEK_SET)==0)
        x = fread(hist,sizeof(History),1,stream);
    fclose(stream);
    hist_lockSet(FALSE);
    return x;
}
