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

#ifndef WATERMARKAPPEARANCE_H_
#define WATERMARKAPPEARANCE_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The WatermarkAppearance class is to define the appearance of watermark.
    //------------------------------------------------------------------------
    class MDFMODEL_API WatermarkAppearance : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization.
        WatermarkAppearance();
        virtual ~WatermarkAppearance();

        // Operations
        // Property: Transparency
        double GetTransparency() const;
        void SetTransparency(const double& dTransparency);

        // Property: Rotation
        double GetRotation() const;
        void SetRotation(const double& dRotation);

        virtual bool Equals(WatermarkAppearance* another);

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden WatermarkAppearance copy constructor and assignment operator.
        WatermarkAppearance(const WatermarkAppearance&);
        WatermarkAppearance& operator=(const WatermarkAppearance&);

        // Data members
        // See corresponding properties for descriptions
        double m_transparency;
        double m_rotation;

        static const double doubleTolerance;
    };

END_NAMESPACE_MDFMODEL
#endif // WATERMARKAPPEARANCE_H_
