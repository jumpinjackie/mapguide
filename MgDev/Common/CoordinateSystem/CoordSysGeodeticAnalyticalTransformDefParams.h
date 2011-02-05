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

#ifndef _CCOORDSYSGEODETICANALYTICALTRANSFORMDEFPARAMS_H_
#define _CCOORDSYSGEODETICANALYTICALTRANSFORMDEFPARAMS_H_

namespace CSLibrary
{
    typedef cs_GeodeticTransform_::csGeodeticXformParameters::csGeodeticXformParmsGeocentric csGeocentricXformParams;

    class CCoordinateSystemGeodeticAnalyticalTransformDefParams :
        public MgCoordinateSystemGeodeticAnalyticalTransformDefParams,
        public CCoordinateSystemGeodeticTransformDefParams
    {
    public:
        CCoordinateSystemGeodeticAnalyticalTransformDefParams(const csGeocentricXformParams& params, INT32 analyticalTransformMethod, bool isProtected);
        ~CCoordinateSystemGeodeticAnalyticalTransformDefParams();

        virtual void Dispose();
        virtual bool IsValid();

        virtual bool IsProtected();

        virtual void CopyTo(void* target) const;

        //MgCoordinateSystemGeodeticAnalyticalTransformDefParams implementation
        virtual double GetDeltaX();
        virtual void SetDeltaX(double deltaX);

        virtual double GetDeltaY();
        virtual void SetDeltaY(double deltaY);

        virtual double GetDeltaZ();
        virtual void SetDeltaZ(double deltaZ);

        virtual double GetRotateX();
        virtual void SetRotateX(double rotateX);

        virtual double GetRotateY();
        virtual void SetRotateY(double rotateY);

        virtual double GetRotateZ();
        virtual void SetRotateZ(double rotateZ);

        virtual double GetScale();
        virtual void SetScale(double scale);

        virtual double GetTranslateX();
        virtual void SetTranslateX(double translateX);

        virtual double GetTranslateY();
        virtual void SetTranslateY(double translateY);

        virtual double GetTranslateZ();
        virtual void SetTranslateZ(double translateZ);

        virtual INT32 GetTransformationMethod();
        virtual void SetTransformationMethod(INT32 analyticalMethodCode);

    private:
        void ReleaseInstance();

    private:
        csGeocentricXformParams* geocentricTransformParams;
        INT32 transformationMethod;
    };

} //namespace CSLibrary

#endif //_CCOORDSYSGEODETICANALYTICALTRANSFORMDEFPARAMS_H_
