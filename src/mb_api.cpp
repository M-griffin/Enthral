/***************************************************************************
 *   Copyright (C) 2004-2017 by Michael Griffin                            *
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


/**
 * Message API - Get Time from DOS Time
 */
static int gettz(void)
{
    struct tm *tm;
    time_t t, gt;

    t = time(NULL);
    tzset();
    tm = gmtime (&t);
    tm->tm_isdst = 0;
    gt = mktime(tm);
    tm = localtime (&t);
    tm->tm_isdst = 0;
    return (int)(((long)mktime(tm)-(long)gt));
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
    return(s);
}

/**
 * Message API - Save JAM Message
 */
int mbapi_jam::SaveMsg(unsigned long msgarea, unsigned long msgnum, int NewReply)
{
    MemMessage mm;
    mm.msgnum = msgnum;

    if(mr.Kind == NETMAIL) {
        mm.Area[0] = 0;
    } else {
        mm.Area[0] = 1;
    }

    struct tm *ptm, stm;
    ptm = &stm;
    ptm = DosDate_to_TmDate((SCOMBO*)(&(xmsg.date_written)), ptm);
    mm.DateTime = mktime(ptm) + gettz();

    strcpy((char *)mm.To,(char *)xmsg.to);
    strcpy((char *)mm.From,(char *)xmsg.from);
    strcpy((char *)mm.Subject,(char *)xmsg.subj);

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

    time((time_t *)&num);

    // Setup Message Header.
    faddr2char(adrs, &mr.aka);

    // 1. Create Current Message ID
    mm.REPLY_CRC = 0;
    mm.MSGID_CRC = 0;

    sprintf((char *)mm.MSGID,"%s %ld", (const char *)adrs, num);

    std::string tmpMsgId;
    if (NewReply) {
        MsgId = jamapi_readmsgid(&mr,msgnum,tmpMsgId);
        sprintf((char *)mm.REPLY,"%s",(char *)tmpMsgId.c_str());
        mm.REPLY_CRC = MsgId; // keep as CRC32, no need to convert back and forth.
    } else {
        tmpMsgId.clear();
        sprintf((char *)mm.REPLY,"%s",(char *)tmpMsgId.c_str());
    }

    return (jamapi_writemsg(&mm, &mr));
}

/**
 * Message API - Fill XMSG Structure that holds basic header Info
 */
void mbapi_jam::fill_xmsg(char *from, char *to, char *subj)
{
    strcpy((char*)xmsg.to,to);
    strcpy((char*)xmsg.from,from);
    strcpy((char*)xmsg.subj,subj);
}

/**
 * Message API - Fill Message Info Structure, JAM -> BBS
 */
void mbapi_jam::mm2MsgInfo(struct MemMessage *mm)
{
    strcpy((char *)MI.AreaName, (char *)mr.mbdisplay);

    strcpy((char*)MI.From, (char*)mm->From);
    strcpy((char*)MI.To,   (char*)mm->To);
    strcpy((char*)MI.Subj, (char*)mm->Subject);

    MI.orig.zone  = mm->OrigNode.Zone;
    MI.orig.net   = mm->OrigNode.Net;
    MI.orig.node  = mm->OrigNode.Node;
    MI.orig.point = mm->OrigNode.Point;

    MI.dest.zone  = mm->DestNode.Zone;
    MI.dest.net   = mm->DestNode.Net;
    MI.dest.node  = mm->DestNode.Node;
    MI.dest.point = mm->DestNode.Point;

    MI.attr         = mm->Attr;

    mm->DateTime   -= gettz();
    MI.date_written = *timeTToStamp(mm->DateTime);

    MI.high_water   = mm->HighWater; // Total Mesasges Include Deleted
    MI.high_msg     = mm->HighWater; // Total Mesasges Include Deleted
    MI.active       = mm->Active;    // Number of Active Mesasge
    MI.cur_msg      = mm->CurrMsg;   // Current Message Number. (1 Base)

    buff.erase();
    buff.assign(mm->TextChunks);

    /* -- Need to Impliment Replies :)
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
    strcpy((char*)mm->From,    (char*)"");
    strcpy((char*)mm->To,      (char*)"");
    strcpy((char*)mm->Subject, (char*)"{Deleted}, Message no longer exists!");

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

    //setup Message Text.
    mm->TextChunks.erase();
    mm->TextChunks.assign((char *)"\r |04Deleted! |08T|07his message no longer exists|08. . .\r\r");
}

/**
 * Message API - Fill Message Info Structure, JAM -> BBS If Message Not Found.
 */
void mbapi_jam::MessageNotFound(MemMessage *mm)
{
    strcpy((char*)mm->From,    (char*)"");
    strcpy((char*)mm->To,      (char*)"");
    strcpy((char*)mm->Subject, (char*)"{Unavailable}, Unable to read message!");

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

    //setup Message Text.
    mm->TextChunks.erase();
    mm->TextChunks.assign((char *)"\r |04Unavailable! |08U|07nable to read this message|08. . .\r\r");
}


int mbapi_jam::ReadMsgArea(unsigned long mbnum, int email)
{
    int res = 0;
    MemMessage mm;

    memset(&mr,0,sizeof(mb_list_rec));
    if(!_msgf.read_mbaselist(&mr,mbnum)) {
        return (FALSE);
    }

    res = jamapi_readmsg(&mr, thisuser->lastmsg, &mm, email, thisuser);
    if (res) {

        if (res == JAM_NO_MESSAGE && thisuser->lastmsg < mm.HighWater) {
            MessageDeleted(&mm); // Populated Generic Deleted Message.
            mm2MsgInfo(&mm);
            return TRUE;

        } else if(res == JAM_NO_MESSAGE && thisuser->lastmsg == mm.HighWater) {
            MessageDeleted(&mm); // Populated Generic Deleted Message.
            mm2MsgInfo(&mm);
            return TRUE;

        } else {
            return FALSE;
        }
    }

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
        (strlen(fflags)>0) ? strcat(fflags,", ") : strcat(fflags,"Crash");
    if(MI.attr & MSGREAD)
        (strlen(fflags)>0) ? strcat(fflags,", ") : strcat(fflags,"Read");
    if(MI.attr & MSGLOCAL)
        (strlen(fflags)>0) ? strcat(fflags,", ") : strcat(fflags,"Local");
}

/**
 * Message API - Setup Message Header for Display in Reader
 */
void mbapi_jam::SetupMsgHdr()
{

    time_t tmt;
    struct tm *mtm;
    char faddr[81]   = {0};
    char timestr[81] = {0};
    char fflags[41]  = {0};
    char namestr[81] = {0};
    char tostr[81]   = {0};

    std::string sFrom;
    *fflags = '\0';

    FidoFlags(fflags);

    if(mr.Kind == NETMAIL || mr.Kind == ECHOMAIL || mr.Type == PRIVATE) {
        if(MI.dest.point > 0) {
            sprintf(faddr,"%d:%d/%d.%d",
                    MI.dest.zone, MI.dest.net, MI.dest.node,MI.dest.point);
            sprintf(tostr,"%s@%s",MI.To,faddr);
        } else {
            sprintf(faddr,"%d:%d/%d", MI.dest.zone, MI.dest.net, MI.dest.node);
            sprintf(tostr,"%s@%s",MI.To,faddr);
        }
    } else
        sprintf(tostr,"%s",MI.To);

    sprintf(tostr,"%s",MI.To);
    sprintf(namestr,"%s",MI.From);

    tmt = stampToTimeT(&MI.date_written);
    mtm = localtime(&tmt);

    strftime(timestr,81,"%A %m/%d/%Y %I:%M %p",mtm);

    sprintf(mHead.curmsg,"%ld", (ulong)thisuser->lastmsg);
    sprintf(mHead.totmsg,"%ld", MI.high_msg);

    strcpy(mHead.from,  (char *)namestr);
    strcpy(mHead.to,    (char *)tostr);
    strcpy(mHead.subj,  (char *)MI.Subj);

    errlog((char *)"DEBUG SetupMsgHdr() From: %s ", namestr);
    errlog((char *)"DEBUG SetupMsgHdr() To: %s ", tostr);

    if (strlen(fflags) <= 1)
        strcpy(mHead.flags, "Echomail");
    else
        strcpy(mHead.flags, fflags);

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

    if ((strlen(Str) + NewLen - OldLen + 1) > BufSiz)
        return NULL;

    memmove(q = p+NewLen, p+OldLen, strlen(p+OldLen)+1);
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
    while (1) {
        id1 = msgtext.find("|",id2);
        if (id1 == std::string::npos)
            break;
        else {
            // Only Allow Colors, otherwise erase pipe.
            temp = msgtext.substr(id1+1,2);
            if (isdigit(temp[0]) && isdigit(temp[1]))
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
    std::string::size_type id1    = 0, lineId1, lineId2, lineId3;
    std::string::size_type index1 = 0;

    int i      = 0;
    int len    = 0;
    int BegininngText = TRUE;
    char szTmp[3] = {0};

    LineRec *tmp;

    std::string Line;
    std::string sTrunc;
    std::string MsgText = buff;

    parseMCI(MsgText);

    std::string::size_type MAX_LEN  = TERM_WIDTH;	  // Max length to write messages lines to

    SetupMsgHdr();

    // Loop through and Covert all New Line Foramting.
    // At the end everything should only have \r for a newline.

    while (1) {
        id1 = MsgText.find("\x1b", 0);
        if (id1 != std::string::npos) {
            MsgText[id1] = '^';
        } else
            break;
    }

    // Possiable Soft Breaks.
    while (1) {
        id1 = MsgText.find("\x8D", 0);
        if (id1 != std::string::npos) {
            MsgText[id1] = '\r';
        } else
            break;
    }

    // Convert \r\n to \r
    while (1) {
        id1 = MsgText.find("\r\n", 0);
        if (id1 != std::string::npos) {
            MsgText.erase(id1+1,1);
        } else
            break;
    }

    // Convert \n to \r
    while (1) {
        id1 = MsgText.find("\n", 0);
        if (id1 != std::string::npos) {
            MsgText[id1] = '\r';
        } else
            break;
    }

    // Now Read in Message Text and Populate Link List.
    while(MsgText.size() > 0) {
        Line.erase();
        id1 = MsgText.find("\r", 0);

        // Check for Blank Lines
        if (id1 == 0) {
            MsgText.erase(0,1);
            mLink.add_to_list("");
            continue;
        }

        if (id1 == std::string::npos) {
            break;
        } else {
            len = MAX_LEN;
            index1 = 0;

            // In This section, we need to count PIPE Codes in the length of the line
            // So that when we wrap a line, at include the pipe codes which ware parsed out
            // When they get printed to the screen.
            index1 = MsgText.find("|",index1);

            // Only get for portion of screen we are working with
            while (index1 != std::string::npos && index1 <= MAX_LEN) {
                index1 = MsgText.find("|",index1);

                //elog ("pipe fixing....");
                if (index1 != std::string::npos && (signed)id1 > len) {
                    if (index1+2 < MsgText.size()) {
                        szTmp[0] = MsgText[index1+1];
                        szTmp[1] = MsgText[index1+2];
                    } else break;

                    //  If Color Pipe, add to max len.
                    if (isdigit(szTmp[0]) && isdigit(szTmp[1])) {
                        len      += 3;
                        index1   += 3;
                    } else {
                        break;
                    }
                } else {
                    break;
                }
                ++index1;
            }

            // Do the nextline is great then message width
            if ((signed)id1 > len) {
                id1 = MsgText.rfind(" ",len-1);
                if (id1 == std::string::npos) {
                    id1 = len-1;
                }

                Line = MsgText.substr(0,id1);
                MsgText.erase(0, id1+1);
            } else {
                // Chop up string into message line / row.
                Line = MsgText.substr(0,id1);
                MsgText.erase(0, id1+1);
            }

            // Clean each broken up line for proper display
            // Remove any special line chars before entering link list.
            while (1) {
                lineId1 = Line.find("\r", 0);
                if (lineId1 != std::string::npos)
                    Line.erase(lineId1,1);
                else
                    break;
            }

            // Cutoff any blank Lines in the beginning of a message.
            if (BegininngText) {
                if (Line != " ") {
                    if (Line.size() > 0) {
                        mLink.add_to_list(Line);
                        ++i;
                        BegininngText = FALSE;
                    }
                }
            } else {
                // Need Toggles for these!
                sTrunc = Line;
                lineId1 = sTrunc.find("SEEN-BY: ",0);
                lineId2 = sTrunc.find("PATH: ",0);
                lineId3 = sTrunc.find("Via ",0);

                if (lineId1 == std::string::npos &&
                    lineId2 == std::string::npos &&
                    lineId3 == std::string::npos) {
                    //mLink.current_node->data = Line;
                    mLink.add_to_list(Line);
                    ++i;
                }
            }
        }
    }

    // Clear Empty line from Bottom and move up.
    mLink.current_node = mLink.last;
    if  (mLink.current_node == 0) return;
    for (;;) {
        if (mLink.current_node->data == " " || mLink.current_node->data.size() <= 1) {
            if(mLink.current_node == 0) {
                break;
            }
            
            // And Not At top of list, don't delete lines below
            tmp = mLink.current_node;      

            if (mLink.current_node->up_link == 0) break;
            mLink.current_node = mLink.current_node->up_link;

            /// Add new
            mLink.current_node->dn_link = 0;

            delete tmp;
        } else
            break;
    }
}

// Sets up mHead Rec with Original Message Header Info For Reply
// And Populates Link List for Message Quoter Text Selection.
void mbapi_jam::MsgSetupQuoteTxt()
{
    int id1 = 0;
    int i = 0;
    int BegininngText = TRUE;

    std::string Line;
    std::string MsgText = buff;

    // Setup the Message Header
    SetupMsgHdr();
    while(1) {
        Line.erase();
        id1  = MsgText.find("\r", 1);
        if (id1 > 74) id1 = 74; // Some Writers don't use newlines.

        if (id1 == -1) break;
        else {
            Line = MsgText.substr(0,id1);
            MsgText.erase(0,id1);

            // Erase any Echomail comming in with longer
            // Lines then 79 Chars on Quoted Text, darn idiots! :)
            // Word Wrapping Quotes is ugly!
            if (Line.size()-1 > 74) {
                Line.erase(74,74-(Line.size()-1));
            }
            // Clean each broken up line for proper display
            // Remove any carriage return chars before entering link list.
            while (1) {
                id1 = Line.find("\r", 0);
                if (id1 != -1)
                    Line.erase(id1,1);
                else break;
            }

            while (1) {
                id1 = Line.find("\n", 0);
                if (id1 != -1)
                    Line.erase(id1,1);
                else break;
            }

            // Cutoff any blank Lines in the beginning of a message.
            if (BegininngText) {
                if (Line != " " || Line.size() < 1)
                    if (Line.size() > 0) {
                        //mLink.current_node->data = Line;
                        mLink.add_to_list(Line);
                        ++i;
                        BegininngText = FALSE;
                    }
            } else {
                if (Line.size() > 0) {
                    //mLink.current_node->data = Line;
                    mLink.add_to_list(Line);
                    ++i;
                }
            }
        }
    }
}

/**
 * Message API - Set Message Last Read Pointer
 */
void mbapi_jam::SetLastRead(unsigned long usr, unsigned long lr)
{
    _msgf.JamAreaSetLast(usr,lr,&mr);
}

/**
 * Message API - Get Message Last Read Pointer
 */
unsigned
long mbapi_jam::GetLastRead(unsigned long usr)
{
    return _msgf.JamAreaGetLast(usr,&mr);;
}
