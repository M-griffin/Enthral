/***************************************************************************
 *   Copyright (C) 2004-2017 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Link Lists used for Full Screen Reader / Quoter & Lists      *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

# include "struct.h"

# include "menu_func.h"
# include "msg_readll.h"
# include "conio.h"

# include <cstdio>
# include <cstring>
# include <cstdlib>

using namespace std;

/**
 * Message Link List Class
 */
msg_readll::msg_readll()
{
    head = 0;
    current_node = 0;
    last = 0;

    Row      = 1;
    Top      = 1;
    Bot      = 1;
    Tot      = 1;
    Num      = 1;
    Lines    = 0;
    Page     = 1;
    TotPages = 0;
    pgRows   = 0;               // # of Rows on Last Page with ScrollDown
    quote    = FALSE;           // Turn on Colors for Quote Windows
    reader   = FALSE;           // Turn on Colors for Message Reader
    arealist = FALSE;
    PAGE_SCROLL = TRUE;         // Page Down Scrolling in List.
    CLEAR_BOX   = TRUE;         // Clear Box. Default.
    strcpy(LoColor,"");	        // Message Text Color
    strcpy(HiColor,"");	        // Message Text HiLight Color
    strcpy(TEXT_COLOR,"");      // Message Text Color
    strcpy(QUOTE_COLOR,"");     // Message Quote
    strcpy(SYS_COLOR,"");       // --- BBS Version
    strcpy(ORIGIN_COLOR,"");    //  * Origin Line ...
}

msg_readll::~msg_readll()
{
}

/**
 * Message Link List - Setup Message Quoter
 */
void msg_readll::initQuote(char *sLow, char *sHigh, int q)
{
    quote = q;              // In Quote Editor for Reader.
    strcpy(LoColor,sLow);   // Message Text Color
    strcpy(HiColor,sHigh);  // Message Text HiLight Color
}

/**
 * Message Link List - Setup Title & Area/User Lists
 */
void msg_readll::initList(char *sLow, int alist)
{
    arealist = alist;       // Set if were using Area List
    strcpy(LoColor,sLow);   // Message Text Color
}

/**
 * Message Link List - for Last Page in list to scroll bottom up
 */
void msg_readll::togglePageScroll(int on)
{
    PAGE_SCROLL = on;
}

/**
 * Message Link List - setup for message reader
 */
void msg_readll::initReader(char *sTEXT, char *sQUOTE, char *sSYS, char *sORIGIN)
{
    reader = TRUE;
    strcpy(TEXT_COLOR,sTEXT);       // Message Text Color
    strcpy(QUOTE_COLOR,sQUOTE);     // Message Quote
    strcpy(SYS_COLOR,sSYS);         // --- BBS Version
    strcpy(ORIGIN_COLOR,sORIGIN);   //  * Origin Line ...
}

/**
 * Message Link List - clear list
 */
void msg_readll::clearBox()
{
    char capture[100]= {0};
    std::string _editbox;
    int boxsize = Bot - Top;

    //Now clear the box First
    for (int t = 0; t < boxsize; t++) {
        sprintf(capture, "\x1b[%i;%iH\x1b[K", (Top)+t, 1);
        _editbox += capture;
    }
    pipe2ansi((char *)_editbox.c_str());
}

/**
 * Message Link List - Remove Control Chars
 */
void msg_readll::stripCRONLY(char *ostr)
{
    int id1;
    std::string tstr = ostr;
    while ((id1=tstr.find("\n",0)) != -1) tstr.erase(id1,1);
    while ((id1=tstr.find("\r",0)) != -1) tstr.erase(id1,1);
    while ((id1=tstr.find("\b",0)) != -1) tstr.erase(id1,1);
    strcpy(ostr,(char *)tstr.c_str());
}


/**
 * Message Link List - Copy data from link lists to buffer
 */
void msg_readll::GetBuffer(std::string &mBuf)
{
    mBuf.erase();
    if(head == 0) {
        return;
    }
    current_node = head;
    while(1) {
        if (quote == TRUE && current_node->flag == TRUE) {
            mBuf += "> " + current_node->data + '\r';
        } else if (quote == FALSE) {
            mBuf += current_node->data + '\r';
        }
        if (current_node->dn_link == 0) break;
        current_node = current_node->dn_link;
    }
}

/**
 * Copy message from link lists to <vector> buffer
 */
void msg_readll::GetVector(vector<list_bar> listb)
{
    listing.swap(listb);
}

/**
 * Copy From buffer into Link List
 */
void msg_readll::PutBuffer(char *mBuff, int Strip)
{
    std::string MsgText, Line;
    MsgText = mBuff;
    int id1, i = 0;
    char TmpStrip[200]= {0};

    while(1) {
        Line.erase();

        while (1) {
            if (MsgText.find("\r", 0) == 0)
                MsgText.erase(0,1);
            else break;
        }
        while (1) {
            id1 = MsgText.find("\n", 0);
            if (id1 != -1)
                MsgText.erase(id1,1);
            else break;
        }
        if (Strip) {
            // Do a quick Loop Through and Remove all | Pipe Color Codes.
            // Used mainly for Message Quoting
            while (1) {
                id1 = MsgText.find("|", 0);
                if (id1 != -1 && isdigit(MsgText[id1+1]) && isdigit(MsgText[id1+2]))
                    MsgText.erase(id1,3);
                else break;
            }
        }
        
        id1 = MsgText.find("\r", 0);
        if (id1 == -1) break;
        else {
            Line = MsgText.substr(0,id1);
            MsgText.erase(0,id1);
            if (Line.size() > 0) {
                strcpy(TmpStrip,(char *)Line.c_str());
                stripCRONLY(TmpStrip);
                add_to_list(TmpStrip);
                current_node->lineNum = (i+1);
                ++i;
            }
        }
    }

    LineRec *tmp;
    Last_Line();

    for (;;) {
        if (current_node->data == "" || current_node->data.size() < 1) {
            if(current_node == 0) {
                break;
            }
            tmp = current_node;

            if (current_node->up_link == 0) break;
            current_node = current_node->up_link;
            current_node->dn_link = 0;
            delete tmp;
            tmp = 0;
        } else
            break;
    }
    current_node->dn_link = 0;
}

/**
 * Copy From buffer into Link List (UserSig) Clean
 */
void msg_readll::PutBufferSig(char *mBuff, int Strip)
{
    int id1;
    std::string Line;
    std::string MsgText;
    int i = 0;

    std::string buffer;

    MsgText = mBuff;
    while (1) {
        id1 = Line.find("\r", 0);
        if (id1 == 0)
            Line.erase(id1,1);
        else break;
    }

    while(1) {
        id1  = MsgText.find("\r", 1);
        if (id1 == -1) break;
        else {
            Line.erase();
            Line = MsgText.substr(0,id1);
            MsgText.erase(0,id1);

            while (1) {
                id1 = Line.find("\r", 0);
                if (id1 != -1)
                    Line.erase(id1,1);
                else break;
            }
            
            add_to_list(Line);
            ++i;
        }
    }

    // Clean Bottom of Message, Leave no Blank Lines below sig.
    LineRec *tmp;
    Last_Line();
    
    for (;;) {
        if (current_node->data == "" || current_node->data.size() < 1) {
            if(current_node == 0) {
                break;
            }
            
            tmp = current_node;
            if (current_node->up_link == 0) break;
            current_node = current_node->up_link;
            
            delete tmp;
            tmp = 0;
        } else
            break;
    }
    current_node->dn_link = 0;
}

/**
 * Remove Tag and Origin Lines from Original Message for Editing!
 */
void msg_readll::cleartags(char *tag, char *origin)
{
    LineRec *tmp;
    current_node = head;

    while(current_node != 0) {
        if (current_node->data == tag || current_node->data.substr(0,3) == "---")
            current_node->data.erase();
        else if (current_node->data == origin)
            current_node->data.erase();

        if (current_node->dn_link == 0) break;
        current_node = current_node->dn_link;
    }

    Last_Line();
    for (;;) {
        if (current_node->data == "" || current_node->data.size() < 1) {
            if(current_node == 0) {
                break;
            }
            tmp = current_node;
            if (current_node->up_link == 0) break;
            current_node = current_node->up_link;

            delete tmp;
            tmp = 0;
        } else
            break;
    }
    current_node->dn_link = 0;
}

/**
 * Clear All (Quoter)
 */
void msg_readll::clear_all()
{
    LineRec *print;
    if(head == 0) {
        return;
    }

    print = head;
    while(print != 0) {
        print->flag = FALSE;
        if (print == last) break;
        print = print->dn_link;
    }
}

/**
 * Select All Lines (Quoter)
 */
void msg_readll::select_all()
{
    LineRec *print;
    if(head == 0) {
        return;
    }

    print = head;
    while(print != 0) {
        print->flag = TRUE;
        if (print == last) break;
        print = print->dn_link;
    }
}

/**
 * flag Lines (Quoter) which line are to be quoted.
 */
void msg_readll::flag_line(int iNum)
{
    LineRec *print;
    long int i = 1;

    if(head == 0) {
        return;
    }

    print = head;
    while(print != 0) {
        if (iNum == i) {
            if (print->flag == TRUE) print->flag = FALSE;
            else print->flag = TRUE;
        }

        if (print == last) break;
        print = print->dn_link;
        ++i;
    }
}

/**
 * delete current line and Move up CTRL+Y
 */
void msg_readll::delete_line_up()
{
    LineRec *tmp;

    if(current_node == 0) {
        return;
    }

    if (current_node->up_link != 0) {
        tmp = current_node;
        current_node = current_node->dn_link;
        current_node->up_link = tmp->up_link;
        tmp->up_link->dn_link = current_node;
        delete tmp;
        tmp = 0;
        current_node = current_node->up_link;
        return;
    }

    current_node->data.erase();
    Row = 1;
    Num = 1;
    Tot = 1;
}

/**
 * Add Line to Link List
 */
void msg_readll::add_to_list(std::string add_data)
{
    LineRec *add = new LineRec;
    if (!add) {
        exit (1);
    }

    add->data = add_data;
    add->flag = FALSE;
    add->lineNum = 0;

    if (current_node == 0) { // New Frist Add!
        add->up_link = 0;
        add->dn_link = 0;
        head = add;
        current_node = add;
        last = add;
        return;
    }

    add->up_link = current_node;

    // In the Middle of list! hmmm
    if (current_node->dn_link != 0) {
        add->dn_link = current_node->dn_link;
        current_node->dn_link = add;
        current_node->dn_link = add;
        current_node = add;
    }
    // Current is the Last one in Lie
    else {
        add->up_link = current_node;
        add->dn_link = 0;
        current_node->dn_link = add;
        current_node = add;
        last = add;
    }
}

/**
 * Move up 1 from current to prev in list
 */
int msg_readll::move_up()
{
    if (current_node == 0) return FALSE;
    if (current_node->up_link == 0) {
        return FALSE;
    }
    current_node = current_node->up_link;
    return TRUE;
}

/**
 * move down 1 from current to next
 */
int msg_readll::move_down()
{
    if (current_node == 0) return FALSE;
    if (current_node->dn_link == 0) {
        return FALSE;
    }
    current_node = current_node->dn_link;
    return TRUE;
}

/**
 * Count how many lines in list from current all the way down
 */
int msg_readll::line_count()
{
    int count = 0;      // Don't include Current
    LineRec *lineTmp;

    if(head == 0) {
        return 0;
    }

    lineTmp = current_node;

    while ( lineTmp->dn_link != 0 ) {
        lineTmp = lineTmp->dn_link;
        ++count;
    }

    return count;
}

/**
 * Count how many lines in list from Top all the way down
 */
int msg_readll::line_total()
{
    int count = 1;      // Include Current
    LineRec *lineTmp;   // always assign temp to not affect global current_node

    if(head == 0) {
        return 0;
    }

    lineTmp = head;

    while ( lineTmp->dn_link != 0 ) {
        if (lineTmp->dn_link == 0) break;
        ++count;
        lineTmp = lineTmp->dn_link;
    }

    Tot = count;
    return count;
}

/**
 * Move to last line in list
 */
void msg_readll::Last_Line()
{
    if(head == 0) {
        return;
    }

    current_node = head;
    while (1) { // Goto LAst!
        if (current_node->dn_link == 0) {
            break;
        }

        current_node = current_node->dn_link;
    }
}

/**
 * Remove all enteries in list
 */
void msg_readll::dispose()
{
    dispose_list();
    return;
}

/**
 * Wipe Out List Completely & Free Memory.
 */
void msg_readll::dispose_list()
{
    LineRec *tmp;
    current_node = head;

    while(current_node != 0) {
        tmp = current_node;

        if (current_node->dn_link == 0) break;
        current_node = current_node->dn_link;
        delete tmp;
        tmp = 0;
    }

    current_node = 0;
    head = 0;
    last = 0;
}

/**
 * Message Reader - Set Text and Quote Colors
 * With Origina and Tags (Full SCreen Reader)
 */
char *msg_readll::parsereadcolors(char *cap,
                                  int   row,
                                  int   col,
                                  std::string data)
{
    std::string tmp;
    tmp = data;
    std::string::size_type id1 = 0, id2 = 0;

    // System Tag
    id1 = tmp.find("---",0);
    if (id1 == 0) {
        sprintf(cap, "\x1b[%i;%iH%s%s",
                row,col,SYS_COLOR,tmp.c_str());
        return cap;
    }

    // Origin Line
    id1 = tmp.find (" * Origin:",0);
    if (id1 == 0) {
        sprintf(cap, "\x1b[%i;%iH%s%s",
                row,col,ORIGIN_COLOR,tmp.c_str());
        return cap;
    }


    // Clean Up Quoted Text, Remove Pipe or Ansi Color Codes
    // Remove any Pipe Color Codes from Already Quoted Text for Reader.
    while (1) {  // Remove any PIPE Color Codes in Quoting.
        id1 = tmp.find("|", 0);
        if (id1 != std::string::npos && isdigit(tmp[id1+1]) && isdigit(tmp[id1+2]))
            tmp.erase(id1,3);
        else break;
    }
    while (1) {  // Remove any ANSI Color Codes
        id1 = tmp.find("\x1b[", 0);
        if (id1 != std::string::npos) {
            id2 = tmp.find("m",id1+3);
            tmp.erase(id1,(id2 - id1)+1);
        } else break;
    }

    // Quote Color
    id1 = tmp.find(">", 0);
    if (id1 != std::string::npos && id1 <= 3) {
        sprintf(cap, "\x1b[%i;%iH%s%s",
                row,col,QUOTE_COLOR,tmp.c_str());
        return cap;
    } else {
        tmp.erase();
        tmp = data;
    }
    // Normal Text Color
    sprintf(cap, "\x1b[%i;%iH%s%s",
            row,col,TEXT_COLOR,tmp.c_str());

    return cap;
}

/**
 * Message Reader - Set Text and Quote Colors
 * With Origina and Tags (Scrolling Reader)
 */
char *msg_readll::parseReadColorScroller(char *cap, std::string data)
{
    std::string tmp;
    tmp = data;
    std::string::size_type id1 = 0, id2 = 0;

    // System Tag
    id1 = tmp.find("---",0);
    if (id1 == 0) {
        sprintf(cap, "%s%s",SYS_COLOR,tmp.c_str());
        return cap;
    }

    // Origin Line
    id1 = tmp.find (" * Origin:",0);
    if (id1 == 0) {
        sprintf(cap, "%s%s",ORIGIN_COLOR,tmp.c_str());
        return cap;
    }

    // Clean Up Quoted Text, Remove Pipe or Ansi Color Codes
    // Remove any Pipe Color Codes from Already Quoted Text for Reader.
    while (1) {  // Remove any PIPE Color Codes in Quoting.
        id1 = tmp.find("|", 0);
        if (id1 != std::string::npos && isdigit(tmp[id1+1]) && isdigit(tmp[id1+2]))
            tmp.erase(id1,3);
        else break;
    }
    while (1) {  // Remove any ANSI Color Codes
        id1 = tmp.find("\x1b[", 0);
        if (id1 != std::string::npos) {
            id2 = tmp.find("m",id1+3);
            tmp.erase(id1,(id2 - id1)+1);
        } else break;
    }

    // Quote Color
    id1 = tmp.find(">", 0);
    if (id1 != std::string::npos && id1 <= 3) {
        sprintf(cap, "%s%s",QUOTE_COLOR,tmp.c_str());
        return cap;
    } else {
        tmp.erase();
        tmp = data;
    }
    // Normal Text Color
    sprintf(cap, "%s%s",TEXT_COLOR,tmp.c_str());

    return cap;
}

/**
 * Inital Fucntion for Drawing List on the Screen
 * And Scrolling text per page.
 */
void msg_readll::box_start(unsigned long CURRENT)
{
    std::string _editbox = LoColor, lData;
    char capture[200]= {0};
    char lineNum[20];
    Page   = 1;

    // Calcuate Box Size and Total Pages
    int boxsize = Bot - Top; // Fist Get Box Size
    Tot = line_total();
    TotPages = Tot / boxsize;
    if (Tot % boxsize > 0) {
        ++TotPages;
    }
    if (Tot <= boxsize) TotPages = 1;

    // Toggle this for Area List Only, fucks up Message Reader!!

    // Test What the Current Area is, if it's started on the Next Page
    // Refresh on that page instead of starting fresh.
    if (arealist) {
        Page = CURRENT / boxsize;
        if (CURRENT % boxsize > 0)
            ++Page;

        if (Page > 1) {
            //box_refresh(CURRENT_MAREA);
            box_refresh(CURRENT);
            return;
        }
    }

    //Now clear the box First
    for (int t = 0; t < boxsize; t++) {
        sprintf(capture, "\x1b[%i;%iH\x1b[K", (Top)+t, 1);
        _editbox += capture;
    }

    // Now Grab as many lines as will fit in the box
    current_node = head; //->dn_link;
    for (int i = 1; i < boxsize+1; i++) {
        if (quote) {
            lData = current_node->data;

            // For Messge Quoteer only!!
            sprintf(lineNum,"%i",current_node->lineNum);
            lspacing(lineNum,3);

            if (current_node->flag == TRUE)
                sprintf(capture, "\x1b[%i;%iH|15%s%s. %s|16", Top+i-1, 1, HiColor, lineNum, (char *)lData.c_str());
            else
                sprintf(capture, "\x1b[%i;%iH|15%s. %s%s|16", Top+i-1, 1, lineNum, LoColor, (char *)lData.c_str());
            // End Of Message Quoter
        }
        // Message Reader
        else if (reader) {
            strcpy(capture,parsereadcolors(capture,Top+i-1,1, (char *)current_node->data.c_str()));
        } else // Else Normal Link Lists like Aeara etc...
            sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, (char *)current_node->data.c_str());

        _editbox += capture;

        if (current_node->dn_link == 0) {
            break;
        }
        current_node = current_node->dn_link;
    }
    sprintf(capture, "\x1b[%i;%iH", Row+Top-1, 1);
    _editbox += capture;
    pipe2ansi((char *)_editbox.c_str());

}

/**
 * Inital Fucntion for Drawing List on the Screen
 * And Scrolling text per page.
 */
void msg_readll::box_scroll_reader()
{
    if (head == 0) return;

    std::string _editbox = LoColor, lData;
    char capture[1024]= {0};

    Page   = 1;
    pgRows = 0;

    int iCoordX = Top,
        iPause;

    int boxsize = TERM_HEIGHT-1;

    // Now Grab as many lines as will fit in the screen up to 24 Lines!
    current_node = head; //->dn_link;
    for (int i = iCoordX; i < boxsize+1; i++) {
        strcpy(capture,parseReadColorScroller(capture,current_node->data.c_str()));
        _editbox += capture;
        _editbox += "|CR";
        if (current_node->dn_link == 0) {
            Page = 0;
            break;
        }
        current_node = current_node->dn_link;
    }

    if (Page == 0) {
        Page = 1;
        _editbox += "|CR";
        pipe2ansi((char *)_editbox.c_str());
        return;
    }
    pipe2ansi((char *)_editbox.c_str());

REDRAW:

    memset(&capture,0,sizeof(capture));
    _editbox.erase();

    iPause = 0;
    do {
        iPause = startpause((char *)"mreadpause.ini");
        if (toupper(iPause) == 'N' || (toupper(iPause) == 'Q')) return;
    } while(iPause == 0);

    ++Page;

    // Now Grab as many lines as will fit in the screen up to 24 Lines!
    for (int i = 1; i < boxsize+1; i++) {
        strcpy(capture,parseReadColorScroller(capture,current_node->data.c_str()));
        _editbox += capture;
        _editbox += "|CR";
        if (current_node->dn_link == 0) {
            Page = 0;
            break;
        }
        current_node = current_node->dn_link;
    }

    pipe2ansi((char *)_editbox.c_str());

    if (Page == TotPages) {
        pipe2ansi((char *)"|CR");
        return;
    }

    if (Page == 0) {
        Page = TotPages;
        pipe2ansi((char *)"|CR");
        return;
    }

    goto REDRAW;
}

/**
 * Scroll Last Page Up from Bottom.
 */
void msg_readll::box_scrolldn()
{
    std::string _editbox = LoColor, lData;
    char capture[200]= {0};
    char lineNum[20];

    // Make sure we can go down another page..
    int boxsize = Bot - Top;

    if (CLEAR_BOX == TRUE) {
        //Now clear the box First
        for (int t = 1; t < boxsize+1; t++) {
            sprintf(capture, "\x1b[%i;%iH\x1b[K", (Top)+t-1, 1);
            _editbox += capture;
        }
    }

    Last_Line();

    // Move up As Many lines that will fit in the box
    for (int i = 1; i < boxsize; i++) {
        if (current_node->up_link == 0) {
            break;
        }

        current_node = current_node->up_link;
    }

    // Now Grab as many lines as will fit in the box
    for (int i = 1; i < boxsize+1; i++) {
        if (quote) {
            lData = current_node->data;
            sprintf(lineNum,"%i",current_node->lineNum);
            lspacing(lineNum,3);

            if (current_node->flag == TRUE)
                sprintf(capture, "\x1b[%i;%iH|15%s%s. %s|16", Top+i-1, 1, HiColor, lineNum, lData.c_str());
            else
                sprintf(capture, "\x1b[%i;%iH|15%s. %s%s", Top+i-1, 1, lineNum, LoColor, lData.c_str());
        } else if (reader) {
            strcpy(capture,parsereadcolors(capture,Top+i-1,1, current_node->data.c_str()));
        } else {
            pgRows = i; // Set # of Rows on Last Page.
            sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str());
        }

        _editbox += capture;
        if (current_node->dn_link == 0) break;
        current_node = current_node->dn_link;
    }

    // Display Message
    Page = TotPages;
    pipe2ansi((char *)_editbox.c_str());
}

/**
 * Page Down in Link List.
 */
void msg_readll::box_pgdn()
{
    std::string _editbox = LoColor, lData;
    char capture[200]= {0};
    char lineNum[20]= {0};

    // Make sure we can go down another page..
    int boxsize = Bot - Top;

    Tot = line_total();
    TotPages = Tot / boxsize;
    if (Tot % boxsize > 0) {
        ++TotPages;
    }
    if (Tot <= boxsize) TotPages = 1;

    // Check Total Number of Pages
    // If On Last page Return
    if (Page == TotPages) {
        --Page;
    }
    ++Page;

    current_node = head;

    if (Page == TotPages && PAGE_SCROLL == TRUE) {
        box_scrolldn();
        return;
    }    
    else {
        pgRows = 0;
    }

    if (CLEAR_BOX == TRUE) {
        for (int t = 0; t < boxsize; t++) {
            sprintf(capture, "\x1b[%i;%iH\x1b[K", (Top)+t, 1);
            _editbox += capture;
        }
    }

    // Now Move Down How Many Pages
    for (int i = 1; i < (boxsize*(Page-1))+1; i++) {
        if (current_node->dn_link == 0) break;
        current_node = current_node->dn_link;
    }

    // Now Grab as many lines as will fit in the box
    for (int i = 1; i < boxsize+1; i++) {
        if (quote) {
            lData = current_node->data;
            sprintf(lineNum,"%i",current_node->lineNum);
            lspacing(lineNum,3);

            if (current_node->flag == TRUE)
                sprintf(capture, "\x1b[%i;%iH|15%s%s. %s|16", Top+i-1, 1, HiColor, lineNum, lData.c_str());
            else
                sprintf(capture, "\x1b[%i;%iH|15%s. %s%s", Top+i-1, 1, lineNum, LoColor, lData.c_str());
        } else if (reader) {
            strcpy(capture,parsereadcolors(capture,Top+i-1,1, current_node->data.c_str()));
        } else {
            sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str());
        }
        
        _editbox += capture;
        if (current_node->dn_link == 0) break;
        current_node = current_node->dn_link;
    }

    pipe2ansi((char *)_editbox.c_str());
}

/**
 * Page Up in Link List.
 */
void msg_readll::box_pgup()
{

    // Faster display to throw in 1 string and send all out at 1 time
    std::string _editbox = LoColor,	lData;
    char capture[200]= {0};
    char lineNum[20];

    pgRows = 0;
    int boxsize = Bot - Top;

    Tot = line_total();
    TotPages = Tot / boxsize;
    if (Tot % boxsize > 0) {
        ++TotPages;
    }
    if (Tot <= boxsize) TotPages = 1;

    // Check Total Number of Pages
    // If On Last page Return
    if (Page == 1) return;
    --Page;
    
    //Now clear the box First
    for (int t = 0; t < boxsize; t++) {
        sprintf(capture, "\x1b[%i;%iH\x1b[K", (Top)+t, 1);
        _editbox += capture;
    }
    // Set Current Node to First In List
    current_node = head;
    if (Page > 1) {
        // Now Move Down How Many Pages
        for (int i = 1; i < (boxsize*(Page-1)+1); i++) {
            if (current_node->dn_link == 0) break;
            current_node = current_node->dn_link;
        }
    }
    // Now Grab as many lines as will fit in the box
    for (int i = 1; i < boxsize+1; i++) {
        if (quote) {
            lData = current_node->data;
            sprintf(lineNum,"%i",current_node->lineNum);
            lspacing(lineNum,3);

            if (current_node->flag == TRUE)
                sprintf(capture, "\x1b[%i;%iH|15%s%s. %s|16", Top+i-1, 1, HiColor, lineNum, lData.c_str());
            else
                sprintf(capture, "\x1b[%i;%iH|15%s. %s%s", Top+i-1, 1, lineNum, LoColor, lData.c_str());
        } else if (reader) {
            strcpy(capture,parsereadcolors(capture,Top+i-1,1, current_node->data.c_str()));
        } else
            sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str());

        _editbox += capture;
        if (current_node->dn_link == 0) break;
        current_node = current_node->dn_link;
    }

    pipe2ansi((char *)_editbox.c_str());
}

/**
 * Refresh the Screen
 */
void msg_readll::box_refresh(unsigned long CURRENT)
{
    if (Page == 0) Page = 1;
    if (Page == 1)
        box_start(CURRENT);
    else {
        --Page;
        box_pgdn();
    }
}

/**
 * Refresh the Screen
 */
void msg_readll::box_overdraw(unsigned long CURRENT)
{
    CLEAR_BOX = FALSE;
    if (Page == 0) Page = 1;
    if (Page == 1)
        box_start(CURRENT);
    else {
        --Page;
        box_pgdn();
    }
    CLEAR_BOX = TRUE;
}

/**
 * And Handles jumping to different pages.
 */
void msg_readll::box_start_vector(unsigned long page, unsigned long list)
{
    std::string _editbox = LoColor, lData;
    char capture[200]= {0};

    Page = page;

    // Calcuate Box Size and Total Pages
    int boxsize = Bot - Top; // Fist Get Box Size
    Tot = listing.size();
    TotPages = Tot / boxsize;
    if (Tot % boxsize > 0) {
        ++TotPages;
    }
    if (Tot <= boxsize)
        TotPages = 1;


    // This doesn't work in full screen message read?!?!?!
    // Testing, only clear the box, if total pages > 1,
    // Otherwise no need to clear since it's fresh draw.
    if (TotPages > 1) {
        //Now clear the box First
        for (int t = 0; t < boxsize; t++) {
            sprintf(capture, "\x1b[%i;%iH\x1b[K", (Top)+t, 1);
            _editbox += capture;
        }
    }

    // Now Grab as many lines as will fit in the box
    for (int i = 1; i < boxsize+1; i++) {
        if (((boxsize*Page)+i)-1 >= (signed)listing.size()) break;

        // If Area has new message rotate output to new lightbars.
        if (listing[((boxsize*Page)+i)-1].isnew) {
            if ((signed)list+1 == (boxsize*Page)+i) {
                current_selection = Top+i-1; // Get current place in box to display.
                sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, (char *)listing[((boxsize*Page)+i)-1].ansi_4.c_str());
            } else {
                sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, (char *)listing[((boxsize*Page)+i)-1].ansi_3.c_str());
            }
        } else {
            if ((signed)list+1 == (boxsize*Page)+i) {
                current_selection = Top+i-1; // Get current place in box to display.
                sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, (char *)listing[((boxsize*Page)+i)-1].ansi_2.c_str());
            } else {
                sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, (char *)listing[((boxsize*Page)+i)-1].ansi_1.c_str());
            }
        }

        _editbox += capture;
    }

    // Write out Box.
    sprintf(capture, "\x1b[%i;%iH", Row+Top-1, 1);
    _editbox += capture;
    pipe2ansi((char *)_editbox.c_str());
}
