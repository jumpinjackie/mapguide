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

#include "../stdafx.h"
#include "IOVector3D.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, ViewDirection);
// Local Elements
ELEM_MAP_ENTRY(2, X);
ELEM_MAP_ENTRY(3, Y);
ELEM_MAP_ENTRY(4, Z);

IOVector3D::IOVector3D(Vector3D* vector, Version& version) :
    SAX2ElementHandler(version),
    m_vector(vector)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_vector);
}

IOVector3D::~IOVector3D()
{
}

void IOVector3D::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eViewDirection:
        m_startElemName = name;
        break;
    }
}

void IOVector3D::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eX:
        m_vector->SetX(wstrToDouble(ch));
        break;

    case eY:
        m_vector->SetY(wstrToDouble(ch));
        break;

    case eZ:
        m_vector->SetZ(wstrToDouble(ch));
        break;
    }
}

void IOVector3D::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOVector3D::Write(MdfStream& fd, Vector3D* vector, Version* version, MgTab& tab)
{
    _ASSERT(NULL != vector);

    fd << tab.tab() << startStr(sViewDirection) << std::endl;
    tab.inctab();

    // Property: X
    fd << tab.tab() << startStr(sX);
    fd << DoubleToStr(vector->GetX());
    fd << endStr(sX) << std::endl;

    // Property: Y
    fd << tab.tab() << startStr(sY);
    fd << DoubleToStr(vector->GetY());
    fd << endStr(sY) << std::endl;

    // Property: Z
    fd << tab.tab() << startStr(sZ);
    fd << DoubleToStr(vector->GetZ());
    fd << endStr(sZ) << std::endl;

    tab.dectab();
    fd << tab.tab() << endStr(sViewDirection) << std::endl;
}
