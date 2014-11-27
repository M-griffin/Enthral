/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose:  This file is a mix of old SMAPI libs types that were used   *
 *             and are kept for new jamlib conversion to working functions *
 *             already inplace.                                            *
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

# ifndef MSG_MBAPI_JAM_H
# define MSG_MBAPI_JAM_H

//# include "typedefs.h"
# include "language.h"
# include "msg_readll.h"
# include "msgs.h"
# include "stamp.h"

# include <string>

using namespace std;


class mbapi_jam : public SESSION
{

public:

    /*
     *  Mb Message API is a WIP, Interface originally for husky SMAPI
     *  Rewritten to keep original compability with already written functions
     *  With new JamLib API.  Have to sort through and remove variables lateron not used anymore.
     */

    typedef dword UMSGID;

    struct _netaddr
    {
        unsigned short zone;
        unsigned short net;
        unsigned short node;
        unsigned short point;
    };

    typedef struct _xmsg
    {
        /* Bitmasks for 'attr' */

#define MSGPRIVATE 0x0001
#define MSGCRASH   0x0002
#define MSGREAD    0x0004
#define MSGSENT    0x0008
#define MSGFILE    0x0010
#define MSGFWD     0x0020     /* is also called intransit flag */
#define MSGORPHAN  0x0040
#define MSGKILL    0x0080
#define MSGLOCAL   0x0100
#define MSGHOLD    0x0200
#define MSGXX2     0x0400     /* you can use this flag as "Direct" attribute */
#define MSGFRQ     0x0800
#define MSGRRQ     0x1000
#define MSGCPT     0x2000
#define MSGARQ     0x4000
#define MSGURQ     0x8000
#define MSGSCANNED 0x00010000L
#define MSGUID     0x00020000L /* xmsg.uid field contains umsgid of msg */
#define MSGIMM     0x00040000L /* Use only if msgtype == MSGTYPE_JAM !
        Used to map the Jam "immediate" attribute. */
#define MSGLOCKED  0x40000000L /* this seems to be a feature of golded  */

        dword attr;

#define XMSG_FROM_SIZE  36
#define XMSG_TO_SIZE    36
#define XMSG_SUBJ_SIZE  72

        unsigned char from[XMSG_FROM_SIZE];
        unsigned char to[XMSG_TO_SIZE];
        unsigned char subj[XMSG_SUBJ_SIZE];

        _netaddr orig;               /* Origination and destination addresses */
        _netaddr dest;

        struct _stamp date_written;   /* When user wrote the msg (UTC) */
        struct _stamp date_arrived;    /* When msg arrived on-line (UTC) */

        sword utc_ofs;              /* Offset from UTC of message writer, in minutes. */


#define MAX_REPLY 9            /* Max number of stored replies to one msg */

        UMSGID replyto;
        UMSGID replies[MAX_REPLY];
        dword umsgid;               /* UMSGID of this message, if (attr&MSGUID) */
        /* This field is only stored on disk -- it  *
         * is not read into memory.                 */

        byte __ftsc_date[20];       /* Obsolete date information.  If it weren't
                                     * for the fact that FTSC standards say that
                                     * one cannot modify an in-transit message,
                                     * I'd be VERY tempted to axe this field
                                     * entirely, and recreate an FTSC-compatible
                                     * date field using the information in
                                     * 'date_written' upon export.  Nobody should
                                     * use this field, except possibly for tossers
                                     * and scanners.  All others should use one
                                     * of the two binary datestamps, above. */
    }
    XMSG;

    struct _msgh
    {
        //    MSG *sq;
        dword id;

        dword bytes_written;
        dword cur_pos;
    };

    struct _msgapi
    {
        dword id;                   /* Must always equal MSGAPI_ID */

        word len;                   /* LENGTH OF THIS STRUCTURE! */
        word type;

        dword num_msg;
        dword cur_msg;
        dword high_msg;
        dword high_water;

        word sz_xmsg;

        byte locked;                /* Base is locked from use by other tasks */
        byte isecho;                /* Is this an EchoMail area?              */

        /* Function pointers for manipulating messages within this area. */
    };

# define MSG_QUOTE    0
# define MSG_TEXT    1
# define MSG_TEAR    2
# define MSG_ORIGIN    3

    typedef enum {LOCAL, NETMAIL, ECHOMAIL, EMAIL, NEWS} MSGTYPE;
    typedef enum {PUBLIC, PRIVATE} POSTTYPE;

    std::string   buff;
    char          cinfbuf[CTLLEN];
    unsigned long buflen, cinflen;
    unsigned long MH,ML,MO,MR;

    // Handle to Current User
    UserRec *thisuser;

    typedef struct
    {

        unsigned char From[XMSG_FROM_SIZE];
        unsigned char To[XMSG_TO_SIZE];
        unsigned char Subj[XMSG_SUBJ_SIZE];
        char AreaName[61];
        struct _stamp date_written;
        struct _stamp date_arrived;
        char *Text;
        int colors[4];
        unsigned int replyto, replies[MAX_REPLY];
        unsigned long num_msg, cur_msg, high_msg, high_water, active;
        unsigned long attr;
        _netaddr orig;
        _netaddr dest;

    } MsgInfoRec;

    // Message API Handles
    MsgInfoRec      MI;
    XMSG            xmsg;
    mb_list_rec     mr;

    typedef struct MsgHead
    {

        char curmsg[10];
        char totmsg[10];
        char from[40];
        char to[40];
        char subj[80];
        char flags[10];
        char time[81];
        char area[50];

    } MsgHead;

    // Helper Calsses
    MsgHead        mHead;
    msg_readll     mLink;
    language       _lang;
    msgs           _msgf;

    // Mesage Packing Variables
    unsigned long msgCopied,   msgProcessed;   // per Area
    unsigned long totaloldMsg, totalmsgCopied;


    // Constructor - Initalize MSGAPI!
    mbapi_jam()
    {

        start_session ( thisuser );
    }

    // Message Posting Functions
    char *faddr2char ( char *s,fidoaddr *fa );
    int  SaveMsg ( unsigned long msgarea, unsigned long msgnum, int NewReply );
    void MakeCtrlHdr ( char *reply );
    void GetMsgID ( char *reply );
    void fill_xmsg ( char *from, char *to, char *subj );

    // Message Main Setup functions
    void Add2MsgInfo();
    void mm2MsgInfo ( struct MemMessage *mm );

    void MessageDeleted ( MemMessage *mm );
    void MessageNotFound ( MemMessage *mm );

    // Main Read Message Per Area Function
    int ReadMsgArea ( unsigned long mbnum, int email = FALSE );

//   time_t stampToTimeT(struct _stamp *st);
//   struct _stamp *timeTToStamp(time_t tt);

    void FidoFlags ( char *fflags );
    void SetupMsgHdr();

    char *strrepl ( char *Str, size_t BufSiz, const char *OldStr, const char *NewStr );
    void stripCR ( char *ostr );
    void stripCRONLY ( char *ostr );

    void parseMCI ( std::string &msgtext );
    void MsgSetupTxt();
    void MsgSetupQuoteTxt();
    void MsgShowTxt2();

    int  GetMsgInfo();
    int  GetMsg();

    void SetLastRead ( unsigned long usr, unsigned long lr );

    unsigned
    long GetLastRead ( unsigned long usr );

    // Message Packing Functions
    void doArea ( int marea, char *cmp );
    int  packmsgarea ( int marea );
};

# endif
