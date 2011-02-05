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

#ifndef MAPDEFINITION_H_
#define MAPDEFINITION_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "Box2D.h"
#include "MapLayer.h"
#include "MapLayerGroup.h"
#include "BaseMapLayerGroup.h"
#include "WatermarkInstance.h"
#include "DisplayScale.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The MapDefinition class is the root document node of the MdfModel. Through its
    // interface, one can get access to all the data in the MapDefinition.
    //------------------------------------------------------------------------
    class MDFMODEL_API MapDefinition : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization.
        MapDefinition(const MdfString& strName, const MdfString& strCoordinateSystem);
        virtual ~MapDefinition();

        // Operations
        // Property : Name
        // The name of the Map
        const MdfString& GetName() const;
        void SetName(const MdfString& strName);

        // Property : CoordinateSystem
        // The WKT string that represents the coordinate system for the MapDefinition.
        const MdfString& GetCoordinateSystem() const;
        void SetCoordinateSystem(const MdfString& strCoordinateSystem);

        // Property : Extents
        // The extents to be used by the MapDefinition
        const Box2D& GetExtents() const;
        void SetExtents(const Box2D& boxExtents);

        // Property : Background
        // The Color for the background of the Map.
        const MdfString& GetBackgroundColor() const;
        void SetBackgroundColor(const MdfString& strColor);

        // Property : Metadata
        // The xml data that represents the metadata information.
        const MdfString& GetMetadata() const;
        void SetMetadata(const MdfString& strMetadata);

        // Property : MapLayers
        // The List of MapLayers.
        MapLayerCollection* GetLayers();

        // Property : MapLayerGroups
        // The collection of LayerGroups.
        MapLayerGroupCollection* GetLayerGroups();

        // Property : MapWatermarks
        // The List of watermark instances.
        WatermarkInstanceCollection* GetWatermarks();

        // Property : FiniteDisplayScales
        // The scales that the base map tiles can be displayed at
        DisplayScaleCollection* GetFiniteDisplayScales();

        // Property : BaseMapLayerGroups
        // The base map groups; used to define tiles for the HTML viewer.
        BaseMapLayerGroupCollection* GetBaseMapLayerGroups();

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden MapDefinition copy constructor and assignment operator.
         MapDefinition(const MapDefinition&);
         MapDefinition& operator=(const MapDefinition&);

        // Data members
        // See corresponding properties for descriptions
        MdfString m_strBkGrnd;
        MapLayerCollection m_listLayers;
        MapLayerGroupCollection m_collLayerGroups;
        WatermarkInstanceCollection m_listWatermarks;
        MdfString m_strName;
        MdfString m_strMetadata;
        MdfString m_strCoordSys;
        Box2D m_boxExtents;
        DisplayScaleCollection m_finiteDisplayScales;
        BaseMapLayerGroupCollection m_baseMapLayerGroups;
    };

END_NAMESPACE_MDFMODEL
#endif // MAPDEFINITION_H_
