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

    Changes made by Johan Billing 2003-10-22

    - Added #include <string.h>
*/

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <stdio.h>
#include <string.h>

#include "jam.h"
#include "structrw.h"

uint16_t jamgetuword(uint8_t *buf,uint32_t offset)
{
    return (uint16_t) buf[offset]+
           buf[offset+1]*256;
}

void jamputuword(uint8_t *buf,uint32_t offset,uint16_t num)
{
    buf[offset]=num%256;
    buf[offset+1]=num/256;
}

void jamputuint32_t(uint8_t *buf,uint32_t offset,uint32_t num)
{
    buf[offset]=num%256;
    buf[offset+1]=(num / 256) % 256;
    buf[offset+2]=(num / 256 / 256) % 256;
    buf[offset+3]=(num / 256 / 256 / 256) % 256;
}

uint32_t jamgetuint32_t(uint8_t *buf,uint32_t offset)
{
    return (uint32_t) buf[offset]+
           buf[offset+1]*256+
           buf[offset+2]*256*256+
           buf[offset+3]*256*256*256;
}

int freadjambaseheader(FILE *fp,s_JamBaseHeader *s_JamBaseHeader)
{
    uint8_t buf[SIZE_JAMBASEHEADER];

    if(fread(buf,SIZE_JAMBASEHEADER,1,fp) != 1)
        return 0;

    memcpy(s_JamBaseHeader->Signature,&buf[JAMBASEHEADER_SIGNATURE],4);

    s_JamBaseHeader->DateCreated = jamgetuint32_t(buf,JAMBASEHEADER_DATECREATED);
    s_JamBaseHeader->ModCounter  = jamgetuint32_t(buf,JAMBASEHEADER_MODCOUNTER);
    s_JamBaseHeader->ActiveMsgs  = jamgetuint32_t(buf,JAMBASEHEADER_ACTIVEMSGS);
    s_JamBaseHeader->PasswordCRC = jamgetuint32_t(buf,JAMBASEHEADER_PASSWORDCRC);
    s_JamBaseHeader->BaseMsgNum  = jamgetuint32_t(buf,JAMBASEHEADER_BASEMSGNUM);

    memcpy(s_JamBaseHeader->RSRVD,&buf[JAMBASEHEADER_RSRVD],1000);

    return 1;
}

int fwritejambaseheader(FILE *fp,s_JamBaseHeader *s_JamBaseHeader)
{
    uint8_t buf[SIZE_JAMBASEHEADER];

    memcpy(&buf[JAMBASEHEADER_SIGNATURE],s_JamBaseHeader->Signature,4);

    jamputuint32_t(buf,JAMBASEHEADER_DATECREATED, s_JamBaseHeader->DateCreated);
    jamputuint32_t(buf,JAMBASEHEADER_MODCOUNTER,  s_JamBaseHeader->ModCounter);
    jamputuint32_t(buf,JAMBASEHEADER_ACTIVEMSGS,  s_JamBaseHeader->ActiveMsgs);
    jamputuint32_t(buf,JAMBASEHEADER_PASSWORDCRC, s_JamBaseHeader->PasswordCRC );
    jamputuint32_t(buf,JAMBASEHEADER_BASEMSGNUM,  s_JamBaseHeader->BaseMsgNum);

    memcpy(&buf[JAMBASEHEADER_RSRVD],s_JamBaseHeader->RSRVD,1000);

    if(fwrite(buf,SIZE_JAMBASEHEADER,1,fp) != 1)
        return 0;

    return 1;
}

int freadjammsgheader(FILE *fp,s_JamMsgHeader *s_JamMsgHeader)
{
    uint8_t buf[SIZE_JAMMSGHEADER];

    if(fread(buf,SIZE_JAMMSGHEADER,1,fp) != 1)
        return 0;

    memcpy(s_JamMsgHeader->Signature,&buf[JAMMSGHEADER_SIGNATURE],4);

    s_JamMsgHeader->Revision      = jamgetuword(buf,JAMMSGHEADER_REVISION);
    s_JamMsgHeader->ReservedWord  = jamgetuword(buf,JAMMSGHEADER_RESERVEDWORD);
    s_JamMsgHeader->SubfieldLen   = jamgetuint32_t(buf,JAMMSGHEADER_SUBFIELDLEN);
    s_JamMsgHeader->TimesRead     = jamgetuint32_t(buf,JAMMSGHEADER_TIMESREAD);
    s_JamMsgHeader->MsgIdCRC      = jamgetuint32_t(buf,JAMMSGHEADER_MSGIDCRC);
    s_JamMsgHeader->ReplyCRC      = jamgetuint32_t(buf,JAMMSGHEADER_REPLYCRC);
    s_JamMsgHeader->ReplyTo       = jamgetuint32_t(buf,JAMMSGHEADER_REPLYTO);
    s_JamMsgHeader->Reply1st      = jamgetuint32_t(buf,JAMMSGHEADER_REPLY1ST);
    s_JamMsgHeader->ReplyNext     = jamgetuint32_t(buf,JAMMSGHEADER_REPLYNEXT);
    s_JamMsgHeader->DateWritten   = jamgetuint32_t(buf,JAMMSGHEADER_DATEWRITTEN);
    s_JamMsgHeader->DateReceived  = jamgetuint32_t(buf,JAMMSGHEADER_DATERECEIVED);
    s_JamMsgHeader->DateProcessed = jamgetuint32_t(buf,JAMMSGHEADER_DATEPROCESSED);
    s_JamMsgHeader->MsgNum        = jamgetuint32_t(buf,JAMMSGHEADER_MSGNUM);
    s_JamMsgHeader->Attribute     = jamgetuint32_t(buf,JAMMSGHEADER_ATTRIBUTE);
    s_JamMsgHeader->Attribute2    = jamgetuint32_t(buf,JAMMSGHEADER_ATTRIBUTE2);
    s_JamMsgHeader->TxtOffset     = jamgetuint32_t(buf,JAMMSGHEADER_TXTOFFSET);
    s_JamMsgHeader->TxtLen        = jamgetuint32_t(buf,JAMMSGHEADER_TXTLEN);
    s_JamMsgHeader->PasswordCRC   = jamgetuint32_t(buf,JAMMSGHEADER_PASSWORDCRC);
    s_JamMsgHeader->Cost          = jamgetuint32_t(buf,JAMMSGHEADER_COST);

    return 1;
}

int fwritejammsgheader(FILE *fp,s_JamMsgHeader *s_JamMsgHeader)
{
    uint8_t buf[SIZE_JAMMSGHEADER];

    memcpy(&buf[JAMMSGHEADER_SIGNATURE],s_JamMsgHeader->Signature,4);

    jamputuword(buf,JAMMSGHEADER_REVISION,      s_JamMsgHeader->Revision);
    jamputuword(buf,JAMMSGHEADER_RESERVEDWORD,  s_JamMsgHeader->ReservedWord);
    jamputuint32_t(buf,JAMMSGHEADER_SUBFIELDLEN,   s_JamMsgHeader->SubfieldLen);
    jamputuint32_t(buf,JAMMSGHEADER_TIMESREAD,     s_JamMsgHeader->TimesRead);
    jamputuint32_t(buf,JAMMSGHEADER_MSGIDCRC,      s_JamMsgHeader->MsgIdCRC);
    jamputuint32_t(buf,JAMMSGHEADER_REPLYCRC,      s_JamMsgHeader->ReplyCRC  );
    jamputuint32_t(buf,JAMMSGHEADER_REPLYTO,       s_JamMsgHeader->ReplyTo);
    jamputuint32_t(buf,JAMMSGHEADER_REPLY1ST,      s_JamMsgHeader->Reply1st);
    jamputuint32_t(buf,JAMMSGHEADER_REPLYNEXT,     s_JamMsgHeader->ReplyNext);
    jamputuint32_t(buf,JAMMSGHEADER_DATEWRITTEN,   s_JamMsgHeader->DateWritten);
    jamputuint32_t(buf,JAMMSGHEADER_DATERECEIVED,  s_JamMsgHeader->DateReceived );
    jamputuint32_t(buf,JAMMSGHEADER_DATEPROCESSED, s_JamMsgHeader->DateProcessed);
    jamputuint32_t(buf,JAMMSGHEADER_MSGNUM,        s_JamMsgHeader->MsgNum);
    jamputuint32_t(buf,JAMMSGHEADER_ATTRIBUTE,     s_JamMsgHeader->Attribute);
    jamputuint32_t(buf,JAMMSGHEADER_ATTRIBUTE2,    s_JamMsgHeader->Attribute2);
    jamputuint32_t(buf,JAMMSGHEADER_TXTOFFSET,     s_JamMsgHeader->TxtOffset);
    jamputuint32_t(buf,JAMMSGHEADER_TXTLEN,        s_JamMsgHeader->TxtLen);
    jamputuint32_t(buf,JAMMSGHEADER_PASSWORDCRC,   s_JamMsgHeader->PasswordCRC);
    jamputuint32_t(buf,JAMMSGHEADER_COST,          s_JamMsgHeader->Cost);

    if(fwrite(buf,SIZE_JAMMSGHEADER,1,fp) != 1)
        return 0;

    return 1;
}

int freadjamindex(FILE *fp,s_JamIndex *s_JamIndex)
{
    uint8_t buf[SIZE_JAMINDEX];

    if(fread(buf,SIZE_JAMINDEX,1,fp) != 1)
        return 0;

    s_JamIndex->UserCRC   = jamgetuint32_t(buf,JAMINDEX_USERCRC);
    s_JamIndex->HdrOffset = jamgetuint32_t(buf,JAMINDEX_HDROFFSET);

    return 1;
}

int fwritejamindex(FILE *fp,s_JamIndex *s_JamIndex)
{
    uint8_t buf[SIZE_JAMINDEX];

    jamputuint32_t(buf,JAMINDEX_USERCRC,   s_JamIndex->UserCRC);
    jamputuint32_t(buf,JAMINDEX_HDROFFSET, s_JamIndex->HdrOffset);

    if(fwrite(buf,SIZE_JAMINDEX,1,fp) != 1)
        return 0;

    return 1;
}

int freadjamlastread(FILE *fp,s_JamLastRead *s_JamLastRead)
{
    uint8_t buf[SIZE_JAMLASTREAD];

    if(fread(buf,SIZE_JAMLASTREAD,1,fp) != 1)
        return 0;

    s_JamLastRead->UserCRC     = jamgetuint32_t(buf,JAMLASTREAD_USERCRC);
    s_JamLastRead->UserID      = jamgetuint32_t(buf,JAMLASTREAD_USERID);
    s_JamLastRead->LastReadMsg = jamgetuint32_t(buf,JAMLASTREAD_LASTREADMSG);
    s_JamLastRead->HighReadMsg = jamgetuint32_t(buf,JAMLASTREAD_HIGHREADMSG);

    return 1;
}

int fwritejamlastread(FILE *fp,s_JamLastRead *s_JamLastRead)
{
    uint8_t buf[SIZE_JAMLASTREAD];

    jamputuint32_t(buf,JAMLASTREAD_USERCRC,s_JamLastRead->UserCRC);
    jamputuint32_t(buf,JAMLASTREAD_USERID,s_JamLastRead->UserID);
    jamputuint32_t(buf,JAMLASTREAD_LASTREADMSG,s_JamLastRead->LastReadMsg);
    jamputuint32_t(buf,JAMLASTREAD_HIGHREADMSG,s_JamLastRead->HighReadMsg);

    if(fwrite(buf,SIZE_JAMLASTREAD,1,fp) != 1)
        return 0;

    return 1;
}

int fwritejamsavesubfield(FILE *fp,s_JamSaveSubfield *s_JamSaveSubfield)
{
    uint8_t buf[SIZE_JAMLASTREAD];

    jamputuword(buf,JAMSAVESUBFIELD_LOID,   s_JamSaveSubfield->LoID);
    jamputuword(buf,JAMSAVESUBFIELD_HIID,   s_JamSaveSubfield->HiID);
    jamputuint32_t(buf,JAMSAVESUBFIELD_DATLEN, s_JamSaveSubfield->DatLen);

    if(fwrite(buf,SIZE_JAMSAVESUBFIELD,1,fp) != 1)
        return 0;

    return 1;
}

void getjamsubfield(uint8_t *buf,s_JamSubfield *Subfield_S)
{
    Subfield_S->LoID   = jamgetuword(buf,JAMSAVESUBFIELD_LOID);
    Subfield_S->HiID   = jamgetuword(buf,JAMSAVESUBFIELD_HIID);
    Subfield_S->DatLen = jamgetuint32_t(buf,JAMSAVESUBFIELD_DATLEN);

    Subfield_S->Buffer = (uint8_t *) buf + SIZE_JAMSAVESUBFIELD;
}
