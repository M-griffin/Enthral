/***************************************************************************
 *   Copyright (C) 2004-2017 by Michael Griffin                            *
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

# include "struct.h"
# include "data_areas.h"
# include "msgs.h"
# include "dtfunc.h"

# include <iostream>

# include <unistd.h>
# include <stdio.h>
# include <string>
# include <fstream>
# include <string>

# include <cstring>
# include <cstdlib>

# define ulong unsigned long

using namespace std;

// Setup lock file for this data area only.
int data_area::data_lockSet(int onoff)
{
    std::string path = LOCKPATH;
    path += _curarea;
    path += ".lck";

    if (!onoff) {
        remove((char *)path.c_str());
        return TRUE;
    }

    //While lock file missing, create, or loop until it disapears.
    FILE *stream;
    while(1) {
        stream = fopen(path.c_str(),"rb+");
        if(stream == NULL) {
            stream = fopen(path.c_str(), "wb");
            if(stream == NULL) {
                return FALSE;
            } else {
                fclose(stream);
                return TRUE;
            }
        }
        fclose(stream);
        usleep(10*20000);
    }
}

void data_area::chkparse(std::string &temp)
{
    std::string temp1;
    unsigned int st1 = -1;
    unsigned int st2 = -1;
    unsigned int  ct = -1;

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
        temp = "";
}

int data_area::da_parse(std::string cfgdata)
{
    if (cfgdata[0] == '#') return FALSE;
    else if (cfgdata.find("AreaName ", 0) !=
             std::string::npos) {
        chkparse(cfgdata);
        datar2->AreaName = cfgdata;
        return FALSE;
    } else if (cfgdata.find("FileName ", 0) !=
               std::string::npos) {
        chkparse(cfgdata);
        datar2->AreaFileName = cfgdata;
        return FALSE;
    } else if (cfgdata.find("AnsiFile ", 0) !=
               std::string::npos) {
        chkparse(cfgdata);
        datar2->AnsiFile = cfgdata;
        return FALSE;
    } else if (cfgdata.find("AnsiMid ", 0) !=
               std::string::npos) {
        chkparse(cfgdata);
        datar2->AnsiMid = cfgdata;
        return FALSE;
    } else if (cfgdata.find("NumOfRecs ", 0) !=
               std::string::npos) {
        chkparse(cfgdata);
        datar2->NumOfRecs = atoi((char *)cfgdata.c_str());
        return TRUE;
    }
    return FALSE;
    if (cfgdata.find("PageBreak ", 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        datar2->PageBreak = atoi((char *)cfgdata.c_str());
        return TRUE;
    }
    return FALSE;
}

int data_area::da_read(char *FileName)
{
    std::string path = DAPATH;
    path += FileName;
    path += ".txt";

    ifstream iNS3;
    iNS3.open( path.c_str() );
    if (!iNS3.is_open()) {
        return FALSE;
    }

    std::string cfgdata;
    for (;;) {
        if(iNS3.eof()) break;
        std::getline(iNS3,cfgdata);
        if(iNS3.eof()) break;
        if(da_parse(cfgdata) == TRUE) break;
    }
    iNS3.close();
    return TRUE;
}

void data_area::rec_parse(std::string cfgdata, int idx)
{
    char sText[200] = {0};
    if (cfgdata[0] == '#') return ;

    sprintf(sText,"Desc[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].Desc = cfgdata;
        return;
    }

    sprintf(sText,"Prompt[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].Prompt = cfgdata;
        return;
    }

    sprintf(sText,"Size[%.03d]",idx);
    if (cfgdata.find(sText, 0)
        != std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].Size = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"CKeys[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].CKeys = cfgdata;
        return;
    }

    sprintf(sText,"CmdType[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].CmdType = cfgdata;
        return;
    }

    sprintf(sText,"MString[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].MString = cfgdata;
        return;
    }

    sprintf(sText,"MciCode[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].MciCode = cfgdata;
        return;
    }

    sprintf(sText,"HiString[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].HiString = cfgdata;
        return;
    }

    sprintf(sText,"LoString[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].LoString = cfgdata;
        return;
    }

    sprintf(sText,"Xcoord[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].Xcoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"Ycoord[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        cmdr2[idx].Ycoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"LBarCmd[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        (cfgdata == "TRUE") ?
        cmdr2[idx].LBarCmd = TRUE :
                             cmdr2[idx].LBarCmd = FALSE;
        return;
    }

    sprintf(sText,"MCICmd[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
        std::string::npos) {
        chkparse(cfgdata);
        (cfgdata == "TRUE") ?
        cmdr2[idx].MCICmd = TRUE :
                            cmdr2[idx].MCICmd = FALSE;
        return;
    }
}

int data_area::rec_exist(char *FileName, int idx)
{
    std::string path = DAPATH;
    path += FileName;
    path += ".txt";
    int ret = FALSE;
    char sText[200]= {0};

    ifstream iFS2;
    iFS2.open( path.c_str() );
    if (!iFS2.is_open()) {
        return ret;
    }

    sprintf(sText,"[DataRec%.03d]",idx);
    std::string cfgdata;
    for (;;) {
        std::getline(iFS2,cfgdata);
        if (cfgdata.find(sText,0) != std::string::npos) ret = TRUE;
        if(iFS2.eof()) break;
    }
    iFS2.close();
    return ret;

}

// Right now this loops through the menu too many times! haha
// Make it parse once for the max command rec found, save cpu! :)
int data_area::rec_cnt(char *MenuName)
{
    int  cnt = 0;
    while (rec_exist(MenuName,cnt)) {
        ++cnt;
    }
    return cnt;
}

int data_area::rec_read(char *FileName, int idx)
{
    std::string path = DAPATH;
    path += FileName;
    path += ".txt";

    //check if the command can be found in the file, else return false!
    // Don't need to Run through parsing each command,
    // Just do an initial Cnt_cmds and don't go over it! :)
    int ret = rec_exist(FileName,idx);
    if (ret == FALSE) return FALSE;

    int skip = FALSE;


    ifstream iFS;
    iFS.open( path.c_str() );
    if (!iFS.is_open()) {
        return FALSE;
    }

    std::string cfgdata;
    for (;;) {
        std::getline(iFS,cfgdata);
        if (skip == FALSE) {
            rec_parse(cfgdata,idx);
        }
        if(iFS.eof()) break;
    }
    iFS.close();
    return TRUE;
}

// Hold Commands Per Menu being Read in.
void data_area::insert_cmdr()
{
    int idx = 0;
    while (rec_read(_curarea,idx)) {
        ++idx;
        if (noc == idx) break;
    }

    inNoc = 0;
    // Run through and Setup Input Commands.
    for (int i = 0; i != noc; i++) {
        // Run through and Sort Input Commands.
        if (cmdr2[i].CmdType == "IN") {
            inexecnum[inNoc] = i;
            cmdr2[i].idx = inNoc+1;
            ++inNoc;
        } else {
            cmdr2[i].idx = 0;
        }

        if (cmdr2[i].MCICmd == TRUE) {
        }
    }

    // Allocate Data Area Record Array for Saving Input.
    if (inNoc != 0) {
        inArray = new std::string [inNoc+1];
        if (!inArray) {
            return;
        }
    }
}

// Read in Current Menu File
int data_area::data_area_readin()
{
    std::string path;
    path = DAPATH;
    path += _curarea;
    path += ".txt";

    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(fstr == NULL) {
        return FALSE;
    }
    fclose(fstr);

    // Setup The Data Area
    datar2 = new DataArea;
    if (!datar2) {
        return FALSE;
    }

    // Read in The Data Area Settings Only
    da_read(_curarea);

    noc   = rec_cnt(_curarea);
    cmdr2 = new DataRec[noc+1];
    if (!cmdr2) {
        return FALSE;
    }

    // Allocate Execution Order of Input Commands
    inexecnum = new short[noc+1];
    if (!inexecnum) {
        return FALSE;
    }

    // Read in all Data Area Commands now.
    insert_cmdr();
    return TRUE;
}

void data_area::data_area_clear()
{
    if (datar2    != 0) delete    datar2;
    if (cmdr2     != 0) delete [] cmdr2;
    if (inexecnum != 0) delete [] inexecnum;
    if (inArray   != 0) delete [] inArray;

    datar2    = 0;
    cmdr2     = 0;
    inexecnum = 0;
    inArray   = 0;
}

// Processing for Menu's with Lightbars & Hotkeys.
void data_area::process_data_area()
{
    int xx      = 1;        // Holds X Coord
    int yy      = 1;        // Holds Y Coord
    int iNoc    = 0;        // Holds Lightbar # of choices
    int choice  = 0;        // Holds Currect Lightbar #
    int *execnum; //[200]={0};   // Holds commands which are Lightbars
    int execnum3[10];
    unsigned char c,cc='\0';  // Hold Input / Lightbar Key
    bool  EscHit  = false;    // Is Input key Escaped char, or Normal Key
    char  outBuff[100];       // Holds Formatted Lightbar Data
    std::string output;       // Buffer for writing all lightbars at the same time
    int       executed;       // Test's for hot keys commands excuted, if non pass through loop
    int       exe = 0;        // FIRSTCMD's Executed

    long cntEscCmds = 0;

    // If no Data Commands Return!
    if (noc == 0) {
        Data_Alive = FALSE;
        return;
    }

    // Allocate Execution Order of Lightbar Commands
    execnum = new int[noc+1];
    if (!execnum) {
        return;
    }

    // Run throguh and Execute FIRSTCMD's then count lightbar commands.
    for (int i = 0; i != noc; i++) {
        // If we find a FIRSTCMD, Execute it right away!
        if (cmdr2[i].CKeys == "FIRSTCMD") {
            darea_docmd(&cmdr2[i]);
            ++exe;
        }
        // Get/Count Ligthbar Commands
        else if (cmdr2[i].LBarCmd == TRUE) {
            execnum[iNoc] = i;
            ++iNoc;
        }
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
        else if (cmdr2[i].CKeys == "PAGEUP") {
            execnum3[cntEscCmds] = i;
            ++cntEscCmds;
        } else if (cmdr2[i].CKeys == "PAGEDN") {
            execnum3[cntEscCmds] = i;
            ++cntEscCmds;
        }
    }

    // If all Commands are FIRSTCMD's then return after execution.
    if (exe == noc) return;

    // Setup of first Command, Highlited menu lightbar
    output.erase();

    // Read Menu Ansi to String that will hold lightbars, Push to screen sametime
    // Will speed up display and make ghosting not apear as much
    data_lockSet(TRUE); // Lock incase another node it trying to write it.
    if (datar2->AnsiFile != "") {
        ansiPrintf((char *)datar2->AnsiFile.c_str());
    }
    data_lockSet(FALSE);

    // Draw Lightbar's
    if (iNoc > 0) {
        // Setup Initial Lightbar as Selected
        xx = cmdr2[execnum[0]].Xcoord;
        yy = cmdr2[execnum[0]].Ycoord;
        sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[0]].HiString.c_str());
        output += outBuff;

        // Setup of Remaining Lightbars in Low highlight Form
        for (int rep = 1; rep != iNoc; rep++) {
            xx = cmdr2[execnum[rep]].Xcoord;
            yy = cmdr2[execnum[rep]].Ycoord;
            sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[rep]].LoString.c_str());
            output += outBuff;
        }

        // Write out all the Lightbars * and reset cursor position.
        output += "\x1b[24;79H";
        pipe2ansi((char*)output.c_str());
    }

    // Handle Lightbar Movement, and HotKey Input Processing.
    std::string sMenu = _curarea;
    while (Data_Alive) {
        memset(&EscapeKey,0,sizeof(EscapeKey));
        c = (char)getkey(true);

        if (iNoc > 0) {
            // Check here for Arrow Key / Escaped Input was Received
            if ((int)c == 27) {
                cc = EscapeKey[1];
                EscHit = true;
            } else EscHit = false;
        }
        output.erase();

        executed = 0;
        if (EscHit) { // Input Key is Escaped Meaning Arrow Keys
            if (cntEscCmds > 0) { // ESC Commands in Menu might overide lightbars.
                for (int ckey = 0; ckey != cntEscCmds; ckey++) {

                    if (cc == '\0' || cc == ' ') {
                        if (cmdr2[execnum3[ckey]].CKeys == "ESC") {
                            ++executed;
                            darea_docmd(&cmdr2[execnum3[ckey]]);
                        }
                    } else if (cc == 'A') {
                        if (cmdr2[execnum3[ckey]].CKeys == "UP") {
                            ++executed;
                            darea_docmd(&cmdr2[execnum3[ckey]]);
                        }
                    } else if (cc == 'B') {
                        if (cmdr2[execnum3[ckey]].CKeys == "DOWN") {
                            ++executed;
                            darea_docmd(&cmdr2[execnum3[ckey]]);
                        }
                    } else if (cc == 'C') {
                        if (cmdr2[execnum3[ckey]].CKeys == "RIGHT") {
                            ++executed;
                            darea_docmd(&cmdr2[execnum3[ckey]]);
                        }
                    } else if (cc == 'D') {
                        if (cmdr2[execnum3[ckey]].CKeys == "LEFT") {
                            ++executed;
                            darea_docmd(&cmdr2[execnum3[ckey]]);
                        }
                    }
                }
                if (executed > 0) return;
            }

            if (iNoc > 0) { // Make Sure we have ligthbar Commands.
                if (cc == 'A') cc = 'D';
                else if (cc == 'B') cc = 'C';

                if (cc == 'D' ) {  // Up & Left
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;

                    if (choice == 0) choice = iNoc-1;
                    else --choice;

                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[24;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str());
                    output += outBuff;
                    pipe2ansi((char*)output.c_str());
                    
                } else if (cc == 'C') { // Down & Right
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;

                    if (choice == iNoc-1) choice = 0;
                    else ++choice;

                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[24;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str());
                    output += outBuff;                    
                    pipe2ansi((char*)output.c_str());
                    
                } else if (cc == 'H' || cc == '1') { //&& menur2.MFlags.EscPassing == TRUE) { // Home Key
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;
                    choice = 0;
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[24;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str());
                    output += outBuff;                    
                    pipe2ansi((char*)output.c_str());
                    
                } else if (cc == '4' || cc == 'K' || cc == 'F') { //&& menur2.MFlags.EscPassing == TRUE) { // End Key
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;
                    choice = iNoc-1;
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[24;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str());
                    output += outBuff;                    
                    pipe2ansi((char*)output.c_str());
                    
                } else {
                    // If we have Escape, check if menu command, otherwise pass it through!
                    // Lateron Add SAPCE and TAB
                    if (cc == '\0' || cc == ' ') {
                        executed = 0;
                        for (int ckey = 0; ckey != noc; ckey++) {
                            if (cmdr2[ckey].CKeys == "ESC") {
                                ++executed;
                                darea_docmd(&cmdr2[ckey]);
                            }
                        }
                        if (executed > 0) {
                            delete [] execnum;
                            execnum = 0;
                            return;
                        }
                    } else { 
                        // Pass through the Key.
                    }
                }
            }
            // No Lightbars, Parse Normal HoyKey Input.
            else {
                // If we have Escape, check if menu command, otherwise pass it through!
                // Lateron Add SAPCE and TAB
                if (cc == '\0' || cc == ' ') {
                    executed = 0;
                    for (int ckey = 0; ckey != noc; ckey++) {
                        if (cmdr2[ckey].CKeys == "ESC") {
                            ++executed;
                            darea_docmd(&cmdr2[ckey]);
                        }
                    }
                    delete [] execnum;
                    execnum = 0;
                    return;
                } else {
                    // Not Parsed Valid
                    delete [] execnum;
                    execnum = 0;
                    return;
                }
            }
        }
        // Normal Key Input
        else {
            // If Enter, Return Cmd # of Lightbar Executed
            // also catch any stacked keys and execute in order!
            if ((int)c == 10) {
                executed = 0;
                // Here Loop through and execute stacked Commands
                for (int ckey = 0; ckey != noc; ckey++) {
                    if (cmdr2[ckey].CKeys == cmdr2[execnum[choice]].CKeys) {
                        ++executed;
                        darea_docmd(&cmdr2[ckey]);
                    }
                }
                if (executed > 0) {
                    delete [] execnum;
                    execnum = 0;
                    return;
                }
            }
            //  Else go through and run stack commands on hot key pressed.
            else {
                executed = 0;
                std::string t1;
                for (int ckey = 0; ckey != noc; ckey++) {
                    if (c == 32) { // Else check if it's a spacebar
                        if (cmdr2[ckey].CKeys == "SPACE") {
                            ++executed;
                            darea_docmd(&cmdr2[ckey]);
                        }
                    } else // Check any remaining Keys Hot Key Input.
                        if (cmdr2[ckey].CKeys[0] == toupper(c) && cmdr2[ckey].CKeys.size() < 2) {
                            ++executed;
                            darea_docmd(&cmdr2[ckey]);
                        }
                    // Add Any More Spelized Menu Input Keys here.
                }
                // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
                if (executed > 0) {
                    delete [] execnum;
                    execnum = 0;
                    return;
                }
            }
        }
    }
    if (execnum > 0) delete [] execnum;
    execnum = 0;
}

// Setup for MCI Code Parsing in Data AReas, Move this to Conio Soon!!
void data_area::parsemci(DataRec *cmdr)
{
    History hist;
    msgs mf;
    mb_list_rec mr;

    unsigned char c1 = cmdr->MString[0];
    unsigned char c2 = cmdr->MString[1];
    unsigned char c3 = cmdr->MString[2];
    memset(&hist,0,sizeof(History));

    char sText[200];

    if (c1 != '%') return;

    switch (c2) {
    case 'U' : // User MCI Codes
        switch (c3) {

        case '#' :
            inArray[cmdr->idx-1] = thisuser->idx;
            break;

        case 'H' :
            inArray[cmdr->idx-1] = (char *)thisuser->handle;
            break;

        case 'N' :
            inArray[cmdr->idx-1] = (char *)thisuser->name;
            break;

        case 'P' :
            inArray[cmdr->idx-1] = (char *)thisuser->password;
            break;

        case 'G' :
            inArray[cmdr->idx-1] = (toupper(thisuser->sex) == 'M') ? "Male" : "Female";
            break;

        case 'E' :
            inArray[cmdr->idx-1] = (char *)thisuser->email;
            break;

        case 'O' :
            inArray[cmdr->idx-1] = (char *)thisuser->usernote;
            break;

        case 'S' :
            inArray[cmdr->idx-1] = (char *)thisuser->startmenu;
            break;

        case 'Q' :
            inArray[cmdr->idx-1] = (char *)thisuser->c_question;
            break;

        case 'A' :
            inArray[cmdr->idx-1] = (char *)thisuser->c_answer;
            break;

        case 'B' :
            inArray[cmdr->idx-1] = Sec2Date(thisuser->dtbday);
            break;

        case 'F' :
            inArray[cmdr->idx-1] = Sec2DateTM(thisuser->dtfirston,1);
            break;

        case 'L' :
            inArray[cmdr->idx-1] = Sec2DateTM(thisuser->dtlaston,1);
            break;

        case 'X' :
            inArray[cmdr->idx-1] = getAge(thisuser->dtbday);
            break;

        case 'Z' :
            inArray[cmdr->idx-1] = (toupper(thisuser->emprivate) == 'Y') ? "Yes" : "No";
            break;

        default :
            break;
        }
        break;

    case 'M' : // User Message MCI Codes
        switch (c3) {

        case '#' :
            sprintf(sText,"%ld",(ulong)thisuser->lastmbarea);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'N' :
            mf.read_mbaselist(&mr,thisuser->lastmbarea);
            inArray[cmdr->idx-1] = (char *)mr.mbdisplay;
            break;

        case 'L' :
            sprintf(sText,"%ld",(ulong)thisuser->lastmsg);
            inArray[cmdr->idx-1] = sText;
            break;

        default :
            break;
        }
        break;

    case 'H' : // System History MCI Codes
        switch (c3) {

        case 'C' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.Calls);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'E' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.Emails);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'P' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.Posts);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'R' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.Replies);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'T' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.Edits);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'V' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.Views);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'L' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.Local);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'N' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.Echomail);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'U' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.Uploads);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'W' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.UploadKb);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'K' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.Deletes);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'D' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.Downloads);
            inArray[cmdr->idx-1] = sText;
            break;

        case 'X' :
            hist_read(&hist);
            sprintf(sText,"%ld", (ulong)hist.DownloadKb);
            inArray[cmdr->idx-1] = sText;
            break;

        default :
            break;
        }
        break;

    case 'D' : // Date Time
        switch (c3) {
        case '1' :
            inArray[cmdr->idx-1] = Sec2DateTM(GetCurrentDTSec(),1);
            break;

        case '2' :
            inArray[cmdr->idx-1] = Sec2DateTM(GetCurrentDTSec(),2);
            break;

        case '3' :
            inArray[cmdr->idx-1] = Sec2DateTM(GetCurrentDTSec(),3);
            break;

        case '4' :
            inArray[cmdr->idx-1] = Sec2DateTM(GetCurrentDTSec(),4);
            break;

        default :
            break;

        }
        break;
    default :
        break;
    }
}

void data_area::parseinput(DataRec *cmdr)
{
    char text[1024]= {0};
    char rBuffer[1024]= {0};

    strcpy(text,cmdr->Prompt.c_str());
    int len = cmdr->Size;
    inputfield(text,len);
    while (1) {
        pipe2ansi(text);
        getline(rBuffer,len);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0) break;
        pipe2ansi((char *)"|15|17");
    }

    inArray[cmdr->idx-1] = rBuffer;
}

unsigned long
data_area::count_file_records()
{
    std::string path = DAPATH;
    path += _curarea;
    path += ".out";

    data_lockSet(TRUE);
    ifstream ins2;
    ins2.open( path.c_str() );
    if (!ins2.is_open()) {
        return 0;
    }

    std::string data;
    unsigned long cnt = 0;
    while (1) {
        std::getline(ins2,data);
        if (data == "</record>") ++cnt;
        if(ins2.eof()) break;
    }
    ins2.close();
    data_lockSet(FALSE);
    return cnt;
}

void data_area::create_ansifile()
{
    // Templates are Held in Data Area, output is in Ansi Directory.
    std::string      path, path2, path3, path4, path5;
    path             = ANSIPATH;
    path            += _curarea;
    path2            = DAPATH;
    path2           += _curarea;
    path3            = path2;
    path4            = path2;
    path5            = path2;

    // Setup Templates and Output file.
    path            += ".ans";
    path2           += ".top";
    path3           += ".mid";
    path4           += ".bot";
    path5           += ".out";

    // Holds all Process Data for Output.
    std::string     top, mid, bot, ansibuff, buff;
    int             c;

    // Read Top File, if can't open ansi, skip it.
    c = '\0';
    FILE *inStream;
    if ((inStream = fopen(path2.c_str(), "r+")) ==  NULL) {

    } else {
        while (c != EOF) {
            c = getc(inStream);
            if (c != EOF) top += c;
        }
        fclose(inStream);
    }

    // Read Mid File
    c = '\0';
    if ((inStream = fopen(path3.c_str(), "r+")) ==  NULL) {
    } else {
        while (c != EOF) {
            c = getc(inStream);
            if (c != EOF) mid += c;
        }
        fclose(inStream);
    }

    // Read Bot File
    c = '\0';
    if ((inStream = fopen(path4.c_str(), "r+")) ==  NULL) {
    } else {
        while (c != EOF) {
            c = getc(inStream);
            if (c != EOF) bot += c;
        }
        fclose(inStream);
    }

    ansibuff        = top;
    std::string::size_type id1 = 0;

    int input       = 0;
    int i           = 0;
    int truncate    = FALSE;

    // Make Sure if There are more records then the limit,
    // we skip the inital to only parse the proper amount.
    if (datar2->NumOfRecs != 0 && datar2->NumOfRecs < numFileRecs) {
        // Reset Which record we start on, and mark for truncation lateron.
        i = numFileRecs - datar2->NumOfRecs;
        truncate = TRUE; 
    }

    // Do so checking on Record Size of Current File, and Truncate if were over and resave it!
    int it = 0;
    if (truncate == TRUE) {
        //Re-Write Out File with proper # of records.
        inputArray = new std::string [(datar2->NumOfRecs*inNoc)+1];
        it = inNoc;

        // Grabbing all input line per record.
        for (int in = 0; in != datar2->NumOfRecs*inNoc; in++) {
            inputArray[in] = fileArray[it];
            ++it;
        }
        truncate_da_buffer();
        if (inputArray != 0) delete [] inputArray;
        inputArray = 0;

        // Reload File Now for Proper Parsing
        if (fileArray != 0) delete [] fileArray;
        fileArray = 0;
        read_file_records();
        return;
    }

    char *temp;
    char MCI[3] = {0};
    int  foundl = FALSE;
    int  foundr = FALSE;
    int  space  = 0;

    // Parse Each Record / Input, and Replace MCI Code in Mid with Input Data.
    input = 0;
    for (i = 0; i != numFileRecs; i++) {    // Parse Each Record
        buff = mid;
        for (int i2 = 0; i2 != inNoc; i2++) {   // Parse Each Input Per Rec.
            // Find First MCI Code and Parse / Replace it.
            id1 = buff.find(cmdr2[inexecnum[i2]].MciCode, 0);
            // Process MCI Code
            if (id1 != std::string::npos) { // ie.. &ON
                memset(&MCI,0,sizeof(MCI));
                space = 0;
                
                // Left Justify
                if (buff[id1+3] == '{') { 
                    MCI[0] = buff[id1+4];
                    MCI[1] = buff[id1+5];
                    space  = atoi(MCI);
                    foundr = FALSE;
                    foundl = TRUE;
                    
                // Right Justify
                } else if (buff[id1+3] == '}') { 
                    MCI[0] = buff[id1+4];
                    MCI[1] = buff[id1+5];
                    space  = atoi(MCI);
                    foundl = FALSE;
                    foundr = TRUE;
                }

                if (space != 0) {
                    temp = new char [fileArray[input].size()+(space+1)];
                    if (!temp) {
                    }
                    // MCI Translation .
                    sprintf(temp,"%s",(char *)fileArray[input].c_str());
                    if (foundl == TRUE) {
                        lspacing(temp,space);
                    } else if (foundr == TRUE) {
                        rspacing(temp,space);
                    }
                }

                //MciCode Holds Original Code, fileArray Holds String from File.
                //If we Parsed Justify, then Erase that MCI Code as well.
                (space == 0) ?
                buff.replace(id1,cmdr2[inexecnum[i2]].MciCode.size(),fileArray[input]):
                buff.replace(id1,cmdr2[inexecnum[i2]].MciCode.size()+3,temp);

                if (space != 0) {
                    if (temp != 0) delete [] temp;
                    temp = 0;
                }
            }
            ++input;
        }
        ansibuff += buff;
        ansibuff += "\r\n";
        buff.erase();
    }

    ansibuff += bot;
    if (fileArray != 0) delete [] fileArray;
    fileArray = 0;

    data_lockSet(TRUE);
    // now write out ansi file.
    ofstream ostr2;
    ostr2.open( path.c_str(), ofstream::out | ofstream::trunc );
    if (!ostr2.is_open()) {
        return;
    }

    // Write out Complete Ansi.
    ostr2 << ansibuff << endl;
    ostr2.close();
    data_lockSet(FALSE);
}

void data_area::read_file_records()
{
    std::string
    path             = DAPATH;
    path            += _curarea;
    path            += ".out";

    std::string data;

    int  rec         = 0;
    int  in          = 0;
    int  idx         = 0;
    int  getinput    = FALSE;

    numFileRecs = count_file_records();
    if (numFileRecs == 0) {
        return;
    }

    // Number of Input Files * Number or Records in the File.
    fileArray = new std::string[(numFileRecs*inNoc)+1];
    if (!fileArray) {
        return;
    }

    data_lockSet(TRUE);
    ifstream ins;
    ins.open( path.c_str() );
    if (!ins.is_open()) {
        return;
    }

    // Loop Though all Records in the File and Read in the Input Strings.
    int iMsg = FALSE;
    for (;;) {
        std::getline(ins,data);
        if (data == "<record>") { }
        else if (data == "<input>") getinput = TRUE;
        else if (data == "<message>") iMsg = TRUE;
        else if (data == "</message>") {
            iMsg = FALSE;
            ++idx;
        } else if (data == "</input>") {
            getinput = FALSE;
            ++in;
        } else if (data == "</record>") {
            ++rec;
            in = 0;
        } else {
            // Start Processing of Input Data Here.
            if (getinput == TRUE) {
                //Cat String
                if (iMsg == TRUE) {
                    fileArray[idx] += data;
                } else {
                    fileArray[idx] = data;
                    ++idx;
                }
            }
        }
        
        // Test for Break;
        if (rec != 0 &&
            rec == numFileRecs) break;
        if(ins.eof()) break;
    }

    ins.close();
    data_lockSet(FALSE);
    create_ansifile();
    return;
}

void data_area::truncate_da_buffer()
{
    std::string path = DAPATH;
    path += _curarea;
    path += ".out";

    data_lockSet(TRUE);
    ofstream ostr;
    ostr.open( path.c_str(), ofstream::out | ofstream::trunc );
    if (!ostr.is_open()) {
        return;
    }

    int input = 0;
    for (int i = 0; i != datar2->NumOfRecs; i++) {
        ostr << "<record>"  << endl;
        for (int i2 = 0; i2 != inNoc; i2++) {
            ostr << "<input>"  << endl;
            ostr << inputArray[input] << endl;
            ostr << "</input>" << endl;
            ++input;
        }
        ostr << "</record>"  << endl;
    }
    ostr.close();
    data_lockSet(FALSE);
}

void data_area::save_da_buffer()
{
    std::string path = DAPATH;
    path += _curarea;
    path += ".out";

    data_lockSet(TRUE);
    ofstream ostr;
    ostr.open( path.c_str(), ofstream::out | ofstream::app );
    if (!ostr.is_open()) {
        data_lockSet(FALSE);
        return;
    }

    ostr << "<record>"  << endl;
    for (int i = 0; i != inNoc; i++) {
        ostr << "<input>"  << endl;
        ostr << inArray[i] << endl;
        ostr << "</input>" << endl;
    }

    ostr << "</record>"  << endl;
    ostr.close();
    data_lockSet(FALSE);
}

int data_area::confirm_da_save(DataRec *cmdr)
{
    char text[1024]= {0};
    char c;

    strcpy(text,cmdr->Prompt.c_str());
    int len = 1;
    inputfield(text,len);
    while (1) {
        pipe2ansi(text);
        c = getkey(true);
        if (toupper(c) == 'Y') {
            memset(&text,0,sizeof(text));
            sprintf(text,"%c",c);
            pipe2ansi(text);
            return TRUE;
        } else if (toupper(c) == 'N') {
            memset(&text,0,sizeof(text));
            sprintf(text,"%c",c);
            pipe2ansi(text);
            return FALSE;
        }
        pipe2ansi((char *)"|15|17");
    }
}

void data_area::darea_docmd(DataRec *cmdr)
{
    unsigned char c1 = cmdr->CmdType[0];
    unsigned char c2 = cmdr->CmdType[1];

    if (cmdr->CmdType.size() < 1) return;

    switch (c1) {
    case 'I' : // Input Data Area Commands
        switch (c2) {
        case 'N' :
            (cmdr->MCICmd == TRUE) ? parsemci(cmdr) : parseinput(cmdr);
            break;
        default :
            break;
        }
        break;

    case 'O' : // Output Data Area Command
        switch (c2) {
        case 'T' : // Write Buffer to File, display prompt and confirm save!
            dataSaved = confirm_da_save(cmdr);
            if (dataSaved == TRUE) save_da_buffer();

            break;

        case 'S' : // Force Save without Prompting for Confirm.
            // This is used more for Last Callers/ Statitcs, and Passing All Values
            // Then having a user input data.
            save_da_buffer();

            break;
        default :
            break;
        }
        break;

    case 'D' : // Data Area Commands
        switch (c2) {
        case 'P' : // Process Area, Read in Data File and Parse AnsiFile
            // Only Process if a Record has been Saved, Save Processing!
            if (dataSaved == TRUE) read_file_records();
            break;

        case 'F' : // Process Area, Read in Data File and Parse AnsiFile
            // Force Processing if needed.
            read_file_records();
            break;

        default :
            break;
        }
        break;

        // Start of Normal Menu Commands.
    case '*' :
        switch (c2) {
        case 'G' : // Exit Data Area
            Data_Alive = FALSE;
            break;
        default :
            break;
        }
        break;


    case '-' :
        switch (c2) {

        case 'T' : // Display a line of Text
            pipe2ansi((char *)cmdr->MString.c_str());
            break;

        case 'F' : // Display a File
            ansiPrintf((char *)cmdr->MString.c_str());
            break;

        case 'P' : // Pause
            startpause();
            break;

        default  : // None Found!
            break;
        }
        break;

    default :
        break;
    }
}

// Menu System Loop.
void data_area::data_loop()
{
    Data_Alive = TRUE;
    data_area_readin();
    while (Data_Alive) {
        process_data_area();
    }
    data_area_clear();
}
