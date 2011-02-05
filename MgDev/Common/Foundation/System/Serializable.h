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

#ifndef _MG_SERIALIZABLE_H
#define _MG_SERIALIZABLE_H

class MgDisposable;
class MgStream;

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// Root class of all serializable objects.
class MG_FOUNDATION_API MgSerializable : public MgGuardDisposable
{
EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default destructor for this class.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual ~MgSerializable();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream) = 0;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Builds a log friendly string representation
    ///
    /// \return
    /// String suitable for logs
    ///
    virtual STRING GetLogString();

};
/// \endcond

#endif
