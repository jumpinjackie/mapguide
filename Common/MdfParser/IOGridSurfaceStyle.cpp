//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "IOExtra.h"
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


IOGridSurfaceStyle::IOGridSurfaceStyle()
{
    this->m_surfaceStyle = NULL;
    this->m_scaleRange = NULL;
}


IOGridSurfaceStyle::IOGridSurfaceStyle(GridScaleRange* scaleRange)
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

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}


void IOGridSurfaceStyle::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"Band") // NOXLATE
        this->m_surfaceStyle->SetBand(ch);
    else if (this->m_currElemName == L"ZeroValue") // NOXLATE
        this->m_surfaceStyle->SetZeroValue(wstrToDouble(ch));
    else if (this->m_currElemName == L"ScaleFactor") // NOXLATE
        this->m_surfaceStyle->SetScaleFactor(wstrToDouble(ch));
    else if (this->m_currElemName == L"DefaultColor") // NOXLATE
        this->m_surfaceStyle->SetDefaultColor(ch);
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
}


void IOGridSurfaceStyle::Write(MdfStream& fd, GridSurfaceStyle* surfaceStyle, Version* version)
{
    fd << tab() << "<SurfaceStyle>" << std::endl; // NOXLATE
    inctab();

    //Property: ElevationBandName
    fd << tab() << "<Band>"; // NOXLATE
    fd << EncodeString(surfaceStyle->GetBand());
    fd << "</Band>" << std::endl; // NOXLATE

    //Property: ZeroValue (optional)
    if (surfaceStyle->GetZeroValue() != 0.0)
    {
        fd << tab() << "<ZeroValue>"; // NOXLATE
        fd << DoubleToStr(surfaceStyle->GetZeroValue());
        fd << "</ZeroValue>" << std::endl; // NOXLATE
    }

    //Property: ScaleFactor (optional)
    if (surfaceStyle->GetScaleFactor() != 1.0)
    {
        fd << tab() << "<ScaleFactor>"; // NOXLATE
        fd << DoubleToStr(surfaceStyle->GetScaleFactor());
        fd << "</ScaleFactor>" << std::endl; // NOXLATE
    }

    //Property: DefaultColor
    fd << tab() << "<DefaultColor>"; // NOXLATE
    fd << EncodeString(surfaceStyle->GetDefaultColor());
    fd << "</DefaultColor>" << std::endl; // NOXLATE

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, surfaceStyle->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</SurfaceStyle>" << std::endl; // NOXLATE
}
