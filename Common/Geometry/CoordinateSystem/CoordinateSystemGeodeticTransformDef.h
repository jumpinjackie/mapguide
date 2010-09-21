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

#ifndef _MGCOORDINATESYSTEMGEODETICTRANSFORMDEF_H_
#define _MGCOORDINATESYSTEMGEODETICTRANSFORMDEF_H_

/// \defgroup MgCoordinateSystemGeodeticTransformDef MgCoordinateSystemGeodeticTransformDef
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// An instance of this class describes a geodetic transformation with all its parameters
/// as it is defined in the dictionary file. The actual transformation operation
/// is performed by the MgCoordinateSystemGeodeticTransformation objects
/// that's being returned by the CreateTransformation method.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticTransformDef : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticTransformDef)

PUBLISHED_API:

    //BEGIN: struct cs_GeodeticTransform_

    virtual STRING GetTransformName() = 0;
    virtual void SetTransformName(CREFSTRING name) = 0;

    virtual STRING GetSourceDatum() = 0;
    virtual void SetSourceDatum(CREFSTRING datumKey) = 0;

    virtual STRING GetTargetDatum() = 0;
    virtual void SetTargetDatum(CREFSTRING datumKey) = 0;

    virtual STRING GetGroup() = 0;
    virtual void SetGroup(CREFSTRING group) = 0;

    virtual STRING GetDescription() = 0;
    virtual void SetDescription(CREFSTRING description) = 0;

    virtual STRING GetSource() = 0;
    virtual void SetSource(CREFSTRING source) = 0;

    virtual INT32 GetEpsgCode() = 0;
    virtual void SetEpsgCode(INT32 epsgCode) = 0;

    virtual INT32 GetEpsgVariation() = 0;
    virtual void SetEpsgVariation(INT32 epsgVariation) = 0;

    virtual bool GetInverseSupported() = 0;
    virtual void SetInverseSupported(bool inverseSupported) = 0;

    virtual INT32 GetMaxIterations() = 0;
    virtual void SetMaxIterations(INT32 maxIterations) = 0;
	
    virtual double GetConvergenceValue() = 0;
    virtual void SetConvergenceValue(double convergenceValue) = 0;
	
    virtual double GetErrorValue() = 0;
    virtual void SetErrorValue(double errorValue) = 0;
	
    virtual double GetAccuracy() = 0;
    virtual void SetAccuracy(double accuracy) = 0;
    
    //END: struct cs_GeodeticTransform_

    virtual bool IsProtected() = 0;
    virtual bool IsValid() = 0;
    
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the type of this geodetic transformation (as defined by MgCoordinateSystemGeodeticTransformDefType).
    /// The type defines the parameters that are being returned by the GetParameters method.
    virtual INT32 GetTransformDefType() = 0;
    
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the extended parameters object valid for this transformation definition. See remarks.
    /// Depending on the type returned by GetTransformDefType(), this method returns one of the following:
    /// 
    /// \li NULL if this is a NULL transformation
    /// \li MgCoordinateSystemGeodeticAnalyticalTransformDefParams if this transformation uses an anylitical transformation method
    /// \li MgCoordinateSystemGeodeticInterpolationTransformDefParams if this transformation uses grid files
    /// \li MgCoordinateSystemGeodeticMultipleRegressionTransformDefParams if this transformation is a multiple regression transformation type
    ///
    /// \return
    /// Returns the extended parameters (or NULL) object for this transformation object. The
    /// caller is responsible for disposing the object being returned. See remarks.
    /// 
    /// \remarks
    /// If a caller needs to change the parameters of a transformation, the values
    /// have to be changed on the object being returned here and then written
    /// back via SetParameters. Setting the values on the parameters object 
    /// has no affect on this transformation definition.
    virtual MgCoordinateSystemGeodeticTransformDefParams* GetParameters() = 0;
    
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the extended parameters for this transformation.
    /// 
    /// \param parameters
    /// The parameters to set for this transformation. The object passed in,
    /// has to be of the same type as it has been returned by GetParameters. Otherwise,
    /// an exception will be thrown. This parameter can only be NULL, if this is a NONE transformation.
    /// 
    /// \remarks
    /// The instance passed in has to be disposed by the caller, i.e. this transformation
    /// object will not take ownership of it.
    virtual void SetParameters(MgCoordinateSystemGeodeticTransformDefParams* parameters) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a deep clone of this geodetic transformation. That is, the cloned object
    /// will also have its extended parameters cloned.
    ///
    /// \return
    /// Returns a new MgCoordinateSystemGeodeticTransformDef object that is an exact clone
    /// of this MgCoordinateSystemGeodeticTransformDef instance. The caller is responsible for
    /// disposing the object being returned.
    ///
    /// \remarks
    /// The returned MgCoordinateSystemGeodeticTransformDef instance will have its protected flag
    /// unset. However, all other properties like name etc will be the same. Any caller will
    /// have to change such values before trying to write the cloned instance into
    /// the dictionary file.
    virtual MgCoordinateSystemGeodeticTransformDef* CreateClone() = 0;
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a new MgCoordinateSystemGeodeticTransformation object that
    /// can be used to perform the actual transformation this transformation
    /// definition describes. This method will fail with an exception,
    /// if this is the NONE transformation.
    ///
    /// \param createInverse
    /// If true, the MgCoordinateSystemGeodeticTransformation being returned will
    /// perform the inverse of the transformation as defined in the dictionary file. That is,
    /// it will convert from the actual target datum to the source datum. Note, that not all
    /// transformations are invertible. This method will fail with an exception then.
    /// If false, this method will return a MgCoordinateSystemGeodeticTransformation
    /// instance that performs the default forward datum transformation.
    ///
    /// \return
    /// Returns the MgCoordinateSystemGeodeticTransformation that performs the actual
    /// datum transformation as described by this definition instance. The caller
    /// is reponsible for disposing the object being returned.
    virtual MgCoordinateSystemGeodeticTransformation* CreateTransformation(bool createInverse) = 0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGeodeticTransformDef;
};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICTRANSFORMDEF_H_
