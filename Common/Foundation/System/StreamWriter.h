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

#ifndef MG_STREAM_WRITER_H
#define MG_STREAM_WRITER_H

class MgPacketParser;

/// \cond INTERNAL
class MG_FOUNDATION_API MgStreamWriter
{
    DECLARE_CLASSNAME(MgStreamWriter)

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructors
    ///
    /// \param streamHelper
    /// Stream helper object holding ACE_SOCK_STREAM
    ///
    MgStreamWriter( MgStreamHelper* streamHelper );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor
    ///
    virtual ~MgStreamWriter();

    /// \brief
    /// Writes the stream header
    ///
    /// \param msh
    /// Stream header to hold the data
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteStreamHeader(MgStreamHeader& msh);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the operation header to the server
    ///
    /// \param operPacket
    /// Operation packet to write
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteOperationHeader(MgOperationPacket& operPacket);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes the stream header to the server
    ///
    virtual MgStreamHelper::MgStreamStatus WriteBinaryStreamHeader(MgBinaryStreamArgumentPacket& bsap);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes the response header from the server
    ///
    /// \param eCode
    /// eCode to send.
    /// \param retValues
    /// no. of return arguments.
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteOperationResponseHeader( MgOperationResponsePacket& mop );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes the header to the server
    ///
    virtual MgStreamHelper::MgStreamStatus WriteArgumentHeader(MgArgumentPacket& map);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the string argument packet to the server
    ///
    /// \param value
    /// value to be written.
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteString(CREFSTRING value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the int64 argument packet to the server
    ///
    /// \param value
    /// value to be written.
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteInt64(INT64 value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the int32 argument packet to the server
    ///
    /// \param value
    /// value to be written.
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteInt32(INT32 value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the int16 argument packet to the server
    ///
    /// \param value
    /// value to be written.
    ///
    /// \return
    /// Stream Status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteInt16(INT16 value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the float argument packet to the server
    ///
    /// \param value
    /// value to be written.
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteSingle(float value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the double argument packet to the server
    ///
    /// \param value
    /// value to be written.
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteDouble(double value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the boolean argument packet to the server
    ///
    /// \param value
    /// value to be written.
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteBoolean(bool value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the byte argument packet to the server
    ///
    /// \param value
    /// value to be written.
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteByte(BYTE value);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the stream to the server
    ///
    /// \param byteReader
    /// ByteReader from which stream to be taken
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteStream(MgByteReader* byteReader);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// This writes the class id argument packet to the server
    ///
    /// \param value
    /// value to be written.
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteClassId(INT32 value);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes the end of stream header
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteStreamEnd();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes the Object to stream
    ///
    /// <param>
    /// ClassId of object to be sent
    /// </param>
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteObject(MgObject* obj);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes the Stream header and Operation header for version 1 stream
    ///
    /// \param eCode
    /// Code specifies whether it is success or failure or success with warnings
    /// \param noOfRetValues
    /// No of return values being sent
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteResponseHeader(MgPacketParser::MgECode eCode, UINT32 noOfRetValues);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes stream header, control packet, and stream end to the stream
    ///
    /// \param packet
    /// Control packet to write to stream
    ///
    /// \return
    /// Stream status
    ///
    virtual MgStreamHelper::MgStreamStatus WriteControlPacket(MgControlPacket& packet);

protected:

    //////////////////////////
    /// Private member data
    Ptr<MgStreamHelper> m_sHelper;
};
/// \endcond

#define MG_STREAM_VERSION   1

#endif // MG_STREAM_WRITER_H
