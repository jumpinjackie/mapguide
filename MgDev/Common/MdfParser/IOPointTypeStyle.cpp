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
#include "IOPointTypeStyle.h"
#include "IOPointRule.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOPointTypeStyle::IOPointTypeStyle()
{
}

IOPointTypeStyle::IOPointTypeStyle(VectorScaleRange * scaleRange)
{
    this->scaleRange = scaleRange;
}

IOPointTypeStyle::~IOPointTypeStyle()
{
}

void IOPointTypeStyle::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"PointTypeStyle") // NOXLATE
    {
        m_startElemName = name;
        this->_pointTypeStyle = new PointTypeStyle();
    }
    else if (m_currElemName == L"PointRule") // NOXLATE
    {
        IOPointRule *IO = new IOPointRule(this->_pointTypeStyle);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOPointTypeStyle::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"DisplayAsText") // NOXLATE
        (this->_pointTypeStyle)->SetDisplayAsText(wstrToBool(ch));
    else if (m_currElemName == L"AllowOverpost") // NOXLATE
        (this->_pointTypeStyle)->SetAllowOverpost(wstrToBool(ch));
}

void IOPointTypeStyle::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->scaleRange->GetFeatureTypeStyles()->Adopt(this->_pointTypeStyle);
        handlerStack->pop();
        this->scaleRange = NULL;
        this->_pointTypeStyle = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOPointTypeStyle::Write(MdfStream &fd, PointTypeStyle *pointTypeStyle)
{
    fd << tab() << "<PointTypeStyle>" << std::endl; // NOXLATE
    inctab();

    //Property: DisplayAsText
    fd << tab() << "<DisplayAsText>"; // NOXLATE
    fd << (BoolToStr(pointTypeStyle->IsDisplayAsText())) ? ("true") : ("false"); // NOXLATE
    fd << "</DisplayAsText>" << std::endl; // NOXLATE

    //Property: AllowOverpost
    fd << tab() << "<AllowOverpost>"; // NOXLATE
    fd << (BoolToStr(pointTypeStyle->IsAllowOverpost())) ? ("true") : ("false"); // NOXLATE
    fd << "</AllowOverpost>" << std::endl; // NOXLATE

    //Property: Rules
    for (int x = 0; x < pointTypeStyle->GetRules()->GetCount(); x++)
    {
        IOPointRule *IO = new IOPointRule();
        IO->Write(fd, static_cast<PointRule*>(pointTypeStyle->GetRules()->GetAt(x)));
        delete IO;
    }

    dectab();
    fd << tab() << "</PointTypeStyle>" << std::endl; // NOXLATE
}
