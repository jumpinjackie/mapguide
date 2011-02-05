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

#ifndef _MGCOORDINATESYSTEMGEODETICINTERPOLATIONTRANSFORMDEFPARAMS_H_
#define _MGCOORDINATESYSTEMGEODETICINTERPOLATIONTRANSFORMDEFPARAMS_H_

/// \defgroup MgCoordinateSystemGeodeticInterpolationTransformDefParams MgCoordinateSystemGeodeticInterpolationTransformDefParams
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// This class holds all parameters that are specific to grid-file based transformation. That is,
/// where the actual datum shift value for a coordinate is determined by interpolating between grid point values
/// as defined in the respective grid file(s).
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticInterpolationTransformDefParams : public MgCoordinateSystemGeodeticTransformDefParams
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticInterpolationTransformDefParams)

PUBLISHED_API:

    //BEGIN: struct csGeodeticXfromParmsFile

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the grid files to be used by this transformation. See the description for
    /// gridFiles parameter.
    ///
    /// \param gridFiles
    /// A (possibly empty) ordered(!) collection of MgCoordinateSystemGeodeticTransformGridFile objects.
    /// The order of the elements contained in the collection is important, because this
    /// instructs the underlying API in which order to use the grid files, e.g. in case of an overlap.
    ///
    /// \remarks
    /// The caller is responsible for disposing the collection of grid files. That is,
    /// this parameter object will not take ownership of the collection passed in.
    virtual void SetGridFiles(MgDisposableCollection* gridFiles) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an ordered, non-live collection MgCoordinateSystemGeodeticTransformGridFile elements
    /// currently used by this parameter object. Changing the collection does NOT
    /// have an affect on this parent parameter instance. Instead, any API client
    /// has to modify the collection and then set it back via SetGridFiles.
    /// The list returned can be an empty but will never be NULL.
    ///
    /// \remarks
    /// The caller is responsible for disposing the collection of grid files being returned
    /// here.
    virtual MgDisposableCollection* GetGridFiles() = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the name of the transformation to fallback to in case the transformation using the
    /// defined grid files doesn't succeed.
    ///
    /// \return
    /// Returns the name of the fallback transformation. Can be empty if no such fallback transformation
    /// is defined.
    ///
    virtual STRING GetFallback() = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the transformation to fallback to, if the transformation could not
    /// be performed using the grid files defined.
    ///
    /// \param fallbackTransformation
    /// Name of the transformation to fallback to.
    virtual void SetFallback(CREFSTRING fallbackTransformation) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a new, empty MgCoordinateSystemGeodeticTransformGridFile object
    /// that can be added to the collection set via SetGridFiles.
    ///
    /// \return
    /// Returns a new, unitialized MgCoordinateSystemGeodeticTransformGridFile instance
    /// The caller is resonsible for disposing the object if no longer needed.
    virtual MgCoordinateSystemGeodeticTransformGridFile* NewGridFile() = 0;

     //END: struct csGeodeticXfromParmsFile

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGeodeticInterpolationTransformDefParams;

};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICINTERPOLATIONTRANSFORMDEFPARAMS_H_
