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

// Enthral SVN: $Id: configbbs.cpp 6 2014-04-02 03:42:27Z merc $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/configbbs.cpp $
// $LastChangedDate: 2014-04-01 22:42:27 -0500 (Tue, 01 Apr 2014) $
// $LastChangedRevision: 6 $
// $LastChangedBy: merc $

# include "struct.h"
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

    std::string path = INIPATH;
    path += "config.ini";

    FILE *stream;
    stream = fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        perror(" ini ***cannot open config.ini, check if it exists and permissions!");
        return false;
    }
    fclose(stream);
    return true;
}


/**
 * Create Defaut Config if Doesn't exist
 */
void createconfig()
{

    std::string name = INIPATH;
    name += "config.ini";

    ofstream outStream2;
    outStream2.open( name.c_str(), ofstream::out | ofstream::trunc );
    if (!outStream2.is_open())
    {
        perror(" ini ***cannot create config.ini, check permissions!");
        exit(1);
        return;
    }

    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | " << BBSVERSION << "        |" << endl;
    outStream2 << "# | Main System Configuration File                                 |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "#" << endl;
	outStream2 << "# Sysop Operator Handle, for Full Sysop Access" << endl;
    outStream2 << "Set SYSOP_NAME ""MySysopHandle"" " << endl;
	outStream2 << "#" << endl;
	outStream2 << "# BBS System Name" << endl;
    outStream2 << "Set SYSTEM_NAME ""MyBbsName"" " << endl;
	outStream2 << "#" << endl;
	outStream2 << "# Path for Node Piped Files and Drop files from Telnetd" << endl; 
	outStream2 << "Set TEMP_PATH    ""/tmp/enthral"" " << endl;
	outStream2 << "#" << endl;
	outStream2 << "# MAX Nodes for Logins" << endl; 
	outStream2 << "Set MAX_NODES    ""5"" " << endl;
	outStream2 << "#" << endl;
	outStream2 << "# MAX Nodes for Logins" << endl; 
	outStream2 << "Set STARTUP_SCRIPT  ""startup.py"" " << endl;
	outStream2 << "#" << endl;
	outStream2 << "# MAX Nodes for Logins" << endl; 
	outStream2 << "Set STARTUP_SCRIPT2  ""startup2.py"" " << endl;


    outStream2.close();
    return;
}

/**
 * Parse Value pulled from Config
 */
void checkcfg(std::string cfgdata)
{

    std::string temp = cfgdata;
    std::string::size_type id1 = 0;

    std::string::size_type st1 = 0;
    std::string::size_type st2 = 0;
    std::string::size_type  ct = 0;

    // Disgards any Config lines with the # Character
    if (temp[0] == '#') return;

    // Sets if LOGGING is on / off
    id1 = 0;
    id1 = temp.find("Set SYSOP_NAME", 0);
    if (id1 != std::string::npos)
    {
        std::string temp1;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        sprintf(SYSOP_NAME,"%s",(char *)temp1.c_str());
		return;
    }

	id1 = 0;
    id1 = temp.find("Set SYSTEM_NAME", 0);
    if (id1 != std::string::npos)
    {
        std::string temp1;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        sprintf(SYSTEM_NAME,"%s",(char *)temp1.c_str());
		return;
    }

	id1 = 0;
    id1 = temp.find("Set TEMP_PATH", 0);
    if (id1 != std::string::npos)
    {
        std::string temp1;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        sprintf(ENTHRALTMP,"%s",(char *)temp1.c_str());
		return;
    }

	id1 = 0;
    id1 = temp.find("Set MAX_NODES", 0);
    if (id1 != std::string::npos)
    {
        std::string temp1;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
		// String to Int

		
        istringstream ( temp1 ) >> MAX_NODES; 
		return;
    }

	id1 = 0;
    id1 = temp.find("Set STARTUP_SCRIPT ", 0);
    if (id1 != std::string::npos)
    {
        std::string temp1;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
		
        sprintf(STARTUP_SCRIPT,"%s",(char *)temp1.c_str());
		return;
    }

	id1 = 0;
    id1 = temp.find("Set STARTUP_SCRIPT2 ", 0);
    if (id1 != std::string::npos)
    {
        std::string temp1;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
		
        sprintf(STARTUP_SCRIPT2,"%s",(char *)temp1.c_str());
		return;
    }

	
}

/**
 * Parse Config File
 */
void parseconfig()
{

    std::string name = INIPATH;
    name += "config.ini";

    ifstream inStream;
    inStream.open( name.c_str() );
    if (!inStream.is_open())
    {
        perror(" ini ***cannot parse config.ini, check if it exists and permissions!");
        exit(1);
    }

    std::string cfgdata;
    for (;;)
    {
        getline(inStream,cfgdata);
        checkcfg(cfgdata);
        if(inStream.eof()) break;
    }
    inStream.close();
    return;
}

