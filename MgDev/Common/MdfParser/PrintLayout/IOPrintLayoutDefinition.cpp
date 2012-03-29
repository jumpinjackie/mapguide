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

#include "../stdafx.h"
#include "../IOUnknown.h"
#include "../Common/IOColor.h"
#include "../Common/IOExtent3D.h"
#include "../Common/IOSize2D.h"
#include "IOPrintLayoutDefinition.h"
#include "IOThickness.h"
#include "IOPrintLayoutElementCollection.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, PrintLayoutDefinition);
// Local Elements
ELEM_MAP_ENTRY(2, Name);
ELEM_MAP_ENTRY(3, Extent);
ELEM_MAP_ENTRY(4, Elements);
ELEM_MAP_ENTRY(5, PaperSize);
ELEM_MAP_ENTRY(6, DeviceName);
ELEM_MAP_ENTRY(7, MediaName);
ELEM_MAP_ENTRY(8, Orientation);
ELEM_MAP_ENTRY(9, PaperMargin);
ELEM_MAP_ENTRY(10, Units);
ELEM_MAP_ENTRY(11, BackgroundColor);
ELEM_MAP_ENTRY(12, ExtendedData1);

IOPrintLayoutDefinition::IOPrintLayoutDefinition(PrintLayoutDefinition* layoutDef, Version& version) :
    SAX2ElementHandler(version),
    m_layoutDef(layoutDef)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_layoutDef);
}

IOPrintLayoutDefinition::~IOPrintLayoutDefinition()
{
}

void IOPrintLayoutDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case ePrintLayoutDefinition:
        m_startElemName = name;
        break;

    case eExtent:
        {
            IOExtent3D* IO = new IOExtent3D(m_layoutDef->GetExtent(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eElements:
        {
            IOPrintLayoutElementCollection* IO = new IOPrintLayoutElementCollection(m_layoutDef->GetElements(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case ePaperSize:
        {
            IOSize2D* IO = new IOSize2D(m_layoutDef->GetPaperSize(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case ePaperMargin:
        {
            IOThickness* IO = new IOThickness(m_layoutDef->GetPaperMargin(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eBackgroundColor:
        {
            IOColor* IO = new IOColor(m_layoutDef->GetBackgroundColor(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData1:
        m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}

void IOPrintLayoutDefinition::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eName:
        m_layoutDef->SetName(ch);
        break;

    case eDeviceName:
        m_layoutDef->SetDeviceName(ch);
        break;

    case eMediaName:
        m_layoutDef->SetMediaName(ch);
        break;

    case eOrientation:
        m_layoutDef->SetOrientation(ch);
        break;

    case eUnits:
        m_layoutDef->SetUnits(ch);
        break;
    }
}

void IOPrintLayoutDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_layoutDef->SetUnknownXml(m_unknownXml);

        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        m_procExtData = false;
    }
}

void IOPrintLayoutDefinition::Write(MdfStream& fd, PrintLayoutDefinition* layoutDef, Version* version, MgTab& tab)
{
    _ASSERT(NULL != layoutDef);

    // Set the expected version
    MdfString strVersion = L"2.0.0";

    fd << tab.tab() << "<PrintLayoutDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"PrintLayoutDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    tab.inctab();

    // Property: Name
    fd << tab.tab() << startStr(sName);
    fd << EncodeString(layoutDef->GetName());
    fd << endStr(sName) << std::endl;

    // Property: Extent
    IOExtent3D::Write(fd, layoutDef->GetExtent(), version, tab);

    // Property: Elements
    IOPrintLayoutElementCollection::Write(fd, layoutDef->GetElements(), version, sElements, tab);

    // Property: PaperSize
    IOSize2D::Write(fd, layoutDef->GetPaperSize(), version, sPaperSize, tab);

    // Property: DeviceName
    fd << tab.tab() << startStr(sDeviceName);
    fd << EncodeString(layoutDef->GetDeviceName());
    fd << endStr(sDeviceName) << std::endl;

    // Property: MediaName
    fd << tab.tab() << startStr(sMediaName);
    fd << EncodeString(layoutDef->GetMediaName());
    fd << endStr(sMediaName) << std::endl;

    // Property: Orientation
    fd << tab.tab() << startStr(sOrientation);
    fd << EncodeString(layoutDef->GetOrientation());
    fd << endStr(sOrientation) << std::endl;

    // Property: PaperMargin
    IOThickness::Write(fd, layoutDef->GetPaperMargin(), version, sPaperMargin, tab);

    // Property: Units
    fd << tab.tab() << startStr(sUnits);
    fd << EncodeString(layoutDef->GetUnits());
    fd << endStr(sUnits) << std::endl;

    // Property: BackgroundColor
    IOColor::Write(fd, layoutDef->GetBackgroundColor(), version, sBackgroundColor, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, layoutDef->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sPrintLayoutDefinition) << std::endl;
}
