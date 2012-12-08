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

#ifndef _MGCOORDINATESYSTEMGEODETICMULTIPLEREGRESSIONTRANSFORMATIONMETHOD_H_
#define _MGCOORDINATESYSTEMGEODETICMULTIPLEREGRESSIONTRANSFORMATIONMETHOD_H_

/// \defgroup MgCoordinateSystemGeodeticMultipleRegressionTransformationMethod MgCoordinateSystemGeodeticMultipleRegressionTransformationMethod
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// This class defines the currently supported transformation methods for
/// multiple regression geodetic transformations as they can be read via the
/// MgCoordinateSystemGeodeticTransformDefDictionary. That is, the constants
/// defined below are only valid for MgCoordinateSystemGeodeticTransformDef instances
/// of type MgCoordinateSystemGeodeticTransformDefType.MultipleRegression.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticMultipleRegressionTransformationMethod
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    ///  None 
    ///
    static const INT32 None = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    ///  Multiple Regression
    ///
    static const INT32 MultipleRegression =     20481; //cs_DTCMTH_MULRG

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    ///  General Polynomial EPSG 
    ///
    static const INT32 GeneralPolynomialEpsg =  20482; //cs_DTCMTH_PLYNM

};
/// \}

#endif //_MGCOORDINATESYSTEMGEODETICMULTIPLEREGRESSIONTRANSFORMATIONMETHOD_H_
