//
//  Copyright (C) 2007 by Autodesk, Inc.
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
#include "IOPath.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOPath::IOPath(Path* path) : IOGraphicElement(path)
{
}

void IOPath::StartPathElement(const wchar_t *name, HandlerStack *handlerStack)
{
    // the element is a path with the supplied name
    m_currElemName = name;
    m_startElemName = name;
}

void IOPath::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
}

void IOPath::ElementChars(const wchar_t *ch)
{
    Path* path = static_cast<Path*>(this->_element);

         IF_STRING_PROPERTY(m_currElemName, path, Geometry, ch)
    else IF_STRING_PROPERTY(m_currElemName, path, FillColor, ch)
    else IF_STRING_PROPERTY(m_currElemName, path, LineColor, ch)
    else IF_STRING_PROPERTY(m_currElemName, path, LineWeight, ch)
    else IF_STRING_PROPERTY(m_currElemName, path, LineWeightScalable, ch)
    else IF_STRING_PROPERTY(m_currElemName, path, LineCap, ch)
    else IF_STRING_PROPERTY(m_currElemName, path, LineJoin, ch)
    else IF_STRING_PROPERTY(m_currElemName, path, LineMiterLimit, ch)
    else IOGraphicElement::ElementChars(ch);
}

void IOPath::Write(MdfStream &fd, Path* path)
{
    IOPath::Write(fd, path, "Path"); // NOXLATE
}

void IOPath::Write(MdfStream &fd, Path* path, std::string name)
{
    fd << tab() << "<" << name << ">" << std::endl;
    inctab();

    IOGraphicElement::Write(fd, path);

    EMIT_STRING_PROPERTY(fd, path, Geometry, false, NULL)
    EMIT_STRING_PROPERTY(fd, path, FillColor, true, L"")         // default is empty string
    EMIT_STRING_PROPERTY(fd, path, LineColor, true, L"")         // default is empty string
    EMIT_DOUBLE_PROPERTY(fd, path, LineWeight, true, 0.0)        // default is 0.0
    EMIT_BOOL_PROPERTY(fd, path, LineWeightScalable, true, true) // default is true
    EMIT_STRING_PROPERTY(fd, path, LineCap, true, L"\'Round\'")  // default is 'Round'
    EMIT_STRING_PROPERTY(fd, path, LineJoin, true, L"\'Round\'") // default is 'Round'
    EMIT_DOUBLE_PROPERTY(fd, path, LineMiterLimit, true, 10.0)   // default is 10.0

    dectab();
    fd << tab() << "</" << name << ">" << std::endl;
}
