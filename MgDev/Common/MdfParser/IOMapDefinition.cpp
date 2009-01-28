//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "IOBaseMapDefinition.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOMapDefinition::IOMapDefinition(Version& version) : SAX2ElementHandler(version)
{
    this->m_map = NULL;
}


IOMapDefinition::IOMapDefinition(MapDefinition* map, Version& version) : SAX2ElementHandler(version)
{
    this->m_map = map;
}


IOMapDefinition::~IOMapDefinition()
{
}


void IOMapDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;

    if (this->m_currElemName == L"MapDefinition") // NOXLATE
    {
        this->m_startElemName = name;
    }
    else
    {
        if (this->m_currElemName == L"Extents") // NOXLATE
        {
            IOExtra* IO = new IOExtra(this->m_map, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        else if (this->m_currElemName == L"MapLayer") // NOXLATE
        {
            IOMapLayer* IO = new IOMapLayer(this->m_map, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        else if (this->m_currElemName == L"MapLayerGroup") // NOXLATE
        {
            IOMapLayerGroup* IO = new IOMapLayerGroup(this->m_map, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        else if (this->m_currElemName == L"BaseMapDefinition") // NOXLATE
        {
            IOBaseMapDefinition* IO = new IOBaseMapDefinition(this->m_map, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
    }
}


void IOMapDefinition::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"Name") // NOXLATE
        this->m_map->SetName(ch);
    else if (this->m_currElemName == L"CoordinateSystem") // NOXLATE
        this->m_map->SetCoordinateSystem(ch);
    else if (this->m_currElemName == L"BackgroundColor") // NOXLATE
        this->m_map->SetBackgroundColor(ch);
    else if (this->m_currElemName == L"Metadata") // NOXLATE
        this->m_map->SetMetadata(ch);
}


void IOMapDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_map = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOMapDefinition::Write(MdfStream& fd, MapDefinition* map, Version* version)
{
    fd << tab() << "<MapDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"MapDefinition-1.0.0.xsd\">" << std::endl; // NOXLATE
    inctab();

    // Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(map->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    fd << tab() << "<CoordinateSystem>"; // NOXLATE
    fd << EncodeString(map->GetCoordinateSystem());
    fd << "</CoordinateSystem>" << std::endl; // NOXLATE

    // Property: Extents
    IOExtra::WriteBox2D(fd, map->GetExtents(), false, version);

    // Property: BackgroundColor
    fd << tab() << "<BackgroundColor>"; // NOXLATE
    fd << EncodeString(map->GetBackgroundColor());
    fd << "</BackgroundColor>" << std::endl; // NOXLATE

    // Property: Metadata
    if (!map->GetMetadata().empty())
    {
        fd << tab() << "<Metadata>"; // NOXLATE
        fd << EncodeString(map->GetMetadata());
        fd << "</Metadata>" << std::endl; // NOXLATE
    }

    // Property: MapLayer
    for (int i=0; i<map->GetLayers()->GetCount(); ++i)
        IOMapLayer::Write(fd, map->GetLayers()->GetAt(i), version);

    // Property: MapLayerGroup
    for (int i=0; i<map->GetLayerGroups()->GetCount(); ++i)
        IOMapLayerGroup::Write(fd, map->GetLayerGroups()->GetAt(i), version);

    // Property: BaseMapDefinition
    if (map->GetFiniteDisplayScales()->GetCount() > 0)
        IOBaseMapDefinition::Write(fd, map, version);

    dectab();
    fd << tab() << "</MapDefinition>" << std::endl; // NOXLATE
}
