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
#include "IOTileWatermarkPosition.h"
#include "IOWatermarkXOffset.h"
#include "IOWatermarkYOffset.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, TilePosition);
ELEM_MAP_ENTRY(2, TileWidth);
ELEM_MAP_ENTRY(3, TileHeight);
ELEM_MAP_ENTRY(4, HorizontalPosition);
ELEM_MAP_ENTRY(5, VerticalPosition);


IOTileWatermarkPosition::IOTileWatermarkPosition(Version& version) : SAX2ElementHandler(version)
{
    this->m_position = NULL;
}


IOTileWatermarkPosition::IOTileWatermarkPosition(TileWatermarkPosition* position, Version& version) : SAX2ElementHandler(version)
{
    this->m_position = position;
}


IOTileWatermarkPosition::~IOTileWatermarkPosition()
{
}


void IOTileWatermarkPosition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eTilePosition:
        this->m_startElemName = name;
        break;

    case eHorizontalPosition:
        {
            WatermarkXOffset* xOffset = new WatermarkXOffset();
            this->m_position->AdoptHorizontalPosition(xOffset);
            IOWatermarkXOffset* IO = new IOWatermarkXOffset(xOffset, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eVerticalPosition:
        {
            WatermarkYOffset* yOffset = new WatermarkYOffset();
            this->m_position->AdoptVerticalPosition(yOffset);
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


void IOTileWatermarkPosition::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eTileWidth:
        this->m_position->SetTileWidth(wstrToDouble(ch));
        break;

    case eTileHeight:
        this->m_position->SetTileHeight(wstrToDouble(ch));
        break;
    }
}


void IOTileWatermarkPosition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
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


void IOTileWatermarkPosition::Write(MdfStream& fd, TileWatermarkPosition* position, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sTilePosition) << std::endl;
    tab.inctab();

    // Property: TileWidth
    fd << tab.tab() << startStr(sTileWidth);
    fd << DoubleToStr(position->GetTileWidth());
    fd << endStr(sTileWidth) << std::endl;

    // Property: TileHeight
    fd << tab.tab() << startStr(sTileHeight);
    fd << DoubleToStr(position->GetTileHeight());
    fd << endStr(sTileHeight) << std::endl;

    // Property: HorizontalPosition
    IOWatermarkXOffset::Write(fd, position->GetHorizontalPosition(), sHorizontalPosition, version, tab);

    // Property: VerticalPosition
    IOWatermarkYOffset::Write(fd, position->GetVerticalPosition(), sVerticalPosition, version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sTilePosition) << std::endl;
}
