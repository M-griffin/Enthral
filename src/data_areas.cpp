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

    if (!onoff)
    {
        remove((char *)path.c_str());
        return TRUE;
    }

    //While lock file missing, create, or loop until it disapears.
    FILE *stream;
    while(1)
    {
        stream = fopen(path.c_str(),"rb+");
        if(stream == NULL)
        {
            stream = fopen(path.c_str(), "wb");
            if(stream == NULL)
            {
                //elog("Error history.lck!");
                return FALSE;
            }
            else
            {
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
        st2 != std::string::npos)
    {
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        temp = temp1;
    }
    else
        temp = "";
}


int data_area::da_parse(std::string cfgdata)
{

    // Disgards any Config lines with the # Character
    if (cfgdata[0] == '#') return FALSE;
    else if (cfgdata.find("AreaName ", 0) !=
             std::string::npos)
    {
        chkparse(cfgdata);
        datar2->AreaName = cfgdata;
        return FALSE;
    }
    else if (cfgdata.find("FileName ", 0) !=
             std::string::npos)
    {
        chkparse(cfgdata);
        datar2->AreaFileName = cfgdata;
        return FALSE;
    }
    else if (cfgdata.find("AnsiFile ", 0) !=
             std::string::npos)
    {
        chkparse(cfgdata);
        datar2->AnsiFile = cfgdata;
        return FALSE;
    }
    else if (cfgdata.find("AnsiMid ", 0) !=
             std::string::npos)
    {
        chkparse(cfgdata);
        datar2->AnsiMid = cfgdata;
        return FALSE;
    }
    else if (cfgdata.find("NumOfRecs ", 0) !=
             std::string::npos)
    {
        chkparse(cfgdata);
        datar2->NumOfRecs = atoi((char *)cfgdata.c_str());
        return TRUE;
    }
    return FALSE;
    if (cfgdata.find("PageBreak ", 0) !=
            std::string::npos)
    {
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
    if (!iNS3.is_open())
    {
        //elog("Couldn't Open DataArea: %s\n", path.c_str());
        return FALSE;
    }

    std::string cfgdata;
    for (;;)
    {
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

    // Disgards any Config lines with the # Character
    char sText[200] = {0};
    if (cfgdata[0] == '#') return ;

    sprintf(sText,"Desc[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].Desc = cfgdata;
        return;
    }

    sprintf(sText,"Prompt[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].Prompt = cfgdata;
        return;
    }

    sprintf(sText,"Size[%.03d]",idx);
    if (cfgdata.find(sText, 0)
            != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].Size = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"CKeys[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].CKeys = cfgdata;
        return;
    }

    sprintf(sText,"CmdType[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].CmdType = cfgdata;
        return;
    }

    sprintf(sText,"MString[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].MString = cfgdata;
        return;
    }

    sprintf(sText,"MciCode[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].MciCode = cfgdata;
        return;
    }

    sprintf(sText,"HiString[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].HiString = cfgdata;
        return;
    }

    sprintf(sText,"LoString[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].LoString = cfgdata;
        return;
    }

    sprintf(sText,"Xcoord[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].Xcoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"Ycoord[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].Ycoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"LBarCmd[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
        chkparse(cfgdata);
        (cfgdata == "TRUE") ?
        cmdr2[idx].LBarCmd = TRUE :
                             cmdr2[idx].LBarCmd = FALSE;
        return;
    }

    sprintf(sText,"MCICmd[%.03d]",idx);
    if (cfgdata.find(sText, 0) !=
            std::string::npos)
    {
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

    // Open file for reading and parsing.
    ifstream iFS2;
    iFS2.open( path.c_str() );
    if (!iFS2.is_open())
    {
        //elog("\nCouldn't Open Data Area Commands: %s\n", path.c_str());
        return ret;
    }

    // Loop Through and Find the The Command
    sprintf(sText,"[DataRec%.03d]",idx);
    std::string cfgdata;
    for (;;)
    {
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
    while (rec_exist(MenuName,cnt))
    {
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


    // Else Read and Parse it
    ifstream iFS;
    iFS.open( path.c_str() );
    if (!iFS.is_open())
    {
        //elog("** Couldn't Open Data Area: %s\n", path.c_str());
        return FALSE;
    }

    std::string cfgdata;
    for (;;)
    {
        std::getline(iFS,cfgdata);
        // If lines are commented out.

        // if (cfgdata == "/*") skip = TRUE;
        // else
        // if (cfgdata == "*/") skip = FALSE;

        if (skip == FALSE) rec_parse(cfgdata,idx);
        if(iFS.eof()) break;
    }
    iFS.close();
    return TRUE;
}

// Hold Commands Per Menu being Read in.
void data_area::insert_cmdr()
{

    int idx = 0;
    while (rec_read(_curarea,idx))
    {
        ++idx;
        if (noc == idx) break;
    }
    //elog("insert_cmdr() : %i Commands Inserted",idx);

    // display_commands(); // {Test to Double Check What Commands
    // And All The fields for each command were loaded.

    inNoc = 0;
    // Run through and Setup Input Commands.
    for (int i = 0; i != noc; i++)
    {
        // Run through and Sort Input Commands.
        if (cmdr2[i].CmdType == "IN")
        {
            inexecnum[inNoc] = i; // don't think this is needed.
            cmdr2[i].idx = inNoc+1; // == 1 and up... 0 = none.
            ++inNoc;
        }
        else
        {
            cmdr2[i].idx = 0;
        }

        if (cmdr2[i].MCICmd == TRUE)
        {
            //elog("Found MCI Command : %i..",inNoc);
            //elog("MCI Command String: %s..",(char *)cmdr2[i].MString.c_str());
        }
    }
    // Allocate Data Area Record Array for Saving Input.
    //elog("Allocating Memory for inArray: inNoc -> %i..",inNoc);
    if (inNoc != 0)
    {
        inArray = new std::string [inNoc+1];
        if (!inArray)
        {
            //elog("Unable to Allocate Memory for inArray: inNoc -> %i, returning...",inNoc);
            return;
        }
    }
    //elog("Done Allocating Memory for inArray: inNoc -> %i..",inNoc);
}


// Read in Current Menu File
int data_area::data_area_readin()
{

    //elog("Readin() in Data Area: %s",_curarea);

    std::string path;
    path = DAPATH;
    path += _curarea;
    path += ".txt";

    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(fstr == NULL)
    {
        //errlog((char *)"Data Area not Found! : %s, returning...",(char *)_curarea);
        return FALSE;
    }
    fclose(fstr);

    // Setup The Data Area
    datar2 = new DataArea;
    if (!datar2)
    {
//        errlog((char *)"Unable to Allocate Memory for Data Area: %s, returning...",(char *)_curarea);
        return FALSE;
    }

    // Read in The Data Area Settings Only
    da_read(_curarea);

    // Allocate Data Area Commands
    noc   = rec_cnt(_curarea);
    cmdr2 = new DataRec[noc+1];
    if (!cmdr2)
    {
        //elog("Unable to Allocate Memory for Data Area Commands: %s, returning...",_curarea);
        return FALSE;
    }

    // Allocate Execution Order of Input Commands
    inexecnum = new short[noc+1];
    if (!inexecnum)
    {
        //elog("Unable to Allocate Memory for Data Area: %s - inexecnum[], returning...",_curarea);
        return FALSE;
    }

    // Read in all Data Area Commands now.
    insert_cmdr();
    return TRUE;
}

void data_area::data_area_clear()
{

    // fileArray & inputArray are allocated and killed on the fly.
    // The Rest are Data Settings and Comamnds which stay Global.
    if (datar2    != 0) delete    datar2;
    if (cmdr2     != 0) delete [] cmdr2;
    if (inexecnum != 0) delete [] inexecnum;
    if (inArray   != 0) delete [] inArray;

    datar2    = 0;
    cmdr2     = 0;
    inexecnum = 0;
    inArray   = 0;
    //elog("Data Area Cleared: %s",_curarea);
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
    if (noc == 0)
    {
        Data_Alive = FALSE;
        return;
    }

    // Allocate Execution Order of Lightbar Commands
    execnum = new int[noc+1];
    if (!execnum)
    {
        //elog("Unable to Allocate Memory for Data Area: %s - execnum[], returning...",_curarea);
        return;
    }

    // Run throguh and Execute FIRSTCMD's then count lightbar commands.
    for (int i = 0; i != noc; i++)
    {
        // If we find a FIRSTCMD, Execute it right away!
        if (cmdr2[i].CKeys == "FIRSTCMD")
        {
            darea_docmd(&cmdr2[i]);
            ++exe;
        }
        // Get/Count Ligthbar Commands
        else if (cmdr2[i].LBarCmd == TRUE)
        {
            execnum[iNoc] = i;
            ++iNoc;
        }
        // Count Escape Keys in Menu System to override passthrough
        if (cmdr2[i].CKeys == "ESC")
        {
            execnum3[cntEscCmds] = i;
            ++cntEscCmds;
        }
        else if (cmdr2[i].CKeys == "LEFT")
        {
            execnum3[cntEscCmds] = i;
            ++cntEscCmds;
        }
        else if (cmdr2[i].CKeys == "RIGHT")
        {
            execnum3[cntEscCmds] = i;
            ++cntEscCmds;
        }
        else if (cmdr2[i].CKeys == "UP")
        {
            execnum3[cntEscCmds] = i;
            ++cntEscCmds;
        }
        else if (cmdr2[i].CKeys == "DOWN")
        {
            execnum3[cntEscCmds] = i;
            ++cntEscCmds;
        }
        /*
        else
        if (strcmp(cmdr2[i].CKeys,"HOME") == 0) {
            execnum3[cntEscCmds] = i;
            ++cntEscCmds;
        }
        else
        if (strcmp(cmdr2[i].CKeys,"END") == 0) {
            execnum3[cntEscCmds] = i;
            ++cntEscCmds;
        }*/
        else if (cmdr2[i].CKeys == "PAGEUP")
        {
            execnum3[cntEscCmds] = i;
            ++cntEscCmds;
        }
        else if (cmdr2[i].CKeys == "PAGEDN")
        {
            execnum3[cntEscCmds] = i;
            ++cntEscCmds;
        }
    }

    // If all Commands are FIRSTCMD's then return after execution.
    if (exe == noc) 
	{
		delete [] execnum;
		execnum = 0;
		return;
	}
		

    // Setup of first Command, Highlited menu lightbar
    output.erase();

    // Read Menu Ansi to String that will hold lightbars, Push to screen sametime
    // Will speed up display and make ghosting not apear as much
    data_lockSet(TRUE); // Lock incase another node it trying to write it.
    if (datar2->AnsiFile != "")
    {
        //readinAnsi(datar2->AnsiFile,output);
        ansiPrintf((char *)datar2->AnsiFile.c_str());
    }
    data_lockSet(FALSE);

    // Draw Lightbar's
    if (iNoc > 0)
    {
        // Setup Initial Lightbar as Selected
        xx = cmdr2[execnum[0]].Xcoord;
        yy = cmdr2[execnum[0]].Ycoord;
        sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[0]].HiString.c_str());
        output += outBuff;

        // Setup of Remaining Lightbars in Low highlight Form
        for (int rep = 1; rep != iNoc; rep++)
        {
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
    while (Data_Alive)
    {
        memset(&EscapeKey,0,sizeof(EscapeKey));
        c = (char)getkey(true);

        if (iNoc > 0)
        {
            // Check here for Arrow Key / Escaped Input was Received
            if ((int)c == 27)
            {
                cc = EscapeKey[1];
                EscHit = true;
            }
            else EscHit = false;
        }
        output.erase();

        // If were in prompt and receive Int/Digit,
        // pass through Digits back to Message Quoter && Title Scan Etc...
        /*
        if (isdigit(c)) {
            if (datar2->MFlags.DigitPassing == TRUE) {
                inPut[0] = '#';
                inPut[1] = c;
                return;
            }
        }*/

        executed = 0;
        if (EscHit)   // Input Key is Escaped Meaning Arrow Keys
        {
            // Skip Moving Lightbars with Up/Dn Keys if were in Message Prompt
            /*
            if (menur2.MFlags.EscPassing == FALSE) { // Arrow Keys
                if (cc == 'A') cc = 'D';
                else if (cc == 'B') cc = 'C';
            }*/
            if (cntEscCmds > 0)   // ESC Commands in Menu might overide lightbars.
            {
                for (int ckey = 0; ckey != cntEscCmds; ckey++)
                {

                    if (cc == '\0' || cc == ' ')
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "ESC")
                        {
                            //elog("- ESC EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);

                            ++executed;
                            //menu_docmd(cmdr2[execnum3[ckey]].CmdKeys,cmdr2[execnum3[ckey]].MString);
                            darea_docmd(&cmdr2[execnum3[ckey]]);
                            //strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                        }
                    }
                    else if (cc == 'A')
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "UP")
                        {
                            //elog("- UP EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);

                            ++executed;
                            darea_docmd(&cmdr2[execnum3[ckey]]);
                            //strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                        }
                    }
                    else if (cc == 'B')
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "DOWN")
                        {
                            //elog("- DOWN EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);

                            ++executed;
                            darea_docmd(&cmdr2[execnum3[ckey]]);
                            //strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                        }
                    }
                    else if (cc == 'C')
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "RIGHT")
                        {
                            //elog("- RIGHT EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);

                            ++executed;
                            darea_docmd(&cmdr2[execnum3[ckey]]);
                            //strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                        }
                    }
                    else if (cc == 'D')
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "LEFT")
                        {
                            //elog("- LEFT EXECUTED! %s",(const char*)cmdr2[execnum3[ckey]].CmdKeys);

                            ++executed;
                            darea_docmd(&cmdr2[execnum3[ckey]]);
                            //strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys);
                        }
                    }
                }
                // Executed == 0, Then Key Pressed was not valid! :)
                // Pass through the ESC then
                //if (tScroll) p->Active = false;
                if (executed > 0) 
				{				
					delete [] execnum;
					execnum = 0;
					return;
				}	
            }



            if (iNoc > 0)   // Make Sure we have ligthbar Commands.
            {
                if (cc == 'A') cc = 'D';
                else if (cc == 'B') cc = 'C';

                if (cc == 'D' )    // Up & Left
                {
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
                    // Send Lightbar output
                    pipe2ansi((char*)output.c_str());
                }
                else if (cc == 'C')   // Down & Right
                {
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
                    // Send Lightbar output
                    pipe2ansi((char*)output.c_str());
                }
                else if (cc == 'H' || cc == '1')   //&& menur2.MFlags.EscPassing == TRUE) { // Home Key
                {
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;
                    choice = 0;
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[24;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str());
                    output += outBuff;
                    // Send Lightbar output
                    pipe2ansi((char*)output.c_str());
                }
                else if (cc == '4' || cc == 'K' || cc == 'F')   //&& menur2.MFlags.EscPassing == TRUE) { // End Key
                {
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;
                    choice = iNoc-1;
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[24;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str());
                    output += outBuff;
                    // Send Lightbar output
                    pipe2ansi((char*)output.c_str());
                }
                else
                {
                    // If we have Escape, check if menu command, otherwise pass it through!
                    // Lateron Add SAPCE and TAB
                    if (cc == '\0' || cc == ' ')
                    {
                        executed = 0;
                        for (int ckey = 0; ckey != noc; ckey++)
                        {
                            if (cmdr2[ckey].CKeys == "ESC")
                            {
                                //if (tScroll) p->Active = false;
                                ++executed;
                                darea_docmd(&cmdr2[ckey]);
                                //strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys);
                            }
                        }
                        // Executed == 0, Then Key Pressed was not valid! :)
                        if (executed > 0)
                        {
                            delete [] execnum;
                            execnum = 0;
                            return;
                        }
                    }
                    else   // Pass through the Key.
                    {
                        //delete [] execnum;
                        //return;
                    }
                }
            }
            // No Lightbars, Parse Normal HoyKey Input.
            else
            {
                // If we have Escape, check if menu command, otherwise pass it through!
                // Lateron Add SAPCE and TAB
                if (cc == '\0' || cc == ' ')
                {
                    executed = 0;
                    for (int ckey = 0; ckey != noc; ckey++)
                    {
                        if (cmdr2[ckey].CKeys == "ESC")
                        {
                            //if (tScroll) p->Active = false;
                            ++executed;
                            darea_docmd(&cmdr2[ckey]);
                            //strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys);
                        }
                    }
                    // Executed == 0, Then Key Pressed was not valid! :)
                    delete [] execnum;
                    execnum = 0;
                    return;
                }
                else   // Pass through the Key.
                {
                    // Not Parsed Valid
                    delete [] execnum;
                    execnum = 0;
                    return;
                }
            }
        }
        // Normal Key Input
        else
        {
            // If Enter, Return Cmd # of Lightbar Executed
            // also catch any stacked keys and execute in order!
            if ((int)c == 10)
            {
                executed = 0;
                // Here Loop through and execute stacked Commands
                for (int ckey = 0; ckey != noc; ckey++)
                {
                    if (cmdr2[ckey].CKeys == cmdr2[execnum[choice]].CKeys)
                    {
                        //if (tScroll) p->Active = false;
                        ++executed;
                        //elog(" * menu - darea_docmd - %s / CmdType",(char *)cmdr2[ckey].CmdType.c_str());
                        //elog(" * menu - darea_docmd - %s / CKeys",(char *)cmdr2[ckey].CKeys.c_str());
                        darea_docmd(&cmdr2[ckey]);
                        //strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys);
                    }
                }
                if (executed > 0)
                {
                    delete [] execnum;    // Else Loop..
                    execnum = 0;
                    return;
                }
            }
            //  Else go through and run stack commands on hot key pressed.
            else
            {
                executed = 0;      // Normal Key Inputed, if Match's
                std::string t1;
                for (int ckey = 0; ckey != noc; ckey++)   // Loop and Run Stacked Commands.
                {
                    if (c == 32)   // Else check if it's a spacebar
                    {
                        if (cmdr2[ckey].CKeys == "SPACE")
                        {
                            //if (tScroll) p->Active = false;
                            ++executed;
                            darea_docmd(&cmdr2[ckey]);
                            //strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys);
                        }
                    }
                    else // Check any remaining Keys Hot Key Input.
                        if (cmdr2[ckey].CKeys[0] == toupper(c) && cmdr2[ckey].CKeys.size() < 2)
                        {
                            //if (tScroll) p->Active = false;
                            ++executed;
                            darea_docmd(&cmdr2[ckey]);
                            //strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys);
                        }
                    // Add Any More Spelized Menu Input Keys here.
                }
                // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
                if (executed > 0)
                {
                    delete [] execnum;    // Else Loop..
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

    History hist; // Struct
    msgs mf;
    mb_list_rec mr;

    unsigned char c1 = cmdr->MString[0];
    unsigned char c2 = cmdr->MString[1];
    unsigned char c3 = cmdr->MString[2];
    memset(&hist,0,sizeof(History));

    char sText[200];

    if (c1 != '%') return;
    //elog("3. parsemci() IN; %c%c%c",c1,c2,c3);

    switch (c2)
    {
    case 'U' : // User MCI Codes
        switch (c3)
        {

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

            // Bug need to rework the age.
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
        switch (c3)
        {

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
        switch (c3)
        {

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

            /*
                unsigned
                short  linelen,
                       pagelen,

                // FS Message Reader Options
                unsigned
                short  readertheme,
                       fsetheme,
                       pageDn,      // Arror Keys - Page Down or line down
                       readerFs;    // Use FullScreen Reader or Old Fashon.
            */

        default :
            break;
        }
        break;

    case 'D' : // Date Time
        switch (c3)
        {
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

    //elog("3. input received: %s ",(char *)inArray[cmdr->idx-1].c_str());
}

void data_area::parseinput(DataRec *cmdr)
{

    // Run GetLine and get users input.
    //elog("3. parseinput() IN; ");
    char text[1024]= {0};
    char rBuffer[1024]= {0};

    strcpy(text,cmdr->Prompt.c_str());
    int len = cmdr->Size;
    inputfield(text,len);
    while (1)
    {
        pipe2ansi(text);
        getline(rBuffer,len);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0) break;
        pipe2ansi((char *)"|15|17");
    }

    inArray[cmdr->idx-1] = rBuffer;
    //elog("3. input received: %s ",(char *)inArray[cmdr->idx-1].c_str());
}

// Never Negative.
unsigned long
data_area::count_file_records()
{

    std::string path = DAPATH;
    path += _curarea;
    path += ".out";

    data_lockSet(TRUE);
    ifstream ins2;
    ins2.open( path.c_str() );
    if (!ins2.is_open())
    {
        //elog("Counldn't Open Data Area File For Record Count: %s\n", path.c_str());
        return 0;
    }

    std::string data;
    unsigned long cnt = 0;
    while (1)
    {
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
    path            += ".ans";  // Output File
    path2           += ".top";  // TOP
    path3           += ".mid";  // MID with MCI Codes
    path4           += ".bot";  // Footer
    path5           += ".out";  // Data File

    std::string     top, mid, bot, ansibuff, buff;   // Holds all Process Data for Output.
    int             c;          // Char Input on File Reads.

    // Read Top File, if can't open ansi, skip it.
    c = '\0';
    FILE *inStream;
    if ((inStream = fopen(path2.c_str(), "r+")) ==  NULL)
    {
        //elog("Counldn't Open Ansi: %s", path2.c_str());
    }
    else
    {
        while (c != EOF)
        {
            c = getc(inStream);
            if (c != EOF) top += c;
        }
        fclose(inStream);
    }

    // Read Mid File
    c = '\0';
    if ((inStream = fopen(path3.c_str(), "r+")) ==  NULL)
    {
        //elog("Counldn't Open Ansi: %s", path3.c_str());
    }
    else
    {
        while (c != EOF)
        {
            c = getc(inStream);
            if (c != EOF) mid += c;
        }
        fclose(inStream);
    }

    // Read Bot File
    c = '\0';
    if ((inStream = fopen(path4.c_str(), "r+")) ==  NULL)
    {
        //elog("Counldn't Open Ansi: %s", path4.c_str());
    }
    else
    {
        while (c != EOF)
        {
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
    if (datar2->NumOfRecs != 0 && datar2->NumOfRecs < numFileRecs)
    {
        // Reset Which record we start on, and mark for truncation lateron.
        i = numFileRecs - datar2->NumOfRecs;
        //elog("NumOfRecs: %i, numFileRecs: %i, i: %i",datar2->NumOfRecs,numFileRecs,i);
        truncate = TRUE; // Make Sure we Truncate the Extra Records when done.
    }

    // Do so checking on Record Size of Current File, and Truncate if were over and resave it!
    int it = 0;
    if (truncate == TRUE)
    {
        //elog("----Starting Truncate");
        //Re-Write Out File with proper # of records.
        inputArray = new std::string [(datar2->NumOfRecs*inNoc)+1];

        // Set up to Skip First Records Worth of Input.
        it = inNoc;
        //elog("NumOfRecs: %i, numFileRecs: %i, it: %i",datar2->NumOfRecs,numFileRecs,it);
        // Grabbing all input line per record.
        for (int in = 0; in != datar2->NumOfRecs*inNoc; in++)
        {
            //elog("----inputArray[%i]: %s, fileArray[%i]: %s", in,(char *)inputArray[in].c_str(), it,(char *)fileArray[it].c_str());
            inputArray[in] = fileArray[it];
            ++it;
        }
        truncate_da_buffer(); // Remove & Resave File Records.
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

    //elog("Looping Through Mid file for MCI Parsing now...");
    // Parse Each Record / Input, and Replace MCI Code in Mid with Input Data.
    input = 0;
    for (i = 0; i != numFileRecs; i++)      // Parse Each Record
    {
        buff = mid;
        for (int i2 = 0; i2 != inNoc; i2++)     // Parse Each Input Per Rec.
        {
            //elog("Mci: %s, %i,: FileArray %s, %i",(char *)cmdr2[inexecnum[i2]].MciCode.c_str(),i2,(char *)fileArray[input].c_str(),input);
            // Find First MCI Code and Parse / Replace it.
            id1 = buff.find(cmdr2[inexecnum[i2]].MciCode, 0);
            // Process MCI Code
            if (id1 != std::string::npos)   // ie.. &ON
            {
                // parse justify spacing right / left passing in string before
                // replacing mci code. to Properly Space Output Ansi.
                memset(&MCI,0,sizeof(MCI));
                space = 0;
                if (buff[id1+3] == '{')   // Left Justify
                {
                    //elog("left justify: %c%c",buff[id1+4],buff[id1+5]);
                    MCI[0] = buff[id1+4]; // Get first Digit
                    MCI[1] = buff[id1+5]; // Get Second Digit
                    space  = atoi(MCI);
                    foundr = FALSE;
                    foundl = TRUE;
                }
                else if (buff[id1+3] == '}')  // Right Justify
                {
                    //elog("right justify: %c%c",buff[id1+4],buff[id1+5]);
                    MCI[0] = buff[id1+4]; // Get first Digit
                    MCI[1] = buff[id1+5]; // Get Second Digit
                    space  = atoi(MCI);
                    //elog("right justify: %i",space);
                    foundl = FALSE;
                    foundr = TRUE;
                }

                if (space != 0)
                {
                    //elog("Allocating temp for Replace (Size): %i",fileArray[input].size());
                    temp = new char [fileArray[input].size()+(space+1)];
                    if (!temp)
                    {
                        //elog("Unable to Allocate Temp for MCI Parsing: %i",fileArray[input].size());
						return;
                    }
                    // MCI Translation .
                    sprintf(temp,"%s",(char *)fileArray[input].c_str());
                    if (foundl == TRUE)
                    {
                        lspacing(temp,space);
                    }
                    else if (foundr == TRUE)
                    {
                        rspacing(temp,space);
                    }
                }

                //parsemci(cmdr2);

                //MciCode Holds Original Code, fileArray Holds String from File.
                //If we Parsed Justify, then Erase that MCI Code as well.
                (space == 0) ?
                buff.replace(id1,cmdr2[inexecnum[i2]].MciCode.size(),fileArray[input]):
                buff.replace(id1,cmdr2[inexecnum[i2]].MciCode.size()+3,temp);

                if (space != 0)
                {
                    if (temp != 0) delete [] temp;
                    temp = 0;
                }
            }
            ++input;
        }
        //elog("Record: %i",i);
        ansibuff += buff;
        ansibuff += "\r\n";  // Add New Line for each MID Line Parsed.
        buff.erase();
    }
    //elog("Finished Parsing MCI Codes...");
    ansibuff += bot;

    // Then Free The Data.
    if (fileArray != 0) delete [] fileArray;
    fileArray = 0;
    //elog("Freeed [] FileArray");


    data_lockSet(TRUE);
    // now write out ansi file.
    ofstream ostr2;
    ostr2.open( path.c_str(), ofstream::out | ofstream::trunc );
    if (!ostr2.is_open())
    {
        //elog( "* Error Creating: %s", path.c_str());
        return;
    }

    // Write out Complete Ansi.
    ostr2 << ansibuff << endl;
    ostr2.close();
    data_lockSet(FALSE);
    //elog("File: %s created.",(char *)path.c_str());
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
    if (numFileRecs == 0)
    {
        //elog("OutFile has 0 Records: %s", path.c_str());
        return;
    }

    //elog("Allocating FileArray: %i,%d", inNoc,numFileRecs);

    // Number of Input Files * Number or Records in the File.
    fileArray = new std::string[(numFileRecs*inNoc)+1];
    if (!fileArray)
    {
        //elog("Counldn't Allocate File Array %s", path.c_str());
        return;
    }

    data_lockSet(TRUE);
    ifstream ins;
    ins.open( path.c_str() );
    if (!ins.is_open())
    {
        //elog("Counldn't Open Data Area File For Reading %s", path.c_str());
        return;
    }

    //elog("Reading in Records: %i,%d", inNoc,numFileRecs);

    // Loop Though all Records in the File and Read in the Input Strings.
    int iMsg = FALSE;
    for (;;)
    {
        std::getline(ins,data);
        if (data == "<record>") { } // Start of Record
        else if (data == "<input>") getinput = TRUE; // Start of Input
        else if (data == "<message>") iMsg = TRUE; // Start of Input
        else if (data == "</message>")
        {
            iMsg = FALSE; // Start of Input
            ++idx; // Goto Next Line now.
        }
        else if (data == "</input>")
        {
            getinput = FALSE;
            ++in;   // End If Input, Next #
        }
        else if (data == "</record>")
        {
            ++rec;   // End of Record, Next #
            in = 0;  // Reset Input Number;
        }
        else
        {
            // Start Processing of Input Data Here.
            if (getinput == TRUE)
            {
                //Cat String
                if (iMsg == TRUE)
                {
                    fileArray[idx] += data;
                }
                else
                {
                    fileArray[idx] = data;
                    ++idx;
                }
            }
        }
        // Test for Break;
        if (rec != 0 &&                //rec == datar2->NumOfRecs) break; // Limit of Records
            rec == numFileRecs) break; // Read in All Records, Extras Get Truncated.
        if(ins.eof()) break;
    }
    ins.close();
    data_lockSet(FALSE);
    //elog("Records All Read in: %i,%d", inNoc,numFileRecs);
    create_ansifile(); // Now Create The File.
    return;
}


void data_area::truncate_da_buffer()
{

    std::string path = DAPATH;
    path += _curarea;
    path += ".out";

    //elog( "* Truncating, Re-Save Buffer to File: %s", path.c_str());

    data_lockSet(TRUE);
    ofstream ostr;
    ostr.open( path.c_str(), ofstream::out | ofstream::trunc );
    if (!ostr.is_open())
    {
        //elog( "* Error Creating: %s", path.c_str());
        return;
    }

    int input = 0;
    for (int i = 0; i != datar2->NumOfRecs; i++)
    {
        ostr << "<record>"  << endl;
        for (int i2 = 0; i2 != inNoc; i2++)
        {
            ostr << "<input>"  << endl;
            //elog("inputArray[%i]: %s",input, (char *)inputArray[input].c_str());
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

    //elog( "* Saving Buffer to File: %s", path.c_str());

    data_lockSet(TRUE);
    ofstream ostr;
    ostr.open( path.c_str(), ofstream::out | ofstream::app );
    if (!ostr.is_open())
    {
        //elog( "* Error Creating: %s", path.c_str());
        data_lockSet(FALSE);
        return;
    }

    ostr << "<record>"  << endl;
    for (int i = 0; i != inNoc; i++)
    {
        //elog("4. inArray Out[%i]: %s",i,inArray[i].c_str());
        ostr << "<input>"  << endl;
        ostr << inArray[i] << endl;
        ostr << "</input>" << endl;
    }
    ostr << "</record>"  << endl;
    ostr.close();
    data_lockSet(FALSE);
}

// Basic for Now!
int data_area::confirm_da_save(DataRec *cmdr)
{

    char text[1024]= {0};
    char c;

    strcpy(text,cmdr->Prompt.c_str());
    int len = 1;
    inputfield(text,len);
    while (1)
    {
        pipe2ansi(text);
        c = getkey(true);
        if (toupper(c) == 'Y')
        {
            memset(&text,0,sizeof(text));
            sprintf(text,"%c",c);
            pipe2ansi(text);
            return TRUE;
        }
        else if (toupper(c) == 'N')
        {
            memset(&text,0,sizeof(text));
            sprintf(text,"%c",c);
            pipe2ansi(text);
            return FALSE;
        }
        pipe2ansi((char *)"|15|17");
    }
}

// Parse Data Command Keys from Menu Selection
void data_area::darea_docmd(DataRec *cmdr)
{

    unsigned char c1 = cmdr->CmdType[0];
    unsigned char c2 = cmdr->CmdType[1];

    //elog(" 1. darea_docmd() CmdType: %s",(char *)cmdr->CmdType.c_str());
    if (cmdr->CmdType.size() < 1) return;

    // _loadnew = false;
    switch (c1)
    {
    case 'I' : // Input Data Area Commands
        switch (c2)
        {
        case 'N' : // Receive and Insert Into Buffer.
            //elog("2. Receiving IN; %s",(char *)cmdr->CmdType.c_str());

            // Passing MCI Code... Or Getting User Input
            (cmdr->MCICmd == TRUE) ? parsemci(cmdr) : parseinput(cmdr);

            break;
        default :
            break;
        }
        break;

    case 'O' : // Output Data Area Command
        switch (c2)
        {
        case 'T' : // Write Buffer to File, display prompt and confirm save!
            //elog("2. Receiving OT; Write Buffer to File; %s",(char *)cmdr->CmdType.c_str());
            dataSaved = confirm_da_save(cmdr);
            // After Saving, automatically run readin and parse to create Ansi with data.
            // Process and Save
            if (dataSaved == TRUE) save_da_buffer();

            break;

        case 'S' : // Force Save without Prompting for Confirm.
            // This is used more for Last Callers/ Statitcs, and Passing All Values
            // Then having a user input data.
            //elog("2. Receiving OS; Write Buffer to File; %s",(char *)cmdr->CmdType.c_str());
            // After Saving, automatically run readin and parse to create Ansi with data.
            // Process and Save
            save_da_buffer();

            break;
        default :
            break;
        }
        break;

    case 'D' : // Data Area Commands
        switch (c2)
        {
        case 'P' : // Process Area, Read in Data File and Parse AnsiFile
            // Only Process if a Record has been Saved, Save Processing!
            if (dataSaved == TRUE) read_file_records(); // {Default}
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
        switch (c2)
        {
        case 'G' : // Exit Data Area
            //elog("2. Receiving Data_Alive = FALSE; %s",(char *)cmdr->CmdType.c_str());
            Data_Alive = FALSE;
            break;
        default :
            break;
        }
        break;


    case '-' :
        switch (c2)
        {
            /*
                            case '^' : // Change Menu
                                strcpy(_curmenu,mString);
                                _loadnew = true;
                                break;
            */
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

    /*
            // Message Reader Return right away
            case '!' :
                // getting passed through
                break;

            // Data Area Menu Commands
            case 'D' :
                switch (c2) {
                    case 'A' : // Jump into another Data Area
                        da_system(mString);
                    break;

                    default :
                    break;
                }
                break;

            // Matrix Menu Commands
            case '*' :
                switch (c2) {
                    case 'L' : // Login
                        if (logon_system(c2))
                            _loadnew = true;
                        break;

                    case 'A' : // Apply
                        logon_system(c2);
                        break;

                    case 'G' : // Logoff
                        System_Alive = FALSE;

                        // Display Logoff Ansi here!

                        term_option(0); // Put Echo Back on!  Cywing
                        //exit(0);
                        break;

                    default  : // None Found!
                        break;
                }
                break;

            // Message System Commands
            case 'M' :
                switch (c2) {
                    case 'R' : // Message Reader
                        msgread_system(c2);
                        _loadnew = true;
                        break;

                    case 'N' : // NewScan Current Area
                        msgread_system(c2);
                        _loadnew = true;
                        break;

                    case 'M' : // MultiScan All Areas
                        BACKUP_AREA = CURRENT_MAREA;
                        CURRENT_MAREA = 1;
                        urec.lastmbarea = 1;
                        while (1) {
                            ret = _msgf.CountMsgs(CURRENT_MAREA);
                            // Check if this is a new and Empty Base!
                            if (ret != -1) {
                                ret = msgread_system('N');
                                if (!ret) break;
                                total = msg_count();
                                total -= 1;
                            }
                            else {
                                // Show the quick scan of the area.
                                msgread_system('N');
                            }
                            if (CURRENT_MAREA != total)
                                msgarea_change(1); // next area
                            else  {
                                CURRENT_MAREA = BACKUP_AREA;
                                urec.lastmbarea = BACKUP_AREA;
                                _loadnew = true;
                                break;
                            }
                        }
                        CURRENT_MAREA = BACKUP_AREA;
                        urec.lastmbarea = BACKUP_AREA;
                        _loadnew = true;
                        break;

                    case 'P' : // Mesasge FSE Post
                        msgpost_system();
                        break;

                    case 'A' : // Mesasge Area List
                        msgarea_list();
                        break;

                    case '+' : // Next Message Area
                        msgarea_change(1);
                        break;
                    case '-' : // Previous Message Area
                        msgarea_change(2);
                        break;
                    default  : // None Found!
                        break;
                }
                break;

            // Email
            case 'E' :
                switch (c2) {
                    case 'W' : // Write Email
                        write_email();
                        _loadnew = true;
                        break;
                    case 'R' : // Read Email
                        break;

                    case 'N' : // Scan for New
                        break;


                    default:  // None Found!
                        break;
                }
                break;

            // Sysop Commands
            case '%' :
                switch (c2) {
                    case '#' : // Menu Editor
                        //menu_edit();

                        _loadnew = true;
                        break;

                    case 'M' : // Message Forum Editor
                        msgedit_system();
                        break;

                    case 'C' : // Pack Current Mesasge Area
                        msgpack_system(FALSE);
                        break;
                    case 'P' : // Pack All Mesasge Areas
                        msgpack_system(TRUE);
                        break;

                    default  : // None Found!
                        break;
                }
                break;

            case '-' :
                switch (c2) {

                    case '^' : // Change Menu
                        strcpy(_curmenu,mString);
                        _loadnew = true;
                        break;

                    case 'T' : // Display a line of Text
                        pipe2ansi(mString);
                        break;

                    case 'F' : // Display a File
                        ansiPrintf(mString);
                        break;

                    default  : // None Found!
                        break;
                }
                break;

            // None Found!
            default  :
                break;
        }
        */
}


// Testing, Display Each Command & Field Loaded Into Data Area:
void data_area::display_commands ()
{

    int idx = 0;
    while (idx != noc)
    {
        //elog("Desc     %s", cmdr2[idx].Desc.c_str());       // # Description
        //elog("Prompt   %s", cmdr2[idx].Prompt.c_str());     // # Text Prompt To Display on Input Commands.
        //elog("Size     %i", cmdr2[idx].Size);       // # Max Number of Chars on Input on Prompt.
        //elog("CKeys    %s", cmdr2[idx].CKeys.c_str());      // # Accepted Input Keys
        //elog("CmdType  %s", cmdr2[idx].CmdType.c_str());    // # Command Type
        //elog("MString  %s", cmdr2[idx].MString.c_str());    // # Data to be Written {UserName}
        //elog("MciCode  %s", cmdr2[idx].MciCode.c_str());    // # MCI Code in ANSI For this Field
        //elog("HiString %s", cmdr2[idx].HiString.c_str());   // # LightBar High String
        //elog("LoString %s", cmdr2[idx].LoString.c_str());   // # LightBar Low String
        //elog("Xcoord   %i", cmdr2[idx].Xcoord);     // # X Coord of Lightbar
        //elog("Ycoord   %i", cmdr2[idx].Ycoord);     // # Y Coord of Lightbar
        //elog("LBarCmd  %i", cmdr2[idx].LBarCmd);    // # Is This a Lightbar Command?
        //elog("MCICmd   %i", cmdr2[idx].MCICmd);     // # Is This a MCI Code, Skip Input and Translate.
        ++idx;
    }
}

// Menu System Loop.
void data_area::data_loop()
{

    //elog("Starting DataArea, User: %s",thisuser->handle);
    Data_Alive = TRUE;
    data_area_readin();         // Allocate Memory and Read In Area and Commands
    while (Data_Alive)          // Data Area is Active
    {
        process_data_area();    // Process Each Command and User Input
    }
    data_area_clear();          // Free All Memory Allocation.
}

