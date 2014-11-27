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

    - Fixed broken JAM_GetSubfield()
    - #includes stdlib.h instead of malloc.h and memory.h

    Changes made by Johan Billing 2000-09-17:

    - Added JAM_GetSubfield_R()

    Backported changes from JAMLIB 1.4.7 made by Johan Billing 2003-10-22

    - JAM_NewSubPacket() and JAM_PutSubField() would give memory leaks under
      low memory conditions. Fixed.

    Other changes made by Johan Billing 2003-10-22

    - Fixed comparison between signed and unsigned variable in
      JAM_DelSubPacket() and JAM_GetSubField()
*/

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

/***********************************************************************
**
**  SUBPACKET.C -- Subfield packet handling
**
**  Author: Bjrn Stenberg (bjorn.stenberg@sth.frontec.se)
**
***********************************************************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "jam.h"

/***********************************************************************
**
**  JAM_NewSubPacket - Create a new subfield packet
**
***********************************************************************/
s_JamSubPacket* JAM_NewSubPacket ( void )
{
    s_JamSubPacket* Sub_PS;

    /* allocate packet struct */
    Sub_PS = ( s_JamSubPacket* ) malloc ( sizeof ( s_JamSubPacket ) );

    if ( !Sub_PS )
        return NULL;

    Sub_PS->NumAlloc  = 20;
    Sub_PS->NumFields = 0;

    /* allocate pointer array */
    Sub_PS->Fields    = ( s_JamSubfield** ) calloc ( Sub_PS->NumAlloc,
                        sizeof ( s_JamSubfield* ) );

    if ( !Sub_PS->Fields )
    {
        free ( Sub_PS );
        return NULL;
    }

    return Sub_PS;
}

/***********************************************************************
**
**  JAM_DelSubPacket - Free the data associated with a subfield packet
**
***********************************************************************/
int JAM_DelSubPacket ( s_JamSubPacket* SubPack_PS )
{
    uint32_t i;

    if ( !SubPack_PS )
        return JAM_BAD_PARAM;

    for ( i=0; i < SubPack_PS->NumFields; i++ )
    {
        s_JamSubfield* Field_PS = SubPack_PS->Fields[i];

        if ( Field_PS->Buffer )
            free ( Field_PS->Buffer );

        free ( Field_PS );
    }

    free ( SubPack_PS->Fields );
    free ( SubPack_PS );

    return 0;
}

/***********************************************************************
**
**  JAM_GetSubfield -- Get first/next subfield from a subfield packet
**                     (not reentrant)
**
***********************************************************************/
s_JamSubfield* JAM_GetSubfield ( s_JamSubPacket* SubPack_PS )
{
    static s_JamSubPacket* LastPack_PS = NULL;
    static uint32_t           NextIndex_I = 0;

    if ( SubPack_PS )
    {
        LastPack_PS = SubPack_PS;
        NextIndex_I = 0;
    }

    if ( NextIndex_I < LastPack_PS->NumFields )
        return LastPack_PS->Fields[ NextIndex_I++ ];

    return NULL;
}

/***********************************************************************
**
**  JAM_GetSubfield_R -- Get first/next subfield from a subfield packet
**                       (reentrant)
**
***********************************************************************/
s_JamSubfield* JAM_GetSubfield_R ( s_JamSubPacket* SubPack_PS , uint32_t* Count_PI )
{
    if ( *Count_PI < SubPack_PS->NumFields )
        return SubPack_PS->Fields[ ( *Count_PI ) ++ ];

    return NULL;
}

/***********************************************************************
**
**  JAM_PutSubfield -- Add a subfield to a subfield packet
**
***********************************************************************/
int JAM_PutSubfield ( s_JamSubPacket* SubPack_PS, s_JamSubfield* Field_PS )
{
    s_JamSubfield*     NewField_PS;
    uint8_t* NewBuf_PC;

    /* do we have to expand the array? */
    if ( SubPack_PS->NumFields == SubPack_PS->NumAlloc )
    {
        s_JamSubfield** Fields_PPS;

        SubPack_PS->NumAlloc *= 2;
        Fields_PPS = ( s_JamSubfield** ) realloc ( SubPack_PS->Fields,
                     SubPack_PS->NumAlloc *
                     sizeof ( s_JamSubfield* ) );

        if ( !Fields_PPS )
            return JAM_NO_MEMORY;

        SubPack_PS->Fields=Fields_PPS;
    }

    /*
    **  Copy the passed subfield
    */

    /* allocate a new subfield */
    NewField_PS = ( s_JamSubfield* ) malloc ( sizeof ( s_JamSubfield ) );

    if ( !NewField_PS )
        return JAM_NO_MEMORY;

    /* allocate a new buffer */
    if ( Field_PS->DatLen )
    {
        NewBuf_PC = ( uint8_t* ) malloc ( Field_PS->DatLen );

        if ( !NewBuf_PC )
        {
            free ( NewField_PS );
            return JAM_NO_MEMORY;
        }

        memcpy ( NewBuf_PC, Field_PS->Buffer, Field_PS->DatLen );
    }
    else
        NewBuf_PC = NULL;

    /* copy field struct */
    NewField_PS->LoID   = Field_PS->LoID;
    NewField_PS->HiID   = Field_PS->HiID;
    NewField_PS->DatLen = Field_PS->DatLen;
    NewField_PS->Buffer = NewBuf_PC;


    /*
    **  Update subfield packet
    */

    SubPack_PS->Fields[ SubPack_PS->NumFields ] = NewField_PS;
    SubPack_PS->NumFields++;

    return 0;
}
