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

#ifndef _MG_GWS_FEATURE_READER_H
#define _MG_GWS_FEATURE_READER_H


/// \cond INTERNAL

////////////////////////////////////////////////////////////////
/// \brief
/// Provides a forward-only, read-only iterator for reading data
/// returned by a call to GwsQueryEngine.
/// \remarks
/// The initial position of the GwsReader interface is prior to
/// the first item. Therefore, you must call ReadNext before you
/// can access any data.
///

class MG_PLATFORMBASE_API  MgGwsFeatureReader : public MgFeatureReader
{
EXTERNAL_API:
    virtual ~MgGwsFeatureReader() {}

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type reader.
    /// Please refer to MgReaderType for list of values
    ///
    /// \return
    /// Returns the type of the Reader.
    ///
    virtual INT32 GetReaderType()
    {
        return MgReaderType::FeatureReader;
    }


protected:

    virtual void Dispose()
    {
        delete this;
    }

INTERNAL_API:

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }


CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_GwsFeatureReader;

};
/// \endcond

#endif
