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

MG_IMPL_DYNCREATE(MgdMapPlotCollection)

// Constructs an empty MgdMapPlotCollection object.
//
MgdMapPlotCollection::MgdMapPlotCollection()
{
    m_mapPlots = new MgNamedCollection();
}

// Returns the number of items in the collection
//
INT32 MgdMapPlotCollection::GetCount()
{
    return m_mapPlots->GetCount();
}

// Returns the item at the specified index
// Throws an out of range exception if the index is invalid
//
MgdMapPlot* MgdMapPlotCollection::GetItem(INT32 index)
{
    //returned value is addref'd by m_maps base class
    return (MgdMapPlot*)m_mapPlots->GetItem(index);
}

// Returns the item with the specified name
// Throws an illegal argument exception if the name does not exist
//
MgdMapPlot* MgdMapPlotCollection::GetItem(CREFSTRING name)
{
    //returned value is addref'd by m_maps base class
    return (MgdMapPlot*)m_mapPlots->GetItem(name);
}

// Sets the item in the collection at the specified index to the specified value
// Throws an out of range exception if the index is out of range.
//
void MgdMapPlotCollection::SetItem(INT32 index, MgdMapPlot* value)
{
    //value is addref'd by m_maps base class
    m_mapPlots->SetItem(index, value);
}

// Adds the specified item to the end of the collection.
//
void MgdMapPlotCollection::Add(MgdMapPlot* value)
{
    //value is addref'd by m_mapPlots base class
    m_mapPlots->Add(value);
}

// Inserts the specified item at the specified index within the collection.
// Items following the insertion point are moved down to accommodate the new item.
// Throws an out of range argument exception if the specified index is out of range
//
void MgdMapPlotCollection::Insert(INT32 index, MgdMapPlot* value)
{
    //value is addref'd by m_mapPlots base class
    m_mapPlots->Insert(index, value);
}

/// <summary>
/// Removes all items from the collection
/// </summary>
void MgdMapPlotCollection::Clear()
{
    m_mapPlots->Clear();
}

// Removes an item from the collection
// Throws an invalid argument exception if the item does not exist within the collection.
//
bool MgdMapPlotCollection::Remove(MgdMapPlot* value)
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
void MgdMapPlotCollection::RemoveAt(INT32 index)
{
    //value at index is released by m_maps base class
    m_mapPlots->RemoveAt(index);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgdMapPlotCollection::Contains(CREFSTRING name)
{
    return m_mapPlots->Contains(name);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgdMapPlotCollection::Contains(MgdMapPlot* value)
{
    return m_mapPlots->Contains(value);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgdMapPlotCollection::IndexOf(CREFSTRING name)
{
    return m_mapPlots->IndexOf(name);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgdMapPlotCollection::IndexOf(MgdMapPlot* value)
{
    return m_mapPlots->IndexOf(value);
}

// Destruct a MgdMapPlotCollection object
//
MgdMapPlotCollection::~MgdMapPlotCollection()
{
    SAFE_RELEASE(m_mapPlots);
}


// Serialize data to a stream
//
void MgdMapPlotCollection::Serialize(MgStream* stream)
{
    INT32 count = this->GetCount();
    stream->WriteInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgdMapPlot> mapPlot = this->GetItem(i);
        Ptr<MgdMap> map = mapPlot->GetMap();
        Ptr<MgdPlotSpecification> plotSpec = mapPlot->GetPlotSpecification();
        Ptr<MgdLayout> layout = mapPlot->GetLayout();
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
void MgdMapPlotCollection::Deserialize(MgStream* stream)
{
    MgStreamReader* streamReader = (MgStreamReader*)stream;

    INT32 count = 0;
    streamReader->GetInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgdMap> map = (MgdMap*)streamReader->GetObject();
        Ptr<MgdPlotSpecification> plotSpec = (MgdPlotSpecification*)streamReader->GetObject();
        Ptr<MgdLayout> layout = (MgdLayout*)streamReader->GetObject();
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

        Ptr<MgdMapPlot> mapPlot;
        switch (plotInstruction)
        {
        case MgdMapPlotInstruction::UseMapCenterAndScale:
            mapPlot = new MgdMapPlot(map, plotSpec, layout);
            break;
        case MgdMapPlotInstruction::UseOverriddenCenterAndScale:
            mapPlot = new MgdMapPlot(map, center, scale, plotSpec, layout);
            break;
        case MgdMapPlotInstruction::UseOverriddenExtent:
            mapPlot = new MgdMapPlot(map, extent, expandToFit, plotSpec, layout);
            break;
        default:
            break;
        }

        this->Add(mapPlot);
    }
}
