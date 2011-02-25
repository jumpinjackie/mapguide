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
#include "IOWatermarkInstance.h"
#include "IOWatermarkAppearance.h"
#include "IOXYWatermarkPosition.h"
#include "IOTileWatermarkPosition.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Watermark);
ELEM_MAP_ENTRY(2, Name);
ELEM_MAP_ENTRY(3, ResourceId);
ELEM_MAP_ENTRY(4, Usage);
ELEM_MAP_ENTRY(5, AppearanceOverride);
ELEM_MAP_ENTRY(6, PositionOverride);
ELEM_MAP_ENTRY(7, XYPosition);
ELEM_MAP_ENTRY(8, TilePosition);
ELEM_MAP_ENTRY(9, ExtendedData1);


IOWatermarkInstance::IOWatermarkInstance(Version& version) : SAX2ElementHandler(version)
{
    this->m_watermark = NULL;
}


IOWatermarkInstance::IOWatermarkInstance(WatermarkInstance* watermark, Version& version) : SAX2ElementHandler(version)
{
    this->m_watermark = watermark;
}


IOWatermarkInstance::~IOWatermarkInstance()
{
}


void IOWatermarkInstance::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eWatermark:
        this->m_startElemName = name;
        break;

    case eAppearanceOverride:
        {
            WatermarkAppearance* appearance = new WatermarkAppearance();
            this->m_watermark->AdoptAppearanceOverride(appearance);
            IOWatermarkAppearance* IO = new IOWatermarkAppearance(appearance, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eXYPosition:
        {
            XYWatermarkPosition* position = new XYWatermarkPosition();
            this->m_watermark->AdoptPositionOverride(position);
            IOXYWatermarkPosition* IO = new IOXYWatermarkPosition(position, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eTilePosition:
        {
            TileWatermarkPosition* position = new TileWatermarkPosition();
            this->m_watermark->AdoptPositionOverride(position);
            IOTileWatermarkPosition* IO = new IOTileWatermarkPosition(position, this->m_version);
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


void IOWatermarkInstance::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eName:
        this->m_watermark->SetName(ch);
        break;

    case eResourceId:
        this->m_watermark->SetResourceId(ch);
        break;

    case eUsage:
        if (::wcscmp(ch, L"WMS") == 0) // NOXLATE
            this->m_watermark->SetUsage(WatermarkInstance::WMS);
        else if (::wcscmp(ch, L"Viewer") == 0) // NOXLATE
            this->m_watermark->SetUsage(WatermarkInstance::Viewer);
        else
            this->m_watermark->SetUsage(WatermarkInstance::All); // treat as "All" if string is incorrect
        break;
    }
}


void IOWatermarkInstance::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_watermark->SetUnknownXml(this->m_unknownXml);

        this->m_watermark = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOWatermarkInstance::Write(MdfStream& fd, WatermarkInstance* watermark, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sWatermark) << std::endl;
    tab.inctab();

    // Property: Name
    fd << tab.tab() << startStr(sName);
    fd << EncodeString(watermark->GetName());
    fd << endStr(sName) << std::endl;

    // Property: ResourceId
    fd << tab.tab() << startStr(sResourceId);
    fd << EncodeString(watermark->GetResourceId());
    fd << endStr(sResourceId) << std::endl;

    // Property: Usage (optional)
    WatermarkInstance::Usage usage = watermark->GetUsage();
    if (usage != WatermarkInstance::All)
    {
        fd << tab.tab() << startStr(sUsage);
        if (usage == WatermarkInstance::WMS)
            fd << "WMS"; // NOXLATE
        else if (usage == WatermarkInstance::Viewer)
            fd << "Viewer"; // NOXLATE
        fd << endStr(sUsage) << std::endl;
    }

    // Property: AppearanceOverride (optional)
    WatermarkAppearance* appearanceOverride = watermark->GetAppearanceOverride();
    if (appearanceOverride)
        IOWatermarkAppearance::Write(fd, appearanceOverride, version, sAppearanceOverride, tab);

    // Property: PositionOverride (optional)
    WatermarkPosition* positionOverride = watermark->GetPositionOverride();
    if (positionOverride)
    {
        fd << tab.tab() << startStr(sPositionOverride) << std::endl;
        tab.inctab();

        XYWatermarkPosition* xyPositionOverride = dynamic_cast<XYWatermarkPosition*>(positionOverride);
        TileWatermarkPosition* tilePositionOverride = dynamic_cast<TileWatermarkPosition*>(positionOverride);

        if (xyPositionOverride)
            IOXYWatermarkPosition::Write(fd, xyPositionOverride, version, tab);
        else if (tilePositionOverride)
            IOTileWatermarkPosition::Write(fd, tilePositionOverride, version, tab);

        tab.dectab();
        fd << endStr(sPositionOverride) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, watermark->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sWatermark) << std::endl;
}
