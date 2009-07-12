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
#include "IOPoint3D.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, Center);
ELEM_MAP_ENTRY(2, CenterPoint);
ELEM_MAP_ENTRY(3, Max);
ELEM_MAP_ENTRY(4, Min);
// Local Elements
ELEM_MAP_ENTRY(5, X);
ELEM_MAP_ENTRY(6, Y);
ELEM_MAP_ENTRY(7, Z);

IOPoint3D::IOPoint3D(Point3D* point3D, Version& version) :
    SAX2ElementHandler(version),
    m_point3D(point3D)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_point3D);
}

IOPoint3D::~IOPoint3D()
{
}

void IOPoint3D::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eCenter:
    case eCenterPoint:
    case eMax:
    case eMin:
        m_startElemName = name;
        break;
    }
}

void IOPoint3D::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eX:
        m_point3D->SetX(wstrToDouble(ch));
        break;

    case eY:
        m_point3D->SetY(wstrToDouble(ch));
        break;

    case eZ:
        m_point3D->SetZ(wstrToDouble(ch));
        break;
    }
}

void IOPoint3D::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOPoint3D::Write(MdfStream& fd, Point3D* point3D, Version* version, const std::string& name)
{
    _ASSERT(NULL != point3D);

    fd << tab() << startStr(name) << std::endl;
    inctab();

    // Property: X
    fd << tab() << startStr(sX);
    fd << DoubleToStr(point3D->GetX());
    fd << endStr(sX) << std::endl;

    // Property: Y
    fd << tab() << startStr(sY);
    fd << DoubleToStr(point3D->GetY());
    fd << endStr(sY) << std::endl;

    // Property: Z
    fd << tab() << startStr(sZ);
    fd << DoubleToStr(point3D->GetZ());
    fd << endStr(sZ) << std::endl;

    dectab();
    fd << tab() << endStr(name) << std::endl;
}
