//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "IODrawingLayerDefinition.h"
#include "VectorScaleRange.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, DrawingLayerDefinition);
ELEM_MAP_ENTRY(2, ResourceId);
ELEM_MAP_ENTRY(3, Opacity);
ELEM_MAP_ENTRY(4, Sheet);
ELEM_MAP_ENTRY(5, LayerFilter);
ELEM_MAP_ENTRY(6, MinScale);
ELEM_MAP_ENTRY(7, MaxScale);
ELEM_MAP_ENTRY(8, ExtendedData1);


IODrawingLayerDefinition::IODrawingLayerDefinition(Version& version) : SAX2ElementHandler(version)
{
    this->m_layer = NULL;
}


IODrawingLayerDefinition::IODrawingLayerDefinition(DrawingLayerDefinition* layer, Version& version) : SAX2ElementHandler(version)
{
    this->m_layer = layer;
}


IODrawingLayerDefinition::~IODrawingLayerDefinition()
{
}


void IODrawingLayerDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eDrawingLayerDefinition:
        this->m_startElemName = name;
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IODrawingLayerDefinition::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eResourceId:
        this->m_layer->SetResourceID(ch);
        break;

    case eOpacity:
        this->m_layer->SetOpacity(wstrToDouble(ch));
        break;

    case eSheet:
        this->m_layer->SetSheet(ch);
        break;

    case eLayerFilter:
        this->m_layer->SetLayerFilter(ch);
        break;

    case eMinScale:
        this->m_layer->SetMinScale(wstrToDouble(ch));
        break;

    case eMaxScale:
        this->m_layer->SetMaxScale(wstrToDouble(ch));
        break;
    }
}


void IODrawingLayerDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_layer->SetUnknownXml(this->m_unknownXml);

        this->m_layer = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IODrawingLayerDefinition::Write(MdfStream& fd, DrawingLayerDefinition* drawingLayer, Version* version)
{
    // verify the LDF version
    MdfString strVersion;
    if (version)
    {
        if (*version == Version(0, 9, 0))
        {
            // LDF in MapGuide 2006
            strVersion = L"1.0.0";
        }
        else if ((*version >= Version(1, 0, 0)) && (*version <= Version(1, 3, 0)))
        {
            // LDF in MapGuide 2007 / 2008 / 2009 / 2010
            strVersion = version->ToString();
        }
        else
        {
            // unsupported LDF version
            // TODO - need a way to return error information
            _ASSERT(false);
            return;
        }
    }
    else
    {
        // use the current highest version
        strVersion = L"1.3.0";
    }

    fd << tab() << "<LayerDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"LayerDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    inctab();

    fd << tab() << startStr(sDrawingLayerDefinition) << std::endl;
    inctab();

    // Property: ResourceId
    fd << tab() << startStr(sResourceId);
    fd << EncodeString(drawingLayer->GetResourceID());
    fd << endStr(sResourceId) << std::endl;

    // Property: Opacity (optional)
    if (drawingLayer->GetOpacity() != 1.0)
    {
        fd << tab() << startStr(sOpacity);
        fd << DoubleToStr(drawingLayer->GetOpacity());
        fd << endStr(sOpacity) << std::endl;
    }

    // Property: Sheet
    fd << tab() << startStr(sSheet);
    fd << EncodeString(drawingLayer->GetSheet());
    fd << endStr(sSheet) << std::endl;

    // Property: LayerFilter (optional)
    if (!drawingLayer->GetLayerFilter().empty())
    {
        fd << tab() << startStr(sLayerFilter);
        fd << EncodeString(drawingLayer->GetLayerFilter());
        fd << endStr(sLayerFilter) << std::endl;
    }

    // Property: MinScale (optional)
    if (drawingLayer->GetMinScale() != 0.0)
    {
        fd << tab() << startStr(sMinScale);
        fd << DoubleToStr(drawingLayer->GetMinScale());
        fd << endStr(sMinScale) << std::endl;
    }

    // Property: MaxScale (optional)
    if (drawingLayer->GetMaxScale() != VectorScaleRange::MAX_MAP_SCALE)
    {
        fd << tab() << startStr(sMaxScale);
        fd << DoubleToStr(drawingLayer->GetMaxScale());
        fd << endStr(sMaxScale) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, drawingLayer->GetUnknownXml(), version);

    dectab();
    fd << tab() << endStr(sDrawingLayerDefinition) << std::endl;

    dectab();
    fd << tab() << "</LayerDefinition>" << std::endl; // NOXLATE
}
