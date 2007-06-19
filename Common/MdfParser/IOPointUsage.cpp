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
#include "IOPointUsage.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOPointUsage::IOPointUsage(SimpleSymbolDefinition* symbolDefinition)
{
    this->_symbolDefinition = symbolDefinition;
    this->_pointUsage = NULL;
}


void IOPointUsage::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"PointUsage") // NOXLATE
    {
        m_startElemName = name;
        this->_pointUsage = new PointUsage();
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOPointUsage::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_pointUsage, AngleControl, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_pointUsage, Angle, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_pointUsage, OriginOffsetX, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_pointUsage, OriginOffsetY, ch)
}


void IOPointUsage::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_pointUsage->SetUnknownXml(UnknownXml());

        this->_symbolDefinition->AdoptPointUsage(this->_pointUsage);
        this->_symbolDefinition = NULL;
        this->_pointUsage = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOPointUsage::Write(MdfStream& fd, PointUsage* pointUsage, Version* version)
{
    fd << tab() << "<PointUsage>" << std::endl; // NOXLATE
    inctab();

    EMIT_STRING_PROPERTY(fd, pointUsage, AngleControl, true, L"\'FromAngle\'") // default is 'FromAngle'
    EMIT_DOUBLE_PROPERTY(fd, pointUsage, Angle, true, 0.0)                     // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, pointUsage, OriginOffsetX, true, 0.0)             // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, pointUsage, OriginOffsetY, true, 0.0)             // default is 0.0

    // write any previously found unknown XML
    if (!pointUsage->GetUnknownXml().empty())
        fd << tab() << toCString(pointUsage->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</PointUsage>" << std::endl; // NOXLATE
}
