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

#ifndef _IORELATEPROPERTY_H
#define _IORELATEPROPERTY_H

#include "SAX2ElementHandler.h"
#include "RelateProperty.h"
#include "AttributeRelate.h"
#include "Version.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;

BEGIN_NAMESPACE_MDFPARSER

class IORelateProperty : public SAX2ElementHandler
{
    public:
        IORelateProperty(Version& version);
        IORelateProperty(AttributeRelate* attributeRelate, Version& version);
        virtual ~IORelateProperty();

        virtual void StartElement(const wchar_t* name, HandlerStack* handlerStack);
        virtual void ElementChars(const wchar_t* ch);
        virtual void EndElement(const wchar_t* name, HandlerStack* handlerStack);

        static void Write(MdfStream& fd, RelateProperty* relateProperty, Version* version, MgTab& tab);

    private:
        RelateProperty* m_relateProperty;
        AttributeRelate* m_attributeRelate;
};

END_NAMESPACE_MDFPARSER
#endif // _IORELATEPROPERTY_H
