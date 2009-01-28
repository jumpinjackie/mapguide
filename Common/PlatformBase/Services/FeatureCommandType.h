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

#ifndef _MGFEATURECOMMANDTYPE_
#define _MGFEATURECOMMANDTYPE_

/// \defgroup MgFeatureCommandType MgFeatureCommandType
/// \ingroup Feature_Service_classes
/// \{

/////////////////////////////////////////////////////////
/// \brief
/// Is a set of constants used to specify the type of an
/// MgFeatureCommand object.
///
class MgFeatureCommandType
{
PUBLISHED_API:
    /////////////////////////////////////////////////////
    /// Specifies the type of an MgInsertFeatures object.
    static const int InsertFeatures = 0;
    /////////////////////////////////////////////////////
    /// Specifies the type of an MgUpdateFeatures object.
    static const int UpdateFeatures = 1;
    /////////////////////////////////////////////////////
    /// Specifies the type of an MgDeleteFeatures object.
    static const int DeleteFeatures = 2;
    /////////////////////////////////////////////////////
    /// Specifies the type of an MgLockFeatures object.
    static const int LockFeatures = 3;
    /////////////////////////////////////////////////////
    /// Specifies the type of an MgUnlockFeatures object.
    static const int UnlockFeatures = 4;
INTERNAL_API:

    static void ValidateRange(INT32 value)
    {
        if (value < 0 || value > 4)
        {
            STRING buffer;
            MgUtil::Int32ToString(value, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgFeatureCommandType.ValidateRange",
                __LINE__, __WFILE__, &arguments, L"MgInvalidValueOutsideRange", NULL);
        }
    }
};
/// \}

#endif
