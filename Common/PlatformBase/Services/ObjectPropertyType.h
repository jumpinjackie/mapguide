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

#ifndef _MGOBJECT_PROPERTY_TYPE_
#define _MGOBJECT_PROPERTY_TYPE_

/// \defgroup MgObjectPropertyType MgObjectPropertyType
/// \ingroup Feature_Schema_Module
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// Defines the type of an MgObjectPropertionDefinition object
/// property, that is, whether the property ccontains a single
/// feature class object, or an unordered or ordered collection
/// of them.
///
class MgObjectPropertyType
{
PUBLISHED_API:
    ////////////////////////////////////////////////////////////////
    /// Signifies that the object property contains a single feature
    /// class object.
    static const int Value = 0;
    //////////////////////////////////////////////////////////////
    /// Signifies that the object property contains more than one
    /// feature class object. The collection of objects is in no
    /// particular order relative to the identity property defined
    /// for the collection. See
    /// MgObjectPropertyDefinition::GetIdentityProperty.
    static const int Collection = 1;
    /////////////////////////////////////////////////////////////////
    /// Signifies that the object property contains more than one
    /// feature class object in ascending or descending order
    /// relative to the identity property defined for the collection.
    /// See MgObjectPropertyDefinition::GetIdentityProperty.
    static const int OrderedCollection = 2;
INTERNAL_API:

    static void ValidateRange(INT32 value)
    {
        if (value < 0 || value > 2)
        {
            STRING buffer;
            MgUtil::Int32ToString(value, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgObjectPropertyType.ValidateRange",
                __LINE__, __WFILE__, &arguments, L"MgInvalidValueOutsideRange", NULL);
        }
    }
};
/// \}

#endif
