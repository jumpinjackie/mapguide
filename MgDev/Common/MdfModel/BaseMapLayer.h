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

#ifndef BASEMAPLAYER_H_
#define BASEMAPLAYER_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "MdfOwnerCollection.h"
#include "LayerDefinition.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // BaseMapLayer defines how a layer should appear in a map.  BaseMapLayer is
    // the class whose instance represents the relationship between a MapDefinition
    // layer and a LayerDefinition object. The BaseMapLayer's Properties override
    // those defined in the LayerDefinition. Furthermore, BaseMapLayers, can reference
    // BaseMapLayerGroups which allow for setting up groups of LayerDefinitions.
    // It is important to note, that multiple BaseMapLayers can hold onto the same
    // reference of a LayerDefinition object. It is the user's responsibility to ensure
    // that LayerDefinitions are deleted.
    // BaseMapLayer is also the base class of MapLayer.  BaseMapLayers are part of
    // BaseMapLayerGroups, whereas MapLayers are part of the main Map.
    //---------------------------------------------------------------------
    class MDFMODEL_API BaseMapLayer: public MdfRootObject
    {
    public:
        // Construction, destruction, initialization
        BaseMapLayer(const MdfString& strName, const MdfString& strLayerUri);
        virtual ~BaseMapLayer();

        // Operations
        // Property : Name
        // The name of BaseMapLayer. Must be a unique BaseMapLayer name.
        const MdfString& GetName() const;
        void SetName(const MdfString& strName);

        //Property : LayerResourceID
        // The Layer ResourceID that this BaseMapLayer refers to for its LayerDefinition object
        const MdfString& GetLayerResourceID() const;
        void SetLayerResourceID(const MdfString& strLayerResourceID);

        // Property : LayerDefinition
        // LayerDefinition that this BaseMapLayer refers to.
        LayerDefinition* GetLayerDefinition();
        void SetLayerDefinition(LayerDefinition* plyrLayerDefinition);

        // Property : Visible
        // Determines if this layer's data is initially visible or not.
        // Default is true.
        bool IsVisible() const;
        void SetVisible(bool bVisibility);

        // Property : Selectable
        // Determines if the features of this layer can be selected.
        // Default is true.
        bool IsSelectable() const;
        void SetSelectable(bool bSelectable);

        // Property : ShowInLegend
        // Whether the legend should be displayed in the legend or not.
        // Default is true.
        bool IsShowInLegend() const;
        void SetShowInLegend(bool bShowInLegend);

        // Property : LegendLabel
        // The optional legend label.
        const MdfString& GetLegendLabel() const;
        void SetLegendLabel(const MdfString& pstrLegendLabel);

        // Property : ExpandInLegend
        // Determines if the layer's styles are shown in the legend.
        // Default is true.
        bool IsExpandInLegend() const;
        void SetExpandInLegend(bool bExpandInLegend);

    private:
        // Hidden copy constructor and assignment operator.
        BaseMapLayer(const BaseMapLayer&);
        BaseMapLayer& operator=(const BaseMapLayer&);

        // Data members
        // See corresponding properties for descriptions
        MdfString m_strName;
        MdfString m_strLayerResourceID;
        MdfString m_strLegendLabel;
        bool m_bVisible;
        bool m_bExpandInLegend;
        LayerDefinition* m_plyrLayer;
        bool m_bSelectable;
        bool m_bShowInLegend;
    };

    typedef MdfOwnerCollection<BaseMapLayer> BaseMapLayerCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<BaseMapLayer>;

END_NAMESPACE_MDFMODEL
#endif //BASEMAPLAYER_H_
