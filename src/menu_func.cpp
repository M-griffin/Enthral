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

# ifdef HAVE_CONFIG_H
# include <config.h>
# endif

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
# include <sstream>

# include <cstdio>
# include <cstdlib>
# include <ctime>
# include <cstring>
# include <unistd.h>

# include <termios.h>
# include <pthread.h>


using namespace std;

/*
 * Menu System Class
 */
menu_func::menu_func()
{
    _loadnew = false;
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

    CURRENT_AREA = 0;
}


menu_func::~menu_func()
{
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

    return TRUE;
}

/*
 * Parse Commands Reads from a Menu File
 */
void menu_func::cmdparse(std::string cfgdata, int idx)
{
    std::string::size_type id1 = 0;
    char sText[200] = {0};

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

    return ret;
}

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

    int ret;
    ret = cmdexist(MenuName,idx);
    if (ret < 1) return FALSE;

    ifstream iFS;
    iFS.open( path.c_str() );
    if (!iFS.is_open()) {
        return FALSE;
    }

    std::string cfgdata;
    for (;;) {
        std::getline(iFS,cfgdata,'\n');
        cmdparse(cfgdata,idx);
        if(iFS.eof()) break;
    }
    iFS.close();

    return TRUE;
}

/**
 * Loads All Commands into Class.
 */
void menu_func::insert_cmdr()
{
    int  idx = 0;
    while (cmds_read(_curmenu,idx)) {
        ++idx;
    }
}

/**
 * Reset or Set Inital Defaults before a menu is read.
 */
void menu_func::menu_reset()
{
    xx      = 1;        // Holds X Coord
    yy      = 1;        // Holds Y Coord

    noc     = 0;        // Number of Commands.
    iNoc    = 0;        // Holds Lightbar # of choices
    sNoc    = 0;        // Holds Scroll Text # of choices
    cntEscCmds = 0;     // Number of ESC Commands.

    c  = '\0';
    cc = '\0';                      // Hold Input / Lightbar Key
    EscHit = false;                 // Is Input key Escaped char, or Normal Key
    memset(&outBuff,0,sizeof(outBuff));   // Holds Formatted Lightbar Data
    output.erase();                 // Buffer for writing all lightbars at the same time
    sRand = 0;                      // Randmise Text Scrollies
    p = 0;                          // For passing Menu command to new thread
    tScroll = false;
    executed = 0;                   // Test's for hot keys commands excuted, if non pass through loop
    
    // This is so scrolling isn't killed 
    // with invalid menu input!
    exe = 0;
}


/**
 * Check if Menu Exists
 */
int menu_func::menu_exists()
{
    std::string path;
    path = MENUPATH;
    path += _curmenu;
    path += ".txt";

    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(fstr == NULL) {
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
    if (_premenu == _curmenu || _curmenu == "") {
        _loadnew = false;
        return ;
    }

    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(fstr == NULL) {
        sleep(1);
        _loadnew = false;
        return;
    } else {
        fclose(fstr);
    }

    // Setup the Default Setting and And Clear Allocations.
    menu_reset(); // Loading new Menu, Clear!

    if (menur2 != 0) {
        delete menur2;
        menur2 = 0;
    }

    menur2 = new MenuRec;
    if (!menur2) {
        return;
    }

    if(!menu_read(_curmenu))
        return;

    if (menur2->MFlags.UseGlobal) {
        nogc = cnt_cmds((char *)"global");
        if (nogc < 1) nogc = 0;
    } else
        nogc = 0;

    noc = cnt_cmds(_curmenu);

    if (cmdr2 != 0) {
        delete [] cmdr2;
        cmdr2 = 0;
    }

    cmdr2 = new CommandRec[noc+nogc+1];
    if (!cmdr2) {
        return;
    }

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
}


/**
 * Scrolling Text to Display in a seperate thread
 * When a menu command is activated.
 */
void *ScrollingText(void *p1)
{
    PASSING *p = (PASSING*)p1;
    ConsoleIO s;

    // Only run scrolling text if were in ANSI Mode.
    if (isANSI == FALSE || p->Active == false) {
        p->Active = false;
        delete p;
        p = 0;
        return NULL;
    }

    usleep(11*11800);

    unsigned
    short boxlen, loop, xpos = 0, ypos = 0, loop2;
    std::string clear, quotestr, q2, output;
    std::string midcolor;
    std::string sidecolor;
    char outBuff[100];

    // Setup The Scrolling Text
    loop = 0;
    quotestr.clear();
    output.clear();
    q2.clear();

    boxlen = p->cmdr2->STLen;
    if ( boxlen-1 > (int)p->cmdr2->MString.size() ) {
        for (int i = 0; i != boxlen; i++) quotestr += " ";
    }

    xpos = p->cmdr2->Xcoord;
    ypos = p->cmdr2->Ycoord;

    midcolor   = p->cmdr2->HiString;
    sidecolor  = p->cmdr2->LoString;

    quotestr  += p->cmdr2->MString;
    loop2 = quotestr.size();
    quotestr  += quotestr.substr ( 0, boxlen-1 );

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

        usleep(10*11800);
    } while (p->Active);

    delete p;
    p = 0;

    return NULL;
}


/**
 * Oneliner/Rumors (WIP Started)
 * When a menu command is activated.
 */
void *DelayedOneliners(void *p1)
{
    PASSING *p = (PASSING*)p1;
    ConsoleIO s;

    // Only run scrolling text if were in ANSI Mode.
    if (isANSI == FALSE || p->Active == false) {
        p->Active = false;
        delete p;
        p = 0;
        return NULL;
    }

    unsigned
    short boxlen, loop, xpos = 0, ypos = 0, loop2;
    std::string clear, quotestr, q2, output;
    std::string midcolor;
    std::string sidecolor;
    char outBuff[100];

    // Setup The Scrolling Text
    loop = 0;
    quotestr.clear();
    output.clear();
    q2.clear();

    boxlen = p->cmdr2->STLen;
    if ( boxlen-1 > (int)p->cmdr2->MString.size() ) {
        for (int i = 0; i != boxlen; i++) quotestr += " ";
    }

    xpos = p->cmdr2->Xcoord;
    ypos = p->cmdr2->Ycoord;

    midcolor   = p->cmdr2->HiString;
    sidecolor  = p->cmdr2->LoString;

    quotestr  += p->cmdr2->MString;
    loop2 = quotestr.size();
    quotestr  += quotestr.substr ( 0, boxlen-1 );

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

        usleep(10*11800);
    } while (p->Active);

    delete p;
    p = 0;

    return NULL;
}


/**
 * Clear Menu When Done.
 */
void menu_func::menu_clear()
{
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

    // If no Menu Commands Return! / Add fallback menu lateron!!
    if (noc == 0) return;

    if (menur2->Directive != "") {
        ansiPrintf((char *)menur2->Directive.c_str());
    }

    // Count Cmds with lightbar or scrolling flag only and ESC Keys
    int Xc = 0;
    if (menur2->MFlags.EOSPrompt == TRUE) {
        getxy();
        Xc = ansi_getx();
    }

    // Not the Same Menu Loaded, ok to Load New One!
    if (_premenu != _curmenu) {
        if (execnum != 0) {
            delete [] execnum;
            execnum = 0;
        }

        if (execnum == 0) {
            execnum = new short[noc+1];
            if (!execnum) {
                return;
            }
        }

        // Check for Text Scrolling Commands
        if (execnum2 != 0) {
            delete [] execnum2;
            execnum2 = 0;
        }

        if (execnum2 == 0) {
            execnum2 = new short[noc+1];
            if (!execnum2) {
                return;
            }
        }

        // Check for Text Scrolling Commands
        if (execnum3 != 0) {
            delete [] execnum3;
            execnum3 = 0;
        }

        if (execnum3 == 0) {
            execnum3 = new short[noc+1]; // Set to # of comamnd for now, preload.
            if (!execnum3) {
                return;
            }
        }

        iNoc = 0;
        sNoc = 0;
        exe  = 0;
        cntEscCmds = 0;

        for (int i = 0; i != noc; i++) {
            // If were overwridding placement of Row on which we display Lightbars
            // This is Mesage prompts and variables lenghts.
            // Place All Lightbars Accross on Same Horzontal Line!
            if (menur2->MFlags.EOSPrompt == TRUE) {
                cmdr2[i].Ycoord = Xc;
            }

            // If we find a FIRSTCMD, Execute it right away!
            if (cmdr2[i].CKeys == "FIRSTCMD") {
                menu_docmd(&cmdr2[i]);
                ++exe;
            }

            // Get Ligthbar Commands
            if (cmdr2[i].LBarCmd && isANSI == TRUE) {
                execnum[iNoc] = i;
                ++iNoc;
            }

            if (cmdr2[i].SText) {
                execnum2[sNoc] = i;
                ++sNoc;
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
            } else if (cmdr2[i].CKeys == "PAGEUP") {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            } else if (cmdr2[i].CKeys == "PAGEDN") {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }
        }

        // Return if we executed all comamnds as FRISTCMD!
        if (exe == noc) {
            return;
        }

        // Make sure there is more then 1 to Randomize
        if (sNoc > 0) {
            srand(time(NULL));
            sRand = rand()%sNoc;
        }

        _premenu = _curmenu;
    }

    // Start Text Scrolling Thread here...
    if (sNoc > 0 && execnum2[0] != '\0' && (int)execnum2[0] != 19 && isANSI == TRUE) {
        tScroll = true;
        p = new PASSING;
        p->Active = true;
        p->cmdr2 = &cmdr2[execnum2[sRand]];
        pthread_create(&ahThread, NULL, ScrollingText,(void *)p);
    }



    output.clear();
    if (iNoc == 0) {

    } else {
        //Throw out a menu prompt if there is one available. - Add MCI Parsing too!
        // Use Same X Row,  these got reversed somehow!!! grrrr
        xx = 1;
        yy = cmdr2[execnum[0]].Ycoord;
    }

    std::string sMenu = _curmenu;
    while (1) {
        // Lightbar menu then draw
        if (iNoc > 0) {

            // Setup of Remaining Lightbars in Low highlight Form
            for (int rep = 0; rep != iNoc; rep++) {
                if (rep != choice) {
                    xx = cmdr2[execnum[rep]].Xcoord;
                    yy = cmdr2[execnum[rep]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[rep]].LoString.c_str());
                    output += outBuff;
                } else {
                    xx = cmdr2[execnum[rep]].Xcoord;
                    yy = cmdr2[execnum[rep]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[rep]].HiString.c_str());
                    output += outBuff;
                }
            }

            // Write out all the Lightbars * and reset cursor position.
            sprintf(outBuff,"\x1b[%i;79H",yy);
            output += outBuff;

            display_menu();
            pipe2ansi((char*)output.c_str());
        }
        // Not Lightbars, print out menu ansi.
        else {
            pipe2ansi((char*)output.c_str());
            display_menu();
        }

        memset(&EscapeKey,0,sizeof(EscapeKey));

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

        if ((int)c == 27) {
            cc = EscapeKey[1];

            if (cc == '0') {
                cc = EscapeKey[2];
            }

            EscHit = true;
        } else {
            EscHit = false;
        }

        output.clear();

        // If were in prompt and receive Int/Digit,
        // pass through Digits back to Message Quoter && Title Scan Etc...
        if (isdigit(c)) {
            if (menur2->MFlags.DigitPassing == TRUE) {
                inPut[0] = '#';
                inPut[1] = c;
                return;
            }
        }

        executed  = 0;
        if (EscHit) {

            // Run through and check for any Menu commands that overive default passthrough!
            if (cntEscCmds > 0) {
                // ESC Commands in Menu might overide lightbars.
                for (int ckey = 0; ckey != (signed)cntEscCmds; ckey++) {

                    if (EscapeKey[1] == '\0' || EscapeKey[1] == ' ') {
                        if (cmdr2[execnum3[ckey]].CKeys == "ESC") {
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    } else if (cc == 'A') {
                        if (cmdr2[execnum3[ckey]].CKeys == "UP") {
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    } else if (cc == 'B') {
                        if (cmdr2[execnum3[ckey]].CKeys == "DOWN") {
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    } else if (cc == 'C') {
                        if (cmdr2[execnum3[ckey]].CKeys == "RIGHT") {
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    } else if (cc == 'D') {
                        if (cmdr2[execnum3[ckey]].CKeys == "LEFT") {
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }

                    else if ((cc == '5' && EscapeKey[2] == '~') || cc == 'V') {
                        if (cmdr2[execnum3[ckey]].CKeys == "PAGEUP") {
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }

                    else if ((cc == '6' && EscapeKey[2] == '~') || cc == 'U') {
                        if (cmdr2[execnum3[ckey]].CKeys == "PAGEDN") {
                            if (tScroll) p->Active = false;
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }
                }
                // Executed == 0, Then Key Pressed was not valid! :)
                // Pass through the ESC then
                if (tScroll) p->Active = false;
                if (executed == 0) {
                    inPut[0] = cc;

                    if (cc == '0')
                        cc = EscapeKey[2];
                } else {
                    return;
                }
            }

            // If True, means we pass A and B up/dn through to system!
            if (menur2->MFlags.EscPassing == FALSE) {
                // Arrow Keys
                if (cc == 'A') cc = 'D';
                else if (cc == 'B') cc = 'C';
            }

            if (iNoc > 0) {
                if (cc == 'D' && executed == 0) {
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
                    pipe2ansi((char*)output.c_str());
                } else if ((cc == 'H' || (cc == '1' && EscapeKey[2] == '~')) && executed == 0 &&
                           menur2->MFlags.EscPassing == TRUE) {
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;
                    choice = 0;
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[%i;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str(),yy);
                    output += outBuff;
                    pipe2ansi((char*)output.c_str());
                } else if ((cc == 'K' || cc == 'F' || (cc == '4' && EscapeKey[2] == '~')) && executed == 0 &&
                           menur2->MFlags.EscPassing == TRUE) {
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
                    // Pass through the Key.
                    if (tScroll) p->Active = false;
                    inPut[0] = cc;
                    return;
                }
            } //</iNoc>
            else {
                // normal Key Input.
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
                        return;
                    } else {
                        // Redraw screen!
                        if (menur2->Directive != "") {
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
                    return;
                }
            } // </</iNoc>
        } // </EscHit>

        // Normal Key Input
        else {
            // If Enter, Return Cmd # of Lightbar Executed
            // also catch any stacked keys and execute in order!

            if ((int)c == 10 && iNoc > 0) {
                executed = 0;
                for (int ckey = 0; ckey != noc; ckey++) {
                    if (cmdr2[ckey].CKeys == cmdr2[execnum[choice]].CKeys) {
                        if (tScroll) p->Active = false;
                        ++executed;
                        menu_docmd(&cmdr2[ckey]);
                        strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                    }
                }
                if (executed > 0) {
                    return;
                } else {
                    // Redraw screen!
                    if (menur2->Directive != "") {
                        ansiPrintf((char *)menur2->Directive.c_str());
                    }
                }
            } else if ((int)c == 10 && iNoc == 0) {
                // Check for ENTER CKEY!
                executed = 0;
                for (int ckey = 0; ckey != noc; ckey++) {
                    if (cmdr2[ckey].CKeys == "ENTER") {
                        if (tScroll) p->Active = false;
                        ++executed;
                        menu_docmd(&cmdr2[ckey]);
                        strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                    }
                }
                if (executed > 0) {
                    return;
                } else {
                    // Redraw screen!
                    if (menur2->Directive != "") {
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
    if (noc == 0) {
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
            _curmenu = (char *)urec->startmenu;
            CURRENT_MAREA    = 0;
            CURRENT_FAREA    = 0;
            CURRENT_MSGTITLE = 0;
            return 1;
        } else {
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
}

/**
 * Command Execute - For Reading / Scanning all and new messages.
 */
int menu_func::msgscan_system(unsigned char c, char *mString)
{
    msg_read _mread;
    unsigned long Origarea = 0;

    Origarea = urec->lastmbarea;
    urec->lastmsg = 0;

    _mread.start(urec); // Check!
    int ret = TRUE;
    int MiltiScan = FALSE;
    int NewScan = FALSE;

    switch(c) {
    case 'R': // Read Area
        ret = _mread.title_scan(NewScan,MiltiScan,mString);
        break;

    case 'N': // NewScan Current Area
        NewScan = TRUE;
        ret = _mread.title_scan(NewScan,MiltiScan,mString);
        break;

    case 'M': // NewScan All Areas
        NewScan = TRUE;
        MiltiScan = TRUE;
        ret = _mread.title_scan(NewScan,MiltiScan,mString);
        break;
    }

    urec->lastmbarea = Origarea;
    CURRENT_MAREA = Origarea;

    urec->lastmsg = 0;
    return ret;
}

/**
 * Command Execute - Post a Message in current Area
 */
void menu_func::msgpost_system(char *mString)
{
    msg_read _mread;
    _mread.start(urec);

    unsigned long Origarea = 0;
    Origarea = urec->lastmbarea;

    std::stringstream iconvert(mString);
    int i = 0;

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
        _mread.SetupMsgPost();
    }

    CURRENT_MAREA = Origarea;
    urec->lastmbarea = Origarea;
}

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
    msg_area _marea;
    _marea.SetupList(urec);
    _marea.init_db();

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

    while (1) {
        strcpy(rBuffer,"");
        lang_get(text,42);
        inputfield(text,len);
        pipe2ansi(text);

        getline(rBuffer,len);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0) {

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

    time_t ctime;
    ctime = user->dtlaston;
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
        fprintf(fp, "No City Supplied\r\n");
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
        fprintf(fp, "GR\r\n");
        fprintf(fp, "%d\r\n", TERM_HEIGHT);
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

    if ((fp = fopen(file, "w")) == NULL)
        return;
    else {
        fprintf(fp, "%lu\r\n", (ulong)user->num);
        fprintf(fp, "%s\r\n", (char *)user->handle);
        fprintf(fp, "%s\r\n", (char *)user->handle);
        fprintf(fp, "\r\n");
        fprintf(fp, "25\r\n");
        fprintf(fp, "%s\r\n", buffer1 );
        fprintf(fp, "16097.00\r\n");
        memset(&buffer1,0,sizeof(buffer1));
        strftime(buffer1, 26, "%m/%d/%y", time_p);
        fprintf(fp, "%s\r\n",buffer1);
        fprintf(fp, "%i\r\n",TERM_WIDTH);
        fprintf(fp, "%i\r\n",TERM_HEIGHT);
        if (isSysop == TRUE) {
            fprintf(fp, "%d\r\n", 255);
            fprintf(fp, "%d\r\n", 1);
            fprintf(fp, "%d\r\n", 1);
            fprintf(fp, "%d\r\n", 1);
            fprintf(fp, "%d\r\n", 1);
        } else {
            fprintf(fp, "%d\r\n", 50);
            fprintf(fp, "%d\r\n", 0);
            fprintf(fp, "%d\r\n", 0);
            fprintf(fp, "%d\r\n", 1);
            fprintf(fp, "%d\r\n", 1);
        }

        fprintf(fp, "%d\r\n", 2225);
        fprintf(fp, "%s\r\n", ANSIPATH);
        fprintf(fp, "%s\r\n", DATAPATH);
        memset(&buffer1,0,sizeof(buffer1));
        strftime(buffer1, 26, "%y%m%d.log", time_p);
        fprintf(fp, "%s\r\n", buffer1);
        fprintf(fp, "%d\r\n", 19200);
        fprintf(fp, "%d\r\n", 0);
        fprintf(fp, "%s\r\n", SYSTEM_NAME);
        fprintf(fp, "%s\r\n", SYSOP_NAME);
        fprintf(fp, "%d\r\n", 83680);
        fprintf(fp, "%d\r\n", 1);
        fprintf(fp, "%d\r\n", 0);
        fprintf(fp, "%d\r\n", 0);
        fprintf(fp, "%d\r\n", 0);
        fprintf(fp, "%d\r\n", 0);
        fprintf(fp, "8N1\r\n");
        fprintf(fp, "%d\r\n", 19200);
        fprintf(fp, "%d\r\n", 0);
        fclose(fp);
    }

    // Test Setting the Terminal back to Original. before running door.
    tcflush( STDOUT_FILENO, TCIFLUSH);
    if ( tcsetattr( STDOUT_FILENO, TCSADRAIN,&old_termios ) ) {
        pipe2ansi((char *)"	|CS Unable to set old_termios! |PA");
    }

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

    std::string parse;
    char sNode[60] = {0};
    std::string::size_type id1;

    switch (c1) {

    case '!' :
        break;

        // Data Area Menu Commands
    case 'D' :
        switch (c2) {
        case 'A' : // Execute This Data Area
            da_system((char *)cmdr->MString.c_str());
            _loadnew = true;
            break;

        case '-' : //Execute Program/Door
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
            fflush(stdout);
            open_keyboard();
            break;

        case 'L' : //Execute Program Download
            start_external((char *)cmdr->MString.c_str());
            fflush(stdout);
            open_keyboard();
            break;

        case 'P' : //Execute Python Script

# ifdef HAVE_PYTHON
            pybbs_run((char *)cmdr->MString.c_str(), user);
            fflush(stdout);
            open_keyboard();
# endif
            break;

        default :
            break;
        }
        break;

        // Data Area Menu Commands
    case 'U' :
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
        switch (c2) {
        case 'L' : // Login
            ret = logon_system(c2);
            if (ret == 2) System_Alive = FALSE;
            else if (ret == 1) {
                _loadnew = true;
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
            term_option(0);
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

    case '%' :
        // Only Sysop can have these commands execute!
        if (isSysop == TRUE) {
            switch (c2) {
            case 'I' : // File Import
                break;

            case '#' : // Menu Editor
                _loadnew = true;
                break;

            case 'M' : // Message Forum Editor
                msgedit_system();
                break;

            case 'F' : // File Forum Editor
                fileedit_system();
                break;

            case 'C' : // Pack Current Mesasge Area
                break;

            case 'P' : // Pack All Mesasge Areas
                break;

            case 'T' : // Truncate All Message Areas
                break;

            case 'R' : // Truncate Current Area
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
        switch (c2) {

        case '^' : // Change Menu
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

    case 'N' : // Node

        switch (c2) {
        case 'W' : // Whois Online.
            node_message(TRUE);
            break;
        case 'M' : // Node Messages - WIP not yet setup correctly
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
    start_session(urec);
}

/**
 * Menu System - Main System Loop Starts Here.
 */
void menu_func::menu_mainloop(UserRec *user)
{
    urec = user;
    start_session(urec);
    char mString[255]= {0};

    while (System_Alive) {
        menu_readin();
        menu_proc(mString);
    }

    lineolm_login(FALSE);
}
