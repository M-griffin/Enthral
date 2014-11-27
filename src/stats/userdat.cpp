
/*--------------------------------------------------------------------------------*/
// Libraries  c/c++

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "../struct.h"

#include <cstdio>              // C Standard I/o
#include <cstring>
#include <cstdlib>
#include <string>               // C++ String Library

using namespace std;            // Standard C++ Namespace

/*--------------------------------------------------------------------------------*/
// Users.DAT Reads - Basic I/O Read
int userread ( UserRec * users, int idx )
{
    std::string userpath = "../data/users.dat";
    int x = 0;;
    FILE * stream;
    stream = fopen ( userpath.c_str(),"rb+" );

    if ( stream == NULL )
    {
        printf ( "\nUnable to read/open Users.dat!" );
        fclose ( stream );
        exit ( 1 );
    }

    fclose ( stream );

    stream = fopen ( userpath.c_str(), "rb" );

    if ( fseek ( stream, ( long int ) idx*sizeof ( UserRec ),SEEK_SET ) ==0 )
        x = fread ( users,sizeof ( UserRec ),1,stream );

    fclose ( stream );
    return x;
}


/*--------------------------------------------------------------------------------*/
// Reads player.dat and gets Total number of Players that have been loaded
int countusers()
{
    UserRec users;
    int i=0;

    while ( userread ( &users,i ) )
    {
        ++i;
    }

    ++i;

    if ( i<1 )
    {
        i=-1;
    }
    else
    {
        i--;
    }

    return ( i );
}
