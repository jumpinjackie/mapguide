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

#ifndef _MGSTREAMREADER_H_
#define _MGSTREAMREADER_H_

/// INTERNAL_CLASS
/// \cond INTERNAL
class MgServerConnection;

class MG_FOUNDATION_API MgStreamReader
{
    DECLARE_CLASSNAME(MgStreamReader)

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructors
    ///
    /// \param streamHelper
    /// Stream helper object holding ACE_SOCK_STREAM
    ///
    MgStreamReader( MgStreamHelper* streamHelper );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor
    ///
    virtual ~MgStreamReader();

    /// \brief
    /// Reads the stream header
    ///
    /// \param msh
    /// Stream header to hold the data
    ///
    virtual void GetStreamHeader(MgStreamHeader& msh);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the operation header to the server
    ///
    /// \param serviceId
    /// serviceId like ResourceService, DrawingService etc.
    ///
    /// \return
    /// Nothing
    ///
    virtual void GetOperationHeader(MgOperationPacket& operPacket);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the response header from the server
    ///
    /// \param packetHeader
    /// Outparameter , packet header received.
    ///
    virtual void GetOperationResponseHeader(MgOperationResponsePacket& mop);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the header from the server
    ///
    virtual void GetArgumentHeader(MgArgumentPacket& map);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the binary stream header from the server
    ///
    virtual void GetBinaryStreamHeader(MgBinaryStreamArgumentPacket& bsap);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This reads the string from the server
    ///
    /// \param value
    /// Value to be read
    ///
    /// \return
    /// Nothing
    ///
    virtual void GetString(REFSTRING wcStr);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This reads the int64 from the server
    ///
    /// \param value
    /// Value to be read
    ///
    /// \return
    /// Nothing
    ///
    virtual void GetInt64(INT64& value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This reads the int32 from the server
    ///
    /// \param value
    /// Value to be read
    ///
    /// \return
    /// Nothing
    ///
    virtual void GetInt32(INT32& value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This reads the int16 from the server
    ///
    /// \param value
    /// Value to be read
    ///
    /// \return
    /// Nothing
    ///
    virtual void GetInt16(INT16& value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This reads the float from the server
    ///
    /// \param value
    /// Value to be read
    ///
    /// \return
    /// Nothing
    ///
    virtual void GetSingle(float& value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This reads the double from the server
    ///
    /// \param value
    /// Value to be read
    ///
    /// \return
    /// Nothing
    ///
    virtual void GetDouble(double& value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This reads the boolean from the server
    ///
    /// \param value
    /// Value to be read
    ///
    /// \return
    /// Nothing
    ///
    virtual void GetBoolean(bool& value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This reads the byte from the server
    ///
    /// \param value
    /// Value to be read
    ///
    /// \return
    /// Nothing
    ///
    virtual void GetByte(BYTE& value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This reads the stream from the server
    ///
    /// \return
    /// ByteReader
    ///
    virtual MgByteReader* GetStream(bool enableDirect = false);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This reads the data from the server
    ///
    /// \param value
    /// Value to be read
    ///
    /// \return
    /// size of data read
    ///
    virtual MgStreamHelper::MgStreamStatus GetData(void* value, INT32 length);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This reads the class id argument packet from the server
    ///
    /// \param value
    /// value to be read.
    ///
    /// \return
    /// Nothing
    ///
    virtual void GetClassId(INT32& value);

    /// \brief
    /// Reads the end of stream header
    ///
    /// \return
    /// Nothing
    ///
    virtual void GetStreamEnd();

    /// \brief
    /// Reads the Object from stream
    ///
    /// \return
    /// MgObject pointer
    ///
    virtual MgObject* GetObject();

    /// \brief
    /// Returns the stream helper contained by this stream
    ///
    ///
    MgStreamHelper* GetStreamHelper();

    /// \brief
    /// Returns the connection this stream belongs to
    ///
    ///
    MgServerConnectionBase* GetConnection();

    /// \brief
    /// Sets the connection this stream belongs to
    ///
    ///
    void SetConnection(MgServerConnectionBase* conn);

protected:
    Ptr<MgStreamHelper> m_sHelper;
    MgServerConnectionBase* m_conn; // raw pointer to avoid circular dependency
};
/// \endcond

#endif // _MGSTREAMREADER_H_

