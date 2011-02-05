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

#ifndef DRAWINGLAYERDEFINITION_H_
#define DRAWINGLAYERDEFINITION_H_

#include "LayerDefinition.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // Layer that specifies a wholly encapsulated DWF data source of pre-styled
    // geometric features and their associated data attributes.
    //-------------------------------------------------------------------------
    class MDFMODEL_API DrawingLayerDefinition :public LayerDefinition
    {
    public:
        // Construction, destruction, initialization
        DrawingLayerDefinition(const MdfString& strDataResourceID, const MdfString &strSheet);
        virtual ~DrawingLayerDefinition();

        // Property : Sheet
        const MdfString& GetSheet() const;
        void SetSheet(const MdfString& strSheet);

        // Property : LayerFilter
        const MdfString& GetLayerFilter() const;
        void SetLayerFilter(const MdfString& strLayerFilter);

        // Property : MinScale
        double GetMinScale() const;
        void SetMinScale(const double& dMinScale);

        // Property : MaxScale
        double GetMaxScale() const;
        void SetMaxScale(const double& dMaxScale);

    private:
        // Hidden copy constructor and assignment operator.
        DrawingLayerDefinition(const DrawingLayerDefinition&);
        DrawingLayerDefinition& operator=(const DrawingLayerDefinition&);

        // Data members
        // The sheet of the dwf file used by this layer
        MdfString m_strSheet;

        // The Layers to be extracted from the sheet.
        MdfString m_strLayerFilter;

        // The minimum value component of the Scale Range.
        double m_dMinScale;

        // The maximum value component of the Scale Range.
        double m_dMaxScale;
    };

END_NAMESPACE_MDFMODEL
#endif //DRAWINGLAYERDEFINITION_H_
