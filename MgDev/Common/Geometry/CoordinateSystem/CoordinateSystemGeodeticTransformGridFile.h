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

#ifndef _MGCOORDINATESYSTEMGEODETICTRANSFORMGRIDFILE_H_
#define _MGCOORDINATESYSTEMGEODETICTRANSFORMGRIDFILE_H_

/// \defgroup MgCoordinateSystemGeodeticTransformGridFile MgCoordinateSystemGeodeticTransformGridFile
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// The MgCoordinateSystemGeodeticInterpolationTransformDefParams parameter class requires
/// instances of MgCoordinateSystemGeodeticTransformGridFile to be set via its
/// SetGridFiles method. An MgCoordinateSystemGeodeticTransformGridFile object simply describes
/// the grid file rather than containing the (grid file) information itself.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticTransformGridFile : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticTransformGridFile)

PUBLISHED_API:

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the format of the grid file as a constant value as defined in
    /// MgCoordinateSystemGeodeticTransformGridFileFormat.
    ///
    /// \return
    /// Returns the file format as an INT32 value.
    virtual INT32 GetFileFormat() = 0;

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets this file's format. Has to be constant
    /// as defined by MgCoordinateSystemGeodeticTransformGridFileFormat.
    virtual void SetFileFormat(INT32 gridFileFormat) = 0;

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// When used within a geodetic transformation that defines a source and a target
    /// datum, this flag instructs the API whether to consider the grid file's content
    /// to be the inverse of the actual conversion direction.
    ///
    /// \return
    /// Returns true, if the grid file provides information to transform between
    /// 2 datums in the inversed direction to what's actually specified in
    /// the parent transformation definition.
    virtual bool GetIsInverseDirection() = 0;

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the flag, whether this grid file carries the information
    /// to perform a datum transformation in the inversed direction
    /// to what's specified in the parent transformation definition.
    virtual void SetIsInverseDirection(bool isInverseDirection) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the physical file name
    ///
    /// \return
    /// The physical file name
    ///
    virtual STRING GetFileName() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the physical file name
    ///
    /// \param fileName (String/string)
    /// The physical file name
    ///
    virtual void SetFileName(CREFSTRING fileName) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this file is protected
    ///
    /// \return
    /// true if this file is protected. false otherwise
    ///
    virtual bool IsProtected() = 0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this file is valid
    ///
    /// \return
    /// true if this file is valid. false otherwise
    ///
    virtual bool IsValid() = 0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGeodeticTransformGridFile;

};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICTRANSFORMGRIDFILE_H_
