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

#ifndef MAPLAYERGROUP_H_
#define MAPLAYERGROUP_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"
#include "MapLayerGroupCommon.h"

BEGIN_NAMESPACE_MDFMODEL

    //---------------------------------------------------------------------
    // DESCRIPTION:
    // MapLayerGroup defines a Group node that MapLayers can reference to by
    // name in their Group Property. MapLayerGroups themselves can reference
    // other MapLayerGroups by Name as their group. All MapLayerGroups should
    // reside in the Map object that holds onto the MapLayers.
    // The properties defined by MapLayerGroups override those defined
    // on the MapLayers and MapLayerGroups that belong to it.
    //---------------------------------------------------------------------
    class MDFMODEL_API MapLayerGroup : public MapLayerGroupCommon
    {
    public:
        // Construction, destruction, initialization
        MapLayerGroup(const MdfString& strName);
        virtual ~MapLayerGroup();

        // Operations
        // Property : Group
        // The optional name of the Group this MapLayerGroup belongs to.
        const MdfString& GetGroup() const;
        void SetGroup(const MdfString& pstrGroupName);

    private:
        // Hidden copy constructor and assignment operator
        MapLayerGroup(const MapLayerGroup&);
        MapLayerGroup& operator=(const MapLayerGroup&);

        // Data members
        // See corresponding properties for descriptions
        MdfString m_strGroup;
    };

    typedef MdfOwnerCollection<MapLayerGroup> MapLayerGroupCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<MapLayerGroup>;

END_NAMESPACE_MDFMODEL
#endif //MAPLAYERGROUP_H_
