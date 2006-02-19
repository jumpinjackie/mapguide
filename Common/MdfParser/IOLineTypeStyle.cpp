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
#include "IOLineTypeStyle.h"
#include "IOLineRule.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOLineTypeStyle::IOLineTypeStyle()
{
}

IOLineTypeStyle::IOLineTypeStyle(VectorScaleRange * scaleRange)
{
    this->scaleRange = scaleRange;
}

IOLineTypeStyle::~IOLineTypeStyle()
{
}

void IOLineTypeStyle::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"LineTypeStyle") // NOXLATE
    {
        m_startElemName = name;
        this->_lineTypeStyle = new LineTypeStyle(); // NOXLATE
    }
    else if (m_currElemName == L"LineRule") // NOXLATE
    {
        IOLineRule *IO = new IOLineRule(this->_lineTypeStyle);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOLineTypeStyle::ElementChars(const wchar_t *ch)
{

}

void IOLineTypeStyle::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->scaleRange->GetFeatureTypeStyles()->Adopt(this->_lineTypeStyle);
        handlerStack->pop();
        this->scaleRange = NULL;
        this->_lineTypeStyle = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOLineTypeStyle::Write(MdfStream &fd, LineTypeStyle *lineTypeStyle)
{
    fd << tab() << "<LineTypeStyle>" << std::endl; // NOXLATE
    inctab();

    //Property: Rules
    for (int x = 0; x < lineTypeStyle->GetRules()->GetCount(); x++)
    {
        IOLineRule *IO = new IOLineRule();
        IO->Write(fd, static_cast<LineRule*>(lineTypeStyle->GetRules()->GetAt(x)));
        delete IO;
    }

    dectab();
    fd << tab() << "</LineTypeStyle>" << std::endl; // NOXLATE
}
