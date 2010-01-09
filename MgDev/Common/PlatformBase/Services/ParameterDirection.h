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

#ifndef MGPARAMETERDIRECTION_H
#define MGPARAMETERDIRECTION_H

/// \defgroup MgParameterDirection MgParameterDirection
/// \ingroup Feature_Service_classes
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// The MgParameterDirection defines integer constants used to signify the 
/// direction in which a parameter value will be used within the context of a 
/// FDO Command.
class MG_FOUNDATION_API MgParameterDirection
{
PUBLISHED_API:
    ///////////////////////////////////////////////////
    /// \brief
    /// Specifies that the parameter is an input parameter.
    ///
    static const INT32 Input = 0;

    ///////////////////////////////////////////////////
    /// \brief
    /// Specifies that the parameter is an output parameter.
    ///
    static const INT32 Output = 1;

    ///////////////////////////////////////////////////
    /// \brief
    /// Specifies that the parameter is an input & output parameter.
    ///
    static const INT32 InputOutput = 2;

    ///////////////////////////////////////////////////
    /// \brief
    /// Specifies that the parameter is a return parameter.
    ///
    static const INT32 Ret = 3;
};
/// \}

#endif
