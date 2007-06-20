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
#include "IOAreaUsage.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOAreaUsage::IOAreaUsage(SimpleSymbolDefinition* symbolDefinition)
{
    this->_symbolDefinition = symbolDefinition;
    this->_areaUsage = NULL;
}


void IOAreaUsage::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"AreaUsage") // NOXLATE
    {
        m_startElemName = name;
        this->_areaUsage = new AreaUsage();
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOAreaUsage::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_areaUsage, AngleControl, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_areaUsage, OriginControl, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_areaUsage, ClippingControl, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_areaUsage, Angle, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_areaUsage, OriginX, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_areaUsage, OriginY, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_areaUsage, RepeatX, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_areaUsage, RepeatY, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_areaUsage, BufferWidth, ch)
}


void IOAreaUsage::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_areaUsage->SetUnknownXml(UnknownXml());

        this->_symbolDefinition->AdoptAreaUsage(this->_areaUsage);
        this->_symbolDefinition = NULL;
        this->_areaUsage = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOAreaUsage::Write(MdfStream& fd, AreaUsage* areaUsage, Version* version)
{
    fd << tab() << "<AreaUsage>" << std::endl; // NOXLATE
    inctab();

    EMIT_STRING_PROPERTY(fd, areaUsage, AngleControl, true, L"\'FromAngle\'") // default is 'FromAngle'
    EMIT_STRING_PROPERTY(fd, areaUsage, OriginControl, true, L"\'Global\'")   // default is 'Global'
    EMIT_STRING_PROPERTY(fd, areaUsage, ClippingControl, true, L"\'Clip\'")   // default is 'Clip'
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, Angle, true, 0.0)                     // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, OriginX, true, 0.0)                   // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, OriginY, true, 0.0)                   // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, RepeatX, true, 0.0)                   // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, RepeatY, true, 0.0)                   // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, areaUsage, BufferWidth, true, 0.0)               // default is 0.0

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, areaUsage->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</AreaUsage>" << std::endl; // NOXLATE
}
