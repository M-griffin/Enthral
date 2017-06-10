/***************************************************************************
 *   Copyright (C) 2004-2017 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Embedding Python for Scripting BBS functions                 *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#define BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_DYN_LINK

# include "struct.h"
# include "conio.h"
# include "menu_func.h"

# include <boost/python.hpp>

# include <boost/filesystem.hpp>
# include <boost/lexical_cast.hpp>

# include <iostream>
# include <string>
# include <csignal>

// Hold UserRec for Session Information.
UserRec *pyUser;

using namespace boost::python;
using namespace boost::filesystem;

/*
 * Python Hook into Menu System Class.
 */
class pyMenuSystem : public menu_func
{

public:
    std::string menuinput;
    CommandRec cmdr;

    pyMenuSystem() {
        menu_setuser(pyUser);
    }

    // Setup Defaults for Running a Menu
    void set(std::string menu) {
        _premenu.clear();
        _gosub.clear();
        _curmenu = menu;
        _loadnew = true;
    }

    // Read in a Menu File w/ Commands
    void readin() {
        menu_readin();
    }

    // Must first populate the cmdr record
    void docmd() {
        menu_docmd (&cmdr);
    }

    // Clear Command for next run
    void clearcmd() {
        memset(&cmdr,0,sizeof(CommandRec));
    }

    // Run a menu, Mstring is populated with CommandKey
    void startmenu(char *mString, uint32_t area) {
        menu_proc(mString, area);
    }
};

/*
 * Python Hook - get Terminal Height
 */
int pyGetTermHeight()
{
    return TERM_HEIGHT;
}

/*
 * Python Hook - get Terminal Width
 */
int pyGetTermWidth()
{
    return TERM_WIDTH;
}

/*
 * Python Hook - Display Ansi File
 */
void pyAnsiPrintf(std::string str)
{
    ConsoleIO s(pyUser);
    if ((signed)str.size() > 0)
        s.ansiPrintf((char *)str.c_str());
    return;
}

// Hold ESC Sequence
static std::string KeyCombination;

/*
 * Python Hook - Return ESC Sequence
 */
std::string pyGetKeyExtended()
{
    return KeyCombination;
}

/*
 * Python Hook - Return Parsed String of MCI And PIPE Colors
 */
std::string pyPipe2String(std::string str)
{
    ConsoleIO s(pyUser);
    return s.pipe2string(str);
}

/*
 * Python Hook - Get Single Key Input
 */
std::string pyGetKey()
{
    ConsoleIO s(pyUser);
    int ch = 0;

    ch = s.getkey(true);
    if (ch == 27) {
        KeyCombination.erase();
        KeyCombination.append(s.EscapeKey);
    } else
        KeyCombination.erase();

    return boost::lexical_cast<std::string>(ch);
}

/*
 * Python Hook - Get Line Input
 */
std::string pyGetLine(int len)
{
    ConsoleIO s(pyUser);
    char str[1024]= {0};
    s.getline(str,len);
    std::string sstr = str;

    return sstr;
}

/*
 * Python Hook - setup Input Field with Length
 */
std::string pyInputField(std::string str, int len)
{
    ConsoleIO s(pyUser);
    char instr[1024]= {0};

    // Make sure string in not passed to big!
    if ((signed)str.size() > 1024)
        str.erase(1023,(signed)str.size() - 1023);

    strcpy(instr, (char *)str.c_str());

    s.inputfield(instr, len);
    std::string sstr = instr;

    return sstr;
}

/*
 * Python Hook - Write Out and Parse text for MCI and PIPE Codes
 */
void pyPipe2Ansi(std::string str)
{
    ConsoleIO s(pyUser);
    if ((signed)str.size() > 0)
        s.pipe2ansi((char *)str.c_str());
    return;
}

/*
 * Python Hook - Write Out Raw String
 */
void pyPutLine(std::string str)
{
    ConsoleIO s(pyUser);
    if ((signed)str.size() > 0)
        s.putline((char *)str.c_str());
    return;
}

/*
 * Python Hook - Animated Pause
 */
void pyStartPause()
{
    ConsoleIO s(pyUser);
    s.startpause();
    return;
}

/*
 * Python Hook - Setup C++ Fucntion Hooks
 */
BOOST_PYTHON_MODULE(bbs_io)
{
    // Terminal
    def("pyGetTermHeight" , pyGetTermHeight);
    def("pyGetTermWidth"  , pyGetTermWidth);

    // Base I/o Input
    def("pyGetKey"        , pyGetKey);
    def("pyGetKeyExtended", pyGetKeyExtended);
    def("pyGetLine"       , pyGetLine);
    def("pyInputField"    , pyInputField);

    // Base I/o Output
    def("pyAnsiPrintf"    , pyAnsiPrintf);
    def("pyPutLine"       , pyPutLine);
    def("pyPipe2Ansi"     , pyPipe2Ansi);
    def("pyPipe2String"   , pyPipe2String);

    def("pyStartPause"    , pyStartPause);
}

/*
 * Python Interface pass script and reference to user record.
 */
void pybbs_run( std::string script, UserRec *usr )
{
    ConsoleIO _io;
    pyUser = usr;

    // Setup scripts path.
    std::string path;
    path = SCRIPTS;
    path += script;

    // Clear for initial run.
    KeyCombination.erase();

    sigset_t signal_set;
    sigemptyset(&signal_set);

    // Block Signals in Python, casue then the BBS doesn't get them.
    sigaddset(&signal_set, SIGHUP);
    sigaddset(&signal_set, SIGTERM);
    sigaddset(&signal_set, SIGINT);
    sigaddset(&signal_set, SIGILL);
    sigaddset(&signal_set, SIGABRT);
    sigaddset(&signal_set, SIGCHLD);
    sigaddset(&signal_set, SIGQUIT);
    sigaddset(&signal_set, SIGKILL);
    sigaddset(&signal_set, SIGWINCH);

    sigprocmask(SIG_BLOCK, &signal_set, NULL);

    try {

        if (!boost::filesystem::exists(path.c_str())) {
            return;
        } 

        _io.errlog((char *)"Starting Boost_Python %s",path.c_str());

        Py_Initialize();

        initbbs_io(); // initialize Pointless

        object module = import("__main__");
        object name_space = module.attr("__dict__");

        object result = exec_file(path.c_str(), name_space, name_space);

        _io.errlog((char *)"Exiting Boost_Python");

    } catch(error_already_set const &) {
        PyErr_Print();

        _io.errlog((char *)"Python Exception");

        // Only Exit if a user, not if sysop debuging!
        if (isSysop == FALSE)
            exit(1);
    }

    _io.errlog((char *)"Finished Boost_Python");

    // Unblock Signals
    sigprocmask(SIG_UNBLOCK, &signal_set, NULL);

    return;
}
