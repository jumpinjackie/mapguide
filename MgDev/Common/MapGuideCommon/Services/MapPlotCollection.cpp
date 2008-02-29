//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

MG_IMPL_DYNCREATE(MgMapPlotCollection)

// Constructs an empty MgMapPlotCollection object.
//
MgMapPlotCollection::MgMapPlotCollection()
{
    m_mapPlots = new MgNamedCollection();
}

// Returns the number of items in the collection
//
INT32 MgMapPlotCollection::GetCount()
{
    return m_mapPlots->GetCount();
}

// Returns the item at the specified index
// Throws an out of range exception if the index is invalid
//
MgMapPlot* MgMapPlotCollection::GetItem(INT32 index)
{
    //returned value is addref'd by m_maps base class
    return (MgMapPlot*)m_mapPlots->GetItem(index);
}

// Returns the item with the specified name
// Throws an illegal argument exception if the name does not exist
//
MgMapPlot* MgMapPlotCollection::GetItem(CREFSTRING name)
{
    //returned value is addref'd by m_maps base class
    return (MgMapPlot*)m_mapPlots->GetItem(name);
}

// Sets the item in the collection at the specified index to the specified value
// Throws an out of range exception if the index is out of range.
//
void MgMapPlotCollection::SetItem(INT32 index, MgMapPlot* value)
{
    //value is addref'd by m_maps base class
    m_mapPlots->SetItem(index, value);
}

// Adds the specified item to the end of the collection.
//
void MgMapPlotCollection::Add(MgMapPlot* value)
{
    //value is addref'd by m_mapPlots base class
    m_mapPlots->Add(value);
}

// Inserts the specified item at the specified index within the collection.
// Items following the insertion point are moved down to accommodate the new item.
// Throws an out of range argument exception if the specified index is out of range
//
void MgMapPlotCollection::Insert(INT32 index, MgMapPlot* value)
{
    //value is addref'd by m_mapPlots base class
    m_mapPlots->Insert(index, value);
}

/// <summary>
/// Removes all items from the collection
/// </summary>
void MgMapPlotCollection::Clear()
{
    m_mapPlots->Clear();
}

// Removes an item from the collection
// Throws an invalid argument exception if the item does not exist within the collection.
//
bool MgMapPlotCollection::Remove(MgMapPlot* value)
{
    bool removed = true;
    try
    {
        //value is released by m_maps base class
        m_mapPlots->Remove(value);
    }
    catch (MgException* e)
    {
        e->Release();
        removed = false;
    }
    return removed;
}

// Removes an item from the collection at the specified index
// Throws an out of range exception if the item does not exist within the collection.
//
void MgMapPlotCollection::RemoveAt(INT32 index)
{
    //value at index is released by m_maps base class
    m_mapPlots->RemoveAt(index);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgMapPlotCollection::Contains(CREFSTRING name)
{
    return m_mapPlots->Contains(name);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgMapPlotCollection::Contains(MgMapPlot* value)
{
    return m_mapPlots->Contains(value);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgMapPlotCollection::IndexOf(CREFSTRING name)
{
    return m_mapPlots->IndexOf(name);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgMapPlotCollection::IndexOf(MgMapPlot* value)
{
    return m_mapPlots->IndexOf(value);
}

// Destruct a MgMapPlotCollection object
//
MgMapPlotCollection::~MgMapPlotCollection()
{
    SAFE_RELEASE(m_mapPlots);
}


// Serialize data to a stream
//
void MgMapPlotCollection::Serialize(MgStream* stream)
{
    INT32 count = this->GetCount();
    stream->WriteInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgMapPlot> mapPlot = this->GetItem(i);
        Ptr<MgMap> map = mapPlot->GetMap();
        Ptr<MgPlotSpecification> plotSpec = mapPlot->GetPlotSpecification();
        Ptr<MgLayout> layout = mapPlot->GetLayout();
        Ptr<MgCoordinate> center = mapPlot->GetCenter();
        Ptr<MgEnvelope> extent = mapPlot->GetExtent();

        stream->WriteObject(map);
        stream->WriteObject(plotSpec);
        stream->WriteObject(layout);
        stream->WriteDouble(center->GetX());
        stream->WriteDouble(center->GetY());
        stream->WriteDouble(mapPlot->GetScale());
        stream->WriteObject(extent);
        stream->WriteInt32(mapPlot->GetMapPlotInstruction());
        stream->WriteBoolean(mapPlot->GetExpandToFit());
    }
}

// Deserialize data from a stream
//
void MgMapPlotCollection::Deserialize(MgStream* stream)
{
    MgStreamReader* streamReader = (MgStreamReader*)stream;

    INT32 count = 0;
    streamReader->GetInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgMap> map = (MgMap*)streamReader->GetObject();
        Ptr<MgPlotSpecification> plotSpec = (MgPlotSpecification*)streamReader->GetObject();
        Ptr<MgLayout> layout = (MgLayout*)streamReader->GetObject();
        double centerX = 0.0;
        streamReader->GetDouble(centerX);
        double centerY = 0.0;
        streamReader->GetDouble(centerY);
        Ptr<MgCoordinate> center = new MgCoordinateXY(centerX, centerY);
        double scale = 0.0;
        streamReader->GetDouble(scale);
        Ptr<MgEnvelope> extent = (MgEnvelope*)streamReader->GetObject();
        INT32 plotInstruction = 0;
        streamReader->GetInt32(plotInstruction);
        bool expandToFit = false;
        streamReader->GetBoolean(expandToFit);

        Ptr<MgMapPlot> mapPlot;
        switch (plotInstruction)
        {
        case MgMapPlotInstruction::UseMapCenterAndScale:
            mapPlot = new MgMapPlot(map, plotSpec, layout);
            break;
        case MgMapPlotInstruction::UseOverriddenCenterAndScale:
            mapPlot = new MgMapPlot(map, center, scale, plotSpec, layout);
            break;
        case MgMapPlotInstruction::UseOverriddenExtent:
            mapPlot = new MgMapPlot(map, extent, expandToFit, plotSpec, layout);
            break;
        default:
            break;
        }

        this->Add(mapPlot);
    }
}
