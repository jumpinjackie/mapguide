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

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the transform name of this instance
    ///
    /// \return
    /// The transform name of this instance
    ///
    virtual STRING GetTransformName() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the transform name of this instance
    ///
    /// \param name (String/string)
    /// The transform name
    ///
    virtual void SetTransformName(CREFSTRING name) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the source datum of this instance
    ///
    /// \return
    /// The source datum of this instance
    ///
    virtual STRING GetSourceDatum() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the source datum of this instance
    ///
    /// \param datumKey (String/string)
    /// The source datum
    ///
    virtual void SetSourceDatum(CREFSTRING datumKey) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the target datum of this instance
    ///
    /// \return
    /// The target datum of this instance
    ///
    virtual STRING GetTargetDatum() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the target datum of this instance
    ///
    /// \param datumKey (String/string)
    /// The target datum
    ///
    virtual void SetTargetDatum(CREFSTRING datumKey) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the group of this instance
    ///
    /// \return
    /// The group of this instance
    ///
    virtual STRING GetGroup() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the group of this instance
    ///
    /// \param group (String/string)
    /// The group
    ///
    virtual void SetGroup(CREFSTRING group) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the description of this instance
    ///
    /// \return
    /// The description of this instance
    ///
    virtual STRING GetDescription() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the description of this instance
    ///
    /// \param description (String/string)
    /// The description
    ///
    virtual void SetDescription(CREFSTRING description) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the source of this instance
    ///
    /// \return
    /// The source of this instance
    ///
    virtual STRING GetSource() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the source of this instance
    ///
    /// \param source (String/string)
    /// The source
    ///
    virtual void SetSource(CREFSTRING source) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the EPSG code of this instance
    ///
    /// \return
    /// The EPSG code of this instance
    ///
    virtual INT32 GetEpsgCode() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the EPSG code of this instance
    ///
    /// \param epsgCode (int)
    /// The EPSG code
    ///
    virtual void SetEpsgCode(INT32 epsgCode) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the EPSG variation of this instance
    ///
    /// \return
    /// The EPSG variation of this instance
    ///
    virtual INT32 GetEpsgVariation() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the EPSG variation of this instance
    ///
    /// \param epsgVariation (int)
    /// The EPSG variation
    ///
    virtual void SetEpsgVariation(INT32 epsgVariation) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether the inverse is supported
    ///
    /// \return
    /// true if this inverse is supported. false otherwise
    ///
    virtual bool GetInverseSupported() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets whether the inverse is supported
    ///
    /// \param inverseSupported (boolean/bool)
    /// true if the inverse is supported. false otherwise
    ///
    virtual void SetInverseSupported(bool inverseSupported) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the maximum iterations of this instance
    ///
    /// \return
    /// The maximum iterations of this instance
    ///
    virtual INT32 GetMaxIterations() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the maximum iterations of this instance
    ///
    /// \param maxIterations (int)
    /// The maximum iterations
    ///
    virtual void SetMaxIterations(INT32 maxIterations) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the convergence value of this instance
    ///
    /// \return
    /// The convergence value of this instance
    ///
    virtual double GetConvergenceValue() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the convergence value of this instance
    ///
    /// \param convergenceValue (double)
    /// The convergence value
    ///
    virtual void SetConvergenceValue(double convergenceValue) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the error value of this instance
    ///
    /// \return
    /// The error value of this instance
    ///
    virtual double GetErrorValue() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the error value of this instance
    ///
    /// \param errorValue (double)
    /// The error value
    ///
    virtual void SetErrorValue(double errorValue) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the accuracy value of this instance
    ///
    /// \return
    /// The accuracy value of this instance
    ///
    virtual double GetAccuracy() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the accuracy value of this instance
    ///
    /// \param accuracy (double)
    /// The accuracy value
    ///
    virtual void SetAccuracy(double accuracy) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the minimum range longitude of this instance
    ///
    /// \return
    /// The minimum range longitude of this instance
    ///
    virtual double GetRangeMinLongitude() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the minimum range longitude of this instance
    ///
    /// \param minLong (double)
    /// The minimum range longitude
    ///
    virtual void SetRangeMinLongitude(double minLong) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the maximum range longitude of this instance
    ///
    /// \return
    /// The maximum range longitude of this instance
    ///
    virtual double GetRangeMaxLongitude() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the maximum range longitude of this instance
    ///
    /// \param maxLong (double)
    /// The maximum range longitude
    ///
    virtual void SetRangeMaxLongitude(double maxLong) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the minimum range latitude of this instance
    ///
    /// \return
    /// The minimum range latitude of this instance
    ///
    virtual double GetRangeMinLatitude() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the minimum range latitude of this instance
    ///
    /// \param minLat (double)
    /// The minimum range latitude
    ///
    virtual void SetRangeMinLatitude(double minLat) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the maximum range latitude of this instance
    ///
    /// \return
    /// The maximum range latitude of this instance
    ///
    virtual double GetRangeMaxLatitude() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the maximum range latitude of this instance
    ///
    /// \param maxLat (double)
    /// The maximum range latitude
    ///
    virtual void SetRangeMaxLatitude(double maxLat) = 0;

    //END: struct cs_GeodeticTransform_

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this instance is protected
    ///
    /// \return
    /// true if this instance is protected. false otherwise
    ///
    virtual bool IsProtected() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this instance is valid
    ///
    /// \return
    /// true if this instance is valid. false otherwise
    ///
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
    /// \li MgCoordinateSystemGeodeticStandaloneTransformDefParams if this transformation is a standalone (aka built-in) transformation type
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
    /// an exception will be thrown. This parameter must not be NULL. Creating a transformation that
    /// is supposed to do nothing except for allowing to switch from one datum to another (that is considered to be equal),
    /// requires to create standalone transformation of type NullX.
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

INTERNAL_API:
    virtual UINT8* SerializeFrom(UINT8* pStream)=0;
    virtual UINT8* SerializeTo(UINT8* pStream)=0;
    virtual UINT32 GetSizeSerialized()=0;

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
