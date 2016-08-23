//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#include "AGGRenderer.h"
#include "LineStyle.h"
#include "SLDSymbols.h"
#include "LineStyleDef.h"
#include "TransformMesh.h"

#include "AGGImageIO.h"

#include "FontManager.h"

#include "ConvertUTF.h"
#include "UnicodeString.h"
#include "AGGFillPatterns.h"
#include "AGGW2DRewriter.h"
#include "whiptk/whip_toolkit.h"

#include "LabelRenderer.h"
#include "LabelRendererLocal.h"

#include "SymbolTrans.h"

#include "agg_context.h"

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

#include "DWFRSInputStream.h"
#include "RSDWFInputStream.h"

#include "SE_SymbolDefProxies.h"

#include "RenderUtil.h"

using namespace DWFToolkit;
using namespace DWFCore;


#define ROUND(x) (int)((x) + 0.5)

#define SYMBOL_BITMAP_SIZE 128
#define SYMBOL_BITMAP_MAX 1024

#ifndef _WIN32

// Linux version of GetTickCount()
// Note: The Windows version of GetTickCount() returns the results in milliseconds.
#include <sys/time.h>

long GetTickCount()
{
    struct timeval time;
    long mtime, seconds, useconds;

    gettimeofday(&time, NULL);
    seconds  = time.tv_sec;
    useconds = time.tv_usec;
    mtime = ((seconds) * 1000 + (long)(useconds/1000.0));
    return mtime;
}

#endif


bool AGGRenderer::s_bClampPoints    = false;
bool AGGRenderer::s_bGeneralizeData = false;


//////////////////////////////////////////////////////////////////////////////
// constructor that allows a backbuffer
AGGRenderer::AGGRenderer(int width,
                         int height,
                         unsigned int* backbuffer,
                         bool requiresClipping,
                         bool localOverposting,
                         double tileExtentOffset)
:
m_width(width),
m_height(height),
m_bgcolor(RS_Color(0xFFFFFF)),
m_extents(0.0, 0.0, 0.0, 0.0),
m_symbolManager(NULL),
m_mapInfo(NULL),
m_layerInfo(NULL),
m_fcInfo(NULL),
m_bIsSymbolW2D(false),
m_bHaveViewport(false),
m_bRequiresClipping(requiresClipping),
m_bLocalOverposting(localOverposting),
m_imsym(NULL),
m_bownbuffer(false),
m_lastFontDef(),
m_lastFont(NULL)
{
    m_rows = backbuffer;

    // get the agg context going
    m_context = new agg_context(m_rows, m_width, m_height);

    // clear the buffer
    // NOTE: we are using the agg blender that assumes the rendering
    // buffer stores premultiplied alpha values - so we clear with the
    // premultiplied background color
//  c()->ren.clear(agg::argb8_packed(bgColor.argb()).premultiply());
//  c()->ren.clear(agg::argb8_packed(0x000000ff).premultiply());

    if (!m_bLocalOverposting)
        m_labeler = new LabelRenderer(this);
    else
        m_labeler = new LabelRendererLocal(this, tileExtentOffset);
}


//////////////////////////////////////////////////////////////////////////////
//default constructor
AGGRenderer::AGGRenderer(int width,
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
m_symbolManager(NULL),
m_mapInfo(NULL),
m_layerInfo(NULL),
m_fcInfo(NULL),
m_bIsSymbolW2D(false),
m_bHaveViewport(false),
m_bRequiresClipping(requiresClipping),
m_bLocalOverposting(localOverposting),
m_imsym(NULL),
m_bownbuffer(true),
m_lastFontDef(),
m_lastFont(NULL)
{
    if (m_width <= 0)
        m_width = 1;
    if (m_height <= 0)
        m_height = 1;
    if (m_width > 16384)
        m_width = 16384;
    if (m_height > 16384)
        m_height = 16384;

    // allocate back buffer
    int len = m_width*m_height;
    m_rows = new unsigned int[len];

    // get the agg context going
    m_context = new agg_context(m_rows, m_width, m_height);

    // clear the buffer
    // NOTE: we are using the agg blender that assumes the rendering
    // buffer stores premultiplied alpha values - so we clear with the
    // premultiplied background color
    c()->ren.clear(agg::argb8_packed(bgColor.argb()).premultiply());

    if (!m_bLocalOverposting)
        m_labeler = new LabelRenderer(this);
    else
        m_labeler = new LabelRendererLocal(this, tileExtentOffset);
}


//////////////////////////////////////////////////////////////////////////////
AGGRenderer::~AGGRenderer()
{
    delete m_context;
    delete m_labeler;
    delete m_imsym;

    if (m_bownbuffer)
        delete[] m_rows;
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::UpdateBackBuffer(int width, int height, unsigned int* backbuffer)
{
    if (m_bownbuffer)
        delete[] m_rows;

    m_rows = backbuffer;
    m_width = width;
    m_height = height;
    m_bownbuffer = false;

    // get the agg context going
    delete m_context;
    m_context = new agg_context(m_rows, m_width, m_height);
}


//////////////////////////////////////////////////////////////////////////////
unsigned int* AGGRenderer::GetBackBuffer(int &width, int& height)
{
    width = m_width;
    height = m_height;
    return m_rows;
}


//////////////////////////////////////////////////////////////////////////////
// This one writes into a file not returning a bytestream using
// default parameters for the width and height.
void AGGRenderer::Save(const RS_String& filename, const RS_String& format)
{
    Save(filename, format, m_width, m_height);
}


//////////////////////////////////////////////////////////////////////////////
// This one writes into a file not returning a bytestream.
void AGGRenderer::Save(const RS_String& filename, const RS_String& format, int width, int height)
{
    AGGImageIO::Save(filename, format, m_rows, m_width, m_height, width, height, m_bgcolor);
}


//////////////////////////////////////////////////////////////////////////////
// Return the rendered image passed in via the imagebuffer (m_rows) as
// a bytestream in the given image format using the provided colorPalette
// if given.
RS_ByteData* AGGRenderer::Save(const RS_String& format, int width, int height,
                               RS_ColorVector* baseColorPalette)
{
    return AGGImageIO::Save(format, m_rows, m_width, m_height, width, height, m_bgcolor, baseColorPalette);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::Combine(const RS_String& fileIn1, const RS_String& fileIn2, const RS_String& fileOut)
{
    AGGImageIO::Combine(fileIn1, fileIn2, fileOut);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::StartMap(RS_MapUIInfo* mapInfo,
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

    // find scale used to convert to pixel coordinates
    // need to take aspect ratios into account
    double arDisplay = (double)m_width / (double)m_height;
    double arMap = m_extents.width() / m_extents.height();

    double scale;
    if (arDisplay > arMap)
        scale = (double)m_height / m_extents.height();
    else
        scale = (double)m_width / m_extents.width();

    m_xform.x0 = scale;
    m_xform.x1 = 0.0;
    m_xform.x2 = -scale * m_extents.minx;
    m_xform.y0 = 0.0;
    m_xform.y1 = scale;
    m_xform.y2 = -scale * m_extents.miny;

    m_ixform.x0 = 1.0 / scale;
    m_ixform.x1 = 0.0;
    m_ixform.x2 = m_extents.minx;
    m_ixform.y0 = 0.0;
    m_ixform.y1 = m_ixform.x0;
    m_ixform.y2 = m_extents.miny;

    double metersPerPixel = METERS_PER_INCH / m_dpi;

    // compute drawing scale
    // drawing scale is map scale converted to [mapping units] / [pixels]
    m_drawingScale = m_mapScale * metersPerPixel / m_metersPerUnit;

    m_labeler->StartLabels();

    // remember the map info
    m_mapInfo = mapInfo;

    // make sure that the mapInfo is in sync with the background member variable
    if (m_mapInfo)
        m_mapInfo->bgcolor() = this->m_bgcolor;

    // do it here, since we will need the renderer's map scales, which are computed above
    InitFontEngine(this);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::EndMap()
{
    // turn off selection mode so the labels draw normal
    SetRenderSelectionMode(false);

    // finally draw all the labels
    m_labeler->BlastLabels();

    // clear the map info
    m_mapInfo = NULL;
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::StartLayer(RS_LayerUIInfo*      legendInfo,
                             RS_FeatureClassInfo* classInfo)
{
    // remember the layer/feature info
    m_layerInfo = legendInfo;
    m_fcInfo = classInfo;
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::EndLayer()
{
    // clear the layer/feature info
    m_layerInfo = NULL;
    m_fcInfo = NULL;
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::StartFeature(RS_FeatureReader* /*feature*/,
                               bool              /*initialPass*/,
                               const RS_String*  /*tooltip*/,
                               const RS_String*  /*url*/,
                               const RS_String*  /*theme*/,
                               double            /*zOffset*/,
                               double            /*zExtrusion*/,
                               RS_ElevationType  /*zOffsetType*/)
{
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::ProcessPolygon(LineBuffer* lb, RS_FillStyle& fill)
{
    _ASSERT(NULL != lb);
    RS_FillStyle* use_fill = &fill;

    // should we use selection style?
    if (m_bSelectionMode)
        use_fill = &m_selFill;

    if (lb->point_count() == 0)
        return;

    // render any polygon fill
    if (use_fill->color().alpha() != 0)
    {
        if (wcscmp(use_fill->pattern().c_str(), L"Solid") != 0)
        {
            agg_context* fillpat = AGGFillPatterns::CreatePatternBitmap(use_fill->pattern().c_str(),
                use_fill->color().abgr(), use_fill->background().abgr());
            fillpat->pf.premultiply();

            typedef agg::image_accessor_wrap<mg_pixfmt_type,
                                            agg::wrap_mode_repeat,
                                            agg::wrap_mode_repeat> img_source_type;
            typedef agg::span_pattern_rgba<img_source_type> span_gen_type;

            img_source_type img_src(fillpat->pf);
            span_gen_type sg(img_src, 0, 0);

            agg::span_allocator<mg_pixfmt_type::color_type> sa;

            // transform to screen coords and transfer to AGG storage
            unsigned int* pathids = (unsigned int*)alloca(lb->geom_count() * sizeof(unsigned int));
            _TransferPoints(c(), lb, &m_xform, pathids, true);

            for (int i=0; i<lb->geom_count(); ++i)
            {
                c()->ras.reset();
                c()->ras.add_path(c()->ps, pathids[i]);
                agg::render_scanlines_aa(c()->ras, c()->sl, c()->ren_pre, sa, sg);
            }

            delete fillpat;
        }
        else
        {
            DrawScreenPolygon(lb, &m_xform, use_fill->color().argb());
        }
    }

    // render any polygon outline
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

    // apply line style if needed
    if ((wcscmp(use_lsym.style().c_str(), L"Solid") != 0))
    {
        // If we didn't generalize earlier, we have to do it now.  It's
        // necessary because the line style engine may produce lots of
        // small moveto-lineto combinations which agg will throw out
        // instead of rendering a dot, resulting in a gap.
        if (!s_bGeneralizeData)
        {
            LineBuffer* optbuffer = workbuffer->Optimize(m_drawingScale, m_pPool);
            if (optbuffer)
            {
                if (spLB.get())
                    LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());

                workbuffer = optbuffer;
                spLB.reset(workbuffer);
            }
        }

        // If we didn't clip earlier, do it now.  If we're zoomed in a
        // lot there can be huge out-of-screen sections which will incur
        // enormous overhead for stuff that's not visible.
        if (!m_bRequiresClipping)
        {
            LineBuffer* clipbuffer = workbuffer->Clip(m_extents, LineBuffer::ctLine, m_pPool);

            // check if it needed clipping
            if (workbuffer != clipbuffer)
            {
                if (spLB.get())
                    LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());

                workbuffer = clipbuffer;
                spLB.reset(workbuffer);
            }
        }

        if (workbuffer)
        {
            // TODO: we should simplify the math that does all that pixel-based stuff
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
    }

    if (workbuffer)
    {
        m_lineStroke.color = use_lsym.color().argb();

        // convert thickness to pixels
        double thickness = use_lsym.width();
        double weightpx = _MeterToMapSize(use_lsym.units(), fabs(thickness)) * m_xform.x0;
        m_lineStroke.weight = rs_max(1.0, weightpx);

        DrawScreenPolyline(workbuffer, &m_xform, m_lineStroke);

        if (spLB.get())
            LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
    }
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::ProcessPolyline(LineBuffer* lb, RS_LineStroke& lsym)
{
    _ASSERT(NULL != lb);
    RS_LineStroke* use_lsym = &lsym;

    // are we drawing a selection?
    if (m_bSelectionMode)
        use_lsym = &(m_selFill.outline());

    if (lb->point_count() == 0)
        return;

    // render any polyline outline
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

    // apply line style if needed
    if ((wcscmp(use_lsym->style().c_str(), L"Solid") != 0))
    {
        // If we didn't generalize earlier, we have to do it now.  It's
        // necessary because the line style engine may produce lots of
        // small moveto-lineto combinations which agg will throw out
        // instead of rendering a dot, resulting in a gap.
        if (!s_bGeneralizeData)
        {
            LineBuffer* optbuffer = workbuffer->Optimize(m_drawingScale, m_pPool);
            if (optbuffer)
            {
                if (spLB.get())
                    LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());

                workbuffer = optbuffer;
                spLB.reset(workbuffer);
            }
        }

        // If we didn't clip earlier, do it now.  If we're zoomed in a
        // lot there can be huge out-of-screen sections which will incur
        // enormous overhead for stuff that's not visible.
        if (!m_bRequiresClipping)
        {
            LineBuffer* clipbuffer = workbuffer->Clip(m_extents, LineBuffer::ctLine, m_pPool);

            // check if it needed clipping
            if (workbuffer != clipbuffer)
            {
                if (spLB.get())
                    LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());

                workbuffer = clipbuffer;
                spLB.reset(workbuffer);
            }
        }

        if (workbuffer)
        {
            // TODO: we should simplify the math that does all that pixel-based stuff
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
    }

    if (workbuffer)
    {
        m_lineStroke.color = use_lsym->color().argb();

        // convert thickness to pixels
        double thickness = use_lsym->width();
        double weightpx = _MeterToMapSize(use_lsym->units(), fabs(thickness)) * m_xform.x0;
        m_lineStroke.weight = rs_max(1.0, weightpx);

        DrawScreenPolyline(workbuffer, &m_xform, m_lineStroke);

        if (spLB.get())
            LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
    }
}

#define PRINT_RS_BOUNDS(ext) \
    printf("      minx = %6.4f miny = %6.4f maxx = %6.4f maxy = %6.4f\n", ext.minx, ext.miny, ext.maxx, ext.maxy); \
    printf("      width = %6.4f height = %6.4f\n", ext.width(), ext.height());


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::ProcessRaster(unsigned char* data,
                                int length,
                                RS_ImageFormat format,
                                int width,                // width of raster in pixels
                                int height,               // height of raster in pixels
                                RS_Bounds& extents,       // extents of (projected) raster image intersected with map
                                TransformMesh* xformMesh)
{
    double cx = 0.5 * (extents.minx + extents.maxx);
    double cy = 0.5 * (extents.miny + extents.maxy);
    WorldToScreenPoint(cx, cy, cx, cy);

    // width and height of (projected) image in device space
    double imgDevW = extents.width()  * m_xform.x0 + 1.0;
    double imgDevH = extents.height() * m_xform.y1 + 1.0;

    // check to see if we need to transform the image
    if (xformMesh != NULL)
    {
        // draw with transform
        DrawScreenRasterTransform(c(), data, length, format, width, height, cx, cy, imgDevW, imgDevH, xformMesh);
    }
    else
    {
        // draw without transform
        DrawScreenRaster(data, length, format, width, height, cx, cy, imgDevW, -imgDevH, 0.0);
    }
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::ProcessMarker(LineBuffer* srclb, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds)
{
    RS_MarkerDef use_mdef = mdef;

    // use the selection style to draw
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

    for (int i=0; i<srclb->point_count(); ++i)
    {
        // if marker is processed from here it should be added to the
        // feature W2D, not the labeling W2D - need the API to reflect that.
        ProcessOneMarker(srclb->x_coord(i), srclb->y_coord(i), use_mdef, allowOverpost, (i==0) ? bounds : NULL);
    }
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds)
{
    RS_InputStream* symbol = NULL;

    // attempt to retrieve the symbol
    if (m_symbolManager && (mdef.type() == RS_MarkerType_W2D))
    {
        // BOGUS:
        // We need to pass in "symbols" as the name of the data for the
        // symbol library resource. This is hardcoded right now.
        // If it ever changes, we will need to update it
        symbol = (RS_InputStream*)m_symbolManager->GetSymbolData(mdef.library().c_str(), L"symbols.dwf");
    }

    // symbol in mapping space units
    double iw = _MeterToMapSize(mdef.units(), mdef.width());
    double ih = _MeterToMapSize(mdef.units(), mdef.height());

    // unrotated symbol bounding box (mapping space)
    RS_Bounds dst(x, y, x+iw, y+ih);

    // convert to pixel size
    iw *= m_xform.x0;
    ih *= m_xform.y1;

    // if it's too big, make it smaller
    // it cannot be too big since we allocate 4*iw*ih bytes of
    // memory to cache the symbol image - this code simply truncates
    // the size, ignoring changes in aspect ratio
    const double MAX_SIZE = 4096.0;
    if (iw > MAX_SIZE)
    {
        iw = MAX_SIZE;
        dst.maxx = dst.minx + iw * m_ixform.x0;
    }
    if (ih > MAX_SIZE)
    {
        ih = MAX_SIZE;
        dst.maxy = dst.miny + ih * m_ixform.y1;
    }

    // get insertion point
    double refX = mdef.insx();
    double refY = mdef.insy();

    // rotation angle
    double angleRad = mdef.rotation() * M_PI180;

    if (mdef.type() == RS_MarkerType_Font)
    {
        // case where we are using a font symbol

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

        // convert font symbol to a simple label
        RS_TextDef tdef(RS_HAlignment_Center, RS_VAlignment_Half);
        RS_FontDef fdef(mdef.library(), mdef.height(), mdef.fontstyle(), mdef.units());

        tdef.font() = fdef;
        tdef.textcolor() = mdef.style().color();
        tdef.rotation() = mdef.rotation();

        // compute placement position for character centerpoint, taking into
        // account the insertion point.
        // TODO: is this way of computing insertion point offset correct, when
        // compared to what SymbolTrans does? Is rotation applied before the symbol
        // is moved to its insertion point or after? Need to check that.
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
        // for non-font symbols we will draw into a temporary image
        // which we can reuse if the symbol is not changing from one
        // feature to the next

        // check to see if the last symbol we got was different or if it
        // is not cached yet
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

            int imsymw = ROUND(iw)+2;
            int imsymh = ROUND(ih)+2;

            if (!symbol || imsymw > SYMBOL_BITMAP_MAX || imsymh > SYMBOL_BITMAP_MAX)
            {
                // Symbol will be too big so we will draw it as
                // geometry directly into the map image instead.
                // So we need to destroy the cached symbol image if any.
                delete m_imsym;
                m_imsym = NULL;
            }
            else
            {
                // otherwise allocate a second bitmap where
                // we will cache the symbol image for reuse

                // did we cache an image previously, but it was different size?
                if (m_imsym
                    && (   (int)m_imsym->rb.width() != imsymw
                        || (int)m_imsym->rb.height() != imsymh))
                {
                    delete m_imsym;
                    m_imsym = NULL;
                }

                // allocate the symbol image
                if (!m_imsym)
                {
                    m_imsym = new agg_context(NULL, imsymw, imsymh);
                }
            }

            if (symbol)
            {
                // case where we are using a W2D symbol from the library
                _ASSERT(mdef.type() == RS_MarkerType_W2D);

                // default bounds of symbol data in W2D
                // for symbols created by MapGuide Studio
                RS_Bounds src(0, 0, SYMBOL_MAX, SYMBOL_MAX);
                SymbolTrans st = SymbolTrans(src, dst, refX, refY, angleRad);

                if (m_imsym)
                {
                    // case where we will cache the symbol image

                    // we will use unrotated symbol bounding box
                    // since rotation will be done by the image copy
                    // also we will use a slight boundary offset
                    // hardcoded to 1 pixel so that geometry exactly on the edge
                    // draws just inside the image
                    RS_Bounds dst1(1, 1, (double)(imsymw-1), (double)(imsymh-1));
                    st = SymbolTrans(src, dst1, 0.0, 0.0, 0.0);

                    m_imw2d = m_imsym;

                    // initialize the temporary symbol image to a transparent background
                    m_imsym->ren.clear(agg::argb8_packed(0x0));
                }
                else
                {
                    // case where we will draw the W2D directly into
                    // the destination image because the symbol is too big
                    m_imw2d = c();
                }

                // also set flags controlling how W2D are rewritten
                // into the destination DWF
                m_bIsSymbolW2D = true;
                m_mdOverrideColors = mdef;

                // copy symbol W2D into destination
                AddDWFContent(symbol, &st, mdef.name(), RS_String(L""), RS_String(L""));

                // make sure we zero out the W2D symbol flags
                m_bIsSymbolW2D = false;
                m_imw2d = NULL;
            }
            else
            {
                // case where we are using an SLD symbol, and fall-through for other cases

                // determine which SLD symbol we need to draw
                // and pick up its polygon point definition
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
                    // default or error
                    poly = (RS_F_Point*)SLD_ERROR;
                    npts = sizeof(SLD_ERROR) / (2 * sizeof(double));
                    found = false;
                }

                // fill color
                int fill = mdef.style().color().argb();

                // outline color
                int outline = mdef.style().outline().color().argb();

                // construct transformer
                RS_Bounds src(0.0, 0.0, 1.0, 1.0);
                SymbolTrans trans(src, dst, refX, refY, angleRad);

                // transform to coordinates of temporary image where we
                // draw symbol before transfering to the map
                LineBuffer* lb = LineBufferPool::NewLineBuffer(m_pPool, 8);
                std::auto_ptr<LineBuffer> spLB(lb);

                double tempx, tempy;
                for (int i=0; i<npts; ++i)
                {
                    tempx = poly[i].x;
                    tempy = poly[i].y;

                    // unit square to world
                    trans.TransformPoint(tempx, tempy);

                    WorldToScreenPoint(tempx, tempy, tempx, tempy);

                    if (i)
                        lb->LineTo(tempx, tempy);
                    else
                        lb->MoveTo(tempx, tempy);
                }

                if (!found)
                {
                    // unknown symbol
                    m_lineStroke.color = 0xffff0000;
                    m_lineStroke.weight = 1.0;
                    DrawScreenPolyline(c(), lb, NULL, m_lineStroke);
                }
                else
                {
                    m_lineStroke.color = outline;
                    m_lineStroke.weight = 1.0;
                    DrawScreenPolygon(c(), lb, NULL, fill);
                    DrawScreenPolyline(c(), lb, NULL, m_lineStroke);
                }

                LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
            }
        }

        if (m_imsym)
        {
            // in case we cached a symbol image, draw it to the main
            // map image

            // get the source image size
            int imsymw = m_imsym->rb.width();
            int imsymh = m_imsym->rb.height();

            double cx = dst.width()  * (0.5 - refX);
            double cy = dst.height() * (0.5 - refY);

            SE_Matrix mtx;
            mtx.rotate(angleRad);
            mtx.translate(dst.minx, dst.miny);
            mtx.transform(cx, cy);

            WorldToScreenPoint(cx, cy, cx, cy);

            // that ought to do it...
            DrawScreenRaster((unsigned char*)m_imsym->m_rows,
                             imsymw * imsymh * 4,
                             RS_ImageFormat_NATIVE,
                             imsymw,
                             imsymh,
                             cx,
                             cy,
                             imsymw,
                             -imsymh,
                             angleRad * M_180PI);
        }
    }

    if (!allowOverpost)
    {
        // now add an exclusion region to the file so that
        // labels do not overlap the symbol

        // default bounds of symbol data in W2D
        // for symbols created by MapGuide Studio
        RS_Bounds src(0.0, 0.0, SYMBOL_MAX, SYMBOL_MAX);

        // a square that we will transform into the dst bounds
        RS_F_Point box[5];
        box[0].x = 0.0;
        box[0].y = 0.0;
        box[1].x = SYMBOL_MAX;
        box[1].y = 0.0;
        box[2].x = SYMBOL_MAX;
        box[2].y = SYMBOL_MAX;
        box[3].x = 0.0;
        box[3].y = SYMBOL_MAX;

        // construct transformer - same as the
        // one used for the actual symbol drawables
        SymbolTrans boxtrans(src, dst, refX, refY, angleRad);

        RS_F_Point pts[4];

        double tempx, tempy;

        for (int i=0; i<4; ++i)
        {
            tempx = box[i].x;
            tempy = box[i].y;

            boxtrans.TransformPoint(tempx, tempy);

            WorldToScreenPoint(tempx, tempy, tempx, tempy);
            pts[i].x = tempx;
            pts[i].y = tempy;
        }

        AddExclusionRegion(pts, 4);
    }

    // set actual (unrotated) bounds with new insertion point if a pointer was passed in
    if (bounds)
    {
        bounds->minx =       -refX  * mdef.width();
        bounds->maxx = (1.0 - refX) * mdef.width();
        bounds->miny =       -refY  * mdef.height();
        bounds->maxy = (1.0 - refY) * mdef.height();
    }
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::ProcessLabelGroup(RS_LabelInfo*    labels,
                                    int              nlabels,
                                    const RS_String& text,
                                    RS_OverpostType  type,
                                    bool             exclude,
                                    LineBuffer*      path,
                                    double           scaleLimit)
{
    // check if we are rendering a selection - bail if so
    if (m_bSelectionMode)
        return;

    // forward it to the label renderer
    m_labeler->ProcessLabelGroup(labels, nlabels, text, type, exclude, path, scaleLimit);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::SetSymbolManager(RS_SymbolManager* manager)
{
    m_symbolManager = manager;
}


//////////////////////////////////////////////////////////////////////////////
RS_MapUIInfo* AGGRenderer::GetMapInfo()
{
    return m_mapInfo;
}


//////////////////////////////////////////////////////////////////////////////
RS_LayerUIInfo* AGGRenderer::GetLayerInfo()
{
    return m_layerInfo;
}


//////////////////////////////////////////////////////////////////////////////
RS_FeatureClassInfo* AGGRenderer::GetFeatureClassInfo()
{
    return m_fcInfo;
}


//////////////////////////////////////////////////////////////////////////////
double AGGRenderer::GetMapScale()
{
    return m_mapScale;
}


//////////////////////////////////////////////////////////////////////////////
double AGGRenderer::GetDrawingScale()
{
    return m_drawingScale;
}


//////////////////////////////////////////////////////////////////////////////
RS_Bounds& AGGRenderer::GetBounds()
{
    return m_extents;
}


//////////////////////////////////////////////////////////////////////////////
double AGGRenderer::GetDpi()
{
    return m_dpi;
}


//////////////////////////////////////////////////////////////////////////////
double AGGRenderer::GetMetersPerUnit()
{
    return m_metersPerUnit;
}


//////////////////////////////////////////////////////////////////////////////
bool AGGRenderer::RequiresClipping()
{
    return m_bRequiresClipping;
}


//////////////////////////////////////////////////////////////////////////////
bool AGGRenderer::RequiresLabelClipping()
{
    // always the same value as RequiresClipping
    return m_bRequiresClipping;
}


//////////////////////////////////////////////////////////////////////////////
bool AGGRenderer::SupportsZ()
{
    // Z values in feature geometry are ignored
    return false;
}


//////////////////////////////////////////////////////////////////////////////
bool AGGRenderer::SupportsTooltips()
{
    // set to false to disable processing of tooltips
    return false;
}


//////////////////////////////////////////////////////////////////////////////
bool AGGRenderer::SupportsHyperlinks()
{
    // set to false to disable processing of hyperlinks
    return false;
}


//////////////////////////////////////////////////////////////////////////////
bool AGGRenderer::UseLocalOverposting()
{
    return m_bLocalOverposting;
}


//////////////////////////////////////////////////////////////////////////////
// WARNING: caller responsible for deleting resulting line buffer
LineBuffer* AGGRenderer::ApplyLineStyle(LineBuffer* srcLB, wchar_t* lineStyle, double lineWidthPixels, double drawingScale, double dpi)
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
                        for (int i=1; i<numCapSegs; ++i)
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


//////////////////////////////////////////////////////////////////////////////
// scale an input number in meters to a mapping
// space number given a device or mapping space unit.
double AGGRenderer::_MeterToMapSize(RS_Units unit, double number)
{
    double scale_factor;

    if (unit == RS_Units_Device) // in meters, fixed size
        scale_factor = m_mapScale / m_metersPerUnit;
    else
        scale_factor = 1.0 / m_metersPerUnit;

    return number * scale_factor;
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::SetRenderSelectionMode(bool mode)
{
    SetRenderSelectionMode(mode, 0x0000FFFF);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::SetRenderSelectionMode(bool mode, int rgba)
{
    SE_Renderer::SetRenderSelectionMode(mode, rgba);

    // initialize the selection styles if needed
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


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::DrawString(const RS_String& s,
                             double           x,
                             double           y,
                             double           width,
                             double           height,
                             const RS_Font*   font,
                             RS_Color&        color,
                             double           angleRad)
{
    // do BIDI conversion
    const RS_String& sConv = m_bidiConverter.ConvertString(s);

    DrawString(c(), sConv, x, y, width, height, font, color, angleRad);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::DrawString(agg_context*     cxt,
                             const RS_String& s,
                             double           x,
                             double           y,
                             double           /*width*/,
                             double           height,
                             const RS_Font*   font,
                             RS_Color&        color,
                             double           angleRad)
{
    if (NULL == font)
        return;

    // Don't draw the text if height > 16384 pixels, since performance will be
    // poor.  16394 pixels should be more than enough (e.g. this allows 13" high
    // text on a 1200dpi device).
    if (height > 16384.0)
        return;

    // For the best performance we want to cache images of glyphs as much as
    // possible.  But when the font height gets large the memory size of the
    // cached images starts to become an issue, especially if many different
    // glyphs get cached.  So when height > 1024 switch to caching glyph outlines,
    // since memory use is then independent of height (at least for scalable fonts).
    agg::glyph_rendering glyph_type = (height > 1024.0)? agg::glyph_ren_outline : agg::glyph_ren_agg_gray8;

    bool font_changed = false;
    if (cxt->last_font != font)
    {
        // convert font path to utf8
        size_t lenf = font->m_filename.length();
        size_t lenbytesf = lenf * 4 + 1;
        char* futf8 = (char*)alloca(lenbytesf);
        DWFCore::DWFString::EncodeUTF8(font->m_filename.c_str(), lenf * sizeof(wchar_t), futf8, lenbytesf);

        // load the font
        bool res = cxt->feng.load_font(futf8, 0, glyph_type);
        cxt->feng.char_map(FT_ENCODING_UNICODE);

        if (!res)
            return;

        cxt->last_font = font;
        cxt->last_glyph_type = glyph_type;
        font_changed = true;
    }
    else if (cxt->last_glyph_type != glyph_type)
    {
        // reload the font using the new glyph caching type - since the
        // font was previously loaded this call is fast
        bool res = cxt->feng.load_font(cxt->feng.name(), 0, glyph_type);
//      cxt->feng.char_map(FT_ENCODING_UNICODE); // this should not be necessary

        if (!res)
            return;

        cxt->last_glyph_type = glyph_type;
//      font_changed = true; // this should not be necessary
    }

    if (font_changed || cxt->last_font_height != height)
    {
        cxt->feng.height(height);
        cxt->last_font_height = height;
    }

    agg::trans_affine mtx;
    mtx *= agg::trans_affine_rotation(angleRad);
//  mtx *= agg::trans_affine_skewing(-0.4, 0);
//  mtx *= agg::trans_affine_translation(1, 0);

    if (font_changed || cxt->last_font_transform != mtx)
    {
        cxt->feng.transform(mtx);
        cxt->last_font_transform = mtx;
    }

    mg_ren_aa_solid ren_aa_solid(cxt->ren);
    ren_aa_solid.color(agg::argb8_packed(color.argb()));

    unsigned int* text;
#ifdef _WIN32
    // TODO: check if we really need to convert UCS-2 to UCS-4 on Windows or
    //       if we can just use the characters directly from the wchar_t array
    lstring ltext = UnicodeString::UTF16toUTF32(s.c_str());
    text = (unsigned int*)ltext.c_str();
#else
    text = (unsigned int*)s.c_str();
#endif

    unsigned int* p = text;
    if (glyph_type == agg::glyph_ren_agg_gray8)
    {
        while (*p)
        {
            const agg::glyph_cache* glyph = cxt->fman.glyph(*p++);
            cxt->fman.add_kerning(&x, &y);
            cxt->fman.init_embedded_adaptors(glyph, x, y);

            agg::render_scanlines(cxt->fman.gray8_adaptor(), cxt->fman.gray8_scanline(), ren_aa_solid);

            x += glyph->advance_x;
            y += glyph->advance_y;
        }
    }
    else
    {
        font_curve_type curves(cxt->fman.path_adaptor());
        font_contour_type contour(curves);

        cxt->ras.filling_rule(agg::fill_non_zero);

        while (*p)
        {
            const agg::glyph_cache* glyph = cxt->fman.glyph(*p++);
            cxt->fman.add_kerning(&x, &y);
            cxt->fman.init_embedded_adaptors(glyph, x, y);

            cxt->ras.reset();
            cxt->ras.add_path(contour);

            agg::render_scanlines(cxt->ras, cxt->sl, ren_aa_solid);

            x += glyph->advance_x;
            y += glyph->advance_y;
        }

        cxt->ras.filling_rule(agg::fill_even_odd);
    }
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::MeasureString(const RS_String& s,
                                double           height,
                                const RS_Font*   font,
                                double           /*angleRad*/,
                                RS_F_Point*      res,       // assumes length equals 4 points
                                float*           offsets)   // assumes length equals length of string
{
    // If the supplied font height is too large AGG will run out of memory
    // when it tries to cache images of the glyphs.  So restrict the height
    // to 1024 for measuring, and then scale the result if necessary.
    double measureHeight = rs_min(1024.0, height);
    double measureScale = height / measureHeight;

    // load the font
    bool font_changed = false;
    if (c()->last_font != font)
    {
        // convert font path to utf8
        size_t lenf = font->m_filename.length();
        size_t lenbytesf = lenf * 4 + 1;
        char* futf8 = (char*)alloca(lenbytesf);
        DWFCore::DWFString::EncodeUTF8(font->m_filename.c_str(), lenf * sizeof(wchar_t), futf8, lenbytesf);

        bool res1 = c()->feng.load_font(futf8, 0, agg::glyph_ren_agg_gray8);
        c()->feng.char_map(FT_ENCODING_UNICODE);

        if (!res1)
            return;

        c()->last_font = font;
        c()->last_glyph_type = agg::glyph_ren_agg_gray8;
        font_changed = true;
    }

    // reset the font transform to identity, to negate any rotation
    if (font_changed || !c()->last_font_transform.is_identity())
    {
        agg::trans_affine trans;
        c()->feng.transform(trans);
        c()->last_font_transform = trans;
    }

    if (font_changed || c()->last_font_height != measureHeight)
    {
        c()->feng.height(measureHeight);
        c()->last_font_height = measureHeight;
    }

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

    unsigned int* text;
#ifdef _WIN32
    // TODO: check if we really need to convert UCS-2 to UCS-4 on Windows or
    //       if we can just use the characters directly from the wchar_t array
    lstring ltext = UnicodeString::UTF16toUTF32(pStrToUse->c_str());
    text = (unsigned int*)ltext.c_str();
#else
    text = (unsigned int*)pStrToUse->c_str();
#endif

    double left = 0;
    double right = 0;
    double bottom = 0;
    double top = 0;
    bool first = true;

    double xpos = 0.0;
    double ypos = 0.0;

    unsigned int* p = text;
    while (*p)
    {
        const agg::glyph_cache* glyph = c()->fman.glyph(*p++);

        if (first)
        {
            if (glyph->bounds.is_valid())
            {
                left = glyph->bounds.x1;
                right = glyph->bounds.x2;
                bottom = glyph->bounds.y1;
                top = glyph->bounds.y2;
            }
            first = false;
        }

        if (offsets)
            *offsets++ = (float)(measureScale*glyph->advance_x);

        c()->fman.add_kerning(&xpos, &ypos);

        xpos += glyph->advance_x;
        ypos += glyph->advance_y;

        // don't include this inside the is_valid check - spaces have
        // is_valid false, yet we still need to adjust the right edge
        // of the bounds to account for them
        if (xpos > right)
            right = xpos;

        if (glyph->bounds.is_valid())
        {
            if (top < glyph->bounds.y2)
                top = glyph->bounds.y2;
            if (bottom > glyph->bounds.y1)
                bottom = glyph->bounds.y1;
        }
    }

    left   *= measureScale;
    right  *= measureScale;
    bottom *= measureScale;
    top    *= measureScale;

    res[0].x = left;
    res[0].y = bottom;
    res[1].x = right;
    res[1].y = bottom;
    res[2].x = right;
    res[2].y = top;
    res[3].x = left;
    res[3].y = top;
}


//////////////////////////////////////////////////////////////////////////////
const RS_Font* AGGRenderer::FindFont(RS_FontDef& def)
{
    if (m_lastFontDef.name().compare(def.name()) != 0 ||
        m_lastFontDef.style() != def.style() ||
        m_lastFontDef.height() > def.height() * 1.001 ||
        m_lastFontDef.height() < def.height() * 0.999)
    {
        m_lastFontDef.name() = def.name();
        m_lastFontDef.style() = def.style();
        m_lastFontDef.height() = def.height();
        m_lastFontDef.units() = def.units();

        m_lastFont = FontManager::Instance()->FindFont(def.name().c_str(),
                          (def.style() & RS_FontStyle_Bold) != 0,
                          (def.style() & RS_FontStyle_Italic) != 0);
    }

    return m_lastFont;
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// SE_Renderer implementation
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::SetPolyClip(LineBuffer* polygon, double bufferWidth)
{
    c()->bPolyClip = false;
    if (NULL != polygon)
    {
        // sanity check - cannot clip against these types
        switch (polygon->geom_type())
        {
            case FdoGeometryType_Point:
            case FdoGeometryType_MultiPoint:
            case FdoGeometryType_LineString:
            case FdoGeometryType_MultiLineString:
            case FdoGeometryType_CurveString:
            case FdoGeometryType_MultiCurveString:
                return;
        }
        c()->bPolyClip = true;

        // compute the screen region affected by the polygon mask
        double minx, miny, maxx, maxy;
        m_xform.transform(polygon->bounds().minx, polygon->bounds().miny, minx, miny);
        m_xform.transform(polygon->bounds().maxx, polygon->bounds().maxy, maxx, maxy);

        // take into account buffer width
        if (bufferWidth > 0)
        {
            minx -= bufferWidth;
            miny -= bufferWidth;
            maxx += bufferWidth;
            maxy += bufferWidth;
        }

        int iminx = (int)rs_max(0, minx-1);
        int iminy = (int)rs_max(0, miny-1);
        int imaxx = (int)rs_min(m_width-1, maxx+1);
        int imaxy = (int)rs_min(m_height-1, maxy+1);

        // set a clip box on the renderer so that it only even looks at this area
        // this is important because we will only clear that section of the alpha mask
        // buffer and the rest may be dirty
        c()->clip_rb.clip_box(iminx, iminy, imaxx, imaxy);

        // transform to screen coords and transfer to AGG storage
        unsigned int* pathids = (unsigned int*)alloca(polygon->geom_count() * sizeof(unsigned int));
        _TransferPoints(c(), polygon, &m_xform, pathids, true);

        // clear the affected region of the alpha mask
        agg::gray8 cc(0);

        if (imaxx >= iminx)
        {        
            unsigned width = (int)imaxx - (int)iminx + 1;
            for (int y = iminy; y <= imaxy; ++y)
                c()->mask_pixf->copy_hline(iminx, y, width, cc);
        }

        // render the alpha mask polygon
        c()->mask_ren.color(agg::gray8(255));
        for (int i=0; i<polygon->geom_count(); ++i)
        {
            c()->ras.reset();
            c()->ras.add_path(c()->ps, pathids[i]);
            agg::render_scanlines(c()->ras, c()->sl, c()->mask_ren);
        }
        c()->ras.reset();

        // render a thick line to represent the buffer
        if (bufferWidth != 0)
        {
            // We will render the thick outline of the polygon with black.  This
            // will give us both an inset into the polygon and an offset out of it.
            // We will zero out the offset during the third draw right below this
            c()->mask_ren.color(agg::gray8(0));

            agg::conv_stroke<agg::path_storage> stroke(c()->ps);
            stroke.width(bufferWidth*2);
            stroke.line_cap(agg::round_cap);
            stroke.line_join(agg::round_join);

            c()->ras.filling_rule(agg::fill_non_zero);
            c()->ras.add_path(stroke);
            agg::render_scanlines(c()->ras, c()->sl, c()->mask_ren);
            c()->ras.filling_rule(agg::fill_even_odd);
            c()->ras.reset();

            // Now we need to render the polygon a third time, using an inverting blender
            // so that the region inside the polygon that was made dark by the rendering
            // of the thick outline becomes "light again" for use by the mask rasterizer.

            typedef agg::pixfmt_alpha_blend_gray<blender_gray_invert<agg::gray8> , agg::rendering_buffer> pixfmt_gray8_invert;
            typedef agg::renderer_base<pixfmt_gray8_invert>                       mg_invert_rb_type;
            typedef agg::renderer_scanline_aa_solid<mg_invert_rb_type>            mg_invert_ren_type;

            pixfmt_gray8_invert pfi(c()->mask_rbuf);
            mg_invert_rb_type reni(pfi);
            mg_invert_ren_type renaai(reni);

            // render the alpha mask polygon, again, this time with the inverting blender
            _TransferPoints(c(), polygon, &m_xform, pathids, true);
            for (int i=0; i<polygon->geom_count(); ++i)
            {
                c()->ras.reset();
                c()->ras.add_path(c()->ps, pathids[i]);
                agg::render_scanlines(c()->ras, c()->sl, renaai);
            }
            c()->ras.reset();
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
// Called when applying a line style on a feature geometry.  Line styles can
// only be applied to linestring and polygon feature geometry types.
void AGGRenderer::ProcessLine(SE_ApplyContext* ctx, SE_RenderLineStyle* style)
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


//////////////////////////////////////////////////////////////////////////////
// Called when applying an area style on a feature geometry.  Area styles can
// can only be applied to polygon feature geometry types.
void AGGRenderer::ProcessArea(SE_ApplyContext* ctx, SE_RenderAreaStyle* style)
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

    bool clip = (!style->solidFill && style->clippingControl == SE_ClippingControl_Clip);
    if (clip)
        SetPolyClip(featGeom, style->bufferWidth);

    SE_ApplyContext local_ctx = *ctx;
    local_ctx.geometry = featGeom;
    SE_Renderer::ProcessArea(&local_ctx, style);

    if (clip)
        SetPolyClip(NULL, 0.0);

    // cleanup
    if (spLB.get())
        LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::_TransferPoints(agg_context* c, LineBuffer* srcLB, const SE_Matrix* xform, unsigned int* pathids, bool isPolygon)
{
    if (s_bClampPoints)
    {
        _TransferPointsClamped(c, srcLB, xform, pathids, isPolygon);
        return;
    }

    c->ps.remove_all();

    int offset = 0;
    int* cntrs = srcLB->cntrs();

    if (pathids)
        *pathids = 0;

    for (int h=0; h<srcLB->geom_count(); ++h)
    {
        if (h && pathids)
            pathids[h] = c->ps.start_new_path();

        int cur_cntr_count = srcLB->geom_size(h);

        for (int i=0; i<cur_cntr_count; ++i)
        {
            int ci = offset;

            // add the initial point
            double sx = srcLB->x_coord(ci);
            double sy = srcLB->y_coord(ci);
            ++ci;

            if (xform)
                xform->transform(sx, sy, sx, sy);

            c->ps.move_to(sx, sy);

            int ptlast = cntrs[i] - 1;
            if (!ptlast)
                continue;

            // add the interior points
            double tx, ty;
            for (int j=1; j<ptlast; ++j)
            {
                tx = srcLB->x_coord(ci);
                ty = srcLB->y_coord(ci);
                ++ci;

                if (xform)
                    xform->transform(tx, ty, tx, ty);

                c->ps.line_to(tx, ty);
            }

            // add the final point
            tx = srcLB->x_coord(ci);
            ty = srcLB->y_coord(ci);
            ++ci;

            if (xform)
                xform->transform(tx, ty, tx, ty);

            // detect a close segment (in a Linebuffer this is caused by first and
            // last point of the contour being equal and size of contour > 2 points)
            if (tx == sx && ty == sy && ptlast > 1) 
                c->ps.close_polygon();
            else if (ptlast == 1)
            {
                // only two points in the buffer - check if this corresponds to a dot
                double dx = tx - sx;
                double dy = ty - sy;
                double len2 = dx*dx + dy*dy;

                // Although dots are replaced by lines with length LINE_SEGMENT_DOT_SIZE,
                // it's possible that the lines end up somewhat larger in the wrapping
                // case due to warping.  We therefore perform the check against a 10 times
                // larger length.  In the future it might be worth relaxing this to simply
                // check for any segment less than one pixel in length.
                if (len2 < 100.0*LINE_SEGMENT_DOT_SIZE*LINE_SEGMENT_DOT_SIZE)
                {
                    // found a dot - draw it as a 1-pixel long line
                    if (len2 > 0.0)
                    {
                        double len = sqrt(len2);
                        dx /= len;
                        dy /= len;
                        c->ps.move_to(tx-0.5*dx, ty-0.5*dy);
                        c->ps.line_to(tx+0.5*dx, ty+0.5*dy);
                    }
                    else
                    {
                        // length of line can be 0 when DOTS_HATCH is rendered from DWF
                        // so use LINE_SEGMENT_DOT_SIZE to render that dots
                        c->ps.move_to(tx-LINE_SEGMENT_DOT_SIZE, ty-LINE_SEGMENT_DOT_SIZE);
                        c->ps.line_to(tx+LINE_SEGMENT_DOT_SIZE, ty+LINE_SEGMENT_DOT_SIZE);
                    }
                }
                else
                {
                    // a normal line segment
                    c->ps.line_to(tx, ty);
                }
            }
            else
                c->ps.line_to(tx, ty);

            offset += cntrs[i];
        }

        cntrs += cur_cntr_count;
    }
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::_TransferPointsClamped(agg_context* c, LineBuffer* srcLB, const SE_Matrix* xform, unsigned int* pathids, bool isPolygon)
{
    c->ps.remove_all();

    int offset = 0;
    int* cntrs = srcLB->cntrs();

    if (pathids)
        *pathids = 0;

    for (int h=0; h<srcLB->geom_count(); ++h)
    {
        if (h && pathids)
            pathids[h] = c->ps.start_new_path();

        int cur_cntr_count = srcLB->geom_size(h);

        for (int i=0; i<cur_cntr_count; ++i)
        {
            int ci = offset;

            // add the initial point
            double sx = srcLB->x_coord(ci);
            double sy = srcLB->y_coord(ci);
            ++ci;

            if (xform)
                xform->transform(sx, sy, sx, sy);
            double sxi = ROUND(sx);
            double syi = ROUND(sy);
            if (!isPolygon)
            {
                sxi += 0.5;
                syi += 0.5;
            }

            c->ps.move_to(sxi, syi);
            double lastxi = sxi;
            double lastyi = syi;
            int numAdded = 1;

            int ptlast = cntrs[i] - 1;
            if (!ptlast)
                continue;

            // add the interior points
            double tx, ty, txi, tyi;
            for (int j=1; j<ptlast; ++j)
            {
                tx = srcLB->x_coord(ci);
                ty = srcLB->y_coord(ci);
                ++ci;

                if (xform)
                    xform->transform(tx, ty, tx, ty);
                txi = ROUND(tx);
                tyi = ROUND(ty);
                if (!isPolygon)
                {
                    txi += 0.5;
                    tyi += 0.5;
                }

                // don't add the same point repeatedly
                if (txi != lastxi || tyi != lastyi)
                {
                    c->ps.line_to(txi, tyi);
                    lastxi = txi;
                    lastyi = tyi;
                    ++numAdded;
                }
            }

            // add the final point
            tx = srcLB->x_coord(ci);
            ty = srcLB->y_coord(ci);
            ++ci;

            if (xform)
                xform->transform(tx, ty, tx, ty);
            txi = ROUND(tx);
            tyi = ROUND(ty);
            if (!isPolygon)
            {
                txi += 0.5;
                tyi += 0.5;
            }

            // detect a close segment (in a Linebuffer this is caused by first and
            // last point of the contour being equal
            if (tx == sx && ty == sy)
            {
                if (numAdded > 1)
                    c->ps.close_polygon();
                else
                {
                    // polygon size is less than 1 pixel - draw it as a dot
                    // HACK - figure out how to draw a dot in AGG with a path storage
//                  int ii = (int)txi;
//                  int jj = (int)tyi;
//                  jj = c->rb.height() - 1 - jj;
//                  c->m_rows[ii + jj * c->rb.width()] = 0xff000000;
                    c->ps.move_to(txi-0.24, tyi);
                    c->ps.line_to(txi+0.24, tyi);
                }
            }
            else if (ptlast == 1 || numAdded == 1)
            {
                // only two points in the buffer - check if this corresponds to a dot
                double dx = tx - sx;
                double dy = ty - sy;
                double len2 = dx*dx + dy*dy;

                // Although dots are replaced by lines with length LINE_SEGMENT_DOT_SIZE,
                // it's possible that the lines end up somewhat larger in the wrapping
                // case due to warping.  We therefore perform the check against a 10 times
                // larger length.  In the future it might be worth relaxing this to simply
                // check for any segment less than one pixel in length.
                if (len2 < 100.0*LINE_SEGMENT_DOT_SIZE*LINE_SEGMENT_DOT_SIZE)
                {
                    // found a dot - draw it as a 1-pixel long line
                    double len = sqrt(len2);
                    dx /= len;
                    dy /= len;
                    c->ps.move_to(tx-0.5*dx, ty-0.5*dy);
                    c->ps.line_to(tx+0.5*dx, ty+0.5*dy);
                }
                else
                {
                    // a normal line segment
                    c->ps.line_to(txi, tyi);
                }
            }
            else
                c->ps.line_to(txi, tyi);

            offset += cntrs[i];
        }

        cntrs += cur_cntr_count;
    }
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::DrawScreenPolyline(LineBuffer* srclb, const SE_Matrix* xform, const SE_LineStroke& lineStroke)
{
    DrawScreenPolyline(c(), srclb, xform, lineStroke);
}


//////////////////////////////////////////////////////////////////////////////
// copied from WritePolylines, except it doesn't do to screen transform - we should refactor
void AGGRenderer::DrawScreenPolyline(agg_context* c, LineBuffer* srclb, const SE_Matrix* xform, const SE_LineStroke& lineStroke)
{
    if ((lineStroke.color & 0xFF000000) == 0)
        return;

    // if you have no geoms, why do you call us at all?
    if (srclb->geom_count() == 0)
        return;

    double weightpx = rs_max(1.0, lineStroke.weight);

    // add to the agg path storage - here it doesn't matter
    // how many geometries there are in the line buffer,
    // so we just pass NULL for the path offsets array
    _TransferPoints(c, srclb, xform, NULL, false);

    // stroke the line as a polygon
    agg::conv_stroke<agg::path_storage> stroke(c->ps);
    stroke.width(weightpx);

    // set up cap / join style
    switch (lineStroke.cap)
    {
        case SE_LineCap_None:
            stroke.line_cap(agg::butt_cap);
            break;
        case SE_LineCap_Triangle:
            stroke.line_cap(agg::triangle_cap);
            break;
        case SE_LineCap_Square:
            stroke.line_cap(agg::square_cap);
            break;
        case SE_LineCap_Round:
        default:
            stroke.line_cap(agg::round_cap);
            break;
    }

    switch (lineStroke.join)
    {
        case SE_LineJoin_None:
            stroke.line_join(agg::bevel_join);  // AGG doesn't support None
            break;
        case SE_LineJoin_Bevel:
            stroke.line_join(agg::bevel_join);
            break;
        case SE_LineJoin_Miter:
            stroke.line_join(agg::miter_join);
            stroke.miter_limit(2.0*lineStroke.miterLimit);  // miter limit is relative to the half-width
            break;
        case SE_LineJoin_Round:
        default:
            stroke.line_join(agg::round_join);
            break;
    }

    c->ras.add_path(stroke);
    c->ras.filling_rule(agg::fill_non_zero);

    if (c->bPolyClip)
    {
        c->clip_ren.color(agg::rgb8_packed(lineStroke.color));
        agg::render_scanlines(c->ras, c->sl, c->clip_ren);
    }
    else
    {
        agg::render_scanlines_aa_solid(c->ras, c->sl, c->ren, agg::argb8_packed(lineStroke.color));
    }

    c->ras.filling_rule(agg::fill_even_odd);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::DrawScreenPolygon(LineBuffer* polygon, const SE_Matrix* xform, unsigned int color)
{
    DrawScreenPolygon(c(), polygon, xform, color);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::DrawScreenPolygon(agg_context* c, LineBuffer* polygon, const SE_Matrix* xform, unsigned int color)
{
    if ((color & 0xFF000000) == 0)
        return;

    // if you have no geoms, why do you call us at all?
    if (polygon->geom_count() == 0)
        return;

    // transform to screen coords and transfer to AGG storage
    unsigned int* pathids = (unsigned int*)alloca(polygon->geom_count() * sizeof(unsigned int));
    _TransferPoints(c, polygon, xform, pathids, true);

    for (int i=0; i<polygon->geom_count(); ++i)
    {
        c->ras.reset();
        c->ras.add_path(c->ps, pathids[i]);
        if (c->bPolyClip)
        {
            c->clip_ren.color(agg::argb8_packed(color));
            agg::render_scanlines(c->ras, c->sl, c->clip_ren);
        }
        else
            agg::render_scanlines_aa_solid(c->ras, c->sl, c->ren, agg::argb8_packed(color));
    }
}


//////////////////////////////////////////////////////////////////////////////
bool AGGRenderer::YPointsUp()
{
    return true;
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::GetWorldToScreenTransform(SE_Matrix& xform)
{
    xform = m_xform;
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::SetWorldToScreenTransform(SE_Matrix& xform)
{
    m_xform = xform;
    m_xform.inverse(m_ixform);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::WorldToScreenPoint(double& inx, double& iny, double& ox, double& oy)
{
    m_xform.transform(inx, iny, ox, oy);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::ScreenToWorldPoint(double& inx, double& iny, double& ox, double& oy)
{
    m_ixform.transform(inx, iny, ox, oy);
}


//////////////////////////////////////////////////////////////////////////////
// returns number of pixels per millimeter device
double AGGRenderer::GetScreenUnitsPerMillimeterDevice()
{
    return m_dpi / MILLIMETERS_PER_INCH;
}


//////////////////////////////////////////////////////////////////////////////
// returns number of pixels per millimeter world
double AGGRenderer::GetScreenUnitsPerMillimeterWorld()
{
    return m_dpi / MILLIMETERS_PER_INCH / m_mapScale;
}


//////////////////////////////////////////////////////////////////////////////
// screen units are pixels
double AGGRenderer::GetScreenUnitsPerPixel()
{
    return 1.0;
}


//////////////////////////////////////////////////////////////////////////////
RS_FontEngine* AGGRenderer::GetRSFontEngine()
{
    return this;
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::AddExclusionRegion(RS_F_Point* fpts, int npts)
{
    m_labeler->AddExclusionRegion(fpts, npts);
}


//////////////////////////////////////////////////////////////////////////////
// labeling - this is the entry API for adding SE labels
// to the label mananger
void AGGRenderer::ProcessSELabelGroup(SE_LabelInfo*   labels,
                                      int             nlabels,
                                      RS_OverpostType type,
                                      bool            exclude,
                                      LineBuffer*     path)
{
    // check if we are rendering a selection - bail if so
    if (m_bSelectionMode)
        return;

    // forward it to the label renderer
    m_labeler->ProcessLabelGroup(labels, nlabels, type, exclude, path);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::DrawScreenRaster(unsigned char* data, int length,
                                   RS_ImageFormat format, int native_width, int native_height,
                                   double x, double y, double w, double h, double angledeg)
{
    DrawScreenRaster(c(), data, length, format, native_width, native_height, x, y, w, h, angledeg);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::DrawScreenRaster(agg_context* cxt, unsigned char* data, int length,
                                   RS_ImageFormat format, int native_width, int native_height,
                                   double x, double y, double w, double h, double angledeg)
{
    if (format == RS_ImageFormat_Unknown)
        return;

    // if it's PNG, decode it and come back around
    if (format == RS_ImageFormat_PNG)
    {
        unsigned int* decoded = AGGImageIO::DecodePNG(data, length, native_width, native_height);
        if (decoded)
            DrawScreenRaster(cxt, (unsigned char*)decoded, native_width * native_height * 4, RS_ImageFormat_ARGB, native_width, native_height, x, y, w, h, angledeg);

        delete [] decoded;
        return;
    }

    if (native_width < 0 || native_height < 0)
        return;

    // set up the image insertion transformation
    agg::trans_affine img_mtx;
    img_mtx.reset();

    img_mtx *= agg::trans_affine_translation(-native_width/2, -native_height/2);
    img_mtx *= agg::trans_affine_scaling(w / native_width, h / native_height);
    img_mtx *= agg::trans_affine_rotation(angledeg * M_PI180);
    img_mtx *= agg::trans_affine_translation(x, y);
    img_mtx.invert();

    double w2 = w * 0.5;
    double h2 = h * 0.5;
    double minx = x - w2;
    double miny = y - h2;
    double maxx = x + w2;
    double maxy = y + h2;

    // create a rectangular path that will be filled using the image
    if (angledeg == 0.0)
    {
        cxt->ras.reset();
        cxt->ras.move_to_d(minx, miny);
        cxt->ras.line_to_d(maxx, miny);
        cxt->ras.line_to_d(maxx, maxy);
        cxt->ras.line_to_d(minx, maxy);
        cxt->ras.close_polygon();
    }
    else
    {
        // create a rotated rectangular path in case there is rotation
        // we give it a 1 pixel safety buffer (not sure if it is really needed)
        SE_Matrix mtx;
        double angleRad = angledeg * M_PI180;
        mtx.translate(-x, -y);
        mtx.rotate(angleRad);
        mtx.translate(x, y);

        double px = minx - 1.0;
        double py = miny - 1.0;
        mtx.transform(px, py, px, py);

        cxt->ras.reset();
        cxt->ras.move_to_d(px, py);

        px = maxx + 1.0;
        py = miny - 1.0;
        mtx.transform(px, py, px, py);
        cxt->ras.line_to_d(px, py);

        px = maxx + 1.0;
        py = maxy + 1.0;
        mtx.transform(px, py, px, py);
        cxt->ras.line_to_d(px, py);

        px = minx - 1.0;
        py = maxy + 1.0;
        mtx.transform(px, py, px, py);
        cxt->ras.line_to_d(px, py);

        cxt->ras.close_polygon();
    }

    // attach an agg buffer to the source image data
    mg_rendering_buffer src(data, native_width, native_height, 4 * native_width);

    // render the data
    RenderWithTransform(src, cxt, img_mtx, format);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::DrawScreenRaster(unsigned char* data, int length,
                                   RS_ImageFormat format, int native_width, int native_height,
                                   double x, double y, double w, double h, double angledeg,
                                   double alpha)
{
    RenderUtil::DrawScreenRaster(this, data, length, format, native_width, native_height,
                                 x, y, w, h, angledeg, alpha);
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::DrawScreenRasterTransform(agg_context* cxt, unsigned char* data, int length,
                                            RS_ImageFormat format, int native_width, int native_height,
                                            double x, double y, double w, double h,
                                            TransformMesh* xformMesh)
{
    // if it's PNG, decode it and come back around
    if (format == RS_ImageFormat_PNG)
    {
        unsigned int* decoded = AGGImageIO::DecodePNG(data, length, native_width, native_height);
        if (decoded)
            DrawScreenRasterTransform(cxt, (unsigned char*)decoded, native_width * native_height * 4, RS_ImageFormat_ARGB, native_width, native_height, x, y, w, h, xformMesh);

        delete [] decoded;
        return;
    }

#ifdef _DEBUG
    long dwStart = GetTickCount();
#endif

    // attach an agg buffer to the source image data
    mg_rendering_buffer src(data, native_width, native_height, 4*native_width);

    // read the xformMesh
    int num_pts = xformMesh->GetTotalPoints();
    int num_verts_per_column = xformMesh->GetTotalVerticalPoints();
    int horz_count = 1;

    for (int i=0; i<num_pts-num_verts_per_column-1; ++i)
    {
        // skip the top row of points
        if (i == horz_count * num_verts_per_column - 1)
        {
            ++i;
            ++horz_count;
        }

        // ul------ur
        // |        |
        // |        |
        // |        |
        // ll------lr

        int lowerLeftIndex  = i;
        int lowerRightIndex = i + num_verts_per_column;
        int upperRightIndex = lowerRightIndex + 1;
        int upperLeftIndex  = lowerLeftIndex  + 1;

        RenderTransformMeshRectangle(src, cxt, format, xformMesh,
            lowerLeftIndex, lowerRightIndex, upperLeftIndex, upperRightIndex);
    }

#ifdef _DEBUG
    printf("  AGGRenderer::DrawScreenRasterTransform() total time = %6.4f (s)\n", (GetTickCount()-dwStart)/1000.0);
#endif
}


//////////////////////////////////////////////////////////////////////////////
// Renders the mesh rectangle defined by the point mappings at the four supplied mesh indices
void AGGRenderer::RenderTransformMeshRectangle(mg_rendering_buffer& src, agg_context* cxt,
                                               RS_ImageFormat format, TransformMesh* transformMesh,
                                               int lowerLeftIndex, int lowerRightIndex,
                                               int upperLeftIndex, int upperRightIndex)
{
    MeshPoint mesh_pt_ll = transformMesh->GetMeshPoint(lowerLeftIndex);
    MeshPoint mesh_pt_lr = transformMesh->GetMeshPoint(lowerRightIndex);
    MeshPoint mesh_pt_ul = transformMesh->GetMeshPoint(upperLeftIndex);
    MeshPoint mesh_pt_ur = transformMesh->GetMeshPoint(upperRightIndex);

    // render the lower triangle
    RenderTransformedTriangle(src, cxt, format,
        mesh_pt_ll.pt_src, mesh_pt_lr.pt_src, mesh_pt_ur.pt_src,
        mesh_pt_ll.pt_dest, mesh_pt_lr.pt_dest, mesh_pt_ur.pt_dest);

    // render the upper triangle
    RenderTransformedTriangle(src, cxt, format,
        mesh_pt_ur.pt_src, mesh_pt_ul.pt_src, mesh_pt_ll.pt_src,
        mesh_pt_ur.pt_dest, mesh_pt_ul.pt_dest, mesh_pt_ll.pt_dest);
}


//////////////////////////////////////////////////////////////////////////////
// Renders the triangle in the source image defined by the three source points into
// the triangle in the supplied context defined by the three destination points.
void AGGRenderer::RenderTransformedTriangle(mg_rendering_buffer& src, agg_context* cxt, RS_ImageFormat format,
                                            RS_F_Point srcPt1, RS_F_Point /*srcPt2*/, RS_F_Point srcPt3,
                                            RS_F_Point destPt1, RS_F_Point destPt2, RS_F_Point destPt3)
{
    // set up the destination parallelogram
    double parallelogram[6] = { destPt1.x, destPt1.y, destPt2.x, destPt2.y, destPt3.x, destPt3.y };

    // set up the destination triangle polygon
    cxt->ras.reset();
    cxt->ras.move_to_d(destPt1.x, destPt1.y);
    cxt->ras.line_to_d(destPt2.x, destPt2.y);
    cxt->ras.line_to_d(destPt3.x, destPt3.y);
    cxt->ras.close_polygon();

    // set up the transformation from the source to destination
    agg::trans_affine img_mtx;
    img_mtx.reset();
    img_mtx.rect_to_parl(srcPt1.x, srcPt1.y, srcPt3.x, srcPt3.y, parallelogram);
    img_mtx.invert(); // renderer uses inverse of matrix

    // render the triangle without anti-aliasing
    // anti-aliasing results in faint grid lines
    RenderWithTransform(src, cxt, img_mtx, format, false);
}


//////////////////////////////////////////////////////////////////////////////
// Renders the source image to the destination context, using the specified
// affine transformation matrix.
void AGGRenderer::RenderWithTransform(mg_rendering_buffer& src, agg_context* cxt,
                                      agg::trans_affine& img_mtx, RS_ImageFormat format, bool antiAlias)
{
    if (format == RS_ImageFormat_ABGR)
    {
        mg_pixfmt_type_abgr pf(src);
        pf.premultiply();

        typedef agg::span_interpolator_linear<> interpolator_type;
        interpolator_type interpolator(img_mtx);

        typedef agg::image_accessor_clip<mg_pixfmt_type_abgr> img_source_type;
        img_source_type img_src(pf, agg::argb8_packed(0));

        typedef agg::span_image_filter_rgba_bilinear_clip<mg_pixfmt_type_abgr, interpolator_type> span_gen_type;
        span_gen_type sg(pf, agg::argb8_packed(0), interpolator);

        agg::span_allocator<mg_pixfmt_type::color_type> sa;

        if (antiAlias)
            agg::render_scanlines_aa(cxt->ras, cxt->sl, cxt->ren_pre, sa, sg);
        else
            agg::render_scanlines_bin(cxt->ras, cxt->sl, cxt->ren_pre, sa, sg);
    }
    else if (format == RS_ImageFormat_ARGB)
    {
        mg_pixfmt_type_argb pf(src);
        pf.premultiply(); // we need to premultiply the alpha - the agg sampler will not work correctly around image edges otherwise

        typedef agg::span_interpolator_linear<> interpolator_type;
        interpolator_type interpolator(img_mtx);

        typedef agg::image_accessor_clip<mg_pixfmt_type_argb> img_source_type;
        img_source_type img_src(pf, agg::argb8_packed(0));

        typedef agg::span_image_filter_rgba_bilinear_clip<mg_pixfmt_type_argb, interpolator_type> span_gen_type;
        span_gen_type sg(pf, agg::argb8_packed(0), interpolator);

        agg::span_allocator<mg_pixfmt_type::color_type> sa;

        // we are using the alpha premultiplied renderer since the source image is premultiplied
        if (antiAlias)
            agg::render_scanlines_aa(cxt->ras, cxt->sl, cxt->ren_pre, sa, sg);
        else
            agg::render_scanlines_bin(cxt->ras, cxt->sl, cxt->ren_pre, sa, sg);
    }
    else if (format == RS_ImageFormat_NATIVE)
    {
        // source image is already premultiplied, declare a pixel format that uses
        // the correct blender
        mg_pixfmt_type_pre pf(src);

        typedef agg::span_interpolator_linear<> interpolator_type;
        interpolator_type interpolator(img_mtx);

        typedef agg::image_accessor_clip<mg_pixfmt_type_pre> img_source_type;
        img_source_type img_src(pf, agg::argb8_packed(0));

        typedef agg::span_image_filter_rgba_bilinear_clip<mg_pixfmt_type_pre, interpolator_type> span_gen_type;
        span_gen_type sg(pf, agg::argb8_packed(0), interpolator);

        agg::span_allocator<mg_pixfmt_type::color_type> sa;

        if (antiAlias)
            agg::render_scanlines_aa(cxt->ras, cxt->sl, cxt->ren_pre, sa, sg);
        else
            agg::render_scanlines_bin(cxt->ras, cxt->sl, cxt->ren_pre, sa, sg);
    }
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::DrawScreenText(const RS_TextMetrics& tm, RS_TextDef& tdef, double insx, double insy,
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


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
////
////             DWF Rewrite and related code
////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Inserts the contents of a given DWF input stream into the current
// output W2D.  The given coord sys transformation is applied and geometry
// will be clipped to the RS_Bounds context of the DWFRenderer.
void AGGRenderer::AddDWFContent(RS_InputStream*  in,
                                CSysTransformer* xformer,
                                const RS_String& section,
                                const RS_String& passwd,
                                const RS_String& w2dfilter)
{
    try
    {
        if (in->available() == 0)
            return;

        // go to beginning of stream
        in->seek(SEEK_SET, 0);

        DWFRSInputStream rsin(in);

        DWFPackageReader oReader(rsin, passwd.c_str());

        DWFPackageReader::tPackageInfo tInfo;
        oReader.getPackageInfo(tInfo);

        if (tInfo.eType != DWFPackageReader::eDWFPackage)
            return; // throw exception?

        bool checkSection = (wcslen(section.c_str()) > 0);

        // read the manifest
        DWFManifest& rManifest = oReader.getManifest();

        // now read the sections
        DWFSection* pSection = NULL;
        DWFManifest::SectionIterator* iSection = (&rManifest)->getSections();

        if (iSection)
        {
            for (; iSection->valid(); iSection->next())
            {
                pSection = iSection->get();

                // call this so that the resource data (like transforms and roles) is read in
                pSection->readDescriptor();

//              DWFGlobalSection* pGlobal = dynamic_cast<DWFGlobalSection*>(pSection);
                DWFEPlotSection* pEPlot = dynamic_cast<DWFEPlotSection*>(pSection);

                if (pEPlot)
                {
                    if (checkSection)
                    {
                        // compare name stored in section to user requested section
                        // Used for point symbols
                        DWFString name = pEPlot->title();

                        // skip current section if its name does not match the name
                        // of the one we need
                        if (name != section.c_str())
                            continue;
                    }

                    // get the resources for the section

                    // first do primary W2D resources
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

                    // then do overlays
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


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::AddW2DContent(RS_InputStream* in, CSysTransformer* xformer, const RS_String& w2dfilter)
{
    WT_Result result;

    // initialize state variables, which are needed by W2D rewriter callbacks
    m_input = in;
    m_xformer = xformer;
    m_bHaveViewport = false;
    m_bLayerPassesFilter = true;
    m_layerFilter = w2dfilter;

    // set output image if not already set
    if (!m_bIsSymbolW2D)
        m_imw2d = c();

    WT_File fin;
    fin.set_file_mode(WT_File::/*WT_File_mode::*/File_Read);

    fin.set_stream_user_data(this);

    SetActions(fin);

    fin.open();

    // do the actual reading
    do
    {
        result = fin.process_next_object();
    }
    while (result == WT_Result::Success);

    fin.close();

    m_input = NULL;

    // clear the output image if we set it in this function
    if (!m_bIsSymbolW2D)
        m_imw2d = NULL;
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::SetActions(WT_File& file)
{
    file.set_stream_open_action (agr_open);
    file.set_stream_close_action(agr_close);
    file.set_stream_read_action (agr_read);
    file.set_stream_seek_action (agr_seek);

//  file.set_dwf_header_action(agr_process_dwf_header);
//  file.set_author_action(agr_process_author);
//  file.set_creator_action(agr_process_creator);
//  file.set_creation_time_action(agr_process_created);
//  file.set_modification_time_action(agr_process_modified);
//  file.set_source_filename_action(agr_process_sourceFilename);
//  file.set_source_creation_time_action(agr_process_sourceCreationTime);
//  file.set_source_modification_time_action(agr_process_sourceModificationTime);
    file.set_units_action(agr_process_units);
//  file.set_embed_action(agr_process_embed);
//  file.set_named_view_action(agr_process_namedView);
//  file.set_view_action(agr_process_view);
//  file.set_plot_info_action(agr_process_plotInfo);
    file.set_background_action(agr_process_background);
    file.set_color_action(agr_process_color);
    file.set_line_weight_action(agr_process_lineWeight);
    file.set_line_style_action(agr_process_lineStyle);
    file.set_layer_action(agr_process_layer);
    file.set_viewport_action(agr_process_viewport);
    file.set_visibility_action(agr_process_visibility);
    file.set_code_page_action(agr_process_codePage);
    file.set_color_map_action(agr_process_colorMap);
//  file.set_comments_action(agr_process_comments);
    file.set_contour_set_action(agr_process_contourSet);
//  file.set_copyright_action(agr_process_copyright);
    file.set_dash_pattern_action(agr_process_dashPattern);
//  file.set_description_action(agr_process_description);
    file.set_fill_action(agr_process_fill);
    file.set_filled_ellipse_action(agr_process_filledEllipse);
    file.set_font_action(agr_process_font);
    file.set_image_action(agr_process_image);
//  file.set_keywords_action(agr_process_keywords);
    file.set_marker_size_action(agr_process_markerSize);
    file.set_marker_symbol_action(agr_process_markerSymbol);
    file.set_merge_control_action(agr_process_mergeControl);
//  file.set_named_view_list_action(agr_process_namedViewList);
    file.set_origin_action(agr_process_origin);
    file.set_outline_ellipse_action(agr_process_outlineEllipse);
    file.set_polygon_action(agr_process_polygon);
    file.set_polytriangle_action(agr_process_polytriangle);
    file.set_polymarker_action(agr_process_polymarker);
    file.set_projection_action(agr_process_projection);
//  file.set_subject_action(agr_process_subject);
//  file.set_title_action(agr_process_title);
//  file.set_unknown_action(agr_process_unknown);
//  file.set_url_action(agr_process_url);
    file.set_png_group4_image_action(agr_process_pngGroup4Image);
    file.set_polyline_action(agr_process_polyline);
    file.set_text_action(agr_process_text);
    file.set_inked_area_action(agr_process_inkedArea);
//  file.set_plot_optimized_action(agr_process_plotOptimized);
//  file.set_group_begin_action(agr_process_groupBegin);
//  file.set_group_end_action(agr_process_groupEnd);
//  file.set_embedded_font_action(agr_process_embeddedFont);
//  file.set_trusted_font_list_action(agr_process_trustedFontList);
//  file.set_blockref_action(agr_process_blockref);
//  file.set_block_meaning_action(agr_process_blockMeaning);
//  file.set_encryption_action(agr_process_encryption);
//  file.set_orientation_action(agr_process_orientation);
    file.set_alignment_action(agr_process_alignment);
//  file.set_password_action(agr_process_password);
//  file.set_guid_action(agr_process_guid);
//  file.set_filetime_action(agr_process_fileTime);
//  file.set_userdata_action(agr_process_userData);
    file.set_pen_pattern_action(agr_process_penPattern);
    file.set_line_pattern_action(agr_process_linePattern);
    file.set_fill_pattern_action(agr_process_fillPattern);
//  file.set_signdata_action(agr_process_DigitalSign);

    file.set_gouraud_polytriangle_action(agr_process_gouraudPolytriangle);
    file.set_gouraud_polyline_action(agr_process_gouraudPolyline);
}


//////////////////////////////////////////////////////////////////////////////
// Given an array of points in W2D logical coordinates, this function:
// 1. Transforms W2D logical coords into their model space using the
//    W2D file's units structure
// 2. Applies coordinate system transformation into the destination
//    map's space
// 3. Optionally clips the resulting points to the user-requested
//    data extent of the destination map
// 4. Transforms clipped points to destination W2D space
// 5. Returns a pointer to an array of W2D points with the
//    total number of output points returned in outNumpts.
//    If the clipping process of a polyline or polygon resulted
//    in a multipolygon or multipolyline, a pointer to a vector
//    containing contour counts will also be returned.
//    *** Both pointers are valid unit the next call to this function. ***
LineBuffer* AGGRenderer::ProcessW2DPoints(WT_File&          file,
                                          WT_Logical_Point* srcpts,
                                          int               numpts,
                                          bool              checkInBounds)
{
    // This transformer may have been modified if a Viewport
    // opcode was encountered in the source W2D. This is needed for
    // support for symbols from ACAD, which do not use the
    // standard W2D extent used by Studio.
    // The function UpdateSymbolTrans does the actual modification and is
    // called by the process_viewport callback.
    SymbolTrans* trans = (SymbolTrans*)m_xformer;

    // Our W2DRewrite component stores the source file Units transform
    // in the source file's desired rendition - a bit of a hack
    // but we cannot store it in the destination file - it messes up some scales
    // plus it has a totally different units of its own.
    WT_Matrix xform = file.desired_rendition().drawing_info().units().dwf_to_application_adjoint_transform();

    LineBuffer* lb = LineBufferPool::NewLineBuffer(m_pPool, numpts);
    std::auto_ptr<LineBuffer> spLB(lb);
    lb->Reset();

    //
    // process the points into mapping space
    //

    WT_Point3D psrc(0.0, 0.0, 0.0);
    WT_Point3D pdst(0.0, 0.0, 0.0);

    for (int i=0; i<numpts; ++i)
    {
        psrc.m_x = srcpts[i].m_x;
        psrc.m_y = srcpts[i].m_y;

        // this puts it into model space of the source data
        xform.transform(psrc, pdst);

        // now transform to mapping space of map
        // using the symbol transformer object (if any)
        if (trans)
            trans->TransformPoint(pdst.m_x, pdst.m_y);

        if (i == 0)
            lb->MoveTo(pdst.m_x, pdst.m_y);
        else
            lb->LineTo(pdst.m_x, pdst.m_y);
    }

    // IMPORTANT: Only do this if the data is a DWF layer
    // or a DWF symbol that is too large to draw using a
    // cached bitmap.  Regular DWF Symbols will be transformed
    // to the correct mapping space location by the symbol code
    if (!IsSymbolW2D())
    {
        //
        // Clipping. For GD rendering a simple bounds check is enough,
        // the renderer itself will take care of the rest
        //
        if (checkInBounds)
        {
            // check if line buffer is completely outside box
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
    // cached bitmap.  Regular DWF Symbols will be transformed
    // to the correct mapping space location by the symbol code
    if (!IsSymbolW2D() || m_imw2d != m_imsym)
    {
        int count = lb->point_count();

        for (int i=0; i<count; ++i)
        {
            WorldToScreenPoint(lb->x_coord(i), lb->y_coord(i), lb->x_coord(i), lb->y_coord(i));
        }
    }
    else
    {
/*
        // for symbols just copy the points to the output array
        // and only invert the y coordinate - we need to flip y since
        // in gd y goes down and in DWF it goes up
        int count = lb->point_count();

        EnsureBufferSize(count);
        RS_D_Point* wpts = m_wtPointBuffer;

        for (int i=0; i<count; ++i)
        {
            wpts[i].x = (int)lb->x_coord(i);
            wpts[i].y = gdImageSY((gdImagePtr)GetW2DTargetImage()) - (int)lb->y_coord(i);
        }
*/
    }

    return spLB.release();
}


//////////////////////////////////////////////////////////////////////////////
// This function scales a W2D space related value from source W2D space
// to destination.  Since the source W2D file can fit into a small piece
// of the destination DWF or be much larger, we need to take that scaling
// into account when carrying over things like line weight, font height,
// etc.  This helper function determines and applies that scale.
double AGGRenderer::ScaleW2DNumber(WT_File& file, WT_Integer32 number)
{
    WT_Matrix xform = file.desired_rendition().drawing_info().units().dwf_to_application_adjoint_transform();

    double scale1 = 1.0 / xform(0, 0); // div because we need inverse

    // number is now in source W2D model units
    double dModelSpace = (double)number * scale1;

    double dMapSpace = dModelSpace;
    if (m_xformer)
    {
        // now we need to convert that to a length in destination map units
        dMapSpace *= m_xformer->GetLinearScale();
    }

    double dDstSpace = dMapSpace;

    // only scale by map scale if we are not a symbol inside a cached image
    if (!m_bIsSymbolW2D)
        dDstSpace *= m_xform.x0;

    return dDstSpace;
}


//////////////////////////////////////////////////////////////////////////////
void AGGRenderer::UpdateSymbolTrans(WT_File& /*file*/, WT_Viewport& viewport)
{
    _ASSERT(m_xformer);

    RS_Bounds alternate_extent(0.0, 0.0, -1.0, -1.0);

    // If a viewport was defined, the symbol W2D likely came from AutoCAD.
    // In that case, the extent of the data inside the W2D is not the same
    // as what Studio saves (0, 0, SYMBOL_MAX, SYMBOL_MAX), so we need to use
    // a different transformation for that data.
    // IMPORTANT: This will destructively modify the transformer that was passed in.
    // It is ugly, but avoids parsing the W2D twice.
    // The assumption here is that Viewport opcodes define the bounds of the data.
    // This is not necessarily true for all W2D files, but is true for ones coming
    // from ACAD through the Dwg Extract and Transform engine.
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

            // TODO: we don't do that yet, since DWF layers can have
            // multiple viewports, which mess up this logic - no single
            // viewport contains the overall extent of the data
/*
            // if it is a DWF layer, also correct for aspect ratio
            // of the destination bounds of the transformer so that
            // we avoid stretching of the layer data.
            // we now assume we know the exact bounds of the data
            // in the w2d
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

            // and finally set the source bounds we got from the viewport opcode
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
