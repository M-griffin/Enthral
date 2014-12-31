#ifndef __PROCESS2_H
#define __PROCESS2_H

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <string>
#include "../struct.h"

using namespace std;

/*--------------------------------------------------------------------------------*/
// Globals
//*MYLIST  // holds all user data
//*MYLIST1 // Sorts All Users by Top Calls / Uploads
//*MYLIST2 // Sorts All Users by Top Posts / Downloads
//*MYLIST4 // Hold ANSI for Writing out

/*--------------------------------------------------------------------------------*/
// Link List for Holding all Users in Users.dat no sorting
typedef struct ListItem_Stats
{
    UserRec user;
    std::string ansi;
    struct ListItem_Stats *next;

} ListItem_Stats;


/*--------------------------------------------------------------------------------*/
// LINK LISTS - Functions to Clear All Alloclated Memory
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
// Clears ListItem
void clearuserdata2(void);

/*--------------------------------------------------------------------------------*/
// Clears ListItem1
void clearuserdata3(void);

/*--------------------------------------------------------------------------------*/
// Clears ListItem2
void clearuserdata4(void);

/*--------------------------------------------------------------------------------*/
// Clears Link List and Free's Current Memory
void clearANSI2(void);

/*--------------------------------------------------------------------------------*/
// LINK LISTS - Functions To Collect and Sort All Data
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
// Stores all Data Recived into Link List w/out Sorting
void userdatacontainer2(UserRec * user);

/*--------------------------------------------------------------------------------*/
// This Receives Users From ListList and Sorts All Users by Calls to the BBS
void callsort(UserRec * user);

/*--------------------------------------------------------------------------------*/
// This Receives Users From ListList and Sorts All Users by Calls to the BBS
void postsort(UserRec * user);

/*--------------------------------------------------------------------------------*/
// Functions Reading ANSI Templates, and Parsing MCI Code and Writing Out ANSI STATS
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
// Write out Ansi file for Top Calls / Posters
void writeANSI2();

/*--------------------------------------------------------------------------------*/
// Holds all Parsed ANSI for Writting out to File
void buildANSI2(string ansi);

/*--------------------------------------------------------------------------------*/
// Parses MCI Codes for Top Callers / Posters
void checkMCI2(string ansidata);

/*--------------------------------------------------------------------------------*/
// Read ANSI Template and parse for MCI code
int parseansi2();

/*--------------------------------------------------------------------------------*/
// Functions To Collect and Populate Link List for Data Sorting and Processing
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
// Function to retrive all users/data from Users.dat Populates Linklist
void getusers2(void);

/*--------------------------------------------------------------------------------*/
// Kicks off Call / Posts Processing
void proccalls(void);

/*--------------------------------------------------------------------------------*/
// Functions That kick off all processing
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
// Main Starter Functions for Collecting and Processing all Data
void doallproc2(void);

#endif
