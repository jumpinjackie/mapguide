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
#include "IOAreaTypeStyle.h"
#include "IOAreaRule.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOAreaTypeStyle::IOAreaTypeStyle()
{
}

IOAreaTypeStyle::IOAreaTypeStyle(VectorScaleRange * scaleRange)
{
    this->scaleRange = scaleRange;
}

IOAreaTypeStyle::~IOAreaTypeStyle()
{
}

void IOAreaTypeStyle::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"AreaTypeStyle") // NOXLATE
    {
        m_startElemName = name;
        this->_areaTypeStyle = new AreaTypeStyle();
    }
    else if (m_currElemName == L"AreaRule") // NOXLATE
    {
        IOAreaRule *IO = new IOAreaRule(this->_areaTypeStyle);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOAreaTypeStyle::ElementChars(const wchar_t *ch)
{

}

void IOAreaTypeStyle::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->scaleRange->GetFeatureTypeStyles()->Adopt(this->_areaTypeStyle);
        handlerStack->pop();
        this->scaleRange = NULL;
        this->_areaTypeStyle = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOAreaTypeStyle::Write(MdfStream &fd, AreaTypeStyle * areaTypeStyle)
{
    fd << tab() << "<AreaTypeStyle>" << std::endl; // NOXLATE
    inctab();

    //Property: Rules
    for (int x = 0; x < areaTypeStyle->GetRules()->GetCount(); x++)
    {
        IOAreaRule *IO = new IOAreaRule();
        IO->Write(fd, static_cast<AreaRule*>(areaTypeStyle->GetRules()->GetAt(x)));
        delete IO;
    }

    dectab();
    fd << tab() << "</AreaTypeStyle>" << std::endl; // NOXLATE
}
