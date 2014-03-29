#ifndef __STAMP_H__
#define __STAMP_H__

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$


typedef unsigned bits;

#ifndef SASC
typedef unsigned char byte;
#endif

typedef signed char sbyte;

typedef unsigned short word;
typedef signed short sword;

#ifdef __alpha    /* add other 64 bit systems here */
typedef unsigned int dword;
typedef signed   int sdword;
#else             /* 32 and 16 bit machines */
typedef unsigned long dword;
typedef signed long sdword;
#endif

#if !defined(__UNIX__) && !defined(SASC) && !defined(__MINGW32__)
typedef unsigned short ushort;
#endif

typedef signed short sshort;

#ifndef __UNIX__
typedef unsigned long ulong;
#endif

typedef signed long slong;

/* DOS-style datestamp */

struct _stamp
{
    struct
    {
        bits da:5;
        bits mo:4;
        bits yr:7;
    }
    date;

    struct
    {
        bits ss:5;
        bits mm:6;
        bits hh:5;
    }
    time;
};

struct _dos_st
{
    word date;
    word time;
};

/* Union so we can access stamp as "int" or by individual components */

union stamp_combo
{
    dword ldate;
    struct _stamp msg_st;
    struct _dos_st dos_st;
};

typedef union stamp_combo SCOMBO;

#endif
