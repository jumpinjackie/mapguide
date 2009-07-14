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
#include "IOPrintLayoutElementCollection.h"
#include "IOUnknown.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, LayoutElement);
// Local Elements
ELEM_MAP_ENTRY(2, ResourceId);
ELEM_MAP_ENTRY(3, Visibility);
ELEM_MAP_ENTRY(4, Opacity);
ELEM_MAP_ENTRY(5, ExtendedData1);

IOPrintLayoutElementCollection::IOPrintLayoutElementCollection(PrintLayoutElementCollection* layoutElems, Version& version) :
    SAX2ElementHandler(version),
    m_layoutElems(layoutElems)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_layoutElems);
}

IOPrintLayoutElementCollection::~IOPrintLayoutElementCollection()
{
}

void IOPrintLayoutElementCollection::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eLayoutElement:
        m_startElemName = name;
        m_currElem.reset(new PrintLayoutElement());
        break;

    case eExtendedData1:
        m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
    }
}

void IOPrintLayoutElementCollection::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eResourceId:
        m_currElem->SetResourceId(ch);
        break;

    case eVisibility:
        m_currElem->SetVisibility(wstrToBool(ch));
        break;

    case eOpacity:
        m_currElem->SetOpacity(wstrToDouble(ch));
        break;
    }
}

void IOPrintLayoutElementCollection::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_currElem->SetUnknownXml(m_unknownXml);
        m_layoutElems->Adopt(m_currElem.release());

        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        m_procExtData = false;
    }
}

void IOPrintLayoutElementCollection::Write(MdfStream& fd, PrintLayoutElementCollection* layoutElems, Version* version, const std::string& name)
{
    _ASSERT(NULL != layoutElems);

    fd << tab() << startStr(name) << std::endl;
    inctab();

    for (int i = 0; i < layoutElems->GetCount(); ++i)
    {
       fd << tab() << startStr(sLayoutElement) << std::endl;
       inctab();

        PrintLayoutElement* layoutElem = dynamic_cast<PrintLayoutElement*>(layoutElems->GetAt(i));
        _ASSERT(NULL != layoutElem);

        // Property: ResourceId
        fd << tab() << startStr(sResourceId);
        fd << EncodeString(layoutElem->GetResourceId());
        fd << endStr(sResourceId) << std::endl;

        // Property: Visibility
        fd << tab() << startStr(sVisibility);
        fd << BoolToStr(layoutElem->GetVisibility());
        fd << endStr(sVisibility) << std::endl;

        // Property: Opacity
        fd << tab() << startStr(sOpacity);
        fd << DoubleToStr(layoutElem->GetOpacity());
        fd << endStr(sOpacity) << std::endl;

        // Write any unknown XML / extended data
        IOUnknown::Write(fd, layoutElem->GetUnknownXml(), version);

        dectab();
        fd << tab() << endStr(sLayoutElement) << std::endl;
    }

    dectab();
    fd << tab() << endStr(name) << std::endl;
}
