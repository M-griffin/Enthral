#ifndef __USERDAT_H
#define __USERDAT_H

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "../struct.h"

/*--------------------------------------------------------------------------------*/
// Reads and Gets all users data from Users.dat
int userread(UserRec * user, int idx);

/*--------------------------------------------------------------------------------*/
// Reads Users.dat and gets Total number of Users in that file
int countusers();

#endif
