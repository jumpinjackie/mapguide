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

#include "GDRenderer.h"
#include "LineStyle.h"
#include "SLDSymbols.h"
#include "LineStyleDef.h"

#include "dwfcore/File.h"
#include "dwfcore/String.h"
#include "dwfcore/Exception.h"
#include "dwfcore/MIME.h"
#include "dwfcore/StreamFileDescriptor.h"
#include "dwfcore/InputStream.h"
#include "dwfcore/FileInputStream.h"
#include "dwfcore/BufferOutputStream.h"
#include "dwfcore/BufferInputStream.h"

#include "dwf/Version.h"
#include "dwf/package/Constants.h"
#include "dwf/package/Manifest.h"
#include "dwf/package/EPlotSection.h"
#include "dwf/package/GlobalSection.h"
#include "dwf/package/DefinedObject.h"
#include "dwf/package/writer/PackageWriter.h"
#include "dwf/package/reader/PackageReader.h"

//GD headers
#include "gd.h"

#include "FontManager.h"
#include "GDUtils.h"

#include "GDFillPatterns.h"
#include "GDW2DRewriter.h"
#include "DWFRSInputStream.h"
#include "RSDWFInputStream.h"
#include "LabelRenderer.h"
#include "LabelRendererLocal.h"
#include "complex_polygon_gd.h"

#include "SymbolTrans.h"
#include "RenderUtil.h"
#include "SE_SymbolDefProxies.h"

using namespace DWFToolkit;
using namespace DWFCore;

#define ROUND(x) (int)(floor(x+0.5))

#define SYMBOL_BITMAP_SIZE 128
#define SYMBOL_BITMAP_MAX 1024

// maximum allowed size for images
#define IMAGE_SIZE_MAX 2048.0*2048.0


// Dummy class used to automate initialization/uninitialization of GD.
class CInitGD
{
public:
    CInitGD()
    {
        // initialize font cache
        gdFontCacheSetup();
    }

    ~CInitGD()
    {
        // free GD resources
        gdFontCacheShutdown();
    }
};

static CInitGD sg_InitGD;


bool GDRenderer::s_bGeneralizeData = false;


//default constructor
GDRenderer::GDRenderer(int width,
                       int height,
                       RS_Color& bgColor,
                       bool requiresClipping,
                       bool localOverposting,
                       double tileExtentOffset)
:
m_width(width),
m_height(height),
m_bgcolor(bgColor),
m_extents(0.0, 0.0, 0.0, 0.0),
m_wtPointBuffer(NULL),
m_wtPointLen(0),
m_symbolManager(NULL),
m_mapInfo(NULL),
m_layerInfo(NULL),
m_featureClassInfo(NULL),
m_bRequiresClipping(requiresClipping),
m_bIsSymbolW2D(false),
m_bHaveViewport(false),
m_imsym(NULL),
m_input(NULL),
m_xformer()
{
    if (m_width <= 0)
        m_width = 1;
    if (m_height <= 0)
        m_height = 1;

    //set max line width to the screen diagonal
    m_maxLineWidth = (int)(0.5 + sqrt((double)(m_width * m_width + m_height * m_height)));

    gdImagePtr img = gdImageCreateTrueColor(m_width, m_height);
    m_imout = img;

    int bgc = ConvertColor(img, bgColor);

    // initialize the image to the supplied color (temporarily turn
    // off alpha blending so the fill has the supplied alpha)
    gdImageAlphaBlending(img, 0);
    gdImageFilledRectangle(img, 0, 0, gdImageSX(img)-1, gdImageSY(img)-1, bgc);
    gdImageAlphaBlending(img, 1);

    // set any transparent color
    if (bgColor.alpha() != 255)
        gdImageColorTransparent(img, bgc);

    if (!localOverposting)
        m_labeler = new LabelRenderer(this);
    else
        m_labeler = new LabelRendererLocal(this, tileExtentOffset);

    m_polyrasterizer = new complex_polygon_gd();
}


GDRenderer::~GDRenderer()
{
    gdImageDestroy((gdImagePtr)m_imout);

    if (m_imsym)
        gdImageDestroy((gdImagePtr)m_imsym);

    delete [] m_wtPointBuffer;
    delete m_labeler;
    delete m_polyrasterizer;
}


void GDRenderer::Save(const RS_String& filename, const RS_String& format)
{
    Save(filename, format, m_width, m_height);
}


void GDRenderer::Save(const RS_String& filename, const RS_String& format, int width, int height)
{
    //get the in-memory image stream
    auto_ptr<RS_ByteData> data;
    data.reset(Save(format, width, height));

    if (NULL == data.get())
    {
        return;
    }

    //We write to a file to avoid sending a FILE* across DLL
    //boundary, which is not always safe.

    //TODO: take care of this string conversion -- shouldn't matter
    //since it is only a temp file name anyway, and they are ascii
    char mbfilename[1024];
    wcstombs(mbfilename, filename.c_str(), 1024);

    FILE* out = fopen(mbfilename, "wb");
    _ASSERT(out);

    if (out)
    {
        unsigned char* bytes = data->GetBytes();
        unsigned int numBytes = data->GetNumBytes();
        if (fwrite(bytes, 1, numBytes, out) != numBytes)
            _ASSERT(false);
    }

    fclose(out);
}


RS_ByteData* GDRenderer::Save(const RS_String& format, int width, int height)
{
    gdImagePtr im = NULL;

    if (width <= 0)
        width = 1;
    if (height <= 0)
        height = 1;

    //do we need to stretch to non-square pixels?
    if (width != m_width || height != m_height || format == L"PNG8")
    {
        if(format == L"PNG8")
            im = gdImageCreate(width, height);
        else
            im = gdImageCreateTrueColor(width, height);

        int bgc = ConvertColor(im, m_bgcolor);

        // initialize the destination image to the bg color (temporarily turn
        // off alpha blending so the fill has the supplied alpha)
        gdImageAlphaBlending(im, 0);
        gdImageFilledRectangle(im, 0, 0, gdImageSX(im)-1, gdImageSY(im)-1, bgc);

        // set any transparent color
        if (m_bgcolor.alpha() != 255)
            gdImageColorTransparent(im, bgc);

        gdImageCopyResized(im, (gdImagePtr)m_imout, 0, 0, 0, 0, width, height, m_width, m_height);
        gdImageAlphaBlending(im, 1);
    }
    else
        im = (gdImagePtr)m_imout;

    // Make output image non-interlaced --- it's a little faster to compress that way.
    gdImageInterlace(im, 0);

    // Make sure the alpha values get saved -- but only if required
    // it is faster not to save them and makes a smaller PNG
    if (m_bgcolor.alpha() != 255)
        gdImageSaveAlpha(im, 1);
    else
        gdImageSaveAlpha(im, 0);

    //get an in-memory image stream
    int size = 0;
    unsigned char* data = NULL;

    // TODO: Tiff
    if (format == L"GIF")       // MgImageFormats::Gif
        data = (unsigned char*)gdImageGifPtr(im, &size);
    else if (format == L"JPG")  // MgImageFormats::Jpeg
        data = (unsigned char*)gdImageJpegPtr(im, &size, 75);
//  else if (format == L"TIF")  // MgImageFormats::Tiff
//      data = (unsigned char*)gdImageTiffPtr(im, &size);
    else if (format == L"PNG"
        || format == L"PNG8")   // MgImageFormats::Png || MgImageFormats::Png8
        data = (unsigned char*)gdImagePngPtr(im, &size);
    else                        // PNG is the default
        data = (unsigned char*)gdImagePngPtr(im, &size);

    auto_ptr<RS_ByteData> byteData;

    byteData.reset((NULL == data)? NULL : new RS_ByteData(data, size));

    gdFree(data);

    //if we allocated a temporary image to stretch-blit, destroy it
    if (im != m_imout)
        gdImageDestroy(im);

    return byteData.release();
}


void GDRenderer::Combine(const RS_String& fileIn1, const RS_String& fileIn2, const RS_String& fileOut)
{
    char mbfileIn1[1024];
    char mbfileIn2[1024];
    char mbfileOut[1024];
    wcstombs(mbfileIn1, fileIn1.c_str(), 1024);
    wcstombs(mbfileIn2, fileIn2.c_str(), 1024);
    wcstombs(mbfileOut, fileOut.c_str(), 1024);

    // read in the images
    FILE* fIn1 = fopen(mbfileIn1, "rb");
    FILE* fIn2 = fopen(mbfileIn2, "rb");
    gdImagePtr img1 = gdImageCreateFromPng(fIn1);
    gdImagePtr img2 = gdImageCreateFromPng(fIn2);
    fclose(fIn1);
    fclose(fIn2);

    // composite the 2nd image onto the 1st
    gdImageSetTile(img1, img2);
    gdImageFilledRectangle(img1, 0, 0, gdImageSX(img1)-1, gdImageSY(img1)-1, gdTiled);
    gdImageSaveAlpha(img1, 1);

    // save the merged image
    FILE* out = fopen(mbfileOut, "wb");
    gdImagePng(img1, out);
    fclose(out);

    // clean up
    gdImageDestroy(img1);
    gdImageDestroy(img2);
}


void GDRenderer::StartMap(RS_MapUIInfo* mapInfo,
                          RS_Bounds&    extents,
                          double        mapScale,
                          double        dpi,
                          double        metersPerUnit,
                          CSysTransformer* /*xformToLL*/)
{
    m_mapScale = mapScale;
    m_dpi = dpi;
    m_metersPerUnit = metersPerUnit;
    m_extents = extents;

    //compute drawing scale
    //drawing scale is map scale converted to [mapping units] / [pixels]
    double metersPerPixel = METERS_PER_INCH / m_dpi;
    m_drawingScale = m_mapScale * metersPerPixel / m_metersPerUnit;

    //find scale used to convert to pixel coordinates
    //need to take aspect ratios into account
    double arDisplay = (double)m_width / (double)m_height;
    double arMap = m_extents.width() / m_extents.height();

    if (arDisplay > arMap)
        m_scale = (double)m_height / m_extents.height();
    else
        m_scale = (double)m_width / m_extents.width();

    m_invScale = 1.0 / m_scale;

    m_offsetX = m_extents.minx;
    m_offsetY = m_extents.miny;

    m_labeler->StartLabels();

    // remember the map info
    m_mapInfo = mapInfo;

    // make sure that the mapInfo is in sync with the background member variable
    if (m_mapInfo)
        m_mapInfo->bgcolor() = this->m_bgcolor;

    // do it here, since we will need the renderer's map scales, which are computed above
    InitFontEngine(this);
}


void GDRenderer::EndMap()
{
    // turn off selection mode so the labels draw normal
    SetRenderSelectionMode(false);

    // finally draw all the labels
    m_labeler->BlastLabels();

    // clear the map info
    m_mapInfo = NULL;
}


void GDRenderer::StartLayer(RS_LayerUIInfo*      layerInfo,
                            RS_FeatureClassInfo* classInfo)
{
    // remember the layer/feature info
    m_layerInfo = layerInfo;
    m_featureClassInfo = classInfo;
}


void GDRenderer::EndLayer()
{
    // clear the layer/feature info
    m_layerInfo = NULL;
    m_featureClassInfo = NULL;
}


void GDRenderer::StartFeature(RS_FeatureReader* /*feature*/,
                              bool              /*initialPass*/,
                              const RS_String*  /*tooltip*/,
                              const RS_String*  /*url*/,
                              const RS_String*  /*theme*/,
                              double            /*zOffset*/,
                              double            /*zExtrusion*/,
                              RS_ElevationType  /*zOffsetType*/)
{
}


void GDRenderer::ProcessPolygon(LineBuffer* lb, RS_FillStyle& fill)
{
    _ASSERT(NULL != lb);
    RS_FillStyle* use_fill = &fill;

    //should we use selection style?
    if (m_bSelectionMode)
        use_fill = &m_selFill;

    if (lb->point_count() == 0)
        return;

    //render any polygon fill
    if (use_fill->color().alpha() != 0)
    {
        _TransformPointsNoClamp(lb);

        int gdc = ConvertColor((gdImagePtr)m_imout, use_fill->color());
        int gdcbg = ConvertColor((gdImagePtr)m_imout, use_fill->background());

        gdImagePtr fillpat = NULL;

        if (wcscmp(use_fill->pattern().c_str(), L"Solid") != 0)
        {
            fillpat = GDFillPatterns::CreatePatternBitmap(use_fill->pattern().c_str(), gdc, gdcbg);
            gdImageSetTile((gdImagePtr)m_imout, fillpat);
        }

        //call the new rasterizer
        m_polyrasterizer->FillPolygon((Point*)m_wtPointBuffer, lb->point_count(), lb->cntrs(), lb->cntr_count(),
            fillpat? gdTiled : gdc, (gdImagePtr)m_imout);

        if (fillpat)
        {
            gdImageSetTile((gdImagePtr)m_imout, NULL);
            gdImageDestroy(fillpat);
        }
    }

    //render any polygon outline
    RS_LineStroke& use_lsym = use_fill->outline();
    if (use_lsym.color().alpha() == 0)
        return;

    LineBuffer* workbuffer = lb;
    std::auto_ptr<LineBuffer> spLB;

    if (s_bGeneralizeData)
    {
        if (workbuffer->point_count() > 6)
        {
            LineBuffer* optbuffer = workbuffer->Optimize(m_drawingScale, m_pPool);
            if (optbuffer)
            {
                workbuffer = optbuffer;
                spLB.reset(workbuffer);
            }
        }
    }

    //apply line style if needed
    if ((wcscmp(use_lsym.style().c_str(), L"Solid") != 0))
    {
        //TODO: we should simplify the math that does all that pixel-based stuff
        LineBuffer* newbuffer = ApplyLineStyle(workbuffer, (wchar_t*)use_lsym.style().c_str(),
            use_lsym.width() * m_dpi / METERS_PER_INCH,     // LineStyle works in pixels
            m_drawingScale,                                 // pixels per map unit
            m_dpi);                                         // dpi

        if (newbuffer)
        {
            if (spLB.get())
                LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());

            workbuffer = newbuffer;
            spLB.reset(workbuffer);
        }
    }

    if (workbuffer)
    {
        WritePolylines(workbuffer, use_lsym, true);

        if (spLB.get())
            LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
    }
}


void GDRenderer::ProcessPolyline(LineBuffer* lb, RS_LineStroke& lsym)
{
    _ASSERT(NULL != lb);
    RS_LineStroke* use_lsym = &lsym;

    //are we drawing a selection?
    if (m_bSelectionMode)
        use_lsym = &(m_selFill.outline());

    if (lb->point_count() == 0)
        return;

    //render any polyline outline
    if (use_lsym->color().alpha() == 0)
        return;

    LineBuffer* workbuffer = lb;
    std::auto_ptr<LineBuffer> spLB;

    if (s_bGeneralizeData)
    {
        if (workbuffer->point_count() > 6)
        {
            LineBuffer* optbuffer = workbuffer->Optimize(m_drawingScale, m_pPool);
            if (optbuffer)
            {
                workbuffer = optbuffer;
                spLB.reset(workbuffer);
            }
        }
    }

    //apply line style if needed
    if ((wcscmp(use_lsym->style().c_str(), L"Solid") != 0))
    {
        //TODO: we should simplify the math that does all that pixel-based stuff
        LineBuffer* newbuffer = ApplyLineStyle(workbuffer, (wchar_t*)use_lsym->style().c_str(),
            use_lsym->width() * m_dpi / METERS_PER_INCH,    // LineStyle works in pixels
            m_drawingScale,                                 // pixels per map unit
            m_dpi);                                         // dpi

        if (newbuffer)
        {
            if (spLB.get())
                LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());

            workbuffer = newbuffer;
            spLB.reset(workbuffer);
        }
    }

    if (workbuffer)
    {
        WritePolylines(workbuffer, *use_lsym, true);

        if (spLB.get())
            LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
    }
}


void GDRenderer::ProcessRaster(unsigned char* data,
                               int length,
                               RS_ImageFormat format,
                               int width, int height,
                               RS_Bounds& extents,
                               TransformMesh* /*xformMesh*/)
{
    // TODO: use xformMesh and call AGGRenderer::DrawScreenRasterTransformed
    //

    double cx = 0.5 * (extents.minx + extents.maxx);
    double cy = 0.5 * (extents.miny + extents.maxy);
    WorldToScreenPoint(cx, cy, cx, cy);

    //pass to the screen space render function
    DrawScreenRaster(data, length, format, width, height, cx, cy, extents.width() * m_scale, extents.height() * m_scale, 0.0);
}


void GDRenderer::ProcessMarker(LineBuffer* srclb, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds)
{
    RS_MarkerDef use_mdef = mdef;

    //use the selection style to draw
    if (m_bSelectionMode)
        use_mdef = RS_MarkerDef(RS_MarkerType_Marker,
                                mdef.width(),
                                mdef.height(),
                                mdef.insx(), mdef.insy(),
                                mdef.rotation(),
                                mdef.units(),
                                SLDType_Square,
                                L"", L"",
                                m_selFill);

    for (int i=0; i<srclb->point_count(); i++)
    {
        //if marker is processed from here it should be added to the
        //feature W2D, not the labeling W2D -- need the API to reflect that.
        ProcessOneMarker(srclb->x_coord(i), srclb->y_coord(i), use_mdef, allowOverpost, (i==0)? bounds : NULL);
    }
}


void GDRenderer::ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds)
{
    RS_InputStream* symbol = NULL;

    //attempt to retrieve the symbol
    if (m_symbolManager && (mdef.type() == RS_MarkerType_W2D))
    {
        // BOGUS:
        // We need to pass in "symbols" as the name of the data for the
        // symbol library resource. This is hardcoded right now.
        // If it ever changes, we will need to update it
        symbol = (RS_InputStream*)m_symbolManager->GetSymbolData(mdef.library().c_str(), L"symbols.dwf");
    }

    //symbol in mapping space units
    double iw = _MeterToMapSize(mdef.units(), mdef.width());
    double ih = _MeterToMapSize(mdef.units(), mdef.height());

    //unrotated symbol bounding box (mapping space)
    RS_Bounds dst(x, y, x+iw, y+ih);

    //convert to pixel size
    iw *= m_scale;
    ih *= m_scale;

    //if it's too big, make it smaller
    //it cannot be too big since we allocate iw * ih * 4 bytes of
    //memory to cache the symbol image -- this code simply truncates
    //the size, ignoring changes in aspect ratio
    const double MAX_SIZE = 4096.0;
    if (iw > MAX_SIZE)
    {
        iw = MAX_SIZE;
        dst.maxx = dst.minx + iw / m_scale;
    }
    if (ih > MAX_SIZE)
    {
        ih = MAX_SIZE;
        dst.maxy = dst.miny + ih / m_scale;
    }

    int devwidth = ROUND(iw);
    int devheight = ROUND(ih);

    //get insertion point
    double refX = mdef.insx();
    double refY = mdef.insy();

    //rotation angle
    double angleRad = mdef.rotation() * M_PI180;

    if (mdef.type() == RS_MarkerType_Font)
    {
        //case where we are using a font symbol

        // TODO: cannot easily check for font symbol repetition
        //       since we forward to the labeling logic...
        // check to see if the last symbol we got was the same
//      if (   mdef.type() != m_lastSymbol.type()
//          || mdef.height() != m_lastSymbol.height()
//          || mdef.insx() != m_lastSymbol.insx()
//          || mdef.insy() != m_lastSymbol.insy()
//          || mdef.rotation() != m_lastSymbol.rotation()
//          || mdef.units() != m_lastSymbol.units()
//          || mdef.library() != m_lastSymbol.library()
//          || mdef.name() != m_lastSymbol.name()
//          || mdef.style().color().argb() != m_lastSymbol.style().color().argb()
//          || mdef.fontstyle() != m_lastSymbol.fontstyle())
//      {
            m_lastSymbol = mdef;

        //convert font symbol to a simple label
        RS_TextDef tdef(RS_HAlignment_Center, RS_VAlignment_Half);
        RS_FontDef fdef(mdef.library(), mdef.height(), mdef.fontstyle(), mdef.units());

        tdef.font() = fdef;
        tdef.textcolor() = mdef.style().color();
        tdef.rotation() = mdef.rotation();

        //compute placement position for character centerpoint, taking into
        //account the insertion point.
        //TODO: is this way of computing insertion point offset correct, when
        //compared to what SymbolTrans does? Is rotation applied before the symbol
        //is moved to its insertion point or after? Need to check that.
        double posx = dst.minx + (0.5 - refX) * dst.width();
        double posy = dst.miny + (0.5 - refY) * dst.height();

        // transform insertion point into pixel space
        WorldToScreenPoint(posx, posy, posx, posy);

        // draw the character
        RS_TextMetrics tm;
        if (GetTextMetrics(mdef.name(), tdef, tm, false))
            DrawScreenText(tm, tdef, posx, posy, NULL, 0, 0.0);
    }
    else
    {
        //for non-font symbols we will draw into a temporary image
        //which we can reuse if the symbol is not changing from one
        //feature to the next

        //check to see if the last symbol we got was different or if it
        //is not cached yet
        if (   mdef.type() != m_lastSymbol.type()
            || mdef.width() != m_lastSymbol.width()
            || mdef.height() != m_lastSymbol.height()
            || mdef.insx() != m_lastSymbol.insx()
            || mdef.insy() != m_lastSymbol.insy()
            || mdef.rotation() != m_lastSymbol.rotation()
            || mdef.markernum() != m_lastSymbol.markernum()
            || mdef.library() != m_lastSymbol.library()
            || mdef.name() != m_lastSymbol.name()
            || mdef.style().color().argb() != m_lastSymbol.style().color().argb()
            || mdef.style().outline().color().argb() != m_lastSymbol.style().outline().color().argb()
            || mdef.style().background().argb() != m_lastSymbol.style().background().argb()
            || !m_imsym)
        {
            m_lastSymbol = mdef;

            int imsymw = devwidth;
            int imsymh = devheight;

            if (imsymw > SYMBOL_BITMAP_MAX || imsymh > SYMBOL_BITMAP_MAX)
            {
                //symbol will be too big so we will draw it as
                //geometry directly into the map image instead.
                //So we need to destroy the cached symbol image if any
                if (m_imsym)
                {
                    gdImageDestroy((gdImagePtr)m_imsym);
                    m_imsym = NULL;
                }
            }
            else
            {
                //otherwise allocate a second bitmap where
                //we will cache the symbol image for reuse

                //did we cache an image previously, but it was different size?
                if (m_imsym
                    && (   gdImageSX((gdImagePtr)m_imsym) != imsymw
                        || gdImageSY((gdImagePtr)m_imsym) != imsymh))
                {
                    gdImageDestroy((gdImagePtr)m_imsym);
                    m_imsym = NULL;
                }

                //allocate the symbol image
                if (!m_imsym)
                    m_imsym = gdImageCreateTrueColor(imsymw, imsymh);
            }

            if (symbol)
            {
                //case where we are using a W2D symbol from the library
                _ASSERT(mdef.type() == RS_MarkerType_W2D);

                //default bounds of symbol data in W2D
                //for symbols created by MapGuide Studio
                RS_Bounds src(0.0, 0.0, SYMBOL_MAX, SYMBOL_MAX);
                SymbolTrans st = SymbolTrans(src, dst, refX, refY, angleRad);

                if (m_imsym)
                {
                    //case where we will cache the symbol image

                    //we will use unrotated symbol bounding box
                    //since rotation will be done by the image copy
                    //also we will use a slight boundary offset
                    //hardcoded to 1 pixel so that geometry exactly on the edge
                    //draws just inside the image
                    RS_Bounds dst1(1.0, 1.0, (double)(imsymw-1), (double)(imsymh-1));
                    st = SymbolTrans(src, dst1, 0.0, 0.0, 0.0);

                    m_imw2d = m_imsym;

                    // initialize the temporary symbol image to a transparent background
                    gdImageAlphaBlending((gdImagePtr)m_imsym, 0);
                    gdImageFilledRectangle((gdImagePtr)m_imsym, 0, 0, gdImageSX((gdImagePtr)m_imsym)-1, gdImageSY((gdImagePtr)m_imsym)-1, 0x7f000000);
                    gdImageAlphaBlending((gdImagePtr)m_imsym, 1);
                }
                else
                {
                    //case where we will draw the W2D directly into
                    //the destination image because the symbol is too big
                    m_imw2d = m_imout;
                }

                //also set flags controlling how W2D are rewritten
                //into the destination DWF
                m_bIsSymbolW2D = true;
                m_mdOverrideColors = mdef;

                //copy symbol W2D into destination
                AddDWFContent(symbol, &st, mdef.name(), RS_String(L""), RS_String(L""));

                //make sure we zero out the W2D symbol flags
                m_bIsSymbolW2D = false;
                m_imw2d = NULL;
            }
            else
            {
                //case where we are using an SLD symbol, and fall-through for other cases

                //determine which SLD symbol we need to draw
                //and pick up its polygon point definition
                RS_F_Point* poly = NULL;
                int npts = 0;
                bool found = true;

                if (mdef.markernum() == SLDType_Square)
                {
                    poly = (RS_F_Point*)SLD_SQUARE;
                    npts = sizeof(SLD_SQUARE) / (2 * sizeof(double));
                }
                else if (mdef.markernum() == SLDType_Circle)
                {
                    poly = (RS_F_Point*)SLD_CIRCLE;
                    npts = sizeof(SLD_CIRCLE) / (2 * sizeof(double));
                }
                else if (mdef.markernum() == SLDType_Triangle)
                {
                    poly = (RS_F_Point*)SLD_TRIANGLE;
                    npts = sizeof(SLD_TRIANGLE) / (2 * sizeof(double));
                }
                else if (mdef.markernum() == SLDType_Star)
                {
                    poly = (RS_F_Point*)SLD_STAR;
                    npts = sizeof(SLD_STAR) / (2 * sizeof(double));
                }
                else if (mdef.markernum() == SLDType_Cross)
                {
                    poly = (RS_F_Point*)SLD_CROSS;
                    npts = sizeof(SLD_CROSS) / (2 * sizeof(double));
                }
                else if (mdef.markernum() == SLDType_X)
                {
                    poly = (RS_F_Point*)SLD_X;
                    npts = sizeof(SLD_X) / (2 * sizeof(double));
                }
                else
                {
                    //default or error
                    poly = (RS_F_Point*)SLD_ERROR;
                    npts = sizeof(SLD_ERROR) / (2 * sizeof(double));
                    found = false;
                }

                //fill color
                RS_Color fill = mdef.style().color();
                int gdcfill = ConvertColor((gdImagePtr)m_imout, fill);

                //outline color
                RS_Color outline = mdef.style().outline().color();
                int gdcline = ConvertColor((gdImagePtr)m_imout, outline);

                //see if symbol will be small enough to draw in cached image
                if (m_imsym)
                {
                    int superw = imsymw;
                    int superh = imsymh;

                    //if it is too small, make it bigger
                    //so that we get an antialiased effect -- only
                    //do this for SLD symbols since DWF symbols are too
                    //varied and not all of them look good with smoothing applied
                    if (!symbol && superw < SYMBOL_BITMAP_SIZE/2 && superh < SYMBOL_BITMAP_SIZE/2)
                        superw = superh = SYMBOL_BITMAP_SIZE;

                    gdImagePtr tmp = gdImageCreateTrueColor(superw, superh);

                    //transform to coordinates of temporary image where we
                    //draw symbol before transfering to the map
                    EnsureBufferSize(npts);
                    RS_D_Point* pts = m_wtPointBuffer;

                    double tempx, tempy;
                    for (int i=0; i<npts; i++)
                    {
                        tempx = poly[i].x;
                        tempy = poly[i].y;

                        pts[i].x = (int)(tempx * (superw - 10.0) + 5.0);
                        pts[i].y = (int)((superh - 10.0) - tempy * (superh - 10.0) + 5.0);
                    }

                    // initialize the temporary supersampled symbol image to a transparent background
                    gdImageAlphaBlending(tmp, 0);
                    gdImageFilledRectangle(tmp, 0, 0, gdImageSX(tmp)-1, gdImageSY(tmp)-1, 0x7f000000);
                    gdImageAlphaBlending(tmp, 1);

                    if (!found)
                    {
                        //unknown symbol
                        RS_Color red(255, 0, 0, 255);
                        int line_weight = rs_min(superw, superh) / 17;
                        // line_weight cannot be zero
                        gdImagePtr brush1 = rs_gdImageThickLineBrush((line_weight > 1)? line_weight : 2, red);
                        gdImageSetBrush(tmp, brush1);

                        gdImageOpenPolygon(tmp, (gdPointPtr)pts, npts, gdBrushed);

                        gdImageSetBrush(tmp, NULL);
                        gdImageDestroy(brush1);
                    }
                    else
                    {
                        //draw fill
                        // TODO: When a filled polygon image is down-sampled, a gray false edge is created.
                        // This edge can only be seen when the real edge is not being drawn.
                        gdImageFilledPolygon(tmp, (gdPointPtr)pts, npts, gdcfill);
                        //draw outline with a thickness set so that when scaled down to
                        //th destination image, the outline is still fully visible

                        int line_weight = rs_min(superw, superh) / 17;
                        // line_weight cannot be zero
                        gdImagePtr brush1 = rs_gdImageThickLineBrush((line_weight > 1)? line_weight : 2, outline);
                        gdImageSetBrush(tmp, brush1);

                        gdImageOpenPolygon(tmp, (gdPointPtr)pts, npts, gdBrushed);

                        gdImageSetBrush(tmp, NULL);
                        gdImageDestroy(brush1);
                    }

                    // initialize the real cached symbol image to a transparent background
                    gdImageAlphaBlending((gdImagePtr)m_imsym, 0);
                    gdImageFilledRectangle((gdImagePtr)m_imsym, 0, 0, gdImageSX((gdImagePtr)m_imsym)-1, gdImageSY((gdImagePtr)m_imsym)-1, 0x7f000000);
                    gdImageAlphaBlending((gdImagePtr)m_imsym, 1);

                    //resample the supersampled temporary image into the cached image
                    gdImageCopyResampled((gdImagePtr)m_imsym, tmp, 0, 0, 0, 0, imsymw, imsymh, superw, superh);

                    gdImageDestroy(tmp);
                }
                else
                {
                    //otherwise symbol was too big and must be drawn as a regular polygon

                    //construct transformer
                    RS_Bounds src(0.0, 0.0, 1.0, 1.0);
                    SymbolTrans trans(src, dst, refX, refY, angleRad);

                    //transform to coordinates of temporary image where we
                    //draw symbol before transfering to the map
                    EnsureBufferSize(npts);
                    RS_D_Point* pts = m_wtPointBuffer;

                    double tempx, tempy;
                    for (int i=0; i<npts; i++)
                    {
                        tempx = poly[i].x;
                        tempy = poly[i].y;

                        //unit square to world
                        trans.TransformPoint(tempx, tempy);

                        //world to device
                        pts[i].x = _TX(tempx);
                        pts[i].y = _TY(tempy);
                    }

                    if (!found)
                    {
                        //unknown symbol
                        RS_Color red(255, 0, 0, 255);
                        int gdcred = ConvertColor((gdImagePtr)m_imout, red);

                        gdImageOpenPolygon((gdImagePtr)m_imout, (gdPointPtr)pts, npts, gdcred);
                    }
                    else
                    {
                        //draw fill
                        gdImageSetAntiAliased((gdImagePtr)m_imout, gdcfill);
                        gdImageFilledPolygon((gdImagePtr)m_imout, (gdPointPtr)pts, npts, gdAntiAliased);

                        //draw outline
                        gdImageSetAntiAliased((gdImagePtr)m_imout, gdcline);
                        gdImageOpenPolygon((gdImagePtr)m_imout, (gdPointPtr)pts, npts, gdAntiAliased);
                    }
                }
            }
        }

        if (m_imsym)
        {
            //in case we cached a symbol image, draw it to the main
            //map image

            //get the source image size
            int imsymw = gdImageSX((gdImagePtr)m_imsym);
            int imsymh = gdImageSY((gdImagePtr)m_imsym);

            //construct transformer from cached image space
            //to rotated map space -- we need this in order to
            //take into account the insertion point
            RS_Bounds src(0.0, 0.0, imsymw, imsymh);
            SymbolTrans trans(src, dst, refX, refY, angleRad);

            //initialize 4 corner points of symbol -- we will
            //destructively transform this array to destination map space
            RS_F_Point b[4];
            b[0].x =    0.0; b[0].y =    0.0;
            b[1].x = imsymw; b[1].y =    0.0;
            b[2].x = imsymw; b[2].y = imsymh;
            b[3].x =    0.0; b[3].y = imsymh;

            for (int i=0; i<4; i++)
            {
                //to world space
                trans.TransformPoint(b[i].x, b[i].y);

                //world to screen space
                WorldToScreenPoint(b[i].x, b[i].y, b[i].x, b[i].y);
            }

            //copy symbol image into destination image
            if (mdef.rotation() == 0.0)
            {
                //upper left point
                int ulx = (int)floor(b[3].x);
                int uly = (int)floor(b[3].y);

                //straight copy without resampling since we are
                //guaranteed for the source image size to equal the
                //symbol pixel size
                gdImageCopy((gdImagePtr)m_imout, (gdImagePtr)m_imsym, ulx, uly, 0, 0, imsymw, imsymh);
            }
            else
            {
                //for rotated copy, we need to scale down the image first
                //allocating an extra image here should not be too much of
                //an overhead since it is usually small
                gdImagePtr tmp = gdImageCreateTrueColor(imsymw+2, imsymh+2);

                //make it transparent
                gdImageAlphaBlending(tmp, 0);
                gdImageFilledRectangle(tmp, 0, 0, imsymw+1, imsymh+1, 0x7f000000);
                gdImageAlphaBlending(tmp, 1);

                //straight copy without resampling since we are
                //guaranteed for the source image size to equal the
                //symbol pixel size
                gdImageCopy(tmp, (gdImagePtr)m_imsym, 1, 1, 0, 0, imsymw, imsymh);

                //for rotated gd copy, we need the midpoint of
                //the destination bounds
                double mx = floor(0.5 * (b[0].x + b[2].x));
                double my = floor(0.5 * (b[0].y + b[2].y));

                //draw rotated symbol onto final destination image
                gdImageCopyRotated((gdImagePtr)m_imout, tmp, mx, my, 0, 0, imsymw+2, imsymh+2, ROUND(mdef.rotation()));

                gdImageDestroy(tmp);
            }
        }
    }

    if (!allowOverpost)
    {
        //now add an exclusion region to the file so that
        //labels do not overlap the symbol

        //default bounds of symbol data in W2D
        //for symbols created by MapGuide Studio
        RS_Bounds src(0.0, 0.0, SYMBOL_MAX, SYMBOL_MAX);

        //a square that we will transform into the dst bounds
        RS_F_Point box[4];
        box[0].x = 0.0;
        box[0].y = 0.0;
        box[1].x = SYMBOL_MAX;
        box[1].y = 0.0;
        box[2].x = SYMBOL_MAX;
        box[2].y = SYMBOL_MAX;
        box[3].x = 0.0;
        box[3].y = SYMBOL_MAX;

        //construct transformer -- same as the
        //one used for the actual symbol drawables
        SymbolTrans boxtrans(src, dst, refX, refY, angleRad);

        RS_F_Point pts[4];

        double tempx, tempy;

        for (int i=0; i<4; i++)
        {
            tempx = box[i].x;
            tempy = box[i].y;

            boxtrans.TransformPoint(tempx, tempy);

            pts[i].x = _TX(tempx);
            pts[i].y = _TY(tempy);
        }

        AddExclusionRegion(pts, 4);
    }

    //set actual (unrotated) bounds with new insertion point if a pointer was passed in
    if (bounds)
    {
        bounds->minx =       -refX  * mdef.width();
        bounds->maxx = (1.0 - refX) * mdef.width();
        bounds->miny =       -refY  * mdef.height();
        bounds->maxy = (1.0 - refY) * mdef.height();
    }
}


void GDRenderer::ProcessLabelGroup(RS_LabelInfo*    labels,
                                   int              nlabels,
                                   const RS_String& text,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path,
                                   double           scaleLimit)
{
    //check if we are rendering a selection -- bail if so
    if (m_bSelectionMode)
        return;

    //forward it to the label renderer
    m_labeler->ProcessLabelGroup(labels, nlabels, text, type, exclude, path, scaleLimit);
}


void GDRenderer::AddExclusionRegion(RS_F_Point* fpts, int npts)
{
    m_labeler->AddExclusionRegion(fpts, npts);
}


void GDRenderer::SetSymbolManager(RS_SymbolManager* manager)
{
    m_symbolManager = manager;
}


RS_MapUIInfo* GDRenderer::GetMapInfo()
{
    return m_mapInfo;
}


RS_LayerUIInfo* GDRenderer::GetLayerInfo()
{
    return m_layerInfo;
}


RS_FeatureClassInfo* GDRenderer::GetFeatureClassInfo()
{
    return m_featureClassInfo;
}


double GDRenderer::GetMapScale()
{
    return m_mapScale;
}


double GDRenderer::GetDrawingScale()
{
    return m_drawingScale;
}


double GDRenderer::GetMetersPerUnit()
{
    return m_metersPerUnit;
}


RS_Bounds& GDRenderer::GetBounds()
{
    return m_extents;
}


double GDRenderer::GetDpi()
{
    return m_dpi;
}


bool GDRenderer::RequiresClipping()
{
    return m_bRequiresClipping;
}


bool GDRenderer::RequiresLabelClipping()
{
    // always the same value as RequiresClipping
    return m_bRequiresClipping;
}


bool GDRenderer::SupportsZ()
{
    // Z values in feature geometry are ignored
    return false;
}


bool GDRenderer::SupportsTooltips()
{
    // set to false to disable processing of tooltips
    return false;
}


bool GDRenderer::SupportsHyperlinks()
{
    // set to false to disable processing of hyperlinks
    return false;
}


void GDRenderer::WritePolylines(LineBuffer* srclb, RS_LineStroke& stroke, bool aa)
{
    //apply the stroke first
    int gdc = ConvertColor((gdImagePtr)m_imout, stroke.color());
    gdImageSetAntiAliased((gdImagePtr)m_imout, gdc);

    double thickness = stroke.width();

    //convert thickness to pixels
    int line_weight = (int)(_MeterToMapSize(stroke.units(), fabs(thickness)) * m_scale);
    if (line_weight > m_maxLineWidth)
        line_weight = m_maxLineWidth;

    gdImagePtr brush1 = NULL;
    if (line_weight > 1)
        brush1 = rs_gdImageThickLineBrush(line_weight, stroke.color());

    //draw the lines
    for (int i=0; i<srclb->cntr_count(); i++)
    {
        int cntr_size = srclb->cntr_size(i);

        if (cntr_size > 1)
        {
            _TransformContourPointsNoClamp(srclb, i);

            //draw antialiased only if thickness is single pixel
            if (aa && line_weight <= 1)
                gdImageOpenPolygon((gdImagePtr)m_imout, (gdPointPtr)m_wtPointBuffer, cntr_size, gdAntiAliased);
            else
            {
                gdImageSetBrush((gdImagePtr)m_imout, brush1);
                gdImageOpenPolygon((gdImagePtr)m_imout, (gdPointPtr)m_wtPointBuffer, cntr_size, brush1? gdBrushed : gdc);
            }
        }
    }

    //unset the stroke
    gdImageSetBrush((gdImagePtr)m_imout, NULL);

    if (brush1)
        gdImageDestroy(brush1);
}


//transforms an x coordinate from mapping to screen space
inline int GDRenderer::_TX(double x)
{
    return (int)floor((x - m_offsetX) * m_scale);
}

//transforms a y coordinate from mapping to screen space
inline int GDRenderer::_TY(double y)
{
//  return (int)floor(m_height - (y - m_offsetY) * m_scale);
    return m_height + (int)floor((m_offsetY - y) * m_scale);  // avoids converting m_height to double
}


//transforms an array of input mapping space points into pixel
//coordinates and places them in the point buffer m_wtPointBuffer
void GDRenderer::_TransformPointsNoClamp(LineBuffer* plb)
{
    EnsureBufferSize(plb->point_count());
    int* wpts = (int*)m_wtPointBuffer;

    for (int i=0; i<plb->point_count(); i++)
    {
        *wpts++ = _TX(plb->x_coord(i));
        *wpts++ = _TY(plb->y_coord(i));
    }
}


//transforms an array of input mapping space points into pixel
//coordinates and places them in the point buffer m_wtPointBuffer
void GDRenderer::_TransformContourPointsNoClamp(LineBuffer* plb, int cntr)
{
    EnsureBufferSize(plb->cntr_size(cntr));
    int* wpts = (int*)m_wtPointBuffer;
    int end = plb->contour_end_point(cntr);
    for (int i=plb->contour_start_point(cntr); i<=end; i++)
    {
        *wpts++ = _TX(plb->x_coord(i));
        *wpts++ = _TY(plb->y_coord(i));
    }
}


void GDRenderer::EnsureBufferSize(int len)
{
    if (!m_wtPointBuffer)
    {
        m_wtPointLen = len;
        m_wtPointBuffer = new RS_D_Point[m_wtPointLen];
    }
    else if (len > m_wtPointLen)
    {
        delete [] m_wtPointBuffer;
        m_wtPointLen = len;
        m_wtPointBuffer = new RS_D_Point[m_wtPointLen];
    }
}


//WARNING: caller responsible for deleting resulting line buffer
LineBuffer* GDRenderer::ApplyLineStyle(LineBuffer* srcLB, wchar_t* lineStyle, double lineWidthPixels, double drawingScale, double dpi)
{
    // if there are less than two indices then we can't style
    int lenInds = srcLB->point_count();
    if (lenInds < 2)
        return NULL;

    // configure the line style definition to use
    LineStyleDef lineStyleDef;
    LineStyle style = lineStyleDef.FindLineStyle(lineStyle);
    lineStyleDef.SetStyle(style, drawingScale, dpi, lineWidthPixels);

    // the starting pixel run
    int pixelRunInd = 0;
    PixelRun pixelRun = lineStyleDef.m_pixelRuns[pixelRunInd];

    // create the destination line buffer
    LineBuffer* destLB = LineBufferPool::NewLineBuffer(m_pPool, 8);
    std::auto_ptr<LineBuffer> spDestLB(destLB);

    // special code for Fenceline1 style
    int numCapSegs = 0;
    double csCapAng = 0.0, snCapAng = 0.0;
    if (style == LineStyle_FENCELINE1)
    {
        // get the size in screen units of the circular decoration
        double decorSize = lineStyleDef.m_pixelRuns[1].m_decorSize / drawingScale;

        numCapSegs = (int)ceil(M_PI * sqrt(0.5*decorSize));

        // restrict this value to be safe - 64 is enough to handle the
        // case of the decoration filling the whole screen
        if (numCapSegs > 64)
            numCapSegs = 64;

        // angle increment
        double dAngle = 2.0*M_PI / numCapSegs;
        csCapAng = cos(dAngle);
        snCapAng = sin(dAngle);
    }

    // loop over the source buffer's segments
    double walkLen = 0.0;
    double sumdLen = 0.0;
    for (int j=1; j<lenInds; ++j)
    {
        // get the vertex and index arrays from the line
        // buffer - these are inside the loop because they
        // change if the line buffer resizes itself

        // don't style if current segment is empty (a move-to)
        if (srcLB->point_type(j) == (unsigned char)LineBuffer::stMoveTo)
            continue;

        // get the data for the segment
        double p0x, p0y, p1x, p1y;
        srcLB->get_point(j-1, p0x, p0y);
        srcLB->get_point(j  , p1x, p1y);

        double dx = p1x - p0x;
        double dy = p1y - p0y;

        // detect a previous move-to
        if (srcLB->point_type(j-1) == (unsigned char)LineBuffer::stMoveTo)
        {
            // reset the distances
            walkLen = 0.0;
            sumdLen = 0.0;

            // reset the pixel run
            pixelRunInd = 0;
            pixelRun    = lineStyleDef.m_pixelRuns[pixelRunInd];
        }

        // skip zero-length segments
        if (dx == 0.0 && dy == 0.0)
            continue;

        // get the length and normalized direction
        double lineLen = sqrt(dx*dx + dy*dy);
        double lineInv = 1.0 / lineLen;
        dx *= lineInv;
        dy *= lineInv;

        // traverse this segment (in model units)
        bool doneSeg = false;
        double x0 = p0x;
        double y0 = p0y;
        double x1, y1;
        do
        {
            // f gives the parametric position on the line
            // corresponding to the end of the pixel run
            double f = walkLen - sumdLen + pixelRun.m_nPixels;
            if (f < lineLen)
            {
                // somewhere before the end of the line
                x1 = p0x + f*dx;
                y1 = p0y + f*dy;
            }
            else
            {
                // only go to the end of the line
                x1 = p1x;
                y1 = p1y;
                doneSeg = true;
            }

            // draw any dash
            if (pixelRun.m_isOn && destLB != srcLB)
            {
                destLB->MoveTo(x0, y0);
                destLB->LineTo(x1, y1);
            }
            x0 = x1;
            y0 = y1;

            // draw any decoration
            if (pixelRun.m_decor != Decoration_None)
            {
                double g = walkLen - sumdLen + pixelRun.m_decorOffset;
                if (g >= 0.0 && g < lineLen)
                {
                    double dxw = 0.5*pixelRun.m_decorSize*dx;
                    double dyw = 0.5*pixelRun.m_decorSize*dy;

                    double xd = p0x + g*dx;
                    double yd = p0y + g*dy;

                    if (pixelRun.m_decor == Decoration_CrossTick)
                    {
                        destLB->MoveTo(xd - dyw, yd + dxw);
                        destLB->LineTo(xd + dyw, yd - dxw);
                    }
                    else if (pixelRun.m_decor == Decoration_Square)
                    {
                        double dxyp = dxw + dyw;
                        double dxym = dxw - dyw;
                        destLB->MoveTo(xd + dxym, yd + dxyp);
                        destLB->LineTo(xd - dxyp, yd + dxym);
                        destLB->LineTo(xd - dxym, yd - dxyp);
                        destLB->LineTo(xd + dxyp, yd - dxym);
                        destLB->LineTo(xd + dxym, yd + dxyp);
                    }
                    else if (pixelRun.m_decor == Decoration_Circle)
                    {
                        // set the first point
                        destLB->MoveTo(xd + dxw, yd + dyw);

                        // set the intermediate points
                        double xc = dxw;
                        double yc = dyw;
                        for (int i=1; i<numCapSegs; i++)
                        {
                            double xt = xc;
                            xc = csCapAng*xc - snCapAng*yc;
                            yc = snCapAng*xt + csCapAng*yc;
                            destLB->LineTo(xd + xc, yd + yc);
                        }

                        // set the final point
                        destLB->LineTo(xd + dxw, yd + dyw);
                    }
                }
            }

            if (!doneSeg)
            {
                // update the walk length
                walkLen += pixelRun.m_nPixels;

                // move to the next pixel run
                ++pixelRunInd;
                if (pixelRunInd == lineStyleDef.m_nRuns)
                    pixelRunInd = 0;
                pixelRun = lineStyleDef.m_pixelRuns[pixelRunInd];
            }
        }
        while (!doneSeg);

        // update the summed line length
        sumdLen += lineLen;
    }

    return spDestLB.release();
}


//-----------------------------------------------------------------------------
//scale an input number in meters to a mapping
//space number given a device or mapping space unit.
//-----------------------------------------------------------------------------
double GDRenderer::_MeterToMapSize(RS_Units unit, double number)
{
    double scale_factor;

    if (unit == RS_Units_Device) // in meters, fixed size
        scale_factor = m_mapScale / m_metersPerUnit;
    else
        scale_factor = 1.0 / m_metersPerUnit;

    return number * scale_factor;
}


void GDRenderer::SetRenderSelectionMode(bool mode)
{
    SetRenderSelectionMode(mode, 0x0000FFFF);
}


void GDRenderer::SetRenderSelectionMode(bool mode, int rgba)
{
    SE_Renderer::SetRenderSelectionMode(mode, rgba);

    //initialize the selection styles if needed
    if (mode)
    {
        RS_Color selLineColor = RS_Color((rgba & 0xFFFFFF00) | 200);
        RS_Color selFillColor(rgba & 0xFFFFFFFF);
        //For backward compatibility, only do the override if we have a fully opaque alpha component, otherwise respect the value defined
        if (selFillColor.alpha() == 255)
            selFillColor = RS_Color((rgba & 0xFFFFFF00) | 160);
        RS_Color selBgColor(0, 0, 0, 0);
        RS_LineStroke selStroke = RS_LineStroke(selLineColor, 0.001, L"Solid", RS_Units_Device);
        m_selFill = RS_FillStyle(selStroke, selFillColor, selBgColor, L"Solid");
    }
}


//////////////////////////////////////////////////////////////////////////////
// Text drawing
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// RS_FontEngine implementation
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


void GDRenderer::MeasureString(const RS_String& s,
                               double           height,
                               const RS_Font*   font,
                               double           angleRad,
                               RS_F_Point*      res,       // assumes length equals 4 points
                               float*           offsets)   // assumes length equals length of string
{
    //gd likes height in points rather than pixels
    height *= 72.0 / m_dpi;

    // The computed height can have roundoff in it, and the rendering code is
    // very sensitive to it.  Remove this roundoff by rounding the height to
    // the nearest 1/65536ths of a point.
    height = floor(height * 65536.0 + 0.5) / 65536.0;

    // If the supplied font height is too large GD can't measure it.  We'll
    // use a reasonable font height for measuring, and then scale the result.
    double measureHeight = rs_min(5000.0, height);
    double measureScale = height / measureHeight;

    // Do any BIDI conversion.  If the offset array is supplied (i.e. for path
    // text) then assume the conversion was already performed on the input string.
    const RS_String* pStrToUse;
    if (offsets)
    {
        pStrToUse = &s;
    }
    else
    {
        const RS_String& sConv = m_bidiConverter.ConvertString(s);

        // ConvertString returns either the converter-owned string or the input
        // string - in either case we can temporarily hold on to the pointer
        pStrToUse = &sConv;
    }

    //convert input to UTF8, which is what GD uses
    size_t len = pStrToUse->length();
    size_t lenbytes = len*4+1;
    char* sutf8 = (char*)alloca(lenbytes);
    DWFString::EncodeUTF8(pStrToUse->c_str(), len * sizeof(wchar_t), sutf8, lenbytes);

    //convert font path to utf8 also
    size_t lenf = font->m_filename.length();
    size_t lenbytesf = lenf * 4 + 1;
    char* futf8 = (char*)alloca(lenbytesf);
    DWFString::EncodeUTF8(font->m_filename.c_str(), lenf * sizeof(wchar_t), futf8, lenbytesf);

    int extent[8];
    gdFTStringExtra extra;
    memset(&extra, 0, sizeof(gdFTStringExtra));
    if (offsets)
        extra.flags |= gdFTEX_XSHOW;
    extra.flags |= gdFTEX_RESOLUTION;
    extra.hdpi = (int)m_dpi;
    extra.vdpi = (int)m_dpi;
    char* err = NULL;
    err = gdImageStringFTEx(NULL, extent, 0, futf8, measureHeight, angleRad, 0, 0, sutf8, &extra);

#ifdef _DEBUG
    if (err) printf("gd text error : %s\n", err);
#endif

    //scale the result
    for (int i=0; i<4; ++i)
    {
        res[i].x = measureScale*extent[2*i];
        res[i].y = measureScale*extent[2*i+1];
    }

    if (extra.xshow && offsets)
    {
        //copy over character spacings into result array
        for (size_t i=0; i<len; ++i)
            offsets[i] = (float)(measureScale*extra.xshow[i]);

        //and then release the gd allocated xshow pointer
        gdFree(extra.xshow);
    }
}


void GDRenderer::DrawString(const RS_String& s,
                            double           x,
                            double           y,
                            double           /*width*/,
                            double           height,
                            const RS_Font*   font,
                            RS_Color&        color,
                            double           angleRad)
{
    if (font == NULL)
        return;

    // Don't draw the text if height > 16384 pixels, since memory usage in the call
    // to gdImageStringFTEx below starts to get too large.  16394 pixels should be
    // more than enough (e.g. this allows 13" high text on a 1200dpi device).
    if (height > 16384.0)
        return;

    //gd likes height in points rather than pixels
    height *= 72.0 / m_dpi;

    // The computed height can have roundoff in it, and the rendering code is
    // very sensitive to it.  Remove this roundoff by rounding the height to
    // the nearest 1/65536ths of a point.
    height = floor(height * 65536.0 + 0.5) / 65536.0;

    // do BIDI conversion
    const RS_String& sConv = m_bidiConverter.ConvertString(s);

    //convert input to UTF8, which is what GD uses
    size_t len = sConv.length();
    size_t lenbytes = len*4+1;
    char* sutf8 = (char*)alloca(lenbytes);
    DWFString::EncodeUTF8(sConv.c_str(), len * sizeof(wchar_t), sutf8, lenbytes);

    //convert font path to utf8 also
    size_t lenf = font->m_filename.length();
    size_t lenbytesf = lenf * 4 + 1;
    char* futf8 = (char*)alloca(lenbytesf);
    DWFString::EncodeUTF8(font->m_filename.c_str(), lenf * sizeof(wchar_t), futf8, lenbytesf);

    //draw the string
    int gdc = ConvertColor((gdImagePtr)m_imout, color);
    gdFTStringExtra extra;
    memset(&extra, 0, sizeof(gdFTStringExtra));
    extra.flags |= gdFTEX_RESOLUTION;
    extra.hdpi = (int)m_dpi;
    extra.vdpi = (int)m_dpi;
    char* err = NULL;
    err = gdImageStringFTEx((gdImagePtr)m_imout, NULL, gdc, futf8, height, angleRad, ROUND(x), ROUND(y), sutf8, &extra);

#ifdef _DEBUG
    if (err) printf("gd text error : %s\n", err);
#endif
}


const RS_Font* GDRenderer::FindFont(RS_FontDef& def)
{
    const RS_Font* pFont = FontManager::Instance()->FindFont(def.name().c_str(),
                          (def.style() & RS_FontStyle_Bold) != 0,
                          (def.style() & RS_FontStyle_Italic) != 0);

    return pFont;
}


// Called when applying a line style on a feature geometry.  Line styles can
// only be applied to linestring and polygon feature geometry types.
void GDRenderer::ProcessLine(SE_ApplyContext* ctx, SE_RenderLineStyle* style)
{
    LineBuffer* featGeom = ctx->geometry;
    std::auto_ptr<LineBuffer> spLB;

    if (s_bGeneralizeData)
    {
        // if the geometry bounds is less than one sixteenth pixel it is too small to try drawing
//      RS_Bounds bds = featGeom->bounds();
//      if ((bds.width() < 0.25*m_drawingScale) && (bds.height() < 0.25*m_drawingScale))
//          return;

        if (featGeom->point_count() > 6)
        {
            featGeom = featGeom->Optimize(0.5*m_drawingScale, m_pPool);
            if (featGeom != ctx->geometry)
                spLB.reset(featGeom);
        }
    }

    SE_ApplyContext local_ctx = *ctx;
    local_ctx.geometry = featGeom;
    SE_Renderer::ProcessLine(&local_ctx, style);

    // cleanup
    if (spLB.get())
        LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
}


// Called when applying an area style on a feature geometry.  Area styles can
// can only be applied to polygon feature geometry types.
void GDRenderer::ProcessArea(SE_ApplyContext* ctx, SE_RenderAreaStyle* style)
{
    LineBuffer* featGeom = ctx->geometry;
    std::auto_ptr<LineBuffer> spLB;

    // can't apply an area style to point and linestring geometry types
    switch (featGeom->geom_type())
    {
        case FdoGeometryType_Point:
        case FdoGeometryType_MultiPoint:
        case FdoGeometryType_LineString:
        case FdoGeometryType_MultiLineString:
        case FdoGeometryType_CurveString:
        case FdoGeometryType_MultiCurveString:
            return;
    }

    if (s_bGeneralizeData)
    {
        // if the geometry bounds is less than one quarter pixel, then it is too small to try drawing
//      RS_Bounds bds = featGeom->bounds();
//      if ((bds.width() < 0.5*m_drawingScale) && (bds.height() < 0.5*m_drawingScale))
//          return;

        if (featGeom->point_count() > 6)
        {
            featGeom = featGeom->Optimize(0.5*m_drawingScale, m_pPool);
            if (featGeom != ctx->geometry)
                spLB.reset(featGeom);
        }
    }

    SE_ApplyContext local_ctx = *ctx;
    local_ctx.geometry = featGeom;
    SE_Renderer::ProcessArea(&local_ctx, style);

    // cleanup
    if (spLB.get())
        LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
////
////             DWF Rewrite and related code
////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


//Inserts the contents of a given DWF input stream
//into the current output W2D. The given coord sys
//transformation is applied and geometry will be clipped
//to the RS_Bounds context of the DWFRenderer
void GDRenderer::AddDWFContent(RS_InputStream*   in,
                               CSysTransformer*  xformer,
                               const RS_String&  section,
                               const RS_String&  passwd,
                               const RS_String&  w2dfilter)
{
    try
    {
        if (in->available() == 0)
            return;

        //go to beginning of stream
        in->seek(SEEK_SET, 0);

        DWFRSInputStream rsin(in);

        DWFPackageReader oReader(rsin, passwd.c_str());

        DWFPackageReader::tPackageInfo tInfo;
        oReader.getPackageInfo(tInfo);

        if (tInfo.eType != DWFPackageReader::eDWFPackage)
        {
            return; //throw exception?
        }

        bool checkSection = (wcslen(section.c_str()) > 0);

        //read the manifest
        DWFManifest& rManifest = oReader.getManifest();

        //now read the sections
        DWFSection* pSection = NULL;
        DWFManifest::SectionIterator* iSection = (&rManifest)->getSections();

        if (iSection)
        {
            for (; iSection->valid(); iSection->next())
            {
                pSection = iSection->get();

                //call this so that the resource data (like transforms and roles) is read in
                pSection->readDescriptor();

                //DWFGlobalSection* pGlobal = dynamic_cast<DWFGlobalSection*>(pSection);
                DWFEPlotSection* pEPlot = dynamic_cast<DWFEPlotSection*>(pSection);

                if (pEPlot)
                {
                    if (checkSection)
                    {
                        //compare name stored in section to user requested
                        //section
                        //Used for point symbols
                        DWFString name = pEPlot->title();

                        //skip current section if its name does
                        //not match the name of the one we need
                        if (name != section.c_str())
                            continue;
                    }

                    // Get the resources for the section

                    //first do primary W2D resources
                    DWFIterator<DWFResource*>* piResources = pEPlot->findResourcesByRole(DWFXML::kzRole_Graphics2d);

                    if (piResources)
                    {
                        for (; piResources->valid(); piResources->next())
                        {
                            DWFResource* pResource = piResources->get();
                            DWFInputStream* pStream = pResource->getInputStream();

                            if (!pStream)
                            {
                                _DWFCORE_THROW(DWFMemoryException, L"Out of memory");
                            }

                            RSDWFInputStream rsdwfin(pStream);
                            AddW2DContent(&rsdwfin, xformer, w2dfilter);

                            DWFCORE_FREE_OBJECT(pStream);
                        }

                        DWFCORE_FREE_OBJECT(piResources);
                        piResources = NULL;
                    }

                    //then do overlays
                    piResources = pEPlot->findResourcesByRole(DWFXML::kzRole_Graphics2dOverlay);

                    if (piResources)
                    {
                        for (; piResources->valid(); piResources->next())
                        {
                            DWFResource* pResource = piResources->get();
                            DWFInputStream* pStream = pResource->getInputStream();

                            if (!pStream)
                            {
                                _DWFCORE_THROW(DWFMemoryException, L"Out of memory");
                            }

                            RSDWFInputStream rsdwfin(pStream);
                            AddW2DContent(&rsdwfin, xformer, w2dfilter);

                            DWFCORE_FREE_OBJECT(pStream);
                        }

                        DWFCORE_FREE_OBJECT(piResources);
                        piResources = NULL;
                    }
                }
            }

            DWFCORE_FREE_OBJECT(iSection);
        }
    }
    catch (DWFException&)
    {
    }
}


void GDRenderer::AddW2DContent(RS_InputStream* in, CSysTransformer* xformer, const RS_String& w2dfilter)
{
    WT_Result result;

    //initialize state variables, which are needed by W2D rewriter callbacks
    m_input = in;
    m_xformer = xformer;
    m_bHaveViewport = false;
    m_bLayerPassesFilter = true;
    m_layerFilter = w2dfilter;

    //set output image if not already set
    if (!m_bIsSymbolW2D)
        m_imw2d = m_imout;

    WT_File fin;
    fin.set_file_mode(WT_File::/*WT_File_mode::*/File_Read);

    fin.set_stream_user_data(this);

    SetActions(fin);

    fin.open();

    // Do the actual reading.
    do
    {
        result = fin.process_next_object();
    }
    while (result == WT_Result::Success);

    fin.close();

    m_input = NULL;

    //clear the output image if we set it in this function
    if (!m_bIsSymbolW2D)
        m_imw2d = NULL;
}


void GDRenderer::SetActions(WT_File& file)
{
    file.set_stream_open_action (gdr_open);
    file.set_stream_close_action(gdr_close);
    file.set_stream_read_action (gdr_read);
    file.set_stream_seek_action (gdr_seek);

    //file.set_dwf_header_action(gdr_process_dwf_header);
    //file.set_author_action(gdr_process_author);
    //file.set_creator_action(gdr_process_creator);
    //file.set_creation_time_action(gdr_process_created);
    //file.set_modification_time_action(gdr_process_modified);
    //file.set_source_filename_action(gdr_process_sourceFilename);
    //file.set_source_creation_time_action(gdr_process_sourceCreationTime);
    //file.set_source_modification_time_action(gdr_process_sourceModificationTime);
    file.set_units_action(gdr_process_units);
    //file.set_embed_action(gdr_process_embed);
    //file.set_named_view_action(gdr_process_namedView);
    //file.set_view_action(gdr_process_view);
    //file.set_plot_info_action(gdr_process_plotInfo);
    file.set_background_action(gdr_process_background);
    file.set_color_action(gdr_process_color);
    file.set_line_weight_action(gdr_process_lineWeight);
    file.set_line_style_action(gdr_process_lineStyle);
    file.set_layer_action(gdr_process_layer);
    file.set_viewport_action(gdr_process_viewport);
    file.set_visibility_action(gdr_process_visibility);
    file.set_code_page_action(gdr_process_codePage);
    file.set_color_map_action(gdr_process_colorMap);
    //file.set_comments_action(gdr_process_comments);
    file.set_contour_set_action(gdr_process_contourSet);
    //file.set_copyright_action(gdr_process_copyright);
    file.set_dash_pattern_action(gdr_process_dashPattern);
    //file.set_description_action(gdr_process_description);
    file.set_fill_action(gdr_process_fill);
    file.set_filled_ellipse_action(gdr_process_filledEllipse);
    file.set_font_action(gdr_process_font);
    file.set_image_action(gdr_process_image);
    //file.set_keywords_action(gdr_process_keywords);
    file.set_marker_size_action(gdr_process_markerSize);
    file.set_marker_symbol_action(gdr_process_markerSymbol);
    file.set_merge_control_action(gdr_process_mergeControl);
    //file.set_named_view_list_action(gdr_process_namedViewList);
    file.set_origin_action(gdr_process_origin);
    file.set_outline_ellipse_action(gdr_process_outlineEllipse);
    file.set_polygon_action(gdr_process_polygon);
    file.set_polytriangle_action(gdr_process_polytriangle);
    file.set_polymarker_action(gdr_process_polymarker);
    file.set_projection_action(gdr_process_projection);
    //file.set_subject_action(gdr_process_subject);
    //file.set_title_action(gdr_process_title);
    //file.set_unknown_action(gdr_process_unknown);
    //file.set_url_action(gdr_process_url);
    file.set_png_group4_image_action(gdr_process_pngGroup4Image);
    file.set_polyline_action(gdr_process_polyline);
    file.set_text_action(gdr_process_text);
    file.set_inked_area_action(gdr_process_inkedArea);
    //file.set_plot_optimized_action(gdr_process_plotOptimized);
    //file.set_group_begin_action(gdr_process_groupBegin);
    //file.set_group_end_action(gdr_process_groupEnd);
    //file.set_embedded_font_action(gdr_process_embeddedFont);
    //file.set_trusted_font_list_action(gdr_process_trustedFontList);
    //file.set_blockref_action(gdr_process_blockref);
    //file.set_block_meaning_action(gdr_process_blockMeaning);
    //file.set_encryption_action(gdr_process_encryption);
    //file.set_orientation_action(gdr_process_orientation);
    file.set_alignment_action(gdr_process_alignment);
    //file.set_password_action(gdr_process_password);
    //file.set_guid_action(gdr_process_guid);
    //file.set_filetime_action(gdr_process_fileTime);
    //file.set_userdata_action(gdr_process_userData);
    file.set_pen_pattern_action(gdr_process_penPattern);
    file.set_line_pattern_action(gdr_process_linePattern);
    file.set_fill_pattern_action(gdr_process_fillPattern);
    //file.set_signdata_action(gdr_process_DigitalSign);

    file.set_gouraud_polytriangle_action(gdr_process_gouraudPolytriangle);
    file.set_gouraud_polyline_action(gdr_process_gouraudPolyline);
}


//Given an array of points in W2D logical coordinates, this function:
// 1. Transform W2D logical coords into their model space using the
//    W2D file's units structure
// 2. Apply coordinate system transformation into the destination
//    map's space
// 3. Optionally clip the resulting point to the user-requested
//    data extent of the destination map
// 4. Transform clipped points to destination W2D space
// 5. Return a pointer to an array of W2D points with the
//    total number of output points returned in outNumpts.
//    If the clipping process of a polyline or polygon resulted
//    in a multipolygon or multipolyline, a pointer to a vector
//    containing contour counts will also be returned.
//    *** Both pointers are valid unit the next call to this function. ***
const RS_D_Point* GDRenderer::ProcessW2DPoints(WT_File&          file,
                                               WT_Logical_Point* srcpts,
                                               int               numpts,
                                               bool              checkInBounds)
{
    //This transformer may have been modified if a Viewport
    //opcode was encountered in the source W2D. This is needed for
    //support for symbols from ACAD, which do not use the
    //standard W2D extent used by Studio.
    //The function UpdateSymbolTrans does the actual modification and is
    //called by the process_viewport callback.
    SymbolTrans* trans = (SymbolTrans*)m_xformer;

    //Our W2DRewrite component stores the source file Units transform
    //in the source file's desired rendition -- a bit of a hack
    //but we cannot store it in the destination file -- it messes up some scales
    //plus it has a totally different units of its own.
    WT_Matrix xform = file.desired_rendition().drawing_info().units().dwf_to_application_adjoint_transform();

    LineBuffer* lb = LineBufferPool::NewLineBuffer(m_pPool, numpts);
    std::auto_ptr<LineBuffer> spLB(lb);
    lb->Reset();

    //
    // Process the points into mapping space
    //

    WT_Point3D psrc(0.0, 0.0, 0.0);
    WT_Point3D pdst(0.0, 0.0, 0.0);

    for (int i=0; i<numpts; i++)
    {
        psrc.m_x = srcpts[i].m_x;
        psrc.m_y = srcpts[i].m_y;

        //this puts it into model space of the source data
        xform.transform(psrc, pdst);

        //now transform to mapping space of map
        //using the symbol transformer object (if any)
        if (trans)
            trans->TransformPoint(pdst.m_x, pdst.m_y);

        if (i == 0)
            lb->MoveTo(pdst.m_x, pdst.m_y);
        else
            lb->LineTo(pdst.m_x, pdst.m_y);
    }

    // IMPORTANT: Only do this if the data is a DWF layer
    // or a DWF symbol that is too large to draw using a
    // cached bitmap. Regular DWF Symbols will be transformed
    // to the correct mapping space location by the symbol code
    if (!IsSymbolW2D())
    {
        //
        // Clipping. For GD rendering a simple bounds check is enough,
        // the renderer itself will take care of the rest
        //
        if (checkInBounds)
        {
            //check if line buffer is completely outside box
            if (   lb->bounds().minx > m_extents.maxx
                || lb->bounds().miny > m_extents.maxy
                || lb->bounds().maxx < m_extents.minx
                || lb->bounds().maxy < m_extents.miny)
            {
                LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
                return NULL;
            }
        }
    }

    // IMPORTANT: Only do this if the data is a DWF layer
    // or a DWF symbol that is too large to draw using a
    // cached bitmap. Regular DWF Symbols will be transformed
    // to the correct mapping space location by the symbol code
    if (!IsSymbolW2D() || m_imw2d != m_imsym)
    {
        if (lb->point_count() > 0)
        {
            _TransformPointsNoClamp(lb);
        }
    }
    else
    {
        //for symbols just copy the points to the output array
        //and only invert the y coordinate -- we need to flip y since
        //in gd y goes down and in DWF it goes up
        int count = lb->point_count();

        EnsureBufferSize(count);
        RS_D_Point* wpts = m_wtPointBuffer;

        for (int i=0; i<count; i++)
        {
            wpts[i].x = (int)lb->x_coord(i);
            wpts[i].y = gdImageSY((gdImagePtr)GetW2DTargetImage()) - (int)lb->y_coord(i);
        }
    }

    //free temporary linebuffer
    LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());

    return m_wtPointBuffer;
}


//This function scales a W2D space related value from
//source W2D space to destination.
//Since the source W2D file can fit into a small piece of the
//destination DWF or be much larger, we need to take that
//scaling into account when carrying over things like line weight,
//font height, etc. this helper funtion determines and applies
//that scale
double GDRenderer::ScaleW2DNumber(WT_File& file, WT_Integer32 number)
{
    WT_Matrix xform = file.desired_rendition().drawing_info().units().dwf_to_application_adjoint_transform();

    double scale1 = 1.0 / xform(0, 0); //div because we need inverse

    //number is now in source W2D model units
    double dModelSpace = (double)number * scale1;

    double dMapSpace = dModelSpace;
    if (m_xformer)
    {
        //now we need to convert that to a length in destination
        //map units.
        dMapSpace *= m_xformer->GetLinearScale();
    }

    double dDstSpace = dMapSpace;

    //only scale by map scale if we are not a symbol inside a cached image
    if (!m_bIsSymbolW2D)
    {
        dDstSpace *= m_scale;
    }

    return dDstSpace;
}


// Fills and returns the point buffer using the supplied line buffer.
// No transformations are applied.
const RS_D_Point* GDRenderer::FillPointBuffer(LineBuffer* lb)
{
    _TransferPoints(lb, NULL);
    return m_wtPointBuffer;
}


void GDRenderer::UpdateSymbolTrans(WT_File& /*file*/, WT_Viewport& viewport)
{
    _ASSERT(m_xformer);

    RS_Bounds alternate_extent(0.0, 0.0, -1.0, -1.0);

    //If a viewport was defined, the symbol W2D likely came from AutoCAD.
    //In that case, the extent of the data inside the W2D is not the same
    //as what Studio saves (0, 0, SYMBOL_MAX, SYMBOL_MAX), so we need to use
    //a different transformation for that data.
    //IMPORTANT: This will destructively modify the transformer that was passed in.
    //It is ugly, but avoids parsing the W2D twice.
    //The assumption here is that Viewport opcodes define the bounds of the data.
    //This is not necessarily true for all W2D files, but is true for ones coming
    //from ACAD through the Dwg Extract and Transform engine.
    if (viewport.contour() && m_bIsSymbolW2D)
    {
        const WT_Contour_Set* cset = viewport.contour();
        const WT_Logical_Point* pts = cset->points();

        _ASSERT(cset->total_points() == 4);

        if (cset->total_points() == 4)
        {
            alternate_extent.minx = rs_min(pts[0].m_x, pts[2].m_x);
            alternate_extent.maxx = rs_max(pts[0].m_x, pts[2].m_x);

            alternate_extent.miny = rs_min(pts[0].m_y, pts[2].m_y);
            alternate_extent.maxy = rs_max(pts[0].m_y, pts[2].m_y);

            //TODO: we don't do that yet, since DWF layers can have
            //multiple viewports, which mess up this logic -- no single
            //viewport contains the overall extent of the data
            /*
            //if it is a DWF layer, also correct for aspect ratio
            //of the destination bounds of the transformer so that
            //we avoid stretching of the layer data.
            //we now assume we know the exact bounds of the data
            //in the w2d
            if (!m_bIsSymbolW2D)
            {
                double arsrc = alternate_extent.width() / alternate_extent.height();

                RS_Bounds dstext = trans.GetSrcBounds();
                double ardst = dstext.width() / dstext.height();

                if (arsrc >= ardst)
                {
                    double newhgt2 = 0.5 * dstext.width() / arsrc;
                    double oldcy = 0.5 * (dstext.miny + dstext.maxy);
                    RS_Bounds newdst(dstext.minx, oldcy - newhgt2, dstext.maxx, oldcy + newhgt2);
                    trans.SetDstBounds(newdst);
                }
                else
                {
                    double newwid2 = 0.5 * dstext.height() * arsrc;
                    double oldcx = 0.5 * (dstext.minx + dstext.maxx);
                    RS_Bounds newdst(oldcx - newwid2, dstext.miny, oldcx + newwid2, dstext.maxy);
                    trans.SetDstBounds(newdst);
                }
            }
            */

            //and finally set the source bounds we got from the viewport
            //opcode
            ((SymbolTrans*)m_xformer)->SetSrcBounds(alternate_extent);
            m_bHaveViewport = true;
        }
    }
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
////
////      END of DWF Rewrite and related code
////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// SE_Renderer implementation
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


void GDRenderer::_TransferPoints(LineBuffer* plb, const SE_Matrix* xform)
{
    int numpts = plb->point_count();
    EnsureBufferSize(numpts);
    int* pts = (int*)m_wtPointBuffer;

    if (!xform || xform->isIdentity())
    {
        for (int i=0; i<numpts; i++)
        {
            *pts++ = (int)plb->x_coord(i);
            *pts++ = (int)plb->y_coord(i);
        }
    }
    else
    {
        for (int i=0; i<numpts; i++)
        {
            double x, y;
            xform->transform(plb->x_coord(i), plb->y_coord(i), x, y);
            pts[0] = (int)x;
            pts[1] = (int)y;

            pts += 2;
        }
    }
}


void GDRenderer::_TransferContourPoints(LineBuffer* plb, int cntr, const SE_Matrix* xform)
{
    EnsureBufferSize(plb->cntr_size(cntr));
    int* pts = (int*)m_wtPointBuffer;

    if (!xform || xform->isIdentity())
    {
        int end = plb->contour_end_point(cntr);
        for (int i=plb->contour_start_point(cntr); i<=end; i++)
        {
            *pts++ = (int)plb->x_coord(i);
            *pts++ = (int)plb->y_coord(i);
        }
    }
    else
    {
        int end = plb->contour_end_point(cntr);
        for (int i=plb->contour_start_point(cntr); i<=end; i++)
        {
            double x, y;
            xform->transform(plb->x_coord(i), plb->y_coord(i), x, y);
            pts[0] = (int)x;
            pts[1] = (int)y;
            pts += 2;
        }
    }
}


//copied from WritePolylines, except it doesn't do to screen transform -- we should refactor
void GDRenderer::DrawScreenPolyline(LineBuffer* srclb, const SE_Matrix* xform, const SE_LineStroke& lineStroke)
{
    if ((lineStroke.color & 0xFF000000) == 0)
        return;

    if (srclb->geom_count() == 0)
        return; //if you have no geoms, why do you call us at all?

    unsigned int color = lineStroke.color;
    RS_Color c((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF);

    int gdc = ConvertColor((gdImagePtr)m_imout, c);
    gdImageSetAntiAliased((gdImagePtr)m_imout, gdc);

    //line width is always device space and units are meters.
    //so convert to equivalent pixel width
    int line_weight = (int)lineStroke.weight;
    if (line_weight > m_maxLineWidth)
        line_weight = m_maxLineWidth;

    gdImagePtr brush1 = NULL;
    if (line_weight > 1)
        brush1 = rs_gdImageThickLineBrush(line_weight, c);

    //draw the lines
    for (int i=0; i<srclb->cntr_count(); i++)
    {
        int cntr_size = srclb->cntr_size(i);

        //convert to integer coords
        _TransferContourPoints(srclb, i, xform);

        if (cntr_size > 1)
        {
            //draw antialiased only if thickness is single pixel
            if (line_weight <= 1)
                gdImageOpenPolygon((gdImagePtr)m_imout, (gdPointPtr)m_wtPointBuffer, cntr_size, gdAntiAliased);
            else
            {
                gdImageSetBrush((gdImagePtr)m_imout, brush1);
                gdImageOpenPolygon((gdImagePtr)m_imout, (gdPointPtr)m_wtPointBuffer, cntr_size, brush1? gdBrushed : gdc);
            }
        }
    }

    //unset the stroke
    gdImageSetBrush((gdImagePtr)m_imout, NULL);

    if (brush1)
        gdImageDestroy(brush1);
}


void GDRenderer::DrawScreenPolygon(LineBuffer* polygon, const SE_Matrix* xform, unsigned int color)
{
    if ((color & 0xFF000000) == 0)
        return;

    if (polygon->geom_count() == 0)
        return; //if you have no geoms, why do you call us at all?

    RS_Color c((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF);

    int gdc = ConvertColor((gdImagePtr)m_imout, c);

    gdImagePtr fillpat = NULL;

    //TODO: do the device space calls need fill pattern support?
    /*
    if (wcscmp(use_fill->pattern().c_str(), L"Solid") != 0)
    {
        fillpat = GDFillPatterns::CreatePatternBitmap(use_fill->pattern().c_str(), gdc, gdcbg);
        gdImageSetTile((gdImagePtr)m_imout, fillpat);
    }
    */

    _TransferPoints(polygon, xform);

    int total_cntrs = 0;
    int total_pts = 0;
    for (int i = 0; i < polygon->geom_count(); ++i)
    {
        int cntrs = polygon->geom_size(i);
        int pts = 0;
        for (int j = total_cntrs; j < total_cntrs + cntrs; ++j)
            pts += polygon->cntr_size(j);

        //call the new rasterizer
        m_polyrasterizer->FillPolygon((Point*)(m_wtPointBuffer + total_pts), pts,
                                      polygon->cntrs() + total_cntrs, cntrs,
                                      fillpat? gdTiled : gdc, (gdImagePtr)m_imout);

        total_cntrs += cntrs;
        total_pts += pts;
    }

    /*
    if (fillpat)
    {
        gdImageSetTile((gdImagePtr)m_imout, NULL);
        gdImageDestroy(fillpat);
    }
    */
}


bool GDRenderer::YPointsUp()
{
    return false;
}


void GDRenderer::GetWorldToScreenTransform(SE_Matrix& xform)
{
    xform.x0 = m_scale;
    xform.x1 = 0.0;
    xform.x2 = - m_offsetX * m_scale;
    xform.y0 = 0.0;
    xform.y1 = - m_scale;
    xform.y2 = m_height /*- 1*/ + m_offsetY * m_scale;
}


void GDRenderer::WorldToScreenPoint(double& inx, double& iny, double& ox, double& oy)
{
    //TODO: assumes no rotation of the viewport
    ox = (inx - m_offsetX) * m_scale;
    oy = m_height /*- 1*/ - (iny - m_offsetY) * m_scale;
}


void GDRenderer::ScreenToWorldPoint(double& inx, double& iny, double& ox, double& oy)
{
    //TODO: assumes no rotation of the viewport
    ox = inx * m_invScale + m_offsetX;
    oy = m_offsetY - (iny /*+ 1.0*/ - m_height) * m_invScale;
}


// returns number of pixels per millimeter device
double GDRenderer::GetScreenUnitsPerMillimeterDevice()
{
    return m_dpi / MILLIMETERS_PER_INCH;
}


// returns number of pixels per millimeter world
double GDRenderer::GetScreenUnitsPerMillimeterWorld()
{
    return m_dpi / MILLIMETERS_PER_INCH / m_mapScale;
}


// screen units are pixels
double GDRenderer::GetScreenUnitsPerPixel()
{
    return 1.0;
}


RS_FontEngine* GDRenderer::GetRSFontEngine()
{
    return this;
}


//labeling -- this is the entry API for adding SE labels
//to the label mananger
void GDRenderer::ProcessSELabelGroup(SE_LabelInfo*   labels,
                                     int             nlabels,
                                     RS_OverpostType type,
                                     bool            exclude,
                                     LineBuffer*     path)
{
    //pass labels to the label renderer here

    //check if we are rendering a selection -- bail if so
    if (m_bSelectionMode)
        return;

    //forward it to the label renderer
    m_labeler->ProcessLabelGroup(labels, nlabels, type, exclude, path);
}


void GDRenderer::DrawScreenRaster(unsigned char* data, int length,
                                  RS_ImageFormat format, int native_width, int native_height,
                                  double x, double y, double w, double h, double angleDeg)
{
    if (format == RS_ImageFormat_Unknown)
        return;

    // compute the scaled / rotated size
    double rotatedW = w;
    double rotatedH = h;
    if (angleDeg != 0.0)
    {
        double csAbs = fabs(cos(angleDeg * M_PI180));
        double snAbs = fabs(sin(angleDeg * M_PI180));
        rotatedW = csAbs*w + snAbs*h;
        rotatedH = snAbs*w + csAbs*h;
    }

    // don't allow images beyond the maximum size
    if (rotatedW * rotatedH > IMAGE_SIZE_MAX)
        return;

    // get the source image
    gdImagePtr src = NULL;
    if (format == RS_ImageFormat_ABGR)
    {
        if (native_width < 0 || native_height < 0)
            return;

        src = gdImageCreateTrueColor(native_width, native_height);

        //TODO: figure out a way to do this without copying the whole thing
        //in such a lame loop
        //at least here we don't call gdImageSetPixel for every pixel
        for (int j=0; j<native_height; j++)
        {
            for (int i=0; i<native_width; i++)
            {
                int srccol = ((int*)data)[i + j * native_width];
                //compute GD alpha and reverse b and r
                int col = gdImageColorAllocateAlpha(src,  srccol        & 0xFF,
                                                         (srccol >> 8)  & 0XFF,
                                                         (srccol >> 16) & 0xFF,
                                                         127 - ((srccol >> 24) & 0xFF)/ 2);

                src->tpixels[j][i] = col;
            }
        }
    }
    else if (format == RS_ImageFormat_ARGB)
    {
        if (native_width < 0 || native_height < 0)
            return;

        src = gdImageCreateTrueColor(native_width, native_height);

        //TODO: figure out a way to do this without copying the whole thing
        //in such a lame loop
        //at least here we don't call gdImageSetPixel for every pixel
        for (int j=0; j<native_height; j++)
        {
            for (int i=0; i<native_width; i++)
            {
                int srccol = ((int*)data)[i + j * native_width];
                //compute GD alpha
                int col = gdImageColorAllocateAlpha(src, (srccol >> 16) & 0xFF,
                                                         (srccol >> 8)  & 0XFF,
                                                         srccol        & 0xFF,
                                                         127 - ((srccol >> 24) & 0xFF)/ 2);

                src->tpixels[j][i] = col;
            }
        }
    }
    else if (format == RS_ImageFormat_PNG)
    {
        //NOTE: native_width and native_height arguments are ignored for PNG
        //      since they're encoded in the png data array
        src = gdImageCreateFromPngPtr(length, data);
    }
    else
        throw "Well implement this already!";

    if (ROUND(angleDeg) == 0)
    {
        double w2 = w * 0.5;
        double h2 = h * 0.5;

        int minx = ROUND(x - w2);
        int maxx = ROUND(x + w2);
        int miny = ROUND(y - h2);
        int maxy = ROUND(y + h2);

        gdImageCopyResampled((gdImagePtr)m_imout, src,
                             minx, miny, 0, 0,
                             maxx-minx, maxy-miny, //TODO: do we need +1?
                             gdImageSX(src), gdImageSY(src));
    }
    else
    {
        // scale it if necessary
        if ((int)w != gdImageSX(src) || (int)h != gdImageSY(src))
        {
            gdImagePtr dst = gdImageCreateTrueColor((int)w, (int)h);
            gdImageAlphaBlending(dst, 0);
            gdImageFilledRectangle(dst, 0, 0, gdImageSX(dst)-1, gdImageSY(dst)-1, 0x7f000000);
            gdImageAlphaBlending(dst, 1);
            gdImageCopyResampled(dst, src, 0, 0, 0, 0, gdImageSX(dst), gdImageSY(dst), gdImageSX(src), gdImageSY(src));
            gdImageDestroy(src);
            src = dst;
        }

        gdImageCopyRotated((gdImagePtr)m_imout, src, x, y, 0, 0, gdImageSX(src), gdImageSY(src), ROUND(angleDeg));
    }

    gdImageDestroy(src);
}


void GDRenderer::DrawScreenRaster(unsigned char* data, int length,
                                  RS_ImageFormat format, int native_width, int native_height,
                                  double x, double y, double w, double h, double angleDeg,
                                  double alpha)
{
    RenderUtil::DrawScreenRaster(this, data, length, format, native_width, native_height,
                                 x, y, w, h, angleDeg, alpha);
}


void GDRenderer::DrawScreenText(const RS_TextMetrics& tm, RS_TextDef& tdef, double insx, double insy,
                                RS_F_Point* path, int npts, double param_position)
{
    if (path)
    {
        // path text - we need to do BIDI conversion before we process the text
        const RS_String& sConv = m_bidiConverter.ConvertString(tm.text);

        // we cannot modify the cached RS_TextMetrics so we create a local one
        // and use it to layout the path text
        RS_TextMetrics tm_local;
        if (GetTextMetrics(sConv, tdef, tm_local, true))
        {
            // TODO: need computed seglens rather than NULL to make things faster
            if (LayoutPathText(tm_local, (RS_F_Point*)path, npts, NULL, param_position, tdef.valign(), 0.5))
                DrawPathText(tm_local, tdef);
        }
    }
    else
    {
        // block text - check that we have a valid text metrics
        if (tm.font != NULL)
            DrawBlockText(tm, tdef, insx, insy);
    }
}
