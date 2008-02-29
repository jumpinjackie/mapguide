//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef SERVER_CONNECTION_IMP_H
#define SERVER_CONNECTION_IMP_H

// forward declaration
class ACE_SOCK_Stream;

/// \cond INTERNAL
//////////////////////////////////////////////////////////////////////////////
/// \brief
/// Server connection class responsible for actual connection to another machine
/// INTERNAL API - DO NOT DOCUMENT
class MG_MAPGUIDE_API MgServerConnectionImp
{
    DECLARE_CLASSNAME(MgServerConnectionImp)

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructor.
    ///
     MgServerConnectionImp();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor.
    ///
     ~MgServerConnectionImp();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Establishes a connection to a MapGuide server
    ///
    /// \param targetName
    /// Ip address for MapGuide server machine
    /// \param portno
    /// port# to use
    ///
    /// \return
    /// true -  on successful connection
    /// false - could not connect
    ///
    ///
    bool Connect(const char* targetCluster, int portno);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Disconnects from MapGuide server
    ///
    void Disconnect();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the ace handle
    ///
    ACE_HANDLE GetHandle();

private:
    ACE_SOCK_Stream* mServer;
};
/// \endcond

#endif

