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
#include "IOExtra.h"
#include <float.h>

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOExtra::IOExtra()
{
    this->minX = +DBL_MAX;
    this->maxX = -DBL_MAX;
    this->minY = +DBL_MAX;
    this->maxY = -DBL_MAX;
    this->map  = NULL;
}

IOExtra::IOExtra(MapDefinition *map)
{
    this->minX = +DBL_MAX;
    this->maxX = -DBL_MAX;
    this->minY = +DBL_MAX;
    this->maxY = -DBL_MAX;
    this->map  = map;
}

IOExtra::~IOExtra()
{
}

void IOExtra::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (::wcscmp(name, L"Extents") == 0) // NOXLATE
    {
        m_startElemName = name;
    }
}

void IOExtra::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"MinX") // NOXLATE
        this->minX = wstrToDouble(ch);
    else if (m_currElemName == L"MinY") // NOXLATE
        this->minY = wstrToDouble(ch);
    else if (m_currElemName == L"MaxX") // NOXLATE
        this->maxX = wstrToDouble(ch);
    else if (m_currElemName == L"MaxY") // NOXLATE
        this->maxY = wstrToDouble(ch);
}

void IOExtra::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->map->SetExtents(Box2D(minX,minY,maxX,maxY));
        handlerStack->pop();
        this->map = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOExtra::WriteBox2D(MdfStream &fd, Box2D box2D, bool autoCorrect /*=true*/)
{
    double x1 = autoCorrect ? box2D.GetMinX() : box2D.GetX1();
    double x2 = autoCorrect ? box2D.GetMaxX() : box2D.GetX2();
    double y1 = autoCorrect ? box2D.GetMinY() : box2D.GetY1();
    double y2 = autoCorrect ? box2D.GetMaxY() : box2D.GetY2();

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
