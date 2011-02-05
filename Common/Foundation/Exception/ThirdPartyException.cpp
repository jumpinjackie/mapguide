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

#include "Foundation.h"

IMPLEMENT_EXCEPTION_ABSTRACT(MgThirdPartyException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgThirdPartyException object.
///
MgThirdPartyException::MgThirdPartyException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    MgException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments),
    m_errorCode(0)
{
    // The following string conversion is used to detect whether or not
    // the given error message is unicode compliant.
    // Without this check, the server will not be able to serialize
    // this exception object correctly.

    bool unicodeCompliant = false;

    MG_TRY()

    string mbStr;

    for (INT32 i = 0; i < m_whyArguments.GetCount(); ++i)
    {
        MgUtil::WideCharToMultiByte(m_whyArguments.GetItem(i), mbStr);
    }

    unicodeCompliant = true;

    MG_CATCH_AND_RELEASE()

    if (!unicodeCompliant)
    {
        m_whyMessageId = L"MgFailedToRetrieveThirdPartyExceptionMesage";
        m_whyArguments.Clear();
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgThirdPartyException::~MgThirdPartyException() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Get the error code.
///
INT32 MgThirdPartyException::GetErrorCode() throw()
{
    return m_errorCode;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set the error code.
///
void MgThirdPartyException::SetErrorCode(INT32 errorCode) throw()
{
    m_errorCode = errorCode;
}
