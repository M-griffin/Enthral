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
# include "language.h"
# include "menu_func.h"
# include "msg_email.h"
# include "users.h"
# include "user_list.h"

# include <fstream>
# include <string>
# include <vector>

# include <cstdio>
# include <ctime>
# include <cctype>
# include <cstring>
# include <cstdlib>
# include <unistd.h>

using namespace std;

/**
 * Email Full Scren List INI - INI Exists?
 */
bool msgemail_ini::msga_exists()
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
    strcpy(sMAX_AREAS,"");

    std::string path = INIPATH;
    path += "msgemail.ini";

    FILE *stream;
    stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        perror("Error unable to read msgemail.ini, check permissions!");
        return false;
    }
    fclose(stream);
    return true;
}

/**
 * Email Full Scren List INI - Create New
 */
void msgemail_ini::msga_create()
{

    std::string name = INIPATH;
    name += "msgemail.ini";

    ofstream outStream2;
    outStream2.open( name.c_str(), ofstream::out | ofstream::trunc );
    if (!outStream2.is_open()) {
        printf( "\nError Creating: %s \n", name.c_str());
        return;
    }

    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | " << BBSVERSION << "           |" << endl;
    outStream2 << "# | Message Area List Text Box Coords                              |" << endl;
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

/**
 * Email Full Scren List INI - Get INI Values
 */
void msgemail_ini::msga_check(std::string cfgdata)
{
    std::string temp = cfgdata;
    std::string::size_type id1 = 0;

    // Disgards any Config lines with the # Character
    if (temp[0] == '#') return;

    // Sets if LOGGING is on / off
    id1 = temp.find("set TOP ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        id1 = atoi(temp1.c_str());
        iTop = id1;
        return;
    }

    // Sets Download Path
    id1 = temp.find("set BOT ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        id1 = atoi(temp1.c_str());
        iBot = id1;
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set PAGENUM ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sPAGENUM,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set PAGETOTAL ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sPAGETOTAL,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set MOREUP ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sMOREUP,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set MOREUP_CHAR ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sMOREUP_CHAR,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set MOREDOWN ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sMOREDOWN,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set MOREDOWN_CHAR ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sMOREDOWN_CHAR,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set MOREMSG_ON ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sMOREMSG_ON,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set MOREMSG_WORD_ON ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sMOREMSG_WORD_ON,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set MOREMSG_OFF ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sMOREMSG_OFF,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set MOREMSG_WORD_OFF ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sMOREMSG_WORD_OFF,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set INPUT_BOX ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sINPUT_BOX,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set TEXT_COLOR ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sTEXT_COLOR,(char *)temp1.c_str());
        return;
    }

    // Sets XY PAGE Num
    id1 = temp.find("set MAX_AREAS ", 0);
    if (id1 != std::string::npos) {
        std::string temp1;
        std::string::size_type st1 = 0;
        std::string::size_type st2 = 0;
        std::string::size_type  ct = 0;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        strcpy(sMAX_AREAS,(char *)temp1.c_str());
        return;
    }
}

/**
 * Email Full Scren List INI - Read and Parse INI File.
 */
void msgemail_ini::msga_parse()
{

    std::string name = INIPATH;
    name += "msgemail.ini";

    ifstream inStream;
    inStream.open( name.c_str() );
    if (!inStream.is_open()) {
        perror("Error unable to parse msgemail.ini, check permissions!");
        return;
    }

    std::string cfgdata;
    for (;;) {
        getline(inStream,cfgdata);
        msga_check(cfgdata);
        if(inStream.eof()) break;
    }
    inStream.close();
    return;
}

/**
 * Email Full Scren List - Class Init
 */
msg_email::msg_email()
{

    tTop     = 1;
    tBot     = 1;
}

/**
 * Email Full Scren List - Parse ANSI Tempalte File
 */
void msg_email::ParseHeader(char *filename)
{

    int c = 0;
    std::string temp;
    std::string path = ANSIPATH;
    path += filename;
    path += ".ans";
    // Get Filesize to Allocate Buffer
    FILE *inStream;

    mb_list_rec mr;

    // Open and Read Ansi file
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL) {
        return;
    }

    char MCI[3] = {0}; // Holds MCI Codes to Parse
    temp = "";   // Holds Ansi
    do {
        memset(&MCI,0,sizeof(MCI));
        c = getc(inStream);
        if (c == '%') {
            MCI[0] = getc(inStream);
            MCI[1] = getc(inStream);

            read_mbaselist(&mr, thisuser->lastmbarea);

            if (strcmp(MCI,"MA") == 0) {
                temp += (char *)mr.mbdisplay;
            }

            /*
            if (strcmp(MCI,"TI") == 0)      { temp += mHead.time;       }
            else if (strcmp(MCI,"FM") == 0) { temp += mHead.from;       }
            else if (strcmp(MCI,"TO") == 0) { temp += mHead.to;         }
            else if (strcmp(MCI,"FL") == 0) { temp += mHead.flags;      }
            else if (strcmp(MCI,"MA") == 0) { temp += mHead.area;       }
            else if (strcmp(MCI,"SU") == 0) { temp += mHead.subj;       }
            else if (strcmp(MCI,"CM") == 0) { temp += mHead.curmsg;     }
            else if (strcmp(MCI,"HM") == 0) { temp += mHead.totmsg;     }
            else { temp += c; temp += MCI; }
            */
            c = getc(inStream);
            if (c != EOF) temp += c;
        } else if (c == '\n') temp += '\r';
        else {
            if (c != EOF) temp += c;
        }
    } while (c != EOF);
    fclose(inStream);

    temp += "\n"; // Extra Space to Fix Next Ansi Sequence
    write(0,(char *)temp.c_str(),temp.size());
}

/**
 * Email Full Scren List - Init Startup
 */
void msg_email::SetupList(UserRec *user)
{
    // Do quick Message Quoter ini parsing
    if (!msga_exists()) {
        perror("Error unable to open msgemail.ini, check if it exists and permissions!");
        //msga_create();
    }
    msga_parse();
    tTop = iTop;
    tBot = iBot;

    thisuser = user;

}


/*
   NOT USED ANYMORE!
   I beleive i combined the User List with Email List into One.
   Check this!! :)

   I beleive this still points to Message Area Files!!

*/
/**
 * Email Full Scren List - Start Interface
 */
char *msg_email::StartList()
{

    return NULL;

    bool more 		 = false;
    bool showmore    = false;
    char mString[10] = {0};
    unsigned char ch;
    std::string _output;
    char outBuffer[200];

    // Startup Message Reader Link List for Holding Message Lines
    language    _lang;
    menu_func 	_mnuf;
    msg_readll 	mLink;

    mLink.initList(sTEXT_COLOR);

    mLink.Top = tTop;
    mLink.Bot = tBot;

    // Mainly for Quoting Text Selection.
    char rBuffer[100]= {0};
    char text[100]= {0};
    char sNum[3]= {0};
    std::string rBuff;
    int id1; //id2,
//    int num, num2;
    int len;

    std::string tmp;
    UserRec trec;

    // Class to Builder User Listing.
    usr_list _usr_list;

    // For Next / Prev Message Areas
    int CurrentPage  = 0;
//    int PreviousPage = 0;
    int bSize        = 0;
    int boxsize      = 0;

    bSize = tBot - tTop;
    ++bSize;

//    int Page;
//    int Tot;
//    int TotPages;

    CURRENT_ULIST = 0;
    vector<list_bar> result;

    errlog((char *)"Build Email UserList - UserID %lu, %lu",thisuser->idx, thisuser->handle);
    result = _usr_list.build_userlist(thisuser, tmp); //, qry);

    // Run through Main Reader Loop until exit from user - This Loop Probably not needed anymore.
    while (1) {
        ParseHeader((char *)"marea");	    // Display Reader Ansi
        //mLink.PutBuffer(OrgMsg, FALSE);	// Send the Original Message into list Buffer
        //mLink.box_start(CURRENT_MAREA);  	// Draw Message Inside of Box

        boxsize =  mLink.Bot - mLink.Top;
        CurrentPage = CURRENT_ULIST / boxsize;

        //  Make sure we have areas.
        if (result.size() > 0) {
            // Update Here with new user list parsing code.
            // mLink.PutBuffer((char *)tmp.c_str(), FALSE);  // Send the Original Message into list Buffer
            errlog((char *)"ulbox_start usr_list");
            mLink.box_start_vector(CurrentPage,CURRENT_ULIST);  // Draw Message Inside of Box
        } else {
            return 0L;
        }

        tmp.erase();
        //UserList(tmp);
        mLink.Tot = result.size();

        while(1) {
            _output.erase();
            more = false;
            showmore = false;


            if (mLink.line_count() > 0) more = true;
            /*-------------------------------------------------------*/
            // Show Down Arrow More!
            if (more) {
                sprintf(outBuffer,"%s\x19",sMOREDOWN); 	// On
                showmore = true;
            } else {
                sprintf(outBuffer,"%s ",sMOREDOWN);		// Off
            }
            _output += outBuffer;

            // Show up Arrow More
            if (mLink.Page > 1) {
                sprintf(outBuffer,"%s\x18",sMOREUP);
                showmore = true;
            } else {
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

            // # of Message Areas
            sprintf(outBuffer,"%s%d",sMAX_AREAS,mLink.Tot);
            _output += outBuffer;

            /*-------------------------------------------------------*/
            pipe2ansi((char *)_output.c_str());
            // If more, Select Menu Prompt with PGDN as Default
            // Otherwise Select Prompt with Next as Default!


            // Make Msgqp2 Prompt Optional, Use if exists!
            // Also Let user Toggle on / off
            _mnuf._premenu.clear();
            _mnuf._premenu = _mnuf._curmenu;
            if ( _mnuf.cmdexist((char *)"msg_alist2",0) ) {
                if (more || mLink.Page > 1) {
                    _mnuf._curmenu.clear();
                    _mnuf._curmenu = "msg_alist2";
                } else {
                    _mnuf._curmenu.clear();
                    _mnuf._curmenu = "msg_alist";
                }
            } else {
                _mnuf._curmenu.clear();
                _mnuf._curmenu = "msg_alist2";
            }


// user list stats won't change,
// cut off here to re-display faster
//JMPINPUT:

            // Draw Input Box
            memset(&text,0, sizeof(text));
            strcpy(text,sINPUT_BOX);
            len = 14;
            inputfield(text,len);
            _output = text;
            pipe2ansi((char *)_output.c_str());

            // Readin the Menu Prompt
            _mnuf.menu_readin();
            _mnuf.menu_proc(mString);
            ch = mString[1];

            // For Menu CmdKey Input
            if (mString[0] == '!') {
                switch (toupper(ch)) {
                case 'Q': // Quit Message Reading
                    mLink.dispose_list();
                    _mnuf.~menu_func();
                    return NULL; //qBuf;

                case 'U': // Page UP
                    mLink.box_pgup();
                    break;

                case 'D': // Page Down
                    mLink.box_pgdn();
                    break;

                case 'C': // Clear All Quoted Text.
                    mLink.clear_all();
                    //ansiPrintf("mquote");
                    mLink.box_refresh(CURRENT_MAREA);
                    break;

                default :
                    break;
                } // end of case
            } else if (mString[0] == '#') {
                // received Digit Input From Menu Prompt
                memset(&rBuffer,0, sizeof(rBuffer));
                memset(&text,0, sizeof(text));
                strcpy(text,sINPUT_BOX);
                len = 14;
                inputfield(text,len);
                pipe2ansi(text);
                sprintf(sNum,"%c",ch);
                getline(rBuffer,len,sNum);
//                num = 0;
                //num2 = 0;
                // Catch any invalid input
                id1 = atoi(rBuffer);

                if (id1 > mLink.Tot || id1 == 0) {
                    // Redraw input box
                    memset(&text,0, sizeof(text));
                    strcpy(text,sINPUT_BOX);
                    len = 14;
                    inputfield(text,len);
                    pipe2ansi(text);
                    mLink.box_refresh(CURRENT_MAREA);
                } else {
                    CURRENT_MAREA = id1-1;
                    thisuser->lastmbarea = CURRENT_MAREA;

                    mLink.dispose(); //clear_all();

                    // Redraw Box with new Area Selection.
                    memcpy(&trec,thisuser,sizeof(UserRec));
//				    ParseMArea(&trec, tmp);
                    mLink.PutBuffer((char *)tmp.c_str(), FALSE);
                    mLink.box_refresh(CURRENT_MAREA);

                    // Redraw input box
                    memset(&text,0, sizeof(text));
                    strcpy(text,sINPUT_BOX);
                    len = 14;
                    inputfield(text,len);
                    pipe2ansi(text);
                    mLink.box_refresh(CURRENT_MAREA);

                    // Display Area Chaned, then exit back to menu.
                    memset(&text,0, sizeof(text));
                    _lang.lang_get(text,27);
                    pipe2ansi(text);

                    // Clean and Exit!
                    mLink.dispose_list();
                    return NULL;
                }
            } else {
                // For Escaped Key Input
                ch = mString[0];
                switch (toupper(ch)) {
                case 'A':
                    mLink.box_pgup();
                    break;

                case 'B':
                    mLink.box_pgdn();
                    break;

                default :
                    break;
                }
            }
        }
    }
}
