/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Basic User Data I/O                                          *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// Enthral SVN: $Id: users.cpp 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/users.cpp $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# include "users.h"
# include "struct.h"
# include "conio.h" // testing. elog

# include <cstdio>
# include <cctype>
# include <cstring>        // gcc 4.3
//# include <algorithm>    // gcc 4.3 transform() (Doesn't work!)
//# include <iterator>
//# include <parallel/algo.h>

# include <string>
# include <unistd.h> // gcc 4.7

using namespace std;

/**
 * User DAT Lock File
 */
int users::users_lockSet(int onoff)
{

    std::string path = LOCKPATH;
    path += "users.lck";

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
                printf("Error users.lck!");
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
 * User Index DAT Lock File
 */
int users::uidx_lockSet(int onoff)
{

    std::string path = LOCKPATH;
    path += "uidx.lck";

    if (!onoff) {
        remove((char *)path.c_str());
        return TRUE;
    }

    //While lock file missing, loop untill it disapears.
    FILE *stream;
    while(1) {
        stream = fopen(path.c_str(),"rb+");
        if(stream == NULL) { // Lock File Missing
            stream = fopen(path.c_str(), "wb");
            if(stream == NULL) {
                printf("Error uidx.lck!");
                return FALSE;
            } else {
                fclose(stream);
                return TRUE;    // Created Lock File
            }
        }
        fclose(stream);
        usleep(10*20000);
    }
}

/**
 * Write User Record Temp
 */
int users::users_writet(UserRec *usr, int idx)
{

    std::string path = DATAPATH;
    path += "users.tmp";

    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL) {
            printf("Error users_write temp!");
            return x;
        }
    }
    if(fseek(stream,(int)idx*sizeof(UserRec),SEEK_SET)==0)
        x = fwrite(usr,sizeof(UserRec),1,stream);
    fclose(stream);
    return x;
}

/**
 * Write User Record
 */
int users::users_write(UserRec *usr, int idx)
{

    std::string path = DATAPATH;
    path += "users.dat";

    users_lockSet(TRUE);
    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL) {
            printf("Error users_write!");
            users_lockSet(FALSE);
            return x;
        }
    }
    if(fseek(stream,(int)idx*sizeof(UserRec),SEEK_SET)==0)
        x = fwrite(usr,sizeof(UserRec),1,stream);
    fclose(stream);
    users_lockSet(FALSE);
    return x;
}

/**
 * Read User Record
 */
int users::users_read(UserRec *usr, int idx)
{

    std::string path = DATAPATH;
    path += "users.dat";

    users_lockSet(TRUE);
    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        stream=fopen(path.c_str(), "wb");
        if(stream == NULL) {
            printf("Error users_read!");
            users_lockSet(FALSE);
            return x;
        }
    }
    fclose(stream);

    stream = fopen(path.c_str(), "rb");
    if(fseek(stream,(int)idx*sizeof(UserRec),SEEK_SET)==0)
        x = fread(usr,sizeof(UserRec),1,stream);
    fclose(stream);
    users_lockSet(FALSE);
    return x;
}

/**
 * Read User Record Index
 */
int users::idx_writet(UserIdx *usr, int idx)
{

    std::string path = DATAPATH;
    path += "uidx.tmp";

    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL) {
            printf("Error uidx_write temp!");
            return x;
        }
    }
    if(fseek(stream,(int)idx*sizeof(UserIdx),SEEK_SET)==0)
        x = fwrite(usr,sizeof(UserIdx),1,stream);
    fclose(stream);
    return x;
}

/**
 * Write User Record Index
 */
int users::idx_write(UserIdx *usr, int idx)
{

    std::string path = DATAPATH;
    path += "uidx.dat";

    uidx_lockSet(TRUE);
    int x = 0;
    FILE *stream=fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        stream=fopen(path.c_str(), "wb");
        if(stream == NULL) {
            printf("Error uidx_write!");
            uidx_lockSet(FALSE);
            return x;
        }
    }
    if(fseek(stream,(int)idx*sizeof(UserIdx),SEEK_SET)==0)
        x = fwrite(usr,sizeof(UserIdx),1,stream);
    fclose(stream);
    uidx_lockSet(FALSE);
    return x;
}

/**
 * Read User Index Record
 */
int users::idx_read(UserIdx *usr, int idx)
{

    std::string path = DATAPATH;
    path += "uidx.dat";

    uidx_lockSet(TRUE);
    int x = 0;
    FILE *stream=fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        stream=fopen(path.c_str(), "wb");
        if(stream == NULL) {
            printf("Error uidx_read!");
            uidx_lockSet(FALSE);
            return x;
        }
    }
    fclose(stream);

    stream = fopen(path.c_str(), "rb");
    if(fseek(stream,(int)idx*sizeof(UserIdx),SEEK_SET)==0)
        x = fread(usr,sizeof(UserIdx),1,stream);
    fclose(stream);
    uidx_lockSet(FALSE);
    return x;
}

/**
 * Count User Index's
 */
int users::idx_count()
{

    UserIdx usr;
    int i = 0;
    while(idx_read(&usr,i)) {
        ++i;
    }
    ++i;

    (i < 1) ? i = -1 : i--;
    return(i);
}

/**
 * Find User by Index
 */
int users::idx_find(char *name)
{

    UserIdx usr;
    int idx = 0;
    std::string temp1, temp2;

    temp1 = (name);

    //move temp1 to lower case for testing!
    for (std::string::size_type i = 0; i < temp1.size(); ++i ) {
        temp1[i] = tolower( temp1[i] );
    }

    while(idx_read(&usr,idx)) {
        temp2 = (char *)(usr.handle);
        //  transform(temp1.begin(),temp1.end(), temp1.begin(), tolower);
        //  transform(temp2.begin(),temp2.end(), temp2.begin(), tolower);
        for (std::string::size_type i = 0; i < temp2.size(); ++i ) {
            temp2[i] = tolower( temp2[i] );
        }

        if(temp1 == temp2) return(idx);
        ++idx;
    }
    return(-1);
}

/**
 * Match User Record by Name
 */
bool users::idx_match(char *name)
{

    int index =- 1;
    index = idx_find(name);
    if (index == -1) return false;
    else return true;
}

/**
 * Create New Index Record
 */
void users::idx_new(char *name, int idx)
{

    UserIdx usr;
    memset(&usr,0,sizeof(UserIdx));
    strcpy((char *)usr.handle, name);
    usr.num = idx;
    idx_write(&usr,idx);

}

/**
 * Check User Password
 */
bool users::check_password(char *name, char *pass)
{

    UserRec r;
    memset(&r,0,sizeof(UserRec));
    int idx = 0;
    std::string temp1, temp2;

    temp1 = (name);
    idx = idx_find(name);
    users_read(&r,idx);
    temp1 = (char *)r.password;
    temp2 = pass;

    SESSION s;
    s.errlog2 ((char *)"* NAME %s,%i",name,idx);
    s.errlog2 ((char *)"* PASS %s",pass);
    s.errlog2 ((char *)"Password Database [%s], Match [%s]",(char *)temp1.c_str(),(char *)temp2.c_str());

    if (temp2.find("\r",0) != std::string::npos)
        s.errlog2 ((char *)"Password found CR!");

    if (temp2.find("\n",0) != std::string::npos)
        s.errlog2 ((char *)"Password found LF!");

    if (temp2.find("\0",0) != std::string::npos)
        s.errlog2 ((char *)"Password found \0!");

    if(temp1 == temp2)
        return true;
    else return false;
}


/**
 * Check Who's Birthday is today (WIP)
 */
/*
int users::getAllBDays() {

    UserRec usr;
    int  i = 0, iResult;
    long dtBdate;

    while(users_read(&usr,i++))
        iResult = isBday(dtBdate);
    }
}

*/
