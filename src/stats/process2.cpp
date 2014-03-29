#include "../struct.h"
#include "process2.h"
#include "userdat.h"

#include <cstdio>              // C Standard I/o
#include <cstring>
#include <cstdlib>
#include <string>               // C++ String Library
#include <fstream>              // Files Streams C++

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

using namespace std;

/*--------------------------------------------------------------------------------*/
// Globals
ListItem_Stats *MYLIST  = 0;   // holds all user data
ListItem_Stats *MYLIST1 = 0;   // Sorts All Users by Top Calls / Uploads
ListItem_Stats *MYLIST2 = 0;   // Sorts All Users by Top Posts / Downloads
ListItem_Stats *MYLIST4 = 0;   // Hold ANSI for Writing out

/*--------------------------------------------------------------------------------*/
// Clears ListItem_Stats
void clearuserdata5()
{

    struct ListItem_Stats *p, *next;
    if ( MYLIST == 0)
    {
        return;
    }

    for(p = MYLIST ; p !=0 ; p = next )
    {
        next = p->next;
        delete (p);
    }
    MYLIST = 0;
    return;
}


/*--------------------------------------------------------------------------------*/
// Clears ListItem_Stats1
void clearuserdata3()
{

    struct ListItem_Stats *p, *next;
    if ( MYLIST1 == 0)
    {
        return;
    }

    for(p = MYLIST1 ; p !=0 ; p = next )
    {
        next = p->next;
        delete (p);
    }
    MYLIST1 = 0;
    return;
}


/*--------------------------------------------------------------------------------*/
// Clears ListItem_Stats2
void clearuserdata4()
{

    struct ListItem_Stats *p, *next;
    if ( MYLIST2 == 0)
    {
        return;
    }

    for(p = MYLIST2 ; p !=0 ; p = next )
    {
        next = p->next;
        delete (p);
    }
    MYLIST2 = 0;
    return;
}


/*--------------------------------------------------------------------------------*/
// Clears Link List and Free's Current Memory
void clearANSI2()
{

    struct ListItem_Stats *p, *next;
    if ( MYLIST4 == 0)
    {
        return;
    }

    for(p = MYLIST4 ; p !=0 ; p = next )
    {
        next = p->next;
        delete (p);
    }
    MYLIST4 = 0;
}

/*--------------------------------------------------------------------------------*/
// LINK LISTS - Functions To Collect and Sort All Data
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
// Stores all Data Recived into Link List w/out Sorting
void userdatacontainer2(UserRec * user)
{

    struct ListItem_Stats *last;
    //string data;

    // Don't Add Co's And Sysop's to Stats
    //short Sec = user->Security;
    //if (Sec > 199) { return; }

    if (strcmp((char *)user->handle,(char *)"Mercyful Fate") == 0)
    {
        return;
    }

    ListItem_Stats *item = new ListItem_Stats;

    if( !item )
    {
        printf("\nMalloc Failed");
        exit(1);
    }

    // If it gets here, no Name Match was found, Add New Player to the Link List with Defaults
    // Creates Player And Sets Record's Defaults all to 0 in Memory.
    item->user = (*user);
    item->next = 0;

//    printf("\n + Name Added: %s, C: %i, P: %i",item->user.handle, item->user.Calls, item->user.Posts );

    if ( MYLIST == 0 )
    {
        MYLIST = item;
        return;
    }

    for( last = MYLIST ; last->next != 0 ; last = last->next );
    last->next = item;
    return;

}

/*--------------------------------------------------------------------------------*/
// This Receives Users From ListList and Sorts All Users by Calls to the BBS
void callsort(UserRec *user)
{

    struct ListItem_Stats *next;
    string data;

    // if (user->handle == "Mercyful Fate") { return; }
    if (user->Calls == 0) return;

    ListItem_Stats *item = new ListItem_Stats;
    if( !item )
    {
        printf("\nMalloc Failed");
        exit(1);
    }


    // If it gets here, no Name Match was found, Add New Weapon to the Link List with Defaults
    // Creates Weapon And Sets Record's Defaults all to 0 in Memory.
    item->user = (*user);
    item->next = 0;

    if ( MYLIST1 == 0 )
    {
        MYLIST1 = item;
        return;
    }


    if(user->Calls > MYLIST1->user.Calls)
    {
        item->next = MYLIST1;
        MYLIST1 = item;
        return;
    }


    // Check to see if an item is inserted before the next item
    for ( next = MYLIST1 ; next->next != 0 ; next = next->next )
    {
        if (item->user.Calls > next->next->user.Calls)
        {
            // Insert Item before the next Item.
            item->next = next->next;
            next->next = item ;
            return;
        }
    }
    next->next = item;
    return;


}

/*--------------------------------------------------------------------------------*/
// This Receives Users From ListList and Sorts All Users by Calls to the BBS
void postsort(UserRec * user)
{

    struct ListItem_Stats *next;
    string data;

    // if (user->handle == "Mercyful Fate") { return; }
    if (user->Posts+user->Replies == 0) return;

    ListItem_Stats *item = new ListItem_Stats;
    if( !item )
    {
        printf("\nMalloc Failed");
        exit(1);
    }

    // If it gets here, no Name Match was found, Add New Weapon to the Link List with Defaults
    // Creates Weapon And Sets Record's Defaults all to 0 in Memory.
    item->user = (*user);
    item->next = 0;

    if ( MYLIST2 == 0 )
    {
        MYLIST2 = item;
        return;
    }


    if((user->Posts+user->Replies) > (MYLIST2->user.Posts+MYLIST2->user.Replies))
    {
        item->next = MYLIST2;
        MYLIST2 = item;
        return;
    }


    // Check to see if an item is inserted before the next item
    for ( next = MYLIST2 ; next->next != 0 ; next = next->next )
    {
        if ((user->Posts+user->Replies) > (next->next->user.Posts+next->next->user.Replies))
        {
            // Insert Item before the next Item.
            item->next = next->next;
            next->next = item ;
            return;
        }
    }
    next->next = item;
    return;


}

/*--------------------------------------------------------------------------------*/
// Functions Reading ANSI Templates, and Parsing MCI Code and Writing Out ANSI STATS
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
// Write out Ansi file for Top Calls / Posters
void writeANSI2()
{

    std::string filename = "topstat2.ans";
    filename = "../ansi/" + filename;

    struct ListItem_Stats *p;

    std::ofstream outStream3;
    outStream3.open( filename.c_str(), std::ofstream::out | std::ofstream::trunc);

    if (!outStream3.is_open())
    {
        printf("\n\nError writing out topstat2.ans");
        clearANSI2();
        exit(1);
        return;
    }

    for ( p = MYLIST4 ; p != 0 ; p = p->next )
    {
        outStream3 << p->ansi;
    }

    outStream3.close();
    clearANSI2();
    return;
}

/*--------------------------------------------------------------------------------*/
// Holds all Parsed ANSI for Writting out to File
void buildANSI2(string ansi)
{


//    printf(" + build Ansi \n%s",ansi.c_str());

    std::string temp = ansi;

    struct ListItem_Stats *last;
    ListItem_Stats *item = new ListItem_Stats;

    if ( !item )
    {
        printf("\nMalloc Failed For Holding Parsed ANSI");
        exit(1);
        return;
    }

    //strcpy(item->ansi,temp.c_str());
    item->ansi = temp;
    item->next = 0;

    if ( MYLIST4 == 0 )
    {
        MYLIST4 = item;
        return;
    }

    for( last = MYLIST4 ; last->next != 0 ; last = last->next );
    last->next = item;
}


/*--------------------------------------------------------------------------------*/
// Parses MCI Codes for Top Callers / Posters
void checkMCI2(string ansidata)
{
    string temp = ansidata;

    // Start String Processing Below
    std::string::size_type id1 = 0;

    id1 = temp.find('$', 0);
    // No MCI Code's Found, skip to next string
    if (id1 == std::string::npos)
    {
        buildANSI2(temp);
        return;
    }

    ListItem_Stats *ca; // Top Callers
    ListItem_Stats *po; // Top Posters

    // Holds all Users Data
    char     nc1[40]= {0}, nc2[40]= {0}, nc3[40]= {0}, nc4[40]= {0}, nc5[40]= {0};
    char     np1[40]= {0}, np2[40]= {0}, np3[40]= {0}, np4[40]= {0}, np5[40]= {0};

    long c1=0, c2=0, c3=0, c4=0, c5=0;
    long p1=0, p2=0, p3=0, p4=0, p5=0;

    // Copy Top Callers
    for ( ca = MYLIST1 ; ca != 0 ; ca = ca->next )
    {
        //printf("\n + ca sorting: %s, C: %i, P: %i",ca->user.handle, ca->user.Calls, ca->user.Posts );

        if(ca > 0)
        {
            strcpy(nc1,(char *)ca->user.handle);
            c1 = ca->user.Calls;
            ca = ca->next;
        }
        if(ca > 0)
        {
            strcpy(nc2,(char *)ca->user.handle);
            c2 = ca->user.Calls;
            ca = ca->next;
        }
        if(ca > 0)
        {
            strcpy(nc3,(char *)ca->user.handle);
            c3 = ca->user.Calls;
            ca = ca->next;
        }
        if(ca > 0)
        {
            strcpy(nc4,(char *)ca->user.handle);
            c4 = ca->user.Calls;
            ca = ca->next;
        }
        if(ca > 0)
        {
            strcpy(nc5,(char *)ca->user.handle);
            c5 = ca->user.Calls;
            ca = ca->next;
        }
        break;
    }

    // Copy Top Posters
    for ( po = MYLIST2 ; po != 0 ; po = po->next )
    {
        //printf("\n + po sorting: %s, C: %i, P: %i",po->user.handle, po->user.Calls, po->user.Posts );

        if(po > 0)
        {
            strcpy(np1,(char *)po->user.handle);
            p1 = po->user.Posts+po->user.Replies;
            po = po->next;
        }
        if(po > 0)
        {
            strcpy(np2,(char *)po->user.handle);
            p2 = po->user.Posts+po->user.Replies;
            po = po->next;
        }
        if(po > 0)
        {
            strcpy(np3,(char *)po->user.handle);
            p3 = po->user.Posts+po->user.Replies;
            po = po->next;
        }
        if(po > 0)
        {
            strcpy(np4,(char *)po->user.handle);
            p4 = po->user.Posts+po->user.Replies;
            po = po->next;
        }
        if(po > 0)
        {
            strcpy(np5,(char *)po->user.handle);
            p5 = po->user.Posts+po->user.Replies;
            po = po->next;
        }
        break;
    }

    // Parsing Starts Here
    while (id1 != std::string::npos)
    {
        // Top Callers
        id1 = temp.find("$CN1", 0);
        if (id1 != std::string::npos)
        {
            temp.replace(id1,4,nc1);
        }

        id1 = temp.find("$CA1", 0);
        if (id1 != std::string::npos)
        {
            char con[15]= {0};
            if (c1 > 0)
            {
                sprintf(con, "%ld",c1);
            }
            temp.replace(id1,4,con);
        }
        id1 = temp.find("$CN2", 0);
        if (id1 != std::string::npos)
        {
            temp.replace(id1,4,nc2);
        }

        id1 = temp.find("$CA2", 0);
        if (id1 != std::string::npos)
        {
            char con[15]= {0};
            if (c2 > 0)
            {
                sprintf(con, "%ld",c2);
            }
            temp.replace(id1,4,con);
        }

        id1 = temp.find("$CN3", 0);
        if (id1 != std::string::npos)
        {
            temp.replace(id1,4,nc3);
        }

        id1 = temp.find("$CA3", 0);
        if (id1 != std::string::npos)
        {
            char con[15]= {0};
            if (c3 > 0)
            {
                sprintf(con, "%ld",c3);
            }
            temp.replace(id1,4,con);
        }

        id1 = temp.find("$CN4", 0);
        if (id1 != std::string::npos)
        {
            temp.replace(id1,4,nc4);
        }

        id1 = temp.find("$CA4", 0);
        if (id1 != std::string::npos)
        {
            char con[15]= {0};
            if (c4 > 0)
            {
                sprintf(con, "%ld",c4);
            }
            temp.replace(id1,4,con);
        }

        id1 = temp.find("$CN5", 0);
        if (id1 != std::string::npos)
        {
            temp.replace(id1,4,nc5);
        }

        id1 = temp.find("$CA5", 0);
        if (id1 != std::string::npos)
        {
            char con[15]= {0};
            if (c5 > 0)
            {
                sprintf(con, "%ld",c5);
            }
            temp.replace(id1,4,con);
        }
        // Top Posters
        id1 = temp.find("$PN1", 0);
        if (id1 != std::string::npos)
        {
            temp.replace(id1,4,np1);
        }

        id1 = temp.find("$PO1", 0);
        if (id1 != std::string::npos)
        {
            char con[15]= {0};
            if (p1 > 0)
            {
                sprintf(con, "%ld",p1);
            }
            temp.replace(id1,4,con);
        }

        id1 = temp.find("$PN2", 0);
        if (id1 != std::string::npos)
        {
            temp.replace(id1,4,np2);
        }

        id1 = temp.find("$PO2", 0);
        if (id1 != std::string::npos)
        {
            char con[15]= {0};
            if (p2 > 0)
            {
                sprintf(con, "%ld",p2);
            }
            temp.replace(id1,4,con);
        }

        id1 = temp.find("$PN3", 0);
        if (id1 != std::string::npos)
        {
            temp.replace(id1,4,np3);
        }

        id1 = temp.find("$PO3", 0);
        if (id1 != std::string::npos)
        {
            char con[15]= {0};
            if (p3 > 0)
            {
                sprintf(con, "%ld",p3);
            }
            temp.replace(id1,4,con);
        }

        id1 = temp.find("$PN4", 0);
        if (id1 != std::string::npos)
        {
            temp.replace(id1,4,np4);
        }

        id1 = temp.find("$PO4", 0);
        if (id1 != std::string::npos)
        {
            char con[15]= {0};
            if (p4 > 0)
            {
                sprintf(con, "%ld",p4);
            }
            temp.replace(id1,4,con);
        }

        id1 = temp.find("$PN5", 0);
        if (id1 != std::string::npos)
        {
            temp.replace(id1,4,np5);
        }

        id1 = temp.find("$PO5", 0);
        if (id1 != std::string::npos)
        {
            char con[15]= {0};
            if (p5 > 0)
            {
                sprintf(con, "%ld",p5);
            }
            temp.replace(id1,4,con);
        }
        buildANSI2(temp);
        return;
    }
    buildANSI2(temp);
    return;
}


// Reads in Ansi file into Buffer Only
int parseansi2()
{

    std::string buff;

    // Setup the Source
    string filename;
    filename = "../stats/toppc.ans";

    FILE *fp;
    int c = 0;
    if ((fp = fopen(filename.c_str(), "r+")) ==  NULL)
    {
        return FALSE;
    }
    do
    {
        c = getc(fp);
        if (c == '\n')
        {
            c = '\r';
        }
        if (c != EOF) buff += c;
    }
    while (c != EOF);
    fclose(fp);

    //printf ("\n + CheckMCI2");
    checkMCI2(buff);
    return TRUE;
}


/*--------------------------------------------------------------------------------*/
// Function to retrive all users/data from Users.dat Populates Linklist
void getusers2(void)
{
    UserRec user;
    string temp;

    int idx = 0;
    int total = countusers();

    if (total > 0)
        while(idx < total)
        {
            userread(&user,idx);
            userdatacontainer2(&user);
            idx++;
        }
    return;
}


/*--------------------------------------------------------------------------------*/
// Kicks off Call / Posts Processing
void proccalls(void)
{
    struct ListItem_Stats *us;

    for ( us = MYLIST ; us != 0 ; us = us->next )
    {
        // Creates Sorted Users by Top Calls
        callsort(&us->user);  // List 1
        postsort(&us->user);  // List 2
    }

    // Kick off Processing of Top Callers Here
    parseansi2();  // Parses Ansi and Replaces MCI Codes
    writeANSI2();  // Writes out Completed ANSI with Statistics

    // Clears Link list with Top Callers
    clearuserdata3();
    clearuserdata4();
    return;
}


/*--------------------------------------------------------------------------------*/
// Main Starter Functions for Collecting and Processing all Data
void doallproc2(void)
{

    getusers2();        // Gets All Users from USERS.DAT

    proccalls();       // Processes and Creates Top Calls/Posts Stats

    clearuserdata5();   // Clears all User Data from Memory
    return;
}


