//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _MG_RASTER_H
#define _MG_RASTER_H

/// \cond INTERNAL

class MgStream;
class MgFeatureService;
class MgEnvelope;
class MgRaster;
template class MG_PLATFORMBASE_API Ptr<MgRaster>;

// INTERNAL_CLASS
class MG_PLATFORMBASE_API MgRaster : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgRaster)

PUBLISHED_API:
    /// \brief
    /// Returns true if the MgRaster represents a null value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsNull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsNull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsNull();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// true  - null value.
    /// false - non-null
    ///
    bool IsNull();

    /// \brief
    /// Sets the MgRaster to a null value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetNull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetNull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetNull();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing
    ///
    void SetNull();

    /// \brief
    /// Get the minimum bounding box around the image.
    /// The bounds are transformed to the active spatial context.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgEnvelope GetBounds();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgEnvelope GetBounds();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgEnvelope GetBounds();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns MgEnvelope object containing bounds
    ///
    MgEnvelope* GetBounds();   /// __get

    /// \brief
    /// Gets the size of image file in the horizontal
    /// direction in pixels (number of columns).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetImageXSize();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetImageXSize();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetImageXSize();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the horizontal image size in pixels (number of columns).
    ///
    INT32 GetImageXSize();   /// __get, __set

    /// \brief
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
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetImageXSize(int size);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetImageXSize(int size);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetImageXSize(int size);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param size (int)
    /// The desired horizontal image size in pixels (number of columns).
    ///
    void SetImageXSize(INT32 size);

    /// \brief
    /// Gets the size of image file in the vertical
    /// direction in pixels (number of rows).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetImageYSize();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetImageYSize();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetImageYSize();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the vertical image size in pixels (number of rows).
    ///
    INT32 GetImageYSize();   /// __get, __set

    /// \brief
    /// Sets the size of image file in the vertical
    /// direction in pixels (number of rows). See SetImageXSize.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetImageYSize(int size);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetImageYSize(int size);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetImageYSize(int size);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param size (int)
    /// The desired vertical image size in pixels
    /// (number of rows).
    ///
    void SetImageYSize(INT32 size);

    /// \brief
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
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader GetStream();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader GetStream();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader GetStream();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Streamed raster image
    ///
    MgByteReader* GetStream();

    /// \brief
    ///  Gets the number of bits per pixel.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetBitsPerPixel();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetBitsPerPixel();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetBitsPerPixel();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    ///  Returns the number of bits per pixel.
    ///
    INT32 GetBitsPerPixel();   /// __get

    /// \brief
    ///  Gets the type of data model (RGB, RGBA, Bitonal, etc.)
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetDataModelType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetDataModelType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetDataModelType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    ///  Returns the type of pixel color data (RGB, RGBA, Bitonal, etc.)
    ///
    INT32 GetDataModelType();   /// __get

    /// \brief
    ///  Gets the number of bands/channels contained in the raster image.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetNumberOfBands();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetNumberOfBands();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetNumberOfBands();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    ///  Returns the numbers of bands/channels contained in the raster image.
    ///
    INT32 GetNumberOfBands ();


    /// \brief
    ///  Gets the currently active band/channel.
    ///
    /// \remarks
    ///  The FdoIRaster Bounds, DataModel, ImageSize, AuxilliaryProperties and Stream
    /// accessors all conform to the current band. NOTE: The return value
    /// is a 1 based index whose maximum value is determined by the result from
    /// GetNumberOfBands.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetCurrentBand();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetCurrentBand();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetCurrentBand();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    ///  Returns the current active band/channel.
    ///
    INT32 GetCurrentBand ();

    /// \brief
    ///  Gets the data type.
    ///
    /// \remarks
    /// Data is organized in an unknown or provider specific manner.
    /// FdoRasterDataType_Unknown = 0
    ///
    /// Data is organized as Unsigned Integers.
    /// FdoRasterDataType_UnsignedInteger = 1
    ///
    /// Data is organized as Signed Integers.
    /// FdoRasterDataType_Integer = 2
    ///
    /// Data is organized as Floats.
    /// FdoRasterDataType_Float = 3
    ///
    /// Data is organized as Doubles.
    /// FdoRasterDataType_Double = 4
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetDataType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetDataType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetDataType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the data type of the raster.
    ///
    INT16 GetDataType();

    /// \brief
    ///  Gets the vertical units.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetVerticalUnits();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetVerticalUnits();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetVerticalUnits();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    ///  Returns the vertical units.
    ///
    STRING  GetVerticalUnits();

    /// \brief
    /// Gets the FDO data type of the NODATA value of the raster.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetNullValueType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetNullValueType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetNullValueType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the FDO data type of the null value.
    ///
    INT32   GetNullValueType();

    /// \brief
    /// Gets the NODATA value of the raster.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// long GetNullValue();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// long GetNullValue();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetNullValue();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the FDO data type of the null value.
    ///
    INT64   GetNullValue();

INTERNAL_API:

    ///If the raster uses a color map, returns the color map contents
    ///otherwise NULL
    MgByte* GetPalette();

    /// \brief
    /// Set the minimum bounding box around the image.
    /// For georeferenced images, the coordinate system is specified by the
    /// spatial context in force when the object is created.  The act of
    /// setting this property georeferences the image.  In order to provide
    /// for non-georeferenced images, this property is Nullable (doesn't
    /// need to be set). (Not exposed via API)
    ///
    void SetBounds(MgEnvelope* bounds);

    ///Sets the number of bits per pixel
    void SetBitsPerPixel(INT32);

    ///Sets the type of pixel color data (RGB, RGBA, Bitonal, etc.)
    void SetDataModelType(INT32);

    ///Sets the (optional) color map
    void SetPalette(MgByte*);


    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);
    MgRaster();
    ~MgRaster();
    void SetMgService(MgFeatureService* service);
    void SetHandle(STRING handle);
    void SetPropertyName(STRING propName) { m_rasterPropName = propName; }

    void SetNumberOfBands (INT32 value);
    void SetCurrentBand (INT32 value);
    void SetDataType(INT16 value);

    void SetVerticalUnits(STRING vertUnits) { m_VerticalUnitsName = vertUnits; }
    void SetNullValueType(INT16 value) { m_NoDataValueType = value; }
    void SetNullValue(INT64 value) { m_NoDataValue = value; }

protected:

    virtual void Dispose()
    {
        delete this;
    }

private:

    void NullCheck();

    INT32 m_xSize;
    INT32 m_ySize;
    STRING m_handle;
    MgEnvelope* m_extent;
    MgFeatureService* m_featureService;
    bool m_isNull;
    STRING m_rasterPropName;
    INT32 m_bpp;
    INT32 m_dataModel;
    Ptr<MgByte> m_palette;
    INT32 m_numBands;
    INT32 m_curBand;
    INT16 m_dataType;
    STRING m_VerticalUnitsName;
    INT16  m_NoDataValueType;
    INT64  m_NoDataValue;

INTERNAL_API:

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_Raster;

};
/// \endcond

#endif
