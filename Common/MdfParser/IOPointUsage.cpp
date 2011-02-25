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
#include "IOPointUsage.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOPointUsage::IOPointUsage(SimpleSymbolDefinition* symbolDefinition, Version& version) : SAX2ElementHandler(version)
{
    this->m_symbolDefinition = symbolDefinition;
    this->m_pointUsage = NULL;
}


void IOPointUsage::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"PointUsage") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_pointUsage = new PointUsage();
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


void IOPointUsage::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_pointUsage, AngleControl, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_pointUsage, Angle, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_pointUsage, OriginOffsetX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_pointUsage, OriginOffsetY, ch)
}


void IOPointUsage::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_pointUsage->SetUnknownXml(this->m_unknownXml);

        this->m_symbolDefinition->AdoptPointUsage(this->m_pointUsage);
        this->m_symbolDefinition = NULL;
        this->m_pointUsage = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOPointUsage::Write(MdfStream& fd, PointUsage* pointUsage, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<PointUsage>" << std::endl; // NOXLATE
    tab.inctab();

    EMIT_STRING_PROPERTY(fd, pointUsage, AngleControl, true, PointUsage::sAngleControlDefault, tab)
    EMIT_DOUBLE_PROPERTY(fd, pointUsage, Angle, true, 0.0, tab)          // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, pointUsage, OriginOffsetX, true, 0.0, tab)  // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, pointUsage, OriginOffsetY, true, 0.0, tab)  // default is 0.0

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, pointUsage->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</PointUsage>" << std::endl; // NOXLATE
}
