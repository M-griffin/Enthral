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

#include "telnet_daemon.hpp"
#include "telnet_decoder.hpp"
#include "config.hpp"

#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <syslog.h>        //  syslogd
#include <uuid/uuid.h>

TelnetDaemon::TelnetDaemon()
    : TelnetDecoder(1)
    , m_term_passing("/tmp/enthral/")
    , m_hostaddr_string("")
    , m_hostname_string("")
    , m_unknowhost("undetected")
    , m_parsed_data("")
    , m_outbound_fd(1)
    , m_inbound_fd(0)
{
    //std::cout << "TelnetDaemon" << std::endl;
}

TelnetDaemon::~TelnetDaemon()
{
    //std::cout << "~TelnetDaemon" << std::endl;
}

/**
 * Main Parent Loop handles passing data beeween the child process
 * and the connected client socket.
 * Blocking so Select 0 or -1 is error
 */
void TelnetDaemon::loop_parent_process(int ptyfd)
{
    bool isConnected = true;

    int selret;
    fd_set rdfdset;

    char character_buffer[BUFSIZE];
    unsigned char ch = 0;

    // Clear any previous buffered data.
    m_parsed_data.clear();

    do
    {
        FD_ZERO(&rdfdset);
        FD_SET(ptyfd, &rdfdset);
        FD_SET(m_inbound_fd, &rdfdset);

        selret = select(ptyfd + 1, &rdfdset, NULL, NULL, NULL);

        // Main Blocking Select Statement
        if (selret <= 0)
        {
            errlog((char *)"1 select %i!", selret);
            isConnected = false;
            break;
        }

        if (FD_ISSET(ptyfd, &rdfdset))
        {
            memset(&character_buffer, 0, BUFSIZE);
            selret = read(ptyfd, character_buffer, BUFSIZE / 2);

            if (selret <= 0)
            {
                errlog((char *)"2 select %i!", selret);
                isConnected = false;
                break;
            }

            if(write(m_outbound_fd, character_buffer, selret) == -1)
            {
                errlog((char *)"1 write m_outbound_fd break!");
                isConnected = false;
                break;
            }
        }

        if (FD_ISSET(m_inbound_fd, &rdfdset))
        {
            memset(&character_buffer, 0, BUFSIZE);
            selret = read(m_inbound_fd, character_buffer, BUFSIZE);

            if (selret <= 0)
            {
                errlog((char *)"3 select %i!", selret);
                isConnected = false;
                break;
            }

            for(unsigned char c : character_buffer)
            {
                try
                {
                    // Enter the Telnet_State and handle parsing options.
                    ch = telnetOptionParse(c);
                }
                catch(std::exception& e)
                {
                    //std::cout << "Exception telnet_process_char: " << e.what() << std::endl;
                    errlog((char *)"Exception telnet_process_char");
                }

                // Skip any incoming nulls, nulls are also return on Telnet options received
                // So we know that there is no valid text data to send to the client.
                if(ch == '\0')
                {
                    continue;
                }

                // Incoming Buffer is filled and Telnet options are parsed out.
                m_parsed_data += ch;
            }

            if(write(ptyfd, (char *)m_parsed_data.c_str(), m_parsed_data.size()) == -1)
            {
                errlog((char *)"1 write ptyfd break!");
                isConnected = false;
                break;
            }

            m_parsed_data.clear();
        }

    }
    while (isConnected);
}

/**
 * IAC Detection loop, pre-loading to determine the clients
 * Terminal Type, and Screen Size if passed.
 * Non-Blocking with Timeout, so select can be 0.
 */
void TelnetDaemon::loop_detection()
{
    // Sleep for 1 seconds to let the initial response flow in
    sleep(1);

    time_t start = time(0);

    // Blocking loop ever second for for get terminal detection
    struct timeval value;
    value.tv_sec = 0;
    value.tv_usec = 10000;


    // This Loop is to catch all returned TelnetOPT negoiations
    // Were pass-through data to Detect the TERM and NAWS etc.
    bool isWaitingTerm = true;

    // Socket Helpers
    int selret;
    fd_set rdfdset;
    char character_buffer[BUFSIZE];
    unsigned char ch = 0;

    double diff = 0.0;

    do
    {
        FD_ZERO(&rdfdset);
        FD_SET(m_inbound_fd, &rdfdset);

        // Main BLocking with (1) Second Delay
        selret = select(m_inbound_fd + 1, &rdfdset, NULL, NULL, &value);

        // Error / Lost Connection on
        if (selret < 0)
        {
            errlog((char *)"1 select %i!", selret);
            isWaitingTerm = false;
            break;
        }

        if (FD_ISSET(m_inbound_fd, &rdfdset))
        {
            // Read incoming Socket
            memset(&character_buffer, 0, BUFSIZE);
            selret = read(m_inbound_fd, character_buffer, BUFSIZE);
            if (selret <= 0)
            {
                errlog((char *)"2 select %i!", selret);
                isWaitingTerm = false;
                break;
            }

            for(unsigned char c : character_buffer)
            {
                try
                {
                    // Enter the Telnet_State and handle parsing options.
                    ch = telnetOptionParse(c);
                }
                catch(std::exception& e)
                {
                    //std::cout << "Exception telnet_process_char: " << e.what() << std::endl;
                    errlog((char *)"Exception telnet_process_char");
                }

                // Skip any incoming nulls, nulls are also return on Telnet options received
                // So we know that there is no valid text data to send to the client.
                if(ch == '\0')
                {
                    continue;
                }

                // Incoming Buffer is filled and Telnet options are parsed out.
                m_parsed_data += ch;
            }
        }


        diff = difftime( time(0), start);
        //errlog((char *)"Diff: %f", diff);

        // Give (3) Seconds for negoiations to complete
        if (diff > 2.0)
        {
            errlog((char *)"Detection Delay Completed.");
            m_parsed_data.clear();
            isWaitingTerm = false;
            break;
        }

    }
    while (isWaitingTerm);
}

/**
 * Reads Client Host Info from passed socket
 *
 */
void TelnetDaemon::get_host_info()
{
    // Resolve IP Address as 1st Passed Argument.
    m_fromlen = sizeof(m_from);
    if (getpeername(0, (struct sockaddr *)&m_from, &m_fromlen) < 0)
    {
        //syslog(LOG_INFO, "Enthral in.telnetd [getpeername] error!");
        m_hostaddr_string = "localhost.unknown";
        m_unknowhost = true;
    }
    else
    {
        m_hostaddr_string = inet_ntoa(m_from.sin_addr);
        errlog((char *)"Connection IP From: %s", (char *)m_hostaddr_string.c_str());
    }

    // Get hostname of conenction.
    m_hp = gethostbyaddr((char *)&m_from.sin_addr, sizeof(struct in_addr), m_from.sin_family);
    if (m_hp)
    {
        m_hostname_string = m_hp->h_name;
    }
    else
    {
        if (toupper(BLOCK_NO_HOSTNAME) == 'T')
        {
            syslog(LOG_INFO, "Enthral in.telnetd [gethostbyaddr] failed!");
            errlog((char *)"Enthral in.telnetd [gethostbyaddr] failed! IP: %s", (char *)m_hostaddr_string.c_str());

            if (write(m_outbound_fd, "\r\nUnable to resolve hostname, connection dropped due to connection abuse.\r\n", 80 ) == -1)
            {
                //std::cout << "Unable to write to socket" << std::endl;
                errlog((char *)"Unable to write to socket");
            }
            exit(1);
        }
        else
        {
            m_hostname_string = "hostname.unknown";
            m_unknowhost = true;
        }
    }

    // Check for Blocked IP Addresses, if found then exit!
    errlog((char *)"---------------------------------------------");
    errlog((char *)"Connection Hostname From: %s", (char *)m_hostname_string.c_str());
    if (toupper(USE_HOSTS_DENY) == 'T')
    {
        load_hosts_deny(m_hostaddr_string, m_hostname_string);
    }
}

/**
 * Sends Initial Server IAC Sequences to Client.
 *
 */
void TelnetDaemon::send_startup_iac()
{
    sendIACSequences(DONT, TELOPT_OLD_ENVIRON);

    sendIACSequences(DO, TELOPT_SGA);
    addReply(TELOPT_SGA);

    sendIACSequences(WILL, TELOPT_ECHO);
    addReply(TELOPT_ECHO);

    sendIACSequences(WILL, TELOPT_SGA);
    addReply(TELOPT_SGA);

    sendIACSequences(WILL, TELOPT_BINARY);
    addReply(TELOPT_BINARY);

    sendIACSequences(DO, TELOPT_BINARY);
    addReply(TELOPT_BINARY);

    sendIACSequences(DO, TELOPT_TTYPE);
    addReply(TELOPT_TTYPE);

    sendIACSequences(DO, TELOPT_NAWS);
    addReply(TELOPT_NAWS);
}

/**
 * Used To created Unique filename passed to BBS server
 * Holds detected terminal emulation for each node spawned.
 */
void TelnetDaemon::make_uuid(char *path)
{
    uuid_t id;
    uuid_generate(id);

    char *string = new char[100];
    uuid_unparse(id, string);

    // Add the path, then append the UUID.
    std::string tmp = path;
    tmp.append(string);

    delete string;
    m_term_passing = std::move(tmp);
}

/**
 * Errlog is used for Easy testing and debugging.
 * Writes Message to a File.
 */
void TelnetDaemon::errlog (char *fmt, ...)
{
    char logfile[1024]= {0};
    va_list arg;
    time_t t;
    tm *tm;
    char buffer[2048]= {0},datestr[81]= {0},buf2[2100]= {0};

    if (toupper(USE_FILE_LOG) == 'T')
    {
        sprintf(logfile,"%sentelnetd.log",PATH);
        va_start(arg, fmt);
        vsprintf(buffer, fmt, arg);
        va_end(arg);

        t  = time(NULL);
        tm = localtime(&t);
        strftime(datestr,81,"%Y/%m/%d %H:%M:%S",tm);
        sprintf(buf2,"\n%s : %s",datestr,buffer);

        FILE *fp;
        fp = fopen(logfile,"at");
        if(fp == NULL) fp = fopen(logfile,"wt");
        if(fp != NULL)
        {
            fputs(buf2,fp);
            fclose(fp);
        }
        else
        {
            syslog(LOG_INFO, "Enthral in.telnetd: Unable to Open/create log file");
        }
    }
}

/**
 * Load hosts.deny block file for Offending IP and HostNames.
 * Can add stright IP, or HostName without Wildcard *,
 */
void TelnetDaemon::load_hosts_deny(std::string ipaddress, std::string hostname)
{
    errlog((char *)"Loading Hosts.Deny");
    std::string badhost;
    std::ifstream in;
    std::string::size_type id1;
    in.open(HOSTS_DENY_PATH);
    if (in.is_open())
    {
        for (;;)
        {
            if(in.eof()) break;
            std::getline(in,badhost,'\n');

            if (badhost.size() != 0)
            {
                // First Check for Bad IP Address
                if( badhost == ipaddress)
                {
                    syslog(LOG_INFO, "Enthral in.telnetd [IP %s] Blocked IP Address Found.", ipaddress.c_str() );
                    errlog((char *)"Blocked IP Address Found, Dropping connection %s", ipaddress.c_str() );

                    if (write(m_outbound_fd, "\r\nIP/Hostname Blocked due to connection abuse.\r\n", 52 ) == -1)
                    {
                        //std::cout << "Unable to write to socket" << std::endl;
                        errlog((char *)"Unable to write to socket");
                    }

                    closelog();
                    in.close();
                    exit(0);
                }
                else
                {
                    //Check if hostname WildCard is blocked.
                    id1 = hostname.find(badhost,0);
                    if (id1 != std::string::npos)
                    {
                        syslog(LOG_INFO, "Enthral in.telnetd [IP %s, HOST %s] Blocked by Hostname.", ipaddress.c_str(), hostname.c_str() );
                        errlog((char *)"Blocked IP Hostname Found, Dropping connection %s - %s", ipaddress.c_str(), hostname.c_str());

                        if (write(m_outbound_fd, "\r\nIP/Hostname Blocked due to connection abuse.\r\n", 52) == -1)
                        {
                            //std::cout << "Unable to write to socket" << std::endl;
                            errlog((char *)"Unable to write to socket");
                        }

                        closelog();
                        in.close();
                        exit(0);
                    }

                }
            }
            if(in.eof()) break;
        }
        in.close();
    }
}

