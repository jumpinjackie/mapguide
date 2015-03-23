//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
#include "IOMapDefinition.h"
#include "IOExtra.h"
#include "IOFill.h"
#include "IOMapLayer.h"
#include "IOMapLayerGroup.h"
#include "IOWatermarkInstance.h"
#include "IOBaseMapDefinition.h"
#include "IOTileSetSource.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, MapDefinition);
ELEM_MAP_ENTRY(2, Name);
ELEM_MAP_ENTRY(3, CoordinateSystem);
ELEM_MAP_ENTRY(4, Extents);
ELEM_MAP_ENTRY(5, BackgroundColor);
ELEM_MAP_ENTRY(6, Metadata);
ELEM_MAP_ENTRY(7, MapLayer);
ELEM_MAP_ENTRY(8, MapLayerGroup);
ELEM_MAP_ENTRY(9, BaseMapDefinition);
ELEM_MAP_ENTRY(10, TileSetSource);
ELEM_MAP_ENTRY(11, Watermarks);
ELEM_MAP_ENTRY(12, Watermark);
ELEM_MAP_ENTRY(13, ExtendedData1);

IOMapDefinition::IOMapDefinition(Version& version) : SAX2ElementHandler(version), m_bReadBaseMapDef(false)
{
    this->m_map = NULL;
}


IOMapDefinition::IOMapDefinition(MapDefinition* map, Version& version) : SAX2ElementHandler(version), m_bReadBaseMapDef(false)
{
    this->m_map = map;
}


IOMapDefinition::~IOMapDefinition()
{
}


void IOMapDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eMapDefinition:
        this->m_startElemName = name;
        break;

    case eExtents:
        {
            IOExtra* IO = new IOExtra(this->m_map, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eMapLayer:
        {
            IOMapLayer* IO = new IOMapLayer(this->m_map, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eMapLayerGroup:
        {
            IOMapLayerGroup* IO = new IOMapLayerGroup(this->m_map, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eBaseMapDefinition:
        {
            IOBaseMapDefinition* IO = new IOBaseMapDefinition(this->m_map, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
            this->m_map->SetTileSourceType(MapDefinition::Inline);
            m_bReadBaseMapDef = true;
        }
        break;

    case eTileSetSource:
        {
            if (!m_bReadBaseMapDef) //Only read this element if no BaseMapDefinition element was read
            {
                IOTileSetSource* IO = new IOTileSetSource(this->m_map->GetTileSetSource(), this->m_version);
                handlerStack->push(IO);
                IO->StartElement(name, handlerStack);
                this->m_map->SetTileSourceType(MapDefinition::TileSetDefinition);
            }
        }
        break;

    case eWatermark:
        {
            Version wdVersion;
            if (!IOMapDefinition::GetWatermarkDefinitionVersion(&this->m_version, wdVersion))
                return;

            WatermarkInstance* watermark = new WatermarkInstance(L"", L"");
            this->m_map->GetWatermarks()->Adopt(watermark);
            IOWatermarkInstance* IO = new IOWatermarkInstance(watermark, wdVersion);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOMapDefinition::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eName:
        this->m_map->SetName(ch);
        break;

    case eCoordinateSystem:
        this->m_map->SetCoordinateSystem(ch);
        break;

    case eBackgroundColor:
        this->m_map->SetBackgroundColor(ch);
        break;

    case eMetadata:
        this->m_map->SetMetadata(ch);
        break;
    }
}


void IOMapDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_map->SetUnknownXml(this->m_unknownXml);
        this->m_map = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


// Determine which WatermarkDefinition schema version to use based
// on the supplied MapDefinition version:
// * MDF version >= 2.4.0  =>  WD version 2.4.0
// * MDF version <= 2.3.0  =>  WD version 2.3.0
bool IOMapDefinition::GetWatermarkDefinitionVersion(Version* mdfVersion, Version& wdVersion)
{
    if (!mdfVersion || *mdfVersion >= Version(2, 4, 0))
        wdVersion = Version(2, 4, 0);
    else if (*mdfVersion <= Version(2, 3, 0))
        wdVersion = Version(2, 3, 0);

    return true;
}


void IOMapDefinition::Write(MdfStream& fd, MapDefinition* map, Version* version, MgTab& tab)
{
    // verify the MDF version
    MdfString strVersion;
    if (version)
    {
        if ((*version >= Version(1, 0, 0)) && (*version <= Version(3, 0, 0)))
        {
            // MDF in MapGuide 2006 - current
            strVersion = version->ToString();
        }
        else
        {
            // unsupported MDF version
            // TODO - need a way to return error information
            _ASSERT(false);
            return;
        }
    }
    else
    {
        // use the current highest version
        strVersion = L"3.0.0";
    }

    if (!version || (*version > Version(1, 0, 0)))
        fd << tab.tab() << "<MapDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"MapDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    else
        fd << tab.tab() << "<MapDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"MapDefinition-1.0.0.xsd\">" << std::endl; // NOXLATE
    tab.inctab();

    // Property: Name
    fd << tab.tab() << startStr(sName);
    fd << EncodeString(map->GetName());
    fd << endStr(sName) << std::endl;

    // Property: CoordinateSystem
    fd << tab.tab() << startStr(sCoordinateSystem);
    fd << EncodeString(map->GetCoordinateSystem());
    fd << endStr(sCoordinateSystem) << std::endl;

    // Property: Extents
    IOExtra::WriteBox2D(fd, map->GetExtents(), false, version, tab);

    // Property: BackgroundColor
    fd << tab.tab() << startStr(sBackgroundColor);
    fd << EncodeString(map->GetBackgroundColor());
    fd << endStr(sBackgroundColor) << std::endl;

    // Property: Metadata
    if (!map->GetMetadata().empty())
    {
        fd << tab.tab() << startStr(sMetadata);
        fd << EncodeString(map->GetMetadata());
        fd << endStr(sMetadata) << std::endl;
    }

    // Property: MapLayer
    for (int i=0; i<map->GetLayers()->GetCount(); ++i)
        IOMapLayer::Write(fd, map->GetLayers()->GetAt(i), version, tab);

    // Property: MapLayerGroup
    for (int i=0; i<map->GetLayerGroups()->GetCount(); ++i)
        IOMapLayerGroup::Write(fd, map->GetLayerGroups()->GetAt(i), version, tab);

    if (map->GetTileSourceType() == MapDefinition::Inline)
    {
        // Property: BaseMapDefinition
        if (map->GetFiniteDisplayScales()->GetCount() > 0)
            IOBaseMapDefinition::Write(fd, map, version, tab);
    }
    else
    {
        // Property: TileSetSource
        IOTileSetSource::Write(fd, map->GetTileSetSource(), version, tab);
    }

    // Property: Watermarks (optional)
    int watermarkCount = map->GetWatermarks()->GetCount();
    if (watermarkCount != 0)
    {
        // only write Watermarks if the MDF version is 2.3.0 or greater
        if (!version || (*version >= Version(2, 3, 0)))
        {
            fd << tab.tab() << startStr(sWatermarks) << std::endl;
            tab.inctab();
            for (int i=0; i<watermarkCount; ++i)
                IOWatermarkInstance::Write(fd, map->GetWatermarks()->GetAt(i), version, tab);
            tab.dectab();
            fd << tab.tab() << endStr(sWatermarks) << std::endl;
        }
    }

    // Write any unknown XML / extended data
    if (!version || (*version >= Version(2, 3, 0)))
        IOUnknown::Write(fd, map->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</MapDefinition>" << std::endl; // NOXLATE
}
