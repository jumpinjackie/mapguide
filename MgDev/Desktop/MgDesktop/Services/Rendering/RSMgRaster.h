#ifndef DESKTOP_RS_RASTER_H
#define DESKTOP_RS_RASTER_H

#include "MgDesktop.h"
#include "RS_Raster.h"
#include "RendererStyles.h"

class RSMgdRaster : public RS_Raster
{
public:

    RSMgdRaster(MgRaster* raster);
    virtual ~RSMgdRaster();

    virtual RS_Bounds       GetExtent();

    virtual int             GetOriginalWidth();
    virtual int             GetOriginalHeight();

    virtual int             GetBitsPerPixel();
    virtual int             GetDataModelType();

    virtual RS_InputStream* GetPalette();

    virtual RS_InputStream* GetStream(RS_ImageFormat format,
                                          int width,
                                          int height
                                          );
    virtual int             GetDataType();
    virtual RS_String       GetVerticalUnits();
    virtual int             GetNullValueType();
    virtual INT64           GetNullValueData();

private:

    MgRaster* m_raster;

    int m_width;
    int m_height;
};

#endif