//
//  Copyright (C) 2010-2011 by Autodesk, Inc.
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
#include "IOXYWatermarkPosition.h"
#include "IOWatermarkXOffset.h"
#include "IOWatermarkYOffset.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, XYPosition);
ELEM_MAP_ENTRY(2, XPosition);
ELEM_MAP_ENTRY(3, YPosition);


IOXYWatermarkPosition::IOXYWatermarkPosition(Version& version) : SAX2ElementHandler(version)
{
    this->m_position = NULL;
}


IOXYWatermarkPosition::IOXYWatermarkPosition(XYWatermarkPosition* position, Version& version) : SAX2ElementHandler(version)
{
    this->m_position = position;
}


IOXYWatermarkPosition::~IOXYWatermarkPosition()
{
}


void IOXYWatermarkPosition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eXYPosition:
        this->m_startElemName = name;
        break;

    case eXPosition:
        {
            WatermarkXOffset* xOffset = new WatermarkXOffset();
            this->m_position->AdoptXPosition(xOffset);
            IOWatermarkXOffset* IO = new IOWatermarkXOffset(xOffset, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eYPosition:
        {
            WatermarkYOffset* yOffset = new WatermarkYOffset();
            this->m_position->AdoptYPosition(yOffset);
            IOWatermarkYOffset* IO = new IOWatermarkYOffset(yOffset, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOXYWatermarkPosition::ElementChars(const wchar_t* ch)
{
}


void IOXYWatermarkPosition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_position->SetUnknownXml(this->m_unknownXml);

        this->m_position = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOXYWatermarkPosition::Write(MdfStream& fd, XYWatermarkPosition* position, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sXYPosition) << std::endl;
    tab.inctab();

    // Property: XPosition
    IOWatermarkXOffset::Write(fd, position->GetXPosition(), sXPosition, version, tab);

    // Property: YPosition
    IOWatermarkYOffset::Write(fd, position->GetYPosition(), sYPosition, version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sXYPosition) << std::endl;
}
