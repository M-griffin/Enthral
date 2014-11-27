/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
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

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$


#define BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_DYN_LINK

# include "struct.h"
# include "conio.h"
# include "menu_func.h"

// Start of new Boost::Python Embedding.
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

// BBS IO Python Modules:

/*
 * Python Hook into Menu System Class.
 */
struct pyMenuSystem : public menu_func
{

    std::string menuinput;
    CommandRec cmdr;

    pyMenuSystem()
    {
        menu_setuser ( pyUser );
    }

    // Setup Defaults for Running a Menu
    void set ( std::string menu )
    {
        _premenu.clear();
        _gosub.clear();
        _curmenu = menu;
        _loadnew = true;
    }

    // Read in a Menu File w/ Commands
    void readin()
    {
        menu_readin();
    }

    // Run a specific Menu CommandRec
    // Must first populate the cmdr record
    void docmd()
    {
        menu_docmd ( &cmdr );
    }

    // Clear Command for next run
    void clearcmd()
    {
        memset ( &cmdr,0,sizeof ( CommandRec ) );
    }

    // Run a menu, Mstring is populated with CommandKey
    void startmenu ( char *mString, uint32_t area )
    {
        menu_proc ( mString, area );
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
void pyAnsiPrintf ( std::string str )
{
    SESSION s ( pyUser );

    if ( ( signed ) str.size() > 0 )
        s.ansiPrintf ( ( char * ) str.c_str() );

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
std::string pyPipe2String ( std::string str )
{
    SESSION s ( pyUser );
    return s.pipe2string ( str );
}

/*
 * Python Hook - Get Single Key Input
 */
std::string pyGetKey()
{
    SESSION s ( pyUser );
    int ch = 0;

    ch = s.getkey ( true );

    if ( ch == 27 )
    {
        KeyCombination.erase();
        KeyCombination.append ( s.EscapeKey );
    }
    else
        KeyCombination.erase();

    return boost::lexical_cast<std::string> ( ch );
}

/*
 * Python Hook - Get Line Input
 */
std::string pyGetLine ( int len )
{
    SESSION s ( pyUser );

    char str[1024]= {0};
    s.getline ( str,len );
    std::string sstr = str;

    return sstr;
}

/*
 * Python Hook - setup Input Field with Length
 */
std::string pyInputField ( std::string str, int len )
{
    SESSION s ( pyUser );

    char instr[1024]= {0};

    // Make sure string in not passed to big!
    if ( ( signed ) str.size() > 1024 )
        str.erase ( 1023, ( signed ) str.size() - 1023 );

    strcpy ( instr, ( char * ) str.c_str() );

    s.inputfield ( instr, len );
    std::string sstr = instr;

    return sstr;
}

/*
 * Python Hook - Write Out and Parse text for MCI and PIPE Codes
 */
void pyPipe2Ansi ( std::string str )
{
    SESSION s ( pyUser );

    //  s.errlog2((char *)str.c_str());
    if ( ( signed ) str.size() > 0 )
        s.pipe2ansi ( ( char * ) str.c_str() );

    return;
}

/*
 * Python Hook - Write Out Raw String
 */
void pyPutLine ( std::string str )
{
    SESSION s ( pyUser );

    if ( ( signed ) str.size() > 0 )
        s.putline ( ( char * ) str.c_str() );

    return;
}

/*
 * Python Hook - Animated Pause
 */
void pyStartPause()
{
    SESSION s ( pyUser );
    s.startpause();
    return;
}


/*
 * Python Hook - Setup C++ Fucntion Hooks
 */
BOOST_PYTHON_MODULE ( bbs_io )
{

    // Access to Menu System
    boost::python::class_<pyMenuSystem> ( "pyMenuSystem" )
    .def ( "set", &pyMenuSystem::set )
    .def ( "readin", &pyMenuSystem::readin )
    .def ( "docmd", &pyMenuSystem::docmd )
    .def ( "clearcmd", &pyMenuSystem::clearcmd )
    .def ( "startmenu", &pyMenuSystem::startmenu );


    // Terminal
    def ( "pyGetTermHeight"  , pyGetTermHeight );
    def ( "pyGetTermWidth"  , pyGetTermWidth );

    // Base I/o Input
    def ( "pyGetKey"        , pyGetKey );
    def ( "pyGetKeyExtended", pyGetKeyExtended );
    def ( "pyGetLine"       , pyGetLine );
    def ( "pyInputField"    , pyInputField );

    // Base I/o Output
    def ( "pyAnsiPrintf"    , pyAnsiPrintf );
    def ( "pyPutLine"       , pyPutLine );
    def ( "pyPipe2Ansi"     , pyPipe2Ansi );
    def ( "pyPipe2String"   , pyPipe2String );

    def ( "pyStartPause"    , pyStartPause );
}



//------------------------------------------------------------------------------
// Name: main()
// Desc: Application's main entry point.
// Note: Scripts should import gc and call gc.collect() to free memory at end.
//------------------------------------------------------------------------------

/*
 * Python Interface pass script and reference to user record.
 */
void pybbs_run ( std::string script, UserRec *usr )
{

    SESSION _io;
    pyUser = usr;

    // Setup scripts path.
    std::string path;
    path = SCRIPTS;
    path += script;


    // Clear for initial run.
    KeyCombination.erase();


    sigset_t signal_set; // We don't need oldset in this program. You can add it,
    //but it's best to use different sigsets for the second
    //and third argument of sigprocmask.
    sigemptyset ( &signal_set );

    // Block Signals in Python, casue then the BBS doesn't get them.
    sigaddset ( &signal_set, SIGHUP );
    sigaddset ( &signal_set, SIGTERM );
    sigaddset ( &signal_set, SIGINT );
    sigaddset ( &signal_set, SIGILL );
    sigaddset ( &signal_set, SIGABRT );
    sigaddset ( &signal_set, SIGCHLD );
    sigaddset ( &signal_set, SIGQUIT );
    sigaddset ( &signal_set, SIGKILL );
    sigaddset ( &signal_set, SIGWINCH );

    sigprocmask ( SIG_BLOCK, &signal_set, NULL );

    try
    {

        //function should be called before Py_Initialize() to inform
        //the interpreter about paths to Python run-time libraries.
        //Next, the Python interpreter is initialized with Py_Initialize(),
        //followed by the execution of a hard-coded Python script that
        //prints the date and time.

        //Py_SetProgramName(BBSPATH);


        /* Simple Test Code
        PyRun_SimpleString("result = 5 ** 2");

        object module(handle<>(borrowed(PyImport_AddModule("__main__"))));
        object dictionary = module.attr("__dict__");
        object result = dictionary["result"];

        int result_value = extract<int>(result);
        std::cout << result_value << std::endl;

        dictionary["result"] = 20;
        PyRun_SimpleString("print result");

        initbbs_io(); // initialize Pointless

        PyRun_SimpleString("import bbs_io");
        PyRun_SimpleString("print bbs_io.add_five(4)");
        */

        // Testing
        //path += "MyModule.py";

        if ( boost::filesystem::exists ( path.c_str() ) ) // does actually exist?
        {
            //if (boost::filesystem::is_regular_file(path.c_str()))        // is p a regular file?
            //    std::cout << path.c_str() << " size is " << boost::filesystem::file_size(path.c_str()) << endl;
        }
        else
        {
            std::cout << "\n *** ERROR: \n" << path.c_str() << " NOT FOUND!" << endl;
            return;
        }

        _io.errlog2 ( ( char * ) "Starting Boost_Python %s",path.c_str() );

        Py_Initialize();

        initbbs_io(); // initialize Pointless

        object module = import ( "__main__" );
        object name_space = module.attr ( "__dict__" );

        // Setup global variables for script.
        //Only works for wavlues set before scripts, is not dynamic!
        //name_space["KeyCombination"]=KeyCombination;

        object result = exec_file ( path.c_str(), name_space, name_space );

        _io.errlog2 ( ( char * ) "Exiting Boost_Python" );

    }
    catch ( error_already_set const & )
    {
        PyErr_Print();
        /*
        boost::python::object sys(
            boost::python::handle<>(PyImport_ImportModule("sys"))
        );
        boost::python::object err = sys.attr("stderr");
        std::string err_text = boost::python::extract<std::string>(err.attr("getvalue")());
        //MessageBox(0, err_text.c_str(), "Python Error", MB_OK);
        _io.errlog2((char *)err_text.c_str());*/
        _io.errlog2 ( ( char * ) "Python Exception" );

        // Only Exit if a user, not if sysop debuging!
        if ( isSysop == FALSE )
            exit ( 1 );
    }

    _io.errlog2 ( ( char * ) "Finished Boost_Python" );

    // Unblock Signals
    sigprocmask ( SIG_UNBLOCK, &signal_set, NULL );

    //do not call this, causes issue memory leaks in BOOST_PYTHON
    // can't run more anymore scripts if this is called!
    // Py_Finalize();
    return;
}
