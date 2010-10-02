//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _MGCOORDINATESYSTEMGEODETICMULTIPLEREGRESSIONTRANSFORMDEFPARAMS_H_
#define _MGCOORDINATESYSTEMGEODETICMULTIPLEREGRESSIONTRANSFORMDEFPARAMS_H_

/// \defgroup MgCoordinateSystemGeodeticMultipleRegressionTransformDefParams MgCoordinateSystemGeodeticMultipleRegressionTransformDefParams
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// This class holds all parameters that are specific to the multiple regression transformation method.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticMultipleRegressionTransformDefParams : public MgCoordinateSystemGeodeticTransformDefParams
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticMultipleRegressionTransformDefParams)

PUBLISHED_API:

    //BEGIN: struct csGeodeticXformParmsDmaMulReg

    //virtual UINT32 GetPhiBitmap(INT32 index) = 0; // range [0..3]; long compiles as 32 bit in MS VC;
    //virtual void SetPhiBitmap(INT32 index, UINT32 value) = 0; //long compiles as 32 bit in MS VC;

    //virtual UINT32 GetLambdaBitmap(INT32 index) = 0; // range [0..3]; long compiles as 32 bit in MS VC;
    //virtual void SetLambdaBitmap(INT32 index, UINT32 value) = 0; //long compiles as 32 bit in MS VC;
    //
    //virtual UINT32 GetHeightBitmap(INT32 index) = 0; // range [0..3]; long compiles as 32 bit in MS VC;
    //virtual void SetHeightBitmap(INT32 index, UINT32 value) = 0; //long compiles as 32 bit in MS VC;

    virtual double GetValidation() = 0;
    virtual void SetValidation(double validation) = 0;

    virtual double GetTestPhi() = 0;
    virtual void SetTestPhi(double testPhi) = 0;

    virtual double GetTestLambda() = 0;
    virtual void SetTestLambda(double testLambda) = 0;

    virtual double GetDeltaPhi() = 0;
    virtual void SetDeltaPhi(double deltaPhi) = 0;

    virtual double GetDeltaLambda() = 0;
    virtual void SetDeltaLambda(double deltaLambda) = 0;

    virtual double GetDeltaHeight() = 0;
    virtual void SetDeltaHeight(double deltaHeight) = 0;

    virtual double GetPhiOffset() = 0;
    virtual void SetPhiOffset(double phiOffset) = 0;

    virtual double GetLambdaOffset() = 0;
    virtual void SetLambdaOffset(double lambdaOffset) = 0;

    virtual double GetNormalizationScale() = 0;
    virtual void SetNormalizationScale(double NormalizationScale) = 0;

    virtual double GetCoefficientPhi(INT32 index) = 0; // range [0..104]
    virtual void SetCoefficientPhi(INT32 index, double value) = 0;

    virtual double GetCoefficientLambda(INT32 index) = 0; // range [0..104]
    virtual void SetCoefficientLambda(INT32 index, double value) = 0;

    virtual double GetCoefficientHeight(INT32 index) = 0; // range [0..104]
    virtual void SetCoefficientHeight(INT32 index, double value) = 0;

    //END: struct csGeodeticXformParmsDmaMulReg

    virtual INT32 GetTransformationMethod() = 0;
    virtual void SetTransformationMethod(INT32 mulRegTransformationMethod) = 0;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId(){return m_cls_id;};

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGeodeticMultipleRegressionTransformDefParams;

};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICMULTIPLEREGRESSIONTRANSFORMDEFPARAMS_H_
