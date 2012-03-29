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

#ifndef _CCOORDINATESYSTEMGEODETICMULTIPLEREGRESSIONTRANSFORMDEFPARAMS_H_
#define _CCOORDINATESYSTEMGEODETICMULTIPLEREGRESSIONTRANSFORMDEFPARAMS_H_


namespace CSLibrary
{
    typedef cs_GeodeticTransform_::csGeodeticXformParameters::csGeodeticXformParmsDmaMulReg_ csMultipleRegressionXformParams;

    class CCoordinateSystemGeodeticMultipleRegressionTransformDefParams :
        public MgCoordinateSystemGeodeticMultipleRegressionTransformDefParams,
        public CCoordinateSystemGeodeticTransformDefParams
    {
    public:

        CCoordinateSystemGeodeticMultipleRegressionTransformDefParams(const csMultipleRegressionXformParams& mulRegParams,
            INT32 mulRegTransformMethod, bool isProtected);
        ~CCoordinateSystemGeodeticMultipleRegressionTransformDefParams();

        virtual void Dispose();
        virtual bool IsValid();
        virtual bool IsProtected();

        //virtual UINT32 GetPhiBitmap(INT32 index /* range [0..3]*/); //long compiles as 32 bit in MS VC;
        //virtual void SetPhiBitmap(INT32 index, UINT32 value); //long compiles as 32 bit in MS VC;

        //virtual UINT32 GetLambdaBitmap(INT32 index /* range [0..3]*/); //long compiles as 32 bit in MS VC;
        //virtual void SetLambdaBitmap(INT32 index, UINT32 value); //long compiles as 32 bit in MS VC;
        //
        //virtual UINT32 GetHeightBitmap(INT32 index /* range [0..3]*/); //long compiles as 32 bit in MS VC;
        //virtual void SetHeightBitmap(INT32 index, UINT32 value); //long compiles as 32 bit in MS VC;

        virtual double GetValidation();
        virtual void SetValidation(double validation);

        virtual double GetTestPhi();
        virtual void SetTestPhi(double testPhi);

        virtual double GetTestLambda();
        virtual void SetTestLambda(double testLambda);

        virtual double GetDeltaPhi();
        virtual void SetDeltaPhi(double deltaPhi);

        virtual double GetDeltaLambda();
        virtual void SetDeltaLambda(double deltaLambda);

        virtual double GetDeltaHeight();
        virtual void SetDeltaHeight(double deltaHeight);

        virtual double GetPhiOffset();
        virtual void SetPhiOffset(double phiOffset);

        virtual double GetLambdaOffset();
        virtual void SetLambdaOffset(double lambdaOffset);

        virtual double GetNormalizationScale();
        virtual void SetNormalizationScale(double NormalizationScale);

        virtual double GetCoefficientPhi(INT32 index /* range [0..104]*/);
        virtual void SetCoefficientPhi(INT32 index, double value);

        virtual double GetCoefficientLambda(INT32 index /* range [0..104]*/);
        virtual void SetCoefficientLambda(INT32 index, double value);

        virtual double GetCoefficientHeight(INT32 index /* range [0..104]*/);
        virtual void SetCoefficientHeight(INT32 index, double value);

        virtual INT32 GetTransformationMethod();
        virtual void SetTransformationMethod(INT32 mulRegTransformationMethod);

    private:
        void CopyTo(void* target) const;

        void Reset();
        void ReleaseInstance();

    private:
        csMultipleRegressionXformParams* mulRegParams;
        INT32 transformationMethod;
    };

} //namespace CSLibrary

#endif //_CCOORDINATESYSTEMGEODETICMULTIPLEREGRESSIONTRANSFORMDEFPARAMS_H_
