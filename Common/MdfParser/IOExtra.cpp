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
#include "IOExtra.h"
#include <float.h>

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOExtra::IOExtra()
{
    this->m_minX = +DBL_MAX;
    this->m_maxX = -DBL_MAX;
    this->m_minY = +DBL_MAX;
    this->m_maxY = -DBL_MAX;
    this->m_map  = NULL;
}


IOExtra::IOExtra(MapDefinition* map)
{
    this->m_minX = +DBL_MAX;
    this->m_maxX = -DBL_MAX;
    this->m_minY = +DBL_MAX;
    this->m_maxY = -DBL_MAX;
    this->m_map  = map;
}


IOExtra::~IOExtra()
{
}


void IOExtra::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"Extents") // NOXLATE
    {
        this->m_startElemName = name;
    }
}


void IOExtra::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"MinX") // NOXLATE
        this->m_minX = wstrToDouble(ch);
    else if (this->m_currElemName == L"MinY") // NOXLATE
        this->m_minY = wstrToDouble(ch);
    else if (this->m_currElemName == L"MaxX") // NOXLATE
        this->m_maxX = wstrToDouble(ch);
    else if (this->m_currElemName == L"MaxY") // NOXLATE
        this->m_maxY = wstrToDouble(ch);
}


void IOExtra::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_map->SetExtents(Box2D(this->m_minX, this->m_minY, this->m_maxX, this->m_maxY));
        this->m_map = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOExtra::WriteBox2D(MdfStream& fd, const Box2D& box2D, bool autoCorrect, Version* version)
{
    double x1 = autoCorrect? box2D.GetMinX() : box2D.GetX1();
    double x2 = autoCorrect? box2D.GetMaxX() : box2D.GetX2();
    double y1 = autoCorrect? box2D.GetMinY() : box2D.GetY1();
    double y2 = autoCorrect? box2D.GetMaxY() : box2D.GetY2();

    //Property: MinX
    fd << tab() << "<MinX>"; // NOXLATE
    fd << DoubleToStr(x1);
    fd << "</MinX>" << std::endl; // NOXLATE

    //Property: MaxX
    fd << tab() << "<MaxX>"; // NOXLATE
    fd << DoubleToStr(x2);
    fd << "</MaxX>" << std::endl; // NOXLATE

    //Property: MinY
    fd << tab() << "<MinY>"; // NOXLATE
    fd << DoubleToStr(y1);
    fd << "</MinY>" << std::endl; // NOXLATE

    //Property: MaxY
    fd << tab() << "<MaxY>"; // NOXLATE
    fd << DoubleToStr(y2);
    fd << "</MaxY>" << std::endl; // NOXLATE
}
