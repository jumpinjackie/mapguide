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
#include "IOExtent3D.h"
#include "IOPoint3D.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, Extent);
// Local Elements
ELEM_MAP_ENTRY(2, Min);
ELEM_MAP_ENTRY(3, Max);

IOExtent3D::IOExtent3D(Extent3D* extent, Version& version) :
    SAX2ElementHandler(version),
    m_extent(extent)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_extent);
}

IOExtent3D::~IOExtent3D()
{
}

void IOExtent3D::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eExtent:
        m_startElemName = name;
        break;

    case eMin:
        {
            IOPoint3D* IO = new IOPoint3D(m_extent->GetMinimumPoint(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eMax:
        {
            IOPoint3D* IO = new IOPoint3D(m_extent->GetMaximumPoint(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    }
}

void IOExtent3D::ElementChars(const wchar_t* ch)
{
}

void IOExtent3D::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOExtent3D::Write(MdfStream& fd, Extent3D* extent, Version* version, MgTab& tab)
{
    _ASSERT(NULL != extent);

    fd << tab.tab() << startStr(sExtent) << std::endl;
    tab.inctab();

    // Property: Min
    IOPoint3D::Write(fd, extent->GetMinimumPoint(), version, sMin, tab);

    // Property: Max
    IOPoint3D::Write(fd, extent->GetMaximumPoint(), version, sMax, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sExtent) << std::endl;
}
