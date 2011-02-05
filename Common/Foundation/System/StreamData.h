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

#ifndef MGSTREAMDATA_H_
#define MGSTREAMDATA_H_

class MgStreamHelper;

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////
//  The StreamData class encapsulates the data pertaining to an
//  input/output stream.
class MG_FOUNDATION_API MgStreamData : public ACE_Data_Block
{
    DECLARE_CLASSNAME(MgStreamData)

    ///////////////////////////////////////////////////////////////////////
    ///  Constructors/Destructors
    public:

    MgStreamData( ACE_HANDLE handle, MgStreamHelper* pStreamHelper );
    MgStreamData( MgStreamData & );
    virtual ~MgStreamData();

    ///////////////////////////////////////////////////////////////////////
    ///  Accessors
    public:

    INT64 GetDataRead();
    void SetDataRead( INT64 total );
    INT64 GetDataSize();
    void SetDataSize( INT64 size );
    ACE_HANDLE GetHandle() const;
    MgStreamHelper* GetStreamHelper();
    UINT32 GetVersion();
    void SetVersion( UINT32 version );

    ///////////////////////////////////////////////////////////////////////
    ///  Member Variables
    protected:

    ///  the underlyng handle of the input/output stream
    const ACE_HANDLE    m_Handle;

    ///  the StreamHelper objec that handles IO for the stream
    Ptr<MgStreamHelper> m_pStreamHelper;

    ///  the version of the stream behing read
    UINT32          m_Version;

    ///  the size of the data payload of the stream
    INT64           m_DataSize;

    ///  the number of bytes that have been read from the stream
    INT64           m_DataRead;

    ///  the current error state of the stream
    bool                m_bError;
};
/// \endcond

#endif  //  MGSTREAMDATA_H_MG
