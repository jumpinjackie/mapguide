//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "MapGuideCommon.h"
#include "ServerConnectionImp.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgServerConnectionImp::MgServerConnectionImp() : mServer(NULL)
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgServerConnectionImp::~MgServerConnectionImp()
{
    if (mServer != NULL)
    {
        Disconnect();
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Establishes a connection to a Mg server
/// </summary>
///
/// <param name="targetName">
/// Ip address for Mg server machine
/// </param>
/// <param name="portno">
/// port# to use
/// </param>
/// <returns>
/// true -  on successful connection
/// false - could not connect
/// </returns>

bool MgServerConnectionImp::Connect(const char* ipAddress, int portno)
{
    if (mServer)
    {
        Disconnect();
    }

    // Create the ACE stream
    mServer = new ACE_SOCK_Stream();

    ACE_INET_Addr serverAddr(portno, ipAddress);

    ACE_SOCK_Connector connector;

    // Server connection backlog could be overrun during high connections.
    // Retry 5 times with a short delay between each attempt.
    const ACE_Time_Value timeout(2); // 2 seconds
    int nRetries = 5;
    int connectResult = -1;

    while (nRetries > 0
        && -1 == (connectResult = connector.connect(*mServer, serverAddr, &timeout)))
    {
        int status = ACE_OS::set_errno_to_wsa_last_error();

        switch (status)
        {
            // Retry on these.
            case ETIMEDOUT:
            case ECONNRESET:
            case EAGAIN:
                --nRetries;
                break;

            // Bail immediately on any other.
            // case ECONNREFUSED:
            // case EHOSTDOWN:
            // case EHOSTUNREACH:
            // case ENETDOWN:
            // case ENETUNREACH:
            case ENOTCONN: // most likely
            default:
                nRetries = 0;
                break;
        }
    }

    if (-1 == connectResult)
    {
        int err = ACE_OS::last_error();
        MG_UNUSED_ARG(err);
        // Could not connect
        delete mServer;
        mServer = NULL;
        return false;
    }


    // TODO: Send the Test request to make sure there is real Mg server

    // Connected fine
    return true;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Disconnects from Mg server
/// </summary>
void MgServerConnectionImp::Disconnect()
{
    if (mServer != NULL)
    {
        // Send a control packet to the server to dump the connection early
        Ptr<MgAceStreamHelper> streamHelper = new MgAceStreamHelper(mServer->get_handle());
        MgStream stream(streamHelper);

        MgControlPacket packet;
        packet.m_PacketHeader = MgPacketParser::mphControl;
        packet.m_PacketVersion = 1;
        packet.m_ControlID = MgPacketParser::mciClose;

        stream.WriteControlPacket(packet);

        // close the writer and attempt to gracefully leave
        mServer->close_writer();

        char buf[256];

        ssize_t len = 0;
        while ((len = mServer->recv((void*)buf, 256, MG_MSG_NOSIGNAL)) > 0)
        {
            // clearing out buffer
        }

        // Long enough.  Bail.
        mServer->close();

        delete mServer;
        mServer = NULL;
    }
}

ACE_HANDLE MgServerConnectionImp::GetHandle()
{
    if (mServer == NULL)
        return 0;

    return mServer->get_handle();
}
