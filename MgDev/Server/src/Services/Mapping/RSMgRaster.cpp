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

#include "ServerMappingServiceDefs.h"
#include "RSMgRaster.h"
#include "RSMgInputStream.h"

RSMgRaster::RSMgRaster(MgRaster* raster)
{
    assert(raster);
    m_raster = raster;
    SAFE_ADDREF(m_raster);

    m_width = m_raster->GetImageXSize();
    m_height = m_raster->GetImageYSize();
}

RSMgRaster::~RSMgRaster()
{
    SAFE_RELEASE(m_raster);
}

RS_Bounds RSMgRaster::GetExtent()
{
    Ptr<MgEnvelope> env = m_raster->GetBounds();

    Ptr<MgCoordinate> ll = env->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = env->GetUpperRightCoordinate();

    return RS_Bounds(ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());
}

int RSMgRaster::GetOriginalWidth()
{
    return m_width;
}

int RSMgRaster::GetOriginalHeight()
{
    return m_height;
}

int RSMgRaster::GetBitsPerPixel()
{
    return m_raster->GetBitsPerPixel();
}

int RSMgRaster::GetDataModelType()
{
    return m_raster->GetDataModelType();
}


//caller frees returned stream
RS_InputStream* RSMgRaster::GetStream(RS_ImageFormat format, int width, int height)
{
    if (width > 0 && height > 0)
    {
        m_raster->SetImageXSize(width);
        m_raster->SetImageYSize(height);
    }

    //TODO: FeatureService does not expose that yet
    /*
    Ptr<MgRasterDataModel> dataModel = m_raster->GetDataModel();

    switch (format)
    {
    case RS_ImageFormat_RGBA :
        dataModel->SetDataModelType(FdoRasterDataModelType_RGBA);
        dataModel->SetBitsPerPixel(32);
        break;
    //TODO: others
    default:
        dataModel->SetBitsPerPixel(32);
        dataModel->SetDataModelType(FdoRasterDataModelType_RGBA);
        break;
    }

    m_raster->SetDataModel(dataModel);
    */

    Ptr<MgByteReader> rdr = m_raster->GetStream();
    if(rdr)
    {
        return new RSMgInputStream(rdr.p);
    }
    else
    {
        return NULL;
    }
}

RS_InputStream* RSMgRaster::GetPalette()
{
    Ptr<MgByte> pal = m_raster->GetPalette();

    if (pal.p)
        return new RSMgInputStream(pal.p);
    else
        return NULL;
}

int RSMgRaster::GetDataType()
{
    return m_raster->GetDataType();
}

RS_String RSMgRaster::GetVerticalUnits()
{
    return m_raster->GetVerticalUnits();
}

int RSMgRaster::GetNullValueType()
{
    return m_raster->GetNullValueType();
}

INT64 RSMgRaster::GetNullValueData()
{
    return m_raster->GetNullValue();
}

