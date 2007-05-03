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
#include "IOVectorScaleRange.h"
#include "IOAreaTypeStyle.h"
#include "IOLineTypeStyle.h"
#include "IOPointTypeStyle.h"
#include "IOCompositeTypeStyle.h"
#include "IFeatureTypeStyleVisitor.h"
#include "IOElevationSettings.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, VectorScaleRange);
ELEM_MAP_ENTRY(2, AreaTypeStyle);
ELEM_MAP_ENTRY(3, LineTypeStyle);
ELEM_MAP_ENTRY(4, PointTypeStyle);
ELEM_MAP_ENTRY(5, CompositeTypeStyle);
ELEM_MAP_ENTRY(6, MinScale);
ELEM_MAP_ENTRY(7, MaxScale);
ELEM_MAP_ENTRY(8, ElevationSettings);


IOVectorScaleRange::IOVectorScaleRange()
{
    this->_scaleRange = NULL;
    this->layer = NULL;
}

IOVectorScaleRange::IOVectorScaleRange(VectorLayerDefinition *layer)
{
    this->_scaleRange = NULL;
    this->layer = layer;
}

IOVectorScaleRange::~IOVectorScaleRange()
{
}

void IOVectorScaleRange::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eVectorScaleRange:
        m_startElemName = name;
        this->_scaleRange = new VectorScaleRange();
        break;

    case eAreaTypeStyle:
        {
            IOAreaTypeStyle *IO = new IOAreaTypeStyle(this->_scaleRange);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eLineTypeStyle:
        {
            IOLineTypeStyle *IO = new IOLineTypeStyle(this->_scaleRange);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case ePointTypeStyle:
        {
            IOPointTypeStyle *IO = new IOPointTypeStyle(this->_scaleRange);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eElevationSettings:
        {
            IOElevationSettings *IO = new IOElevationSettings(this->_scaleRange);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eCompositeTypeStyle:
        {
            IOCompositeTypeStyle *IO = new IOCompositeTypeStyle(this->_scaleRange);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}

void IOVectorScaleRange::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"MinScale") // NOXLATE
        (this->_scaleRange)->SetMinScale(wstrToDouble(ch));
    else if (m_currElemName == L"MaxScale") // NOXLATE
        (this->_scaleRange)->SetMaxScale(wstrToDouble(ch));
}

void IOVectorScaleRange::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_scaleRange->SetUnknownXml(UnknownXml());

        this->layer->GetScaleRanges()->Adopt(this->_scaleRange);
        this->layer = NULL;
        this->_scaleRange = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOVectorScaleRange::Write(MdfStream &fd, VectorScaleRange *scaleRange, Version* version)
{
    fd << tab() << "<VectorScaleRange>" << std::endl; // NOXLATE
    inctab();

    //Property: MinScale (optional)
    if (scaleRange->GetMinScale() != 0.0)
    {
        fd << tab() << "<MinScale>"; // NOXLATE
        fd << DoubleToStr(scaleRange->GetMinScale());
        fd << "</MinScale>" << std::endl; // NOXLATE
    }

    //Property: MaxScale (optional)
    if (scaleRange->GetMaxScale() != VectorScaleRange::MAX_MAP_SCALE)
    {
        fd << tab() << "<MaxScale>"; // NOXLATE
        fd << DoubleToStr(scaleRange->GetMaxScale());
        fd << "</MaxScale>" << std::endl; // NOXLATE
    }

    //Property: FeatureTypeStyle
    for (int x = 0; x < scaleRange->GetFeatureTypeStyles()->GetCount(); x++)
    {
        if (dynamic_cast<AreaTypeStyle*>(scaleRange->GetFeatureTypeStyles()->GetAt(x)) != 0)
        {
            IOAreaTypeStyle * IO = new IOAreaTypeStyle();
            IO->Write(fd, dynamic_cast<AreaTypeStyle*>(scaleRange->GetFeatureTypeStyles()->GetAt(x)), version);
            delete IO;
        }
        else if (dynamic_cast<LineTypeStyle*>(scaleRange->GetFeatureTypeStyles()->GetAt(x)) != 0)
        {
            IOLineTypeStyle * IO = new IOLineTypeStyle();
            IO->Write(fd, dynamic_cast<LineTypeStyle*>(scaleRange->GetFeatureTypeStyles()->GetAt(x)), version);
            delete IO;
        }
        else if (dynamic_cast<PointTypeStyle*>(scaleRange->GetFeatureTypeStyles()->GetAt(x)) != 0)
        {
            IOPointTypeStyle * IO = new IOPointTypeStyle();
            IO->Write(fd, dynamic_cast<PointTypeStyle*>(scaleRange->GetFeatureTypeStyles()->GetAt(x)), version);
            delete IO;
        }
        else if (dynamic_cast<CompositeTypeStyle*>(scaleRange->GetFeatureTypeStyles()->GetAt(x)) != 0)
        {
            if (!version || ((*version) >= Version(1, 1, 0))) // don't write to pre-1.1.0 schema
                IOCompositeTypeStyle::Write(fd, dynamic_cast<CompositeTypeStyle*>(scaleRange->GetFeatureTypeStyles()->GetAt(x)), version);
            else
            {
                // TODO - save the composite type style as extended data
            }
        }
    }

    if (!version || ((*version) >= Version(1, 1, 0))) // don't write to pre-1.1.0 schema
    {
        ElevationSettings* elevationSettings = scaleRange->GetElevationSettings();
        if(elevationSettings != NULL)
        {
            IOElevationSettings * IO = new IOElevationSettings();
            IO->Write(fd, elevationSettings);
            delete IO;
        }
    }
    else
    {
        // TODO - save the elevation settings as extended data
    }

    // Write any previously found unknown XML
    if (!scaleRange->GetUnknownXml().empty())
        fd << tab() << toCString(scaleRange->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</VectorScaleRange>" << std::endl; // NOXLATE
}
