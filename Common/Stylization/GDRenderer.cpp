//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
/*
#include "gdfontt.h"
#include "gdfonts.h"
#include "gdfontmb.h"
#include "gdfontl.h"
#include "gdfontg.h"
*/

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

using namespace DWFToolkit;
using namespace DWFCore;

#define ROUND(x) (int)(floor(x+0.5))

#define SYMBOL_BITMAP_SIZE 128
#define SYMBOL_BITMAP_MAX 1024

//using this in contructor
#pragma warning(disable:4355)


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

static CInitGD s_InitGD;


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
m_extents(0,0,0,0),
m_wtPointBuffer(NULL),
m_wtPointLen(0),
m_symbolManager(NULL),
m_mapInfo(NULL),
m_layerInfo(NULL),
m_fcInfo(NULL),
m_bRequiresClipping(requiresClipping),
m_bIsSymbolW2D(false),
m_bHaveViewport(false),
m_imsym(NULL),
m_input(NULL),
m_xformer(),
m_pPool(NULL)
{
    if (m_width <= 0)
        m_width = 1;
    if (m_height <= 0)
        m_height = 1;

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

    if (m_wtPointBuffer)
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
    RS_ByteData* data = Save(format, width, height);

    if (data == NULL)
        return;

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

    data->Dispose();
}


RS_ByteData* GDRenderer::Save(const RS_String& format, int width, int height)
{
    gdImagePtr im = NULL;

    if (width <= 0)
        width = 1;
    if (height <= 0)
        height = 1;

    //do we need to stretch to non-square pixels?
    if (width != m_width || height != m_height)
    {
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
    else if (format == L"PNG")  // MgImageFormats::Png
        data = (unsigned char*)gdImagePngPtr(im, &size);
    else                        // PNG is the default
        data = (unsigned char*)gdImagePngPtr(im, &size);

    RS_ByteData* byteData = (data)? new RS_ByteData(data, size) : NULL;

    gdFree(data);

    //if we allocated a temporary image to stretch-blit, destroy it
    if (im != m_imout)
        gdImageDestroy(im);

    return byteData;
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
    m_extents = extents;
    m_mapScale = mapScale;
    m_dpi = dpi;
    m_metersPerUnit = metersPerUnit;

    //find scale used to convert to pixel coordinates
    //need to take aspect ratios into account
    double arDisplay = (double)m_width / (double)m_height;
    double arMap = m_extents.width() / m_extents.height();

    if (arDisplay > arMap)
    {
        m_scale = (double)m_height / m_extents.height();
    }
    else
    {
        m_scale = (double)(m_width) / m_extents.width();
    }

    m_offsetX = m_extents.minx;
    m_offsetY = m_extents.miny;

    double metersPerPixel = 0.0254 / m_dpi;

    //compute drawing scale
    //drawing scale is map scale converted to [mapping units] / [pixels]
    m_drawingScale = m_mapScale * metersPerPixel / m_metersPerUnit;

    m_invScale = 1.0 / m_scale;

    m_labeler->StartLabels();

    // remember the map info
    m_mapInfo = mapInfo;

    // do it here, since we will need the renderer's map scales, which are computed above
    InitFontEngine(this, this);
}


void GDRenderer::EndMap()
{
    // turn off selection mode so the labels draw normal
    SetRenderSelectionMode(false);

    //finally draw all the labels
    m_labeler->BlastLabels();

    // clear the map info
    m_mapInfo = NULL;
}


void GDRenderer::StartLayer(RS_LayerUIInfo*      legendInfo,
                            RS_FeatureClassInfo* classInfo)
{
    // remember the layer/feature info
    m_layerInfo = legendInfo;
    m_fcInfo = classInfo;
}


void GDRenderer::EndLayer()
{
    // clear the layer/feature info
    m_layerInfo = NULL;
    m_fcInfo = NULL;
}


void GDRenderer::StartFeature(RS_FeatureReader* /*feature*/,
                              const RS_String* /*tooltip*/,
                              const RS_String* /*url*/,
                              const RS_String* /*theme*/,
                              double /*zOffset*/,
                              double /*zExtrusion*/,
                              RS_ElevationType /*zOffsetType*/)
{
}


void GDRenderer::ProcessPolygon(LineBuffer* lb,
                                RS_FillStyle& fill)
{
    RS_FillStyle* use_fill = &fill;

    //should we use selection style?
    if (m_bSelectionMode)
        use_fill = &m_selFill;

    LineBuffer* workbuffer = lb;

    if (workbuffer->point_count() == 0)
        return;

    if (use_fill->color().alpha() != 0)
    {
        _TransformPointsNoClamp(workbuffer->points(), workbuffer->point_count());

        int gdc = ConvertColor((gdImagePtr)m_imout, use_fill->color());
        int gdcbg = ConvertColor((gdImagePtr)m_imout, use_fill->background());

        gdImagePtr fillpat = NULL;

        if (wcscmp(use_fill->pattern().c_str(), L"Solid") != 0)
        {
            fillpat = GDFillPatterns::CreatePatternBitmap(use_fill->pattern().c_str(), gdc, gdcbg);
            gdImageSetTile((gdImagePtr)m_imout, fillpat);
        }

        //call the new rasterizer
        m_polyrasterizer->FillPolygon((Point*)m_wtPointBuffer, workbuffer->point_count(), workbuffer->cntrs(), workbuffer->cntr_count(),
            fillpat? gdTiled : gdc, (gdImagePtr)m_imout);

        if (fillpat)
        {
            gdImageSetTile((gdImagePtr)m_imout, NULL);
            gdImageDestroy(fillpat);
        }
    }

    //write out the polygon outline as a bunch of WT_Polylines
    if (use_fill->outline().color().alpha() == 0)
        return;

    bool deleteBuffer = false;

    //apply line style if needed
    if ((wcscmp(use_fill->outline().style().c_str(), L"Solid") != 0))
    {
        //TODO: we should simplify the math that does all that pixel-based stuff
        workbuffer = ApplyLineStyle(workbuffer, (wchar_t*)use_fill->outline().style().c_str(),
            use_fill->outline().width() * m_dpi / 0.0254 /*LineStyle works in pixels*/,
            m_drawingScale, /* pixels per map unit */
            m_dpi /* dpi */ );
        deleteBuffer = true;
    }

    if (workbuffer)
    {
        WritePolylines(workbuffer, use_fill->outline(), true);

        if (deleteBuffer)
            delete workbuffer; //it's not allocated on the line buffer pool
    }
}


void GDRenderer::ProcessPolyline(LineBuffer* srclb,
                                 RS_LineStroke& lsym)
{
    RS_LineStroke* use_lsym = &lsym;

    //are we drawing a selection?
    if (m_bSelectionMode)
        use_lsym = &(m_selFill.outline());

    if (srclb->point_count() == 0)
        return;

    if (use_lsym->color().alpha() == 0)
        return;

    bool deleteBuffer = false;

    //apply line style if needed
    LineBuffer* workbuffer = srclb;

    if ((wcscmp(use_lsym->style().c_str(), L"Solid") != 0))
    {
        //TODO: we should simplify the math that does all that pixel-based stuff
        workbuffer = ApplyLineStyle(workbuffer, (wchar_t*)use_lsym->style().c_str(),
            use_lsym->width() * m_dpi / 0.0254 /*LineStyle works in pixels*/,
            m_drawingScale, /* pixels per map unit */
            m_dpi /* dpi */ );
        deleteBuffer = true;
    }

    if (workbuffer)
    {
        WritePolylines(workbuffer, *use_lsym, true);

        if (deleteBuffer)
            delete workbuffer;
    }
}


void GDRenderer::ProcessRaster(unsigned char* data,
                               int length,
                               RS_ImageFormat format,
                               int width, int height,
                               RS_Bounds extents)
{
    double cx = (extents.minx + extents.maxx) * 0.5;
    double cy = (extents.miny + extents.maxy) * 0.5;
    WorldToScreenPoint(cx, cy, cx, cy);

    //pass to the screen space render function
    DrawScreenRaster(data, length, format, width, height, 
        cx, cy, extents.width() * m_scale, extents.height() * m_scale, 0.0);
}


void GDRenderer::ProcessMarker(LineBuffer* srclb, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds)
{
    RS_MarkerDef use_mdef = mdef;

    //use the selection style to draw
    if (m_bSelectionMode)
           use_mdef = RS_MarkerDef( mdef.width(),
                                    mdef.height(),
                                    0.5, 0.5,
                                    mdef.rotation(),
                                    mdef.units(),
                                    L"", SLD_SQUARE_NAME,
                                    m_selFill);

    for (int i=0; i<srclb->point_count(); i++)
    {
        //if marker is processed from here it should be added to the
        //feature W2D, not the labeling W2D -- need the API to reflect that.
        ProcessOneMarker(srclb->points()[2*i], srclb->points()[2*i+1], use_mdef, allowOverpost, (i==0)? bounds : NULL);
    }
}


void GDRenderer::ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds)
{
    RS_InputStream* symbol = NULL;
    bool is_font_symbol = !(mdef.library().empty() || wcsstr(mdef.library().c_str(), L"Library://"));

    //attempt to retrieve the symbol
    if (m_symbolManager && !mdef.library().empty() && !mdef.name().empty() && !is_font_symbol)
    {
        //
        // BOGUS:
        // We need to pass in "symbols" as the name of the data for the
        // symbol library resource. This is hardcoded right now.
        // If it ever changes, we will need to update it
        //
        symbol = (RS_InputStream*)m_symbolManager->GetSymbolData(mdef.library().c_str(), /*NOXLATE*/L"symbols.dwf");
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
    double anglerad = mdef.rotation() * M_PI180;

    if (!symbol && is_font_symbol)
    {
        //font symbol

        m_lastSymbol = mdef;

        //convert font symbol to a simple label and sent to label manager
        RS_TextDef tdef(RS_HAlignment_Center, RS_VAlignment_Half);
        RS_FontDef fdef(mdef.library(), mdef.height(), mdef.fontstyle(), mdef.units());

        tdef.font() = fdef;
        tdef.color() = mdef.style().color();
        tdef.rotation() = mdef.rotation();

        //compute placement position for character centerpoint, taking into
        //account the insertion point.
        //TODO: is this way of computing insertion point offset correct, when
        //compared to what SymbolTrans does? Is rotation applied before the symbol
        //is moved to its insertion point or after? Need to check that.
        double posx = dst.minx + (0.5 - refX) * dst.width();
        double posy = dst.miny + (0.5 - refY) * dst.height();

        m_labeler->ProcessLabel(posx, posy, mdef.name(), tdef);
    }
    else
    {
        //for non-font symbols we will draw into a temporary image
        //which we can reuse if the symbol is not changing from one
        //feature to the next

        //check to see if the last symbol we got was different or if it
        //is not cached yet
        if (   mdef.library() != m_lastSymbol.library()
            || mdef.name() != m_lastSymbol.name()
            || mdef.rotation() != m_lastSymbol.rotation()
            || mdef.width() != m_lastSymbol.width()
            || mdef.height() != m_lastSymbol.height()
            || mdef.style().outline().color().argb() != m_lastSymbol.style().outline().color().argb()
            || mdef.style().color().argb() != m_lastSymbol.style().color().argb()
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

            if (!symbol)
            {
                //SLD symbol (i.e. smybol from library is NULL)

                RS_F_Point* poly = NULL;
                int npts = 0;

                //determine which SLD symbol we need to draw
                //and pick up its polygon point definition
                const wchar_t* nm = mdef.name().c_str();

                if (wcscmp(nm, SLD_CIRCLE_NAME) == 0)
                {
                    poly = (RS_F_Point*)SLD_CIRCLE;
                    npts = sizeof(SLD_CIRCLE) / (2 * sizeof(double));
                }
                else if (wcscmp(nm, SLD_TRIANGLE_NAME) == 0)
                {
                    poly = (RS_F_Point*)SLD_TRIANGLE;
                    npts = sizeof(SLD_TRIANGLE) / (2 * sizeof(double));
                }
                else if (wcscmp(nm, SLD_STAR_NAME) == 0)
                {
                    poly = (RS_F_Point*)SLD_STAR;
                    npts = sizeof(SLD_STAR) / (2 * sizeof(double));
                }
                else if (wcscmp(nm, SLD_CROSS_NAME) == 0)
                {
                    poly = (RS_F_Point*)SLD_CROSS;
                    npts = sizeof(SLD_CROSS) / (2 * sizeof(double));
                }
                else if (wcscmp(nm, SLD_X_NAME) == 0)
                {
                    poly = (RS_F_Point*)SLD_X;
                    npts = sizeof(SLD_X) / (2 * sizeof(double));
                }
                else if (wcscmp(nm, SLD_SQUARE_NAME) == 0)
                {
                    poly = (RS_F_Point*)SLD_SQUARE;
                    npts = sizeof(SLD_SQUARE) / (2 * sizeof(double));
                }
                else
                {
                    //default is a square
                    poly = (RS_F_Point*)SLD_ERROR;
                    npts = sizeof(SLD_ERROR) / (2 * sizeof(double));
                    nm = NULL;
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

                        pts[i].x = (int)(tempx * (superw - 10.) + 5.);
                        pts[i].y = (int)((superh - 10.) - tempy * (superh - 10.) + 5.);
                    }

                    // initialize the temporary supersampled symbol image to a transparent background
                    gdImageAlphaBlending(tmp, 0);
                    gdImageFilledRectangle(tmp, 0, 0, gdImageSX(tmp)-1, gdImageSY(tmp)-1, 0x7f000000);
                    gdImageAlphaBlending(tmp, 1);

                    if (!nm)
                    {
                        //unknown symbol or symbol library error
                        RS_Color red(255, 0, 0, 255);
                        gdImagePtr brush1 = rs_gdImageThickLineBrush(rs_min(superw, superh) / 17, red);
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

                        gdImagePtr brush1 = rs_gdImageThickLineBrush(rs_min(superw, superh) / 17, outline);
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
                    gdImageCopyResampled((gdImagePtr)m_imsym, tmp, 0, 0, 0, 0,
                        imsymw, imsymh, superw, superh);

                    gdImageDestroy(tmp);
                }
                else
                {
                    //otherwise symbol was too big and must be drawn as a regular polygon

                    //construct transformer
                    RS_Bounds src(0.0, 0.0, 1.0, 1.0);
                    SymbolTrans trans(src, dst, refX, refY, anglerad);

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

                    if (!nm)
                    {
                        //unknown symbol or symbol library error
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
            else
            {
                //dwf symbol from the library

                //default bounds of symbol data in W2D
                //for symbols created by MapGuide Studio
                RS_Bounds src(0,0,SYMBOL_MAX,SYMBOL_MAX);
                SymbolTrans st = SymbolTrans(src, dst, refX, refY, anglerad);

                if (m_imsym)
                {
                    //case where we will cache the symbol image

                    //we will use unrotated symbol bounding box
                    //since rotation will be done by the image copy
                    //also we will use a slight boundary offset
                    //hardcoded to 1 pixel so that geometry exactly on the edge
                    //draws just inside the image
                    RS_Bounds dst1(1, 1, (double)(imsymw-1), (double)(imsymh-1));
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
            RS_Bounds src(0,0,imsymw,imsymh);
            SymbolTrans trans(src, dst, refX, refY, anglerad);

            //initialize 4 corner points of symbol -- we will
            //destructively transform this array to destination map space
            RS_F_Point b[4];
            b[0].x = 0.0; b[0].y = 0.0;
            b[1].x = imsymw; b[1].y = 0.0;
            b[2].x = imsymw; b[2].y = imsymh;
            b[3].x = 0.0; b[3].y = imsymh;

            for (int i=0; i<4; i++)
            {
                //to world space
                trans.TransformPoint(b[i].x, b[i].y);

                //world to screen space
                WorldToScreenPoint(b[i].x, b[i].y, b[i].x, b[i].y);
            }

            //copy symbol image into destination image
            if (anglerad == 0.0)
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
                int mx = (int)floor(0.5 * (b[0].x + b[2].x));
                int my = (int)floor(0.5 * (b[0].y + b[2].y));

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
        RS_Bounds src(0,0,SYMBOL_MAX,SYMBOL_MAX);

        //a square that we will transform into the dst bounds
        RS_F_Point box[5];
        box[0].x = 0;
        box[0].y = 0;
        box[1].x = SYMBOL_MAX;
        box[1].y = 0;
        box[2].x = SYMBOL_MAX;
        box[2].y = SYMBOL_MAX;
        box[3].x = 0;
        box[3].y = SYMBOL_MAX;

        //construct transformer -- same as the
        //one used for the actual symbol drawables
        SymbolTrans boxtrans(src, dst, refX, refY, anglerad);

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

        m_labeler->AddExclusionRegion(pts, 4);
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


void GDRenderer::ProcessLabel(double x, double y, const RS_String& text, RS_TextDef& tdef)
{
    //check if we are rendering a selection -- bail if so
    if (m_bSelectionMode)
        return;

    //forward it to the label renderer
    m_labeler->ProcessLabel(x, y, text, tdef);
}


void GDRenderer::ProcessLabelGroup(RS_LabelInfo*    labels,
                                   int              nlabels,
                                   const RS_String& text,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path)
{
    //check if we are rendering a selection -- bail if so
    if (m_bSelectionMode)
        return;

    //forward it to the label renderer
    m_labeler->ProcessLabelGroup(labels, nlabels, text, type, exclude, path);
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
    return m_fcInfo;
}


double GDRenderer::GetMapScale()
{
    return m_mapScale;
}


RS_Bounds& GDRenderer::GetBounds()
{
    return m_extents;
}


double GDRenderer::GetDpi()
{
    return m_dpi;
}


double GDRenderer::GetMetersPerUnit()
{
    return m_metersPerUnit;
}


double GDRenderer::GetMapToScreenScale()
{
    return m_scale;
}


bool GDRenderer::RequiresClipping()
{
    return m_bRequiresClipping;
}


void GDRenderer::WritePolylines(LineBuffer* srclb, RS_LineStroke& stroke, bool aa)
{
    //apply the stroke first

    int gdc = ConvertColor((gdImagePtr)m_imout, stroke.color());
    gdImageSetAntiAliased((gdImagePtr)m_imout, gdc);

    double thickness = stroke.width();

    //convert thickness to equivalent mapping space width
    int line_weight = (int)(_MeterToMapSize(stroke.units(), fabs(thickness)) * m_scale);

    gdImagePtr brush1 = NULL;

    if (line_weight > 1)
    {
        brush1 = rs_gdImageThickLineBrush(line_weight, stroke.color());
    }

    //draw the lines
    int index = 0;

    for (int i=0; i<srclb->cntr_count(); i++)
    {
        int cntr_size = srclb->cntrs()[i];

        if (cntr_size > 1)
        {
            _TransformPointsNoClamp(srclb->points()+index, cntr_size);

            //draw antialiased only if thickness is single pixel
            if (aa && line_weight <= 1)
                gdImageOpenPolygon((gdImagePtr)m_imout, (gdPointPtr)m_wtPointBuffer, cntr_size, gdAntiAliased);
            else
            {
                gdImageSetBrush((gdImagePtr)m_imout, brush1);
                gdImageOpenPolygon((gdImagePtr)m_imout, (gdPointPtr)m_wtPointBuffer, cntr_size, brush1 ? gdBrushed : gdc);
            }
        }

        index += 2*cntr_size;
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
    return m_height /*-1*/ - (int)floor((y - m_offsetY) * m_scale);
}


//transforms an array of input mapping space points
//into W2D coordinates and places them in the DWF
//point buffer m_wtPointBuffer
void GDRenderer::_TransformPointsNoClamp(double* inpts, int numpts)
{
    EnsureBufferSize(numpts);
    int* wpts = (int*)m_wtPointBuffer;

    int h1 = m_height /*- 1*/;
    double x,y;
    int ix, iy;

    for (int i=0; i<numpts; i++)
    {
        x = (*inpts++ - m_offsetX) * m_scale;
        y = (*inpts++ - m_offsetY) * m_scale;

        //TODO: slow
        ix = ROUND(x);
        iy = ROUND(y);

        *wpts++ = ix;
        *wpts++ = h1 - iy;
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
LineBuffer* GDRenderer::ApplyLineStyle(LineBuffer* srcLB, wchar_t* lineStyle, double lineWidth, double drawingScale, double dpi)
{
    // if there are less than two indices then we can't style
    int lenInds = srcLB->point_count();
    if (lenInds < 2)
        return NULL;

    LineStyleDef lineStyleDef;
    LineStyle style = lineStyleDef.FindLineStyle(lineStyle);
    // configure the line style definition to use
    lineStyleDef.SetStyle(style, drawingScale, dpi, lineWidth);

    // the starting pixel run
    int pixelRunInd = 0;
    PixelRun pixelRun = lineStyleDef.m_pixelRuns[pixelRunInd];

    // create the destination line buffer
    LineBuffer* destLB = new LineBuffer(8);

    // special code for Fenceline1 style
    int numCapSegs = 0;
    double csCapAng = 0.0, snCapAng = 0.0;
    if (style == LineStyle_FENCELINE1)
    {
        numCapSegs = (int)ceil(M_PI * sqrt(0.5*lineStyleDef.m_pixelRuns[1].m_decorSize));

        // restrict this value to be safe
        // TODO: what's the correct upper limit
        if (numCapSegs > 32)
            numCapSegs = 32;

        // angle increment
        double dAngle = 2.0*M_PI / numCapSegs;
        csCapAng = cos(dAngle);
        snCapAng = sin(dAngle);
    }

    // loop over the source buffer's segments
    double walkLen = 0.0;
    double sumdLen = 0.0;
    for (int j=1; j<lenInds; j++)
    {
        // get the vertex and index arrays from the line
        // buffer - these are inside the loop because they
        // change if the line buffer resizes itself
        double* srcLBpts = srcLB->points();
        unsigned char* srcLBtypes = srcLB->types();

        // don't style if current segment is empty (a move-to)
        if (srcLBtypes[j] == (unsigned char)LineBuffer::stMoveTo)
            continue;

        // get the data for the segment
        double p0x = srcLBpts[2*j-2];
        double p0y = srcLBpts[2*j-1];
        double p1x = srcLBpts[2*j  ];
        double p1y = srcLBpts[2*j+1];

        double dx = p1x - p0x;
        double dy = p1y - p0y;

        // detect a previous move-to
        if (srcLBtypes[j-1] == (unsigned char)LineBuffer::stMoveTo)
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
                pixelRunInd++;
                if (pixelRunInd == lineStyleDef.m_nRuns)
                    pixelRunInd = 0;
                pixelRun = lineStyleDef.m_pixelRuns[pixelRunInd];
            }
        }
        while (!doneSeg);

        // update the summed line length
        sumdLen += lineLen;
    }

    return destLB;
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


//Converts a given pixel distance to mapping units
//for use when slightly offsetting a label from the symbol
//bounds so that the text is more readable
double GDRenderer::_PixelToMapSize(Renderer* renderer, int pixels)
{
    // formula is as follows:
    //
    // Mapping Distance = Pixel Distance * (meters/pixel) * mapscale / (meters/map unit)
    //

    return (double)pixels * (0.0254 / renderer->GetDpi()) * renderer->GetMapScale() / renderer->GetMetersPerUnit();
}


void GDRenderer::SetRenderSelectionMode(bool mode)
{
    SE_Renderer::SetRenderSelectionMode(mode);

    //initialize the selection styles if needed
    if (mode)
    {
        RS_Color selLineColor = RS_Color(0, 0, 255, 200);
        RS_Color selFillColor = RS_Color(0, 0, 255, 160);
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

//////////////////////////////////////////////////////////////////////////////
void GDRenderer::DrawString(const RS_String& s,
                            int              x,
                            int              y,
                            double           height,
                            const RS_Font*   font,
                            const RS_Color&  color,
                            double           anglerad)
{
    if (font == NULL)
        return;

    //gd likes height in points rather than pixels
    height *= 72.0 / m_dpi;

    // The computed height can have roundoff in it, and the rendering code is
    // very sensitive to it.  Remove this roundoff by rounding the height to
    // the nearest 1/65536ths of a point.
    height = floor(height * 65536.0 + 0.5) / 65536.0;

    //convert input to UTF8, which is what GD uses
    size_t len = s.length();
    size_t lenbytes = len*4+1;
    char* sutf8 = (char*)alloca(lenbytes);
    DWFString::EncodeUTF8(s.c_str(), len * sizeof(wchar_t), sutf8, lenbytes);

    //convert font path to utf8 also
    size_t lenf = font->m_filename.length();
    size_t lenbytesf = lenf * 4 + 1;
    char* futf8 = (char*)alloca(lenbytesf);
    DWFString::EncodeUTF8(font->m_filename.c_str(), lenf * sizeof(wchar_t), futf8, lenbytesf);

    //draw the string
    int gdc = ConvertColor((gdImagePtr)m_imout, (RS_Color&)color);
    char* err = NULL;
    err = gdImageStringFT((gdImagePtr)m_imout, NULL, gdc, futf8, height, anglerad, x, y, sutf8);

#ifdef _DEBUG
    if (err) printf("gd text error : %s\n", err);
#endif
}


//////////////////////////////////////////////////////////////////////////////
void GDRenderer::MeasureString(const RS_String&  s,
                               double            height,
                               const RS_Font*    font,
                               double            anglerad,
                               RS_F_Point*       res, //assumes 4 points in this array
                               float*            offsets) //assumes length equals 2 * length of string
{
    //gd likes height in points rather than pixels
    height *= 72.0 / m_dpi;

    // The computed height can have roundoff in it, and the rendering code is
    // very sensitive to it.  Remove this roundoff by rounding the height to
    // the nearest 1/65536ths of a point.
    height = floor(height * 65536.0 + 0.5) / 65536.0;

    //convert input to UTF8, which is what GD uses
    size_t len = s.length();
    size_t lenbytes = len*4+1;
    char* sutf8 = (char*)alloca(lenbytes);
    DWFString::EncodeUTF8(s.c_str(), len * sizeof(wchar_t), sutf8, lenbytes);

    //convert font path to utf8 also
    size_t lenf = font->m_filename.length();
    size_t lenbytesf = lenf * 4 + 1;
    char* futf8 = (char*)alloca(lenbytesf);
    DWFString::EncodeUTF8(font->m_filename.c_str(), lenf * sizeof(wchar_t), futf8, lenbytesf);

    int extent[8];
    gdFTStringExtra extra;
    memset(&extra, 0, sizeof(gdFTStringExtra));
    extra.flags |= gdFTEX_XSHOW;
    char* err = NULL;
    err = gdImageStringFTEx((gdImagePtr)NULL, (int*)&extent[0], 0, futf8, height, anglerad, 0, 0, sutf8, offsets? &extra : NULL);

    for (int i=0; i<4; i++)
    {
        res[i].x = extent[2*i];
        res[i].y = extent[2*i+1];
    }

#ifdef _DEBUG
    if (err) printf ("gd text error : %s\n", err);
#endif

    if (extra.xshow && offsets)
    {
        //copy over character spacings into result array
        //there are 2 numbers per character -- kerned and unkerned delta
        memcpy(offsets, extra.xshow, sizeof(float) * (len - 1));

        //and then release the gd allocated xshow pointer
        gdFree(extra.xshow);
    }
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
        oReader.getPackageInfo( tInfo );

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
                                _DWFCORE_THROW(DWFMemoryException,L"Out of memory");
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
    catch (DWFException& )
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

    m_pPool = new LineBufferPool();

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

    delete m_pPool;
    m_pPool = NULL;

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

    LineBuffer* lb = m_pPool->NewLineBuffer(numpts);
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
                m_pPool->FreeLineBuffer(lb);
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
            _TransformPointsNoClamp(lb->points(), lb->point_count());
        }
    }
    else
    {
        //for symbols just copy the points to the output array
        //and only invert the y coordinate -- we need to flip y since
        //in gd y goes down and in DWF it goes up
        double* srcpts = lb->points();
        int count = lb->point_count();

        EnsureBufferSize(count);
        RS_D_Point* wpts = m_wtPointBuffer;

        for (int i=0; i<count; i++)
        {
            wpts[i].x = (int)*srcpts++;
            wpts[i].y = gdImageSY((gdImagePtr)GetW2DTargetImage()) - (int)*srcpts++;
        }
    }

    //free temporary linebuffer
    m_pPool->FreeLineBuffer(lb);

    return m_wtPointBuffer;
}


//This function scales a W2D space related value from
//source W2D space to destination.
//Since the source W2D file can fit into a small piece of the
//destination DWF or be much larger, we need to take that
//scaling into account when carrying over things like line weight,
//font height, etc. this helper funtion determines and applies
//that scale
double GDRenderer::ScaleW2DNumber(WT_File&     file,
                                        WT_Integer32 number)
{
    WT_Matrix xform = file.desired_rendition().drawing_info().units().dwf_to_application_adjoint_transform();

    double scale1 = 1.0 / xform(0,0); //div because we need inverse

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

void GDRenderer::UpdateSymbolTrans(WT_File& /*file*/, WT_Viewport& viewport)
{
    _ASSERT(m_xformer);

    RS_Bounds alternate_extent(0,0,-1,-1);

    //If a viewport was defined, the symbol W2D likely came from AutoCAD.
    //In that case, the extent of the data inside the W2D is not the same
    //as what Studio saves (0,0,SYMBOL_MAX,SYMBOL_MAX), so we need to use
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
            alternate_extent.minx = rs_min( pts[0].m_x, pts[2].m_x);
            alternate_extent.maxx = rs_max( pts[0].m_x, pts[2].m_x);

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
                    double oldcx = 0.5 * ( dstext.minx + dstext.maxx);
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


void GDRenderer::_TransferPoints(double* src, int numpts, const SE_Matrix* xform)
{
    EnsureBufferSize(numpts);
    int* pts = (int*)m_wtPointBuffer;

    if (!xform)
    {
        for (int i=0; i<numpts; i++)
        {
            *pts++ = (int)(*src);
            src++;
            *pts++ = (int)(*src);
            src++;
        }
    }
    else
    {
        for (int i=0; i<numpts; i++)
        {
            double x, y;
            xform->transform(src[0], src[1], x, y);
            pts[0] = (int)x;
            pts[1] = (int)y;

            src += 2;
            pts += 2;
        }
    }
}


//copied from WritePolylines, except it doesn't do to screen trasnform -- we should refactor.
void GDRenderer::DrawScreenPolyline(LineBuffer* srclb, const SE_Matrix* xform, unsigned int color, double weightpx)
{
    RS_Color c((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF);

    int gdc = ConvertColor((gdImagePtr)m_imout, c);
    gdImageSetAntiAliased((gdImagePtr)m_imout, gdc);

    //line width is always device space and units are meters.
    //so convert to equivalent pixel width
    int line_weight = (int)weightpx;

    gdImagePtr brush1 = NULL;

    if (line_weight > 1)
    {
        brush1 = rs_gdImageThickLineBrush(line_weight, c);
    }

    //draw the lines
    int index = 0;

    for (int i=0; i<srclb->cntr_count(); i++)
    {
        int cntr_size = srclb->cntrs()[i];

        //convert to integer coords
        _TransferPoints(srclb->points() + index, cntr_size, xform);

        if (cntr_size > 1)
        {
            //draw antialiased only if thickness is single pixel
            if (line_weight <= 1)
                gdImageOpenPolygon((gdImagePtr)m_imout, (gdPointPtr)m_wtPointBuffer, cntr_size, gdAntiAliased/*gdc*/);
            else
            {
                gdImageSetBrush((gdImagePtr)m_imout, brush1);
                gdImageOpenPolygon((gdImagePtr)m_imout, (gdPointPtr)m_wtPointBuffer, cntr_size, brush1 ? gdBrushed : gdc);
            }
        }

        index += 2*cntr_size;
    }

    //unset the stroke
    gdImageSetBrush((gdImagePtr)m_imout, NULL);

    if (brush1)
        gdImageDestroy(brush1);
}


void GDRenderer::DrawScreenPolygon(LineBuffer* polygon, const SE_Matrix* xform, unsigned int color)
{
    RS_Color c((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF);

    if (polygon->point_count() == 0)
        return;

    if (c.alpha() != 0)
    {
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

        _TransferPoints(polygon->points(), polygon->point_count(), xform);

        //call the new rasterizer
        m_polyrasterizer->FillPolygon((Point*)m_wtPointBuffer, polygon->point_count(), polygon->cntrs(), polygon->cntr_count(),
            fillpat? gdTiled : gdc, (gdImagePtr)m_imout);

        /*
        if (fillpat)
        {
            gdImageSetTile((gdImagePtr)m_imout, NULL);
            gdImageDestroy(fillpat);
        }
        */
    }
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


double GDRenderer::GetPixelsPerMillimeterScreen()
{
    return m_dpi / 25.4;
}


double GDRenderer::GetPixelsPerMillimeterWorld()
{
    return m_dpi / 25.4 / m_mapScale;
}

RS_FontEngine* GDRenderer::GetFontEngine()
{
    return this;
}

//labeling -- this is the entry API for adding SE labels
//to the label mananger
void GDRenderer::ProcessLabelGroup(SE_LabelInfo*    labels,
                                   int              nlabels,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path)
{
    //pass labels to the label renderer here

    //check if we are rendering a selection -- bail if so
    if (m_bSelectionMode)
        return;

    //forward it to the label renderer
    m_labeler->ProcessLabelGroup(labels, nlabels, type, exclude, path);
}


void GDRenderer::DrawScreenRaster(unsigned char* data, int length, RS_ImageFormat format, int native_width, int native_height, 
        double x, double y, double w, double h, double angledeg)
{
    if (format == RS_ImageFormat_RGBA)
    {
        gdImagePtr src = gdImageCreateTrueColor(native_width, native_height);

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

        if (angledeg == 0)
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
            //TODO: must scale from native width/height to requested width/height
            gdImageCopyRotated((gdImagePtr)m_imout, src, x, y, 0, 0, native_width, native_height, ROUND(angledeg));
        }

        gdImageDestroy(src);
    }
    else if (format == RS_ImageFormat_PNG)
    {
        //note width and height arguments are ignored for PNG, since they are encoded
        //in the png data array

        gdImagePtr src = gdImageCreateFromPngPtr(length, data);

        if (angledeg == 0)
        {
            double w2 = w * 0.5;
            double h2 = h * 0.5;

            int minx = ROUND(x - w2);
            int maxx = ROUND(x + w2);
            int miny = ROUND(y - h2);
            int maxy = ROUND(y + h2);

            gdImageCopyResampled((gdImagePtr)m_imout, src,
                                 minx, miny, 0, 0,
                                 maxx-minx, maxy-miny,
                                 gdImageSX(src), gdImageSY(src));
        }
        else
        {
            //TODO: must scale from native width/height to requested width/height
            gdImageCopyRotated((gdImagePtr)m_imout, src, x, y, 0, 0, gdImageSX(src), gdImageSY(src), ROUND(angledeg));
        }

        gdImageDestroy(src);
    }
    else
        throw "Well implement this already!";
}


void GDRenderer::DrawScreenText(const RS_String& txt, RS_TextDef& tdef, double insx, double insy, double* path, int npts, double param_position)
{
    if (path)  //path text
    {
        RS_TextMetrics tm;
        GetTextMetrics(txt, tdef, tm, true);
        //TODO: need computed seglens rather than NULL to make things faster
        LayoutPathText(tm, (RS_F_Point*)path, npts, NULL, param_position, tdef.valign(), 0);
        DrawPathText(tm, tdef);
    }
    else //block text
    {
        RS_TextMetrics tm;
        GetTextMetrics(txt, tdef, tm, false);
        DrawBlockText(tm, tdef, insx, insy);
    }
}
