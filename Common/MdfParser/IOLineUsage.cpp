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
#include "IOLineUsage.h"
#include "IOPath.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOLineUsage::IOLineUsage(SimpleSymbolDefinition* symbolDefinition)
{
    this->_symbolDefinition = symbolDefinition;
    this->_lineUsage = NULL;
}


void IOLineUsage::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"LineUsage") // NOXLATE
    {
        m_startElemName = name;
        this->_lineUsage = new LineUsage();
    }
    else if (m_currElemName == L"DefaultPath") // NOXLATE
    {
        Path* path = new Path();
        this->_lineUsage->AdoptDefaultPath(path);
        IOPath* IO = new IOPath(path);
        handlerStack->push(IO);
        IO->StartPathElement(name, handlerStack);
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOLineUsage::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_lineUsage, AngleControl, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_lineUsage, UnitsControl, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_lineUsage, VertexControl, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_lineUsage, Angle, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_lineUsage, StartOffset, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_lineUsage, EndOffset, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_lineUsage, Repeat, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_lineUsage, VertexAngleLimit, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_lineUsage, VertexJoin, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_lineUsage, VertexMiterLimit, ch)
}


void IOLineUsage::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_lineUsage->SetUnknownXml(UnknownXml());

        this->_symbolDefinition->AdoptLineUsage(this->_lineUsage);
        this->_symbolDefinition = NULL;
        this->_lineUsage = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOLineUsage::Write(MdfStream& fd, LineUsage* lineUsage, Version* version)
{
    fd << tab() << "<LineUsage>" << std::endl; // NOXLATE
    inctab();

    EMIT_STRING_PROPERTY(fd, lineUsage, AngleControl, true, L"\'FromGeometry\'") // default is 'FromGeometry'
    EMIT_STRING_PROPERTY(fd, lineUsage, UnitsControl, true, L"\'Absolute\'")     // default is 'Absolute'
    EMIT_STRING_PROPERTY(fd, lineUsage, VertexControl, true, L"\'OverlapNone\'") // default is 'OverlapNone'
    EMIT_DOUBLE_PROPERTY(fd, lineUsage, Angle, true, 0.0)                        // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, lineUsage, StartOffset, true, 0.0)                  // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, lineUsage, EndOffset, true, 0.0)                    // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, lineUsage, Repeat, true, 0.0)                       // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, lineUsage, VertexAngleLimit, true, 0.0)             // default is 0.0
    EMIT_STRING_PROPERTY(fd, lineUsage, VertexJoin, true, L"\'Round\'")          // default is 'Round'
    EMIT_DOUBLE_PROPERTY(fd, lineUsage, VertexMiterLimit, true, 5.0)             // default is 5.0

    if (lineUsage->GetDefaultPath() != NULL)
        IOPath::Write(fd, lineUsage->GetDefaultPath(), "DefaultPath", version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, lineUsage->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</LineUsage>" << std::endl; // NOXLATE
}
