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

#ifndef MG_OUT_OF_RANGE_EXCEPTION_H_
#define MG_OUT_OF_RANGE_EXCEPTION_H_

/// \ingroup Exceptions_Module

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when a parameter value passed to a method
/// is not in the valid range specified by its documentation.
///
/// \remarks
/// This exception is generally thrown by the collection classes when the
/// index passed to a method is less than zero, or greater or equal to the
/// number of elements in the collection.
///
class MG_FOUNDATION_API MgOutOfRangeException : public MgSystemException
{
    DECLARE_CLASSNAME(MgOutOfRangeException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgOutOfRangeException object.
    ///
    /// \param methodName
    /// Name of the method where the exception occurred.
    /// \param lineNumber
    /// Line number where the exception occurred.
    /// \param fileName
    /// File name where the exception occurred.
    /// \param whatArguments
    /// Collection of arguments used to format the message that describes what the exception is.
    /// \param whyMessageId
    /// ID of the message that describes why the exception occurs.
    /// \param whyArguments
    /// Collection of arguments used to format the message that describes why the exception occurs.
    ///
    MgOutOfRangeException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgOutOfRangeException object.
    ///
    virtual ~MgOutOfRangeException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgOutOfRangeException)

CLASS_ID:
    static const INT32 m_cls_id = Foundation_Exception_MgOutOfRangeException;

};

#endif
