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
#include "SAX2ElementHandler.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


SAX2ElementHandler::SAX2ElementHandler()
{
    this->m_currElemId = -1;
    this->m_procExtData = false;
}


SAX2ElementHandler::~SAX2ElementHandler()
{
}


void SAX2ElementHandler::ParseUnknownXml(const wchar_t* name, HandlerStack* handlerStack)
{
    IOUnknown* IO = new IOUnknown(&this->m_unknownXml, false);
    handlerStack->push(IO);
    IO->StartElement(name, handlerStack);
}


void SAX2ElementHandler::ParseUnknownXml2(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_procExtData)
    {
        // store any unknown elements encountered while
        // processing extended data
        IOUnknown* IO = new IOUnknown(&this->m_unknownXml, true);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
//  else
//  {
//      // unknown element encountered
//      _ASSERT(false);
//  }
}
