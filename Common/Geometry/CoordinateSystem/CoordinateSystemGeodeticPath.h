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

#ifndef _MGCOORDINATESYSTEMGEODETICPATH_H_
#define _MGCOORDINATESYSTEMGEODETICPATH_H_

/// \defgroup MgCoordinateSystemGeodeticPath MgCoordinateSystemGeodeticPath
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// A geodetic (transformation) path allows to concatenate multiple
/// geodetic transformations. For example, converting between 2 datums
/// cannot be achieved by doing 1 datum shift only but requires to
/// do 1 or more datums shifts in between.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticPath : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticPath)

PUBLISHED_API:

    //properties

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the path name of this geodetic path
    ///
    /// \return
    /// The path name of this geodetic path
    ///
    virtual STRING GetPathName()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the path name of this geodetic path
    ///
    /// \param sName (String/string)
    /// The path name
    ///
    virtual void SetPathName(CREFSTRING sName)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the description of this geodetic path
    ///
    /// \return
    /// The description of this geodetic path
    ///
    virtual STRING GetDescription()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the description of this geodetic path
    ///
    /// \param sDescription (String/string)
    /// The description
    ///
    virtual void SetDescription(CREFSTRING sDescription)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the group of this geodetic path
    ///
    /// \return
    /// The group of this geodetic path
    ///
    virtual STRING GetGroup()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the group of this geodetic path
    ///
    /// \param sGroup (String/string)
    /// The group
    ///
    virtual void SetGroup(CREFSTRING sGroup)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the source of this geodetic path
    ///
    /// \return
    /// The source of this geodetic path
    ///
    virtual STRING GetSource()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the source of this geodetic path
    ///
    /// \param sSource (String/string)
    /// The source
    ///
    virtual void SetSource(CREFSTRING sSource)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the target datum of this geodetic path
    ///
    /// \return
    /// The target datum of this geodetic path
    ///
    virtual STRING GetTargetDatum()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the target datum of this geodetic path
    ///
    /// \param sTargetDatum (String/string)
    /// The target datum
    ///
    virtual void SetTargetDatum(CREFSTRING sTargetDatum)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the source datum of this geodetic path
    ///
    /// \return
    /// The source datum of this geodetic path
    ///
    virtual STRING GetSourceDatum()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the source datum of this geodetic path
    ///
    /// \param sSourceDatum (String/string)
    /// The source datum
    ///
    virtual void SetSourceDatum(CREFSTRING sSourceDatum)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the accuracy of this geodetic path
    ///
    /// \return
    /// The accuracy of this geodetic path
    ///
    virtual double GetAccuracy()=0; /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the accuracy of this geodetic path
    ///
    /// \param accuracy (String/string)
    /// The accuracy
    ///
    virtual void SetAccuracy(double accuracy)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this geodetic path is reversible
    ///
    /// \return
    /// true if this geodetic path is reversible
    ///
    virtual bool GetIsReversible()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets whether this geodetic path is reversible
    ///
    /// \param isReversible (boolean/bool)
    /// true if reversible. false otherwise
    ///
    virtual void SetIsReversible(bool isReversible)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the EPSG code of of this geodetic path
    ///
    /// \return
    /// The EPSG code of this geodetic path
    ///
    virtual INT32 GetEpsgCode()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the EPSG code of of this geodetic path
    ///
    /// \param epsgCode (int)
    /// the EPSG code
    ///
    virtual void SetEpsgCode(INT32 epsgCode)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the EPSG variant of this geodetic path
    ///
    /// \return
    /// The EPSG variant of this geodetic path
    ///
    virtual INT32 GetEpsgVariant()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the EPSG variant of of this geodetic path
    ///
    /// \param epsgVariant (int)
    /// the EPSG variant
    ///
    virtual void SetEpsgVariant(INT32 epsgVariant)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an ordered collection of MgCoordinateSystemGeodeticPathElement objects
    /// that form this geodetic transformation path. See remarks.
    ///
    /// \return
    /// Returns a (possibly empty) collection of MgCoordinateSystemGeodeticPathElement instances.
    /// The caller is responsible for disposing the collection being returned.
    ///
    /// \remarks
    /// Changing the collection being returned has no affect on this parent geodetic
    /// path instance. Instead, the collection can be modified and then re-set via
    /// SetPathElements.
    virtual MgDisposableCollection* GetPathElements() = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the individual transformation steps that form this geodetic transformation
    /// path.
    ///
    /// \remarks
    /// The caller is responsible for disposing the collection passed in, i.e.
    /// this geodetic path instance will not take ownership of the elements.
    virtual void SetPathElements(MgDisposableCollection* pathElements) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this geodetic path is protected
    ///
    /// \return
    /// true if this geodetic path is protected. false otherwise
    ///
    virtual bool IsProtected() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this geodetic path is valid
    ///
    /// \return
    /// true if this geodetic path is valid. false otherwise
    ///
    virtual bool IsValid() = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a new, unitialized MgCoordinateSystemGeodeticPathElement instance.
    /// Calling this method does not affect the underlying dictionary file.
    /// Typically, an API client will retrieve new objects through this method
    /// so they can be added to the collection of path elements that can be set
    /// via SetPathElements.
    ///
    /// \return
    /// Returns a new MgCoordinateSystemGeodeticPathElement instance. The caller is responsible
    /// for disposing the object being returned.
    virtual MgCoordinateSystemGeodeticPathElement* NewPathElement() = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a deep clone of this geodetic path. That is, the cloned object
    /// will also have its geodetic path element information cloned.
    ///
    /// \return
    /// Returns a new MgCoordinateSystemGeodeticPath object that is an exact clone
    /// of this MgCoordinateSystemGeodeticPath instance. The caller is responsible for
    /// disposing the object being returned.
    ///
    /// \remarks:
    /// The returned MgCoordinateSystemGeodeticPath instance will have its protected flag
    /// unset. However, all other properties like name etc will be the same. Any caller will
    /// have to change such values before trying to write the cloned instance into
    /// the dictionary file.
    virtual MgCoordinateSystemGeodeticPath* CreateClone() = 0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGeodeticPath;
};
/// \}

#endif //_MGCOORDINATESYSTEMGEODETICPATH_H_
