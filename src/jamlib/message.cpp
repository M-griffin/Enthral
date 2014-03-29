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
    - Added JAM_AddEmptyMessage()
    - Added error messages JAM_NO_MESSAGE and JAM_CORRUPT_MSG for
      JAM_ReadMsgHeader()
    - #includes stdlib.h instead of malloc.h and memory.h
    - Fixed a bug that caused JAM_AddMessage() to fail when trying to
      add an empty message to the messagebase under Linux.

    Backported changes from JAMLIB 1.4.7 made by Johan Billing 2003-10-26

    - Fixed memory leaks that would occur if JAM_ReadMsgHeader() failed
    - Added JAM_DeleteMessage()

    Other changes made by Johan Billing 2003-10-26

    - Fixed comparison between signed and unsigned variable in JAM_AddMessage()

    - Improved handling of ActiveMsgs counter. JAM_AddMessage() now only
      increases ActiveMsgs if the added message does not have MSG_DELETED set.
      JAM_ChangeMsgHeader() decreases ActiveMsgs if MSG_DELETED is set and the
      message wasn't already deleted. JAM_DeleteMessage() now only decreases
      ActiveMsgs if the message wasn't already deleted.

*/

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

/***********************************************************************
**
**  Message.C -- Message handling
**
**  Author: Bj”rn Stenberg (bjorn.stenberg@sth.frontec.se)
**
***********************************************************************/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "jam.h"
#include "structrw.h"

/***********************************************************************
**
**  JAM_ReadMsgHeader - Read message header
**
***********************************************************************/
int JAM_ReadMsgHeader( s_JamBase* 	Base_PS,
                       uint32_t 		MsgNo_I,
		       s_JamMsgHeader*	Header_PS,
		       s_JamSubPacket** SubfieldPack_PPS )
{
    s_JamIndex Index_S;

    if ( !Base_PS || !Header_PS )
	return JAM_BAD_PARAM;

    /* find index record */
    if ( fseek( Base_PS->IdxFile_PS, MsgNo_I * sizeof( s_JamIndex ), SEEK_SET ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* read index record */
    if ( 1 > freadjamindex(Base_PS->IdxFile_PS,&Index_S) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* message is not there */
    if(Index_S.HdrOffset == 0xffffffff && Index_S.UserCRC == 0xffffffff)
    {
       return JAM_NO_MESSAGE;
    }

    /* find header */
    if ( fseek( Base_PS->HdrFile_PS, Index_S.HdrOffset, SEEK_SET ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* read header */
    if ( 1 > freadjammsgheader(Base_PS->HdrFile_PS,Header_PS) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* are Subfields requested? */
    if ( SubfieldPack_PPS && Header_PS->SubfieldLen ) {
	s_JamSubPacket*		SubPacket_PS;
    s_JamSubfield        Subfield_S;

    char*       Buf_PC;
	char*       Ptr_PC;
	char*       Roof_PC;
    int         BufSize_I = Header_PS->SubfieldLen;

	Buf_PC = (char *) malloc( BufSize_I );
	if ( !Buf_PC )
	    return JAM_NO_MEMORY;

	/* read all subfields */
	if ( 1 > fread( Buf_PC, BufSize_I, 1, Base_PS->HdrFile_PS ) ) {
	    Base_PS->Errno_I = errno;
	    free (Buf_PC);
	    return JAM_IO_ERROR;
	}

	SubPacket_PS = JAM_NewSubPacket();

	if ( !SubPacket_PS ) {
	    free (Buf_PC);
	    return JAM_NO_MEMORY;
	}

	Roof_PC = Buf_PC + BufSize_I;

	/* cut out the subfields */
	for ( Ptr_PC = Buf_PC;
	      Ptr_PC < Roof_PC;
              Ptr_PC += Subfield_S.DatLen + SIZE_JAMSAVESUBFIELD ) {

	    int		  Status_I;

            getjamsubfield((uint8_t *)Ptr_PC, &Subfield_S);

            if((char *)Subfield_S.Buffer + Subfield_S.DatLen > Roof_PC) {
                JAM_DelSubPacket( SubPacket_PS );
	        free (Buf_PC);
                return JAM_CORRUPT_MSG;
            }

            Status_I = JAM_PutSubfield( SubPacket_PS, &Subfield_S );

            if ( Status_I )  {
                JAM_DelSubPacket( SubPacket_PS );
	        free (Buf_PC);
	        return Status_I;
            }
        }

	free( Buf_PC );

	*SubfieldPack_PPS = SubPacket_PS;
    }
    else
	if ( SubfieldPack_PPS )
	    /* fields requested but none found */
	    /* return an empty packet */
	    *SubfieldPack_PPS = JAM_NewSubPacket();

    return 0;
}

/***********************************************************************
**
**  JAM_ReadMsgText - Read message text
**
***********************************************************************/
int JAM_ReadMsgText( s_JamBase* Base_PS,
		     uint32_t 	Offset_I,
		     uint32_t 	Length_I,
		     uint8_t* 	Buffer_PC )
{
    if ( !Base_PS || !Buffer_PC )
	return JAM_BAD_PARAM;

    if ( !Length_I )
	return 0;

    if ( fseek( Base_PS->TxtFile_PS, Offset_I, SEEK_SET ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    if ( 1 > fread( Buffer_PC, Length_I, 1, Base_PS->TxtFile_PS ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    return 0;
}

/***********************************************************************
**
**  JAM_ChangeMsgHeader - Change a message header
**
***********************************************************************/
int JAM_ChangeMsgHeader( s_JamBase* 	 Base_PS,
			 uint32_t 	    	 MsgNo_I,
			 s_JamMsgHeader* Header_PS )
{
    s_JamBaseHeader 	BaseHeader_S;
    s_JamMsgHeader      OldHeader_S;
    s_JamIndex 		Index_S;
    int			Status_I;

    if ( !Base_PS )
	return JAM_BAD_PARAM;

    if ( !Base_PS->Locked_I )
	return JAM_NOT_LOCKED;

    /* read message base header */
    Status_I = JAM_ReadMBHeader( Base_PS, &BaseHeader_S );
    if ( Status_I )
	return Status_I;

    /* find index record */
    if ( fseek( Base_PS->IdxFile_PS, MsgNo_I * sizeof( s_JamIndex ),
	        SEEK_SET ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* read index record */
    if ( 1 > freadjamindex(Base_PS->IdxFile_PS,&Index_S) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* find header */
    if ( fseek( Base_PS->HdrFile_PS, Index_S.HdrOffset, SEEK_SET ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* read old message header */
    if ( 1 > freadjammsgheader( Base_PS->HdrFile_PS, &OldHeader_S ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* find header */
    if ( fseek( Base_PS->HdrFile_PS, Index_S.HdrOffset, SEEK_SET ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* write header */
    if ( 1 > fwritejammsgheader(Base_PS->HdrFile_PS,Header_PS) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    if( ( Header_PS->Attribute & MSG_DELETED ) && !(OldHeader_S.Attribute & MSG_DELETED) ) {
        /* message is deleted now but wasn't before */
        BaseHeader_S.ActiveMsgs--;
    }

    Status_I = JAM_WriteMBHeader( Base_PS, &BaseHeader_S );
    if ( Status_I )
	return Status_I;

    return 0;
}

/***********************************************************************
**
**  JAM_AddMessage - Add a message to a message base
**
***********************************************************************/
int JAM_AddMessage( s_JamBase* 		Base_PS,
		    s_JamMsgHeader*	Header_PS,
		    s_JamSubPacket*	SubPack_PS,
		    uint8_t*		Text_PC,
		    uint32_t		TextLen_I )
{
    s_JamBaseHeader 	BaseHeader_S;
    s_JamIndex 		Index_S;
    long 		Offset_I;
    int			Status_I;
    uint32_t  		TotLen_I;

   if ( !Base_PS )
		return JAM_BAD_PARAM;

   if ( !Base_PS->Locked_I )
		return JAM_NOT_LOCKED;

   /* read message base header */
   Status_I = JAM_ReadMBHeader( Base_PS, &BaseHeader_S );
	if ( Status_I )
		return Status_I;

   /*
   **  Add text if any
   */

   Header_PS->TxtOffset = 0;
   Header_PS->TxtLen    = 0;

   if(Text_PC && TextLen_I!=0)
   {
        /* go to end of text file */
	if ( fseek( Base_PS->TxtFile_PS, 0, SEEK_END ) ) {
	   	    Base_PS->Errno_I = errno;
           return JAM_IO_ERROR;
	}

	/* store text offset (for header) */
   	Offset_I = ftell( Base_PS->TxtFile_PS );
	if ( Offset_I == -1 ) {
	   Base_PS->Errno_I = errno;
	   return JAM_IO_ERROR;
   	}

	Header_PS->TxtOffset = Offset_I;
   	Header_PS->TxtLen    = TextLen_I;

	/* write text */
   	if ( 1 > fwrite( Text_PC, TextLen_I, 1, Base_PS->TxtFile_PS ) ) {
	 	 	 Base_PS->Errno_I = errno;
	   return JAM_IO_ERROR;
    	}
   }

   /*
   **  Add header
   */

   /* go to end of header file */
   if ( fseek( Base_PS->HdrFile_PS, 0, SEEK_END ) ) {
		Base_PS->Errno_I = errno;
		return JAM_IO_ERROR;
   }

   /* calculate the size of all Subfields */
   TotLen_I = 0;
   if ( SubPack_PS ) {
	s_JamSubfield*	Subfield_PS;

	for ( Subfield_PS = JAM_GetSubfield( SubPack_PS ); Subfield_PS;
	      Subfield_PS = JAM_GetSubfield( NULL ) )
	   TotLen_I += sizeof( s_JamSaveSubfield ) + Subfield_PS->DatLen;
   }

   Header_PS->SubfieldLen = TotLen_I;

   /* go to end of index file */
   if ( fseek( Base_PS->IdxFile_PS, 0, SEEK_END ) ) {
	       Base_PS->Errno_I = errno;
        return JAM_IO_ERROR;
   }

   /* find out new message number (for message header) */
   Offset_I = ftell( Base_PS->IdxFile_PS );
   if ( Offset_I == -1 ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
   }

   /* update header */
   Header_PS->MsgNum = Offset_I / sizeof( s_JamIndex ) +
		                 BaseHeader_S.BaseMsgNum;
   memcpy( Header_PS->Signature, HEADERSIGNATURE, 4 );
   Header_PS->Revision = CURRENTREVLEV;

   /* go to end of header file */
   if ( fseek( Base_PS->HdrFile_PS, 0, SEEK_END ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
   }

   /* find out new header offset (for index record) */
   Offset_I = ftell( Base_PS->HdrFile_PS );
   if ( Offset_I == -1 ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
   }
   Index_S.HdrOffset = Offset_I;

   /* write new header */
   if ( 1 > fwritejammsgheader(Base_PS->HdrFile_PS,Header_PS) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
   }

   /* write Subfields */
   if ( SubPack_PS ) {
	s_JamSubfield*	Subfield_PS;
	uint8_t 		User_AC[101];

	/* clear username */
	User_AC[0] = 0;

	for ( Subfield_PS = JAM_GetSubfield( SubPack_PS ); Subfield_PS;
	   Subfield_PS = JAM_GetSubfield( NULL ) ) {

	   /* first, save Subfield header */
           if ( 1 > fwritejamsavesubfield(Base_PS->HdrFile_PS,(s_JamSaveSubfield *)Subfield_PS) ) {
	      Base_PS->Errno_I = errno;
	      return JAM_IO_ERROR;
           }

           /* then, save Subfield data if any*/
	   if(Subfield_PS->DatLen) {
	      if ( 1 > fwrite( Subfield_PS->Buffer, Subfield_PS->DatLen,
	           1, Base_PS->HdrFile_PS ) ) {
        	   Base_PS->Errno_I = errno;
	           return JAM_IO_ERROR;
	      }
	   }

	   /* store username for index file */
	   if ( Subfield_PS->LoID == JAMSFLD_RECVRNAME ) {
	      memcpy( User_AC, Subfield_PS->Buffer, Subfield_PS->DatLen );
	      User_AC[ Subfield_PS->DatLen ] = 0;
	   }
	}

	/* update index record */
	if ( User_AC[0] )
	   Index_S.UserCRC = JAM_Crc32( User_AC, strlen( (char *)User_AC ) );
	else
	   Index_S.UserCRC = JAM_NO_CRC;
   }
   else
 	/* update index record */
 	Index_S.UserCRC = JAM_NO_CRC;

   /*
   **  Add index
   */

   /* write index record */
   if ( 1 > fwritejamindex(Base_PS->IdxFile_PS,&Index_S) ) {
		Base_PS->Errno_I = errno;
		return JAM_IO_ERROR;
   }

   if(!(Header_PS->Attribute & MSG_DELETED))
        BaseHeader_S.ActiveMsgs++; /* Only increase ActiveMsgs if MSG_DELETED not set */

   /* write message base header */

   Status_I = JAM_WriteMBHeader( Base_PS, &BaseHeader_S );
   if ( Status_I )
        return Status_I;

   return 0;
}

/***********************************************************************
**
**  JAM_AddEmptyMessage - Add a empty message entry to a message base
**
***********************************************************************/
int JAM_AddEmptyMessage( s_JamBase* 		Base_PS)
{
   s_JamIndex Index_S;

   if ( !Base_PS )
	return JAM_BAD_PARAM;

   if ( !Base_PS->Locked_I )
	return JAM_NOT_LOCKED;

   /* go to end of index file */
   if ( fseek(  Base_PS->IdxFile_PS, 0, SEEK_END ) ) {
		Base_PS->Errno_I = errno;
		return JAM_IO_ERROR;
   }

   /*
   **  Add index
   */

   Index_S.HdrOffset = 0xffffffff;
   Index_S.UserCRC = 0xffffffff;

   /* write index record */
   if ( 1 > fwritejamindex(Base_PS->IdxFile_PS,&Index_S) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
   }
   return 0;
}

/***********************************************************************
 **
 **  JAM_DeleteMessage - Delete message from messagebase
 **
 ***********************************************************************/
int JAM_DeleteMessage( s_JamBase*	 Base_PS,
		       uint32_t		 MsgNo_I )
{
    s_JamBaseHeader	BaseHeader_S;
    s_JamMsgHeader	Header_S;
    s_JamIndex		Index_S;
    int			Status_I;
    uint32_t               OldAttribute_I;

    if ( !Base_PS )
	return JAM_BAD_PARAM;

    if ( !Base_PS->Locked_I )
	return JAM_NOT_LOCKED;

    /* read message base header */
    Status_I = JAM_ReadMBHeader( Base_PS, &BaseHeader_S );
    if ( Status_I )
	return Status_I;

    /* find index record */
    if ( fseek( Base_PS->IdxFile_PS, MsgNo_I * sizeof( s_JamIndex ), SEEK_SET ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* read index record */
    if ( 1 > freadjamindex( Base_PS->IdxFile_PS, &Index_S ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* message is not there */
    if(Index_S.HdrOffset == 0xffffffff && Index_S.UserCRC == 0xffffffff)
    {
	return JAM_NO_MESSAGE;
    }

    /* find header */
    if ( fseek( Base_PS->HdrFile_PS, Index_S.HdrOffset, SEEK_SET ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* read header */
    if ( 1 > freadjammsgheader( Base_PS->HdrFile_PS, &Header_S ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    OldAttribute_I = Header_S.Attribute;
    Header_S.Attribute |= MSG_DELETED;

    /* find header */
    if ( fseek( Base_PS->HdrFile_PS, Index_S.HdrOffset, SEEK_SET ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* write header */
    if ( 1 > fwritejammsgheader( Base_PS->HdrFile_PS, &Header_S ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    /* find index record */
    if ( fseek( Base_PS->IdxFile_PS, MsgNo_I * sizeof( s_JamIndex ), SEEK_SET ) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    Index_S.HdrOffset = 0xffffffff;
    Index_S.UserCRC = 0xffffffff;

    /* write index record */
    if ( 1 > fwritejamindex(Base_PS->IdxFile_PS,&Index_S) ) {
	Base_PS->Errno_I = errno;
	return JAM_IO_ERROR;
    }

    if(!(OldAttribute_I & MSG_DELETED))
        BaseHeader_S.ActiveMsgs--; /* decrease ActiveMsgs if the message wasn't already deleted */

    /* write message base header */
    Status_I = JAM_WriteMBHeader( Base_PS, &BaseHeader_S );
    if ( Status_I )
	return Status_I;

    return 0;
}

/***********************************************************************
**
**  JAM_Errno - Report the latest C library error code
**
***********************************************************************/
int JAM_Errno( s_JamBase* Base_PS )
{
    return Base_PS->Errno_I;
}

/***********************************************************************
**
**  JAM_ClearMsgHeader - Clear a message header
**
***********************************************************************/
int JAM_ClearMsgHeader( s_JamMsgHeader* Header_PS )
{
    if (!Header_PS)
	return JAM_BAD_PARAM;

    memset( Header_PS, 0, sizeof( s_JamMsgHeader ) );
    memcpy( Header_PS->Signature, HEADERSIGNATURE, 4 );

    Header_PS->Revision    = CURRENTREVLEV;
    Header_PS->MsgIdCRC    = JAM_NO_CRC;
    Header_PS->ReplyCRC	   = JAM_NO_CRC;
    Header_PS->PasswordCRC = JAM_NO_CRC;

    return 0;
}
