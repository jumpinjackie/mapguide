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
#include "IOExtra.h"
#include <float.h>

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Extents);
ELEM_MAP_ENTRY(2, MinX);
ELEM_MAP_ENTRY(3, MaxX);
ELEM_MAP_ENTRY(4, MinY);
ELEM_MAP_ENTRY(5, MaxY);


IOExtra::IOExtra(Version& version) : SAX2ElementHandler(version)
{
    this->m_minX = +DBL_MAX;
    this->m_maxX = -DBL_MAX;
    this->m_minY = +DBL_MAX;
    this->m_maxY = -DBL_MAX;
    this->m_map  = NULL;
    this->m_tileset = NULL;
}


IOExtra::IOExtra(MapDefinition* map, Version& version) : SAX2ElementHandler(version)
{
    this->m_minX = +DBL_MAX;
    this->m_maxX = -DBL_MAX;
    this->m_minY = +DBL_MAX;
    this->m_maxY = -DBL_MAX;
    this->m_map  = map;
    this->m_tileset = NULL;
}

IOExtra::IOExtra(TileSetDefinition* tileset, Version& version) : SAX2ElementHandler(version)
{
    this->m_minX = +DBL_MAX;
    this->m_maxX = -DBL_MAX;
    this->m_minY = +DBL_MAX;
    this->m_maxY = -DBL_MAX;
    this->m_map  = NULL;
    this->m_tileset = tileset;
}


IOExtra::~IOExtra()
{
}


void IOExtra::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eExtents:
        this->m_startElemName = name;
        break;
    }
}


void IOExtra::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eMinX:
        this->m_minX = wstrToDouble(ch);
        break;

    case eMaxX:
        this->m_maxX = wstrToDouble(ch);
        break;

    case eMinY:
        this->m_minY = wstrToDouble(ch);
        break;

    case eMaxY:
        this->m_maxY = wstrToDouble(ch);
        break;
    }
}


void IOExtra::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        if (NULL != this->m_map)
        {
            this->m_map->SetExtents(Box2D(this->m_minX, this->m_minY, this->m_maxX, this->m_maxY));
            this->m_map = NULL;
        }
        else if (NULL != this->m_tileset)
        {
            this->m_tileset->SetExtents(Box2D(this->m_minX, this->m_minY, this->m_maxX, this->m_maxY));
            this->m_tileset;
        }
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOExtra::WriteBox2D(MdfStream& fd, const Box2D& box2D, bool autoCorrect, Version* version, MgTab& tab)
{
    double x1 = autoCorrect? box2D.GetMinX() : box2D.GetX1();
    double x2 = autoCorrect? box2D.GetMaxX() : box2D.GetX2();
    double y1 = autoCorrect? box2D.GetMinY() : box2D.GetY1();
    double y2 = autoCorrect? box2D.GetMaxY() : box2D.GetY2();

    fd << tab.tab() << startStr(sExtents) << std::endl;
    tab.inctab();

    // Property: MinX
    fd << tab.tab() << startStr(sMinX);
    fd << DoubleToStr(x1);
    fd << endStr(sMinX) << std::endl;

    // Property: MaxX
    fd << tab.tab() << startStr(sMaxX);
    fd << DoubleToStr(x2);
    fd << endStr(sMaxX) << std::endl;

    // Property: MinY
    fd << tab.tab() << startStr(sMinY);
    fd << DoubleToStr(y1);
    fd << endStr(sMinY) << std::endl;

    // Property: MaxY
    fd << tab.tab() << startStr(sMaxY);
    fd << DoubleToStr(y2);
    fd << endStr(sMaxY) << std::endl;

    tab.dectab();
    fd << tab.tab() << endStr(sExtents) << std::endl;
}
