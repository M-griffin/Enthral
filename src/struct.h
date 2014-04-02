/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Global Data Structures                                       *
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

# ifndef STRUCT_H
# define STRUCT_H

#include <string>
#include <stdint.h> // int32_t etc type for 32/64 Bit.

using namespace std;

# define BBSVERSION            "Enthral BBS [v.631/Alpha]"
extern char OSSYSTEM[1024];
extern int UTF8Output;

extern std::string screen_buffer;

//typedef uint32_t ulong;    /* must be 32 bits wide */  // in <stdint.h>
//typedef uint16_t ushort;   /* must be 16 bits wide */
//typedef uint8_t  uchar;    /* must be  8 bits wide */

# define CTLLEN 1024

# define SP              0x20
# define ESC             '\x1b'
# define BS              0x08
# define DEL             0x7f

# define CTRLA           0x01
# define CTRLB           0x02
# define CTRLC           0x03
# define CTRLD           0x04
# define CTRLE           0x05
# define CTRLF           0x06
# define CTRLG           0x07
# define CTRLH           0x08
# define CTRLI           0x09
# define CTRLJ           0x0a
# define CTRLK           0x0b
# define CTRLL           0x0c
# define CTRLM           0x0d
# define CTRLN           0x0e
# define CTRLO           0x0f
# define CTRLP           0x10
# define CTRLQ           0x11
# define CTRLR           0x12
# define CTRLS           0x13
# define CTRLT           0x14
# define CTRLU           0x15
# define CTRLV           0x16
# define CTRLW           0x17
# define CTRLX           0x18
# define CTRLY           0x19
# define CTRLZ           0x1a

// This holds the info for each Telnet Session
// For passing Between Classes
//________________________________________________________________


extern char BBSPATH[255];
extern char DATAPATH[255];
extern char MENUPATH[255];
extern char ANSIPATH[255];
extern char MESGPATH[255];
extern char FILEPATH[255];
extern char LOCKPATH[255];
extern char INIPATH[255];
extern char DAPATH[255];
extern char NODEPATH[255];
extern char NODEDIR[10];
extern char USRSIG[255];
extern char SCRIPTS[255];
extern char ENTHRALTMP[255];

extern char UsersIP[1024];
extern char UsersHOST[1024];
extern int  UserLoggedIn;

extern char INPUT_COLOR[7];
extern int  CURRENT_MAREA;
extern int  CURRENT_FAREA;
extern int  CURRENT_MSGTITLE;
extern int  CURRENT_ULIST;
extern int  USE_CHAR;
extern int  NODE_NUM;

extern int  TERM_HEIGHT;
extern int  TERM_WIDTH;
extern int  MAX_NODES;

extern int  isSysop;			// Exclusive Message Board Access to Sysop Areas!  HACK!!
extern char SYSOP_NAME[80];
extern char SYSTEM_NAME[80];

extern char CLIENT_TERM[255];

extern char STARTUP_SCRIPT[255];
extern char STARTUP_SCRIPT2[255];

extern int  isANSI;				// Detected and useing ANSI Terminal.
extern int  isANSIMATION;
extern int  TOGGLE_BACKSPACE;
extern int  PAUSE_SCROLING;		// Used to Pause Scrolling Text during Node Messages / Notifications.

extern  struct termios old_termios;

# define BOOL int
# define BitSet(arg,posn) ((arg) | (1L << (posn)))
# define BitClr(arg,posn) ((arg) & ~(1L << (posn)))
# define BitTst(arg,posn) BOOL((arg) & (1L << (posn)))
# define BitFlp(arg,posn) ((arg) ^ (1L << (posn)))

//extern int TRUE;
//extern int FALSE;
#define TRUE 1
#define FALSE 0

// History Data.
enum
{
    HIST_CALLS,
    HIST_EMAILS,
    HIST_POSTS,
    HIST_REPLIES,
    HIST_EDITS,
    HIST_DELETES,
    HIST_VIEWS,
    HIST_LOCAL,
    HIST_ECHOMAIL,
    HIST_UPLOADS,
    HIST_UPLOADKB,
    HIST_DOWNLOADS,
    HIST_DOWNLAODSKB,
    HIST_LASTCALLTIME,
    HIST_LASTCALLER
};


// Link List for Holding Message and Linklist Lines in FSE and msg_readll
typedef struct LineRec
{
    std::string data;      // Main Data String
    uint16_t    flag;      // Message Quote, Text Flagged True/False
    uint16_t    lineNum;   // Current Line Number
    LineRec     *up_link;
    LineRec     *dn_link;

} LineRec;

typedef struct __attribute__((packed)) FILEAREA
{
    uint32_t  idx;			// Index in Dat files
    uint32_t  num;			// Number of Files in Base
    uint8_t   name[30];		// Base Name
    uint8_t   path[255];	// Path to files
    // SL
    // Password
} FILEAREA;

struct ListItem
{
    FILEAREA area;
    struct ListItem *next;
};

/// Just a rough draft, redo file layout!
typedef struct __attribute__((packed)) FILES
{

    uint32_t idx;		// Index in Dat files
    uint8_t name[80];	// Filename
    //long Date;	// Date Uploaded
    uint32_t size;		// File Size in bytes
    uint8_t sSize;		// Converted size to KB, MEG, GIG
    // Flag - Valid / Invalid
} FILES;

typedef struct list_bar
{
    uint16_t     x; // Screen Cords
    uint16_t     y; // Screen Cord
    uint32_t isnew; // has new / or count of total.
    uint32_t msgnum;
    uint32_t msgid;  // CRC32
    uint32_t repid;  // CRC32
    std::string ansi_1;
    std::string ansi_2;
    std::string ansi_3;
    std::string ansi_4;

} list_bar;


typedef struct AreaRec
{
    list_bar lbar;
    struct AreaRec *next;

} AreaRec;


struct ListItem2
{
    FILES  files;
    struct ListItem *next;
};

typedef struct __attribute__((packed)) LangRec
{
    uint8_t Lang[1024];

} LangRec;

typedef struct __attribute__((packed)) MenuFlags
{
    bool ClrScrBefore;  // clear screen before menu display
    bool NoMenuPrompt;  // no menu prompt whatsoever?
    bool ForcePause;    // force a pause before menu display?
    bool ClrScrAfter;   // clear screen after command received
    bool UseGlobal;     // use global menu commands?
    bool DigitPassing;  // Pass Though Digit Input for Functions
    bool EscPassing;    // Pass Though ESC Keys, Arrorw, Pgup/dn, Esc, Home etc...
    bool EOSPrompt;     // End of Screen Prompt Lightbars XY instead of Hardcoded.

} MenuFlags;

typedef struct MenuRec
{
    std::string MenuName;     // menu name
    std::string Directive;    // normal menu text file
    std::string MenuPrompt;   // menu prompt
    std::string Acs;          // access requirements
    std::string Password;     // password required

    uint16_t  ForceInput;     // 0 user defaults, 1 Hotkeys 1 ENTER
    uint16_t  ForceHelpLevel; // 0 user defaults, 1 force display, 0 force expert
    bool      Lightbar;       // Lightbar Menu?
    MenuFlags MFlags;         // menu status variables }

} MenuRec;

typedef struct CommandRec
{
    std::string  LDesc;       // command description
    std::string  SDesc;       // command string
    std::string  CKeys;       // command execution keys
    std::string  Acs;         // access requirements
    std::string  CmdKeys;     // command keys
    std::string  MString;     // command data
    bool  SText;              // Is this a Scroll Test Comamnd
    uint16_t STLen;           // Length of Scroll Text
    bool  LBarCmd;            // Is This a Lighbar Cmd
    std::string  HiString;    // Highlighed
    std::string  LoString;    // Unhighlighted
    uint16_t Xcoord;          // Lightbar X coord
    uint16_t Ycoord;          // Lightbar Y coord

} CommandRec;

// Used for passing menu commands between threads for scrolling
typedef struct PASSING
{
    bool 						Active;
    CommandRec                  *cmdr2;

} PASSING;


typedef struct __attribute__((packed)) Callers
{
    uint32_t idx;
    uint32_t dtLastOn;

} Callers;

typedef struct __attribute__((packed)) UserIdx
{
    uint8_t  handle[30];
    uint32_t num;

} UserIdx;

typedef struct __attribute__((packed)) UserRec
{
    uint32_t
    idx,
    num;

    // Stats Info
    uint8_t
    handle[30],
    name[30],
    password[30],
    sex,
    email[40],
    usernote[40],
    startmenu[20],
    c_question[80],
    c_answer[80],
    msg_txtcolor[3],
    msg_quotecolor[3];

    // Stats
    int32_t
    dtbday,
    dtfirston,
    dtlaston;

	uint32_t
    lastmbarea,
    lastmsg,
    // History
    Calls,
    Emails,
    Posts,
    Replies,
    Edits,
    Deletes,
    Views,
    Local,
    Echomail,
    Uploads,
    UploadKb,
    Downloads,
    DownloadKb;

    uint16_t
    bsdel_swap,
    unused,
    emprivate;

    // FS Message Reader Options
    uint16_t
    readertheme,
    fsetheme,
    pageDn,      // Arror Keys - Page Down or line down
    readerFs;    // Use FullScreen Reader or Old Fashon.

} UserRec;


typedef struct __attribute__((packed)) fidoaddr
{
    uint16_t
    zone,
    net,
    node,
    point;

    uint8_t
    domain[13];

} fidoaddr;

typedef struct __attribute__((packed)) mbareaflags
{
    uint8_t mbrealname;
    uint8_t mbvisible;       // Sysop Only!!
    uint8_t mbansi;
    uint8_t mb8bitx;
    uint8_t mbstrip;
    uint8_t mbaddtear;
    uint8_t mbnopubstat;
    uint8_t mbnocreadit;
    uint8_t mbinternet;
    uint8_t mbfileattach;
    uint8_t mbstripcolor;

} mbareaflags;

typedef struct __attribute__((packed)) mb_list_rec
{
    uint16_t idx;
    uint16_t Type;
    uint16_t Kind;
    uint16_t Active;
    uint16_t Pubpriv;
    uint16_t Allowalias;
    uint16_t scantype;
    uint16_t fidoaddr;
    uint16_t sigtype;
    uint16_t mbdisplay[61];
    uint16_t mbfile[36];
    uint16_t mbpath[81];
    uint16_t password[16];
    uint8_t  origin_line[66];
    uint8_t  colors[4];
    uint8_t  infofile[36];
    uint32_t MaxMsgs;
    uint32_t MaxAge;
    uint32_t first;
    uint32_t last;
    uint32_t current;
    uint32_t lastread;
    uint32_t status;
    mbareaflags flags;
    struct fidoaddr aka;

} mb_list_rec;


typedef struct __attribute__((packed)) fb_list_rec
{
    uint16_t idx;
    uint16_t Type;               // JAM
    uint16_t Kind;               // Local
    uint16_t Active;
    uint16_t Pubpriv;            // Private File Area
    uint16_t Allowalias;         // hmm ?!?
    uint16_t scantype;           //
    uint16_t fidoaddr;
    uint16_t sigtype;
    uint16_t reserved_int[15];
    uint8_t  filepath[1024];    // Path to import files.
    uint8_t  fbdisplay[61];     // Ansi File Header Name
    uint8_t  fbfile[36];        // File Board DOS Filename
    uint8_t  fbpath[81];        // File Board Path
    uint8_t  password[16];
    uint8_t  OriginLine[66];
    uint8_t  colors[4];
    uint32_t MaxFiles;
    uint32_t MaxAge;
    uint32_t first;
    uint32_t last;
    uint32_t current;
    uint32_t lastread;
    uint32_t status;
    mbareaflags flags;
    struct fidoaddr aka;

} fb_list_rec;

typedef struct __attribute__((packed)) LastRead
{
    uint32_t UserCRC;     // CRC-32 of user name (lowercase)  (1)
    uint32_t UserID;      // Unique UserID
    uint32_t MsgID;            // CRC MSGID  Unique Msg #
    uint32_t RepID; // CRC REPLY  Reply to Topic,.

} LastRead;

typedef struct __attribute__((packed)) EmailIdx
{
    uint32_t  UserID;      // Unique UserID of Email TO:
    uint16_t  New;         // Read / Unread Message
    uint8_t   Subj[90];    // Message Subject

} EmailIdx;

typedef struct __attribute__((packed)) JamIndex
{
    uint32_t CRC;     // CRC-32 of user name (lowercase)  (1)
    uint32_t OffSet;  // Message Text Offset

} JamIndex;

/*
typedef struct __attribute__((packed)) MsgStats {

    uint32_t Views;    // # of times message was viewed
    uint32_t Replies;  // # of Replies to this message
    uint32_t Date;     // Date Posted.
    uint32_t From[40]; // Posted From
    uint8_t  Subj[90]; // Message Subject

} MsgStats; */


typedef struct __attribute__((packed)) NewScan
{
    uint16_t set;      // TRUE / FALSE if this area is to be scanned.

} NewScan;


typedef struct DataArea
{
    std::string     AreaName;
    std::string     AreaFileName;
    std::string     AnsiFile;
    std::string     AnsiMid;
    uint16_t    NumOfRecs;    // 0 Unlimited / 1 or More to Limit for Lists
    uint16_t    PageBreak;    // 0 Ignore, else break on this man for next page.

} DataArea;


typedef struct DataRec
{
    uint16_t idx;                    // Command #, Used Internal for Input Record #.
    std::string Desc;           // # Description
    std::string Prompt;         // # Text Prompt To Display on Input Commands.
    uint32_t Size;         // # Max Number of Chars on Input
    std::string CKeys;          // # Accepted Input Keys
    std::string CmdType;        // # Command Type
    std::string MString;        // # Data to be Written {UserName}
    std::string MciCode;        // # MCI Code in ANSI For this Field
    std::string HiString;       // # LightBar High String
    std::string LoString;       // # LightBar Low String
    uint16_t Xcoord;                 // # X Coord of Lightbar
    uint16_t Ycoord;                 // # Y Coord of Lightbar
    uint16_t LBarCmd;                // # Is This a Lightbar Command?
    uint16_t MCICmd;                 // # Is This a MCI Code, Skip Input and Translate.

} DataRec;

typedef struct __attribute__((packed)) History
{
    uint32_t Calls;
    uint32_t Emails;
    uint32_t Posts;
    uint32_t Replies;
    uint32_t Edits;
    uint32_t Deletes;
    uint32_t Views;
    uint32_t Local;
    uint32_t Echomail;
    uint32_t Uploads;
    uint32_t UploadKb;
    uint32_t Downloads;
    uint32_t DownloadKb;
    uint32_t LastCallTime;
    uint8_t LastCaller[40];

} History;


typedef struct __attribute__((packed)) MsgStats
{
    uint32_t Unread;
    uint32_t Posts;
    uint32_t Replies;
    uint32_t Views;
    uint32_t dtLastPostTime;
    uint8_t  LastPoster[40];
    uint8_t  PostSubject[40];

} MsgStats;

typedef struct __attribute__((packed)) FileIdx
{
    uint32_t idx;   // Extended Description Index
    uint32_t size;  // File Size
    uint32_t dls;   // # of Downloads
    uint32_t date;  // When it was uploaded
    uint8_t uploader [40];  // Uploader
    uint8_t filename [255]; // File Name w/ out path

} FileIdx;


/***************************************************
 * OLD FOR Form.dat Message Conversions
 *
 */

// Message Base Structes
typedef struct __attribute__((packed)) ACSrecL
{
    unsigned long onn,off;

} ACSrecL;

typedef struct __attribute__((packed)) ACSrecS
{
    unsigned short onn,off;

} ACSrecS;

typedef struct __attribute__((packed)) ACSrec
{
    ACSrecL mgroup,
    fgroup;
    ACSrecS eflags,
    fflags;
    unsigned long
    since_mn;
    unsigned int
    dotm,
    dotw,
    min_logons,
    min_nodes,
    tI01,
    tI02,
    tI03;
    unsigned char
    min_sl,
    gender,
    min_age,
    tC01,
    tC02,
    tC03,
    tC04;

} ACSrec;

typedef struct __attribute__((packed)) fidoaddr_OLD
{
    unsigned
    short  zone,
    net,
    node,
    point;

    unsigned
    char   domain[13];

} fidoaddr_OLD;

typedef struct __attribute__((packed)) mbareaflags_OLD
{
    char mbrealname;
    char mbvisible;       // Sysop Only!!
    char mbansi;
    char mb8bitx;
    char mbstrip;
    char mbaddtear;
    char mbnopubstat;
    char mbnocreadit;
    char mbinternet;
    char mbfileattach;
    char mbstripcolor;

} mbareaflags_OLD;

typedef struct __attribute__((packed)) mb_list_rec_OLD
{
    int idx;
    int Type;
    int Kind;
    int Active;
    int Pubpriv;
    int Allowalias;
    int scantype;
    int fidoaddr;
    int sigtype;
    int reserved_int[15];
    char mbdisplay[61];
    char mbfile[36];
    char mbpath[81];
    char password[16];
    ACSrec ReadACS;
    ACSrec WriteACS;
    ACSrec SysopACS;
    ACSrec NetworkACS;
    char OriginLine[66];
    unsigned char colors[4];
    char infofile[36];
    char reserved_char[149];
    unsigned long MaxMsgs;
    unsigned long MaxAge;
    unsigned long first;
    unsigned long last;
    unsigned long current;
    unsigned long lastread;
    unsigned long status;
    long reserved_long[3];
    mbareaflags_OLD flags;
    struct fidoaddr_OLD aka;

} mb_list_rec_OLD;

# endif

