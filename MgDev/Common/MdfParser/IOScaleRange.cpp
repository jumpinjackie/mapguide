//
//  Copyright (C) 2011 by Autodesk, Inc.
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
#include "IOScaleRange.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ScaleRange);
ELEM_MAP_ENTRY(2, MinScale);
ELEM_MAP_ENTRY(3, MaxScale);
ELEM_MAP_ENTRY(4, ExtendedData1);


IOScaleRange::IOScaleRange(Version& version) : SAX2ElementHandler(version)
{
    this->m_scaleRange = NULL;
}


IOScaleRange::IOScaleRange(ScaleRange* scaleRange, Version& version) : SAX2ElementHandler(version)
{
    this->m_scaleRange = scaleRange;
}


IOScaleRange::~IOScaleRange()
{
}


void IOScaleRange::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOScaleRange::ElementChars(const wchar_t* ch)
{
    //TODO: Implement this while deserialization is needed.
}


void IOScaleRange::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOScaleRange::Write(MdfStream& fd, ScaleRange* scaleRange, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sScaleRange) << std::endl;
    tab.inctab();

    // Property: MinScale
    fd << tab.tab() << startStr(sMinScale);
    fd << DoubleToStr(scaleRange->GetMinScale());
    fd << endStr(sMinScale) << std::endl;

    // Property: MaxScale
    fd << tab.tab() << startStr(sMaxScale);
    fd << DoubleToStr(scaleRange->GetMaxScale());
    fd << endStr(sMaxScale) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, scaleRange->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sScaleRange) << std::endl;
}
