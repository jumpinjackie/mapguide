//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, SurfaceStyle);
ELEM_MAP_ENTRY(2, Band);
ELEM_MAP_ENTRY(3, ZeroValue);
ELEM_MAP_ENTRY(4, ScaleFactor);
ELEM_MAP_ENTRY(5, DefaultColor);

IOGridSurfaceStyle::IOGridSurfaceStyle():surfaceStyle(NULL), scaleRange(NULL)
{
}

IOGridSurfaceStyle::IOGridSurfaceStyle(GridScaleRange * pScaleRange):surfaceStyle(NULL), scaleRange(pScaleRange)
{
}

IOGridSurfaceStyle::~IOGridSurfaceStyle()
{
}

void IOGridSurfaceStyle::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eSurfaceStyle:
        m_startElemName = name;
        this->surfaceStyle = new GridSurfaceStyle();
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}

void IOGridSurfaceStyle::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Band") // NOXLATE
        (this->surfaceStyle)->SetBand(ch);
    else if (m_currElemName == L"ZeroValue") // NOXLATE
        (this->surfaceStyle)->SetZeroValue(wstrToDouble(ch));
    else if (m_currElemName == L"ScaleFactor") // NOXLATE
        (this->surfaceStyle)->SetScaleFactor(wstrToDouble(ch));
    else if (m_currElemName == L"DefaultColor") // NOXLATE
        (this->surfaceStyle)->SetDefaultColor(ch);
}

void IOGridSurfaceStyle::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
         if (!UnknownXml().empty())
            this->surfaceStyle->SetUnknownXml(UnknownXml());

       //this->scaleRange->GetGridStyles()->Adopt(this->surfaceStyle);
        this->scaleRange->AdoptSurfaceStyle(this->surfaceStyle);
        handlerStack->pop();
        this->scaleRange = NULL;
        this->surfaceStyle = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOGridSurfaceStyle::Write(MdfStream &fd,  GridSurfaceStyle *pSurfaceStyle)
{
    fd << tab() << "<SurfaceStyle>" << std::endl; // NOXLATE
    inctab();

    //Property: ElevationBandName
    fd << tab() << "<Band>"; // NOXLATE
    fd << EncodeString(pSurfaceStyle->GetBand());
    fd << "</Band>" << std::endl; // NOXLATE

    //Property: ZeroValue (optional)
    if (pSurfaceStyle->GetZeroValue() != 0.0)
    {
        fd << tab() << "<ZeroValue>"; // NOXLATE
        fd << DoubleToStr(pSurfaceStyle->GetZeroValue());
        fd << "</ZeroValue>" << std::endl; // NOXLATE
    }

    //Property: ScaleFactor (optional)
    if (pSurfaceStyle->GetScaleFactor() != 1.0)
    {
        fd << tab() << "<ScaleFactor>"; // NOXLATE
        fd << DoubleToStr(pSurfaceStyle->GetScaleFactor());
        fd << "</ScaleFactor>" << std::endl; // NOXLATE
    }

    //Property: DefaultColor
    fd << tab() << "<DefaultColor>"; // NOXLATE
    fd << EncodeString(pSurfaceStyle->GetDefaultColor());
    fd << "</DefaultColor>" << std::endl; // NOXLATE

    // Write any previously found unknown XML
    if (!pSurfaceStyle->GetUnknownXml().empty())
    {
        fd << toCString(pSurfaceStyle->GetUnknownXml()); 
    }

    dectab();
    fd << tab() << "</SurfaceStyle>" << std::endl; // NOXLATE
}
