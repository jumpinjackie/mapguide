//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include <stdio.h>

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

using namespace DWFToolkit;
using namespace DWFCore;
using namespace std;

#include "W2DRewriter.h"
#include "UnicodeString.h"
#include "SLDSymbols.h"

//table we use for keeping track of object nodes
typedef DWFWCharKeySkipList<unsigned int> NodeTable;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//------------------------------Utilities--------------------------------------
//-----------------------------------------------------------------------------

WT_Color Util_ConvertColor(RS_Color & color)
{
    WT_Color ret(color.red(), color.green(), color.blue(), color.alpha());

    return ret;
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

    _ASSERT( len < 65536); //sanity check since we alloc on the stack

    unsigned short* utf16Bytes = (unsigned short*)alloca(len * sizeof (unsigned short));
    DwfRendererConvertUTF32toUTF16((const unsigned long*)wstr, utf16Bytes, len);

#endif

    WT_String ret((WT_Unsigned_Integer16 const *)utf16Bytes);

    return ret;
}


//-----------------------------------------------------------------------------
//--------------------------W2D I/O overloads----------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


WT_Result my_open (WT_File & file)
{
    //this is freed in EndMap() (loops over all layer streams)
    DWFBufferOutputStream* bos = DWFCORE_ALLOC_OBJECT( DWFBufferOutputStream(100000));

    file.set_stream_user_data(bos);
    return WT_Result::Success;
}

WT_Result my_close (WT_File & file)
{
    file.set_stream_user_data(WD_Null);
    return WT_Result::Success;
}

WT_Result my_read (WT_File & /*file*/, int /*desired_bytes*/, int & /*bytes_read*/, void * /*buffer*/)
{
    //should never get called, overloaded for consistency
    _ASSERT(false);
    return WT_Result::Success;
}

WT_Result my_write (WT_File & file,int size,void const * buffer)
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
: m_w2dFile(NULL),
  m_mapInfo(NULL),
  m_layerInfo(NULL),
  m_featureClass(NULL),
  m_attributes(NULL),
  m_featureClassInfo(NULL),
  m_extents(0,0,0,0),
  m_mapExtents(0,0,0,0),
  m_wtPointBuffer(NULL),
  m_symbolManager(NULL),
  m_bIsSymbolW2D(false),
  m_hatchFac(NULL),
  m_fillFac(NULL),
  m_obsMesh(NULL),
  m_w2dStream(NULL),
  m_w2dLabels(NULL),
  m_labelStream(NULL),
  m_pPage(NULL),
  m_input(NULL),
  m_xformer(NULL),
  m_bHaveLabels(false),
  m_bHaveViewport(false)
{
    m_lLayerStreams.reserve(8);
    m_lLabelStreams.reserve(8);
    m_lAttributeResources.reserve(8);

    m_hObjNodes = new NodeTable();
}

DWFRenderer::~DWFRenderer()
{
    if (m_wtPointBuffer)
        delete[] m_wtPointBuffer;

    if (m_hatchFac)
        delete m_hatchFac;

    if (m_fillFac)
        delete m_fillFac;

    if (m_obsMesh)
        delete m_obsMesh; //should not get here

    delete (NodeTable*)m_hObjNodes;
}

//-----------------------------------------------------------------------------
//
// Initializes map generation with required map properties.
//
//-----------------------------------------------------------------------------
void DWFRenderer::StartMap( RS_MapUIInfo* mapInfo,
                            RS_Bounds&    extents,
                            double        mapScale,
                            double        dpi,
                            double        metersPerUnit,
                            CSysTransformer* xformToLL
                             )
{
    if (dpi == 0.0)
        m_dpi = 96.0; // default -- 96 is usually true for Windows
    else
        m_dpi = dpi;

    m_metersPerUnit = metersPerUnit;
    m_extents = extents;

    double metersPerPixel = 0.0254 / m_dpi;

    //TODO: we can compute the map scale
    //given the extents and the screen bounds,
    //so if it is not specified, do the default computation
    if (mapScale <= 0)
    {
        m_mapScale = 1.0; //avoid 0 map scale
    }
    else
        m_mapScale = mapScale;

    //compute drawing scale
    //drawing scale is map scale converted to [mapping units] / [pixels]
    m_drawingScale = m_mapScale * metersPerPixel / m_metersPerUnit;

    //scale used to convert to DWF logical coordinates in the range [0, 2^30-1]
    m_scale = (double)(INT_MAX/2) / rs_max(m_extents.width(), m_extents.height());

    m_offsetX = m_extents.minx - m_extents.width() * 0.5;
    m_offsetY = m_extents.miny - m_extents.height() * 0.5;

    m_nObjectId = 1;
    m_layerNum = 1;

    if (xformToLL)
        m_obsMesh = new ObservationMesh(m_extents, xformToLL);

    // remember the map info
    m_mapInfo = mapInfo;
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
        iter != m_lLayerStreams.end(); iter++)
    {
        DWFCORE_FREE_OBJECT ( *iter);
    }

    for (stream_list::iterator iter = m_lLabelStreams.begin();
        iter != m_lLabelStreams.end(); iter++)
    {
        DWFCORE_FREE_OBJECT ( *iter);
    }

    for (stream_list::iterator iter = m_lLayoutStreams.begin();
        iter != m_lLayoutStreams.end(); iter++)
    {
        DWFCORE_FREE_OBJECT( *iter );
    }

    for (stream_list::iterator iter = m_lLayoutLabelStreams.begin();
        iter != m_lLayoutLabelStreams.end(); iter++)
    {
        DWFCORE_FREE_OBJECT( *iter );
    }

    m_lLayerStreams.clear();
    m_lLabelStreams.clear();
    m_lLayoutStreams.clear();
    m_lLayoutLabelStreams.clear();

    if (m_obsMesh)
    {
        delete m_obsMesh;
        m_obsMesh = NULL;
    }

    // clear the map info
    m_mapInfo = NULL;
}

//-----------------------------------------------------------------------------
//
// Indicates beginning of a map layer. This function creates a W2D for feature
// data and a W2D for labeling. It also sets up selection support / object
// deifnition. Labeling W2D is not opened -- it will only be opened if there
// are labels.
//
//-----------------------------------------------------------------------------

void DWFRenderer::StartLayer(RS_LayerUIInfo*      legendInfo,
                             RS_FeatureClassInfo* classInfo
                             )
{
    if (classInfo && legendInfo &&
       (legendInfo->selectable() || legendInfo->hastooltips() || legendInfo->hashyperlinks()))
    {
        _ASSERT(m_attributes == NULL);
        _ASSERT(m_featureClass == NULL);

        m_attributes = DWFCORE_ALLOC_OBJECT (
            DWFToolkit::DWFObjectDefinitionResource(
                    DWFXML::kzElement_PageObjectDefinition,
                    DWFXML::kzRole_ObjectDefinition));

        m_featureClass  = DWFCORE_ALLOC_OBJECT (
                DWFToolkit::DWFDefinedObject(classInfo->name().c_str()));

        m_featureClassInfo = classInfo;

        //DWFToolkit::DWFProperty* prop = DWFCORE_ALLOC_OBJECT (
        //    DWFProperty(L"hello1", L"hello2", L"hello3"));

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
    m_w2dFile->set_stream_write_action (my_write);
    m_w2dFile->set_stream_user_data(WD_Null);

    //overload WHIP I/O functions with our memory streaming ones
    m_w2dLabels->set_stream_open_action (my_open);
    m_w2dLabels->set_stream_close_action(my_close);
    m_w2dLabels->set_stream_read_action (my_read);
    m_w2dLabels->set_stream_seek_action (my_seek);
    m_w2dLabels->set_stream_write_action (my_write);
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

    if (legendInfo)
    {
        //set the W2D layer id -- this is needed for the EMap case
        //but should not matter for the EPlot case
        WT_Layer lr(*m_w2dFile, m_layerNum, Util_ConvertString(legendInfo->name().c_str()));
        m_w2dFile->desired_rendition().layer() = lr;

        WT_Layer lrl(*m_w2dLabels, m_layerNum, Util_ConvertString(legendInfo->name().c_str()));
        m_w2dLabels->desired_rendition().layer() = lrl;

        m_layerNum++;
    }

    m_imgID = 0;

    // remember the layer info
    m_layerInfo = legendInfo;
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

    if (m_w2dLabels)
    {
        if (m_bHaveLabels)
        {
            //only need to keep track of label w2d if
            //stream was opened -- which happens when there
            //is at least one label
            m_lLabelStreams.push_back((DWFBufferOutputStream*)m_w2dLabels->stream_user_data());
        }
        else
            m_lLabelStreams.push_back(NULL);//add NULL for no labels

        m_w2dLabels->close();
        delete m_w2dLabels;
        m_w2dLabels = NULL;
        m_bHaveLabels = false;
    }
    else
        m_lLabelStreams.push_back(NULL); //add NULL for no labels

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

    // clear the layer info
    m_layerInfo = NULL;
}

//----------------------------------------------------------------------------
// All subsequent drawables will be considered as part of a single feature
// identifier by the given feature reader, until the next call to StartFeature()
//----------------------------------------------------------------------------
void DWFRenderer::StartFeature(RS_FeatureReader* feature,
                               const RS_String* tooltip,
                               const RS_String* url)
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
void DWFRenderer::ProcessPolygon (  LineBuffer*         srclb,
                                    RS_FillStyle&       fill)
{
    LineBuffer* workbuffer = srclb->Optimize(m_drawingScale, &m_lbPool);

    if (workbuffer->point_count() == 0)
    {
        m_lbPool.FreeLineBuffer(workbuffer);
        return;
    }

    if (fill.color().alpha() != 0)
    {
        WriteFill(fill);

        _TransformPointsNoClamp(workbuffer->points(), workbuffer->point_count());

        //just a polygon, no need for a contourset
        if (workbuffer->cntr_count() == 1)
        {
            //fill
            WT_Polygon fillpoly(workbuffer->point_count(), m_wtPointBuffer, false);
            m_w2dFile->desired_rendition().fill() = true;
            m_w2dFile->desired_rendition().color() = Util_ConvertColor(fill.color());
            fillpoly.serialize(*m_w2dFile);
        }
        else //otherwise make a contour set
        {
            //write out the fill (as a contour set)
            //TODO:::::: &cntrs.front() may not work and is not legal
            WT_Contour_Set cset(*m_w2dFile, workbuffer->cntr_count(), (WT_Integer32*)workbuffer->cntrs(),
                workbuffer->point_count(), m_wtPointBuffer, true);

            cset.serialize(*m_w2dFile);
        }
    }

    //TODO: once macro is implemented we will not need to write the
    //polyline twice

    //write out the polygon outline as a bunch of WT_Polylines
    if (fill.outline().color().alpha() == 0)
    {
        m_lbPool.FreeLineBuffer(workbuffer);
        return;
    }

    WriteStroke(fill.outline());

    //apply line style if needed
    if ((_wcsicmp(fill.outline().style().c_str(), L"Solid") != 0))
    {
        LineStyleDef lineStyleDef;
        WT_Line_Pattern lpat(WT_Line_Pattern::Solid);
        WT_Dash_Pattern dpat(WT_Dash_Pattern::kNull);

        int patid = lineStyleDef.ConvertToDashPattern(fill.outline().style().c_str(), m_dpi, fill.outline().width() * 100 / 2.54 * m_dpi, dpat, lpat);

        if (patid < WT_Line_Pattern::Count)
            m_w2dFile->desired_rendition().line_pattern() = lpat;
        else
            m_w2dFile->desired_rendition().dash_pattern() = dpat;
    }
    else
        m_w2dFile->desired_rendition().line_pattern() = WT_Line_Pattern(WT_Line_Pattern::Solid);

    WritePolylines(workbuffer);

    //zero out the dash pattern -- must do when done with it
    if (m_w2dFile->desired_rendition().dash_pattern() != WT_Dash_Pattern::kNull)
        m_w2dFile->desired_rendition().dash_pattern() = WT_Dash_Pattern::kNull;

    if (m_obsMesh)
        m_obsMesh->ProcessPoint(workbuffer->points()[0], workbuffer->points()[1]);

    m_lbPool.FreeLineBuffer(workbuffer);
}


//-----------------------------------------------------------------------------
//
// Polyline features. Transformation to W2D space is done here.
//
//-----------------------------------------------------------------------------
void DWFRenderer::ProcessPolyline(LineBuffer*           srclb,
                                  RS_LineStroke&        lsym)
{
    if (srclb->point_count() == 0)
        return;

    if (lsym.color().alpha() == 0)
        return;

    WriteStroke(lsym);

    //apply line style if needed
    LineBuffer* workbuffer = srclb->Optimize(m_drawingScale, &m_lbPool);

    //apply line style if needed
    if ((_wcsicmp(lsym.style().c_str(), L"Solid") != 0))
    {
        LineStyleDef lineStyleDef;
        WT_Line_Pattern lpat(WT_Line_Pattern::Solid);
        WT_Dash_Pattern dpat(WT_Dash_Pattern::kNull);

        int patid = lineStyleDef.ConvertToDashPattern(lsym.style().c_str(), m_dpi, lsym.width() * 100 / 2.54 * m_dpi, dpat, lpat);

        if (patid < WT_Line_Pattern::Count)
            m_w2dFile->desired_rendition().line_pattern() = lpat;
        else
            m_w2dFile->desired_rendition().dash_pattern() = dpat;
    }
    else
        m_w2dFile->desired_rendition().line_pattern() = WT_Line_Pattern(WT_Line_Pattern::Solid);

    WritePolylines(workbuffer);

    //zero out the dash pattern -- must do when done with it
    if (m_w2dFile->desired_rendition().dash_pattern() != WT_Dash_Pattern::kNull)
        m_w2dFile->desired_rendition().dash_pattern() = WT_Dash_Pattern::kNull;

    if (m_obsMesh)
        m_obsMesh->ProcessPoint(workbuffer->points()[0], workbuffer->points()[1]);

    m_lbPool.FreeLineBuffer(workbuffer);
}

//-----------------------------------------------------------------------------
//
// Raster serialization.
//
//-----------------------------------------------------------------------------
void DWFRenderer::ProcessRaster(unsigned char*    data,
                                int               length,
                                RS_ImageFormat    format,
                                int               width,
                                int               height,
                                RS_Bounds         extents)
{
    if (format != RS_ImageFormat_RGBA && format != RS_ImageFormat_PNG)
    {
        //TODO:
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

    if (format == RS_ImageFormat_RGBA)
    {
        WT_Image img( (WT_Unsigned_Integer16)height,
                    (WT_Unsigned_Integer16)width,
                    WT_Image::/*WT_Image_Format::*/RGBA,
                    m_imgID++,
                    NULL,
                    length,
                    (WT_Byte*)data,
                    WT_Logical_Point(x0, y0),
                    WT_Logical_Point(x1, y1),
                    false);

        img.serialize(*m_w2dFile);
    }
    else if (format == RS_ImageFormat_PNG)
    {
        WT_PNG_Group4_Image img( (WT_Unsigned_Integer16)height,
                    (WT_Unsigned_Integer16)width,
                    WT_PNG_Group4_Image::PNG,
                    m_imgID++,
                    NULL,
                    length,
                    (WT_Byte*)data,
                    WT_Logical_Point(x0, y0),
                    WT_Logical_Point(x1, y1),
                    false);

        img.serialize(*m_w2dFile);
    }
}


//-----------------------------------------------------------------------------
//
// Marker Symbols
//
//-----------------------------------------------------------------------------
void DWFRenderer::ProcessMarker(LineBuffer* srclb,
                                RS_MarkerDef& mdef,
                                bool allowOverpost)
{
    for (int i=0; i<srclb->point_count(); i++)
    {
        //if marker is processed from here it should be added to the
        //feature W2D, not the labeling W2D -- need the API to reflect that.
        ProcessOneMarker(srclb->points()[2*i], srclb->points()[2*i+1], mdef, allowOverpost);
    }
}


void DWFRenderer::ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost)
{
    WT_File* file = m_w2dFile;

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

        symbol = (RS_InputStream*)m_symbolManager->GetSymbolData(mdef.library().c_str(),
                                                L"symbols.dwf"); //NOXLATE
    }

    //default symbol
    double angle = mdef.rotation() * M_PI / 180.0;

    //default bounds of symbol data in W2D
    //for symbols created by MapGuide Studio
    RS_Bounds src(0,0,SYMBOL_MAX,SYMBOL_MAX);

    //default reference point (negated, to mimic the one that is stored in the
    //symbol library)
    double refX = mdef.insx();
    double refY = mdef.insy();

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
    double lastAspect = (m_lastSymbol.width() == 0.0) ? 0.0 : m_lastSymbol.height() / m_lastSymbol.width();
    double aspect = (mdef.width() == 0.0) ? 0.0 : mdef.height() / mdef.width();
    
    RS_Bounds dst;

    if (aspect <= 1.0)
    {
        double diff = 0.5 * (1.0 - aspect) * SYMBOL_MAX;
        dst = RS_Bounds(0, diff, SYMBOL_MAX, SYMBOL_MAX - diff);
    }
    else
    {
        double diff = 0.5 * (1.0 - aspect) * SYMBOL_MAX;
        dst = RS_Bounds(diff, 0, SYMBOL_MAX - diff, SYMBOL_MAX);
    }

    //construct transformer -- we will use one
    //even for the default symbol -- makes sure
    //it goes through the same transformation path
    SymbolTrans trans(src, dst, refX, refY, angle);

    if (!symbol)
    {
        if (is_font_symbol)
        {
            //TODO: cannot easily check for font symbol repetition
            //since we forward to the labeling logic...
            ////check to see if the last symbol we got was the same
            //if (   mdef.library() != m_lastSymbol.library()
            //    || mdef.name() != m_lastSymbol.name()
            //    || mdef.rotation() != m_lastSymbol.rotation()
            //    || mdef.style().outline().color().argb() != m_lastSymbol.style().outline().color().argb()
            //    || mdef.style().color().argb() != m_lastSymbol.style().color().argb()
            //    || mdef.style().background().argb() != m_lastSymbol.style().background().argb()
            //    || aspect != lastAspect)
            //{
                m_lastSymbol = mdef;

            //convert font symbol to a simple label
            RS_TextDef tdef(RS_HAlignment_Center, RS_VAlignment_Half);
            RS_FontDef fdef(mdef.library(), mdef.height(), mdef.fontstyle(), mdef.units());

            tdef.font() = fdef;
            tdef.color() = mdef.style().color();
            tdef.rotation() = mdef.rotation();

            //approximately carry over the insertion point -- it will map the unit square
            //based insertion point to a square of size font height.
            //Also convert the insertion point vector from a lower-left corner delta
            //to an offset from the center of the character -- since we use Center/Halfline
            //alignment when we draw it as text
            double scaledRefX = (0.5 - refX) * fdef.height();
            double scaledRefY = (0.5 - refY) * fdef.height();

            //pass on to label drawing code to draw the text using a macro
            RS_LabelInfo info(x, y, scaledRefX, scaledRefY, tdef.font().units(), tdef, false);
            ProcessLabelGroup(&info, 1, mdef.name(), RS_OverpostType_All, false, NULL);
        }
        else
        {
            //check to see if the last symbol we got was the same
            if (   mdef.library() != m_lastSymbol.library()
                || mdef.name() != m_lastSymbol.name()
                || mdef.rotation() != m_lastSymbol.rotation()
                || mdef.style().outline().color().argb() != m_lastSymbol.style().outline().color().argb()
                || mdef.style().color().argb() != m_lastSymbol.style().color().argb()
                || mdef.style().background().argb() != m_lastSymbol.style().background().argb()
                || aspect != lastAspect)
            {
                m_lastSymbol = mdef;

                //it is one of the default SLD symbols
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
                    //default or error
                    poly = (RS_F_Point*)SLD_ERROR;
                    npts = sizeof(SLD_ERROR) / (2 * sizeof(double));
                    nm = NULL;
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

                    if (!nm)
                    {
                        //symbol library error or unknown symbol

                        file->desired_rendition().color() = WT_Color(255,0,0);
                        WT_Polyline symbol(npts, pts, true);
                        symbol.serialize(*file);
                    }
                    else
                    {
                        if (mdef.style().color().argb() == RS_Color::EMPTY_COLOR_ARGB)
                            file->desired_rendition().color() = WT_Color(0,0,255);
                        else
                            file->desired_rendition().color() = WT_Color(Util_ConvertColor(mdef.style().color()));

                        WT_Polygon symbolFill(npts, pts, true);
                        symbolFill.serialize(*file);

                        if (mdef.style().color().argb() == RS_Color::EMPTY_COLOR_ARGB)
                            file->desired_rendition().color() = WT_Color(127,127,127);
                        else
                            file->desired_rendition().color() = WT_Color(Util_ConvertColor(mdef.style().outline().color()));

                        WT_Polyline symbol(npts, pts, true);
                        symbol.serialize(*file);
                    }

                //end macro definition 
                EndMacro(file);
            }

            PlayMacro(file, 0, rs_max(mdef.width(), mdef.height()), mdef.units(), x, y);
        }
    }
    else
    {
        //case where we are using a symbol from the library

        //check to see if the last symbol we got was the same
        if (   mdef.library() != m_lastSymbol.library()
            || mdef.name() != m_lastSymbol.name()
            || mdef.rotation() != m_lastSymbol.rotation()
            || mdef.style().outline().color().argb() != m_lastSymbol.style().outline().color().argb()
            || mdef.style().color().argb() != m_lastSymbol.style().color().argb()
            || mdef.style().background().argb() != m_lastSymbol.style().background().argb()
            || aspect != lastAspect)
        {
            m_lastSymbol = mdef;

            //also set flags controlling how W2D are rewritten
            //into the destination DWF
            m_bIsSymbolW2D = true;
            m_mdOverrideColors = mdef;

            //enclose W2D geometry in a macro
            BeginMacro(file, 0, SYMBOL_MAX);

                //copy symbol W2D into destination
                AddDWFContent(symbol, &trans, mdef.name(), RS_String(L""), RS_String(L""));

            //end macro definition and play the macro
            EndMacro(file);

            m_bIsSymbolW2D = false;
        }

        PlayMacro(file, 0, rs_max(mdef.width(), mdef.height()), mdef.units(), x, y);
    }

    if (!allowOverpost)
    {
        //now add an exclusion region to the file so that
        //labels do not overlap the symbol

        //a square that we will transform into the dst bounds
        WT_Logical_Point box[5];
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
        SymbolTrans boxtrans(src, dst, refX, refY, angle);

        EnsureBufferSize(5);
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

        //USe macro slot 1, since we want to keep the symbol in macro 0
        //so that we can reuse it if the next point symbol macro we need
        //is the same symbol
        BeginMacro(file, 1, SYMBOL_MAX);

            //if there is no rotation, send just 2 points
            if (angle == 0)
            {
                WT_Logical_Point axisbox[2];
                axisbox[0].m_x = pts[0].m_x;
                axisbox[0].m_y = pts[0].m_y;
                axisbox[1].m_x = pts[2].m_x;
                axisbox[1].m_y = pts[2].m_y;

                WT_Polyline excludearea(2, axisbox, true);
                excludearea.serialize(*file);
            }
            else
            {
                WT_Polyline excludearea(4, pts, true);
                excludearea.serialize(*file);
            }

        EndMacro(file);

        //write the exclusion area
        file->write("(Overpost All False True (");
            PlayMacro(file, 1, rs_max(mdef.width(), mdef.height()), mdef.units(), x, y);
        file->write("))");
    }

    if (m_obsMesh)
        m_obsMesh->ProcessPoint(x, y);
}

//-----------------------------------------------------------------------------
//
// Text labels
//
//-----------------------------------------------------------------------------
void DWFRenderer::ProcessLabel(double           x,
                               double           y,
                               const RS_String& text,
                               RS_TextDef&      tdef)
{
    RS_LabelInfo info(x, y, 0.0, 0.0, tdef.font().units(), tdef, false);
    ProcessLabelGroup(&info, 1, text, RS_OverpostType_All, false, NULL);
}


void DWFRenderer::ProcessLabelGroup( RS_LabelInfo*       labels,
                                     int                 nlabels,
                                     const RS_String&    text,
                                     RS_OverpostType     type,
                                     bool                exclude,
                                     LineBuffer*         /*path*/)
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

    //need some reasonable scale for the macro
    //this scale will cancel out with the macro
    //size at which we play the macro

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
            int scale = (int)_MeterToW2DMacroUnit(RS_Units_Device, 1.0);

            //symbol is device space
            //we will need to place the label inside
            //a consant size macro at an offset
            //corresponding to the constant size offset from the
            //symbol center point

            //NOTE: Having a mapping space font size inside this
            //constant size macro will cause problems, but is
            //unavoidable with current Macro implementation in viewer

            double offx = _MeterToW2DMacroUnit(info->dunits(), info->dx());
            double offy = _MeterToW2DMacroUnit(info->dunits(), info->dy());

            WT_Integer32 ioffx = (offx < 0) ? (int)floor(offx) : (int)ceil(offx);
            WT_Integer32 ioffy = (offy < 0) ? (int)floor(offy) : (int)ceil(offy);

            BeginMacro(file, i+1, scale);

                ProcessMultilineText(file, text, info->tdef(), ioffx, ioffy);

            EndMacro(file);
        }
        else
        {
            //symbol was mapping space
            int scale = (int)_MeterToW2DMacroUnit(info->tdef().font().units(), 1.0);

            BeginMacro(file, i+1, scale);

                ProcessMultilineText(file, text, info->tdef(), 0, 0);

            EndMacro(file);
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
            WT_Integer32    parts_to_sync =     WT_Rendition::Color_Bit           |
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

                PlayMacro(file, i+1, 1, RS_Units_Device, info->x(), info->y());
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

                PlayMacro(file, i+1, 1, info->tdef().font().units(), posx, posy);
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
}

void DWFRenderer::EndMacro(WT_File* file)
{
    //close the Define Macro opcode
    file->write("))");
}


void DWFRenderer::PlayMacro(WT_File* file, int id, double sizeMeters, RS_Units unit, double x, double y)
{
    //play the macro at the given position with the given screen size
    int size = (int)_MeterToW2DMacroUnit(unit, sizeMeters);

    if (unit == RS_Units_Device)
        size = -size;

    WT_Integer32 ix = (WT_Integer32)_TX(x);
    WT_Integer32 iy = (WT_Integer32)_TY(y);

    char temp[256];

    sprintf(temp, " G %d S %d M 1 %d,%d", id, size, ix, iy);
    file->write(temp);
}


void DWFRenderer::ProcessMultilineText(WT_File* file, const RS_String& txt, RS_TextDef& tdef, int x, int y)
{
    //check if we need to break up
    //in MapGuide a newline in a string is indicated by a \n string, not the \n character
    if (!wcsstr(txt.c_str(), L"\\n"))
    {
        WT_String wtstr(Util_ConvertString(txt.c_str()));
        WT_Logical_Point pt(x, y);
        WT_Text wttext(pt, wtstr);
        wttext.serialize(*file);
        return;
    }

    //otherwise we need to break up into several strings

    //make a copy, TODO: this can be more efficient by using global string buffer
    size_t len = wcslen(txt.c_str());
    wchar_t* cpy = new wchar_t[len + 1];
    wcscpy(cpy, txt.c_str());

    //look for occurences of newline combo and remember start pointers
    //of each line
    std::vector<wchar_t*> line_breaks;

    line_breaks.push_back(cpy); //first line
    wchar_t* lbr = cpy;
    while ((lbr = ::wcsstr(lbr, L"\\n")) != 0)
    {
        *lbr++ = 0; *lbr++ = 0; //null terminate line break
        line_breaks.push_back(lbr);
    }

    size_t num_lines = line_breaks.size();

    //find vertical shift increment for each line
    WT_Integer32 line_hgt = (WT_Integer32)_MeterToW2DMacroUnit(tdef.font().units(), tdef.font().height());

    //depending on alignment we may go up or down with the line increment
    //take that into account by shifting initial y value
    int ypos = y;

    switch (tdef.valign())
    {
    case RS_VAlignment_Descent:
    case RS_VAlignment_Base:
        ypos += ((int)num_lines - 1) * line_hgt;
        break;
    case RS_VAlignment_Half:
        ypos += ((int)num_lines - 1) * line_hgt / 2;
        break;
    case RS_VAlignment_Cap:
    case RS_VAlignment_Ascent:
        //ypos = y;
        break;
    }

    //now draw each text line
    for (size_t i=0; i<num_lines; i++, ypos -= line_hgt)
    {
        WT_String wtstr(Util_ConvertString(line_breaks[i]));
        WT_Logical_Point pt(x, ypos);
        WT_Text wttext(pt, wtstr);
        wttext.serialize(*file);
    }

    //memory leaks are not cool
    delete [] cpy;
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
void DWFRenderer::_TransformPointsNoClamp(double* inpts, int numpts)
{
    EnsureBufferSize(numpts);
    WT_Integer32* wpts = (WT_Integer32*)m_wtPointBuffer;

    /*
    //loop unrolled 8 times using Duff's device.
    //See Stroustrup 6.6 Ex. 15
    int n = (numpts + 7) / 8;
    switch(numpts % 8)
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
        *wpts++ = (WT_Integer32)((*inpts++ - m_offsetX) * m_scale);
        *wpts++ = (WT_Integer32)((*inpts++ - m_offsetY) * m_scale);
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
    int index = 0;

    for (int i=0; i<srclb->cntr_count(); i++)
    {
        int cntr_size = srclb->cntrs()[i];

        if (cntr_size > 0)
        {
            _TransformPointsNoClamp(srclb->points()+index, cntr_size);
            WT_Polyline polyline(cntr_size, m_wtPointBuffer, false);
            polyline.serialize(*m_w2dFile);

            index += 2*cntr_size;
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

    int line_weight = 0;

    double thickness = stroke.width();

    //line width is always device space.
    //so convert to equivalent mapping space width
    line_weight = (int)(fabs(thickness) / m_metersPerUnit * m_mapScale * m_scale);

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
        hvalenum = m_hatchFac->find_index( fill.pattern().c_str() );
        if ( hvalenum != EMapHatchPatternFactory::nonexistent )
        {
            WT_User_Hatch_Pattern hatchpattern(0);
            m_hatchFac->request_pattern( hvalenum, hatchpattern );
            m_w2dFile->desired_rendition().user_hatch_pattern() = hatchpattern;
        }
        else
        {
            EMapFillPatternFactory::Enum fvalenum;
            fvalenum = m_fillFac->find_index( fill.pattern().c_str() );
            if ( fvalenum != EMapFillPatternFactory::nonexistent )
            {
                WT_User_Fill_Pattern fillpattern(0);
                m_fillFac->request_pattern( fvalenum, fillpattern );
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
// Serializes font and text alignement and rendering settings
//
//-----------------------------------------------------------------------------
void DWFRenderer::WriteTextDef(WT_File* file, RS_TextDef& tdef)
{
    //determine DWF space font height
    double hgt = _MeterToW2DMacroUnit(tdef.font().units(), tdef.font().height());
    //double hgt = _MeterToMapSize(tdef.font().units(), tdef.font().height());
    //hgt *= m_scale;

    file->desired_rendition().font().font_name() = WT_String(Util_ConvertString(tdef.font().name().c_str()));
    file->desired_rendition().font().style().set_bold((tdef.font().style() & RS_FontStyle_Bold) != 0);
    file->desired_rendition().font().style().set_italic((tdef.font().style() & RS_FontStyle_Italic) != 0);
    file->desired_rendition().font().style().set_underlined((tdef.font().style() & RS_FontStyle_Underline) != 0);
    file->desired_rendition().font().height() = (WT_Integer32)hgt;
    file->desired_rendition().font().rotation() = (WT_Unsigned_Integer16)(tdef.rotation() / 360.0 * 65536);

    file->desired_rendition().color() = Util_ConvertColor(tdef.color());

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

    //ghosting offset : 1 pixel
    double metersPerPixel = 1.0 / m_dpi * 2.54 / 100.0;
    int offset = (int)_MeterToW2DMacroUnit(tdef.font().units(), metersPerPixel);

    WT_Color bgcolor = Util_ConvertColor(tdef.bgcolor());

    file->desired_rendition().contrast_color() = bgcolor.rgba();

    switch (tdef.textbg())
    {
    case RS_TextBackground_None    : file->desired_rendition().text_background() = WT_Text_Background(WT_Text_Background::None, 0);
        break;
    case RS_TextBackground_Ghosted : file->desired_rendition().text_background() = WT_Text_Background(WT_Text_Background::Ghosted, offset);
        break;
    case RS_TextBackground_Opaque  : file->desired_rendition().text_background() = WT_Text_Background(WT_Text_Background::Solid, 0);
        break;
    default:
        break;
    }
}

//-----------------------------------------------------------------------------
//scale an input number in meters to a mapping
//space number given a device or mapping space unit.
//-----------------------------------------------------------------------------
double DWFRenderer::_MeterToMapSize(RS_Units unit, double number)
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
double DWFRenderer::_PixelToMapSize(Renderer* renderer, int pixels)
{
    // formula is as follows:
    //
    // Mapping Distance = Pixel Distance * (meters/pixel) * mapscale / (meters/map unit)
    //

    return (double)pixels * (0.0254 / renderer->GetDpi()) * renderer->GetMapScale() / renderer->GetMetersPerUnit();
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
        double w2dUnitsPerInch = 4096.0; //TODO: replace by 4096 when fixed
        scale_factor = 100.0 / 2.54 * w2dUnitsPerInch;
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
        _DWFCORE_THROW( DWFIOException, L"Failed to open W2D file" );
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
void DWFRenderer::StoreAttributes(RS_FeatureReader* feature, const RS_String* tooltip,
                                  const RS_String* url)
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
        const unsigned char* base64 = NULL;
        base64 = m_keyEncode.EncodeKey(feature);

        //skip writing of feature data if there is no id
        if (*base64 != 0)
        {
            DWFString sid = (const char*)base64;

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
                DWFDefinedObjectInstance* oi = DWFCORE_ALLOC_OBJECT( DWFDefinedObjectInstance(*m_featureClass, id));

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

RS_Bounds& DWFRenderer::GetBounds()
{
    return m_extents;
}

double DWFRenderer::GetDpi()
{
    return m_dpi;
}

double DWFRenderer::GetMetersPerUnit()
{
    return m_metersPerUnit;
}

double DWFRenderer::GetMapToW2DScale()
{
    return m_scale;
}

bool DWFRenderer::RequiresClipping()
{
    return true;
}

bool DWFRenderer::UseLocalOverposting()
{
    return false; // not applicable
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


void DWFRenderer::SetSymbolManager(RS_SymbolManager* manager)
{
    m_symbolManager = manager;
}

void DWFRenderer::StartLayout(RS_Bounds& extents)
{
    // Reset m_mapScale and m_metersPerUnit
    Init(extents);

    //scale used to convert to DWF logical coordinates in the range [0, 2^30-1]
    m_scale = (double)(INT_MAX/2) / rs_max(m_extents.width(), m_extents.height());

    //m_layerName = itemName;
    m_w2dFile = new WT_File();
    m_w2dLabels = new WT_File();

    //overload WHIP I/O functions with our memory streaming ones
    m_w2dFile->set_stream_open_action (my_open);
    m_w2dFile->set_stream_close_action(my_close);
    m_w2dFile->set_stream_read_action (my_read);
    m_w2dFile->set_stream_seek_action (my_seek);
    m_w2dFile->set_stream_write_action (my_write);
    m_w2dFile->set_stream_user_data(WD_Null);

    //overload WHIP I/O functions with our memory streaming ones
    m_w2dLabels->set_stream_open_action (my_open);
    m_w2dLabels->set_stream_close_action(my_close);
    m_w2dLabels->set_stream_read_action (my_read);
    m_w2dLabels->set_stream_seek_action (my_seek);
    m_w2dLabels->set_stream_write_action (my_write);
    m_w2dLabels->set_stream_user_data(WD_Null);

    m_bHaveLabels = false;

    OpenW2D(m_w2dFile);

    m_imgID = 0;
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

    if (m_w2dLabels)
    {
        if (m_bHaveLabels)
        {
            //only need to keep track of label w2d if
            //stream was opened -- which happens when there
            //is at least one label
            m_lLayoutLabelStreams.push_back((DWFBufferOutputStream*)m_w2dLabels->stream_user_data());
        }

        m_w2dLabels->close();
        delete m_w2dLabels;
        m_w2dLabels = NULL;
        m_bHaveLabels = false;
    }
}

void DWFRenderer::Init(RS_Bounds& extents)
{
    m_mapExtents = m_extents;

    m_metersPerUnit = 0.0254;
    m_mapScale = 1.0;
    m_extents = extents;

    double metersPerPixel = 0.0254 / m_dpi;

    // drawing scale is map scale converted to [mapping units] / [pixels]
    m_drawingScale = m_mapScale * metersPerPixel / m_metersPerUnit;

    // scale used to convert to DWF logical coordinates in the range [0, 2^31-1]
    m_scale = (double)(INT_MAX - 256) / rs_max(extents.width(), extents.height());

    m_offsetX = 0;
    m_offsetY = 0;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
////
////
////             DWF Rewrite and related code
////
////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

//Inserts the contents of a given DWF input stream
//into the current output W2D. The given coord sys
//transformation is applied and geometry will be clipped
//to the RS_Bounds context of the DWFRenderer
void DWFRenderer::AddDWFContent(RS_InputStream*   in,
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

                        DWFCORE_FREE_OBJECT( piResources );
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

                        DWFCORE_FREE_OBJECT( piResources );
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
                dump_bookmarks( pRoot );
                }
                }
                */

            }

            DWFCORE_FREE_OBJECT(iSection);
        }

    }
    catch (DWFException& )
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
const WT_Logical_Point* DWFRenderer::ProcessW2DPoints(  WT_File&             file,
                                                        WT_Logical_Point*    srcpts,
                                                        int                  numpts,
                                                        LineBuffer::GeomOperationType clipType,
                                                        int&                 outNumpts,
                                                        std::vector<int>**   outCntrs
                                                      )
{
    //This transformer may have been modified if a Viewport
    //opcode was encountered in the source W2D. This is needed for
    //support for symbols from ACAD, which do not use the
    //standard W2D extent used by Studio.
    //The function UpdateSymbolTrans does the actual modification and is
    //called by the process_viewport callback.
    SymbolTrans* trans = (SymbolTrans*)m_xformer;

    WT_Matrix xform = file.desired_rendition().drawing_info().units().dwf_to_application_adjoint_transform();

    LineBuffer* lb = m_lbPool.NewLineBuffer(numpts);
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
            LineBuffer* lbc = lb->Clip(GetBounds(), clipType, &m_lbPool);

            if (lbc == NULL)
            {
                //points are fully outside bounds, return 0

                m_lbPool.FreeLineBuffer(lb);
                outNumpts = 0;
                return NULL;
            }

            //if the polygon needed clipping, use the clipped version from now in
            if (lbc != lb)
            {
                m_lbPool.FreeLineBuffer(lb);
                lb = lbc;
            }
        }
    }

    //
    // Transfer contours to return vector
    //
    m_cntrs.clear();

    for (int i=0; i<lb->cntr_count(); i++)
        m_cntrs.push_back(lb->cntrs()[i]);

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
            _TransformPointsNoClamp(lb->points(), lb->point_count());
        }
    }
    else
    {
        //for symbols just copy the points to the output array without
        //transofrming to mapping space
        double* srcpts = lb->points();
        int count = lb->point_count();

        EnsureBufferSize(count);
        WT_Logical_Point* wpts = (WT_Logical_Point*)m_wtPointBuffer;

        for (int i=0; i<count; i++)
        {
            wpts[i].m_x = (WT_Integer32)*srcpts++;
            wpts[i].m_y = (WT_Integer32)*srcpts++;
        }
    }

    outNumpts = lb->point_count();

    //free clipped buffer
    m_lbPool.FreeLineBuffer(lb);

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
WT_Integer32 DWFRenderer::ScaleW2DNumber(WT_File&     file,
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
////
////      END of DWF Rewrite and related code
////
////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
