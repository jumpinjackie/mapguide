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
#include "IOGridScaleRange.h"
#include "IOVectorScaleRange.h"
#include "GridSurfaceStyle.h"
#include "GridColorStyle.h"
#include "IOGridSurfaceStyle.h"
#include "IOGridColorStyle.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, GridScaleRange);
ELEM_MAP_ENTRY(2, SurfaceStyle);
ELEM_MAP_ENTRY(3, ColorStyle);
ELEM_MAP_ENTRY(4, MinScale);
ELEM_MAP_ENTRY(5, MaxScale);
ELEM_MAP_ENTRY(6, RebuildFactor);

IOGridScaleRange::IOGridScaleRange():_scaleRange(NULL), layer(NULL)
{
}

IOGridScaleRange::IOGridScaleRange(GridLayerDefinition *player):_scaleRange(NULL), layer(player)
{
}

IOGridScaleRange::~IOGridScaleRange()
{
}

void IOGridScaleRange::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eGridScaleRange:
        m_startElemName = name;
        this->_scaleRange = new GridScaleRange();
        break;

    case eSurfaceStyle:
        {
            IOGridSurfaceStyle *IO = new IOGridSurfaceStyle(this->_scaleRange);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eColorStyle:
        {
            IOGridColorStyle *IO = new IOGridColorStyle(this->_scaleRange);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}

void IOGridScaleRange::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"MinScale") // NOXLATE
        (this->_scaleRange)->SetMinScale(wstrToDouble(ch));
    else if (m_currElemName == L"MaxScale") // NOXLATE
        (this->_scaleRange)->SetMaxScale(wstrToDouble(ch));
    else if (m_currElemName == L"RebuildFactor")// NOXLATE
        (this->_scaleRange)->SetRebuildFactor(wstrToDouble(ch));
}

void IOGridScaleRange::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_scaleRange->SetUnknownXml(UnknownXml());

        this->layer->GetScaleRanges()->Adopt(this->_scaleRange);
        handlerStack->pop();
        this->layer = NULL;
        this->_scaleRange = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOGridScaleRange::Write(MdfStream &fd,  GridScaleRange *scaleRange)
{
    fd << tab() << "<GridScaleRange>" << std::endl; // NOXLATE
    inctab();

    //Property: MinScale (optional)
    if (scaleRange->GetMinScale() != 0.0)
    {
        fd << tab() << "<MinScale>"; // NOXLATE
        fd << DoubleToStr(scaleRange->GetMinScale());
        fd << "</MinScale>" << std::endl; // NOXLATE
    }

    //Property: MaxScale (optional)
    if (scaleRange->GetMaxScale() != VectorScaleRange::MAX_MAP_SCALE)
    {
        fd << tab() << "<MaxScale>"; // NOXLATE
        fd << DoubleToStr(scaleRange->GetMaxScale());
        fd << "</MaxScale>" << std::endl; // NOXLATE
    }

    // Property : Surface Style
    GridSurfaceStyle *pGridSurfaceStyle = scaleRange->GetSurfaceStyle();
    if(NULL != pGridSurfaceStyle)
    {
        IOGridSurfaceStyle * IO = new IOGridSurfaceStyle();
        IO->Write(fd, pGridSurfaceStyle);
        delete IO;
    }

    // Property : Color Style
    GridColorStyle *pGridColorStyle = scaleRange->GetColorStyle();
    if(NULL != pGridColorStyle)
    {
        IOGridColorStyle * IO = new IOGridColorStyle();
        IO->Write(fd, pGridColorStyle);
        delete IO;
    }

    // Property : RebuildFactor
    fd << tab() << "<RebuildFactor>"; // NOXLATE
    fd << DoubleToStr(scaleRange->GetRebuildFactor());
    fd << "</RebuildFactor>" << std::endl; // NOXLATE

        // Write any previously found unknown XML
    if (!scaleRange->GetUnknownXml().empty())
    {
        fd << toCString(scaleRange->GetUnknownXml()); 
    }

    dectab();
    fd << tab() << "</GridScaleRange>" << std::endl; // NOXLATE
}
