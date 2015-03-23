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
#include "IOGridSurfaceStyle.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, SurfaceStyle);
ELEM_MAP_ENTRY(2, Band);
ELEM_MAP_ENTRY(3, ZeroValue);
ELEM_MAP_ENTRY(4, ScaleFactor);
ELEM_MAP_ENTRY(5, DefaultColor);
ELEM_MAP_ENTRY(6, ExtendedData1);


IOGridSurfaceStyle::IOGridSurfaceStyle(Version& version) : SAX2ElementHandler(version)
{
    this->m_surfaceStyle = NULL;
    this->m_scaleRange = NULL;
}


IOGridSurfaceStyle::IOGridSurfaceStyle(GridScaleRange* scaleRange, Version& version) : SAX2ElementHandler(version)
{
    this->m_surfaceStyle = NULL;
    this->m_scaleRange = scaleRange;
}


IOGridSurfaceStyle::~IOGridSurfaceStyle()
{
}


void IOGridSurfaceStyle::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eSurfaceStyle:
        this->m_startElemName = name;
        this->m_surfaceStyle = new GridSurfaceStyle();
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOGridSurfaceStyle::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eBand:
        this->m_surfaceStyle->SetBand(ch);
        break;

    case eZeroValue:
        this->m_surfaceStyle->SetZeroValue(wstrToDouble(ch));
        break;

    case eScaleFactor:
        this->m_surfaceStyle->SetScaleFactor(wstrToDouble(ch));
        break;

    case eDefaultColor:
        this->m_surfaceStyle->SetDefaultColor(ch);
        break;
    }
}


void IOGridSurfaceStyle::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_surfaceStyle->SetUnknownXml(this->m_unknownXml);

        this->m_scaleRange->AdoptSurfaceStyle(this->m_surfaceStyle);
        this->m_scaleRange = NULL;
        this->m_surfaceStyle = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOGridSurfaceStyle::Write(MdfStream& fd, GridSurfaceStyle* surfaceStyle, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sSurfaceStyle) << std::endl;
    tab.inctab();

    // Property: Band
    fd << tab.tab() << startStr(sBand);
    fd << EncodeString(surfaceStyle->GetBand());
    fd << endStr(sBand) << std::endl;

    // Property: ZeroValue (optional)
    if (surfaceStyle->GetZeroValue() != 0.0)
    {
        fd << tab.tab() << startStr(sZeroValue);
        fd << DoubleToStr(surfaceStyle->GetZeroValue());
        fd << endStr(sZeroValue) << std::endl;
    }

    // Property: ScaleFactor (optional)
    if (surfaceStyle->GetScaleFactor() != 1.0)
    {
        fd << tab.tab() << startStr(sScaleFactor);
        fd << DoubleToStr(surfaceStyle->GetScaleFactor());
        fd << endStr(sScaleFactor) << std::endl;
    }

    // Property: DefaultColor
    fd << tab.tab() << startStr(sDefaultColor);
    fd << EncodeString(surfaceStyle->GetDefaultColor());
    fd << endStr(sDefaultColor) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, surfaceStyle->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sSurfaceStyle) << std::endl;
}
