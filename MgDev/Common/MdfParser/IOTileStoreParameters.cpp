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
#include "IOTileStoreParameters.h"
#include "IONameStringPair.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOTileStoreParameters::IOTileStoreParameters(Version& version) : SAX2ElementHandler(version)
{
    this->m_params = NULL;
}

IOTileStoreParameters::IOTileStoreParameters(TileStoreParameters* params, Version& version) : SAX2ElementHandler(version)
{
    this->m_params = params;
}

IOTileStoreParameters::~IOTileStoreParameters()
{

}

void IOTileStoreParameters::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"TileStoreParameters") // NOXLATE
    {
        this->m_startElemName = name;
    }
    else
    {
        if (this->m_currElemName == L"Parameter")
        {
            IONameStringPair* IO = new IONameStringPair(this->m_params, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
    }
}

void IOTileStoreParameters::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"TileProvider") // NOXLATE
    {
        this->m_params->SetTileProvider(ch);
    }
}

void IOTileStoreParameters::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_params = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOTileStoreParameters::Write(MdfStream& fd, TileStoreParameters* params, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<TileStoreParameters>" << std::endl; // NOXLATE
    tab.inctab();

    // Property: TileProvider
    fd << tab.tab() << startStr("TileProvider"); //NOXLATE
    fd << EncodeString(params->GetTileProvider());
    fd << endStr("TileProvider") << std::endl; //NOXLATE

    // Property: Parameters
    for (int i=0; i<params->GetParameters()->GetCount(); ++i)
        IONameStringPair::Write(fd, "Parameter", params->GetParameters()->GetAt(i), version, tab); //NOXLATE

    tab.dectab();
    fd << tab.tab() << "</TileStoreParameters>" << std::endl; // NOXLATE
}