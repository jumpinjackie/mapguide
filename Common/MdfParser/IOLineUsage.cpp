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
    this->m_symbolDefinition = symbolDefinition;
    this->m_lineUsage = NULL;
}


void IOLineUsage::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"LineUsage") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_lineUsage = new LineUsage();
    }
    else if (this->m_currElemName == L"DefaultPath") // NOXLATE
    {
        Path* path = new Path();
        this->m_lineUsage->AdoptDefaultPath(path);
        IOPath* IO = new IOPath(path);
        handlerStack->push(IO);
        IO->StartPathElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOLineUsage::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_lineUsage, AngleControl, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_lineUsage, UnitsControl, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_lineUsage, VertexControl, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_lineUsage, Angle, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_lineUsage, StartOffset, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_lineUsage, EndOffset, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_lineUsage, Repeat, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_lineUsage, VertexAngleLimit, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_lineUsage, VertexJoin, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_lineUsage, VertexMiterLimit, ch)
}


void IOLineUsage::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_lineUsage->SetUnknownXml(this->m_unknownXml);

        this->m_symbolDefinition->AdoptLineUsage(this->m_lineUsage);
        this->m_symbolDefinition = NULL;
        this->m_lineUsage = NULL;
        this->m_startElemName = L"";
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
