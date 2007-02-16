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

#ifndef _IOGRIDCOLOR_H
#define _IOGRIDCOLOR_H

#include "SAX2ElementHandler.h"
#include "GridColor.h"
#include "GridColorRule.h"
#include "ChannelBand.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;

BEGIN_NAMESPACE_MDFPARSER

class IOGridColor : public SAX2ElementHandler
{
    protected:
        GridColorRule *     colorRule;

    public:
        IOGridColor();
        IOGridColor(GridColorRule * colorRule);
        ~IOGridColor();
        void Write(MdfStream &fd,  GridColor * color);

        virtual void StartElement(const wchar_t *name, HandlerStack *handlerStack);
        virtual void ElementChars(const wchar_t *ch);
        virtual void EndElement(const wchar_t *name, HandlerStack *handlerStack);
};

class IOGridColorBands : public IOGridColor
{
    private:
        GridColorBands*     color;
        ChannelBand*        redChannel;
        ChannelBand*        greenChannel;
        ChannelBand*        blueChannel;

    public:
        IOGridColorBands();
        IOGridColorBands(GridColorRule * colorRule);
        ~IOGridColorBands();

        void Write(MdfStream &fd,  GridColorBands * color);

        virtual void StartElement(const wchar_t *name, HandlerStack *handlerStack);
        virtual void ElementChars(const wchar_t *ch);
        virtual void EndElement(const wchar_t *name, HandlerStack *handlerStack);
};

END_NAMESPACE_MDFPARSER
#endif // _IOGRIDCOLOR_H
