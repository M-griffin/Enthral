/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Some Basic Message I/O Fuctions                              *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// Enthral SVN: $Id: msgs.cpp 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msgs.cpp $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# include "struct.h"
# include "mb_api.h"
# include "mb_jam.h"

# include "msgs.h"
# include "conio.h"
# include "language.h"
# include "msg_area.h"

# include <cstdio>
# include <cstring>  // gcc 4.3
# include <cstdlib>  // gcc 4.3
# include <unistd.h> // gcc 4.7

# include <string>

using namespace std;

/*
int msgs::read_emaillist(EmailIdx *em, int recno)
{

    int x = 0;
    std::string path = DATAPATH;
    path += "email.dat";

    FILE *fptr = fopen(path.c_str(),"rb+");
    if(fptr == NULL)
    {
        fptr = fopen(path.c_str(), "wb");
        if(fptr == NULL)
        {
            perror("Error unable to read email.dat, check permissions!");
            return x;
        }
    }

    if(fseek(fptr,(int)recno*sizeof(EmailIdx),SEEK_SET)==0)
        x=fread(em,sizeof(EmailIdx),1,fptr);

    fclose(fptr);
    return(x);
}

int msgs::save_emaillist(EmailIdx *em, int recno)
{

    std::string path = DATAPATH;
    path += "email.dat";

    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL)
        {
            perror("Error unable to write email.dat, check permissions!");
            return x;
        }
    }
    if(fseek(stream,(int)recno*sizeof(EmailIdx),SEEK_SET)==0)
        x = fwrite(em,sizeof(EmailIdx),1,stream);
    fclose(stream);
    return x;
}
*/

/**
 * Message Area File - Lock File
 */
int msgs::mbaselist_lockSet(int onoff)
{


    std::string path = LOCKPATH;
    path += "msgforums.lck";

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
                printf("Error msgforums.lck!");
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
 * Message Area File - Read Message Area List
 */
int msgs::read_mbaselist(mb_list_rec *mr, int rec)
{
    int x = 0;
    std::string path = DATAPATH;

    path += "msgforums.dat";

    mbaselist_lockSet(TRUE);
    FILE *fptr = fopen(path.c_str(),"rb+");
    if(fptr == NULL) {
        fptr = fopen(path.c_str(), "wb");
        if(fptr == NULL) {
            perror("Error unable to read msgforums.dat, check permissions!");
            mbaselist_lockSet(FALSE);
            return x;
        }
    }

    if(fseek(fptr,(int)rec*sizeof(mb_list_rec),SEEK_SET)==0)
        x=fread(mr,sizeof(mb_list_rec),1,fptr);

    fclose(fptr);
    mbaselist_lockSet(FALSE);
    return(x);
}


/**
 * Message Area File - Temp Holder When removing
 */
int msgs::save_mbasetemp(mb_list_rec *mr, int rec)
{
    FILE *fptr;
    int x = 0;

    std::string path = DATAPATH;

    path += "msgforums.tmp";

    mbaselist_lockSet(TRUE);
    fptr=fopen(path.c_str(),"rb+");
    if(fptr==NULL) {
        fptr=fopen(path.c_str(),"wb");
        if(fptr==NULL) {
            perror("Error unable to write msgforums.tmp, check permissions!");
            mbaselist_lockSet(FALSE);
            return x;
        }
    }

    if(fseek(fptr,(int)rec*sizeof(mb_list_rec),SEEK_SET)==0)
        x=fwrite(mr,sizeof(mb_list_rec),1,fptr);

    fclose(fptr);
    mbaselist_lockSet(FALSE);
    return(x);
}


/**
 * Message Area File - Save Message Area
 */
int msgs::save_mbaselist(mb_list_rec *mr, int rec)
{
    std::string path = DATAPATH;

    path += "msgforums.dat";

    mbaselist_lockSet(TRUE);
    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL) {
            perror("Error unable to write msgforums.dat, check permissions!");
            mbaselist_lockSet(FALSE);
            return x;
        }
    }
    if(fseek(stream,(int)rec*sizeof(mb_list_rec),SEEK_SET)==0)
        x = fwrite(mr,sizeof(mb_list_rec),1,stream);
    fclose(stream);
    mbaselist_lockSet(FALSE);
    return x;
}


/**
 * Message Area File - Count Message Areas
 */
int msgs::msg_count()
{

    int i = 0;
    mb_list_rec mb;

    while(read_mbaselist(&mb,i)) {
        ++i;
    }
    ++i;
    if(i < 1)   i = -1;
    else i--;
    return(i);
}


/**
 * Message Area File - Find msg area by filename.ext
 */
int msgs::msg_find(char *tfile)
{

    string temp1, temp2;
    mb_list_rec mb;
    temp1 = (tfile);

    int idx = 0;
    while(read_mbaselist(&mb,idx)) {
        temp2 = (char *)(mb.mbfile);
        if(temp1 == temp2) return(idx);
        idx++;
    }
    return(-1);
}

/**
 * Message Area File - Lock file to JAM Last Read Index
 */
int msgs::jlr_lockSet(int onoff)
{

    std::string path = LOCKPATH;
    path += "ljr.lck";

    if (!onoff) {
        remove((char *)path.c_str());
        return TRUE;
    }

    //While lock file missing, loop untill it disapears.
    FILE *stream;
    while(1) {
        stream = fopen(path.c_str(),"rb+");
        if(stream == NULL) {   // Lock File Missing
            stream = fopen(path.c_str(), "wb");
            if(stream == NULL) {
                printf("Error jlr.lck!");
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
 * Message Area File - Read Users Last Read Pointer
 */
int msgs::readlr(LastRead *lr, int idx, mb_list_rec *mb)
{

    char path[1024]= {0};
    //printf("%s%s.jlr", mb->mbpath, mb->mbfile);
    sprintf(path,"%s%s.jlr2", MESGPATH, (char *)mb->mbfile);
    jlr_lockSet(TRUE);

    int x = 0;
    FILE *stream;

    stream = fopen(path,"rb+");
    if(stream == NULL) {
        jlr_lockSet(FALSE);
        return (0);
    } else fclose(stream);

    stream = fopen(path, "rb");
    if(fseek(stream,(int)idx*sizeof(LastRead),SEEK_SET)==0)
        x = fread(lr,sizeof(LastRead),1,stream);
    fclose(stream);
    jlr_lockSet(FALSE);
    return (x);
}

/**
 * Message Area File - Write Users Last Read Pointer
 */
int msgs::writelr(LastRead *lr, int idx, mb_list_rec *mb)
{

    char path[1024]= {0};
    sprintf(path,"%s%s.jlr2", MESGPATH, (char *)mb->mbfile);
    jlr_lockSet(TRUE);

    FILE *stream;
    int x = 0;

    stream=fopen(path,"rb+");
    if(stream == NULL) {
        stream=fopen(path, "wb");
        if(stream == NULL) {
            write(0,path,sizeof(path));
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


/**
 * Custom Jam Message Pointers, Pointers by Msg # is retarded
 * Switched to Pointer by unique MSGID field, will never get wiped out
 * and can ways be searched for correct msg after purges and prunes.
 */
ulong msgs::JamAreaGetLast(long usernum, mb_list_rec *mb)
{

    ConsoleIO _s;

    LastRead lr;
    memset(&lr,0,sizeof(LastRead));

    if (!readlr(&lr,usernum,mb)) {
        return 0;
    }

    // First we check with a quick and dirty by msg #, like old way, but
    // We verify the CRC32 value of the MSGID to make sure.
    std::string tmp;
    unsigned long crcvalue = (ulong)jamapi_readmsgid(mb,lr.RepID,tmp);
    if (lr.MsgID == crcvalue) {
        return (lr.RepID);
    }

    // Else we have to search through all messages. or start at begining.
    // Returns 0L for Nuttin, or Message Number of Matching MsgID.
    unsigned long ret;
    ret = buildmsgid(lr.MsgID,mb);
    if (ret != 0) ++ret; // When we set, we minus 1, add it back now.

    return (ret);
}


/**
 * Set is now done by MSGID, it's universal and unique, and holds better
 * If not found, default checks last Message Number as a backup.
 *  Forget.. ?!?! :-)
 */
void msgs::JamAreaSetLast(long usernum, long msgnum, mb_list_rec *mb)
{
    // Consider this branched off seperate,
    // Now we need take the currnet message_index
    // then we need to get the header with the MSGID.
    // we then save the msgid that will be in crc32 into lr. MsgID

    LastRead lr;
    memset(&lr,0,sizeof(LastRead));

    std::string tmp;

    //	Get msg id..
    //	Header_S.MsgIdCRC=JAM_Crc32(buf,strlen((char *)buf));

    lr.RepID = msgnum; //original message # (Quick Searches)
    lr.MsgID = (ulong)jamapi_readmsgid(mb, msgnum, tmp);


    if (!writelr(&lr,usernum,mb)) {
        //	_s.errlog((char *)"JamAreaSetLast: lr.RepID CRC32 (%lu) lr.MsgID CRC32 (%lu)",lr.RepID,lr.MsgID);
    }
}



/**
 * Read Area and get Count of Total Messages
 * use elist = jamapi_build_public(&mr, thisuser); / Private
 * (Updated to work with new system)
 */
unsigned long msgs::CountMsgs(unsigned long mbnum, UserRec *usr)
{
    mb_list_rec   mb;
    memset(&mb,0,sizeof(mb_list_rec));

    // Get Public/Private status of currnet area
    read_mbaselist(&mb, mbnum);

    // This procedure uses ja->active as real true count of total
    // Live messages per area.
    return jamapi_countmsgs(&mb, usr);
}


/**
 * Count new message from Area passed.
 * (Updated to work with new system)
 */
unsigned long msgs::CountNewMsgs(unsigned long mbnum, UserRec *usr)
{

    ConsoleIO       s(usr); // Pass User Incase there are MCI Codes for User Info.

    mb_list_rec mr;

    memset(&mr,0,sizeof(mb_list_rec));
    read_mbaselist(&mr, mbnum);

    unsigned long msgcnt = 0;
    msgcnt = CountMsgs(mbnum, usr);

    unsigned long lastread = JamAreaGetLast(usr->idx, &mr);

//	s.errlog2((char *)"CountNewMessages() mbnum %lu, msgcnt %lu, lastread %lu ",mbnum, msgcnt, lastread);

    // No New Messages
    if (lastread >= msgcnt || msgcnt == 0)
        return 0;

    // Last Read is Index of eList[] Array 0 Based.
    // Incriment 1 so we can subtract from total messages, not Zero Based.
    if (lastread < msgcnt) {
        return (msgcnt - lastread);
    }
    return 0;
}


/**
 * Counts New Mesages for All Areas (Includes Email)
 * (Updated to work with new system)
 */
unsigned long msgs::CountAllNewMsgs(UserRec *usr)
{

    ulong counter = 1;
    ulong msgcnt  = 0;

    msgcnt = 0;  // Skip Email!

    mb_list_rec   mb;
    memset(&mb,0,sizeof(mb_list_rec));

    while(read_mbaselist(&mb, counter)) {
        msgcnt += CountNewMsgs(counter, usr);
        ++counter;
    }

    return(msgcnt);
}


/**
 * New User, Reset all Last Reads Pointers to 0
 * (Updated to work with new system)
 */
void msgs::resetlastread(UserRec *usr)
{
    unsigned long i = 0;
    mb_list_rec mb;
    memset(&mb,0,sizeof(mb_list_rec));

    while(read_mbaselist(&mb,i)) {
        JamAreaSetLast(usr->idx, 0, &mb);
        ++i;
    }
}
