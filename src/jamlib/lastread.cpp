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

    - Changed source to use feof() instead of errno == EPASTEOF
    - Changed source to use structrw to read and write structures
*/

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

/***********************************************************************
**
**  LASTREAD.C -- Lastread pointer handling
**
**  Author: Bjrn Stenberg (bjorn.stenberg@sth.frontec.se)
**
***********************************************************************/
#include <stdio.h>
#include <errno.h>

#include "jam.h"
#include "structrw.h"

/***********************************************************************
**
**  File global variables
**
***********************************************************************/

/***********************************************************************
**
**  JAM_ReadLastRead - Read LastRead record
**
***********************************************************************/
int JAM_ReadLastRead ( s_JamBase* Base_PS,
                       uint32_t User_I,
                       s_JamLastRead* Record_PS )
{
    s_JamLastRead Record_S;
    int Pos_I;

    if ( !Record_PS )
        return JAM_BAD_PARAM;

    if ( fseek ( Base_PS->LrdFile_PS, 0, SEEK_SET ) )
    {
        Base_PS->Errno_I = errno;
        return JAM_IO_ERROR;
    }

    for ( Pos_I = 0; ; Pos_I++ )
    {

        if ( 1 > freadjamlastread ( Base_PS->LrdFile_PS,&Record_S ) )
        {
            if ( feof ( Base_PS->LrdFile_PS ) )
                return JAM_NO_USER;

            Base_PS->Errno_I = errno;
            return JAM_IO_ERROR;
        }

        if ( Record_S.UserID == User_I )
        {
            Base_PS->LastUserPos_I  = Pos_I;
            Base_PS->LastUserId_I = User_I;
            *Record_PS = Record_S;
            return 0;
        }
    }

    return 0;
}

/***********************************************************************
**
**  JAM_WriteLastRead - Write LastRead record
**
***********************************************************************/
int JAM_WriteLastRead ( s_JamBase* Base_PS,
                        uint32_t User_I,
                        s_JamLastRead* Record_PS )
{
    s_JamLastRead Record_S;
    int Pos_I;

    if ( !Record_PS )
        return JAM_BAD_PARAM;

    /* if the last read is stored */
    if ( User_I == Base_PS->LastUserId_I )
    {
        Pos_I = Base_PS->LastUserPos_I * sizeof ( s_JamLastRead );

        if ( fseek ( Base_PS->LrdFile_PS, Pos_I, SEEK_SET ) )
        {
            Base_PS->Errno_I = errno;
            return JAM_IO_ERROR;
        }

        /* be safe, check it */
        if ( 1 > freadjamlastread ( Base_PS->LrdFile_PS,&Record_S ) )
        {
            Base_PS->Errno_I = errno;
            return JAM_IO_ERROR;
        }

        /* is it where we expected it to be? */
        if ( User_I == Record_S.UserID )
        {

            if ( fseek ( Base_PS->LrdFile_PS, Pos_I, SEEK_SET ) )
            {
                Base_PS->Errno_I = errno;
                return JAM_IO_ERROR;
            }

            if ( 1 > fwritejamlastread ( Base_PS->LrdFile_PS,Record_PS ) )
            {
                Base_PS->Errno_I = errno;
                return JAM_IO_ERROR;
            }

            fflush ( Base_PS -> LrdFile_PS );

            return 0;
        }
    }

    /* no last position, or position incorrect */
    if ( fseek ( Base_PS->LrdFile_PS, 0, SEEK_SET ) )
    {
        Base_PS->Errno_I = errno;
        return JAM_IO_ERROR;
    }

    for ( Pos_I = 0; ; Pos_I++ )
    {

        if ( 1 > freadjamlastread ( Base_PS->LrdFile_PS,&Record_S ) )
        {
            if ( feof ( Base_PS->LrdFile_PS ) )
            {

                /* user not in file, append a new record  */
                if ( fseek ( Base_PS->LrdFile_PS, 0, SEEK_END ) )
                {
                    Base_PS->Errno_I = errno;
                    return JAM_IO_ERROR;
                }

                break;
            }

            Base_PS->Errno_I = errno;
            return JAM_IO_ERROR;
        }

        /* found the user? */
        if ( Record_S.UserID == User_I )
        {
            if ( fseek ( Base_PS->LrdFile_PS, Pos_I * sizeof ( s_JamLastRead ),
                         SEEK_SET ) )
            {
                Base_PS->Errno_I = errno;
                return JAM_IO_ERROR;
            }

            break;
        }
    }

    if ( 1 > fwritejamlastread ( Base_PS->LrdFile_PS,Record_PS ) )
    {
        Base_PS->Errno_I = errno;
        return JAM_IO_ERROR;
    }

    fflush ( Base_PS->LrdFile_PS );

    return 0;
}
