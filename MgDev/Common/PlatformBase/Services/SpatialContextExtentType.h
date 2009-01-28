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

/// \defgroup MgSpatialContextExtentType MgSpatialContextExtentType
/// \ingroup Feature_Service_classes
/// \{

#ifndef _MG_SPATIAL_CONTEXT_EXTENT_TYPE_H
#define _MG_SPATIAL_CONTEXT_EXTENT_TYPE_H

/////////////////////////////////////////////////////////////////
/// \brief
/// The SpatialContextExtentType enumeration defines how the extent of
/// a context should be handled by the provider.
class MgSpatialContextExtentType
{
PUBLISHED_API:
    /// The spatial extent of the context is static and must be specified
    /// when the context is created.
    static const int scStatic = 0;
    /// The spatial extent of the context is dynamic and changes as data is
    /// added and removed from the context.
    static const int scDynamic = 1;
INTERNAL_API:

    static void ValidateRange(INT32 value)
    {
        if (value < 0 || value > 1)
        {
            STRING buffer;
            MgUtil::Int32ToString(value, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgSpatialContextExtentType.ValidateRange",
                __LINE__, __WFILE__, &arguments, L"MgInvalidValueOutsideRange", NULL);
        }
    }

};
/// \}

#endif
