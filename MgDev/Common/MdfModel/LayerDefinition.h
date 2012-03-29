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

#ifndef LAYERDEFINITION_H_
#define LAYERDEFINITION_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "WatermarkInstance.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // LayerDefinition is a root element in the Spatial Map Definition Format.
    // It is a free standing definition for presenting a wholly
    // encapsulated source of data. The LayerDefinition class is an abstract class
    // that is used as a base class for a set of data specific Layer classes.
    // LayerDefinitions are encapsulated by MapLayer objects that make up a Map. They
    // are referenced primarily by their Uri.
    //-------------------------------------------------------------------------
class MDFMODEL_API LayerDefinition : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization
        LayerDefinition(const MdfString& strResourceID);
        virtual ~LayerDefinition();

        // Operations
        // Property : ResourceID
        const MdfString& GetResourceID() const;
        void SetResourceID(const MdfString& strResourceID);

        // Property : Opacity
        const double GetOpacity() const;
        void SetOpacity(const double opacity);

        // Property : Watermarks
        // The List of watermark instance in this layer.
        WatermarkInstanceCollection* GetWatermarks();

    private:
        // Data members
        // The ID of the DataSource used by this layer
        MdfString m_strResourceID;

        // Bitmap to be used in the legend.
        MdfString m_strLegend;

        // The opacity value
        double m_opacity;

        WatermarkInstanceCollection m_listWatermarks;
    };

END_NAMESPACE_MDFMODEL
#endif //LAYERDEFINITION_H_
