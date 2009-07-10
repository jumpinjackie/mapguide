//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "IOMapViewport.h"
#include "IOPoint3D.h"
#include "IOStringObjectCollection.h"
#include "IOMapView.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, MapViewport);
// Local Elements
ELEM_MAP_ENTRY(2, CenterPoint);
ELEM_MAP_ENTRY(3, Orientation);
ELEM_MAP_ENTRY(4, MapName);
ELEM_MAP_ENTRY(5, VisibleLayerNames);
ELEM_MAP_ENTRY(6, Locked);
ELEM_MAP_ENTRY(7, On);
ELEM_MAP_ENTRY(8, MapView);
// Sub Elements
ELEM_MAP_ENTRY(9, Name);

IOMapViewport::IOMapViewport(MapViewport* mapViewport, Version& version) :
    IOPrintLayoutElement(mapViewport, version)
{
}

IOMapViewport::~IOMapViewport()
{
}

void IOMapViewport::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eMapViewport:
        m_startElemName = name;
        break;

    case eCenterPoint:
        {
            IOPoint3D* IO = new IOPoint3D(&GetMapViewport()->GetCenterPoint(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    
    case eVisibleLayerNames:
        {
            IOStringObjectCollection* IO = new IOStringObjectCollection(&GetMapViewport()->GetVisibleLayerNames(), m_version, sVisibleLayerNames, sName);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    
    case eMapView:
        {
            IOMapView* IO = new IOMapView(&GetMapViewport()->GetMapView(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    
    default:
        IOPrintLayoutElement::StartElement(name, handlerStack);
        break;
    }
}

void IOMapViewport::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eOrientation:
        GetMapViewport()->SetOrientation(wstrToDouble(ch));
        break;

    case eMapName:
        GetMapViewport()->SetMapName(ch);
        break;

    case eLocked:
        GetMapViewport()->SetIsLocked(wstrToBool(ch));
        break;

    case eOn:
        GetMapViewport()->SetIsOn(wstrToBool(ch));
        break;

    default:
        IOPrintLayoutElement::ElementChars(ch);
        break;
    }
}

void IOMapViewport::Write(MdfStream& fd, MapViewport* mapViewport, Version& version, const std::string& name)
{
    _ASSERT(NULL != mapViewport);

    fd << tab() << startStr(name) << std::endl;
    inctab();

    IOPrintLayoutElement::Write(fd, mapViewport, version);

    // Property: CenterPoint
    IOPoint3D::Write(fd, &mapViewport->GetCenterPoint(), version, sCenterPoint);

    // Property: Orientation
    fd << tab() << startStr(sOrientation);
    fd << DoubleToStr(mapViewport->GetOrientation());
    fd << endStr(sOrientation) << std::endl;

    // Property: MapName
    fd << tab() << startStr(sMapName);
    fd << EncodeString(mapViewport->GetMapName());
    fd << endStr(sMapName) << std::endl;

    // Property: VisibleLayerNames
    IOStringObjectCollection::Write(fd, &mapViewport->GetVisibleLayerNames(), version, sVisibleLayerNames, sName);

    // Property: Locked
    fd << tab() << startStr(sLocked);
    fd << BoolToStr(mapViewport->GetIsLocked());
    fd << endStr(sLocked) << std::endl;

    // Property: On
    fd << tab() << startStr(sOn);
    fd << BoolToStr(mapViewport->GetIsOn());
    fd << endStr(sOn) << std::endl;

    // Property: MapView
    IOMapView::Write(fd, &mapViewport->GetMapView(), version, sMapView);

    dectab();
    fd << tab() << endStr(name) << std::endl;
}
