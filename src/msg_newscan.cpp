/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Message NewScan Toggles for Active and Inactive Areas        *
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

# include "struct.h"
# include "msg_newscan.h"

# include <cstdio>
# include <string>
# include <unistd.h> // gcc 4/7

using namespace std;

/*
  (WIP)
  When ever a message base is created, a newscan file /record should be created for
  each user with a default of TRUE

  When a new user is added they should be added to each newscan dat
 */

/**
 * NewScan Config - Lock File
 */
int msg_newscan::new_lockSet ( int onoff )
{

    std::string path = LOCKPATH;
    path += "mnewscan.lck";

    if ( !onoff )
    {
        remove ( ( char * ) path.c_str() );
        return TRUE;
    }

    //While lock file missing, loop untill it disapears.
    FILE *stream;

    while ( 1 )
    {
        stream = fopen ( path.c_str(),"rb+" );

        if ( stream == NULL )  // Lock File Missing
        {
            stream = fopen ( path.c_str(), "wb" );

            if ( stream == NULL )
            {
                printf ( "Error mnewscan.lck!" );
                return FALSE;
            }
            else
            {
                fclose ( stream );
                return TRUE;    // Created Lock File
            }
        }

        fclose ( stream );
        usleep ( 10*20000 );
    }
}

/**
 * NewScan Config - Read
 */
int msg_newscan::new_read ( NewScan *ns, int idx, char *mbase )
{

    char path[255];
    sprintf ( path,"%s%s.ns", MESGPATH, mbase );
    new_lockSet ( TRUE );

    int x = 0;
    FILE *stream;

    stream = fopen ( path,"rb+" );

    if ( stream == NULL )
    {
        new_lockSet ( FALSE );
        return ( 0 );
    }
    else fclose ( stream );

    stream = fopen ( path, "rb" );

    if ( fseek ( stream, ( int ) idx*sizeof ( NewScan ),SEEK_SET ) ==0 )
        x = fread ( ns,sizeof ( NewScan ),1,stream );

    fclose ( stream );
    new_lockSet ( FALSE );
    return ( x );
}

/**
 * NewScan Config - Write
 */
int msg_newscan::new_write ( NewScan *ns, int idx, char *mbase )
{

    char path[255];
    sprintf ( path,"%s%s.ns", MESGPATH, mbase );
    new_lockSet ( TRUE );

    FILE *stream;
    int x = 0;

    stream=fopen ( path,"rb+" );

    if ( stream == NULL )
    {
        stream=fopen ( path, "wb" );

        if ( stream == NULL )
        {
            write ( 0,path,sizeof ( path ) );
            new_lockSet ( FALSE );
            return 0;
        }
    }

    if ( fseek ( stream, ( int ) idx*sizeof ( NewScan ),SEEK_SET ) ==0 )
        x = fwrite ( ns,sizeof ( NewScan ),1,stream );

    fclose ( stream );
    new_lockSet ( FALSE );
    return ( x );
}

/**
 * NewScan Config - Count
 */
int msg_newscan::new_count ( char *mbase )
{

    int i = 0;
    NewScan ns;

    while ( new_read ( &ns,i,mbase ) )
    {
        ++i;
    }

    if ( i < 1 )    i = -1;
    else i--;

    return ( i );
}

/**
 * NewScan Config - read area
 */
int msg_newscan::read_mbase ( mb_list_rec *mr, int rec )
{

    int x = 0;
    std::string path = DATAPATH;
    path += "forums.dat";

    FILE *fptr = fopen ( path.c_str(),"rb+" );

    if ( fptr == NULL )
    {
        fptr = fopen ( path.c_str(), "wb" );

        if ( fptr == NULL )
        {
            printf ( "Error forum_read!" );
            return x;
        }
    }

    if ( fseek ( fptr, ( int ) rec*sizeof ( mb_list_rec ),SEEK_SET ) ==0 )
        x=fread ( mr,sizeof ( mb_list_rec ),1,fptr );

    fclose ( fptr );
    return ( x );
}

/**
 * NewScan Config - get count of new.
 */
int msg_newscan::check_mbase ( UserRec *thisuser )
{

    mb_list_rec mr;
    NewScan ns;
    int i;

    read_mbase ( &mr,thisuser->lastmbarea );
    i = new_count ( ( char * ) mr.mbfile );

    if ( i > ( signed ) thisuser->idx ) return TRUE;

    new_read ( &ns, thisuser->idx, ( char * ) mr.mbfile );

    if ( ns.set == TRUE ) return TRUE;

    return FALSE;
}
