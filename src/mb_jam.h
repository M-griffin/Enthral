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

# ifndef MSG_MBJAM_H
# define MSG_MBJAM_H

#include "jam.h"
#include "stamp.h"

#include <string>
#include <vector>

#define NO_TYPEDEF_UCHAR
#define NO_TYPEDEF_USHORT
#define NO_TYPEDEF_ULONG

/* Header flags */
#define FLAG_PVT             1
#define FLAG_CRASH           2
#define FLAG_RECD            4
#define FLAG_SENT            8
#define FLAG_FILEATTACH     16
#define FLAG_INTRANSIT      32
#define FLAG_ORPHAN         64
#define FLAG_KILLSENT      128
#define FLAG_LOCAL         256
#define FLAG_HOLD          512
/*      FLAG_UNUSED       1024 */
#define FLAG_FILEREQ      2048
#define FLAG_RREQ         4096
#define FLAG_IRRR         8192
#define FLAG_AUDIT       16384
#define FLAG_UPDATEREQ   32768

#define AMIN(a,b)  ((a)<(b)? (a):(b))

#define BOOL int

typedef enum {LOCAL, NETMAIL, ECHOMAIL, EMAIL, NEWS} MSGTYPE;
typedef enum {PUBLIC, PRIVATE} POSTTYPE;


struct jbList
{
    struct jbNode *First;
    struct jbNode *Last;
};

struct jbNode
{
    struct jbNode *Next;
};

struct Node4D
{
    uint16_t Zone,Net,Node,Point;
};

struct MemMessage
{
    uint32_t msgnum;

    struct Node4D OrigNode;
    struct Node4D DestNode;

    uint8_t Area[80];

    uint8_t To[36];
    uint8_t From[36];
    uint8_t Subject[72];
    //uint8_t DateTime[20];
    time_t DateTime;

    uint8_t MSGID[80];
    uint8_t REPLY[80];

    uint32_t MSGID_CRC;
    uint32_t REPLY_CRC;

    uint16_t Attr;
    uint16_t Cost;

    uint8_t Type;
    uint16_t Flags;

    uint32_t HighWater; // Total Messages (Includes Deleted)
    uint32_t Active;    // Active Messages (Excludes Deleted)
    uint32_t CurrMsg;   // Current Message

    std::string TextChunks;

    //struct jbList TextChunks;
    //struct jbList SeenBy;
    //struct jbList Path;
};

struct openbase
{
    uint32_t lastuse;
    s_JamBase* Base_PS;
    struct jam_Area *area;
};

struct jam_Area
{
    struct jam_Area *Next;
    mb_list_rec *area; //   struct Area *area;
    s_JamBase *Base_PS;
    uint32_t BaseNum;
    uint32_t OldNum;
    uint32_t OldHighWater;
    uint32_t HighWater;
    uint32_t Active;    // Only Active, Excluding delete messages.
    BOOL newmsg;
};

time_t stampToTimeT(struct _stamp *st);
struct _stamp *timeTToStamp(time_t tt);

union stamp_combo *TmDate_to_DosDate(struct tm *tmdate, union stamp_combo *dosdate);
struct tm *DosDate_to_TmDate(union stamp_combo *dosdate, struct tm *tmdate);


BOOL  jamapi_purgemsg(mb_list_rec *area, uint32_t msgnum);
BOOL  jamapi_writemsg(struct MemMessage *mm, mb_list_rec *area);

uint32_t jamapi_countmsgs(mb_list_rec *area, UserRec *thisuser=NULL);

vector < unsigned long > jamapi_build_private(mb_list_rec *area, UserRec *thisuser=NULL);
vector < unsigned long > jamapi_build_public(mb_list_rec *area, UserRec *thisuser=NULL);

BOOL  jamapi_readmsg(mb_list_rec *area, uint32_t num, struct MemMessage *mm, int email=FALSE, UserRec *thisuser=NULL);

uint32_t jamapi_readmsgid(mb_list_rec *area, uint32_t num, std::string &tmpMsgId);

uint32_t buildmsgid(uint32_t MsgID, mb_list_rec *area);

#endif

