/*
    structrw - Platform-independent reading and writing of JAM structs
    Copyright (C) 1999 Johan Billing

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
*/

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#define JAMBASEHEADER_SIGNATURE     0
#define JAMBASEHEADER_DATECREATED   4
#define JAMBASEHEADER_MODCOUNTER    8
#define JAMBASEHEADER_ACTIVEMSGS   12
#define JAMBASEHEADER_PASSWORDCRC  16
#define JAMBASEHEADER_BASEMSGNUM   20
#define JAMBASEHEADER_RSRVD        24

#define SIZE_JAMBASEHEADER       1024

#define JAMMSGHEADER_SIGNATURE      0
#define JAMMSGHEADER_REVISION       4
#define JAMMSGHEADER_RESERVEDWORD   6
#define JAMMSGHEADER_SUBFIELDLEN    8
#define JAMMSGHEADER_TIMESREAD     12
#define JAMMSGHEADER_MSGIDCRC      16
#define JAMMSGHEADER_REPLYCRC      20
#define JAMMSGHEADER_REPLYTO       24
#define JAMMSGHEADER_REPLY1ST      28
#define JAMMSGHEADER_REPLYNEXT     32
#define JAMMSGHEADER_DATEWRITTEN   36
#define JAMMSGHEADER_DATERECEIVED  40
#define JAMMSGHEADER_DATEPROCESSED 44
#define JAMMSGHEADER_MSGNUM        48
#define JAMMSGHEADER_ATTRIBUTE     52
#define JAMMSGHEADER_ATTRIBUTE2    56
#define JAMMSGHEADER_TXTOFFSET     60
#define JAMMSGHEADER_TXTLEN        64
#define JAMMSGHEADER_PASSWORDCRC   68
#define JAMMSGHEADER_COST          72

#define SIZE_JAMMSGHEADER 76

#define JAMINDEX_USERCRC      0
#define JAMINDEX_HDROFFSET    4

#define SIZE_JAMINDEX         8

#define JAMLASTREAD_USERCRC       0
#define JAMLASTREAD_USERID        4
#define JAMLASTREAD_LASTREADMSG   8
#define JAMLASTREAD_HIGHREADMSG  12

#define SIZE_JAMLASTREAD         16

#define JAMSAVESUBFIELD_LOID     0
#define JAMSAVESUBFIELD_HIID     2
#define JAMSAVESUBFIELD_DATLEN   4

#define SIZE_JAMSAVESUBFIELD     8

int freadjambaseheader(FILE *fp,s_JamBaseHeader *s_JamBaseHeader);
int fwritejambaseheader(FILE *fp,s_JamBaseHeader *s_JamBaseHeader);

int freadjammsgheader(FILE *fp,s_JamMsgHeader *s_JamMsgHeader);
int fwritejammsgheader(FILE *fp,s_JamMsgHeader *s_JamMsgHeader);

int freadjamindex(FILE *fp,s_JamIndex *s_JamIndex);
int fwritejamindex(FILE *fp,s_JamIndex *s_JamIndex);

int freadjamlastread(FILE *fp,s_JamLastRead *s_JamLastRead);
int fwritejamlastread(FILE *fp,s_JamLastRead *s_JamLastRead);

int fwritejamsavesubfield(FILE *fp,s_JamSaveSubfield *s_JamSaveSubfield);
void getjamsubfield(uint8_t *buf,s_JamSubfield *Subfield_S);


