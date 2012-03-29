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
#include "IOImage.h"
#include "IOResourceRef.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOImage::IOImage(Image* image, Version& version) : IOGraphicElement(image, version)
{
}


void IOImage::StartImageElement(const wchar_t* name, HandlerStack* handlerStack)
{
    // the element is an image with the supplied name
    this->m_currElemName = name;
    this->m_startElemName = name;
}


void IOImage::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"ExtendedData1") // NOXLATE
    {
        this->m_procExtData = true;
    }
    else
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOImage::ElementChars(const wchar_t* ch)
{
    Image* image = static_cast<Image*>(this->m_element);

         IF_STRING_PROPERTY(this->m_currElemName, image, Content, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, image, ResourceId, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, image, LibraryItemName, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, image, SizeX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, image, SizeY, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, image, SizeScalable, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, image, Angle, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, image, PositionX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, image, PositionY, ch)
    else IOGraphicElement::ElementChars(ch);
}


void IOImage::Write(MdfStream& fd, Image* image, Version* version, MgTab& tab)
{
    // We must emit either the content or a reference, but
    // not both.  It's invalid for all strings to be empty,
    // but to ensure the XML is valid we still write an empty
    // reference.
    fd << tab.tab() << "<Image>" << std::endl; // NOXLATE
    tab.inctab();

    IOGraphicElement::Write(fd, image, version, tab);

    if (image->GetContent().size() > 0)
    {
        EMIT_STRING_PROPERTY(fd, image, Content, false, NULL, tab)
    }
    else
    {
        _ASSERT(image->GetLibraryItemName().size() > 0);
        IOResourceRef::Write(fd, "Reference", image->GetResourceId(), image->GetLibraryItemName(), true, version, tab); // NOXLATE
    }

    EMIT_DOUBLE_PROPERTY(fd, image, SizeX, false, 1.0, tab)
    EMIT_DOUBLE_PROPERTY(fd, image, SizeY, false, 1.0, tab)
    EMIT_BOOL_PROPERTY(fd, image, SizeScalable, true, true, tab) // default is true
    EMIT_DOUBLE_PROPERTY(fd, image, Angle, true, 0.0, tab)       // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, image, PositionX, true, 0.0, tab)   // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, image, PositionY, true, 0.0, tab)   // default is 0.0

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, image->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</Image>" << std::endl; // NOXLATE
}
