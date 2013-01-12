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
#include "IOLineUsage.h"
#include "IOPath.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOLineUsage::IOLineUsage(SimpleSymbolDefinition* symbolDefinition, Version& version) : SAX2ElementHandler(version)
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

        // In SymbolDefinition version 1.1.0 and higher the default value for
        // VertexControl is 'OverlapWrap' (this value is now the initial one
        // in LineUsage).  In version 1.0.0, however, the default value is
        // 'OverlapNone'.  If we're deserializing a version 1.0.0 stream then
        // we need to change the initial value to old default value.
        if (this->m_version == Version(1, 0, 0))
            this->m_lineUsage->SetVertexControl(L"'OverlapNone'"); // NOXLATE
    }
    else if (this->m_currElemName == L"DefaultPath") // NOXLATE
    {
        Path* path = new Path();
        this->m_lineUsage->AdoptDefaultPath(path);
        IOPath* IO = new IOPath(path, this->m_version);
        handlerStack->push(IO);
        IO->StartPathElement(name, handlerStack);
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
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOLineUsage::Write(MdfStream& fd, LineUsage* lineUsage, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<LineUsage>" << std::endl; // NOXLATE
    tab.inctab();

    EMIT_STRING_PROPERTY(fd, lineUsage, AngleControl, true, LineUsage::sAngleControlDefault, tab)
    EMIT_STRING_PROPERTY(fd, lineUsage, UnitsControl, true, LineUsage::sUnitsControlDefault, tab)

    // Property: VertexControl

    // In SymbolDefinition version 1.1.0 and higher the 'OverlapNoWrap' setting has
    // been removed.  For all versions we now replace it with 'OverlapNone'.
    MdfString strVertexControl = lineUsage->GetVertexControl();
    if (strVertexControl.size() > 0 && _wcsicmp(strVertexControl.c_str(), L"'OverlapNoWrap'") == 0) // NOXLATE
        strVertexControl = L"'OverlapNone'"; // NOXLATE

    bool emitVertexControl = true;
    if (!version || (*version >= Version(1, 1, 0)))
    {
        // in SymbolDefinition version 1.1.0 and higher the default value
        // for VertexControl is 'OverlapWrap'
        if (strVertexControl.size() == 0)
            emitVertexControl = false;
        else if (_wcsicmp(strVertexControl.c_str(), LineUsage::sVertexControlDefault) == 0)
            emitVertexControl = false;
    }
    else if (*version == Version(1, 0, 0))
    {
        // in SymbolDefinition version 1.0.0 the default value for
        // VertexControl is 'OverlapNone'
        if (strVertexControl.size() == 0)
            emitVertexControl = false;
        else if (_wcsicmp(strVertexControl.c_str(), LineUsage::sVertexControlDefault10) == 0)
            emitVertexControl = false;
    }

    if (emitVertexControl)
    {
        fd << tab.tab() << "<VertexControl>";       // NOXLATE
        fd << EncodeString(strVertexControl);
        fd << "</VertexControl>" << std::endl;  // NOXLATE
    }

    // Property: Angle
    EMIT_DOUBLE_PROPERTY(fd, lineUsage, Angle, true, 0.0, tab)   // default is 0.0

    // Property: StartOffset / EndOffset
    bool emitStartOffset = false;
    bool emitEndOffset = false;
    MdfString strStartOffset = lineUsage->GetStartOffset();
    MdfString strEndOffset = lineUsage->GetEndOffset();
    if (!version || (*version >= Version(1, 1, 0)))
    {
        // in SymbolDefinition version 1.1.0 and higher the default values
        // for StartOffset and EndOffset are unspecified (or any negative
        // value)
        double val;

        if (strStartOffset.size() > 0)
        {
            if (!wstrToDouble(strStartOffset, val) || val >= 0.0)
                emitStartOffset = true;
        }

        if (strEndOffset.size() > 0)
        {
            if (!wstrToDouble(strEndOffset, val) || val >= 0.0)
                emitEndOffset = true;
        }
    }
    else if (*version == Version(1, 0, 0))
    {
        // In SymbolDefinition version 1.0.0 the default values for StartOffset
        // and EndOffset are zero.  If the current values are unspecified then
        // make them -1.0 to give the right behavior in version 1.0.0.

        emitStartOffset = true;
        if (strStartOffset.size() == 0)
            strStartOffset = L"-1.0";           // NOXLATE

        emitEndOffset = true;
        if (strEndOffset.size() == 0)
            strEndOffset = L"-1.0";             // NOXLATE
    }

    if (emitStartOffset)
    {
        fd << tab.tab() << "<StartOffset>";         // NOXLATE
        fd << EncodeString(strStartOffset);
        fd << "</StartOffset>" << std::endl;    // NOXLATE
    }

    if (emitEndOffset)
    {
        fd << tab.tab() << "<EndOffset>";           // NOXLATE
        fd << EncodeString(strEndOffset);
        fd << "</EndOffset>" << std::endl;      // NOXLATE
    }

    EMIT_DOUBLE_PROPERTY(fd, lineUsage, Repeat, true, 0.0, tab)           // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, lineUsage, VertexAngleLimit, true, 0.0, tab) // default is 0.0
    EMIT_STRING_PROPERTY(fd, lineUsage, VertexJoin, true, LineUsage::sVertexJoinDefault, tab)
    EMIT_DOUBLE_PROPERTY(fd, lineUsage, VertexMiterLimit, true, 5.0, tab) // default is 5.0

    if (lineUsage->GetDefaultPath())
        IOPath::Write(fd, lineUsage->GetDefaultPath(), "DefaultPath", version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, lineUsage->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</LineUsage>" << std::endl; // NOXLATE
}
