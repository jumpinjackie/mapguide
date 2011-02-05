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

#ifndef MGSTREAM_H_
#define MGSTREAM_H_

class MgByteReader;
class MgStringCollection;

class MgStream;
template class MG_FOUNDATION_API Ptr<MgStream>;

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////
//  The MgStream class provides an interface to read and write data via TCP/IP
class MG_FOUNDATION_API MgStream : public MgDisposable, public MgStreamReader, public MgStreamWriter
{
    DECLARE_CLASSNAME(MgStream)

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default Constructor
    ///
    ///
    MgStream();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructors
    ///
    /// \param streamHelper
    /// Stream helper object holding ACE_SOCK_STREAM
    ///
    MgStream( MgStreamHelper* streamHelper );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor
    ///
    virtual ~MgStream();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the stream helper contained by this stream
    ///
    ///
    MgStreamHelper* GetStreamHelper();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the stream helper for this stream
    ///
    ///
    void SetStreamHelper(MgStreamHelper* helper);

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class id
    ///
    /// \return
    /// class id
    ///
    INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Self destructing method
    ///
    virtual void Dispose();

CLASS_ID:

    static const INT32 m_cls_id = Foundation_Data_Stream;
};
/// \endcond

#endif  //  MGSTREAM_H
