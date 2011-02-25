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

#include "../stdafx.h"
#include "../Common/IOPoint3D.h"
#include "../Common/IOStringObjectCollection.h"
#include "IOMapViewportDefinition.h"
#include "IOMapView.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, MapViewportDefinition);
// Inherited Elements
ELEM_MAP_ENTRY(2, Description);
ELEM_MAP_ENTRY(3, ResourceId);
ELEM_MAP_ENTRY(4, Stylization);
ELEM_MAP_ENTRY(5, Data);
ELEM_MAP_ENTRY(6, ExtendedData1);
// Local Elements
ELEM_MAP_ENTRY(7, MapName);
ELEM_MAP_ENTRY(8, HiddenLayerNames);
ELEM_MAP_ENTRY(9, Locked);
ELEM_MAP_ENTRY(10, On);
ELEM_MAP_ENTRY(11, MapView);
// Sub Elements
ELEM_MAP_ENTRY(12, Name);

IOMapViewportDefinition::IOMapViewportDefinition(MapViewportDefinition* mapViewportDef, Version& version) :
    IOPrintLayoutElementDefinition(mapViewportDef, version)
{
}

IOMapViewportDefinition::~IOMapViewportDefinition()
{
}

void IOMapViewportDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eMapViewportDefinition:
        m_startElemName = name;
        break;

    case eHiddenLayerNames:
        {
            IOStringObjectCollection* IO = new IOStringObjectCollection(GetMapViewportDefinition()->GetHiddenLayerNames(), m_version, sHiddenLayerNames, sName);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eMapView:
        {
            IOMapView* IO = new IOMapView(GetMapViewportDefinition()->GetMapView(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eMapName:
    case eLocked:
    case eOn:
        break;

    default:
        IOPrintLayoutElementDefinition::StartElement(name, handlerStack);
        break;
    }
}

void IOMapViewportDefinition::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eMapName:
        GetMapViewportDefinition()->SetMapName(ch);
        break;

    case eLocked:
        GetMapViewportDefinition()->SetIsLocked(wstrToBool(ch));
        break;

    case eOn:
        GetMapViewportDefinition()->SetIsOn(wstrToBool(ch));
        break;

    default:
        IOPrintLayoutElementDefinition::ElementChars(ch);
        break;
    }
}

void IOMapViewportDefinition::Write(MdfStream& fd, MapViewportDefinition* mapViewportDef, Version* version, MgTab& tab)
{
    _ASSERT(NULL != mapViewportDef);

    // Set the expected version
    MdfString strVersion = L"2.0.0";

    fd << tab.tab() << "<PrintLayoutElementDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"PrintLayoutDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    tab.inctab();

    fd << tab.tab() << startStr(sMapViewportDefinition) << std::endl;
    tab.inctab();

    IOPrintLayoutElementDefinition::Write(fd, mapViewportDef, version, tab);

    // Property: MapName
    fd << tab.tab() << startStr(sMapName);
    fd << EncodeString(mapViewportDef->GetMapName());
    fd << endStr(sMapName) << std::endl;

    // Property: HiddenLayerNames
    IOStringObjectCollection::Write(fd, mapViewportDef->GetHiddenLayerNames(), version, sHiddenLayerNames, sName, tab);

    // Property: Locked
    fd << tab.tab() << startStr(sLocked);
    fd << BoolToStr(mapViewportDef->GetIsLocked());
    fd << endStr(sLocked) << std::endl;

    // Property: On
    fd << tab.tab() << startStr(sOn);
    fd << BoolToStr(mapViewportDef->GetIsOn());
    fd << endStr(sOn) << std::endl;

    // Property: MapView
    IOMapView::Write(fd, mapViewportDef->GetMapView(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sMapViewportDefinition) << std::endl;

    tab.dectab();
    fd << tab.tab() << "</PrintLayoutElementDefinition>" << std::endl; // NOXLATE
}
