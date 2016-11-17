/***************************************************************************
 *   Telnet Daemon Copyright (C) 2004-2016 by Michael Griffin              *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef TELNETDAEMON_H
#define TELNETDAEMON_H

#include "telnet_decoder.hpp"

#include <string>

#include <arpa/inet.h>    // inet_ntoa
#include <netinet/in.h>   // gethostbyaddr
#include <netdb.h>        // gethostbyaddr


class TelnetDaemon
    : public TelnetDecoder
{

public:

    TelnetDaemon();
    ~TelnetDaemon();

    std::string          m_term_passing;

    std::string          m_hostaddr_string;
    std::string          m_hostname_string;
    std::string          m_unknowhost;
    std::string          m_parsed_data;
    int                  m_outbound_fd; // #define OUTFD 1
    int                  m_inbound_fd;  // #define INFD  0

    struct sockaddr_in   m_from;
    socklen_t            m_fromlen;
    struct hostent      *m_hp;

    const int            BUFSIZE = 4096;


    // Main Communication Loops.
    void loop_parent_process(int ptyfd);
    void loop_detection();

    // Start of Methods
    void get_host_info();
    void send_startup_iac();
    void make_uuid(char *path);
    void errlog (char *fmt, ...);

    void load_hosts_deny(std::string ipaddress, std::string hostname);

};

#endif // TELNETDAEMON_H
