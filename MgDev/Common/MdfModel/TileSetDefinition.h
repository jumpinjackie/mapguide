//
//  Copyright (C) 2004-2014 by Autodesk, Inc.
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
#ifndef TILESETDEFINITION_H_
#define TILESETDEFINITION_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "Box2D.h"
#include "DisplayScale.h"
#include "BaseMapLayerGroup.h"
#include "TileStoreParameters.h"

BEGIN_NAMESPACE_MDFMODEL

    class MDFMODEL_API TileSetDefinition : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization.
        TileSetDefinition();
        virtual ~TileSetDefinition();

        // Property: TileStoreParameters
        // Defines the parameters to access the tile cache
        TileStoreParameters* GetTileStoreParameters();

        // Property : Extents
        // The extents to be used by the MapDefinition
        const Box2D& GetExtents() const;
        void SetExtents(const Box2D& boxExtents);

        // Property : BaseMapLayerGroups
        // The base map groups; used to define tiles for the HTML viewer.
        BaseMapLayerGroupCollection* GetBaseMapLayerGroups();

    private:
        // Hidden MapDefinition copy constructor and assignment operator.
        TileSetDefinition(const TileSetDefinition&);
        TileSetDefinition& operator=(const TileSetDefinition&);

        // Data members
        // See corresponding properties for descriptions
        TileStoreParameters m_parameters;
        Box2D m_boxExtents;
        BaseMapLayerGroupCollection m_baseMapLayerGroups;
    };

END_NAMESPACE_MDFMODEL

#endif //TILESETDEFINITION_H_