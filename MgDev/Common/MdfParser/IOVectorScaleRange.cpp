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
#include "IOElevationSettings.h"
#include "IOUnknown.h"

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


IOVectorScaleRange::IOVectorScaleRange(VectorLayerDefinition* layer)
{
    this->_scaleRange = NULL;
    this->layer = layer;
}


IOVectorScaleRange::~IOVectorScaleRange()
{
}


void IOVectorScaleRange::StartElement(const wchar_t* name, HandlerStack* handlerStack)
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
            IOAreaTypeStyle* IO = new IOAreaTypeStyle(this->_scaleRange);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eLineTypeStyle:
        {
            IOLineTypeStyle* IO = new IOLineTypeStyle(this->_scaleRange);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case ePointTypeStyle:
        {
            IOPointTypeStyle* IO = new IOPointTypeStyle(this->_scaleRange);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eElevationSettings:
        {
            IOElevationSettings* IO = new IOElevationSettings(this->_scaleRange);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eCompositeTypeStyle:
        {
            IOCompositeTypeStyle* IO = new IOCompositeTypeStyle(this->_scaleRange);
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


void IOVectorScaleRange::ElementChars(const wchar_t* ch)
{
    if (m_currElemName == L"MinScale") // NOXLATE
        this->_scaleRange->SetMinScale(wstrToDouble(ch));
    else if (m_currElemName == L"MaxScale") // NOXLATE
        this->_scaleRange->SetMaxScale(wstrToDouble(ch));
}


void IOVectorScaleRange::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_scaleRange->SetUnknownXml(UnknownXml());

        this->layer->GetScaleRanges()->Adopt(this->_scaleRange);
        this->layer = NULL;
        this->_scaleRange = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOVectorScaleRange::Write(MdfStream& fd, VectorScaleRange* scaleRange, Version* version)
{
    fd << tab() << "<VectorScaleRange>" << std::endl; // NOXLATE
    inctab();

    MdfStringStream fdExtData;

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
    for (int i=0; i<scaleRange->GetFeatureTypeStyles()->GetCount(); ++i)
    {
        FeatureTypeStyle* fts = scaleRange->GetFeatureTypeStyles()->GetAt(i);

        if (dynamic_cast<AreaTypeStyle*>(fts) != 0)
        {
            IOAreaTypeStyle::Write(fd, dynamic_cast<AreaTypeStyle*>(fts), version);
        }
        else if (dynamic_cast<LineTypeStyle*>(fts) != 0)
        {
            IOLineTypeStyle::Write(fd, dynamic_cast<LineTypeStyle*>(fts), version);
        }
        else if (dynamic_cast<PointTypeStyle*>(fts) != 0)
        {
            IOPointTypeStyle::Write(fd, dynamic_cast<PointTypeStyle*>(fts), version);
        }
        else if (dynamic_cast<CompositeTypeStyle*>(fts) != 0)
        {
            // only write CompositeTypeStyle if the LDF version is 1.1.0 or greater
            if (!version || (*version >= Version(1, 1, 0)))
            {
                IOCompositeTypeStyle::Write(fd, dynamic_cast<CompositeTypeStyle*>(fts), version);
            }
            else if (*version == Version(1, 0, 0))
            {
                // save CompositeTypeStyle as extended data for LDF version 1.0.0
                inctab();
                IOCompositeTypeStyle::Write(fdExtData, dynamic_cast<CompositeTypeStyle*>(fts), version);
                dectab();
            }
        }
    }

    //Property: ElevationSettings
    ElevationSettings* elevationSettings = scaleRange->GetElevationSettings();
    if (elevationSettings != NULL)
    {
        // only write ElevationSettings if the LDF version is 1.1.0 or greater
        if (!version || (*version >= Version(1, 1, 0)))
        {
            IOElevationSettings::Write(fd, elevationSettings, version);
        }
        else if (*version == Version(1, 0, 0))
        {
            // save ElevationSettings as extended data for LDF version 1.0.0
            inctab();
            IOElevationSettings::Write(fdExtData, elevationSettings, version);
            dectab();
        }
    }

    // Add any unknown XML to the extended data
    if (!scaleRange->GetUnknownXml().empty())
    {
        inctab();
        fdExtData << tab() << toCString(scaleRange->GetUnknownXml());
        dectab();
    }

    // Write the unknown XML / extended data
    IOUnknown::WriteRaw(fd, fdExtData.str(), version);

    dectab();
    fd << tab() << "</VectorScaleRange>" << std::endl; // NOXLATE
}
