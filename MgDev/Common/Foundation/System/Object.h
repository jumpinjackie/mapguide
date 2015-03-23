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

#ifndef MG_OBJECT_H_
#define MG_OBJECT_H_

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Root class of all non-exception objects
class MG_FOUNDATION_API MgObject
{
EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class.
    ///
    /// \return
    /// Class Identifier.
    ///
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the name of the class.
    ///
    /// \return
    /// Returns the name of the class.
    ///
    virtual STRING GetClassName();

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the name of the class. This is an interanl method used by
    /// SWIG to create .NET API.
    ///
    /// \return
    /// Returns the name of the class.
    ///
    virtual char* GetMultiByteClassName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the namespace of the class. This is an interanl method used by
    /// SWIG to create .NET API.
    ///
    /// \return
    /// Returns the namespace of the class.
    ///
    virtual char* GetNameSpace();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Default destructor for this class.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual ~MgObject();


    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determine if this object is of the specified class.
    ///
    /// \return
    /// true if this object is of the specified class, false otherwise.
    ///
    bool IsOfClass(INT32 classId);
};
/// \endcond

#endif
