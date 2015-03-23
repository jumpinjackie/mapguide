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

#ifndef _MGLAYERGROUPTYPE_H_
#define _MGLAYERGROUPTYPE_H_

/// \defgroup MgLayerGroupType MgLayerGroupType
/// \ingroup Maps_and_Layers_Module
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// MgLayerGroupType defines integer constants used to identify
/// the type of a layer group.
class MgLayerGroupType
{
PUBLISHED_API:
    ////////////////////////////////////////////
    /// \brief
    /// Specifies that the layer group is a normal layer group.
    ///
    static const INT32 Normal = 1;
    /////////////////////////////////////////////////
    /// \brief
    /// Specifies that the layer is a base map layer group (i.e.
    /// it contains base map layers).
    ///
    static const INT32 BaseMap = 2;
    /////////////////////////////////////////////////
    /// \brief
    /// Specifies that the layer is a base map layer from a TileSetDefinition resource.
    ///
    /// \remarks
    /// From a client application perspective, a layer group with of this type should be treated
    /// the same as layer group of type \link MgLayerGroupType::BaseMap BaseMap \endlink
    ///
    /// \since 3.0
    static const INT32 BaseMapFromTileSet = 3;
};
/// \}

#endif //_MGLAYERGROUPTYPE_H_
