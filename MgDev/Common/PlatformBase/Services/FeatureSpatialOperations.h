//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _MGFEATURESPATIALOPERATIONS_
#define _MGFEATURESPATIALOPERATIONS_

/// \defgroup MgFeatureSpatialOperations MgFeatureSpatialOperations
/// \ingroup Feature_Service_classes
/// \{

/////////////////////////////////////////////////////////////
/// \brief
/// Is a set of constants used to type spatial operations.
/// \remarks
/// A value from this set of constants is used as an argument
/// incalls to MgFeatureQueryOptions::SetSpatialFilter.
///
class MgFeatureSpatialOperations
{
PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Test whether the geometric property value spatially \link Contains contains \endlink
    /// the literal geometric value.
    static const int Contains = 0;
    //////////////////////////////////////////////////////////////////////////////////
    /// Test whether the geometric property value \link Crosses crosses \endlink
    /// the given geometry.
    static const int Crosses = 1;
    ///////////////////////////////////////////////////////////////////////
    /// Test whether the geometric property value is \link Disjoint disjoint \endlink
    /// from the given geometry.
    static const int Disjoint = 2;
    //////////////////////////////////////////////////////////////////////////////////
    /// Test whether the geometric property value is \link Equals equal \endlink
    /// to the given geometry.
    static const int Equals = 3;
    ////////////////////////////////////////////////////////////////
    /// Test whether the geometric property \link Intersects intersects \endlink
    /// the given geometry.
    static const int Intersects = 4;
    //////////////////////////////////////////////////////////////////////////////
    /// Test whether the geometric property \link Overlaps overlaps \endlink
    /// the given geometry.
    static const int Overlaps = 5;
    /////////////////////////////////////////////////////////////////
    /// Test whether the geometric property \link Touches touches \endlink the
    /// given geometry.
    static const int Touches = 6;
    /////////////////////////////////////////////////////////////////////////////
    /// Test whether the geometric property is \link Within within \endlink
    /// the given geometry.
    static const int Within = 7;
    //////////////////////////////////////////////////////////////////////////////////////////
    /// Test whether the geometric property value is \link CoveredBy covered by \endlink
    /// the given geometry.
    static const int CoveredBy = 8;
    /////////////////////////////////////////////////////////////////////////////
    /// Test whether the geometric property is \link Inside inside \endlink
    /// the given geometry.
    static const int Inside = 9;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Test whether the envelope of the geometric property \link EnvelopeIntersects intersects \endlink
    /// the envelope of the given geometry.
    static const int EnvelopeIntersects = 10;
INTERNAL_API:

    static void ValidateRange(INT32 value)
    {
        if (value < 0 || value > 10)
        {
            STRING buffer;
            MgUtil::Int32ToString(value, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgFeatureSpatialOperations.ValidateRange",
                __LINE__, __WFILE__, &arguments, L"MgInvalidValueOutsideRange", NULL);
        }
    }

    static STRING ToString(INT32 value)
    {
        STRING text;
        switch (value)
        {
        case Contains:
            text = L"Contains";  //NOXLATE
            break;
        case Crosses:
            text = L"Crosses";  //NOXLATE
            break;
        case Disjoint:
            text = L"Disjoint";  //NOXLATE
            break;
        case Equals:
            text = L"Equals";  //NOXLATE
            break;
        case Intersects:
            text = L"Intersects";  //NOXLATE
            break;
        case Overlaps:
            text = L"Overlaps";  //NOXLATE
            break;
        case Touches:
            text = L"Touches";  //NOXLATE
            break;
        case Within:
            text = L"Within";  //NOXLATE
            break;
        case CoveredBy:
            text = L"CoveredBy";  //NOXLATE
            break;
        case Inside:
            text = L"Inside";  //NOXLATE
            break;
        case EnvelopeIntersects:
            text = L"EnvelopeIntersects";  //NOXLATE
            break;
        default:
            break;
        }
        return text;
    }

};
/// \}

#endif
