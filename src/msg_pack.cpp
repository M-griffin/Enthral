/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Message Area Truncation and Pack Function, WIP!!!            *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// Enthral SVN: $Id: msg_pack.cpp 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msg_pack.cpp $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

/*

# include "users.h"
# include "struct.h"
# include "conio.h"
# include "msg_pack.h"

# include <string>
# include <fstream>

# include <cstdio>
# include <cstring>
# include <cstdlib>



using namespace std;



    Work in Progress, Only Packs 1 Base So Far, have to update Reply Links
    Later on and a few extras, although not keep track of it at the moment.

    also add a header count for jam, so we can see how many purged messages
    have been packed, can't seem to tell otherwise.
*/
/*

msg_pack::msg_pack() {

//    mh      = NULL;
 //   AHandle = NULL;
    lastmsg = 0; // Current Message
    total   = 0; // Total Areas
    mbnum   = 0; // Current Message Base Number Starting at 0;
    msgProcessed = 0; // Count total messages termed / skipped.
}

int msg_pack::ReadMessages() {

    char path[255]={0};

    // Since this isn't in out data file for areas, open it manually.
    sprintf(path,"%spacktmp",mb.mbpath);

    // Open Mesasge Area and Readin Current Message.
    if (AHandle != NULL) MsgCloseArea(AHandle);
    AHandle = MsgOpenArea((byte *)path, MSGAREA_CRIFNEC, mb.Type);
    if (AHandle == NULL) return(FALSE);
    else Add2MsgInfo();



    // Make sure were not past the highest message in this area.
    if(lastmsg > MI.high_msg) {
        return FALSE;
    }

    // Open Message Area
    mh = MsgOpenMsg(AHandle, MOPEN_RW, lastmsg);
    if(mh == NULL) return FALSE;

    // Read in Message
    if (GetMsg() == FALSE) {
        // If we skipped a killed message.
        //++msgProcessed;
    }
    MsgCloseMsg(mh);

    //if  (xmsg.attr & MSGLOCKED) ++msgProcessed;

    // Now save it back to orininal mesage area.
    SaveMsg(mbnum,lastmsg,TRUE);
    return TRUE;
}

// Read in only current are and truncate mesasge base.
void msg_pack::TruncCurrentArea(int iArea, int MaxMsgs) {

    // Add Lock Files Lateron.
    mbnum = CURRENT_MAREA+1;
    int prevmessages = 0;
    int newmessages  = 0;

    // Readin Current Message Base to Clone it's basic setup
    memset(&mb,0,sizeof(mb_list_rec));
    total        = _msgf.msg_count();       // Count Message Areas
    prevmessages = _msgf.CountMsgs(mbnum);  // Count Messages in Current Area
    _msgf.read_mbaselist(&mb, mbnum);

    if (AHandle != NULL) {
        CloseMsgArea();
    }

    if (prevmessages < MaxMsgs) return;

    //rename to avoid duplicate files
    char sLine[200],  sLine1[200],  sLine2[200],  sLine3[200];
    char sNLine[200], sNLine1[200], sNLine2[200];

    // Original Message Areas
    sprintf(sLine,"%s%s.jdt", mb.mbpath, mb.mbfile);
    sprintf(sLine1,"%s%s.jhr", mb.mbpath, mb.mbfile);
    sprintf(sLine2,"%s%s.jdx", mb.mbpath, mb.mbfile);
    sprintf(sLine3,"%s%s.jlr", mb.mbpath, mb.mbfile);

    // NewTemp Message Areas (Tmp Packing Base)
    sprintf(sNLine,"%spacktmp.jdt", mb.mbpath);
    sprintf(sNLine1,"%spacktmp.jhr", mb.mbpath);
    sprintf(sNLine2,"%spacktmp.jdx", mb.mbpath);

    // Rename Current to packing
    //putline(sLine); putline(sNLine);
    rename(sLine,sNLine);
    rename(sLine1,sNLine1);
    rename(sLine2,sNLine2);

    remove(sLine3); // Reset all Last Reads on this area to 0!

    // Re-Create Original Message Base
    // So we can Copy Messages into it fresh!
    _msgf.save_msgbase(&mb);

    // Now Loop throguh Each Mesasge And Copy form Temp back to Original
    // Skipping any Purged / Deleted Messages for Clean Up.
    //lastmsg = 0;

    lastmsg = prevmessages - (MaxMsgs-1);

    do {
        ++lastmsg;
    }
    while(ReadMessages() == TRUE);

    // Remove Left Over Temp (Packing) Area
    remove(sNLine);
    remove(sNLine1);
    remove(sNLine2);

    if (AHandle != NULL) {
        CloseMsgArea();
    }

    newmessages = _msgf.CountMsgs(mbnum);

    if (AHandle != NULL) {
        CloseMsgArea();
    }

    // Display Total of Messages Copied.


    pipe2ansi(sLine);

    // Reset Evenyone's Last Read Pointer if it's Greater Then message num = -1!
    users _usr;
    int iTotal = _usr.idx_count();
    int idx;

    lastmsg = prevmessages - (MaxMsgs-1);

    //Basically where resetting all high message pointers
    //if current pointer is higher then where we start, then add this difference, else set to 0
    for (int iCnt = 0; iCnt != iTotal; iCnt++) {
        idx = GetLastRead(iCnt);
        // Last Read, >= Deleted Message, set Last Read Down
        if (idx > lastmsg) {
            idx = idx - lastmsg;
        }
        else {
            idx = 0;
        }
        SetLastRead(iCnt,idx);
    }
}


// Read in only current are and pack mesasge base.
void msg_pack::PackCurrentArea(int iArea) {

    // Add Lock Files Lateron.
    mbnum = CURRENT_MAREA+1;
    int prevmessages = 0;
    int newmessages  = 0;

    // Readin Current Message Base to Clone it's basic setup
    memset(&mb,0,sizeof(mb_list_rec));
    total        = _msgf.msg_count();       // Count Message Areas
    prevmessages = _msgf.CountMsgs(mbnum);  // Count Messages in Current Area
    _msgf.read_mbaselist(&mb, mbnum);

    if (AHandle != NULL) {
        CloseMsgArea();
    }

    //rename to avoid duplicate files
    char sLine[200],  sLine1[200],  sLine2[200], sLine3[200];
    char sNLine[200], sNLine1[200], sNLine2[200];

    // Original Message Areas
    sprintf(sLine,"%s%s.jdt", mb.mbpath, mb.mbfile);
    sprintf(sLine1,"%s%s.jhr", mb.mbpath, mb.mbfile);
    sprintf(sLine2,"%s%s.jdx", mb.mbpath, mb.mbfile);
    sprintf(sLine3,"%s%s.jlr", mb.mbpath, mb.mbfile);

    // NewTemp Message Areas (Tmp Packing Base)
    sprintf(sNLine,"%spacktmp.jdt", mb.mbpath);
    sprintf(sNLine1,"%spacktmp.jhr", mb.mbpath);
    sprintf(sNLine2,"%spacktmp.jdx", mb.mbpath);

    // Rename Current to packing
    //putline(sLine); putline(sNLine);
    rename(sLine,sNLine);
    rename(sLine1,sNLine1);
    rename(sLine2,sNLine2);

    remove(sLine3);

    // Re-Create Original Message Base
    // So we can Copy Messages into it fresh!
    _msgf.save_msgbase(&mb);

    // Now Loop throguh Each Mesasge And Copy form Temp back to Original
    // Skipping any Purged / Deleted Messages for Clean Up.
    lastmsg = 0;
    do {
        ++lastmsg;
    }
    while(ReadMessages() == TRUE);

    // Remove Left Over Temp (Packing) Area
    remove(sNLine);
    remove(sNLine1);
    remove(sNLine2);

    if (AHandle != NULL) {
        CloseMsgArea();
    }

    newmessages = _msgf.CountMsgs(mbnum);

    if (AHandle != NULL) {
        CloseMsgArea();
    }

    // Display Total of Messages Copied.

    pipe2ansi(sLine);
}

// Change to next message area, if doesn't exist return FALSE!
int msg_pack::ChangeNextArea() {

    total = _msgf.msg_count(); // count Message Areas
    --total;
    if(mbnum < total) {
        ++mbnum;
        lastmsg = 1;
        return (TRUE);
    }
    return (FALSE);

}

// Loop throguh each message area and pack bases.
void msg_pack::PackAllAreas() {

    // Loop though each area.
    mbnum    = 0;
    lastmsg  = 1;
    int done = FALSE;

    pipe2ansi((char *)"|CSPacking Message Areas...");
    do {
        PackCurrentArea(mbnum);
        if (ChangeNextArea() == FALSE)
            done = TRUE;
    }
    while (done == FALSE);
}

// Loop throguh each message area and pack bases.
void msg_pack::TruncAllAreas() {

    // Loop though each area.
    mbnum    = 0;
    lastmsg  = 1;
    int done = FALSE;

    int MaxMesgs = 200;
    pipe2ansi((char *)"|CSTruncating Message Areas...");
    do {
        TruncCurrentArea(mbnum,MaxMesgs); // Supply Max # of Message PEr Area here.
        if (ChangeNextArea() == FALSE)
            done = TRUE;
    }
    while (done == FALSE);
}




// New Message Function for Packing Message Areas.

#include "msgapi.h"
#include "prog.h"


#define PROGRAM_NAME "sqpack v1.2.4-release"

unsigned long msgCopied,   msgProcessed;   // per Area
unsigned long totaloldMsg, totalmsgCopied;

#define UINT32 unsigned int
#define UINT16 unsigned short
#define ULONG  unsigned long


// returns zero if msg was killed, nonzero if it was copied

int processMsg(dword msgNum, dword numMsg, HAREA oldArea, HAREA newArea,
               s_area *area, UINT32 shift)
{
    HMSG msg, newMsg;
    XMSG xmsg;
    struct tm tmTime;
    time_t ttime, actualTime = time(NULL);
    char *text, *ctrlText;
    dword  textLen, ctrlLen;
    int unsent, i, rc = 0;

    //   unsigned long offset;

    msg = MsgOpenMsg(oldArea, MOPEN_RW, msgNum);
    if (msg == NULL) return rc;

    if (MsgReadMsg(msg, &xmsg, 0, 0, NULL, 0, NULL)==(dword)-1L) {
        MsgCloseMsg(msg);
        msgProcessed++;
        return rc;
    }

    unsent = ((xmsg.attr & MSGLOCAL) && !(xmsg.attr & MSGSENT)) || (xmsg.attr & MSGLOCKED);

    if (unsent || (((area -> max == 0) || ((numMsg - msgProcessed + msgCopied) <= area -> max) ||
        (area -> keepUnread && !(xmsg.attr & MSGREAD))) && !((xmsg.attr & MSGREAD) && area -> killRead))) {
        //only max msgs should be in new area

        if (xmsg.attr & MSGLOCAL) {
            DosDate_to_TmDate((SCOMBO*)&(xmsg.date_written), &tmTime);
        } else {
            DosDate_to_TmDate((SCOMBO*)&(xmsg.date_arrived), &tmTime);
        }
        //     DosDate_to_TmDate(&(xmsg.attr & MSGLOCAL ? xmsg.date_written
        //xmsg.date_arrived), &tmTime);
        ttime = mktime(&tmTime);
        if (ttime == 0xfffffffflu) ttime = 0; // emx

        if (unsent || (area -> purge == 0) || ttime == 0 ||
            (abs(actualTime - ttime) <= (area -> purge * 24 *60 * 60))) {
            xmsg.replyto = MsgUidToMsgn(oldArea, xmsg.replyto, UID_EXACT) > shift ? MsgUidToMsgn(oldArea, xmsg.replyto, UID_EXACT) - shift : 0;
            if ((area->msgbType & MSGTYPE_SQUISH) == MSGTYPE_SQUISH){

                for (i = 0; i < MAX_REPLY; i++)
                    xmsg.replies[i] = MsgUidToMsgn(oldArea, xmsg.replies[i], UID_EXACT) > shift ? MsgUidToMsgn(oldArea, xmsg.replies[i], UID_EXACT) - shift : 0;
            }else {
                xmsg.replies[0] = MsgUidToMsgn(oldArea, xmsg.replies[0], UID_EXACT) > shift ? MsgUidToMsgn(oldArea, xmsg.replies[0], UID_EXACT) - shift : 0;
                xmsg.xmreplynext = MsgUidToMsgn(oldArea, xmsg.xmreplynext, UID_EXACT) > shift ? MsgUidToMsgn(oldArea, xmsg.xmreplynext, UID_EXACT) - shift : 0;
            }
            // copy msg
            textLen = MsgGetTextLen(msg);
            ctrlLen = MsgGetCtrlLen(msg);

            text = (char *) malloc(textLen+1);
            text[textLen] = '\0';

            ctrlText = (char *) malloc(ctrlLen+1);
            ctrlText[ctrlLen] = '\0';

            MsgReadMsg(msg, NULL, 0, textLen, (byte*)text, ctrlLen, (byte*)ctrlText);

            if (area->msgbType & MSGTYPE_SDM)
                MsgWriteMsg(msg, 0, &xmsg, (byte*)text, textLen, textLen, ctrlLen, (byte*)ctrlText);
            else {
                newMsg = MsgOpenMsg(newArea, MOPEN_CREATE, 0);
                MsgWriteMsg(newMsg, 0, &xmsg, (byte*)text, textLen, textLen, ctrlLen, (byte*)ctrlText);
                MsgCloseMsg(newMsg);
            }

            msgCopied++;
            free(text);
            free(ctrlText);
            rc = 1;
        }

    }
    MsgCloseMsg(msg);
    msgProcessed++;
    return rc;
}

UINT32 getShiftedNum(UINT32 msgNum, UINT32 rmCount, UINT32 *rmMap)
{
   UINT32 i, nMsgNum = msgNum;
   msgNum += rmMap[1];
   for (i = 0; i < rmCount; i+=2)
    if (msgNum >= rmMap[i])
        nMsgNum -= rmMap[i + 1];
    else
        break;
   return msgNum > 0L ? msgNum : 0L;
}

void updateMsgLinks(UINT32 msgNum, HAREA area, UINT32 rmCount, UINT32 *rmMap, int areaType)
{
   HMSG msg;
   XMSG xmsg;
   int i;

   msg = MsgOpenMsg(area, MOPEN_RW, getShiftedNum(msgNum, rmCount, rmMap));
   if (msg == NULL) return;

   MsgReadMsg(msg, &xmsg, 0, 0, NULL, 0, NULL);

   xmsg.replyto = getShiftedNum(xmsg.replyto, rmCount, rmMap);
   if ((areaType & MSGTYPE_SQUISH) == MSGTYPE_SQUISH)
    for (i = 0; i < MAX_REPLY; i++)
        xmsg.replies[i] = getShiftedNum(xmsg.replies[i], rmCount, rmMap);
   else {
    xmsg.replies[0] = getShiftedNum(xmsg.replies[0], rmCount, rmMap);
    xmsg.xmreplynext = getShiftedNum(xmsg.xmreplynext, rmCount, rmMap);
   }

   MsgWriteMsg(msg, 0, &xmsg, NULL, 0, 0, 0, NULL);
   MsgCloseMsg(msg);
}



void purgeArea(s_area *area)
{
    char *oldName = area -> fileName;
    char *newName=NULL;
    HAREA oldArea=NULL, newArea = NULL;
    dword highMsg, i, j, numMsg, hw=0;
    int areaType = area -> msgbType & (MSGTYPE_JAM | MSGTYPE_SQUISH | MSGTYPE_SDM);

    UINT32 *oldLastread, *newLastread = NULL;
    UINT32 *removeMap;
    UINT32 rmIndex = 0;

    if (area->nopack) {
            printf("   No purging needed!\n");
            return;
    }

    //generated tmp-FileName
    xstrscat(&newName, oldName, "_tmp", NULL);

    //oldArea = MsgOpenArea((byte *) oldName, MSGAREA_NORMAL, -1, -1, -1, MSGTYPE_SQUISH);
    oldArea = MsgOpenArea((byte *) oldName, MSGAREA_NORMAL, (word) areaType);

    //if (oldArea) newArea = MsgOpenArea((byte *) newName, MSGAREA_CREATE, area.fperm, area.uid, area.gid,MSGTYPE_SQUISH);
    if (oldArea) {
        if (areaType == MSGTYPE_SDM)
            newArea = oldArea;
        else
            newArea = MsgOpenArea((byte *) newName, MSGAREA_CREATE, (word) areaType);
    }

    if ((oldArea != NULL) && (newArea != NULL)) {
        ULONG lcount;

        highMsg = MsgGetHighMsg(oldArea);
        numMsg = MsgGetNumMsg(oldArea);
        if (areaType != MSGTYPE_SDM) hw = MsgGetHighWater(oldArea);
        readLastreadFile(oldName, &oldLastread, &lcount, oldArea, areaType);
        if (oldLastread) {
            newLastread = (UINT32 *) malloc(lcount * sizeof(UINT32));
            memcpy(newLastread, oldLastread, lcount * sizeof(UINT32));
        }

        removeMap = (UINT32 *) calloc(2, sizeof(UINT32));

        for (i = j = 1; i <= highMsg; i++, j++) {
            if (!processMsg(j, numMsg, oldArea, newArea, area,
                removeMap[1])) {
                if (!(rmIndex & 1)) {
                // We started to delete new portion of
                    removeMap = (UINT32 *) realloc(removeMap, (rmIndex + 2) * sizeof(UINT32));
                    removeMap[rmIndex++] = i;
                    removeMap[rmIndex] = 0;
                };
                removeMap[rmIndex]++; // Anyway, update counter
                if (areaType == MSGTYPE_SDM)
                    MsgKillMsg(oldArea, j--);
            } else {
                // We are copying msgs
                if (rmIndex & 1) rmIndex++;
            };
        };

        if (rmIndex && areaType == MSGTYPE_SDM) {
            // renumber the area
            char oldmsgname[PATHLEN], newmsgname[PATHLEN];
            for (i = j = 1; i <= highMsg; i++) {
                strncpy(oldmsgname, oldName, PATHLEN);
                Add_Trailing(oldmsgname, PATH_DELIM);
                strncpy(newmsgname, oldmsgname, PATHLEN);
                sprintf(oldmsgname+strlen(oldmsgname), "%u.msg", (unsigned int)i);
                sprintf(newmsgname+strlen(newmsgname), "%u.msg", (unsigned int)j);
                if (access(oldmsgname, 0))
                    continue;
                if (i == j) {
                    j++;
                    continue;
                }
                if (rename(oldmsgname, newmsgname) == 0)
                    j++;
            }
        }

        if (rmIndex > 2) { // there were several areas with deleted msgs
            for (j = 1; j <= highMsg; j++)
                updateMsgLinks(i, newArea, rmIndex + 1, removeMap, areaType);
        }

        if (rmIndex) { // someting was removed, maybe need to update lastreadfile
           for (j = 0; j < lcount; j++) {
              for (i=0; i<rmIndex; i+=2) {
             if (oldLastread[j] >= removeMap[i]) {
                if (oldLastread[j] >= removeMap[i] + removeMap[i+1]) {
                   newLastread[j] -= removeMap[i+1];
                } else {
                   newLastread[j] -= oldLastread[j] - removeMap[i] + 1;
                }
             }
              }
           }
        }

        writeLastreadFile(oldName, newLastread, lcount, newArea, areaType);

        MsgCloseArea(oldArea);
        if (areaType != MSGTYPE_SDM) {
                if ((numMsg - msgCopied) > hw) hw=0;
                else hw -= (numMsg - msgCopied);
            MsgSetHighWater(newArea, hw);
            MsgCloseArea(newArea);
        }

        printf("   oldMsg: %lu   newMsg: %lu\n", (unsigned long)numMsg, msgCopied);
        totaloldMsg+=numMsg; totalmsgCopied+=msgCopied; // total

        free(oldLastread);
        free(newLastread);

        //rename oldArea to newArea
        renameArea(areaType, oldName, newName);
    }
    else {
        if (oldArea) MsgCloseArea(oldArea);
        printf("Could not open %s or create %s.\n", oldName, newName);
    }
    free(newName);
}

*/
