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
#include "../IOUnknown.h"
#include "../Common/IOPoint3D.h"
#include "../Common/IOVector3D.h"
#include "IOMapView.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, MapView);
// Local Elements
ELEM_MAP_ENTRY(2, Center);
ELEM_MAP_ENTRY(3, Height);
ELEM_MAP_ENTRY(4, Rotation);
ELEM_MAP_ENTRY(5, ViewDirection);
ELEM_MAP_ENTRY(6, ModelUnits);
ELEM_MAP_ENTRY(7, ExtendedData1);

IOMapView::IOMapView(MapView* mapView, Version& version) :
    SAX2ElementHandler(version),
    m_mapView(mapView)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_mapView);
}

IOMapView::~IOMapView()
{
}

void IOMapView::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eMapView:
        m_startElemName = name;
        break;

    case eCenter:
        {
            IOPoint3D* IO = new IOPoint3D(m_mapView->GetCenter(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eViewDirection:
        {
            IOVector3D* IO = new IOVector3D(m_mapView->GetViewDirection(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData1:
        m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}

void IOMapView::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eHeight:
        m_mapView->SetHeight(wstrToDouble(ch));
        break;

    case eRotation:
        m_mapView->SetRotation(wstrToDouble(ch));
        break;

    case eModelUnits:
        m_mapView->SetModelUnits(ch);
        break;
    }
}

void IOMapView::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_mapView->SetUnknownXml(m_unknownXml);

        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        m_procExtData = false;
    }
}

void IOMapView::Write(MdfStream& fd, MapView* mapView, Version* version, MgTab& tab)
{
    _ASSERT(NULL != mapView);

    fd << tab.tab() << startStr(sMapView) << std::endl;
    tab.inctab();

    // Property: Center
    IOPoint3D::Write(fd, mapView->GetCenter(), version, sCenter, tab);

    // Property: Height
    fd << tab.tab() << startStr(sHeight);
    fd << DoubleToStr(mapView->GetHeight());
    fd << endStr(sHeight) << std::endl;

    // Property: Rotation
    fd << tab.tab() << startStr(sRotation);
    fd << DoubleToStr(mapView->GetRotation());
    fd << endStr(sRotation) << std::endl;

    // Property: ViewDirection
    IOVector3D::Write(fd, mapView->GetViewDirection(), version, tab);

    // Property: ModelUnits
    fd << tab.tab() << startStr(sModelUnits);
    fd << EncodeString(mapView->GetModelUnits());
    fd << endStr(sModelUnits) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, mapView->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sMapView) << std::endl;
}
