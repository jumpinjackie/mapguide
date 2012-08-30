#include "RSMgRaster.h"
#include "RSMgInputStream.h"

RSMgdRaster::RSMgdRaster(MgRaster* raster)
{
    assert(raster);
    m_raster = raster;
    SAFE_ADDREF(m_raster);

    m_width = m_raster->GetImageXSize();
    m_height = m_raster->GetImageYSize();
}

RSMgdRaster::~RSMgdRaster()
{
    SAFE_RELEASE(m_raster);
}

RS_Bounds RSMgdRaster::GetExtent()
{
    Ptr<MgEnvelope> env = m_raster->GetBounds();

    Ptr<MgCoordinate> ll = env->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = env->GetUpperRightCoordinate();

    return RS_Bounds(ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());
}

int RSMgdRaster::GetOriginalWidth()
{
    return m_width;
}

int RSMgdRaster::GetOriginalHeight()
{
    return m_height;
}

int RSMgdRaster::GetBitsPerPixel()
{
    return m_raster->GetBitsPerPixel();
}

int RSMgdRaster::GetDataModelType()
{
    return m_raster->GetDataModelType();
}


//caller frees returned stream
RS_InputStream* RSMgdRaster::GetStream(RS_ImageFormat format, int width, int height)
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
        return new RSMgdInputStream(rdr.p);
    }
    else
    {
        return NULL;
    }
}

RS_InputStream* RSMgdRaster::GetPalette()
{
    Ptr<MgByte> pal = m_raster->GetPalette();

    if (pal.p)
        return new RSMgdInputStream(pal.p);
    else
        return NULL;
}

int RSMgdRaster::GetDataType()
{
    return m_raster->GetDataType();
}

RS_String RSMgdRaster::GetVerticalUnits()
{
    return m_raster->GetVerticalUnits();
}

int RSMgdRaster::GetNullValueType()
{
    return m_raster->GetNullValueType();
}

INT64 RSMgdRaster::GetNullValueData()
{
    return m_raster->GetNullValue();
}

