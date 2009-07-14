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
#include "IOMapView.h"
#include "IOPoint3D.h"
#include "IOVector3D.h"
#include "IOUnknown.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, MapView);
// Local Elements
ELEM_MAP_ENTRY(2, Center);
ELEM_MAP_ENTRY(3, Height);
ELEM_MAP_ENTRY(4, TwistAngle);
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

    case eTwistAngle:
        m_mapView->SetTwistAngle(wstrToDouble(ch));
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

void IOMapView::Write(MdfStream& fd, MapView* mapView, Version* version)
{
    _ASSERT(NULL != mapView);

    fd << tab() << startStr(sMapView) << std::endl;
    inctab();

    // Property: Center
    IOPoint3D::Write(fd, mapView->GetCenter(), version, sCenter);

    // Property: Height
    fd << tab() << startStr(sHeight);
    fd << DoubleToStr(mapView->GetHeight());
    fd << endStr(sHeight) << std::endl;

    // Property: TwistAngle
    fd << tab() << startStr(sTwistAngle);
    fd << DoubleToStr(mapView->GetTwistAngle());
    fd << endStr(sTwistAngle) << std::endl;

    // Property: ViewDirection
    IOVector3D::Write(fd, mapView->GetViewDirection(), version);

    // Property: ModelUnits
    fd << tab() << startStr(sModelUnits);
    fd << EncodeString(mapView->GetModelUnits());
    fd << endStr(sModelUnits) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, mapView->GetUnknownXml(), version);

    dectab();
    fd << tab() << endStr(sMapView) << std::endl;
}
