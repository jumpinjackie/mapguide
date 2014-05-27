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

#ifndef _MG_FEATURE_PROPERTY_TYPE_H_
#define _MG_FEATURE_PROPERTY_TYPE_H_

/// \defgroup MgFeaturePropertyType MgFeaturePropertyType
/// \ingroup Feature_Properties_Module
/// \{

///////////////////////////////////////////////////////////
/// \brief
/// Defines names for the different types of property
/// definitions. See MgPropertyDefinition::GetPropertyType.
///
class MgFeaturePropertyType
{
PUBLISHED_API:

    /////////////////////////////////////////////////
    /// \brief
    /// Type name for a data property definition. See
    /// MgDataPropertyDefinition.
    static const int DataProperty           =  100;

    ////////////////////////////////////////////////////
    /// \brief
    /// Type name for an object property definition. See
    /// MgObjectPropertyDefinition.
    static const int ObjectProperty         =  101;

    //////////////////////////////////////////////////////
    /// \brief
    /// Type name for a geometric property definition. See
    /// MgGeometricPropertyDefinition.
    static const int GeometricProperty      =  102;

    ///////////////////////////////////////////////////////////
    /// \brief
    /// Type name for an association property definition.
    ///
    /// \remarks
    /// Currently there is no class for an association property
    /// definition.
    static const int AssociationProperty    =  103;

    ///////////////////////////////////////////////////
    /// \brief
    /// Type name for a raster property definition. See
    /// MgRasterPropertyDefinition.
    static const int RasterProperty         =  104;
};
/// \}

#endif
