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

#ifndef RSMGFEATUREREADER_H
#define RSMGFEATUREREADER_H

#include "RS_FeatureReader.h"

struct PropertyStub
{
    STRING m_name;
    const wchar_t* m_name_cstr;
    int m_recordIndex;
    int m_dataType;
};

class LineBuffer;
class CSysTransformer;
class RS_Raster;
class RS_InputStream;

//RS_FeatureReader implementation based on an MgFeatureReader
class MG_SERVER_MAPPING_API RSMgFeatureReader : public RS_FeatureReader
{
public:
    RSMgFeatureReader(MgFeatureReader* reader, MgFeatureService* svcFeature, MgResourceIdentifier* featResId, MgFeatureQueryOptions* options, const STRING& geomPropName);
    virtual ~RSMgFeatureReader();

    //--------------------------------------------
    // RS_FeatureReader implementation
    //--------------------------------------------

    virtual bool ReadNext();
    virtual void Close   ();
    virtual void Reset   ();

    virtual bool            IsNull         (const wchar_t* propertyName);
    virtual bool            GetBoolean     (const wchar_t* propertyName);
    virtual unsigned char   GetByte        (const wchar_t* propertyName);
    virtual FdoDateTime     GetDateTime    (const wchar_t* propertyName);
    virtual float           GetSingle      (const wchar_t* propertyName);
    virtual double          GetDouble      (const wchar_t* propertyName);
    virtual short           GetInt16       (const wchar_t* propertyName);
    virtual int             GetInt32       (const wchar_t* propertyName);
    virtual long long       GetInt64       (const wchar_t* propertyName);
    virtual const wchar_t*  GetString      (const wchar_t* propertyName);
    virtual LineBuffer*     GetGeometry    (const wchar_t* propertyName, LineBuffer* lb, CSysTransformer* xformer);
    virtual RS_Raster*      GetRaster      (const wchar_t* propertyName);
    virtual const wchar_t*  GetAsString    (const wchar_t* propertyName);
    virtual RS_InputStream* GetBLOB        (const wchar_t* propertyName);
    virtual RS_InputStream* GetCLOB        (const wchar_t* propertyName);
    virtual int             GetPropertyType(const wchar_t* propertyName);

    virtual const wchar_t*        GetGeomPropName  ();
    virtual const wchar_t*        GetRasterPropName();
    virtual const wchar_t* const* GetIdentPropNames(int& count);
    virtual const wchar_t* const* GetPropNames     (int& count);

    virtual FdoIFeatureReader* GetInternalReader();

private:
    MgFeatureReader* m_reader;
    MgClassDefinition* m_class;

    int m_numProps;
    PropertyStub* m_vProps;

    wchar_t** m_propNames;
    wchar_t** m_idpropNames;
    int m_numIdProps;

    STRING m_geomPropName;
    STRING m_rasterPropName;
    STRING m_cachePropValue;

    //needed for resetting of the reader
    MgFeatureService* m_svcFeature;
    MgResourceIdentifier* m_resId;
    MgFeatureQueryOptions* m_options;
};

#endif
