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

// Enthral SVN: $Id: data_areas.h 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/data_areas.h $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $


# ifndef DATA_AREAS_H
# define DATA_AREAS_H

# include "struct.h"
# include "conio.h"

# include <string>

using namespace std;

typedef struct data_area : public SESSION
{

public:
    UserRec      *thisuser;
    DataArea     *datar2;
    DataRec      *cmdr2;

    int           Data_Alive;    // Data Area is Active!
    int           noc;           // Number of Commands in Loaded Menu
    short        *inexecnum;
    std::string  *inArray;       // Input String Array.
    std::string  *fileArray;     // File Array to Hold all Records
    std::string  *inputArray;    // For Truncating.
    int           inNoc;         // Input Number of Commands
    long          numFileRecs;   // Number of Recs in Data File.
    int           dataSaved;     // If Data was Saved.

    char _curarea[20];           // Current Area Name

    data_area(UserRec *usr)
    {
        dataSaved = FALSE;
        thisuser = usr;
        // Pass to CONIO for Pipe2Ansi MCI Codes.
        // For User MCI Data.

        datar2    = 0;
        cmdr2     = 0;
        inexecnum = 0;
        inArray   = 0;

        start_session(thisuser);
    }

    // Lock File so Nodes don't Collide.
    int  data_lockSet(int onoff);
    // New Menu Text File Parsing
    void chkparse(std::string &temp);

    int  da_parse(std::string cfgdata);
    int  da_read(char *FileName);

    void rec_parse(std::string cfgdata, int idx);
    int  rec_exist(char *FileName, int idx);
    int  rec_cnt(char  *FileName);
    int  rec_read(char *FileName, int idx);

    // Read in Current Data File
    // Hold Commands Per Menu being Read in.
    void insert_cmdr();
    int  data_area_readin();
    void data_area_clear();
    // Menu System Loop.
    // Processing for Menu's with Lightbars & Hotkeys.
    void process_data_area();
    // Parse Menu Keys from Menu Selection
    void parsemci(DataRec *cmdr);
    void parseinput(DataRec *cmdr);
    // Write Output of Buffer.
    unsigned long
    count_file_records();
    // Process Output Data File
    void create_ansifile();
    void read_file_records();
    void truncate_da_buffer();
    void save_da_buffer();
    int  confirm_da_save(DataRec *cmdr);
    void darea_docmd(DataRec *cmdr);

    void display_commands();
    void data_loop();


} data_area;

# endif
