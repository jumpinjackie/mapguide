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

#include <exception>
// The following three headers are needed to compile under Linux.
#include <stdexcept>
#include <typeinfo>
#include <iostream>

using namespace std;

IMPLEMENT_EXCEPTION_ABSTRACT(MgSystemException)

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgSystemException object.
///
MgSystemException::MgSystemException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    MgException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgSystemException::~MgSystemException() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// Create a MgException from mapping a standard CPP exception to a MgException.
///</summary>
///
MgException* MgSystemException::Create(exception& stdLibException,
    CREFSTRING methodName, INT32 lineNumber, CREFSTRING fileName) throw()
{
    // The following string conversion is used to detect whether or not
    // the given error message is unicode compliant.
    // Without this check, the server will not be able to serialize
    // this exception object correctly.

    STRING messageId;
    MgStringCollection arguments;
    bool unicodeCompliant = false;

    MG_TRY()

    STRING wcStr;

    MgUtil::MultiByteToWideChar(stdLibException.what(), wcStr);
    arguments.Add(wcStr);

    unicodeCompliant = true;

    MG_CATCH_AND_RELEASE()

    if (unicodeCompliant)
    {
        messageId = L"MgFormatInnerExceptionMessage";
    }
    else
    {
        messageId = L"MgFailedToRetrieveSystemExceptionMesage";
        arguments.Clear();
    }

    // Use typeid to determine the specific standard library exception type.
    // An equivalent MgException* is returned.
    // If typeid cannot determine the specific standard library exception type
    // a new MgUnclassifiedException is returned.

    if (typeid(stdLibException) == typeid(std::domain_error))
    {
        return new MgDomainException(methodName, lineNumber, fileName, NULL, messageId, &arguments);
    }
    else if (typeid(stdLibException) == typeid(invalid_argument))
    {
        return new MgInvalidArgumentException(methodName, lineNumber, fileName, NULL, messageId, &arguments);
    }
    else if (typeid(stdLibException) == typeid(length_error))
    {
        return new MgLengthException(methodName, lineNumber, fileName, NULL, messageId, &arguments);
    }
    else if (typeid(stdLibException) == typeid(overflow_error))
    {
        return new MgOverflowException(methodName, lineNumber, fileName, NULL, messageId, &arguments);
    }
    else if (typeid(stdLibException) == typeid(out_of_range)
          || typeid(stdLibException) == typeid(range_error))
    {
        return new MgOutOfRangeException(methodName, lineNumber, fileName, NULL, messageId, &arguments);
    }
    else if (typeid(stdLibException) == typeid(underflow_error))
    {
        return new MgUnderflowException(methodName, lineNumber, fileName, NULL, messageId, &arguments);
    }
    else if (typeid(stdLibException) == typeid(bad_alloc))
    {
        return new MgOutOfMemoryException(methodName, lineNumber, fileName, NULL, messageId, &arguments);
    }
    else if (typeid(stdLibException) == typeid(bad_cast))
    {
        return new MgInvalidCastException(methodName, lineNumber, fileName, NULL, messageId, &arguments);
    }
    else if (typeid(stdLibException) == typeid(logic_error))
    {
        return new MgLogicException(methodName, lineNumber, fileName, NULL, messageId, &arguments);
    }
    else if (typeid(stdLibException) == typeid(bad_exception)
          || typeid(stdLibException) == typeid(bad_typeid)
          || typeid(stdLibException) == typeid(ios_base::failure)
          || typeid(stdLibException) == typeid(runtime_error))
    {
        return new MgRuntimeException(methodName, lineNumber, fileName, NULL, messageId, &arguments);
    }
    else
    {
        return new MgUnclassifiedException(methodName, lineNumber, fileName, NULL, messageId, &arguments);
    }
}
