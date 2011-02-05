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

#include "MapGuideCommon.h"
#include "MapPlotInstruction.h"


//////////////////////////////////////////////////////////////
// Default constructor
MgMapPlot::MgMapPlot()
{
}


//////////////////////////////////////////////////////////////
// Create and initialize an MgMapPlot instance.
MgMapPlot::MgMapPlot(MgMap* map, MgPlotSpecification* plotSpec, MgLayout* layout)
{
    if (NULL == map || NULL == plotSpec)
    {
        throw new MgNullArgumentException(L"MgMapPlot.MgMapPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Initialize();

    m_map = SAFE_ADDREF(map);
    m_plotSpec = SAFE_ADDREF(plotSpec);
    m_layout = SAFE_ADDREF(layout);

    m_plotInstruction = MgMapPlotInstruction::UseMapCenterAndScale;
}


//////////////////////////////////////////////////////////////
MgMapPlot::MgMapPlot(MgMap* map, MgCoordinate* center, double scale,
                     MgPlotSpecification* plotSpec, MgLayout* layout)
{
    if (NULL == map || NULL == center || NULL == plotSpec)
    {
        throw new MgNullArgumentException(L"MgMapPlot.MgMapPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Initialize();

    m_map = SAFE_ADDREF(map);
    SAFE_RELEASE(m_center);
    m_center = SAFE_ADDREF(center);
    m_scale = scale;
    m_plotSpec = SAFE_ADDREF(plotSpec);
    m_layout = SAFE_ADDREF(layout);

    m_plotInstruction = MgMapPlotInstruction::UseOverriddenCenterAndScale;
}


//////////////////////////////////////////////////////////////
MgMapPlot::MgMapPlot(MgMap* map, MgEnvelope* extent, bool expandToFit,
                     MgPlotSpecification* plotSpec, MgLayout* layout)
{
    if (map == NULL || extent == NULL || plotSpec == NULL)
    {
        throw new MgNullArgumentException(L"MgMapPlot.MgMapPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Initialize();

    m_map = SAFE_ADDREF(map);
    SAFE_RELEASE(m_extent);
    m_extent = SAFE_ADDREF(extent);
    m_bExpandToFit = expandToFit;
    m_plotSpec = SAFE_ADDREF(plotSpec);
    m_layout = SAFE_ADDREF(layout);

    m_plotInstruction = MgMapPlotInstruction::UseOverriddenExtent;
}


//////////////////////////////////////////////////////////////
// Initialize class members which may not be set by the constructors
void MgMapPlot::Initialize()
{
    Ptr<MgCoordinate> initCoord = new MgCoordinateXY(0, 0);
    m_center = initCoord;
    SAFE_ADDREF(m_center);
    m_scale = 0;
    m_extent = new MgEnvelope(initCoord, initCoord);
    m_bExpandToFit = true;
    m_layout = NULL;
}


//////////////////////////////////////////////////////////////
// Get and Set the MgMap to plot.
MgMap* MgMapPlot::GetMap()
{
    return SAFE_ADDREF(m_map);
}


//////////////////////////////////////////////////////////////
// Set the MgMap to plot.
void MgMapPlot::SetMap(MgMap* map)
{
    SAFE_RELEASE(m_map);
    m_map = SAFE_ADDREF(map);
}


//////////////////////////////////////////////////////////////
// Get the MapPlotInstruction, which defines how the view of the map
// to be plotted is defined.
INT32 MgMapPlot::GetMapPlotInstruction()
{
    return m_plotInstruction;
}


//////////////////////////////////////////////////////////////
// Set the MapPlotInstruction.
void MgMapPlot::SetMapPlotInstruction(INT32 plotInstruction)
{
    m_plotInstruction = plotInstruction;
}


//////////////////////////////////////////////////////////////
// Get and the view center.
MgCoordinate* MgMapPlot::GetCenter()
{
    return SAFE_ADDREF(m_center);
}


//////////////////////////////////////////////////////////////
// Get the view scale.
double MgMapPlot::GetScale()
{
    return m_scale;
}


//////////////////////////////////////////////////////////////
// Set the view center and scale.
void MgMapPlot::SetCenterAndScale(MgCoordinate* center, double scale)
{
    SAFE_RELEASE(m_center);
    m_center = SAFE_ADDREF(center);
    m_scale = scale;
    m_plotInstruction = MgMapPlotInstruction::UseOverriddenCenterAndScale;
}


//////////////////////////////////////////////////////////////
// Get the extent.
MgEnvelope* MgMapPlot::GetExtent()
{
    return SAFE_ADDREF(m_extent);
}


//////////////////////////////////////////////////////////////
// Get the expand to fit flag.
bool MgMapPlot::GetExpandToFit()
{
    return m_bExpandToFit;
}


//////////////////////////////////////////////////////////////
// Set the extent and expand to fit flag.
void MgMapPlot::SetExtent(MgEnvelope* extent, bool expandToFit)
{
    SAFE_RELEASE(m_extent);
    m_extent = SAFE_ADDREF(extent);
    m_bExpandToFit = expandToFit;
    m_plotInstruction = MgMapPlotInstruction::UseOverriddenExtent;
}


//////////////////////////////////////////////////////////////
// Get the plot specification
MgPlotSpecification* MgMapPlot::GetPlotSpecification()
{
    return SAFE_ADDREF(m_plotSpec);
}


//////////////////////////////////////////////////////////////
// Set the plot specification.
void MgMapPlot::SetPlotSpecification(MgPlotSpecification* plotSpec)
{
    SAFE_RELEASE(m_plotSpec);
    m_plotSpec = SAFE_ADDREF(plotSpec);
}


//////////////////////////////////////////////////////////////
// Get the layout.  This value is optional; if null the map will fill
// the entire page area within the margins.
MgLayout* MgMapPlot::GetLayout()
{
    return SAFE_ADDREF(m_layout);
}


//////////////////////////////////////////////////////////////
// Set the layout.
void MgMapPlot::SetLayout(MgLayout* layout)
{
    SAFE_RELEASE(m_layout);
    m_layout = SAFE_ADDREF(layout);
}


//////////////////////////////////////////////////////////////
// Destructor
MgMapPlot::~MgMapPlot()
{
    SAFE_RELEASE(m_map);
    SAFE_RELEASE(m_plotSpec);
    SAFE_RELEASE(m_layout);
    SAFE_RELEASE(m_center);
    SAFE_RELEASE(m_extent);
}


//////////////////////////////////////////////////////////////
// Serialize data to TCP/IP stream
void MgMapPlot::Serialize(MgStream* stream)
{
    throw new MgNotImplementedException(L"MgMapPlot.Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////
// Deserialize data from TCP/IP stream
void MgMapPlot::Deserialize(MgStream* stream)
{
    throw new MgNotImplementedException(L"MgMapPlot.Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////
// Get the name of object
STRING MgMapPlot::GetName()
{
    return m_map->GetName();
}


//////////////////////////////////////////////////////////////
// Does it allow to set the name
bool MgMapPlot::CanSetName()
{
    // indirectly via SetMap
    return true;
}
