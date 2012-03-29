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

#ifndef RS_FEATUREREADER_H_
#define RS_FEATUREREADER_H_

#include "LineBuffer.h"
#include "CSysTransformer.h"
#include "RS_Raster.h"
#include <Fdo.h>

// Defines a feature data reader interface.
class RS_FeatureReader
{
public:
    virtual bool ReadNext() = 0;
    virtual void Close   () = 0;
    virtual void Reset   () = 0;

    virtual bool            IsNull         (const wchar_t* propertyName) = 0;
    virtual bool            GetBoolean     (const wchar_t* propertyName) = 0;
    virtual unsigned char   GetByte        (const wchar_t* propertyName) = 0;
    virtual FdoDateTime     GetDateTime    (const wchar_t* propertyName) = 0;
    virtual float           GetSingle      (const wchar_t* propertyName) = 0;
    virtual double          GetDouble      (const wchar_t* propertyName) = 0;
    virtual short           GetInt16       (const wchar_t* propertyName) = 0;
    virtual int             GetInt32       (const wchar_t* propertyName) = 0;
    virtual long long       GetInt64       (const wchar_t* propertyName) = 0;
    virtual const wchar_t*  GetString      (const wchar_t* propertyName) = 0;
    virtual LineBuffer*     GetGeometry    (const wchar_t* propertyName, LineBuffer* lb, CSysTransformer* xformer) = 0;
    virtual RS_Raster*      GetRaster      (const wchar_t* propertyName) = 0;
    virtual const wchar_t*  GetAsString    (const wchar_t* propertyName) = 0;
    virtual RS_InputStream* GetBLOB        (const wchar_t* propertyName) = 0;
    virtual RS_InputStream* GetCLOB        (const wchar_t* propertyName) = 0;
    virtual int             GetPropertyType(const wchar_t* propertyName) = 0;

    virtual const wchar_t*        GetGeomPropName  ()           = 0;
    virtual const wchar_t*        GetRasterPropName()           = 0;
    virtual const wchar_t* const* GetIdentPropNames(int& count) = 0;
    virtual const wchar_t* const* GetPropNames     (int& count) = 0;

    virtual FdoIFeatureReader* GetInternalReader() = 0;
};

#endif
