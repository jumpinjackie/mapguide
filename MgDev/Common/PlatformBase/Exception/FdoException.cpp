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

#include "PlatformBase.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgFdoException, MgThirdPartyException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgFdoException object.
///
MgFdoException::MgFdoException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    MgThirdPartyException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments),
    m_nativeErrorCode(0)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgFdoException object.
///
MgFdoException::MgFdoException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments, INT64 nativeErrorCode) throw() :
    MgThirdPartyException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments), 
    m_nativeErrorCode(nativeErrorCode)
{
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgFdoException::~MgFdoException() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Get the native error code.
///
INT64 MgFdoException::GetNativeErrorCode()
{
    return m_nativeErrorCode;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serialize data to TCP/IP stream.
///
void MgFdoException::Serialize(MgStream* stream) throw()
{
    MgThirdPartyException::Serialize(stream);
    stream->WriteInt64(m_nativeErrorCode);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserialize data from TCP/IP stream.
///
void MgFdoException::Deserialize(MgStream* stream) throw()
{
    MgThirdPartyException::Deserialize(stream);
    stream->GetInt64(m_nativeErrorCode);
}
