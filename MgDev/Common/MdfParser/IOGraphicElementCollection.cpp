//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
#include "IOGraphicElementCollection.h"
#include "IOPath.h"
#include "IOImage.h"
#include "IOText.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOGraphicElementCollection::IOGraphicElementCollection(GraphicElementCollection* elementCollection, Version& version) : SAX2ElementHandler(version)
{
    this->m_elementCollection = elementCollection;
}


IOGraphicElementCollection::~IOGraphicElementCollection()
{
}


void IOGraphicElementCollection::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"Graphics") // NOXLATE
    {
        this->m_startElemName = name;
    }
    else if (this->m_currElemName == L"Path") // NOXLATE
    {
        Path* path = new Path();
        this->m_elementCollection->Adopt(path);
        IOPath* IO = new IOPath(path, this->m_version);
        handlerStack->push(IO);
        IO->StartPathElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"Image") // NOXLATE
    {
        Image* image = new Image();
        this->m_elementCollection->Adopt(image);
        IOImage* IO = new IOImage(image, this->m_version);
        handlerStack->push(IO);
        IO->StartImageElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"Text") // NOXLATE
    {
        Text* text = new Text();
        this->m_elementCollection->Adopt(text);
        IOText* IO = new IOText(text, this->m_version);
        handlerStack->push(IO);
        IO->StartTextElement(name, handlerStack);
    }
    else
        ParseUnknownXml(name, handlerStack);
}


void IOGraphicElementCollection::ElementChars(const wchar_t* ch)
{
}


void IOGraphicElementCollection::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_elementCollection = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOGraphicElementCollection::Write(MdfStream& fd, GraphicElementCollection* elementCollection, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<Graphics>" << std::endl; // NOXLATE
    tab.inctab();

    int numElements = elementCollection->GetCount();
    for (int i=0; i<numElements; ++i)
    {
        GraphicElement* elem = elementCollection->GetAt(i);

        Path* path = dynamic_cast<Path*>(elem);
        if (path)
        {
            IOPath::Write(fd, path, version, tab);
            continue;
        }

        Image* image = dynamic_cast<Image*>(elem);
        if (image)
        {
            IOImage::Write(fd, image, version, tab);
            continue;
        }

        Text* text = dynamic_cast<Text*>(elem);
        if (text)
        {
            IOText::Write(fd, text, version, tab);
            continue;
        }
    }

    tab.dectab();
    fd << tab.tab() << "</Graphics>" << std::endl; // NOXLATE
}
