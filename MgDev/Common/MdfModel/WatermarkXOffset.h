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

#ifndef WATERMARKXOFFSET_H_
#define WATERMARKXOFFSET_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "WatermarkOffsetUnit.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The WatermarkXOffest class is to define the offset in X-axis of the watermark.
    //------------------------------------------------------------------------
    class MDFMODEL_API WatermarkXOffset : public MdfRootObject
    {
    public:

        // Describes the content of the Alignment property
        enum HorizontalAlignment
        {
            Left,
            Center,
            Right
        };

        // Construction, destruction, initialization.
        WatermarkXOffset();
        virtual ~WatermarkXOffset();

        // Operations
        // Property: Offset
        double GetOffset() const;
        void SetOffset(const double& dOffset);

        // Property : Unit
        WatermarkOffset::WatermarkOffsetUnit GetUnit() const;
        void SetUnit(WatermarkOffset::WatermarkOffsetUnit unit);

        // Property : Alignment
        HorizontalAlignment GetAlignment() const;
        void SetAlignment(HorizontalAlignment alignment);

        virtual bool Equals(WatermarkXOffset* another);

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden WatermarkXOffset copy constructor and assignment operator.
        WatermarkXOffset(const WatermarkXOffset&);
        WatermarkXOffset& operator=(const WatermarkXOffset&);

        // Data members
        // See corresponding properties for descriptions
        double m_offset;

        // The meaning of the string in the Unit property.
        WatermarkOffset::WatermarkOffsetUnit m_unit;

        // The meaning of the string in the Alignment property.
        HorizontalAlignment m_alignment;

        static const double doubleTolerance;
    };

END_NAMESPACE_MDFMODEL
#endif // WATERMARKXOFFSET_H_
