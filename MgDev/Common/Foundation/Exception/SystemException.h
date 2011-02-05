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

#ifndef MG_SYSTEM_EXCEPTION_H_
#define MG_SYSTEM_EXCEPTION_H_

/// \ingroup Exceptions_Module

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Base class for all exceptions representing a system error.
///
class MG_FOUNDATION_API MgSystemException : public MgException
{
EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgSystemException object.
    ///
    virtual ~MgSystemException() throw();

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Create a MgException from mapping a standard CPP exception to a MgException.
    ///
    static MgException* Create(exception& stdLibException,
        CREFSTRING methodName, INT32 lineNumber, CREFSTRING fileName) throw();

protected:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgSystemException object.
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
    MgSystemException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

INTERNAL_API:

    DECLARE_EXCEPTION_ABSTRACT(MgSystemException)

CLASS_ID:

    static const INT32 m_cls_id = Foundation_Exception_MgSystemException;
};

#endif
