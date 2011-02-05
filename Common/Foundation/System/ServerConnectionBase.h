//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#ifndef MG_SERVER_CONNECTION_BASE_H
#define MG_SERVER_CONNECTION_BASE_H

/// \cond INTERNAL
//////////////////////////////////////////////////////////////////
/// \brief
/// Represents a connection to a MapGuide server
/// INTERNAL_ONLY:
class MG_FOUNDATION_API MgServerConnectionBase : public MgGuardDisposable
{
    INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Internal method used for getting stream helper
    /// which will then be used in serialize/deserialize
    ///
    /// \return
    /// MgStreamHelper which has method to write or read data
    /// over the wire
    ///
    virtual MgStream* GetStream() = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor. This will clean up the connection.
    ///
    virtual ~MgServerConnectionBase();
};
/// \endcond

#endif
