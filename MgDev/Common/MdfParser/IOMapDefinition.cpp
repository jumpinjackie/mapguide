//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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


IOMapDefinition::IOMapDefinition()
{
    this->_map = NULL;
}


IOMapDefinition::IOMapDefinition(MapDefinition* map)
{
    this->_map = map;
}


IOMapDefinition::~IOMapDefinition()
{
}


void IOMapDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;

    if (m_currElemName == L"MapDefinition") // NOXLATE
    {
        m_startElemName = name;
    }
    else
    {
        if (m_currElemName == L"Extents") // NOXLATE
        {
            IOExtra* IO = new IOExtra(this->_map);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        else if (m_currElemName == L"MapLayer") // NOXLATE
        {
            IOMapLayer* IO = new IOMapLayer(this->_map);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        else if (m_currElemName == L"MapLayerGroup") // NOXLATE
        {
            IOMapLayerGroup* IO = new IOMapLayerGroup(this->_map);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        else if (m_currElemName == L"BaseMapDefinition") // NOXLATE
        {
            IOBaseMapDefinition* IO = new IOBaseMapDefinition(this->_map);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
    }
}


void IOMapDefinition::ElementChars(const wchar_t* ch)
{
    if (m_currElemName == L"Name") // NOXLATE
        this->_map->SetName(ch);
    else if (m_currElemName == L"CoordinateSystem") // NOXLATE
        this->_map->SetCoordinateSystem(ch);
    else if (m_currElemName == L"BackgroundColor") // NOXLATE
        this->_map->SetBackgroundColor(ch);
    else if (m_currElemName == L"Metadata") // NOXLATE
        this->_map->SetMetadata(ch);
}


void IOMapDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        handlerStack->pop();
        this->_map = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOMapDefinition::Write(MdfStream& fd, MapDefinition* map, Version* version)
{
    fd << tab() << "<MapDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"MapDefinition-1.0.0.xsd\">" << std::endl; // NOXLATE
    inctab();

    //Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(map->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    fd << tab() << "<CoordinateSystem>"; // NOXLATE
    fd << EncodeString(map->GetCoordinateSystem());
    fd << "</CoordinateSystem>" << std::endl; // NOXLATE

    //Property: Extents
    fd << tab() << "<Extents>" << std::endl; // NOXLATE
    inctab();
    IOExtra::WriteBox2D(fd, map->GetExtents(), false, version);
    dectab();
    fd << tab() << "</Extents>" << std::endl; // NOXLATE

    //Property: BackgroundColor
    fd << tab() << "<BackgroundColor>"; // NOXLATE
    fd << EncodeString(map->GetBackgroundColor());
    fd << "</BackgroundColor>" << std::endl; // NOXLATE

    //Property: Metadata
    if (map->GetMetadata().length() > 0)
    {
        fd << tab() << "<Metadata>"; // NOXLATE
        fd << EncodeString(map->GetMetadata());
        fd << "</Metadata>" << std::endl; // NOXLATE
    }

    //Property: MapLayer
    for (int i=0; i<map->GetLayers()->GetCount(); ++i)
        IOMapLayer::Write(fd, map->GetLayers()->GetAt(i), version);

    //Property: MapLayerGroup
    for (int i=0; i<map->GetLayerGroups()->GetCount(); ++i)
        IOMapLayerGroup::Write(fd, map->GetLayerGroups()->GetAt(i), version);

    //Property: BaseMapDefinition
    if (map->GetFiniteDisplayScales()->GetCount() > 0)
        IOBaseMapDefinition::Write(fd, map, version);

    dectab();
    fd << tab() << "</MapDefinition>" << std::endl; // NOXLATE
}
