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
#include "IOAreaUsage.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOAreaUsage::IOAreaUsage(SimpleSymbolDefinition* symbolDefinition, Version& version) : SAX2ElementHandler(version)
{
    this->m_symbolDefinition = symbolDefinition;
    this->m_areaUsage = NULL;
}


void IOAreaUsage::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"AreaUsage") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_areaUsage = new AreaUsage();
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


void IOAreaUsage::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_areaUsage, AngleControl, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_areaUsage, OriginControl, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_areaUsage, ClippingControl, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_areaUsage, Angle, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_areaUsage, OriginX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_areaUsage, OriginY, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_areaUsage, RepeatX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_areaUsage, RepeatY, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_areaUsage, BufferWidth, ch)
}


void IOAreaUsage::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_areaUsage->SetUnknownXml(this->m_unknownXml);

        this->m_symbolDefinition->AdoptAreaUsage(this->m_areaUsage);
        this->m_symbolDefinition = NULL;
        this->m_areaUsage = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOAreaUsage::Write(MdfStream& fd, AreaUsage* areaUsage, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<AreaUsage>" << std::endl; // NOXLATE
    tab.inctab();

    EMIT_STRING_PROPERTY(fd, areaUsage, AngleControl, true, AreaUsage::sAngleControlDefault, tab)
    EMIT_STRING_PROPERTY(fd, areaUsage, OriginControl, true, AreaUsage::sOriginControlDefault, tab)
    EMIT_STRING_PROPERTY(fd, areaUsage, ClippingControl, true, AreaUsage::sClippingControlDefault, tab)
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, Angle, true, 0.0, tab)       // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, OriginX, true, 0.0, tab)     // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, OriginY, true, 0.0, tab)     // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, RepeatX, true, 0.0, tab)     // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, RepeatY, true, 0.0, tab)     // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, BufferWidth, true, 0.0, tab) // default is 0.0

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, areaUsage->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</AreaUsage>" << std::endl; // NOXLATE
}
