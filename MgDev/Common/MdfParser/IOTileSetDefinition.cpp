//
//  Copyright (C) 2004-2014 by Autodesk, Inc.
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
#include "IOTileSetDefinition.h"
#include "IOExtra.h"
#include "IOTileStoreParameters.h"
#include "IOBaseMapLayerGroup.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, TileSetDefinition);
ELEM_MAP_ENTRY(2, TileStoreParameters);
ELEM_MAP_ENTRY(3, CoordinateSystem);
ELEM_MAP_ENTRY(4, Extents);
ELEM_MAP_ENTRY(5, FiniteDisplayScale);
ELEM_MAP_ENTRY(6, BaseMapLayerGroup);
ELEM_MAP_ENTRY(7, ExtendedData1);

IOTileSetDefinition::IOTileSetDefinition(Version& version) : SAX2ElementHandler(version)
{
    this->m_tileset = NULL;
}


IOTileSetDefinition::IOTileSetDefinition(TileSetDefinition* tileset, Version& version) : SAX2ElementHandler(version)
{
    this->m_tileset = tileset;
}


IOTileSetDefinition::~IOTileSetDefinition()
{
}


void IOTileSetDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eTileSetDefinition:
        this->m_startElemName = name;
        break;

    case eTileStoreParameters:
        {
            IOTileStoreParameters* IO = new IOTileStoreParameters(this->m_tileset->GetTileStoreParameters(), this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtents:
        {
            IOExtra* IO = new IOExtra(this->m_tileset, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eBaseMapLayerGroup:
        {
            IOBaseMapLayerGroup* IO = new IOBaseMapLayerGroup(this->m_tileset, this->m_version);
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


void IOTileSetDefinition::ElementChars(const wchar_t* ch)
{

}


void IOTileSetDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_tileset = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOTileSetDefinition::Write(MdfStream& fd, TileSetDefinition* tileset, Version* version, MgTab& tab)
{
    // verify the MDF version
    MdfString strVersion;
    if (version)
    {
        if (*version == Version(3, 0, 0))
        {
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

    fd << tab.tab() << "<TileSetDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"TileSetDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    tab.inctab();

    // Property: TileStoreParameters
    IOTileStoreParameters::Write(fd, tileset->GetTileStoreParameters(), version, tab);

    // Property: Extents
    IOExtra::WriteBox2D(fd, tileset->GetExtents(), false, version, tab);

    // Property: BaseMapLayerGroups
    BaseMapLayerGroupCollection* baseMapGroups = tileset->GetBaseMapLayerGroups();
    for (int i=0; i<baseMapGroups->GetCount(); ++i)
        IOBaseMapLayerGroup::Write(fd, static_cast<BaseMapLayerGroup*>(baseMapGroups->GetAt(i)), version, tab);

    tab.dectab();
    fd << tab.tab() << "</TileSetDefinition>" << std::endl; // NOXLATE
}
