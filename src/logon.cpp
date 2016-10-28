/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Logon & New User Application Routines                        *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// Enthral SVN: $Id: logon.cpp 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/logon.cpp $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# include "struct.h"
# include "logon.h"

# include "dtfunc.h"
# include "language.h"
# include "menu_func.h"
# include "users.h"
# include "history.h"
# include "node.h"
# include "msgs.h"

# include <cstdio>
# include <cstring> //gcc 4.3
# include <cstdlib> //gcc 4.3
# include <unistd.h> // gcc 4.7

# include <string>
# include <openssl/evp.h>

# define ulong unsigned long

using namespace std;


int  isSysop = FALSE;
int  isANSIMATION = TRUE;

/**
 * Logon Class for Checking User Data
 */
logon::logon()
{
    useScript     = FALSE;
    InvalidLogins = 0;
}

/**
 * Check Login Name
 */
void logon::Handle(UserRec *u)
{

    char rBuffer[1014] = {0};
    char text[1024]    = {0};
    char text2[1024]   = {0};
    int  len = sizeof(u->handle)-1;

    while (1)
    {
        memset(&text,0,sizeof(text));
        lang_get(text,8);
        inputfield(text,len);
        pipe2ansi(text);
        getline(rBuffer,len);
        if (strcmp(rBuffer,"")         != 0 &&
                strcmp(rBuffer,"0")    != 0 &&
                strcmp(rBuffer,"\n")   != 0 &&
                atoi(rBuffer)          ==  0)
        {

            // Check if already exists
            if(!idx_match(rBuffer))
            {
                strcpy((char *)u->handle, rBuffer);
                break;
            }
            else   // Incorrect Handle
            {
                lang_get(text2,18);
                pipe2ansi(text2);
            }
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }
}

/**
 * New User - Get Name
 */
void logon::Name(UserRec *u)
{

    char rBuffer[1024]= {0};
    char text[1024]= {0};
    lang_get(text,9);
    int len = sizeof(u->name)-1;
    inputfield(text,len);
    pipe2ansi(text);
    while(1)
    {
        // Ask for Users Real Name
        getline(rBuffer,len);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {
            strcpy((char *)u->name, rBuffer);
            break;
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }

}

/**
 * New User - Get Password
 */
void logon::Password(UserRec *u)
{

    char rBuffer[1024]= {0};
    char text[1024]= {0};
    // Ask for Users Password
    lang_get(text,10);
    int len = sizeof(u->password)-1;
    inputfield(text,len);
    pipe2ansi(text);
    while(1)
    {
        getline(rBuffer,len,NULL,TRUE);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {
            strcpy((char *)u->password, rBuffer);
            break;
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }

}

/**
 * New User - Verify Password
 */
int logon::VerifyPassword(UserRec *u)
{

    char rBuffer[1024]= {0};
    char text[1024]= {0};
    // Ask for Users Password
    lang_get(text,45);
    int len = sizeof(u->password)-1;
    inputfield(text,len);
    pipe2ansi(text);
    while(1)
    {
        getline(rBuffer,len,NULL,TRUE);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {
            if (strcmp((char *)u->password, rBuffer) == 0 && strlen(rBuffer) > 5)
            {
                return TRUE;
            }
            else
            {
                // Wrong password, doesn't match!
                memset(&text,0,sizeof(text));
                lang_get(text,46);
                pipe2ansi(text);
                return FALSE;
            }
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }
}

/**
 * New User - Get Gender
 */
void logon::Sex(UserRec *u)
{

    char text[1024]= {0};
    unsigned char c;
    // Ask Sex M/F
    lang_get(text,11);
    int len = 1;
    inputfield(text,len);
    pipe2ansi(text);
    while (1)
    {
        c = getkey(true);
        if (toupper(c) == 'M' || toupper(c) == 'F')
        {
            sprintf(text,"|15|17%c",c);
            pipe2ansi(text);
            u->sex = c;
            break;
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }
}

/**
 * New User - Get BirthDay
 */
void logon::BDay(UserRec *u)
{

    char rBuffer[1024] = {0};
    char text[1024]    = {0};
    char text2[1024]   = {0};
    long secs = 0;

    // Ask for Users Bitchday
    lang_get(text,12);
    int len = 10;
    inputfield(text,len);
    pipe2ansi(text);
    while(1)
    {
        getline(rBuffer,len+1);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {
            if (isdigit(rBuffer[0]) && isdigit(rBuffer[1]) &&
                    isdigit(rBuffer[3]) && isdigit(rBuffer[4]) &&
                    isdigit(rBuffer[6]) && isdigit(rBuffer[7]) &&
                    isdigit(rBuffer[8]) && isdigit(rBuffer[9]) &&
                    rBuffer[2] == '/'   && rBuffer[5] == '/')
            {

                if (VerifyBday(rBuffer))
                {
                    secs = Date2Sec(rBuffer);
                    u->dtbday = secs;
                    break;
                }
                else
                {
                    memset(&rBuffer,0,sizeof(rBuffer));
                    lang_get(text2,38);
                    pipe2ansi(text2);
                    pipe2ansi(text);
                }
            }
            else
            {
                memset(&rBuffer,0,sizeof(rBuffer));
                lang_get(text2,38);
                pipe2ansi(text2);
                pipe2ansi(text);

            }
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }
}

/**
 * New User - Get Email
 */
void logon::Email(UserRec *u)
{

    char rBuffer[1024] = {0};
    char text[1024]    = {0};
//    char text2[100]= {0};
//    std::string tmp;
//    int i;

    // Ask for Email Address
    int len = sizeof(u->email)-1;
    lang_get(text,13);
    inputfield(text,len);
    pipe2ansi(text);
    while(1)
    {
        getline(rBuffer,len);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {
    //      tmp = rBuffer;
            //i = strlen(rBuffer);
            // Remove checking for invalid format!
            //if (tmp.find("@",0) != std::string::npos && (rBuffer[i-4] == '.' || rBuffer[i-3] == '.')) {

            strcpy((char *)u->email, rBuffer);
            break;

            //}
            /*
            else {
                memset(&rBuffer,0,sizeof(rBuffer));
                memset(&text2,0,sizeof(text2));
                memset(&text,0,sizeof(text));

                lang_get(text2,39);
                pipe2ansi(text2);

                lang_get(text,13);
                inputfield(text,len);
                pipe2ansi(text);

            } */
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }
}

/**
 * New User - Get Email Private?
 */
void logon::EmailPriv(UserRec *u)
{

    char text[1024] = {0};
    unsigned char c;
    // Ask to Keep Email Private
    lang_get(text,14);
    int len = 1;
    inputfield(text,len);
    pipe2ansi(text);
    while (1)
    {
        c = getkey(true);
        if (toupper(c) == 'Y')
        {
            u->emprivate = true;
            break;
        }
        else if (toupper(c) == 'N')
        {
            u->emprivate = false;
            break;
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }
    sprintf(text,"|15|17%c",c);
    pipe2ansi(text);
}

/**
 * New User - Get User note
 */
void logon::Note(UserRec *u)
{

    char rBuffer[1024] = {0};
    char text[1024]    = {0};
    // Desired User Note
    lang_get(text,15);
    int len = sizeof(u->usernote)-1;
    inputfield(text,len);
    pipe2ansi(text);
    while(1)
    {
        getline(rBuffer,len);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {
            strcpy((char *)u->usernote, rBuffer);
            break;
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }
}

/**
 * New User - Get Challenge Question
 */
void logon::ChallengeQuestion(UserRec *u)
{

    char rBuffer[1024] = {0};
    char text[1024]    = {0};

    lang_get(text,16);
    int len = 39;
    inputfield(text,len);
    pipe2ansi(text);
    while(1)
    {
        getline(rBuffer,len);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {
            strcpy((char *)u->c_question, rBuffer);
            break;
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }
}

/**
 * New User - Get challenge Answer
 */
void logon::ChallengeAnswer(UserRec *u)
{

    char rBuffer[1024] = {0};
    char text[1024]    = {0};

    lang_get(text,40);
    int len = 39;
    inputfield(text,len);
    pipe2ansi(text);
    while(1)
    {
        getline(rBuffer,len,NULL,TRUE);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {
            strcpy((char *)u->c_answer, rBuffer);
            break;
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }
}

/**
 * New User - Start newuser applciation
 */
void logon::application()
{

//  errlog((char *)" * Application");

    /*
    if (useScript) {
        ScriptApplication();
        return;
    }*/

//  errlog((char *)" * Application Cont...");

    UserRec u;
    memset(&u,0,sizeof(UserRec));

    node _node;
    // Setup Default Node User as logging in...
    sprintf((char *)u.handle,"%s",(char *)"New User Application");
    _node.node_write(&u,NODE_NUM);

    memset(&u,0,sizeof(UserRec));

    // Starting New User Application Process
    ansiPrintf((char *)"newuser");
    char text[100]= {0};
    unsigned char c;

    // Ask If want to Register a new account
    lang_get(text,7);
    int len = 1;
    inputfield(text,len);
    pipe2ansi(text);
    while (1)
    {
        c = getkey(true);
        if (toupper(c) == 'Y') break;
        else if (toupper(c) == 'N') return;
    }
    sprintf(text,"|15|17%c",c);
    pipe2ansi(text);

    ansiPrintf((char *)"newmsg");

    Handle(&u);    // Handle
    Name(&u);      // Name
    Password(&u);  // Password

    int i = 0;
    while (1)
    {
        i = VerifyPassword(&u);
        if (i == TRUE) break;
        // Failed Verification, start over on password.
        Password(&u);
    }
    Sex(&u);       // Sex
    BDay(&u);      // Birthday
    Email(&u);     // Email
    EmailPriv(&u); // Keep Email Private
    Note(&u);      // Note
    ChallengeQuestion(&u);
    ChallengeAnswer(&u);
    // Save or Change User Info
//  errlog((char *)" * verify_info...");

    verify_info(&u,(char *)"verify");

}

/*
// WIP
void logon::ScriptApplication() {

    errlog((char *)" * Script Application");

    UserRec u;
    memset(&u,0,sizeof(UserRec));
    // Starting New User Application Process
    ansiPrintf((char *)"newuser");
    char text[100]={0};
    unsigned char c;

    // Ask If want to Register a new account
    lang_get(text,7);
    int len = 1;
    inputfield(text,len);
    pipe2ansi(text);
    while (1) {
        c = getkey(true);
        if (toupper(c) == 'Y') break;
        else if (toupper(c) == 'N') return;
    }
    sprintf(text,"|15|17%c",c);
    pipe2ansi(text);

    Handle(&u);    // Handle
    Name(&u);      // Name
    Password(&u);  // Password
    Sex(&u);       // Sex
    BDay(&u);      // Birthday
    Email(&u);     // Email
    EmailPriv(&u); // Keep Email Private
    Note(&u);      // Note
    ChallengeQuestion(&u);
    ChallengeAnswer(&u);
    // Save or Change User Info

    errlog((char *)" * Verify_info");

    verify_info(&u,(char *)"verify");
    save_user(&u);

}
*/

/**
 * New User - Parses Ansi Screen for Varify Inforamtion
 */
void logon::ParseVerify(char *filename, UserRec *u)
{

//  errlog((char *)" * ParseVerify!");

    std::string path = ANSIPATH;
    path += filename;
    path += ".ans";

    char MCI[255]     = {0};   // Holds MCI Codes to Parse
    char sTemp[1024]  = {0};
    char masked[1024] = {0};
    int  space = 0, foundr = 0 , foundl = 0;
    int  c = 0;
    std::string buff;

    // Buffer in Ansi
    FILE *inStream;
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL)
    {

        printf("\r\nParseVerify Fail!");
//      errlog((char *)" * ParseVerify Fail!");
        return;
    }
    do
    {
        c = getc(inStream);
        if  (c != EOF) buff += c;
    }
    while (c != EOF);
    fclose(inStream);

    std::string::size_type id1 = 0;
    while (id1 != std::string::npos)
    {
        // parse justify spacing right / left passing in string before
        // replacing mci code. to Properly Space Output Ansi.
        id1 = buff.find("%", 0);
        if (id1 == std::string::npos) break;
        memset(&MCI,0,sizeof(MCI));
        space = 0;
        // Check if MCI Code is Justified then Process this.
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
        // Now Get MCI Code
        MCI[0] = buff[id1+1]; // Get first Digit
        MCI[1] = buff[id1+2]; // Get Second Digit

        memset(&sTemp,0,sizeof(sTemp));
        // Insert MCI Parsing here so we can reaplace full result with propering spacing.

        if (strcmp(MCI,"UH") == 0)
        {
            sprintf(sTemp,"%s", u->handle);
        }
        else if (strcmp(MCI,"UN") == 0)
        {
            sprintf(sTemp,"%s", u->name);
        }
        else if (strcmp(MCI,"UP") == 0)
        {
            // Mask Password
            sprintf(masked,"%s", u->password);
            mask(masked);
            sprintf(sTemp,"%s", masked);
        }
        else if (strcmp(MCI,"US") == 0)
        {
            if (u->sex == 'f' || u->sex == 'F') sprintf(sTemp,"female");
            else sprintf(sTemp,"male");
        }
        else if (strcmp(MCI,"BD") == 0)
        {
            sprintf(sTemp,"%s", Sec2Date(u->dtbday));
        }
        else if (strcmp(MCI,"NO") == 0)
        {
            sprintf(sTemp,"%s", u->usernote);
        }
        else if (strcmp(MCI,"CQ") == 0)
        {
            sprintf(sTemp,"%s", u->c_question);
        }
        else if (strcmp(MCI,"CA") == 0)
        {
            sprintf(sTemp,"%s", u->c_answer);
        }
        else if (strcmp(MCI,"EM") == 0)
        {
            sprintf(sTemp,"%s",u->email);
        }
        else if (strcmp(MCI,"PR") == 0)
        {
            if (u->emprivate) sprintf(sTemp,"yes");
            else sprintf(sTemp,"no");
        }

        // MCI Translation .
        if (foundl == TRUE)
        {
            lspacing(sTemp,space);
        }
        else if (foundr == TRUE)
        {
            rspacing(sTemp,space);
        }

        //If we Parsed Justify, then Erase that MCI Code as well.
        (space != 0) ?
        buff.replace(id1,6,sTemp):
        buff.replace(id1,3,sTemp);
    }
    //elog("Finished Parsing MCI Codes...");
    pipe2ansi((char *)buff.c_str());
}

/**
 * New User - Save User
 */
void logon::save_user(UserRec *u)
{

    BOOL EncryptOk = TRUE;

    // Setup Encryption for User Password.
    EVP_MD_CTX mdctx;
    const EVP_MD *md;
//    char mess1[] = "Test Message\n";
 //   char mess2[] = "Hello World\n";
    unsigned char md_value[EVP_MAX_MD_SIZE]={0};
    unsigned int  md_len = 0;

    OpenSSL_add_all_digests();

    md = EVP_get_digestbyname("SHA1");
    if(!md)
    {
        EncryptOk = FALSE;
    }

    if (EncryptOk)
    {
        EVP_MD_CTX_init(&mdctx);
        EVP_DigestInit_ex(&mdctx, md, NULL);
        EVP_DigestUpdate(&mdctx, (char *)u->handle, strlen((char *)u->handle));
        EVP_DigestUpdate(&mdctx, (char *)u->password, strlen((char *)u->password));
        EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
        EVP_MD_CTX_cleanup(&mdctx);

        // Testing
        //putline((char*)"\r\nDigest: ");
        //for(i = 0; i < md_len; i++) //printf("%02x",md_value[i]);
    }

    //pipe2ansi((char *)"|CR- Testing Encryption |CR- Generating 60 test users for list testing.");
    //startpause();

    // For setting last read pointers.
    msgs mf;
    int idx = 0;

    idx = idx_count();
    if (idx == -1) idx = 0;

    //printf ("\nidx = %i",idx);

    u->idx         = idx;
    u->num         = idx;
    u->lastmbarea  = 0;
    u->lastmsg     = 0;
    u->bsdel_swap  = FALSE;
    u->unused      = 0;
    u->readertheme = 0;
    u->fsetheme    = 0;
    u->pageDn      = FALSE;
    u->readerFs    = TRUE;
    u->dtfirston   = GetCurrentDTSec();
    u->dtlaston    = u->dtfirston;

    strcpy((char *)u->startmenu,"logon");

    idx_new((char *)u->handle,idx);
    users_write(u,idx);
    mf.resetlastread(u);  // Fix this to set high pointer on all areas.

    pipe2ansi((char *)"|CS|CR|15S|07av|08ing user information.. |15.|DE |15S|07av|08ed |CR|CR |DE");

    pipe2ansi((char *)"|CR|04* Warning: |15This bbs uses 80x25 on some ansi screens.  It's highly ");
    pipe2ansi((char *)"|CR|04           |15reccomended that you turn off the status bar so that all ");
    pipe2ansi((char *)"|CR|04           |15aNSI screens will display properly. ");

    pipe2ansi((char *)"|CR|CR|PA");

}

/**
 * New User - Verify Info Screen
 */
void logon::verify_info(UserRec *u, char *mString)
{
    // Initalize Menu Function Class
    menu_func _mf;
    char zString[1024] = {0};
    char text[1024]    = {0};
    bool done          = false;
    unsigned char ch   = 0;
    int len            = 0;

    // Setup the Menu For Lightbars
    _mf._premenu = _mf._curmenu;
    _mf._curmenu = mString;

    while(!done)
    {
        // Setup Screen Display Ansi Header
        pipe2ansi((char *)"|CS");
        ParseVerify((char *)"verify",u);
        // Readin the Menu Prompt
        _mf.menu_readin();
        _mf.menu_proc(zString);
        ch = zString[1];
        switch (toupper(ch))
        {
        case 'Q': // Save & Exit
            save_user(u);
            return;

        case 'E': // Edit / Change a Setting
            // Display Mdify which string
            lang_get(text,22);
            len = 1;
            inputfield(text,len);
            pipe2ansi(text);

            ch = getkey(true);
            // Draw out key inputted
            sprintf(text,"|15|17%c",ch);
            pipe2ansi(text);
            if (isdigit(ch))
            {
                switch (ch)
                {
                case '1':
                    Handle(u);
                    break;

                case '2':
                    Name(u);
                    break;

                case '3':
                    Password(u);
                    VerifyPassword(u);
                    break;

                case '4':
                    Sex(u);
                    break;

                case '5':
                    BDay(u);
                    break;

                case '6':
                    Email(u);
                    break;

                case '7':
                    EmailPriv(u);
                    break;

                case '8':
                    Note(u);
                    break;

                case '9':
                    ChallengeQuestion(u);
                    break;

                case '0':
                    ChallengeAnswer(u);
                    break;

                default :
                    break;
                }
            }
            break;

        case 'A': // Abort
            return;

        default :
            break;
        }
    }
}

/**
 * User Login - Ask for Handle / Password
 */
int logon::logon_system(UserRec *urec)
{

    UserRec u;
    UserIdx idx;

    char rBuffer[1024] = {0};
    char text[1024]    = {0};
    int  usernum       = 0;

    ansiPrintf((char *)"login");

    memset(urec,0,sizeof(UserRec));
    strcpy((char *)urec->handle,(char *)"User Logging In");


    memset(&u,0,sizeof(UserRec));
    //strcpy(rBuffer,"");
    lang_get(text,4);
    int len = sizeof(u.handle)-1;
    inputfield(text,len);
    pipe2ansi(text);

    while (1)
    {

        memset(rBuffer,0,sizeof(rBuffer));
        getline(rBuffer,len);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {

            errlog2((char *)"User Login Attempt: %s", rBuffer);

            // Check if already exists
            usernum = atoi(rBuffer); // Check if User Number was entered.
            if (usernum != 0)
            {
                if (idx_read(&idx,usernum-1))
                {
//                    elog("User # found: %i, %s",usernum, idx.handle);
                    strcpy((char *)u.handle, (char *)idx.handle);
                    //pipe2ansi(text); // Redisplay prompt
                    //putline(u.handle);
                    break;
                }
                else
                {
//                    elog("User # NOT found: %i, %s",usernum, rBuffer);
                    lang_get(text,6);
                    pipe2ansi(text);
                    sleep(1);
                    return 0;
                }
            }
            else if(idx_match(rBuffer))
            {
                strcpy((char *)u.handle,rBuffer);
//                elog("User Name found: %i, %s",usernum, rBuffer);
                break;
            }
            else
            {
                lang_get(text,6);
                pipe2ansi(text);
                sleep(1);
                return 0;
//                elog("User Name NOT found: %i, %s",usernum, rBuffer);
            }

            //break; // Move onto password.

        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }


    // Fill GLobal User Containter with Users Data that just logged in.
    memset(urec,0,sizeof(UserRec));
    u.idx = idx_find((char *)u.handle);
    users_read(urec,u.idx);

    std::string buff;
    readinAnsi("login2",buff);

    // Parse Logon 2 if exists.
    int id1;

    // Node Number
    id1 = buff.find("%NN",0);
    if (id1 != -1)
    {
        sprintf(text,"%i",NODE_NUM);
        buff.replace(id1,3,text);
    }
    // User Number
    id1 = buff.find("%UN",0);
    if (id1 != -1)
    {
        sprintf(text,"%ld",(ulong)u.idx+1);
        buff.replace(id1,3,text);
    }
    // Last On
    id1 = buff.find("%LO",0);
    if (id1 != -1)
    {
        sprintf(text,"%s",Sec2DateTM(urec->dtlaston,2));
        buff.replace(id1,3,text);
    }
    // Last On
    id1 = buff.find("%BD",0);
    if (id1 != -1)
    {
        sprintf(text,"%s",Sec2Date(urec->dtbday));
        buff.replace(id1,3,text);
    }

    errlog2((char *)"User Login Attempt (handle): %s", urec->handle);

    pipe2ansi((char *)buff.c_str());
    buff.erase();

    // Add Scrolling text here telling the user welcome and their user #...
    // Ask for Users Password
    lang_get(text,5);
    len = sizeof(u.password)-1;
    inputfield(text,len);
    pipe2ansi(text);
    while(1)
    {
        memset(rBuffer,0,sizeof(rBuffer));
        getline(rBuffer,len,NULL,TRUE);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {
            if (check_password((char *)u.handle,rBuffer))
            {
                strcpy((char *)u.password,rBuffer);
                break;
            }
            else   // Incorrect Login - password
            {

                errlog2((char *)"%s Incorrect Login - password.",(char *)urec->handle);

                lang_get(text,6);
                pipe2ansi(text);
                return 0;
            }
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }

    // Fill GLobal User Containter with Users Data that just logged in.
//   memset(urec,0,sizeof(UserRec));
//   u.idx = idx_find(u.handle);
//   users_read(urec,u.idx);
    // login successful

    // Set Default to 1st Area, 0 = Email.
    //  if (urec->lastmbarea == 0)
    //      urec->lastmbarea = 1;

    // Write History.
    hist_update(HIST_CALLS,urec);
    hist_display(); // testing.

    // Setup Node File.
    /*
        memset(urec,0,sizeof(UserRec));
        u.idx = idx_find(u.handle);
        users_read(urec,u.idx);*/

    // Fresh login, reset variables to 0 starting.
    urec->lastmbarea = 0;
    urec->lastmsg = 0;
    CURRENT_MAREA = 0;

    //_usr.users_write(user,0);
    users_write(urec,u.idx);

    node _node;
    _node.node_write(urec,NODE_NUM);
    UserLoggedIn = TRUE;

    errlog2((char *)"%s has logged in successfully.",(char *)urec->handle);

//    _node.node_global_announce_login();

    // HACK!! For BLock Sysop Message Areas!!
    if (strcmp((char *)urec->handle, SYSOP_NAME) == 0)
    {
        isSysop = TRUE;
        //mf.resetlastread(urec);
//        errlog((char *)"%s is a Sysop!", (char *)urec->handle);
    }
    else
    {

//        errlog((char *)"%s is NOT a Sysop!", (char *)urec->handle);
    }


    return 1;
}

/**
 * User Login - Forgot Password, Ask Challenge Question
 */
void logon::forgot_password()
{

    UserRec u;
    UserIdx idx;

    char rBuffer[1024] = {0};
    char text[1024]    = {0};
    int  usernum       = 0;
    int  index         = 0;

    std::string check;
    std::string check2;

    ansiPrintf((char *)"forgot");

    memset(&u,0,sizeof(UserRec));
//    lang_get(text,4);
    sprintf(text,"handle: ");
    int len = sizeof(u.handle)-1;
    inputfield(text,len);
    pipe2ansi(text);

    while (1)
    {
        memset(rBuffer,0,sizeof(rBuffer));
        getline(rBuffer,len);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {
            // Check if already exists
            usernum = atoi(rBuffer); // Check if User Number was entered.
            if (usernum != 0)
            {
                if (idx_read(&idx,usernum-1))
                {
//                    elog("User # found: %i, %s",usernum, idx.handle);
                    strcpy((char *)u.handle, (char *)idx.handle);
                    //pipe2ansi(text); // Redisplay prompt
                    //putline(u.handle);
                }
                else
                {
//                    elog("User # NOT found: %i, %s",usernum, rBuffer);
                }
            }
            else if(idx_match(rBuffer))
            {
                strcpy((char *)u.handle, rBuffer);
//                elog("User Name found: %i, %s",usernum, rBuffer);
            }
            else
            {
//                elog("User Name NOT found: %i, %s",usernum, rBuffer);
            }

            break; // Move onto password.

        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }

    index = idx_find((char *)u.handle);
    memset(&u,0,sizeof(UserRec));
    users_read(&u,index);

    pipe2ansi((char *)"|CR|CR|07Challenge Question: |15");
    sprintf((char *)text,"%s",(char *)u.c_question);
    pipe2ansi(text);

    pipe2ansi((char *)"|CR");

    // Add Scrolling text here telling the user welcome and their user #...
    // Ask for Users Password
//   lang_get(text,5);
    sprintf(text,"answer: ");
    len = 60; //sizeof(u.c_answer)-1;
    inputfield(text,len);
    pipe2ansi(text);
    while(1)
    {
        memset(rBuffer,0,sizeof(rBuffer));
        getline(rBuffer,len,NULL,TRUE);
        if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
        {
            check  = rBuffer;
            check2 = (char *)u.c_answer;
            if (check == check2)
            {

                // Sysop Protection!
                if (strcmp((char *)u.handle, SYSOP_NAME) == 0)
                {
                    sprintf(text,"|CR|CR|15In-Correct! |07aborting . . .|CR|PA");
                    pipe2ansi(text);
                    return; //0;
                }

                sprintf(text,"|CR|CR|15Correct! Your forgotten password is: |07%s|CR|PA",u.password);
                pipe2ansi(text);
                break;
            }
            else   // Incorrect Login - password
            {

                sprintf(text,"|CR|CR|15In-Correct! |07aborting . . .|CR|PA");
                pipe2ansi(text);
                return; //0;
            }
        }
        //pipe2ansi("|15|17");
        pipe2ansi(text);
    }

    return; //1;
}

/**
 * User Info - Show User Info in Main Menu
 * Konfig, Make Chagnes or Updates
 */
void logon::userinfo(UserRec *u, char *mString)
{
    // Initalize Menu Function Class
    menu_func _mf;
    char zString[1024] = {0};
    char text[1024]    = {0};
    bool done          = false;
    char ch            = 0;
    int  len           = 0;

    // Setup the Menu For Lightbars
    _mf.menu_setuser(u);
    _mf._premenu = _mf._curmenu;
    _mf._curmenu = mString;

    /*
        sprintf(text,"|07User Note     : %s |07Last On   : %s ");
        sprintf(text,"|07Start Menu    : %s |07First On  : %s ");

        Purge User, reset message pointers.

    */

    while(!done)
    {
        // Setup Screen Display Ansi Header
        pipe2ansi((char *)"|CS");
        ParseVerify(mString,u);
        // Readin the Menu Prompt
        _mf.menu_readin();
        _mf.menu_proc(zString);
        ch = zString[1];
        switch (toupper(ch))
        {
        case 'Q': // Save & Exit
            users_write(u,u->idx);
            return;

        case 'E': // Edit / Change a Setting
            // Display Mdify which string
            lang_get(text,22);
            len = 1;
            inputfield(text,len);
            pipe2ansi(text);

            ch = getkey(true);
            // Draw out key inputted
            sprintf(text,"|15|17%c",ch);
            pipe2ansi(text);
            //if (isdigit(ch))
           // {
                switch (ch)
                {

        case 'A':
           Handle(u);
           break;

        case 'B':
           Name(u);
           break;

                case '1':
                    Password(u);
                    VerifyPassword(u);
                    break;

                case '2':
                    Sex(u);
                    break;

                case '3':
                    BDay(u);
                    break;

                case '4':
                    Email(u);
                    break;

                case '5':
                    EmailPriv(u);
                    break;

                case '6':
                    Note(u);
                    break;

                case '7':
                    ChallengeQuestion(u);
                    break;

                case '8':
                    ChallengeAnswer(u);
                    break;

                case '9': // toggle ansimations
                    if (isANSIMATION == TRUE)
                    {
                        isANSIMATION = FALSE;
                    }
                    else
                    {
                        isANSIMATION = TRUE;
                    }
                    break;

                case '0': // set autosig.
                    _mf.msg_autosig();
                    break;

                default :
                    break;
                }
            //}
            break;

        case 'A': // Abort
            return;

        default :
            break;
        }
    }
}


