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

/// \defgroup MgPropertyType MgPropertyType
/// \ingroup Feature_Properties_Module
/// \{

#ifndef _MG_PROPERTY_TYPE_H_
#define _MG_PROPERTY_TYPE_H_

////////////////////////////////////////////////////////////
/// \brief
/// Defines the names for the various property types.
///
/// \remarks
/// All of the property types are data property types except
/// Feature, Geometry, Null, and Raster.
///
class MgPropertyType
{
PUBLISHED_API:
    ///////////////////////////////////////////////////
    /// \brief
    /// Type name for a null property.
    ///
    /// \remarks
    /// This property type name is not currently used.
    static const int Null     =  0;

    /////////////////////////////////////////////////////////
    /// \brief
    /// Type name for a boolean property (MgBooleanProperty).
    static const int Boolean  =  1;

    ////////////////////////////////////////////////////////
    /// \brief
    /// Type name for a byte (unsigned 8 bit value) property
    /// (MgByteProperty).
    static const int Byte     =  2;

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Type name for an MgDateTime property (MgDateTimeProperty).
    static const int DateTime =  3;

    /////////////////////////////////////////////////////////
    /// \brief
    /// Type name for a single precision floating point value
    /// property (MgSingleProperty).
    static const int Single   =  4;

    /////////////////////////////////////////////////////////
    /// \brief
    /// Type name for a double precision floating point value
    /// property (MgDoubleProperty).
    static const int Double   =  5;

    ////////////////////////////////////////////////////////
    /// \brief
    /// Type name for a 16 bit signed integer value property
    /// (MgInt16Property).
    static const int Int16    =  6;

    ////////////////////////////////////////////////////////
    /// \brief
    /// Type name for a 32 bit signed integer value property
    /// (MgInt32Property).
    static const int Int32    =  7;

    ////////////////////////////////////////////////////////
    /// \brief
    /// Type name for a 64 bit signed integer value property
    /// (MgInt64Property).
    static const int Int64    =  8;

    ///////////////////////////////////////////////////////
    /// \brief
    /// Type name for a string property (MgStringProperty).
    static const int String   =  9;

    ////////////////////////////////////////////////
    /// \brief
    /// Type name for a Binary Large OBject property
    /// (MgBlobProperty).
    static const int Blob     = 10;

    ///////////////////////////////////////////////////
    /// \brief
    /// Type name for a Character Large OBject property
    /// (MgClobProperty).
    static const int Clob     = 11;

    /////////////////////////////////////////////////////////
    /// \brief
    /// Type name for a feature property (MgFeatureProperty).
    static const int Feature  = 12;

    ///////////////////////////////////////////////////////////
    /// \brief
    /// Type name for a geometry property (MgGeometryProperty).
    static const int Geometry = 13;

    ///////////////////////////////////////////////////////
    /// \brief
    /// Type name for a raster property (MgRasterProperty).
    static const int Raster   = 14;

    ///////////////////////////////////////////////////////
    /// \brief
    /// Type name for a decimal property.
    /// \since 2.2
    static const int Decimal  = 15;
};
/// \}

#endif
