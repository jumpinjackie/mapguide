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

IMPLEMENT_EXCEPTION_DEFAULTS(MgConfigurationLoadFailedException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgConfigurationLoadFailedException object.
///
MgConfigurationLoadFailedException::MgConfigurationLoadFailedException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    MgApplicationException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgConfigurationLoadFailedException::~MgConfigurationLoadFailedException() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieve the formatted exception message for the specified locale.
///
STRING MgConfigurationLoadFailedException::GetExceptionMessage(CREFSTRING locale) throw()
{
    return MgException::GetExceptionMessage(locale, MgResources::FailedToLoadConfigurationFile);
}
