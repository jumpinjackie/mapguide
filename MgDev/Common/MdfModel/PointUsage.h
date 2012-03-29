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

#ifndef POINTUSAGE_H_
#define POINTUSAGE_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API PointUsage : public MdfRootObject
    {
    public:
        // Used with string properties
        static const wchar_t* sAngleControlDefault;
        static const wchar_t* sAngleControlValues;

    public:
        // Construction, destruction, initialization
        PointUsage();
        virtual ~PointUsage();

        const MdfString& GetAngleControl() const;
        void SetAngleControl(const MdfString& angleControl);

        const MdfString& GetAngle() const;
        void SetAngle(const MdfString& angle);

        const MdfString& GetOriginOffsetX() const;
        void SetOriginOffsetX(const MdfString& originOffsetX);

        const MdfString& GetOriginOffsetY() const;
        void SetOriginOffsetY(const MdfString& originOffsetY);

     private:
        // Hidden copy constructor and assignment operator.
        PointUsage(const PointUsage&);
        PointUsage& operator=(const PointUsage&);

        // Data members
        MdfString m_sAngleControl;
        MdfString m_sAngle;
        MdfString m_sOriginOffsetX;
        MdfString m_sOriginOffsetY;
    };

END_NAMESPACE_MDFMODEL
#endif // POINTUSAGE_H_
