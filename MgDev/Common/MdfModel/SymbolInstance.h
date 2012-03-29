//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef SYMBOLINSTANCE_H_
#define SYMBOLINSTANCE_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "MdfOwnerCollection.h"
#include "SymbolDefinition.h"
#include "SizeContexts.h"
#include "Override.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API SymbolInstance : public MdfRootObject
    {
    public:
        // Used with string properties
        static const wchar_t* sPositioningAlgorithmDefault;
        static const wchar_t* sPositioningAlgorithmValues;

    public:
        // Describes the usage context for the symbol instance
        enum UsageContext
        {
            ucUnspecified,  // default usage (first one found, in order of point, line, area)
            ucPoint,        // point usage
            ucLine,         // line usage
            ucArea          // area usage
        };

        // Describes the geometry context for the symbol instance
        enum GeometryContext
        {
            gcUnspecified,
            gcPoint,
            gcLineString,
            gcPolygon
        };

    public:
        // Construction, destruction, initialization
        SymbolInstance();
        virtual ~SymbolInstance();

        const MdfString& GetResourceId() const;
        void SetResourceId(const MdfString& symbolReference);

        SymbolDefinition* GetSymbolDefinition();
        void AdoptSymbolDefinition(SymbolDefinition* symbolDefinition);
        SymbolDefinition* OrphanSymbolDefinition();

        OverrideCollection* GetParameterOverrides();

        const MdfString& GetScaleX() const;
        void SetScaleX(const MdfString& scaleX);

        const MdfString& GetScaleY() const;
        void SetScaleY(const MdfString& scaleY);

        const MdfString& GetInsertionOffsetX() const;
        void SetInsertionOffsetX(const MdfString& insertionOffsetX);

        const MdfString& GetInsertionOffsetY() const;
        void SetInsertionOffsetY(const MdfString& insertionOffsetY);

        SizeContext GetSizeContext() const;
        void SetSizeContext(SizeContext sizeContext);

        const MdfString& GetDrawLast() const;
        void SetDrawLast(const MdfString& drawLast);

        const MdfString& GetCheckExclusionRegion() const;
        void SetCheckExclusionRegion(const MdfString& checkExclusionRegion);

        const MdfString& GetAddToExclusionRegion() const;
        void SetAddToExclusionRegion(const MdfString& addToExclusionRegion);

        const MdfString& GetPositioningAlgorithm() const;
        void SetPositioningAlgorithm(const MdfString& positioningAlgorithm);

        const MdfString& GetRenderingPass() const;
        void SetRenderingPass(const MdfString& renderingPass);

        UsageContext GetUsageContext() const;
        void SetUsageContext(UsageContext usageContext);

        GeometryContext GetGeometryContext() const;
        void SetGeometryContext(GeometryContext geometryContext);

    private:
        // Hidden copy constructor and assignment operator.
        SymbolInstance(const SymbolInstance&);
        SymbolInstance& operator=(const SymbolInstance&);

        // Data members
        MdfString m_sResourceId;
        SymbolDefinition* m_pSymbolDefinition;
        OverrideCollection m_collOverrides;
        MdfString m_sScaleX;
        MdfString m_sScaleY;
        MdfString m_sInsertionOffsetX;
        MdfString m_sInsertionOffsetY;
        SizeContext m_eSizeContext;
        MdfString m_sDrawLast;
        MdfString m_sCheckExclusionRegion;
        MdfString m_sAddToExclusionRegion;
        MdfString m_sPositioningAlgorithm;
        MdfString m_sRenderingPass;
        UsageContext m_eUsageContext;
        GeometryContext m_eGeometryContext;
    };

    typedef MdfOwnerCollection<SymbolInstance> SymbolInstanceCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<SymbolInstance>;

END_NAMESPACE_MDFMODEL
#endif // SYMBOLINSTANCE_H_
