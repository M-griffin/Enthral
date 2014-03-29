/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
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

#ifndef _CONSOLE
#define _CONSOLE

# include "struct.h"
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>


struct olm
{
    char olm_msg[200];
    int  olm_user;
    int  olm_number;
};

struct nodemessage
{
    short  command;
    int     data1;
    int     data2;
    int     data3;
    char str[200];
};

extern int serhandle;
extern int sockfd;

void clear_nodes();
int  init_nodes();
int  console_active();
int  init_console();
void open_console();
void close_console();
int  console_select_input(int, fd_set *);
int  console_pending_input(fd_set *);
int  console_getc();
int  console_putsn(char *, size_t n, int buffering=TRUE);
void finalize_console();

#endif
