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

#ifndef HILLSHADE_H_
#define HILLSHADE_H_

#include <vector>
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // Class HillShade is used to specify how to shade given a band and a light
    // source.
    //-------------------------------------------------------------------------
    class MDFMODEL_API HillShade : public MdfRootObject
    {
    public:
        HillShade();
        virtual ~HillShade();

        // Property : Band
        const MdfString& GetBand() const;
        void SetBand(const MdfString& strBandName);

        // Property : Azimuth, in degrees.
        double GetAzimuth() const;
        void SetAzimuth(double dAzimuth);

        // Property : Altitude, in degrees.
        double GetAltitude() const;
        void SetAltitude(double dAltitude);

        // Property : Scale Factor, Optional. Default value is 1.
        // Apply to band before computing hillshade.
        double GetScaleFactor() const;
        void SetScaleFactor(double dScaleFactor);

    private:
        // Hidden copy constructor and assignment.
        HillShade(const HillShade &);
        HillShade& operator = (const HillShade &);

        // Band
        MdfString m_strBandName;

        // Azimuth
        double    m_dAzimuth;

        // Altitude
        double    m_dAltitude;

        // Scale Factor
        double    m_dScaleFactor;
    };

END_NAMESPACE_MDFMODEL
#endif // HILLSHADE_H_
