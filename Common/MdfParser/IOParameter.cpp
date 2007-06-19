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
#include "IOParameter.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOParameter::IOParameter(ParameterCollection* parameterCollection)
{
    this->_parameterCollection = parameterCollection;
    this->_parameter = NULL;
}


IOParameter::~IOParameter()
{
}


void IOParameter::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"Parameter") // NOXLATE
    {
        m_startElemName = name;
        this->_parameter = new Parameter();
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOParameter::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_parameter, Identifier, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_parameter, DefaultValue, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_parameter, DisplayName, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_parameter, Description, ch)
    else IF_ENUM_5(m_currElemName, this->_parameter, Parameter, DataType, ch, String, Boolean, Integer, Real, Color)
}


void IOParameter::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_parameter->SetUnknownXml(UnknownXml());

        this->_parameterCollection->Adopt(this->_parameter);
        this->_parameterCollection = NULL;
        this->_parameter = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOParameter::Write(MdfStream& fd, Parameter* parameter, Version* version)
{
    fd << tab() << "<Parameter>" << std::endl; // NOXLATE
    inctab();

    EMIT_STRING_PROPERTY(fd, parameter, Identifier, false, NULL)
    EMIT_STRING_PROPERTY(fd, parameter, DefaultValue, false, NULL)
    EMIT_STRING_PROPERTY(fd, parameter, DisplayName, true, L"")                               // default is empty string
    EMIT_STRING_PROPERTY(fd, parameter, Description, true, L"")                               // default is empty string
    EMIT_ENUM_5(fd, parameter, Parameter, DataType, String, Boolean, Integer, Real, Color, 1) // default is String

    // write any previously found unknown XML
    if (!parameter->GetUnknownXml().empty())
        fd << tab() << toCString(parameter->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</Parameter>" << std::endl; // NOXLATE
}
