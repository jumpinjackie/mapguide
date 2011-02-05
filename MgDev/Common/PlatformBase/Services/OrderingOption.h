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

#ifndef _MGORDERINGOPTION_
#define _MGORDERINGOPTION_

/// \defgroup MgOrderingOption MgOrderingOption
/// \ingroup Feature_Service_classes
/// \{

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Defines a type used to specify the order in which features
/// are returned by MgFeatureService::SelectFeatures. This type
/// is used as an argument to the MgFeatureQueryOptions::SetOrderingFilter.
///
class MgOrderingOption
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Signifies that the features are returned in ascending order
    /// relative to the properties specified in the first argument of
    /// MgFeatureQueryOptions::SetOrderingFilter.
    static const int Ascending = 0;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Signifies that the features are returned in descending order
    /// relative to the properties specified in the first argument of
    /// MgFeatureQueryOptions::SetOrderingFilter.
    static const int Descending = 1;
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

            throw new MgInvalidArgumentException(L"MgOrderingOption.ValidateRange",
                __LINE__, __WFILE__, &arguments, L"MgInvalidValueOutsideRange", NULL);
        }
    }
};
/// \}

#endif
