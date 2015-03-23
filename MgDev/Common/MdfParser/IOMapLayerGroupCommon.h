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

#ifndef _IOMAPLAYERGROUPCOMMON_H
#define _IOMAPLAYERGROUPCOMMON_H

#include "SAX2ElementHandler.h"
#include "MapLayerGroupCommon.h"
#include "MapDefinition.h"
#include "TileSetDefinition.h"
#include "Version.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;

BEGIN_NAMESPACE_MDFPARSER

class IOMapLayerGroupCommon : public SAX2ElementHandler
{
    protected:
        IOMapLayerGroupCommon(Version& version);

    public:
        IOMapLayerGroupCommon(MapDefinition* map, Version& version);
        IOMapLayerGroupCommon(TileSetDefinition* tileset, Version& version);
        virtual ~IOMapLayerGroupCommon();

        virtual void ElementChars(const wchar_t* ch);

        static void Write(MdfStream& fd, MapLayerGroupCommon* layerGroup, Version* version, MgTab& tab);

    protected:
        MapLayerGroupCommon* m_layerGroup;
        MapDefinition* m_map;
        TileSetDefinition* m_tileset;
};

END_NAMESPACE_MDFPARSER
#endif // _IOMAPLAYERGROUPCOMMON_H
