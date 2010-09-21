//
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

    virtual STRING GetPathName()=0;
    virtual void SetPathName(CREFSTRING)=0;
    
    virtual STRING GetDescription()=0;  /// __get
    virtual void SetDescription(CREFSTRING)=0;
    
    virtual STRING GetGroup()=0;  /// __get
    virtual void SetGroup(CREFSTRING)=0;
    
    virtual STRING GetSource()=0;  /// __get
    virtual void SetSource(CREFSTRING)=0;
    
    virtual STRING GetTargetDatum()=0;  /// __get
    virtual void SetTargetDatum(CREFSTRING)=0;

    virtual STRING GetSourceDatum()=0;  /// __get
    virtual void SetSourceDatum(CREFSTRING)=0;

    virtual double GetAccuracy()=0; /// __get
    virtual void SetAccuracy(double accuracy)=0;

    virtual bool GetIsReversible()=0;
    virtual void SetIsReversible(bool isReversible)=0;

    virtual INT32 GetEpsgCode()=0;
    virtual void SetEpsgCode(INT32 epsgCode)=0;
    
    virtual INT32 GetEpsgVariant()=0;
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
    
    virtual bool IsProtected() = 0;
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
