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
#include "IOHillShade.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOHillShade::IOHillShade():colorStyle(NULL),hillShade(NULL)
{
}

IOHillShade::IOHillShade(GridColorStyle * pColorStyle):colorStyle(pColorStyle),hillShade(NULL)
{
}

IOHillShade::~IOHillShade()
{
}

void IOHillShade::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"HillShade") // NOXLATE
    {
        m_startElemName = name;
        this->hillShade = new HillShade();
    }
}

void IOHillShade::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Band") // NOXLATE
        (this->hillShade)->SetBand(ch);
    else if (m_currElemName == L"Azimuth") // NOXLATE
        this->hillShade->SetAzimuth(wstrToDouble(ch));
    else if (m_currElemName == L"Altitude") // NOXLATE
        this->hillShade->SetAltitude(wstrToDouble(ch));
    else if (m_currElemName == L"ScaleFactor") // NOXLATE
        this->hillShade->SetScaleFactor(wstrToDouble(ch));
}

void IOHillShade::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->colorStyle->AdoptHillShade(this->hillShade);
        handlerStack->pop();
        this->colorStyle = NULL;
        this->hillShade = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOHillShade::Write(MdfStream &fd,  HillShade *pHillShade)
{
    fd << tab() << "<HillShade>" << std::endl; // NOXLATE
    inctab();

    //Property: Band
    fd << tab() << "<Band>"; // NOXLATE
    fd << EncodeString(pHillShade->GetBand());
    fd << "</Band>" << std::endl; // NOXLATE

    // Property : Azimuth
    fd << tab() << "<Azimuth>"; // NOXLATE
    fd << pHillShade->GetAzimuth();
    fd << "</Azimuth>" << std::endl; // NOXLATE

    // Property : Altitude
    fd << tab() << "<Altitude>"; // NOXLATE
    fd << pHillShade->GetAltitude();
    fd << "</Altitude>" << std::endl; // NOXLATE

    // Property : ScaleFactor (optional)
    if (pHillShade->GetScaleFactor() != 1.0)
    {
        fd << tab() << "<ScaleFactor>"; // NOXLATE
        fd << pHillShade->GetScaleFactor();
        fd << "</ScaleFactor>" << std::endl; // NOXLATE
    }

    dectab();
    fd << tab() << "</HillShade>" << std::endl; // NOXLATE
}
