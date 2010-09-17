//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "IOWatermarkDefinition.h"
#include "IOSimpleSymbolDefinition.h"
#include "IOCompoundSymbolDefinition.h"
#include "IOWatermarkAppearance.h"
#include "IOXYWatermarkPosition.h"
#include "IOTileWatermarkPosition.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, WatermarkDefinition);
ELEM_MAP_ENTRY(2, WatermarkSource);
ELEM_MAP_ENTRY(3, SimpleSymbolDefinition);
ELEM_MAP_ENTRY(4, CompoundSymbolDefinition);
ELEM_MAP_ENTRY(5, Appearance);
ELEM_MAP_ENTRY(6, Position);
ELEM_MAP_ENTRY(7, XYPosition);
ELEM_MAP_ENTRY(8, TilePosition);
ELEM_MAP_ENTRY(9, ExtendedData);

IOWatermarkDefinition::IOWatermarkDefinition(Version& version) 
: SAX2ElementHandler(version), m_watermark(NULL)
{
}


IOWatermarkDefinition::IOWatermarkDefinition(WatermarkDefinition* watermark, Version& version)
: SAX2ElementHandler(version), m_watermark(watermark)
{
}


IOWatermarkDefinition::~IOWatermarkDefinition()
{
    delete this->m_watermark;
}


void IOWatermarkDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eWatermarkDefinition:
        this->m_startElemName = name;
        break;

        //TODO: Other elements
    case eSimpleSymbolDefinition:
        {
            Version sdVersion;
            if (!IOWatermarkDefinition::GetSymbolDefinitionVersion(
                &this->m_version, sdVersion))
                return;

            SimpleSymbolDefinition* simpleSymbol = new SimpleSymbolDefinition();
            this->m_watermark->AdoptSource(simpleSymbol);
            IOSimpleSymbolDefinition* IO = new IOSimpleSymbolDefinition(simpleSymbol, sdVersion);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    case eCompoundSymbolDefinition:
        {
            Version sdVersion;
            if (!IOWatermarkDefinition::GetSymbolDefinitionVersion(
                &this->m_version, sdVersion))
                return;
            CompoundSymbolDefinition* compoundSymbol = new CompoundSymbolDefinition();
            this->m_watermark->AdoptSource(compoundSymbol);
            IOCompoundSymbolDefinition* IO = new IOCompoundSymbolDefinition(
                compoundSymbol, sdVersion);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    case eAppearance:
        {
            WatermarkAppearance* appearance = new WatermarkAppearance();
            this->m_watermark->AdoptAppearance(appearance);
            IOWatermarkAppearance* IO = new IOWatermarkAppearance(
                appearance, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    case eXYPosition:
        {
            XYWatermarkPosition* position = new XYWatermarkPosition();
            this->m_watermark->AdoptPosition(position);
            IOXYWatermarkPosition* IO = new IOXYWatermarkPosition(
                position, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    case eTilePosition:
        {
            TileWatermarkPosition* position = new TileWatermarkPosition();
            this->m_watermark->AdoptPosition(position);
            IOTileWatermarkPosition* IO = new IOTileWatermarkPosition(
                position, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOWatermarkDefinition::ElementChars(const wchar_t* ch)
{
}


void IOWatermarkDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_watermark->SetUnknownXml(this->m_unknownXml);

        this->m_watermark = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}

// Determine which SymbolDefinition schema version to use based
// on the supplied WDF version:
// * WDF version <= 1.0.0  =>  SD version 1.1.0
bool IOWatermarkDefinition::GetSymbolDefinitionVersion(Version* wmVersion, Version& sdVersion)
{
    sdVersion = Version(1, 1, 0);
    return true;
}


void IOWatermarkDefinition::Write(MdfStream& fd, WatermarkDefinition* watermark, Version* version)
{
    MdfString strVersion = version->ToString();
    fd << tab() << "<WatermarkDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"WatermarkDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    inctab();

    fd << tab() << startStr(sWatermarkSource) << std::endl;
    inctab();

    SymbolDefinition* symbol = watermark->GetSource();
    Version sdVersion;
    if (IOWatermarkDefinition::GetSymbolDefinitionVersion(version, sdVersion))
    {
        SimpleSymbolDefinition* simpleSymbol = dynamic_cast<SimpleSymbolDefinition*>(symbol);
        CompoundSymbolDefinition* compoundSymbol = dynamic_cast<CompoundSymbolDefinition*>(symbol);

        if (simpleSymbol)
            IOSimpleSymbolDefinition::Write(fd, simpleSymbol, false, &sdVersion);
        else if (compoundSymbol)
            IOCompoundSymbolDefinition::Write(fd, compoundSymbol, false, &sdVersion);
    }

    dectab();
    fd << endStr(sWatermarkSource) << std::endl;

    IOWatermarkAppearance::Write(fd, watermark->GetAppearance(), version, sAppearance);

    fd << tab() << startStr(sPosition) << std::endl;
    inctab();

    WatermarkPosition* position = watermark->GetPosition();
    XYWatermarkPosition* xyPosition = dynamic_cast<XYWatermarkPosition*>(position);
    TileWatermarkPosition* tilePosition = dynamic_cast<TileWatermarkPosition*>(position);

    if (xyPosition)
        IOXYWatermarkPosition::Write(fd, xyPosition, version);
    else if (tilePosition)
        IOTileWatermarkPosition::Write(fd, tilePosition, version);

    dectab();
    fd << endStr(sPosition) << std::endl;

    //Position

    dectab();
    fd << tab() << "</WatermarkDefinition>" << std::endl; // NOXLATE
}
