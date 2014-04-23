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

#ifndef WATERMARKDEFINITION_H_
#define WATERMARKDEFINITION_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "SymbolDefinition.h"
#include "WatermarkAppearance.h"
#include "WatermarkPosition.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The WatermarkDefinition class is the root document node of the MdfModel. Through its
    // interface, one can get access to all the data in the WatermarkDefinition.
    //------------------------------------------------------------------------
    class MDFMODEL_API WatermarkDefinition : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization.
        WatermarkDefinition();
        virtual ~WatermarkDefinition();

        // Operations

        // Property: Content
        const SymbolDefinition* GetContent() const;
        SymbolDefinition* GetContent();
        void AdoptContent(SymbolDefinition* pContent);
        SymbolDefinition* OrphanContent();

        // Property: Appearance
        const WatermarkAppearance* GetAppearance() const;
        WatermarkAppearance* GetAppearance();
        void AdoptAppearance(WatermarkAppearance* pAppearance);
        WatermarkAppearance* OrphanAppearance();

        // Property: Position
        const WatermarkPosition* GetPosition() const;
        WatermarkPosition* GetPosition();
        void AdoptPosition(WatermarkPosition* pPosition);
        WatermarkPosition* OrphanPosition();

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden WatermarkDefinition copy constructor and assignment operator.
        WatermarkDefinition(const WatermarkDefinition&);
        WatermarkDefinition& operator=(const WatermarkDefinition&);

        // Data members
        // See corresponding properties for descriptions
        SymbolDefinition* m_content;
        WatermarkAppearance* m_appearance;
        WatermarkPosition* m_position;
    };

END_NAMESPACE_MDFMODEL
#endif // WATERMARKDEFINITION_H_
