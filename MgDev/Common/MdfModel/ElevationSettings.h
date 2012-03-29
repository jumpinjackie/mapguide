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

#ifndef ElevationSettings_H_
#define ElevationSettings_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "LengthConverter.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The ElevationSettings class is used to store elevation properties
    // used for rendering 2.5D objects.
    //-------------------------------------------------------------------------
class MDFMODEL_API ElevationSettings : public MdfRootObject
    {
    public:
        enum ElevationType
        {
            Absolute,
            RelativeToGround
        };

        // Construction, destruction, initialization
        ElevationSettings();
        virtual ~ElevationSettings();

        // Operations
        // Property : ZOffset
        const MdfString& GetZOffsetExpression() const;
        void SetZOffsetExpression(const MdfString& strZOffsetExpression);

        // Property : ZOffsetExpression
        const MdfString& GetZExtrusionExpression() const;
        void SetZExtrusionExpression(const MdfString& strZExtrusionExpression);

        // Property : ElevationType
        ElevationType GetElevationType() const;
        void SetElevationType(ElevationType elevationType);

        // Property : Unit
        LengthUnit GetUnit() const;
        void SetUnit(LengthUnit eUnit);

    private:
        // Hidden copy constructor and assignment operator.
        ElevationSettings(const ElevationSettings&);
        ElevationSettings& operator=(const ElevationSettings&);

        // Data members
        MdfString m_strZOffsetExpression;
        MdfString m_strZExtrusionExpression;
        LengthUnit m_eUnit;
        ElevationType m_elevationType;
    };

END_NAMESPACE_MDFMODEL
#endif //ElevationSettings_H_
