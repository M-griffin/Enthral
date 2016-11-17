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

#include "telnet_decoder.hpp"
#include "telnet.hpp"

#include <cstdio>
#include <string>
#include <fstream>


/**
 * @brief Telnet Option Acknowledgment Reply
 * @param cmd
 * @return
 */
unsigned char TelnetDecoder::telnetOptionAcknowledge(unsigned char cmd)
{
    switch(cmd)
    {
    case DO:
        return WILL;
    case WILL:
        return DO;
    case DONT:
        return WONT;
    case WONT:
        return DONT;
    }
    return 0;
}

/**
 * @brief Handle Telnet Option Negotiation
 * @param cmd
 * @return
 */
unsigned char TelnetDecoder::telnetOptionDeny(unsigned char cmd)
{
    switch(cmd)
    {
    case DO:
        return WONT;
    case WILL:
        return DONT;
    case DONT:
        return WONT;
    case WONT:
        return DONT;
    }
    return 0;
}

/**
 * @brief Decoded Sub-option Negotiation from Client.
 */
void TelnetDecoder::decodeBuffer()
{
    //std::cout << "decodeByffer 240 - SE received" << "\r\n";

    // Check if this was a sequence we were waiting for.
    if(checkReply(m_subnegoOption))
    {
        // Remove it
        //del_reply(m_subnegoOption);
    }

    // Now process the buffer per the options
    switch(m_subnegoOption)
    {
    case TELOPT_NAWS:
        m_naws_col = (256 * (unsigned char)data_sequence[0]) + (unsigned char)data_sequence[1];
        m_naws_row = (256 * (unsigned char)data_sequence[2]) + (unsigned char)data_sequence[3];
        //std::cout << "TELOPT_NAWS option: " << m_naws_col << " x " << m_naws_row << "\r\n";
        break;

    case TELOPT_TTYPE:
        m_term_type = data_sequence;
        //std::cout << "TELOPT_TTYPE option: " << m_term_type << "\r\n";
        break;

    case TELOPT_NEW_ENVIRON:
        // Add Variable Or Tokenize here!!
        //std::cout << "TELOPT_NEW_ENVIRON data: " << data_sequence << "\r\n";
        break;

    // Not used at this time.
    case TELOPT_LINEMODE:
        // Add Variable Or Tokenize here!!
        //std::cout << "TELOPT_LINEMODE data: " << data_sequence << "\r\n";
        break;

    default:
        //std::cout << "Invalid option: " << (int) m_subnegoOption << m_subnegoOption << "\r\n";
        break;
    }
    m_teloptStage = 0;
    m_subnegoOption = 0;
}

/**
 * @brief Parse Telnet Commands/Options from Incoming Data.
 *        Also handles responses to clients
 * @param c
 * @return
 */
unsigned char TelnetDecoder::telnetOptionParse(unsigned char c)
{
    // TEL-OPT Parser
    switch(m_teloptStage)
    {
    // Stage 0: Find IAC Sequence Initiator
    case 0:
        if(c != IAC)
        {
            // Pass-through normal text data.
            return c;
        }
        else
        {
            // Start of command sequence. Move to next stage.
            m_teloptStage++;
        }
        break;

    // Stage 1: Find Command
    case 1:
        // Check if Binary Mode is ON, if we get double double IAC, pass through
        if(c == IAC && m_is_binary)
        {
            // If were in Binary Mode, Than IAC IAC = IAC.
            //std::cout << "\r\n Got double IAC BINARY!!\r\n" << "\r\n";
            m_teloptStage = 0;
            return IAC;
        }
        else if (c == IAC)
        {
            // If were NOT in binary mode, (WINDOWS TELNET CONSOLE)
            // Then IAC is sent as double IAC for single command starter!
            //std::cout << "\r\n Got double IAC!!\r\n" << "\r\n";
            //m_teloptStage = 0;
            break;
        }

        // Parse 2 Bytes Commands, No Responses Given.
        switch(c)
        {
        // Most of these are pass through from LINE-MODE which is not used.
        case GA:    //     249        /* you may reverse the line */
        case EL:    //     248        /* erase the current line */
        case EC:    //     247        /* erase the current character */
        case AYT:   //     246        /* are you there */
        case AO:    //     245        /* abort output--but let prog finish */
        case IP:    //     244        /* interrupt process--permanently */
        case BREAK: //     243        /* break */
        case DM:    //     242        /* data mark--for connect. cleaning */
        case NOP:   //     241        /* nop */
        case EOR:   //     239        /* end of record (transparent mode) */
        case ABORT: //     238        /* Abort process */
        case SUSP:  //     237        /* Suspend process */
        case xEOF:  //     236        /* End of file: EOF is already used... */
            // Pass Through commands that don't need Response.
            //std::cout << "[IAC] [" << (int)c << "] PASS-THROUGH" << "\r\n";
            m_teloptStage = 0;
            break;

        // Handle Data Received from Client.
        case SE:    //     240        /* end sub negotiation */
            //std::cout << "[IAC] [" << (int)c << "] SE" << "\r\n";
            decodeBuffer();
            data_sequence.clear();
            m_teloptStage = 0;
            break;

        default:
            //std::cout << "[IAC] [" << (int)c << "] MOVE to 3 BYTE" << "\r\n";
            // Move to 3 Byte Commands
            m_teloptCommand = c;
            m_teloptStage++;
            break;
        }
        break;

    // Stage 2: Parse Commands that need a Reply (3 Byte Sequences)
    case 2:

        //std::cout << "[IAC] [" << (int)c << "] STAGE 2" << "\r\n";

        // Catch if were getting Invalid Option!.
        if(TELCMD_OK(m_teloptCommand))
        {
            //std::cout << "[IAC] [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
        }
        else
        {
            // Hopefully won't get here!
            //std::cout << "INVALID TELNET COMMAND[IAC] [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
            m_teloptStage = 0;
            break;
        }

        switch(m_teloptCommand)
        {
        // DON'T, Only Valid Response is WONT
        case DONT:
            // Handle Flags, if we received them shit it down.
            switch(c)
            {
            case TELOPT_ECHO:
                m_is_echo = false;
                break;
            case TELOPT_BINARY:
                m_is_binary = false;
                break;
            case TELOPT_SGA:
                m_is_sga = false;
                break;
            case TELOPT_LINEMODE:
                m_is_linemode = false;
                break;

            default:
                break;
            }

            //std::cout << "[IAC] RECEIVED DONT [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
            if(!checkReply(c))
            {
                //std::cout << "[IAC] DONT -> WONT [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                sendIACSequences(telnetOptionAcknowledge(m_teloptCommand),c);
            }
            else
            {
                //std::cout << "[IAC] DONT -> REPLY TO RESPONSE! [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                deleteReply(c);
            }
            m_teloptStage = 0;
            break;

        case DO: // Replies WILL / WON'T
            //std::cout << "[IAC] RECEIVED DO [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
            switch(c)
            {
            case TELOPT_ECHO:
                if(!checkReply(c))
                {
                    //std::cout << "[IAC] DO TELOPT_ECHO [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    sendIACSequences(telnetOptionDeny(m_teloptCommand),c);
                }
                else
                {
                    //std::cout << "[IAC] DO TELOPT_ECHO -> REPLY TO RESPONSE! [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    m_is_echo = true;
                    deleteReply(c);
                }
                break;

            case TELOPT_BINARY:
                if(!checkReply(c))
                {
                    //std::cout << "[IAC] DO TELOPT_BINARY [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    sendIACSequences(telnetOptionAcknowledge(m_teloptCommand),c);
                }
                else
                {
                    //std::cout << "[IAC] DO TELOPT_BINARY -> REPLY TO RESPONSE! [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    m_is_binary = true;
                    deleteReply(c);
                }
                break;

            case TELOPT_SGA:
                if(!checkReply(c))
                {
                    //std::cout << "[IAC] DO TELOPT_SGA [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    sendIACSequences(telnetOptionAcknowledge(m_teloptCommand),c);
                }
                else
                {
                    //std::cout << "[IAC] DO TELOPT_SGA -> REPLY TO RESPONSE! [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    m_is_sga = true;
                    deleteReply(c);
                }
                break;

            // Pass Through so we don't respond or keep looping.
            case TELOPT_TTYPE:
            case TELOPT_NAWS:
                break;

            case TELOPT_LINEMODE:
                if(!checkReply(c))
                {
                    //std::cout << "[IAC] DO LINEMODE [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    sendIACSequences(telnetOptionAcknowledge(m_teloptCommand),c);
                }
                else
                {
                    //std::cout << "[IAC] DO LINEMODE -> REPLY TO RESPONSE! [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    m_is_linemode = true;
                    deleteReply(c);
                }
                break;

            default:
                //std::cout << "[IAC] DO -> WONT [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                sendIACSequences(telnetOptionDeny(m_teloptCommand),c);
                break;
            }
            m_teloptStage = 0;
            break;

        // WILL means the Server Will DO IT!
        // We reply Fine, do it!
        case WILL: // Replies DO And DONT
            //std::cout << "[IAC] RECEIVED WILL [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
            // Don't response to WILL Requests.
            switch(c)
            {
            case TELOPT_ECHO:
                if(!checkReply(c))
                {
                    //std::cout << "[IAC] WILL TELOPT_ECHO [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    sendIACSequences(telnetOptionAcknowledge(m_teloptCommand),c);
                }
                else
                {
                    //std::cout << "[IAC] WILL TELOPT_ECHO -> REPLY TO RESPONSE! [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    m_is_echo = true;
                    deleteReply(c);
                }
                break;

            case TELOPT_BINARY:
                if(!checkReply(c))
                {
                    //std::cout << "[IAC] WILL TELOPT_BINARY [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    sendIACSequences(telnetOptionAcknowledge(m_teloptCommand),c);
                }
                else
                {
                    //std::cout << "[IAC] WILL TELOPT_BINARY -> REPLY TO RESPONSE! [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    m_is_binary = true;
                    deleteReply(c);
                }
                break;

            case TELOPT_SGA:
                if(!checkReply(c))
                {
                    //std::cout << "[IAC] WILL TELOPT_SGA [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    sendIACSequences(telnetOptionAcknowledge(m_teloptCommand),c);
                }
                else
                {
                    //std::cout << "[IAC] WILL TELOPT_SGA -> REPLY TO RESPONSE! [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    m_is_sga = true;
                    deleteReply(c);
                }
                break;

            case TELOPT_TTYPE:
                // Ask for Terminal Type from Client!
                //std::cout << "Ask for TTYPE" << "\r\n";
                sendTTYPERequest();
                break;

            // Pass Through so we don't respond or keep looping.
            case TELOPT_NAWS:
                break;

            case TELOPT_NEW_ENVIRON:
                // Locks up Windows Telnet!!?!?!  Work this out later on.
                //sendENVRequest();
                //addReply(TELOPT_NEW_ENVIRON);
                break;

            case TELOPT_LINEMODE:
                if(!checkReply(c))
                {
                    //std::cout << "[IAC] WILL LINEMODE [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    sendIACSequences(telnetOptionAcknowledge(m_teloptCommand),c);
                }
                else
                {
                    //std::cout << "[IAC] WILL LINEMODE -> REPLY TO RESPONSE! [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                    m_is_linemode = true;
                    deleteReply(c);
                }
                break;

            default :
                //std::cout << "[IAC] WILL -> DONT [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                sendIACSequences(telnetOptionDeny(m_teloptCommand),c);
                break;
            }
            m_teloptStage = 0;
            break;

        // WON'T, Only Valid Response is DONT
        case WONT:
            // Handle Flags, if we received them shit it down.
            switch(c)
            {
            case TELOPT_ECHO:
                m_is_echo = false;
                break;
            case TELOPT_BINARY:
                m_is_binary = false;
                break;
            case TELOPT_SGA:
                m_is_sga = false;
                break;
            case TELOPT_LINEMODE:
                m_is_linemode = false;
                break;

            default:
                break;
            }

            if(!checkReply(c))
            {
                //std::cout << "[IAC] RECEIVED WONT [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                sendIACSequences(telnetOptionAcknowledge(m_teloptCommand), c);
            }
            else
            {
                //std::cout << "[IAC] RECEIVED WONT -> REPLY TO RESPONSE! [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                deleteReply(c);
            }
            m_teloptStage = 0;
            break;

        // Start of Sub Negotiations and Stages 3 - 4
        case SB: // 250
            //std::cout << " SB!!! [IAC] TELNET_STATE_SB [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";

            // Setup the Option when we get SE
            // So we know how to process the buffer when it ends.
            m_subnegoOption = c;

            if(c == TELOPT_TTYPE)
            {
                m_currentOption = c;
                m_teloptStage = 3;
            }
            else if(c == TELOPT_NEW_ENVIRON)
            {
                m_currentOption = c;
                m_teloptStage = 3;
            }
            else if(c == TELOPT_NAWS)
            {
                m_currentOption = c;
                m_teloptStage = 5;
            }
            else if(c == TELOPT_LINEMODE)
            {
                m_currentOption = c;
                m_teloptStage = 7;
            }
            else
            {
                //std::cout << "[IAC] TELNET_STATE_SB UNSUPPORTED [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
                // Invalid, reset back.
                m_teloptStage = 0;
            }
            break;

        default:
            // Options or Commands Not Parsed, RESET.
            //std::cout << "[IAC] INVALID 3 Options [" << (int)m_teloptCommand << "] [" << (int)c << "]" << "\r\n";
            m_teloptStage = 0;
            break;
        }
        break;

    case 3:
        //std::cout << "--> STAGE 3 [" << (int)c << "]" << "\r\n";
        //Options will be 1 After SB
        switch(m_currentOption)
        {
        // IAC SB TTYPE TELQUAL_IS
        case TELOPT_TTYPE:
            if(c == TELQUAL_IS)
            {
                //std::cout << "[IAC] TELQUAL_IS [" << (int)m_currentOption << "] [" << (int)c << "]" << "\r\n";
                m_teloptStage = 4;
            }
            else
                m_teloptStage = 0;
            break;

        case TELOPT_NEW_ENVIRON:
            if(c == TELQUAL_IS)
            {
                //std::cout << "[IAC] TELQUAL_IS [" << (int)m_currentOption << "] [" << (int)c << "]" << "\r\n";
                m_teloptStage = 6;
            }
            else
                m_teloptStage = 0;
            break;

        default:
            //printf("\r\n [Stage 3 - unregistered stuff it] - %i, %i \r\n",opt, c);
            if(c == SE)
            {
                //std::cout << "[IAC] SB END [" << (int)m_currentOption << "] [" << (int)c << "]" << "\r\n";
                m_teloptStage = 0;
            }
            else
            {
                // reset
                m_teloptStage = 0;
            }
            break;
        }
        break;

    // Only Gets here on TTYPE Sub-Negotiation.
    case 4:
        //std::cout << "--> STAGE 4 TTYPE [" << (int)c << "]" << "\r\n";
        if(c != IAC && c != SE)
        {
            if(c == '\x00' || c == '\0')
                data_sequence += '\0';
            else
                data_sequence += static_cast<char>(c);

            if (data_sequence.size() >= SB_MAXLEN)
            {
                // Invalid Sequences,, just clear.
                data_sequence.clear();
            }
        }

        // check for End of Sequence IAC SE.
        if(c == IAC)
        {
            // IAC then 240 to close sequence for TTYPE
            m_teloptStage = 1;
        }
        /*
        else if (c == SE)
        {
            //data_sequence.clear();
            m_teloptStage = 0;
        }*/
        break;

    case 5:
        //std::cout << "--> STAGE 5 NAWS [" << (int)c << "]" << "\r\n";

        if(c != IAC && c != SE)
        {
            if(c == '\x00' || c == '\0')
                data_sequence += '\0';
            else
                data_sequence += static_cast<char>(c);

            if (data_sequence.size() >= SB_MAXLEN)
            {
                // Invalid Sequences,, just clear.
                data_sequence.clear();
            }
        }

        // check for End of Sequence IAC SE.
        if(c == IAC)
        {
            // IAC then 240 to close sequence for NAWS
            m_teloptStage = 1;
        }
        /*
        else if (c == SE)
        {
            //data_sequence.clear();
            m_teloptStage = 0;
        }*/
        break;

    case 6:
        //std::cout << "--> STAGE 6 TELOPT_NEW_ENVIRON [" << (int)c << "]" << "\r\n";

        if(c != IAC && c != SE)
        {
            if(c == '\x00' || c == '\0')
                data_sequence += ' ';
            else
                data_sequence += static_cast<char>(c);

            if (data_sequence.size() >= SB_MAXLEN)
            {
                // Invalid Sequences,, just clear.
                data_sequence.clear();
                m_teloptStage = 0;
            }
        }

        // check for End of Sequence IAC SE.
        if(c == IAC)
        {
            // IAC then 240 to close sequence for NAWS
        }
        else if (c == SE)
        {
            //data_sequence.clear();
            m_teloptStage = 0;
        }
        break;

    case 7:
        /*
        * Right now were just parsing out the data received
        * so we can reply back WON'T so it will toggle LINEMODE off!
        * LINUX/BSD console telnet's default to LINEMODE ON, and ignore
        * DONT and WONT requests until we initiate the discussion with DO.
        * Then we we received this Sub Options, which we need to parse out.
        *
        * Notes: Add some Extra Variables.  If Binary, We need to parse
        * and ignore double IAC!  they are Unicode chars.
        * then only look for IAC / 240 for SE to exit.
        */
        //std::cout << "--> STAGE 7 TELOPT_LINEMODE [" << (int)c << "]" << "\r\n";

        if(c != IAC && c != SE)
        {
            data_sequence += static_cast<char>(c);
            if (data_sequence.size() >= SB_MAXLEN)
            {
                // Invalid Sequences,, just clear.
                data_sequence.clear();
            }
        }

        // check for End of Sequence IAC SE.
        if(c == IAC)
        {
            // IAC then 240 to close sequence for NAWS
        }
        else if (c == SE)
        {
            m_teloptStage = 0;
            data_sequence.erase();
            // All Done, Now it's ok to turn off LINEMODE!
            sendIACSequences(telnetOptionDeny(WONT), TELOPT_LINEMODE);
        }
        break;
    }

    // No no valid Data, Return null character which are ignored.
    return '\0';
}
