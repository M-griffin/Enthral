/***************************************************************************
 *   Copyright (C) 2004-2017 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Full Screen Message Editor w/ Own LinkList Seperated         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

# include "struct.h"
# include "console.h"
# include "conio.h"
# include "language.h"
# include "menu_func.h"
# include "users.h"
# include "msg_readll.h"

# include "msg_fse.h"
# include "msg_quote.h"

# include <vector>
# include <string>
# include <fstream>

# include <cstdio>
# include <cstring>
# include <cstdlib>
# include <unistd.h>


using namespace std;

# define MAX_WIDTH 79

/**
 * Full Screen Editor - Startup with User Info
 */
msg_fse::msg_fse(UserRec *urec)
{
    head = 0;
    current_node = 0;
    last = 0;

    Row = 0;    // Y - Row
    Col = 0;    // X - Colume
    Len = 0;    // Line Length
    Top = 0;    // Top Margin
    Bot = 0;    // Bottom Margin
    Num = 0;    // Number of Current line were at in Link List
    Tot = 0;    // Total Number of Lines in Link List

    OVERWRITE = false;
    MSG_REPLY = false;

    TRow      = 0;      // Temp Row, that were Line Wrapping On
    cont      = false;  // If continious wrappign is on or off
    thisuser  = urec;   // Setup Users Information for Posting And Message Stats

    bPush     = FALSE;  // If were pushing from the middle of the line.

    strcpy(sTHEME_NAME,"");
    strcpy(sANSI_FILE,"");
    strcpy(sTEXT_COLOR,"");
    strcpy(sMENU_PROMPT,"");
}

msg_fse::~msg_fse()
{
}


/**
 * Full Screen Editor - Add Line to Link List
 * NOTE: Should replace this is std::vector<string> in future!!
 */
void msg_fse::add_to_list(std::string add_data)
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

    add->dn_link = 0;
    add->up_link = current_node;

    // In the Middle of a Line! hmmm
    if (current_node->dn_link != 0) {
        add->dn_link = current_node->dn_link;
        current_node->dn_link->up_link = add;
        current_node->dn_link = add;
        current_node = add;
        last = add;
    }
    // Current is the Last one in Lie
    else {
        add->dn_link = 0;
        current_node->dn_link = add;
        current_node = add;
        last = add;
    }
}

/**
 * Full Screen Editor - Strip Control Chars
 */
void msg_fse::stripCRONLY(char *ostr)
{
    int id1;
    std::string tstr = ostr;
    while ((id1=tstr.find("\n",0)) != -1) tstr.erase(id1,1);
    while ((id1=tstr.find("\r",0)) != -1) tstr.erase(id1,1);
    while ((id1=tstr.find("\b",0)) != -1) tstr.erase(id1,1);
    strcpy(ostr,(char *)tstr.c_str());
}

/**
 * Full Screen Editor - Copy Data From buffer (Mesage Quoter) into Link List (Message)
 * Or Any Text Being passed to the Interface ie AutoSig!
 */
void msg_fse::PutBuffer(char *mBuff)
{
    std::string MsgText, Line;
    MsgText = mBuff;

    std::string::size_type id1 = 0;
    int i = 0;
    char TmpStrip[200]= {0};

    if (current_node == 0) return;

    while(1) {
        Line.erase();
        while (1) {
            if (MsgText.find("\r", 0) == 0)
                MsgText.erase(0,1);
            else break;
        }

        // Now get Carriage return at end of line to mark next line.
        id1 = MsgText.find("\r", 0);

        // Make Sure only to Add New Lines when being used,
        // First Line is already Setup! So Skip it
        if (id1 == std::string::npos) break;
        else {
            Line = MsgText.substr(0,id1);
            MsgText.erase(0,id1);
            if (Line.size() > 0) {
                strcpy(TmpStrip,(char *)Line.c_str());
                stripCRONLY(TmpStrip);

                // If first line already exists, use it before adding a new one!!
                if (current_node != 0 && i == 0) {
                    current_node->data = TmpStrip;
                } else {
                    add_to_list(TmpStrip);
                }
                ++i;
            }
        }
    }
    add_to_list("");
}

/**
 * Full Screen Editor - Copy Data from link list to buffer
 * For Returning Message back for saving.
 */
void msg_fse::save_all()
{
    buffer.erase();
    LineRec *tmp;

    // Remove any blank lines at the bottom of Text!  Clean it up.
    Last_Line();
    for (;;) {
        if (current_node->data == "" || current_node->data.size() < 1) {
            if(current_node == 0) {
                break;
            }
            tmp = current_node;
            current_node = current_node->up_link;
            current_node->dn_link = 0;
            delete tmp;
            tmp = 0;

        } else
            break;
    }
    current_node->dn_link = 0;

    Last_Line();

    // Now Copy Link List to Buffer For Saving
    if(head == 0) {
        return;
    }

    // Make Sure Link List Stays at current if there is none!
    current_node = head;
    do {
        current_node->data += '\r';
        buffer += current_node->data;

        if (current_node->dn_link == 0) break;
        current_node = current_node->dn_link;
    } while ( current_node != 0 );
}

/**
 * Full Screen Editor - Move to Previous Line
 */
int msg_fse::move_up()
{
    if(current_node == 0) {
        return FALSE;
    }

    if (current_node->up_link == 0) {
        return FALSE;
    }
    current_node = current_node->up_link;
    return TRUE;
}


/**
 * Full Screen Editor - Move Down to Next Line
 */
int msg_fse::move_down()
{
    if(current_node == 0) {
        return FALSE;
    }

    if (current_node->dn_link == 0) {
        return FALSE;
    }
    current_node = current_node->dn_link;
    return TRUE;
}

/**
 * Full Screen Editor - Reset to Begining of Link List.
 */
void msg_fse::move_first()
{
    current_node = head;
    Num = 1;
}

/**
 * Full Screen Editor - Jump to End of Message
 */
void msg_fse::MoveAllDown()
{
    Row = 1;

    if (head == 0) return;
    Last_Line();

    Tot = line_total();
    Num = Tot;
    if (Tot <= (Bot-Top)) {
        Row = Num;
    } else
        Row = Bot-Top;

    box_redraw();
}


/**
 * Full Screen Editor - Move to last line in list
 */
void msg_fse::move_last()
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

    Tot = line_total();
    Num = Tot;
    if (Tot <= (Bot-Top)) {
        Row = Num;
    } else
        Row = Bot-Top;
}

/**
 * Full Screen Editor - Count how many lines in list from
 *						current all the way down
 */
int msg_fse::line_count()
{
    int count = 0;
    LineRec *lineTmp;

    if(current_node == 0) {
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
 * Full Screen Editor - Count how many lines in list from Top all the way down
 */
int msg_fse::line_total()
{
    int count = 1;
    LineRec *lineTmp;

    if(head == 0) {
        return 0;
    }

    lineTmp = head;

    while ( lineTmp->dn_link != 0 ) {
        lineTmp = lineTmp->dn_link;
        ++count;
    }

    Tot = count;
    return count;
}

/**
 * Full Screen Editor - Delete Line and Move up 
 * One Line then to the End (for Backspaces)
 */
void msg_fse::delete_line_up()
{
    LineRec *tmp;

    if(current_node == 0) {
        return;
    }

    if ( current_node->up_link != 0) {
    
        tmp = current_node;
        if (current_node->dn_link != 0) {
            current_node->up_link->dn_link = current_node->dn_link;
            current_node->dn_link->up_link = current_node->up_link;
        } else {
            current_node->up_link->dn_link = 0;
        }

        current_node = current_node->up_link;
        delete tmp;
        tmp = 0;

        if (current_node->dn_link == 0)
            last = current_node;

        Line.erase();
        Line = current_node->data;
        Col  = Line.size()+1;

        --Num;
        Tot = line_total();
        return;
    }

    head = current_node;

    // First Line in List, just Clear.
    current_node->data.erase();
    Line.erase();
    Col = 1;
    Row = 1;
    Num = 1;
    Tot = line_total();

}

/**
 * Full Screen Editor - Modified Delete Line, Deletes Current Line, then Moves Lower to Current
 * when no more lower lines starts moving up.. CTRL+Y
 */
void msg_fse::delete_line()
{
    LineRec *tmp;

    int Total = line_total();
    Tot = Total;

    if(current_node == 0) {
        return;
    }

    if (current_node->dn_link != 0) {
        tmp = current_node;

        if (current_node->up_link != 0) {
            current_node->up_link->dn_link = current_node->dn_link;
        }
        current_node = current_node->dn_link;

        if (tmp->up_link != 0) {
            current_node->up_link = tmp->up_link;
        } else {
            current_node->up_link = 0;
            head = current_node;
            if (current_node->dn_link == 0) {
                last = current_node;
            }
        }
        delete tmp;
        tmp = 0;

        Line = current_node->data;
        Col = 1;
        --Tot;

    }
    else {
        if (current_node->up_link != 0) {

            // Delete Line
            tmp = current_node;
            current_node = current_node->up_link;
            current_node->dn_link = 0;
            delete tmp;
            tmp = 0;

            last = current_node;
            Line = current_node->data;
            Col = 1;
            --Row;
            --Num;
            --Tot;
        }
        else {
            head = current_node;
            last = current_node;
            current_node->data.erase();
            Line.erase();
            Col = 1;
            Row = 1;
            Num = 1;
            Tot = 1;
        }
    }
}

/**
 * Full Screen Editor - Remove all enteries in list
 */
void msg_fse::dispose_list()
{
    LineRec *tmp;
    current_node = head;
    while(current_node != 0) {
        tmp = current_node;
        current_node = current_node->dn_link;
        delete tmp;
        tmp = 0;
    }

    head = 0;
    current_node = 0;
    last = 0;
}

/**
 * Full Screen Editor - Remove all enteries in list
 */
void msg_fse::clear_all()
{
    dispose_list();
    Line.erase();
    Num = 1;
    Col = 1;
    Row = 1;
    Tot = 1;
}

/**
 * Full Screen Editor - Insert line in middle of paragraph on screen
 */
void msg_fse::insert_line()
{
    current_node->data = Line;
    add_to_list("");
    ++Tot;
    Line.erase();
    Col = 1;
    box_redraw();
}

/**
 * Full Screen Editor - Clear Message
 */
void msg_fse::box_clear()
{
    int boxsize = Bot - Top;
    std::string _editbox = "";
    char capture[200]= {0};

    for (int t = 0; t != boxsize; t++) {
        sprintf(capture, "\x1b[%i;%iH\x1b[K", (Top)+t, 1);
        _editbox += capture;
    }

    sprintf(capture, "\x1b[%i;%iH", Top+1, 1);
    _editbox += capture;

    putline((char *)_editbox.c_str());
    clear_all();
    Tot = 1;
    Num = 1;
    Row = 1;
    add_to_list("");
}

/**
 * Full Screen Editor - ReDraw Message
 */
void msg_fse::box_redraw()
{
    LineRec *cur;
    char capture[200]= {0};
    std::string _editbox = "";

    pipe2ansi(sTEXT_COLOR);

    // Check here if We are Scrolling all the lines in the Box up by 1
    if (Row == (Bot-Top+1)) {
        --Row;
    }

    // Fist Get Box Size
    int boxsize = Bot - Top;
    cur = current_node;

    // Start at lines above current row to top of box
    int Cnt2 = Row;
    while (Cnt2 > 1) {
        if (!move_up()) break;
        --Cnt2;
    }

    //Now clear the box First
    for (int t = 0; t != boxsize; t++) {
        sprintf(capture, "\x1b[%i;%iH\x1b[K", (Top)+t, 1);
        _editbox += capture;
    }

    // Now Grab as many lines as will fit in the box
    int i = 0;
    for (i = 0; i < boxsize; i++) {
        sprintf(capture, "\x1b[%i;%iH%s", Top+i, 1, current_node->data.c_str());
        _editbox += capture;
        if (!move_down()) break;
    }

    current_node = cur;
    sprintf(capture, "\x1b[%i;%iH", Row+Top-1, Col);
    _editbox += capture;
    putline((char *)_editbox.c_str());
}

/**
 * Full Screen Editor - Normal word wrapping, End of Line.
 */
void msg_fse::word_wrap()
{
    std::string::size_type iPos = 0;
    std::string::size_type id1;

    iPos = Line.find_last_of(" ",MAX_WIDTH-1);
    int iLineSize = Line.size();

    // Cut out Last Word and move to next line if a Space is found in the line.
    if (iPos != std::string::npos) {
        TLine = Line.substr(iPos+1,iLineSize-1);
        Line.erase(iPos,iPos-iLineSize-1);
        current_node->data = Line;
        add_to_list("");
        Line.erase();
        Line = TLine;

        while ((id1=Line.find(" ",0)) != std::string::npos) Line.erase(id1,1);
        current_node->data = Line;

        Col  = Line.size()+1;
        ++Row;
        ++Num;
        ++Tot;

        box_redraw();
    }
    // No Space was found ,so just grab the last letter in the line!
    else {
        TLine = Line.substr(iLineSize-1,1);
        Line.erase(iLineSize-1,1);
        current_node->data = Line;

        add_to_list("");
        Line.erase();
        Line = TLine;
        while ((id1=Line.find(" ",0)) != std::string::npos) Line.erase(id1,1);
        current_node->data = Line;

        Col  = Line.size()+1;
        ++Row;
        ++Num;
        ++Tot;

        box_redraw();
    }
}

/**
 * Full Screen Editor - Word wrapping, reformat Paragraph
 */
int msg_fse::format_paragraph()
{
    std::string sTmpLine;

    std::string::size_type iLineSize = 0;   // Current Line Size
    std::string::size_type iPos = 0;        // String Position
    int iRet = 0;

    Line.erase();
    Line = current_node->data;
    iLineSize = Line.size();

    //Find the Lesser of, if it's the line, make sure it's less then last char!
    if ((MAX_WIDTH-1) < (iLineSize-1)) {
        iRet = MAX_WIDTH-1;
    } else {
        iRet = iLineSize-2;
    }

    if (iLineSize >= MAX_WIDTH) {
        iPos = Line.find_last_of(" ", iRet);
        if (iPos != std::string::npos) {
            sTmpLine = Line.substr(iPos,iPos-(iLineSize-1));
            Line.erase(iPos,iPos-(iLineSize-1));
            current_node->data.erase();
            current_node->data = Line;
            iRet = TRUE;
        } else {
            sTmpLine = Line.substr(iRet,iRet-(iLineSize-1));
            Line.erase(iRet,iRet-(iLineSize-1));
            current_node->data.erase();
            current_node->data = Line;
            iRet = FALSE;
        }

        // Append
        if (current_node->dn_link != 0) {
            current_node = current_node->dn_link;

            //remove only leading Spaces
            while (1) {
                if (sTmpLine[0] == ' ')
                    sTmpLine.erase(0,1);
                else break;
            }
            // Remove only Trailing Spaces.
            while (1) {
                if (sTmpLine[(int)sTmpLine.size()-1] == ' ')
                    sTmpLine.erase((int)sTmpLine.size()-1,1);
                else break;
            }

            if (iRet == TRUE)
                current_node->data = sTmpLine + " " + current_node->data;
            else
                current_node->data = sTmpLine + current_node->data;

            if (current_node->data.size()-1 >= MAX_WIDTH) {
                return TRUE;
            } else
                return FALSE;
        }

        // Create New
        else {
            add_to_list("");
            current_node->data.erase();

            //remove only leading Spaces
            while (1) {
                if (sTmpLine[0] == ' ')
                    sTmpLine.erase(0,1);
                else break;
            }
            // Remove only Trailing Spaces.
            while (1) {
                if (sTmpLine[(int)sTmpLine.size()-1] == ' ')
                    sTmpLine.erase((int)sTmpLine.size()-1,1);
                else break;
            }
            
            current_node->data = sTmpLine;
            ++Tot;
            current_node = current_node->up_link;
            return FALSE;
        }
    }
    
    return FALSE;
}

/**
 * Full Screen Editor - Extended Word Wrapping, For Insert Pushing inside Current Line
 */
void msg_fse::word_wrapex()
{
    LineRec *tmp_node;
    current_node->data.erase();
    current_node->data = Line;

    std::string sTmpLine;

    std::string::size_type iLineSize;
    std::string::size_type iPos;
    std::string::size_type id1;
    int iRet;

    iLineSize = Line.size();

    //Find the Lesser of, if it's the line, make sure it's less then last char!
    if ((MAX_WIDTH-1) < (iLineSize-1)) {
        iRet = MAX_WIDTH-1;
    } else {
        iRet = iLineSize-2;
    }

    // Were going to find the last word, and cut that off of current and move to next!
    iPos = Line.find_last_of(" ",iRet);
    if (iPos != std::string::npos) {
        sTmpLine = Line.substr(iPos,iPos-(iLineSize-1));
        Line.erase(iPos,iPos-(iLineSize-1));
        current_node->data.erase();
        current_node->data = Line;
        iRet = TRUE;
    } else {
        sTmpLine = Line.substr(iLineSize-1,1);
        Line.erase(iLineSize-1,1);
        current_node->data.erase();
        current_node->data = Line;
        iRet = FALSE;
    }

    // Append
    if (current_node->dn_link != 0) {
        tmp_node = current_node;
        current_node = current_node->dn_link;

        while ((id1=sTmpLine.find(" ",0)) != std::string::npos) sTmpLine.erase(id1,1);
        if (iRet == TRUE)
            current_node->data = sTmpLine + " " + current_node->data;
        else
            current_node->data = sTmpLine + current_node->data;

        if (current_node->data.size() >= MAX_WIDTH) {
            do {
                iRet = format_paragraph();
            } while(iRet == TRUE);
        }

        current_node = tmp_node;
        Line.erase();
        Line = current_node->data;

    }
    // Create New
    else {
        add_to_list("");
        current_node->data.erase();

        while ((id1=sTmpLine.find(" ",0)) != std::string::npos) sTmpLine.erase(id1,1);
        current_node->data = sTmpLine;
        ++Tot;
        current_node = current_node->up_link;
    }

    Line.erase();
    Line = current_node->data;
    box_redraw();
}

/**
 * Full Screen Editor - Add char to current line, and test what mode were in
 */
void msg_fse::add_char(unsigned char c)
{
    std::string sCharTemp;

    int iRepeat = 0;
    int iLineSize;
    int iColPos;

    iLineSize = Line.size();
    iColPos   = Col-1;

    sCharTemp = c;

    // Test if were Pushing from Middle of the Line.
    if (iColPos < iLineSize)
        bPush = TRUE;
    else
        bPush = FALSE;

    // First Eval if were pushing text from the middle of a line.
    if (bPush == TRUE) {
        Line.insert(iColPos, sCharTemp);
    }
    // Else were Appending Normal Text to the End of the Line.
    else {
        // If Cursor is past the allocated space, Buffer Line to Current Position!
        if (iColPos > iLineSize) {
            iRepeat = (iColPos - iLineSize);
            for(int i = 0; i < iRepeat; i++) {
                Line += ' ';
            }
        }

        if (c == '%') {
            Line += "%";

        } else {
            Line += sCharTemp;
        }
    }
    
    ++Col;
    ++iColPos;
    iLineSize = Line.size();

    current_node->data.erase();
    current_node->data = Line;

    // If we were pushing from the middle of the line,
    if (bPush == TRUE) {
        if (iLineSize >= MAX_WIDTH) {
            word_wrapex();
        } else {
            return;
        }
    }
    // Else We've appended to the End of the Line
    else {
        if (iLineSize >= MAX_WIDTH)
            word_wrap();
        else { // Nothing modified, just return.
            return;
        }
    }

    box_redraw();
}

/**
 * Full Screen Editor - Move up A Line
 */
void msg_fse::up_arrow()
{
    current_node->data = Line;
    move_up();
    Line = current_node->data;

    if (Row > 1) --Row;
    if (Num > 1) --Num;

    if (Row != 1)
        write(0,"\x1b[A",3);
    else
        box_redraw();
}

/**
 * Full Screen Editor - Move down A Line
 */
void msg_fse::dn_arrow()
{
    if (Tot == Num) return;
    current_node->data = Line;

    if (Row != (Bot-Top)) {
        ++Row;
    }
    ++Num;

    move_down();
    Line = current_node->data;

    if (Row != (Bot-Top))
        write(0,"\x1b[B",3);
    else
        box_redraw();
}

/**
 * Full Screen Editor - Move right
 */
void msg_fse::rt_arrow()
{
    if (Col < MAX_WIDTH) {
        ++Col;
        write(0,"\x1b[C",3);
    } else if (Col > MAX_WIDTH) {
        --Col;
    }
}

/**
 * Full Screen Editor - Move left
 */
void msg_fse::lt_arrow()
{
    if (Col != 1) {
        write(0,"\x1b[D",3);
        --Col;
    }
}

/**
 * Full Screen Editor - Move Home
 */
void msg_fse::home_cursor()
{
    Col = 1;
    char sLine[50]= {0};
    sprintf(sLine,"\x1b[%i;%iH", Row+Top-1, Col);
    putline(sLine);
}

/**
 * Full Screen Editor - Move END
 */
void msg_fse::end_cursor()
{
    Col = Line.size()+1;
    char sLine[50]= {0};
    sprintf(sLine,"\x1b[%i;%iH", Row+Top-1, Col);
    putline(sLine);
}

/**
 * Full Screen Editor - Move Last Line in List
 */
void msg_fse::Last_Line()
{
    if(head == 0) {
        return;
    }

    current_node = head;
    while (1) {
        if (current_node->dn_link == 0) {
            break;
        }
        current_node = current_node->dn_link;
    }
}

/**
 * Full Screen Editor - Last Page, Scroll from Bottom up
 */
void msg_fse::box_scrolldn()
{
    std::string _editbox;
    char capture[255]= {0};

    // Set Text Color
    pipe2ansi(sTEXT_COLOR);

    // Make sure we can go down another page..
    int boxsize = Bot - Top;   // Get boxsize

    //Now clear the box First
    for (int t = 0; t < boxsize; t++) {
        sprintf(capture, "\x1b[%i;%iH\x1b[K", (Top)+t, 1);
        _editbox += capture;
    }

    // Jump to last line
    Last_Line();

    // Move up As Many lines that will fit in the box
    for (int i = 1; i < boxsize; i++) {
        if (!move_up()) break;
    }
    // Now Grab as many lines as will fit in the box
    for (int i = 1; i < boxsize+1; i++) {
        sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str());
        _editbox += capture;
        if (!move_down()) break;
    }
    // Display Message
    pipe2ansi((char *)_editbox.c_str());
    Line = current_node->data;
}

/**
 * Full Screen Editor - Parse Subject for Regarding (reply)
 */
int msg_fse::parsere(char *qText)
{
    std::string reguard;
    reguard = qText;
    std::string::size_type id1;

    id1 = reguard.find("|RE",0);
    if (id1 != std::string::npos) {
        reguard.replace(id1,3,mHLocal.to);
    }
    id1 = reguard.find("|CR",0);
    if (id1 != std::string::npos) {
        reguard.replace(id1,3,"\r");
    }

    sprintf(qText,"%s",(char *)reguard.c_str());
    return TRUE;
}

/**
 * Full Screen Editor - Options with ESC or / is hit.
 */
int msg_fse::options_prompt(unsigned char c)
{
    menu_func _mnuf;
    msg_quote _msgq;

    char mString[10] = {0};
    unsigned char ch;
    char qText[100]= {0};

    std::string qbuf;
    std::string QuoteStr;
    std::string sOrgMsg;
    int idx = 0;
    int id1 = 0;

    // Check if Pass through FSE input ie.. /s sent. or Lightbar Menu
    if (c == '\0') {
        _mnuf._premenu.clear();
        _mnuf._premenu = _mnuf._curmenu;
        _mnuf._curmenu.clear();
        _mnuf._curmenu = sMENU_PROMPT;
        _mnuf._loadnew = true;
        _mnuf.menu_readin();
    }

    while (1) {
        if (c == '\0') {
            _mnuf.menu_proc(mString);
            ch = mString[1];
        } else {
            mString[0] = '!';
            ch = c;
        }

        // For Menu CmdKey Input
        if (mString[0] == '!')
            switch (toupper(ch)) {

            case 'S' : // Save Message
                if (line_total() == 1 && Line.size() == 0 && current_node == head) {
                    dispose_list();
                    return FALSE;
                }

                if (Line != "") {
                    current_node->data = Line;
                }

                // Redraw..
                ansi_file(sANSI_FILE);
                box_redraw();
                save_all();
                dispose_list();
                return TRUE;

            case 'A' : // Abort Message
                dispose_list();
                return FALSE;

            case 'X' : // Continue with Current Message
                ansi_file(sANSI_FILE);
                box_redraw();
                return (2);

            case '?' : // Display list of HotKey Commands
                ansiPrintf((char *)"fsehelp");
                getkey(true);

                if (c == '\0') break;
                else {
                    ansi_file(sANSI_FILE);
                    box_redraw();
                    return (2);
                }

            case 'Q' : // Quote Message Text
                if (!MSG_REPLY) {
                    ansi_file(sANSI_FILE);
                    box_redraw();
                    return(2);
                }
                _msgq.setup_quoter();
                _msgq.StartQuoter(reader_list->head,qbuf);

                // Get Quoted Text from QUOTE Class then throw it in the FSE
                if (qbuf.size() > 0) {
                    _lang.lang_get(qText,29);
                    if (strlen(qText) > 0) {
                        parsere(qText);
                        QuoteStr = qText;
                        QuoteStr += '\r';
                    }
                    // Text
                    QuoteStr += qbuf;
                    _lang.lang_get(qText,30);
                    if (strlen(qText) > 0) {
                        parsere(qText);
                        QuoteStr += qText;
                        QuoteStr += '\r';
                    }

                    // Append Quoted Text into message.
                    PutBuffer((char *)QuoteStr.c_str());
                    ansi_file(sANSI_FILE);
                    add_to_list("");
                    MoveAllDown();
                } else {
                    ansi_file(sANSI_FILE);
                    box_redraw();
                }
                return (2);

            case 'C' : // Clear Message
                box_clear();
                return (2);

            case ']': // Next Theme
                idx = thisuser->fsetheme;
                ++idx;
                id1 = change_theme(idx);

                if (id1 == FALSE) {
                    change_theme(thisuser->fsetheme);
                } else {
                    thisuser->fsetheme = idx;
                }
                ansi_file(sANSI_FILE);
                box_redraw();
                return (2);

            case '[': // Previous Theme

                idx = thisuser->fsetheme;
                if (idx != 0) {
                    --idx;
                }
                if (idx == 0) {
                    id1 = change_theme();
                } else {
                    id1 = change_theme(idx);
                }

                if (id1 == FALSE) {
                    change_theme(thisuser->fsetheme);
                } else {
                    thisuser->fsetheme = idx;
                }
                ansi_file(sANSI_FILE);
                box_redraw();
                return (2);
            }
    }
}

/**
 * Full Screen Editor - Insert Lines Comming from Message 
 * Reader stright into FSE List for Message Edit.
 * Used on Reply or Edit.
 */
void msg_fse::insert_lines()
{
    LineRec *lineTmp;
    if(reader_list->head == 0) {
        return;
    }

    dispose_list();
    lineTmp = reader_list->head;

    int i = 0;
    while ( lineTmp != 0 ) {
        add_to_list(lineTmp->data);
        if (lineTmp->dn_link == 0 || lineTmp->dn_link == reader_list->head) break;
        lineTmp = lineTmp->dn_link;
        ++i;
    }
    return;

}

/**
 * Full Screen Editor - Split line into seperate list of words.
 */
std::vector<std::string> inline msg_fse::split_string(const std::string &source,
        const char *delimiter,
        bool keepEmpty)
{
    std::vector<std::string> results;

    size_t prev = 0;
    size_t next = 0;

    while ((next = source.find_first_of(delimiter, prev)) != std::string::npos) {
        if (keepEmpty || (next - prev != 0)) {
            results.push_back(source.substr(prev, next - prev));
        }
        prev = next + 1;
    }

    if (prev < source.size()) {
        results.push_back(source.substr(prev));
    }

    return results;
}

/**
 * Full Screen Editor - Handle DEL Key
 */
void msg_fse::delete_key()
{
    std::string::size_type id1 = 0;
    std::string sTmp;  // Delete lineup clears line, so we need to store it.
    int TCol = Col;    // Keep track of original position

    char sLine[1024]= {0};

    int iRepeat = 0;
    int iLineSize;
    int iColPos;
    int bPush;

    iLineSize = Line.size();
    iColPos   = Col-1;

    //if ((signed)Line.size() < (Col-1))
    if (Col < (signed)Line.size()+1) {
        Line.erase(Col-1,1);
        current_node->data = Line;
        sprintf(sLine,"\x1b[%i;%iH\x1b[K%s\x1b[%i;%iH", Row+Top-1, 1,Line.c_str(), Row+Top-1, Col);
        putline(sLine);
    }
    else {
        current_node->data = Line;

        // Second we need to check if cursor is past line size, if so we need to pad it
        // So when we pull new text up it lines with cursor, not the end of the line.
        if (iColPos < iLineSize)
            bPush = TRUE;
        else
            bPush = FALSE;

        // First Eval if were pushing text from the middle of a line.
        if (bPush == FALSE) {
            // If Cursor is past the allocated space, Buffer Line to Current Position!
            if (iColPos > iLineSize) {
                iRepeat = (iColPos - iLineSize);
                for(int i = 0; i < iRepeat; i++) {
                    Line += ' ';
                }
            }
        }

        current_node->data = Line;
        TLine = current_node->data;

        if (!move_down())
            return;

        sTmp = current_node->data;

        // First check if line below is empty,
        // if it is, just delete it and return
        if (sTmp.size() == 0) {
            ++Row;
            delete_line_up();
            --Row;
            Col = TCol;
            box_redraw();
            return;
        }

        // First Check if Currnet Line we are on TLine has room on it
        if ((TLine.size()) <= MAX_WIDTH) {
            if (TLine.size()+sTmp.size() <= MAX_WIDTH) {
                ++Num;
                delete_line_up();
                current_node->data += " " + sTmp;
                Line = current_node->data;
                Col = TCol;
                box_redraw();
            } else {
                id1 = sTmp.size()-1;
                id1 = sTmp.rfind(" ",id1);

                while ( (TLine.size()+id1+1) > MAX_WIDTH && id1 != std::string::npos) {
                    id1 = sTmp.rfind(" ",id1-1);
                }

                if ((TLine.size()+id1+1) <= MAX_WIDTH && id1 != std::string::npos) {
                    move_up();
                    TLine.append(sTmp.substr(0, id1));
                    current_node->data = TLine;

                    move_down();
                    sTmp.erase(0,id1+1);
                    current_node->data = sTmp;

                    move_up();
                    Line = current_node->data;
                    box_redraw();
                }
            }
        }
    }
}


/**
 * Full Screen Editor - Main FSE Input Parsing Function
 */
int msg_fse::poll_chr(int reply, int msg_edit, MsgHead *mH, msg_readll *mL)
{
    // Variables for Client Side Key Sequences
    std::string changemsg;

    UserRec usr;

    int clientBS  = BACKSPACE;
    int clientDEL = DELETE;

    char sLine[255]= {0};
    std::string  tmp2;

    msg_quote    _msgq;
    std::string  sOrgMsg;

    memcpy(&mHLocal,mH,sizeof(MsgHead));

    if (fse_parse(thisuser->fsetheme) == FALSE)
        fse_parse();

    // Make sure we have a 1 line current in here.
    if (head == 0) {
        add_to_list("");
        current_node = head;
    }

    // Char Function Variables
    OVERWRITE  = false;     // Default to Insert Mode
    Row = 1;                // Default Row
    Col = 1;                // Default Colume
    Num = 1;                // Current Line Number in List
    Tot = 1;                // Total Lines
    unsigned char c;        // Holds Char Input

    int TCol = 0;           // Temp Column
    std::string::size_type id1;

    // Setup Line and Cursor Position top of Input Box
    sprintf(sLine,"\x1b[%i;%iH", Top, Col);
    putline(sLine);
    std::string QuoteStr;

    // If were Replying to a message, Start Message Quoter
    if (reply == TRUE) {
        MSG_REPLY = TRUE;
        reader_list = mL;       // Copy of Original Message in Link List.
        options_prompt('Q');    // Jump right into Quoter if a Message Reply.
        line_total();
        Line = current_node->data;
    } else {
        MSG_REPLY = FALSE;
        if (msg_edit == TRUE) {
            // Populates Original Message Into Editor.
            reader_list = mL;   // Copy of Original Message in Link List.
            insert_lines();     // Copy Original Read_list to mLink for FSE.
            add_to_list("");    // Add Line to the End.
            move_first();
            Line = current_node->data;
            Num = 1;
            Row = 1;
            line_total();
        }
        ansi_file(sANSI_FILE);
        box_redraw();
    }

    while (1) {
        if (thisuser->bsdel_swap == TRUE) {
            clientBS  = DELETE;
            clientDEL = BACKSPACE;
        } else {
            clientBS  = BACKSPACE;
            clientDEL = DELETE;
        }

        sprintf(sLine,"%s%s%.2d%s%.2d\x1b[%i;%iH",sTEXT_COLOR,sRow,Num,sCol,Col,Row+Top-1, Col);
        pipe2ansi(sLine);

        // Get char Input for FSE.
        strcpy(EscapeKey,"");
        c = getkey(true);

        current_node->data = Line;

        // Swap BackSpace and DEL
        if (c == 0x18) { // CTRL X
            changemsg.erase();
            if (thisuser->bsdel_swap == TRUE) {
                pipe2ansi((char *)"|CS|CR|15Toggled Backspace & Delete keys to |07[|12Telnet Client Mode|07]|15.|CR|CR|PA |CS");
                thisuser->bsdel_swap = FALSE;
            } else {
                pipe2ansi((char *)"|CS|CR|15Toggled Backspace & Delete keys to |07[|12Terminal Mode|07]|15.|CR|CR|PA |CS");
                thisuser->bsdel_swap = TRUE;
            }

            // Save Change
            usr = *thisuser;
            users _usr;
            _usr.users_write(&usr,usr.idx);

            ansi_file(sANSI_FILE);
            box_redraw();
        }
        // Do Delete
        else if (c == clientDEL ) {
            delete_key();
        }

        // Do Destructive Backspace
        // BS = 0x08 - in VT100 Backspace can be 127 as well, but
        // to avoid confusion this has to be disabled unless we make a toggle.
        // Becaseu it's also used as DEL.
        else if (c == clientBS) {
            if ((signed)Line.size() < (Col-1)) {
                lt_arrow();
            } else if (Line == "") { // Empty, just move cursor up and end!
                delete_line_up();
                if (Row > 1) --Row;
                box_redraw();
            }
            // Normal Destructive Backspace..  Normal from end of Line!
            else if (Col != 1 ) {
                Line.erase(Col-2, 1);
                current_node->data = Line;
                --Col;
                sprintf(sLine,"\x1b[%i;%iH\x1b[K%s\x1b[%i;%iH", Row+Top-1, 1,Line.c_str(), Row+Top-1, Col);
                putline(sLine);
            } else {
                tmp2 = Line;
                // Were in Insert Mode, Made sure were not on the First Row!
                if (Num != 1) { // if 1st Row, Do Nothing!!!
                    if ((signed)Line.size()+1 > Col) {   // Were in Insert Mode
                        current_node->data = Line;
                        move_up();
                        TLine = current_node->data;
                        TCol  = TLine.size()+1;
                        move_down();
                        Line  = current_node->data;

                        if ((TLine.size()) <= MAX_WIDTH) {
                            if (TLine.size()+Line.size() <= MAX_WIDTH) {
                                delete_line_up();
                                if (Row > 1) --Row;
                                Col = current_node->data.size()+1;
                                current_node->data += " " + tmp2;
                                Line = current_node->data;
                                box_redraw();
                            } else {
                                id1 = tmp2.size()-1;
                                id1 = tmp2.rfind(" ",id1);

                                while ( (TLine.size()+id1+1) > MAX_WIDTH && id1 != std::string::npos) {
                                    id1 = tmp2.rfind(" ",id1-1);
                                }

                                if ((TLine.size()+id1+1) <= MAX_WIDTH && id1 != std::string::npos) {
                                    move_up();

                                    TCol = TLine.size()+1;
                                    TLine.append(Line.substr(0, id1));
                                    current_node->data = TLine;

                                    // Move back down to original line, and cut out text moved
                                    // To the Above line.
                                    move_down();
                                    Line.erase(0,id1+1);
                                    current_node->data = Line;
                                    move_up();

                                    // Reset Line Data to Current Line
                                    Line = current_node->data;
                                    Col = TCol;
                                    if (Row > 1) --Row;
                                    if (Num > 1) --Num;
                                    box_redraw();
                                }
                            }
                        }
                    } else {
                        delete_line_up();
                        if (Row > 1) --Row;
                        box_redraw();
                    }
                }
            }
        }
        // Escaped Keys.. Arrow Keys etc..
        else if (c == ESC) {
            switch (EscapeKey[1]) {

            case 'A' : // Up Arrow
                up_arrow();
                break;

            case 'B' : // Down Arrorw
                dn_arrow();
                break;

            case 'C' : // Right Arrow
                rt_arrow();
                break;

            case 'D' : // Left Arror
                lt_arrow();
                break;

            case '1' : // Home Cursor
            case 'H' :
                home_cursor();
                break;

            case '4' : // End Cursor
            case 'K' : // ?
            case 'F' : // VT100
                end_cursor();
                break;

                //case '1' : // Delete
            case '3' : // Delete
                // Moved to Function
                delete_key();
                break;
            case '5' : // Shift PageUP
            case 'V' :
                break;

            case '6' : // Shift PageDN
            case 'U' :
                break;

            case '\0' :
            case ' '  : // ESC
                id1 = options_prompt();
                if (id1 == 2) {
                    ansi_file(sANSI_FILE);
                    box_redraw();
                    break;
                } // Continue
                else {
                    return id1;
                }

            default  :
                break;
            }
        }
        // Handle ENTER \ New Line
        else if (c == '\r' || c == '\n') {
            // Note, add if cursor is in middle of line.. cut and move to next line!
            // 1st Check if char Position is in the Middle of the Line
            if ((int)Line.size() > (Col-1)) {
                TLine = Line.substr(Col-1, Line.size());
                Line.erase(Col-1, (Col-1)-Line.size());
                current_node->data = Line;
                add_to_list("");
                current_node->data = TLine;
                Line = current_node->data;
                Col  = 1;

                ++Tot;
                ++Row;
                ++Num;
                box_redraw();
            } else {
                current_node->data = Line;
                add_to_list("");
                Col  = 1;
                Line = "";
                ++Tot;
                ++Row;
                ++Num;
                box_redraw();
            }
        }
        // Check for Normal Letters, Numbers, Ascii Printable chars Etc..
        else if (((int)c > 31 && (int)c <= 255) || c == '~' || c == '`') {
            // Jump into Options Prompt...
            if (Col == 1 && c == '/') {
                id1 = options_prompt();
                if (id1 == 2) {
                    ansi_file(sANSI_FILE);
                    box_redraw();
                } // Continue
                else {
                    return id1;
                }
            } else {
                add_char(c);
                sprintf(sLine,"\x1b[%i;%iH\x1b[K%s\x1b[%i;%iH", Row+Top-1, 1,(char *)Line.c_str(), Row+Top-1, Col);
                putline(sLine);

            }
        }
        // Delete Current Line
        else if (c == CTRLY) { // CTRL Y
            delete_line();
            box_redraw();
        }
        // Insert New  Line
        else if (c == CTRLN) { // CTRL N
            insert_line();
        }
    }
}

/**
 * Full Screen Editor - Change ANSI Template if Exists
 */
int msg_fse::change_theme(int idx)
{
    if (fse_parse(idx) == FALSE)
        return FALSE;

    thisuser->fsetheme = idx;

    UserRec usr;
    usr = *thisuser;
    users _usr;
    _usr.users_write(&usr,usr.idx);

    return TRUE;
}

/**
 * Full Screen Editor - Parse ANSI Template
 */
void msg_fse::ansi_file(char *filename)
{
    int c = 0;
    std::string temp = "";
    std::string path = ANSIPATH;
    path += filename;
    path += ".ans";
    char MCI[3]     = {0};
    char sTemp[255] = {0};

    std::string::size_type id1 = 0;

    sprintf(sTemp,"#%i. %s",thisuser->fsetheme+1,sTHEME_NAME);
    FILE *inStream;
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL) {
        return;
    }

    do {
        memset(&MCI,0,sizeof(MCI));
        c = getc(inStream);
        if (c == '%') {
            MCI[0] = getc(inStream);
            MCI[1] = getc(inStream);
            if (strcmp(MCI,"TO") == 0) {
                temp += mHLocal.to;
            } else if (strcmp(MCI,"FM") == 0) {
                temp += mHLocal.from;
            } else if (strcmp(MCI,"MA") == 0) {
                temp += mHLocal.area;
            } else if (strcmp(MCI,"SU") == 0) {
                temp += mHLocal.subj;
            } else if (strcmp(MCI,"TH") == 0) {
                temp += sTemp;
            } else if (strcmp(MCI,"BS") == 0) {
                if (thisuser->bsdel_swap == TRUE)
                    temp += "Terminal Mode";
                else
                    temp += "Telnet Mode";
            } else {
                temp += c;
                temp += MCI;
            }
            temp += getc(inStream);
        } else if (c == '\n') {} //temp += '\r';
        else temp += c;
    } while (c != EOF);
    fclose(inStream);
    temp += "\n"; // Extra Space to Fix Next Ansi Sequence

    do {
        id1 = temp.find("\x1b[2J",0);
        if (id1 != std::string::npos) {
            temp.replace(id1,4,"|CS");
        }
    } while(id1 != std::string::npos);

    pipe2ansi((char *)temp.c_str());
}

/**
 * Full Screen Editor INI - Check INI Exists
 */
bool msg_fse::fse_exists()
{
    std::string path = INIPATH;
    path += "fse.ini";

    FILE *stream;
    stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
        perror("Error unable to read fse.ini, check permissions!");
        return false;
    }
    fclose(stream);
    return true;
}

/**
 * Full Screen Editor INI - Generate INI
 */
void msg_fse::fse_create()
{
    std::string name = INIPATH;
    name += "fse.ini";

    ofstream outStream2;
    outStream2.open( name.c_str(), ofstream::out | ofstream::trunc );
    if (!outStream2.is_open()) {
        printf( "\nError Creating: %s \n", name.c_str());
        return;
    }

    outStream2 << "# Generated By Program " << endl;
    outStream2.close();
    return;
}

/**
 * Full Screen Editor INI - Parse Helper
 */
void msg_fse::fse_chkpar(std::string &temp)
{
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
    temp = temp1;
}

/**
 * Full Screen Editor INI - get INI Values
 */
void msg_fse::fse_check(std::string cfgdata)
{
    std::string::size_type id1 = 0;
    if (cfgdata[0] == '#') return;
    else if (cfgdata.find("set THEME_NAME ", 0)
             != std::string::npos) {
        fse_chkpar(cfgdata);
        strcpy(sTHEME_NAME,(char*)cfgdata.c_str());
    } else if (cfgdata.find("set ANSI_FILE ", 0)
               != std::string::npos) {
        fse_chkpar(cfgdata);
        strcpy(sANSI_FILE,(char*)cfgdata.c_str());
    } else if (cfgdata.find("set TEXT_COLOR ", 0)
               != std::string::npos) {
        fse_chkpar(cfgdata);
        strcpy(sTEXT_COLOR,(char*)cfgdata.c_str());
    } else if (cfgdata.find("set MENU_PROMPT ", 0)
               != std::string::npos) {
        fse_chkpar(cfgdata);
        strcpy(sMENU_PROMPT,(char*)cfgdata.c_str());
    } else if (cfgdata.find("set TOP ", 0)
               != std::string::npos) {
        fse_chkpar(cfgdata);
        id1 = atoi(cfgdata.c_str());
        Top = id1;
    } else if (cfgdata.find("set BOT ", 0)
               != std::string::npos) {
        fse_chkpar(cfgdata);
        id1 = atoi(cfgdata.c_str());
        Bot = id1;
    } else if (cfgdata.find("set ROW ", 0)
               != std::string::npos) {
        fse_chkpar(cfgdata);
        strcpy(sRow,(char*)cfgdata.c_str());
    } else if (cfgdata.find("set COL ", 0)
               != std::string::npos) {
        fse_chkpar(cfgdata);
        strcpy(sCol,(char*)cfgdata.c_str());
    }
}

/**
 * Full Screen Editor INI - Read / Parse INI File.
 */
int msg_fse::fse_parse(int idx)
{
    if (!fse_exists()) {
        perror("Error unable to open fse.ini, check permissions!");
    }

    char name[1024]  = {0};
    char name2[1024] = {0};

    sprintf(name,"%sfse.ini",INIPATH);
    sprintf(name2,"%sfse%i.ini",INIPATH,idx);

    // Check if Default Theme, else look for requested.
    if (idx != 0) strcpy(name,name2);

    FILE *stream;
    stream = fopen(name,"rb+");
    if(stream == NULL) {
        return FALSE;
    }
    fclose(stream);


    ifstream inStream;
    inStream.open( name );
    if (!inStream.is_open()) {
        perror("Error unable to parse fse.ini, check permissions!");
        return FALSE;
    }

    std::string cfgdata;
    for (;;) {
        std::getline(inStream,cfgdata);
        fse_check(cfgdata);
        if(inStream.eof()) break;
    }
    inStream.close();
    return TRUE;
}
