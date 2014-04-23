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

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the validation value of this instance
    ///
    /// \return
    /// The validation value of this instance
    ///
    virtual double GetValidation() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the validation value of this instance
    ///
    /// \param validation (double)
    /// The validation value
    ///
    virtual void SetValidation(double validation) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the test phi value of this instance
    ///
    /// \return
    /// The test phi value of this instance
    ///
    virtual double GetTestPhi() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the test phi value of this instance
    ///
    /// \param testPhi (double)
    /// The test phi value
    ///
    virtual void SetTestPhi(double testPhi) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the test lambda value of this instance
    ///
    /// \return
    /// The test lambda value of this instance
    ///
    virtual double GetTestLambda() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the test lambda value of this instance
    ///
    /// \param testLambda (double)
    /// The test lambda value
    ///
    virtual void SetTestLambda(double testLambda) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the delta phi value of this instance
    ///
    /// \return
    /// The delta phi value of this instance
    ///
    virtual double GetDeltaPhi() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the delta phi value of this instance
    ///
    /// \param deltaPhi (double)
    /// The delta phi value
    ///
    virtual void SetDeltaPhi(double deltaPhi) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the delta lambda value of this instance
    ///
    /// \return
    /// The delta lambda value of this instance
    ///
    virtual double GetDeltaLambda() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the delta lambda value of this instance
    ///
    /// \param deltaLambda (double)
    /// The delta lambda value
    ///
    virtual void SetDeltaLambda(double deltaLambda) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the delta height value of this instance
    ///
    /// \return
    /// The delta height value of this instance
    ///
    virtual double GetDeltaHeight() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the delta height value of this instance
    ///
    /// \param deltaHeight (double)
    /// The delta height value
    ///
    virtual void SetDeltaHeight(double deltaHeight) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the phi offset value of this instance
    ///
    /// \return
    /// The phi offset value of this instance
    ///
    virtual double GetPhiOffset() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the phi offset value of this instance
    ///
    /// \param phiOffset (double)
    /// The phi offset value
    ///
    virtual void SetPhiOffset(double phiOffset) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the lambda offet value of this instance
    ///
    /// \return
    /// The lambda offet value of this instance
    ///
    virtual double GetLambdaOffset() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the lambda offet value of this instance
    ///
    /// \param lambdaOffset (double)
    /// The lambda offet value
    ///
    virtual void SetLambdaOffset(double lambdaOffset) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the normalization scale of this instance
    ///
    /// \return
    /// The normalization scale of this instance
    ///
    virtual double GetNormalizationScale() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the normalization scale of this instance
    ///
    /// \param NormalizationScale (double)
    /// The normalization scale
    ///
    virtual void SetNormalizationScale(double NormalizationScale) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coefficent phi value of this instance at the specified index
    ///
    /// \param index (int)
    /// The index which to retrieve the coefficient phi value
    ///
    /// \return
    /// The coefficent phi value of this instance at the specified index
    ///
    virtual double GetCoefficientPhi(INT32 index) = 0; // range [0..104]

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the coefficent phi value of this instance at the specified index
    ///
    /// \param index (int)
    /// The index which to set the coefficient phi value
    /// \param value (double)
    /// The coefficient phi value
    ///
    virtual void SetCoefficientPhi(INT32 index, double value) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coefficent lambda value of this instance at the specified index
    ///
    /// \param index (int)
    /// The index which to retrieve the coefficient lambda value
    ///
    /// \return
    /// The coefficent lambda value of this instance at the specified index
    ///
    virtual double GetCoefficientLambda(INT32 index) = 0; // range [0..104]

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the coefficent lambda value of this instance at the specified index
    ///
    /// \param index (int)
    /// The index which to set the coefficient lambda value
    /// \param value (double)
    /// The coefficient lambda value
    ///
    virtual void SetCoefficientLambda(INT32 index, double value) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coefficent height value of this instance at the specified index
    ///
    /// \param index (int)
    /// The index which to retrieve the coefficient height value
    ///
    /// \return
    /// The coefficent height value of this instance at the specified index
    ///
    virtual double GetCoefficientHeight(INT32 index) = 0; // range [0..104]

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the coefficent height value of this instance at the specified index
    ///
    /// \param index (int)
    /// The index which to set the coefficient height value
    /// \param value (double)
    /// The coefficient height value
    ///
    virtual void SetCoefficientHeight(INT32 index, double value) = 0;

    //END: struct csGeodeticXformParmsDmaMulReg

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the transformation method of this instance
    ///
    /// \return
    /// The transformation method of this instance
    ///
    virtual INT32 GetTransformationMethod() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the transformation method of this instance
    ///
    /// \param mulRegTransformationMethod (int)
    /// The transformation method
    ///
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
