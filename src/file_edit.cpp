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

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

/*
# include "struct.h"
//# include "msgapi.h"
# include "file_edit.h"
# include "files.h"

# include <cstdio>
# include <cstring>
# include <cstdlib>
# include <cctype>

# include <string>

using namespace std;


typedef enum {LOCAL, NETMAIL, ECHOMAIL, EMAIL, NEWS} MSGTYPE;
typedef enum {PUBLIC, PRIVATE} POSTTYPE;

char fbtypes[5][21]= {"Local","Netmail","Echomail","Email","NewsGroup"};
char areatyp[2][11]= {"Public","Private"};

# define MSG_QUOTE  0
# define MSG_TEXT   1
# define MSG_TEAR   2
# define MSG_ORIGIN 3


char *file_edit::faddr2char(char *s,fidoaddr *fa)
{

    if(fa->point)
        sprintf(s,"%u:%u/%u.%u",
                fa->zone,fa->net,fa->node,fa->point);
    else
        sprintf(s,"%u:%u/%u",
                fa->zone,fa->net,fa->node);
    return(s);
}

void file_edit::fbeditmenu()
{

    int x = 0, page = 0;
    unsigned char ch;

    while(1)
    {
        putline((char *)"\x1b[0m\x1b[2J");
        putline((char *)". File Area Editor \r\n\r\n");
        x=list_file_bases(page);
        putline((char *)"\n [d]elete, [e]dit, [i]nsert, [m]ove, [q]uit : \x1b[1;44m \x1b[D");

        ch = getkey(true);
        switch(toupper(ch))
        {
        case 'D':
            delete_fbase();
            break;
        case 'I':
            insert_fbase();
            break;
        case 'E':
            modify_fbase();
            break;
        case 'M':
            move_fbase();
            break;
            //case 'B': import_backbone();  break;
        case 'Q':
            return;
        case '[':
            page--;
            if(page < 0)
                page = 0;
            break;
        case ']':
            if(x)
                page++;
            break;
        default :
            break;
        }
    }
}

// Ok somehow i took out the pages for a look, fix later! :)
int file_edit::list_file_bases(int page)
{

    fb_list_rec fbl;
    char disp[61] = {0},
                    tType[12]= {0};
    char outbuff[1024]= {0};
    int i = _ffunc.file_count();
    if (i != 0)
    {
        i = 0;
        while(_ffunc.read_fbaselist(&fbl,i++))
        {
            strcpy(disp, (char *)fbl.fbdisplay);
            if(fbl.Type == MSGTYPE_JAM)  strcpy(tType,"Jam");
            if(fbl.Type == MSGTYPE_SQUISH) strcpy(tType,"Squish");
            sprintf(outbuff,
                    "%3d %-34s %-10s %7.7s %7.7s %7.7s\r\n",
                    (i),
                    disp,
                    fbl.fbfile,
                    fbtypes[fbl.Kind],
                    tType,
                    areatyp[fbl.Pubpriv]);
            putline(outbuff);
        }
    }
    else
    {
        putline((char *)" No Areas Found!\r\n");
        return FALSE;
    }
    return TRUE;
}



void file_edit::init_fbase(fb_list_rec *fb)
{

    strcpy((char *)fb->fbdisplay,"New File Base");     // Description
    strcpy((char *)fb->fbpath,FILEPATH);               // Path to Message Files
    strcpy((char *)fb->fbfile,"newbase");              // Filename
    fb->Type       = MSGTYPE_JAM;              // Default to JAM
    fb->Kind       = 0;                        // Local
    fb->Pubpriv    = 0;                        // Public


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
}


void file_edit::kill_fbase(int fbnum)
{

    fb_list_rec fbin;
    int i1,count=0;
    char ppath[81]= {0};
    char ppath2[81]= {0};

    i1=0;
    while(_ffunc.read_fbaselist(&fbin,i1))
    {
        if(i1 != fbnum)
        {
            fbin.idx = count;
            count++;
            if(!_ffunc.save_fbasetemp(&fbin, fbin.idx))
            {
                putline((char *)"Error saving to temp file.");
            }
        }
        i1++;
    }

    // Remove Normal file
    sprintf(ppath,"%sfiles.dat",DATAPATH);
    remove(ppath);
    // Rename Tmp to Original
    sprintf(ppath2,"%sfiles.tmp",DATAPATH);
    rename(ppath2, ppath);
    // Remove Tmp
    remove(ppath2);
}

void file_edit::delete_fbase()
{

    char ttxt[21]= {0};
    int num, total_fbases;
    char sLine[200]= {0};

    total_fbases = _ffunc.file_count();
    sprintf(sLine,"\r\nDelete which? [1-%d]: \x1b[1;44m  \x1b[2D", total_fbases);
    putline(sLine);
    getline(ttxt,sizeof(ttxt));
    if (strcmp(ttxt," ") == 0) return;
    num = atoi(ttxt);
    --num;
    if(num >= 0 && num <= total_fbases)
    {
        kill_fbase(num);
    }
}


void file_edit::poke_fbase(int pokenum)
{

    fb_list_rec fbin;
    int total_fbases = _ffunc.file_count();

    if(total_fbases>0)
    {
        for(int i1 = total_fbases; i1 > pokenum; i1--)
        {
            _ffunc.read_fbaselist(&fbin,i1-1);
            fbin.idx++;
            _ffunc.save_fbaselist(&fbin,fbin.idx);
        }
    }
    else
        pokenum=0;

    init_fbase(&fbin);
    fbin.idx=pokenum;
    _ffunc.save_fbaselist(&fbin,fbin.idx);
    total_fbases++;
}

void file_edit::insert_fbase()
{

    char sLine[200];
    fb_list_rec fbin;
    memset(&fbin,0,sizeof(fb_list_rec));
    char ttxt[21]= {0};
    int num, total_fbases;

    total_fbases = _ffunc.file_count();
    sprintf(sLine,"\r\n Insert before which? [1-%d]: \x1b[1;44m  \x1b[2D", total_fbases+1);
    putline(sLine);

    getline(ttxt,sizeof(ttxt));
    if (strcmp(ttxt," ") == 0) return;
    num = atoi(ttxt);
    --num;

    if(num >= 0 && num <= total_fbases)
    {
        if(num == 0 && total_fbases == 0)
        {
            init_fbase(&fbin);
            fbin.idx = total_fbases;
            _ffunc.save_fbaselist(&fbin, fbin.idx);
            total_fbases++;
        }
        else
        {
            poke_fbase(num);
        }
    }
}



void file_edit::swap_fbase(int iFrom, int iTo)
{

    fb_list_rec fbto, fbfrom;
    _ffunc.read_fbaselist(&fbfrom, iFrom);

    if(iFrom < iTo)
    {
        for(int i1 = iFrom+1; i1 <= iTo; i1++)
        {
            _ffunc.read_fbaselist(&fbto,i1);
            fbto.idx--;
            _ffunc.save_fbaselist(&fbto, fbto.idx);
        }
    }
    else
    {
        for(int i1 = iFrom; i1 > iTo; i1--)
        {
            _ffunc.read_fbaselist(&fbto, i1-1);
            fbto.idx++;
            _ffunc.save_fbaselist(&fbto, fbto.idx);
        }
    }
    fbfrom.idx = iTo;
    _ffunc.save_fbaselist(&fbfrom, fbfrom.idx);
}


void file_edit::move_fbase()
{

    char ttxt[21]= {0};
    char sLine[200]= {0};
    int num = 0, num1 = 0, total_fbases = 0;
    total_fbases = _ffunc.file_count();

    sprintf(sLine,"\n Move which? [1-%d]: \x1b[1;44m  \x1b[2D", total_fbases);
    putline(sLine);
    getline(ttxt,sizeof(ttxt));
    if (strcmp(ttxt," ") == 0) return;
    num = atoi(ttxt);
    --num;

    if(num1 >= 0 && num1 < total_fbases)
    {
        sprintf(sLine,"\n Position Before? [1-%d]: \x1b[1;44m  \x1b[2D", total_fbases);
        putline(sLine);
        getline(ttxt,sizeof(ttxt));
        if (strcmp(ttxt," ") == 0) return;
        num1 = atoi(ttxt);
        --num1;

        if(num >= 0 && num < total_fbases)
            swap_fbase(num1, num);
    }
}


void file_edit::modify_fbase()
{

    char ttxt[21]= {0};
    char sLine[200]= {0};
    int num;
    int total_fbases = _ffunc.file_count();

    sprintf(sLine,"\n Edit which? [1-%d]: \x1b[1;44m  \x1b[2D", total_fbases);
    putline(sLine);
    getline(ttxt,sizeof(ttxt));
    if (strcmp(ttxt," ") == 0) return;

    num = atoi(ttxt);
    --num;

    if(num>=0 && num<=total_fbases)
    {
        mod_fbase(num);
    }
}

void file_edit::edit_aka(fb_list_rec *fr)
{

    char ttxt[16];
    char *p;
    putline((char *)"\n\n Enter Fido Address: \x1b[1;44m                              \x1b[30D");
    getline(ttxt,sizeof(ttxt));
    if (strcmp(ttxt," ") == 0) return;
    //strcpy(fbl.mbdisplay,ttxt);

    //get_aka(ttxt);  Maybe Impliment Later

    p=strtok(ttxt,":/.");
    if(p)   fr->aka.zone=atoi(p);
    p=strtok(NULL,":/.");
    if(p)   fr->aka.net=atoi(p);
    p=strtok(NULL,":/.");
    if(p)   fr->aka.node=atoi(p);
    p=strtok(NULL,":/.");
    if(p)   fr->aka.point=atoi(p);

    //mci("~SM`09Domain`0F: `1F");
    //ttxt[0]='\0';
    //get_str(config.aka[num].domain,21);
}


void file_edit::ACS(ACSrec *acs, char *fstr) {

    class acs_levels *level = new acs_levels;
    level->ACS_setup(acs, fstr);
    delete level;
}


void file_edit::mod_fbase(int fbnum)
{

    fb_list_rec fbl;
    char faka[41]= {0};
    char sigtxt[3][21]= {"None","Short Sig.","Long Sig."};
    std::string TF;
    char rBuffer[200]= {0};

    _ffunc.read_fbaselist(&fbl, fbnum);
    if(fbl.sigtype < 0 || fbl.sigtype > 2) fbl.sigtype = 0;

    int total_fbases = _ffunc.file_count();
    unsigned char ch;
    char sLine[200];

    while(1)
    {

        faddr2char(faka,&fbl.aka);
        strcpy(rBuffer,"");
        putline((char *)"\x1b[0m\x1b[2J");
        putline((char *)".----------------------------------------------------------------.\r\n");
        putline((char *)"| File Area Editor                                               |\r\n");
        putline((char *)"`----------------------------------------------------------------'\r\n");
        sprintf(sLine,"  Area : %d of %d", fbnum+1,total_fbases);
        putline(sLine);
        putline((char *)"\r\n");
        putline((char *)".-------------------------.");
        putline((char *)"\r\n");
        sprintf(sLine,"| [A]. Description        | %s", fbl.fbdisplay);
        putline(sLine);
        putline((char *)"\r\n");
        sprintf(sLine,"| [B]. Filename           | %s", fbl.fbfile);
        putline(sLine);
        if (fbl.Type == MSGTYPE_JAM) TF = "JAM";
        else TF = "SQUISH";
        putline((char *)"\r\n");
        sprintf(sLine,"| [C]. Message Type       | %s", TF.c_str());
        putline(sLine);
        putline((char *)"\r\n");
        sprintf(sLine,"| [D]. Forum Kind         | %s", fbtypes[fbl.Kind]);
        putline(sLine);
        putline((char *)"\r\n");
        sprintf(sLine,"| [E]. Posting Type       | %s", areatyp[fbl.Pubpriv]);
        putline(sLine);
        putline((char *)"\r\n");
        sprintf(sLine,"| [F]. File Path          | %s", fbl.filepath);
        putline(sLine);
        if (fbl.Allowalias) TF = "yes";
        else TF = "no";
        putline((char *)"\r\n");
        sprintf(sLine,"| [G]. Allow Aliases      | %s", TF.c_str());
        putline(sLine);
        putline((char *)"\r\n");
        sprintf(sLine,"| [H]. Post ACS           | ");
        putline(sLine);
        putline((char *)"\r\n");
        sprintf(sLine,"| [I]. Read ACS           | ");
        putline(sLine);
        putline((char *)"\r\n");
        sprintf(sLine,"| [J]. Sysop ACS          | ");
        putline(sLine);
        putline((char *)"\r\n");
        sprintf(sLine,"| [K]. Signature          | %s", sigtxt[fbl.sigtype]);
        putline(sLine);
        putline((char *)"\r\n");
        sprintf(sLine,"| [L]. Mail address       | %s", faka);
        putline(sLine);


        mci("%sQuoted (%d)`0F, ",
            ansic(fbl.colors[MSG_QUOTE]),
            fbl.colors[MSG_QUOTE]);
        mci("%sText (%d)`0F,",
            ansic(fbl.colors[MSG_TEXT]),
            fbl.colors[MSG_TEXT]);
        mci("%sTear (%d)`0F,",
            ansic(fbl.colors[MSG_TEAR]),
            fbl.colors[MSG_TEAR]);
        mci("%sOrigin (%d)~SM",
            ansic(fbl.colors[MSG_ORIGIN]),
            fbl.colors[MSG_ORIGIN]);

        putline((char *)"\r\n");
        sprintf(sLine,"| [M]. Origin Line        | %s", fbl.OriginLine);
        putline(sLine);
        putline((char *)"\r\n");
        putline((char *)"`-------------------------'\r\n\r\n");

        putline((char *)" [d]elete, [e]dit, [i]nsert, [m]ove, [q]uit : \x1b[1;44m \x1b[D");

        ch = getkey(true);
        switch(toupper(ch))
        {

        case 'A':
            putline((char *)"\r\n\r\n Description: \x1b[1;44m                              \x1b[30D");
            getline(rBuffer,sizeof(rBuffer));
            if (strcmp(rBuffer," ") == 0) break;
            strcpy((char *)fbl.fbdisplay,rBuffer);
            break;

        case 'B':
            putline((char *)"\r\n\r\n New Filename: \x1b[1;44m                              \x1b[30D");
            getline(rBuffer,sizeof(rBuffer));
            if (strcmp(rBuffer," ") == 0) break;
            strcpy((char *)fbl.fbfile,rBuffer);
            break;

        case 'C':
            if(fbl.Type == MSGTYPE_JAM) fbl.Type = MSGTYPE_SQUISH;
            else                        fbl.Type = MSGTYPE_JAM;
            break;

        case 'D':
            ++fbl.Kind;
            if(fbl.Kind > 4)
                fbl.Kind = 0;
            switch(fbl.Kind)
            {
            case LOCAL    :
                fbl.Pubpriv = PUBLIC;
                break;
            case ECHOMAIL :
                fbl.Pubpriv = PUBLIC;
                break;
            case NETMAIL  :
                fbl.Pubpriv = PRIVATE;
                break;
            case EMAIL    :
                fbl.Pubpriv = PRIVATE;
                break;
            case NEWS     :
                fbl.Pubpriv = PUBLIC;
                break;
            }
            break;

        case 'E':
            if(fbl.Pubpriv==PUBLIC)
            {
                switch(fbl.Kind)
                {
                case LOCAL    :
                    fbl.Pubpriv = PRIVATE;
                    break;
                case ECHOMAIL :
                    fbl.Pubpriv = PUBLIC;
                    break;
                case NETMAIL  :
                    fbl.Pubpriv = PRIVATE;
                    break;
                case EMAIL    :
                    fbl.Pubpriv = PRIVATE;
                    break;
                case NEWS     :
                    fbl.Pubpriv = PUBLIC;
                    break;
                }
            }
            else
            {
                switch(fbl.Kind)
                {
                case LOCAL    :
                    fbl.Pubpriv = PUBLIC;
                    break;
                case ECHOMAIL :
                    fbl.Pubpriv = PUBLIC;
                    break;
                case NETMAIL  :
                    fbl.Pubpriv = PRIVATE;
                    break;
                case EMAIL    :
                    fbl.Pubpriv = PRIVATE;
                    break;
                case NEWS     :
                    fbl.Pubpriv = PUBLIC;
                    break;
                }
            }
            break;

        case 'F':
            putline((char *)"\r\n\r\n File Path: \x1b[1;44m                                                       \x1b[55D");
            getline(rBuffer,sizeof(rBuffer));
            if (strcmp(rBuffer," ") == 0) break;
            strcpy((char *)fbl.filepath,rBuffer);
            break;

        case 'G':
            fbl.Allowalias = (fbl.Allowalias)?0:1;
            break;

            ///          case 'H': ACS(&fbl.WriteACS,fbl.mbdisplay); break;
                        case 'I': ACS(&fbl.ReadACS, fbl.mbdisplay); break;
                        case 'J': ACS(&fbl.SysopACS,fbl.mbdisplay); break;

        case 'K':
            fbl.sigtype++;
            if(fbl.sigtype<0 || fbl.sigtype>2)
                fbl.sigtype=0;
            break;

        case 'L':
            edit_aka(&fbl);
            //fbl.fidoaddr=pick_aka();
            break;

        case 'M':
            putline((char *)"\r\n\r\n Origin Line: \x1b[1;44m                              \x1b[30D");
            getline(rBuffer,sizeof(rBuffer));
            if (strcmp(rBuffer," ") == 0) break;
            strcpy((char *)fbl.OriginLine,rBuffer);
            break;

        case 'Q':
            _ffunc.save_fbaselist(&fbl, fbnum);
            _ffunc.save_filebase(&fbl);
            return;
        }
    }
}

*/
