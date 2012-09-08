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

#ifndef MG_EXCEPTION_H_
#define MG_EXCEPTION_H_

/// \ingroup Exceptions_Module

class MgByteReader;
class MgStringCollection;

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Base class for exceptions.
///
class MG_FOUNDATION_API MgException : public MgSerializable
{
    DECLARE_CLASSNAME(MgException)

PUBLISHED_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieve the formatted exception message.  This is the localized and
    /// end user readable string.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetExceptionMessage() throw();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetExceptionMessage() throw();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetExceptionMessage() throw();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Localized exception message.  See MgSiteConnection for information on specifying
    /// the locale.
    ///
    /// \remarks
    /// This method used to be called GetMessage prior to MapGuide Open Source 2.2 / MapGuide Enterprise 2011 / AutoCAD Map3D 2011
    ///
    STRING GetExceptionMessage() throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieve the formatted exception details.  This includes the user readable
    /// string and the stack of method names which generated the error.  This string
    /// provides more information to web application developers than the message.
    /// If a method does not have any parameters it will be omitted from the details.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetDetails() throw();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetDetails() throw();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetDetails() throw();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Formatted exception details in current locale.  See MgSiteConnection for information on specifying
    /// the locale.
    ///
    STRING GetDetails() throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieve the formatted exception stack trace.  This includes file and line
    /// numbers, method names, and method parameters.  This string is useful for
    /// debugging !MapGuide.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetStackTrace() throw();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetStackTrace() throw();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetStackTrace() throw();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Call stack trace with method parameters
    ///
    STRING GetStackTrace() throw();

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieve the formatted exception message for the specified locale.
    ///
    /// \param locale
    /// The locale identifier to format the details text.
    /// Locale codes are 2 letters language codes defined by ISO 639-1.
    /// Please refer to  http://www.loc.gov/standards/iso639-2/englangn.html
    ///
    /// \return
    /// Formatted exception message
    ///
    virtual STRING GetExceptionMessage(CREFSTRING locale) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieve the formatted exception details for the specified locale.
    ///
    /// \param locale
    /// The locale identifier to format the details text.
    /// Locale codes are 2 letters language codes defined by ISO 639-1.
    /// Please refer to  http://www.loc.gov/standards/iso639-2/englangn.html
    ///
    /// \return
    /// Formatted exception details
    ///
    STRING GetDetails(CREFSTRING locale) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieve the formatted exception stack trace for the specified locale.
    ///
    /// \param locale
    /// The locale identifier to format the details text.
    /// Locale codes are 2 letters language codes defined by ISO 639-1.
    /// Please refer to  http://www.loc.gov/standards/iso639-2/englangn.html
    ///
    /// \return
    /// Formatted exception stack trace
    ///
    STRING GetStackTrace(CREFSTRING locale) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Add updated stack information to the exception.
    ///
    /// \param methodName
    /// Unqualified method name to add to call stack
    /// \param lineNumber
    /// Line number to add to call stack
    /// \param fileName
    /// File name to add to call stack
    ///
    void AddStackTraceInfo(CREFSTRING methodName, INT32 lineNumber, CREFSTRING fileName) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Add updated stack information to the exception.
    ///
    /// \param methodName
    /// Unqualified method name to add to call stack
    /// \param methodParams
    /// String of method parameters to add to call stack
    /// \param lineNumber
    /// Line number to add to call stack
    /// \param fileName
    /// File name to add to call stack
    ///
    void AddStackTraceInfo(CREFSTRING methodName, CREFSTRING methodParams, INT32 lineNumber, CREFSTRING fileName) throw();


    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct the object.
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgException() throw();

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

protected:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct the object.
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
    MgException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Nothing
    ///
    virtual void Dispose() throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Format a message with the specified arguments.
    /// This method should be called for critical errors when the configuration
    /// or resource file has not been loaded.
    ///
    STRING FormatMessage(CREFSTRING resourceStr,
        MgStringCollection* arguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Format a message using the specified locale.
    ///
    STRING FormatMessage(CREFSTRING locale, CREFSTRING section,
        CREFSTRING resourceId, MgStringCollection* arguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Format the details using the specified locale.
    ///
    STRING FormatDetails(CREFSTRING locale) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Format the stack trace using the specified locale.
    ///
    STRING FormatStackTrace(CREFSTRING locale) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieve the formatted exception message for the specified resource string.
    /// This method should be called for critical errors when the configuration
    /// or resource file has not been loaded.
    ///
    STRING GetExceptionMessage(CREFSTRING locale, CREFSTRING resourceStr) throw();

private:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Return currently defined locale for user, or default locale.
    ///
    STRING GetLocale() throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Add the specified method name to the collection.
    ///
    /// \param methodName
    /// Qualified method name where the exception occurred
    ///
    void AddMethodName(CREFSTRING methodName) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Add the specified method params to the collection.
    ///
    /// \param methodParams
    /// Method params where the exception occurred
    ///
    void AddMethodParams(CREFSTRING methodParams) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Add the specified line number to the collection.
    ///
    /// \param lineNumber
    /// Line number where the exception occurred
    ///
    void AddLineNumber(INT32 lineNumber) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Add the specified file name to the collection.
    ///
    /// \param fileName
    /// File name where the exception occurred
    ///
    void AddFileName(CREFSTRING fileName) throw();

protected:

    /// Collection of method names which trace the stack leading up to the exception.
    MgStringCollection m_methodNames;

    /// Collection of method parameters which trace the stack leading up to the exception.
    MgStringCollection m_methodParams;

    /// Collection of line numbers which trace the stack leading up to the exception.
    MgStringCollection m_lineNumbers;

    /// Collection of file names which trace the stack leading up to the exception.
    MgStringCollection m_fileNames;

    /// Collection of arguments used to format the message that describes what the exception is.
    MgStringCollection m_whatArguments;

    /// ID of the message that describes why the exception occurs.
    STRING m_whyMessageId;

    /// Collection of arguments used to format the message that describes why the exception occurs.
    MgStringCollection m_whyArguments;

    /// The localized message.
    STRING m_message;

    /// The localized details.
    STRING m_details;

    /// The localized stack trace.
    STRING m_stackTrace;

INTERNAL_API:

    typedef STRING (*LocaleCallbackFunc)();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Registers an application specific callback function to retrieve the
    /// current user locale.
    ///
    static void RegisterLocaleCallback(LocaleCallbackFunc func);


    DECLARE_EXCEPTION_ABSTRACT(MgException)

private:

    static LocaleCallbackFunc sm_localeCallbackFunc;

CLASS_ID:

    static const INT32 m_cls_id = Foundation_Exception_MgException;
};

#endif
