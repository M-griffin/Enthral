/*
    JAMLIB - A JAM subroutine library
    Copyright (C) 1999 Björn Stenberg

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Changes made by Johan Billing 2000-04-16:

    - Added #defines for JAM_NO_MESSAGE and JAM_CORRUPT_MSG
    - Added #ifndef linux around typedefs for ushort and uint32_t
    - Added prototype for JAM_AddEmptyMessage()

    Backported changes from JAMLIB 1.4.7 made by Johan Billing 2003-10-26

    - Added prototype for JAM_DeleteMessage()
*/

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

/***********************************************************************
**
**  JAM Definitions
**
***********************************************************************/


#ifndef __JAM_H__
#define __JAM_H__

#include <cstdlib>
#include <cstdio>
#include <cctype>

#include <stdint.h>


//#ifndef linux
//typedef unsigned short ushort;   /* must be 16 bits wide */
//typedef unsigned long  ulong;    /* must be 32 bits wide */
//#endif

//typedef unsigned char  uchar;    /* must be  8 bits wide */

/*
typedef uint16_t ushort;   // must be 16 bits wide
typedef uint32_t ulong;    // must be 32 bits wide   // in <stdint.h>
typedef uint8_t  uchar;    // must be  8 bits wide
*/

/*
**  Error codes
*/
#define JAM_BAD_PARAM   1  /* one or more parameters are wrong */
#define JAM_IO_ERROR		2  /* i/o error. check JAM_Errno() for details */
#define JAM_LOCK_FAILED 3  /* lock could not be set */
#define JAM_NOT_LOCKED  4  /* the message base was not locked before writing */
#define JAM_NO_MEMORY   5  /* out of memory! */
#define JAM_NO_USER	6  	/* user not found */
#define JAM_NO_MESSAGE  7  /* message has been deleted */
#define JAM_CORRUPT_MSG 8  /* message header is corrupt */

/*
**  CRC definitions
*/

#define JAM_NO_CRC	0xffffffff

/*
**  File extensions
*/
#define EXT_HDRFILE     ".jhr"
#define EXT_TXTFILE     ".jdt"
#define EXT_IDXFILE     ".jdx"
#define EXT_LRDFILE     ".jlr"

/*
**  Revision level and header signature
*/
#define CURRENTREVLEV   1
#define HEADERSIGNATURE "JAM\0"

/*
**  Header file information block, stored first in all .JHR files
*/
typedef struct
{
    uint8_t  Signature[4];      /* <J><A><M> followed by <NUL> */
    uint32_t  DateCreated;       /* Creation date */
    uint32_t  ModCounter;        /* Last processed counter */
    uint32_t  ActiveMsgs;        /* Number of active (not deleted) msgs */
    uint32_t  PasswordCRC;       /* CRC-32 of password to access */
    uint32_t  BaseMsgNum;        /* Lowest message number in index file */
    uint8_t  RSRVD[1000];       /* Reserved space */
} s_JamBaseHeader;

/*
**  Message status bits
*/
#define MSG_LOCAL       0x00000001L /* Msg created locally */
#define MSG_INTRANSIT   0x00000002L /* Msg is in-transit */
#define MSG_PRIVATE     0x00000004L /* Private */
#define MSG_READ        0x00000008L /* Read by addressee */
#define MSG_SENT        0x00000010L /* Sent to remote */
#define MSG_KILLSENT    0x00000020L /* Kill when sent */
#define MSG_ARCHIVESENT 0x00000040L /* Archive when sent */
#define MSG_HOLD        0x00000080L /* Hold for pick-up */
#define MSG_CRASH       0x00000100L /* Crash */
#define MSG_IMMEDIATE   0x00000200L /* Send Msg now, ignore restrictions */
#define MSG_DIRECT      0x00000400L /* Send directly to destination */
#define MSG_GATE        0x00000800L /* Send via gateway */
#define MSG_FILEREQUEST 0x00001000L /* File request */
#define MSG_FILEATTACH  0x00002000L /* File(s) attached to Msg */
#define MSG_TRUNCFILE   0x00004000L /* Truncate file(s) when sent */
#define MSG_KILLFILE    0x00008000L /* Delete file(s) when sent */
#define MSG_RECEIPTREQ  0x00010000L /* Return receipt requested */
#define MSG_CONFIRMREQ  0x00020000L /* Confirmation receipt requested */
#define MSG_ORPHAN      0x00040000L /* Unknown destination */
#define MSG_ENCRYPT     0x00080000L /* Msg text is encrypted */
#define MSG_COMPRESS    0x00100000L /* Msg text is compressed */
#define MSG_ESCAPED     0x00200000L /* Msg text is seven bit ASCII */
#define MSG_FPU         0x00400000L /* Force pickup */
#define MSG_TYPELOCAL   0x00800000L /* Msg is for local use only (no export) */
#define MSG_TYPEECHO    0x01000000L /* Msg is for conference distribution */
#define MSG_TYPENET     0x02000000L /* Msg is direct network mail */
#define MSG_NODISP      0x20000000L /* Msg may not be displayed to user */
#define MSG_LOCKED      0x40000000L /* Msg is locked, no editing possible */
#define MSG_DELETED     0x80000000L /* Msg is deleted */

/*
**  Message header
*/
typedef struct
{
    uint8_t  Signature[4];              /* <J><A><M> followed by <NUL> */
    uint16_t Revision;                  /* CURRENTREVLEV */
    uint16_t ReservedWord;              /* Reserved */
    uint32_t  SubfieldLen;               /* Length of Subfields */
    uint32_t  TimesRead;                 /* Number of times message read */
    uint32_t  MsgIdCRC;                  /* CRC-32 of MSGID line */
    uint32_t  ReplyCRC;                  /* CRC-32 of REPLY line */
    uint32_t  ReplyTo;                   /* This msg is a reply to.. */
    uint32_t  Reply1st;                  /* First reply to this msg */
    uint32_t  ReplyNext;                 /* Next msg in reply chain */
    uint32_t  DateWritten;               /* When msg was written */
    uint32_t  DateReceived;              /* When msg was received/read */
    uint32_t  DateProcessed;             /* When msg was processed by packer */
    uint32_t  MsgNum;                    /* Message number (1-based) */
    uint32_t  Attribute;                 /* Msg attribute, see "Status bits" */
    uint32_t  Attribute2;                /* Reserved for future use */
    uint32_t  TxtOffset;                 /* Offset of text in text file */
    uint32_t  TxtLen;                    /* Length of message text */
    uint32_t  PasswordCRC;               /* CRC-32 of password to access msg */
    uint32_t  Cost;                      /* Cost of message */
} s_JamMsgHeader;

/*
**  Message header Subfield types
*/
#define JAMSFLD_OADDRESS    0
#define JAMSFLD_DADDRESS    1
#define JAMSFLD_SENDERNAME  2
#define JAMSFLD_RECVRNAME   3
#define JAMSFLD_MSGID       4
#define JAMSFLD_REPLYID     5
#define JAMSFLD_SUBJECT     6
#define JAMSFLD_PID         7
#define JAMSFLD_TRACE       8
#define JAMSFLD_ENCLFILE    9
#define JAMSFLD_ENCLFWALIAS 10
#define JAMSFLD_ENCLFREQ    11
#define JAMSFLD_ENCLFILEWC  12
#define JAMSFLD_ENCLINDFILE 13
#define JAMSFLD_EMBINDAT    1000
#define JAMSFLD_FTSKLUDGE   2000
#define JAMSFLD_SEENBY2D    2001
#define JAMSFLD_PATH2D      2002
#define JAMSFLD_FLAGS       2003
#define JAMSFLD_TZUTCINFO   2004
#define JAMSFLD_UNKNOWN     0xffff

/*
**  Message header Subfield
*/
typedef struct
{
    uint16_t LoID;       /* Field ID, 0 - 0xffff */
    uint16_t HiID;       /* Reserved for future use */
    uint32_t  DatLen;     /* Length of buffer that follows */
    uint8_t* Buffer;     /* DatLen bytes of data */
} s_JamSubfield;

typedef struct
{
    uint16_t LoID;       /* Field ID, 0 - 0xffff */
    uint16_t HiID;       /* Reserved for future use */
    uint32_t  DatLen;     /* Length of buffer that follows */
} s_JamSaveSubfield;

/*
**  Message index record
*/
typedef struct
{
    uint32_t  UserCRC;    /* CRC-32 of destination username */
    uint32_t  HdrOffset;  /* Offset of header in .JHR file */
} s_JamIndex;

/*
**  Lastread structure, one per user
*/
typedef struct
{
    uint32_t  UserCRC;     /* CRC-32 of user name (lowercase) */
    uint32_t  UserID;      /* Unique UserID */
    uint32_t  LastReadMsg; /* Last read message number */
    uint32_t  HighReadMsg; /* Highest read message number */
} s_JamLastRead;

/*
**  JAMLIB message base handle
*/
typedef struct
{
    FILE* HdrFile_PS;      /* File handle for .JHR file */
    FILE* TxtFile_PS;      /* File handle for .JDT file */
    FILE* IdxFile_PS;      /* File handle for .JDX file */
    FILE* LrdFile_PS;      /* File handle for .JLR file */
    int   Errno_I;	   /* last i/o error */
    int   Locked_I;	   /* is area locked? */
    uint32_t LastUserPos_I;   /* last position of lastread record */
    uint32_t LastUserId_I;    /* userid for the last read lastread record */
} s_JamBase;

/*
**  JAMLIB subfield packet
*/
typedef struct
{
    s_JamSubfield** Fields;
    uint32_t	    NumFields;
    uint32_t	    NumAlloc;
} s_JamSubPacket;


/*
**  JAMLIB function declarations
*/

/* mbase.c */
int JAM_OpenMB   ( uint8_t* Basename_PC, s_JamBase** NewArea_PPS );
int JAM_CloseMB  ( s_JamBase* Area_PS );
int JAM_CreateMB ( uint8_t* Basename_PC, uint32_t BaseMsg_I, s_JamBase** NewArea_PPS );
int JAM_RemoveMB ( s_JamBase* Area_PS, uint8_t* Basename_PC );
int JAM_LockMB	 ( s_JamBase* Area_PS, int Timeout_I );
int JAM_UnlockMB ( s_JamBase* Area_PS );

int JAM_ReadMBHeader ( s_JamBase* Area_PS, s_JamBaseHeader* Header_PS );
int JAM_WriteMBHeader( s_JamBase* Area_PS, s_JamBaseHeader*	Header_PS );

int JAM_FindUser
(
    s_JamBase*	Area_PS,
    uint32_t 		UserCrc_I,
    uint32_t 		StartMsg_I,
    uint32_t* 		MsgNo_PI
);

int JAM_GetMBSize	(s_JamBase* Area_PS, uint32_t* Messages_PI );

/* message.c */

int JAM_ReadMsgHeader
(
    s_JamBase* 		    Area_PS,
    uint32_t               MsgNo_I,
    s_JamMsgHeader* 	Header_PS,
    s_JamSubPacket** 	SubfieldPack_PPS
);

int JAM_ReadMsgText
(
    s_JamBase* 	Area_PS,
    uint32_t 		Offset_I,
    uint32_t 		Length_I,
    uint8_t* 		Buffer_PC
);

int JAM_AddMessage
(
    s_JamBase* 		Area_PS,
    s_JamMsgHeader*	Header_PS,
    s_JamSubPacket*	SubPack_PS,
    uint8_t*		Text_PC,
    uint32_t			TextLen_I
);

int JAM_AddEmptyMessage	( s_JamBase* 		Area_PS );
int JAM_DeleteMessage	( s_JamBase*		Base_PS, uint32_t MsgNo_I );
int JAM_ChangeMsgHeader	( s_JamBase* 		Area_PS, uint32_t MsgNo_I, s_JamMsgHeader* Header_PS );
int JAM_ClearMsgHeader	( s_JamMsgHeader* 	Header_PS );
int JAM_Errno		    ( s_JamBase*        Area_PS );

/* lastread.c */

int JAM_ReadLastRead	( s_JamBase* Area_PS, uint32_t User_I, s_JamLastRead* Record_PS );
int JAM_WriteLastRead	( s_JamBase* Area_PS, uint32_t User_I, s_JamLastRead* Record_PS );

/* subpacket.c */

s_JamSubPacket* JAM_NewSubPacket( void );

int	JAM_DelSubPacket	( s_JamSubPacket* SubPack_PS );

s_JamSubfield* 	JAM_GetSubfield		( s_JamSubPacket* SubPack_PS );
s_JamSubfield*	JAM_GetSubfield_R	( s_JamSubPacket* SubPack_PS, uint32_t* Count_PI);

int JAM_PutSubfield		( s_JamSubPacket* SubPack_PS, s_JamSubfield* Field_PS );

/* crc32.c */

uint32_t JAM_Crc32	(uint8_t* Buffer_PC, uint32_t Length_I);

#endif

