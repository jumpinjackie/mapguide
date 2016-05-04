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
#include "IOTileSetSource.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOTileSetSource::IOTileSetSource(Version& version) : SAX2ElementHandler(version)
{
    this->m_source = NULL;
}

IOTileSetSource::IOTileSetSource(TileSetSource* source, Version& version) : SAX2ElementHandler(version)
{
    this->m_source = source;
}

IOTileSetSource::~IOTileSetSource()
{

}

void IOTileSetSource::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"TileSetSource") // NOXLATE
    {
        this->m_startElemName = name;
    }
}

void IOTileSetSource::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"ResourceId") // NOXLATE
    {
        this->m_source->SetResourceId(ch);
    }
}

void IOTileSetSource::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_source = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOTileSetSource::Write(MdfStream& fd, TileSetSource* source, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<TileSetSource>" << std::endl; // NOXLATE
    tab.inctab();

    fd << tab.tab() << startStr("ResourceId"); //NOXLATE
    fd << EncodeString(source->GetResourceId());
    fd << endStr("ResourceId") << std::endl; //NOXLATE

    tab.dectab();
    fd << tab.tab() << "</TileSetSource>" << std::endl; // NOXLATE
}