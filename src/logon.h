/***************************************************************************
 *   Copyright (C) 2004-2017 by Michael Griffin                            *
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

# ifndef LOGON_H
# define LOGON_H

# include "struct.h"
# include "language.h"
# include "users.h"
# include "conio.h"

class logon :
    private
    users,
    language,
    ConsoleIO
{

    int useScript;
    int InvalidLogins;

public:
    logon();
    void Handle(UserRec *u);
    void Name(UserRec *u);
    void Password(UserRec *u);
    int  VerifyPassword(UserRec *u);
    void BDay(UserRec *u);
    void Sex(UserRec *u);
    void Email(UserRec *u);
    void EmailPriv(UserRec *u);
    void Note(UserRec *u);
    void ChallengeQuestion(UserRec *u);
    void ChallengeAnswer(UserRec *u);
    void application();
    void ScriptApplication();

    void ParseVerify(char *filename, UserRec *u);
    void save_user(UserRec *u);
    void verify_info(UserRec *u, char *mString = (char *)"verify");
    int  logon_system(UserRec *urec);
    void forgot_password();

    void save_existinguser(UserRec *u, unsigned long);
    void userinfo(UserRec *u, char *mString);

};

# endif
