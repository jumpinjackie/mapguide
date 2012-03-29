//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
#include "IOPointSymbolization2D.h"
#include "IOMarkSymbol.h"
#include "IOImageSymbol.h"
#include "IOFontSymbol.h"
#include "IOW2DSymbol.h"
#include "IOBlockSymbol.h"
#include "ISymbolVisitor.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, PointSymbolization2D);
ELEM_MAP_ENTRY(2, Mark);
ELEM_MAP_ENTRY(3, Image);
ELEM_MAP_ENTRY(4, Font);
ELEM_MAP_ENTRY(5, W2D);
ELEM_MAP_ENTRY(6, Block);
ELEM_MAP_ENTRY(7, ExtendedData1);


IOPointSymbolization2D::IOPointSymbolization2D(Version& version) : SAX2ElementHandler(version)
{
    this->m_pointSymbolization2D = NULL;
    this->m_pointRule = NULL;
    this->m_ioSymbol = NULL;
}


IOPointSymbolization2D::IOPointSymbolization2D(PointRule* pointRule, Version& version) : SAX2ElementHandler(version)
{
    this->m_pointSymbolization2D = NULL;
    this->m_pointRule = pointRule;
    this->m_ioSymbol = NULL;
}


IOPointSymbolization2D::~IOPointSymbolization2D()
{
}


void IOPointSymbolization2D::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    if (ePointSymbolization2D == this->m_currElemId)
    {
        this->m_startElemName = name;
        this->m_pointSymbolization2D = new PointSymbolization2D();
    }
    else if (eExtendedData1 == this->m_currElemId)
    {
        this->m_procExtData = true;
    }
    else if (eUnknown == this->m_currElemId)
    {
        ParseUnknownXml(name, handlerStack);
    }
    else
    {
        switch (this->m_currElemId)
        {
        case eMark:
            this->m_ioSymbol = new IOMarkSymbol(this->m_version);
            break;
        case eImage:
            this->m_ioSymbol = new IOImageSymbol(this->m_version);
            break;
        case eFont:
            this->m_ioSymbol = new IOFontSymbol(this->m_version);
            break;
        case eW2D:
            this->m_ioSymbol = new IOW2DSymbol(this->m_version);
            break;
        case eBlock:
            this->m_ioSymbol = new IOBlockSymbol(this->m_version);
            break;
        }
        if (this->m_ioSymbol)
        {
            handlerStack->push(this->m_ioSymbol);
            this->m_ioSymbol->StartElement(name, handlerStack);
        }
    }
}


void IOPointSymbolization2D::ElementChars(const wchar_t* ch)
{
}


void IOPointSymbolization2D::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        if (this->m_pointSymbolization2D)
        {
            this->m_pointSymbolization2D->SetUnknownXml(this->m_unknownXml);

            this->m_pointRule->AdoptSymbolization(this->m_pointSymbolization2D);
            if (this->m_ioSymbol)
            {
                this->m_pointSymbolization2D->AdoptSymbol(this->m_ioSymbol->GetSymbol());

                // delete this here - a new one is created in each call to StartElement
                delete this->m_ioSymbol;
                this->m_ioSymbol = NULL;
            }
            this->m_pointSymbolization2D = NULL;
        }

        this->m_pointRule= NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOPointSymbolization2D::Write(MdfStream& fd, PointSymbolization2D* pointSymbolization2D, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sPointSymbolization2D) << std::endl;
    tab.inctab();

    // Property: Symbol
    MarkSymbol* markSymbol = dynamic_cast<MarkSymbol*>(pointSymbolization2D->GetSymbol());
    ImageSymbol* imageSymbol = dynamic_cast<ImageSymbol*>(pointSymbolization2D->GetSymbol());
    FontSymbol* fontSymbol = dynamic_cast<FontSymbol*>(pointSymbolization2D->GetSymbol());
    W2DSymbol* w2DSymbol = dynamic_cast<W2DSymbol*>(pointSymbolization2D->GetSymbol());
    BlockSymbol* blockSymbol = dynamic_cast<BlockSymbol*>(pointSymbolization2D->GetSymbol());
    if (markSymbol)
        IOMarkSymbol::Write(fd, markSymbol, version, tab);
    else if (imageSymbol)
        IOImageSymbol::Write(fd, imageSymbol, version, tab);
    else if (fontSymbol)
        IOFontSymbol::Write(fd, fontSymbol, version, tab);
    else if (w2DSymbol)
        IOW2DSymbol::Write(fd, w2DSymbol, version, tab);
    else if (blockSymbol)
        IOBlockSymbol::Write(fd, blockSymbol, version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, pointSymbolization2D->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sPointSymbolization2D) << std::endl;
}
