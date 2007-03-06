//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#ifndef AREAUSAGE_H_
#define AREAUSAGE_H_

#include "MdfModel.h"
#include "Usage.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API AreaUsage : public Usage
    {
    public:
        enum OriginControl
        {
            Global,
            Local,
            Centroid
        };

        enum ClippingControl
        {
            Clip,
            Inside,
            Overlap
        };

        // Construction, destruction, initialization
        AreaUsage();
        virtual ~AreaUsage();

        OriginControl GetOriginControl() const;
        void SetOriginControl(OriginControl originControl);

        AngleControl GetAngleControl() const;
        void SetAngleControl(AngleControl angleControl);

        ClippingControl GetClippingControl() const;
        void SetClippingControl(ClippingControl clippingControl);

        const MdfString& GetOriginX() const;
        void SetOriginX(const MdfString& originX);

        const MdfString& GetOriginY() const;
        void SetOriginY(const MdfString& originY);

        const MdfString& GetAngle() const;
        void SetAngle(const MdfString& angle);

        const MdfString& GetRepeatX() const;
        void SetRepeatX(const MdfString& repeatX);

        const MdfString& GetRepeatY() const;
        void SetRepeatY(const MdfString& repeatY);

        const MdfString& GetBufferWidth() const;
        void SetBufferWidth(const MdfString& bufferWidth);

        virtual void AcceptVisitor(IUsageVisitor& iuVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        AreaUsage(const AreaUsage&);
        AreaUsage& operator=(const AreaUsage&);

        // Data members
        OriginControl m_eOriginControl;
        AngleControl m_eAngleControl;
        ClippingControl m_eClippingControl;

        MdfString m_sOriginX;
        MdfString m_sOriginY;
        MdfString m_sAngle;
        MdfString m_sRepeatX;
        MdfString m_sRepeatY;
        MdfString m_sBufferWidth;
    };

END_NAMESPACE_MDFMODEL
#endif // AREAUSAGE_H_
