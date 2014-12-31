/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Jamlib interface functions rewritten from crashmail          *
 *            for direct use in a bbs system with reference to Husky SMAPI *
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
# include "conio.h"
# include "mb_jam.h"
# include "mb_api.h"
# include "jam.h"
# include "stamp.h"

# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <cstdlib>
# include <cstddef>
# include <vector>
# include <string>

using namespace std;

struct openbase *jam_openbases;
static int is_dst = -1;

/**
 * Message Base JAM - Find out the current status of daylight savings time
 */
static void InitCvt(void)
{
    time_t tnow;
    tnow = time(NULL);
    is_dst = !!(localtime(&tnow)->tm_isdst);
}

/**
 * Message Base JAM - Convert a DOS-style bitmapped date into a 'struct tm'
 */
struct tm *DosDate_to_TmDate(union stamp_combo *dosdate, struct tm *tmdate)
{
    if(is_dst == -1)
    {
        InitCvt();
    }

    if(dosdate->ldate == 0)
    {
        time_t t=0;
        struct tm *tm;
        tm = gmtime(&t);
        memcpy(tmdate, tm, sizeof(*tm));
        return tmdate;
    }

    tmdate->tm_mday = dosdate->msg_st.date.da;
    tmdate->tm_mon = dosdate->msg_st.date.mo - 1;
    tmdate->tm_year = dosdate->msg_st.date.yr + 80;
    tmdate->tm_hour = dosdate->msg_st.time.hh;
    tmdate->tm_min = dosdate->msg_st.time.mm;
    tmdate->tm_sec = dosdate->msg_st.time.ss << 1;
    tmdate->tm_isdst = is_dst;
    return tmdate;
}

/**
 * Message Base JAM - Convert a 'struct tm' into an Opus/DOS bitmapped date
 */
union stamp_combo *TmDate_to_DosDate(struct tm *tmdate, union stamp_combo *dosdate)
{
    if(tmdate && dosdate)
    {
        dosdate->msg_st.date.da = tmdate->tm_mday;
        dosdate->msg_st.date.mo = tmdate->tm_mon + 1;
        dosdate->msg_st.date.yr = tmdate->tm_year - 80;
        dosdate->msg_st.time.hh = tmdate->tm_hour;
        dosdate->msg_st.time.mm = tmdate->tm_min;
        dosdate->msg_st.time.ss = tmdate->tm_sec >> 1;
    }
    return dosdate;
}

/**
 * Message Base JAM - Pad String 2 Digits.
 */
static void print02d(char **str, int i)
{
    * (*str) ++=i/10+'0';
    * (*str) ++=i%10+'0';
}

/**
 * Message Base JAM - Months
 */
char months_ab[][4] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

/**
 * Message Base JAM - Time Stamp to Char String
 */
char *sc_time(union stamp_combo *sc, char *string)
{
    if(sc->msg_st.date.yr == 0)
    {
        *string = '\0';
    }
    else
    {
        sprintf(string, "%02d %s %02d  %02d:%02d:%02d", sc->msg_st.date.da,
                months_ab[sc->msg_st.date.mo - 1], (sc->msg_st.date.yr + 80) % 100,
                sc->msg_st.time.hh, sc->msg_st.time.mm, sc->msg_st.time.ss << 1);
    }
    return string;
}

/**
 * Message Base JAM - String to Time Stamp
 */
char *fts_time(char *string, struct tm *tmdate)
{
    union stamp_combo dosdate;
    return sc_time(TmDate_to_DosDate(tmdate, &dosdate), string);
}

uint32_t jam_lastnum;
//long jam_utcoffset;
long jam_utcoffset = 0xbaadf00d;

/**
 * Message Base JAM - String Copy
 */
void mystrncpy(uint8_t *dest, uint8_t *src, uint32_t len)
{
    strncpy((char *) dest, (char *) src, (size_t) len-1);
    dest[len-1]=0;
}

/**
 * Message Base JAM - Copy Out Message Flags from Text
 */
BOOL jbstrcpy(uint8_t *dest, uint8_t *src, uint32_t maxlen, uint32_t *jbc)
{
    uint32_t d=0;
    uint32_t stopchar1,stopchar2;
    uint32_t jbcpos;

    jbcpos= *jbc;
    while(src[jbcpos]==32 || src[jbcpos]==9) jbcpos++;

    if(src[jbcpos]=='"')
    {
        jbcpos++;
        stopchar1='"';
        stopchar2=0;
    }
    else
    {
        stopchar1=' ';
        stopchar2=9;
    }

    while(src[jbcpos] != stopchar1 &&
            src[jbcpos] != stopchar2 &&
            src[jbcpos] != 10 &&
            src[jbcpos] != 0)
    {
        if(src[jbcpos]=='\\' && src[jbcpos+1]!=0 && src[jbcpos+1]!=10)
            jbcpos++;

        if(d<maxlen-1)
            dest[d++]=src[jbcpos];
        jbcpos++;
    }

    dest[d]=0;
    if(src[jbcpos]==9 || src[jbcpos]==' ' || src[jbcpos]=='"')
        jbcpos++;

    *jbc=jbcpos;
    if(d!=0 || stopchar1=='"')
        return (TRUE);
    return (FALSE);
}

/**
 * Message Base JAM - Get Rest of Message Text
 */
BOOL jbstrcpyrest(uint8_t *dest,uint8_t *src,uint32_t maxlen,uint32_t *jbc)
{
    uint32_t d=0;
    uint32_t jbcpos;
    jbcpos=*jbc;

    while(src[jbcpos]==32 || src[jbcpos]==9)
        jbcpos++;

    while(src[jbcpos]!=10 && src[jbcpos]!=0)
    {
        if(d<maxlen-1)
            dest[d++]=src[jbcpos];
        jbcpos++;
    }

    dest[d]=0;
    *jbc=jbcpos;
    if(d!=0)
        return (TRUE);
    return (FALSE);
}

/**
 * Message Base JAM - Strip tailing spaces
 */
void striptrail(uint8_t *str)
{
    int c;
    for(c=strlen((char *) str)-1; str[c] < 33 && c>=0; c--)
        str[c]=0;
}

/**
 * Message Base JAM - Strip leading spaces.
 */
void striplead(uint8_t *str)
{
    int c = 0;
    while(str[c]==' ')
    {
        c++;
    }
    strcpy((char *) str, (char *) &str[c]);
}

/**
 * Message Base JAM - Strip Leading & Tailing Spaces
 */
void stripleadtrail(uint8_t *str)
{
    striplead(str);
    striptrail(str);
}

BOOL jam_nomem;

/**
 * Message Base JAM - Add Fields to Jam Sub Pakcet.
 */
void jam_addfield(s_JamSubPacket *SubPacket_PS,uint32_t fieldnum,uint8_t *fielddata)
{
    s_JamSubfield    Subfield_S;
    Subfield_S.LoID   = fieldnum;
    Subfield_S.HiID   = 0;
    Subfield_S.DatLen = strlen((char *) fielddata);
    Subfield_S.Buffer = fielddata;

    if(JAM_PutSubfield(SubPacket_PS, &Subfield_S) == JAM_NO_MEMORY)
        jam_nomem=TRUE;
}

/**
 * Message Base JAM - JAM Flag Struct
 */
struct flag
{
    uint8_t *name;
    uint32_t jamflagbit;
    uint32_t fidoflagbit;
};

/**
 * Message Base JAM - JAM Flag Array
 */
struct flag jam_flagarray[] =
{
    { (uint8_t *) "PVT", MSG_PRIVATE,     FLAG_PVT         },
    { (uint8_t *) "HLD", MSG_HOLD,        FLAG_HOLD        },
    { (uint8_t *) "CRA", MSG_CRASH,       FLAG_CRASH       },
    { (uint8_t *) "K/S", MSG_KILLSENT,    FLAG_KILLSENT    },
    { (uint8_t *) "SNT", MSG_SENT,        FLAG_SENT        },
    { (uint8_t *) "RCV", MSG_READ,        FLAG_RECD        },
    { (uint8_t *) "A/S", MSG_ARCHIVESENT, 0,               },
    { (uint8_t *) "DIR", MSG_DIRECT,      0                },
    { (uint8_t *) "FIL", MSG_FILEATTACH,  FLAG_FILEATTACH  },
    { (uint8_t *) "FRQ", MSG_FILEREQUEST, FLAG_FILEREQ     },
    { (uint8_t *) "IMM", MSG_IMMEDIATE,   0                },
    { (uint8_t *) "KFS", MSG_KILLFILE,    0                },
    { (uint8_t *) "TFS", MSG_TRUNCFILE,   0                },
    { (uint8_t *) "LOK", MSG_LOCAL,       FLAG_LOCAL       },
    { (uint8_t *) "RRQ", MSG_RECEIPTREQ,  FLAG_RREQ        },
    { (uint8_t *) "CFM", MSG_CONFIRMREQ,  0                },
    { (uint8_t *) "FPU", MSG_FPU,         0                },
    { (uint8_t *) "",    MSG_INTRANSIT,   FLAG_INTRANSIT   },
    { (uint8_t *) "",    MSG_ORPHAN,      FLAG_ORPHAN      },
    { NULL,  0,               0                }
};

/**
 * Message Base JAM - JAM, Find and set Flags from Message Text
 */
uint32_t jam_findflag(uint8_t *name)
{
    int c;
    for(c=0; jam_flagarray[c].name; c++)
    {
        if(strncasecmp((char *) jam_flagarray[c].name, (char *) name, sizeof(jam_flagarray[c].name)) ==0)
            return (jam_flagarray[c].jamflagbit);
    }
    return (0);
}

/**
 * Message Base JAM - Copy Node Address into 4D Struct
 */
void Copy4D(struct Node4D *node1,struct Node4D *node2)
{
    node1->Zone=node2->Zone;
    node1->Net=node2->Net;
    node1->Node=node2->Node;
    node1->Point=node2->Point;
}

/**
 * Message Base JAM - Parse 4D Node Address for Match
 */
BOOL Parse4DTemplate(uint8_t *buf, struct Node4D *node,struct Node4D *tpl)
{
    uint32_t c   = 0;
    uint32_t val = 0;
    BOOL GotZone,GotNet,GotNode,GotVal;

    GotZone=FALSE;
    GotNet=FALSE;
    GotNode=FALSE;
    GotVal=FALSE;
    Copy4D(node,tpl);

    for(c=0; c<strlen((char *) buf); c++)
    {
        if(buf[c]==':')
        {
            if(GotZone || GotNet || GotNode) return (FALSE);

            if(GotVal) node->Zone=val;

            GotZone=TRUE;
            GotVal=FALSE;
        }
        else if(buf[c]=='/')
        {
            if(GotNet || GotNode) return (FALSE);

            if(GotVal) node->Net=val;

            GotNet=TRUE;
            GotVal=FALSE;
        }
        else if(buf[c]=='.')
        {
            if(GotNode) return (FALSE);

            if(GotVal) node->Node=val;

            GotNode=TRUE;
            GotVal=FALSE;
        }
        else if(buf[c]>='0' && buf[c]<='9')
        {
            if(!GotVal)
            {
                val=0;
                GotVal=TRUE;
            }

            val*=10;
            val+=buf[c]-'0';
        }
        else return (FALSE);
    }

    if(GotVal)
    {
        if(GotZone && !GotNet)  node->Net=val;
        else if(GotNode)        node->Point=val;
        else                    node->Node=val;
    }
    return (TRUE);
}

/**
 * Message Base JAM - Parse 4D Node
 */
BOOL Parse4D(uint8_t *buf, struct Node4D *node)
{
    struct Node4D tpl4d = { 0,0,0,0 };
    return Parse4DTemplate(buf,node,&tpl4d);
}

/**
 * Message Base JAM - Parse 5D Node with @ bbs
 */
BOOL Parse5D(uint8_t *buf, struct Node4D *n4d, uint8_t *domain)
{
    uint32_t c=0;
    uint8_t buf2[100];
    domain[0]=0;
    mystrncpy(buf2,buf,100);

    for(c=0; c<strlen((char *) buf2); c++)
        if(buf2[c]=='@') break;

    if(buf2[c]=='@')
    {
        buf2[c]=0;
        mystrncpy(domain,&buf2[c+1],20);
    }
    return Parse4D(buf2,n4d);
}

/**
 * Message Base JAM - Copy Node 4D string to String
 */
void Print4D(struct Node4D *n4d,uint8_t *dest)
{
    if(n4d->Point)
        sprintf((char *) dest,"%u:%u/%u.%u",n4d->Zone,
                n4d->Net,
                n4d->Node,
                n4d->Point);
    else
        sprintf((char *) dest,"%u:%u/%u",n4d->Zone,
                n4d->Net,
                n4d->Node);
}

/**
 * Message Base JAM - Dos Stamp to Local Time.
 */
time_t stampToTimeT(struct _stamp *st)
{
    time_t tt;
    struct tm tms;
    tms.tm_sec   = st->time.ss << 1;
    tms.tm_min   = st->time.mm;
    tms.tm_hour  = st->time.hh;
    tms.tm_mday  = st->date.da;
    tms.tm_mon   = st->date.mo - 1;
    tms.tm_year  = st->date.yr + 80;
    tms.tm_isdst = -1;
    tt = mktime(&tms);
    return tt;
}

/**
 * Message Base JAM - Local Time to DOS Stamp.
 */
struct _stamp *timeTToStamp(time_t tt)
{
    struct tm *tmsp;
    static struct _stamp st;
    tmsp = localtime(&tt);
    st.time.ss = tmsp->tm_sec >> 1;
    st.time.mm = tmsp->tm_min;
    st.time.hh = tmsp->tm_hour;
    st.date.da = tmsp->tm_mday;
    st.date.mo = tmsp->tm_mon + 1;
    st.date.yr = tmsp->tm_year - 80;
    return (&st);
}

/**
 * Message Base JAM - Close Current JAM Area
 */
void jam_closebase()
{
    if(jam_openbases) delete jam_openbases;
    return;
}

/**
 * Message Base JAM - Open Current JAM Area
 */
s_JamBase *jam_openbase(struct jam_Area *area)
{
//    int c;
    SESSION _s;
    std::string path = (char *) MESGPATH;
    path += (char *) area->area->mbfile;

//   _s.errlog((char *)"OpenMB: JAM_OpenMB - jam_openbases.");
    jam_openbases = new openbase;
    if(!jam_openbases)
    {
//      _s.errlog((char *)"OpenMB: JAM_OpenMB - !jam_openbases. - No Memory!");
        return NULL;
    }

//   _s.errlog((char *)"OpenMB: JAM_OpenMB");
    if(JAM_OpenMB((uint8_t *) path.c_str(),&jam_openbases->Base_PS))
    {
//      _s.errlog((char *)"OpenMB: !JAM_OpenMB");
        if(jam_openbases->Base_PS)
        {
//          _s.errlog((char *)"OpenMB: free(thisbase->Base_PS);");
            free(jam_openbases->Base_PS);
        }

        //printf("Creating JAM messagebase \"%s\"",(char *)path.c_str());
//      _s.errlog((char *)"OpenMB: JAM_CreateMB: %s", (char *)path.c_str());
        if(JAM_CreateMB((uint8_t *) path.c_str(),1,&jam_openbases->Base_PS))
        {
//         _s.errlog((char *)"OpenMB: !JAM_CreateMB");
            if(jam_openbases->Base_PS)
            {
//            _s.errlog((char *)"OpenMB: free(thisbase->Base_PS);");
                free(jam_openbases->Base_PS);
            }

            printf("Failed to create JAM messagebase \"%s\"", (char *) path.c_str());
            delete jam_openbases;
            jam_openbases = 0;
            return (NULL);
        }
    }

//   _s.errlog((char *)"OpenMB:  return(thisbase->Base_PS);");
    // Set the rest
    //jam_openbases->lastuse=jam_lastnum++;
    jam_openbases->area=area;
    return (jam_openbases->Base_PS);
}

/**
 * Message Base JAM - Setup and Read Current JAM Area
 */
struct jam_Area *jam_getarea(mb_list_rec *area)
{
    SESSION _s;

//    _s.errlog((char*)"6. jam_getarea(): ");
    struct jam_Area *ja;
    uint32_t num;
    s_JamBaseHeader Header_S;

    std::string path = (char *) MESGPATH;
    path += (char *) area->mbfile;
//    _s.errlog((char*)"6. jam_getarea(): %s",(char *)path.c_str());

    // This is the first time we use this area
    ja = new jam_Area;
    if(!ja)    //(jam_Area *)osAllocCleared(sizeof(struct jam_Area))))
    {
//        _s.errlog((char*)"6. jam_getarea(): !ja");
        //nomem=TRUE;
        return (FALSE);
    }

//   jbAddNode(&jam_AreaList,(struct jbNode *)ja);
//    _s.errlog((char*)"6. jam_getarea(): ja->area=area;");
    ja->area=area;

//    _s.errlog((char*)"6. jam_getarea(): jam_openbase;");
    if(!(ja->Base_PS=jam_openbase(ja)))
    {
//       _s.errlog((char*)"6. jam_getarea(): !jam_openbase;");
        if(ja) delete ja;
        ja = 0;
        return (NULL);
    }

//    _s.errlog((char*)"6. jam_getarea(): JAM_GetMBSize; Total # of Messages");
    if(JAM_GetMBSize(ja->Base_PS,&num))
    {
//       _s.errlog((char*)"6. jam_getarea(): !JAM_GetMBSize;");
        printf("Failed to get # of Message in JAM area \"%s\"", (char *) path.c_str());
        JAM_CloseMB(ja->Base_PS);
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (NULL);
    }


//   _s.errlog((char*)"6. jam_getarea(): JAM_ReadMBHeader; NUM: %lu",num);
    if(JAM_ReadMBHeader(ja->Base_PS,&Header_S))
    {
        // Area with 0 messages.. not a problem!!
//      _s.errlog((char*)"6. jam_getarea(): !JAM_ReadMBHeader;");
        JAM_CloseMB(ja->Base_PS);
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (NULL);
    }

///////////////////////////////
//   Problem Area should rewrok this set to 0 as default for all bases
//   Then use as need to skip starting messages.
//
//   ja->BaseNum=Header_S.BaseMsgNum; // Count of All Messages.
//
//     This is when messages are packed from 1020 to 1000, Header_S.BaseMsgNum;
//     will pad 20 onto select message numvber to avoid recount on all messages
//   This is stupid and will not be used.
//
//////////////////////////////
    ja->BaseNum = 0;
    ja->Active=Header_S.ActiveMsgs;  // Excluding Delete

//    _s.errlog((char*)" *** jam_getarea(): JAM_ReadMBHeader Num:%lu ja->BaseNum:%lu ja->Active:%lu",num, ja->BaseNum,ja->Active);
    //ja->OldHighWater=0;
    //ja->HighWater=0;
    ja->HighWater = num; //ja->num;
//    _s.errlog((char*)"6. jam_getarea(): return ja!; ja->HighWater: %ld, ja->BaseNum: %ld",num,ja->BaseNum);
    return (ja);
}

/**
 * Message Base JAM - Delete Message in Current Area
 */
BOOL jamapi_purgemsg(mb_list_rec *area, uint32_t msgnum)
{
    SESSION _s;
    struct jam_Area *ja;
    int res;

//    _s.errlog((char *)"jamapi_purgemsg() - jam_getarea()");
    if(!(ja=jam_getarea(area)))
    {
//      _s.errlog((char *)"jamapi_purgemsg() - !jam_getarea()");
        return (FALSE);
    }
    --msgnum; // Messages Start at 0, 1 through HighWater are passed.

//    _s.errlog((char *)"jamapi_purgemsg() - LockMB");
    std::string path = (char *) MESGPATH;
    path += (char *) area->mbfile;

    if(JAM_LockMB(ja->Base_PS,10))
    {
//      _s.errlog((char *)"Timeout when trying to lock JAM messagebase \"%s\"",(char *)path.c_str());
        JAM_UnlockMB(ja->Base_PS);
        JAM_CloseMB(ja->Base_PS);
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (FALSE);
    }

//    _s.errlog((char *)"Deleting message in area :  \"%s\" %lu",(char *)path.c_str(),msgnum);
    res = JAM_DeleteMessage(ja->Base_PS, msgnum);
    if(res)
    {
//        _s.errlog((char *)"jamapi_purgemsg() - !JAM_DeleteMessage()");
    }

    JAM_UnlockMB(ja->Base_PS);
//   _s.errlog((char *)"jamapi_purgemsg() - Finished");
    JAM_CloseMB(ja->Base_PS);
    free(ja->Base_PS);
    delete ja;
    ja = 0;
    return (FALSE);
}

/**
 * Message Base JAM - Save/ Write Message in Current Area
 */
BOOL jamapi_writemsg(struct MemMessage *mm, mb_list_rec *area)
{
    //struct TextChunk *chunk;
//    struct Path *pathnode;
    struct jam_Area *ja;
    s_JamSubPacket*    SubPacket_PS;
    s_JamMsgHeader    Header_S;
    uint8_t buf[100]= {0},newflags[100]= {0},flag[10]= {0};
    uint32_t c,f,jbcpos,linebegin,linelen;
    uint8_t *msgtext;
    uint32_t msgsize, msgpos;
    int res;

    SESSION _s;
    // Get an area to write to
//   _s.errlog((char *)"4. MB_JAM() - jam_getarea()");

    if(!(ja=jam_getarea(area)))
    {
//      _s.errlog((char *)"4. MB_JAM() - !jam_getarea()");
        return (FALSE);
    }

    // Start import
//   _s.errlog((char *)"4. MB_JAM() - JAM_ClearMsgHeader()");
    ja->newmsg=TRUE;
    JAM_ClearMsgHeader(&Header_S);

    if(!(SubPacket_PS = JAM_NewSubPacket()))
    {
        //   nomem=TRUE;
//       _s.errlog((char *)"4. MB_JAM() - !JAM_NewSubPacket()");
        JAM_CloseMB(ja->Base_PS);
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (FALSE);
    }
    // Allocate memory to store message text in
    msgpos=0;
    msgsize=0;

    // for(chunk=(struct TextChunk *)mm->TextChunks.First;chunk;chunk=chunk->Next)
//      msgsize+=chunk->Length;

//   _s.errlog((char *)"4. MB_JAM() - TextChunks.size()");
    msgsize = (uint32_t) mm->TextChunks.size();
    if(msgsize != 0)
    {
        msgtext = new uint8_t [msgsize]; //  (uint8_t *)osAlloc(msgsize)))
        if(!msgtext)
        {
            printf("%s", (char *) "Out of memory: msgtext");
            JAM_DelSubPacket(SubPacket_PS);
            JAM_CloseMB(ja->Base_PS);
            free(ja->Base_PS);
            delete ja;
            ja = 0;
            return (FALSE);
        }
    }
    else   // Can;t save message with no text!
    {
        JAM_DelSubPacket(SubPacket_PS);
        JAM_CloseMB(ja->Base_PS);
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (FALSE);
    }


//   _s.errlog((char *)"3. MB_JAM() - Do Header");
    // Do header
    // Header_S.DateProcessed = NULL;
    // Header_S.DateWritten = time(NULL); //FidoToTime(mm->DateTime);

    // Damned time zones... dates should be in local time in JAM
    Header_S.DateProcessed = 0L; //NULL; //mm->DateTime; // Processed for Crashmail!
    Header_S.DateWritten = mm->DateTime;
    Header_S.Cost = mm->Cost;
    Header_S.MsgIdCRC = JAM_Crc32(mm->MSGID,strlen((char *) mm->MSGID));
    Header_S.ReplyCRC = JAM_Crc32(mm->REPLY,strlen((char *) mm->REPLY));
    jam_addfield(SubPacket_PS,JAMSFLD_MSGID,mm->MSGID);
    jam_addfield(SubPacket_PS,JAMSFLD_REPLYID,mm->REPLY);
    jam_addfield(SubPacket_PS,JAMSFLD_PID, (uint8_t *) BBSVERSION);
//   _s.errlog((char *)"Write: F: %s, T: %s, S: %s",(char *)mm->From,(char *)mm->To,(char *)mm->Subject);

    // Add header fields
    if(mm->From[0])
        jam_addfield(SubPacket_PS,JAMSFLD_SENDERNAME, (uint8_t *) mm->From);

    if(mm->To[0])
        jam_addfield(SubPacket_PS,JAMSFLD_RECVRNAME, (uint8_t *) mm->To);

    if(mm->Subject[0])
        jam_addfield(SubPacket_PS,JAMSFLD_SUBJECT, (uint8_t *) mm->Subject);

//   _s.errlog((char *)"3. MB_JAM() - Setup Netmail");
    if(area->Kind == NETMAIL)
    {
        // Addresses in netmail //
        Print4D(&mm->OrigNode,buf);
        jam_addfield(SubPacket_PS,JAMSFLD_OADDRESS,buf);
        Print4D(&mm->DestNode,buf);
        jam_addfield(SubPacket_PS,JAMSFLD_DADDRESS,buf);
    }
    else
    {
        // Addresses in echomail //
        Print4D(&mm->OrigNode,buf);
        jam_addfield(SubPacket_PS,JAMSFLD_OADDRESS,buf);
    }

    // Header attributes //
    for(c=0; jam_flagarray[c].name; c++)
        if(mm->Attr & jam_flagarray[c].fidoflagbit)
            Header_S.Attribute |= jam_flagarray[c].jamflagbit;

    if(mm->Attr & FLAG_FILEATTACH)
    {
        Header_S.Attribute |= MSG_FILEATTACH;
        c=0;
        while(mm->Subject[c]!=0)
        {
            f=0;
            while(mm->Subject[c]!=0 && mm->Subject[c]!=32 && mm->Subject[c]!=',' && f<80)
                buf[f++]=mm->Subject[c++];

            buf[f]=0;
            while(mm->Subject[c]==32 || mm->Subject[c]==',') c++;
            if(buf[0]!=0)
                jam_addfield(SubPacket_PS,JAMSFLD_ENCLFILE,buf);
        }
    }

    if(mm->Attr & FLAG_FILEREQ)
    {
        Header_S.Attribute |= MSG_FILEREQUEST;
        c=0;
        while(mm->Subject[c]!=0)
        {
            f=0;
            while(mm->Subject[c]!=0 && mm->Subject[c]!=32 && mm->Subject[c]!=',' && f<80)
                buf[f++]=mm->Subject[c++];

            buf[f]=0;
            while(mm->Subject[c]==32 || mm->Subject[c]==',') c++;
            if(buf[0]!=0)
                jam_addfield(SubPacket_PS,JAMSFLD_ENCLFREQ,buf);
        }
    }

    // Echomail/netmail attribute

    if(mm->Area[0]==0)
        Header_S.Attribute |= MSG_TYPENET;

    else
        Header_S.Attribute |= MSG_TYPEECHO;

    //for(chunk=(struct TextChunk *)mm->TextChunks.First;chunk;chunk=chunk->Next)
    for(c=0; c<mm->TextChunks.length();)
    {
        linebegin=msgpos;

        while(mm->TextChunks[c]!=13 && c<mm->TextChunks.length())
        {
            if(mm->TextChunks[c]!=10)
                msgtext[msgpos++]=mm->TextChunks[c];
            c++;
        }

        if(mm->TextChunks[c]==13 && c<mm->TextChunks.length())
            msgtext[msgpos++]=mm->TextChunks[c++];

        linelen=msgpos-linebegin;
        if(linelen!=0)
        {
            /*
            if(linelen>=5 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""PID:",5)==0)
            {
                mystrncpy(buf,&msgtext[linebegin+5],AMIN(100,linelen-5));
                stripleadtrail(buf);
                jam_addfield(SubPacket_PS,JAMSFLD_PID,buf);
                msgpos=linebegin;
            }
            else if(linelen>=7 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""MSGID:",7)==0)
            {
                mystrncpy(buf,&msgtext[linebegin+7],AMIN(100,linelen-7));
                stripleadtrail(buf);
                jam_addfield(SubPacket_PS,JAMSFLD_MSGID,buf);
                Header_S.MsgIdCRC=JAM_Crc32(buf,strlen((char *)buf));
                msgpos=linebegin;
            }
            else if(linelen>=7 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""REPLY:",7)==0)
            {
                mystrncpy(buf,&msgtext[linebegin+7],AMIN(100,linelen-7));
                stripleadtrail(buf);
                jam_addfield(SubPacket_PS,JAMSFLD_REPLYID,buf);
                Header_S.ReplyCRC=JAM_Crc32(buf,strlen((char *)buf));
                msgpos=linebegin;
            }
            else */
            if(linelen>=7 && strncmp((char *) &msgtext[linebegin], (char *) "\x01""FLAGS:",7) ==0)
            {
                mystrncpy(buf,&msgtext[linebegin+7],AMIN(100,linelen-7));
                stripleadtrail(buf);
                jbcpos=0;
                newflags[0]=0;
                while(jbstrcpy(flag,buf,10,&jbcpos))
                {
                    uint32_t flagbit;
                    if((flagbit=jam_findflag(flag)))
                    {
                        Header_S.Attribute |= flagbit;
                    }
                    else
                    {
                        strcat((char *) newflags, (char *) flag);
                        strcat((char *) newflags, (char *) " ");
                    }
                }

                stripleadtrail(newflags);
                if(newflags[0]!=0)
                    jam_addfield(SubPacket_PS,JAMSFLD_FLAGS,newflags);

                msgpos=linebegin;
            }
            else if(linelen>=5 && strncmp((char *) &msgtext[linebegin], (char *) "\x01""INTL",5) ==0)
            {
                // Remove this kludge
                msgpos=linebegin;
            }
            else if(linelen>=5 && strncmp((char *) &msgtext[linebegin], (char *) "\x01""TOPT",5) ==0)
            {
                // Remove this kludge
                msgpos=linebegin;
            }
            else if(linelen>=5 && strncmp((char *) &msgtext[linebegin], (char *) "\x01""FMPT",5) ==0)
            {
                // Remove this kludge
                msgpos=linebegin;
            }
            else if(msgtext[linebegin]==1)
            {
                mystrncpy(buf,&msgtext[linebegin+1],AMIN(100,linelen-1));
                stripleadtrail(buf);
                jam_addfield(SubPacket_PS,JAMSFLD_FTSKLUDGE,buf);
                msgpos=linebegin;
            }
        }
    }

    // Seen-by
    /*
       if(config.cfg_Flags & CFG_IMPORTSEENBY)
       {
          uint8_t *buf;
          uint32_t c,d;

          if((buf=mmMakeSeenByBuf(&mm->SeenBy)))
          {
             c=0;

             while(buf[c]!=0)
             {
                d=c;

                while(buf[d]!=0 && buf[d]!=13)
                   d++;

                if(buf[d]==13)
                {
                   buf[d++]=0;
                   jam_addfield(SubPacket_PS,JAMSFLD_SEENBY2D,&buf[c+9]);
                }

                c=d;
             }
          }

          osFree(buf);
       }

       // Path

       for(pathnode=(struct Path *)mm->Path.First;pathnode;pathnode=pathnode->Next)
          for(c=0;c<pathnode->Paths;c++)
             jam_addfield(SubPacket_PS,JAMSFLD_PATH2D,pathnode->Path[c]);

       if(jam_nomem)
       {
          LogWrite(1,SYSTEMERR,(uint8_t *)"Out of memory");
          JAM_DelSubPacket(SubPacket_PS);
          if(msgsize) osFree(msgtext);
          return(FALSE);
       }
    */


    //Write message

    // Don't post empty messages. check before locking.
    if(msgsize == 0)
    {
//        _s.errlog((char *)"3. MB_JAM() - msgsize == 0");
        JAM_CloseMB(ja->Base_PS);
        delete msgtext;
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (FALSE);
    }


//   _s.errlog((char *)"3. MB_JAM() - LockMB");
    std::string path = (char *) MESGPATH;
    path += (char *) area->mbfile;

    if(JAM_LockMB(ja->Base_PS,10))
    {
//      _s.errlog((char *)"3. MB_JAM() - Timedout!");
        printf("Timeout when trying to lock JAM messagebase \"%s\"", (char *) path.c_str());
        JAM_UnlockMB(ja->Base_PS);
        JAM_DelSubPacket(SubPacket_PS);
        JAM_CloseMB(ja->Base_PS);
        delete msgtext;
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (FALSE);
    }

//   _s.errlog((char *)"3. MB_JAM() - AddMessage!");
    res=JAM_AddMessage(ja->Base_PS,&Header_S,SubPacket_PS,msgtext,msgpos);

//   _s.errlog((char *)"3. MB_JAM() - UnLockMB");
    JAM_UnlockMB(ja->Base_PS);
    JAM_DelSubPacket(SubPacket_PS);
    delete msgtext;

    if(res)
    {
//      _s.errlog((char *)"3. MB_JAM() - Failed to Write Message.");
        printf("Failed to write message to JAM messagebase \"%s\"", (char *) path.c_str());
        JAM_CloseMB(ja->Base_PS);
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (FALSE);
    }
    //uint32_t HighWater;
    // Set and Get HighWater For Area
    //jam_gethighwater(ja);
    //jam_writehighwater(ja);

    ja->HighWater += 1;
    JAM_CloseMB(ja->Base_PS);
    free(ja->Base_PS);
    delete ja;
    ja = 0;
    return (TRUE);
}

/**
 * Message Base JAM - Edit Existing/ Re-Write Message in Current Area
 */
BOOL jamapi_editmsg(struct MemMessage *mm, mb_list_rec *area)
{
    //struct TextChunk *chunk;
//    struct Path *pathnode;
    struct jam_Area *ja;
    s_JamSubPacket*    SubPacket_PS;
    s_JamMsgHeader    Header_S;
    uint8_t buf[100]= {0},newflags[100]= {0},flag[10]= {0};
    uint32_t c,f,jbcpos,linebegin,linelen;
    uint8_t *msgtext;
    uint32_t msgsize,msgpos;
    int res;

    SESSION _s;

    // Get an area to write to
//   _s.errlog((char *)"4. MB_JAM() - jam_getarea()");
    if(!(ja=jam_getarea(area)))
    {
//      _s.errlog((char *)"4. MB_JAM() - !jam_getarea()");
        return (FALSE);
    }

    // Start import
//   _s.errlog((char *)"4. MB_JAM() - JAM_ClearMsgHeader()");
    ja->newmsg=TRUE;
    JAM_ClearMsgHeader(&Header_S);
    if(!(SubPacket_PS = JAM_NewSubPacket()))
    {
        //   nomem=TRUE;
//       _s.errlog((char *)"4. MB_JAM() - !JAM_NewSubPacket()");
        JAM_CloseMB(ja->Base_PS);
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (FALSE);
    }

    // Allocate memory to store message text in
    msgpos=0;
    msgsize=0;

    // for(chunk=(struct TextChunk *)mm->TextChunks.First;chunk;chunk=chunk->Next)
//      msgsize+=chunk->Length;

//   _s.errlog((char *)"4. MB_JAM() - TextChunks.size()");
    msgsize = (uint32_t) mm->TextChunks.size();

    // Later on convert this to string for unicode support aginst char *;)
    if(msgsize != 0)
    {
        msgtext = new uint8_t [msgsize]; //  (uint8_t *)osAlloc(msgsize)))
        if(!msgtext)
        {
            printf("%s", (char *) "Out of memory: msgtext");
            JAM_DelSubPacket(SubPacket_PS);
            JAM_CloseMB(ja->Base_PS);
            free(ja->Base_PS);
            delete ja;
            ja = 0;
            return (FALSE);
        }
    }
    else   // Can;t save message with no text!
    {
        JAM_DelSubPacket(SubPacket_PS);
        JAM_CloseMB(ja->Base_PS);
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (FALSE);
    }

//   _s.errlog((char *)"3. MB_JAM() - Do Header");
    // Do header
    // Header_S.DateProcessed = NULL;
    // Header_S.DateWritten = time(NULL); //FidoToTime(mm->DateTime);

    // Damned time zones... dates should be in local time in JAM
    Header_S.DateProcessed = 0L; //NULL; //mm->DateTime; // Processed for Crashmail!
    Header_S.DateWritten = mm->DateTime;
    Header_S.Cost = mm->Cost;
    Header_S.MsgIdCRC = JAM_Crc32(mm->MSGID,strlen((char *) mm->MSGID));
    Header_S.ReplyCRC = JAM_Crc32(mm->REPLY,strlen((char *) mm->REPLY));
    jam_addfield(SubPacket_PS,JAMSFLD_MSGID,mm->MSGID);
    jam_addfield(SubPacket_PS,JAMSFLD_REPLYID,mm->REPLY);
    jam_addfield(SubPacket_PS,JAMSFLD_PID, (uint8_t *) BBSVERSION);

//   _s.errlog((char *)"Write: F: %s, T: %s, S: %s",(char *)mm->From,(char *)mm->To,(char *)mm->Subject);
    // Add header fields
    if(mm->From[0])
        jam_addfield(SubPacket_PS,JAMSFLD_SENDERNAME, (uint8_t *) mm->From);

    if(mm->To[0])
        jam_addfield(SubPacket_PS,JAMSFLD_RECVRNAME, (uint8_t *) mm->To);

    if(mm->Subject[0])
        jam_addfield(SubPacket_PS,JAMSFLD_SUBJECT, (uint8_t *) mm->Subject);

//   _s.errlog((char *)"3. MB_JAM() - Setup Netmail");
    if(area->Kind == NETMAIL)
    {
        // Addresses in netmail //
        Print4D(&mm->OrigNode,buf);
        jam_addfield(SubPacket_PS,JAMSFLD_OADDRESS,buf);
        Print4D(&mm->DestNode,buf);
        jam_addfield(SubPacket_PS,JAMSFLD_DADDRESS,buf);
    }
    else
    {
        // Addresses in echomail //
        Print4D(&mm->OrigNode,buf);
        jam_addfield(SubPacket_PS,JAMSFLD_OADDRESS,buf);
    }

    // Header attributes //
    for(c=0; jam_flagarray[c].name; c++)
        if(mm->Attr & jam_flagarray[c].fidoflagbit)
            Header_S.Attribute |= jam_flagarray[c].jamflagbit;

    if(mm->Attr & FLAG_FILEATTACH)
    {
        Header_S.Attribute |= MSG_FILEATTACH;
        c=0;
        while(mm->Subject[c]!=0)
        {
            f=0;
            while(mm->Subject[c]!=0 && mm->Subject[c]!=32 && mm->Subject[c]!=',' && f<80)
                buf[f++]=mm->Subject[c++];

            buf[f]=0;
            while(mm->Subject[c]==32 || mm->Subject[c]==',') c++;
            if(buf[0]!=0)
                jam_addfield(SubPacket_PS,JAMSFLD_ENCLFILE,buf);
        }
    }

    if(mm->Attr & FLAG_FILEREQ)
    {
        Header_S.Attribute |= MSG_FILEREQUEST;
        c=0;
        while(mm->Subject[c]!=0)
        {
            f=0;
            while(mm->Subject[c]!=0 && mm->Subject[c]!=32 && mm->Subject[c]!=',' && f<80)
                buf[f++]=mm->Subject[c++];

            buf[f]=0;
            while(mm->Subject[c]==32 || mm->Subject[c]==',') c++;
            if(buf[0]!=0)
                jam_addfield(SubPacket_PS,JAMSFLD_ENCLFREQ,buf);
        }
    }

    // Echomail/netmail attribute
    if(mm->Area[0]==0)
        Header_S.Attribute |= MSG_TYPENET;

    else
        Header_S.Attribute |= MSG_TYPEECHO;

    //for(chunk=(struct TextChunk *)mm->TextChunks.First;chunk;chunk=chunk->Next)
    for(c=0; c<mm->TextChunks.length();)
    {
        linebegin=msgpos;
        while(mm->TextChunks[c]!=13 && c<mm->TextChunks.length())
        {
            if(mm->TextChunks[c]!=10)
                msgtext[msgpos++]=mm->TextChunks[c];
            c++;
        }

        if(mm->TextChunks[c]==13 && c<mm->TextChunks.length())
            msgtext[msgpos++]=mm->TextChunks[c++];

        linelen=msgpos-linebegin;
        if(linelen!=0)
        {
            /*
            if(linelen>=5 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""PID:",5)==0)
            {
                mystrncpy(buf,&msgtext[linebegin+5],AMIN(100,linelen-5));
                stripleadtrail(buf);
                jam_addfield(SubPacket_PS,JAMSFLD_PID,buf);
                msgpos=linebegin;
            }
            else if(linelen>=7 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""MSGID:",7)==0)
            {
                mystrncpy(buf,&msgtext[linebegin+7],AMIN(100,linelen-7));
                stripleadtrail(buf);
                jam_addfield(SubPacket_PS,JAMSFLD_MSGID,buf);
                Header_S.MsgIdCRC=JAM_Crc32(buf,strlen((char *)buf));
                msgpos=linebegin;
            }
            else if(linelen>=7 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""REPLY:",7)==0)
            {
                mystrncpy(buf,&msgtext[linebegin+7],AMIN(100,linelen-7));
                stripleadtrail(buf);
                jam_addfield(SubPacket_PS,JAMSFLD_REPLYID,buf);
                Header_S.ReplyCRC=JAM_Crc32(buf,strlen((char *)buf));
                msgpos=linebegin;
            }
            else */
            if(linelen>=7 && strncmp((char *) &msgtext[linebegin], (char *) "\x01""FLAGS:",7) ==0)
            {
                mystrncpy(buf,&msgtext[linebegin+7],AMIN(100,linelen-7));
                stripleadtrail(buf);
                jbcpos=0;
                newflags[0]=0;
                while(jbstrcpy(flag,buf,10,&jbcpos))
                {
                    uint32_t flagbit;
                    if((flagbit=jam_findflag(flag)))
                    {
                        Header_S.Attribute |= flagbit;
                    }
                    else
                    {
                        strcat((char *) newflags, (char *) flag);
                        strcat((char *) newflags, (char *) " ");
                    }
                }
                stripleadtrail(newflags);
                if(newflags[0]!=0)
                    jam_addfield(SubPacket_PS,JAMSFLD_FLAGS,newflags);
                msgpos=linebegin;
            }
            else if(linelen>=5 && strncmp((char *) &msgtext[linebegin], (char *) "\x01""INTL",5) ==0)
            {
                // Remove this kludge
                msgpos=linebegin;
            }
            else if(linelen>=5 && strncmp((char *) &msgtext[linebegin], (char *) "\x01""TOPT",5) ==0)
            {
                // Remove this kludge
                msgpos=linebegin;
            }
            else if(linelen>=5 && strncmp((char *) &msgtext[linebegin], (char *) "\x01""FMPT",5) ==0)
            {
                // Remove this kludge
                msgpos=linebegin;
            }
            else if(msgtext[linebegin]==1)
            {
                mystrncpy(buf,&msgtext[linebegin+1],AMIN(100,linelen-1));
                stripleadtrail(buf);
                jam_addfield(SubPacket_PS,JAMSFLD_FTSKLUDGE,buf);
                msgpos=linebegin;
            }
        }
    }
    // Seen-by
    /*
       if(config.cfg_Flags & CFG_IMPORTSEENBY)
       {
          uint8_t *buf;
          uint32_t c,d;

          if((buf=mmMakeSeenByBuf(&mm->SeenBy)))
          {
             c=0;

             while(buf[c]!=0)
             {
                d=c;

                while(buf[d]!=0 && buf[d]!=13)
                   d++;

                if(buf[d]==13)
                {
                   buf[d++]=0;
                   jam_addfield(SubPacket_PS,JAMSFLD_SEENBY2D,&buf[c+9]);
                }

                c=d;
             }
          }

          osFree(buf);
       }

       // Path

       for(pathnode=(struct Path *)mm->Path.First;pathnode;pathnode=pathnode->Next)
          for(c=0;c<pathnode->Paths;c++)
             jam_addfield(SubPacket_PS,JAMSFLD_PATH2D,pathnode->Path[c]);

       if(jam_nomem)
       {
          LogWrite(1,SYSTEMERR,(uint8_t *)"Out of memory");
          JAM_DelSubPacket(SubPacket_PS);
          if(msgsize) osFree(msgtext);
          return(FALSE);
       }
    */
    //Write message
    // Don't post empty messages. check before locking.
    if(msgsize == 0)
    {
//        _s.errlog((char *)"3. MB_JAM() - msgsize == 0");
        JAM_CloseMB(ja->Base_PS);
        delete msgtext;
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (FALSE);
    }
//   _s.errlog((char *)"3. MB_JAM() - LockMB");
    std::string path = (char *) MESGPATH;
    path += (char *) area->mbfile;

    if(JAM_LockMB(ja->Base_PS,10))
    {
//      _s.errlog((char *)"3. MB_JAM() - Timedout!");
        printf("Timeout when trying to lock JAM messagebase \"%s\"", (char *) path.c_str());
        JAM_UnlockMB(ja->Base_PS);
        JAM_DelSubPacket(SubPacket_PS);
        JAM_CloseMB(ja->Base_PS);
        delete msgtext;
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (FALSE);
    }

//   _s.errlog((char *)"3. MB_JAM() - AddMessage!");
    res=JAM_AddMessage(ja->Base_PS,&Header_S,SubPacket_PS,msgtext,msgpos);
//   _s.errlog((char *)"3. MB_JAM() - UnLockMB");
    JAM_UnlockMB(ja->Base_PS);
    JAM_DelSubPacket(SubPacket_PS);
    delete msgtext;

    if(res)
    {
//      _s.errlog((char *)"3. MB_JAM() - Failed to Write Message.");
        printf("Failed to write message to JAM messagebase \"%s\"", (char *) path.c_str());
        JAM_CloseMB(ja->Base_PS);
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (FALSE);
    }

    //uint32_t HighWater;
    // Set and Get HighWater For Area
    //jam_gethighwater(ja);
    //jam_writehighwater(ja);

    ja->HighWater += 1;
    JAM_CloseMB(ja->Base_PS);
    free(ja->Base_PS);
    delete ja;
    ja = 0;
    return (TRUE);
}

/**
 * Message Base JAM - Extract Origin Line and Node Address from Message Text
 */
BOOL ExtractAddress(uint8_t *origin, struct Node4D *n4d)
{
    uint32_t pos,e;
    uint8_t addr[50];
    uint8_t domain[20];

    pos=strlen((char *) origin);
    while(pos>0 && origin[pos]!='(') pos--;

    /* Find address */
    if(origin[pos]!='(')
        return (FALSE);

    pos++;
    while(origin[pos]!=13 && (origin[pos]<'0' || origin[pos]>'9')) pos++;

    /* Skip (FidoNet ... ) */
    e=0;
    while(origin[pos]!=')' && e<49)
        addr[e++]=origin[pos++];

    addr[e]=0;
    return Parse5D(addr,n4d,domain);
}

/**
 * Message Base JAM - Read Kludge Lines from Message Text
 */
void ProcessKludge(struct MemMessage *mm,uint8_t *kludge)
{
    struct Node4D node;
    uint8_t buf[60];
    uint32_t c,d;
    /*
    if(strncmp((char *)kludge,(char *)"\x01RESCANNED",10)==0)
    {
        mm->Flags |= MMFLAG_RESCANNED;
    }*/

    /*
    if(strncmp((char *)kludge,(char *)"\x01MSGID:",7)==0)
    {
        for(d=0,c=8;d<79 && kludge[c]!=13 && kludge[c]!=0;c++,d++)
            mm->MSGID[d]=kludge[c];

        mm->MSGID[d]=0;
    }

    if(strncmp((char *)kludge,(char *)"\x01REPLY:",7)==0)
    {
        for(d=0,c=8;d<79 && kludge[c]!=13 && kludge[c]!=0;c++,d++)
            mm->REPLY[d]=kludge[c];

        mm->REPLY[d]=0;
    }
    */
    if(mm->Area[0]==0)
    {
        if(strncmp((char *) kludge, (char *) "\x01" "FMPT",5) ==0)
            mm->OrigNode.Point=atoi((char *) &kludge[6]);

        if(strncmp((char *) kludge, (char *) "\x01TOPT",5) ==0)
            mm->DestNode.Point=atoi((char *) &kludge[6]);

        if(strncmp((char *) kludge, (char *) "\x01INTL",5) ==0)
        {
            if(kludge[5]==':')
                c=7;
            else
                c=6;
            for(d=0; d<59 && kludge[c]!=32 && kludge[c]!=0; c++,d++)
                buf[d]=kludge[c];

            buf[d]=0;
            if(Parse4D(buf,&node))
            {
                mm->DestNode.Zone = node.Zone;
                mm->DestNode.Net  = node.Net;
                mm->DestNode.Node = node.Node;
            }

            if(kludge[c]==32) c++;
            for(d=0; d<59 && kludge[c]!=32 && kludge[c]!=0 && kludge[c]!=13; c++,d++)
                buf[d]=kludge[c];

            buf[d]=0;
            if(Parse4D(buf,&node))
            {
                mm->OrigNode.Zone = node.Zone;
                mm->OrigNode.Net  = node.Net;
                mm->OrigNode.Node = node.Node;
            }
        }
    }
}

/**
 * Message Base JAM - Read Raw Message and Populate Struct
 */
void mmAddLine(struct MemMessage *mm,uint8_t *buf)
{
    /*
    if(mm->Area[0] && strncmp((char *)buf,(char *)"SEEN-BY:",8)==0)
        return AddSeenby(&buf[9],&mm->SeenBy);

    else if(mm->Area[0] && strncmp((char *)buf,(char *)"\x01PATH:",6)==0)
        return mmAddPath(&buf[7],&mm->Path);
    */

    if(mm->Area[0] && strncmp((char *) buf, (char *) " * Origin: ",11) == 0)
    {
        struct Node4D n4d;
        if(ExtractAddress(buf,&n4d))
        {
            if(n4d.Zone == 0) n4d.Zone=mm->OrigNode.Zone;
            Copy4D(&mm->OrigNode,&n4d);
        }
    }

    else if(buf[0] == 1)
        ProcessKludge(mm,buf);

    mm->TextChunks.append((const char *) buf);
//    return mmAddBuf(&mm->TextChunks,buf,(uint32_t)strlen((char *)buf));
}

/**
 * Message Base JAM - Create Kludge Lines
 */
void jam_makekludge(struct MemMessage *mm,uint8_t *pre,uint8_t *data,uint32_t len)
{
    uint8_t *buf;
    buf = new uint8_t[strlen((char *) pre) +len+10];
    if(!buf) return;
    strcpy((char *) buf, (char *) pre);
    if(len && data) mystrncpy(&buf[strlen((char *) buf)],data,len+1);
    strcat((char *) buf, (char *) "\x0d");
    mmAddLine(mm,buf);
    delete [] buf;
}

/**
 * Message Base JAM - Count Message In Current Area
 */
uint32_t jamapi_countmsgs(mb_list_rec *area, UserRec *thisuser)
{
    int      res=0;
    uint32_t ret=0;
    uint32_t cnt=0;
    uint32_t crc=0;
    uint32_t idx=0;

    struct jam_Area *ja;
    SESSION _s;

    // Open the area
    if(!(ja=jam_getarea(area)))
    {
//       _s.errlog((char *)"4. MB_JAM() - ! countmsgs jam_getarea()");
        return 0;
    }

    // Get Messages only to this specific user.
    if(area->Pubpriv == PRIVATE)
    {
        crc = JAM_Crc32((uint8_t*) thisuser->handle, strlen((char *) thisuser->handle));
//        _s.errlog((char *)" **** 4. MB_JAM() - countmsgs JAM_FindUser: crc %lu : %s",crc,thisuser->handle);
        while(1)
        {
            // Private Area, we need to loop and pick out each message
            // to This user, ja->Active; will get all active for all users
            // And so the only way is to loop this area.
            res = JAM_FindUser(ja->Base_PS,
                               crc,
                               idx,
                               &ret);
            if(res)
            {
                JAM_CloseMB(ja->Base_PS);
                free(ja->Base_PS);
                delete ja;
                ja = 0;

                if(res == JAM_NO_USER)
                {
//                    _s.errlog((char *)"countmsgs return count %lu",cnt);
//                    _s.errlog((char *)"4. countmsgs JAM_FindUser() user CRC not found! ");
                    return (cnt);   // Users has no Messages that exists
                }
                else
                {
//                    _s.errlog((char *)"countmsgs return count %lu",cnt);
//                    _s.errlog((char *)"4. countmsgs JAM_FindUser() user ERROR not found! ");
                    return (cnt);   // Error Reading, Return count ie.. End of File.
                }
            }
            else
            {
                idx=ret+1; //found, move to next
                ++cnt;     // Incriment message count
                //cnt = ret;
            }
        }
    }
    else
    {
        // Not Email, return HighWater for Total.
        //        _s.errlog((char *)"4. countmsgs !email ja->Highwater");
        //cnt = ja->HighWater;
        // Activbe Messages?!?!?  otherwise need to keep track and
        // Rework how add and subtract on new and deleted!!?!?
        cnt = ja->Active;
    }

    // Close down area.
    JAM_CloseMB(ja->Base_PS);
    free(ja->Base_PS);
    delete ja;
    ja = 0;
//    _s.errlog((char *)"countmsgs return count %lu",cnt);
    return cnt;
}


/**
 * Message Base JAM - Builds Array of Actual Email/Netmail Messages, and grabs
 *                      Real Msg #, into an Vector Array for Translating List #
 *                       to Actual Message Number. Use to find message to (you), (them)
 */
vector<unsigned long> jamapi_build_private(mb_list_rec *area, UserRec *thisuser)
{
    vector<unsigned long> elist;
    unsigned long idx=0; // Messages Are 1 Based, not Zero.  Add as Idx+1
    int      res=0;
    uint32_t ret=0;
    uint32_t cnt=0;
    uint32_t crc=0; // Fix to check if real name or not!  Fido

    struct jam_Area *ja;
    SESSION _s;

    // Open the area
    if(!(ja = jam_getarea(area)))
    {
//       _s.errlog((char *)"4. jamapi_buildemail - !jam_getarea()");
        return elist;
    }

    // If Real Name Flag, ie Fido, Scan by maching Real Name insead of Handle.
    if((area->flags.mbrealname & 0x01) == 0)
    {
        crc = JAM_Crc32((uint8_t*) thisuser->handle, strlen((char *) thisuser->handle));
//        _s.errlog((char *)" **** 4. jamapi_buildemail - JAM_FindUser: crc handle %lu : %s",crc,thisuser->handle);
    }
    else
    {
        crc = JAM_Crc32((uint8_t*) thisuser->name, strlen((char *) thisuser->name));
//        _s.errlog((char *)" **** 4. jamapi_buildemail - JAM_FindUser: crc name %lu : %s",crc,thisuser->name);
    }

//    _s.errlog2((char *)"4.3 jamapi_build_private JAM_FindUser() idx %lu, ret %lu, res %i, cnt: %i ",idx, ret , res, cnt);
    while(1)
    {
        //Email loop, only cont messages written by them!
        res = JAM_FindUser(ja->Base_PS,
                           crc,
                           idx,
                           &ret);
        if(res)
        {
            JAM_CloseMB(ja->Base_PS);
            free(ja->Base_PS);
            delete ja;
            ja = 0;
            if(res == JAM_NO_USER)
            {
//                _s.errlog((char *)"4. jamapi_buildemail JAM_FindUser() user CRC not found! idx %lu, ret %lu, res %i ",idx,ret,res);
                return (elist);   // Messages no longer exists for this user
            }
            else
            {
//                _s.errlog((char *)"4. jamapi_buildemail JAM_FindUser() user ERROR not found! ");
                return (elist);   // Error Reading, Return count
            }
        }
        else
        {
            idx = ret+1; // found, move to next
            ++cnt;       // Incriment message count
            elist.push_back(ret+1);
//            _s.errlog((char *)"4. jamapi_buildemail JAM_FindUser() idx %lu, ret %lu, res %i ",idx,ret,res);
        }
    }
    // Close down area.
    JAM_CloseMB(ja->Base_PS);
    free(ja->Base_PS);
    delete ja;
    ja = 0;
    return elist;
}

/**
 * Message Base JAM - Build Message list, and send back vector of all valid messages
 *                       Skips any errors or deleted messages.
 */
vector<unsigned long> jamapi_build_public(mb_list_rec *area, UserRec *thisuser)
{
    vector<unsigned long> elist;
    MemMessage mm;

    int      res=0;
    unsigned long    idx=0; // Messages Are 1 Based, not Zero. Added idx +1.
//  uint32_t ret=0;
//  uint32_t cnt=0;
    struct jam_Area *ja;
    s_JamSubPacket* SubPacket_PS;
    s_JamMsgHeader  Header_S;
    SESSION _s;
//    msgs _msgf;

    /* Not needed anymore, more universal now!
        int areakind;
        if (area->Pubpriv == PRIVATE)
            areakind = TRUE;
        else
            areakind = FALSE;
    */

    // Open the area
    if(!(ja = jam_getarea(area)))
    {
//        _s.errlog2((char *)"4.1 jamapi_build_public - !jam_getarea()");
        return elist;
    }

    while(1)
    {
//        _s.errlog((char *)"4.2 jamapi_build_public JAM_ReadMsgHeader() idx %lu, ret %lu, res %i, cnt: %i ",idx,ret,res,cnt);
        res=JAM_ReadMsgHeader(ja->Base_PS, idx, &Header_S, &SubPacket_PS);
//        _s.errlog2((char *)"4.3 jamapi_build_public JAM_ReadMsgHeader() idx %lu, ret %lu, res %i, cnt: %i ",idx, ret , res, cnt);
        if(res)
        {
            if(res == JAM_NO_MESSAGE)
            {
//                _s.errlog2((char *)"4.4 jamapi_build_public JAM_ReadMsgHeader() JAM_NO_MESSAGE! idx %lu, ret %lu, res %i ",idx,ret,res);
                // Skip over and don't add to elist.
                ++idx;
                continue;
            }
            else
            {
                JAM_CloseMB(ja->Base_PS);
                free(ja->Base_PS);
                delete ja;
                ja = 0;
//                _s.errlog2((char *)"4.5 jamapi_build_public JAM_ReadMsgHeader() unable to read! - Done! ");
                return (elist);   // Error Reading, Return count
            }
        }
        else
        {
            // Make sure it's not deleted.
            if(Header_S.Attribute & MSG_DELETED)
            {
                ++idx;     // Incriment message count
//                _s.errlog2((char *)"4.6 jamapi_build_public MSG_DELETED idx %lu, ret %lu, res %i ",idx,ret,res);
                continue;
            }
            else
            {
                /* Extra insurance testing, was not needed!!!
                 * Headers Should have deleted flag, I did see one message where
                 * the To field was empty and was caught by readmsg!  Keep eye out if
                 * we need to re-enable this code block,
                 *
                // Make Sure message is not deleted and read status.
                res = jamapi_readmsg(area, idx, &mm, areakind, thisuser);
                if (res)
                {
                    // Check return value,  if messaege = NO_MESSAGE, then deleted, skip to next!
                    // And there are more messages in this area.
                    if (res == JAM_NO_MESSAGE && idx <= mm.HighWater)
                    {
                        _s.errlog2((char *)" *** jamapi_readmsg == JAM_NO_MESSAGE && num_titles < mm.HighWater");
                        idx++;
                        continue; // Skip and goto next message
                        //MessageDeleted(&mm); // Populated Generic Deleted Message.
                    }
                    else
                    {
                        _s.errlog2((char *)" *** !jamapi_readmsg() ");
                        idx++;
                        continue;  // Skip and goto next message
                        //MessageNotFound(&mm); // Populated Generic Deleted Message.
                    }
                }
                */
                elist.push_back(idx+1);
//                _s.errlog2((char *)"4.7 jamapi_build_public JAM_ReadMsgHeader() idx %lu, ret %lu, res %i,  ",idx,ret,res,cnt);
                ++idx;
                continue;
            }
        }
    }

    // Close down area.
    JAM_CloseMB(ja->Base_PS);
    free(ja->Base_PS);
    delete ja;
    ja = 0;
    return elist;
}

/**
 * Message Base JAM - Read Message in Currnet Area
 */
BOOL jamapi_readmsg(mb_list_rec *area, uint32_t num, struct MemMessage *mm, int email, UserRec *thisuser)
{
    //struct MemMessage *mm;
    struct jam_Area *ja;
    uint8_t *msgtext;
    uint8_t buf[200] = {0},
                       domain[20] = {0};
    int res,c;
    s_JamSubPacket* SubPacket_PS;
    s_JamMsgHeader  Header_S;
    s_JamSubfield*  Field_PS;
    struct Node4D n4d;
    BOOL hasaddr;
    uint8_t flagsbuf[200] = {0},
                            filesubject[200] = {0};
//    ushort oldattr;
    res = 0;
//    uint32_t ret = 0;

    SESSION _s;
    if(num != 0)
        --num;

    mm->HighWater = 0;
    mm->Active    = 0;
    mm->CurrMsg   = num;

    // Open the area
    if(!(ja=jam_getarea(area)))
    {
//      _s.errlog((char *)"4. MB_JAM() - !jam_getarea()");
        return (TRUE);
    }

//   _s.errlog((char *)"4. MB_JAM() - After jam_getarea(): BaseNum %lu, Num %lu, HighWater: %lu Active: %lu",ja->BaseNum,num,ja->HighWater,ja->Active);
    mm->HighWater = ja->HighWater; //+1;
    mm->Active    = ja->Active;

// If email, scan for first message to this user.
    /*
        int JAM_FindUser( s_JamBase*    Base_PS,
              uint32_t     UserCrc_I,
              uint32_t     StartMsg_I,
              uint32_t*     MsgNo_PI )*/

    // Read message header
    res=JAM_ReadMsgHeader(ja->Base_PS, num - ja->BaseNum, &Header_S, &SubPacket_PS);
    std::string path = (char *) MESGPATH;
    path += (char *) area->mbfile;
    if(res)
    {
        if(res == JAM_NO_MESSAGE)
        {
            JAM_CloseMB(ja->Base_PS);
            free(ja->Base_PS);
            delete ja;
            ja = 0;
//          _s.errlog((char *)"4. JAM_ReadMsgHeader() message no longer exists #%lu in JAM messagebase \"%s\"",num,(char *)path.c_str());
            return (res);   // Message no longer exists
        }
        else
        {
            JAM_CloseMB(ja->Base_PS);
            free(ja->Base_PS);
            delete ja;
            ja = 0;
//         _s.errlog((char *)"4. JAM_ReadMsgHeader()Failed to read message #%lu in JAM messagebase \"%s\" RES: %lu",num,(char *)path.c_str(), res);
            return (res);
        }
    }

    // Check if deleted
    if(Header_S.Attribute & MSG_DELETED)
    {
        // Message deleted
        JAM_CloseMB(ja->Base_PS);
        JAM_DelSubPacket(SubPacket_PS);
        free(ja->Base_PS);
        delete ja;
        ja = 0;
        return (TRUE);
    }
    // Check if already sent
    /*
        if(!isrescanning)
        {
            if((Header_S.Attribute & MSG_SENT) || !(Header_S.Attribute & MSG_LOCAL))
            {
                // Don't touch if the message is sent or not local
              JAM_DelSubPacket(SubPacket_PS);
                jam_closebase();
                 return(TRUE);
            }
       }
    */
    // Read message text
    msgtext=NULL;

    // Message Text
    if(Header_S.TxtLen)
    {
        //if(!(msgtext=(uint8_t *)osAlloc(Header_S.TxtLen+1))) // One extra byte for the terminating zero
        msgtext = new uint8_t[Header_S.TxtLen+1];
        if(!msgtext)
        {
//          _s.errlog((char *)"4. MB_JAM() Failed to allocate msgtext \"%lu in JAM messagebase \"%s\"",num,(char *)path.c_str());        // nomem=TRUE;
            JAM_DelSubPacket(SubPacket_PS);
            JAM_CloseMB(ja->Base_PS);
            free(ja->Base_PS);
            delete ja;
            ja = 0;
            return (TRUE);
        }

        // Read Just Message Text
        res=JAM_ReadMsgText(ja->Base_PS,Header_S.TxtOffset,Header_S.TxtLen,msgtext);

        if(res)
        {
//         _s.errlog((char *)"4. MB_JAM() Failed to read message #%lu in JAM messagebase \"%s\"",num,(char *)path.c_str());
            JAM_DelSubPacket(SubPacket_PS);
            JAM_CloseMB(ja->Base_PS);
            free(ja->Base_PS);
            delete ja;
            ja = 0;
            return (TRUE);
        }
        msgtext[Header_S.TxtLen]=0;
    }
    // Allocate message structure
    //if(!(mm=mmAlloc()))
    /*
    mm = new
       {
      JAM_DelSubPacket(SubPacket_PS);
      if(msgtext) osFree(msgtext);
      return(FALSE);
       }*/

    /*
       if(mr.Kind == NETMAIL) { //if(area->AreaType == AREATYPE_NETMAIL)
      strcpy((char *)mm->Area,(char *)"");

       else
      strcpy((char *)mm->Area,(char *)area->Tagname);
      */

    mm->msgnum=num;

    // Subfields
    flagsbuf[0]=0;
    filesubject[0]=0;
    hasaddr=FALSE;

    for(Field_PS=JAM_GetSubfield(SubPacket_PS); Field_PS; Field_PS=JAM_GetSubfield(NULL))
    {
        switch(Field_PS->LoID)
        {
            case JAMSFLD_OADDRESS:
                mystrncpy(buf,Field_PS->Buffer,Field_PS->DatLen+1);
                if(Parse5D(buf,&n4d,domain))
                {
                    mm->OrigNode.Zone=n4d.Zone;
                    mm->OrigNode.Net=n4d.Net;
                    mm->OrigNode.Node=n4d.Node;
                    mm->OrigNode.Point=n4d.Point;
                }
                break;

            case JAMSFLD_DADDRESS:
                mystrncpy(buf,Field_PS->Buffer,Field_PS->DatLen+1);
                if(hasaddr)
                {
//                _s.errlog((char *)"4. MB_JAM() Warning: Multiple DADDRESS not supported by CrashMail");
                }
                else
                {
                    hasaddr=TRUE;
                    if(Parse5D(buf,&n4d,domain))
                    {
                        mm->DestNode.Zone=n4d.Zone;
                        mm->DestNode.Net=n4d.Net;
                        mm->DestNode.Node=n4d.Node;
                        mm->DestNode.Point=n4d.Point;
                    }
                }
                break;

            case JAMSFLD_SENDERNAME:
                mystrncpy(buf,Field_PS->Buffer,Field_PS->DatLen+1);
                mystrncpy(mm->From,buf,36);
                break;

            case JAMSFLD_RECVRNAME:
                mystrncpy(buf,Field_PS->Buffer,Field_PS->DatLen+1);
                mystrncpy(mm->To,buf,36);
                break;

            case JAMSFLD_SUBJECT:
                mystrncpy(buf,Field_PS->Buffer,Field_PS->DatLen+1);
                mystrncpy(mm->Subject,buf,72);
                break;

                /*  Make this a sysop toggle.
                    WE are only reading a message.

                case JAMSFLD_MSGID:
                    jam_makekludge(mm,(uint8_t *)"\x01" "MSGID: ",Field_PS->Buffer,Field_PS->DatLen);
                    break;

                case JAMSFLD_REPLYID:
                    jam_makekludge(mm,(uint8_t *)"\x01" "REPLY: ",Field_PS->Buffer,Field_PS->DatLen);
                    break;

                case JAMSFLD_PID:
                    jam_makekludge(mm,(uint8_t *)"\x01" "PID: ",Field_PS->Buffer,Field_PS->DatLen);
                    break;

                case JAMSFLD_ENCLFILE:
                    if(filesubject[0]) {}//_s.errlog((char *)"4. MB_JAM() Warning: Multiple ENCLOSEDFILE not supported.");
                    else mystrncpy(filesubject,Field_PS->Buffer,Field_PS->DatLen+1);
                    break;

                case JAMSFLD_ENCLFREQ:
                //            _s.errlog((char *)"4. MB_JAM() Warning: ENCLOSEDFREQ not supported.");

                    break;

                case JAMSFLD_ENCLFWALIAS:
                //            _s.errlog((char *)"4. MB_JAM() Warning: ENCLOSEDFILEWALIAS not supported.");
                    break;

                case JAMSFLD_ENCLFILEWC:
                //            _s.errlog((char *)"4. MB_JAM() Warning: ENCLOSEDFILEWCARD with wildcards not supported.");
                    break;

                case JAMSFLD_ENCLINDFILE:
                //            _s.errlog((char *)"4. MB_JAM() Warning:  ENCLOSEDINDIRECTFILE not supported.");

                    break;

                case JAMSFLD_FTSKLUDGE:
                    jam_makekludge(mm,(uint8_t *)"\x01",Field_PS->Buffer,Field_PS->DatLen);
                    break;

                case JAMSFLD_SEENBY2D:
                    jam_makekludge(mm,(uint8_t *)"SEEN-BY: ",Field_PS->Buffer,Field_PS->DatLen);
                    break;

                case JAMSFLD_PATH2D:
                    jam_makekludge(mm,(uint8_t *)"\01" "PATH: ",Field_PS->Buffer,Field_PS->DatLen);
                    break;

                case JAMSFLD_FLAGS:
                    strcpy((char *)flagsbuf,(const char *)"\x01" "FLAGS: ");
                    mystrncpy(&flagsbuf[8],Field_PS->Buffer,Field_PS->DatLen+1);
                    // Don't add until attributes from header has been added
                    break;
                */
        }
    }

    if(filesubject[0])
    {
        mm->Attr|=FLAG_FILEATTACH;
        mystrncpy(mm->Subject,filesubject,72);
    }

    // Message header
    // MakeFidoDate(Header_S.DateWritten+jam_utcoffset,mm->DateTime);
    mm->DateTime = Header_S.DateWritten;
    mm->Cost=Header_S.Cost;

    for(c=0; jam_flagarray[c].name; c++)
    {
        if(Header_S.Attribute & jam_flagarray[c].jamflagbit)
        {
            if(jam_flagarray[c].fidoflagbit)
            {
                mm->Attr |= jam_flagarray[c].fidoflagbit;
            }
            else if(jam_flagarray[c].name[0] && strlen((const char *) flagsbuf) <90)
            {
                if(flagsbuf[0]==0) strcpy((char *) flagsbuf, (char *) "\x01" "FLAGS: ");
                else               strcat((char *) flagsbuf, (char *) " ");
                strcat((char *) flagsbuf, (char *) jam_flagarray[c].name);
            }
        }
    }
    if(flagsbuf[0])
    {
        strcat((char *) flagsbuf, (char *) "\x0d");
        mmAddLine(mm,buf);
    }

//    oldattr = mm->Attr;
    mm->Attr = mm->Attr & (FLAG_PVT|FLAG_CRASH|FLAG_FILEATTACH|FLAG_FILEREQ|FLAG_RREQ|FLAG_IRRR|FLAG_AUDIT|FLAG_HOLD);
    // Add own kludges
    /*
       if(area->AreaType == AREATYPE_NETMAIL)
       {
          if(mm->OrigNode.Zone != mm->DestNode.Zone )
          {
             sprintf((char *)buf,"\x01" "INTL %u:%u/%u %u:%u/%u\x0d",
                mm->DestNode.Zone,
                mm->DestNode.Net,
                mm->DestNode.Node,
                mm->OrigNode.Zone,
                mm->OrigNode.Net,
                mm->OrigNode.Node);
            mmAddLine(mm,buf);
          }
          if(mm->OrigNode.Point)
          {
             sprintf((char *)buf,"\x01" "FMPT %u\x0d",mm->OrigNode.Point);
             mmAddLine(mm,buf);
          }
          if(mm->DestNode.Point)
          {
             sprintf((char *)buf,"\x01" "TOPT %u\x0d",mm->DestNode.Point);
             mmAddLine(mm,buf);
          }
       }
    */

    /*
       if((config.cfg_Flags & CFG_ADDTID) && !isrescanning)
          AddTID(mm);
    */
    /*
       if(isrescanning)
       {
          sprintf((char *)buf,"\x01RESCANNED %u:%u/%u.%u\x0d",area->Aka->Node.Zone,
                                                      area->Aka->Node.Net,
                                                      area->Aka->Node.Node,
                                                      area->Aka->Node.Point);
          mmAddLine(mm,buf);
       }*/

    // Message text
    // make sure it's clear before reading new.
    if(msgtext)
    {
        // Extract origin address
        if(mm->Area[0])
        {
            uint32_t textpos,d;
            uint8_t originbuf[200];
            struct Node4D n4d;

            textpos=0;
            while(msgtext[textpos])
            {
                d=textpos;
                while(msgtext[d] != 13 && msgtext[d] != 0)
                    d++;
                if(msgtext[d] == 13)
                    d++;
                if(d-textpos > 11 && strncmp((char *) &msgtext[textpos], (char *) " * Origin: ",11) ==0)
                {
                    mystrncpy(originbuf,&msgtext[textpos],AMIN(d-textpos,200));
                    if(ExtractAddress(originbuf,&n4d))
                        Copy4D(&mm->OrigNode,&n4d);
                }
                textpos=d;
            }
        }
        mm->TextChunks.append((const char *) msgtext);
//       _s.errlog((char *)"mm->TextChunks");
//       _s.errlog((char *)mm->TextChunks.c_str());

        /*
              //if(!(mmAddBuf(&mm->TextChunks,msgtext,Header_S.TxtLen)))
              {
          JAM_DelSubPacket(SubPacket_PS);
          osFree(msgtext);
          mmFree(mm);
          return(FALSE);
              }*/
    }
    // Free JAM message

    delete [] msgtext;
    msgtext = 0;
    JAM_DelSubPacket(SubPacket_PS);

    // Message reading done
    /*
       if(isrescanning) mm->Flags |= MMFLAG_RESCANNED;
       else             mm->Flags |= MMFLAG_EXPORTED;

       if(!(*handlefunc)(mm))
       {
          mmFree(mm);
          return(FALSE);
       }
       if(!isrescanning)
       {
          scan_total++;

           // Update message header

            if(config.cfg_Flags & CFG_ALLOWKILLSENT)
            {
                if((oldattr & FLAG_KILLSENT) && (area->AreaType == AREATYPE_NETMAIL))
                {
                    // Delete message with KILLSENT flag
                _s.errlog((char *)"4. MB_JAM() Deleting message with KILLSENT flag");
                   Header_S.Attribute |= MSG_DELETED;
                }
            }
          Header_S.Attribute |= MSG_SENT;
          Header_S.DateProcessed = time(NULL);
          Header_S.DateProcessed -= jam_utcoffset;
          // jam_openbases might have been changed in handlefunc
    */
    /* -- Only on exports of messages.
      if(!(ja=jam_getarea(area)))
      {
         delete mm;
         return(FALSE);
      }
      if(JAM_LockMB(ja->Base_PS,10))
      {
            _s.errlog((char *)"Timeout when trying to lock JAM messagebase \"%s\"",area->Path);
         delete mm;
         return(FALSE);
      }
      if(JAM_ChangeMsgHeader(ja->Base_PS,num-ja->BaseNum,&Header_S)) {
        _s.errlog((char *)"Failed to update header of message \"%s\"",area->Path);
      }
      JAM_UnlockMB(ja->Base_PS);
       }

       //delete mm; -- Only on Errors, this will pass back for message system!
    */
    JAM_CloseMB(ja->Base_PS);
    free(ja->Base_PS);
    delete ja;
    ja = 0;
    return (FALSE);
}

/**
 * Message Base JAM - Rename Message Base Files (Usually Temp)
 */
BOOL osRename(uint8_t *oldfile,uint8_t *newfile)
{
    if(rename((char *) oldfile, (char *) newfile) == 0)
        return (TRUE);
    return (FALSE);
}

/**
 * Message Base JAM - Deletes Message Base Files (Usually Temp)
 */
BOOL osDelete(uint8_t *file)
{
    if(remove((char *) file) == 0)
        return (TRUE);
    return (FALSE);
}

//long jam_utcoffset = 0xbaadf00d;
/**
 * Message Base JAM - Pack Message Area, Rebuilds each Area
 *                  - Skipping Empty and Deleted Messages.
 */
BOOL PackJamArea(mb_list_rec *area)
{
    uint32_t //today,
//             active  = 0,
//             basenum = 0,
    total   = 0,
    del     = 0,
    num     = 0;
    //      day;
    s_JamBase *Base_PS,*NewBase_PS;
    s_JamBaseHeader BaseHeader_S;
    s_JamMsgHeader   Header_S;
    s_JamSubPacket*  SubPacket_PS;

    int res,res1,res2;
    uint8_t buf[200],oldname[200],tmpname[200];
    BOOL firstwritten;
    uint8_t *msgtext;

    /* Some timezone tricks */
    /*
       if(jam_utcoffset == 0xbaadf00d)
       {
          time_t t1,t2;
          struct tm *tp;
          t1=time(NULL);
          tp=gmtime(&t1);
          tp->tm_isdst=-1;
          t2=mktime(tp);
          jam_utcoffset=t2-t1;
       }
    */
    printf("Processing %s...\n", (char *) area->mbdisplay);
    std::string path = (char *) MESGPATH;
    path += (char *) area->mbfile;

    if(JAM_OpenMB((uint8_t *) path.c_str(),&Base_PS))
    {
        printf(" Failed to open messagebase \"%s\"\n", (char *) path.c_str());
        return (TRUE);
    }
    if(JAM_LockMB(Base_PS,10))
    {
        printf(" Timeout when trying to lock messagebase \"%s\"\n", (char *) path.c_str());
        JAM_CloseMB(Base_PS);
        return (TRUE);
    }
    if(JAM_ReadMBHeader(Base_PS,&BaseHeader_S))
    {
        printf(" Failed to read header of messagebase \"%s\"\n", (char *) path.c_str());
        JAM_UnlockMB(Base_PS);
        JAM_CloseMB(Base_PS);
        return (TRUE);
    }
    if(JAM_GetMBSize(Base_PS,&total))
    {
        printf(" Failed to get size of messagebase \"%s\"\n", (char *) path.c_str());
        JAM_UnlockMB(Base_PS);
        JAM_CloseMB(Base_PS);
        return (TRUE);
    }

//   basenum = BaseHeader_S.BaseMsgNum;
//   active  = BaseHeader_S.ActiveMsgs;
    if(total == 0)
    {
        printf(" Area is empty\n");
        JAM_UnlockMB(Base_PS);
        JAM_CloseMB(Base_PS);
        return (TRUE);
    }
    /*
       if(maint && area->KeepNum!=0)
       {
            num=0;
            del=0;
            while(num < total && active > area->KeepNum && !ctrlc)
            {
               res=JAM_ReadMsgHeader(Base_PS,num,&Header_S,NULL);

             if(res == 0)
                {
                    // Read success

                   if(!(Header_S.Attribute & MSG_DELETED))
                    {
                        // Not already deleted

                     if(verbose)
                      printf(" Deleting message #%lu by number\n",basenum+num);

                        Header_S.Attribute |= MSG_DELETED;
                        JAM_ChangeMsgHeader(Base_PS,num,&Header_S);

                        BaseHeader_S.ActiveMsgs--;
                        JAM_WriteMBHeader(Base_PS,&BaseHeader_S);

                        active--;
                        del++;
                    }
                }

                num++;
            }

          if(ctrlc)
          {
                JAM_UnlockMB(Base_PS);
                JAM_CloseMB(Base_PS);
             return(TRUE);
          }

          printf(" %lu messages deleted by number, %lu messages left\n",del,active);
       }
    */

    /*
       if(maint && area->KeepDays!=0)
       {
      del=0;
      num=0;

      today=(time(NULL)-jam_utcoffset) / (24*60*60);

        while(num < total && !ctrlc)
        {
           res=JAM_ReadMsgHeader(Base_PS,num,&Header_S,NULL);

         if(res == 0)
            {
                // Read success

                day=Header_S.DateReceived / (24*60*60);

                if(day == 0)
                    day=Header_S.DateProcessed / (24*60*60);

                if(day == 0)
                    day=Header_S.DateWritten / (24*60*60);

               if(today-day > area->KeepDays && !(Header_S.Attribute & MSG_DELETED))
                {
                    // Not already deleted and too old

                 if(verbose)
                  printf(" Deleting message #%lu by date\n",basenum+num);

                    Header_S.Attribute |= MSG_DELETED;
                    JAM_ChangeMsgHeader(Base_PS,num,&Header_S);

                    BaseHeader_S.ActiveMsgs--;
                    JAM_WriteMBHeader(Base_PS,&BaseHeader_S);

                    del++;
                    active--;
                }
            }

            num++;
        }

      if(ctrlc)
      {
            JAM_UnlockMB(Base_PS);
            JAM_CloseMB(Base_PS);
         return(TRUE);
      }

      printf(" %lu messages deleted by date, %lu messages left\n",del,active);
       }
    */

    /*  if(pack)
      {*/

    strcpy((char *) buf, (char *) path.c_str());
    strcat((char *) buf, (char *) ".entemp");

    if(JAM_CreateMB(buf,1,&NewBase_PS))
    {
        printf(" Failed to create new messagebase \"%s\"\n",buf);
        JAM_UnlockMB(Base_PS);
        JAM_CloseMB(Base_PS);
        return (TRUE);
    }
    if(JAM_LockMB(NewBase_PS,10))
    {
        printf(" Timeout when trying to lock messagebase \"%s\"\n",buf);
        JAM_UnlockMB(Base_PS);
        JAM_CloseMB(Base_PS);
        JAM_CloseMB(NewBase_PS);
        JAM_RemoveMB(NewBase_PS,buf);
        return (TRUE);
    }

    /* Copy messages */
    del=0;
    num=0;
    firstwritten=FALSE;
    BaseHeader_S.ActiveMsgs=0;

    // while(num < total && !ctrlc)
    while(num < total)
    {
        res=JAM_ReadMsgHeader(Base_PS,num,&Header_S,NULL);
        if(res)
        {
            if(res == JAM_NO_MESSAGE)
            {
                if(firstwritten)
                {
                    JAM_AddEmptyMessage(NewBase_PS);
                }
                else
                {
                    BaseHeader_S.BaseMsgNum++;
                    del++;
                }
            }
            else
            {
                // printf(" Failed to read message %ld, cannot pack messagebase\n", num+basenum);
                JAM_UnlockMB(Base_PS);
                JAM_CloseMB(Base_PS);
                JAM_UnlockMB(NewBase_PS);
                JAM_CloseMB(NewBase_PS);
                JAM_RemoveMB(NewBase_PS,buf);
                return (TRUE);
            }
        }
        else
        {
            if(Header_S.Attribute & MSG_DELETED)
            {
                if(firstwritten)
                {
                    JAM_AddEmptyMessage(NewBase_PS);
                }
                else
                {
                    BaseHeader_S.BaseMsgNum++;
                    del++;
                }
            }
            else
            {
                if(!firstwritten)
                {
                    /* Set basenum */
                    res=JAM_WriteMBHeader(NewBase_PS,&BaseHeader_S);
                    if(res)
                    {
                        printf(" Failed to write messagebase header, cannot pack messagebase\n");
                        JAM_UnlockMB(Base_PS);
                        JAM_CloseMB(Base_PS);
                        JAM_UnlockMB(NewBase_PS);
                        JAM_CloseMB(NewBase_PS);
                        JAM_RemoveMB(NewBase_PS,buf);
                        return (TRUE);
                    }
                    firstwritten=TRUE;
                }

                /* Read header with all subpackets*/
                res=JAM_ReadMsgHeader(Base_PS,num,&Header_S,&SubPacket_PS);
                if(res)
                {
                    // printf(" Failed to read message %ld, cannot pack messagebase\n",num+basenum);
                    JAM_UnlockMB(Base_PS);
                    JAM_CloseMB(Base_PS);
                    JAM_UnlockMB(NewBase_PS);
                    JAM_CloseMB(NewBase_PS);
                    JAM_RemoveMB(NewBase_PS,buf);
                    return (TRUE);
                }

                /* Read message text */
                msgtext=NULL;
                if(Header_S.TxtLen)
                {
                    if(!(msgtext=new uint8_t[Header_S.TxtLen]))
                    {
                        printf("Out of memory\n");
                        JAM_DelSubPacket(SubPacket_PS);
                        JAM_UnlockMB(Base_PS);
                        JAM_CloseMB(Base_PS);
                        JAM_UnlockMB(NewBase_PS);
                        JAM_CloseMB(NewBase_PS);
                        JAM_RemoveMB(NewBase_PS,buf);
                        return (FALSE);
                    }

                    res=JAM_ReadMsgText(Base_PS,Header_S.TxtOffset,Header_S.TxtLen,msgtext);
                    if(res)
                    {
                        //   printf(" Failed to read message %ld, cannot pack messagebase\n",num+basenum);
                        JAM_DelSubPacket(SubPacket_PS);
                        JAM_UnlockMB(Base_PS);
                        JAM_CloseMB(Base_PS);
                        JAM_UnlockMB(NewBase_PS);
                        JAM_CloseMB(NewBase_PS);
                        JAM_RemoveMB(NewBase_PS,buf);
                        return (TRUE);
                    }
                }

                /* Write new message */
                res=JAM_AddMessage(NewBase_PS,&Header_S,SubPacket_PS,msgtext,Header_S.TxtLen);
                if(msgtext) delete(msgtext);
                JAM_DelSubPacket(SubPacket_PS);
                BaseHeader_S.ActiveMsgs++;
                if(res)
                {
                    // printf(" Failed to copy message %ld (disk full?!?Permissions?!?), cannot pack messagebase\n",num+basenum);
                    JAM_UnlockMB(Base_PS);
                    JAM_CloseMB(Base_PS);
                    JAM_UnlockMB(NewBase_PS);
                    JAM_CloseMB(NewBase_PS);
                    JAM_RemoveMB(NewBase_PS,buf);
                    return (TRUE);
                }
            }
        }
        num++;
    }

    /* Write back header */
    BaseHeader_S.ModCounter++;  // Hmmm is this needed?
    res=JAM_WriteMBHeader(NewBase_PS,&BaseHeader_S);
    if(res)
    {
        printf(" Failed to write messagebase header, cannot pack messagebase\n");
        JAM_UnlockMB(Base_PS);
        JAM_CloseMB(Base_PS);
        JAM_UnlockMB(NewBase_PS);
        JAM_CloseMB(NewBase_PS);
        JAM_RemoveMB(NewBase_PS,buf);
        return (TRUE);
    }

    JAM_UnlockMB(Base_PS);
    JAM_CloseMB(Base_PS);
    JAM_UnlockMB(NewBase_PS);
    JAM_CloseMB(NewBase_PS);

    /*
            if(ctrlc)
            {
                JAM_RemoveMB(NewBase_PS,buf);
             return(TRUE);
            }
    */
    /* This could not be done with JAMLIB... */
    sprintf((char *) oldname,"%s%s", (char *) path.c_str(),EXT_HDRFILE);
    sprintf((char *) tmpname,"%s.entemp%s", (char *) path.c_str(),EXT_HDRFILE);
    res1=osDelete(oldname);
    res2=osRename(tmpname,oldname);

    if(res1 && res2)
    {
        sprintf((char *) oldname,"%s%s", (char *) path.c_str(),EXT_TXTFILE);
        sprintf((char *) tmpname,"%s.entemp%s", (char *) path.c_str(),EXT_TXTFILE);
        res1=osDelete(oldname);
        res2=osRename(tmpname,oldname);
    }
    if(res1 && res2)
    {
        sprintf((char *) oldname,"%s%s", (char *) path.c_str(),EXT_IDXFILE);
        sprintf((char *) tmpname,"%s.entemp%s", (char *) path.c_str(),EXT_IDXFILE);
        res1=osDelete(oldname);
        res2=osRename(tmpname,oldname);
    }
    if(res1 && res2)
    {
        sprintf((char *) oldname,"%s%s", (char *) path.c_str(),EXT_LRDFILE);
        sprintf((char *) tmpname,"%s.entemp%s", (char *) path.c_str(),EXT_LRDFILE);
        /* Keep lastread file */
        res2=osDelete(tmpname);
    }
    if(!res1 || !res2)
    {
        printf(" Failed to update area. The area might be in use by another program.\n");
        return (FALSE);
    }

    //  printf(" %ld deleted messages removed from messagebase\n",del);
    /*
        }
        else
        {
     JAM_UnlockMB(Base_PS);
     JAM_CloseMB(Base_PS);
        }*/
    return (TRUE);
}


/**
 * Message Base JAM - Reads Message ID of Currnet Message
 */
uint32_t jamapi_readmsgid(mb_list_rec *area, uint32_t num, std::string &tmpMsgId)
{
    //struct MemMessage *mm;
    struct jam_Area *ja;
//    uint8_t *msgtext;
//    uint8_t buf[200] = {0},domain[20] = {0};
    int res; //,c;
    s_JamSubPacket* SubPacket_PS;
    s_JamMsgHeader  Header_S;
    s_JamSubfield*  Field_PS;
//    struct Node4D n4d;
//    BOOL hasaddr;
//    uint8_t flagsbuf[200] = {0},filesubject[200] = {0};
//    ushort oldattr;

    res = 0;
//    uint32_t ret = 0;
    SESSION _s;

    if(num != 0)
        --num;
    // Open the area
    if(!(ja=jam_getarea(area)))
    {
//      _s.errlog((char *)"4. readmsgid() - !jam_getarea()");
        return (0L);
    }
//   _s.errlog((char *)"4. readmsgid() - After jam_getarea(): BaseNum %lu, Num %lu, HighWater: %lu Active: %lu",ja->BaseNum,num,ja->HighWater,ja->Active);

    // Read message header
//   res=JAM_ReadMsgHeader(ja->Base_PS, num - ja->BaseNum, &Header_S, &SubPacket_PS);
    res = JAM_ReadMsgHeader(ja->Base_PS, num, &Header_S, &SubPacket_PS);
    if(res)
    {
        if(res == JAM_NO_MESSAGE)
        {
            JAM_CloseMB(ja->Base_PS);
            free(ja->Base_PS);
            delete ja;
            ja = 0;
//          _s.errlog((char *)"4. readmsgid() message no longer exists #%lu ",num);
            return (0L);   // Message no longer exists
        }
        else
        {
            JAM_CloseMB(ja->Base_PS);
            free(ja->Base_PS);
            delete ja;
            ja = 0;
//         _s.errlog((char *)"4. readmsgid() failed to read #%lu ",num);
            return (0L);
        }
    }
    uint32_t crcvalue = 0L;
    crcvalue = Header_S.MsgIdCRC;
    //Header_S.ReplyCRC;  // add switch in function for this.

    // Check if deleted
    if(Header_S.Attribute & MSG_DELETED)
    {
//       _s.errlog((char *)"4. readmsgid() flagged deleted. #%lu ",num);
        // Message deleted
        /*
              JAM_CloseMB(ja->Base_PS);
              JAM_DelSubPacket(SubPacket_PS);
              free(ja->Base_PS);
              delete ja;
              ja = 0;
              return(0L);*/
    }
    // Searching for a Flag,  skipp for now, and do only header.
    for(Field_PS=JAM_GetSubfield(SubPacket_PS); Field_PS; Field_PS=JAM_GetSubfield(NULL))
    {
        switch(Field_PS->LoID)
        {
            case JAMSFLD_MSGID:
                //jam_makekludge(mm,(uint8_t *)"\x01" "MSGID: ",Field_PS->Buffer,Field_PS->DatLen);
                //sprintf(newid,"%s",Field_PS->Buffer);

                // This value gets returned back.
                tmpMsgId.append((char *) Field_PS->Buffer,Field_PS->DatLen);
//              _s.errlog((char *)"[*] MSGID CRC/VALUE newid: %lu - %s",crcvalue,(const char*)tmpMsgId.c_str());
                break;

            case JAMSFLD_REPLYID:
                //jam_makekludge(mm,(uint8_t *)"\x01" "REPLY: ",Field_PS->Buffer,Field_PS->DatLen);
                break;
        }
    }
    JAM_DelSubPacket(SubPacket_PS);
    JAM_CloseMB(ja->Base_PS);
    free(ja->Base_PS);
    delete ja;
    ja = 0;
//    _s.errlog((char *)"[*] 4. readmsgid() return crcvalue #%lu ",crcvalue);
    return (crcvalue);
}


/**
 * Message Base JAM - Scan Message ID For Custom Last Read Pointer
 *                     Loops through all messages checking if MsgID Matches CRC32.
 */
uint32_t buildmsgid(uint32_t MsgID, mb_list_rec *area)
{
    uint32_t res=0;
    uint32_t num=0;

    struct jam_Area *ja;
    std::string tmp;
    SESSION _s;

    if(!(ja=jam_getarea(area)))
        return (0L);

    unsigned long start=ja->BaseNum;
    num = 0;
    while(1)
    {
        res = jamapi_readmsgid(area, num, tmp);
        if(res == MsgID)
        {
            JAM_CloseMB(ja->Base_PS);
            free(ja->Base_PS);
            delete ja;
            ja = 0;
            return num;
        }

        ++num;
        if(num > start)
        {
            JAM_CloseMB(ja->Base_PS);
            free(ja->Base_PS);
            delete ja;
            ja = 0;
            break;
        }
    }
    return 0L;
}


/*

// Reference only ...

BOOL jam_readmsg(struct Area *area,BOOL (*handlefunc)(struct MemMessage *mm))
{
   uint32_t start,end;
   struct jam_Area *ja;

   // Open the area

   if(!(ja=jam_getarea(area)))
    {
        if(nomem)
          return(FALSE);

        return(TRUE); // Area did not exist and could not be created. Go on anyway.
    }

   if(config.cfg_jam_Flags & CFG_JAM_HIGHWATER)
      jam_gethighwater(ja);

   if(ja->HighWater) start=ja->HighWater+1;
   else              start=ja->BaseNum;

    if(start < ja->BaseNum)
        start=ja->BaseNum;

   end   = ja->BaseNum + ja->OldNum;

   while(start < end && !ctrlc)
   {
      if(!jam_ExportJAMNum(area,start,handlefunc,FALSE))
         return(FALSE);

      start++;
   }

   if(ctrlc)
      return(FALSE);

   ja->HighWater=end-1;

   return(TRUE);
   }

}
*/


/*
void jam_makekludge(struct MemMessage *mm,uint8_t *pre,uint8_t *data,uint32_t len)
{
   uint8_t *buf;

    if(!(buf=(uint8_t *)osAlloc(strlen((char *)pre)+len+10))) // A few bytes extra
        return;

   strcpy((char *)buf,(char *)pre);
   if(len && data) mystrncpy(&buf[strlen((char *)buf)],data,len+1);
   strcat((char *)buf,(char *)"\x0d");
   mmAddLine(mm,buf);

    osFree(buf);
}
*/

/*
s_JamBase *jam_openbase(struct jam_Area *area)
{
   int c;
   struct openbase *thisbase;

// See if that area already is open /

   for(c=0;c<config.cfg_jam_MaxOpen;c++)
      if(jam_openbases[c].area == area && jam_openbases[c].lastuse)
      {
         jam_openbases[c].lastuse=jam_lastnum++;
         return(jam_openbases[c].Base_PS);
      }

   // We must open it now

  // Try to get an empty slot

   thisbase=NULL;

   for(c=0;c<config.cfg_jam_MaxOpen && !thisbase;c++)
      if(jam_openbases[c].lastuse == 0) thisbase=&jam_openbases[c];

   // Otherwise, remove a slot that hasn't been used for a long time

   if(!thisbase)
   {
      thisbase=&jam_openbases[0];

      for(c=0;c<config.cfg_jam_MaxOpen;c++)
         if(jam_openbases[c].lastuse < thisbase->lastuse) thisbase=&jam_openbases[c];

      thisbase->lastuse=0;
      JAM_CloseMB(thisbase->Base_PS);
      free(thisbase->Base_PS);
   }

 // Open area

   if(JAM_OpenMB(area->area->Path,&thisbase->Base_PS))
   {
      if(thisbase->Base_PS) free(thisbase->Base_PS);

      LogWrite(2,SYSTEMINFO,(uint8_t *)"Creating JAM messagebase \"%s\"",area->area->Path);

      if(JAM_CreateMB(area->area->Path,1,&thisbase->Base_PS))
      {
         if(thisbase->Base_PS) free(thisbase->Base_PS);
         LogWrite(1,SYSTEMERR,(uint8_t *)"Failed to create JAM messagebase \"%s\"",area->area->Path);
         return(NULL);
      }
   }

   // Set the rest

   thisbase->lastuse=jam_lastnum++;
   thisbase->area=area;

   return(thisbase->Base_PS);
}

struct jam_Area *jam_getarea(struct Area *area)
{
   struct jam_Area *ja;
   uint32_t num;
   s_JamBaseHeader Header_S;

//    Check if area already exists

   for(ja=(struct jam_Area *)jam_AreaList.First;ja;ja=ja->Next)
      if(ja->area == area)
      {
         if(!(ja->Base_PS=jam_openbase(ja)))
            return(NULL);

         return(ja);
      }

   // This is the first time we use this area

   if(!(ja=(jam_Area *)osAllocCleared(sizeof(struct jam_Area))))
   {
      nomem=TRUE;
      return(FALSE);
   }

   jbAddNode(&jam_AreaList,(struct jbNode *)ja);
   ja->area=area;

   if(!(ja->Base_PS=jam_openbase(ja)))
      return(NULL);

   if(JAM_GetMBSize(ja->Base_PS,&num))
   {
      LogWrite(1,TOSSINGERR,(uint8_t *)"Failed to get size of JAM area \"%s\"",area->Path);
      return(NULL);
   }

   ja->OldNum=num;

   if(JAM_ReadMBHeader(ja->Base_PS,&Header_S))
   {
      LogWrite(1,TOSSINGERR,(uint8_t *)"Failed to read header of JAM area \"%s\"",area->Path);
      return(NULL);
   }

   ja->BaseNum=Header_S.BaseMsgNum;

   ja->OldHighWater=0;
   ja->HighWater=0;

   return(ja);
}

void jam_gethighwater(struct jam_Area *ja)
{
   uint8_t buf[200];
   osFile fh;
   uint32_t num;

   strcpy((char *)buf,(char *)ja->area->Path);
   strcat((char *)buf,(char *)".cmhw");

   if((fh=osOpen(buf,MODE_OLDFILE)))
   {
      if(osRead(fh,&num,sizeof(uint32_t)))
      {
         ja->HighWater=num;
         ja->OldHighWater=num;
      }

      osClose(fh);
   }
}

void jam_writehighwater(struct jam_Area *ja)
{
   uint8_t buf[200];
   osFile fh;
   uint32_t num;

   strcpy((char *)buf,(char *)ja->area->Path);
   strcat((char *)buf,".cmhw");

   num=ja->HighWater;

   if((fh=osOpen(buf,MODE_NEWFILE)))
   {
      osWrite(fh,&num,sizeof(uint32_t));
      osClose(fh);
   }
}

BOOL jam_beforefunc(void)
{
   time_t t1,t2;
   struct tm *tp;

   jbNewList(&jam_AreaList);

   if(config.cfg_jam_MaxOpen == 0)
      config.cfg_jam_MaxOpen = 5;

   if(!(jam_openbases=(openbase *)osAllocCleared(config.cfg_jam_MaxOpen * sizeof(struct openbase))))
   {
      nomem=TRUE;
      return(FALSE);
   }

  // Some timezone tricks

   t1=time(NULL);
   tp=gmtime(&t1);
   tp->tm_isdst=-1;
   t2=mktime(tp);
   jam_utcoffset=t2-t1;

   jam_lastnum=1;

   return(TRUE);
}

BOOL jam_afterfunc(BOOL success)
{
   int c;
   struct jam_Area *ja;

   if(success && (config.cfg_jam_Flags & CFG_JAM_HIGHWATER))
      for(ja=(struct jam_Area *)jam_AreaList.First;ja;ja=ja->Next)
         if(ja->HighWater != ja->OldHighWater)
            jam_writehighwater(ja);

   if(success && (config.cfg_jam_Flags & CFG_JAM_LINK))
      for(ja=(struct jam_Area *)jam_AreaList.First;ja;ja=ja->Next)
         if(ja->newmsg)
            jam_linkmb(ja->area,ja->OldNum);

   for(c=0;c<config.cfg_jam_MaxOpen;c++)
      if(jam_openbases[c].lastuse)
      {
         JAM_CloseMB(jam_openbases[c].Base_PS);
         free(jam_openbases[c].Base_PS);
      }

   osFree(jam_openbases);
   jbFreeList(&jam_AreaList);

    return(TRUE);
}

BOOL jam_nomem;

void jam_addfield(s_JamSubPacket *SubPacket_PS,uint32_t fieldnum,uint8_t *fielddata)
{
   s_JamSubfield    Subfield_S;

   Subfield_S.LoID   = fieldnum;
   Subfield_S.HiID   = 0;
   Subfield_S.DatLen = strlen((char *)fielddata);
   Subfield_S.Buffer = fielddata;

   if(JAM_PutSubfield( SubPacket_PS, &Subfield_S) == JAM_NO_MEMORY)
      jam_nomem=TRUE;
}

struct flag
{
   uint8_t *name;
   uint32_t jamflagbit;
   uint32_t fidoflagbit;
};

struct flag jam_flagarray[] =
{ { (uint8_t *)"PVT", MSG_PRIVATE,     FLAG_PVT         },
  { (uint8_t *)"HLD", MSG_HOLD,        FLAG_HOLD        },
  { (uint8_t *)"CRA", MSG_CRASH,       FLAG_CRASH       },
  { (uint8_t *)"K/S", MSG_KILLSENT,    FLAG_KILLSENT    },
  { (uint8_t *)"SNT", MSG_SENT,        FLAG_SENT        },
  { (uint8_t *)"RCV", MSG_READ,        FLAG_RECD        },
  { (uint8_t *)"A/S", MSG_ARCHIVESENT, 0,               },
  { (uint8_t *)"DIR", MSG_DIRECT,      0                },
  { (uint8_t *)"FIL", MSG_FILEATTACH,  FLAG_FILEATTACH  },
  { (uint8_t *)"FRQ", MSG_FILEREQUEST, FLAG_FILEREQ     },
  { (uint8_t *)"IMM", MSG_IMMEDIATE,   0                },
  { (uint8_t *)"KFS", MSG_KILLFILE,    0                },
  { (uint8_t *)"TFS", MSG_TRUNCFILE,   0                },
  { (uint8_t *)"LOK", MSG_LOCAL,       FLAG_LOCAL       },
  { (uint8_t *)"RRQ", MSG_RECEIPTREQ,  FLAG_RREQ        },
  { (uint8_t *)"CFM", MSG_CONFIRMREQ,  0                },
  { (uint8_t *)"FPU", MSG_FPU,         0                },
  { (uint8_t *)"",    MSG_INTRANSIT,   FLAG_INTRANSIT   },
  { (uint8_t *)"",    MSG_ORPHAN,      FLAG_ORPHAN      },
  { NULL,  0,               0                } };

uint32_t jam_findflag(uint8_t *name)
{
   int c;

   for(c=0;jam_flagarray[c].name;c++)
      if(stricmp((char *)jam_flagarray[c].name,(char *)name)==0) return(jam_flagarray[c].jamflagbit);

   return(0);
}

BOOL jam_importfunc(struct MemMessage *mm,struct Area *area)
{
   struct TextChunk *chunk;
   struct Path *pathnode;
   struct jam_Area *ja;
   s_JamSubPacket*    SubPacket_PS;
   s_JamMsgHeader    Header_S;
   uint8_t buf[100],newflags[100],flag[10];
   uint32_t c,f,jbcpos,linebegin,linelen;
   uint8_t *msgtext;
   uint32_t msgsize,msgpos;
   int res;

   // Get an area to write to

   if(!(ja=jam_getarea(area)))
      return(FALSE);

   // Start import

   ja->newmsg=TRUE;

   JAM_ClearMsgHeader(&Header_S);

   if(!(SubPacket_PS = JAM_NewSubPacket()))
   {
      nomem=TRUE;
      return(FALSE);
   }

   // Allocate memory to store message text in

   msgpos=0;
   msgsize=0;

   for(chunk=(struct TextChunk *)mm->TextChunks.First;chunk;chunk=chunk->Next)
      msgsize+=chunk->Length;

   if(msgsize != 0)
   {
      if(!(msgtext=(uint8_t *)osAlloc(msgsize)))
      {
         LogWrite(1,SYSTEMERR,(uint8_t *)"Out of memory");
         JAM_DelSubPacket(SubPacket_PS);
         return(FALSE);
      }
   }

   // Do header

   Header_S.DateProcessed = time(NULL);
   Header_S.DateWritten = FidoToTime(mm->DateTime);

   // Damned time zones... dates should be in local time in JAM
   Header_S.DateProcessed -= jam_utcoffset;
   Header_S.DateWritten -= jam_utcoffset;

   Header_S.Cost=mm->Cost;
   Header_S.MsgIdCRC=JAM_Crc32(mm->MSGID,strlen((char *)mm->MSGID));
   Header_S.ReplyCRC=JAM_Crc32(mm->REPLY,strlen((char *)mm->REPLY));

   // Add header fields

   if(mm->From[0])
      jam_addfield(SubPacket_PS,JAMSFLD_SENDERNAME,(uint8_t *)mm->From);

   if(mm->To[0])
      jam_addfield(SubPacket_PS,JAMSFLD_RECVRNAME,(uint8_t *)mm->To);

   if(mm->Subject[0])
      jam_addfield(SubPacket_PS,JAMSFLD_SUBJECT,(uint8_t *)mm->Subject);

   if(mm->Area[0] == 0)
   {
      // Addresses in netmail //

      Print4D(&mm->OrigNode,buf);
      jam_addfield(SubPacket_PS,JAMSFLD_OADDRESS,buf);

      Print4D(&mm->DestNode,buf);
      jam_addfield(SubPacket_PS,JAMSFLD_DADDRESS,buf);
   }
   else
   {
      // Addresses in echomail //

      Print4D(&mm->Origin4D,buf);
      jam_addfield(SubPacket_PS,JAMSFLD_OADDRESS,buf);
   }

   // Header attributes //

   for(c=0;jam_flagarray[c].name;c++)
      if(mm->Attr & jam_flagarray[c].fidoflagbit)
         Header_S.Attribute |= jam_flagarray[c].jamflagbit;

   if(mm->Attr & FLAG_FILEATTACH)
   {
      Header_S.Attribute |= MSG_FILEATTACH;

      c=0;

      while(mm->Subject[c]!=0)
      {
         f=0;
         while(mm->Subject[c]!=0 && mm->Subject[c]!=32 && mm->Subject[c]!=',' && f<80)
            buf[f++]=mm->Subject[c++];

         buf[f]=0;

         while(mm->Subject[c]==32 || mm->Subject[c]==',') c++;

         if(buf[0]!=0)
            jam_addfield(SubPacket_PS,JAMSFLD_ENCLFILE,buf);
      }
   }

   if(mm->Attr & FLAG_FILEREQ)
   {
      Header_S.Attribute |= MSG_FILEREQUEST;

      c=0;

      while(mm->Subject[c]!=0)
      {
         f=0;
         while(mm->Subject[c]!=0 && mm->Subject[c]!=32 && mm->Subject[c]!=',' && f<80)
            buf[f++]=mm->Subject[c++];

         buf[f]=0;

         while(mm->Subject[c]==32 || mm->Subject[c]==',') c++;

         if(buf[0]!=0)
            jam_addfield(SubPacket_PS,JAMSFLD_ENCLFREQ,buf);
      }
   }

   // Echomail/netmail attribute

   if(mm->Area[0]==0)
      Header_S.Attribute |= MSG_TYPENET;

   else
      Header_S.Attribute |= MSG_TYPEECHO;

   // Separate kludges from text

   for(chunk=(struct TextChunk *)mm->TextChunks.First;chunk;chunk=chunk->Next)
      for(c=0;c<chunk->Length;)
      {
            linebegin=msgpos;

         while(chunk->Data[c]!=13 && c<chunk->Length)
            {
                if(chunk->Data[c]!=10)
                    msgtext[msgpos++]=chunk->Data[c];

                c++;
            }

         if(chunk->Data[c]==13 && c<chunk->Length)
                msgtext[msgpos++]=chunk->Data[c++];

            linelen=msgpos-linebegin;

         if(linelen!=0)
         {
            if(linelen>=5 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""PID:",5)==0)
            {
               mystrncpy(buf,&msgtext[linebegin+5],MIN(100,linelen-5));
               stripleadtrail(buf);
               jam_addfield(SubPacket_PS,JAMSFLD_PID,buf);
                    msgpos=linebegin;
            }
            else if(linelen>=7 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""MSGID:",7)==0)
            {
               mystrncpy(buf,&msgtext[linebegin+7],MIN(100,linelen-7));
               stripleadtrail(buf);
               jam_addfield(SubPacket_PS,JAMSFLD_MSGID,buf);
                    msgpos=linebegin;
            }
            else if(linelen>=7 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""REPLY:",7)==0)
            {
               mystrncpy(buf,&msgtext[linebegin+7],MIN(100,linelen-7));
               stripleadtrail(buf);
               jam_addfield(SubPacket_PS,JAMSFLD_REPLYID,buf);
                    msgpos=linebegin;
            }
            else if(linelen>=7 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""FLAGS:",7)==0)
            {
               mystrncpy(buf,&msgtext[linebegin+7],MIN(100,linelen-7));
               stripleadtrail(buf);

               jbcpos=0;
               newflags[0]=0;

               while(jbstrcpy(flag,buf,10,&jbcpos))
               {
                  uint32_t flagbit;

                  if((flagbit=jam_findflag(flag)))
                  {
                     Header_S.Attribute |= flagbit;
                  }
                  else
                  {
                     strcat((char *)newflags,(char *)flag);
                     strcat((char *)newflags,(char *)" ");
                  }
               }

               stripleadtrail(newflags);

               if(newflags[0]!=0)
                  jam_addfield(SubPacket_PS,JAMSFLD_FLAGS,newflags);

                    msgpos=linebegin;
            }
            else if(linelen>=5 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""INTL",5)==0)
            {
               // Remove this kludge
                    msgpos=linebegin;
            }
            else if(linelen>=5 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""TOPT",5)==0)
            {
               // Remove this kludge
                    msgpos=linebegin;
            }
            else if(linelen>=5 && strncmp((char *)&msgtext[linebegin],(char *)"\x01""FMPT",5)==0)
            {
               // Remove this kludge
                    msgpos=linebegin;
            }
            else if(msgtext[linebegin]==1)
            {
               mystrncpy(buf,&msgtext[linebegin+1],MIN(100,linelen-1));
               stripleadtrail(buf);
               jam_addfield(SubPacket_PS,JAMSFLD_FTSKLUDGE,buf);
                    msgpos=linebegin;
            }
         }
      }

   // Seen-by

   if(config.cfg_Flags & CFG_IMPORTSEENBY)
   {
      uint8_t *buf;
      uint32_t c,d;

      if((buf=mmMakeSeenByBuf(&mm->SeenBy)))
      {
         c=0;

         while(buf[c]!=0)
         {
            d=c;

            while(buf[d]!=0 && buf[d]!=13)
               d++;

            if(buf[d]==13)
            {
               buf[d++]=0;
               jam_addfield(SubPacket_PS,JAMSFLD_SEENBY2D,&buf[c+9]);
            }

            c=d;
         }
      }

      osFree(buf);
   }

   // Path

   for(pathnode=(struct Path *)mm->Path.First;pathnode;pathnode=pathnode->Next)
      for(c=0;c<pathnode->Paths;c++)
         jam_addfield(SubPacket_PS,JAMSFLD_PATH2D,pathnode->Path[c]);

   if(jam_nomem)
   {
      LogWrite(1,SYSTEMERR,(uint8_t *)"Out of memory");
      JAM_DelSubPacket(SubPacket_PS);
      if(msgsize) osFree(msgtext);
      return(FALSE);
   }

   //Write message

   if(JAM_LockMB(ja->Base_PS,10))
   {
      LogWrite(1,SYSTEMERR,(uint8_t *)"Timeout when trying to lock JAM messagebase \"%s\"",area->Path);
      JAM_DelSubPacket(SubPacket_PS);
      if(msgsize) osFree(msgtext);
      return(FALSE);
   }

   if(msgsize == 0)
   {
      msgtext=(uint8_t *)"";
      msgpos=1;
   }

   res=JAM_AddMessage(ja->Base_PS,&Header_S,SubPacket_PS,msgtext,msgpos);

   JAM_UnlockMB(ja->Base_PS);
   JAM_DelSubPacket(SubPacket_PS);
   if(msgsize) osFree(msgtext);

   if(res)
   {
      LogWrite(1,SYSTEMERR,(uint8_t *)"Failed to write message to JAM messagebase \"%s\"",area->Path);
      return(FALSE);
   }

   return(TRUE);
}

void jam_makekludge(struct MemMessage *mm,uint8_t *pre,uint8_t *data,uint32_t len)
{
   uint8_t *buf;

    if(!(buf=(uint8_t *)osAlloc(strlen((char *)pre)+len+10))) // A few bytes extra
        return;

   strcpy((char *)buf,(char *)pre);
   if(len && data) mystrncpy(&buf[strlen((char *)buf)],data,len+1);
   strcat((char *)buf,(char *)"\x0d");
   mmAddLine(mm,buf);

    osFree(buf);
}

BOOL jam_ExportJAMNum(struct Area *area,uint32_t num,BOOL (*handlefunc)(struct MemMessage *mm),BOOL isrescanning)
{
   struct MemMessage *mm;
   struct jam_Area *ja;
   uint8_t *msgtext;
   uint8_t buf[200],domain[20];
   int res,c;
   s_JamSubPacket*      SubPacket_PS;
   s_JamMsgHeader    Header_S;
   s_JamSubfield* Field_PS;
   struct Node4D n4d;
   BOOL hasaddr;
   uint8_t flagsbuf[200],filesubject[200];
   ushort oldattr;

   // Open the area

   if(!(ja=jam_getarea(area)))
      return(FALSE);

   // Read message header

   res=JAM_ReadMsgHeader(ja->Base_PS,num-ja->BaseNum,&Header_S,&SubPacket_PS);

   if(res)
   {
      if(res == JAM_NO_MESSAGE)
      {
         return(TRUE); // Message no longer exists
      }
      else
      {
         LogWrite(1,TOSSINGERR,(uint8_t *)"Failed to read message #%lu in JAM messagebase \"%s\"",num,area->Path);
         return(TRUE);
      }
   }

    // Check if deleted

   if(Header_S.Attribute & MSG_DELETED)
   {
      // Message deleted
      JAM_DelSubPacket(SubPacket_PS);
      return(TRUE);
   }

   // Check if already sent

    if(!isrescanning)
    {
        if((Header_S.Attribute & MSG_SENT) || !(Header_S.Attribute & MSG_LOCAL))
        {
            // Don't touch if the message is sent or not local
          JAM_DelSubPacket(SubPacket_PS);
          return(TRUE);
        }
   }

   // Read message text

   msgtext=NULL;

   if(Header_S.TxtLen)
   {
      if(!(msgtext=(uint8_t *)osAlloc(Header_S.TxtLen+1))) // One extra byte for the terminating zero
      {
         nomem=TRUE;
         JAM_DelSubPacket(SubPacket_PS);
         return(FALSE);
      }

      res=JAM_ReadMsgText(ja->Base_PS,Header_S.TxtOffset,Header_S.TxtLen,msgtext);

      if(res)
      {
         LogWrite(1,TOSSINGERR,(uint8_t *)"Failed to read message #%lu in JAM messagebase \"%s\"",num,area->Path);
         JAM_DelSubPacket(SubPacket_PS);
         return(FALSE);
      }

      msgtext[Header_S.TxtLen]=0;
   }

   // Allocate message structure

   if(!(mm=mmAlloc()))
   {
      JAM_DelSubPacket(SubPacket_PS);
      if(msgtext) osFree(msgtext);
      return(FALSE);
   }

   if(area->AreaType == AREATYPE_NETMAIL)
      strcpy((char *)mm->Area,(char *)"");

   else
      strcpy((char *)mm->Area,(char *)area->Tagname);

   mm->msgnum=num;

   // Subfields

   flagsbuf[0]=0;
   filesubject[0]=0;
   hasaddr=FALSE;

   for(Field_PS=JAM_GetSubfield(SubPacket_PS);Field_PS;Field_PS=JAM_GetSubfield(NULL))
   {
      switch(Field_PS->LoID)
      {
         case JAMSFLD_OADDRESS:
            mystrncpy(buf,Field_PS->Buffer,Field_PS->DatLen+1);

            if(Parse5D(buf,&n4d,domain))
            {
               mm->OrigNode.Zone=n4d.Zone;
               mm->OrigNode.Net=n4d.Net;
               mm->OrigNode.Node=n4d.Node;
               mm->OrigNode.Point=n4d.Point;
            }

            break;

         case JAMSFLD_DADDRESS:
            mystrncpy(buf,Field_PS->Buffer,Field_PS->DatLen+1);

            if(hasaddr)
            {
               LogWrite(1,TOSSINGERR,(uint8_t *)"Warning: Multiple DADDRESS not supported by CrashMail");               }
            else
            {
               hasaddr=TRUE;

               if(Parse5D(buf,&n4d,domain))
               {
                  mm->DestNode.Zone=n4d.Zone;
                  mm->DestNode.Net=n4d.Net;
                  mm->DestNode.Node=n4d.Node;
                  mm->DestNode.Point=n4d.Point;
               }
            }
            break;

         case JAMSFLD_SENDERNAME:
            mystrncpy(buf,Field_PS->Buffer,Field_PS->DatLen+1);
            mystrncpy(mm->From,buf,36);
            break;

         case JAMSFLD_RECVRNAME:
            mystrncpy(buf,Field_PS->Buffer,Field_PS->DatLen+1);
            mystrncpy(mm->To,buf,36);
            break;

         case JAMSFLD_MSGID:
            jam_makekludge(mm,(uint8_t *)"\x01" "MSGID: ",Field_PS->Buffer,Field_PS->DatLen);
            break;

         case JAMSFLD_REPLYID:
            jam_makekludge(mm,(uint8_t *)"\x01" "REPLY: ",Field_PS->Buffer,Field_PS->DatLen);
            break;

         case JAMSFLD_SUBJECT:
            mystrncpy(buf,Field_PS->Buffer,Field_PS->DatLen+1);
            mystrncpy(mm->Subject,buf,72);
            break;

         case JAMSFLD_PID:
            jam_makekludge(mm,(uint8_t *)"\x01" "PID: ",Field_PS->Buffer,Field_PS->DatLen);
            break;

         case JAMSFLD_ENCLFILE:
            if(filesubject[0]) LogWrite(1,TOSSINGERR,(uint8_t *)"Warning: Multiple ENCLOSEDFILE not supported by CrashMail");
            else mystrncpy(filesubject,Field_PS->Buffer,Field_PS->DatLen+1);
            break;

         case JAMSFLD_ENCLFREQ:
            LogWrite(1,TOSSINGERR,(uint8_t *)"Warning: ENCLOSEDFREQ not supported by CrashMail");
            break;

         case JAMSFLD_ENCLFWALIAS:
            LogWrite(1,TOSSINGERR,(uint8_t *)"Warning: ENCLOSEDFILEWALIAS not supported by CrashMail");
            break;

         case JAMSFLD_ENCLFILEWC:
            LogWrite(1,TOSSINGERR,(uint8_t *)"Warning: ENCLOSEDFILEWCARD with wildcards not supported by CrashMail");
            break;

         case JAMSFLD_ENCLINDFILE:
            LogWrite(1,TOSSINGERR,(uint8_t *)"Warning: ENCLOSEDINDIRECTFILE not supported by CrashMail");
            break;

         case JAMSFLD_FTSKLUDGE:
            jam_makekludge(mm,(uint8_t *)"\x01",Field_PS->Buffer,Field_PS->DatLen);
            break;

         case JAMSFLD_SEENBY2D:
            jam_makekludge(mm,(uint8_t *)"SEEN-BY: ",Field_PS->Buffer,Field_PS->DatLen);
            break;

         case JAMSFLD_PATH2D:
            jam_makekludge(mm,(uint8_t *)"\01" "PATH: ",Field_PS->Buffer,Field_PS->DatLen);
            break;

         case JAMSFLD_FLAGS:
            strcpy((char *)flagsbuf,(const char *)"\x01" "FLAGS: ");
            mystrncpy(&flagsbuf[8],Field_PS->Buffer,Field_PS->DatLen+1);
            // Don't add until attributes from header has been added
            break;
      }
   }

   if(filesubject[0])
   {
      mm->Attr|=FLAG_FILEATTACH;
      mystrncpy(mm->Subject,filesubject,72);
   }

   // Message header

   MakeFidoDate(Header_S.DateWritten+jam_utcoffset,mm->DateTime);
   mm->Cost=Header_S.Cost;

   for(c=0;jam_flagarray[c].name;c++)
      if(Header_S.Attribute & jam_flagarray[c].jamflagbit)
      {
         if(jam_flagarray[c].fidoflagbit)
         {
            mm->Attr |= jam_flagarray[c].fidoflagbit;
         }
         else if(jam_flagarray[c].name[0] && strlen((const char *)flagsbuf)<90)
         {
            if(flagsbuf[0]==0) strcpy((char *)flagsbuf,(char *)"\x01" "FLAGS: ");
            else               strcat((char *)flagsbuf,(char *)" ");

            strcat((char *)flagsbuf,(char *)jam_flagarray[c].name);
         }
      }

   if(flagsbuf[0])
   {
      strcat((char *)flagsbuf,(char *)"\x0d");
      mmAddLine(mm,buf);
   }

   oldattr = mm->Attr;

   mm->Attr = mm->Attr & (FLAG_PVT|FLAG_CRASH|FLAG_FILEATTACH|FLAG_FILEREQ|FLAG_RREQ|FLAG_IRRR|FLAG_AUDIT|FLAG_HOLD);

   // Add own kludges

   if(area->AreaType == AREATYPE_NETMAIL)
   {
      if(mm->OrigNode.Zone != mm->DestNode.Zone || (config.cfg_Flags & CFG_FORCEINTL))
      {
         sprintf((char *)buf,"\x01" "INTL %u:%u/%u %u:%u/%u\x0d",
            mm->DestNode.Zone,
            mm->DestNode.Net,
            mm->DestNode.Node,
            mm->OrigNode.Zone,
            mm->OrigNode.Net,
            mm->OrigNode.Node);

         mmAddLine(mm,buf);
      }

      if(mm->OrigNode.Point)
      {
         sprintf((char *)buf,"\x01" "FMPT %u\x0d",mm->OrigNode.Point);
         mmAddLine(mm,buf);
      }

      if(mm->DestNode.Point)
      {
         sprintf((char *)buf,"\x01" "TOPT %u\x0d",mm->DestNode.Point);
         mmAddLine(mm,buf);
      }
   }

   if((config.cfg_Flags & CFG_ADDTID) && !isrescanning)
      AddTID(mm);

   if(isrescanning)
   {
      sprintf((char *)buf,"\x01RESCANNED %u:%u/%u.%u\x0d",area->Aka->Node.Zone,
                                                  area->Aka->Node.Net,
                                                  area->Aka->Node.Node,
                                                  area->Aka->Node.Point);
      mmAddLine(mm,buf);
   }

   // Message text

   if(msgtext)
   {
      // Extract origin address

      if(mm->Area[0])
      {
         uint32_t textpos,d;
         uint8_t originbuf[200];
         struct Node4D n4d;

         textpos=0;

         while(msgtext[textpos])
         {
            d=textpos;

            while(msgtext[d] != 13 && msgtext[d] != 0)
               d++;

            if(msgtext[d] == 13)
               d++;

            if(d-textpos > 11 && strncmp((char *)&msgtext[textpos],(char *)" * Origin: ",11)==0)
            {
               mystrncpy(originbuf,&msgtext[textpos],MIN(d-textpos,200));

               if(ExtractAddress(originbuf,&n4d))
                     Copy4D(&mm->Origin4D,&n4d);
            }

            textpos=d;
         }
      }

      if(!(mmAddBuf(&mm->TextChunks,msgtext,Header_S.TxtLen)))
      {
         JAM_DelSubPacket(SubPacket_PS);
         osFree(msgtext);
         mmFree(mm);
         return(FALSE);
      }
   }

   // Free JAM message

   if(msgtext) osFree(msgtext);
   JAM_DelSubPacket(SubPacket_PS);

   // Message reading done

   if(isrescanning) mm->Flags |= MMFLAG_RESCANNED;
   else             mm->Flags |= MMFLAG_EXPORTED;

   if(!(*handlefunc)(mm))
   {
      mmFree(mm);
      return(FALSE);
   }

   if(!isrescanning)
   {
      scan_total++;

       // Update message header

        if(config.cfg_Flags & CFG_ALLOWKILLSENT)
        {
            if((oldattr & FLAG_KILLSENT) && (area->AreaType == AREATYPE_NETMAIL))
            {
                // Delete message with KILLSENT flag

                LogWrite(2,TOSSINGINFO,(uint8_t *)"Deleting message with KILLSENT flag");
               Header_S.Attribute |= MSG_DELETED;
            }
        }

       Header_S.Attribute |= MSG_SENT;

      Header_S.DateProcessed = time(NULL);
      Header_S.DateProcessed -= jam_utcoffset;

      // jam_openbases might have been changed in handlefunc

      if(!(ja=jam_getarea(area)))
      {
         mmFree(mm);
         return(FALSE);
      }

      if(JAM_LockMB(ja->Base_PS,10))
       {
          LogWrite(1,SYSTEMERR,(uint8_t *)"Timeout when trying to lock JAM messagebase \"%s\"",area->Path);
         mmFree(mm);
         return(FALSE);
       }

      if(JAM_ChangeMsgHeader(ja->Base_PS,num-ja->BaseNum,&Header_S))
         LogWrite(1,TOSSINGERR,(uint8_t *)"Failed to update header of message #%lu in JAM messagebase \"%s\"",num,area->Path);

      JAM_UnlockMB(ja->Base_PS);
   }

   mmFree(mm);
   return(TRUE);
}

BOOL jam_exportfunc(struct Area *area,BOOL (*handlefunc)(struct MemMessage *mm))
{
   uint32_t start,end;
   struct jam_Area *ja;

   // Open the area

   if(!(ja=jam_getarea(area)))
    {
        if(nomem)
          return(FALSE);

        return(TRUE); // Area did not exist and could not be created. Go on anyway.
    }

   if(config.cfg_jam_Flags & CFG_JAM_HIGHWATER)
      jam_gethighwater(ja);

   if(ja->HighWater) start=ja->HighWater+1;
   else              start=ja->BaseNum;

    if(start < ja->BaseNum)
        start=ja->BaseNum;

   end   = ja->BaseNum + ja->OldNum;

   while(start < end && !ctrlc)
   {
      if(!jam_ExportJAMNum(area,start,handlefunc,FALSE))
         return(FALSE);

      start++;
   }

   if(ctrlc)
      return(FALSE);

   ja->HighWater=end-1;

   return(TRUE);
}

BOOL jam_rescanfunc(struct Area *area,uint32_t max,BOOL (*handlefunc)(struct MemMessage *mm))
{
   uint32_t start;
   struct jam_Area *ja;

   // Open the area

   if(!(ja=jam_getarea(area)))
      return(FALSE);

   start=ja->BaseNum;

   if(max !=0 && ja->OldNum > max)
      start=ja->BaseNum+ja->OldNum-max;

   while(start < ja->BaseNum + ja->OldNum && !ctrlc)
   {
      if(!jam_ExportJAMNum(area,start,handlefunc,TRUE))
         return(FALSE);

      start++;
   }

   if(ctrlc)
      return(FALSE);

   return(TRUE);
}

// linking

struct Msg
{
   unsigned long MsgIdCRC;
   unsigned long ReplyCRC;
   unsigned long ReplyTo;
   unsigned long Reply1st;
   unsigned long ReplyNext;
   unsigned long OldReplyTo;
   unsigned long OldReply1st;
   unsigned long OldReplyNext;
};

int jam_CompareMsgIdReply(s_JamBase *Base_PS,struct Msg *msgs,uint32_t msgidmsg,uint32_t replymsg)
{
   int Status_I;
   s_JamMsgHeader     MsgIdHeader_S;
   s_JamMsgHeader     ReplyHeader_S;
   s_JamSubPacket*   MsgIdSubPacket_PS;
   s_JamSubPacket*   ReplySubPacket_PS;
   s_JamSubfield*    MsgIdField_PS = NULL;
   s_JamSubfield*    ReplyField_PS = NULL;

   if(msgs[msgidmsg].MsgIdCRC != msgs[replymsg].ReplyCRC)
      return(FALSE);

   if(config.cfg_jam_Flags & CFG_JAM_QUICKLINK)
      return(TRUE);

   Status_I = JAM_ReadMsgHeader(Base_PS,msgidmsg,&MsgIdHeader_S,&MsgIdSubPacket_PS );

   if(Status_I)
      return(FALSE);

   Status_I = JAM_ReadMsgHeader(Base_PS,replymsg,&ReplyHeader_S,&ReplySubPacket_PS );

   if(Status_I)
   {
      JAM_DelSubPacket(MsgIdSubPacket_PS);
      return(FALSE);
   }

   for ( MsgIdField_PS = JAM_GetSubfield( MsgIdSubPacket_PS ); MsgIdField_PS; MsgIdField_PS = JAM_GetSubfield( NULL ) )
      if(MsgIdField_PS->LoID == JAMSFLD_MSGID) break;

   for ( ReplyField_PS = JAM_GetSubfield( ReplySubPacket_PS ); ReplyField_PS; ReplyField_PS = JAM_GetSubfield( NULL ) )
      if(ReplyField_PS->LoID == JAMSFLD_REPLYID) break;

   if(!ReplyField_PS || !MsgIdField_PS)
   {
      JAM_DelSubPacket(MsgIdSubPacket_PS);
      JAM_DelSubPacket(ReplySubPacket_PS);
      return(FALSE);
   }

   if(ReplyField_PS->DatLen != MsgIdField_PS->DatLen)
   {
      JAM_DelSubPacket(MsgIdSubPacket_PS);
      JAM_DelSubPacket(ReplySubPacket_PS);
      return(FALSE);
   }

   if(strncmp((char *)ReplyField_PS->Buffer,(char *)MsgIdField_PS->Buffer,ReplyField_PS->DatLen) != 0)
   {
      JAM_DelSubPacket(MsgIdSubPacket_PS);
      JAM_DelSubPacket(ReplySubPacket_PS);
      return(FALSE);
   }

   JAM_DelSubPacket(MsgIdSubPacket_PS);
   JAM_DelSubPacket(ReplySubPacket_PS);

   return(TRUE);
}

//dest is a reply to num
void jam_setreply(struct Msg *msgs,uint32_t nummsgs,uint32_t base,uint32_t num,uint32_t dest)
{
   int n,times;

   if(msgs[dest].ReplyTo)
      return; // Already linked

   msgs[dest].ReplyTo=num+base;

   if(msgs[num].Reply1st == 0)
   {
      msgs[num].Reply1st=dest+base;
   }
   else
   {
      n=msgs[num].Reply1st-base;
      if(n == dest) return;

      if (n < 0 || n >= nummsgs)
      {
         // Oops! Base seems to be b0rken
         printf("Warning: message #%ld is linked to something outside the base\n", num + base);
         return;
      }

      times=0;

      while(msgs[n].ReplyNext)
      {
            times++;

            if(times > 1000) // Something appears to have gone wrong//
            {
              printf("Warning: >1000 replies to message %ld or circular reply links\n",num+base);
                return;
            }

         n=msgs[n].ReplyNext-base;
         if(n == dest) return;

         if (n < 0 || n >= nummsgs)
         {
            // Oops! Base seems to be b0rken
            printf("Warning: message #%ld is linked to something outside the base\n", num + base);
            return;
         }
      }

      msgs[n].ReplyNext=dest+base;
   }
}

int jam_linkmb(struct Area *area,uint32_t oldnum)
{
   struct jam_Area *ja;
   uint32_t nummsgs,res,c,d;
   struct Msg *msgs;

   printf("Linking JAM area %s                       \n",area->Tagname);
   fflush(stdout);

   if(!(ja=jam_getarea(area)))
      return(FALSE);

   if(JAM_GetMBSize(ja->Base_PS,&nummsgs))
   {
      LogWrite(1,TOSSINGERR,(uint8_t *)"Failed to get size of JAM area \"%s\"",area->Path);
      return(FALSE);
   }

   if(nummsgs == 0)
      return(TRUE); // Nothing to do

   // Read msgid/reply

   if(!(msgs=(Msg *)osAlloc(nummsgs*sizeof(struct Msg))))
   {
      LogWrite(1,SYSTEMERR,(uint8_t *)"Out of memory, cannot link JAM area %s",area->Tagname);
      return(FALSE);
   }

   for(c=0;c<nummsgs;c++)
   {
      s_JamMsgHeader         Header_S;

      res = JAM_ReadMsgHeader( ja->Base_PS, c, &Header_S, NULL);

      msgs[c].MsgIdCRC=-1;
      msgs[c].ReplyCRC=-1;
      msgs[c].ReplyTo=0;
      msgs[c].Reply1st=0;
      msgs[c].ReplyNext=0;
      msgs[c].OldReplyTo=0;
      msgs[c].OldReply1st=0;
      msgs[c].OldReplyNext=0;

      if(!res)
      {
         msgs[c].MsgIdCRC=Header_S.MsgIdCRC;
         msgs[c].ReplyCRC=Header_S.ReplyCRC;
         msgs[c].ReplyTo=Header_S.ReplyTo;
         msgs[c].Reply1st=Header_S.Reply1st;
         msgs[c].ReplyNext=Header_S.ReplyNext;
         msgs[c].OldReplyTo=Header_S.ReplyTo;
         msgs[c].OldReply1st=Header_S.Reply1st;
         msgs[c].OldReplyNext=Header_S.ReplyNext;
      }
   }

   for(c=oldnum;c<nummsgs;c++)
   {
      if(msgs[c].ReplyCRC != -1)
      {
         // See if this is a reply to a message //

         for(d=0;d<nummsgs;d++)
            if(jam_CompareMsgIdReply(ja->Base_PS,msgs,d,c))
               jam_setreply(msgs,nummsgs,ja->BaseNum,d,c);
      }

      if(msgs[c].MsgIdCRC != -1)
      {
          // See if there are any replies to this message //

         for(d=0;d<nummsgs;d++)
            if(jam_CompareMsgIdReply(ja->Base_PS,msgs,c,d))
               jam_setreply(msgs,nummsgs,ja->BaseNum,c,d);
      }
   }

   // Update links //

   for(c=0;c<nummsgs;c++)
      if(msgs[c].ReplyTo != msgs[c].OldReplyTo || msgs[c].Reply1st != msgs[c].OldReply1st || msgs[c].ReplyNext != msgs[c].OldReplyNext)
      {
         s_JamMsgHeader         Header_S;

         if(JAM_LockMB(ja->Base_PS,10))
         {
            LogWrite(1,SYSTEMERR,(uint8_t *)"Timeout when trying to lock JAM messagebase \"%s\"",area->Path);
            osFree(msgs);
                return(FALSE);
         }

         res = JAM_ReadMsgHeader( ja->Base_PS, c, &Header_S, NULL);

         if(!res)
         {
            Header_S.ReplyTo=msgs[c].ReplyTo;
            Header_S.Reply1st=msgs[c].Reply1st;
            Header_S.ReplyNext=msgs[c].ReplyNext;

            JAM_ChangeMsgHeader(ja->Base_PS,c,&Header_S);
            JAM_UnlockMB(ja->Base_PS);
         }
      }

   osFree(msgs);

    return(TRUE);
}

*/
