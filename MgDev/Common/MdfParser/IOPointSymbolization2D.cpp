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
#include "IOPointSymbolization2D.h"
#include "IOMarkSymbol.h"
#include "IOImageSymbol.h"
#include "IOFontSymbol.h"
#include "IOW2DSymbol.h"
#include "IOBlockSymbol.h"
#include "ISymbolVisitor.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOPointSymbolization2D::IOPointSymbolization2D()
{
    this->_PointSymbolization2D = NULL;
    this->pointRule = NULL;
    this->ioSymbol = NULL;
}

IOPointSymbolization2D::IOPointSymbolization2D(PointRule * pointRule)
{
    this->_PointSymbolization2D = NULL;
    this->pointRule = pointRule;
    this->ioSymbol = NULL;
}

IOPointSymbolization2D::~IOPointSymbolization2D()
{
}

void IOPointSymbolization2D::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"PointSymbolization2D") // NOXLATE
    {
        m_startElemName = name;
        this->_PointSymbolization2D = new PointSymbolization2D();
    }
    else
    {
        if (m_currElemName == L"Mark") // NOXLATE
            this->ioSymbol = new IOMarkSymbol();
        else if (m_currElemName == L"Image") // NOXLATE
            this->ioSymbol = new IOImageSymbol();
        else if (m_currElemName == L"Font") // NOXLATE
            this->ioSymbol = new IOFontSymbol();
        else if (m_currElemName == L"W2D") // NOXLATE
            this->ioSymbol = new IOW2DSymbol();
        else if (m_currElemName == L"Block") // NOXLATE
            this->ioSymbol = new IOBlockSymbol();
        if (this->ioSymbol)
        {
            handlerStack->push(this->ioSymbol);
            this->ioSymbol->StartElement(name, handlerStack);
        }
    }
}

void IOPointSymbolization2D::ElementChars(const wchar_t *ch)
{
}

void IOPointSymbolization2D::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (this->_PointSymbolization2D != NULL)
        {
            this->pointRule->AdoptSymbolization(this->_PointSymbolization2D);
            if (this->ioSymbol != NULL)
            {
                this->_PointSymbolization2D->AdoptSymbol(this->ioSymbol->GetSymbol());
                // delete this here - a new one is created in each call to StartElement
                delete this->ioSymbol;
            }
            this->_PointSymbolization2D = NULL;
        }
        handlerStack->pop();
        this->pointRule= NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOPointSymbolization2D::Write(MdfStream &fd, PointSymbolization2D *PointSymbolization2D)
{
    fd << tab() << "<PointSymbolization2D>" << std::endl; // NOXLATE
    inctab();

    //Property: Symbol
    MarkSymbol* markSymbol = dynamic_cast<MarkSymbol*>(PointSymbolization2D->GetSymbol());
    ImageSymbol* imageSymbol = dynamic_cast<ImageSymbol*>(PointSymbolization2D->GetSymbol());
    FontSymbol* fontSymbol = dynamic_cast<FontSymbol*>(PointSymbolization2D->GetSymbol());
    W2DSymbol* w2DSymbol = dynamic_cast<W2DSymbol*>(PointSymbolization2D->GetSymbol());
    BlockSymbol* blockSymbol = dynamic_cast<BlockSymbol*>(PointSymbolization2D->GetSymbol());
    if (markSymbol)
        IOMarkSymbol::Write(fd, markSymbol);
    else if (imageSymbol)
        IOImageSymbol::Write(fd, imageSymbol);
    else if (fontSymbol)
        IOFontSymbol::Write(fd, fontSymbol);
    else if (w2DSymbol)
        IOW2DSymbol::Write(fd, w2DSymbol);
    else if (blockSymbol)
        IOBlockSymbol::Write(fd, blockSymbol);

    dectab();
    fd << tab() << "</PointSymbolization2D>" << std::endl; // NOXLATE
}
