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

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

# ifndef FILES_H
# define FILES_H

# include "struct.h"

# include <string>

typedef struct files
{

    // Import Functions
    int  fileidx_lockSet ( int onoff );

    // User Index File for Faster Read / Writes
    int  fileidx_writet ( FileIdx *fil, char *areaname, int idx );
    int  fileidx_write ( FileIdx *fil, char *areaname, int idx );
    int  fileidx_read ( FileIdx *fil, char *areaname, int idx );
    int  fileidx_count ( char *areaname );
    int  fileidx_find ( char *name );
    bool fileidx_match ( char *name );
    void fileidx_display();
    void fileidx_new ( char *filename, unsigned long size, char *areaname, int idx );

    // Inpot File Functions.
    int import_filedir();


    // Files.Dat area listing.
    int  read_fbaselist ( fb_list_rec *fr, int recno );
    int  save_fbasetemp ( fb_list_rec *fr, int recno );
    int  save_fbaselist ( fb_list_rec *fr, int recno );
    int  file_count();
    void save_filebase ( fb_list_rec *fb );
    int  file_find ( char *tfile );

    int  jlr_lockSet ( int onoff );
    int  readlr ( LastRead *lr, int idx, fb_list_rec *fb );
    int  writelr ( LastRead *lr, int idx, fb_list_rec *fb );

    /*
    unsigned long
    JamAreaGetLast(long usernum, fb_list_rec *fb);
    void JamAreaSetLast(long usernum, long filenum, fb_list_rec *fb);
    */
    /* Useless, Can't determine deleted messages!
    int  readidx(JamIndex *dx, int idx, mb_list_rec *mb);
    int  countidx(mb_list_rec *mb);
    */

    unsigned long
    CountFiles ( unsigned long fbnum ),
               CountNewFiles ( unsigned long fbnum, UserRec *usr ),
               CountAllNewFiles ( UserRec *usr );

    void resetlastread ( UserRec *usr );

    void ParseFArea ( UserRec *usr, std::string &tmp );
    void FileAreaScan ( int newscan );

    //void filearea_list(UserRec *usr);

} files;

# endif
