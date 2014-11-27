/***************************************************************************
 *   Copyright (C) 2004-2013 by Michael Griffin                            *
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

//# define _XOPEN_SOURCE
# include "conio.h"
# include "dtfunc.h"

# include <boost/lexical_cast.hpp>
# include <iostream>
# include <time.h>
# include <cstdio>
# include <cstring> //gcc 4.3
# include <cstdlib> //gcc 4.3
# include <string>
# include <sstream>


using namespace std;

// Example to format from Seconds to Date/Time -MF
/*
int main () {

    struct tm *tm_ptr,*tm_ptr2,  ts;
    time_t  the_time, tm2,tm3;
    char buf[256];
    char *result;
    double buf2 = 0;
    double buf3 = 0;

    time (&the_time);
    tm_ptr = localtime(&the_time);

    // Convert Current Time to Seconds
    strftime(buf, 256, "%A %d %B  %Y %I:%M:%S %p", tm_ptr);
    printf("\nFormatted: %s\n",buf);
    printf("\nRAW: %ld",the_time);
    sprintf(buf,"%ld",the_time);
    buf2 = atol(buf);

    // Convert Seconds to Current Time
    tm2 = (time_t)buf2;
    tm_ptr = localtime(&tm2);
    strftime(buf, 256, "%A %d %B %Y %I:%M:%S %p", tm_ptr);
    printf("\nFormatted: %s\n", buf);

    buf2 = 0;
    memset(&buf,0,sizeof(buf));
    memset(&ts,0,sizeof(tm));

    ts.tm_mday = 4;
    ts.tm_mon = 4 - 1;
    ts.tm_year = 1977 - 1900;
    ts.tm_isdst = -1;

    time_t tmp_t;
    tmp_t = mktime(&ts);

    memset(&buf,0,sizeof(buf));
    printf("\n MKTIME DATE RAW: %d",tmp_t);

    long i = Date2Sec("04/04/1977");
    printf("\n TEST DATE RAW: %ld ",i);

    sprintf(buf,"%s",Sec2Date(i));
    printf("\nDATE Formatted: %s\n", buf);
    return 0;

}*/

// Future Edit.
// Split this up into MCI Codes for Days, Hours, minues, seconds!!
char *getLastCallDays ( std::time_t lastcall )
{
    std::time_t  theTime, callTime;


    // Convert Current Time to Breakdown Format
    theTime = time ( 0 );
    callTime = lastcall;

    std::string s;
    std::stringstream ss;

    if ( theTime != ( std::time_t ) ( -1 ) && callTime != ( std::time_t ) ( -1 ) )
    {
        double difference = std::difftime ( theTime, callTime ); /// (60 * 60 * 24);

        int days = ( int ) difference / 86400;
        int hours = ( int ) ( difference / 3600 ) - ( days * 24 );
        int mins = ( int ) ( difference / 60 ) - ( days * 1440 ) - ( hours * 60 );
        int secs = ( int ) difference % 60;

        // Use Space for Padding each Time
        bool space = false;

        if ( days > 0 )
        {
            space = true;

            if ( days > 1 )
                ss << days  << ") Days";
            else
                ss << days  << ") Day";
        }

        if ( hours > 0 )
        {
            if ( space )
                ss << ", ";

            space = true;

            if ( hours > 1 )
                ss << hours << ") Hours";
            else
                ss << hours << ") Hour";
        }

        if ( mins > 0 )
        {
            if ( space )
                ss << ", ";

            space = true;

            if ( mins > 1 )
                ss << mins  << ") Minutes";
            else
                ss << mins  << ") Minute";
        }

        if ( secs > 0 )
        {
            if ( space )
                ss << ", ";

            if ( secs > 1 )
                ss << secs  << ") Seconds";
            else
                ss << secs  << ") Second";
        }

        s = ss.str();
        return ( char * ) s.c_str();

    }

    return ( char * ) "error";
}



char *getAge ( std::time_t sec )
{
    std::time_t  theTime, birthTime;
    std::tm     *aTime,   *bTime;

    // Convert Current Time to Breakdown Format
    theTime = GetCurrentDTSec();
    aTime = std::localtime ( &theTime );

    int currentday   = aTime->tm_mday;
    int currentmonth = aTime->tm_mon  + 1;     // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
    int currentyear  = aTime->tm_year + 1900;

    // Convert Birth Time to Breakdown Format
    birthTime = sec;
    bTime = std::localtime ( &birthTime );

    int birthday   = bTime->tm_mday;
    int birthmonth = bTime->tm_mon  + 1;     // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
    int birthyear  = bTime->tm_year + 1900;

    int ageInYears = currentyear - birthyear;

    if ( birthmonth > currentmonth )
        --ageInYears;
    else if ( birthmonth == currentmonth && birthday >= currentday )
        --ageInYears;

    std::string s = boost::lexical_cast<std::string> ( ageInYears );
    return ( char * ) s.c_str();
}


int isBday ( std::time_t sec )
{

    std::time_t  tmp1;
    std::tm     *t2;

    char date[25]= {0},
                   date2[25]= {0};

    //static char buff[4]={0};

    // Convert Current Time to Birthdat Format.
    tmp1 = GetCurrentDTSec();
    t2 = localtime ( &tmp1 );
    strftime ( date, 256, "%m/%d/%Y", t2 );

    SESSION s;

    // Setup Birthday.
    strcpy ( date2,Sec2Date ( sec ) );

    // Current Date/Time
    int i = 0, t = 0, mm, dd;
    char dt[5]= {0};
    dt[i++] = date[t++];
    dt[i++] = date[t++];
    mm = atoi ( dt );
    ++t;
    i = 0;
    dt[i++] = date[t++];
    dt[i++] = date[t++];
    dd = atoi ( dt );
    ++t;
    i = 0;
    dt[i++] = date[t++];
    dt[i++] = date[t++];
    dt[i++] = date[t++];
    dt[i++] = date[t++];
    //yy = atoi(dt);

    // BirthDate and Time
    i = 0;
    t = 0;
    int mm2, dd2;
    char dt2[5]= {0};
    dt2[i++] = date2[t++];
    dt2[i++] = date2[t++];
    mm2 = atoi ( dt2 );
    ++t;
    i = 0;
    dt2[i++] = date2[t++];
    dt2[i++] = date2[t++];
    dd2 = atoi ( dt2 );
    ++t;
    i = 0;
    dt2[i++] = date2[t++];
    dt2[i++] = date2[t++];
    dt2[i++] = date2[t++];
    dt2[i++] = date2[t++];
    //yy2 = atoi(dt2);

    if ( dd == dd2 && mm == mm2 )
        return TRUE;

    return FALSE;

}


int VerifyBday ( char *date )
{

    int i = 0, t = 0, mm, dd,  yy;
    char dt[5]= {0};
    dt[i++] = date[t++];
    dt[i++] = date[t++];
    mm = atoi ( dt );
    ++t;
    i = 0;
    dt[i++] = date[t++];
    dt[i++] = date[t++];
    dd = atoi ( dt );
    ++t;
    i = 0;
    dt[i++] = date[t++];
    dt[i++] = date[t++];
    dt[i++] = date[t++];
    dt[i++] = date[t++];
    yy = atoi ( dt );

    // Get Current Year.
    char curdate[25]= {0};
    time_t  tmp1;
    tm     *t2;

    tmp1 = GetCurrentDTSec();
    t2 = std::localtime ( &tmp1 );
    strftime ( curdate, 256, "%Y", t2 );

    // Just some basic verification!
    if ( mm < 1 || mm > 12 ) return FALSE;

    if ( dd < 1 || dd > 31 ) return FALSE;

    // Older then 13 to Access!
    if ( ( yy < 1 ) || ( yy > atoi ( curdate )-13 ) || ( yy < atoi ( curdate )-100 ) ) return FALSE;

    return TRUE;
}


// Get Current Date/Time and return seconds
time_t GetCurrentDTSec()
{
    std::time_t  the_time;
    the_time = time ( 0 );
    return the_time;
}

//Seconds to Date Format Birthday
char *Sec2Date ( std::time_t sec )
{

    std::time_t currnetTime;
    std::tm     *tStamp;
    char buf[256] = {0};

    currnetTime = sec;
    tStamp = std::localtime ( &currnetTime );

    //cout << "sec test: " << sec << endl;
    /*
    int currentday   = tStamp->tm_mday;
    int currentmonth = tStamp->tm_mon  + 1;     // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
    int currentyear  = tStamp->tm_year + 1900;
    */
    //cout << "date test: " << currentmonth << currentday << currentyear << endl;

    strftime ( buf, 256, "%m/%d/%Y", tStamp );

    std::string s = buf;
    return ( char * ) s.c_str();
}

time_t Date2Sec ( char *strdate ) // "04/04/1977"
{

    // Pre-Parsing and Converting from 1/1/2007 to 01/01/2007
    std::string dtCompare;
    dtCompare = strdate;

    std::string::size_type id1 = 0;

    id1 = dtCompare.find ( "/",0 );

    // Insert Preceeding 0 to update to 01/ from 1/
    if ( id1 < 2 )
    {
        dtCompare.insert ( 0,"0" );
    }

    id1 = dtCompare.find ( "/",3 );

    // Insert Preceeding 0 to update to 01/01 from 01/1
    if ( id1 < 5 )
    {
        dtCompare.insert ( 3,"0" );
    }

    int i = 0, t = 0, mm, dd,  yy;
    tm ts;

    memset ( &ts,0,sizeof ( tm ) );
    char dt[5]= {0};
    dt[i++] = dtCompare[t++];
    dt[i++] = dtCompare[t++];
    mm = atoi ( dt );
    ++t;
    i = 0;
    dt[i++] = dtCompare[t++];
    dt[i++] = dtCompare[t++];
    dd = atoi ( dt );
    ++t;
    i = 0;
    dt[i++] = dtCompare[t++];
    dt[i++] = dtCompare[t++];
    dt[i++] = dtCompare[t++];
    dt[i++] = dtCompare[t++];
    yy = atoi ( dt );


    //cout << "test: " << dd << mm << yy << endl;


    ts.tm_mday  = dd;
    ts.tm_mon   = mm - 1;
    ts.tm_year  = yy - 1900;
    ts.tm_isdst = -1;

    time_t tmp_t;
    tmp_t = mktime ( &ts );

    //cout << "epoc: " << tmp_t << endl;

    return ( tmp_t );
}


char *Sec2DateTM ( std::time_t time, int format )
{
    std::time_t  the_time;
    std::tm *tm_ptr;

    char buff[256] = {0};
    memset ( &buff,0,sizeof ( buff ) );
    // Convert Seconds to Current Time
    the_time = ( time_t ) time;
    tm_ptr = std::localtime ( &the_time );

    /*
    int currentday   = tm_ptr->tm_mday;
    int currentmonth = tm_ptr->tm_mon  + 1;     // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
    int currentyear  = tm_ptr->tm_year + 1900;

    cout << "date test: " << currentday << currentmonth << currentyear << endl;
    */

    // Setup Formatting For Every DAT & Seperate Time Format (WIP)
    switch ( format )
    {
        case 1:  // Default Message Date / Time Format
            strftime ( buff, 256, "%A %B %d, %Y %I:%M:%S%p", tm_ptr );
            break;

        case 2: // Date / Time
            strftime ( buff, 256, "%m/%d/%Y %I:%M%p", tm_ptr );
            break;

        case 3: // Date
            strftime ( buff, 256, "%m/%d/%Y", tm_ptr );
            break;

        case 4: // Time
            strftime ( buff, 256, "%I:%M%p", tm_ptr );
            break;

        case 5: // Date / Time
            strftime ( buff, 256, "%A %m/%d/%Y %I:%M%p", tm_ptr );
            break;

        case 6: // Date / Time
            strftime ( buff, 256, "%Y-%m-%d %I:%M%p", tm_ptr );
            break;

        default :
            break;
    }

    std::string s = buff;
    return ( char * ) s.c_str();

}

time_t StampToDosTime ( struct _stamp *st )
{

    time_t tt;
    struct tm tms;
    tms.tm_sec   = st->time.ss << 1;
    tms.tm_min   = st->time.mm;
    tms.tm_hour  = st->time.hh;
    tms.tm_mday  = st->date.da;
    tms.tm_mon   = st->date.mo - 1;
    tms.tm_year  = st->date.yr + 80;
    tms.tm_isdst = -1;
    tt = mktime ( &tms );
    return tt;
}

struct _stamp *DosTimeToStamp ( time_t tt )
{

    struct tm *tmsp;
    static struct _stamp st;
    tmsp = localtime ( &tt );
    st.time.ss = tmsp->tm_sec >> 1;
    st.time.mm = tmsp->tm_min;
    st.time.hh = tmsp->tm_hour;
    st.date.da = tmsp->tm_mday;
    st.date.mo = tmsp->tm_mon + 1;
    st.date.yr = tmsp->tm_year - 80;
    return ( &st );
}
