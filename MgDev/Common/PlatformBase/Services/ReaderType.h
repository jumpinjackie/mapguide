//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _MGREADERTYPE_
#define _MGREADERTYPE_

/// \defgroup MgReaderType MgReaderType
/// \ingroup Feature_Service_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// Type of reader represented by an MgReader
///
/// \remarks
/// Defines the type of a reader whose class is derived from
/// MgReader. The three derived classes are MgDataReader,
/// MgFeatureReader, and MgSqlDataReader. An MgReaderType value
/// is returned by the reader's GetReaderType method.
///
class MgReaderType
{
PUBLISHED_API:
    /////////////////////////////////////////////////////////
    /// Signifies that the object is of type MgFeatureReader.
    static const int FeatureReader = 0;
    //////////////////////////////////////////////////////
    /// Signifies that the object is of type MgDataReader.
    static const int DataReader = 1;
    /////////////////////////////////////////////////////////
    /// Signifies that the object is of type MgSqlDataReader.
    static const int SqlDataReader = 2;
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

            throw new MgInvalidArgumentException(L"MgReaderType.ValidateRange",
                __LINE__, __WFILE__, &arguments, L"MgInvalidValueOutsideRange", NULL);
        }
    }
};
/// \}

#endif
