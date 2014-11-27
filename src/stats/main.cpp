// Updated 2013-10-18 Michael Griffin
// Stilll oldskool code from 2004 or so
// Small updates for Enthral, could rewrite this lateron.

// Basic Stats Generation. (Post / Calls) For Now.
// 32 and 64 Bit compatiable.

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

# include <cstdio>
# include <cstring>
# include <cstdlib>
# include <unistd.h>
# include <string>
# include "../struct.h"
# include "process2.h"

using namespace std;

char BBSPATH[255]    = {0};

int main ( void )
{
    // Set up program to use bbs paths.
    char *pPath;
    pPath = std::getenv ( ( char * ) "ENTHRAL" );

    //printf((char *)" *** Environment 1: pPath: %s\r\n",pPath);
    if ( pPath!=NULL )
    {
        strcpy ( BBSPATH,pPath );
    }

    // Setup stats program path off bbs path.
    strcat ( BBSPATH, ( char * ) "/stats" );

    //printf((char *)" *** Environment 2: BBSPath: %s\r\n",BBSPATH);

    // Change to BBS path as current working folder.
    int iRet = 0;

    if ( ( iRet = chdir ( BBSPATH ) ) == 0 ) {}

    doallproc2();  // Kicks off Top Calls/Posts

    return 0;
}
