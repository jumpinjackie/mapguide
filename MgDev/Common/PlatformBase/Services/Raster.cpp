//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "PlatformBase.h"
#include "FeatureService.h"
#include "GeometryCommon.h"
#include "RasterTypes.h"

MG_IMPL_DYNCREATE(MgRaster)

MgRaster::MgRaster()
{
    m_xSize = 0;
    m_ySize = 0;
    m_extent = NULL;
    m_isNull = false;
    m_featureService = NULL;
    m_handle = L"";
    m_rasterPropName = L"";
    m_bpp = 32;
    m_dataModel = MgRasterDataModelType::RGBA;
    m_palette = NULL;
    m_numBands = 0;
    m_curBand = 0;
    m_dataType = 0;
    m_VerticalUnitsName = L"";
}

MgRaster::~MgRaster()
{
    SAFE_RELEASE(m_extent);
    SAFE_RELEASE(m_featureService);
}


/// <summary>
/// Returns true if the MgRaster represents a null value.
/// </summary>
/// <returns>
/// true  - null value.
/// false - non-null
/// </returns>
bool MgRaster::IsNull()
{
    return m_isNull;
}

/// <summary>Sets the MgRaster to a null value.</summary>
/// <returns>Returns nothing</returns>
void MgRaster::SetNull()
{
    m_isNull = true;
}

/// <summary>
/// Get the minimum bounding box around the image.
/// The bounds are transformed to the active spatial context.
/// </summary>
/// <returns>Returns MgEnvelope object containing bounds</returns>
MgEnvelope* MgRaster::GetBounds()
{
    NullCheck();

    return SAFE_ADDREF((MgEnvelope*)m_extent);
}

/// <summary>
/// Set the minimum bounding box around the image.
/// For georeferenced images, the coordinate system is specified by the
/// spatial context in force when the object is created.  The act of
/// setting this property georeferences the image.  In order to provide
/// for non-georeferenced images, this property is Nullable (doesn't
/// need to be set).
/// </summary>
void MgRaster::SetBounds(MgEnvelope* bounds)
{
    NullCheck();

    m_extent = SAFE_ADDREF(bounds);
}

/// <summary>
/// Gets the size of image file in the horizontal
/// direction in pixels (number of columns).
/// </summary>
/// <returns>
/// Returns the horizontal image size in pixels (number of columns).
/// </returns>
INT32 MgRaster::GetImageXSize()
{
    NullCheck();

    return m_xSize;
}

/// <summary>
/// Sets the size of image file in the horizontal
/// direction in pixels (number of columns).
/// By setting the X and Y image size properties prior to getting
/// a reader for the data, the client application can control the
/// density of pixels fetched by the provider, which may reduce
/// the amount of data shipped by the reader and reduce processing
/// time significantly.  For example, a certain query in the
/// coordinate system of the raster object class returns a
/// MgRaster object with image size 12091 by 8043.  But the
/// image only needs to be displayed in an area 1167 by 776 on
/// the users monitor (note that the aspect ratios are the same
/// because the query is constructed based on the display area).
/// By setting the image size to these lower values the FDO raster
/// subsystem may be able to use a previously subsampled image
/// where the resolution has been reduced by a factor of 4.
/// So instead of transferring 97MB, it can transfer 6MB (a reduction
/// by 4 in both dimensions reduces the amount of data by a factor of 16)
/// and subsample the image again to the desired resolution in a
/// more timely manner (less data to process means less total time),
/// with no appreciable difference in display quality to the user.
/// </summary>
/// <param name="size">
/// The desired horizontal image size in pixels (number of columns).
/// </param>
void MgRaster::SetImageXSize(INT32 size)
{
    NullCheck();

    m_xSize = size;
}

/// <summary>
/// Gets the size of image file in the vertical
/// direction in pixels (number of rows).
/// </summary>
/// <returns>
/// Returns the vertical image size in pixels (number of rows).
/// </returns>
INT32 MgRaster::GetImageYSize()
{
    NullCheck();

    return m_ySize;
}

/// <summary>Sets the size of image file in the vertical
/// direction in pixels (number of rows). See SetImageXSize.</summary>
/// <param name="size">The desired vertical image size in pixels
/// (number of rows).</param>
void MgRaster::SetImageYSize(INT32 size)
{
    NullCheck();

    m_ySize = size;
}

void MgRaster::SetMgService(MgFeatureService* service)
{
    m_featureService = SAFE_ADDREF(service);
}

/// <summary>
/// Get the source of image data.
/// Image data is shipped using a paradigm similar to BLOB I/O.
/// When fetching a raster image the GetStream() method provides
/// a GisIStreamReader from which the client may request the image data.
/// The format of the image data expected is determined by the
/// DataModel property.  The data type, bit depth, tiling and
/// organization specify the meaning of the image data.
/// By setting the DataModel property prior to getting a
/// reader for the data, the FDO client application can control the
/// way data is retrieved from the FDO raster subsystem.
/// When a tiled image is fetched, the image data will be tiled by default,
/// unless a spatial query is used that covers a portion of a tile,
/// in which case the image is converted to monolithic form prior to
/// returning to the client.  The client may force a monolithic
/// image in the former case by setting the DataModel to monolithic
/// prior to getting the stream reader.  Or conversely it can request
/// a re-tiling of the remaining sub-image by setting the DataModel
/// property to tiled (in this case the tile origin is at the new upper
/// left corner of the sub-image).
/// Note that altering the DataModel may have significant performance issues.
/// </summary>
MgByteReader* MgRaster::GetStream()
{
    NullCheck();

    // If no feature service is assigned it means some one directly created this pointer
    // and accessing it.
    if (L"" == m_handle || m_featureService == NULL)
    {
        throw new MgInvalidOperationException(L"MgRaster.GetStream", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgByteReader> byteReader;
    if (!m_isNull)
    {
        byteReader = m_featureService->GetRaster(m_handle, m_xSize, m_ySize, m_rasterPropName);
    }

    return byteReader.Detach();
}

void MgRaster::SetHandle(STRING handle)
{
    m_handle = handle;
}

void MgRaster::Serialize(MgStream* stream)
{
    stream->WriteBoolean(m_isNull);
    if (!m_isNull)
    {
        stream->WriteInt32(m_xSize);
        stream->WriteInt32(m_ySize);
        stream->WriteString(m_rasterPropName);
        stream->WriteInt32(m_bpp);
        stream->WriteInt32(m_dataModel);
        stream->WriteObject(m_palette.p);
//      stream->WriteObject(m_extent);
        stream->WriteInt32(m_numBands);
        stream->WriteInt32(m_curBand);
        stream->WriteInt16(m_dataType);
        stream->WriteString(m_VerticalUnitsName);
        stream->WriteInt16(m_NoDataValueType);
        stream->WriteInt64(m_NoDataValue);
    }
}

void MgRaster::Deserialize(MgStream* stream)
{
    stream->GetBoolean(m_isNull);
    if (!m_isNull)
    {
        stream->GetInt32(m_xSize);
        stream->GetInt32(m_ySize);
        stream->GetString(m_rasterPropName);
        stream->GetInt32(m_bpp);
        stream->GetInt32(m_dataModel);
        m_palette = (MgByte*)stream->GetObject();
//      m_extent = (MgEnvelope*)stream->GetObject();
        stream->GetInt32(m_numBands);
        stream->GetInt32(m_curBand);
        stream->GetInt16(m_dataType);
        stream->GetString(m_VerticalUnitsName);
        stream->GetInt16(m_NoDataValueType);
        stream->GetInt64(m_NoDataValue);
    }
}

void MgRaster::NullCheck()
{
    if (m_isNull)
    {
        MgStringCollection arguments;
        arguments.Add(L"Raster");

        throw new MgNullPropertyValueException(L"MgRaster.NullCheck",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}


///Sets the number of bits per pixel
void MgRaster::SetBitsPerPixel(INT32 bpp)
{
    m_bpp = bpp;
}

///Sets the type of pixel color data (RGB, RGBA, Bitonal, etc.)
void MgRaster::SetDataModelType(INT32 dataModel)
{
    m_dataModel = dataModel;
}

///Returns the number of bits per pixel
INT32 MgRaster::GetBitsPerPixel()
{
    return m_bpp;
}

///Returns the type of pixel color data (RGB, RGBA, Bitonal, etc.)
INT32 MgRaster::GetDataModelType()
{
    return m_dataModel;
}

///If the raster uses a color map, returns the color map contents
///otherwise NULL
MgByte* MgRaster::GetPalette()
{
    return SAFE_ADDREF(m_palette.p);
}

///Sets the (optional) color map
void MgRaster::SetPalette(MgByte* pal)
{
    m_palette = SAFE_ADDREF(pal);
}

INT32 MgRaster::GetNumberOfBands ()
{
    return m_numBands;
}

void MgRaster::SetNumberOfBands (INT32 value)
{
    m_numBands = value;
}

INT32 MgRaster::GetCurrentBand ()
{
    return m_curBand;
}

void MgRaster::SetCurrentBand (INT32 value)
{
    m_curBand = value;
}

INT16 MgRaster::GetDataType()
{
    return m_dataType;
}

void MgRaster::SetDataType(INT16 value)
{
    m_dataType = value;
}

STRING MgRaster::GetVerticalUnits()
{
    return m_VerticalUnitsName;
}

INT32 MgRaster::GetNullValueType()
{
    return (INT32)m_NoDataValueType;
}

INT64 MgRaster::GetNullValue()
{
    return m_NoDataValue;
}





