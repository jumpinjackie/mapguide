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

#include "MgDesktop.h"
#include "MapPlotInstruction.h"


//////////////////////////////////////////////////////////////
// Default constructor
MgdMapPlot::MgdMapPlot()
{
}


//////////////////////////////////////////////////////////////
// Create and initialize an MgdMapPlot instance.
MgdMapPlot::MgdMapPlot(MgdMap* map, MgdPlotSpecification* plotSpec, MgdLayout* layout)
{
    if (NULL == map || NULL == plotSpec)
    {
        throw new MgNullArgumentException(L"MgdMapPlot.MgdMapPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Initialize();

    m_map = SAFE_ADDREF(map);
    m_plotSpec = SAFE_ADDREF(plotSpec);
    m_layout = SAFE_ADDREF(layout);

    m_plotInstruction = MgdMapPlotInstruction::UseMapCenterAndScale;
}


//////////////////////////////////////////////////////////////
MgdMapPlot::MgdMapPlot(MgdMap* map, MgCoordinate* center, double scale,
                     MgdPlotSpecification* plotSpec, MgdLayout* layout)
{
    if (NULL == map || NULL == center || NULL == plotSpec)
    {
        throw new MgNullArgumentException(L"MgdMapPlot.MgdMapPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Initialize();

    m_map = SAFE_ADDREF(map);
    SAFE_RELEASE(m_center);
    m_center = SAFE_ADDREF(center);
    m_scale = scale;
    m_plotSpec = SAFE_ADDREF(plotSpec);
    m_layout = SAFE_ADDREF(layout);

    m_plotInstruction = MgdMapPlotInstruction::UseOverriddenCenterAndScale;
}


//////////////////////////////////////////////////////////////
MgdMapPlot::MgdMapPlot(MgdMap* map, MgEnvelope* extent, bool expandToFit,
                     MgdPlotSpecification* plotSpec, MgdLayout* layout)
{
    if (map == NULL || extent == NULL || plotSpec == NULL)
    {
        throw new MgNullArgumentException(L"MgdMapPlot.MgdMapPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Initialize();

    m_map = SAFE_ADDREF(map);
    SAFE_RELEASE(m_extent);
    m_extent = SAFE_ADDREF(extent);
    m_bExpandToFit = expandToFit;
    m_plotSpec = SAFE_ADDREF(plotSpec);
    m_layout = SAFE_ADDREF(layout);

    m_plotInstruction = MgdMapPlotInstruction::UseOverriddenExtent;
}


//////////////////////////////////////////////////////////////
// Initialize class members which may not be set by the constructors
void MgdMapPlot::Initialize()
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
MgdMap* MgdMapPlot::GetMap()
{
    return SAFE_ADDREF(m_map);
}


//////////////////////////////////////////////////////////////
// Set the MgMap to plot.
void MgdMapPlot::SetMap(MgdMap* map)
{
    SAFE_RELEASE(m_map);
    m_map = SAFE_ADDREF(map);
}


//////////////////////////////////////////////////////////////
// Get the MapPlotInstruction, which defines how the view of the map
// to be plotted is defined.
INT32 MgdMapPlot::GetMapPlotInstruction()
{
    return m_plotInstruction;
}


//////////////////////////////////////////////////////////////
// Set the MapPlotInstruction.
void MgdMapPlot::SetMapPlotInstruction(INT32 plotInstruction)
{
    m_plotInstruction = plotInstruction;
}


//////////////////////////////////////////////////////////////
// Get and the view center.
MgCoordinate* MgdMapPlot::GetCenter()
{
    return SAFE_ADDREF(m_center);
}


//////////////////////////////////////////////////////////////
// Get the view scale.
double MgdMapPlot::GetScale()
{
    return m_scale;
}


//////////////////////////////////////////////////////////////
// Set the view center and scale.
void MgdMapPlot::SetCenterAndScale(MgCoordinate* center, double scale)
{
    SAFE_RELEASE(m_center);
    m_center = SAFE_ADDREF(center);
    m_scale = scale;
    m_plotInstruction = MgdMapPlotInstruction::UseOverriddenCenterAndScale;
}


//////////////////////////////////////////////////////////////
// Get the extent.
MgEnvelope* MgdMapPlot::GetExtent()
{
    return SAFE_ADDREF(m_extent);
}


//////////////////////////////////////////////////////////////
// Get the expand to fit flag.
bool MgdMapPlot::GetExpandToFit()
{
    return m_bExpandToFit;
}


//////////////////////////////////////////////////////////////
// Set the extent and expand to fit flag.
void MgdMapPlot::SetExtent(MgEnvelope* extent, bool expandToFit)
{
    SAFE_RELEASE(m_extent);
    m_extent = SAFE_ADDREF(extent);
    m_bExpandToFit = expandToFit;
    m_plotInstruction = MgdMapPlotInstruction::UseOverriddenExtent;
}


//////////////////////////////////////////////////////////////
// Get the plot specification
MgdPlotSpecification* MgdMapPlot::GetPlotSpecification()
{
    return SAFE_ADDREF(m_plotSpec);
}


//////////////////////////////////////////////////////////////
// Set the plot specification.
void MgdMapPlot::SetPlotSpecification(MgdPlotSpecification* plotSpec)
{
    SAFE_RELEASE(m_plotSpec);
    m_plotSpec = SAFE_ADDREF(plotSpec);
}


//////////////////////////////////////////////////////////////
// Get the layout.  This value is optional; if null the map will fill
// the entire page area within the margins.
MgdLayout* MgdMapPlot::GetLayout()
{
    return SAFE_ADDREF(m_layout);
}


//////////////////////////////////////////////////////////////
// Set the layout.
void MgdMapPlot::SetLayout(MgdLayout* layout)
{
    SAFE_RELEASE(m_layout);
    m_layout = SAFE_ADDREF(layout);
}


//////////////////////////////////////////////////////////////
// Destructor
MgdMapPlot::~MgdMapPlot()
{
    SAFE_RELEASE(m_map);
    SAFE_RELEASE(m_plotSpec);
    SAFE_RELEASE(m_layout);
    SAFE_RELEASE(m_center);
    SAFE_RELEASE(m_extent);
}


//////////////////////////////////////////////////////////////
// Serialize data to TCP/IP stream
void MgdMapPlot::Serialize(MgStream* stream)
{
    throw new MgNotImplementedException(L"MgdMapPlot.Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////
// Deserialize data from TCP/IP stream
void MgdMapPlot::Deserialize(MgStream* stream)
{
    throw new MgNotImplementedException(L"MgdMapPlot.Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////
// Get the name of object
STRING MgdMapPlot::GetName()
{
    return m_map->GetName();
}


//////////////////////////////////////////////////////////////
// Does it allow to set the name
bool MgdMapPlot::CanSetName()
{
    // indirectly via SetMap
    return true;
}
