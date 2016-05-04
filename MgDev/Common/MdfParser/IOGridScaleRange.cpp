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
#include "IOGridScaleRange.h"
#include "IOVectorScaleRange.h"
#include "GridSurfaceStyle.h"
#include "GridColorStyle.h"
#include "IOGridSurfaceStyle.h"
#include "IOGridColorStyle.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, GridScaleRange);
ELEM_MAP_ENTRY(2, MinScale);
ELEM_MAP_ENTRY(3, MaxScale);
ELEM_MAP_ENTRY(4, SurfaceStyle);
ELEM_MAP_ENTRY(5, ColorStyle);
ELEM_MAP_ENTRY(6, RebuildFactor);
ELEM_MAP_ENTRY(7, ExtendedData1);


IOGridScaleRange::IOGridScaleRange(Version& version) : SAX2ElementHandler(version)
{
    this->m_scaleRange = NULL;
    this->m_layer = NULL;
}


IOGridScaleRange::IOGridScaleRange(GridLayerDefinition* layer, Version& version) : SAX2ElementHandler(version)
{
    this->m_scaleRange = NULL;
    this->m_layer = layer;
}


IOGridScaleRange::~IOGridScaleRange()
{
}


void IOGridScaleRange::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eGridScaleRange:
        this->m_startElemName = name;
        this->m_scaleRange = new GridScaleRange();
        break;

    case eSurfaceStyle:
        {
            IOGridSurfaceStyle* IO = new IOGridSurfaceStyle(this->m_scaleRange, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eColorStyle:
        {
            IOGridColorStyle* IO = new IOGridColorStyle(this->m_scaleRange, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOGridScaleRange::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eMinScale:
        this->m_scaleRange->SetMinScale(wstrToDouble(ch));
        break;

    case eMaxScale:
        this->m_scaleRange->SetMaxScale(wstrToDouble(ch));
        break;

    case eRebuildFactor:
        this->m_scaleRange->SetRebuildFactor(wstrToDouble(ch));
        break;
    }
}


void IOGridScaleRange::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_scaleRange->SetUnknownXml(this->m_unknownXml);

        this->m_layer->GetScaleRanges()->Adopt(this->m_scaleRange);
        this->m_layer = NULL;
        this->m_scaleRange = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOGridScaleRange::Write(MdfStream& fd, GridScaleRange* scaleRange, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sGridScaleRange) << std::endl;
    tab.inctab();

    // Property: MinScale (optional)
    if (scaleRange->GetMinScale() != 0.0)
    {
        fd << tab.tab() << startStr(sMinScale);
        fd << DoubleToStr(scaleRange->GetMinScale());
        fd << endStr(sMinScale) << std::endl;
    }

    // Property: MaxScale (optional)
    if (scaleRange->GetMaxScale() != VectorScaleRange::MAX_MAP_SCALE)
    {
        fd << tab.tab() << startStr(sMaxScale);
        fd << DoubleToStr(scaleRange->GetMaxScale());
        fd << endStr(sMaxScale) << std::endl;
    }

    // Property : Surface Style
    GridSurfaceStyle* gridSurfaceStyle = scaleRange->GetSurfaceStyle();
    if (gridSurfaceStyle)
        IOGridSurfaceStyle::Write(fd, gridSurfaceStyle, version, tab);

    // Property : Color Style
    GridColorStyle* gridColorStyle = scaleRange->GetColorStyle();
    if (gridColorStyle)
        IOGridColorStyle::Write(fd, gridColorStyle, version, tab);

    // Property : RebuildFactor
    fd << tab.tab() << startStr(sRebuildFactor);
    fd << DoubleToStr(scaleRange->GetRebuildFactor());
    fd << endStr(sRebuildFactor) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, scaleRange->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sGridScaleRange) << std::endl;
}
