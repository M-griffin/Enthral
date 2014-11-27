/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
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

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$


# include "struct.h"

# include "menu_func.h"
# include "msg_readll.h"
# include "conio.h"

# include <cstdio>
# include <cstring>
# include <cstdlib>
//# include <string>


using namespace std;


/**
 * Message Link List Class
 */
msg_readll::msg_readll()
{

    // Initalize Link List Class Defaults
    //head = new LineRec;
    //head->data = "";
    //head->up_link  = head;
    //head->dn_link  = head;
    //current_node   = head;

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
    strcpy ( LoColor,"" );         // Message Text Color
    strcpy ( HiColor,"" );         // Message Text HiLight Color
    strcpy ( TEXT_COLOR,"" );   // Message Text Color
    strcpy ( QUOTE_COLOR,"" );  // Message Quote
    strcpy ( SYS_COLOR,"" );    // --- BBS Version
    strcpy ( ORIGIN_COLOR,"" ); //  * Origin Line ...
}

msg_readll::~msg_readll()
{

    //if (head) delete head;
    //if (current_node) delete current_node;
    //if (last) delete last;

    //head = 0;
    //current_node = 0;
    //last = 0;

}


/**
 * Message Link List - Setup Message Quoter
 */
void msg_readll::initQuote ( char *sLow, char *sHigh, int q )
{

    quote = q;                // In Quote Editor for Reader.
    strcpy ( LoColor,sLow );    // Message Text Color
    strcpy ( HiColor,sHigh );    // Message Text HiLight Color
}


/**
 * Message Link List - Setup Title & Area/User Lists
 */
void msg_readll::initList ( char *sLow, int alist )
{

    arealist = alist;       // Set if were using Area List
    // Mainly for Box_Start() Proper list placement.
    strcpy ( LoColor,sLow ); // Message Text Color
}

/**
 * Message Link List - for Last Page in list to scroll bottom up
 */
void msg_readll::togglePageScroll ( int on )
{

    PAGE_SCROLL = on;
}

/**
 * Message Link List - setup for message reader
 */
void msg_readll::initReader ( char *sTEXT, char *sQUOTE, char *sSYS, char *sORIGIN )
{

    reader = TRUE;
    strcpy ( TEXT_COLOR,sTEXT );    // Message Text Color
    strcpy ( QUOTE_COLOR,sQUOTE );  // Message Quote
    strcpy ( SYS_COLOR,sSYS );      // --- BBS Version
    strcpy ( ORIGIN_COLOR,sORIGIN ); //  * Origin Line ...
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
    for ( int t = 0; t < boxsize; t++ )
    {
        sprintf ( capture, "\x1b[%i;%iH\x1b[K", ( Top ) +t, 1 );
        _editbox += capture;
    }

    pipe2ansi ( ( char * ) _editbox.c_str() );
}

/**
 * Message Link List - Remove Control Chars
 */
void msg_readll::stripCRONLY ( char *ostr )
{

    int id1;
    std::string tstr = ostr;

    while ( ( id1=tstr.find ( "\n",0 ) ) != -1 ) tstr.erase ( id1,1 );

    while ( ( id1=tstr.find ( "\r",0 ) ) != -1 ) tstr.erase ( id1,1 );

    while ( ( id1=tstr.find ( "\b",0 ) ) != -1 ) tstr.erase ( id1,1 );

    strcpy ( ostr, ( char * ) tstr.c_str() );
}


/**
 * Message Link List - Copy data from link lists to buffer
 */
void msg_readll::GetBuffer ( std::string &mBuf )
{

    //errlog2((char *)"GetBufferSig()");
    mBuf.erase();

    if ( head == 0 )
    {
        return;
    }

    current_node = head;

    while ( 1 )
    {
        if ( quote == TRUE && current_node->flag == TRUE )
        {
            mBuf += "> " + current_node->data + '\r';
        }
        else if ( quote == FALSE )
        {
            mBuf += current_node->data + '\r';
        }

        if ( current_node->dn_link == 0 ) break;

        current_node = current_node->dn_link;
    }
}

/**
 * Copy message from link lists to <vector> buffer
 */
void msg_readll::GetVector ( vector<list_bar> listb )
{

    // Swap passed with Global
    listing.swap ( listb );
    //SESSION s;

    /* only a test to verify contents passed properly.
    ulong max_areas = listing.size();
    for (ulong i = 0; i != max_areas; i++) {
        s . errlog((char *)"GetVector str1: %s",(char *)listing[i].ansi_1.c_str());
        s . errlog((char *)"GetVector str2: %s",(char *)listing[i].ansi_2.c_str());
        s . errlog((char *)"GetVector str3: %s",(char *)listing[i].ansi_3.c_str());
        s . errlog((char *)"GetVector str4: %s",(char *)listing[i].ansi_4.c_str());
    }

    if (listb.size()>1) s . errlog((char *)"GetVector listb: %lu",listb.size());
    */
}



/**
 * Copy From buffer into Link List
 */
void msg_readll::PutBuffer ( char *mBuff, int Strip )
{

    //errlog2((char *)"PutBuffer msg_readll() START LOOP!");

    std::string MsgText, Line;
    MsgText = mBuff;
    int id1, i = 0;
    char TmpStrip[200]= {0};

    while ( 1 )
    {
        Line.erase();

        // Remove All CR/LF's if Exist for Normal Link Lists
        while ( 1 )
        {
            if ( MsgText.find ( "\r", 0 ) == 0 )
                MsgText.erase ( 0,1 );
            else break;
        }

        while ( 1 )
        {
            id1 = MsgText.find ( "\n", 0 );

            if ( id1 != -1 )
                MsgText.erase ( id1,1 );
            else break;
        }

        if ( Strip )
        {
            // Do a quick Loop Through and Remove all | Pipe Color Codes.
            // Used mainly for Message Quoting
            while ( 1 )
            {
                id1 = MsgText.find ( "|", 0 );

                if ( id1 != -1 && isdigit ( MsgText[id1+1] ) && isdigit ( MsgText[id1+2] ) )
                    MsgText.erase ( id1,3 );
                else break;
            }
        }

        id1 = MsgText.find ( "\r", 0 );
        // Make Sure only to Add New Lines when being used,
        // First Line is already Setup! So Skip it

        //if (id1 != -1 && i != 0) add_to_list("");

        //if (id1 != -1) add_to_list("");
        if ( id1 == -1 ) break;
        else
        {

            //errlog2((char *)"PutBuffer msg_readll() ELSE! LOOP!");

            Line = MsgText.substr ( 0,id1 );
            MsgText.erase ( 0,id1 );

            if ( Line.size() > 0 )
            {
                strcpy ( TmpStrip, ( char * ) Line.c_str() );
                stripCRONLY ( TmpStrip );
                /*
                /// Bug Fix!
                // If first line already exists, use it before adding a new one!!
                if (current_node != 0 && i == 0) {
                    current_node->data = TmpStrip;
                }
                else {
                    add_to_list(TmpStrip);
                }
                */
                add_to_list ( TmpStrip );
                //current_node->data = TmpStrip;
                current_node->lineNum = ( i+1 );
                ++i;
            }
        }
    }

    LineRec *tmp;
    //current_node = last;
    /*
    current_node = head;
    while (1) { // Goto LAst!
        //errlog2(" $$$ box_scrolldn 2.1 LOOP X");
        if (current_node->dn_link == 0) {
            //errlog2(" $$$ box_scrolldn 2.2");
            break;
        }

        current_node = current_node->dn_link;
    } */
    Last_Line();

    for ( ;; )
    {
        if ( current_node->data == "" || current_node->data.size() < 1 )
        {
            if ( current_node == 0 )
            {
                break;
            }

            tmp = current_node;

            if ( current_node->up_link == 0 ) break;

            current_node = current_node->up_link;
            current_node->dn_link = 0;

            //current_node->up_link = tmp->up_link;
            //tmp->up_link->dn_link = current_node;

            delete tmp;
            tmp = 0;

            /*
            if (!move_up()) {
                break;
            } */
        }
        else
            break;
    }

    current_node->dn_link = 0;
}


/**
 * Copy From buffer into Link List (UserSig) Clean
 */
void msg_readll::PutBufferSig ( char *mBuff, int Strip )
{

    int id1;
    std::string Line;
    std::string MsgText;
    int i = 0;

    std::string buffer;

    //errlog2((char *)"PutBufferSig()");

    MsgText = mBuff;

    while ( 1 )
    {
        id1 = Line.find ( "\r", 0 );

        if ( id1 == 0 )
            Line.erase ( id1,1 );
        else break;
    }

    //add_to_list("");
    while ( 1 )
    {
        id1  = MsgText.find ( "\r", 1 );

        //if (id1 != -1 && i != 0) add_to_list("");
        //if (id1 != -1) add_to_list("");
        if ( id1 == -1 ) break;
        else
        {
            Line.erase();
            Line = MsgText.substr ( 0,id1 );
            MsgText.erase ( 0,id1 );

            while ( 1 )
            {
                id1 = Line.find ( "\r", 0 );

                if ( id1 != -1 )
                    Line.erase ( id1,1 );
                else break;
            }


            /// Bug Fix!
            /*
            // If first line already exists, use it before adding a new one!!
            if (current_node != 0 && i == 0) {
                current_node->data = Line;
            }
            else {
                add_to_list(Line);
            } */
            add_to_list ( Line );
            ++i;
        }
    }

    // Clean Bottom of Message, Leave no Blank Lines below sig.
    LineRec *tmp;
    //current_node = last;
    /*
    current_node = head;
    while (1) { // Goto LAst!
        //errlog2(" $$$ box_scrolldn 2.1 LOOP X");
        if (current_node->dn_link == 0) {
            //errlog2(" $$$ box_scrolldn 2.2");
            break;
        }

        current_node = current_node->dn_link;
    } */
    Last_Line();

    for ( ;; )
    {
        if ( current_node->data == "" || current_node->data.size() < 1 )
        {
            if ( current_node == 0 )
            {
                break;
            }

            tmp = current_node;

            if ( current_node->up_link == 0 ) break;

            current_node = current_node->up_link;

            //current_node->up_link = tmp->up_link;
            //tmp->up_link->dn_link = current_node;
            delete tmp;
            tmp = 0;

            //if (!move_up()) {
            //    break;
            //}
        }
        else
            break;
    }

    current_node->dn_link = 0;

}


/**
 * Remove Tag and Origin Lines from Original Message for Editing!
 */
void msg_readll::cleartags ( char *tag, char *origin )
{

    LineRec *tmp;
    current_node = head; //head->dn_link;

    //errlog2((char *)"cleartags()");
    while ( current_node != 0 )
    {
        if ( current_node->data == tag || current_node->data.substr ( 0,3 ) == "---" )
            current_node->data.erase();
        else if ( current_node->data == origin )
            current_node->data.erase();

        //current_node = current_node->dn_link;
        if ( current_node->dn_link == 0 ) break;

        current_node = current_node->dn_link;
    }

    // (Delete and Move Up) remove any blank lines from bottom going up...
    // For Cleaner Message Display with no extra lines.
    /*
    for (;;) {
        if (!move_down()) {
            break;
        }
    }*/
    //current_node = last;  // Starting at the Last, No Down, only Up!
    Last_Line();

    for ( ;; )
    {
        if ( current_node->data == "" || current_node->data.size() < 1 )
        {
            if ( current_node == 0 )
            {
                break;
            }

            tmp = current_node;

            if ( current_node->up_link == 0 ) break;

            current_node = current_node->up_link;

            //current_node->up_link = tmp->up_link;
            //tmp->up_link->dn_link = current_node;
            delete tmp;
            tmp = 0;

            //if (!move_up()) {
            //    break;
            //}
        }
        else
            break;
    }

    current_node->dn_link = 0;
}

/**
 * Clear All (Quoter)
 */
void msg_readll::clear_all()
{

    //errlog2((char *)"clear all()");
    LineRec *print;

    if ( head == 0 )
    {
        return;
    }

    //errlog2((char *)"clear all 2()");

    print = head;

    while ( print != 0 )
    {
        //errlog2((char *)"clear all 3()");
        print->flag = FALSE;

        if ( print == last ) break;

        print = print->dn_link;
    }

    //errlog2((char *)"clear all 4()");

}

/**
 * Select All Lines (Quoter)
 */
void msg_readll::select_all()
{

    LineRec *print;

    //errlog2((char *)"select all()");
    if ( head == 0 )
    {
        return;
    }

    print = head;

    while ( print != 0 )
    {
        print->flag = TRUE;

        //print = print->dn_link;
        if ( print == last ) break;

        print = print->dn_link;
    }

}

// Testing Only
/*
void msg_readll::display_list(void)
{

    unsigned char c;
    LineRec *print;
    long int i = 0;

    if(head == 0)
    {
        // Empty
        return;
    }

    print = head;
    while(print != 0)
    {
        printf("\n%s",print->data.c_str());
        print = print->dn_link;
        ++i;
    }
    c = getkey(true);
}
*/


/**
 * flag Lines (Quoter) which line are to be quoted.
 */
void msg_readll::flag_line ( int iNum )
{

    LineRec *print;
    long int i = 1;

    //errlog2((char *)"flag_line()");
    if ( head == 0 )
    {
        return;
    }

    print = head;

    while ( print != 0 )
    {
        if ( iNum == i )
        {
            if ( print->flag == TRUE ) print->flag = FALSE;
            else print->flag = TRUE;
        }

        if ( print == last ) break;

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

    //int Total = line_total();
    //errlog2((char *)"delete line up()");
    if ( current_node == 0 )
    {
        return;
    }

    if ( current_node->up_link != 0 ) // At least 1 Line Above Current!
    {
        tmp = current_node;
        current_node = current_node->dn_link;
        current_node->up_link = tmp->up_link;
        tmp->up_link->dn_link = current_node;
        delete tmp;
        tmp = 0;
        // No Lines Below this, move to top line
        //move_up();
        current_node = current_node->up_link;
        //current_node->dn_link = 0;
        return;
    }

    // This is the First Line, just Clear It.
    current_node->data.erase();
    Row = 1;
    Num = 1;
    Tot = 1;
}


/**
 * Add Line to Link List
 */
void msg_readll::add_to_list ( std::string add_data )
{

    LineRec *add = new LineRec;

    if ( !add )
    {
        //errlog2((char *)"unable to allocate memory readll add_to_list()");
        exit ( 1 );
    }

    //errlog2((char *)"add_to_list readll Done 1");

    //errlog2((char *)add_data.c_str());
    add->data = add_data;
    add->flag = FALSE;
    add->lineNum = 0;

    //errlog2((char *)"add_to_list readll Done 2");
    if ( current_node == 0 ) // New Frist Add!
    {
        //errlog2((char *)"add_to_list readll Done 2.1");
        add->up_link = 0;
        add->dn_link = 0;
        head = add;
        current_node = add;
        last = add;
        return;
    }

    /// Always add after current node, then link back up.
    //errlog2((char *)"add_to_list readll Done 2.2");
    //add->dn_link = 0;
    //errlog2((char *)"add_to_list readll Done 2.3");
    add->up_link = current_node;



    //errlog2((char *)"add_to_list readll Done 3");
    // In the Middle of list! hmmm
    if ( current_node->dn_link != 0 )
    {

        //errlog2((char *)"add_to_list readll Done 4");
        add->dn_link = current_node->dn_link;
        //add->up_link = current_node;

        //errlog2((char *)"add_to_list readll Done 5");
        // current_node->dn_link->up_link = add;
        current_node->dn_link = add;

        //errlog2((char *)"add_to_list readll Done 8");

        /// SAME
        current_node->dn_link = add;

        //errlog2((char *)"add_to_list readll Done 9");
        current_node = add;

        //errlog2((char *)"add_to_list readll Done 10");
        //last = add;

    }
    // Current is the Last one in Lie
    else
    {
        //errlog2((char *)"add_to_list readll Done 6");
        add->up_link = current_node;
        add->dn_link = 0;

        //errlog2((char *)"add_to_list readll Done 7");

        //errlog2((char *)"add_to_list readll Done 8");

        /// SAME
        current_node->dn_link = add;

        //errlog2((char *)"add_to_list readll Done 9");
        current_node = add;

        //errlog2((char *)"add_to_list readll Done 10");
        last = add;
    }
}


/**
 * Move up 1 from current to prev in list
 */
int msg_readll::move_up()
{

    // Make Sure Link List Stays at current if there is none!
    if ( current_node == 0 ) return FALSE;

    if ( current_node->up_link == 0 )
    {
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

    // Make Sure Link List Stays at current if there is none!
    if ( current_node == 0 ) return FALSE;

    if ( current_node->dn_link == 0 )
    {
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

    if ( head == 0 )
    {
        return 0;
    }

    // Make Sure Link List Stays at current if there is none!
    lineTmp = current_node;

    // Count how many Lines from Current to Bottom
    while ( lineTmp->dn_link != 0 )
    {
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

    if ( head == 0 )
    {
        return 0;
    }

    // Make Sure Link List Stays at current if there is none!
    lineTmp = head;

    // Count how many Lines from Current to Bottom
    while ( lineTmp->dn_link != 0 )
    {
        if ( lineTmp->dn_link == 0 ) break;

        ++count;
        lineTmp = lineTmp->dn_link;

    }

    Tot = count;
    //errlog2("line_total msgread: %i",Tot);
    return count;

}


/**
 * Move to last line in list
 */
void msg_readll::Last_Line()
{

    //LineRec *lineTmp;

    //is Empty?
    if ( head == 0 )
    {
        return;
    }

    /*lineTmp = head;
    lineTmp = lineTmp->up_link;
    current_node = lineTmp;*/
    //current_node = last;
    current_node = head;

    while ( 1 ) // Goto LAst!
    {
        //errlog2(" $$$ box_scrolldn 2.1 LOOP X");
        if ( current_node->dn_link == 0 )
        {
            //errlog2(" $$$ box_scrolldn 2.2");
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

    //errlog2((char *)"dispose readll");

    dispose_list();
    //errlog2((char *)"dispose readll done!");
    return;

    /*
    int Total = line_total();

    if (current_node == 0) return;
    current_node = last;

    // Delete All the Lines
    for ( ; Total != 0; Total--) {
        Num = Total;
        delete_line_up();
    }
    //errlog2((char *)"dispose readll done!");
    */
}


/**
 * Wipe Out List Completely & Free Memory.
 */
void msg_readll::dispose_list()
{

    //errlog2((char *)"dispose_list readll");

    LineRec *tmp;
    current_node = head;

    while ( current_node != 0 )
    {
        tmp = current_node;

        if ( current_node->dn_link == 0 ) break;

        current_node = current_node->dn_link;
        delete tmp;
        tmp = 0;


    }

    //delete head;
    //delete last;
    current_node = 0;
    head = 0;
    last = 0;
}

/**
 * Message Reader - Set Text and Quote Colors
 * With Origina and Tags (Full SCreen Reader)
 */
char *msg_readll::parsereadcolors ( char *cap,
                                    int   row,
                                    int   col,
                                    std::string data )
{


    std::string tmp;
    tmp = data;
    std::string::size_type id1 = 0, id2 = 0;

    // System Tag
    id1 = tmp.find ( "---",0 );

    if ( id1 == 0 )
    {
        sprintf ( cap, "\x1b[%i;%iH%s%s",
                  row,col,SYS_COLOR,tmp.c_str() );
        return cap;
    }

    // Origin Line
    id1 = tmp.find ( " * Origin:",0 );

    if ( id1 == 0 )
    {
        sprintf ( cap, "\x1b[%i;%iH%s%s",
                  row,col,ORIGIN_COLOR,tmp.c_str() );
        return cap;
    }


    // Clean Up Quoted Text, Remove Pipe or Ansi Color Codes
    // Remove any Pipe Color Codes from Already Quoted Text for Reader.
    while ( 1 )  // Remove any PIPE Color Codes in Quoting.
    {
        id1 = tmp.find ( "|", 0 );

        if ( id1 != std::string::npos && isdigit ( tmp[id1+1] ) && isdigit ( tmp[id1+2] ) )
            tmp.erase ( id1,3 );
        else break;
    }

    while ( 1 )  // Remove any ANSI Color Codes
    {
        id1 = tmp.find ( "\x1b[", 0 );

        if ( id1 != std::string::npos )
        {
            id2 = tmp.find ( "m",id1+3 );
            tmp.erase ( id1, ( id2 - id1 ) +1 );
        }
        else break;
    }


    // Quote Color
    //if (tmp[0] == '>' || tmp[1] == '>' || tmp[2] == '>')
    id1 = tmp.find ( ">", 0 );

    if ( id1 != std::string::npos && id1 <= 3 )
    {


        sprintf ( cap, "\x1b[%i;%iH%s%s",
                  row,col,QUOTE_COLOR,tmp.c_str() );
        return cap;
    }
    else
    {
        tmp.erase();
        tmp = data;
    }

    // Normal Text Color
    sprintf ( cap, "\x1b[%i;%iH%s%s",
              row,col,TEXT_COLOR,tmp.c_str() );

    return cap;
}

/**
 * Message Reader - Set Text and Quote Colors
 * With Origina and Tags (Scrolling Reader)
 */
char *msg_readll::parseReadColorScroller ( char *cap, std::string data )
{

    std::string tmp;
    tmp = data;
    std::string::size_type id1 = 0, id2 = 0;

    // System Tag
    id1 = tmp.find ( "---",0 );

    if ( id1 == 0 )
    {
        sprintf ( cap, "%s%s",SYS_COLOR,tmp.c_str() );
        return cap;
    }

    // Origin Line
    id1 = tmp.find ( " * Origin:",0 );

    if ( id1 == 0 )
    {
        sprintf ( cap, "%s%s",ORIGIN_COLOR,tmp.c_str() );
        return cap;
    }

    // Clean Up Quoted Text, Remove Pipe or Ansi Color Codes
    // Remove any Pipe Color Codes from Already Quoted Text for Reader.
    while ( 1 )  // Remove any PIPE Color Codes in Quoting.
    {
        id1 = tmp.find ( "|", 0 );

        if ( id1 != std::string::npos && isdigit ( tmp[id1+1] ) && isdigit ( tmp[id1+2] ) )
            tmp.erase ( id1,3 );
        else break;
    }

    while ( 1 )  // Remove any ANSI Color Codes
    {
        id1 = tmp.find ( "\x1b[", 0 );

        if ( id1 != std::string::npos )
        {
            id2 = tmp.find ( "m",id1+3 );
            tmp.erase ( id1, ( id2 - id1 ) +1 );
        }
        else break;
    }


    // Quote Color
    //if (tmp[0] == '>' || tmp[1] == '>' || tmp[2] == '>')
    id1 = tmp.find ( ">", 0 );

    if ( id1 != std::string::npos && id1 <= 3 )
    {
        sprintf ( cap, "%s%s",QUOTE_COLOR,tmp.c_str() );
        return cap;
    }
    else
    {
        tmp.erase();
        tmp = data;
    }

    // Normal Text Color
    sprintf ( cap, "%s%s",TEXT_COLOR,tmp.c_str() );

    return cap;
}

// Still used for message reader scroller i believe!
// Reader Uses old version, New Full Screen
// Interfaces use box_start_vector()

/**
 * Inital Fucntion for Drawing List on the Screen
 * And Scrolling text per page.
 */
void msg_readll::box_start ( unsigned long CURRENT )
{

    std::string _editbox = LoColor, lData;
    char capture[200]= {0};
    char lineNum[5];
    Page   = 1;
    //pgRows = 0;

    // Calcuate Box Size and Total Pages
    int boxsize = Bot - Top; // Fist Get Box Size
    Tot = line_total();
    TotPages = Tot / boxsize;

    if ( Tot % boxsize > 0 )
    {
        ++TotPages;
    }

    if ( Tot <= boxsize ) TotPages = 1;

    // Toggle this for Area List Only, fucks up Message Reader!!

    // Test What the Current Area is, if it's started on the Next Page
    // Refresh on that page instead of starting fresh.
    if ( arealist )
    {
        Page = CURRENT / boxsize;

        if ( CURRENT % boxsize > 0 )
            ++Page;

        if ( Page > 1 )
        {
            //box_refresh(CURRENT_MAREA);
            box_refresh ( CURRENT );
            return;
        }
    }

    //display_list();

    //Now clear the box First
    for ( int t = 0; t < boxsize; t++ )
    {
        sprintf ( capture, "\x1b[%i;%iH\x1b[K", ( Top ) +t, 1 );
        _editbox += capture;
    }


    // Now Grab as many lines as will fit in the box
    current_node = head; //->dn_link;

    for ( int i = 1; i < boxsize+1; i++ )
    {
        if ( quote )
        {
            lData = current_node->data;
            /*
            lData = current_node->data.substr(0,74);
            if (lData.size() < 74)
            {
                strcpy(capture,lData.c_str());
                rspacing(capture,74);
                lData = capture;
            }*/

            // For Messge Quoteer only!!
            sprintf ( lineNum,"%i",current_node->lineNum );
            lspacing ( lineNum,3 );

            if ( current_node->flag == TRUE )
                sprintf ( capture, "\x1b[%i;%iH|15%s%s. %s|16", Top+i-1, 1, HiColor, lineNum, ( char * ) lData.c_str() );
            else
                sprintf ( capture, "\x1b[%i;%iH|15%s. %s%s|16", Top+i-1, 1, lineNum, LoColor, ( char * ) lData.c_str() );

            // End Of Message Quoter
        }
        // Message Reader
        else if ( reader )
        {
            strcpy ( capture,parsereadcolors ( capture,Top+i-1,1, ( char * ) current_node->data.c_str() ) );
        }
        else // Else Normal Link Lists like Aeara etc...
            sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, ( char * ) current_node->data.c_str() );

        _editbox += capture;

        if ( current_node->dn_link == 0 )
        {
            break;
        }

        current_node = current_node->dn_link;
    }

    sprintf ( capture, "\x1b[%i;%iH", Row+Top-1, 1 );
    _editbox += capture;
    pipe2ansi ( ( char * ) _editbox.c_str() );

}


// Line Reader - old school scrolling.
// Still in Use for Scroll Themes.
/**
 * Inital Fucntion for Drawing List on the Screen
 * And Scrolling text per page.
 */
void msg_readll::box_scroll_reader()
{

    //errlog2((char *)"box scroll reader");
    if ( head == 0 ) return;

    std::string _editbox = LoColor, lData;
    char capture[1024]= {0};

    Page   = 1;
    pgRows = 0;

    int iCoordX = Top,
        //iCoordY,
        iPause;

    int boxsize = TERM_HEIGHT-1;

    // Now Grab as many lines as will fit in the screen up to 24 Lines!
    current_node = head; //->dn_link;

    for ( int i = iCoordX; i < boxsize+1; i++ )
    {
        //current_node->data;
        strcpy ( capture,parseReadColorScroller ( capture,current_node->data.c_str() ) );
        _editbox += capture;
        _editbox += "|CR";

        if ( current_node->dn_link == 0 )
        {
            Page = 0;
            break;
        }

        current_node = current_node->dn_link;

    }

    if ( Page == 0 ) // || Page == TotPages) {
    {
        Page = 1;
        _editbox += "|CR";
        pipe2ansi ( ( char * ) _editbox.c_str() );
        return;
    }

    pipe2ansi ( ( char * ) _editbox.c_str() );

REDRAW:
    //errlog2((char *)"box scroll reader 2");
    memset ( &capture,0,sizeof ( capture ) );
    _editbox.erase();

    iPause = 0;

    do
    {
        iPause = startpause ( ( char * ) "mreadpause.ini" );

        if ( toupper ( iPause ) == 'N' || ( toupper ( iPause ) == 'Q' ) ) return;
    }
    while ( iPause == 0 );

    ++Page;

    // Now Grab as many lines as will fit in the screen up to 24 Lines!
    for ( int i = 1; i < boxsize+1; i++ )
    {
        //current_node->data;
        strcpy ( capture,parseReadColorScroller ( capture,current_node->data.c_str() ) );
        _editbox += capture;
        _editbox += "|CR";

        if ( current_node->dn_link == 0 )
        {
            Page = 0;
            break;
        }

        current_node = current_node->dn_link;
    }

    //_editbox += "|CR";
    pipe2ansi ( ( char * ) _editbox.c_str() );

    if ( Page == TotPages )
    {
        pipe2ansi ( ( char * ) "|CR" );
        return;
    }

    //errlog2((char *)"box scroll reader 5");
    if ( Page == 0 )
    {
        Page = TotPages;
        pipe2ansi ( ( char * ) "|CR" );
        //errlog2((char *)"box scroll reader 6");
        return;
    }

    //errlog2((char *)"box scroll reader 7");
    goto REDRAW;

}

/*
void msg_readll::box_pgdn_scroll() {

    std::string _editbox = LoColor, lData;
    char capture[200]={0};
//    char lineNum[5];
    int iPause;
    int boxsize = 23;

REDRAW2:
    memset(&capture,0,sizeof(capture));
    _editbox.erase();

    iPause = 0;
    //pipe2ansi((char *)"ipause start");
    do {

        iPause = startpause((char *)"mreadpause.ini");
        if (toupper(iPause) == 'N' || (toupper(iPause) == 'Q')) return;
    }
    while(iPause == 0);

    //pipe2ansi((char *)"ipause done1");
    Page = 1;

    // Now Grab as many lines as will fit in the screen up to 24 Lines!
    // current_node =  current_node->dn_link;
    for (int i = 0; i < boxsize; i++) {
        //current_node->data;
        strcpy(capture,parseReadColorScroller(capture,current_node->data.c_str()));
        _editbox += capture;
        _editbox += "|CR";
        if (current_node->dn_link == 0) { Page = 0; break; }
        current_node = current_node->dn_link;
    }

    pipe2ansi((char *)_editbox.c_str());

    if (Page == 0)
        return;
    else
        goto REDRAW2;
}
*/

/**
 * Scroll Last Page Up from Bottom.
 */
void msg_readll::box_scrolldn()
{

    std::string _editbox = LoColor, lData;
    char capture[200]= {0};
    char lineNum[5];

    // Make sure we can go down another page..
    int boxsize = Bot - Top;   // Get boxsize

    //errlog2(" $$$ box_scrolldn 1");

    if ( CLEAR_BOX == TRUE )
    {
        //Now clear the box First
        for ( int t = 1; t < boxsize+1; t++ )
        {
            sprintf ( capture, "\x1b[%i;%iH\x1b[K", ( Top ) +t-1, 1 );
            _editbox += capture;
        }
    }

    //errlog2(" $$$ box_scrolldn 2");

    // Jump to last line
    //Last_Line();

    //if (last == 0)  {
    //    errlog("ERR box_scrolldn: Last was not allocated properly!");
    //    return;
    //}
    /*
    //current_node = last;

    //current_node = head;  A;ready assign head!
    while (1) { // Goto LAst!
        //errlog2(" $$$ box_scrolldn 2.1 LOOP X");
        if (current_node->dn_link == 0) {
            //errlog2(" $$$ box_scrolldn 2.2");
            break;
        }

        current_node = current_node->dn_link;
    } */
    Last_Line();

    //errlog2(" $$$ box_scrolldn 2.3");

    // Move up As Many lines that will fit in the box
    for ( int i = 1; i < boxsize; i++ )
    {
        //errlog2((char *)current_node->data.c_str());
        //errlog2(" $$$ box_scrolldn 2.4 LOOP X");
        if ( current_node->up_link == 0 )
        {
            //errlog2(" $$$ box_scrolldn 2.5");
            break;
        }

        current_node = current_node->up_link;
    }

    // Now Grab as many lines as will fit in the box
    //errlog2(" $$$ box_scrolldn 3");
    for ( int i = 1; i < boxsize+1; i++ )
    {
        if ( quote )
        {
            lData = current_node->data;
            /*
            //errlog2(" $$$ box_scrolldn 3.1");
            lData = current_node->data.substr(0,74);
            if (lData.size() < 74)
            {
                strcpy(capture,lData.c_str());
                rspacing(capture,74);
                lData = capture;
            }*/

            sprintf ( lineNum,"%i",current_node->lineNum );
            lspacing ( lineNum,3 );

            if ( current_node->flag == TRUE )
                sprintf ( capture, "\x1b[%i;%iH|15%s%s. %s|16", Top+i-1, 1, HiColor, lineNum, lData.c_str() );
            else
                sprintf ( capture, "\x1b[%i;%iH|15%s. %s%s", Top+i-1, 1, lineNum, LoColor, lData.c_str() );
        }
        else if ( reader )
        {
            //errlog2(" $$$ box_scrolldn 3.2");
            strcpy ( capture,parsereadcolors ( capture,Top+i-1,1, current_node->data.c_str() ) );
        }
        else
        {
            //errlog2(" $$$ box_scrolldn 3.3");
            pgRows = i; // Set # of Rows on Last Page.
            sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str() );
        }

        //errlog2(" $$$ box_scrolldn 3.4");
        _editbox += capture;

        if ( current_node->dn_link == 0 ) break;

        current_node = current_node->dn_link;
    }

    //errlog2(" $$$ box_scrolldn 3.5");
    // Display Message
    Page = TotPages;
    pipe2ansi ( ( char * ) _editbox.c_str() );

}


/**
 * Page Down in Link List.
 */
void msg_readll::box_pgdn()
{

    std::string _editbox = LoColor, lData;
    char capture[200]= {0};
    char lineNum[5]= {0};

    // Make sure we can go down another page..
    int boxsize = Bot - Top;   // Get boxsize

    Tot = line_total();
    TotPages = Tot / boxsize;

    if ( Tot % boxsize > 0 )
    {
        ++TotPages;
    }

    if ( Tot <= boxsize ) TotPages = 1;

    // Check Total Number of Pages
    // If On Last page Return
    if ( Page == TotPages )
    {
        --Page;    //return;
    }

    ++Page;

    current_node = head; //->dn_link;

    if ( Page == TotPages && PAGE_SCROLL == TRUE )
    {
        box_scrolldn();
        return;
    }/*

    else if ((Tot - boxsize) <= CURRENT_MAREA) {
        errlog2(" $$$ box_pgdn 1.2");
        box_scrolldn();
        return;

    }  */
    else
    {
        // Number of Rows if This was the Last Page Only.
        pgRows = 0;
    }

    if ( CLEAR_BOX == TRUE )
    {
        //Now clear the box First
        for ( int t = 0; t < boxsize; t++ )
        {
            sprintf ( capture, "\x1b[%i;%iH\x1b[K", ( Top ) +t, 1 );
            _editbox += capture;
        }
    }

    //errlog2(" $$$ box_pgdn 3");
    // Set Current Node to First In List

    //* NOTE TESTING the +1 on i < boxsize.

    //errlog2(" $$$ box_pgdn 4");
    // Now Move Down How Many Pages
    for ( int i = 1; i < ( boxsize* ( Page-1 ) ) +1; i++ )
    {
        //for (int i = 1; i < (boxsize*(Page-1)); i++) {
        if ( current_node->dn_link == 0 ) break;

        current_node = current_node->dn_link;

        //if (!move_down()) break;
    }

    //errlog2(" $$$ box_pgdn 5");
    // Now Grab as many lines as will fit in the box
    for ( int i = 1; i < boxsize+1; i++ )
    {
        if ( quote )
        {
            lData = current_node->data;
            //errlog2(" $$$ box_pgdn 5.1");
            /*
            lData = current_node->data.substr(0,74);
            if (lData.size() < 74)
            {
                strcpy(capture,lData.c_str());
                rspacing(capture,74);
                lData = capture;
            }
            */
            sprintf ( lineNum,"%i",current_node->lineNum );
            lspacing ( lineNum,3 );

            if ( current_node->flag == TRUE )
                sprintf ( capture, "\x1b[%i;%iH|15%s%s. %s|16", Top+i-1, 1, HiColor, lineNum, lData.c_str() );
            else
                sprintf ( capture, "\x1b[%i;%iH|15%s. %s%s", Top+i-1, 1, lineNum, LoColor, lData.c_str() );
        }
        else if ( reader )
        {
            //errlog2(" $$$ box_pgdn 5.2");
            strcpy ( capture,parsereadcolors ( capture,Top+i-1,1, current_node->data.c_str() ) );
        }
        else
        {
            //errlog2(" $$$ box_pgdn 5.3");
            sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str() );
        }

        //errlog2(" $$$ box_pgdn 5.4");
        _editbox += capture;

        if ( current_node->dn_link == 0 ) break;

        current_node = current_node->dn_link;
    }

    //errlog2(" $$$ box_pgdn 5.5");
    // Display Message
    pipe2ansi ( ( char * ) _editbox.c_str() );
}

/**
 * Page Up in Link List.
 */
void msg_readll::box_pgup()
{

    // Faster display to throw in 1 string and send all out at 1 time
    std::string _editbox = LoColor,    lData;
    char capture[200]= {0};
    char lineNum[5];

    // Number of Rows if This was the Last Page Only.
    pgRows = 0;

    // Make sure we can go down another page..
    int boxsize = Bot - Top;   // Get boxsize

    Tot = line_total();
    TotPages = Tot / boxsize;

    if ( Tot % boxsize > 0 )
    {
        ++TotPages;
    }

    if ( Tot <= boxsize ) TotPages = 1;

    // Check Total Number of Pages
    // If On Last page Return
    if ( Page == 1 ) return;

    --Page;

    //Now clear the box First
    for ( int t = 0; t < boxsize; t++ )
    {
        sprintf ( capture, "\x1b[%i;%iH\x1b[K", ( Top ) +t, 1 );
        _editbox += capture;
    }

    // Set Current Node to First In List
    current_node = head; //->dn_link;

    if ( Page > 1 )
    {
        // Now Move Down How Many Pages
        for ( int i = 1; i < ( boxsize* ( Page-1 ) +1 ); i++ )
        {
            if ( current_node->dn_link == 0 ) break;

            current_node = current_node->dn_link;
        }
    }

    // Now Grab as many lines as will fit in the box
    for ( int i = 1; i < boxsize+1; i++ )
    {
        if ( quote )
        {
            lData = current_node->data;
            /*
            lData = current_node->data.substr(0,74);
            if (lData.size() < 74)
            {
                strcpy(capture,lData.c_str());
                rspacing(capture,74);
                lData = capture;
            }*/

            sprintf ( lineNum,"%i",current_node->lineNum );
            lspacing ( lineNum,3 );

            if ( current_node->flag == TRUE )
                sprintf ( capture, "\x1b[%i;%iH|15%s%s. %s|16", Top+i-1, 1, HiColor, lineNum, lData.c_str() );
            else
                sprintf ( capture, "\x1b[%i;%iH|15%s. %s%s", Top+i-1, 1, lineNum, LoColor, lData.c_str() );
        }
        else if ( reader )
        {
            strcpy ( capture,parsereadcolors ( capture,Top+i-1,1, current_node->data.c_str() ) );
        }
        else
            sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str() );

        _editbox += capture;

        if ( current_node->dn_link == 0 ) break;

        current_node = current_node->dn_link;
    }

    // Display Message
    pipe2ansi ( ( char * ) _editbox.c_str() );
}


// Used for Message Quoter / And Lists.
/**
 * Refresh the Screen
 */
void msg_readll::box_refresh ( unsigned long CURRENT )
{

    if ( Page == 0 ) Page = 1;

    if ( Page == 1 )
        box_start ( CURRENT );
    else
    {
        --Page;
        box_pgdn();
    }
}

// used for faster dispaly without clearing first.
// Dont think this is used anymore!!
/**
 * Refresh the Screen
 */
void msg_readll::box_overdraw ( unsigned long CURRENT )
{

    CLEAR_BOX = FALSE;

    if ( Page == 0 ) Page = 1;

    if ( Page == 1 )
        box_start ( CURRENT );
    else
    {
        --Page;
        box_pgdn();
    }

    CLEAR_BOX = TRUE;
}

// New Full Screen Vector Interface
// Not Used, anymore
/**
 * Not used anymore, box_start_vector Does it all now!
 */
void msg_readll::box_pgup_vector()
{

//    errlog((char *)" box_pgup_vector ");

    // Faster display to throw in 1 string and send all out at 1 time
    std::string _editbox = LoColor, lData;
    char capture[200]= {0};

    // Number of Rows if This was the Last Page Only.
    pgRows = 0;

    // Make sure we can go down another page..
    int boxsize = Bot - Top;   // Get boxsize

    Tot = line_total();
    TotPages = Tot / boxsize;

    if ( Tot % boxsize > 0 )
    {
        ++TotPages;
    }

    if ( Tot <= boxsize ) TotPages = 1;

    // Check Total Number of Pages
    // If On Last page Return

//    if (Page == 1) return;
//    --Page;

    //Now clear the box First
    for ( int t = 0; t < boxsize; t++ )
    {
        sprintf ( capture, "\x1b[%i;%iH\x1b[K", ( Top ) +t, 1 );
        _editbox += capture;
    }

    // Set to Start of Page.
    int current_result = boxsize* ( Page-1 );

    // Now Grab as many lines as will fit in the box
    for ( int i = 1; i < boxsize+1; i++ )
    {

//        errlog((char *)"%i - current_result %i, CURRENT_MAREA %i, == lsitingsize() %i", current_result, CURRENT_MAREA,i,listing.size() );

        /// Paging up
        if ( listing[current_result].isnew )
        {
            if ( current_result == CURRENT_MAREA )
            {
//                errlog((char *)" current_result == CURRENT_MAREA %i",Top+i-1);
                current_selection = Top+i-1; // Get current place in box to display.
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, listing[current_result].ansi_4.c_str() );
            }
            else if ( current_result < ( signed ) listing.size() )
            {
//                errlog((char *)" current_result %i ",Top+i-1);
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, listing[current_result].ansi_3.c_str() );
            }
            else
                break;
        }
        else
        {
            if ( current_result == CURRENT_MAREA )
            {
//                errlog((char *)" current_result == CURRENT_MAREA %i",Top+i-1);
                current_selection = Top+i-1; // Get current place in box to display.
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, listing[current_result].ansi_2.c_str() );
            }
            else if ( current_result < ( signed ) listing.size() )
            {
//                errlog((char *)" current_result %i ",Top+i-1);
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, listing[current_result].ansi_1.c_str() );
            }
            else
                break;
        }

        ++current_result;
        _editbox += capture;

        if ( CURRENT_MAREA+i > ( signed ) listing.size() ) break;
    }

    // Display Message
    pipe2ansi ( ( char * ) _editbox.c_str() );
}


// Not implimented yet - gets messy!
void msg_readll::box_scrolldn_vector()
{

    std::string _editbox = LoColor, lData;
    char capture[200]= {0};

    // Make sure we can go down another page..
    int boxsize = Bot - Top;   // Get boxsize

    //errlog2(" $$$ box_scrolldn 1");

    if ( CLEAR_BOX == TRUE )
    {
        //Now clear the box First
        for ( int t = 1; t < boxsize+1; t++ )
        {
            sprintf ( capture, "\x1b[%i;%iH\x1b[K", ( Top ) +t-1, 1 );
            _editbox += capture;
        }
    }

    //errlog2(" $$$ box_scrolldn 2");

    // Jump to last line
    //Last_Line();

    //if (last == 0)  {
    //    errlog("ERR box_scrolldn: Last was not allocated properly!");
    //    return;
    //}
    /*
    //current_node = last;

    //current_node = head;  A;ready assign head!
    while (1) { // Goto LAst!
        //errlog2(" $$$ box_scrolldn 2.1 LOOP X");
        if (current_node->dn_link == 0) {
            //errlog2(" $$$ box_scrolldn 2.2");
            break;
        }

        current_node = current_node->dn_link;
    } */
//    Last_Line();

    //errlog2(" $$$ box_scrolldn 2.3");

    // Move up As Many lines that will fit in the box
    //for (int i = 1; i < boxsize; i++) {
    //errlog2((char *)current_node->data.c_str());
    //errlog2(" $$$ box_scrolldn 2.4 LOOP X");
    //if (current_node->up_link == 0) {
    //errlog2(" $$$ box_scrolldn 2.5");
    //    break;
    //}

    //current_node = current_node->up_link;

    //}

    // Now Grab as many lines as will fit in the box
    //errlog2(" $$$ box_scrolldn 3");
    for ( int i = 1; i < boxsize+1; i++ )
    {


        //errlog2(" $$$ box_scrolldn 3.3");
        pgRows = i; // Set # of Rows on Last Page.

        if ( CURRENT_MAREA )
            sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str() );
        else
            sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str() );



        //errlog2(" $$$ box_scrolldn 3.4");
        _editbox += capture;

        if ( current_node->dn_link == 0 ) break;

        current_node = current_node->dn_link;
    }

    //errlog2(" $$$ box_scrolldn 3.5");
    // Display Message
    Page = TotPages;
    pipe2ansi ( ( char * ) _editbox.c_str() );

}

/*
// Used for Message Quoter / And Lists.
void msg_readll::box_refresh_vector(unsigned long CURRENT) {

    errlog((char *)" box_refresh_vector ");
    if (Page == 0) Page = 1;
    if (Page == 1)
        box_start_vector(CurrentPage,CURRENT_MAREA);
    else {
        --Page;
        box_pgdn_vector();
    }
}
*/

// Not Used!
void msg_readll::box_pgdn_vector()
{

//    errlog((char *)" box_pgdn_vector ");

    std::string _editbox = LoColor, lData;
    char capture[200]= {0};

    // Make sure we can go down another page..
    int boxsize = Bot - Top;   // Get boxsize

    Tot = listing.size();
    TotPages = Tot / boxsize;

    if ( Tot % boxsize > 0 )
    {
        ++TotPages;
    }

    if ( Tot <= boxsize ) TotPages = 1;

    // Check Total Number of Pages
    // If On Last page Return
    if ( Page == TotPages )
    {
        --Page;    //return;
    }

    ++Page;


    if ( Page == TotPages && PAGE_SCROLL == TRUE )
    {
        // box_scrolldn();
        // return;
    }
    else   // Number of Rows if This was the Last Page Only.
    {
        pgRows = 0;
    }

    if ( CLEAR_BOX == TRUE )
    {
        //Now clear the box First
        for ( int t = 0; t < boxsize; t++ )
        {
            sprintf ( capture, "\x1b[%i;%iH\x1b[K", ( Top ) +t, 1 );
            _editbox += capture;
        }
    }

    // Move Down to Current Record for Starting Position.
    int current_result = boxsize* ( Page-1 );

    // Now Grab as many lines as will fit in the box
    for ( int i = 1; i < boxsize+1; i++ )
    {

//        errlog((char *)"%i - current_result %i, CURRENT_MAREA %i, == lsitingsize() %i", current_result, CURRENT_MAREA,i,listing.size() );
        if ( CURRENT_MAREA+i > ( signed ) listing.size() ) break;

        /// Paging Down
        if ( listing[current_result].isnew )
        {
            if ( current_result == CURRENT_MAREA )
            {
                current_selection = Top+i-1; // Get current place in box to display.
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, listing[current_result].ansi_4.c_str() );

            }
            else if ( current_result < ( signed ) listing.size() )
            {
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, listing[current_result].ansi_3.c_str() );
            }
            else
                break;
        }
        else
        {
            if ( current_result == CURRENT_MAREA )
            {
                current_selection = Top+i-1; // Get current place in box to display.
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, listing[current_result].ansi_2.c_str() );

            }
            else if ( current_result < ( signed ) listing.size() )
            {
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, listing[current_result].ansi_1.c_str() );
            }
            else
                break;
        }

        ++current_result;
        _editbox += capture;

        // if (CURRENT_MAREA+(i-2) == listing.size()) break;
        //if (current_node->dn_link == 0) break;
        //current_node = current_node->dn_link;
    }

    pipe2ansi ( ( char * ) _editbox.c_str() );
}


// Merging all 3 Functions into a single one that takes care of listings.
// No longer need Row up or Down, universal accross all Lists.
// Previous functions are only kept for references!

/**
 * New Faster Full Screen Interface for Lightbars and Lists
 * This one fucntions does all start and  pageup/down
 * And Handles jumping to different pages.
 */
void msg_readll::box_start_vector ( unsigned long page, unsigned long list )
{
    std::string _editbox = LoColor, lData;
    char capture[200]= {0};

    Page = page;

    // Calcuate Box Size and Total Pages
    int boxsize = Bot - Top; // Fist Get Box Size
    Tot = listing.size();
    TotPages = Tot / boxsize;

    if ( Tot % boxsize > 0 )
    {
        ++TotPages;
    }

    if ( Tot <= boxsize )
        TotPages = 1;


    // This doesn't work in full screen message read?!?!?!
    // Testing, only clear the box, if total pages > 1,
    // Otherwise no need to clear since it's fresh draw.
    if ( TotPages > 1 )
    {
        //Now clear the box First
        for ( int t = 0; t < boxsize; t++ )
        {
            sprintf ( capture, "\x1b[%i;%iH\x1b[K", ( Top ) +t, 1 );
            _editbox += capture;
        }
    }

    // Now Grab as many lines as will fit in the box
    for ( int i = 1; i < boxsize+1; i++ )
    {
        if ( ( ( boxsize*Page ) +i )-1 >= ( signed ) listing.size() ) break;

//        errlog((char *)"ulist_start_vector list+1, %i < (boxsize*CurrentPage)+i, %i Top+i-1 %i",list+1,(boxsize*Page)+i,Top+i-1);

        // If Area has new message rotate output to new lightbars.
        if ( listing[ ( ( boxsize*Page ) +i )-1].isnew )
        {
//        if (listing[current_result].isnew) {
            //   if (current_result == list) {
            if ( ( signed ) list+1 == ( boxsize*Page ) +i )
            {
//                errlog((char *)"!!! isNEW = 'Y' 4");
                // Current Area
                current_selection = Top+i-1; // Get current place in box to display.
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, ( char * ) listing[ ( ( boxsize*Page ) +i )-1].ansi_4.c_str() );
//                errlog((char *)capture);
            }
            else
            {
//                errlog((char *)"!!! isNEW = 'Y' 3");
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, ( char * ) listing[ ( ( boxsize*Page ) +i )-1].ansi_3.c_str() );
//                errlog((char *)capture);
            }
        }
        else
        {
            if ( ( signed ) list+1 == ( boxsize*Page ) +i )
            {
//            if (current_result == list) {
//                errlog((char *)"!!! isNEW = 'N' 2");
                // Current Area
                current_selection = Top+i-1; // Get current place in box to display.
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, ( char * ) listing[ ( ( boxsize*Page ) +i )-1].ansi_2.c_str() );
//                errlog((char *)capture);
            }
            else
            {
//                errlog((char *)"!!! isNEW = 'N' 1");
                sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, ( char * ) listing[ ( ( boxsize*Page ) +i )-1].ansi_1.c_str() );
//                errlog((char *)capture);
            }
        }

        _editbox += capture;
    }

    // Write out Box.
    sprintf ( capture, "\x1b[%i;%iH", Row+Top-1, 1 );
    _editbox += capture;
    pipe2ansi ( ( char * ) _editbox.c_str() );

}
