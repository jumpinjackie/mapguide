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
#include "EPlotRenderer.h"

#include "dwfcore/File.h"
#include "dwfcore/String.h"
#include "dwfcore/MIME.h"
#include "dwfcore/UUID.h"
#include "dwfcore/StreamFileDescriptor.h"
#include "dwfcore/FileInputStream.h"
#include "dwfcore/BufferOutputStream.h"
#include "dwfcore/BufferInputStream.h"

#include "dwf/whiptk/whip_toolkit.h"
#include "dwf/Version.h"
#include "dwf/package/Constants.h"
#include "dwf/package/Interface.h"
#include "dwf/package/EPlotSection.h"
#include "dwf/package/DefinedObject.h"
#include "dwf/package/writer/DWF6PackageWriter.h"
#include "dwf/package/writer/extensions/6.01/PackageVersionExtension.h"

using namespace DWFToolkit;
using namespace DWFCore;
using namespace std;


EPlotRenderer::EPlotRenderer(const RS_String& filename,
                             double           pageWidth,
                             double           pageHeight,
                             const RS_String& pageUnits)
                             : DWFRenderer(), m_dMapOffsetX(0), m_dMapOffsetY(0), m_bLayoutPlot(false)
{
    m_dPageWidth = pageWidth;
    m_dPageHeight = pageHeight;

    m_dwfFilename = filename;
    m_pageUnits = pageUnits;

    m_uuid = new DWFUUID;

    m_dMapWidth = m_dPageWidth;
    m_dMapHeight = m_dPageHeight;
    m_dMapBoundsHeight = m_dPageHeight;

    // Set the cache for the EPlot sections to be added to the output DWF
    DWFFile oDWF( m_dwfFilename.c_str() );

    DWF6PackageVersionExtension* pVersionExtension =
        DWFCORE_ALLOC_OBJECT(DWFPackageVersionTypeInfoExtension(DWFInterface::kzEPlot_ID));

    m_dwfPackageWriter = new DWF6PackageWriter ( oDWF, L"", pVersionExtension);
}


EPlotRenderer::~EPlotRenderer()
{
    delete m_uuid;

    delete m_dwfPackageWriter;
    m_dwfPackageWriter = NULL;
}


void EPlotRenderer::StartMap(RS_MapUIInfo*    mapInfo,
                             RS_Bounds&       extents,
                             double           mapScale,
                             double           dpi,
                             double           metersPerUnit,
                             CSysTransformer* /*xformToLL*/)
{
    m_bFirst = true;

    //init super

    DWFRenderer::StartMap(mapInfo, extents, mapScale, dpi, metersPerUnit, NULL);
    //
    // create an eplot
    //

    //
    // where did this dwf page originate?
    //
    DWFSource oSource( mapInfo->name().c_str(), MAP_PRODUCT_NAME, L"" );

    //
    // we will be adding a graphic resource to this page (namely the W2D)
    // this dictates that we define paper for this section
    // TODO: here we can play with the portrait/landscape orientation
    // of the page to do a better fit -- use code below as
    // guideline, as it is out of date
    //
    /*
            // check if aspect of page vs image don't match and rotate page if not.
            if (((aspectRatios[0]) > 1.0) ^
                (( pageSize.Width / pageSize.Height) > 1.0))
            {
                float temp = pageSize.Width;
                pageSize.Width = pageSize.Height;
                pageSize.Height = temp;

                temp = marginMin.Width;
                marginMin.Width = marginMin.Height;
                marginMin.Height = temp;

                temp = marginMax.Width;
                marginMax.Width = marginMax.Height;
                marginMax.Height = temp;
            }
    */

    DWFPaper::teUnits units = DWFPaper::eInches;

    if (_wcsnicmp(m_pageUnits.c_str(), L"mm", 3) == 0 ||
        _wcsnicmp(m_pageUnits.c_str(), L"millimeters", 12) == 0 )
    {
        units = DWFPaper::eMillimeters;
    }

    double anPaperClip[4] = { 0,
                              0,
                              m_dPageWidth,
                              m_dPageHeight};

    DWFPaper oPaper( m_dPageWidth,
                     m_dPageHeight,
                     units,
                     mapInfo->bgcolor().argb(),
                     (const double*)anPaperClip );

    m_pPage = DWFCORE_ALLOC_OBJECT( DWFEPlotSection(mapInfo->name().c_str(),
                                                    m_uuid->uuid(false),
                                                    1,
                                                    oSource,
                                                    mapInfo->bgcolor().argb(),
                                                    &oPaper) );
}


void EPlotRenderer::EndMap()
{
    //add all the layer resources to the section

    try
    {
        //////////////////////////////////////////////////////////////////////
        // Page layout transformations
        //////////////////////////////////////////////////////////////////////

        //net map width on the page
        double pgw = m_dMapWidth;                                      // m_dMapWidth is initialized to be equal to m_dPageWidth
                                                                       // Calling MgPrintLayout::ComputeMapOffsetAndSize() will
                                                                       // set m_dMapWidth for generating print layout

        //net map height on the page
        double pgh = m_dMapHeight;                                     // m_dMapHeight is initialized to be equal to m_dPageHeight
                                                                       // Calling MgPrintLayout::ComputeMapOffsetAndSize() will
                                                                       // set m_dMapHeight for generating print layout

        //net page aspect
        double pgar = pgw / pgh;

        RS_Bounds b = GetBounds();
        double mapar = b.width() / b.height();

        double scaleToPage = 1.0;

        double tx = 0.0, ty = 0.0;

        if (mapar >= pgar)
        {
            //map is wider than page (in x direction)
            if (mapar >= 1.0)
                scaleToPage = pgw;
            else
                scaleToPage = pgw / mapar;

            //if we are using this EplotRenderer for a print preview
            //then the positioning is done by MgPrintLayout::ComputeMapOffsetAndSize()
            //so do not try and center the map here
            if (!IsLayoutPlot())
            {
                //take up the full horizontal extent available
                tx = (m_dPageWidth - pgw) * 0.5;

                //move map up to center it vertically
                ty = (m_dPageHeight - pgw / mapar) * 0.5;
            }
        }
        else
        {
            //map is thinner and taller than page (in y direction)
            if (mapar <= 1.0)
                scaleToPage = pgh;
            else
                scaleToPage = pgh * mapar;

            //if we are using this EplotRenderer for a print preview
            //then the positioning is done by MgPrintLayout::ComputeMapOffsetAndSize()
            //so do not try and center the map here
            if (!IsLayoutPlot())
            {
                //move to the right so that it is centered horizontally
                tx = (m_dPageWidth - pgh * mapar) * 0.5;

                //we will take up the full vertical extent available
                ty = (m_dPageHeight - pgh) * 0.5;
            }
        }

        if (IsLayoutPlot())
        {
            // Add offset amounts to the translation values.
            // The values of m_dMapOffsetX and m_dMapOffsetY are initialized to zero and are
            // set when MgPrintLayout::ComputeMapOffsetAndSize() is called for generating print layout.
            tx = m_dMapOffsetX;
            ty = m_dMapOffsetY;
        }

        //W2D to page transform for map
        // Define a transformation that scales and translates the map to the map box
        double anTransform[4][4] = {
            { scaleToPage/(double)(INT_MAX/2), 0, 0, 0 },
            { 0, scaleToPage/(double)(INT_MAX/2), 0, 0 },
            { 0, 0, 1, 0 },
            { tx - pgw*0.5, ty - pgh*0.5, 0, 1 }
            //note the half page size correction is always there because
            //of the way the W2D is written
        };

        //W2D to page transformation for layout elements
        // Need to define a transformation that scales the layout to the page
        double maxdim2 = rs_max(m_dPageHeight, m_dPageWidth);
        double scale2 = maxdim2 /(double)(INT_MAX/2);
        double anTransform2[4][4] = {
            scale2, 0, 0, 0,
            0, scale2, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1 };

        //clip rect for layout elements -> it's the whole page
        double layoutClip[4] = { 0,
                                 0,
                                 m_dPageWidth,
                                 m_dPageHeight};

        //clip rect for map data -> restrict to map area (will be different
        //in the case where we are doing a print layout)
        double mapClip[4] = { m_dMapOffsetX,
                              m_dMapOffsetY,
                              m_dMapWidth + m_dMapOffsetX,
                              m_dMapHeight + m_dMapOffsetY};

        //make sure W2D to page transform is applied -- this hidden flag is needed
        //otherwise the viewer will ignore that transform
        m_pPage->addProperty(DWFCORE_ALLOC_OBJECT(DWFProperty(L"_CompleteTransform", L"true", L"hidden", L"", L"")), true);

        //////////////////////////////////////////////////////////////////////
        // Feature Data resource W2D (one W2D per each map layer)
        //////////////////////////////////////////////////////////////////////
        objdefres_list::iterator oditer = m_lAttributeResources.begin();
        stream_list::iterator iter = m_lLayerStreams.begin();
        for ( ; iter != m_lLayerStreams.end(); iter++, ++oditer)
        {
            if (m_bFirst)
            {
                AddW2DResource( *iter,
                                *oditer,
                                (const double*)anTransform,
                                (const double*)mapClip,
                                DWFXML::kzRole_Graphics2d,
                                L"Layer name"); //TODO:
                m_bFirst = false;
            }
            else
            {
                AddW2DResource( *iter,
                                *oditer,
                                (const double*)anTransform,
                                (const double*)mapClip,
                                DWFXML::kzRole_Graphics2dOverlay,
                                L"Layer name");
            }
        }

        //IMPORTANT
        //Since we added the object definition resources to the EPlot
        //section, it now owns them. We need to clear the object resources
        //collection so that DWFRenderer does not clean them out.
        //TODO: this will be cleaned up once
        m_lAttributeResources.clear();

        //////////////////////////////////////////////////////////////////////
        // Label resource W2D
        //////////////////////////////////////////////////////////////////////

        for (stream_list::iterator iter = m_lLabelStreams.begin();
            iter != m_lLabelStreams.end(); ++iter)
        {
            if (*iter)
            {
                AddW2DResource( *iter,
                    NULL, //no attribute data for labels
                    (const double*)anTransform,
                    (const double*)mapClip,
                    DWFXML::kzRole_Graphics2dOverlay,
                    L"Layer name - Labels");
            }
        }

        //////////////////////////////////////////////////////////////////////
        // Layout resource W2D
        //////////////////////////////////////////////////////////////////////

        for (stream_list::iterator iter = m_lLayoutStreams.begin();
            iter != m_lLayoutStreams.end(); ++iter)
        {
            AddW2DResource( *iter,
                NULL,
                (const double*)anTransform2,
                (const double*)layoutClip,
                DWFXML::kzRole_Graphics2dOverlay,
                L"Layer name - Layout");
        }

        for (stream_list::iterator iter = m_lLayoutLabelStreams.begin();
            iter != m_lLayoutLabelStreams.end(); ++iter)
        {
            if (NULL != *iter)
            {
                AddW2DResource( *iter,
                    NULL,
                    (const double*)anTransform2,
                    (const double*)layoutClip,
                    DWFXML::kzRole_Graphics2dOverlay,
                    L"Layer name - Layout Labels");
            }
        }

        //////////////////////////////////////////////////////////////////////
        // Write out the DWF
        //////////////////////////////////////////////////////////////////////

        m_dwfPackageWriter->addSection( m_pPage );
    }
    catch (DWFException& )
    {
        //TODO:
        //throw MgException or may be not even catch?
    }

    //NOTE: we should not free the EPlotSection since the DWF owns it
    m_pPage = NULL;

    //////////////////////////////////////////////////////////////////////
    // If we call DWFRenderer::EndMap() right now then it will free all
    // the output streams in the layer/label/layout stream lists, and then
    // later when we try to write the DWF we crash.  Instead, let's move
    // the streams out of the layer/label/layout lists and into another
    // set of lists which gets cleaned up after we write the DWF.
    //////////////////////////////////////////////////////////////////////

    for (stream_list::iterator iter = m_lLayerStreams.begin();
        iter != m_lLayerStreams.end(); ++iter)
    {
        m_lSectionStreams.push_back(*iter);
    }

    for (stream_list::iterator iter = m_lLabelStreams.begin();
        iter != m_lLabelStreams.end(); ++iter)
    {
        m_lSectionLabelStreams.push_back(*iter);
    }

    for (stream_list::iterator iter = m_lLayoutStreams.begin();
        iter != m_lLayoutStreams.end(); ++iter)
    {
        m_lSectionLayoutStreams.push_back(*iter);
    }

    for (stream_list::iterator iter = m_lLayoutLabelStreams.begin();
        iter != m_lLayoutLabelStreams.end(); ++iter)
    {
        m_lSectionLayoutLabelStreams.push_back(*iter);
    }

    m_lLayerStreams.clear();
    m_lLabelStreams.clear();
    m_lLayoutStreams.clear();
    m_lLayoutLabelStreams.clear();

    // now we can call the super
    DWFRenderer::EndMap();
}


void EPlotRenderer::StartLayer(RS_LayerUIInfo*      layerInfo,
                               RS_FeatureClassInfo* classInfo)
{
    // init super first
    DWFRenderer::StartLayer(layerInfo, classInfo);

    // add a viewport so that entities get properly clipped
    WT_Logical_Point points[4];
    RS_Bounds& extent = GetBounds();
    points[0].m_x = (int)_TX(extent.minx);
    points[0].m_y = (int)_TY(extent.miny);
    points[1].m_x = (int)_TX(extent.maxx);
    points[1].m_y = (int)_TY(extent.miny);
    points[2].m_x = (int)_TX(extent.maxx);
    points[2].m_y = (int)_TY(extent.maxy);
    points[3].m_x = (int)_TX(extent.minx);
    points[3].m_y = (int)_TY(extent.maxy);

    WT_Viewport viewport1(*m_w2dFile, "", 4, points);
    WT_Viewport viewport2(*m_w2dLabels, "", 4, points);
    m_w2dFile->desired_rendition().viewport() = viewport1;
    m_w2dLabels->desired_rendition().viewport() = viewport2;
}


void EPlotRenderer::EndLayer()
{
    DWFRenderer::EndLayer();
}


void EPlotRenderer::Done()
{
    m_dwfPackageWriter->write( MAP_PRODUCT_AUTHOR,
                               MAP_PRODUCT_NAME,
                               MAP_PRODUCT_VERSION,
                               MAP_PRODUCT_AUTHOR,
                               _DWFTK_VERSION_STRING,
                               DWFZipFileDescriptor::eZipSmallest);

    // now that we've created written out the map, we can clean up
    // all the streams
    for (stream_list::iterator iter = m_lSectionStreams.begin();
        iter != m_lSectionStreams.end(); ++iter)
    {
        DWFCORE_FREE_OBJECT ( *iter);
    }

    for (stream_list::iterator iter = m_lSectionLabelStreams.begin();
        iter != m_lSectionLabelStreams.end(); ++iter)
    {
        DWFCORE_FREE_OBJECT ( *iter);
    }

    for (stream_list::iterator iter = m_lSectionLayoutStreams.begin();
        iter != m_lSectionLayoutStreams.end(); ++iter)
    {
        DWFCORE_FREE_OBJECT( *iter );
    }

    for (stream_list::iterator iter = m_lSectionLayoutLabelStreams.begin();
        iter != m_lSectionLayoutLabelStreams.end(); ++iter)
    {
        DWFCORE_FREE_OBJECT( *iter );
    }

    m_lSectionStreams.clear();
    m_lSectionLabelStreams.clear();
    m_lSectionLayoutStreams.clear();
    m_lSectionLayoutLabelStreams.clear();
}


// Disable this optimization for EPlots.  When the plotted DWF is
// zoomed in using the DWF viewer, we want to see text and not lines.
bool EPlotRenderer::OptimizeGeometry()
{
    return false;
}


//TODO: fold this function and its twin residing in EMapUpdateRenderer
//into DWFRenderer.
void EPlotRenderer::AddW2DResource(DWFCore::DWFBufferOutputStream* w2dStream,
                                   DWFToolkit::DWFObjectDefinitionResource* objdefs,
                                   const double*  transform,
                                   const double*  clip,
                                   const wchar_t* role,
                                   const wchar_t* name)
{
    //
    // define the resource - this must be allocated on the heap
    //
    DWFGraphicResource* p2Dgfx =
        DWFCORE_ALLOC_OBJECT( DWFGraphicResource(name,              // title
                                                role,               //DWFXML::kzRole_Graphics2d, // role
                                                DWFMIME::kzMIMEType_W2D,  // MIME type
                                                L"",                // author
                                                L"",                // description
                                                L"",                // creation time
                                                L"") );             // modification time

    if (p2Dgfx == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate resource" );
    }

    p2Dgfx->configureGraphic( (const double*)transform,
                            NULL,
                            (const double*)clip );

    //create an input stream out of the W2D output stream
    //Note the output stream will be freed by DWFRenderer::EndMap
    //which happens after the DWF package is created so everything is cool
    DWFBufferInputStream* is = DWFCORE_ALLOC_OBJECT(DWFBufferInputStream(
        w2dStream->buffer(), w2dStream->bytes()));

    if (is == NULL)
    {
        DWFCORE_FREE_OBJECT( p2Dgfx );

        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate file stream" );
    }

    p2Dgfx->setInputStream( is );

    p2Dgfx->setObjectID(m_uuid->next(false));

    //
    // finally, drop the resource into the page
    //
    m_pPage->addResource( p2Dgfx, true );

    ///////////////////////////////////////////////////////////////////
    // Add the object attribute data to the section
    ///////////////////////////////////////////////////////////////////
    if (objdefs)
    {
        objdefs->setParentObjectID(p2Dgfx->objectID());
        m_pPage->addResource(objdefs, true);
    }
}


void EPlotRenderer::SetPageWidth(double width)
{
    m_dPageWidth = width;
}


void EPlotRenderer::SetPageHeight(double height)
{
    m_dPageHeight = height;
}


void EPlotRenderer::SetPageSizeUnits(std::wstring units)
{
    m_pageUnits = units;
}


void EPlotRenderer::SetMapWidth(double width)
{
    m_dMapWidth = width;
}


void EPlotRenderer::SetMapHeight(double height)
{
    m_dMapHeight = height;
}


void EPlotRenderer::EnableLayoutPlot()
{
    m_bLayoutPlot = true;
}


bool EPlotRenderer::IsLayoutPlot()
{
    return m_bLayoutPlot;
}
