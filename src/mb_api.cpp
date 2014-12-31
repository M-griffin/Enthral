/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose:  High Level Message API Functions that are used in the bbs   *
 *             to communicate with mb_jam which is the new jamlib          *
 *             from crashmail rewritten to use in a working bbs system     *
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
# include "msgs.h"
# include "mb_api.h"
# include "mb_jam.h"

# include <time.h>
# include <cstdio>
# include <cstring>
# include <cstdlib>
# include <string>

using namespace std;

// Message API is a translation class from origianl functions used with SMAPI
// To the New JAMLib from Crashmail Customized to Function the same way
// With Code Already built and minor customizations. Replaces (msg_api.cpp).
// This is a bit messy and will need a cleaner rewrite!

/**
 * Message API - Get Time from DOS Time
 */
static int gettz(void)
{
    struct tm *tm;
    time_t t, gt;

    t = time(NULL);
    tzset();
    tm = gmtime(&t);
    tm->tm_isdst = 0;
    gt = mktime(tm);
    tm = localtime(&t);
    tm->tm_isdst = 0;
    return (int)(((long) mktime(tm)- (long) gt));
}

/**
 * Message API - Fido Address to String
 */
char *mbapi_jam::faddr2char(char *s,fidoaddr *fa)
{
    if(fa->point)
        sprintf(s,"%u:%u/%u.%u",
                fa->zone,fa->net,fa->node,fa->point);
    else
        sprintf(s,"%u:%u/%u",
                fa->zone,fa->net,fa->node);
    return (s);
}

/**
 * Message API - Save JAM Message
 */
int mbapi_jam::SaveMsg(unsigned long msgarea, unsigned long msgnum, int NewReply)
{
    // Setup Interface into new Jam API
    MemMessage mm;
    mm.msgnum = msgnum;

    if(mr.Kind == NETMAIL)
    {
        mm.Area[0] = 0; //Netmail
    }
    else
    {
        mm.Area[0] = 1; //Local/Echomail
    }

    struct tm *ptm, stm;
    ptm = &stm;
    ptm = DosDate_to_TmDate((SCOMBO*)(& (xmsg.date_written)), ptm);
    mm.DateTime = mktime(ptm) + gettz();

    strcpy((char *) mm.To, (char *) xmsg.to);
    strcpy((char *) mm.From, (char *) xmsg.from);
    strcpy((char *) mm.Subject, (char *) xmsg.subj);
    mm.OrigNode.Zone  = xmsg.orig.zone;
    mm.OrigNode.Net   = xmsg.orig.net;
    mm.OrigNode.Node  = xmsg.orig.node;
    mm.OrigNode.Point = xmsg.orig.point;
    mm.DestNode.Zone  = xmsg.dest.zone;
    mm.DestNode.Net   = xmsg.dest.net;
    mm.DestNode.Node  = xmsg.dest.node;
    mm.DestNode.Point = xmsg.dest.point;
    mm.Attr           = xmsg.attr;
    mm.Cost           = 0;
    mm.TextChunks = buff;

    char adrs[21]= {0};
    unsigned long num;
    unsigned long MsgId;
    time((time_t *) &num);

    /*
    fidoaddr fa;
    fa.zone     = mm.OrigNode.Zone;
    fa.net         = mm.OrigNode.Net;
    fa.node     = mm.OrigNode.Node;
    fa.point     = mm.OrigNode.Point;*/

    // Setup Message Header.
    faddr2char(adrs, &mr.aka);

    // 1. Create Current Message ID
    mm.REPLY_CRC = 0;
    mm.MSGID_CRC = 0;

    // MSGID doesn't have \01MSGID:  in the string, this is control char!
    //sprintf((char *)mm.MSGID,"\01MSGID: %s %ld", (const char *)adrs, num);
    sprintf((char *) mm.MSGID,"%s %ld", (const char *) adrs, num);

    // 2. If Reply, grab MSGID of original message, place in reply.
    std::string tmpMsgId;
    if(NewReply)
    {
        // if Reply, pass original MSGID under to get for reply for linking.
        MsgId = jamapi_readmsgid(&mr,msgnum,tmpMsgId);
        sprintf((char *) mm.REPLY,"%s", (char *) tmpMsgId.c_str());
        mm.REPLY_CRC = MsgId; // keep as CRC32, no need to convert back and forth.
//        errlog((char *)"SaveMsg (Reply) - MSGID (%s), REPLY (%s)",mm.MSGID,mm.REPLY);
//        errlog((char *)"SaveMsg (Reply) - MSGID_CRC (%lu), REPLY_CRC (%lu)",mm.MSGID_CRC,mm.REPLY_CRC);
    }
    else
    {
        tmpMsgId.clear();
        sprintf((char *) mm.REPLY,"%s", (char *) tmpMsgId.c_str());
    }
//    errlog((char *)"[*] SaveMsg - MSGID: (%s), REPLY: (%s)",mm.MSGID,mm.REPLY);
//    errlog((char *)"[*] SaveMsg - MSGID_CRC (%lu), REPLY_CRC (%lu)",mm.MSGID_CRC,mm.REPLY_CRC);
    return (jamapi_writemsg(&mm, &mr));
}

/*
void mbapi_jam::MakeCtrlHdr(char *reply)
{
    char adrs[21]={0};
    unsigned long num;
    time((time_t *)&num);

    faddr2char(adrs, &mr.aka);
    if(strlen(reply) == 0) {
        sprintf(cinfbuf,"\01MSGID: %s %ld\01PID: %s", adrs, num, BBSVERSION);
    }
    else {
        sprintf(cinfbuf,"\01MSGID: %s %ld\01%s\01PID: %s", adrs, num, reply, BBSVERSION);
    }
}*/

/*
void mbapi_jam::GetMsgID(char *reply)
{
    int  iLineBeg = 0, iLineEnd = 0, iCnt = 0;
    char line[1024] = {0};

    while(iLineBeg < cinflen && iLineEnd < cinflen)
    {
        memset(line,0,sizeof(line));
        while(( iLineEnd < cinflen &&
                iLineEnd < (80+iLineBeg)) &&
                cinfbuf[iLineEnd] != '\01') {
            iLineEnd++;
        }
        while(((iLineEnd > iLineBeg) &&
                (iLineEnd > 0) &&
                (iLineEnd > (80+iLineBeg-10))) &&
                cinfbuf[iLineEnd] != '\01')
        {
            iLineEnd--;
        }
        if((iLineEnd >= iLineBeg) && iLineEnd <= cinflen)
        {
            memcpy(line,cinfbuf+iLineBeg,iLineEnd-iLineBeg);
            stripCR(line);
            if(strncmp(line,"MSGID",5) == 0)
            {
                strcpy(reply,line);
                break;
            }
            iCnt++;
        }
        else{
            return;
        }
        iLineEnd++;
        iLineBeg=iLineEnd;
    }
}*/

/**
 * Message API - Fill XMSG Structure that holds basic header Info
 */
void mbapi_jam::fill_xmsg(char *from, char *to, char *subj)
{
    strcpy((char*) xmsg.to,to);
    strcpy((char*) xmsg.from,from);
    strcpy((char*) xmsg.subj,subj);
}

/**
 * Message API - Fill Message Info Structure, JAM -> BBS
 */
void mbapi_jam::mm2MsgInfo(struct MemMessage *mm)
{
    strcpy((char *) MI.AreaName, (char *) mr.mbdisplay);
    /*
    MI.colors[0]    = mr.colors[0];
    MI.colors[1]    = mr.colors[1];
    MI.colors[2]    = mr.colors[2];
    MI.colors[3]    = mr.colors[3];
    */

    strcpy((char*) MI.From, (char*) mm->From);
    strcpy((char*) MI.To, (char*) mm->To);
    strcpy((char*) MI.Subj, (char*) mm->Subject);
    MI.orig.zone    = mm->OrigNode.Zone;
    MI.orig.net     = mm->OrigNode.Net;
    MI.orig.node    = mm->OrigNode.Node;
    MI.orig.point   = mm->OrigNode.Point;
    MI.dest.zone    = mm->DestNode.Zone;
    MI.dest.net     = mm->DestNode.Net;
    MI.dest.node    = mm->DestNode.Node;
    MI.dest.point   = mm->DestNode.Point;
    MI.attr         = mm->Attr;
    mm->DateTime   -= gettz();
    MI.date_written = *timeTToStamp(mm->DateTime);

    // MI.date_written = *timeTToStamp(&mm->DateTime -= gettz(););
    // MI.date_arrived = mm->date_arrived;
    // MI.replyto      = mm->replyto;

    MI.high_water   = mm->HighWater; // Total Mesasges Include Deleted
    MI.high_msg     = mm->HighWater; // Total Mesasges Include Deleted
    MI.active       = mm->Active;    // Number of Active Mesasge
    MI.cur_msg      = mm->CurrMsg;   // Current Message Number. (1 Base)

    //setup Message Text.
    buff.erase(); // Clear it first, not ghost test.
    buff.assign(mm->TextChunks);

    /* -- Need to Impliment ReplyChain
        for(int i = 0; i < 10; i++) {
            if(xmsg.replies[i] != 0){
                MI.replies[i]   = xmsg.replies[i];
            }
        }
    */
}

/**
 * Message API - Fill Message Info Structure, JAM -> BBS If Message Deleted.
 */
void mbapi_jam::MessageDeleted(MemMessage *mm)
{
    strcpy((char*) mm->From, (char*) "");
    strcpy((char*) mm->To, (char*) "");
    strcpy((char*) mm->Subject, (char*) "{Deleted}, Message no longer exists!");

    mm->OrigNode.Zone = 0;
    mm->OrigNode.Net = 0;
    mm->OrigNode.Node = 0;
    mm->OrigNode.Point = 0;
    mm->DestNode.Zone = 0;
    mm->DestNode.Net = 0;
    mm->DestNode.Node = 0;
    mm->DestNode.Point  = 0;
    mm->Attr  = 0;
    mm->DateTime = 0;

//    MI.date_written = *timeTToStamp(mm->DateTime);
    // MI.date_written = *timeTToStamp(&mm->DateTime -= gettz(););
    // MI.date_arrived = mm->date_arrived;
    // MI.replyto      = mm->replyto;
//    mm->HighWater; // Total Mesasges Include Deleted
//    mm->Active;    // Number of Active Mesasge
//    mm->CurrMsg;   // Current Message Number. (1 Base)

    //setup Message Text.
    mm->TextChunks.erase();
    mm->TextChunks.assign((char *) "\r |04Deleted! |08T|07his message no longer exists|08. . .\r\r");
}

/**
 * Message API - Fill Message Info Structure, JAM -> BBS If Message Not Found.
 */
void mbapi_jam::MessageNotFound(MemMessage *mm)
{
    strcpy((char*) mm->From, (char*) "");
    strcpy((char*) mm->To, (char*) "");
    strcpy((char*) mm->Subject, (char*) "{Unavailable}, Unable to read message!");

    mm->OrigNode.Zone = 0;
    mm->OrigNode.Net = 0;
    mm->OrigNode.Node = 0;
    mm->OrigNode.Point = 0;
    mm->DestNode.Zone = 0;
    mm->DestNode.Net = 0;
    mm->DestNode.Node = 0;
    mm->DestNode.Point  = 0;
    mm->Attr  = 0;
    mm->DateTime = 0;

//    MI.date_written = *timeTToStamp(mm->DateTime);
    // MI.date_written = *timeTToStamp(&mm->DateTime -= gettz(););
    // MI.date_arrived = mm->date_arrived;
    // MI.replyto      = mm->replyto;
//    mm->HighWater; // Total Mesasges Include Deleted
//    mm->Active;    // Number of Active Mesasge
//    mm->CurrMsg;   // Current Message Number. (1 Base)

    //setup Message Text.
    mm->TextChunks.erase();
    mm->TextChunks.assign((char *) "\r |04Unavailable! |08U|07nable to read this message|08. . .\r\r");
}


/**
 * Message API - Read Message Area (Checks If Message Exists?)
 **/

/* This function is a load of crap now.. rework this with new stuff!!
 * This crap should onlu be called prior to last read being done
 * So that thisuser->lastmsg is a valid msg number!!
 * Otherwise this could return false on a deleted message or past message index.

 */

int mbapi_jam::ReadMsgArea(unsigned long mbnum, int email)
{
    errlog2((char *) "1. ReadMsgArea ");
    int res = 0;

    // Setup Interface into new Jam API
    MemMessage mm;
    memset(&mr,0,sizeof(mb_list_rec));
    if(!_msgf.read_mbaselist(&mr,mbnum))
    {
//        errlog((char *)"1. ReadMsgArea: !_msgf.read_mbaselist(&mr,mbnum) ");
        return (FALSE);
    }

    errlog2((char *) "1. jamapi_readmsg ");
//    errlog((char *)" *** ReadMsgArea: jamapi_readmsg(&mr, thisuser->lastmsg %lu, &mm) ",thisuser->lastmsg);
    // Should this be here?  mmmm
    res = jamapi_readmsg(&mr, thisuser->lastmsg, &mm, email, thisuser);
    if(res)
    {
        // Check return value,  if messaege = NO_MESSAGE, then deleted, skip to next!
        // And there are more messages in this area.
        if(res == JAM_NO_MESSAGE && thisuser->lastmsg < mm.HighWater)
        {
            errlog2((char *) "1. jamapi_readmsg JAM_NO_MESSAGE ");
            //pipe2ansi((char *)"|CR|CR|08T|07his message no longer exists, skipping to next|08. . .|DE|DE");
            MessageDeleted(&mm);    // Populated Generic Deleted Message.
            mm2MsgInfo(&mm);
            return TRUE;
        }
        else if(res == JAM_NO_MESSAGE && thisuser->lastmsg == mm.HighWater)
        {
            errlog2((char *) "1. jamapi_readmsg JAM_NO_MESSAGE 2");
            MessageDeleted(&mm);    // Populated Generic Deleted Message.
            mm2MsgInfo(&mm);
            return TRUE;
        }
        else
        {
            errlog2((char *) "1. jamapi_readmsg JAM_NO_MESSAGE ELSE ");
//            errlog((char *)" *** 2. ReadMsgArea: !jamapi_readmsg(&mr, thisuser->lastmsg %lu, &mm) ",thisuser->lastmsg);
            return FALSE;
        }
    }
    errlog2((char *) "1. jamapi_readmsg mm2MsgInfo");
//    errlog((char *)"2 *** ReadMsgArea: Done, Populate mm2MsgInfo(); ");
    mm2MsgInfo(&mm);
    return (TRUE);
}


/**
 * Message API - Set Fido Flags for Currnet Message
 */
void mbapi_jam::FidoFlags(char *fflags)
{
    if(MI.attr & MSGPRIVATE)
        strcat(fflags,"Private");

    if(MI.attr & MSGCRASH)
        (strlen(fflags) >0) ? strcat(fflags,", ") : strcat(fflags,"Crash");

    if(MI.attr & MSGREAD)
        (strlen(fflags) >0) ? strcat(fflags,", ") : strcat(fflags,"Read");

    if(MI.attr & MSGLOCAL)
        (strlen(fflags) >0) ? strcat(fflags,", ") : strcat(fflags,"Local");
}

/**
 * Message API - Setup Message Header for Display in Reader
 */
void mbapi_jam::SetupMsgHdr()
{
    time_t tmt;
    struct tm *mtm;
    char faddr[81] = {0},
       timestr[81] = {0},
        fflags[41] = {0},
       namestr[81] = {0},
         tostr[81] = {0};

//    unsigned int repf = 0, rept = 0;
    std::string sFrom;
//  int id1;
//    rept    = MI.replyto;
//    repf    = MI.replies[0];
    *fflags = '\0';

    errlog2((char *) "SetupMsgHdr - FidoFlags");
    FidoFlags(fflags);

    /*
    if(mr.Kind != LOCAL && mr.Kind != EMAIL){
        if(MI.orig.point) {
            sprintf(faddr,"%d:%d/%d.%d",
            MI.orig.zone, MI.orig.net, MI.orig.node,MI.orig.point);
            sprintf(namestr,"%s",MI.From);
        }
        else {
            sprintf(faddr,"%d:%d/%d", MI.orig.zone, MI.orig.net, MI.orig.node);
            sprintf(namestr,"%s",MI.From);
        }
    }
    else{
        sprintf(namestr,"%s",MI.From);
    }
    */

    errlog2((char *) "SetupMsgHdr - mr.Kind");
    if(mr.Kind == NETMAIL || mr.Kind == ECHOMAIL || mr.Type == PRIVATE)
    {
        if(MI.dest.point > 0)
        {
            sprintf(faddr,"%d:%d/%d.%d",
                    MI.dest.zone, MI.dest.net, MI.dest.node,MI.dest.point);
            sprintf(tostr,"%s@%s",MI.To,faddr);
        }
        else
        {
            sprintf(faddr,"%d:%d/%d", MI.dest.zone, MI.dest.net, MI.dest.node);
            sprintf(tostr,"%s@%s",MI.To,faddr);
        }
    }
    else
        sprintf(tostr,"%s",MI.To);
    sprintf(tostr,"%s",MI.To);
    sprintf(namestr,"%s",MI.From);

    errlog2((char *) "stampToTimeT - mr.Kind");
    tmt = stampToTimeT(&MI.date_written);
    mtm = localtime(&tmt);
    //Add Case Statement for Setting Time Format.

    errlog2((char *) "timestr - mr.Kind");
    //strftime(timestr,81,"%m/%d/%Y %H:%M",mtm);
    strftime(timestr,81,"%A %m/%d/%Y %I:%M %p",mtm);

    errlog2((char *) "timestr - mHead.curmsg");
    sprintf(mHead.curmsg,"%ld", (ulong) thisuser->lastmsg);
    sprintf(mHead.totmsg,"%ld", MI.high_msg);

    // Remove for Normal Echomail
    /*
        sFrom = namestr;
        id1 = sFrom.find("@",0);
        if (id1 != -1) {
            sFrom.erase(id1,sFrom.size() - id1);
        }
    */

    //strcpy(mHead.from,  sFrom.c_str());
    errlog2((char *) "timestr - mHead.from");
    strcpy(mHead.from, (char *) namestr);
    strcpy(mHead.to, (char *) tostr);
    strcpy(mHead.subj, (char *) MI.Subj);

    // Echomail is left Blank so set it up - Probably change to Sent!! fix lateron.
    errlog2((char *) "timestr - fflags");
    (strlen(fflags) <= 1) ? strcpy(mHead.flags, "Echomail") : strcpy(mHead.flags, fflags);
    strcpy(mHead.time,  timestr);
    strcpy(mHead.area,  MI.AreaName);
}

/**
 * Message API - String Replace
 */
char *mbapi_jam::strrepl(char *Str, size_t BufSiz, const char *OldStr, const char *NewStr)
{
    int OldLen, NewLen;
    char *p, *q;

    if(NULL == (p = strstr(Str, OldStr)))
        return Str;

    OldLen = strlen(OldStr);
    NewLen = strlen(NewStr);

    if((strlen(Str) + NewLen - OldLen + 1) > BufSiz)
        return NULL;

    memmove(q = p+NewLen, p+OldLen, strlen(p+OldLen) +1);
    memcpy(p, NewStr, NewLen);
    return q;
}

/**
 * Message API - Strip CR, BS and Escapes
 */
void mbapi_jam::stripCR(char *ostr)
{
    while(strchr(ostr,'\n')) strrepl(ostr,500,"\n","");
    while(strchr(ostr,'\r')) strrepl(ostr,500,"\r","");
    while(strchr(ostr,'\b')) strrepl(ostr,500,"\b","");
    while(strchr(ostr,'\x1b')) strrepl(ostr,500,"\x1b","");
}

/**
 * Message API - Strip CR, BS not Escapes
 */
void mbapi_jam::stripCRONLY(char *ostr)
{
    while(strchr(ostr,'\n')) strrepl(ostr,500,"\n","");
    while(strchr(ostr,'\r')) strrepl(ostr,500,"\r","");
    while(strchr(ostr,'\b')) strrepl(ostr,500,"\b","");
}

/**
 * Message API - Parse Out MCI Codes That are Not Pipe Color Codes
 */
void mbapi_jam::parseMCI(std::string &msgtext)
{
    std::string::size_type id1;
    std::string::size_type id2;
    std::string temp;

    id2 = 0;
    while(1)
    {
        id1 = msgtext.find("|",id2);
        if(id1 == std::string::npos)
            break;
        else
        {
            // Only Allow Colors, otherwise erase pipe.
            temp = msgtext.substr(id1+1,2);
            if(isdigit(temp[0]) && isdigit(temp[1]))
                id2 = id1+1;
            else
                msgtext.erase(id1,1);
        }
    }
}

/**
 * Message API - Setup Message Text, Populates Link List (msg_readll)
 */
void mbapi_jam::MsgSetupTxt()
{
    std::string::size_type id1    = 0, id2 = 0, id3 = 0;
    std::string::size_type index1 = 0;

    //int index2 = 0,
    int i      = 0;
    int len    = 0;
    int BegininngText = TRUE;
    char szTmp[3] = {0};

    LineRec *tmp;
    std::string Line;
    std::string sTrunc;
    std::string MsgText = buff;
    char output[1024] = {0};

    errlog2((char *) "MsgSetupTxt() - parseMCI");
    // Parse Out MCI Codes from Message Text, Only Allow Color Pipe Codes.
    parseMCI(MsgText);

    //errlog2((char *)MsgText.c_str());
    int MAX_LEN  = TERM_WIDTH;      // Max length to write messages lines to
    errlog2((char *) "SetupMsgHdr()");

    // Setup the Message Header
    SetupMsgHdr();

    // Loop through and Covert all New Line Foramting.
    // At the end everything should only have \r for a newline.

    while(1)
    {
        id1 = MsgText.find("\x1b", 0);
        if(id1 != std::string::npos)
        {
            //MsgText.erase(id1,1);
            // Change Escape to ^ character.
            MsgText[id1] = '^';
        }
        else
            break;
        errlog2((char *) "MsgSetupTxt() - loop - ESC");
    }

    // Possiable Soft Breaks.
    while(1)
    {
        id1 = MsgText.find("\x8D", 0);
        if(id1 != std::string::npos)
        {
            MsgText.erase(id1,1);
        }
        else
            break;
        errlog2((char *) "MsgSetupTxt() - loop - x8D");
    }
    errlog2((char *) "MsgSetupTxt() - loop");

    // Convert \r\n to \r
    while(1)
    {
        id1 = MsgText.find("\r\n", 0);
        if(id1 != std::string::npos)
        {
            //MsgText.erase(id1,1);
            // Change Escape to ^ character.
            MsgText.erase(id1+1,1);
        }
        else
            break;
        errlog2((char *) "MsgSetupTxt() - loop - CRLF");
    }


    // Convert \n to \r
    while(1)
    {
        id1 = MsgText.find("\n", 0);
        if(id1 != std::string::npos)
        {
            MsgText[id1] = '\r';
        }
        else
            break;
        errlog2((char *) "MsgSetupTxt() - loop - CR");
    }

    //errlog2((char *)" *** MsgSetupHdr!");
    // Now Read in Message Text and Populate Link List.
    while(MsgText.size() > 0)
    {
        errlog2((char *) "MsgSetupTxt() - loop - Populate Link List.");

        // Each Line ends with '\r.
        // Although some Echomail doesn't use this standard.
        Line.erase();
        id1 = MsgText.find("\r", 0);

        // Check for Blank Lines
        if(id1 == 0)
        {
            MsgText.erase(0,1);
            mLink.add_to_list("");
            continue;
        }

        if(id1 == std::string::npos)
        {
            errlog2((char*) "break!!!");
            break;
        }
        else
        {
            len = MAX_LEN;
            index1 = 0;
            // In This section, we need to count PIPE Codes in the length of the line
            // So that when we wrap a line, at include the pipe codes which ware parsed out
            // When they get printed to the screen.
            // Only get for portion of screen we are working with
            while(index1 != std::string::npos && index1 <= MAX_LEN)
            {
                index1 = MsgText.find("|",index1);
                //elog ("pipe fixing....");
                if(index1 != std::string::npos &&
                        (signed) id1 > len)
                {
                    if(index1+2 < MsgText.size())
                    {
                        szTmp[0] = MsgText[index1+1];  // Get First # after Pipe
                        szTmp[1] = MsgText[index1+2];  // Get Second Number After Pipe
                    }
                    else break;

                    //  If Color Pipe, add to max len.
                    if(isdigit(szTmp[0]) && isdigit(szTmp[1]))
                    {
                        len      += 3;
                        index1   += 3;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
                ++index1;
            }
            // Do the nextline is great then message width
            //if ((signed)id1 > len) id1 = len; // Some Writers don't use newlines.
            // If currnet line longer then usually, find space in reverse from line max!
            if((signed) id1 > len)
            {
                id1 = MsgText.rfind(" ",len-1);
                if(id1 == std::string::npos)    // Means continious line,cut it.
                    id1 = len-1;
                ++id1; // Skip Space for next line.
            }
            sprintf(output,"[*] id1 %i, len %i",id1, len);
            errlog2((char *) output);

            // Chop up string into message line / row.
            Line = MsgText.substr(0,id1+1);
            MsgText.erase(0,id1+1);
            if(len < (signed) Line.size())
            {
                Line = Line.substr(0,len-1);
            }
            sprintf(output,"[*] id1 %i, len %i, line: %s",id1, len, (char *) Line.c_str());
            // errlog2((char *)output);

            // Clean each broken up line for proper display
            // Remove any special line chars before entering link list.
            while(1)
            {
                id1 = Line.find("\r", 0);
                if(id1 != std::string::npos)
                    Line.erase(id1,1);
                else
                    break;
            }

            // Cutoff any blank Lines in the beginning of a message.
            if(BegininngText)
            {
                //errlog2((char *)" *** Begining!");
                if(Line != " ")
                {
                    if(Line.size() > 0)
                    {
                        //mLink.current_node->data = Line;
                        mLink.add_to_list(Line);
                        ++i;
                        BegininngText = FALSE;
                    }
                }
            }
            else
            {
                //errlog2((char *)" *** strip Seen-by!");
                // Need Toggles for these!
                sTrunc = Line;
                id1 = sTrunc.find("SEEN-BY: ",0);
                id2 = sTrunc.find("PATH: ",0);
                id3 = sTrunc.find("Via ",0);
                if(id1 == std::string::npos &&
                        id2 == std::string::npos &&
                        id3 == std::string::npos)
                {
                    //mLink.current_node->data = Line;
                    mLink.add_to_list(Line);
                    ++i;
                }
            }
        }
    }
    mLink.current_node = mLink.last;
    if(mLink.current_node == 0) return;
    for(;;)
    {
        if(mLink.current_node->data == " " || mLink.current_node->data.size() <= 1)
        {
            if(mLink.current_node == 0)
            {
                break;
            }
            tmp = mLink.current_node;      // And Not At top of list, don't delete lines below
            if(mLink.current_node->up_link == 0) break;
            mLink.current_node = mLink.current_node->up_link;

            /// Add new
            mLink.current_node->dn_link = 0;
            delete tmp;
        }
        else
            break;
    }
    errlog2((char *) " *** MsgSetupTxt DONE!");
}

/**
 * Message API - Setup Message Test For Quote Class
 *               Sets up mHead Rec with Original Message Header Info For Reply
 *               And Populates Link List for Message Quoter Text Selection.
 */

// Sets up mHead Rec with Original Message Header Info For Reply
// And Populates Link List for Message Quoter Text Selection.
void mbapi_jam::MsgSetupQuoteTxt()
{
    //////errlog2("MsgSetupQuoteTxt");
    int id1 = 0;
    int i = 0;
    int BegininngText = TRUE;
    std::string Line;
    std::string MsgText = buff; // Get Message Buffer!

    // Setup the Message Header
    SetupMsgHdr();
    while(1)
    {
        // Each Line ends with '\r' in jam api
        Line.erase();
        id1  = MsgText.find("\r", 1);
        if(id1 > 74) id1 = 74;    // Some Writers don't use newlines.

        // Make Sure only to Add New Lines when being used,
        // First Line is already Setup! So Skip it
        //if (id1 != -1) mLink.add_to_list("");
        if(id1 == -1) break;
        else
        {
            Line = MsgText.substr(0,id1);
            MsgText.erase(0,id1);
            // Erase any Echomail comming in with longer
            // Lines then 79 Chars on Quoted Text, darn idiots! :)
            // Word Wrapping Quotes is ugly!
            if(Line.size()-1 > 74)
            {
                Line.erase(74,74- (Line.size()-1));
            }
            // Clean each broken up line for proper display
            // Remove any carriage return chars before entering link list.
            while(1)
            {
                id1 = Line.find("\r", 0);
                if(id1 != -1)
                    Line.erase(id1,1);
                else break;
            }

            while(1)
            {
                id1 = Line.find("\n", 0);
                if(id1 != -1)
                    Line.erase(id1,1);
                else break;
            }

            // Cutoff any blank Lines in the beginning of a message.
            if(BegininngText)
            {
                if(Line != " " || Line.size() < 1)
                {
                    if(Line.size() > 0)
                    {
                        //mLink.current_node->data = Line;
                        mLink.add_to_list(Line);
                        ++i;
                        BegininngText = FALSE;
                    }
                }
            }
            else
            {
                if(Line.size() > 0)
                {
                    //mLink.current_node->data = Line;
                    mLink.add_to_list(Line);
                    ++i;
                }
            }
        }
    }
}


/* Deprecreated from SMAPI Code. not used anymore.
 *
// Grabs Message Text into Bufer from Data Files.
int mbapi_jam::GetMsg() {

    //////errlog2("Get Message");
    int ret = TRUE;
    buff.erase();
//    cinflen = MsgGetCtrlLen(mh);

//    MsgReadMsg(mh,&xmsg,0L,0L,NULL,cinflen,(byte *)cinfbuf);
    cinfbuf[cinflen] = '\0';
//    buflen = MsgGetTextLen(mh);

    // Allocate
    char *tbuf = new char [buflen+1];

//    MsgReadMsg(mh,NULL,0L,buflen,(byte *)tbuf,0L,NULL);
    buff.assign(tbuf);
    delete [] tbuf;

    Add2MsgInfo();
    return ret;
}
*/

/*
// Grabs Basic Msg Info for gih Msg Counts!
int mbapi_jam::GetMsgInfo() {

    int ret = TRUE;
    buff.erase();

//    mh = MsgOpenMsg(AHandle, MOPEN_RW, 1);
//    if (mh == NULL) return FALSE;

//    cinflen = MsgGetCtrlLen(mh);
//    MsgReadMsg(mh,&xmsg,0L,0L,NULL,cinflen,(byte *)cinfbuf);

//    MsgCloseMsg(mh);
 //   if (mh == NULL) return FALSE;

    Add2MsgInfo();
    return ret;
}
*/

/**
 * Message API - Set Message Last Read Pointer
 */
void mbapi_jam::SetLastRead(unsigned long usr, unsigned long lr)
{
    // New Last Read by MSG Index, not Msg Number!
    _msgf.JamAreaSetLast(usr,lr,&mr);
}

/**
 * Message API - Get Message Last Read Pointer
 */
unsigned
long mbapi_jam::GetLastRead(unsigned long usr)
{
    unsigned long lr = 0;

    // New Last Read by MSG Index, not Msg Number!
    lr = _msgf.JamAreaGetLast(usr,&mr);
    return lr;
}
