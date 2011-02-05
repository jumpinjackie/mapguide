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

IMPLEMENT_EXCEPTION_DEFAULTS(MgInvalidArgumentException, MgSystemException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgInvalidArgumentException object.
///
MgInvalidArgumentException::MgInvalidArgumentException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    MgSystemException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgInvalidArgumentException::~MgInvalidArgumentException() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// Retrieve the formatted exception message for the specified locale.
/// <param name="nLocale">The locale identifier to format the message text.</param>
///
///
STRING MgInvalidArgumentException::GetExceptionMessage(CREFSTRING locale) throw()
{
    //
    // Format:
    //
    //  { Description on WHAT the exception is }
    //  [ Reason on WHY the exception occurs ]
    //
    // Example:
    //
    //  Invalid argument(s):
    //      [1] = 2005-12-08T15:30
    //  The XML date time is invalid because not all of the fields were found.
    //

    MG_TRY()

    // Note that for any empty string argument, MgResources::BlankArgument
    // must be used to prevent MgProperty::SetName (via MgStringCollection::Add)
    // from infinitely throwing the MgInvalidArgumentException.

    if (m_message.empty())
    {
        m_message = FormatMessage(locale, MgResources::Resource_Exception_Section,
            GetClassName(), NULL);

        const INT32 numArgsPerLine = 2;
        const INT32 size = m_whatArguments.GetCount();
        assert(0 == (size % numArgsPerLine));

        for (INT32 i = 1; i < size; i += numArgsPerLine)
        {
            STRING argIndex = m_whatArguments.GetItem(i-1);
            STRING argValue = m_whatArguments.GetItem(i);

            m_message += L"\n\t[";
            m_message += argIndex;
            m_message += L"] = \"";
            m_message += argValue;
            m_message += L"\"";
        }

        if (!m_whyMessageId.empty())
        {
            m_message += L"\n";
            m_message += FormatMessage(locale, MgResources::ErrorDescription,
                m_whyMessageId, &m_whyArguments);
        }
    }

    MG_CATCH_AND_RELEASE()

    return m_message;
}
