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

#ifndef _IOEXTRA_H
#define _IOEXTRA_H

#include "SAX2ElementHandler.h"
#include "Box2D.h"
#include "MapDefinition.h"
#include "TileSetDefinition.h"
#include "Version.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;

BEGIN_NAMESPACE_MDFPARSER

class IOExtra : public SAX2ElementHandler
{
    public:
        IOExtra(Version& version);
        IOExtra(MapDefinition* map, Version& version);
        IOExtra(TileSetDefinition* map, Version& version);
        virtual ~IOExtra();

        virtual void StartElement(const wchar_t* name, HandlerStack* handlerStack);
        virtual void ElementChars(const wchar_t* ch);
        virtual void EndElement(const wchar_t* name, HandlerStack* handlerStack);

        static void WriteBox2D(MdfStream& fd, const Box2D& box2D, bool autoCorrect, Version* version, MgTab& tab);

    private:
        double m_minX;
        double m_maxX;
        double m_minY;
        double m_maxY;
        MapDefinition* m_map;
        TileSetDefinition* m_tileset;
};

END_NAMESPACE_MDFPARSER
#endif // _IOEXTRA_H
