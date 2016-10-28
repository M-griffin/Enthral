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

// Enthral SVN: $Id: msg_edit.cpp 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/msg_edit.cpp $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# include "struct.h"

# include "mb_api.h"
# include "msg_edit.h"
# include "msgs.h"

# include <cstdio>
# include <ctype.h>
# include <cstring>
# include <cstdlib>

# include <string>

typedef enum {LOCAL, NETMAIL, ECHOMAIL, EMAIL, NEWS} MSGTYPE;
typedef enum {PUBLIC, PRIVATE} POSTTYPE;

char mbtypes[5][21]= {"Local","Netmail","Echomail","Email","NewsGroup"};
char posttyp[2][11]= {"Public","Private"};

# define MSG_QUOTE	0
# define MSG_TEXT	1
# define MSG_TEAR	2
# define MSG_ORIGIN	3



/**
 * Message Area Editor - Inital New Message Area
 */
void msg_edit::init_mbase(mb_list_rec *mb)
{

    memset(mb,0,sizeof(mb_list_rec));
    sprintf((char *)mb->mbdisplay,"%s",(char *)"New Message Base");  // Description
    sprintf((char *)mb->mbpath,"%s",(char *)MESGPATH);               // Path to Message Files
    sprintf((char *)mb->mbfile,"%s",(char *)"newbase");	           // Filename
//    mb->Type       = MSGTYPE_JAM;              // Default to JAM
    mb->Kind       = 0;                        // Local
    mb->Pubpriv    = 0;                        // Public

    /*
    copy_ACS(&mb->SysopACS,   &config.DefMsg.SysopACS);
    copy_ACS(&mb->ReadACS,    &config.DefMsg.ReadACS);
    copy_ACS(&mb->WriteACS,   &config.DefMsg.WriteACS);
    copy_ACS(&mb->NetworkACS, &config.DefMsg.NetworkACS);

    strcpy(mb->OriginLine,config.DefMsg.OriginLine);

    mb->colors[MSG_QUOTE]  =config.DefMsg.colors[MSG_QUOTE];
    mb->colors[MSG_TEXT]   =config.DefMsg.colors[MSG_TEXT];
    mb->colors[MSG_TEAR]   =config.DefMsg.colors[MSG_TEAR];
    mb->colors[MSG_ORIGIN] =config.DefMsg.colors[MSG_ORIGIN];

    mb->Kind       = config.DefMsg.Kind;
    mb->Type       = config.DefMsg.Type;
    mb->Pubpriv    = config.DefMsg.Pubpriv;
    mb->Allowalias = config.DefMsg.Allowalias;
    mb->scantype   = config.DefMsg.scantype;
    mb->fidoaddr   = config.DefMsg.fidoaddr;
    mb->MaxMsgs    = config.DefMsg.MaxMsgs;
    mb->MaxAge     = config.DefMsg.MaxAge;
    */

    // Setup Bit Flags for Toggable options.  WIP
    BitClr(mb->flags.mbrealname,0);                   // 0x01 == off, 0x02 == on
    BitClr(mb->flags.mbvisible,0);                    // 0x01
    BitClr(mb->flags.mbansi,0);                       // 0x01
    BitClr(mb->flags.mb8bitx,0);                      // 0x01
    BitClr(mb->flags.mbstrip,0);                      // 0x01
    BitClr(mb->flags.mbaddtear,0);                    // 0x01
    BitClr(mb->flags.mbnopubstat,0);                  // 0x01
    BitClr(mb->flags.mbnocreadit,0);                  // 0x01
    BitClr(mb->flags.mbinternet,0);                   // 0x01
    BitClr(mb->flags.mbfileattach,0);                 // 0x01
    BitClr(mb->flags.mbstripcolor,0);                 // 0x01

}

/**
 * Message Area Editor - Remove New Message Area from DAT
 */
void msg_edit::kill_mbase(int mbnum)
{

    mb_list_rec mbin;
    int i1=0,count=0;
    char ppath[1024]= {0};
    char ppath2[1024]= {0};

    while(_mfunc.read_mbaselist(&mbin,i1))
    {
        if(i1 != mbnum)
        {
            mbin.idx = count;
            count++;
            if(!_mfunc.save_mbasetemp(&mbin, mbin.idx))
            {
                perror("Error unable to save temp msgarea, check permissions!");
            }
        }
        i1++;
    }

    // Remove Normal file
    sprintf(ppath,"%smsgforums.dat",DATAPATH);
    remove(ppath);
    // Rename Tmp to Original
    sprintf(ppath2,"%smsgforums.tmp",DATAPATH);
    rename(ppath2, ppath);
    // Remove Tmp
    remove(ppath2);
}

/**
 * Message Area Editor - Delete New Message Area
 */
void msg_edit::delete_mbase()
{

    char ttxt[21]= {0};
    int num, total_mbases;
    char sLine[200]= {0};

    total_mbases = _mfunc.msg_count();
    if (total_mbases < 1)
    {
        return;
    }
    int len = 4;
    sprintf(sLine,"|CR |09Delete which? |07[|151|07-|15%d|07]|08: ", total_mbases);
    inputfield(sLine,len);
    pipe2ansi(sLine);
    getline(ttxt,sizeof(ttxt));
    if (strcmp(ttxt," ") == 0) return;
    num = atoi(ttxt);
    if(num > 0 && num <= total_mbases)
    {
        kill_mbase(num-1);
    }
}

/**
 * Message Area Editor - Insert Message Arae at
 */
void msg_edit::poke_mbase(int pokenum)
{

    mb_list_rec mbin;
    int total_mbases;

    total_mbases = _mfunc.msg_count();
    memset(&mbin,0,sizeof(mb_list_rec));

    //printf ("\n pokenum %i, total_mbases %i",   pokenum,total_mbases);

    //Append At End of File
    if (pokenum > total_mbases)
    {
        init_mbase(&mbin);
        _mfunc.save_mbaselist(&mbin,pokenum);
        return;

    }
    // Insert Before a comamnd
    else if(total_mbases < 1)
    {
        _mfunc.read_mbaselist(&mbin,0);
        _mfunc.save_mbaselist(&mbin,1);
        init_mbase(&mbin);
        _mfunc.save_mbaselist(&mbin,0);
        return;
    }
    else
    {
        for(int i1 = total_mbases; i1 > pokenum; i1--)
        {
            _mfunc.read_mbaselist(&mbin,i1-1);
            _mfunc.save_mbaselist(&mbin,i1);
        }
        memset(&mbin,0,sizeof(mb_list_rec));
    }
    init_mbase(&mbin);
    _mfunc.save_mbaselist(&mbin,pokenum);
}

/**
 * Message Area Editor - Insert a New Message Area
 */
void msg_edit::insert_mbase()
{

    char sLine[200];
    mb_list_rec mbin;
    memset(&mbin,0,sizeof(mb_list_rec));
    char ttxt[21]= {0};
    int num, total_mbases;
    int count;

    total_mbases = _mfunc.msg_count();
    count = total_mbases;

    if (count == -1)
        count = 0;

    int len = 4;
    sprintf(sLine,"|CR |09Insert before which? |07[|151|07-|15%d|07]|08: ", count+1);
    inputfield(sLine,len);
    pipe2ansi(sLine);

    getline(ttxt,sizeof(ttxt));
    if (strcmp(ttxt," ") == 0) return;
    num = atoi(ttxt);

    if (num > 0 && num <= count+1)
    {

        if(total_mbases < 1)
        {
            init_mbase(&mbin);
            _mfunc.save_mbaselist(&mbin, 0);
        }
        else
        {
            poke_mbase(num-1);
        }
    }
}


/**
 * Message Area Editor - RePosition Areas
 */
void msg_edit::swap_mbase(int iFrom, int iTo)
{

    mb_list_rec mbfrom, mbin;

    int total_mbases;//, count;

    // First Read in the Base here!
    memset(&mbfrom,0,sizeof(mb_list_rec));
    _mfunc.read_mbaselist(&mbfrom, iFrom);

    // Next Purge the the Original..
    kill_mbase(iFrom);

    total_mbases = _mfunc.msg_count();

    //Append At End of File
    if (iTo >= total_mbases)
    {
        _mfunc.save_mbaselist(&mbfrom,iTo);
        return;

    }
    // Insert Before a comamnd
    else if(total_mbases < 1)
    {
        _mfunc.read_mbaselist(&mbin,0);
        _mfunc.save_mbaselist(&mbin,1);
        _mfunc.save_mbaselist(&mbfrom,0);
        return;
    }
    else
    {
        for(int i1 = total_mbases; i1 > iTo; i1--)
        {
            _mfunc.read_mbaselist(&mbin,i1-1);
            _mfunc.save_mbaselist(&mbin,i1);
        }
    }
    _mfunc.save_mbaselist(&mbfrom,iTo);
    return;
}


/**
 * Message Area Editor - Move Area
 */
void msg_edit::move_mbase()
{

    char ttxt[21]= {0};
    char sLine[200]= {0};
    int num = 0, num1 = 0, total_mbases = 0;

    total_mbases = _mfunc.msg_count();
    if (total_mbases < 1)
    {
        return;
    }

    int len = 4;
    sprintf(sLine,"|CR |09Move which? |07[|151|07-|15%d|07]|08: ", total_mbases);
    inputfield(sLine,len);
    pipe2ansi(sLine);
    getline(ttxt,sizeof(ttxt));
    if (strcmp(ttxt," ") == 0) return;
    num = atoi(ttxt);

    if(num > 0 && num <= total_mbases+1)
    {

        sprintf(sLine,"|CR |09Position Before? |07[|151|07-|15%d|07]|08: ", total_mbases+1);
        len = 4;
        inputfield(sLine,len);
        pipe2ansi(sLine);
        getline(ttxt,sizeof(ttxt));
        if (strcmp(ttxt," ") == 0) return;
        num1 = atoi(ttxt);

        if(num1 > 0 && num1 <= total_mbases+1)
            swap_mbase(num-1, num1-1);
    }
}

/**
 * Message Area Editor - Edit Area
 */
void msg_edit::modify_mbase()
{

    char ttxt[21]= {0};
    char sLine[200]= {0};
    int num;
    int total_mbases;

    total_mbases = _mfunc.msg_count();
    if (total_mbases < 1)
    {
        return;
    }

    int len = 4;
    sprintf(sLine,"|CR |09Edit which? |07[|151|07-|15%d|07]|08: ", total_mbases);
    inputfield(sLine,len);
    pipe2ansi(sLine);
    getline(ttxt,sizeof(ttxt));
    if (strcmp(ttxt," ") == 0) return;

    num = atoi(ttxt);

    if(num > 0 && num <= total_mbases)
    {
        mod_mbase(num);
    }
}

/**
 * Message Area Editor - Edit Fido
 */
void msg_edit::edit_aka(mb_list_rec *mr)
{

    char sLine[200]= {0};
    char ttxt[16]= {0};
    char *p;

    int len = 16;
    sprintf(sLine,"|CR |09Enter Fido Address|08: |07[|15xx|07:|15xx|07/|15xx]: ");
    inputfield(sLine,len);
    pipe2ansi(sLine);
    getline(ttxt,sizeof(ttxt));
    if (strcmp(ttxt," ") == 0) return;
    //strcpy(mbl.mbdisplay,ttxt);

    //get_aka(ttxt);  Maybe Impliment Later

    p=strtok(ttxt,":/.");
    if(p)	mr->aka.zone=atoi(p);
    p=strtok(NULL,":/.");
    if(p)	mr->aka.net=atoi(p);
    p=strtok(NULL,":/.");
    if(p)	mr->aka.node=atoi(p);
    p=strtok(NULL,":/.");
    if(p)	mr->aka.point=atoi(p);

    //mci("~SM`09Domain`0F: `1F");
    //ttxt[0]='\0';
    //get_str(config.aka[num].domain,21);
}

/*
void msg_edit::ACS(ACSrec *acs, char *fstr) {

    class acs_levels *level = new acs_levels;
    level->ACS_setup(acs, fstr);
    delete level;
}*/


/**
 * Message Area Editor - Update Area
 */
void msg_edit::mod_mbase(int mbnum)
{

    mb_list_rec mbl;
    char faka[41]= {0};
    //char sigtxt[3][21]={"None","Short Sig.","Long Sig."};
    std::string TF;
    char rBuffer[200]= {0};

    memset(&mbl,0,sizeof(mb_list_rec));
    _mfunc.read_mbaselist(&mbl, mbnum-1);
    if(mbl.sigtype < 0 || mbl.sigtype > 2) mbl.sigtype = 0;

    int total_mbases = _mfunc.msg_count();
    unsigned char ch;
    char sLine[200];
    std::string output;
    while(1)
    {
        output.erase();
        faddr2char(faka,&mbl.aka);
        memset(&rBuffer,0,sizeof(rBuffer));
        output += "|CS|11 Message Forum Editor                                      ";
        output += "|CR|15--------------------------------------------------------------";
        sprintf(sLine,"|CR |07Current Forum|08: |11%d |07of |03%d", mbnum,total_mbases);
        output += sLine;
        sprintf(sLine,"|CR|CR |15A|07. Description         |11%s|16", (char *)mbl.mbdisplay);
        output += sLine;
        sprintf(sLine,"|CR |15B|07. Filename            |11%s|16", (char *)mbl.mbfile);
        output += sLine;

		//if (mbl.Type == MSGTYPE_JAM) TF = "JAM";
        //else TF = "SQUISH";

		TF = "JAM";

        sprintf(sLine,"|CR |15C|07. Message Type        |11%s|16", TF.c_str());
        output += sLine;
        sprintf(sLine,"|CR |15D|07. Forum Kind          |11%s|16", mbtypes[mbl.Kind]);
        output += sLine;
        sprintf(sLine,"|CR |15E|07. Posting Type        |11%s|16", posttyp[mbl.Pubpriv]);
        output += sLine;;
        //sprintf(sLine,"|CR |15F|07. [Unused]            |15|19%s|16", mbl.infofile);

        if ((mbl.flags.mbvisible & 0x01) == 0)
            TF = "no";
        else
            TF = "yes";

        sprintf(sLine,"|CR |15F|07. Sysop Area          |11%s|16", TF.c_str());

        output += sLine;
        //if (mbl.Allowalias) TF = "yes";
        //else TF = "no";
        TF.erase();

        if ((mbl.flags.mbrealname & 0x01) == 0)
            TF = "no";
        else
            TF = "yes";

        sprintf(sLine,"|CR |15G|07. Force RealNames     |11%s|16", TF.c_str());
        TF.erase();
        output += sLine;
        sprintf(sLine,"|CR |15H|07. |09[|01Unused|09]|CR |15I|07. |09[|01Unused|09]|CR |15J|07. |09[|01Unused|09]|CR |15K|07. |09[|01Unused|09]");
        output += sLine;
        sprintf(sLine,"|CR |15L|07. AKA Address         |11%s|16", faka);
        output += sLine;

        /*
        mci("%sQuoted (%d)`0F, ",
        	ansic(mbl.colors[MSG_QUOTE]),
        	mbl.colors[MSG_QUOTE]);
        mci("%sText (%d)`0F,",
        	ansic(mbl.colors[MSG_TEXT]),
        	mbl.colors[MSG_TEXT]);
        mci("%sTear (%d)`0F,",
        	ansic(mbl.colors[MSG_TEAR]),
        	mbl.colors[MSG_TEAR]);
        mci("%sOrigin (%d)~SM",
        	ansic(mbl.colors[MSG_ORIGIN]),
        	mbl.colors[MSG_ORIGIN]);
        */

        sprintf(sLine,"|CR |15M|07. Origin Line         |11%s|16", mbl.origin_line);
        output += sLine;
        output += "|CR|CR |15q|07.uit |15] +|07. next |15[ -|07. prev: \x1b[1;44m \x1b[D";

        pipe2ansi((char *)output.c_str());
        ch = getkey(true);
        switch(toupper(ch))
        {

        case 'A':
            pipe2ansi((char *)"|CR |09|16Description: \x1b[1;44m                              \x1b[30D");
            getline(rBuffer,sizeof(rBuffer));
            if (strcmp(rBuffer," ") == 0) break;
            strcpy((char *)mbl.mbdisplay, rBuffer);
            break;

        case 'B':
            pipe2ansi((char *)"|CR |09|16New Filename: \x1b[1;44m                              \x1b[30D");
            getline(rBuffer,sizeof(rBuffer));
            if (strcmp(rBuffer," ") == 0) break;
            strcpy((char *)mbl.mbfile, rBuffer);
            break;

        case 'C':
            //if(mbl.Type == MSGTYPE_JAM)	mbl.Type = MSGTYPE_SQUISH;
            //else						mbl.Type = MSGTYPE_JAM;
            break;

        case 'D':
            ++mbl.Kind;
            if(mbl.Kind > 4)
                mbl.Kind = 0;
            switch(mbl.Kind)
            {
            case LOCAL    :
                mbl.Pubpriv = PUBLIC;
                break;
            case ECHOMAIL :
                mbl.Pubpriv = PUBLIC;
                break;
            case NETMAIL  :
                mbl.Pubpriv = PRIVATE;
                break;
            case EMAIL    :
                mbl.Pubpriv = PRIVATE;
                break;
            case NEWS     :
                mbl.Pubpriv = PUBLIC;
                break;
            }
            break;

        case 'E':
            if(mbl.Pubpriv==PUBLIC)
            {
                switch(mbl.Kind)
                {
                case LOCAL	  :
                    mbl.Pubpriv = PRIVATE;
                    break;
                case ECHOMAIL :
                    mbl.Pubpriv = PUBLIC;
                    break;
                case NETMAIL  :
                    mbl.Pubpriv = PRIVATE;
                    break;
                case EMAIL    :
                    mbl.Pubpriv = PRIVATE;
                    break;
                case NEWS	  :
                    mbl.Pubpriv = PUBLIC;
                    break;
                }
            }
            else
            {
                switch(mbl.Kind)
                {
                case LOCAL	  :
                    mbl.Pubpriv = PUBLIC;
                    break;
                case ECHOMAIL :
                    mbl.Pubpriv = PUBLIC;
                    break;
                case NETMAIL  :
                    mbl.Pubpriv = PRIVATE;
                    break;
                case EMAIL    :
                    mbl.Pubpriv = PRIVATE;
                    break;
                case NEWS	  :
                    mbl.Pubpriv = PUBLIC;
                    break;
                }
            }
            break;

        case 'F':
            mbl.flags.mbvisible = (mbl.flags.mbvisible)?0:1;
            break;

        case 'G':
            //mbl.Allowalias = (mbl.Allowalias)?0:1;  break;
            mbl.flags.mbrealname = (mbl.flags.mbrealname)?0:1;
            break;

            /*			case 'H': ACS(&mbl.WriteACS,mbl.mbdisplay); break;
            			case 'I': ACS(&mbl.ReadACS, mbl.mbdisplay); break;
            			case 'J': ACS(&mbl.SysopACS,mbl.mbdisplay); break;*/

        case 'K':
            mbl.sigtype++;
            if(mbl.sigtype<0 || mbl.sigtype>2)
                mbl.sigtype=0;
            break;

        case 'L':
            edit_aka(&mbl);
            //mbl.fidoaddr=pick_aka();
            break;

        case 'M':
            pipe2ansi((char *)"|CR |09|16Origin Line: \x1b[1;44m                                                            \x1b[60D");
            getline(rBuffer,sizeof(rBuffer));
            if (strcmp(rBuffer," ") == 0) break;
            strcpy((char *)mbl.origin_line, rBuffer);
            break;

        case 'Q':
            _mfunc.save_mbaselist(&mbl, mbnum-1);
//            _mfunc.save_msgbase(&mbl);
            return;
        case '+':
        case ']':
            _mfunc.save_mbaselist(&mbl, mbnum-1);
//            _mfunc.save_msgbase(&mbl);

            if (mbnum+1 > total_mbases)
                break;

            ++mbnum;
            memset(&mbl,0,sizeof(mb_list_rec));
            _mfunc.read_mbaselist(&mbl, mbnum-1);
            if(mbl.sigtype < 0 || mbl.sigtype > 2) mbl.sigtype = 0;

            //total_mbases = _mfunc.msg_count();
            break;
        case '-':
        case '[':
            _mfunc.save_mbaselist(&mbl, mbnum-1);
//            _mfunc.save_msgbase(&mbl);

            if (mbnum != 1)
            {
                --mbnum;
                memset(&mbl,0,sizeof(mb_list_rec));
                _mfunc.read_mbaselist(&mbl, mbnum-1);
                if(mbl.sigtype < 0 || mbl.sigtype > 2) mbl.sigtype = 0;
            }
            break;
        }
    }
}

/**
 * Message Area Editor - Fido Address to String
 */
char *msg_edit::faddr2char(char *s,fidoaddr *fa)
{

    if(fa->point > 0)
        sprintf(s,"%u:%u/%u.%u",
                fa->zone,fa->net,fa->node,fa->point);
    else
        sprintf(s,"%u:%u/%u",
                fa->zone,fa->net,fa->node);
    return(s);
}


/*
void msg_edit::compile_mbase()
{

    int total_mbases = _mfunc.msg_count();
    if (total_mbases < 1)
    {
        pipe2ansi((char *)"|CR|CR |08C|07om|15piling Areas|08... |10No Areas Found!");
        return;
    }

    pipe2ansi((char *)"|CR|CR |08C|07om|15piling Areas|08...");

    mb_list_rec mbl;

    int i = _mfunc.msg_count();
    int areaNum = 0;
    if (i<1) return;

    // Purge Foruns2.dat
    //_mfunc.purgeForums2();

    // Recreate Forums2.dat
    i = 0;
    areaNum = 0;
    while(_mfunc.read_mbaselist(&mbl,i))
    {
        // Not SysOp Area
        //  if ((mbl.flags.mbvisible & 0x01) == 0) {  // old forums 2 recompile.
        _mfunc.save_mbaselist(&mbl, areaNum);
        ++areaNum;
        //  }
        ++i;
    }
    pipe2ansi((char *)"|CR|CR |08C|07om|15piling Areas Completed|08! |CR|CR|PA");
}
*/

/**
 * Message Area Editor - List Message Areas (Generic!)
 */
int msg_edit::list_message_bases(int page)
{

    mb_list_rec mbl;
    char disp[61] = {0},
                    tType[12]= {0};
    char outbuff[1024]= {0};
    int rno;
    std::string TF;

    int i = _mfunc.msg_count();
    if (i > 0)
    {
        i = 0;

        sprintf(outbuff,"|15%3s. |09%-30s |03%-10s |11%7.7s |07%7.7s |08%7.7s |15%s|CR","#","Message Area","Filename","Echo","Format","Pub"," SysOp");
        pipe2ansi(outbuff);
        pipe2ansi((char *)"----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----|CR");
        for(int i1=0; i1<14; i1++)
        {

            rno=(page*14);
            if(_mfunc.read_mbaselist(&mbl,rno+i1))
            {
                //  while(_mfunc.read_mbaselist(&mbl,i+rno)) {
                //++i1;
                if ((mbl.flags.mbvisible & 0x01) == 0)
                    TF = "no";
                else
                    TF = "yes";


                strcpy(disp, (char *)mbl.mbdisplay);
				strcpy(tType,"Jam");
                //if(mbl.Type == MSGTYPE_JAM)	 strcpy(tType,"Jam");
                //if(mbl.Type == MSGTYPE_SQUISH) strcpy(tType,"Squish");
                sprintf(outbuff,
                        "|15%3d. |09%-30s |03%-10s |11%7.7s |07%7.7s |08%7.7s  |15%s|CR",
                        (rno+i1)+1,
                        disp,
                        (char *)mbl.mbfile,
                        mbtypes[mbl.Kind],
                        tType,
                        posttyp[mbl.Pubpriv],
                        (char *)TF.c_str());
                pipe2ansi(outbuff);

            }
        }
    }
    else
    {
        pipe2ansi((char *)"|13 No Forums Found, Please Insert!|CR|CR |09By default, first area should be Email|08. |CR |09Second Area should be System Announcements|08. |CR");
        return FALSE;
    }
    return TRUE;
}

/**
 * Message Area Editor - Area Editor (Generic)
 */
void msg_edit::mbeditmenu()
{

//    int x = 0;
    int page = 0;
    unsigned char ch;
    std::string output;
    char sLine[200]= {0};

    int total_mbases = 0,total_pages = 0;

    /*
    mb_list_rec_OLD mbl_old;
    mb_list_rec mbl;


    /// Temp for copying old to new format_paragraph

    memset(&mbl,0,sizeof(mb_list_rec));

    int i = 0;
    while(_mfunc.read_mbaseoldlist(&mbl_old,i))
    {
    	mbl.idx = mbl_old.idx;
    	mbl.Type = mbl_old.Type;
    	mbl.Kind = mbl_old.Kind;
    	mbl.Active = mbl_old.Active;
    	mbl.Pubpriv = mbl_old.Pubpriv;
    	mbl.Allowalias = mbl_old.Allowalias;
    	mbl.scantype = mbl_old.scantype;
    	mbl.fidoaddr = mbl_old.fidoaddr;
    	mbl.sigtype = mbl_old.sigtype;

    	strcpy((char *)mbl.mbdisplay,mbl_old.mbdisplay);
    	strcpy((char *)mbl.mbfile,mbl_old.mbfile);
    	strcpy((char *)mbl.mbpath,mbl_old.mbpath);
    	strcpy((char *)mbl.password,mbl_old.password);

    	strcpy((char *)mbl.origin_line,mbl_old.OriginLine);
    	strcpy((char *)mbl.colors,(char *)mbl_old.colors);
    	strcpy((char *)mbl.infofile,mbl_old.infofile);

    	mbl.MaxMsgs = mbl_old.MaxMsgs;
    	mbl.MaxAge = mbl_old.MaxAge;
    	mbl.first = mbl_old.first;
    	mbl.last = mbl_old.last;
    	mbl.current = mbl_old.current;
    	mbl.lastread = mbl_old.lastread;
    	mbl.status = mbl_old.status;


    	mbl.aka.zone = mbl_old.aka.zone;
    	mbl.aka.net = mbl_old.aka.net;
    	mbl.aka.node = mbl_old.aka.node;
    	mbl.aka.point = mbl_old.aka.point;
    	strcpy((char *)mbl.aka.domain,(char *)mbl_old.aka.domain);

    	_mfunc.save_mbaselist(&mbl,i++);
    }

    return;*/


    while(1)
    {

        total_mbases = _mfunc.msg_count();
        if (total_mbases > 14)
        {
            total_pages =  (total_mbases / 14);
            if (total_mbases % 14) ++total_pages;
        }
        else
        {
            total_pages = 1;
        }

        if (isSysop == TRUE)
        {
            output += "|CS|11 Message Forum Editor                                      ";
        }
        else
        {
            output += "|CS|11 Message Forum Editor |07[|04Normal User View, Toggle Back to Continue Editing|12!!|07] ";
        }
        output += "|CR|15--------------------------------------------------------------";
        sprintf(sLine,"|CR |07Forums|08: |11%d |07Page|08: |03%.2d|07 / |03%.2d|CR|CR",total_mbases,page+1,total_pages);
        output += sLine;
        pipe2ansi((char *)output.c_str());

        // Display Areas
        //x=list_message_bases(page);
        list_message_bases(page);

        //pipe2ansi("|CR|15d|07.elete, |15e|07.dit, |15i|07.nsert, |15m|07.ove, |15q|07.uit : \x1b[1;44m \x1b[D");
        //pipe2ansi((char *)"|CR |15c|07.ompile areas|08.     |09Generate Mesg Area List for Normal Users|08!");
        //pipe2ansi((char *)"|CR |15t|07.oggle Sysop View|08. |09Shows all areas as how a normal user would see them|08!");
        pipe2ansi((char *)"|CR |15i|07.nsert, |15e|07.dit, |15m|07.ove, |15d|07.elete, |15[|07.prev page, |15]|07.next page |15q|07.uit : \x1b[1;44m \x1b[D");

        ch = getkey(true);
        switch(toupper(ch))
        {
        case 'D':
            delete_mbase();
            break;
        case 'I':
            insert_mbase();
            break;
        case 'E':
            modify_mbase();
            break;
        case 'M':
            move_mbase();
            break;
		/*
        case 'C':
            compile_mbase();
            break;*/

        //case 'B':	import_backbone();	break;
        case 'Q':
            isSysop = TRUE;
            return;
        case '[':
            page--;
            if(page < 0)
                page = 0;
            break;
        case ']':
            if(page+1 < total_pages)
                page++;
            break;
        case 'T':
            if (isSysop == TRUE) isSysop = FALSE;
            else isSysop = TRUE;
            break;
        default :
            break;
        }
    }
}



