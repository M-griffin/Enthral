/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Some Basic File I/O Fuctions                                 *
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
//# include "msgapi.h"
# include "files.h"
# include "conio.h"
# include "language.h"
# include "file_list.h"

# include <cstdio>
# include <cstring>     // gcc 4.3
# include <cstdlib>     // gcc 4.3
//# include <cctype>

# include <string>


using namespace std;


/*
int files::fileidx_lockSet(int onoff)
{

    std::string path = LOCKPATH;
    path += "fileidx.lck";

    if (!onoff)
    {
        remove((char *)path.c_str());
        return TRUE;
    }

    //While lock file missing, loop untill it disapears.
    FILE *stream;
    while(1)
    {
        stream = fopen(path.c_str(),"rb+");
        if(stream == NULL)   // Lock File Missing
        {
            stream = fopen(path.c_str(), "wb");
            if(stream == NULL)
            {
                printf("Error fileidx.lck!");
                return FALSE;
            }
            else
            {
                fclose(stream);
                return TRUE;    // Created Lock File
            }
        }
        fclose(stream);
        usleep(10*20000);
    }
}


int files::fileidx_writet(FileIdx *fil, char *areaname, int idx)
{

    std::string path = FILEPATH;
    path += areaname;
    path += "fileidx.tmp";

    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL)
        {
            printf("Error fileidx_write temp!");
            return x;
        }
    }
    if(fseek(stream,(int)idx*sizeof(FileIdx),SEEK_SET)==0)
        x = fwrite(fil,sizeof(FileIdx),1,stream);
    fclose(stream);
    return x;
}

int files::fileidx_write(FileIdx *fil,  char *areaname, int idx)
{

    std::string path = FILEPATH;
    path += areaname;
    path += "fileidx.dat";

    fileidx_lockSet(TRUE);
    int x = 0;
    FILE *stream=fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        stream=fopen(path.c_str(), "wb");
        if(stream == NULL)
        {
            printf("Error fileidx_write!");
            fileidx_lockSet(FALSE);
            return x;
        }
    }
    if(fseek(stream,(int)idx*sizeof(FileIdx),SEEK_SET)==0)
        x = fwrite(fil,sizeof(FileIdx),1,stream);
    fclose(stream);
    fileidx_lockSet(FALSE);
    return x;
}

int files::fileidx_read(FileIdx *fil, char *areaname, int idx)
{

    std::string path = FILEPATH;
    path += areaname;
    path += "fileidx.dat";

    fileidx_lockSet(TRUE);
    int x = 0;
    FILE *stream=fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        stream=fopen(path.c_str(), "wb");
        if(stream == NULL)
        {
            printf("Error fileidx_read!");
            fileidx_lockSet(FALSE);
            return x;
        }
    }
    fclose(stream);

    stream = fopen(path.c_str(), "rb");
    if(fseek(stream,(int)idx*sizeof(FileIdx),SEEK_SET)==0)
        x = fread(fil,sizeof(FileIdx),1,stream);
    fclose(stream);
    fileidx_lockSet(FALSE);
    return x;
}

int files::fileidx_count(char *areaname)
{

    FileIdx fil;
    int i = 0;
    while(fileidx_read(&fil,areaname,i))
    {
        ++i;
    }
    (i < 1) ? i = -1 : i--;
    return(i);
}

int files::fileidx_find(char *name)
{

    fb_list_rec fr;

    CURRENT_FAREA = 1;
    read_fbaselist( &fr , CURRENT_FAREA);

    FileIdx fil;
    int idx = 0;
    std::string temp1, temp2;

    temp1 = (name);
    //move temp1 to lower case for testing!
    for (std::string::size_type i = 0; i < temp1.size(); ++i )
    {
        temp1[i] = tolower( temp1[i] );
    }

    while(fileidx_read(&fil, (char *)fr.fbfile, idx))
    {
        temp2 = (char *)(fil.filename);
        //  transform(temp1.begin(),temp1.end(), temp1.begin(), tolower);
        //  transform(temp2.begin(),temp2.end(), temp2.begin(), tolower);
        for (std::string::size_type i = 0; i < temp1.size(); ++i )
        {
            temp2[i] = tolower( temp2[i] );
        }
        return(idx);
        idx++;
    }
    return(-1);

}


bool files::fileidx_match(char *name)
{

    int index = -1;

    index = fileidx_find(name);
    if (index == -1) return false;
    else return true;
}



// ---------------------------------------------------------

// Read in Files From Import Directory.

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>


int main(int argc, char *argv[])
{



// Display files in current Area!
void files::fileidx_display()
{

    fb_list_rec fr;
    CURRENT_FAREA = 1;
    read_fbaselist( &fr , CURRENT_FAREA);
    std::string qbuf;

    FileIdx fil;
    int i = 0;
    std::string list = "";
    char fname[1024] = {0};
    char fs[1024] = {0};
    std::string fsize;


    // Build File Listing.
    while(fileidx_read(&fil, (char *)fr.fbfile, i++))
    {

     // fix this!!
                if (fil.size < 1000) {
                    sprintf(fs,"%+4ld B.",fil.size);
                    fsize = fs;
                }
                else
                if (fil.size < 1000*1000) {
                    sprintf(fs,"%+4ld KB",fil.size / (1000));
                    fsize = fs;
                }
                else
                if (fil.size < (1000*1000*1000)) {
                    sprintf(fs,"%+4ld MB", fil.size / (1000*1000));
                    fsize = fs;
                }
                else
                if (fil.size < (1000*1000*1000*1000)) {
                    sprintf(fs,"%+4ld GB",fil.size / (1000*1000*1000));
                    fsize = fs;
                }

                sprintf(fname,"%-7s %s", (char *)fsize.c_str() , fil.filename);
                list += fname;
                list += "\r";

    }

    msg_readll listing;
    listing.PutBuffer((char *)list.c_str());

    filelist _fi;
    _fi.setup_filelist();
    _fi.Startfilelist(listing.head,qbuf);

    // process any flagged files!
}

void files::fileidx_new(char *filename, unsigned long size, char *areaname, int idx)
{

    FileIdx fil;
    memset(&fil,0,sizeof(FileIdx));

    strcpy((char *)fil.filename, filename);
    fil.size = size;
    fil.idx = 0;
    fileidx_write(&fil, areaname, idx);

}

int files::import_filedir()
{


    CURRENT_FAREA = area were trying to import from!

    SESSION s;
    fb_list_rec fr;

    CURRENT_FAREA = 1;
    read_fbaselist( &fr , CURRENT_FAREA);

    DIR             *dip;
    struct dirent   *dit;

    FILE *fp;
    int             i = 0;
    unsigned char c;
    int             idx = 0;
    int             autoimport=FALSE;
    bool            file_exists = false;
    unsigned long   filesize;

    std::string readfile;
    std::string filepath = (char *)fr.filepath;

    s.pipe2ansi((char *)"Importing File Area: ");
    s.pipe2ansi((char *)fr.fbfile);
    //s.elog2("\npath, area: %s, %i",filepath.c_str(), CURRENT_FAREA);
    dip = opendir(filepath.c_str());
    if (dip == NULL)
    {
        //s.elog2("can't open dir!!");
        return FALSE;
    }

    while ((dit = readdir(dip)) != NULL)
    {

        if ((strcmp(dit->d_name,".") != 0) && (strcmp(dit->d_name,"..") != 0))
        {
            i++;
            //s.elog2("\nRead Dirs: %s", dit->d_name);

            // Skip any files we already have!
            file_exists = fileidx_match(dit->d_name);

            if (!file_exists)
            {
                if (autoimport == FALSE )
                {
                    s.pipe2ansi((char *)"|CR");
                    s.pipe2ansi(dit->d_name);
                    s.pipe2ansi((char *)"|CR- Import File? [y/n/q/a]: ");
                    c = s.onekey((char *)"YNynQqAa");
                }
                if (c == 'A' || c == 'a')
                {
                    c = 'Y';
                    // First Read file to Get filesize, then add to listing.
                    readfile = (char *)fr.filepath;
                    readfile += "/";
                    readfile += dit->d_name;
                    fp = fopen(readfile.c_str(),"rb");
                    if (fp != NULL)
                    {
                        fseek(fp, 0L, SEEK_END);
                        filesize = ftell(fp);
                        fclose(fp);
                    }

                    idx = fileidx_count((char *)fr.fbfile);
                    fileidx_new(dit->d_name, filesize, (char *)fr.fbfile, idx);
                    autoimport = TRUE;
                }
                else if (c == 'Q' || c == 'q') break;
                else if (c == 'Q' || c == 'q') break;
                else if (c == 'Y' || c == 'y')
                {
                    // First Read file to Get filesize, then add to listing.
                    readfile = (char *)fr.filepath;
                    readfile += "/";
                    readfile += dit->d_name;
                    fp = fopen(readfile.c_str(),"rb");
                    if (fp != NULL)
                    {
                        fseek(fp, 0L, SEEK_END);
                        filesize = ftell(fp);
                        fclose(fp);
                    }

                    idx = fileidx_count((char *)fr.fbfile);
                    fileidx_new(dit->d_name, filesize, (char *)fr.fbfile, idx);
                }
                else if (c == 'N' || c == 'n') continue;
            }
        }
    }
    //s.elog2("\n\nreaddir() found a total of %i files\n", i);

    if (closedir(dip) == -1)
    {
        return FALSE;
    }

    return TRUE;
}


// -----------
int files::read_fbaselist(fb_list_rec *fr, int rec)
{

    int x = 0;
    std::string path = DATAPATH;

    // Rigged Systop Exclusion.

    if (isSysop == TRUE) {
        path += "files.dat";
    }
    else {
        path += "files2.dat";
    }
    path += "files.dat";

    FILE *fptr = fopen(path.c_str(),"rb+");
    if(fptr == NULL)
    {
        fptr = fopen(path.c_str(), "wb");
        if(fptr == NULL)
        {
            printf("Error file_read!");
            return x;
        }
    }

    if(fseek(fptr,(int)rec*sizeof(fb_list_rec),SEEK_SET)==0)
        x=fread(fr,sizeof(fb_list_rec),1,fptr);

    fclose(fptr);
    return(x);
}

int files::save_fbasetemp(fb_list_rec *fr, int rec)
{

    FILE *fptr;
    int x = 0;

    std::string path = DATAPATH;

    // Rigged Systop Exclusion.

    if (isSysop == TRUE) {
        path += "forums.tmp";
    }
    else {
        path += "forums2.tmp";
    }
    path += "files.tmp";

    fptr=fopen(path.c_str(),"rb+");
    if(fptr==NULL)
    {
        fptr=fopen(path.c_str(),"wb");
        if(fptr==NULL)
        {
            printf("Error creating files.tmp");
            return x;
        }
    }

    if(fseek(fptr,(int)rec*sizeof(fb_list_rec),SEEK_SET)==0)
        x=fwrite(fr,sizeof(fb_list_rec),1,fptr);

    fclose(fptr);
    return(x);
}


int files::save_fbaselist(fb_list_rec *fr, int rec)
{

    std::string path = DATAPATH;

    // Rigged Systop Exclusion.

    if (isSysop == TRUE) {
        path += "forums.dat";
    }
    else {
        path += "forums2.dat";
    }
    path += "files.dat";

    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL)
        {
            printf("Error forum_write!");
            return x;
        }
    }
    if(fseek(stream,(int)rec*sizeof(fb_list_rec),SEEK_SET)==0)
        x = fwrite(fr,sizeof(fb_list_rec),1,stream);
    fclose(stream);
    return x;
}

int files::file_count()
{

    int i = 0;
    fb_list_rec fb;

    while(read_fbaselist(&fb,i++));
    if(i < 1)   i = -1;
    else i--;
    return(i);
}

void files::save_filebase(fb_list_rec *fb)
{

    struct _minf m;
    HAREA harea;
    HMSG hfile;

    char path[255];
    sprintf(path,"%s%s", fb->fbpath, fb->fbfile);

    m.req_version = 0;
    m.def_zone = 1;


    if(MsgOpenApi(&m) == 0 ){
        harea = MsgOpenArea((byte *)path, MSGAREA_CRIFNEC, fb->Type | MSGTYPE_ECHO);
        if(harea != NULL){
            hfile = MsgOpenMsg(harea, MOPEN_CREATE,0);
            if(hfile != NULL) MsgCloseMsg(hfile);
            else printf("\r\nUnable to create file area.");
            MsgCloseArea(harea);
        }
        else printf("\r\nUnable to open %s", path);
        MsgCloseApi();
    }
}

int files::file_find(char *tfile)
{

    string temp1, temp2;
    fb_list_rec fb;
    temp1 = (tfile);

    int idx = 0;
    while(read_fbaselist(&fb,idx))
    {
        temp2 = (char *)(fb.fbfile);
        if(temp1 == temp2) return(idx);
        idx++;
    }
    return(-1);
}

int files::jlr_lockSet(int onoff)
{

    std::string path = LOCKPATH;
    path += "ljr.lck";

    if (!onoff)
    {
        remove((char *)path.c_str());
        return TRUE;
    }

    //While lock file missing, loop untill it disapears.
    FILE *stream;
    while(1)
    {
        stream = fopen(path.c_str(),"rb+");
        if(stream == NULL)     // Lock File Missing
        {
            stream = fopen(path.c_str(), "wb");
            if(stream == NULL)
            {
                printf("Error jlr.lck!");
                return FALSE;
            }
            else
            {
                fclose(stream);
                return TRUE;    // Created Lock File
            }
        }
        fclose(stream);
        usleep(10*20000);
    }
}

int files::readlr(LastRead *lr, int idx, fb_list_rec *fb)
{

    char path[255];
    sprintf(path,"%s%s.jlr", fb->fbpath, fb->fbfile);
    jlr_lockSet(TRUE);

    int x = 0;
    FILE *stream;

    stream = fopen(path,"rb+");
    if(stream == NULL)
    {
        jlr_lockSet(FALSE);
        return (0);
    }
    else fclose(stream);

    stream = fopen(path, "rb");
    if(fseek(stream,(int)idx*sizeof(LastRead),SEEK_SET)==0)
        x = fread(lr,sizeof(LastRead),1,stream);
    fclose(stream);
    jlr_lockSet(FALSE);
    return (x);
}


int files::writelr(LastRead *lr, int idx, fb_list_rec *fb)
{

    char path[255];
    sprintf(path,"%s%s.jlr", fb->fbpath, fb->fbfile);
    jlr_lockSet(TRUE);

    FILE *stream;
    int x = 0;

    stream=fopen(path,"rb+");
    if(stream == NULL)
    {
        stream=fopen((char *)path, "wb");
        if(stream == NULL)
        {
            x = write(0,path,sizeof(path));
            jlr_lockSet(FALSE);
            sleep(3);
            return 0;
        }
    }
    if(fseek(stream,(int)idx*sizeof(LastRead),SEEK_SET)==0)
        x = fwrite(lr,sizeof(LastRead),1,stream);
    fclose(stream);
    jlr_lockSet(FALSE);
    return (x);
}

unsigned
long files::JamAreaGetLast(long usernum, fb_list_rec *fb)
{

    LastRead lr;
    memset(&lr,0,sizeof(LastRead));
    readlr(&lr,usernum,fb);

    //return (lr.LastReadMsg);
}

void files::JamAreaSetLast(long usernum, long filenum, fb_list_rec *fb)
{

    LastRead lr;
    memset(&lr,0,sizeof(LastRead));
    readlr(&lr,usernum,fb);
//   lr.LastReadMsg = filenum;
    writelr(&lr,usernum,fb);
}

// Counts all Active Mesages excluding Purged!!
unsigned long files::CountFiles(unsigned long fbnum)
{

    MSGA         *AHandle = NULL;
    fb_list_rec   fr;
    char path[81] = {0};
    unsigned long totfile = 0;

    if(!read_fbaselist(&fr,fbnum))
    {
        return 0;
    }
    sprintf(path,"%s%s",fr.fbpath, fr.fbfile);

    if (AHandle != NULL)
    {
//        MsgCloseArea(AHandle);
    }

//   AHandle = MsgOpenArea((byte *)path, MSGAREA_CRIFNEC, fr.Type);
    if (AHandle == NULL)
    {
        totfile = 0;
    }

    totfile = MsgHighMsg(AHandle);
    --totfile;
    if(AHandle != NULL)
    {
//       MsgCloseArea(AHandle);
        AHandle = NULL;
    }
    AHandle = NULL;
    return totfile;
}

// Counts Active New Mesages
unsigned long files::CountNewFiles(unsigned long fbnum, UserRec *usr)
{

    SESSION       s(usr); // Pass User Incase there are MCI Codes for User Info.
    fb_list_rec fr;
    read_fbaselist(&fr,fbnum);

    unsigned long cnt = CountFiles(fbnum);
    if (cnt == -1) cnt = 0;
    else ++cnt;

    unsigned long lr = JamAreaGetLast(usr->idx,&fr);

    //s.elog ("1. Messages Scan %s,Total #%ld, LR #%ld",mr.mbdisplay,cnt,lr);

    if (lr == -1)   lr = 0;
    else if (lr > cnt )  lr = cnt;
    else if (lr != 0)  --lr;
    if (cnt == 0)   lr = 0;

    //s.elog ("2. Messages Scan %s,Total #%ld, LR #%ld",mr.mbdisplay,cnt,lr);
    lr = cnt-lr;
    //s.elog ("3. Messages for %s, Total #%ld, LR #%ld",mr.mbdisplay,cnt,lr);
    return lr;
}

// Counts all Active New Mesages for All Total new waiting per user.
unsigned long files::CountAllNewFiles(UserRec *usr)
{

    unsigned long i = 0, cnt = 0;
    fb_list_rec fb;
    SESSION       s(usr); // Pass User Incase there are MCI Codes for User Info.

    while(read_fbaselist(&fb,i))
    {
        cnt += CountNewFiles(i,usr);
        //s.elog ("New Messages for %s, #%ld",usr->handle,cnt);
        ++i;
    }
    return(cnt);
}

// New User, Reset all Last Reads.
void files::resetlastread(UserRec *usr)
{

    unsigned long i = 0;
    fb_list_rec fb;

    while(read_fbaselist(&fb,i))
    {
        JamAreaSetLast(usr->idx, 0, &fb);
        ++i;
    }
}

// Message Area Listing - Add Later into Normal MCI Code Parsing!!!
void files::ParseFArea(UserRec *usr, std::string &temp)
{

    fb_list_rec  fb;     // Message Base # and Name
    SESSION      s(usr); // Pass User Incase there are MCI Codes for User Info.

    int c = 0;
    //std::string temp = "";
    std::string path  = "";
    std::string temp3 = "";
    temp.erase();


    FILE *inStream;
    int  idx = 1;
    int  cnt = 0;
    int  i   = 0;
    int  basenum = 0;
    long lr  = 0;

    char MCI[3]= {0};
    char temp2[100]= {0};
    int  space  = 0;
    int  foundr = FALSE;
    int  foundl = FALSE;
    int  reset  = FALSE;
    int  dont   = FALSE;

    std::string ans  = "";
    std::string ans1 = "";
    std::string ans2 = "";
    std::string ans3 = "";
    std::string ans4 = "";

    // Prefetch ansi's
    path = ANSIPATH;
    path += "fareamid1.ans";
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL)
    {
        return;
    }
    while (c != EOF)
    {
        c = getc(inStream);
        if (c != EOF) ans1 += c;
    }
    fclose(inStream);
    c = '\0';
    path = ANSIPATH;
    path += "fareamid2.ans";
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL)
    {
        return;
    }
    while (c != EOF)
    {
        c = getc(inStream);
        if (c != EOF) ans2 += c;
    }
    fclose(inStream);
    c = '\0';
    path = ANSIPATH;
    path += "fareamid3.ans";
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL)
    {
        return;
    }
    while (c != EOF)
    {
        c = getc(inStream);
        if (c != EOF) ans3 += c;
    }
    fclose(inStream);
    c = '\0';
    path = ANSIPATH;
    path += "fareamid4.ans";
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL)
    {
        return;
    }
    while (c != EOF)
    {
        c = getc(inStream);
        if (c != EOF) ans4 += c;
    }
    fclose(inStream);

    basenum = file_count();
    while(read_fbaselist(&fb,idx++))
    {

        //sprintf(mb.OriginLine,"|08h|04a|12u|14nting the cha|12p|04e|08l |07+EViL+");
        //save_fbaselist(&mb,idx-1);
        reset = FALSE;
        temp3 = "";

        if (CURRENT_FAREA == idx-1)
            ans = ans2; // Display HighLight None.
        else
            ans = ans1; // Dispaly Normal None

        // Used to display Areas with new message as #3
        if (dont)
        {
RESTART:
            temp3.erase();
            if (CURRENT_FAREA == idx-1)
                ans = ans4; // Dispaly HighLight New
            else
                ans = ans3; // Dispaly Normal New
        }

        i = 0;
        c = 0;
        do
        {
            memset(&MCI,0,sizeof(MCI));
            c = ans[i];
            if (c == '\0') break;

            // Check for Spacing MCI Code
            switch (c)
            {
            case '{' : // Left Justify
                MCI[0] = ans[++i];
                MCI[1] = ans[++i];
                space = atoi(MCI);

                if (space != 0)
                {
                    foundl = TRUE;
                }
                else
                {
                    temp3 += c;
                    temp3 += MCI;
                }
                break;

            case '}' : // Right Justify
                MCI[0] = ans[++i];
                MCI[1] = ans[++i];
                space = atoi(MCI);

                if (space != 0)
                {
                    foundr = TRUE;
                }
                else
                {
                    temp3 += c;
                    temp3 += MCI;
                }
                break;

            case '|' : // Pipe Codes
                MCI[0] = ans[++i];
                MCI[1] = ans[++i];
                if (strcmp(MCI,"F#") == 0)
                {
                    sprintf(temp2,"%i",idx-1);
                    if (foundl)
                    {
                        s.lspacing(temp2,space);
                        foundl = FALSE;
                    }
                    else if (foundr)
                    {
                        s.rspacing(temp2,space);
                        foundr = FALSE;
                    }
                    temp3 += temp2;
                }
                else if (strcmp(MCI,"FA") == 0)
                {
                    sprintf(temp2,"%s",fb.fbdisplay);
                    if (foundl)
                    {
                        s.lspacing(temp2,space);
                        foundl = FALSE;
                    }
                    else if (foundr)
                    {
                        s.rspacing(temp2,space);
                        foundr = FALSE;
                    }
                    temp3 += temp2;
                }
                else if (strcmp(MCI,"FT") == 0)
                {
                    cnt = CountFiles(idx-1);
                    if (cnt == -1) cnt = 0;
                    else ++cnt;
                    sprintf(temp2,"%i",cnt);
                    if (foundl)
                    {
                        s.lspacing(temp2,space);
                        foundl = FALSE;
                    }
                    else if (foundr)
                    {
                        s.rspacing(temp2,space);
                        foundr = FALSE;
                    }
                    temp3 += temp2;
                }
                else if (strcmp(MCI,"FN") == 0)
                {
                    // Count how many unread messages
                    cnt = CountFiles(idx-1);
                    if (cnt == -1) cnt = 0;
                    else ++cnt;

                    lr = JamAreaGetLast(usr->idx,&fb);
                    if (lr == -1)   lr = 0;
                    if (lr > cnt )  lr = cnt;
                    else if (lr != 0) --lr;
                    if (cnt == 0)   lr = 0;

                    // Re-Process and Reset to Correct Mid Ansi
                    if (cnt-lr >= 1 && reset == FALSE && (ans != ans2 || ans != ans4))
                    {
                        reset = TRUE;
                        goto RESTART;
                    }

                    sprintf(temp2,"%ld",cnt-lr);
                    if (foundl)
                    {
                        s.lspacing(temp2,space);
                        foundl = FALSE;
                    }
                    else if (foundr)
                    {
                        s.rspacing(temp2,space);
                        foundr = FALSE;
                    }
                    temp3 += temp2;
                }
                else
                {
                    temp3 += c;
                    temp3 += MCI;
                }
                break;

            case '\n' :
                temp3 += '\r';
                break;

            default :
                temp3 += c;
                break;
            }
            ++i;
        }
        while (c != '\0');

        temp3 += "\r";
        temp += temp3;
    }
}

*/
