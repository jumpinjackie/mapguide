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

#ifndef _MGCOORDINATESYSTEMGEODETICTRANSFORMATIONMETHOD_H_
#define _MGCOORDINATESYSTEMGEODETICTRANSFORMATIONMETHOD_H_

/// \defgroup MgCoordinateSystemGeodeticTransformationMethod MgCoordinateSystemGeodeticTransformationMethod
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// Defines the type of a geodetic transformation method
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticTransformationMethod
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// None
    ///
    static const INT32 None = 0;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Use the Molodensky transformation to convert to WGS84
    ///
    static const INT32 Molodensky = 1;      
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Use the Multiple Regression Transformation formulas.
    /// If such a definition is not available, default to the
    /// Bursa/Wolfe Seven Parameter Transformation.
    ///
    static const INT32 MReg = 2;            
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Use the Bursa/Wolfe to convert to WGS84, default to
    /// Molodensky if the necessary parameters are not present.
    ///
    static const INT32 Bursa = 3;           
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Use the NADCON or Canadian National Transformation
    /// emulation as appropriate to convert to NAD83, and
    /// consider the result to be WGS84 coordinates.
    ///
    static const INT32 NAD27 = 4;           
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Consider the coordinates to be WGS84 coordinates already, no shift is to be performed.
    ///
    static const INT32 NAD83 = 5;           
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// The coordinates are WGS84 coordinates already, no datum shift is required
    ///
    static const INT32 WGS84 = 6;           
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Use an internal formula to convert to WGS84
    ///
    static const INT32 WGS72 = 7;           
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Use the NADCOM algorithm, but use the HPGN data
    /// files, to shift the coordinates to NAD83, then
    /// consider the result to be WGS84 coordinates
    /// without any further datum shift
    ///
    static const INT32 HPGN = 8;            
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Seven Parameter Transformation
    ///
    static const INT32 SevenParameter = 9;  
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// AGD66<->GDA94 via Grid Files
    ///
    static const INT32 AGD66 = 10;          
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Three Parameter Transformation - Obsolete technique that is kept around for backward compatibility
    ///
    static const INT32 ThreeParameter = 11; 
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Six Parameter Transformation
    ///
    static const INT32 SixParameter = 12;   
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Four Parameter Transformation
    ///
    static const INT32 FourParameter = 13;  
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// AGD84<->GDA94 via Grid Files
    ///
    static const INT32 AGD84 = 14;          
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// NZGD49<->NZGD2K via Grid File
    ///
    static const INT32 NZGD49 = 15;         
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// ATS77<->NAD83 via Grid Files
    ///
    static const INT32 ATS77 = 16;          
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Is GDA94, no shift required.
    ///
    static const INT32 GDA94 = 17;          
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Is NZGD2000, no shift required.
    ///
    static const INT32 NZGD2K = 18;         
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// NAD83<->CSRS Via Grid Files.
    ///
    static const INT32 CSRS = 19;           
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// TOKYO<->JGD2k via Grid Files
    ///
    static const INT32 TOKYO = 20;          
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// NTF<->RGF93 via Grid File
    ///
    static const INT32 RGF93 = 21;          
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// ED50 <--> ETRF89 via Grid Files
    ///
    static const INT32 ED50 = 22;           
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// DHDN<->ETRF89 via Grid Files
    ///
    static const INT32 DHDN = 23;           
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Is ETRF89, no shift required
    ///
    static const INT32 ETRF89 = 24;         
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Local Geodetic Reference System
    ///
    static const INT32 Lclgrf = 99;         
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Geocentric Translation
    ///
    static const INT32 Geocentric = 25;     
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Swiss Ch1903 to CH1903+
    ///
    static const INT32 CHENYX = 26;         
};
/// \}

#endif //_MGCOORDINATESYSTEMGEODETICTRANSFORMATIONMETHOD_H_
