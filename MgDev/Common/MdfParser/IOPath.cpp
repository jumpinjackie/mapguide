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
    else IF_STRING_PROPERTY(m_currElemName, path, LineMiterLimit, ch)
// can't use IF_ENUM_4 since the allowed values don't exactly match the enums
//  else IF_ENUM_4(m_currElemName, path, Path, LineCap, ch, CapNone, CapRound, CapTriangle, CapSquare)
//  else IF_ENUM_4(m_currElemName, path, Path, LineJoin, ch, JoinNone, JoinBevel, JoinRound, JoinMiter)
    else if (m_currElemName == L"LineCap") // NOXLATE
    {
        if      (::wcscmp(ch, L"None") == 0) // NOXLATE
            path->SetLineCap(Path::CapNone);
        else if (::wcscmp(ch, L"Round") == 0) // NOXLATE
            path->SetLineCap(Path::CapRound);
        else if (::wcscmp(ch, L"Triangle") == 0) // NOXLATE
            path->SetLineCap(Path::CapTriangle);
        else if (::wcscmp(ch, L"Square") == 0) // NOXLATE
            path->SetLineCap(Path::CapSquare);
    }
    else if (m_currElemName == L"LineJoin")
    {
        // can't use IF_ENUM_4 since the allowed values don't exactly match the enums
        if      (::wcscmp(ch, L"None") == 0) // NOXLATE
            path->SetLineJoin(Path::JoinNone);
        else if (::wcscmp(ch, L"Bevel") == 0) // NOXLATE
            path->SetLineJoin(Path::JoinBevel);
        else if (::wcscmp(ch, L"Round") == 0) // NOXLATE
            path->SetLineJoin(Path::JoinRound);
        else if (::wcscmp(ch, L"Miter") == 0) // NOXLATE
            path->SetLineJoin(Path::JoinMiter);
    }
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

    EMIT_STRING_PROPERTY(fd, path, Geometry, false)
    EMIT_STRING_PROPERTY(fd, path, FillColor, true)
    EMIT_STRING_PROPERTY(fd, path, LineColor, true)
    EMIT_STRING_PROPERTY(fd, path, LineWeight, true)
    EMIT_STRING_PROPERTY(fd, path, LineWeightScalable, true)
// can't use EMIT_ENUM_4 since the allowed values don't exactly match the enums
//  EMIT_ENUM_4(fd, path, Path, LineCap, CapNone, CapRound, CapTriangle, CapSquare, 2) // CapRound is default
//  EMIT_ENUM_4(fd, path, Path, LineJoin, JoinNone, JoinBevel, JoinRound, JoinMiter, 3) // JoinRound is default
    if (path->GetLineCap() != Path::CapRound)
    {
        fd << tab() << "<LineCap>"; // NOXLATE
        switch (path->GetLineCap())
        {
        case Path::CapNone:
            fd << "None"; // NOXLATE
            break;
        case Path::CapRound:
            fd << "Round"; // NOXLATE
            break;
        case Path::CapTriangle:
            fd << "Triangle"; // NOXLATE
            break;
        case Path::CapSquare:
            fd << "Square"; // NOXLATE
            break;
        }
        fd << "</LineCap>" << std::endl; // NOXLATE
    }
    if (path->GetLineJoin() != Path::JoinRound)
    {
        fd << tab() << "<LineJoin>"; // NOXLATE
        switch (path->GetLineJoin())
        {
        case Path::JoinNone:
            fd << "None"; // NOXLATE
            break;
        case Path::JoinBevel:
            fd << "Bevel"; // NOXLATE
            break;
        case Path::JoinRound:
            fd << "Round"; // NOXLATE
            break;
        case Path::JoinMiter:
            fd << "Miter"; // NOXLATE
            break;
        }
        fd << "</LineJoin>" << std::endl; // NOXLATE
    }
    EMIT_STRING_PROPERTY(fd, path, LineMiterLimit, true)

    dectab();
    fd << tab() << "</" << name << ">" << std::endl;
}
