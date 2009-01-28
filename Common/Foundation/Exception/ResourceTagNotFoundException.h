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

/// \ingroup Exceptions_Module

#ifndef MG_RESOURCE_TAG_NOT_FOUND_EXCEPTION_H_
#define MG_RESOURCE_TAG_NOT_FOUND_EXCEPTION_H_

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when a resource tag is not found.
///
class MG_FOUNDATION_API MgResourceTagNotFoundException : public MgApplicationException
{
    DECLARE_CLASSNAME(MgResourceTagNotFoundException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgResourceTagNotFoundException object.
    ///
    /// \param methodName
    /// Name of the method where the exception occurred.
    /// \param lineNumber
    /// Line number where the exception occurred.
    /// \param fileName
    /// File name where the exception occurred.
    /// \param arguments
    /// Collection of arguments associated with the exception.
    ///
    MgResourceTagNotFoundException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct the MgResourceTagNotFoundException object.
    ///
    virtual ~MgResourceTagNotFoundException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgResourceTagNotFoundException)

CLASS_ID:

    static const INT32 m_cls_id = Foundation_Exception_MgResourceTagNotFoundException;
};

#endif
