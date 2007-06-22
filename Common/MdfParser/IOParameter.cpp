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
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOParameter::IOParameter(ParameterCollection* parameterCollection)
{
    this->m_parameterCollection = parameterCollection;
    this->m_parameter = NULL;
}


IOParameter::~IOParameter()
{
}


void IOParameter::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"Parameter") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_parameter = new Parameter();
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


void IOParameter::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_parameter, Identifier, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_parameter, DefaultValue, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_parameter, DisplayName, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_parameter, Description, ch)
    else IF_ENUM_5(this->m_currElemName, this->m_parameter, Parameter, DataType, ch, String, Boolean, Integer, Real, Color)
}


void IOParameter::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_parameter->SetUnknownXml(this->m_unknownXml);

        this->m_parameterCollection->Adopt(this->m_parameter);
        this->m_parameterCollection = NULL;
        this->m_parameter = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
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

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, parameter->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</Parameter>" << std::endl; // NOXLATE
}
