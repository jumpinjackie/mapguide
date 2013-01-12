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
#include "DWFRenderer.h"

#include "dwf/whiptk/whip_toolkit.h"

#include "dwfcore/File.h"
#include "dwfcore/String.h"
#include "dwfcore/Exception.h"
#include "dwfcore/MIME.h"
#include "dwfcore/StreamFileDescriptor.h"
#include "dwfcore/InputStream.h"
#include "dwfcore/FileInputStream.h"
#include "dwfcore/BufferOutputStream.h"
#include "dwfcore/BufferInputStream.h"
#include "dwfcore/SkipList.h"

#include "dwf/Version.h"
#include "dwf/package/Constants.h"
#include "dwf/package/Manifest.h"
#include "dwf/package/EPlotSection.h"
#include "dwf/package/GlobalSection.h"
#include "dwf/package/DefinedObject.h"
#include "dwf/package/writer/PackageWriter.h"
#include "dwf/package/reader/PackageReader.h"

#include "LineStyleDef.h"
#include "DWFRSInputStream.h"
#include "RSDWFInputStream.h"
#include "CSysTransformer.h"
#include "SymbolTrans.h"

#include "whip_hatch_library.h"
#include "whip_fill_library.h"

#include "ObservationMesh.h"

#include "W2DRewriter.h"
#include "UnicodeString.h"
#include "SLDSymbols.h"
#include "RS_Font.h"

#include "RS_FeatureReader.h"

#include "FontManager.h"

//GD headers
#include "gd.h"

//AGG headers
#include "agg_context.h"
#include "AGGRenderer.h"
#include "AGGImageIO.h"

#include "RenderUtil.h"

using namespace DWFToolkit;
using namespace DWFCore;
using namespace std;

#define ROUND(x) (int)(floor(x+0.5))

// maximum allowed size for images
#define IMAGE_SIZE_MAX 2048.0*2048.0

//table we use for keeping track of object nodes
typedef DWFWCharKeySkipList<unsigned int> NodeTable;


//-----------------------------------------------------------------------------
//------------------------------Utilities--------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


WT_Color Util_ConvertColor(RS_Color& color)
{
    return WT_Color(color.red(), color.green(), color.blue(), color.alpha());
}


WT_Color Util_ConvertColor(unsigned int argb)
{
    return WT_Color((argb >> 16) & 0xFF,
                    (argb >>  8) & 0xFF,
                    (argb      ) & 0xFF,
                    (argb >> 24) & 0xFF);
}


int DwfRendererConvertUTF32toUTF16(const unsigned long* src, unsigned short* dst, size_t dstLen)
{
    size_t dstCount = 0;

    while (*src)
    {
        unsigned long ch = *src++;

        //check for legality
        if (   (ch >= 0xD800 && ch <= 0xDFFF) //utf16 surrogates are illegal
            || (ch > 0x0010FFFF)) //bigger than max utf32 value
        {
            if (dstCount >= dstLen)
                return -1;

            //if value is illegal use a replacement character
            dst[dstCount++] = 0xFFFD; //UNI_REPLACEMENT_CHAR;
        }

        if (ch <= 0xFFFF)
        {
            if (dstCount >= dstLen)
                return -1;

            dst[dstCount++] = (unsigned short)ch; /* normal case */
        }
        else
        {
            if (dstCount+1 >= dstLen)
                return -1;

            ch -= 0x0010000UL;
            dst[dstCount++] = (unsigned short)((ch >> 10) + 0xD800);
            dst[dstCount++] = (unsigned short)((ch & 0x3FFUL) + 0xDC00);
        }
    }

    if (dstCount >= dstLen)
        return -1;

    dst[dstCount++] = (unsigned short)0;

    return (int)dstCount;
}


//converts a wide character array to a whip string
//WHIP strings use unsigned short internally
//so on Linux we have to convert to UTF16 representation
WT_String Util_ConvertString(const wchar_t* wstr)
{
#ifdef _WIN32
    unsigned short* utf16Bytes = (unsigned short*) wstr;
#else

    size_t len = wcslen(wstr) * 2 + 1; //upper bound for converted string length

    _ASSERT(len < 65536); //sanity check since we alloc on the stack

    unsigned short* utf16Bytes = (unsigned short*)alloca(len * sizeof(unsigned short));
    DwfRendererConvertUTF32toUTF16((const unsigned long*)wstr, utf16Bytes, len);

#endif

    return WT_String((WT_Unsigned_Integer16 const *)utf16Bytes);
}


//-----------------------------------------------------------------------------
//--------------------------W2D I/O overloads----------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


WT_Result my_open(WT_File & file)
{
    //this is freed in EndMap() (loops over all layer streams)
    DWFBufferOutputStream* bos = DWFCORE_ALLOC_OBJECT(DWFBufferOutputStream(100000));
    file.set_stream_user_data(bos);
    return WT_Result::Success;
}


WT_Result my_close(WT_File & file)
{
    file.set_stream_user_data(WD_Null);
    return WT_Result::Success;
}


WT_Result my_read(WT_File & /*file*/, int /*desired_bytes*/, int & /*bytes_read*/, void * /*buffer*/)
{
    //should never get called, overloaded for consistency
    _ASSERT(false);
    return WT_Result::Success;
}


WT_Result my_write(WT_File & file, int size, void const * buffer)
{
    DWFBufferOutputStream* fp = (DWFBufferOutputStream*) file.stream_user_data();
    fp->write(buffer, size);
    return WT_Result::Success;
}


WT_Result my_seek(WT_File & /*file*/, int /*distance*/, int & /*amount_seeked*/)
{
    //should never get called, overloaded for consistency
    _ASSERT(false);
    return WT_Result::Unknown_File_Read_Error;
}


//-----------------------------------------------------------------------------
//--------------------------DWFRenderer----------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


DWFRenderer::DWFRenderer()
: m_mapInfo(NULL),
  m_layerInfo(NULL),
  m_featureClass(NULL),
  m_attributes(NULL),
  m_featureClassInfo(NULL),
  m_extents(0.0, 0.0, 0.0, 0.0),
  m_mapExtents(0.0, 0.0, 0.0, 0.0),
  m_wtPointBuffer(NULL),
  m_symbolManager(NULL),
  m_bIsSymbolW2D(false),
  m_hatchFac(NULL),
  m_fillFac(NULL),
  m_obsMesh(NULL),
  m_w2dFile(NULL),
  m_w2dStream(NULL),
  m_w2dLabels(NULL),
  m_labelStream(NULL),
  m_w2dActive(NULL),
  m_pPage(NULL),
  m_input(NULL),
  m_xformer(NULL),
  m_bHaveViewport(false),
  m_bHaveLabels(false),
  m_drawableCount(0),
  m_labelMacroCount(0),
  m_linePatternActive(true)
{
    m_lLayerStreams.reserve(8);
    m_lLabelStreams.reserve(8);
    m_lAttributeResources.reserve(8);

    m_hObjNodes = new NodeTable();
}


DWFRenderer::~DWFRenderer()
{
    delete[] m_wtPointBuffer;
    delete m_hatchFac;
    delete m_fillFac;
    delete m_obsMesh;

    delete (NodeTable*)m_hObjNodes;

    // could be non-NULL if an exception was raised before this was
    // added to the attribute resources collection
    delete m_attributes;
}


//-----------------------------------------------------------------------------
//
// Initializes map generation with required map properties.
//
//-----------------------------------------------------------------------------
void DWFRenderer::StartMap(RS_MapUIInfo* mapInfo,
                           RS_Bounds&    extents,
                           double        mapScale,
                           double        dpi,
                           double        metersPerUnit,
                           CSysTransformer* xformToLL)
{
    if (dpi == 0.0)
        m_dpi = STANDARD_DISPLAY_DPI; // default -- 96 is usually true for Windows
    else
        m_dpi = dpi;

    m_metersPerUnit = metersPerUnit;
    m_extents = extents;

    double metersPerPixel = METERS_PER_INCH / m_dpi;

    // TODO: we can compute the map scale given the extents and the screen
    //       bounds, so if it is not specified, do the default computation

    // avoid 0 map scale
    m_mapScale = (mapScale <= 0.0)? 1.0 : mapScale;

    // drawing scale is map scale converted to [mapping units] / [pixels]
    m_drawingScale = m_mapScale * metersPerPixel / m_metersPerUnit;

    // scale used to convert to DWF logical coordinates in the range [0, 2^30-1]
    m_scale = (double)(INT_MAX/2) / rs_max(m_extents.width(), m_extents.height());

    m_offsetX = m_extents.minx - 0.5*m_extents.width();
    m_offsetY = m_extents.miny - 0.5*m_extents.height();

    m_nObjectId = 1;
    m_layerNum = 1;

    if (xformToLL)
        m_obsMesh = new ObservationMesh(m_extents, xformToLL);

    // remember the map info
    m_mapInfo = mapInfo;

    // initialize the font engine, now that scales have been set
    InitFontEngine(this);
}


//-----------------------------------------------------------------------------
//
// Tells the Renderer we are done with map generation. It should be overloaded
// to do useful things like save an EPlot or EMap DWF.
//
//-----------------------------------------------------------------------------
void DWFRenderer::EndMap()
{
    for (stream_list::iterator iter = m_lLayerStreams.begin();
        iter != m_lLayerStreams.end(); ++iter)
    {
        DWFCORE_FREE_OBJECT(*iter);
    }

    for (stream_list::iterator iter = m_lLabelStreams.begin();
        iter != m_lLabelStreams.end(); ++iter)
    {
        DWFCORE_FREE_OBJECT(*iter);
    }

    for (stream_list::iterator iter = m_lLayoutStreams.begin();
        iter != m_lLayoutStreams.end(); ++iter)
    {
        DWFCORE_FREE_OBJECT(*iter);
    }

    for (stream_list::iterator iter = m_lLayoutLabelStreams.begin();
        iter != m_lLayoutLabelStreams.end(); ++iter)
    {
        DWFCORE_FREE_OBJECT(*iter);
    }

    m_lLayerStreams.clear();
    m_lLabelStreams.clear();
    m_lLayoutStreams.clear();
    m_lLayoutLabelStreams.clear();

    delete m_obsMesh;
    m_obsMesh = NULL;

    // clear the map info
    m_mapInfo = NULL;
}


//-----------------------------------------------------------------------------
//
// Indicates beginning of a map layer. This function creates a W2D for feature
// data and a W2D for labeling. It also sets up selection support / object
// definition. Labeling W2D is not opened -- it will only be opened if there
// are labels.
//
//-----------------------------------------------------------------------------
void DWFRenderer::StartLayer(RS_LayerUIInfo* layerInfo, RS_FeatureClassInfo* classInfo)
{
    if (classInfo && layerInfo &&
       (layerInfo->selectable() || layerInfo->hastooltips() || layerInfo->hashyperlinks()))
    {
        _ASSERT(m_featureClass == NULL);

        // could be non-NULL if an exception was raised before this was
        // added to the attribute resources collection
        delete m_attributes;
        m_attributes = NULL;

        m_attributes = DWFCORE_ALLOC_OBJECT(DWFObjectDefinitionResource(
                                        DWFXML::kzElement_PageObjectDefinition,
                                        DWFXML::kzRole_ObjectDefinition));

        m_featureClass = DWFCORE_ALLOC_OBJECT(DWFDefinedObject(classInfo->name().c_str()));
        m_featureClassInfo = classInfo;

        //DWFProperty* prop = DWFCORE_ALLOC_OBJECT(DWFProperty(L"hello1", L"hello2", L"hello3"));
        //m_featureClass->addProperty(prop, true);

        //add the object to the resource -- we will not have to deallocate it
        //since it is now owned by the resource
        m_attributes->addObject(m_featureClass);
    }
    else
    {
        m_featureClass = NULL;
        m_featureClassInfo = NULL;
    }

    //clear object node table
    ((NodeTable*)m_hObjNodes)->clear();

    m_w2dFile = new WT_File();
    m_w2dLabels = new WT_File();

    //overload WHIP I/O functions with our memory streaming ones
    m_w2dFile->set_stream_open_action (my_open);
    m_w2dFile->set_stream_close_action(my_close);
    m_w2dFile->set_stream_read_action (my_read);
    m_w2dFile->set_stream_seek_action (my_seek);
    m_w2dFile->set_stream_write_action(my_write);
    m_w2dFile->set_stream_user_data(WD_Null);

    //overload WHIP I/O functions with our memory streaming ones
    m_w2dLabels->set_stream_open_action (my_open);
    m_w2dLabels->set_stream_close_action(my_close);
    m_w2dLabels->set_stream_read_action (my_read);
    m_w2dLabels->set_stream_seek_action (my_seek);
    m_w2dLabels->set_stream_write_action(my_write);
    m_w2dLabels->set_stream_user_data(WD_Null);

    m_bHaveLabels = false;

    OpenW2D(m_w2dFile);

    //set up default line decorations
    WT_Line_Style style;
    style.dash_start_cap() = WT_Line_Style::Round_Cap;
    style.dash_end_cap() = WT_Line_Style::Round_Cap;
    style.line_start_cap() = WT_Line_Style::Round_Cap;
    style.line_end_cap() = WT_Line_Style::Round_Cap;
    style.line_join() = WT_Line_Style::Bevel_Join;
    style.adapt_patterns() = false;
    m_w2dFile->desired_rendition().line_style() = style;

    if (layerInfo)
    {
        //set the W2D layer id -- this is needed for the EMap case
        //but should not matter for the EPlot case
        WT_Layer lr(*m_w2dFile, m_layerNum, Util_ConvertString(layerInfo->name().c_str()));
        m_w2dFile->desired_rendition().layer() = lr;

        WT_Layer lrl(*m_w2dLabels, m_layerNum, Util_ConvertString(layerInfo->name().c_str()));
        m_w2dLabels->desired_rendition().layer() = lrl;

        m_layerNum++;
    }

    m_imgID = 0;
    m_drawableCount = 0;
    m_labelMacroCount = 0;

    // remember the layer info
    m_layerInfo = layerInfo;

    // initialize the pattern tracking flag
    m_linePatternActive = true;

    // we must reset the cached symbol information each time we begin
    // a layer to ensure that the necessary initial opcodes are added
    // to the W2D for the layer
    m_lastSymbol = RS_MarkerDef();
}


//-----------------------------------------------------------------------------
//
// Cleans up W2D files after processing for a layer is done. Should be
// overloaded in child classes to do useful things like add W2D files
// as resources to an EPlot or EMap.
//
//-----------------------------------------------------------------------------
void DWFRenderer::EndLayer()
{
    //keep track of memory output streams so that we can free them
    //when the map is done
    m_lLayerStreams.push_back((DWFBufferOutputStream*)m_w2dFile->stream_user_data());

    m_w2dFile->close();
    delete m_w2dFile;
    m_w2dFile = NULL;

    if (m_bHaveLabels && m_labelMacroCount > 0)
    {
        //Only need to keep track of label w2d if the stream was opened,
        //which happens when there is at least one label.  The check for
        //macros ensures that the w2d is not empty, since the viewer
        //doesn't like it otherwise (i.e. it crashes).
        m_lLabelStreams.push_back((DWFBufferOutputStream*)m_w2dLabels->stream_user_data());
    }
    else
        m_lLabelStreams.push_back(NULL); //add NULL for no labels

    m_w2dLabels->close();
    delete m_w2dLabels;
    m_w2dLabels = NULL;
    m_bHaveLabels = false;

    if (m_featureClass)
    {
        m_lAttributeResources.push_back(m_attributes);
        //note: we don't have to delete m_attributes, since
        //it will eventually be owned by the DWF graphic resource
        //which will free it
        m_attributes = NULL;
    }
    else
    {
        m_lAttributeResources.push_back(NULL);
    }

    //it's ok for it to be null
    m_featureClass = NULL;
    m_featureClassInfo = NULL;

    // clear the layer info
    m_layerInfo = NULL;
}


//----------------------------------------------------------------------------
// All subsequent drawables will be considered as part of a single feature
// identifier by the given feature reader, until the next call to StartFeature()
//----------------------------------------------------------------------------
void DWFRenderer::StartFeature(RS_FeatureReader* feature,
                               bool /*initialPass*/,
                               const RS_String* tooltip,
                               const RS_String* url,
                               const RS_String* /*theme*/,
                               double /*zOffset*/,
                               double /*zExtrusion*/,
                               RS_ElevationType /*zOffsetType*/)
{
    //attributes and selection support
    StoreAttributes(feature, tooltip, url);
}


//-----------------------------------------------------------------------------
//
// Polygon features. Transformation to W2D space is done here. Also handles
// holes etc.
//
//-----------------------------------------------------------------------------
void DWFRenderer::ProcessPolygon(LineBuffer* srclb, RS_FillStyle& fill)
{
    LineBuffer* workbuffer = srclb->Optimize(m_drawingScale, m_pPool);
    std::auto_ptr<LineBuffer> spLB(workbuffer);

    if (workbuffer->point_count() == 0)
    {
        LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
        return;
    }

    if (fill.color().alpha() != 0)
    {
        WriteFill(fill);

        _TransformPointsNoClamp(workbuffer);

        if (workbuffer->cntr_count() == 1)
        {
            // just a polygon, no need for a contour set
            WT_Polygon polygon(workbuffer->point_count(), m_wtPointBuffer, false);
            polygon.serialize(*m_w2dFile);
            IncrementDrawableCount();
        }
        else
        {
            // otherwise make a contour set

            // ensure the contours are oriented per the DWF spec
            OrientContours(workbuffer->cntr_count(), workbuffer->cntrs(), m_wtPointBuffer);

            // now create the contour set
            WT_Contour_Set cset(*m_w2dFile, workbuffer->cntr_count(), (WT_Integer32*)workbuffer->cntrs(), workbuffer->point_count(), m_wtPointBuffer, true);
            cset.serialize(*m_w2dFile);
            IncrementDrawableCount();
        }
    }

    //write out the polygon outline as a bunch of WT_Polylines
    if (fill.outline().color().alpha() == 0)
    {
        LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
        return;
    }

    WriteStroke(fill.outline());

    bool oldLinePatternActive = m_linePatternActive;

    //determine pattern to apply
    WT_Line_Pattern lpat(WT_Line_Pattern::Solid);
    WT_Dash_Pattern dpat(WT_Dash_Pattern::kNull);
    if ((_wcsnicmp(fill.outline().style().c_str(), L"Solid", 6) != 0))
    {
        int patid = ConvertToDashPattern(fill.outline().style().c_str(), m_dpi, fill.outline().width() / METERS_PER_INCH * m_dpi, dpat, lpat);
        m_linePatternActive = patid < WT_Line_Pattern::Count;
    }
    else
        m_linePatternActive = true;

    //apply the line or dash pattern
    if (m_linePatternActive)
    {
        if (m_linePatternActive != oldLinePatternActive)
        {
            // We're switching from a dash pattern back to a line pattern.  We need to
            // force Whip to write out the line pattern opcode, otherwise the drawable
            // will be rendered with the dash pattern.  We do this by setting a line
            // pattern on the actual rendition (not the desired rendition) which is
            // s different than the one we want.
            m_w2dFile->rendition().line_pattern() = WT_Line_Pattern::Illegal;
        }

        m_w2dFile->desired_rendition().line_pattern() = lpat;
    }
    else
    {
        if (m_linePatternActive != oldLinePatternActive)
        {
            // We're switching from a line pattern back to a dash pattern.  We need to
            // force Whip to write out the dash pattern opcode, otherwise the drawable
            // will be rendered with the line pattern.  We do this by setting a dash
            // pattern on the actual rendition (not the desired rendition) which is
            // different than the one we want.
            m_w2dFile->rendition().dash_pattern() = WT_Dash_Pattern::kNull;
        }

        m_w2dFile->desired_rendition().dash_pattern() = dpat;
    }

    WritePolylines(workbuffer);

    if (m_obsMesh)
        m_obsMesh->ProcessPoint(workbuffer->x_coord(0), workbuffer->y_coord(0));

    LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
}


//-----------------------------------------------------------------------------
//
// Polyline features. Transformation to W2D space is done here.
//
//-----------------------------------------------------------------------------
void DWFRenderer::ProcessPolyline(LineBuffer* srclb, RS_LineStroke& lsym)
{
    if (srclb->point_count() == 0)
        return;

    if (lsym.color().alpha() == 0)
        return;

    WriteStroke(lsym);

    bool oldLinePatternActive = m_linePatternActive;

    //determine pattern to apply
    WT_Line_Pattern lpat(WT_Line_Pattern::Solid);
    WT_Dash_Pattern dpat(WT_Dash_Pattern::kNull);
    if ((_wcsnicmp(lsym.style().c_str(), L"Solid", 6) != 0))
    {
        int patid = ConvertToDashPattern(lsym.style().c_str(), m_dpi, lsym.width() / METERS_PER_INCH * m_dpi, dpat, lpat);
        m_linePatternActive = patid < WT_Line_Pattern::Count;
    }
    else
        m_linePatternActive = true;

    //apply the line or dash pattern
    if (m_linePatternActive)
    {
        if (m_linePatternActive != oldLinePatternActive)
        {
            // We're switching from a dash pattern back to a line pattern.  We need to
            // force Whip to write out the line pattern opcode, otherwise the drawable
            // will be rendered with the dash pattern.  We do this by setting a line
            // pattern on the actual rendition (not the desired rendition) which is
            // s different than the one we want.
            m_w2dFile->rendition().line_pattern() = WT_Line_Pattern::Illegal;
        }

        m_w2dFile->desired_rendition().line_pattern() = lpat;
    }
    else
    {
        if (m_linePatternActive != oldLinePatternActive)
        {
            // We're switching from a line pattern back to a dash pattern.  We need to
            // force Whip to write out the dash pattern opcode, otherwise the drawable
            // will be rendered with the line pattern.  We do this by setting a dash
            // pattern on the actual rendition (not the desired rendition) which is
            // different than the one we want.
            m_w2dFile->rendition().dash_pattern() = WT_Dash_Pattern::kNull;
        }

        m_w2dFile->desired_rendition().dash_pattern() = dpat;
    }

    LineBuffer* workbuffer = srclb->Optimize(m_drawingScale, m_pPool);
    std::auto_ptr<LineBuffer> spLB(workbuffer);

    WritePolylines(workbuffer);

    if (m_obsMesh)
        m_obsMesh->ProcessPoint(workbuffer->x_coord(0), workbuffer->y_coord(0));

    LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
}


//-----------------------------------------------------------------------------
//
// Raster serialization.
//
//-----------------------------------------------------------------------------
void DWFRenderer::ProcessRaster(unsigned char* _data,
                                int            _length,
                                RS_ImageFormat _format,
                                int            width,
                                int            height,
                                RS_Bounds&     extents,
                                TransformMesh* xformMesh)
{
    unsigned char* data = _data;
    int length = _length;
    RS_ImageFormat format = _format;

    agg_context* aggcxt = NULL;

    if (NULL != xformMesh)
    {
        double cx = 0.5 * (extents.minx + extents.maxx);
        double cy = 0.5 * (extents.miny + extents.maxy);
        WorldToScreenPoint(cx, cy, cx, cy);
        cx = cx / GetScreenUnitsPerPixel();
        cy = cy / GetScreenUnitsPerPixel();

        // width and height of (projected) image in device space
        double imgDevW = extents.width() * m_scale / GetScreenUnitsPerPixel() + 1;
        double imgDevH = extents.height() * m_scale / GetScreenUnitsPerPixel() + 1;

        // create agg_context for outputting projected raster
        //
        aggcxt = new agg_context(NULL, (int)imgDevW, (int)imgDevH);
        aggcxt->ren.clear(agg::argb8_packed(0));

        AGGRenderer::DrawScreenRasterTransform(aggcxt, data, length, format, width, height, cx, cy, imgDevW, imgDevH, xformMesh);
        //AGGRenderer::_DrawDummyRect(aggcxt, imgDevW, imgDevH, xformMesh);
        int aggw = aggcxt->rb.width();
        int aggh = aggcxt->rb.height();

        RS_Color bgColor = 0;
        RS_ByteData* byte_data = AGGImageIO::Save(L"PNG", aggcxt->m_rows, aggw, aggh, aggw, aggh, bgColor);
        data = byte_data->GetBytes();
        length = byte_data->GetNumBytes();
        format = RS_ImageFormat_PNG;
    }

    if (format != RS_ImageFormat_ABGR && format != RS_ImageFormat_PNG)
    {
        //TODO: support other formats...
        _ASSERT(false);
        return;
    }

    if (m_obsMesh)
    {
        m_obsMesh->ProcessPoint(extents.minx, extents.miny);
        m_obsMesh->ProcessPoint(extents.maxx, extents.miny);
        m_obsMesh->ProcessPoint(extents.maxx, extents.maxy);
        m_obsMesh->ProcessPoint(extents.minx, extents.maxy);
    }

    WT_Integer32 x0 = (WT_Integer32)_TX(extents.minx);
    WT_Integer32 y0 = (WT_Integer32)_TY(extents.miny);
    WT_Integer32 x1 = (WT_Integer32)_TX(extents.maxx);
    WT_Integer32 y1 = (WT_Integer32)_TY(extents.maxy);

    if (format == RS_ImageFormat_ABGR)
    {
        WT_Image img((WT_Unsigned_Integer16)height,
                     (WT_Unsigned_Integer16)width,
                     WT_Image::/*WT_Image_Format::*/RGBA,
                     m_imgID++,
                     NULL,
                     length,
                     data,
                     WT_Logical_Point(x0, y0),
                     WT_Logical_Point(x1, y1),
                     false);

        img.serialize(*m_w2dFile);
        IncrementDrawableCount();
    }
    else if (format == RS_ImageFormat_PNG)
    {
        WT_PNG_Group4_Image img((WT_Unsigned_Integer16)height,
                    (WT_Unsigned_Integer16)width,
                    WT_PNG_Group4_Image::PNG,
                    m_imgID++,
                    NULL,
                    length,
                    data,
                    WT_Logical_Point(x0, y0),
                    WT_Logical_Point(x1, y1),
                    false);

        img.serialize(*m_w2dFile);
        IncrementDrawableCount();
    }

    delete aggcxt;
}


//-----------------------------------------------------------------------------
//
// Marker Symbols
//
//-----------------------------------------------------------------------------
void DWFRenderer::ProcessMarker(LineBuffer* srclb, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds)
{
    for (int i=0; i<srclb->point_count(); i++)
    {
        //if marker is processed from here it should be added to the
        //feature W2D, not the labeling W2D -- need the API to reflect that.
        ProcessOneMarker(srclb->x_coord(i), srclb->y_coord(i), mdef, allowOverpost, bounds);
    }
}


void DWFRenderer::ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds)
{
    WT_File* file = m_w2dFile;

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

    //default bounds of symbol data in W2D
    //for symbols created by MapGuide Studio
    RS_Bounds src(0, 0, SYMBOL_MAX, SYMBOL_MAX);

    //default reference point (negated, to mimic the one that is stored in the
    //symbol library)
    double refX = mdef.insx();
    double refY = mdef.insy();

    //rotation angle
    double angleRad = mdef.rotation() * M_PI180;

    //we will fit all symbol data inside this extent
    //we will then enclose all that data in a Macro
    //opcode which will use SYMBOL_MAX as its scale
    //this will be done for both device space and mapping space
    //sized symbols -- the difference happens when we play the macro
    //Device space macros are played with negative size

    //construct a scaling bounding box that will take care of
    //the desired aspect ratio -- not that overall scaling to the
    //correct size will be done using the macro attributes, but we
    //need to take aspect into account ourselves, since macro
    //only has a single overall scale factor
    double lastAspect = (m_lastSymbol.width() == 0.0)? DBL_MAX : m_lastSymbol.height() / m_lastSymbol.width();
    double aspect = (mdef.width() == 0.0)? DBL_MAX : mdef.height() / mdef.width();

    RS_Bounds dst;
    if (aspect <= 1.0)
        dst = RS_Bounds(0, 0, SYMBOL_MAX, aspect * SYMBOL_MAX);
    else
        dst = RS_Bounds(0, 0, SYMBOL_MAX / aspect, SYMBOL_MAX);

    //construct transformer -- we will use one
    //even for the default symbol -- makes sure
    //it goes through the same transformation path
    SymbolTrans trans(src, dst, refX, refY, angleRad);

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

        //approximately carry over the insertion point -- it will map the unit square
        //based insertion point to a square of size font height.
        //Also convert the insertion point vector from a lower-left corner delta
        //to an offset from the center of the character -- since we use Center/Halfline
        //alignment when we draw it as text
        double scaledRefX = (0.5 - refX) * fdef.height();
        double scaledRefY = (0.5 - refY) * fdef.height();

        //pass on to label drawing code to draw the text using a macro
        RS_LabelInfo info(x, y, scaledRefX, scaledRefY, tdef.font().units(), tdef);
        ProcessLabelGroup(&info, 1, mdef.name(), RS_OverpostType_All, false, NULL, 0.0);
    }
    else if (symbol)
    {
        //case where we are using a W2D symbol from the library
        _ASSERT(mdef.type() == RS_MarkerType_W2D);

        //check to see if the last symbol we got was the same
        if (   mdef.type() != m_lastSymbol.type()
            || mdef.insx() != m_lastSymbol.insx()
            || mdef.insy() != m_lastSymbol.insy()
            || mdef.rotation() != m_lastSymbol.rotation()
            || mdef.library() != m_lastSymbol.library()
            || mdef.name() != m_lastSymbol.name()
            || mdef.style().color().argb() != m_lastSymbol.style().color().argb()
            || mdef.style().outline().color().argb() != m_lastSymbol.style().outline().color().argb()
            || mdef.style().background().argb() != m_lastSymbol.style().background().argb()
            || aspect != lastAspect)
        {
            m_lastSymbol = mdef;

            //also set flags controlling how W2Ds are rewritten into the destination DWF
            m_bIsSymbolW2D = true;
            m_mdOverrideColors = mdef;

            //enclose W2D geometry in a macro
            BeginMacro(file, 0, SYMBOL_MAX);

                //copy symbol W2D into destination
                AddDWFContent(symbol, &trans, mdef.name(), RS_String(L""), RS_String(L""));

            //end macro definition and play the macro
            EndMacro(file, 0);

            m_bIsSymbolW2D = false;
        }

        PlayMacro(file, 0, rs_max(mdef.width(), mdef.height()), mdef.units(), x, y);
    }
    else
    {
        //case where we are using an SLD symbol, and fall-through for other cases

        //check to see if the last symbol we got was the same
        if (   mdef.type() != m_lastSymbol.type()
            || mdef.insx() != m_lastSymbol.insx()
            || mdef.insy() != m_lastSymbol.insy()
            || mdef.rotation() != m_lastSymbol.rotation()
            || mdef.markernum() != m_lastSymbol.markernum()
            || mdef.style().color().argb() != m_lastSymbol.style().color().argb()
            || mdef.style().outline().color().argb() != m_lastSymbol.style().outline().color().argb()
            || aspect != lastAspect)
        {
            m_lastSymbol = mdef;

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

            EnsureBufferSize(npts);
            WT_Logical_Point* pts = m_wtPointBuffer;

            double tempx, tempy;
            for (int i=0; i<npts; i++)
            {
                //transform from unity to a SYMBOL_MAX sized square
                tempx = poly[i].x * SYMBOL_MAX;
                tempy = poly[i].y * SYMBOL_MAX;

                trans.TransformPoint(tempx, tempy);

                pts[i].m_x = (WT_Integer32)tempx;
                pts[i].m_y = (WT_Integer32)tempy;
            }

            //enclose W2D geometry in a macro
            BeginMacro(file, 0, SYMBOL_MAX);

                if (!found)
                {
                    //unknown symbol
                    file->desired_rendition().color() = WT_Color(255, 0, 0);
                    WT_Polyline symbol(npts, pts, false);
                    symbol.serialize(*file);
                    IncrementDrawableCount();
                }
                else
                {
                    if (mdef.style().color().argb() == RS_Color::EMPTY_COLOR_ARGB)
                        file->desired_rendition().color() = WT_Color(0, 0, 255);
                    else
                        file->desired_rendition().color() = WT_Color(Util_ConvertColor(mdef.style().color()));

                    WT_Polygon symbolFill(npts, pts, false);
                    symbolFill.serialize(*file);
                    IncrementDrawableCount();

                    if (mdef.style().outline().color().argb() == RS_Color::EMPTY_COLOR_ARGB)
                        file->desired_rendition().color() = WT_Color(127, 127, 127);
                    else
                        file->desired_rendition().color() = WT_Color(Util_ConvertColor(mdef.style().outline().color()));

                    WT_Polyline symbol(npts, pts, false);
                    symbol.serialize(*file);
                    IncrementDrawableCount();
                }

            //end macro definition
            EndMacro(file, 0);
        }

        PlayMacro(file, 0, rs_max(mdef.width(), mdef.height()), mdef.units(), x, y);
    }

    if (!allowOverpost)
    {
        //now add an exclusion region to the file so that
        //labels do not overlap the symbol

        //a square that we will transform into the dst bounds
        WT_Logical_Point box[4];
        box[0].m_x = (WT_Integer32)0;
        box[0].m_y = (WT_Integer32)0;
        box[1].m_x = (WT_Integer32)SYMBOL_MAX;
        box[1].m_y = (WT_Integer32)0;
        box[2].m_x = (WT_Integer32)SYMBOL_MAX;
        box[2].m_y = (WT_Integer32)SYMBOL_MAX;
        box[3].m_x = (WT_Integer32)0;
        box[3].m_y = (WT_Integer32)SYMBOL_MAX;

        //construct transformer -- same as the
        //one used for the actual symbol drawables
        SymbolTrans boxtrans(src, dst, refX, refY, angleRad);

        EnsureBufferSize(4);
        WT_Logical_Point* pts = m_wtPointBuffer;

        double tempx, tempy;
        for (int i=0; i<4; i++)
        {
            tempx = box[i].m_x;
            tempy = box[i].m_y;

            boxtrans.TransformPoint(tempx, tempy);

            pts[i].m_x = (WT_Integer32)tempx;
            pts[i].m_y = (WT_Integer32)tempy;
        }

        //define a macro for the overpost region and then play the
        //macro inside an Overpost opcode
        //NOTE: cannot define a macro inside the overpost opcode!!

        //Use macro slot 1, since we want to keep the symbol in macro 0
        //so that we can reuse it if the next point symbol macro we need
        //is the same symbol
        BeginMacro(file, 1, SYMBOL_MAX);

            // if there's no rotation, send just 2 points
            if (angleRad == 0.0)
            {
                WT_Logical_Point axisbox[2];
                axisbox[0].m_x = pts[0].m_x;
                axisbox[0].m_y = pts[0].m_y;
                axisbox[1].m_x = pts[2].m_x;
                axisbox[1].m_y = pts[2].m_y;

                WT_Polyline excludearea(2, axisbox, false);
                excludearea.serialize(*file);
                IncrementDrawableCount();
            }
            else
            {
                WT_Polyline excludearea(4, pts, false);
                excludearea.serialize(*file);
                IncrementDrawableCount();
            }

        EndMacro(file, 1);

        //write the exclusion area
        BeginOverpostGroup(file, RS_OverpostType_All, false, true);
            PlayMacro(file, 1, rs_max(mdef.width(), mdef.height()), mdef.units(), x, y);
        EndOverpostGroup(file);
    }

    //set actual (unrotated) bounds with new insertion point if a pointer was passed in
    if (bounds)
    {
        bounds->minx =       -refX  * mdef.width();
        bounds->maxx = (1.0 - refX) * mdef.width();
        bounds->miny =       -refY  * mdef.height();
        bounds->maxy = (1.0 - refY) * mdef.height();
    }

    if (m_obsMesh)
        m_obsMesh->ProcessPoint(x, y);
}


//-----------------------------------------------------------------------------
//
// Text labels
//
//-----------------------------------------------------------------------------
void DWFRenderer::ProcessLabelGroup(RS_LabelInfo*    labels,
                                    int              nlabels,
                                    const RS_String& text,
                                    RS_OverpostType  type,
                                    bool             exclude,
                                    LineBuffer*      /*path*/,
                                    double           /*scaleLimit*/)
{
    if (nlabels == 0)
        return;

    WT_File* file = m_w2dFile;

    //check if it is a text layer or a feature label
    //This is sufficient to check the overpost type
    //to see if it matches that used only for a text layer,
    //but nevertheless this is a hack and there should be a way to indicate
    //a text feature (as opposed to a label)
    if (type != RS_OverpostType_All)
    {
        //open the label W2D if it's not yet open
        if (!m_bHaveLabels)
        {
            OpenW2D(m_w2dLabels);
            m_bHaveLabels = true;
        }

        file = m_w2dLabels;
    }

    // Choose a decent reference size for computing the macro scale.  A
    // small percentage of the map width is reasonable.
    double refSizeMeters = 0.01 * m_extents.width() * m_metersPerUnit;

    //define a macro for each candidate label
    for (int i=0; i<nlabels; i++)
    {
        RS_LabelInfo* info = &labels[i];

        //if the symbol size was mapping space
        //we will place the label macro at the edge of the
        //symbol, without using an offset, otherwise we
        //need to include the offset in the macro so that
        //the offset is constant size when drawn
        if (info->dunits() == RS_Units_Device)
        {
            int scale = (int)_MeterToW2DMacroUnit(RS_Units_Device, refSizeMeters);

            //symbol is device space
            //we will need to place the label inside
            //a constant size macro at an offset
            //corresponding to the constant size offset from the
            //symbol center point

            //NOTE: Having a mapping space font size inside this
            //constant size macro will cause problems, but is
            //unavoidable with current Macro implementation in viewer

            double offx = _MeterToW2DMacroUnit(info->dunits(), info->dx());
            double offy = _MeterToW2DMacroUnit(info->dunits(), info->dy());

            WT_Integer32 ioffx = (offx < 0)? (int)floor(offx) : (int)ceil(offx);
            WT_Integer32 ioffy = (offy < 0)? (int)floor(offy) : (int)ceil(offy);

            BeginMacro(file, i+1, scale);

                ProcessMultilineText(file, text, info->tdef(), ioffx, ioffy);

            EndMacro(file, i+1);
        }
        else
        {
            //symbol was mapping space
            int scale = (int)_MeterToW2DMacroUnit(info->tdef().font().units(), refSizeMeters);

            BeginMacro(file, i+1, scale);

                ProcessMultilineText(file, text, info->tdef(), 0, 0);

            EndMacro(file, i+1);
        }
    }

    //now play the label macros inside an overpost
    BeginOverpostGroup(file, type, true, exclude);

        for (int i=0; i<nlabels; i++)
        {
            RS_LabelInfo* info = &labels[i];

            WriteTextDef(file, info->tdef());

            //we need to sync the rendition manually
            //since we are about to play macros by writing
            //to the file directly
            WT_Integer32 parts_to_sync = WT_Rendition::Color_Bit           |
                                     //  WT_Rendition::Color_Map_Bit       |
                                     //  WT_Rendition::Fill_Bit            |
                                     //  WT_Rendition::Fill_Pattern_Bit    |
                                     //  WT_Rendition::Merge_Control_Bit   |
                                     //  WT_Rendition::BlockRef_Bit        |
                                         WT_Rendition::Visibility_Bit      |
                                     //  WT_Rendition::Line_Weight_Bit     |
                                         WT_Rendition::Pen_Pattern_Bit     |
                                     //  WT_Rendition::Line_Pattern_Bit    |
                                     //  WT_Rendition::Line_Caps_Bit       |
                                     //  WT_Rendition::Line_Join_Bit       |
                                     //  WT_Rendition::Marker_Size_Bit     |
                                     //  WT_Rendition::Marker_Symbol_Bit   |
                                     //  WT_Rendition::URL_Bit             |
                                         WT_Rendition::Layer_Bit           |
                                     //  WT_Rendition::Viewport_Bit        |
                                         WT_Rendition::Font_Extension_Bit  |
                                         WT_Rendition::Font_Bit            |
                                     //  WT_Rendition::Object_Node_Bit     |
                                         WT_Rendition::Text_Background_Bit |
                                         WT_Rendition::Text_HAlign_Bit     |
                                         WT_Rendition::Text_VAlign_Bit     |
                                         WT_Rendition::Contrast_Color_Bit;

            file->desired_rendition().sync(*file, parts_to_sync);

            if (info->dunits() == RS_Units_Device)
            {
                //symbol is device space -- we will play it
                //at the symbol centerpoint. In this case
                //the label macro already has the offset from the
                //symbol center inside it

                //NOTE: The problem here is that if the symbol was constant
                //size while the label is mapping space, the macro will play as
                //a mapping space macro -- this will cause it to move with
                //respect to the symbol which is bad, but unavoidable
                //with the current implementation of macro

                PlayMacro(file, i+1, refSizeMeters, RS_Units_Device, info->x(), info->y());
            }
            else
            {
                //symbol is mapping space -- we will play
                //the label macro at the mapping space position
                //given by the symbol center and the label offset

                double posx = info->x();
                double posy = info->y();

                posx += _MeterToMapSize(info->dunits(), info->dx());
                posy += _MeterToMapSize(info->dunits(), info->dy());

                PlayMacro(file, i+1, refSizeMeters, info->tdef().font().units(), posx, posy);
            }
        }

    EndOverpostGroup(file);
}


void DWFRenderer::BeginOverpostGroup(WT_File* file, RS_OverpostType type, bool render, bool exclude)
{
    char op[256];
    sprintf(op, "(Overpost ");

    switch (type)
    {
    case RS_OverpostType_All:
        strcat(op, "All ");
        break;
    case RS_OverpostType_AllFit:
        strcat(op, "AllFit ");
        break;
    case RS_OverpostType_FirstFit:
    default:
        strcat(op, "FirstFit ");
        break;
    }

    if (render)
        strcat(op, "True ");
    else
        strcat(op, "False ");

    if (exclude)
        strcat(op, "True (");
    else
        strcat(op, "False (");

    //open the overpost group
    file->write(op);
}


void DWFRenderer::EndOverpostGroup(WT_File* file)
{
    //close the overpost group
    file->write("))");
}


void DWFRenderer::BeginMacro(WT_File* file, int id, int scale)
{
    char temp[256];
    sprintf(temp, "(Macro %d %d (", id, scale);
    file->write(temp);

    m_macroDrawableCounts[id] = m_drawableCount;
}


void DWFRenderer::EndMacro(WT_File* file, int id)
{
    //close the Define Macro opcode
    file->write("))");

    //update the number of drawables contained in this macro
    m_macroDrawableCounts[id] = m_drawableCount - m_macroDrawableCounts[id];
}


void DWFRenderer::PlayMacro(WT_File* file, int id, double sizeMeters, RS_Units unit, double x, double y)
{
    // don't play empty macro definitions
    if (m_macroDrawableCounts[id] == 0)
        return;

    //play the macro at the given position with the given screen size
    int size = (int)_MeterToW2DMacroUnit(unit, sizeMeters);

    if (unit == RS_Units_Device)
        size = -size;

    WT_Integer32 ix = (WT_Integer32)_TX(x);
    WT_Integer32 iy = (WT_Integer32)_TY(y);

    char temp[256];
    sprintf(temp, " G %d S %d M 1 %d,%d", id, size, ix, iy);
    file->write(temp);

    // keep track if this macro was played inside the label w2d
    if (file == m_w2dLabels)
        ++m_labelMacroCount;
}


void DWFRenderer::IncrementDrawableCount()
{
    ++m_drawableCount;
}


//////////////////////////////////////////////////////////////////////////////
// Finds occurences of line breaks and adds the start pointer of each
// line to the supplied array.  Line breaks can be any of the following:
//   "\\n"                      // sequence currently used by MG OS / Enterprise
//   \n\r  (char 13 + char 10)  // common in RDBMS like Oracle
//   \r\n  (char 10 + char 13)
//   \n    (char 13)            // used by MG 6.5
//   \r    (char 10)            // common in Linux
size_t DWFRenderer::SplitLabel(wchar_t* label, vector<wchar_t*>& line_breaks)
{
    _ASSERT(label != NULL);
    if (label == NULL)
        return 0;

    // first line
    line_breaks.push_back(label);

    for (;;)
    {
        // find the next line break - note that we must search
        // for the 2-character sequences FIRST
        wchar_t* found;
        for (;;)
        {
            found = ::wcsstr(label, L"\\n");
            if (found != NULL)
            {
                // null terminate line break (2 characters)
                *found++ = 0;
                *found++ = 0;
                break;
            }

            found = ::wcsstr(label, L"\n\r");
            if (found != NULL)
            {
                // null terminate line break (2 characters)
                *found++ = 0;
                *found++ = 0;
                break;
            }

            found = ::wcsstr(label, L"\r\n");
            if (found != NULL)
            {
                // null terminate line break (2 characters)
                *found++ = 0;
                *found++ = 0;
                break;
            }

            found = ::wcsstr(label, L"\n");
            if (found != NULL)
            {
                // null terminate line break (1 character)
                *found++ = 0;
                break;
            }

            found = ::wcsstr(label, L"\r");
            if (found != NULL)
            {
                // null terminate line break (1 character)
                *found++ = 0;
            }

            break;
        }

        if (found == NULL)
            break;

        label = found;
        line_breaks.push_back(label);
    }

    return line_breaks.size();
}


void DWFRenderer::ProcessMultilineText(WT_File* file, const RS_String& txt, RS_TextDef& tdef, int x, int y)
{
    //make a temporary copy
    size_t len = wcslen(txt.c_str());
    wchar_t* cpy = (wchar_t*)alloca((len + 1) * sizeof(wchar_t));
    wcscpy(cpy, txt.c_str());

    //break it up
    vector<wchar_t*> line_breaks;
    size_t num_lines = SplitLabel(cpy, line_breaks);

    //optimization
    if (num_lines <= 1)
    {
        WT_Logical_Point pt(x, y);
        WT_Text wttext(pt, Util_ConvertString(txt.c_str()));
        wttext.serialize(*file);
        IncrementDrawableCount();
        return;
    }

    //find vertical shift increment for each line
    double line_hgt = _MeterToW2DMacroUnit(tdef.font().units(), tdef.font().height());

    //account for any rotation of the text
    WT_Integer32 line_hgt_x = 0;
    WT_Integer32 line_hgt_y = (WT_Integer32)line_hgt;
    if (tdef.rotation() != 0.0)
    {
        double angleRad = tdef.rotation() * M_PI180;
        line_hgt_x = (WT_Integer32)(-line_hgt * sin(angleRad));
        line_hgt_y = (WT_Integer32)( line_hgt * cos(angleRad));
    }

    //Depending on vertical alignment we may go up or down with the line
    //increment.  Take that into account by shifting initial y value.  In
    //the case of rotated text it's a shift in both x and y.
    int xpos = x;
    int ypos = y;

    switch (tdef.valign())
    {
    case RS_VAlignment_Descent:
    case RS_VAlignment_Base:
        xpos += ((int)num_lines - 1) * line_hgt_x;
        ypos += ((int)num_lines - 1) * line_hgt_y;
        break;
    case RS_VAlignment_Half:
        xpos += ((int)num_lines - 1) * line_hgt_x / 2;
        ypos += ((int)num_lines - 1) * line_hgt_y / 2;
        break;
    case RS_VAlignment_Cap:
    case RS_VAlignment_Ascent:
        //xpos = x;
        //ypos = y;
        break;
    }

    //now draw each text line
    for (size_t i=0; i<num_lines; i++, xpos -= line_hgt_x, ypos -= line_hgt_y)
    {
        WT_Logical_Point pt(xpos, ypos);
        WT_Text wttext(pt, Util_ConvertString(line_breaks[i]));
        wttext.serialize(*file);
        IncrementDrawableCount();
    }
}


//-----------------------------------------------------------------------------
//
// Transformation functions from mapping to W2D space.
// It's a little ghetto to have separate ones for x and y
// but makes the rest of the code a little cleaner.
//
//-----------------------------------------------------------------------------


//transforms an x coordinate from mapping to DWF space
double DWFRenderer::_TX(double x)
{
    //none of these should happen -- otherwise the DWF will get integer overflow
    //since we scale the bounding extents to be the full int32 range
    //so we clamp, in case of floating point precision inaccuracies
    //that might make a cast to int wrap around to negative territory
//    if (x > m_extents.maxx)
//        x = m_extents.maxx;
//    else if (x < m_extents.minx)
//        x = m_extents.minx;

    return (x - m_offsetX) * m_scale;
}


//transforms a y coordinate from mapping to DWF space
double DWFRenderer::_TY(double y)
{
    //none of these should happen -- otherwise the DWF will get integer overflow
    //since we scale the bounding extents to be the full int32 range
    //so we clamp, in case of floating point precision inaccuracies
    //that might make a cast to int wrap around to negative territory
//    if (y > m_extents.maxy)
//        y = m_extents.maxy;
//    else if (y < m_extents.miny)
//        y = m_extents.miny;

    return (y - m_offsetY) * m_scale;
}


//transforms an array of input mapping space points
//into W2D coordinates and places them in the DWF
//point buffer m_wtPointBuffer
void DWFRenderer::_TransformPointsNoClamp(LineBuffer* plb)
{
    int numpts = plb->point_count();
    EnsureBufferSize(numpts);
    WT_Integer32* wpts = (WT_Integer32*)m_wtPointBuffer;

    /*
    //loop unrolled 8 times using Duff's device.
    //See Stroustrup 6.6 Ex. 15
    int n = (numpts + 7) / 8;
    switch (numpts % 8)
    {
    case 0: do {  *wpts++ = (WT_Integer32)((*inpts++ - m_offsetX) * m_scale);
                  *wpts++ = (WT_Integer32)((*inpts++ - m_offsetY) * m_scale);
    case 7:       *wpts++ = (WT_Integer32)((*inpts++ - m_offsetX) * m_scale);
                  *wpts++ = (WT_Integer32)((*inpts++ - m_offsetY) * m_scale);
    case 6:       *wpts++ = (WT_Integer32)((*inpts++ - m_offsetX) * m_scale);
                  *wpts++ = (WT_Integer32)((*inpts++ - m_offsetY) * m_scale);
    case 5:       *wpts++ = (WT_Integer32)((*inpts++ - m_offsetX) * m_scale);
                  *wpts++ = (WT_Integer32)((*inpts++ - m_offsetY) * m_scale);
    case 4:       *wpts++ = (WT_Integer32)((*inpts++ - m_offsetX) * m_scale);
                  *wpts++ = (WT_Integer32)((*inpts++ - m_offsetY) * m_scale);
    case 3:       *wpts++ = (WT_Integer32)((*inpts++ - m_offsetX) * m_scale);
                  *wpts++ = (WT_Integer32)((*inpts++ - m_offsetY) * m_scale);
    case 2:       *wpts++ = (WT_Integer32)((*inpts++ - m_offsetX) * m_scale);
                  *wpts++ = (WT_Integer32)((*inpts++ - m_offsetY) * m_scale);
    case 1:       *wpts++ = (WT_Integer32)((*inpts++ - m_offsetX) * m_scale);
                  *wpts++ = (WT_Integer32)((*inpts++ - m_offsetY) * m_scale);
            } while (--n > 0);
    }
    */

    //non-unrolled loop
    for (int i=0; i<numpts; i++)
    {
        *wpts++ = (WT_Integer32)((plb->x_coord(i) - m_offsetX) * m_scale);
        *wpts++ = (WT_Integer32)((plb->y_coord(i) - m_offsetY) * m_scale);
    }
}


void DWFRenderer::_TransformContourPointsNoClamp(LineBuffer* plb, int cntr)
{
    EnsureBufferSize(plb->cntr_size(cntr));
    WT_Integer32* wpts = (WT_Integer32*)m_wtPointBuffer;

    //non-unrolled loop
    int end = plb->contour_end_point(cntr);
    for (int i=plb->contour_start_point(cntr); i<=end; i++)
    {
        *wpts++ = (WT_Integer32)((plb->x_coord(i) - m_offsetX) * m_scale);
        *wpts++ = (WT_Integer32)((plb->y_coord(i) - m_offsetY) * m_scale);
    }
}


void DWFRenderer::_TransformPoints(LineBuffer* plb, const SE_Matrix* xform)
{
    int numpts = plb->point_count();
    EnsureBufferSize(numpts);
    WT_Integer32* wpts = (WT_Integer32*)m_wtPointBuffer;

    if (!xform)
    {
        for (int i=0; i<numpts; i++)
        {
            *wpts++ = (WT_Integer32)plb->x_coord(i);
            *wpts++ = (WT_Integer32)plb->y_coord(i);
        }
    }
    else
    {
        for (int i=0; i<numpts; i++)
        {
            double x, y;
            xform->transform(plb->x_coord(i), plb->y_coord(i), x, y);
            *wpts++ = (WT_Integer32)x;
            *wpts++ = (WT_Integer32)y;
        }
    }
}


void DWFRenderer::_TransformContourPoints(LineBuffer* plb, int cntr, const SE_Matrix* xform)
{
    EnsureBufferSize(plb->cntr_size(cntr));
    WT_Integer32* wpts = (WT_Integer32*)m_wtPointBuffer;

    if (!xform)
    {
        int end = plb->contour_end_point(cntr);
        for (int i=plb->contour_start_point(cntr); i<=end; i++)
        {
            *wpts++ = (WT_Integer32)plb->x_coord(i);
            *wpts++ = (WT_Integer32)plb->y_coord(i);
        }
    }
    else
    {
        int end = plb->contour_end_point(cntr);
        for (int i=plb->contour_start_point(cntr); i<=end; i++)
        {
            double x, y;
            xform->transform(plb->x_coord(i), plb->y_coord(i), x, y);
            *wpts++ = (WT_Integer32)x;
            *wpts++ = (WT_Integer32)y;
        }
    }
}


//-----------------------------------------------------------------------------
//
// Writes multi-polyline geometry to W2D. Input is a LineBuffer
// possibly containing Move segments, in mapping space units
//
//-----------------------------------------------------------------------------
void DWFRenderer::WritePolylines(LineBuffer* srclb)
{
    m_w2dFile->desired_rendition().fill() = false;

    //save to dwf polylines
    for (int i=0; i<srclb->cntr_count(); i++)
    {
        int cntr_size = srclb->cntr_size(i);
        if (cntr_size > 0)
        {
            _TransformContourPointsNoClamp(srclb, i);
            WT_Polyline polyline(cntr_size, m_wtPointBuffer, false);
            polyline.serialize(*m_w2dFile);
            IncrementDrawableCount();
        }
    }
}


//-----------------------------------------------------------------------------
//
// Serializes a line stroke
//
//-----------------------------------------------------------------------------
void DWFRenderer::WriteStroke(RS_LineStroke & stroke)
{
    m_w2dFile->desired_rendition().color() = Util_ConvertColor(stroke.color());

    //convert thickness to W2D units
    double thickness = stroke.width();
    int line_weight = (int)(_MeterToMapSize(stroke.units(), fabs(thickness)) * m_scale);

    m_w2dFile->desired_rendition().line_weight() = WT_Line_Weight(line_weight);
}


//-----------------------------------------------------------------------------
//
// Serializes a polygon fill style
//
//-----------------------------------------------------------------------------
void DWFRenderer::WriteFill(RS_FillStyle& fill)
{
    m_w2dFile->desired_rendition().fill() = true;
    m_w2dFile->desired_rendition().color() = Util_ConvertColor(fill.color());
    m_w2dFile->desired_rendition().contrast_color() = Util_ConvertColor(fill.background()).rgba();

    if (fill.pattern() == L"Solid")
    {
        //set to solid pattern
        m_w2dFile->desired_rendition().user_fill_pattern() = WT_User_Fill_Pattern(-1);
        m_w2dFile->desired_rendition().user_hatch_pattern() = WT_User_Hatch_Pattern(-1);
    }
    else
    {
        if (!m_hatchFac)
            m_hatchFac = new EMapHatchPatternFactory;

        if (!m_fillFac)
            m_fillFac = new EMapFillPatternFactory;

        EMapHatchPatternFactory::Enum hvalenum;
        hvalenum = m_hatchFac->find_index(fill.pattern().c_str());
        if (hvalenum != EMapHatchPatternFactory::nonexistent)
        {
            WT_User_Hatch_Pattern hatchpattern(0);
            m_hatchFac->request_pattern(hvalenum, hatchpattern);
            m_w2dFile->desired_rendition().user_hatch_pattern() = hatchpattern;
        }
        else
        {
            EMapFillPatternFactory::Enum fvalenum;
            fvalenum = m_fillFac->find_index(fill.pattern().c_str());
            if (fvalenum != EMapFillPatternFactory::nonexistent)
            {
                WT_User_Fill_Pattern fillpattern(0);
                m_fillFac->request_pattern(fvalenum, fillpattern);
                m_w2dFile->desired_rendition().user_fill_pattern() = fillpattern;
            }
            else
            {
                m_w2dFile->desired_rendition().user_fill_pattern() = WT_User_Fill_Pattern(-1);
                m_w2dFile->desired_rendition().user_hatch_pattern() = WT_User_Hatch_Pattern(-1);
            }
        }
    }
}


//-----------------------------------------------------------------------------
//
// Serializes font and text alignment and rendering settings
//
//-----------------------------------------------------------------------------
void DWFRenderer::WriteTextDef(WT_File* file, RS_TextDef& tdef)
{
    //determine DWF space font height
    double hgt = _MeterToW2DMacroUnit(tdef.font().units(), tdef.font().height());
    //double hgt = _MeterToMapSize(tdef.font().units(), tdef.font().height());
    //hgt *= m_scale;

    file->desired_rendition().font().font_name() = Util_ConvertString(tdef.font().name().c_str());
    file->desired_rendition().font().style().set_bold((tdef.font().style() & RS_FontStyle_Bold) != 0);
    file->desired_rendition().font().style().set_italic((tdef.font().style() & RS_FontStyle_Italic) != 0);
    file->desired_rendition().font().style().set_underlined((tdef.font().style() & RS_FontStyle_Underline) != 0);
    file->desired_rendition().font().height() = (WT_Integer32)hgt;
    file->desired_rendition().font().rotation() = (WT_Unsigned_Integer16)(tdef.rotation() / 360.0 * 65536.0);

    file->desired_rendition().color() = Util_ConvertColor(tdef.textcolor());

    //text alignment
    switch (tdef.halign())
    {
    case RS_HAlignment_Center : file->desired_rendition().text_halign() = WT_Text_HAlign(WT_Text_HAlign::Center);
        break;
    case RS_HAlignment_Left   : file->desired_rendition().text_halign() = WT_Text_HAlign(WT_Text_HAlign::Left);
        break;
    case RS_HAlignment_Right  : file->desired_rendition().text_halign() = WT_Text_HAlign(WT_Text_HAlign::Right);
        break;
    default                   : file->desired_rendition().text_halign() = WT_Text_HAlign(WT_Text_HAlign::Center);
        break;
    }

    switch (tdef.valign())
    {
    case RS_VAlignment_Descent : file->desired_rendition().text_valign() = WT_Text_VAlign(WT_Text_VAlign::Descentline);
        break;
    case RS_VAlignment_Base    : file->desired_rendition().text_valign() = WT_Text_VAlign(WT_Text_VAlign::Baseline);
        break;
    case RS_VAlignment_Half    : file->desired_rendition().text_valign() = WT_Text_VAlign(WT_Text_VAlign::Halfline);
        break;
    case RS_VAlignment_Cap     : file->desired_rendition().text_valign() = WT_Text_VAlign(WT_Text_VAlign::Capline);
        break;
    case RS_VAlignment_Ascent  : file->desired_rendition().text_valign() = WT_Text_VAlign(WT_Text_VAlign::Ascentline);
        break;
    default                    : file->desired_rendition().text_valign() = WT_Text_VAlign(WT_Text_VAlign::Descentline);
        break;
    }

    //text background style
    if ((tdef.textbg() & RS_TextBackground_Ghosted) != 0)
    {
        //ghosting offset : 1 pixel
        double metersPerPixel = METERS_PER_INCH / m_dpi;
        int offset = (int)_MeterToW2DMacroUnit(tdef.font().units(), metersPerPixel);
        file->desired_rendition().text_background() = WT_Text_Background(WT_Text_Background::Ghosted, offset);
        file->desired_rendition().contrast_color() = Util_ConvertColor(tdef.ghostcolor()).rgba();
    }
    else if ((tdef.textbg() & RS_TextBackground_Opaque) != 0)
    {
        file->desired_rendition().text_background() = WT_Text_Background(WT_Text_Background::Solid, 0);
        file->desired_rendition().contrast_color() = Util_ConvertColor(tdef.opaquecolor()).rgba();
    }
    else
    {
        file->desired_rendition().text_background() = WT_Text_Background(WT_Text_Background::None, 0);
        file->desired_rendition().contrast_color() = WT_Contrast_Color(0, 0, 0, 255);
    }
}


// Scales an input length in meters in the specified units - device or
// mapping - to a length in mapping space.
double DWFRenderer::_MeterToMapSize(RS_Units unit, double number)
{
    double scale_factor;
    if (unit == RS_Units_Device) // in meters, fixed size
        scale_factor = m_mapScale / m_metersPerUnit;
    else
        scale_factor = 1.0 / m_metersPerUnit;

    return number * scale_factor;
}


//-----------------------------------------------------------------------------
//scale an input number in meters to a mapping
//space number given a device or mapping space unit.
//This is similar to _MeterToMapSize, but for use with macro opcode
//i.e. it scales device space numbers to W2D units using the Macro
//factor of 4096 units / inch
//-----------------------------------------------------------------------------
double DWFRenderer::_MeterToW2DMacroUnit(RS_Units unit, double number)
{
    double scale_factor;
    if (unit == RS_Units_Device) // in meters, fixed size
    {
        double w2dUnitsPerInch = 4096.0;
        scale_factor = 1.0 / METERS_PER_INCH * w2dUnitsPerInch;
    }
    else
        scale_factor = m_scale / m_metersPerUnit;

    return number * scale_factor;
}


//-----------------------------------------------------------------------------
//
// Helper function to open a W2D file with good initial settings
//
//-----------------------------------------------------------------------------
void DWFRenderer::OpenW2D(WT_File* file)
{
    file->set_file_mode(WT_File::/*WT_File_Mode::*/File_Write);

    file->heuristics().set_allow_binary_data(true);
    file->heuristics().set_allow_drawable_merging(false);

    //no need to compress since we will put the w2d file in a package
    //m_w2dFile->heuristics().set_allow_data_compression(true);
    //m_w2dFile->heuristics().set_allow_indexed_colors(false);

    //TODO: what to do?
    if (file->open() != WT_Result::Success)
    {
        _DWFCORE_THROW(DWFIOException, L"Failed to open W2D file");
    }

    //set transformations...
    WT_Matrix m;
    m.set_identity();

    m(0,0) = m_scale;
    m(1,1) = m_scale;
    m(3,0) = -m_offsetX*m_scale;
    m(3,1) = -m_offsetY*m_scale;

    file->desired_rendition().visibility() = true;
    file->desired_rendition().drawing_info().units().set_application_to_dwf_transform(m);
    file->desired_rendition().drawing_info().units().serialize(*file);
}


//-----------------------------------------------------------------------------
//
// Helper function to start a new W2D object node and write out the attributes
// of the current feature into a DWF object definition instance
//
//-----------------------------------------------------------------------------
void DWFRenderer::StoreAttributes(RS_FeatureReader* feature, const RS_String* tooltip, const RS_String* url)
{
    if (m_featureClass && m_featureClassInfo)
    {
        //set hyperlink onto the rendition
        if (url)
        {
            WT_String wtstr = Util_ConvertString(url->c_str());
            WT_URL wturl;
            wturl.add_url_optimized(0, wtstr, wtstr, *m_w2dFile);

            m_w2dFile->desired_rendition().url() = wturl;
        }

        //object selection and attributes support

        //generate unique feature key
        const char* base64 = m_keyEncode.EncodeKey(feature);

        //skip writing of feature data if there is no id
        if (base64 != 0 && *base64 != 0)
        {
            DWFString sid(base64);

            //check if a piece of that feature has already been written
            //by looking up the id in the node table
            unsigned int* node = ((NodeTable*)m_hObjNodes)->find(sid);
            if (node)
                m_w2dFile->desired_rendition().object_node() = WT_Object_Node(*m_w2dFile, *node);
            else
                m_w2dFile->desired_rendition().object_node() = WT_Object_Node(*m_w2dFile, m_nObjectId);

            //we need to sync the rendition manually
            //since we may be defining/playing macros by writing
            //to the file directly
            WT_Integer32 parts_to_sync = WT_Rendition::Object_Node_Bit | WT_Rendition::URL_Bit;
            m_w2dFile->desired_rendition().sync(*m_w2dFile, parts_to_sync);

            //if the object node already exists in the table,
            //we do not need to create a new object instance since
            //we have one already
            if (!node)
            {
                wchar_t id[16];
                swprintf(id, 16, L"%d", m_nObjectId);

                //do not use m_featureClass->instance() here
                //we prefer creating the instance manually since m_featureClass will
                //build up a huge list of all its instances internally
                DWFDefinedObjectInstance* oi = DWFCORE_ALLOC_OBJECT(DWFDefinedObjectInstance(*m_featureClass, id));

                //there are two strings in the list, corresponding to each property
                //the first is the actual name of the property and the second is
                //the display name of the property. Use the actual name to retrieve the
                //value and the display name for the name of the DWF property we store
                for (unsigned i=0; i<m_featureClassInfo->mappings().size()/2; i++)
                {
                    oi->addProperty(
                        DWFCORE_ALLOC_OBJECT(DWFProperty(
                                        m_featureClassInfo->mappings()[2*i+1].c_str(),
                                        feature->GetAsString(m_featureClassInfo->mappings()[2*i].c_str()),
                                        L""))
                        , true);
                }

                if (tooltip)
                {
                    oi->addProperty(
                        DWFCORE_ALLOC_OBJECT(DWFProperty(
                                        L"_#ToolTip", //this is what the DWF EMap Viewer expects
                                        tooltip->c_str(),
                                        L""))
                        , true);
                }

                m_attributes->addInstance(oi);

                oi->identify(sid);

                //remember the object node associated with this feature
                ((NodeTable*)m_hObjNodes)->insert(oi->id(), m_nObjectId);

                m_nObjectId++;
            }
        }
    }
}


//-----------------------------------------------------------------------------
//
// Helper function to convert from a MapGuide pattern to a WHIP toolkit
// dash pattern.  Note that this cannot handle decorated patterns --> these
// are special cased in WT_Line_Pattern.
//
//-----------------------------------------------------------------------------
int DWFRenderer::ConvertToDashPattern(const wchar_t* lineStyleName,
                                      double dpi,
                                      double lineWeightPixels,
                                      WT_Dash_Pattern& dash,
                                      WT_Line_Pattern& lpat)
{
    // first detect decorated line patterns - these are hardcoded/defined
    // as WHIP WT_Line_Patterns
    if (_wcsnicmp(lineStyleName, L"FENCELINE1", 11) == 0)
    {
        lpat.set(WT_Line_Pattern::Decorated_Circle_Fence);
        return WT_Line_Pattern::Decorated_Circle_Fence;
    }
    else if (_wcsnicmp(lineStyleName, L"FENCELINE2", 11) == 0)
    {
        lpat.set(WT_Line_Pattern::Decorated_Square_Fence);
        return WT_Line_Pattern::Decorated_Square_Fence;
    }
    else if (_wcsnicmp(lineStyleName, L"TRACKS", 7) == 0)
    {
        lpat.set(WT_Line_Pattern::Decorated_Wide_Tracks);
        return WT_Line_Pattern::Decorated_Wide_Tracks;
    }
    else if (_wcsnicmp(lineStyleName, L"Rail", 5) == 0)
    {
        lpat.set(WT_Line_Pattern::Decorated_Tracks);
        return WT_Line_Pattern::Decorated_Tracks;
    }

    // for all other patterns we will use a custom defined WT_Dash_Pattern
    LineStyleDef lineStyleDef;
    LineStyle lineStyle = lineStyleDef.FindLineStyle(lineStyleName);
    lineStyleDef.SetStyle(lineStyle, 1.0, dpi, lineWeightPixels);

    // convert to WHIP values for the on/off pixels for the custom dash pattern
    // note that WHIP requires an even number of pixel runs
    WT_Integer32 id = lineStyle + WT_Line_Pattern::Count + 1;

    WT_Integer16 numRuns = (WT_Integer16)lineStyleDef.m_nRuns;
    if (numRuns % 2)
        ++numRuns;

    WT_Integer16* runs = (WT_Integer16*)alloca(sizeof(WT_Integer16) * numRuns);
    memset(runs, 0, sizeof(WT_Integer16) * numRuns);

    for (int i=0; i<lineStyleDef.m_nRuns; i++)
        runs[i] = (WT_Integer16)lineStyleDef.m_pixelRuns[i].m_nPixels;

    dash.set(id, numRuns, runs);

    return id;
}


//----------------------------------------------------------------------------
//
// Used by ProcessPolygon and DrawScreenPolygon to ensure that contours in
// contour sets are oriented according to the DWF spec.  The spec requires
// that filled areas be specified by clockwise wound points, and holes be
// specified by counter-clockwise wound points.  So in our context the first
// contour needs to be CW, and the remaining ones CCW.  The implementation
// assumes simple contours (no self-intersection).
//
//----------------------------------------------------------------------------
void DWFRenderer::OrientContours(int numContours, int* contourCounts, WT_Logical_Point* wtPointBuffer)
{
    if (numContours == 1)
        return;

    int start_pt = 0;
    int end_pt = 0;

    // iterate over the contours
    for (int j=0; j<numContours; ++j)
    {
        // get point range for current contour
        start_pt = end_pt;
        end_pt += contourCounts[j];

        if (contourCounts[j] < 2)
            continue;

        // compute area of current contour
        double x0;
        double y0;
        double x1 = wtPointBuffer[end_pt-1].m_x;
        double y1 = wtPointBuffer[end_pt-1].m_y;
        double area2 = 0.0;
        for (int i=start_pt; i<end_pt; ++i)
        {
            x0 = x1;
            y0 = y1;
            x1 = wtPointBuffer[i].m_x;
            y1 = wtPointBuffer[i].m_y;
            area2 += x0*y1 - y0*x1;
        }

        // the first contour (outer ring) needs to be CW (area < 0), and the
        // remaining ones CCW (area > 0)
        bool flipContour = false;
        if (j == 0)
        {
            if (area2 > 0.0)
                flipContour = true;
        }
        else if (area2 < 0.0)
            flipContour = true;

        if (flipContour)
        {
            int halfCount = contourCounts[j] / 2;
            for (int i=0; i<halfCount; ++i)
            {
                int pt0 = start_pt + i;
                int pt1 = end_pt - i - 1;
                WT_Integer32 xTmp = wtPointBuffer[pt0].m_x;
                WT_Integer32 yTmp = wtPointBuffer[pt0].m_y;
                wtPointBuffer[pt0].m_x = wtPointBuffer[pt1].m_x;
                wtPointBuffer[pt0].m_y = wtPointBuffer[pt1].m_y;
                wtPointBuffer[pt1].m_x = xTmp;
                wtPointBuffer[pt1].m_y = yTmp;
            }
        }
    }
}


void DWFRenderer::SetSymbolManager(RS_SymbolManager* manager)
{
    m_symbolManager = manager;
}


RS_MapUIInfo* DWFRenderer::GetMapInfo()
{
    return m_mapInfo;
}


RS_LayerUIInfo* DWFRenderer::GetLayerInfo()
{
    return m_layerInfo;
}


RS_FeatureClassInfo* DWFRenderer::GetFeatureClassInfo()
{
    return m_featureClassInfo;
}


double DWFRenderer::GetMapScale()
{
    return m_mapScale;
}


double DWFRenderer::GetDrawingScale()
{
    return m_drawingScale;
}


double DWFRenderer::GetMetersPerUnit()
{
    return m_metersPerUnit;
}


double DWFRenderer::GetDpi()
{
    return m_dpi;
}


RS_Bounds& DWFRenderer::GetBounds()
{
    return m_extents;
}


bool DWFRenderer::RequiresClipping()
{
    return true;
}


bool DWFRenderer::RequiresLabelClipping()
{
    return true;
}


bool DWFRenderer::SupportsZ()
{
    // Z values in feature geometry are ignored
    return false;
}


double DWFRenderer::GetMapToW2DScale()
{
    return m_scale;
}


void DWFRenderer::EnsureBufferSize(int len)
{
    if (!m_wtPointBuffer)
    {
        m_wtPointLen = len;
        m_wtPointBuffer = new WT_Logical_Point[m_wtPointLen];
    }
    else if (len > m_wtPointLen)
    {
        delete [] m_wtPointBuffer;
        m_wtPointLen = len;
        m_wtPointBuffer = new WT_Logical_Point[m_wtPointLen];
    }
}


void DWFRenderer::StartLayout(RS_Bounds& extents)
{
    // Reset m_mapScale and m_metersPerUnit
    Init(extents);

    //scale used to convert to DWF logical coordinates in the range [0, 2^30-1]
    m_scale = (double)(INT_MAX/2) / rs_max(m_extents.width(), m_extents.height());

    m_w2dFile = new WT_File();
    m_w2dLabels = new WT_File();

    //overload WHIP I/O functions with our memory streaming ones
    m_w2dFile->set_stream_open_action (my_open);
    m_w2dFile->set_stream_close_action(my_close);
    m_w2dFile->set_stream_read_action (my_read);
    m_w2dFile->set_stream_seek_action (my_seek);
    m_w2dFile->set_stream_write_action(my_write);
    m_w2dFile->set_stream_user_data(WD_Null);

    //overload WHIP I/O functions with our memory streaming ones
    m_w2dLabels->set_stream_open_action (my_open);
    m_w2dLabels->set_stream_close_action(my_close);
    m_w2dLabels->set_stream_read_action (my_read);
    m_w2dLabels->set_stream_seek_action (my_seek);
    m_w2dLabels->set_stream_write_action(my_write);
    m_w2dLabels->set_stream_user_data(WD_Null);

    m_bHaveLabels = false;

    OpenW2D(m_w2dFile);

    m_imgID = 0;
    m_drawableCount = 0;
    m_labelMacroCount = 0;
}


void DWFRenderer::EndLayout()
{
    m_extents = m_mapExtents;

    //keep track of memory output streams so that we can free them
    //when the map is done
    m_lLayoutStreams.push_back((DWFBufferOutputStream*)m_w2dFile->stream_user_data());

    m_w2dFile->close();
    delete m_w2dFile;
    m_w2dFile = NULL;

    if (m_bHaveLabels && m_labelMacroCount > 0)
    {
        //Only need to keep track of label w2d if the stream was opened,
        //which happens when there is at least one label.  The check for
        //macros ensures that the w2d is not empty, since the viewer
        //doesn't like it otherwise (i.e. it crashes).
        m_lLayoutLabelStreams.push_back((DWFBufferOutputStream*)m_w2dLabels->stream_user_data());
    }
    else
        m_lLayoutLabelStreams.push_back(NULL); //add NULL for no labels

    m_w2dLabels->close();
    delete m_w2dLabels;
    m_w2dLabels = NULL;
    m_bHaveLabels = false;
}


void DWFRenderer::Init(RS_Bounds& extents)
{
    m_mapExtents = m_extents;

    m_metersPerUnit = METERS_PER_INCH;
    m_mapScale = 1.0;
    m_extents = extents;

    double metersPerPixel = METERS_PER_INCH / m_dpi;

    // drawing scale is map scale converted to [mapping units] / [pixels]
    m_drawingScale = m_mapScale * metersPerPixel / m_metersPerUnit;

    // scale used to convert to DWF logical coordinates in the range [0, 2^31-1]
    m_scale = (double)(INT_MAX - 256) / rs_max(extents.width(), extents.height());

    m_offsetX = 0;
    m_offsetY = 0;
}


//-----------------------------------------------------------------------------
//
//             SE Renderer
//
//-----------------------------------------------------------------------------


void DWFRenderer::DrawScreenPolyline(LineBuffer* geom, const SE_Matrix* xform, const SE_LineStroke& lineStroke)
{
    if (lineStroke.color == 0)
        return;

    // draw to the active file if it's set
    WT_File* file = m_w2dActive? m_w2dActive : m_w2dFile;

    file->desired_rendition().fill() = false;
    file->desired_rendition().color() = Util_ConvertColor(lineStroke.color);

    // the supplied weight is already in W2D units
    int line_weight = (int)lineStroke.weight;
    file->desired_rendition().line_weight() = WT_Line_Weight(line_weight);

    // for now always draw solid lines
    file->desired_rendition().line_pattern() = WT_Line_Pattern(WT_Line_Pattern::Solid);

    // set up cap / join style
    WT_Line_Style::WT_Capstyle_ID capStyle;
    switch (lineStroke.cap)
    {
        case SE_LineCap_None:
            capStyle = WT_Line_Style::Butt_Cap;
            break;
        case SE_LineCap_Triangle:
            capStyle = WT_Line_Style::Diamond_Cap;
            break;
        case SE_LineCap_Square:
            capStyle = WT_Line_Style::Square_Cap;
            break;
        case SE_LineCap_Round:
        default:
            capStyle = WT_Line_Style::Round_Cap;
            break;
    }

    WT_Line_Style::WT_Joinstyle_ID joinStyle;
    switch (lineStroke.join)
    {
        case SE_LineJoin_None:
            joinStyle = WT_Line_Style::Bevel_Join;  // Whip doesn't support None
            break;
        case SE_LineJoin_Bevel:
            joinStyle = WT_Line_Style::Bevel_Join;
            break;
        case SE_LineJoin_Miter:
            joinStyle = WT_Line_Style::Miter_Join;
            break;
        case SE_LineJoin_Round:
        default:
            joinStyle = WT_Line_Style::Round_Join;
            break;
    }

    WT_Line_Style style;
    style.dash_start_cap() = capStyle;
    style.dash_end_cap()   = capStyle;
    style.line_start_cap() = capStyle;
    style.line_end_cap()   = capStyle;
    style.line_join()      = joinStyle;
    style.adapt_patterns() = false;

    // Allow miters for all interior angles, but set the miter length
    // limit to the specified value.  Whip only allows integer values
    // for the miter length limit...
    // NOTE: Heidi does not reliably handle miter joins when the actual miter
    //       length is close to the limit (there are screen artifacts).  The
    //       artifacts become quite pronounced for small miter length limits.
    //       We therefore don't allow miter length limits less than 1.
    style.miter_angle() = 0;
    style.miter_length() = (WT_Unsigned_Integer16)(lineStroke.miterLimit + 0.5);

    m_w2dFile->desired_rendition().line_style() = style;

    // save to dwf polylines
    for (int i=0; i<geom->cntr_count(); i++)
    {
        int cntr_size = geom->cntr_size(i);
        if (cntr_size > 0)
        {
            _TransformContourPoints(geom, i, xform);
            WT_Polyline polyline(cntr_size, m_wtPointBuffer, false);
            polyline.serialize(*file);
            IncrementDrawableCount();
        }
    }

    // zero out the dash pattern -- must do when done with it
//  if (file->desired_rendition().dash_pattern() != WT_Dash_Pattern::kNull)
//      file->desired_rendition().dash_pattern() = WT_Dash_Pattern::kNull;
}


void DWFRenderer::DrawScreenPolygon(LineBuffer* geom, const SE_Matrix* xform, unsigned int fill)
{
    if (fill == 0)
        return;

    if (geom->point_count() == 0)
        return;

    // draw to the active file if it's set
    WT_File* file = m_w2dActive? m_w2dActive : m_w2dFile;

    file->desired_rendition().fill() = true;
    file->desired_rendition().color() = Util_ConvertColor(fill);
//  file->desired_rendition().contrast_color() = Util_ConvertColor(fill.background()).rgba();

    // for now always draw solid fills
    file->desired_rendition().user_fill_pattern() = WT_User_Fill_Pattern(-1);
    file->desired_rendition().user_hatch_pattern() = WT_User_Hatch_Pattern(-1);

    // save to dwf polygons
    _TransformPoints(geom, xform);

    if (geom->cntr_count() == 1)
    {
        // just a polygon, no need for a contour set
        WT_Polygon polygon(geom->point_count(), m_wtPointBuffer, false);
        polygon.serialize(*file);
        IncrementDrawableCount();
    }
    else
    {
        // otherwise make a contour set

        // ensure the contours are oriented per the DWF spec
        OrientContours(geom->cntr_count(), geom->cntrs(), m_wtPointBuffer);

        // now create the contour set
        WT_Contour_Set cset(*file, geom->cntr_count(), (WT_Integer32*)geom->cntrs(), geom->point_count(), m_wtPointBuffer, true);
        cset.serialize(*file);
        IncrementDrawableCount();
    }

    // zero out the dash pattern -- must do when done with it
//  if (file->desired_rendition().dash_pattern() != WT_Dash_Pattern::kNull)
//      file->desired_rendition().dash_pattern() = WT_Dash_Pattern::kNull;
}


void DWFRenderer::DrawScreenRaster(unsigned char* data,
                                   int            length,
                                   RS_ImageFormat format,
                                   int            native_width,
                                   int            native_height,
                                   double         x,
                                   double         y,
                                   double         w,
                                   double         h,
                                   double         angleDeg)
{
    if (format != RS_ImageFormat_RGB  &&
        format != RS_ImageFormat_ABGR &&
        format != RS_ImageFormat_PNG)
    {
        //TODO: support other formats...
        _ASSERT(false);
        return;
    }

    // draw to the active file if it's set
    WT_File* file = m_w2dActive? m_w2dActive : m_w2dFile;

    // find the corners of the image
    SE_Matrix xform;
    xform.rotate(angleDeg * M_PI180);
    xform.translate(x, y);

    double ptsx[4], ptsy[4];
    xform.transform(-0.5*w, -0.5*h, ptsx[0], ptsy[0]);
    xform.transform( 0.5*w, -0.5*h, ptsx[1], ptsy[1]);
    xform.transform( 0.5*w,  0.5*h, ptsx[2], ptsy[2]);
    xform.transform(-0.5*w,  0.5*h, ptsx[3], ptsy[3]);

    WT_Logical_Point dstpts[4];
    for (int i=0; i<4; i++)
    {
        dstpts[i].m_x = (WT_Integer32)ptsx[i];
        dstpts[i].m_y = (WT_Integer32)ptsy[i];
    }

    if (ROUND(angleDeg) == 0)
    {
        // simple case of no rotation
        if (format == RS_ImageFormat_RGB || format == RS_ImageFormat_ABGR)
        {
            WT_Image img((WT_Unsigned_Integer16)native_height,
                         (WT_Unsigned_Integer16)native_width,
                         (format == RS_ImageFormat_RGB)? WT_Image::RGB : WT_Image::RGBA,
                         m_imgID++,
                         NULL,
                         length,
                         data,
                         dstpts[0],
                         dstpts[2],
                         false);

            img.serialize(*file);
            IncrementDrawableCount();
        }
        else if (format == RS_ImageFormat_PNG)
        {
            WT_PNG_Group4_Image img((WT_Unsigned_Integer16)native_height,
                         (WT_Unsigned_Integer16)native_width,
                         WT_PNG_Group4_Image::PNG,
                         m_imgID++,
                         NULL,
                         length,
                         data,
                         dstpts[0],
                         dstpts[2],
                         false);

            img.serialize(*file);
            IncrementDrawableCount();
        }
    }
    else
    {
        // rotated case
        if (format == RS_ImageFormat_RGB || format == RS_ImageFormat_ABGR)
        {
            RotateRGBAImage((WT_Unsigned_Integer16)native_height,
                            (WT_Unsigned_Integer16)native_width,
                            (format == RS_ImageFormat_RGB)? WT_Image::RGB : WT_Image::RGBA,
                            NULL,
                            data,
                            m_imgID++,
                            dstpts,
                            *file);
            IncrementDrawableCount();
        }
        else if (format == RS_ImageFormat_PNG)
        {
            // compute the scaled size
            double pixelsPerW2D = 1.0 / GetScreenUnitsPerPixel();
            double scaledW = w * pixelsPerW2D;
            double scaledH = h * pixelsPerW2D;

            // don't allow images beyond the maximum size
            if (scaledW * scaledH > IMAGE_SIZE_MAX)
                return;

            // compute the rotated size
            double minx = rs_min(ptsx[0], rs_min(ptsx[1], rs_min(ptsx[2], ptsx[3])));
            double maxx = rs_max(ptsx[0], rs_max(ptsx[1], rs_max(ptsx[2], ptsx[3])));
            double miny = rs_min(ptsy[0], rs_min(ptsy[1], rs_min(ptsy[2], ptsy[3])));
            double maxy = rs_max(ptsy[0], rs_max(ptsy[1], rs_max(ptsy[2], ptsy[3])));
            double rotatedW = (maxx - minx) * pixelsPerW2D;
            double rotatedH = (maxy - miny) * pixelsPerW2D;

            // don't allow images beyond the maximum size
            if (rotatedW * rotatedH > IMAGE_SIZE_MAX)
                return;

            // load the source image
            gdImagePtr src = gdImageCreateFromPngPtr(length, data);

            // scale it if necessary
            if ((int)scaledW != gdImageSX(src) || (int)scaledH != gdImageSY(src))
            {
                gdImagePtr dst = gdImageCreateTrueColor((int)scaledW, (int)scaledH);
                gdImageAlphaBlending(dst, 0);
                gdImageFilledRectangle(dst, 0, 0, gdImageSX(dst)-1, gdImageSY(dst)-1, 0x7f000000);
                gdImageAlphaBlending(dst, 1);
                gdImageCopyResampled(dst, src, 0, 0, 0, 0, gdImageSX(dst), gdImageSY(dst), gdImageSX(src), gdImageSY(src));
                gdImageDestroy(src);
                src = dst;
            }

            // rotate the image
            gdImagePtr dst = gdImageCreateTrueColor((int)rotatedW, (int)rotatedH);
            gdImageAlphaBlending(dst, 0);
            gdImageFilledRectangle(dst, 0, 0, gdImageSX(dst)-1, gdImageSY(dst)-1, 0x7f000000);
            gdImageAlphaBlending(dst, 1);
            gdImageCopyRotated(dst, src, 0.5*rotatedW, 0.5*rotatedH, 0, 0, gdImageSX(src), gdImageSY(src), ROUND(angleDeg));
            gdImageSaveAlpha(dst, 1);

            // create the DWF image from the PNG data
            int pngSize = 0;
            unsigned char* pngData = (unsigned char*)gdImagePngPtr(dst, &pngSize);

            WT_PNG_Group4_Image img((WT_Unsigned_Integer16)gdImageSY(dst),
                                    (WT_Unsigned_Integer16)gdImageSX(dst),
                                    WT_PNG_Group4_Image::PNG,
                                    m_imgID++,
                                    NULL,
                                    pngSize,
                                    pngData,
                                    WT_Logical_Point((WT_Integer32)minx, (WT_Integer32)miny),
                                    WT_Logical_Point((WT_Integer32)maxx, (WT_Integer32)maxy),
                                    false);
            img.serialize(*file);
            IncrementDrawableCount();

            gdFree(pngData);
            gdImageDestroy(dst);
            gdImageDestroy(src);
        }
    }
}


void DWFRenderer::DrawScreenRaster(unsigned char* data, int length,
                                   RS_ImageFormat format, int native_width, int native_height,
                                   double x, double y, double w, double h, double angleDeg,
                                   double alpha)
{
    RenderUtil::DrawScreenRaster(this, data, length, format, native_width, native_height,
                                 x, y, w, h, angleDeg, alpha);
}


void DWFRenderer::DrawScreenText(const RS_TextMetrics&  tm,
                                 RS_TextDef&      tdef,
                                 double           insx,
                                 double           insy,
                                 RS_F_Point*      path,
                                 int              npts,
                                 double           param_position)
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


bool DWFRenderer::YPointsUp()
{
    return true;
}


void DWFRenderer::GetWorldToScreenTransform(SE_Matrix& xform)
{
    // "screen space" in DWF is W2D space
    xform.x0 = m_scale;
    xform.x1 = 0.0;
    xform.x2 = - m_offsetX * m_scale;
    xform.y0 = 0.0;
    xform.y1 = m_scale;
    xform.y2 = - m_offsetY * m_scale;
}


void DWFRenderer::WorldToScreenPoint(double& inx, double& iny, double& ox, double& oy)
{
    // "screen space" in DWF is W2D space
    ox = (inx - m_offsetX) * m_scale;
    oy = (iny - m_offsetY) * m_scale;
}


void DWFRenderer::ScreenToWorldPoint(double& inx, double& iny, double& ox, double& oy)
{
    // "screen space" in DWF is W2D space
    ox = inx / m_scale + m_offsetX;
    oy = iny / m_scale + m_offsetY;
}


// returns number of W2D units per millimeter device
double DWFRenderer::GetScreenUnitsPerMillimeterDevice()
{
    //  [ W2D ]     [ World ]     meter-world     mm-world      [ W2D ]
    // --------- * ----------- * ------------- * ---------  =  ---------
    // [ World ]   meter-world   1000 mm-world   mm-screen     mm-screen
    return m_scale / m_metersPerUnit / 1000.0 * m_mapScale;
}


// returns number of W2D units per millimeter world
double DWFRenderer::GetScreenUnitsPerMillimeterWorld()
{
    //  [ W2D ]     [ World ]     meter-world       [ W2D ]
    // --------- * ----------- * -------------  =  --------
    // [ World ]   meter-world   1000 mm-world     mm-world
    return m_scale / m_metersPerUnit / 1000.0;
}


// returns number of W2D units per physical pixel
double DWFRenderer::GetScreenUnitsPerPixel()
{
    // Start with:
    //     GetScreenUnitsPerMillimeterDevice() * MILLIMETERS_PER_INCH / m_dpi
    // and simplify it to get:
    return m_scale * m_drawingScale;
}


RS_FontEngine* DWFRenderer::GetRSFontEngine()
{
    return this;
}


void DWFRenderer::ProcessSELabelGroup(SE_LabelInfo*   labels,
                                      int             nlabels,
                                      RS_OverpostType type,
                                      bool            exclude,
                                      LineBuffer*     /*path*/)
{
    if (nlabels == 0)
        return;

    WT_File* file = m_w2dFile;

    //check if it is a text layer or a feature label
    //This is sufficient to check the overpost type
    //to see if it matches that used only for a text layer,
    //but nevertheless this is a hack and there should be a way to indicate
    //a text feature (as opposed to a label)
    if (type != RS_OverpostType_All)
    {
        //open the label W2D if it's not yet open
        if (!m_bHaveLabels)
        {
            OpenW2D(m_w2dLabels);
            m_bHaveLabels = true;
        }

        file = m_w2dLabels;
    }

    // For composite symbolizations we only use mapping-space macros, meaning
    // the symbols will change size as you zoom dynamically.  The symbols
    // will always be redrawn at the correct size after any map update.

    // Choose a decent reference size for computing the macro scale.  A
    // small percentage of the map width is reasonable.
    double refSizeMeters = 0.01 * m_extents.width() * m_metersPerUnit;
    int scale = (int)_MeterToW2DMacroUnit(RS_Units_Model, refSizeMeters);

    // define a macro for each candidate label
    for (int i=0; i<nlabels; i++)
    {
        SE_LabelInfo* info = &labels[i];

        // For the macro definition we only account for the symbol rotation.
        // The translation is applied when we play the macro.
        SE_Matrix m;
        m.rotate(info->anglerad);

        BeginMacro(file, i+1, scale);

            m_w2dActive = file;
            DrawSymbol(info->style->symbol, m, info->anglerad);
            m_w2dActive = NULL;

        EndMacro(file, i+1);
    }

    // now play the label macros inside an overpost
    BeginOverpostGroup(file, type, true, exclude);

        for (int i=0; i<nlabels; i++)
        {
            SE_LabelInfo* info = &labels[i];

            //we need to sync the rendition manually
            //since we are about to play macros by writing
            //to the file directly
            WT_Integer32 parts_to_sync = WT_Rendition::Color_Bit           |
                                     //  WT_Rendition::Color_Map_Bit       |
                                     //  WT_Rendition::Fill_Bit            |
                                     //  WT_Rendition::Fill_Pattern_Bit    |
                                     //  WT_Rendition::Merge_Control_Bit   |
                                     //  WT_Rendition::BlockRef_Bit        |
                                         WT_Rendition::Visibility_Bit      |
                                     //  WT_Rendition::Line_Weight_Bit     |
                                         WT_Rendition::Pen_Pattern_Bit     |
                                     //  WT_Rendition::Line_Pattern_Bit    |
                                     //  WT_Rendition::Line_Caps_Bit       |
                                     //  WT_Rendition::Line_Join_Bit       |
                                     //  WT_Rendition::Marker_Size_Bit     |
                                     //  WT_Rendition::Marker_Symbol_Bit   |
                                     //  WT_Rendition::URL_Bit             |
                                         WT_Rendition::Layer_Bit           |
                                     //  WT_Rendition::Viewport_Bit        |
                                         WT_Rendition::Font_Extension_Bit  |
                                         WT_Rendition::Font_Bit            |
                                     //  WT_Rendition::Object_Node_Bit     |
                                         WT_Rendition::Text_Background_Bit |
                                         WT_Rendition::Text_HAlign_Bit     |
                                         WT_Rendition::Text_VAlign_Bit     |
                                         WT_Rendition::Contrast_Color_Bit;

            file->desired_rendition().sync(*file, parts_to_sync);

            // PlayMacro expects a point in mapping units
            double posx, posy;
            ScreenToWorldPoint(info->x, info->y, posx, posy);

            PlayMacro(file, i+1, refSizeMeters, RS_Units_Model, posx, posy);
        }

    EndOverpostGroup(file);
}


void DWFRenderer::AddExclusionRegion(RS_F_Point* fpts, int npts)
{
    // draw to the active file if it's set
    WT_File* file = m_w2dActive? m_w2dActive : m_w2dFile;

    // For composite symbolizations we only use mapping-space macros, meaning
    // the symbols will change size as you zoom dynamically.  The symbols
    // will always be redrawn at the correct size after any map update.

    // Choose a decent reference scale for the macro.  A small percentage
    // of the map width is reasonable.
    double refSizeMeters = 0.01 * m_extents.width() * m_metersPerUnit;
    int scale = (int)_MeterToW2DMacroUnit(RS_Units_Model, refSizeMeters);

    BeginMacro(file, 0, scale);

        // if there's no rotation, send just 2 points
        if (npts == 4 &&
            fpts[0].y == fpts[1].y &&
            fpts[1].x == fpts[2].x &&
            fpts[2].y == fpts[3].y &&
            fpts[3].x == fpts[0].x)
        {
            WT_Logical_Point axisbox[2];
            axisbox[0].m_x = (WT_Integer32)fpts[0].x;
            axisbox[0].m_y = (WT_Integer32)fpts[0].y;
            axisbox[1].m_x = (WT_Integer32)fpts[2].x;
            axisbox[1].m_y = (WT_Integer32)fpts[2].y;

            WT_Polyline excludearea(2, axisbox, false);
            excludearea.serialize(*file);
            IncrementDrawableCount();
        }
        else
        {
            WT_Logical_Point* dstpts = (WT_Logical_Point*)alloca(npts * sizeof(WT_Logical_Point));
            for (int i=0; i<npts; i++)
            {
                dstpts[i].m_x = (WT_Integer32)fpts[i].x;
                dstpts[i].m_y = (WT_Integer32)fpts[i].y;
            }

            WT_Polyline excludearea(npts, dstpts, false);
            excludearea.serialize(*file);
            IncrementDrawableCount();
        }

    EndMacro(file, 0);

    // write the exclusion area
    BeginOverpostGroup(file, RS_OverpostType_All, false, true);

        //we need to sync the rendition manually
        //since we are about to play macros by writing
        //to the file directly
        WT_Integer32 parts_to_sync = WT_Rendition::Color_Bit           |
                                 //  WT_Rendition::Color_Map_Bit       |
                                 //  WT_Rendition::Fill_Bit            |
                                 //  WT_Rendition::Fill_Pattern_Bit    |
                                 //  WT_Rendition::Merge_Control_Bit   |
                                 //  WT_Rendition::BlockRef_Bit        |
                                     WT_Rendition::Visibility_Bit      |
                                 //  WT_Rendition::Line_Weight_Bit     |
                                     WT_Rendition::Pen_Pattern_Bit     |
                                 //  WT_Rendition::Line_Pattern_Bit    |
                                 //  WT_Rendition::Line_Caps_Bit       |
                                 //  WT_Rendition::Line_Join_Bit       |
                                 //  WT_Rendition::Marker_Size_Bit     |
                                 //  WT_Rendition::Marker_Symbol_Bit   |
                                 //  WT_Rendition::URL_Bit             |
                                     WT_Rendition::Layer_Bit           |
                                 //  WT_Rendition::Viewport_Bit        |
                                     WT_Rendition::Font_Extension_Bit  |
                                     WT_Rendition::Font_Bit            |
                                 //  WT_Rendition::Object_Node_Bit     |
                                     WT_Rendition::Text_Background_Bit |
                                     WT_Rendition::Text_HAlign_Bit     |
                                     WT_Rendition::Text_VAlign_Bit     |
                                     WT_Rendition::Contrast_Color_Bit;

        file->desired_rendition().sync(*file, parts_to_sync);

        // PlayMacro expects a point in mapping units
        double posx = 0.0;
        double posy = 0.0;
        ScreenToWorldPoint(posx, posy, posx, posy);

        PlayMacro(file, 0, refSizeMeters, RS_Units_Model, posx, posy);

    EndOverpostGroup(file);
}


//-----------------------------------------------------------------------------
//
//             RS_FontEngine
//
//-----------------------------------------------------------------------------


void DWFRenderer::MeasureString(const RS_String& s,
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

    // Scale the result.  Also need to flip the y-coordinates.  With GD y points
    // down, and we need to return the bounds in the renderer's space.
    for (int i=0; i<4; ++i)
    {
        res[i].x =  measureScale*extent[2*i];
        res[i].y = -measureScale*extent[2*i+1];
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


void DWFRenderer::DrawString(const RS_String& s,
                             double           x,
                             double           y,
                             double           /*width*/,
                             double           height,
                             const RS_Font*   font,
                             RS_Color&        color,
                             double           angleRad)
{
    if (NULL == font)
    {
        return;
    }

    // draw to the active file if it's set
    WT_File* file = m_w2dActive? m_w2dActive : m_w2dFile;

    file->desired_rendition().font().font_name() = Util_ConvertString(font->m_familyname.c_str());
    file->desired_rendition().font().style().set_bold(font->m_bold);
    file->desired_rendition().font().style().set_italic(font->m_italic);
    file->desired_rendition().font().style().set_underlined(false);
    file->desired_rendition().font().height() = (WT_Integer32)height;
    file->desired_rendition().font().rotation() = (WT_Unsigned_Integer16)(angleRad / (2.0*M_PI) * 65536.0);

    file->desired_rendition().color() = Util_ConvertColor(color);

    // don't do any BIDI conversion - the client viewer will handle this

    WT_String wtstr(Util_ConvertString(s.c_str()));
    WT_Logical_Point pt((WT_Integer32)x, (WT_Integer32)y);

    WT_Text wttext(pt, wtstr);
    wttext.serialize(*file);
    IncrementDrawableCount();
}


const RS_Font* DWFRenderer::FindFont(RS_FontDef& def)
{
    return FontManager::Instance()->FindFont(def.name().c_str(),
                          (def.style() & RS_FontStyle_Bold) != 0,
                          (def.style() & RS_FontStyle_Italic) != 0);
}


//////////////////////////////////////////////////////////////
////
////             DWF Rewrite and related code
////
//////////////////////////////////////////////////////////////


//Inserts the contents of a given DWF input stream
//into the current output W2D. The given coord sys
//transformation is applied and geometry will be clipped
//to the RS_Bounds context of the DWFRenderer
void DWFRenderer::AddDWFContent(RS_InputStream*  in,
                                CSysTransformer* xformer,
                                const RS_String& section,
                                const RS_String& passwd,
                                const RS_String& w2dfilter)
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

                /*
                if (pGlobal == NULL)
                {
                wcout << L"\t[OK] (" << pSection->order() << L") v" << pSection->version();

                if (pSection->name())
                {
                wcout << L" [" << (const wchar_t*)(pSection->name()) << L"]";
                }

                if (pSection->objectID())
                {
                wcout << L" [" << (const wchar_t*)(pSection->objectID()) << L"]";
                }

                wcout << endl;
                }
                else
                {
                wcout << L"\t[OK] (Global Section)" << endl;

                const DWFBookmark* pRoot = pGlobal->bookmark();
                if (pRoot)
                {
                wcout << L"\tDumping bookmarks..." << endl;
                dump_bookmarks(pRoot);
                }
                }
                */
            }

            DWFCORE_FREE_OBJECT(iSection);
        }

    }
    catch (DWFException&)
    {
    }
}


void DWFRenderer::AddW2DContent(RS_InputStream* in, CSysTransformer* xformer, const RS_String& w2dfilter)
{
    WT_Result result;

    //initialize state variables, which are needed by W2D rewriter callbacks
    m_input = in;
    m_xformer = xformer;
    m_bHaveViewport = false;
    m_layerFilter = w2dfilter;
    m_bLayerPassesFilter = true;

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
}


void DWFRenderer::SetActions(WT_File& file)
{
    file.set_stream_open_action (simple_open);
    file.set_stream_close_action(simple_close);
    file.set_stream_read_action (simple_read);
    file.set_stream_seek_action (simple_seek);

    file.set_dwf_header_action(simple_process_dwf_header);
    file.set_author_action(simple_process_author);
    file.set_creator_action(simple_process_creator);
    file.set_creation_time_action(simple_process_created);
    file.set_modification_time_action(simple_process_modified);
    file.set_source_filename_action(simple_process_sourceFilename);
    file.set_source_creation_time_action(simple_process_sourceCreationTime);
    file.set_source_modification_time_action(simple_process_sourceModificationTime);
    file.set_units_action(simple_process_units);
    file.set_embed_action(simple_process_embed);
    file.set_named_view_action(simple_process_namedView);
    file.set_view_action(simple_process_view);
    file.set_plot_info_action(simple_process_plotInfo);
    file.set_background_action(simple_process_background);
    file.set_color_action(simple_process_color);
    file.set_line_weight_action(simple_process_lineWeight);
    file.set_line_style_action(simple_process_lineStyle);
    file.set_layer_action(simple_process_layer);
    file.set_viewport_action(simple_process_viewport);
    file.set_visibility_action(simple_process_visibility);
    file.set_code_page_action(simple_process_codePage);
    file.set_color_map_action(simple_process_colorMap);
    file.set_comments_action(simple_process_comments);
    file.set_contour_set_action(simple_process_contourSet);
    file.set_copyright_action(simple_process_copyright);
    file.set_dash_pattern_action(simple_process_dashPattern);
    file.set_description_action(simple_process_description);
    file.set_fill_action(simple_process_fill);
    file.set_filled_ellipse_action(simple_process_filledEllipse);
    file.set_font_action(simple_process_font);
    file.set_image_action(simple_process_image);
    file.set_keywords_action(simple_process_keywords);
    file.set_marker_size_action(simple_process_markerSize);
    file.set_marker_symbol_action(simple_process_markerSymbol);
    file.set_merge_control_action(simple_process_mergeControl);
    file.set_named_view_list_action(simple_process_namedViewList);
    file.set_origin_action(simple_process_origin);
    file.set_outline_ellipse_action(simple_process_outlineEllipse);
    file.set_polygon_action(simple_process_polygon);
    file.set_polytriangle_action(simple_process_polytriangle);
    file.set_polymarker_action(simple_process_polymarker);
    file.set_projection_action(simple_process_projection);
    file.set_subject_action(simple_process_subject);
    file.set_title_action(simple_process_title);
    file.set_unknown_action(simple_process_unknown);
    file.set_url_action(simple_process_url);
    file.set_png_group4_image_action(simple_process_pngGroup4Image);
    file.set_polyline_action(simple_process_polyline);
    file.set_text_action(simple_process_text);
    file.set_inked_area_action(simple_process_inkedArea);
    file.set_plot_optimized_action(simple_process_plotOptimized);
    file.set_group_begin_action(simple_process_groupBegin);
    file.set_group_end_action(simple_process_groupEnd);
    file.set_embedded_font_action(simple_process_embeddedFont);
    file.set_trusted_font_list_action(simple_process_trustedFontList);
    file.set_blockref_action(simple_process_blockref);
    file.set_block_meaning_action(simple_process_blockMeaning);
    file.set_encryption_action(simple_process_encryption);
    file.set_orientation_action(simple_process_orientation);
    file.set_alignment_action(simple_process_alignment);
    file.set_password_action(simple_process_password);
    file.set_guid_action(simple_process_guid);
    file.set_filetime_action(simple_process_fileTime);
    file.set_userdata_action(simple_process_userData);
    file.set_pen_pattern_action(simple_process_penPattern);
    file.set_line_pattern_action(simple_process_linePattern);
    file.set_fill_pattern_action(simple_process_fillPattern);
    file.set_signdata_action(simple_process_DigitalSign);

    file.set_gouraud_polytriangle_action(simple_process_gouraudPolytriangle);
    file.set_gouraud_polyline_action(simple_process_gouraudPolyline);
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
const WT_Logical_Point* DWFRenderer::ProcessW2DPoints(WT_File&           file,
                                                      WT_Logical_Point*  srcpts,
                                                      int                numpts,
                                                      LineBuffer::GeomOperationType clipType,
                                                      int&               outNumpts,
                                                      vector<int>**      outCntrs)
{
    //This transformer may have been modified if a Viewport
    //opcode was encountered in the source W2D. This is needed for
    //support for symbols from ACAD, which do not use the
    //standard W2D extent used by Studio.
    //The function UpdateSymbolTrans does the actual modification and is
    //called by the process_viewport callback.
    SymbolTrans* trans = (SymbolTrans*)m_xformer;

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

    //
    // Clip the points to requested map data extent
    //

    //IMPORTANT: Only do this for DWF layers.
    //DWF Symbols are immune from clipping

    if (!IsSymbolW2D())
    {
        if (   clipType == LineBuffer::ctArea
            || clipType == LineBuffer::ctLine
            || clipType == LineBuffer::ctPoint)
        {
            //we have now built a line buffer representing the DWF entity
            //but transformed into the mapping space of the destination map
            //We need to clip this to the requested map extent
            LineBuffer* lbc = lb->Clip(GetBounds(), clipType, m_pPool);

            if (lbc == NULL)
            {
                //points are fully outside bounds, return 0
                LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
                outNumpts = 0;
                return NULL;
            }

            //if the polygon needed clipping, use the clipped version from now in
            if (lbc != lb)
            {
                LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
                lb = lbc;
                spLB.reset(lb);
            }
        }
    }

    //
    // Transfer contours to return vector
    //
    m_cntrs.clear();

    for (int i=0; i<lb->cntr_count(); i++)
        m_cntrs.push_back(lb->cntr_size(i));

    //set the return contour pointer if more than
    *outCntrs = &m_cntrs;

    //
    // Convert to destination W2D space
    //

    // IMPORTANT: Only do this if the data is a DWF layer
    // and not a DWF symbol. DWF Symbols will be transformed
    // to the correct mapping space location by the Macro opcode
    // that encloses them.

    if (!IsSymbolW2D())
    {
        if (lb->point_count() > 0)
        {
            _TransformPointsNoClamp(lb);
        }
    }
    else
    {
        //for symbols just copy the points to the output array without
        //transofrming to mapping space
        int count = lb->point_count();

        EnsureBufferSize(count);
        WT_Logical_Point* wpts = (WT_Logical_Point*)m_wtPointBuffer;

        for (int i=0; i<count; i++)
        {
            wpts[i].m_x = (WT_Integer32)lb->x_coord(i);
            wpts[i].m_y = (WT_Integer32)lb->y_coord(i);
        }
    }

    outNumpts = lb->point_count();

    //free clipped buffer
    LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());

    return m_wtPointBuffer;
}


//similar to ProcessW2DPoints, but performs no clipping
const WT_Logical_Point* DWFRenderer::TransformW2DPoints(WT_File& file, WT_Logical_Point* srcpts, int numpts)
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

    //
    // Process the points into mapping space
    // and then into destination W2D space
    //

    EnsureBufferSize(numpts);
    WT_Logical_Point* wpts = (WT_Logical_Point*)m_wtPointBuffer;

    WT_Point3D psrc(0.0, 0.0, 0.0);
    WT_Point3D pdst(0.0, 0.0, 0.0);

    for (int i=0; i<numpts; i++)
    {
        psrc.m_x = srcpts[i].m_x;
        psrc.m_y = srcpts[i].m_y;

        //this puts it into model space of the source data
        //This is mostly used for DWF layers which have
        //a WHIP (Units) opcode specified that transforms from
        //W2D space to model/mapping space
        xform.transform(psrc, pdst);

        //now transform to mapping space of map
        //using the symbol transformer object
        if (trans)
            trans->TransformPoint(pdst.m_x, pdst.m_y);

        //and now transform from mapping space to W2D space
        //without clamping
        wpts[i].m_x = (WT_Integer32)((pdst.m_x - m_offsetX) * m_scale);
        wpts[i].m_y = (WT_Integer32)((pdst.m_y - m_offsetY) * m_scale);
    }

    return m_wtPointBuffer;
}


//This function scales a W2D space related value from
//source W2D space to destination.
//Since the source W2D file can fit into a small piece of the
//destination DWF or be much larger, we need to take that
//scaling into account when carrying over things like line weight,
//font height, etc. this helper funtion determines and applies
//that scale
WT_Integer32 DWFRenderer::ScaleW2DNumber(WT_File& file, WT_Integer32 number)
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

    //only scale by map scale if we are not a symbol inside a macro
    //since macro scaling already takes map scale into account
    if (!m_bIsSymbolW2D)
    {
        dDstSpace *= m_scale;
    }

    return (WT_Integer32)dDstSpace;
}


void DWFRenderer::UpdateSymbolTrans(WT_File& /*file*/, WT_Viewport& viewport)
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
            //multiple viewports, which messes up this logic -- no single
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


// Each UIGraphic in the map needs a unique ID, and this ID is referenced
// when updating the UIGraphic properties (legend label and display in
// legend properties).  Unfortunately the UIGraphic object IDs are not
// stored as part of the runtime map - they are created with the initial
// EMap and then forgotten.  We could update the runtime object model to
// store these, but we also want to keep the size of the runtime objects
// as small as possible.
//
// Since the only UIGraphic objects we need to update are ones directly
// associated with a layer or a layer group (not ones associated with
// a scale range), we can avoid storing UIGraphic object IDs by basing
// them off of the layer / layer group object IDs.  This helper method
// does that.
DWFString DWFRenderer::GetUIGraphicObjectIdFromLayerObjectId(const wchar_t* guid)
{
    _ASSERT(guid != NULL && wcslen(guid) == 36);

    DWFString str(guid);
    wchar_t* newGuid = (wchar_t*)(const wchar_t*)str;

    // just change the first character in the ID
    if (*newGuid != L'0')
        *newGuid = L'0';
    else
        *newGuid = L'1';

    return str;
}
