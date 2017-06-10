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

# ifndef DATA_AREAS_H
# define DATA_AREAS_H

# include "struct.h"
# include "conio.h"

# include <string>

using namespace std;

typedef struct data_area : public ConsoleIO {

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

    data_area(UserRec *usr) {
        dataSaved = FALSE;
        thisuser = usr;
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

    void insert_cmdr();
    int  data_area_readin();
    void data_area_clear();
    void process_data_area();
    void parsemci(DataRec *cmdr);
    void parseinput(DataRec *cmdr);

    unsigned long
    count_file_records();

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
