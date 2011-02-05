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

#ifndef BASEMAPLAYERGROUP_H_
#define BASEMAPLAYERGROUP_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"
#include "MapLayerGroupCommon.h"
#include "BaseMapLayer.h"

BEGIN_NAMESPACE_MDFMODEL

    //---------------------------------------------------------------------
    // DESCRIPTION:
    // BaseMapLayerGroup defines a group of MapLayers that can be used in
    // a base map tile set for the HTML viewer.  For the DWF viewer this
    // should be treated as a normal group below all other groups.
    //---------------------------------------------------------------------
    class MDFMODEL_API BaseMapLayerGroup : public MapLayerGroupCommon
    {
    public:
        // Construction, destruction, initialization
        BaseMapLayerGroup(const MdfString& strName);
        virtual ~BaseMapLayerGroup();

        // Operations
        // Property : Layers
        // The layers that are part of this group. The order of the layers
        // represents the draw order, layers first is the list are drawn
        // over top of layers later in the list.
        BaseMapLayerCollection* GetLayers();

    private:
        // Hidden copy constructor and assignment operator
        BaseMapLayerGroup(const BaseMapLayerGroup&);
        BaseMapLayerGroup& operator=(const BaseMapLayerGroup&);

        // Data members
        // See corresponding properties for descriptions
        BaseMapLayerCollection m_layers;
    };

    typedef MdfOwnerCollection<BaseMapLayerGroup> BaseMapLayerGroupCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<BaseMapLayerGroup>;

END_NAMESPACE_MDFMODEL
#endif //BASEMAPLAYERGROUP_H_
