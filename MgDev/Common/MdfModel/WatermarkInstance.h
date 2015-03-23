//
//  Copyright (C) 2010-2011 by Autodesk, Inc.
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

#ifndef WATERMARKINSTANCE_H_
#define WATERMARKINSTANCE_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "MdfOwnerCollection.h"
#include "WatermarkDefinition.h"
#include "WatermarkAppearance.h"
#include "WatermarkPosition.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // WatermarkInstance defines how a watermark should be made part of a layer / map.
    // It is important to note, that multiple WatermarkInstance can hold onto the same
    // reference of a WatermarkDefinition object. It is the user's responsibility to ensure
    // that duplicated WatermarkDefinitions are deleted.
    //------------------------------------------------------------------------
    class MDFMODEL_API WatermarkInstance : public MdfRootObject
    {
    public:

        // Describes the content of the Usage property
        enum Usage
        {
            WMS = 1,
            Viewer = 2,
            All = WMS | Viewer
        };

        // Construction, destruction, initialization.
        WatermarkInstance(const MdfString& strName, const MdfString& strWatermarkResourceID);
        virtual ~WatermarkInstance();

        // Operations

        // Property : Name
        // The name of WatermarkInstance.  Must be a unique WatermarkInstance name.
        const MdfString& GetName() const;
        void SetName(const MdfString& strName);

        // Property : ResourceId
        // The Watermark resource ID that this WatermarkInstance refers to for its WatermarkDefinition object.
        const MdfString& GetResourceId() const;
        void SetResourceId(const MdfString& strResourceId);

        // Property: Usage
        const Usage GetUsage() const;
        void SetUsage(Usage usage);

        // Property: AppearanceOverride
        const WatermarkAppearance* GetAppearanceOverride() const;
        WatermarkAppearance* GetAppearanceOverride();
        void AdoptAppearanceOverride(WatermarkAppearance* pAppearanceOverride);
        WatermarkAppearance* OrphanAppearanceOverride();

        // Property: PositionOverride
        const WatermarkPosition* GetPositionOverride() const;
        WatermarkPosition* GetPositionOverride();
        void AdoptPositionOverride(WatermarkPosition* pPositionOverride);
        WatermarkPosition* OrphanPositionOverride();

        // Property : WatermarkDefinition
        // WatermarkDefinition that this WatermarkInstance refers to.
        const WatermarkDefinition* GetWatermarkDefinition() const;
        WatermarkDefinition* GetWatermarkDefinition();
        void AdoptWatermarkDefinition(WatermarkDefinition* pWatermarkDefinition);
        WatermarkDefinition* OrphanWatermarkDefinition();

        bool Equals(WatermarkInstance* another);

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden WatermarkInstance copy constructor and assignment operator.
        WatermarkInstance(const WatermarkInstance&);
        WatermarkInstance& operator=(const WatermarkInstance&);

        // Data members
        // See corresponding properties for descriptions
        MdfString m_strName;
        MdfString m_strResourceId;
        Usage m_usage;
        WatermarkAppearance* m_appearanceOverride;
        WatermarkPosition* m_positionOverride;
        WatermarkDefinition* m_pWatermarkDefinition;
    };

    typedef MdfOwnerCollection<WatermarkInstance> WatermarkInstanceCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<WatermarkInstance>;

END_NAMESPACE_MDFMODEL
#endif // WATERMARKINSTANCE_H_
