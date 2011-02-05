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

#ifndef MG_MAPGUIDE_STREAM_H_
#define MG_MAPGUIDE_STREAM_H_

class MgByteReader;
class MgStringCollection;

class MgMapGuideStream;
template class MG_MAPGUIDE_API Ptr<MgMapGuideStream>;

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////
//  The MgMapGuideStream class provides MapGuide specific enchancements to MgStream
class MG_MAPGUIDE_API MgMapGuideStream : public MgStream
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMapGuideStream)

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default Constructor
    ///
    ///
    MgMapGuideStream();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructors
    ///
    /// \param streamHelper
    /// Stream helper object holding ACE_SOCK_STREAM
    ///
    MgMapGuideStream( MgStreamHelper* streamHelper );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor
    ///
    virtual ~MgMapGuideStream();

    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// This writes the operation header to the server
    ///</summary>
    ///<param name="serviceId">
    /// serviceId like ResourceService, DrawingService etc.
    ///</param>
    ///<returns>
    ///Nothing
    ///</returns>
    virtual void GetOperationHeader(MgOperationPacket& operPacket);

    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Gets the response header from the server
    ///</summary>
    ///<param name="packetHeader">
    /// Outparameter , packet header received.
    ///</param>
    virtual void GetOperationResponseHeader(MgOperationResponsePacket& mop);

    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// This writes the operation header to the server
    ///</summary>
    ///<param name="operPacket">
    /// Operation packet to write
    ///</param>
    ///<returns>
    /// Stream status of write
    ///</returns>
    virtual MgStreamHelper::MgStreamStatus WriteOperationHeader(MgOperationPacket& operPacket);

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Self destructing method
    ///
    virtual void Dispose();
};
/// \endcond

#endif  //  MGSTREAM_H
