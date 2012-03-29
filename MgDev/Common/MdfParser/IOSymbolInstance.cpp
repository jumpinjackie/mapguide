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
#include "IOSymbolInstance.h"
#include "IOOverrideCollection.h"
#include "IOSimpleSymbolDefinition.h"
#include "IOCompoundSymbolDefinition.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOSymbolInstance::IOSymbolInstance(SymbolInstanceCollection* symbolInstanceCollection, Version& version) : SAX2ElementHandler(version)
{
    this->m_symbolInstanceCollection = symbolInstanceCollection;
}


void IOSymbolInstance::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"SymbolInstance") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_symbolInstance = new SymbolInstance();
    }
    else if (this->m_currElemName == L"SimpleSymbolDefinition") // NOXLATE
    {
        Version sdVersion;
        if (!IOSymbolInstance::GetSymbolDefinitionVersion(&this->m_version, sdVersion))
            return;

        SimpleSymbolDefinition* simpleSymbol = new SimpleSymbolDefinition();
        this->m_symbolInstance->AdoptSymbolDefinition(simpleSymbol);
        IOSimpleSymbolDefinition* IO = new IOSimpleSymbolDefinition(simpleSymbol, sdVersion);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"CompoundSymbolDefinition") // NOXLATE
    {
        Version sdVersion;
        if (!IOSymbolInstance::GetSymbolDefinitionVersion(&this->m_version, sdVersion))
            return;

        CompoundSymbolDefinition* compoundSymbol = new CompoundSymbolDefinition();
        this->m_symbolInstance->AdoptSymbolDefinition(compoundSymbol);
        IOCompoundSymbolDefinition* IO = new IOCompoundSymbolDefinition(compoundSymbol, sdVersion);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"ParameterOverrides") // NOXLATE
    {
        IOOverrideCollection* IO = new IOOverrideCollection(this->m_symbolInstance->GetParameterOverrides(), this->m_version);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"RenderingPass") // NOXLATE
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"UsageContext") // NOXLATE
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"GeometryContext") // NOXLATE
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


void IOSymbolInstance::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, ResourceId, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, ScaleX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, ScaleY, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, InsertionOffsetX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, InsertionOffsetY, ch)
    else IF_ENUM_2(this->m_currElemName, this->m_symbolInstance, MdfModel, SizeContext, ch, DeviceUnits, MappingUnits)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, DrawLast, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, CheckExclusionRegion, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, AddToExclusionRegion, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, PositioningAlgorithm, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, RenderingPass, ch)
    else if (this->m_currElemName == L"UsageContext")       // NOXLATE
    {
        if (::wcscmp(ch, L"Point") == 0)                    // NOXLATE
            this->m_symbolInstance->SetUsageContext(SymbolInstance::ucPoint);
        else if (::wcscmp(ch, L"Line") == 0)                // NOXLATE
            this->m_symbolInstance->SetUsageContext(SymbolInstance::ucLine);
        else if (::wcscmp(ch, L"Area") == 0)                // NOXLATE
            this->m_symbolInstance->SetUsageContext(SymbolInstance::ucArea);
        else
            this->m_symbolInstance->SetUsageContext(SymbolInstance::ucUnspecified);
    }
    else if (this->m_currElemName == L"GeometryContext")    // NOXLATE
    {
        if (::wcscmp(ch, L"Point") == 0)                    // NOXLATE
            this->m_symbolInstance->SetGeometryContext(SymbolInstance::gcPoint);
        else if (::wcscmp(ch, L"LineString") == 0)          // NOXLATE
            this->m_symbolInstance->SetGeometryContext(SymbolInstance::gcLineString);
        else if (::wcscmp(ch, L"Polygon") == 0)             // NOXLATE
            this->m_symbolInstance->SetGeometryContext(SymbolInstance::gcPolygon);
        else
            this->m_symbolInstance->SetGeometryContext(SymbolInstance::gcUnspecified);
    }
}


void IOSymbolInstance::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_symbolInstance->SetUnknownXml(this->m_unknownXml);

        this->m_symbolInstanceCollection->Adopt(this->m_symbolInstance);
        this->m_symbolInstanceCollection = NULL;
        this->m_symbolInstance = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


// Determine which SymbolDefinition schema version to use based
// on the supplied LDF version:
// * LDF version == 2.4.0  =>  SD version 2.4.0
// * LDF version == 2.3.0  =>  SD version 1.1.0
// * LDF version == 1.3.0  =>  SD version 1.1.0
// * LDF version == 1.2.0  =>  SD version 1.1.0
// * LDF version <= 1.1.0  =>  SD version 1.0.0
bool IOSymbolInstance::GetSymbolDefinitionVersion(Version* ldfVersion, Version& sdVersion)
{
    if (!ldfVersion || *ldfVersion >= Version(2, 4, 0))
        sdVersion = Version(2, 4, 0);
    else if (*ldfVersion >= Version(1, 2, 0))
        sdVersion = Version(1, 1, 0);
    else if (*ldfVersion <= Version(1, 1, 0))
        sdVersion = Version(1, 0, 0);
    else
    {
        // unsupported LDF version
        // TODO - need a way to return error information
        _ASSERT(false);
        return false;
    }

    return true;
}


void IOSymbolInstance::Write(MdfStream& fd, SymbolInstance* symbolInstance, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<SymbolInstance>" << std::endl; // NOXLATE
    tab.inctab();

    MdfStringStream fdExtData;

    // we must write either a symbol definition or reference, but not both
    SymbolDefinition* symbol = symbolInstance->GetSymbolDefinition();
    if (symbol)
    {
        Version sdVersion;
        if (!IOSymbolInstance::GetSymbolDefinitionVersion(version, sdVersion))
            return;

        SimpleSymbolDefinition* simpleSymbol = dynamic_cast<SimpleSymbolDefinition*>(symbol);
        CompoundSymbolDefinition* compoundSymbol = dynamic_cast<CompoundSymbolDefinition*>(symbol);

        if (simpleSymbol)
            IOSimpleSymbolDefinition::Write(fd, simpleSymbol, false, &sdVersion,tab);
        else if (compoundSymbol)
            IOCompoundSymbolDefinition::Write(fd, compoundSymbol, false, &sdVersion, tab);
    }
    else
    {
        EMIT_STRING_PROPERTY(fd, symbolInstance, ResourceId, false, NULL, tab)
    }

    IOOverrideCollection::Write(fd, symbolInstance->GetParameterOverrides(), version, tab);

    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, ScaleX, true, 1.0, tab)                          // default is 1.0
    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, ScaleY, true, 1.0, tab)                          // default is 1.0
    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, InsertionOffsetX, true, 0.0, tab)                // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, InsertionOffsetY, true, 0.0, tab)                // default is 0.0
    EMIT_ENUM_2(fd, symbolInstance, MdfModel, SizeContext, DeviceUnits, MappingUnits, 1, tab) // default is DeviceUnits
    EMIT_BOOL_PROPERTY(fd, symbolInstance, DrawLast, true, false, tab)                        // default is false
    EMIT_BOOL_PROPERTY(fd, symbolInstance, CheckExclusionRegion, true, false, tab)            // default is false
    EMIT_BOOL_PROPERTY(fd, symbolInstance, AddToExclusionRegion, true, false, tab)            // default is false
    EMIT_STRING_PROPERTY(fd, symbolInstance, PositioningAlgorithm, true, SymbolInstance::sPositioningAlgorithmDefault, tab)

    if (!version || (*version >= Version(1, 2, 0)))
    {
        // write new version 1.2.0 properties

        // Property: RenderingPass
        EMIT_INTEGER_PROPERTY(fd, symbolInstance, RenderingPass, true, 0, tab)               // default is 0

        // Property: UsageContext
        SymbolInstance::UsageContext usageContext = symbolInstance->GetUsageContext();
        if (usageContext != SymbolInstance::ucUnspecified)
        {
            fd << tab.tab() << "<UsageContext>";         // NOXLATE
            if (usageContext == SymbolInstance::ucPoint)
                fd << "Point";                       // NOXLATE
            else if (usageContext == SymbolInstance::ucLine)
                fd << "Line";                        // NOXLATE
            else if (usageContext == SymbolInstance::ucArea)
                fd << "Area";                        // NOXLATE
            fd << "</UsageContext>" << std::endl;    // NOXLATE
        }

        // Property: GeometryContext
        SymbolInstance::GeometryContext geomContext = symbolInstance->GetGeometryContext();
        if (geomContext != SymbolInstance::gcUnspecified)
        {
            fd << tab.tab() << "<GeometryContext>";      // NOXLATE
            if (geomContext == SymbolInstance::gcPoint)
                fd << "Point";                       // NOXLATE
            else if (geomContext == SymbolInstance::gcLineString)
                fd << "LineString";                  // NOXLATE
            else if (geomContext == SymbolInstance::gcPolygon)
                fd << "Polygon";                     // NOXLATE
            fd << "</GeometryContext>" << std::endl; // NOXLATE
        }
    }
    else if (*version >= Version(1, 0, 0))
    {
        // save new properties as extended data for LDF versions 1.0.0 and 1.1.0
        tab.inctab();

        // Property: RenderingPass
        EMIT_INTEGER_PROPERTY(fdExtData, symbolInstance, RenderingPass, true, 0, tab)        // default is 0

        // Property: UsageContext
        SymbolInstance::UsageContext usageContext = symbolInstance->GetUsageContext();
        if (usageContext != SymbolInstance::ucUnspecified)
        {
            fdExtData << tab.tab() << "<UsageContext>";         // NOXLATE
            if (usageContext == SymbolInstance::ucPoint)
                fdExtData << "Point";                       // NOXLATE
            else if (usageContext == SymbolInstance::ucLine)
                fdExtData << "Line";                        // NOXLATE
            else if (usageContext == SymbolInstance::ucArea)
                fdExtData << "Area";                        // NOXLATE
            fdExtData << "</UsageContext>" << std::endl;    // NOXLATE
        }

        // Property: GeometryContext
        SymbolInstance::GeometryContext geomContext = symbolInstance->GetGeometryContext();
        if (geomContext != SymbolInstance::gcUnspecified)
        {
            fdExtData << tab.tab() << "<GeometryContext>";      // NOXLATE
            if (geomContext == SymbolInstance::gcPoint)
                fdExtData << "Point";                       // NOXLATE
            else if (geomContext == SymbolInstance::gcLineString)
                fdExtData << "LineString";                  // NOXLATE
            else if (geomContext == SymbolInstance::gcPolygon)
                fdExtData << "Polygon";                     // NOXLATE
            fdExtData << "</GeometryContext>" << std::endl; // NOXLATE
        }

        tab.dectab();
    }

    // Write the unknown XML / extended data
    IOUnknown::Write(fd, symbolInstance->GetUnknownXml(), fdExtData.str(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</SymbolInstance>" << std::endl; // NOXLATE
}
