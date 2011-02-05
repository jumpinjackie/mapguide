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

#include "stdafx.h"
#include "GridData.h"
#include "RS_Raster.h"
#include "Bounds.h"
#include "RS_InputStream.h"

#ifdef MG_MAX_PATH
#undef MG_MAX_PATH
#endif

#ifdef _WIN32
#define MG_MAX_PATH _MAX_PATH
#else
#define MG_MAX_PATH PATH_MAX
#endif

const float MG_NODATA_VALUE = FLT_MAX;

//*************************************************************************************************************
bool
GetUniqueBandName(
    wchar_t*           pNewBandName,
    GridData*         pGisGrid,
    FdoString*       pBandName
)
{
    if ((pGisGrid) && (pBandName))
    {
        wcscpy(pNewBandName, pBandName);
        int num(0);
        while(pGisGrid->GetBand(pNewBandName) != NULL)
        {
            swprintf(pNewBandName, wcslen(pNewBandName), L"%s%d", pBandName, num);
            num++;
        }
        return true;
    }
    else
        return false;
}

#define READELEVATIONDATA(elevationd_band_type, elevation_fdo_type, function_name) \
            FdoPtr<elevation_fdo_type> val = static_cast<elevation_fdo_type*>(nullValue); \
            elevationd_band_type fdoNullValue= val->function_name(); \
            elevationd_band_type curVal; \
            for (unsigned long y = 0; y < actualRows; ++y) \
            { \
                for (unsigned long x = 0; x < actualCols; ++x) \
                { \
                    pCurPos = (unsigned char*)(pRasterData + y*rowLength + x*bitPerPixel/8 ); \
                    curVal = (*(reinterpret_cast<elevationd_band_type *>(pCurPos))); \
                    if (curVal != fdoNullValue) \
                    { \
                        float val = (float)curVal; \
                        pGisBand->SetValue(colStartPos + x, rowStartPos + y, Band::Double32, &val); \
                    } \
                    else \
                    { \
                        const float nullv = MG_NODATA_VALUE; \
                        pGisBand->SetValue(colStartPos + x, rowStartPos + y, Band::Double32, (float*)&nullv); \
                    } \
                } \
            }


GridData::GridData(const Point2D& point, double xExtent, double yExtent, unsigned int nXCount, unsigned int nYCount):
            m_westSourthPoint(point), m_xExtent (xExtent), m_yExtent (yExtent),
            m_nXCount (nXCount), m_nYCount (nYCount), m_pElevationBand(NULL),
            m_pColorBand(NULL),m_pStylizedBand(NULL), m_pDrapedColorBand(NULL),
            m_dCoordSysUnitLength(1.0), m_dx(0), m_dy(0), m_isdx(0), m_isdy(0)
{
    if (m_nXCount)
    {
        m_dx = m_xExtent / m_nXCount;
        m_isdx = 1.0 / (m_dx * m_dCoordSysUnitLength);
    }

    if (m_nYCount)
    {
        m_dy = m_yExtent / m_nYCount;
        m_isdy = 1.0 / (m_dy * m_dCoordSysUnitLength);
    }

}

//*************************************************************************************************************
GridData::~GridData()
{
    for (GISGRIDVECTOR::iterator itr = m_BandVector.begin();
         itr != m_BandVector.end();
         ++itr)
    {
        delete *itr;
        *itr = NULL;
    }

    delete m_pColorBand;
    m_pColorBand = NULL;

    delete m_pDrapedColorBand;
    m_pDrapedColorBand = NULL;

    delete m_pStylizedBand;
    m_pStylizedBand = NULL;
}

//*************************************************************************************************************

Band* GridData::CreateBand(Band::BandDataType dataType, const MdfModel::MdfString& name)
{
    Band* pBand = new Band(dataType, this);
    if (NULL == pBand)
        return NULL;

    pBand->SetName(name.c_str());

    if (InsertBand(pBand))
    {
        return pBand;
    }

    delete pBand;
    return NULL;
}

//*************************************************************************************************************

Band* GridData::GetBand(const MdfModel::MdfString& name) const
{
    for (GISGRIDVECTOR::const_iterator itr = m_BandVector.begin();
         itr != m_BandVector.end();
         ++itr)
    {
        if (*itr)
        {
            if(0 == name.compare ((*itr)->GetName()))
                return *itr;
        }
    }
    return NULL;
}

//*************************************************************************************************************

Band* GridData::GetBand(unsigned int index) const
{
    if (index >= m_BandVector.size())
    {
        return NULL;
    }

    return m_BandVector.at(index);
}

//*************************************************************************************************************

bool GridData::InsertBand( Band* pBand)
{
    //The band is identity by the name in the grid,
    //so we have check the name is not empty before insert to the grid.
    if (NULL == pBand->GetName())
    {
        return false;
    }

    //Before insert the band to the grid, we will check is this band
    //already in the grid.
    if (NULL != GetBand(pBand->GetName ()))
    {
        return false;
    }

    m_BandVector.push_back ( pBand );
    pBand->SetOwerGrid(this);

    return true;
}

//*************************************************************************************************************

bool GridData::SetElevationBand(const MdfModel::MdfString& name)
{
    Band* pBand = GetBand(name);

    if (NULL == pBand)
    {
        return false;
    }

    m_pElevationBand = pBand;
    return true;
}

//*************************************************************************************************************

bool GridData::SetElevationBand(unsigned int index)
{
    Band* pBand = GetBand(index);

    if (NULL == pBand)
    {
        return false;
    }

    m_pElevationBand = pBand;
    return true;
}

//*************************************************************************************************************

Band* GridData::GetColorBand() const
{
    //Now, the stylization will based on the color band, it always suppose there are exsiting a color band
    //to improve the performace before it call apply stylization.
    //So i create a default color band for it.

    if (NULL == m_pColorBand)
    {
        Band* pColorBand = new Band(Band::UnsignedInt32, const_cast<GridData*>(this));
        m_pColorBand = pColorBand;
    }

    return m_pColorBand;
}

//*************************************************************************************************************

Band* GridData::GetElevationBandDataForStylization()
{
    //Now, the stylization will need a elevation band to stored the stylized elevation band.
    //to improve the performace before it call apply stylization.
    //So i create a default color band for it.

    if (NULL == m_pStylizedBand)
    {
        Band* pStylizedBand = new Band(Band::Double64, const_cast<GridData*>(this));
        m_pStylizedBand = pStylizedBand;
    }

    return m_pStylizedBand;
}

Band* GridData::GetDrapedColorBand()    const
{
    return m_pDrapedColorBand;
}

void GridData::CreateDrapedColorBand()
{
    if (NULL == m_pDrapedColorBand)
    {
        m_pDrapedColorBand = new Band(Band::UnsignedInt32, const_cast<GridData*>(this));
    }
}

const Band* GridData::GetCacheHillShadeBand(const MdfModel::HillShade *pHS) const
{
    if (NULL != this->m_spMdfHillShade.get()
        && this->m_spMdfHillShade->GetAzimuth() == pHS->GetAzimuth()
        && this->m_spMdfHillShade->GetAltitude() == pHS->GetAltitude()
        && this->m_spMdfHillShade->GetScaleFactor() == pHS->GetScaleFactor()
        && this->m_spMdfHillShade->GetBand() == pHS->GetBand()
        && NULL != this->GetBand(pHS->GetBand())
        && NULL != this->m_spHillShadeBand.get())
    { // Cached
        return this->m_spHillShadeBand.get();
    }
    return NULL;
}

bool GridData::SetCacheHillShadeBand(
                                    const MdfModel::HillShade *pHS,
                                    Band                   *pHSBand)
{
    if (NULL == pHS
        || NULL == this->GetBand(pHS->GetBand())
        || NULL == pHSBand)
    {
        return false;
    }

    if (NULL == this->m_spMdfHillShade.get())
    {
        this->m_spMdfHillShade.reset(new MdfModel::HillShade);
    }
    this->m_spMdfHillShade->SetAzimuth(pHS->GetAzimuth());
    this->m_spMdfHillShade->SetAltitude(pHS->GetAltitude());
    this->m_spMdfHillShade->SetScaleFactor(pHS->GetScaleFactor());
    this->m_spMdfHillShade->SetBand(pHS->GetBand());
    this->m_spHillShadeBand.reset(pHSBand);

    return true;
}

Band* GridData::GetNoHillShadeColorBand()
{
    return this->m_spNoHillShadeColorBand.get();
}

const Band* GridData::GetNoHillShadeColorBand() const
{
    return this->m_spNoHillShadeColorBand.get();
}

bool GridData::SetNoHillShadeColorBand(Band *pNoHillShadeColorBand)
{
    this->m_spNoHillShadeColorBand.reset(pNoHillShadeColorBand);
    return true;
}

void GridData::ReadRaster( RS_Raster*      pRaster,
                           FdoString*      pBandName,
                           unsigned long    cols,
                           unsigned long    rows,
                           double           xMin,
                           double           yMin,
                           double           xMax,
                           double           yMax,
                           bool             bAlignment
)
{
    Band* pGisBand = NULL;
    RS_InputStream* reader = NULL;
    FdoByte* pRasterData = NULL;
    FdoByte* pPaletteBuf = NULL;
    wchar_t pUniqueBandName[MG_MAX_PATH] = { 0 };
    try
    {
        //FdoPtr<FdoRasterDataModel> dataModel = pRaster->GetDataModel();
        //FdoPtr<FdoByteArray> byteArray = pRaster->GetBounds();
        //FdoPtr<FdoFgfGeometryFactory> pAwkbFactory = FdoFgfGeometryFactory::GetInstance();
        //FdoPtr<FdoIGeometry> pGeometry = pAwkbFactory->CreateGeometryFromFgf(byteArray);
        //FdoPtr<FdoIEnvelope> envelope = pGeometry->GetEnvelope();
        FdoInt32 bitPerPixel = pRaster->GetBitsPerPixel();
        FdoRasterDataModelType dataModelType = (FdoRasterDataModelType)pRaster->GetDataModelType();
        RS_Bounds env(pRaster->GetExtent());
        // Compute the acutal pixel size of returned raster
        unsigned long actualCols = (unsigned long) (((env.maxx - env.minx) / (xMax - xMin)) * cols + 0.5);
        unsigned long actualRows = (unsigned long) (((env.maxy - env.miny) / (yMax - yMin)) * rows + 0.5);
        unsigned long colStartPos = (unsigned long) (((env.minx - xMin) / (xMax - xMin)) * cols + 0.5);
        unsigned long rowStartPos = (unsigned long) (((yMax - env.maxy) / (yMax - yMin)) * rows + 0.5);

        //pRaster->SetImageXSize(actualCols);
        //pRaster->SetImageYSize(actualRows);
        reader = pRaster->GetStream(/*this value is ignored*/RS_ImageFormat_ABGR, actualCols, actualRows);

        if (reader)
        {
            //FireOnStepQuery();
            // Compute the row length of image. If bAlignment is true, bits size of each row must be a multiple of 8 byte.
            unsigned long rowBitSize = (actualCols)*bitPerPixel;
            unsigned long rowLength;
            if (bAlignment)
                rowLength = (rowBitSize%64)? ((rowBitSize + 64 - rowBitSize%64) / 8):(rowBitSize/8);
            else
                rowLength = rowBitSize/8;
            unsigned long bufferLength = rowLength * actualRows;

            if (reader->available() != bufferLength)
            {
                delete reader;
                return;
            }

            //Create an buffer to hold data
            pRasterData = new FdoByte[bufferLength];
            if (NULL == pRasterData)
            {
                throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(CLNT_5_OUTOFMEMORY)));
            }

            reader->read(pRasterData, bufferLength);

            // Get grid data type
            Band::BandDataType gridDataType = GetGridDataType(pRaster);
            GetUniqueBandName(pUniqueBandName, this,  pBandName);

            pGisBand = CreateBand(gridDataType, pUniqueBandName);
            FdoString* pVerticalUnit = pRaster->GetVerticalUnits().c_str();
            pGisBand->SetUnitName(pVerticalUnit);

            // Set NULL value for Band
            if (dataModelType == FdoRasterDataModelType_Data)
            {
                //TODO: Are we guaranteed
                //that FLT_MAX will always indicate the NODATA value for all grids?
                if (gridDataType == Band::Double32)
                {
                    pGisBand->SetNullValue(Band::Double32, (void*)&MG_NODATA_VALUE);
                }
                else
                {
                    double nullval = DBL_MAX;
                    pGisBand->SetNullValue(Band::Double64, (void*)&nullval);
                }
            }
            else if ((dataModelType == FdoRasterDataModelType_RGB)
                || (dataModelType == FdoRasterDataModelType_RGBA))
            {
                unsigned int nullValue = 0x00000000;
                pGisBand->SetAllToValue(gridDataType, (char*)&nullValue);
            }
            else
            {
                unsigned long nullValue(0);
                pGisBand->SetAllToValue(gridDataType, (char*)(&nullValue));
            }

            unsigned char* pCurPos;
            FdoRasterDataType rasterDataType = (FdoRasterDataType)pRaster->GetDataType();
            if ((dataModelType == FdoRasterDataModelType_RGB)
                || (dataModelType == FdoRasterDataModelType_RGBA))
            {
                unsigned int color;
                if (24 == bitPerPixel)
                {
                    for (unsigned long y = 0; y < actualRows; ++y)
                    {
                        //FireOnStepQuery();
                        for (unsigned long x = 0; x < actualCols; ++x)
                        {
                            pCurPos = (unsigned char*)(pRasterData + y*rowLength + x*bitPerPixel/8 );
                            color = ( 0xFF000000 | ((*(pCurPos)) << 16) | ((*(pCurPos + 1) )<< 8) | (*(pCurPos + 2)));
                            pGisBand->SetValue(colStartPos + x, rowStartPos + y, Band::UnsignedInt32, &color );
                        }
                    }
                }
                else if (48 == bitPerPixel) //BUG: 48 bpp should not be using a 32 bit grid size!
                {
                    for (unsigned long y = 0; y < actualRows; ++y)
                    {
                        //FireOnStepQuery();
                        for (unsigned long x = 0; x < actualCols; ++x)
                        {
                            pCurPos = (unsigned char*)(pRasterData + y*rowLength + x*bitPerPixel/8 );
                            color = ( 0xFF000000 | ((*(pCurPos)) << 32) | ((*(pCurPos + 2)) << 16) | (*(pCurPos + 4)) << 8 );
                            pGisBand->SetValue(colStartPos + x, rowStartPos + y, Band::UnsignedInt32, &color );//BUG: 48 bpp should not be using a 32 bit grid size!
                        }
                    }
                }
                else //32 bpp
                {
                    //allocate a row where we will do pixel conversion
                    unsigned char* row = new unsigned char[actualCols * 4];

                    for (unsigned long y=0; y<actualRows; ++y)
                    {
                        //FireOnStepQuery();

                        //convert from RGBA to BGRA
                        pCurPos = pRasterData + y*rowLength; //source pixel pointer
                        unsigned char* pix = row; //destination pixel pointer

                        for (unsigned int x=0; x<actualCols; ++x)
                        {
                            pix[1] = pCurPos[1];
                            pix[3] = pCurPos[3];
                            pix[2] = pCurPos[0];
                            pix[0] = pCurPos[2];

                            pCurPos += 4;
                            pix += 4;
                        }

                        //set the whole row into the grid
                        pGisBand->SetRowValue(0, y, Band::UnsignedInt32, (char*)row, actualCols);
                    }

                    delete [] row;
                }
            }
            else if (dataModelType == FdoRasterDataModelType_Palette)
            {
                //FdoPtr<FdoIRasterPropertyDictionary> propDict = pRaster->GetAuxiliaryProperties();
                //FdoPtr<FdoDataValue> pal = propDict->GetProperty(L"Palette");
                //FdoLOBValue* palLOB = static_cast<FdoLOBValue*>(pal.p);
                struct RgbColor
                {
                    union {
                        struct { FdoByte red; FdoByte green; FdoByte blue; FdoByte alpha; } rgba;
                        unsigned int packed;
                    };
                };

                unsigned int color;
                unsigned int pos;
                if (bitPerPixel >= 8)
                {
                    RS_InputStream* pStream = pRaster->GetPalette();
                    pPaletteBuf = new FdoByte[pStream->available()];
                    if (NULL == pPaletteBuf)
                    {
                        throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(CLNT_5_OUTOFMEMORY)));
                    }

                    pStream->read(pPaletteBuf, pStream->available());
                    RgbColor* palette = reinterpret_cast<RgbColor*>(pPaletteBuf);

                    for (unsigned long y = 0; y < actualRows; ++y)
                    {
                        //FireOnStepQuery();
                        for (unsigned long x = 0; x< actualCols; ++x)
                        {
                            pCurPos = (unsigned char*)(pRasterData + y*rowLength + x*bitPerPixel/8 );
                            switch(bitPerPixel)
                            {
                                case 8:
                                    pos = *(reinterpret_cast<UINT8*>(pCurPos));
                                    break;
                                case 16:
                                    pos = *(reinterpret_cast<UINT16*>(pCurPos));
                                    break;
                                case 32:
                                    pos = *(reinterpret_cast<UINT32*>(pCurPos));
                                    break;
                                default:
                                    pos = 0;
                                    break;
                            }
                            color = 0xFF000000 | palette[pos].rgba.red << 16 | palette[pos].rgba.green << 8 | palette[pos].rgba.blue;
                            pGisBand->SetValue(colStartPos + x, rowStartPos + y, Band::UnsignedInt32, &color );
                        }
                    }
                }
                else
                {
                    char value, bitNum, temp;
                    unsigned long byteNum;

                    for (unsigned long y = 0; y < actualRows; ++y)
                    {
                        bitNum = 0;
                        value = 0;
                        temp = 0;
                        byteNum = 0;
                        //FireOnStepQuery();
                        for (unsigned long x = 0; x < actualCols; ++x)
                        {
                            if (bitNum >= 8)
                            {
                                bitNum = 0;
                                byteNum++;
                            }
                            if (bitNum == 0)
                                temp = (*((char*)(pRasterData + y*rowLength + byteNum)));

                            value = (char)((temp << (8 - bitNum - bitPerPixel)) >> (8 - bitPerPixel));
                            pGisBand->SetValue(x, y, gridDataType, &value);
                            bitNum += bitPerPixel;
                        }
                    }
                }
            }
            else if ((dataModelType == FdoRasterDataModelType_Data) &&
                     ((rasterDataType == FdoRasterDataType_Integer) ||
                      (rasterDataType == FdoRasterDataType_UnsignedInteger)))
            {
                Band::BandDataType bandDataType = GetGridDataType(pRaster, false);
                FdoDataType type = (FdoDataType)pRaster->GetNullValueType();
                FdoDataValue* nullValue = NULL;
                switch(type)
                {
                case FdoDataType_Double:
                    nullValue = FdoDoubleValue::Create((double)pRaster->GetNullValueData());
                    break;
                case FdoDataType_Single:
                    nullValue = FdoSingleValue::Create((float)pRaster->GetNullValueData());
                    break;
                case FdoDataType_Int16:
                    nullValue = FdoInt16Value::Create((FdoInt16)pRaster->GetNullValueData());
                    break;
                case FdoDataType_Int32:
                    nullValue = FdoInt32Value::Create((FdoInt32)pRaster->GetNullValueData());
                    break;
                case FdoDataType_Int64:
                    nullValue = FdoInt64Value::Create(pRaster->GetNullValueData());
                    break;
                default: break;
                }

                switch (bandDataType)
                {
                case Band::UnsignedInt8:
                    {
                        READELEVATIONDATA(UINT8, FdoByteValue, GetByte);
                        break;
                    }
                case Band::UnsignedInt16:
                    {
                        READELEVATIONDATA(UINT16, FdoInt16Value, GetInt16);
                        break;
                    }
                case Band::UnsignedInt32:
                    {
                        READELEVATIONDATA(UINT32, FdoInt32Value, GetInt32);
                        break;
                    }
                case Band::UnsignedInt64:
                    {
                        READELEVATIONDATA(UINT64, FdoInt64Value, GetInt64);
                        break;
                    }
                case Band::Int8:
                    {
                        READELEVATIONDATA(INT8, FdoByteValue, GetByte);
                        break;
                    }
                case Band::Int16:
                    {
                        READELEVATIONDATA(INT16, FdoInt16Value, GetInt16);
                        break;
                    }
                case Band::Int32:
                    {
                        READELEVATIONDATA(INT32, FdoInt32Value, GetInt32);
                        break;
                    }
                case Band::Int64:
                    {
                        READELEVATIONDATA(INT64, FdoInt64Value, GetInt64);
                        break;
                    }
                case Band::Double32:
                    {
                        READELEVATIONDATA(float, FdoSingleValue, GetSingle);
                        break;
                    }
                case Band::Double64:
                    {
                        READELEVATIONDATA(double, FdoSingleValue, GetSingle);
                        break;
                    }
                }
                if(nullValue)
                    nullValue->Release();
            }
            else
            {
                for (unsigned long y = 0; y < actualRows; ++y)
                {
                    //FireOnStepQuery();
                    pGisBand->SetRowValue(colStartPos, rowStartPos + y, gridDataType,
                                            (char*)(pRasterData + y*rowLength), actualCols);
                }
            }

            // TODO:  When we get past the Mako crunch, we should revisit and probably remove this code.
            //        Leaf, Brett and I agree that we should not completely remove this code at this
            //        time.  I changed it from 25000 to 30000, so we can map anything above the earth's
            //        surface in feet (Mt Everest is 28717 feet in elevation).  - lap - 3/10/06
            // When new version of FDO RFP, which modifies resampling algorithm for dem file to BSpline, is avaiable,
            // we will remove code to smooth elevation and use the following code to recover modified NULL value to its original value.
            // NOTE:  The smoothing code has been removed.
            if (dataModelType == FdoRasterDataModelType_Data)
            {
                float fNullValue = MG_NODATA_VALUE;
                double dCurValue(0.0);
                for (unsigned long y = rowStartPos; (y < actualRows + rowStartPos); ++y)
                {
                    //FireOnStepQuery();
                    for (unsigned long x = colStartPos; (x < actualCols + colStartPos); ++x)
                    {
                        // If elevation value is greater than 30000, we will consider it a NULL value
                        bool ret = pGisBand->GetValueAsDouble(x, y, dCurValue);
                        if (!ret || (dCurValue > 30000.0)||(dCurValue < -30000.0))
                        {
                            pGisBand->SetValue(x, y, gridDataType, (char*)(&fNullValue));
                        }
                    }
                }
            }
            else if (dataModelType == FdoRasterDataModelType_Bitonal)
            {
                //Fix DID 885517
            }
        }
    }
    catch(...)
    {
        delete [] pRasterData;
        delete [] pPaletteBuf;
        delete reader;

        throw;
    }

    delete [] pRasterData;
    delete [] pPaletteBuf;
    delete reader;
}

Band::BandDataType
GridData::GetGridDataType(RS_Raster* pRaster, bool bBandDataType) const
{
    Band::BandDataType gridDataType = Band::UnsignedInt8;
    FdoInt32 bitPerPixel = pRaster->GetBitsPerPixel();
    FdoRasterDataType rasterDataType = (FdoRasterDataType)pRaster->GetDataType();

    if (bBandDataType)
    {
        if (FdoRasterDataModelType_Palette == (FdoRasterDataModelType)pRaster->GetDataModelType())
        {
            return Band::UnsignedInt32;
        }
        else if (FdoRasterDataModelType_Data == (FdoRasterDataModelType)pRaster->GetDataModelType())
        {
            return Band::Double32;
        }
    }

    switch (bitPerPixel)
    {
        case 1:
            gridDataType = Band::Boolean;
            break;
        case 2:
            gridDataType = Band::Bit2;
            break;
        case 4:
            gridDataType = Band::Bit4;
            break;
        case 8:
            {
                switch (rasterDataType)
                {
                    case FdoRasterDataType_UnsignedInteger:
                        gridDataType = Band::UnsignedInt8;
                        break;
                    case FdoRasterDataType_Integer:
                        gridDataType = Band::Int8;
                        break;
                    default :
                        gridDataType = Band::UnsignedInt8;
                        break;
                }
                break;
            }
        case 16:
            {
                switch (rasterDataType)
                {
                    case FdoRasterDataType_UnsignedInteger:
                        gridDataType = Band::UnsignedInt16;
                        break;
                    case FdoRasterDataType_Integer:
                        gridDataType = Band::Int16;
                        break;
                    case FdoRasterDataType_Float:
                    case FdoRasterDataType_Double:
                        //should not happen
                        break;
                    default:
                        gridDataType = Band::UnsignedInt16;
                        break;
                }
                break;
            }
        case 24:
            {
                switch (rasterDataType)
                {
                    case FdoRasterDataType_UnsignedInteger:
                        gridDataType = Band::UnsignedInt32;
                        break;
                    case FdoRasterDataType_Integer:
                        gridDataType = Band::Int32;
                        break;
                    default:
                        gridDataType = Band::UnsignedInt32;
                        break;
                }
                break;
            }

        case 32:
            {
                switch (rasterDataType)
                {
                    case FdoRasterDataType_UnsignedInteger:
                        gridDataType = Band::UnsignedInt32;
                        break;
                    case FdoRasterDataType_Integer:
                        gridDataType = Band::Int32;
                        break;
                    case FdoRasterDataType_Float:
                    case FdoRasterDataType_Double:
                        gridDataType = Band::Double32;
                        break;
                    default:
                        gridDataType = Band::UnsignedInt32;
                        break;
                }
                break;
            }
        case 48:
            {
                switch (rasterDataType)
                {
                    case FdoRasterDataType_UnsignedInteger:
                        gridDataType = Band::UnsignedInt32;
                        break;
                    case FdoRasterDataType_Integer:
                        gridDataType = Band::Int32;
                        break;
                    default:
                        gridDataType = Band::UnsignedInt32;
                        break;
                }
                break;
            }
        case 64:
            {
                switch (rasterDataType)
                {
                    case FdoRasterDataType_UnsignedInteger:
                        gridDataType = Band::UnsignedInt64;
                        break;
                    case FdoRasterDataType_Integer:
                        gridDataType = Band::Int64;
                        break;
                    case FdoRasterDataType_Float:
                    case FdoRasterDataType_Double:
                        gridDataType = Band::Double64;
                        break;
                    default:
                        gridDataType = Band::UnsignedInt64;
                        break;
                }
                break;
            }
    }

    return gridDataType;
}
