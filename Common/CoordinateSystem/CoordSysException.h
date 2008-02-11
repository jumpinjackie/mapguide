//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _CEXCEPTION_H_
#define _CEXCEPTION_H_

//////////////////////////////////////////////////////////////////
///<summary>
/// This is a base class for all exceptions.
///</summary>

class COORDINATE_SYSTEM_API CException
{
    ///////////////////////////////////////////////////////
    /// Methods
public:
    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Retrieve the formatted exception message.
    /// </summary>
    /// <returns>
    /// Exception message.
    /// </returns>
    virtual STRING GetMessage() throw() = 0;

    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Retrieve the formatted exception details.
    /// </summary>
    /// <returns>
    /// Formatted exception details.
    /// </returns>
    STRING GetDetails() throw();

    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Destructor for a CException object
    ///</summary>
    ///<returns>Nothing</returns>
    virtual ~CException() throw();

protected:
    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Construct a CException object
    ///<param name="methodName">Name of the method where the exception occurred</param>
    ///<param name="lineNumber">Line number where the exception occurred</param>
    ///<param name="fileName">File name where the exception occurred</param>
    ///<param name="reason">The reason why the exception occurred</param>
    ///</summary>
    ///
    CException(CREFSTRING methodName, int lineNumber, CREFSTRING fileName, CREFSTRING reason) throw();

    ///////////////////////////////////////////////////////
    /// Member data
protected:
    STRING m_methodName;
    int m_lineNumber;
    STRING m_fileName;
    STRING m_reason;

    static const STRING CExceptionGetDetails1;
    static const STRING CExceptionGetDetails2;
    static const STRING CExceptionGetDetails3;
};

#endif
