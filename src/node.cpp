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

// Enthral SVN: $Id: node.cpp 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/node.cpp $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# include "struct.h"
# include "node.h"
# include "conio.h"

# include <cstdio>
# include <cstdlib>
# include <unistd.h> // gcc 4.7

using namespace std;

/**
 * Node Lock File
 */
int node::node_lockSet(int onoff)
{

    std::string path = LOCKPATH;
    path += "node.lck";

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
                perror("Error unable to read node.lck, check permissions!");
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
 * Clear Node File
 */
int node::node_remove(int nodenum)
{
    char path[255]= {0};
    sprintf(path,"%snode%i.dat",NODEPATH,nodenum);
    //remove(path);
    unlink(path);
    return TRUE;
}


/**
 * Clear Node Folder and all Drop files
 */
int node::node_remove_dropfiles(int nodenum)
{

    char path[255]= {0};
    sprintf(path,"rm -Rf %snode%i/*",NODEPATH,nodenum);
    system(path);
    return TRUE;
}

/**
 * Check if Node DAT ile exists.
 */
int node::node_exists(int nodenum)
{

    char path[255]= {0};
    sprintf(path,"%snode%i.dat",NODEPATH,nodenum);

    FILE *stream;
    stream = fopen(path,"rb+");
    if(stream == NULL)
    {
        return FALSE;
    }
    fclose(stream);
    return TRUE;
}

/**
 * Check if Node PIPE File exists.
 */
int node::node_socket_exists(int nodenum)
{
    SESSION s;
    char path[255]= {0};
    //sprintf(path,"%snode%i.dat",NODEPATH,nodenum);
    snprintf(path, sizeof path, "%s/enthral_sock%d", ENTHRALTMP, nodenum);

    s.pipe2ansi(path);

    FILE *stream;
    stream = fopen(path,"rb+");
    if(stream == NULL)
    {
        return FALSE;
    }
    fclose(stream);
    return TRUE;
}

/**
 * Read Node File
 */
int node::node_read(UserRec *user, int nodenum)
{

    char path[200]= {0};
    sprintf(path,"%snode%i.dat",NODEPATH,nodenum);

    node_lockSet(TRUE);

    int x   = 0;
    FILE *stream = fopen(path,"rb+");
    if(stream == NULL)
    {
        stream = fopen(path, "wb");
        if(stream == NULL)
        {
            //printf("Error creating callers");
            node_lockSet(FALSE);
            return x;
        }
    }
    if(fseek(stream,0,SEEK_SET)==0)
        x=fread(user,sizeof(UserRec),1,stream);

    fclose(stream);
    node_lockSet(FALSE);
    return(x);
}

/**
 * Write Node File
 */
int node::node_write(UserRec *user, int nodenum)
{

    char path[200]= {0};
    sprintf(path,"%snode%i.dat",NODEPATH,nodenum);

    node_lockSet(TRUE);
    int x   = 0;
    FILE *stream = fopen(path,"rb+");
    if(stream == NULL)
    {
        stream = fopen(path, "wb");
        if(stream == NULL)
        {
            perror("Error unable to read node.dat, check permissions!");
            node_lockSet(FALSE);
            return x;
        }
    }
    if(fseek(stream,0,SEEK_SET)==0)
        x = fwrite(user,sizeof(UserRec),1,stream);
    fclose(stream);
    node_lockSet(FALSE);
    return x;
}



/**
 * Generic List of Whois Online
 */
void node::whoisonline()
{

    UserRec user;
    char buffer[255]= {0};

    SESSION s;
    s.ansiPrintf((char *)"whois");

    // Add config option for how many to display!
    for(int i = 1; i < 6; i++)
    {
        if (node_exists(i) == TRUE)
        {
            // If Node Exists, Check for Socket, if Found get read user info.
            node_read(&user,i);
            snprintf(buffer, sizeof buffer, "      |13%02d    |15%-18s |07%-15s|CR", i, user.handle, user.usernote);
            s.pipe2ansi(buffer);
        }
        else
        {
            // No one found, mark this node as Empty!
            snprintf(buffer, sizeof buffer, "      |13%02d    |11<|03empty|11>|CR", i);
            s.pipe2ansi(buffer);
        }
    }
}


