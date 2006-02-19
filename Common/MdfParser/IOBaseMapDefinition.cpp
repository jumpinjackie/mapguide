//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "IOBaseMapDefinition.h"
#include "IOExtra.h"
#include "IOVectorLayerDefinition.h"
#include "IODrawingLayerDefinition.h"
#include "IOBaseMapLayerGroup.h"
#include "DisplayScale.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOBaseMapDefinition::IOBaseMapDefinition()
{
}

IOBaseMapDefinition::IOBaseMapDefinition(MapDefinition *map)
{
    this->map = map;
}

IOBaseMapDefinition::~IOBaseMapDefinition()
{
}

void IOBaseMapDefinition::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"BaseMapDefinition") // NOXLATE
    {
        m_startElemName = name;
    }
    else
    {
        if (m_currElemName == L"FiniteDisplayScale")
        {
            // default processing in ElementChars
        }
        else if (m_currElemName == L"BaseMapLayerGroup")
        {
            IOBaseMapLayerGroup * IO = new IOBaseMapLayerGroup(this->map);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
    }
}

void IOBaseMapDefinition::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"FiniteDisplayScale") // NOXLATE
    {
        double val = wstrToDouble(ch);
        (this->map)->GetFiniteDisplayScales()->Adopt(new DisplayScale(val));
    }
}

void IOBaseMapDefinition::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        handlerStack->pop();
        this->map = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOBaseMapDefinition::Write(MdfStream &fd, MapDefinition * map)
{
    fd << tab() << "<BaseMapDefinition>" << std::endl; // NOXLATE
    inctab();

    //Property: FiniteDisplayScales
    DisplayScaleCollection * finiteDisplayScales = this->map->GetFiniteDisplayScales();
    for (int i = 0; i < finiteDisplayScales->GetCount(); i++)
    {
        fd << tab() << "<FiniteDisplayScale>"; // NOXLATE
        fd << DoubleToStr((static_cast<DisplayScale*>(finiteDisplayScales->GetAt(i)))->GetValue());
        fd << "</FiniteDisplayScale>" << std::endl; // NOXLATE
    }

    //Property: BaseMapLayerGroups
    BaseMapLayerGroupCollection * baseMapGroups = this->map->GetBaseMapLayerGroups();
    IOBaseMapLayerGroup * ioBaseMapLayerGroup = new IOBaseMapLayerGroup();
    for (int i = 0; i < baseMapGroups->GetCount(); i++)
    {
        ioBaseMapLayerGroup->Write(fd, static_cast<BaseMapLayerGroup*>(baseMapGroups->GetAt(i)));
    }
    delete ioBaseMapLayerGroup;

    dectab();
    fd << tab() << "</BaseMapDefinition>" << std::endl; // NOXLATE
}
