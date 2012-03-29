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

#ifndef MAPLAYERGROUPCOMMON_H_
#define MAPLAYERGROUPCOMMON_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //---------------------------------------------------------------------
    // DESCRIPTION:
    // MapLayerGroupCommon is the common base class of MapLayerGroup and BaseMapLayerGroup.
    // It defines the set of common properties that are available for layer
    // groups.
    //---------------------------------------------------------------------
    class MDFMODEL_API MapLayerGroupCommon: public MdfRootObject
    {
    public:
        // Construction, destruction, initialization
        MapLayerGroupCommon(const MdfString& strName);
        virtual ~MapLayerGroupCommon();

        // Operations
        // Property : Name
        // The name of MapLayerGroupCommon. Must be a unique MapLayerGroupCommon name.
        const MdfString& GetName() const;
        void SetName(const MdfString& strName);

        // Property : Visible
        // Determines if this group's data is initially visible or not.
        // Default is true.
        bool IsVisible() const;
        void SetVisible(bool bVisible);

        // Property : ShowInLegend
        // Whether the group should be displayed in the legend or not.
        // Default is true.
        bool IsShowInLegend() const;
        void SetShowInLegend(bool bLegend);

        // Property : ExpandInLegend
        // Determines if the Group should initially be expanded when shown in the legend.
        // Default is true.
        bool IsExpandInLegend() const;
        void SetExpandInLegend(bool bExpandInLegend);

        // Property : LegendLabel
        // The legend label for this group, if empty, the name should be used.
        const MdfString& GetLegendLabel() const;
        void SetLegendLabel(const MdfString& strLegendLabel);

    private:
        // Hidden copy constructor and assignment operator
        MapLayerGroupCommon(const MapLayerGroupCommon&);
        MapLayerGroupCommon& operator=(const MapLayerGroupCommon&);

        // Data members
        // See corresponding properties for descriptions
        MdfString m_strName;
        MdfString m_strLegendLabel;
        bool m_bVisible;
        bool m_bShowInLegend;
        bool m_bExpandInLegend;
    };

END_NAMESPACE_MDFMODEL
#endif //MAPLAYERGROUPCOMMON_H_
