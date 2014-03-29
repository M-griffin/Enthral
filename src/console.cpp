/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose: Working on IPC Node Chat                                     *
 *                                                                         *
 *   Node Messaging Modeled after Day Dream particially.                   *
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

#include "console.h"
#include "struct.h"

#include <cstdio>
#include <cstdlib>


#include <errno.h>
#include <fcntl.h>
#include <signal.h>


#include <sys/types.h>  //mkfifo gcc 3.x
#include <sys/stat.h>   //mkfifo gcc 3.x

#include <sys/socket.h>
#include <sys/un.h>

//#include <unistd.h>
//#include <syslog.h>
//#include <sys/ioctl.h>
#include <termios.h>
//#include <sys/wait.h>

// Unicode Output Encoding.
#include <iostream> // cout
#include <clocale>  // locale
#include <cwchar>   // wchar_t wide characters
#include <string>   // string and wstring
#include <fstream>  // ifstream file streams.

std::string screen_buffer;

/**
 * Wijnand Modderman-Lenstra
 * CP437 is a tuple of length 256, mapping equivalent unicode
 * characters for each CP437(n), where n is index of CP437.
 */
wchar_t CP437TABLE[] = {
    L'\u0000', L'\u0001', L'\u0002', L'\u0003', L'\u0004', L'\u0005',
    L'\u0006', L'\u0007', L'\u0008', L'\u0009', L'\u000A', L'\u000B',
    L'\u000C', L'\u000D', L'\u000E', L'\u000F', L'\u0010', L'\u0011',
    L'\u0012', L'\u0013', L'\u0014', L'\u0015', L'\u0016', L'\u0017',
    L'\u0018', L'\u0019', L'\u001A', L'\u001B', L'\u001C', L'\u001D',
    L'\u001E', L'\u001F', L'\u0020', L'\u0021', L'\u0022', L'\u0023',
    L'\u0024', L'\u0025', L'\u0026', L'\u0027', L'\u0028', L'\u0029',
    L'\u002A', L'\u002B', L'\u002C', L'\u002D', L'\u002E', L'\u002F',
    L'\u0030', L'\u0031', L'\u0032', L'\u0033', L'\u0034', L'\u0035',
    L'\u0036', L'\u0037', L'\u0038', L'\u0039', L'\u003A', L'\u003B',
    L'\u003C', L'\u003D', L'\u003E', L'\u003F', L'\u0040', L'\u0041',
    L'\u0042', L'\u0043', L'\u0044', L'\u0045', L'\u0046', L'\u0047',
    L'\u0048', L'\u0049', L'\u004A', L'\u004B', L'\u004C', L'\u004D',
    L'\u004E', L'\u004F', L'\u0050', L'\u0051', L'\u0052', L'\u0053',
    L'\u0054', L'\u0055', L'\u0056', L'\u0057', L'\u0058', L'\u0059',
    L'\u005A', L'\u005B', L'\u005C', L'\u005D', L'\u005E', L'\u005F',
    L'\u0060', L'\u0061', L'\u0062', L'\u0063', L'\u0064', L'\u0065',
    L'\u0066', L'\u0067', L'\u0068', L'\u0069', L'\u006A', L'\u006B',
    L'\u006C', L'\u006D', L'\u006E', L'\u006F', L'\u0070', L'\u0071',
    L'\u0072', L'\u0073', L'\u0074', L'\u0075', L'\u0076', L'\u0077',
    L'\u0078', L'\u0079', L'\u007A', L'\u007B', L'\u007C', L'\u007D',
    L'\u007E', L'\u007F', L'\u00C7', L'\u00FC', L'\u00E9', L'\u00E2',
    L'\u00E4', L'\u00E0', L'\u00E5', L'\u00E7', L'\u00EA', L'\u00EB',
    L'\u00E8', L'\u00EF', L'\u00EE', L'\u00EC', L'\u00C4', L'\u00C5',
    L'\u00C9', L'\u00E6', L'\u00C6', L'\u00F4', L'\u00F6', L'\u00F2',
    L'\u00FB', L'\u00F9', L'\u00FF', L'\u00D6', L'\u00DC', L'\u00A2',
    L'\u00A3', L'\u00A5', L'\u20A7', L'\u0192', L'\u00E1', L'\u00ED',
    L'\u00F3', L'\u00FA', L'\u00F1', L'\u00D1', L'\u00AA', L'\u00BA',
    L'\u00BF', L'\u2310', L'\u00AC', L'\u00BD', L'\u00BC', L'\u00A1',
    L'\u00AB', L'\u00BB', L'\u2591', L'\u2592', L'\u2593', L'\u2502',
    L'\u2524', L'\u2561', L'\u2562', L'\u2556', L'\u2555', L'\u2563',
    L'\u2551', L'\u2557', L'\u255D', L'\u255C', L'\u255B', L'\u2510',
    L'\u2514', L'\u2534', L'\u252C', L'\u251C', L'\u2500', L'\u253C',
    L'\u255E', L'\u255F', L'\u255A', L'\u2554', L'\u2569', L'\u2566',
    L'\u2560', L'\u2550', L'\u256C', L'\u2567', L'\u2568', L'\u2564',
    L'\u2565', L'\u2559', L'\u2558', L'\u2552', L'\u2553', L'\u256B',
    L'\u256A', L'\u2518', L'\u250C', L'\u2588', L'\u2584', L'\u258C',
    L'\u2590', L'\u2580', L'\u03B1', L'\u00DF', L'\u0393', L'\u03C0',
    L'\u03A3', L'\u03C3', L'\u00B5', L'\u03C4', L'\u03A6', L'\u0398',
    L'\u03A9', L'\u03B4', L'\u221E', L'\u03C6', L'\u03B5', L'\u2229',
    L'\u2261', L'\u00B1', L'\u2265', L'\u2264', L'\u2320', L'\u2321',
    L'\u00F7', L'\u2248', L'\u00B0', L'\u2219', L'\u00B7', L'\u221A',
    L'\u207F', L'\u00B2', L'\u25A0', L'\u00A0' };


static int conin = 0;
static int conout = 0;
static int conon = 0;
static int dummyfd = 0;

int sockfd;
int serhandle;
struct List *olms;

static struct termios oldtty;
static struct sockaddr_un sock;

/**
 * Create Internode PIPE Sockets for Node Communication
 */
static void create_internode_socket()
{

    char socket_name[4096] = {0};
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr,"%s ***cannot create communication socket, check permissions!",ENTHRALTMP);
    }
    snprintf(socket_name, sizeof socket_name, "%s/enthral_sock%d", ENTHRALTMP, NODE_NUM);
    unlink(socket_name);

    strncpy(sock.sun_path, socket_name, sizeof sock.sun_path);
    sock.sun_path[sizeof sock.sun_path - 1] = 0;
    sock.sun_family = AF_UNIX;
    if (bind(sockfd, (struct sockaddr *) &sock, sizeof sock) < 0)
    {
        fprintf(stderr,"%s ***cannot bind communication socket, check permissions!",ENTHRALTMP);
        close(sockfd);
    }
}

/**
 * Clear Node Files on System Exit.
 */
void clear_nodes()
{

    char buff[512] = {0};

    tcsetattr(0, TCSANOW, &oldtty);

    finalize_console();
    snprintf(buff, sizeof(buff), "%s/enthral%dr", ENTHRALTMP, NODE_NUM);
    unlink(buff);
    close(sockfd);
    snprintf(buff, sizeof(buff), "%s/enthral_sock%d", ENTHRALTMP, NODE_NUM);
    unlink(buff);
    snprintf(buff, sizeof(buff), "%s/enthral%dw", ENTHRALTMP, NODE_NUM);
    unlink(buff);
    snprintf(buff, sizeof(buff), "%s/nodeinfo%d.data", ENTHRALTMP, NODE_NUM);
    unlink(buff);

	// Clear Terminal file from Telnetd
	snprintf(buff, sizeof(buff), "%s", CLIENT_TERM);
    remove(buff);
	
}


/**
 * Initalize And Create Node Sockets 
 */
int init_nodes()
{

    serhandle = open(ttyname(0), O_RDWR);

    // Create communication fifos for Snoop
    create_internode_socket();
    if (init_console() == -1)
    {
        fprintf(stderr,"%s ***communication socket(s) failed to init, check permissions!",ENTHRALTMP);		
    }

    //atexit(clear_nodes);
    return 1;
}

/**
 * Set block on Nodes
 */
int set_blocking_mode(int fd, int mode)
{

    int fl;
    if ((fl = fcntl(fd, F_GETFL)) == -1)
        return -1;
    fl &= ~O_NONBLOCK;
    return fcntl(fd, F_SETFL, fl | (mode ? O_NONBLOCK : 0));
}

/**
 * Read from Console/Node socket
 */
ssize_t safe_read(int fd, void *buf, size_t buflen)
{

    char *p;
    ssize_t bread, n;

    bread = 0;
    p = (char *) buf;
    while (buflen)
    {
        if ((n = read(fd, p, buflen)) == -1)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        p += n;
        bread += n;
        buflen -= n;
    }

    return bread;
}

/**
 * Write Console/Node socket
 */
ssize_t safe_write(int fd, const void *buf, size_t buflen)
{

    char *p;
    ssize_t bwrite, n;

    bwrite = 0;
    p = (char *) buf;
    while (buflen)
    {
        if ((n = write(fd, p, buflen)) == -1)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        p += n;
        bwrite += n;
        buflen -= n;
    }

    return bwrite;
}

/**
 * Check if console is blocking or Not.
 */
int console_active()
{
    return conon != 0;
}

/**
 * Initalize console node pipe i/o
 */
int init_console()
{

    char buffer[4096] = {0};
    struct sigaction sigact;
    sigset_t sigset;

    /* Writing to pipe with no readers on the other side raises
     * SIGPIPE and its default action is to terminate the program.
     */
    sigact.sa_handler = SIG_IGN;
    sigemptyset(&sigset);
    sigact.sa_mask = sigset;
    sigact.sa_flags = SA_RESTART;
    if (sigaction(SIGPIPE, &sigact, NULL) == -1)
        abort();

    snprintf(buffer, sizeof buffer, "%s/enthral%dw", ENTHRALTMP, NODE_NUM);
    unlink(buffer);
    if (mkfifo(buffer, 0777) == -1)
    {
        //syslog(LOG_ERR, "cannot mkfifo(\"%.200s\"): %m", buffer);
        fprintf(stderr,"%s Cannot create communication FIFO\r\n",ENTHRALTMP);
        exit(1);
    }

    /* We must first open the writing FIFO as O_RDONLY, if we're
     * ever going to get it opened as O_WRONLY. Without this trick
     * open() would always return ENXIO.
     *
     * We can still know whether there are no readers on the other
     * side of pipe, since safe_write() will return EPIPE on such case.
     */
    if ((dummyfd = open(buffer, O_RDONLY | O_NONBLOCK)) == -1 ||
            (conout = open(buffer, O_WRONLY | O_NONBLOCK)) == -1)
        abort();

    close(dummyfd);
    set_blocking_mode(conout, 0);

    snprintf(buffer, sizeof(buffer), "%s/enthral%dr", ENTHRALTMP, NODE_NUM);
    unlink(buffer);
    if (mkfifo(buffer, 0777) == -1)
    {
        fprintf(stderr,"%s ***cannot create communication FIFO socket (Nodes), check permissions!",ENTHRALTMP);
        exit(1);
    }

    /* Opening a FIFO for O_RDONLY in non blocking mode should
     * work on all systems.
     */
    if ((conin = open(buffer, O_RDONLY | O_NONBLOCK)) == -1 ||
            (dummyfd = open(buffer, O_WRONLY | O_NONBLOCK)) == -1)
        abort();

    set_blocking_mode(conin, 0);

    return 0;
}

/**
 * Finialize console node pipe i/o
 */
void finalize_console()
{
    close(conin);
    close(conout);
    close(dummyfd);
    conon = 0;
}

/**
 * Open descriptor
 */
void open_console()
{

    if (conon != 2)
        conon = 1;
}

/**
 * Close descriptor
 */
void close_console()
{

    if (conon != 2)
        conon = 0;
}

/**
 * Select where to get input from, Console / Node
 */
int console_select_input(int maxfd, fd_set *set)
{

    FD_SET(conin, set);
    return maxfd < conin ? conin : maxfd;
}

/**
 * Check for waiting Data
 */
int console_pending_input(fd_set *set)
{
    return FD_ISSET(conin, set);
}

/**
 * Get single Character Input
 */
int console_getc(void)
{
    char ch;
    switch (read(conin, &ch, 1))
    {
    case 0:
        return EOF;
    case -1:
        if (errno == EPIPE)
        {
            conon = 0;
            return EOF;
        }
        return -1;
    default:
        return ch;
    }
}


/**
 * Used for printing multibyte (UTF-8) Characters
 * To Console / STDOUT
 */
void print_wide(const std::wstring& wstr)
{
    std::mbstate_t state = std::mbstate_t();
    for(wchar_t wc : wstr)
    {
        std::string mb(MB_CUR_MAX, '\0');
        int ret = std::wcrtomb(&mb[0], wc, &state);
		if ((ret == 0) || (ret > MB_CUR_MAX)) 
			break;

		// Skip any Trailing / Embedded null from Wide -> multibtye
		// Conversion, don't send NULL's to the screen. 
		for(char ch: mb)
		{
			if (ch != '\0')
				std::cout << ch << flush;
		}
    }
}


/**
 * Main Translation loop from cp437 to Wide Unicode.
 * 
 */
void cp437toUTF8(std::string cp347)
{
	std::wstring wstr;
	int ascii_value = 0;

	// Loop and wirte out after translation to UTF-8
	for (int i = 0; i < (signed)cp347.size(); i++)
	{
		ascii_value = std::char_traits<char>().to_int_type(cp347[i]);
		wstr = CP437TABLE[ascii_value];
		print_wide(wstr); // Normal UTF8 Output		
    }
}


/**
 * Main call to write output to console
 * 
 */
int console_putsn(char *str, size_t n, int buffering)
{
    int writecnt = 0;

	/*
	 * New UTF8- Translation Code Here.  
	 * If UTF8 is Active, translate all output to Unicode
	 * And Use Wide Char Array and Output function for local display only
	 * Node Spy should do this also!
	 */

	std::string::size_type id1 = 0;
	std::string myCP437;
	myCP437 = static_cast<char *>(str);

	// Find Most Recent Screen Clear, Restart buffer fresh
	// Otherwise Keep appending to the buffer
	if (buffering)
	{
		id1 = myCP437.rfind("\x1b[2J",myCP437.size()-1);
		if (id1 != std::string::npos)
		{
			screen_buffer.erase();
			screen_buffer = myCP437.substr(id1);
		}
		else 
			screen_buffer += myCP437;
	}
	
	
	if (UTF8Output)
	{		
		cp437toUTF8(myCP437);
	}
	else
	{
    	// Write to console first for Telnet Users Connection
    	writecnt = write(1,(char*)str,n); //- - Normal Output for Connection.
	}
	
    // Safe Write to Sockets for Snooping IPC Connections locally.
    // - Local connection For snoop sysop utils.
    writecnt = safe_write(conout, str, n);
    if (writecnt == -1 && errno == EPIPE)
    {
        return 0;
    }
    else
    {
        return writecnt;
    }
}
