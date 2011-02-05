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

#ifndef MAPLAYER_H_
#define MAPLAYER_H_

#include "MdfModel.h"
#include "BaseMapLayer.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // MapLayer defines how a layer should be made part of a map.
    // The MapLayer extends the BaseMapLayer class by adding a group property so
    // that it can be added to a MapLayerGroup.
    // It is important to note, that multiple MapLayers can hold onto the same
    // reference of a LayerDefinition object. It is the user's responsibility to ensure
    // that LayerDefinitions are deleted.
    //---------------------------------------------------------------------
    class MDFMODEL_API MapLayer : public BaseMapLayer
    {
    public:
        // Construction, destruction, initialization
        MapLayer(const MdfString& strName, const MdfString& strLayerUri);
        virtual ~MapLayer();

        // Property : Group
        // The optional name of the Group this MapLayer belongs to.
        const MdfString& GetGroup() const;
        void SetGroup(const MdfString& strGroup);

    private:
        // Hidden copy constructor and assignment operator.
        MapLayer(const MapLayer&);
        MapLayer& operator=(const MapLayer&);

        // Data members
        MdfString m_strGroup;
    };

    typedef MdfOwnerCollection<MapLayer> MapLayerCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<MapLayer>;

END_NAMESPACE_MDFMODEL
#endif //MAPLAYER_H_
