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

#ifndef MG_AUTHENTICATION_FAILED_EXCEPTION_H_
#define MG_AUTHENTICATION_FAILED_EXCEPTION_H_

/// \ingroup Exceptions_Module

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when authentication with the MapGuide site server fails.
///
class MG_MAPGUIDE_API MgAuthenticationFailedException : public MgSystemException
{
    DECLARE_CLASSNAME(MgAuthenticationFailedException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgAuthenticationFailedException object.
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
    MgAuthenticationFailedException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgAuthenticationFailedException object.
    ///
    virtual ~MgAuthenticationFailedException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgAuthenticationFailedException)

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Exception_MgAuthenticationFailedException;
};

#endif
