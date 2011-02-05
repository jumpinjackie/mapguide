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

#ifndef MG_FDO_EXCEPTION_H_
#define MG_FDO_EXCEPTION_H_

/// \ingroup Exceptions_Module

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when FDO encounters an error.
///
class MG_PLATFORMBASE_API MgFdoException : public MgThirdPartyException
{
    DECLARE_CLASSNAME(MgFdoException)
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieve the native error code of the underlying FDO exception.
    ///
    /// \return
    /// native error code of an fdo exception
    ///
    INT64 GetNativeErrorCode();

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgFdoException object.
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
    MgFdoException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgFdoException object.
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
    /// \param nativeErrorCode
    /// Error code indicating specific underlying database error in the FDO component, if available.
    ///
    MgFdoException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments,
        INT64 nativeErrorCode) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgFdoException object.
    ///
    virtual ~MgFdoException() throw();

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream) throw();


    DECLARE_EXCEPTION_DEFAULTS(MgFdoException)

CLASS_ID:

    static const INT32 m_cls_id = PlatformBase_Exception_MgFdoException;

private:
    INT64 m_nativeErrorCode;
};

#endif
