//
//  Copyright (C) 2011 by Autodesk, Inc.
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

#ifndef SCALERANGE_H_
#define SCALERANGE_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // ScaleRange describes the min and max scales for a layer's scale range.
    //-------------------------------------------------------------------------
    class MDFMODEL_API ScaleRange : public MdfRootObject
    {
    public:
        // Represents the maximum value for the map scale.
        static const double MAX_MAP_SCALE;

    public:
        // Construction, destruction, initialization
        ScaleRange();
        virtual ~ScaleRange();

        // Operations
        // Property : MinScale
        double GetMinScale() const;
        void SetMinScale(const double& dMinScale);

        // Property : MaxScale
        double GetMaxScale() const;
        void SetMaxScale(const double& dMaxScale);

    private:

        // Data members
        // The minimum value component of the Scale Range.
        double m_dMinScale;

        // The maximum value component of the Scale Range.
        double m_dMaxScale;
    };

END_NAMESPACE_MDFMODEL
#endif //SCALERANGE_H_
