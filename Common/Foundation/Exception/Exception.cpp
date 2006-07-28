//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

// To undefine GetMessage macro defined in windows
#ifdef WIN32
    #undef GetMessage
#endif

IMPLEMENT_EXCEPTION_ABSTRACT(MgException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgException.
///
MgException::MgException(CREFSTRING methodName, INT32 lineNumber,
    CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    m_whyMessageId(whyMessageId)
{
    MG_TRY()

    AddStackTraceInfo(methodName, lineNumber, fileName);

    if (NULL != whatArguments)
    {
        for (INT32 i = 0; i < whatArguments->GetCount(); ++i)
        {
            m_whatArguments.Add(whatArguments->GetItem(i));
        }
    }

    if (NULL != whyArguments)
    {
        for (INT32 i = 0; i < whyArguments->GetCount(); ++i)
        {
            m_whyArguments.Add(whyArguments->GetItem(i));
        }
    }

    MG_CATCH_AND_RELEASE()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgException::~MgException() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Dispose the object.
///
void MgException::Dispose() throw()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return currently defined locale for user, or default locale.
///
STRING MgException::GetLocale() throw()
{
    //TSW TODO:  Talked to Reva about how we want to deal with this.  We need to register a
    // callback function so we can hook this functionality at the application level.
    // If the hook isn't present, then we should fallback to the config file
    // TODO: If callbackFunctionRegistered  locale = (*callbackFunction)()

    STRING locale;

    /* TODO: Replace this stuff with a callback to the server
    MG_TRY()

    MgUserInformation* currUserInfo = MgUserInformation::GetCurrentUserInfo();

    if (NULL != currUserInfo)
    {
        locale = currUserInfo->GetLocale();
    }

    MG_CATCH_AND_RELEASE()
    */

    if (locale.empty())
    {
        MG_TRY()

        MgConfiguration* configuration = MgConfiguration::GetInstance();

        if (NULL != configuration && configuration->IsFileLoaded())
        {
            configuration->GetStringValue(
                MgFoundationConfigProperties::GeneralPropertiesSection,
                MgFoundationConfigProperties::GeneralPropertyDefaultLocale,
                locale,
                MgFoundationConfigProperties::DefaultGeneralPropertyDefaultLocale);
        }

        MG_CATCH_AND_RELEASE()

        if (locale.empty())
        {
            locale = MgResources::DefaultLocale;
        }
    }

    return locale;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Add the specified method name to the collection.
///
void MgException::AddMethodName(CREFSTRING methodName) throw()
{
    m_methodNames.Add(methodName);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Add the specified line number to the collection.
///
void MgException::AddLineNumber(INT32 lineNumber) throw()
{
    STRING str;

    MgUtil::Int32ToString(lineNumber, str);

    m_lineNumbers.Add(str);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Add the specified file name to the collection.
///
void MgException::AddFileName(CREFSTRING fileName) throw()
{
    m_fileNames.Add(fileName);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Add updated stack information to the exception.
///
void MgException::AddStackTraceInfo(CREFSTRING methodName, INT32 lineNumber,
    CREFSTRING fileName) throw()
{
    // Do not add to the stack if the same method name is already currently
    // on the stack.

    INT32 stackSize = m_methodNames.GetCount();

    if (stackSize > 0)
    {
        if (methodName != m_methodNames.GetItem(stackSize - 1))
        {
            AddMethodName(methodName);
            AddLineNumber(lineNumber);
            AddFileName(fileName);
        }
    }
    else
    {
        AddMethodName(methodName);
        AddLineNumber(lineNumber);
        AddFileName(fileName);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serialize data to TCP/IP stream.
///
void MgException::Serialize(MgStream* stream) throw()
{
    m_methodNames.Serialize(stream);
    m_lineNumbers.Serialize(stream);
    m_fileNames.Serialize(stream);

    m_whatArguments.Serialize(stream);
    stream->WriteString(m_whyMessageId);
    m_whyArguments.Serialize(stream);

    stream->WriteString(m_message);
    stream->WriteString(m_details);
    stream->WriteString(m_stackTrace);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserialize data from TCP/IP stream.
///
void MgException::Deserialize(MgStream* stream) throw()
{
    m_methodNames.Deserialize(stream);
    m_lineNumbers.Deserialize(stream);
    m_fileNames.Deserialize(stream);

    m_whatArguments.Deserialize(stream);
    stream->GetString(m_whyMessageId);
    m_whyArguments.Deserialize(stream);

    stream->GetString(m_message);
    stream->GetString(m_details);
    stream->GetString(m_stackTrace);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Format a message with the specified arguments.
/// This method should be called for critical errors when the configuration
/// or resource file has not been loaded.
///
STRING MgException::FormatMessage(CREFSTRING resourceStr,
    MgStringCollection* arguments) throw()
{
    assert(NULL != arguments);
    STRING message;

    MG_TRY()

    MgResources* resources = MgResources::GetInstance();

    if (NULL != resources)
    {
        message = resources->FormatMessage(resourceStr, arguments);
    }

    MG_CATCH_AND_RELEASE()

    return message;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Format a message using the specified locale.
///
STRING MgException::FormatMessage(CREFSTRING locale, CREFSTRING section,
    CREFSTRING resourceId, MgStringCollection* arguments) throw()
{
    STRING message;
    MgResources* resources = NULL;

    MG_TRY()

    resources = MgResources::GetInstance();

    if (NULL != resources)
    {
        // try using the supplied locale
        STRING resourceStr = resources->GetStringResource(locale, section,
            resourceId);

        message = resources->FormatMessage(resourceStr, arguments);
    }

    MG_CATCH_AND_RELEASE()

    if (message.empty() && NULL != resources && locale != MgResources::DefaultLocale)
    {
        MG_TRY()

        // try using the default locale as a backup
        STRING resourceStr = resources->GetStringResource(MgResources::DefaultLocale, section,
            resourceId);

        message = resources->FormatMessage(resourceStr, arguments);

        MG_CATCH_AND_RELEASE()
    }

    if (message.empty() && NULL != resources)
    {
        MG_TRY()

        MgStringCollection resourceArguments;
        resourceArguments.Add(resourceId);

        message = resources->FormatMessage(
            MgResources::FailedToRetrieveMessage, &resourceArguments);

        MG_CATCH_AND_RELEASE()
    }

    return message;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Format the stack trace using the specified locale.
///
STRING MgException::FormatStackTrace(CREFSTRING locale, bool includeAll) throw()
{
    STRING stackTrace;
    const INT32 stackSize = m_methodNames.GetCount();
    assert(stackSize > 0);
    assert(m_lineNumbers.GetCount() == stackSize);
    assert(m_fileNames.GetCount() == stackSize);

    if (stackSize > 0)
    {
        const INT32 startIndex = includeAll ? (stackSize - 1) : 0;

        MG_TRY()

        MgResources* resources = MgResources::GetInstance();

        if (NULL != resources)
        {
            STRING resourceId = includeAll ?
                L"MgFormatAllExceptionStackTrace" :
                L"MgFormatExceptionStackTrace";
            STRING resourceStr = resources->GetStringResource(locale,
                MgResources::ErrorDescription, resourceId);

            for (INT32 i = startIndex; i >= 0; --i)
            {
                MgStringCollection arguments;

                arguments.Add(m_methodNames.GetItem(i));
                arguments.Add(m_lineNumbers.GetItem(i));
                arguments.Add(m_fileNames.GetItem(i));

                stackTrace += resources->FormatMessage(resourceStr, &arguments);
                stackTrace += L"\n";
            }
        }

        MG_CATCH_AND_RELEASE()

        if (stackTrace.empty())
        {
            STRING methodStr = includeAll ?
                MgResources::FormatExceptionLocationMethods :
                MgResources::FormatExceptionLocationMethod;
            STRING lineStr = includeAll ?
                MgResources::FormatExceptionLocationLines :
                MgResources::FormatExceptionLocationLine;
            STRING fileStr = includeAll ?
                MgResources::FormatExceptionLocationFiles :
                MgResources::FormatExceptionLocationFile;

            for (INT32 i = startIndex; i >= 0; --i)
            {
                stackTrace += methodStr;
                stackTrace += m_methodNames.GetItem(i);
                stackTrace += lineStr;
                stackTrace += m_lineNumbers.GetItem(i);
                stackTrace += fileStr;
                stackTrace += m_fileNames.GetItem(i);
                stackTrace += L"\n";
            }
        }
    }

    return stackTrace;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieve the formatted exception message for the specified resource string.
/// This method should be called for critical errors when the configuration
/// or resource file has not been loaded.
///
STRING MgException::GetMessage(CREFSTRING locale, CREFSTRING resourceStr) throw()
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

    if (m_message.empty())
    {
        m_message = FormatMessage(resourceStr, &m_whatArguments);

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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieve the formatted exception message for the specified locale.
///
STRING MgException::GetMessage(CREFSTRING locale) throw()
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

    if (m_message.empty())
    {
        m_message = FormatMessage(locale, MgResources::Resource_Exception_Section,
            GetClassName(), &m_whatArguments);

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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieve the formatted exception details for the specified locale.
///
STRING MgException::GetDetails(CREFSTRING locale) throw()
{
    //
    // Format:
    //
    //  { Description on WHAT the exception is }
    //  [ Reason on WHY the exception occurs ]
    //  { Location of WHERE the exception is caught }
    //
    // Example:
    //
    //  Invalid argument(s):
    //      [1] = 2005-12-08T15:30
    //  The XML date time is invalid because not all of the fields were found.
    //  Exception occured in method MgDateTime.MgDateTime at line 88 in file MgDateTime.cpp
    //

    MG_TRY()

    if (m_details.empty())
    {
        m_details = GetMessage(locale);
        m_details += L"\n";
        m_details += FormatStackTrace(locale, false);
    }

    MG_CATCH_AND_RELEASE()

    return m_details;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieve the formatted exception stack trace for the specified locale.
///
STRING MgException::GetStackTrace(CREFSTRING locale) throw()
{
    //
    // Format:
    //
    //  { Locations of WHERE the exceptions are caught }
    //
    // Example:
    //
    //  - MgServerResourceService.GetResourceContent line 719 file ServerResourceService.cpp
    //  - MgApplicationRepositoryManager.GetResourceContent line 350 file ApplicationRepositoryManager.cpp
    //  - MgResourceDefinitionManager.GetResource line 624 file ResourceDefinitionManager.cpp
    //

    MG_TRY()

    if (m_stackTrace.empty())
    {
        m_stackTrace = FormatStackTrace(locale, true);
    }

    MG_CATCH_AND_RELEASE()

    return m_stackTrace;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieve the formatted exception message for the specified locale.
///
STRING MgException::GetMessage() throw()
{
    return GetMessage(GetLocale());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieve the formatted exception details for the specified locale.
///
STRING MgException::GetDetails() throw()
{
    return GetDetails(GetLocale());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieve the formatted stack trace for the specified locale.
///
STRING MgException::GetStackTrace() throw()
{
    return GetStackTrace(GetLocale());
}
