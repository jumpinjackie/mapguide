//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _MGLAYERTYPE_H_
#define _MGLAYERTYPE_H_

/// \defgroup MgLayerType MgLayerType
/// \ingroup Maps_and_Layers_Module
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// MgLayerType defines integer constants used to identify the
/// type of a layer.
class MgLayerType
{
PUBLISHED_API:
    ////////////////////////////////////////////
    /// \brief
    /// Specifies that the layer is a dynamic layer.
    ///
    static const INT32 Dynamic = 1;
    /////////////////////////////////////////////////
    /// \brief
    /// Specifies that the layer is a base map layer.
    ///
    static const INT32 BaseMap = 2;
};
/// \}

#endif //_MGLAYERTYPE_H_
