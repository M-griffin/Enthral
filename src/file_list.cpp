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
# include "msg_readll.h"
# include "menu_func.h"
# include "file_list.h"

# include <cstdio>
# include <cstring> // gcc 4.3
# include <cstdlib> // gcc 4.3
# include <ctime>
# include <cctype>
# include <fstream>
# include <string>

using namespace std;

/*
bool filelist_ini::filelist_exists()
{
    iTop = 0;
    iBot = 0;

    strcpy(sPAGENUM,"");
    strcpy(sPAGETOTAL,"");
    strcpy(sMOREUP,"");
    strcpy(sMOREUP_CHAR,"");
    strcpy(sMOREDOWN,"");
    strcpy(sMOREDOWN_CHAR,"");
    strcpy(sMOREMSG_ON,"");
    strcpy(sMOREMSG_WORD_ON,"");
    strcpy(sMOREMSG_OFF,"");
    strcpy(sMOREMSG_WORD_OFF,"");
    strcpy(sINPUT_BOX,"");
    strcpy(sTEXT_COLOR,"");
    strcpy(sTEXT_HILIGHT,"");
    strcpy(sQUOTE_RANGE,"");

    std::string path = INIPATH;
    path += "filelist.ini";

    FILE *stream;
    stream = fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        return false;
    }
    fclose(stream);
    return true;
}

void filelist_ini::filelist_create()
{

    std::string name = INIPATH;
    name += "filelist.ini";

    ofstream outStream2;
    outStream2.open( name.c_str(), ofstream::out | ofstream::trunc );
    if (!outStream2.is_open())
    {
        printf( "\nError Creating: %s \n", name.c_str());
        return;
    }

    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | " << BBSVERSION << "       rBuffer    |" << endl;
    outStream2 << "# | filelist Box Coords                                 |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | Note : This file is regenerated with defaults if missing.      |" << endl;
    outStream2 << "# | Note : Any Lines starting with # will be ignored!              |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << endl;
    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | Set the TOP Line # for the Top Margin                          |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "set TOP \"5\"" << endl;
    outStream2 << endl;
    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | Set the BOTTOM Line # for the Bottom Margin                    |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "set BOT \"22\"" << endl;
    outStream2 << endl;
    outStream2.close();
    return;
}

void filelist_ini::filelist_chkpar(std::string &data)
{

    std::string temp1;
    int st1 = -1;
    int st2 = -1;
    signed int  ct = -1;

    st1 = data.find('"', 0);
    st2 = data.find('"', st1+1);
    ++st1;
    temp1 = data.substr(st1,st2);
    ct = st2 - st1;
    if (temp1.length() > ct)
        temp1.erase(ct,temp1.length());
    data = temp1;
}

void filelist_ini::filelist_check(std::string cfgdata)
{

    std::string temp = cfgdata;
    int id1 = -1;

    // Disgards any Config lines with the # Character
    if (temp[0] == '#') return;

    // Sets if LOGGING is on / off
    id1 = -1;
    id1 = temp.find("set TOP ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        id1 = atoi(temp.c_str());
        iTop = id1;
        return;
    }

    // Sets Download Path
    id1 = -1;
    id1 = temp.find("set BOT ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        id1 = atoi(temp.c_str());
        iBot = id1;
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set PAGENUM ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sPAGENUM,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set PAGETOTAL ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sPAGETOTAL,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set MOREUP ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sMOREUP,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set MOREUP_CHAR ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sMOREUP_CHAR,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set MOREDOWN ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sMOREDOWN,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set MOREDOWN_CHAR ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sMOREDOWN_CHAR,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set MOREMSG_ON ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sMOREMSG_ON,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set MOREMSG_WORD_ON ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sMOREMSG_WORD_ON,(char *)temp.c_str());
        return;
    }

// Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set MOREMSG_OFF ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sMOREMSG_OFF,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set MOREMSG_WORD_OFF ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sMOREMSG_WORD_OFF,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set INPUT_BOX ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sINPUT_BOX,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set TEXT_COLOR ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sTEXT_COLOR,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set TEXT_HILIGHT ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sTEXT_HILIGHT,(char *)temp.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = -1;
    id1 = temp.find("set QUOTE_RANGE ", 0);
    if (id1 != -1)
    {
        filelist_chkpar(temp);
        strcpy(sQUOTE_RANGE,(char *)temp.c_str());
        return;
    }
}

void filelist_ini::filelist_parse()
{

    std::string name = INIPATH;
    name += "filelist.ini";

    ifstream inStream;
    inStream.open( name.c_str() );
    if (!inStream.is_open())
    {
        printf("Couldn't Open Config File: %s\n", name.c_str());
        return;
    }

    std::string cfgdata;
    for (;;)
    {
        getline(inStream,cfgdata);
        filelist_check(cfgdata);
        if(inStream.eof()) break;
    }
    inStream.close();
    return;
}



filelist::filelist()
{

    tTop    = 1;
    tBot    = 1;
}

void filelist::Parsefilelist(char *filename)
{

    int c = 0;
    std::string temp;
    std::string path = ANSIPATH;
    path += filename;
    path += ".ans";
    // Get Filesize to Allocate Buffer
    FILE *inStream;
    int id1;

    // Open and Read Ansi file
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL)
    {
        return;
    }

    char MCI[3] = {0}; // Holds MCI Codes to Parse
    temp = "";   // Holds Ansi
    do
    {
        memset(&MCI,0,sizeof(MCI));
        c = getc(inStream);
        if (c == '%')
        {
            MCI[0] = getc(inStream);
            MCI[1] = getc(inStream);

           //ie example...
            if (strcmp(MCI,"TI") == 0)      { temp += mHead.time;       }
            else if (strcmp(MCI,"FM") == 0) { temp += mHead.from;       }

            c = getc(inStream);
            if (c != EOF) temp += c;
        }
        else if (c == '\n') temp += '\r';
        else
        {
            if (c != EOF) temp += c;
        }
    }
    while (c != EOF);
    fclose(inStream);

    temp += "\n"; // Extra Space to Fix Next Ansi Sequence

    do
    {
        id1 = temp.find("\x1b[2J",0);
        if (id1 != -1)
        {
            temp.replace(id1,4,"|CS");
        }
    }
    while(id1 != std::string::npos);

    id1 = write(0,(char *)temp.c_str(),temp.size());
}

void filelist::setup_filelist()
{

    // Do quick Message Quoter ini parsing
    if (!filelist_exists())
    {
        filelist_create();
    }
    filelist_parse();
    tTop = iTop;
    tBot = iBot;
}

// Insert Lines into Quoters Link List.
void filelist::insert_lines(LineRec *orgLink, msg_readll *mLink)
{

    int count = 1;
    int id1   = 0, id2 = 0;
    LineRec *lineTmp;

    if(orgLink->dn_link == orgLink)
    {
        return;
    }
    else
    {
        lineTmp = orgLink;
        while ( lineTmp->dn_link != orgLink )
        {
            lineTmp = lineTmp->dn_link;
            // Don't need to Remove color Codes.
            while (1)    // Remove any PIPE Color Codes in Quoting.
            {
                id1 = lineTmp->data.find("|", 0);
                if (id1 != -1 && isdigit(lineTmp->data[id1+1]) && isdigit(lineTmp->data[id1+2]))
                    lineTmp->data.erase(id1,3);
                else break;
            }

            while (1)    // Remove any ANSI Color Codes
            {
                id1 = lineTmp->data.find("\x1b[", 0);
                if (id1 != -1)
                {
                    id2 = lineTmp->data.find("m",id1+3);
                    lineTmp->data.erase(id1,(id2 - id1)+1);
                }
                else break;
            }

            mLink->add_to_list(lineTmp->data);
            mLink->current_node->lineNum = count;
            ++count;
        }
        return;
    }
}



void filelist::par_qstring(char *rBuffer, msg_readll *mLink)
{

    char cBuff2[20]= {0};
    char cBuff3[20]= {0};
    std::string rBuff;
    std::string rTmp, rTmp2;
    unsigned int id1, id2;
    unsigned int num, num2;

    num  = 0;
    num2 = 0;
    while (1)
    {
        rBuff = rBuffer;
        id1   = rBuff.find("-",0);
        id2   = rBuff.find(",",0);
        // Check if Range and Single Line Selection Are both Present
        if (id1 != -1 && id2 != -1)
        {
            // Chop up String between each ',' char... then process seperatly.
            num2 = id2;
            memset(&cBuff2,0, sizeof(cBuff2));
            for (int i = 0; i != id2; i++)
            {
                cBuff2[i] = rBuffer[i];
            }

            // Remove Substring from original, Then Process what we parsed.
            rBuff.erase(0,num2+1);
            strcpy(rBuffer,rBuff.c_str());

            // Start Seperation 1-6
            rBuff = cBuff2;
            id1 = rBuff.find("-",0);
            id2 = rBuff.find(",",0);
            strcpy (cBuff3,cBuff2);

            if (id1 != -1)    // Range ie 1-2
            {
                num = id1;
                memset(&cBuff2,0, sizeof(cBuff2));
                for (int i = 0; i != num; i++)
                {
                    cBuff2[i] = cBuff3[i];
                }
                id1 = atoi(cBuff2);
                if (id1 > mLink->Tot || id1 == 0) break;
                memset(&cBuff2,0, sizeof(cBuff2));
                for (int i = 0; cBuff3[num+1] != '\0'; i++)
                {
                    ++num;
                    cBuff2[i] = cBuff3[num];
                }
                id2 = atoi(cBuff2);
                if (id2 > mLink->Tot || id2 < id1) break;
                for (int i = id1; i != id2+1; i++)
                {
                    mLink->flag_line(i);
                }
            }
            else    // Else single Digit
            {
                id1 = atoi(cBuff3);
                if (id1 > mLink->Tot || id1 == 0) break;
                mLink->flag_line(id1);
            }
            // Done, Loop Though and Process next part of Original String.
        }
        // Then check if only Range ... 1-6 - Working!
        else if (id1 != -1)   // id1 = '-' Position
        {
            num = id1;
            memset(&cBuff2,0, sizeof(cBuff2));
            for (int i = 0; i != num; i++)
            {
                cBuff2[i] = rBuffer[i];
            }
            id1 = atoi(cBuff2);
            if (id1 > mLink->Tot || id1 == 0) break;

            memset(&cBuff2,0, sizeof(cBuff2));
            for (int i = 0; rBuffer[num+1] != '\0'; i++)
            {
                ++num;
                cBuff2[i] = rBuffer[num];
            }
            id2 = atoi(cBuff2);
            if (id2 > mLink->Tot || id2 < id1) break;

            for (int i = id1; i != id2+1; i++)
            {
                mLink->flag_line(i);
            }
            break;
        }
        // Then check if only Selections 1,6 - Working!
        else if (id2 != -1)     // id2 = ',' Position
        {
            num2 = id2;
            num  = id2;
            memset(&cBuff2,0, sizeof(cBuff2));
            for (int i = 0; i != num; i++)
            {
                cBuff2[i] = rBuffer[i];
            }
            id1 = atoi(cBuff2);

            // Erase Char then comma from original and cntinue, ie: 2,
            id2 = rBuff.find(",",0);
            if (id2 != -1)
            {
                rBuff.erase(0,id2+1);
                strcpy(rBuffer,rBuff.c_str());
            }

            if (id1 > mLink->Tot || id1 == 0) break;
            mLink->flag_line(id1);
            if (id2 == -1)  break;
        }
        else   // Single Digit Left Over. & Done
        {
            id1 = atoi(rBuffer);
            if (id1 > mLink->Tot || id1 == 0) return;
            mLink->flag_line(id1);
            return;
        }
    }
}

void filelist::Startfilelist(LineRec *orgLink, std::string &retbuf)
{

    bool more        = false;
    bool showmore    = false;
    char mString[10] = {0};
    unsigned char ch;
    std::string _output;
    char outBuffer[200];

    // Startup Message Reader Link List for Holding Message Lines
    menu_func   _mnuf; // Menu System for Lightbars
    msg_readll  mLink; // Link List for Holding Message Quote Data

    mLink.initQuote(sTEXT_COLOR,sTEXT_HILIGHT,TRUE);
    mLink.Top = tTop;
    mLink.Bot = tBot;

    // Mainly for Quoting Text Selection.
    char rBuffer[100] = {0};
    char text[100]    = {0};
    char sNum[20]     = {0};
    std::string rBuff;
    std::string rTmp, rTmp2;
    unsigned long id1, id2;
    int len;

    // Quote Buffer to return to FSE.
    // We Return
    //static char qBuf[BUFLEN];
    //memset(&qBuf,0,BUFLEN);

    std::string qbuf;
    qbuf.erase();

    insert_lines(orgLink,&mLink);   // Take Original List and Copy it to Message Quoter

    // Run through Main Reader Loop until exit from user - This Loop Probably not needed anymore.
    while (1)
    {
        // Setup a new Message Quoter ansi
        ansiPrintf((char *)"mquote");
        mLink.box_refresh(CURRENT_FAREA); // So Always Displayed Highlighting on Current Page!

        while(1)
        {
            _output = "";
            more = false;
            showmore = false;
            if (mLink.Page != mLink.TotPages) more = true;


            // Show Down Arrow More!
            if (more)
            {
                sprintf(outBuffer,"%s\x19",sMOREDOWN);  // On
                showmore = true;
            }
            else
            {
                sprintf(outBuffer,"%s ",sMOREDOWN);     // Off
            }
            _output += outBuffer;

            // Show up Arrow More
            if (mLink.Page > 1)
            {
                sprintf(outBuffer,"%s\x18",sMOREUP);
                showmore = true;
            }
            else
            {
                sprintf(outBuffer,"%s ",sMOREUP);
            }
            _output += outBuffer;

            if (showmore)
                sprintf(outBuffer,"%s%s",sMOREMSG_ON,sMOREMSG_WORD_ON);
            else
                sprintf(outBuffer,"%s%s",sMOREMSG_OFF,sMOREMSG_WORD_OFF);
            _output += outBuffer;

            // Show Current/Total Pages
            sprintf(outBuffer,"%s%.2d",sPAGENUM,mLink.Page);
            _output += outBuffer;
            sprintf(outBuffer,"%s%.2d",sPAGETOTAL,mLink.TotPages);
            _output += outBuffer;
            // Max Quote Range
            sprintf(outBuffer,"%s%d",sQUOTE_RANGE,mLink.Tot);
            _output += outBuffer;


            pipe2ansi((char *)_output.c_str());
            // If more, Select Menu Prompt with PGDN as Default
            // Otherwise Select Prompt with Next as Default!

            _mnuf._premenu.clear();
            _mnuf._premenu = _mnuf._curmenu;

            // Make Msgqp2 Prompt Optional, Use if exists!
            // Also Let user Toggle on / off
            if (_mnuf.cmdexist((char *)"msg_qprompt2",0) //&& thisuser->msgp2 )
            {
                if (more || mLink.Page > 1)
                {
                    _mnuf._curmenu.clear();
                    _mnuf._curmenu = "msg_qprompt2";
                }
                else
                {
                    _mnuf._curmenu.clear();
                    _mnuf._curmenu = "msg_qprompt";
                }
            }
            else
            {
                _mnuf._curmenu.clear();
                _mnuf._curmenu = "msg_qprompt";
            }

            // Readin the Menu Prompt
            _mnuf.menu_readin();
            // Process Messeage Promt with lightbar menu
            _mnuf.menu_proc(mString);
            // Process Return Input from Lightbars, Next, Prev, Quit ...
            //mString[0] should = !
            ch = mString[1];

            // For Menu CmdKey Input
            if (mString[0] == '!')
                switch (toupper(ch))
                {

                case 'Q': // Quit Message Reading
                    //strcpy(qBuf,mLink.GetBuffer());
                    mLink.GetBuffer(retbuf);
                    mLink.dispose_list();
                    return;

                case 'U': // Page UP
                    mLink.box_pgup();
                    break;

                case 'D': // Page Down
                    mLink.box_pgdn();
                    break;

                case '?': // Help Screen with Commands
                    ansiPrintf((char *)"mreadhlp");
                    getkey(true);
                    ansiPrintf((char *)"mquote");
                    //mLink.box_refresh();
                    mLink.box_overdraw(CURRENT_FAREA);
                    break;

                case 'A': // Select all Text To Quote
                    mLink.select_all();
                    //mLink.box_refresh();
                    mLink.box_overdraw(CURRENT_FAREA);
                    break;

                case 'C': // Clear All Quoted Text.
                    mLink.clear_all();
                    //mLink.box_refresh();
                    mLink.box_overdraw(CURRENT_FAREA);
                    break;

                default :
                    break;
                } // end of case
            else if (mString[0] == '#')
            {
                // received Digist Input From Menu Prompt, Start Input for Quoting Text!
                memset(&rBuffer,0, sizeof(rBuffer));
                memset(&text,0, sizeof(text));
                strcpy(text,sINPUT_BOX);
                len = 8;
                inputfield(text,len);
                pipe2ansi(text);

                sprintf(sNum,"%c",ch);
                getline(rBuffer,len,sNum,FALSE,(char *)"1234567890,-");

                rBuff = rBuffer;
                // For # & ENTER, Then DIGIT & ENTER Again.
                id1 = rBuff.find("-",0);
                id2 = rBuff.find(",",0);

                // If a Single #, Check if Valid
                if (id1 == std::string::npos && id2 == std::string::npos)
                {
                    id1 = atol(rBuffer);
                    if (id1 > mLink.Tot || id1 == 0 || strlen(rBuffer) > 3)
                    {
                        ansiPrintf((char *)"mquote");
                        mLink.box_refresh(CURRENT_FAREA);
                        break;
                    }
                }

                // Remove any invalid junk at the end,
                while (1)
                {
                    if (rBuff[rBuff.size()-1] == '-' || rBuff[rBuff.size()-1] == ',')
                    {
                        rBuff.erase(rBuff.size()-1,1);
                        sprintf(rBuffer,"%s",(char *)rBuff.c_str());
                    }
                    else
                    {
                        break;
                    }
                }

                // Chop Up and Test Quote Flaging String.
                par_qstring(rBuffer,&mLink);
                break;
            }
            else    // For Escaped Key Input
            {
                ch = mString[0];
                switch (toupper(ch))
                {
                case 'A':
                    // Scroll Up 1 Line ( forced to paeg up / down for now)
                    // Don't care much for single scrolling, anoying online!
                    // Page is much nicer and faster ;)
                    mLink.box_pgup();
                    break;

                case 'B': // Scroll down 1 Line ( forced to paeg up / down for now)
                    mLink.box_pgdn();
                    break;

                default :
                    break;
                }
            }
        }
    }
}

*/
