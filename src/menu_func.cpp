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

// Enthral SVN: $Id: menu_func.cpp 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/menu_func.cpp $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

// Automake
# ifdef HAVE_CONFIG_H
# include <config.h>
# endif

// Automake.
# ifdef HAVE_PYTHON
# include "pyenthral.h"
# endif

# include "struct.h"
# include "menu_func.h"

# include "msg_edit.h"
# include "msg_read.h"

# include "conio.h"
# include "logon.h"
# include "users.h"
# include "node.h"
# include "data_areas.h"
# include "msg_fse.h"
# include "msg_area.h"
# include "msg_email.h"
# include "msg_pack.h"

# include "user_list.h"

# include "file_edit.h"
# include "file_area.h"
# include "files.h"


# include <string>
# include <iostream>
# include <fstream>
# include <sstream>    // Stream Numbers to Strings.

# include <cstdio>
# include <cstdlib>
# include <ctime>
# include <cstring>   // gcc 4.3
# include <unistd.h>  // gcc 4.7

# include <termios.h>
# include <pthread.h>


using namespace std;

/*
 * Menu System Class
 */
menu_func::menu_func()
{
    ////errlog2("menu_func() menu_func new instance!");
    _loadnew = false;

    // _curmenu.clear();
    // _premenu.clear();
    // _gosub.clear();

    menur2   = 0;
    cmdr2    = 0;

    execnum  = 0;
    execnum2 = 0;
    execnum3 = 0;

    System_Alive = TRUE;
    MsgsLeft = 0;
    CurMsgs  = 0;
    TotMsgs  = 0;
    choice   = 0;

    // For Lightbar Starting Position
    // Now uses menu system for lightbars.
    CURRENT_AREA = 0;
    //menu_reset();
}


menu_func::~menu_func()
{
    ////errlog2("~menu_func()");
    if (menur2   != 0) delete    menur2;
    if (cmdr2    != 0) delete [] cmdr2;

    if (execnum  != 0) delete [] execnum;
    if (execnum2 != 0) delete [] execnum2;
    if (execnum3 != 0) delete [] execnum3;

    menur2   = 0;
    cmdr2    = 0;

    execnum  = 0;
    execnum2 = 0;
    execnum3 = 0;
    choice   = 0;
    ////errlog2("~menu_func() Done!");

}


/*
 * Parse Helper for Menu's / Commands
 */
void menu_func::chkparse(std::string &temp)
{
    std::string temp1;
    std::string::size_type st1 = 0;
    std::string::size_type st2 = 0;
    std::string::size_type  ct = 0;

    st1 = temp.find('"', 0);
    st2 = temp.find('"', st1+1);

    if (st1 != std::string::npos &&
        st2 != std::string::npos) {
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        temp = temp1;
    } else
        temp.erase();
}

/*
 * Parse Menu.TXT Files
 */
int menu_func::mnuparse(std::string cfgdata)
{
    std::string::size_type id1 = 0;

    // Disgards any Config lines with the # Character
    if (cfgdata[0] == '#') return FALSE;

    id1 = cfgdata.find("MenuName ", 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        menur2->MenuName = cfgdata;
        return FALSE;
    }

    id1 = cfgdata.find("Directive ", 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        menur2->Directive = cfgdata;
        return FALSE;
    }

    id1 = cfgdata.find("MenuPrompt ", 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        menur2->MenuPrompt = cfgdata;
        return FALSE;
    }

    id1 = cfgdata.find("Acs ", 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        menur2->Acs = cfgdata;
        return FALSE;
    }

    id1 = cfgdata.find("Password ", 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        menur2->Password = cfgdata;
        return FALSE;
    }

    id1 = cfgdata.find("ForceInput ", 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        menur2->ForceInput = atoi((char *)cfgdata.c_str());
        return FALSE;
    }

    id1 = cfgdata.find("ForceHelpLevel ", 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        menur2->ForceHelpLevel = atoi((char *)cfgdata.c_str());
        return FALSE;
    }

    id1 = cfgdata.find("Lightbar ", 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        if (cfgdata == "TRUE") menur2->Lightbar = TRUE;
        else menur2->Lightbar = FALSE;
        return FALSE;
    }

    id1 = cfgdata.find("DigitPassing ", 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        if (cfgdata == "TRUE") menur2->MFlags.DigitPassing = TRUE;
        else menur2->MFlags.DigitPassing = FALSE;
        return FALSE;
    }

    id1 = cfgdata.find("EscPassing ", 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        if (cfgdata == "TRUE") menur2->MFlags.EscPassing = TRUE;
        else menur2->MFlags.EscPassing = FALSE;
        return FALSE; // Last Menu Setting, don't need to parse rest of file.
    }
    // End of Screen, now get X/Y and put lightbars there, instead of hard coded xY
    id1 = cfgdata.find("EOSPrompt ", 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        if (cfgdata == "TRUE") menur2->MFlags.EOSPrompt = TRUE;
        else menur2->MFlags.EOSPrompt = FALSE;
        return TRUE; // Last Menu Setting, don't need to parse rest of file.
    }

    return FALSE;
}

/*
 * Loop for Reading reading a menu
 */
int menu_func::menu_read(std::string MenuName)
{
    std::string path;
    path = MENUPATH;
    path.append(MenuName);
    path.append(".txt");

    // Else Read and Parse it
    ifstream iFS;
    iFS.open( path.c_str() );
    if (!iFS.is_open()) {
//        errlog((char *)"Couldn't Open Menu Area: %s",(char *)path.c_str());
        return FALSE;
    }

    std::string cfgdata;

    // New Menu Command, Not all Menus have it, Default to FALSE!
    menur2->MFlags.EOSPrompt = FALSE;
    for (;;) {
        if(iFS.eof()) break;
        std::getline(iFS,cfgdata,'\n');
        if(iFS.eof()) break;
        if (mnuparse(cfgdata) == TRUE) break;
    }
    iFS.close();

    //memcpy(menur,&_menur,sizeof(CommandRec));
    return TRUE;
}

/*
 * Parse Commands Reads from a Menu File
 */
void menu_func::cmdparse(std::string cfgdata, int idx)
{
    std::string::size_type id1 = 0;
    char sText[200] = {0};

    // Disgards any Config lines with the # Character
    if (cfgdata[0] == '#') return ;

    sprintf(sText,"LDesc[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].LDesc = cfgdata;
        return;
    }

    sprintf(sText,"SDesc[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].SDesc = cfgdata;
        return;
    }

    sprintf(sText,"CKeys[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].CKeys = cfgdata;
        return;
    }

    sprintf(sText,"Acs[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].Acs = cfgdata;
        return;
    }

    sprintf(sText,"CmdKeys[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].CmdKeys = cfgdata;
        return;
    }

    sprintf(sText,"MString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].MString = cfgdata;
        return;
    }

    sprintf(sText,"HiString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].HiString = cfgdata;
        return;
    }

    /*
    id1 = -1;
    sprintf(sText,"MidString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != -1) {
        chkparse(cfgdata);
        strcpy(cmdr->MidString,(char *)cfgdata.c_str());
        return;
    }*/

    sprintf(sText,"LoString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].LoString = cfgdata;
        return;
    }

    sprintf(sText,"Xcoord[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].Xcoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"Ycoord[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].Ycoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"LBarCmd[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        if (cfgdata == "TRUE")
            cmdr2[idx].LBarCmd = TRUE;
        else
            cmdr2[idx].LBarCmd = FALSE;
        return;
    }

    sprintf(sText,"SText[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        if (cfgdata == "TRUE")
            cmdr2[idx].SText = TRUE;
        else
            cmdr2[idx].SText = FALSE;
        return;
    }

    sprintf(sText,"STLen[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].STLen = atoi((char *)cfgdata.c_str());
        return;
    }

}

/*
 * Check if Command Exists (Used for Counting Commands)
 */
int menu_func::cmdexist(std::string MenuName, int idx)
{
    std::string path = MENUPATH;
    path += MenuName;
    path += ".txt";
    int ret = FALSE;
    char sText[200]= {0};

    // Open file for reading and parsing.
    ifstream iFS2;
    iFS2.open( path.c_str() );
    if (!iFS2.is_open()) {
        //errlog((char *)"Couldn't Open Menu Command [cmdexist()]: %s\n", path.c_str());
        return ret;
    }

    // Loop Through and Find the The Command
    sprintf(sText,"[CommandRec%.03d]",idx);
    std::string cfgdata;
    for (;;) {
        std::getline(iFS2,cfgdata,'\n');
        if (cfgdata.find(sText,0) != std::string::npos)
            ret = TRUE;
        if(iFS2.eof()) break;
    }
    iFS2.close();


    //if (ret==TRUE) sprintf(sText,"EXISTS! CommandRec [%.03d]",idx);
    //else sprintf(sText,"NOT EXISTS! CommandRec [%.03d]",idx);
    //putline(sText);

    return ret;
}


/*
 * Right now this loops through the menu too many times! haha
 * Make it parse once for the max command rec found, save cpu! :)
 * Files are small, so no rush! :)
 */
int menu_func::cnt_cmds(std::string MenuName)
{
    int  cnt = 0;
    while (cmdexist(MenuName,cnt)) {
        ++cnt;
    }
    return cnt;
}

/**
 * Loop to Read commands in a Menu File.
 */
int menu_func::cmds_read(std::string MenuName, int idx)
{
    std::string path = MENUPATH;
    path += MenuName;
    path += ".txt";

    //CommandRec _cmdr;
    //memset(&_cmdr,0,sizeof(CommandRec));

    //check if the command can be found in the file, else return false!
    // Don't need to Run through parsing each command,
    // Just do an initial Cnt_cmds and don't go over it! :)

    int ret;
    ret = cmdexist(MenuName,idx);
    if (ret < 1) return FALSE;


    // Else Read and Parse it
    ifstream iFS;
    iFS.open( path.c_str() );
    if (!iFS.is_open()) {
        //errlog((char *)"Couldn't Open Menu Commands: %s\n", path.c_str());
        return FALSE;
    }

    std::string cfgdata;
    for (;;) {
        std::getline(iFS,cfgdata,'\n');
        cmdparse(cfgdata,idx);
        //cfgdata += "\r\n";   (5 Times per menu., need to rewrite.!!)
        //putline((char *)cfgdata.c_str());
        if(iFS.eof()) break;
    }
    iFS.close();

    //memcpy(cmdr,&_cmdr,sizeof(CommandRec));
    return TRUE;
}

/*
// Convert From Old .DAT format to TEXT.
int menu_func::convert_read(std::string menu, CommandRec *cmdr, int idx)
{
    std::string path = MENUPATH;
    path += menu;
    path += ".mnu";

    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        return x;
    }
    fclose(stream);

    stream = fopen(path.c_str(), "rb");
    if(fseek(stream,(int)idx*sizeof(CommandRec),SEEK_SET)==0)
        x = fread(cmdr,sizeof(CommandRec),1,stream);
    fclose(stream);
    return x;
}
*/

// Convert From Old .DAT format to TEXT.
// Make these either XML or insert into a SQL table. :)
void menu_func::menu_convert(std::string menuname)
{
    /*   Outdated, save for update from char to std::string.
        CommandRec cmdr;
        memset(&cmdr,0,sizeof(CommandRec));

        std::string mname = MENUPATH;
        mname += menuname;
        mname += ".txt";

        ofstream oFS2;
        oFS2.open( mname.c_str(), ofstream::out | ofstream::trunc );
        if (!oFS2.is_open()) {
            printf("Error Creating Menu!");
            return;
        }

        char sText[255]={0};
        std::string sTmp;
        int idx = 0;

        oFS2 << sTmp << endl;
        oFS2 << "#" << endl;
        oFS2 << "# This is an Example Menu" << endl;
        oFS2 << "#" << endl;
        oFS2 << "#" << endl;
        oFS2 << endl;
        oFS2 << "[MenuRec]" << endl;
        oFS2 << "MenuName        = \"\"" << endl;
        oFS2 << "Directive       = \"\"" << endl;
        oFS2 << "MenuPrompt      = \"\"" << endl;
        oFS2 << "Acs             = \"\"" << endl;
        oFS2 << "Password        = \"\"" << endl;
        oFS2 << "ForceInput      = \"0\"" << endl;
        oFS2 << "ForceHelpLevel  = \"0\"" << endl;
        oFS2 << "Lightbar        = \"TRUE\"" << endl;
        oFS2 << endl;
        oFS2 << "[MenuFlags]" << endl;
        oFS2 << "ClrScrBefore    = \"FLASE\"" << endl;
        oFS2 << "NoMenuPrompt    = \"FLASE\"" << endl;
        oFS2 << "ForcePause      = \"FLASE\"" << endl;
        oFS2 << "ClrScrAfter     = \"FLASE\"" << endl;
        oFS2 << "UseGlobal       = \"FLASE\"" << endl;
        oFS2 << endl;

        while (convert_read(menuname, &cmdr, idx)) {

            sprintf(sText,"[CommandRec%.03d]",idx);
            sTmp = sText;
            oFS2 << sTmp << endl;

            sprintf(sText,"LDesc[%.03d]      = \"%s\"",idx,cmdr.LDesc);
            sTmp = sText;
            oFS2 << sTmp << endl;
            sprintf(sText,"SDesc[%.03d]      = \"%s\"",idx,cmdr.SDesc);
            sTmp = sText;
            oFS2 << sTmp << endl;
            sprintf(sText,"CKeys[%.03d]      = \"%s\"",idx,cmdr.CKeys);
            sTmp = sText;
            oFS2 << sTmp << endl;
            sprintf(sText,"Acs[%.03d]        = \"%s\"",idx,cmdr.Acs);
            sTmp = sText;
            oFS2 << sTmp << endl;
            sprintf(sText,"CmdKeys[%.03d]    = \"%s\"",idx,cmdr.CmdKeys);
            sTmp = sText;
            oFS2 << sTmp << endl;
            sprintf(sText,"MString[%.03d]    = \"%s\"",idx,cmdr.MString);
            sTmp = sText;
            oFS2 << sTmp << endl;
            sprintf(sText,"HiString[%.03d]   = \"%s\"",idx,cmdr.HiString);
            sTmp = sText;
            oFS2 << sTmp << endl;
            sprintf(sText,"LoString[%.03d]   = \"%s\"",idx,cmdr.LoString);
            sTmp = sText;
            oFS2 << sTmp << endl;

        //menur->MFlags.EOSPrompt = TRUE


            sprintf(sText,"Xcoord[%.03d]     = \"%d\"",idx,cmdr.Xcoord);
            sTmp = sText;
            oFS2 << sTmp << endl;
            sprintf(sText,"Ycoord[%.03d]     = \"%d\"",idx,cmdr.Ycoord);
            sTmp = sText;
            oFS2 << sTmp << endl;
            sprintf(sText,"LBarCmd[%.03d]    = \"%i\"",idx,cmdr.LBarCmd);
            sTmp = sText;
            oFS2 << sTmp << endl;
            sprintf(sText,"SText[%.03d]      = \"%i\"",idx,cmdr.SText);
            sTmp = sText;
            oFS2 << sTmp << endl;
            sprintf(sText,"STLen[%.03d]      = \"%i\"",idx,cmdr.STLen);
            sTmp = sText;
            oFS2 << sTmp << endl;
            oFS2 << endl;
            ++idx;
        }
        oFS2.close();
        */
}



/**
 * Loads All Commands into Class.
 */
void menu_func::insert_cmdr()
{
    //menus _mnu;
    //CommandRec cmdr;
    //memset(&cmdr,0,sizeof(CommandRec));
    //  int idx = 0;
    //  while (menu_read(_curmenu))
    //  {
    //      ++idx;
    //      if (noc == idx) break;
//   }

    int  idx = 0;
    while (cmds_read(_curmenu,idx)) {
        //cmdr2[noc] = *cmdr;
        //memset(&cmdr,0,sizeof(CommandRec));
        ++idx;
        //if (noc == MAX_MENU_COMMANDS-1) break;
    }

}

/**
 * Reset or Set Inital Defaults before a menu is read.
 */
void menu_func::menu_reset()
{
    xx      = 1;//  = 1;        // Holds X Coord
    yy      = 1;//  = 1;        // Holds Y Coord

    noc     = 0;                // Number of Commands.
    iNoc    = 0;//  = 0         // Holds Lightbar # of choices
    sNoc    = 0;//  = 0;        // Holds Scroll Text # of choices
    //choice  = 0;//  = 0;        // Holds Currect Lightbar #
    cntEscCmds = 0; // = 0;     // Number of ESC Commands.
    //memset(&execnum,0,sizeof(execnum)); //={0};    // Holds commands which are Lightbars

    /*
        ////errlog2(" *** menu_reset()");

        ////errlog2(" *** menu_reset() - Delete menur2");
        if (menur2)   delete    menur2;
        ////errlog2(" *** menu_reset() - Delete cmdr2");
        if (cmdr2)    delete [] cmdr2;

        // Clear
        ////errlog2(" *** menu_reset() - Delete execnum");
        if (execnum)  delete [] execnum;
        ////errlog2(" *** menu_reset() - Delete execnum2");
        if (execnum2) delete [] execnum2;
        ////errlog2(" *** menu_reset() - Delete execnum3");
        if (execnum3) delete [] execnum3;

        ////errlog2(" *** menu_reset() - cont");

        menur2 = 0;
        cmdr2 = 0;
        execnum = 0;
        execnum2 = 0;
        execnum3 = 0;
    */

    //memset(&execnum2,0,sizeof(execnum2));//={0};   // Holds commands which are Scroll Text
    //memset(&execnum3,0,sizeof(execnum3));//={0};   // Holds commands which are Scroll Text

    c  = '\0';
    cc = '\0';                      // Hold Input / Lightbar Key
    EscHit = false;                 // Is Input key Escaped char, or Normal Key
    memset(&outBuff,0,sizeof(outBuff));   // Holds Formatted Lightbar Data
    output.erase();                 // Buffer for writing all lightbars at the same time
    sRand = 0;                      // Randmise Text Scrollies
    p = 0;                          // For passing Menu command to new thread
    tScroll = false;
    executed = 0;                   // Test's for hot keys commands excuted, if non pass through loop
    // This is so scrolling isn't killed with invalid menu input!
    exe = 0;

}


/**
 * Check if Menu Exists
 */
int menu_func::menu_exists()
{
    ////errlog2("readin() 3");
    /// Check if Menu Exists first!

    std::string path;
    path = MENUPATH;
    path += _curmenu;
    path += ".txt";

    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(fstr == NULL) {
//        errlog((char *)"Menu not Found! : %s ",(char *)_curmenu.c_str());
        sleep(1);
        return FALSE;
    } else {
        fclose(fstr);
    }
    return TRUE;

}


/**
 * Function to Read in a Menu set in the class
 */
void menu_func::menu_readin()
{
    int idx  = 0;
    int nogc = 0;
    std::string path;
    path = MENUPATH;
    path += _curmenu;
    path += ".txt";

    // If we have the same menu, abort and return, no reason to re-load same!
    ////errlog2("readin() 1 %s,%s",(char *)_premenu.c_str(),(char *)_curmenu.c_str());
    if (_premenu == _curmenu || _curmenu == "") {
        _loadnew = false;
        return ;
    }

    ////errlog2("readin() 3");
    /// Check if Menu Exists first!
    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(fstr == NULL) {
//        errlog((char *)"Menu not Found! : %s ",(char *)_curmenu.c_str());
        sleep(1);
        _loadnew = false;
        return;
    } else {
        fclose(fstr);
    }

    // Setup the Default Setting and And Clear Allocations.
    menu_reset(); // Loading new Menu, Clear!

    ////errlog2("readin() 2");
    //memset(&menur2,0,sizeof(MenuRec));
    // Setup The Data Area

    if (menur2 != 0) {
        ////errlog2("readin() delete menur2;");
        delete menur2;
        menur2 = 0;
    }

    ////errlog2("readin() new menur2;");
    menur2 = new MenuRec;
    if (!menur2) {
//            errlog((char *)"Unable to Allocate Memory for Menu: %s, returning...",(char *)_curmenu.c_str());
        return;
    }

//    else errlog((char *)"Unable to Allocate Memory for Menu 1: %s, returning...",(char *)_curmenu.c_str());

    // menu_read(_curmenu);
    // noc = cnt_cmds(_curmenu);
    // Allocate global menu commands if any
    ////errlog2("readin() 4");

    if(!menu_read(_curmenu))
        return;

    if (menur2->MFlags.UseGlobal) {
        nogc = cnt_cmds((char *)"global");
        if (nogc < 1) nogc = 0;
    } else
        nogc = 0;

    noc = cnt_cmds(_curmenu);

    // Allocate & Load Commands
    //memset(&cmdr2,0,sizeof(CommandRec[MAX_MENU_COMMANDS]));
    ////errlog2("readin() 5, %i, %i",noc,nogc);

    if (cmdr2 != 0) {
        ////errlog2("readin() delete [] cmdr2;");
        delete [] cmdr2;
        cmdr2 = 0;
    }

    ////errlog2("readin() new [] cmdr2;");
    cmdr2 = new CommandRec[noc+nogc+1];
    if (!cmdr2) {
//            errlog((char *)"Unable to Allocate Memory for cmdr2: %s, returning...",(char *)_curmenu.c_str());
        return;
    }

//    else errlog((char *)"Unable to Allocate Memory for cmdr2 1: %s, returning...",(char *)_curmenu.c_str());

    ////errlog2("readin() 7.1 - %i, %i",noc,nogc);

    insert_cmdr();
    if (nogc > 0) {
        idx = 0;
        while(cmds_read((char *)"global",idx)) {
            ++noc;
            ++idx;
        }
    }

    // Reset Load New Menu Flag
    _loadnew = false;
    ////errlog2("readin() 8 Done, %i, %i",noc,nogc);
}


/**
 * Scrolling Text to Display in a seperate thread
 * When a menu command is activated.
 */
void *ScrollingText(void *p1)
{
    // Thanks to Darkwing for original reference code :)
    PASSING *p = (PASSING*)p1;
    ConsoleIO s;

    //s.////errlog2("ScrollingText!! 1");

    // Only run scrolling text if were in ANSI Mode.
    if (isANSI == FALSE || p->Active == false) {
        p->Active = false;
        delete p;
        p = 0;
        return NULL;
    }

    usleep(11*11800);

    //s.////errlog2("ScrollingText!! 2");
    unsigned
    short boxlen, loop, xpos = 0, ypos = 0, loop2;
    //char  sidecolor[04], midcolor[04],
    std::string clear, quotestr, q2, output;
    std::string midcolor;
    std::string sidecolor;
    char outBuff[100];

    // Setup The Scrolling Text
    loop = 0;
    quotestr.clear();
    output.clear();
    q2.clear();

    //s.////errlog2("ScrollingText!! 3");
    boxlen = p->cmdr2->STLen;
    if ( boxlen-1 > (int)p->cmdr2->MString.size() ) {
        for (int i = 0; i != boxlen; i++) quotestr += " ";
    }

    //s.////errlog2("ScrollingText!! 4");
    xpos = p->cmdr2->Xcoord;
    ypos = p->cmdr2->Ycoord;

    //strcpy(midcolor,  p->cmdr2->HiString);
    //strcpy(sidecolor, p->cmdr2->LoString);
    midcolor   = p->cmdr2->HiString;
    sidecolor  = p->cmdr2->LoString;

    quotestr  += p->cmdr2->MString;
    loop2 = quotestr.size();
    quotestr  += quotestr.substr ( 0, boxlen-1 );

    // s.////errlog2("ScrollingText!! 5");
    // Loop Scrolling Text Here
    do {
        if (PAUSE_SCROLING) {
            usleep(12*11800);
            continue;
        }

        // Now Write Scrolling Menu Command Text
        output.clear();
        sprintf(outBuff, "\x1b[%i;%iH",ypos,xpos);
        output  += outBuff;
        if (loop > loop2) loop = 0;
        output  += sidecolor;
        q2 = quotestr.substr (loop, 2 );
        output  += q2;
        output  += midcolor;
        q2 = quotestr.substr (loop + 2, (boxlen - 4));
        output  += q2;
        output  += sidecolor;
        q2 = quotestr.substr (loop + (boxlen - 2), 2);
        output  += q2;
        ++loop;
        s.pipe2ansi((char*)output.c_str());
        // Give small delay to Slow Text Scrolling Down
        usleep(10*11800);
    } while (p->Active);
    //s.////errlog2("ScrollingText!! 6");
    delete p;
    p = 0;
    //s.////errlog2("ScrollingText!! 7");
    return NULL;
}


/**
 * Oneliner/Rumors (WIP Started)
 * When a menu command is activated.
 */
void *DelayedOneliners(void *p1)
{
    // Thanks to Darkwing for original reference code :)

    PASSING *p = (PASSING*)p1;
    ConsoleIO s;

    //s.////errlog2("ScrollingText!! 1");

    // Only run scrolling text if were in ANSI Mode.
    if (isANSI == FALSE || p->Active == false) {
        p->Active = false;
        delete p;
        p = 0;
        return NULL;
    }

    //s.////errlog2("ScrollingText!! 2");
    unsigned
    short boxlen, loop, xpos = 0, ypos = 0, loop2;
    //char  sidecolor[04], midcolor[04],
    std::string clear, quotestr, q2, output;
    std::string midcolor;
    std::string sidecolor;
    char outBuff[100];

    // Setup The Scrolling Text
    loop = 0;
    quotestr.clear();
    output.clear();
    q2.clear();

    //s.////errlog2("ScrollingText!! 3");
    boxlen = p->cmdr2->STLen;
    if ( boxlen-1 > (int)p->cmdr2->MString.size() ) {
        for (int i = 0; i != boxlen; i++) quotestr += " ";
    }

    //s.////errlog2("ScrollingText!! 4");
    xpos = p->cmdr2->Xcoord;
    ypos = p->cmdr2->Ycoord;

    //strcpy(midcolor,  p->cmdr2->HiString);
    //strcpy(sidecolor, p->cmdr2->LoString);
    midcolor   = p->cmdr2->HiString;
    sidecolor  = p->cmdr2->LoString;

    quotestr  += p->cmdr2->MString;
    loop2 = quotestr.size();
    quotestr  += quotestr.substr ( 0, boxlen-1 );

    // s.////errlog2("ScrollingText!! 5");
    // Loop Scrolling Text Here
    do {

        if (PAUSE_SCROLING) {
            usleep(12*11800);
            continue;
        }

        // Now Write Scrolling Menu Command Text
        output.clear();
        sprintf(outBuff, "\x1b[%i;%iH",ypos,xpos);
        output  += outBuff;
        if (loop > loop2) loop = 0;
        output  += sidecolor;
        q2 = quotestr.substr (loop, 2 );
        output  += q2;
        output  += midcolor;
        q2 = quotestr.substr (loop + 2, (boxlen - 4));
        output  += q2;
        output  += sidecolor;
        q2 = quotestr.substr (loop + (boxlen - 2), 2);
        output  += q2;
        ++loop;
        s.pipe2ansi((char*)output.c_str());
        // Give small delay to Slow Text Scrolling Down
        usleep(10*11800);
    } while (p->Active);
    //s.////errlog2("ScrollingText!! 6");
    delete p;
    p = 0;
    //s.////errlog2("ScrollingText!! 7");
    return NULL;
}


/**
 * Clear Menu When Done.
 */
void menu_func::menu_clear()
{
    // Clear
    ////errlog2(" *** menu_clearbars()");
    if (execnum)  delete [] execnum;
    if (execnum2) delete [] execnum2;
    if (execnum3) delete [] execnum3;
    execnum  = 0;
    execnum2 = 0;
    execnum3 = 0;

}


/**
 * Geenrates Menu from commands like Telegard / Renegade.
 * Instead of using an ANSI File.
 */
void menu_func::display_menu(BOOL forcelevel)
{
    BOOL ignore = FALSE;
    char text [1024]= {0};
    char text2[1024]= {0};

    if (forcelevel==FALSE) {
        if (menur2->ForceHelpLevel == 0) return;
    }

    std::string output = "|CS|07|16|CR";
    sprintf(text,"%s",(char *)menur2->MenuName.c_str() );
    cspacing(text);
    output += "|CR|CR|15|16";
    output += text;
    output += "|CR|CR|07|16";


    memset(&text,0,1024);
    ulong cnt=0;

    // Set this in menu.txt lateron, or an override.
    char desc_color[4]     = {"|07"};
    char cmd_color[4]      = {"|15"};
    char bracket_color[4]  = {"|08"};
    char bracket2_color[4] = {"|07"};

    strcpy(text, (char *)" %s%24s %s(%s%s%s)     " );

    for (long i = 0; i != noc; i++) {
        // Ignore Execute on First Load Commands
        if (cmdr2[i].CKeys == "FIRSTCMD") {
            ignore = TRUE;
        } else // Ignore Lightbar Commands
            if (cmdr2[i].LBarCmd) {
                ignore = TRUE;
            } else // Ignore Scrolling Text
                if (cmdr2[i].SText) {
                    ignore = TRUE;
                } else // Ignore Blank Keys
                    if (cmdr2[i].CKeys == "") {
                        ignore = TRUE;
                    } else // Ignore Stacked. Only Display First.
                        if (cmdr2[i].LDesc == "") {
                            ignore = TRUE;
                        }

        // Check Ignore Status to Write out.
        if (ignore) {
            ignore = FALSE;
        } else {
            // Start Displaying Menu Text
            // Menu Commands spacing
            sprintf(text2, text ,
                    desc_color,
                    (char *)cmdr2[i].LDesc.c_str(),
                    bracket_color,
                    cmd_color,
                    (char *)cmdr2[i].CKeys.c_str(),
                    bracket2_color);
//			errlog (text2);
            output += text2;
            ++cnt;

            // 3 Rows, Menus
            if (menur2->ForceHelpLevel == 1) {
                if (cnt == 2) {
                    // Goto Next Line, Reset.
                    cnt = 0;
                    output += "\r\n";
                }
            } else {
                // 1 Row, Matrix
                if (menur2->ForceHelpLevel == 2) {
                    // Goto Next Line, Reset.
                    cnt = 0;
                    output += "\r\n";
                }
            }
        }
    }

    if (output.size() > 2) {
        output += "|CR|CR";
        pipe2ansi((char *)output.c_str());
    }
    return;
}

/**
 * Used for Lightbars and Hotkey Menu's
 * This is also used in Full Screen Interfaces
 * for Lightbars slections and Return
 * PassThough commands from Menu Lightbar Prompts.
 */
void menu_func::menu_bars(char *inPut)
{
    std::vector< list_bar > lbar;
    std::string::size_type id1 = 0;

    //putline("make it to menu bars!!");
    ////errlog2("menu_bars() 1");

    // If no Menu Commands Return! / Add fallback menu lateron!!
    if (noc == 0) return;

    // Read Menu Ansi to String that will hold lightbars, Push to screen sametime
    // Will speed up display and make ghosting not apear as much
    ////errlog2("menu_bars() 2");
    BOOL menufile = FALSE;
    if (menur2->Directive != "") {
        //readinAnsi(menur2.Directive,output);
        menufile = ansiPrintf((char *)menur2->Directive.c_str());
    }

    ////errlog2("menu_bars() 3");
    // Count Cmds with lightbar or scrolling flag only and ESC Keys
    int Xc = 0;
    if (menur2->MFlags.EOSPrompt == TRUE) {
        ////errlog2("menu_bars() 3.1");
        getxy();
        Xc = ansi_getx();

        //pusprintf(outstring,"\x1b[u\x1b[K%s",(char *)sLine[iCnt].c_str());
        //pipe2ansi(outstring);
    }

    //printf("\ncur: %s, pre: %s",_curmenu,_premenu);
    // Not the Same Menu Loaded, ok to Load New One!
    ////errlog2("menu_bars() 5.1");
    if (_premenu != _curmenu) {
        //menu_clear();

        //if (!menur2)
        ////errlog2("menu_bars() 5.1 * !menur2");

        //if (!cmdr2)
        ////errlog2("menu_bars() 5.1 * !cmdr2");


        ////errlog2("menu_bars() 5.2 execnum");
        if (execnum != 0) {
            ////errlog2("menu_bars() delete execnum;");
            delete [] execnum;
            execnum = 0;
        }


        if (execnum == 0) {
            execnum = new short[noc+1];  // Set to # of comamnd for now, preload.
            if (!execnum) {
//                errlog((char *)"Unable to Allocate Memory for process_menu_area: %s - execnum[], returning...",(char *)_curmenu.c_str());
                return;
            }
        }
//        else errlog((char *)"Unable to Allocate Memory for process_menu_area: %s - execnum[] 1, returning...",(char *)_curmenu.c_str());


        // Check for Text Scrolling Commands
        ////errlog2("menu_bars() 5.3 execnum2");
        if (execnum2 != 0) {
            ////errlog2("menu_bars() delete execnum2;");
            delete [] execnum2;
            execnum2 = 0;
        }

        if (execnum2 == 0) {
            execnum2 = new short[noc+1]; // Set to # of comamnd for now, preload.
            if (!execnum2) {
//                errlog((char *)"Unable to Allocate Memory for process_menu_area: %s - execnum2[], returning...",(char *)_curmenu.c_str());
                return;
            }
        }
//        else errlog((char *)"Unable to Allocate Memory for process_menu_area: %s - execnum2[] 1, returning...",(char *)_curmenu.c_str());

        // Check for Text Scrolling Commands
        ////errlog2("menu_bars() 5.3 execnum3");
        if (execnum3 != 0) {
            ////errlog2("menu_bars() delete execnum3;");
            delete [] execnum3;
            execnum3 = 0;
        }

        if (execnum3 == 0) {
            execnum3 = new short[noc+1]; // Set to # of comamnd for now, preload.
            if (!execnum3) {
//                errlog((char *)"Unable to Allocate Memory for process_menu_area: %s - execnum3[], returning...",(char *)_curmenu.c_str());
                return;
            }
        }
//        else errlog((char *)"Unable to Allocate Memory for process_menu_area: %s - execnum3[] 1, returning...",(char *)_curmenu.c_str());

        iNoc = 0;
        sNoc = 0;
        exe  = 0;
        cntEscCmds = 0;

        for (int i = 0; i != noc; i++) {
            ////errlog2("menu_bars() 6");
            // If were overwridding placement of Row on which we display Lightbars
            // This is Mesage prompts and variables lenghts.
            // Place All Lightbars Accross on Same Horzontal Line!
            if (menur2->MFlags.EOSPrompt == TRUE) {
                ////errlog2("menu_bars() 6.1 - EOS");
                cmdr2[i].Ycoord = Xc;
                /// This needs to be fixed, defaulting to TRUE!!!
            }

            // If we find a FIRSTCMD, Execute it right away!
            if (cmdr2[i].CKeys == "FIRSTCMD") {
                ////errlog2("menu_bars() 6.2 - FIRSTCMD");
                menu_docmd(&cmdr2[i]);
                // Count /Commands, if we execute all FIRSTCMDS then return when completed.
                ++exe;
            }

            // Get Ligthbar Commands
            if (cmdr2[i].LBarCmd && isANSI == TRUE) {
                ////errlog2("menu_bars() 6.3 - Lightbar");
                execnum[iNoc] = i;
                ++iNoc;
            }

            if (cmdr2[i].SText) {
                ////errlog2("menu_bars() 6.4 - Scrolling");
                execnum2[sNoc] = i;
                ++sNoc;
            }

            //while running throguh each command, put into a vector formatted.
            // No ansi menu file, generate menu commands.
            if (!menufile && iNoc == 0) {
                //errlog((char *)"menu_bars() 6.4 - Setting Menu Command Layout");
                // Insert code here for generating generic menu with commands.
            }

            ////errlog2("menu_bars() 10");
            // Count Escape Keys in Menu System to override passthrough
            if (cmdr2[i].CKeys == "ESC") {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            } else if (cmdr2[i].CKeys == "LEFT") {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            } else if (cmdr2[i].CKeys == "RIGHT") {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            } else if (cmdr2[i].CKeys == "UP") {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            } else if (cmdr2[i].CKeys == "DOWN") {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }
            /*
            else
            if (cmdr2[i].CKeys == "HOME")
            {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else
            if (cmdr2[i].CKeys == "END")
            {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }*/

            else if (cmdr2[i].CKeys == "PAGEUP") {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            } else if (cmdr2[i].CKeys == "PAGEDN") {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }
        }

        ////errlog2("menu_bars() 11");
        // Return if we executed all comamnds as FRISTCMD!
        if (exe == noc) {
            return;
        }

        ////errlog2("menu_bars() 11.1");
        // Make sure there is more then 1 to Randomize
        if (sNoc > 0) {
            srand(time(NULL));
            sRand = rand()%sNoc;
        }


        _premenu = _curmenu;
    }

    ////errlog2("menu_bars() 13");
    // Start Text Scrolling Thread here...
    if (sNoc > 0 && execnum2[0] != '\0' && (int)execnum2[0] != 19 && isANSI == TRUE) {
        ////errlog2("menu_bars() 14");
        //tScroll = true;
        tScroll = true;
        p = new PASSING;
        p->Active = true;
        p->cmdr2 = &cmdr2[execnum2[sRand]];
        pthread_create(&ahThread, NULL, ScrollingText,(void *)p);
    }

    ////errlog2("menu_bars() 15");
    // Setup of first Command, Highlited menu lightbar
    output.clear();

    ////errlog2("menu_bars() 16");
    //Throw out a menu prompt if there is one available. - Add MCI Parsing too!
    // Use Same X Row,  these got reversed somehow!!! grrrr
    if (iNoc == 0) {
        ////errlog2("menu_bars() 16- MENU PROMPT 1");

// MENU PROMPT NOT NEEDED HERE, IS BELOW WITH MENU DISPLAY
//        sprintf(outBuff,"%s",(char *)menur2->MenuPrompt.c_str());
//        output += outBuff;
    } else {
        ////errlog2("menu_bars() 16- MENU PROMPT 2");
        //Throw out a menu prompt if there is one available. - Add MCI Parsing too!
        // Use Same X Row,  these got reversed somehow!!! grrrr
        xx = 1;
        yy = cmdr2[execnum[0]].Ycoord;
        //     sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)menur2->MenuPrompt.c_str());
//   //     output += outBuff;
    }



    /*
        // Lightbar menu then draw
        if (iNoc > 0) {
        xx = cmdr2[execnum[0]].Xcoord; yy = cmdr2[execnum[0]].Ycoord;
        sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,cmdr2[execnum[0]].HiString);
        output += outBuff;

        // Setup of Remaining Lightbars in Low highlight Form
        for (int rep = 1; rep != iNoc; rep++) {
            xx = cmdr2[execnum[rep]].Xcoord; yy = cmdr2[execnum[rep]].Ycoord;
            sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,cmdr2[execnum[rep]].LoString);
            output += outBuff;
        }

        // Write out all the Lightbars * and reset cursor position.
        //output += "\x1b[24;79H";
        sprintf(outBuff,"\x1b[%i;79H",yy);
        output += outBuff;

        pipe2ansi((char*)output.c_str());
        }
        // Not Lightbars, print out menu ansi.
        else {
            pipe2ansi((char*)output.c_str());
        }
    */
    ////errlog2("menu_bars() 17");

    // For Title Scan and Other Interfaces, newscan doesn't
    // Always start on first message, just to

//	errlog((char *)"#### choice1: %lu = CURRENT_AREA: %lu;",choice, CURRENT_AREA);
    //choice = CURRENT_AREA;
//	errlog((char *)"#### choice2: %lu = CURRENT_AREA: %lu;",choice, CURRENT_AREA);

    std::string sMenu = _curmenu;
    while (1) {
        ////errlog2("menu_bars() 18");
        // Lightbar menu then draw
        if (iNoc > 0) {
            ////errlog2("menu_bars() 19 - Lightbar Processing here!!");
            // Setup of Remaining Lightbars in Low highlight Form
            for (int rep = 0; rep != iNoc; rep++) {
                if (rep != choice) {
                    ////errlog2("menu_bars() 19.1");
                    xx = cmdr2[execnum[rep]].Xcoord;
                    yy = cmdr2[execnum[rep]].Ycoord;
                    ////errlog2("menu_bars() 19.2");
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[rep]].LoString.c_str());
                    ////errlog2("menu_bars() 19.3");
                    output += outBuff;
                } else {
                    ////errlog2("menu_bars() 19.4");
                    xx = cmdr2[execnum[rep]].Xcoord;
                    yy = cmdr2[execnum[rep]].Ycoord;
                    ////errlog2("menu_bars() 19.5");
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[rep]].HiString.c_str());
                    ////errlog2("menu_bars() 19.6");
                    output += outBuff;
                }
            }

            // Write out all the Lightbars * and reset cursor position.
            //output += "\x1b[24;79H";
            sprintf(outBuff,"\x1b[%i;79H",yy);
            output += outBuff;

            display_menu();
            pipe2ansi((char*)output.c_str());
        }
        // Not Lightbars, print out menu ansi.
        else {
            ////errlog2("menu_bars() 19 - NO Lightbar Processing");
            pipe2ansi((char*)output.c_str());

            // Write out commands.  - Check for Helplevel!!
            display_menu();
        }

        ////errlog2("menu_bars() 20 - GETKEY!");
        memset(&EscapeKey,0,sizeof(EscapeKey));


        //Can display global prompt here!
        output.clear();

        //Replcae Messages Left...
        output.clear();
        output = menur2->MenuPrompt;

        // If lightbar prompt, reset to begining of line.
        if (iNoc > 0) {
            xx = 1;
            sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)menur2->MenuPrompt.c_str());
            output = outBuff;
        }

        if (output.size() > 1) {
            sprintf(outBuff,"%ld",MsgsLeft);
            id1 = output.find("%ML",0);
            if (id1 != std::string::npos) {
                output.replace(id1,3,outBuff);
            }
            sprintf(outBuff,"%ld",CurMsgs);
            id1 = output.find("%CM",0);
            if (id1 != std::string::npos) {
                output.replace(id1,3,outBuff);
            }
            sprintf(outBuff,"%ld",TotMsgs);
            id1 = output.find("%HM",0);
            if (id1 != std::string::npos) {
                output.replace(id1,3,outBuff);
            }
            pipe2ansi((char *)output.c_str());
        }


        // Main System / Menu System input.
        c = getkey(true);

        ////errlog2("menu_bars() 21 - GOT KEY NOW PROCESS!");
        // Check here for Arrow Key / Escaped Input was Received
        if ((int)c == 27) {
            cc = EscapeKey[1];

            if (cc == '0') {
                // Tranlate to Terminal Escape with Preceding 0
                cc = EscapeKey[2];
            }

            EscHit = true;
        } else {
            EscHit = false;
            /*
            // Only display alpha Numeric
            if( (int)c >= 33 && (int)c <= 127) {
            	putline((char *)&c);
            }*/
        }

        ////errlog2("menu_bars() 22");
        output.clear();

        // If were in prompt and receive Int/Digit,
        // pass through Digits back to Message Quoter && Title Scan Etc...
        if (isdigit(c)) {
            if (menur2->MFlags.DigitPassing == TRUE) {
                inPut[0] = '#';
                inPut[1] = c;
                //menu_clear();
                return;
            }
        }

        ////errlog2("menu_bars() 23 .0 ");
        executed  = 0;
        if (EscHit) {
            // Input Key is Escaped Meaning Arrow Keys
            ////errlog2("menu_bars() 23.1");

            //elog ("* ESC Hit: %c, cntEscCmds: %ld",cc,cntEscCmds);

            // Run through and check for any Menu commands that overive default passthrough!
            if (cntEscCmds > 0) {
                // ESC Commands in Menu might overide lightbars.
                for (int ckey = 0; ckey != (signed)cntEscCmds; ckey++) {

                    if (EscapeKey[1] == '\0' || EscapeKey[1] == ' ') {
                        if (cmdr2[execnum3[ckey]].CKeys == "ESC") {
                            //elog("- ESC EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    } else if (cc == 'A') {
                        if (cmdr2[execnum3[ckey]].CKeys == "UP") {
                            ////errlog2("- UP EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    } else if (cc == 'B') {
                        if (cmdr2[execnum3[ckey]].CKeys == "DOWN") {
                            ////errlog2("- DOWN EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    } else if (cc == 'C') {
                        if (cmdr2[execnum3[ckey]].CKeys == "RIGHT") {
                            //elog("- RIGHT EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    } else if (cc == 'D') {
                        if (cmdr2[execnum3[ckey]].CKeys == "LEFT") {
                            //elog("- LEFT EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }

                    else if ((cc == '5' && EscapeKey[2] == '~') || cc == 'V') {
                        if (cmdr2[execnum3[ckey]].CKeys == "PAGEUP") {
                            //elog("- LEFT EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }

                    else if ((cc == '6' && EscapeKey[2] == '~') || cc == 'U') {
                        if (cmdr2[execnum3[ckey]].CKeys == "PAGEDN") {
                            //elog("- LEFT EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }


                    // Now check for extended arrors from terminals with preceeding 0.
                    /*
                    else if (cc == '0')
                    {
                    	if (EscapeKey[2] == '\0' || EscapeKey[2] == ' ')
                        {
                            if (cmdr2[execnum3[ckey]].CKeys == "ESC")
                            {
                                //elog("- ESC EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                                if (tScroll) p->Active = false;
                                ++executed;
                                menu_docmd(&cmdr2[execnum3[ckey]]);
                                strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                            }
                        }
                        if (EscapeKey[2] == 'A')
                        {
                            if (cmdr2[execnum3[ckey]].CKeys == "UP")
                            {
                                ////errlog2("- UP EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                                if (tScroll) p->Active = false;
                                ++executed;
                                menu_docmd(&cmdr2[execnum3[ckey]]);
                                strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                            }
                        }
                        else if (EscapeKey[2] == 'B')
                        {
                            if (cmdr2[execnum3[ckey]].CKeys == "DOWN")
                            {
                                ////errlog2("- DOWN EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                                if (tScroll) p->Active = false;
                                ++executed;
                                menu_docmd(&cmdr2[execnum3[ckey]]);
                                strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                            }
                        }
                        else if (EscapeKey[2] == 'C')
                        {
                            if (cmdr2[execnum3[ckey]].CKeys == "RIGHT")
                            {
                                //elog("- RIGHT EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                                if (tScroll) p->Active = false;
                                ++executed;
                                menu_docmd(&cmdr2[execnum3[ckey]]);
                                strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                            }
                        }
                        else if (EscapeKey[2] == 'D')
                        {
                            if (cmdr2[execnum3[ckey]].CKeys == "LEFT")
                            {
                                //elog("- LEFT EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                                if (tScroll) p->Active = false;
                                ++executed;
                                menu_docmd(&cmdr2[execnum3[ckey]]);
                                strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                            }
                        }*/
                }
                // Executed == 0, Then Key Pressed was not valid! :)
                // Pass through the ESC then
                if (tScroll) p->Active = false;
                if (executed == 0) {
                    inPut[0] = cc;

                    // Translater to Terminal ESC Keys
                    if (cc == '0')
                        cc = EscapeKey[2];
                } else {
                    ////errlog2("menu_bars() 23.2 return");
                    //menu_clear();
                    return;
                }
            }

            // If True, means we pass A and B up/dn through to system!
            if (menur2->MFlags.EscPassing == FALSE) {
                // Arrow Keys
                ////errlog2("menu_bars() 23.3 - ESC PASSING!!!");
                if (cc == 'A') cc = 'D';
                else if (cc == 'B') cc = 'C';
            }
            //elog ("Start Lightbar Processing... CMD: %c, cntEscCmds: %ld", cc, cntEscCmds);

            if (iNoc > 0) {
                ////errlog2("menu_bars() 23.4 - iNoc > 0");
                if (cc == 'D' && executed == 0) {
                    // Up & Left
                    ////errlog2("menu_bars() 23.4 - D");
                    //printf ("\n D!");

                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;

                    if (choice == 0) choice = iNoc-1;
                    else --choice;

                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[%i;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str(),yy);
                    output += outBuff;
                    // Send Lightbar output
                    pipe2ansi((char*)output.c_str());
                } else if (cc == 'C' && executed == 0) {
                    // Down & Right
                    ////errlog2("menu_bars() 23.4 - C");
                    //printf ("\n C!");
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;

                    if (choice == iNoc-1) choice = 0;
                    else ++choice;

                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[%i;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str(),yy);
                    output += outBuff;
                    // Send Lightbar output
                    //errlog2((char*)output.c_str());
                    pipe2ansi((char*)output.c_str());
                } else if ((cc == 'H' || (cc == '1' && EscapeKey[2] == '~')) && executed == 0 &&
                           menur2->MFlags.EscPassing == TRUE) {
                    // Home Key
                    ////errlog2("menu_bars() 23.4 - H / 1");
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;
                    choice = 0;
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[%i;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str(),yy);
                    output += outBuff;
                    // Send Lightbar output
                    pipe2ansi((char*)output.c_str());
                } else if ((cc == 'K' || cc == 'F' || (cc == '4' && EscapeKey[2] == '~')) && executed == 0 &&
                           menur2->MFlags.EscPassing == TRUE) {
                    ////errlog2("menu_bars() 23.4 - 4 / K");
                    // || menur2.MFlags.EndHomeLightbar == TRUE) { // End Key
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;
                    choice = iNoc-1;
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[%i;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str(),yy);
                    output += outBuff;
                    // Send Lightbar output
                    pipe2ansi((char*)output.c_str());
                } else {
                    ////errlog2("menu_bars() 23.5 - ELSE");

                    // printf ("\n ELSE!");

                    // If we have Escape, check if menu command, otherwise pass it through!
                    // Lateron Add SAPCE and TAB
                    /*
                    if ((cc == '\0' || cc == ' ') && executed == 0) {
                        executed = 0;
                        for (int ckey = 0; ckey != noc; ckey++) {
                            if (strcmp(cmdr2[ckey].CKeys,"ESC") == 0) {
                                if (tScroll) p->Active = false;
                                ++executed;
                                menu_docmd(cmdr2[ckey].CmdKeys,cmdr2[ckey].MString);
                                strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys);
                            }
                        }
                        // Executed == 0, Then Key Pressed was not valid! :)
                        // Pass through the ESC then
                        if (tScroll) p->Active = false;
                        if (executed == 0) inPut[0] = cc;
                        return;
                    }
                    else {*/
                    // Pass through the Key.
                    if (tScroll) p->Active = false;
                    inPut[0] = cc;
                    //menu_clear();
                    return;
                    //}
                }
            } //</iNoc>
            else {
                // normal Key Input.
                ////errlog2("menu_bars() 23.6 - ELSE");
                if ((int)c == 10) {
                    executed = 0;
                    // Here Loop through and execute stacked Commands
                    for (int ckey = 0; ckey != noc; ckey++) {
                        if ( cmdr2[ckey].CKeys == "ENTER" ) {
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[ckey]);
                            strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                        } else if ( cmdr2[ckey].CKeys == cmdr2[execnum[choice]].CKeys ) {
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[ckey]);
                            strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                        }
                    }

                    if (executed > 0) {
                        //menu_clear();
                        return;
                    } else {
                        // Redraw screen!
                        if (menur2->Directive != "") {
                            //readinAnsi(menur2.Directive,output);
                            ansiPrintf((char *)menur2->Directive.c_str());
                        }
                    }
                }
                //  Else go through and run stack commands on hot key pressed.
                else {
                    executed = 0;     // Normal Key Inputed, if Match's
                    std::string t1;
                    for (int ckey = 0; ckey != noc; ckey++) {
                        // Loop and Run Stacked Commands.
                        if (c == 32) {
                            // Else check if it's a spacebar
                            if (cmdr2[ckey].CKeys == "SPACE") {
                                if (tScroll) p->Active = false;
                                ++executed;
                                menu_docmd(&cmdr2[ckey]);
                                strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                            }
                        } else // Check any remaining Keys Hot Key Input.
                            if (cmdr2[ckey].CKeys[0] == toupper(c) && cmdr2[ckey].CKeys.size() < 2) {
                                if (tScroll) p->Active = false;
                                ++executed;
                                menu_docmd(&cmdr2[ckey]);
                                strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                            }
                    }
                }
                // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
                if (executed > 0) {
                    //menu_clear();
                    return;
                }
            } // </</iNoc>
        } // </EscHit>

        // Normal Key Input
        else {
            // If Enter, Return Cmd # of Lightbar Executed
            // also catch any stacked keys and execute in order!

            if ((int)c == 10 && iNoc > 0) {
                // Only Executte [ENTER] on Lightbars!
                executed = 0;
                // Here Loop through and execute stacked Commands
                for (int ckey = 0; ckey != noc; ckey++) {
                    if (cmdr2[ckey].CKeys == cmdr2[execnum[choice]].CKeys) {
                        if (tScroll) p->Active = false;
                        ++executed;
                        menu_docmd(&cmdr2[ckey]);
                        strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                    }
                }
                if (executed > 0) {
                    //menu_clear();
                    ////errlog2("Excuted Return Passthrough or Normal Key!");
                    return;
                } else {
                    // Redraw screen!
                    if (menur2->Directive != "") {
                        //readinAnsi(menur2.Directive,output);
                        ansiPrintf((char *)menur2->Directive.c_str());
                    }
                }
            } else if ((int)c == 10 && iNoc == 0) {
                // Check for ENTER CKEY!
                executed = 0;
                // Here Loop through and execute stacked Commands
                for (int ckey = 0; ckey != noc; ckey++) {
                    if (cmdr2[ckey].CKeys == "ENTER") {
                        if (tScroll) p->Active = false;
                        ++executed;
                        menu_docmd(&cmdr2[ckey]);
                        strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                    }
                }
                if (executed > 0) {
                    //menu_clear();
                    return;
                } else {
                    // Redraw screen!
                    if (menur2->Directive != "") {
                        //readinAnsi(menur2.Directive,output);
                        ansiPrintf((char *)menur2->Directive.c_str());
                    }
                }
            }
            //  Else go through and run stack commands on hot key pressed.
            else {
                executed = 0;      // Normal Key Inputed, if Match's
                std::string t1;
                for (int ckey = 0; ckey != noc; ckey++) {
                    // Loop and Run Stacked Commands.
                    if (c == 32) {
                        // Else check if it's a spacebar
                        if (cmdr2[ckey].CKeys == "SPACE") {
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[ckey]);
                            strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                        }
                    } else // Check any remaining Keys Hot Key Input.
                        if (cmdr2[ckey].CKeys[0] == toupper(c) && cmdr2[ckey].CKeys.size() < 2) {
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[ckey]);
                            strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                        }
                }
                // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
                if (executed > 0) {
                    //menu_clear();
                    return;
                }
            }
        } // </EscHit> After Else
        ////errlog2("menu_bars() 24.5 - While NEXT!");
    } // </while (1)>
}

/**
 * Main Entry Point for a Menu and Startup.
 */
void menu_func::menu_proc(char *mString, uint32_t area)
{
    CURRENT_AREA = area;
    strcpy(mString,"");
    ////errlog2("menu_proc1 noc(%i)",noc);
    if (noc == 0) {
        // Don't reallu need to exit the system here..  fix lateron.. :)
        ////errlog2("Err: menu_proc registering noc(%i) for ()",noc,(char *)_curmenu.c_str());
        //putline((char *)"Menu (%s) is Registering - 0 commands, please fix this!",(char *)_curmenu.c_str());
        menu_reset();
        startpause();
        System_Alive = FALSE;
        return;
    }
    menu_bars(mString);
}

/**
 * Command Execute - Login System (Matrix)
 */
int menu_func::logon_system(unsigned char c)
{
    logon lgn;
    static int logins;
    switch (c) {
    case 'L' :
        if (lgn.logon_system(urec)) {
            // User Login Successful, Setup Login Defaults!
            _curmenu = (char *)urec->startmenu;
            CURRENT_MAREA    = 0;//urec->lastmbarea;
            CURRENT_FAREA    = 0;//1; // 0 Sysop Base!
            CURRENT_MSGTITLE = 0;//1; // 0 Sysop Base!
            return 1;
        } else {
            // User Login Failed
            ++logins;
            if (logins >= 3) return 2;
            return 0;
        }
        break;

    case 'A' :
        lgn.application();
        return 0;
        break;

    case 'P' :
        lgn.forgot_password();
        return 0;
        break;

    case 'E' :
        lgn.userinfo(urec,(char *)"userinfo");
        return 0;
        break;

    default  :
        break;
    }
    return 0;
}

/**
 * Command Execute - Start Message Editor (Edit Bases)
 */
void menu_func::msgedit_system()
{
    msg_edit _medit;
    _medit.mbeditmenu();
}


/**
 * Command Execute - file System Editor
 */
void menu_func::fileedit_system()
{
    //  file_edit _fedit;
    //  _fedit.fbeditmenu();
}


/*
// Start General Areas Message Reader, Email Scan throguh this needs work!!
int menu_func::msgread_system(unsigned char c, char *mString) {

    errlog((char *)"msgread_system() 1");
    //printf ("c = %c, mString = %s",c, mString);
    // Handle to Message Reader Class
    msg_read _mread;
    int tmpMbase = 0;
	int ret = TRUE;

	CURRENT_MAREA = urec->lastmbarea;
	tmpMbase = 0;  // Not used yet.

	//urec->lastmbarea = CURRENT_MAREA;
	//tmpMbase = CURRENT_MAREA;


	errlog((char *)" *** CURRENT_MAREA %lu = urec->lastmbarea %lu tmpMbase %lu",CURRENT_MAREA,urec->lastmbarea,tmpMbase);
    //CURRENT_MAREA = urec->lastmbarea;


	// If were in email area, skip to email/netmail reader.
//	if (CURRENT_MAREA == 0) { // Fix later to detect board type, email/netmail.

		switch(c) {
        	case 'R':
				// After reading, return, not skipping to next area.
				 errlog((char *)"emailread_system case 'R'");
				ret = emailread_system(c,mString);
				return ret;
				break;

			case 'N':
				errlog((char *)"emailread_system case 'N'");
				// After newscanning, move on to normal areas.
				ret = emailread_system(c,mString);
				++CURRENT_MAREA;
				++urec->lastmbarea;
				break;

			case 'M' :
				errlog((char *)"emailread_system case 'M'");
				ret = emailread_system((unsigned char)'N',mString);
				++CURRENT_MAREA;
				++urec->lastmbarea;
				break;

		}
//	}


	else {

		// Setup
	    _mread.start(urec);

		switch(c) {
		    case 'R':
		        // Start Normal Message Reading / Get Starting Msg #
		        ////errlog2("msgread_system() 1.1 R");
		        ret = _mread.StartReader(FALSE);
		        break;

		    case 'N':
		        ////errlog2("msgread_system() 1.2 N");
		        // Current area newscan
		        ret = _mread.StartReader(TRUE);
		        break;

		    case 'M': // Multi-Scan, Scan all Areas!!
		        ////errlog2("msgread_system() 1.3 M");
		        // Current area newscan
		        ret = _mread.StartReader(TRUE,TRUE);
		        break;

		}
	//}
    //urec->lastmbarea = tmpMbase;
    //CURRENT_MAREA = tmpMbase;
    return ret;
}
*/

/* Left Off commented out Origarea = urec->lastmbarea; so that each base selected
  would current newscan proeprly.  Problem with global newscan, starting at current
 not first area, but other problem is email being first.. !!

 */

/*
// Start Email TitleScan / Message Reader - Seems good For Emails.
int menu_func::emailread_system(unsigned char c, char *mString) {


    errlog((char *)"emailread_system 1");

    // Handle to Message Reader Class
    msg_read _mread;
    unsigned long Origarea;
    int SysopScan = FALSE;

    ////errlog2("emailread_system 2");
    // Lets Sysop / Admin Read All Email in the System.
    if (strcmp(mString,"sysop") == 0) {
        SysopScan = TRUE;
    }

    ////errlog2("emailread_system 3");
    Origarea = urec->lastmbarea;

    ////errlog2("emailread_system 4");

	if (atoi(mString) != 0) {
        urec->lastmbarea = atoi(mString);
    }
    else {
        urec->lastmbarea = 0;
    }

//    CURRENT_MAREA = urec->lastmbarea;

    _mread.start(urec); // Check!
    int ret = TRUE;

    ////errlog2("emailread_system 5");
    switch(c) {
        case 'R':
			 errlog((char *)"emailread_system case 'R'");
            ////errlog2("emailread_system 6.1");
            // Start Normal Message Reading / Get Starting Msg #
            //_mread.StartReader(FALSE,FALSE);//,TRUE);
            //ret = _mread.StartEmailReader(FALSE,SysopScan);
			ret = _mread.title_scan(FALSE,SysopScan);
            break;

        case 'N':
   		    errlog((char *)"emailread_system case 'N'");
            // Newscan
            //ret = _mread.StartReader(TRUE,FALSE);//,TRUE);
            ////errlog2("emailread_system 6.2");
//            ret = _mread.StartEmailReader(TRUE);
			ret = _mread.title_scan(TRUE,SysopScan);
            break;
    }

    ////errlog2("emailread_system 7");
    // Reset back to original.
 //   urec->lastmbarea = Origarea;
 //   CURRENT_MAREA = Origarea;
    return ret;
}

*/

//

/**
 * Command Execute - For Reading / Scanning all and new messages.
 */
int menu_func::msgscan_system(unsigned char c, char *mString)
{
    msg_read _mread;
    unsigned long Origarea = 0;

    Origarea = urec->lastmbarea;

    // Always set starting message to 0.
    // Gets reset to last read on newscan.
    urec->lastmsg = 0;

    _mread.start(urec); // Check!
    int ret = TRUE;
    int MiltiScan = FALSE;
    int NewScan = FALSE;

    // TESTING OUTPUT
    //pipe2ansi((char *)"|CR|15mstring: ");
    //pipe2ansi((char *)mString);
    //pipe2ansi((char *)"|CR|PA");


    switch(c) {
    case 'R': // Read Area
//			 errlog((char *)"msgscan_system case 'R'");
        ////errlog2("emailread_system 6.1");
        // Start Normal Message Reading / Get Starting Msg #
        //_mread.StartReader(FALSE,FALSE);
        //ret = _mread.StartEmailReader(FALSE,SysopScan);
        ret = _mread.title_scan(NewScan,MiltiScan,mString);
        break;

    case 'N': // NewScan Current Area
//   		    errlog((char *)"msgscan_system case 'N'");
        // Newscan Current
        //ret = _mread.StartReader(TRUE,FALSE);//,TRUE);
        NewScan = TRUE;
        ret = _mread.title_scan(NewScan,MiltiScan,mString);
        break;

    case 'M': // NewScan All Areas
//   		    errlog((char *)"msgscan_system case 'M'");
        ////errlog2("msgread_system() 1.3 M");
        // Current area newscan
        //ret = _mread.StartReader(TRUE,TRUE);
        NewScan = TRUE;
        MiltiScan = TRUE;
        ret = _mread.title_scan(NewScan,MiltiScan,mString);
        break;
    }

    //if (MiltiScan == TRUE)
    //    urec->lastmbarea = Origarea;

    urec->lastmbarea = Origarea;
    CURRENT_MAREA = Origarea;

    // Always reset last message to 0.
    urec->lastmsg = 0;
    return ret;
}

/*
// Email Startup
void menu_func::msgemail_system()
{
    msg_read _mread;
    _mread.start(urec);
    CURRENT_MAREA = urec->lastmbarea;
    _mread.DoPostEmail(FALSE);
}
*/


/**
 * Command Execute - Post a Message in current Area
 */
void menu_func::msgpost_system(char *mString)
{
    msg_read _mread;
    _mread.start(urec);

    unsigned long Origarea = 0;
    Origarea = urec->lastmbarea;

    std::stringstream iconvert(mString);   // In
    int i = 0;
    // Convert String to Int
    if (iconvert >> i) {
        urec->lastmbarea = i;
        CURRENT_MAREA = i;
    } else {
        CURRENT_MAREA = urec->lastmbarea;
    }

    mb_list_rec mr;
    memset(&mr,0,sizeof(mb_list_rec));
    msgs _msgf;
    _msgf.read_mbaselist(&mr, CURRENT_MAREA);

    if (mr.Pubpriv == PRIVATE) {
        _mread.DoPostEmail(FALSE);
    } else {
        _mread.SetupMsgPost(); // Freezing On Return with Abort.
    }

    // Reset to Original Area if we forced changed by menu command.
    CURRENT_MAREA = Origarea;
    urec->lastmbarea = Origarea;
}

// Pack Message Areas (WIP)
/*
void menu_func::msgpack_system(int all) {

    msg_pack _mpack;
    // Before Language String
    if (all == TRUE) {
        _mpack.PackAllAreas();
    }
    else {
        _mpack.PackCurrentArea(urec->lastmbarea);
    }

    // After Language String
    pipe2ansi((char *)"|CR|04completed. |07");
    getkey(true);  // Pause
}

// Pack Message Areas
void menu_func::msgtrunc_system(int all) {

    msg_pack _mpack;
    // Before Language String
    int MaxMsgs = 200;

    if (all == TRUE) {
        _mpack.TruncAllAreas();
    }
    else {
        _mpack.TruncCurrentArea(urec->lastmbarea,MaxMsgs);
    }

    // After Language String
    pipe2ansi((char *)"|CR|04completed. |07");
    getkey(true);  // Pause
}
*/


/**
 * Command Execute - Change Message Area
 */
void menu_func::msgarea_change(int change)
{
    int total = msg_count();

    if (total > 0)
        --total;
    switch(change) {
    case 1:
        if((signed)urec->lastmbarea < total) ++urec->lastmbarea;
        break;

    case 2:
        if((signed)urec->lastmbarea > 0) --urec->lastmbarea;
        break;
    }
    CURRENT_MAREA = urec->lastmbarea;
}


/**
 * Command Execute - Show Message Area Listing
 */
void menu_func::msgarea_list()
{
    //UserRec trec;
    //memcpy(&trec,urec,sizeof(UserRec));

    msg_area _marea;
    _marea.SetupList(urec);
    _marea.init_db();
    //_marea.StartList();

    // when exiting list, reset user area
    // to global that was selected in list.
    urec->lastmbarea = CURRENT_MAREA;
}


/**
 * Command Execute - User Listing
 */
int menu_func::usrlist_list()
{
    int idx;
    UserRec trec;
    memcpy(&trec,urec,sizeof(UserRec));

    usr_list _ulist;
    _ulist.SetupList(urec);
    idx = _ulist.StartList(FALSE);
    return idx;
}

/*
// Change Message Area
void menu_func::filearea_change(int change)
{
    int total = file_count();
    --total;

    if (urec->lastmbarea > total)
        urec->lastmbarea = CURRENT_FAREA;

    switch(change)
    {
    case 1:
        if(urec->lastmbarea < total) ++urec->lastmbarea;
        break;

    case 2:
        if(urec->lastmbarea > 1) --urec->lastmbarea;
        break;
    }
    CURRENT_FAREA = urec->lastmbarea;
}

// Set file Area List
void menu_func::filearea_list()
{
    UserRec trec;
    memcpy(&trec,urec,sizeof(UserRec));

    std::string tmp;
    ParseFArea(&trec, tmp);

    file_area _farea;
    _farea.SetupList(urec);
    _farea.StartList((char *)tmp.c_str());
}
*/


/**
 * Command Execute - Write Private Mail.
 */
void menu_func::write_email()
{
    UserRec u;
    char rBuffer[100];
    char text[100]= {0};
    int  len = sizeof(u.handle)-1;

    memset(&u,0,sizeof(UserRec));

    // Ask for Users Handle / Or User Number
    while (1) {
        strcpy(rBuffer,"");
        lang_get(text,42);
        inputfield(text,len);
        pipe2ansi(text);

        getline(rBuffer,len);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0) {
            // Check if already exists
            if(idx_match(rBuffer)) {
                strcpy((char *)u.handle, rBuffer);
                break;
            } else {
                // Incorrect Handle
                strcpy(rBuffer,"");
                lang_get(text,43);
                pipe2ansi(text);
            }
        }
        // List all users or Abort on blank!
        pipe2ansi((char *)"|15|17");
    }
}


/**
 * Command Execute - Read Private Mail.
 */
void menu_func::email_list()
{

    msg_email _email;
    _email.SetupList(urec);
    _email.StartList();

}

/**
 * Command Execute - Start External (Door/Script)
 */
void menu_func::start_external(char *mString)
{
    std::string path = NODEPATH;
    path.append(NODEDIR);
    path.append("/");

    char file[1024]= {0};

    FILE *fp;
    char buffer1[255] = {0}, buffer2[255] = {0}, *ptr;
    struct tm *time_p;
    int tmp = 0;

    // Setup Current Date etc,,
    time_t ctime;
    ctime = user->dtlaston;
    // Now Create Door.SYS
    time_p = localtime(&ctime);

    sprintf(file,"%sDORINFO%d.DEF",path.c_str(),NODE_NUM);
    if ((fp = fopen(file, "w")) == NULL)
        return;
    else {
        ptr = strrchr((char *)user->handle, ' ');
        if (ptr == NULL) {
            strcpy(buffer1, (char *)user->handle);
            strcpy(buffer2, "NLN");
        } else {
            memset(buffer1, 0, 26);
            strncpy(buffer1,(char *)user->handle,  (ptr - (char *)user->handle));
            strcpy(buffer2, (char *)user->handle + (ptr - (char *)user->handle) + 1);
        }

        for (tmp = 0; tmp < 26; tmp++) {
            /* names have to be UPPERCASE */
            buffer1[tmp] = toupper(buffer1[tmp]);
            buffer2[tmp] = toupper(buffer2[tmp]);
        }
        fprintf(fp, "%s\r\n", SYSTEM_NAME);
        fprintf(fp, "%s\r\n", SYSOP_NAME);
        fprintf(fp, "%s\r\n", "");
        fprintf(fp, "COM1\r\n");
        fprintf(fp, "38400 BAUD,8,N,1\r\n");
        fprintf(fp, " 0\r\n");
        fprintf(fp, "%s\r\n", buffer1);
        fprintf(fp, "%s\r\n", buffer2);

        /*
        if (user.user_zipcity[0] == 0)
            fprintf(fp, "No City Supplied\r\n");
        else
            fprintf(fp, "%s\r\n", "n/a");
        */
        fprintf(fp, "No City Supplied\r\n");

        /*
        if (ansi == 1)
            fprintf(fp, "1\r\n");
        else
            fprintf(fp, "0\r\n");
        */
        fprintf(fp, "1\r\n");


        if (isSysop == TRUE)
            fprintf(fp, "%d\r\n", 255);
        else
            fprintf(fp, "%d\r\n", 100);

        fprintf(fp, "%d\r\n", 256);
        fprintf(fp, "-1\r\n");
        fclose(fp);
    }

    // Outdated DOOR.SYS Format Though from DD
    sprintf(file,"%sDOOR.SYS",path.c_str());
    //elog2(file);

    if ((fp = fopen(file, "w")) == NULL)
        return;
    else {
        strftime(buffer1, 26, "%m/%d/%y", time_p);

        fprintf(fp, "COM1:\r\n");
        fprintf(fp, "38400\r\n");
        fprintf(fp, "8\r\n");
        fprintf(fp, "%d\r\n", NODE_NUM);
        fprintf(fp, "19200\r\n");
        fprintf(fp, "Y\r\n");
        fprintf(fp, "Y\r\n");
        fprintf(fp, "Y\r\n");
        fprintf(fp, "Y\r\n");
        fprintf(fp, "%s\r\n", user->handle);

        /*
        if (user.user_zipcity[0] == 0)
            fprintf(fp, "No City Supplied\r\n");
        else
            fprintf(fp, "%s\r\n", user.user_zipcity);
        if (user.user_voicephone[0] == 0) {
            fprintf(fp, "No Phone Supplied\r\n");
            fprintf(fp, "No Phone Supplied\r\n");
        } else {
            fprintf(fp, "%s\r\n", "n/a");
            fprintf(fp, "%s\r\n", "n/a");
        }
        */

        fprintf(fp, "Not Supplied\r\n");
        fprintf(fp, "Not Supplied\r\n");
        fprintf(fp, "Not Supplied\r\n");

        fprintf(fp, "<encrypted>\r\n");

        if (isSysop == TRUE)
            fprintf(fp, "%d\r\n", 255);
        else
            fprintf(fp, "%d\r\n", 100);

        fprintf(fp, "%d\r\n", 1);
        fprintf(fp, "%s\r\n", buffer1);
        fprintf(fp, "%d\r\n", 256 * 60);
        fprintf(fp, "%d\r\n", 256);

        /*
        if (ansi == 1)
            fprintf(fp, "GR\r\n");
        else
            fprintf(fp, "NG\r\n");
        */
        fprintf(fp, "GR\r\n");

        fprintf(fp, "%d\r\n", TERM_HEIGHT); // Screen Length

        //fprintf(fp, "%c\r\n", (user.user_toggles & (1L << 4) ? 'Y' : 'N'));
        fprintf(fp, "Y\r\n");

        fprintf(fp, "1,2,3,4,6,7\r\n");
        fprintf(fp, "1\r\n");
        fprintf(fp, "01/01/99\r\n");
        fprintf(fp, "%lu\r\n", (ulong)user->num);
        fprintf(fp, "X\r\n");
        fprintf(fp, "%d\r\n", 0); //user.user_ulfiles);
        fprintf(fp, "%d\r\n", 0); //user.user_dlfiles);
        fprintf(fp, "0\r\n");
        fprintf(fp, "9999\r\n");

        fclose(fp);
    }

    sprintf(file,"%sCHAIN.TXT",path.c_str());
    //elog2(file);

    if ((fp = fopen(file, "w")) == NULL)
        return;
    else {
        // Chain.txt
        //1                                 User number
        fprintf(fp, "%lu\r\n", (ulong)user->num);
        //MRBILL                            User alias
        fprintf(fp, "%s\r\n", (char *)user->handle);
        //Bill                              User real name - We don't keep these!
        fprintf(fp, "%s\r\n", (char *)user->handle);
        //                                User callsign (HAM radio)
        fprintf(fp, "\r\n");
        //21                                User age
        fprintf(fp, "25\r\n"); // Default Age
        //M                                 User sex
        fprintf(fp, "%s\r\n", buffer1 );
        //16097.00                        User gold
        fprintf(fp, "16097.00\r\n");
        //05/19/89 User last logon date
        memset(&buffer1,0,sizeof(buffer1));
        strftime(buffer1, 26, "%m/%d/%y", time_p);
        fprintf(fp, "%s\r\n",buffer1);
        //80                                User colums
        fprintf(fp, "%i\r\n",TERM_WIDTH);
        //25                                User width
        fprintf(fp, "%i\r\n",TERM_HEIGHT);
        //255                               User security level (0-255)
        if (isSysop == TRUE) {
            fprintf(fp, "%d\r\n", 255);
            //1                                 1 if Co-SysOp, 0 if not
            //1                                 1 if SysOp, 0 if not
            //1                                 1 if ANSI, 0 if not
            //0                                 1 if at remote, 0 if local console
            fprintf(fp, "%d\r\n", 1);
            fprintf(fp, "%d\r\n", 1);
            fprintf(fp, "%d\r\n", 1);
            fprintf(fp, "%d\r\n", 1);
        } else {
            fprintf(fp, "%d\r\n", 50);
            //1                                 1 if Co-SysOp, 0 if not
            //1                                 1 if SysOp, 0 if not
            //1                                 1 if ANSI, 0 if not
            //0                                 1 if at remote, 0 if local console
            fprintf(fp, "%d\r\n", 0);
            fprintf(fp, "%d\r\n", 0);
            fprintf(fp, "%d\r\n", 1);
            fprintf(fp, "%d\r\n", 1);
        }

        //2225.78                        User number of seconds left till logoff
        fprintf(fp, "%d\r\n", 2225);
        //F:\WWIV\GFILES\                   System GFILES directory (gen. txt files)
        fprintf(fp, "%s\r\n", ANSIPATH);
        //F:\WWIV\DATA\                     System DATA directory
        fprintf(fp, "%s\r\n", DATAPATH);
        //890519.LOG                        System log of the day
        memset(&buffer1,0,sizeof(buffer1));
        strftime(buffer1, 26, "%y%m%d.log", time_p);
        fprintf(fp, "%s\r\n", buffer1);
        //2400                              User baud rate
        fprintf(fp, "%d\r\n", 19200);
        //2                                 System com port
        fprintf(fp, "%d\r\n", 0);
        //MrBill's Abode (the original)     System name
        //The incredible inedible MrBill    System SysOp
        fprintf(fp, "%s\r\n", SYSTEM_NAME);
        fprintf(fp, "%s\r\n", SYSOP_NAME);
        //83680                             Time user logged on/# of secs. from midn.
        fprintf(fp, "%d\r\n", 83680);
        fprintf(fp, "%d\r\n", 1);          //   User number of seconds on system so far
        //5050                              User number of uploaded k
        //22                                User number of uploads
        //42                                User amount of downloaded k
        //1                                 User number of downloads
        fprintf(fp, "%d\r\n", 0);
        fprintf(fp, "%d\r\n", 0);
        fprintf(fp, "%d\r\n", 0);
        fprintf(fp, "%d\r\n", 0);
        //8N1                               User parity
        fprintf(fp, "8N1\r\n");
        //2400                              Com port baud rate
        fprintf(fp, "%d\r\n", 19200);
        //7400                              WWIVnet node number
        fprintf(fp, "%d\r\n", 0);
        fclose(fp);
    }
    // Execute Exteral Command (form later to another process!!!

    // Test Setting the Terminal back to Original. before running door.
    tcflush( STDOUT_FILENO, TCIFLUSH);
    if ( tcsetattr( STDOUT_FILENO, TCSADRAIN,&old_termios ) ) {
        //return( 2 );
        pipe2ansi((char *)"	|CS Unable to set old_termios! |PA");
    }

    //pipe2ansi("|CS");
    //int iRet =
    system((char *)mString);
}

/**
 * Command Execute - Data Areas (Custom Menu System)
 * This should be incoperate into the main menu system
 */
void menu_func::da_system(char *mString)
{
    //elog ("Starting Data Area: %s",mString);
    data_area _da(urec);
    strcpy(_da._curarea,mString);
    _da.data_loop();
}


/**
 * Command Execute - Create User Auto Signature)
 */
void menu_func::msg_autosig()
{
    msg_read _mread;
    _mread.start(urec);
    _mread.SetupUserSig();
}


/**
 * Menu System - Run Menu Command
 */
void menu_func::menu_docmd(CommandRec *cmdr)
{
    unsigned char c1 = cmdr->CmdKeys[0];
    unsigned char c2 = cmdr->CmdKeys[1];

    int ret;

    ////errlog2("menu_docmd!! 2");
    std::string parse;
    char sNode[60] = {0};
    std::string::size_type id1;

    // elog("Do MenuCmd: %c, %c",CmdKey[0],CmdKey[1]);

    // _loadnew = false;
    ////errlog2("menu_docmd!! 3");
    switch (c1) {
        // Message Reader Return right away
    case '!' :

//			errlog((char *)"menu_docmd!! 3.2 - passthrough with !");
        // getting passed through
        break;

        // Data Area Menu Commands
    case 'D' :
        ////errlog2("menu_docmd!! 4");
        switch (c2) {
        case 'A' : // Execute This Data Area
            da_system((char *)cmdr->MString.c_str());
            _loadnew = true;
            break;

        case '-' : //Execute Program/Door

            // Parse the mString for a Node Numbare and replace
            // %NN = Node Number
            parse = cmdr->MString;
            while (1) {
                id1 = parse.find("%NN",0);
                if (id1 == std::string::npos) break;
                sprintf(sNode,"%i",NODE_NUM);
                parse.replace(id1,3,sNode);
            }

            // Now Parse Out Username/handle if passed.
            while (1) {
                id1 = parse.find("%UN",0);
                if (id1 == std::string::npos) break;
                sprintf(sNode,"\"%s\"",user->handle);
                parse.replace(id1,3,sNode);
            }

            start_external((char *)parse.c_str());
            // Flush when returning from doors
            fflush(stdout);
            open_keyboard();

            // pipe2ansi((char *)"|CS");
            break;

        case 'L' : //Execute Program Download
//                    elog2(mString);
            start_external((char *)cmdr->MString.c_str());
            // Flush when returning from doors
            fflush(stdout);
            open_keyboard();
            // pipe2ansi((char *)"|CR");
            // startpause();
            break;

        case 'P' : //Execute Python Script

# ifdef HAVE_PYTHON

            // Some terms needs terminal reset for proper display.
            // Ansi terminals are ok!
            pybbs_run((char *)cmdr->MString.c_str(), user);
            // Flush when returning from scripts.
            fflush(stdout);
            open_keyboard();

            //pipe2ansi((char *)"|CR|07Returned from script... |PA");
# endif
            break;

        default :
            break;
        }
        break;

        // Data Area Menu Commands
    case 'U' :
        ////errlog2("menu_docmd!! 5");
        switch (c2) {
        case 'L' : //Execute Program Download
            start_external((char *)cmdr->MString.c_str());
            break;

        default :
            break;
        }
        break;

        // Matrix Menu Commands
    case '*' :
        ////errlog2("menu_docmd!! 6");
        switch (c2) {
        case 'L' : // Login
            ret = logon_system(c2);
            if (ret == 2) System_Alive = FALSE;
            else if (ret == 1) {
                _loadnew = true;
                //_node.node_global_announce_login();
                lineolm_login(TRUE);
            }
            break;

        case 'A' : // Apply
            logon_system(c2);
            break;

        case 'G' : // Logoff
            System_Alive = FALSE;

            // Display Logoff Ansi here!
            ansiPrintf((char *)"logoff");
            term_option(0); // Put Echo Back on!  Cywing
            break;

        case 'P' : //Forgot Password
            logon_system(c2);
            break;

        case 'E' : // User Editor
            logon_system(c2);
            break;

        default  : // None Found!
            break;
        }
        break;

        // Message System Commands
    case 'M' :
        ////errlog2("menu_docmd!! 7");
        switch (c2) {
        case 'R' : // Message Reader
            msgscan_system(c2,(char *)cmdr->MString.c_str());
            _loadnew = true;
            break;

        case 'N' : // NewScan Current Area
            msgscan_system(c2,(char *)cmdr->MString.c_str());
            _loadnew = true;
            break;


        case 'M' : // MultiScan All Areas
            msgscan_system(c2,(char *)cmdr->MString.c_str());
            _loadnew = true;
            break;

        case 'P' : // Public and Private Post New Message.
            msgpost_system((char *)cmdr->MString.c_str());
            break;

        case 'A' : // Mesasge Area List
            msgarea_list();
            break;

        case 'U' : // User List
            usrlist_list();
            break;

        case '+' : // Next Message Area
            msgarea_change(1);
            break;

        case '-' : // Previous Message Area
            msgarea_change(2);
            break;

        case 'S' : // Autosig
            msg_autosig();
            break;

        default  : // None Found!
            break;
        }
        break;

        /*
            case 'F' :
                ////errlog2("menu_docmd!! 8");
                switch (c2)
                {
                case 'A' : // File Area List
        filearea_list();
        break;

                case '+' : // Next File Area
        filearea_change(1);
        break;

                case '-' : // Previous File Area
        filearea_change(2);
        break;

                case 'L' : // List Files
        //            fileidx_display(); // Testing only!
        break;
                }
                break;*/

        // Email
        /*
            case 'E' :
                ////errlog2("menu_docmd!! 9");
                switch (c2)
                {
                case 'W' : // Write Email
                    msgemail_system();
                    _loadnew = true;
                    break;
                case 'R' : // Email Reader
                    msgscan_system(c2,(char *)cmdr->MString.c_str());
                    _loadnew = true;
                    break;

                case 'N' : // Email NewScan
                    msgscan_system(c2,(char *)cmdr->MString.c_str());
                    _loadnew = true;
                    break;


                default:  // None Found!
                    break;
                }
                break;
        */
        // Sysop Commands
    case '%' :
        ////errlog2("menu_docmd!! 10");
        // Only Sysop can have these commands execute!
        if (isSysop == TRUE) {
            switch (c2) {
            case 'I' : // File Import
//                import_filedir();
                break;

            case '#' : // Menu Editor
                //menu_edit();
                _loadnew = true;
                break;

            case 'M' : // Message Forum Editor
                msgedit_system();
                break;

            case 'F' : // File Forum Editor
                fileedit_system();
                break;

            case 'C' : // Pack Current Mesasge Area
//                        msgpack_system(FALSE);
                break;

            case 'P' : // Pack All Mesasge Areas
//                        msgpack_system(TRUE);
                break;

            case 'T' : // Truncate All Message Areas
//                        msgtrunc_system(TRUE);
                break;

            case 'R' : // Truncate Current Area
//                        msgtrunc_system(FALSE);
                break;

            case 'E' : // Sysop Email Scan
                msgscan_system('R',(char *)"sysop");
                break;

            default  : // None Found!
                break;
            }
        }
        break;

    case '-' :
        ////errlog2("menu_docmd!! 11");
        switch (c2) {

        case '^' : // Change Menu
            //strcpy(_curmenu,mString);
            _curmenu.clear();
            _curmenu = cmdr->MString;
            _loadnew = true;
            choice = 0;
            break;

        case '/' : // Gosub Menu - Remember current Menu, then changes to new
            _gosub.clear();
            _gosub = _curmenu;
            _curmenu.clear();
            _curmenu = cmdr->MString;
            _loadnew = true;
            choice = 0;
            break;

        case '!' : // Return to Original Menu from GoSub Menu.
            if (_gosub == "") break;
            _curmenu.clear();
            _curmenu = _gosub;
            _loadnew = true;
            choice = 0;
            break;

        case 'T' : // Display a line of Text
            pipe2ansi((char *)cmdr->MString.c_str());
            cmdr->MString.clear();
            break;

        case 'F' : // Display a File
            ansiPrintf((char *)cmdr->MString.c_str());
            cmdr->MString.clear();
            break;

        case 'P' : // Pause
            startpause();
            break;

        case 'S' : // System Info
            ansiPrintf((char *)"sysinfo");
            break;

        default  : // None Found!
            break;
        }
        break;

        // Node
    case 'N' : // Node

        switch (c2) {
        case 'W' : // Whois Online.
            //_node.whoisonline();
            node_message(TRUE);
            break;
        case 'M' : // Node Messages - WIP not yet setup correctly
            //lineolm(1, "hi, testing this a test message");
            // Lists and sends a node mode per selection.
            node_message();
            break;
        default :
            break;
        }
        break;

        // None Found!
    default  :
        break;
    }
}

/**
 * Menu System - Session Startup, Pass User Record
 */
void menu_func::menu_setuser(UserRec *user)
{
    urec = user;
    start_session(urec);   // Pass User Info to STDIO Conio Class
}


/**
 * Menu System - Main System Loop Starts Here.
 */
void menu_func::menu_mainloop(UserRec *user)
{
    urec = user;
    start_session(urec);   // Pass User Info to STDIO Conio Class
    char mString[255]= {0};

    while (System_Alive) {
        // System is Active
        ////errlog2("main_loop: readin() 1");
        menu_readin();

        ////errlog2("main_loop: readin() 2 , %i",noc);//,nogc);
        ////errlog2("main_loop: menu_proc 3");
        menu_proc(mString);
        ////errlog2("main_loop: menu_proc 4");
    }

    // Done, Send Logoff Message.
    lineolm_login(FALSE);
}
