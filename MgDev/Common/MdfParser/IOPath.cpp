//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOPath::IOPath(Path* path, Version& version) : IOGraphicElement(path, version)
{
}


void IOPath::StartPathElement(const wchar_t* name, HandlerStack* handlerStack)
{
    // the element is a path with the supplied name
    this->m_currElemName = name;
    this->m_startElemName = name;
}


void IOPath::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"Geometry")
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"FillColor")
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"LineColor")
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"LineWeight")
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"LineWeightScalable")
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"LineCap")
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"LineJoin")
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"LineMiterLimit")
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"ScaleX")
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"ScaleY")
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
    {
        this->m_procExtData = true;
    }
    else
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOPath::ElementChars(const wchar_t* ch)
{
    Path* path = static_cast<Path*>(this->m_element);

         IF_STRING_PROPERTY(this->m_currElemName, path, Geometry, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, path, FillColor, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, path, LineColor, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, path, LineWeight, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, path, LineWeightScalable, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, path, LineCap, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, path, LineJoin, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, path, LineMiterLimit, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, path, ScaleX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, path, ScaleY, ch)
    else IOGraphicElement::ElementChars(ch);
}


void IOPath::Write(MdfStream& fd, Path* path, Version* version, MgTab& tab)
{
    IOPath::Write(fd, path, "Path", version, tab); // NOXLATE
}


void IOPath::Write(MdfStream& fd, Path* path, std::string name, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<" << name << ">" << std::endl;
    tab.inctab();

    MdfStringStream fdExtData;

    IOGraphicElement::Write(fd, path, version, tab);

    EMIT_STRING_PROPERTY(fd, path, Geometry, false, NULL, tab)

    if (!version || (*version >= Version(2, 4, 0)))
    {
        EMIT_DOUBLE_PROPERTY(fd, path, ScaleX, true, 1.0, tab)    // default is 1.0
        EMIT_DOUBLE_PROPERTY(fd, path, ScaleY, true, 1.0, tab)    // default is 1.0
    }
    else
    {
        // save new property as extended data for symbol definition version <= 1.1.0
        tab.inctab();

        EMIT_DOUBLE_PROPERTY(fdExtData, path, ScaleX, true, 1.0, tab)    // default is 1.0
        EMIT_DOUBLE_PROPERTY(fdExtData, path, ScaleY, true, 1.0, tab)    // default is 1.0

        tab.dectab();
    }

    EMIT_STRING_PROPERTY(fd, path, FillColor, true, L"", tab)         // default is empty string
    EMIT_STRING_PROPERTY(fd, path, LineColor, true, L"", tab)         // default is empty string
    EMIT_DOUBLE_PROPERTY(fd, path, LineWeight, true, 0.0, tab)        // default is 0.0
    EMIT_BOOL_PROPERTY(fd, path, LineWeightScalable, true, true, tab) // default is true
    EMIT_STRING_PROPERTY(fd, path, LineCap, true, Path::sLineCapDefault, tab)
    EMIT_STRING_PROPERTY(fd, path, LineJoin, true, Path::sLineJoinDefault, tab)
    EMIT_DOUBLE_PROPERTY(fd, path, LineMiterLimit, true, 5.0, tab)    // default is 5.0

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, path->GetUnknownXml(), fdExtData.str(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</" << name << ">" << std::endl;
}
