/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Config INI I/O                                               *
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

# include "config.h"

# include <stdio.h>
# include <string>
# include <fstream>
# include <sstream>

# include <cstring> // gcc 4.3
# include <cstdlib> // gcc 4.3

using namespace std;


/**
 * Check if Config Data Exists
 */
bool configdataexists()
{

    std::string path = PATH;
    path += "config.ini";

    FILE *stream;
    stream = fopen ( path.c_str(),"rb+" );

    if ( stream == NULL )
    {
        perror ( " ini ***cannot open config.ini, check if it exists and permissions!" );
        return false;
    }

    fclose ( stream );
    return true;
}


/**
 * Create Defaut Config if Doesn't exist
 */
void createconfig()
{

    std::string name = PATH;
    name += "config.ini";

    ofstream outStream2;
    outStream2.open ( name.c_str(), ofstream::out | ofstream::trunc );

    if ( !outStream2.is_open() )
    {
        perror ( " ini ***cannot create config.ini, check permissions!" );
        exit ( 1 );
        return;
    }

    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | Entelnetd Configuration File                                   |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "#" << endl;
    outStream2 << "# Use File Log along with Syslog - creates entelnetd.log file in program folder (T/F)" << endl;
    outStream2 << "Set USE_FILE_LOG \"T\" " << endl;
    outStream2 << "#" << endl;
    outStream2 << "# Use hosts.deny file to block unwanted IP and Hostname Connections. (T/F)" << endl;
    outStream2 << "Set USE_HOSTS_DENY \"T\" " << endl;
    outStream2 << "#" << endl;
    outStream2 << "# Block connections that don't resolve a hostname. (T/F)" << endl;
    outStream2 << "Set BLOCK_NO_HOSTNAME \"T\" " << endl;
    outStream2 << "#" << endl;
    outStream2 << "# Path to hosts.deny file" << endl;
    outStream2 << "Set HOSTS_DENY_PATH \"/home/merc/hosts.deny\" " << endl;
    outStream2 << "#" << endl;
    outStream2 << "# Path to Enthral Temp Folder for Drop Files to BBS" << endl;
    outStream2 << "Set ENTHRAL_TEMP_PATH \"/tmp/enthral/\" " << endl;

    outStream2.close();
    return;
}

/**
 * Parse Value pulled from Config
 */
void checkcfg ( std::string cfgdata )
{
    std::string temp = cfgdata;
    std::string::size_type id1 = 0;

    std::string::size_type st1 = 0;
    std::string::size_type st2 = 0;
    std::string::size_type  ct = 0;

    // Disgards any blank or Config lines with starting with the # Character
    if ( temp.size() == 0 || temp[0] == '#' ) return;

    // Sets if LOGGING is on / off
    id1 = 0;
    id1 = temp.find ( "Set USE_FILE_LOG", 0 );

    if ( id1 != std::string::npos )
    {
        std::string temp1;

        st1 = temp.find ( '"', 0 );
        st2 = temp.find ( '"', st1+1 );
        ++st1;
        temp1 = temp.substr ( st1,st2 );
        ct = st2 - st1;

        if ( temp1.length() > ct )
            temp1.erase ( ct,temp1.length() );

        USE_FILE_LOG = temp1[0];
        return;
    }

    id1 = 0;
    id1 = temp.find ( "Set USE_HOSTS_DENY", 0 );

    if ( id1 != std::string::npos )
    {
        std::string temp1;

        st1 = temp.find ( '"', 0 );
        st2 = temp.find ( '"', st1+1 );
        ++st1;
        temp1 = temp.substr ( st1,st2 );
        ct = st2 - st1;

        if ( temp1.length() > ct )
            temp1.erase ( ct,temp1.length() );

        USE_HOSTS_DENY = temp1[0];
        return;
    }

    id1 = 0;
    id1 = temp.find ( "Set BLOCK_NO_HOSTNAME", 0 );

    if ( id1 != std::string::npos )
    {
        std::string temp1;

        st1 = temp.find ( '"', 0 );
        st2 = temp.find ( '"', st1+1 );
        ++st1;
        temp1 = temp.substr ( st1,st2 );
        ct = st2 - st1;

        if ( temp1.length() > ct )
            temp1.erase ( ct,temp1.length() );

        BLOCK_NO_HOSTNAME = temp1[0];
        return;
    }

    id1 = 0;
    id1 = temp.find ( "Set HOSTS_DENY_PATH", 0 );

    if ( id1 != std::string::npos )
    {
        std::string temp1;

        st1 = temp.find ( '"', 0 );
        st2 = temp.find ( '"', st1+1 );
        ++st1;
        temp1 = temp.substr ( st1,st2 );
        ct = st2 - st1;

        if ( temp1.length() > ct )
            temp1.erase ( ct,temp1.length() );

        sprintf ( HOSTS_DENY_PATH,"%s", ( char * ) temp1.c_str() );
        return;
    }

    id1 = 0;
    id1 = temp.find ( "Set ENTHRAL_TEMP_PATH", 0 );

    if ( id1 != std::string::npos )
    {
        std::string temp1;

        st1 = temp.find ( '"', 0 );
        st2 = temp.find ( '"', st1+1 );
        ++st1;
        temp1 = temp.substr ( st1,st2 );
        ct = st2 - st1;

        if ( temp1.length() > ct )
            temp1.erase ( ct,temp1.length() );

        term_passing = temp1;
        return;
    }

}

/**
 * Parse Config File
 */
void parseconfig()
{

    std::string name = PATH;
    name += "config.ini";

    ifstream inStream;
    inStream.open ( name.c_str() );

    if ( !inStream.is_open() )
    {
        perror ( " ini ***cannot parse config.ini, check if it exists and permissions!" );
        exit ( 1 );
    }

    std::string cfgdata;

    for ( ;; )
    {
        getline ( inStream,cfgdata );
        checkcfg ( cfgdata );

        if ( inStream.eof() ) break;
    }

    inStream.close();
    return;
}
