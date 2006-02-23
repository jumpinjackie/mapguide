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

#ifndef CNULLREFERENCEEXCEPTION_H_
#define CNULLREFERENCEEXCEPTION_H_

namespace CSLibrary
{
//////////////////////////////////////////////////////////////////
///<summary>
/// This exception is thrown when a null reference is encountered
/// when a non-null value is expected.
///</summary>

class COORDINATE_SYSTEM_API CNullReferenceException : public CException
{
public:
    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Construct a CNullReferenceException object
    ///<param name="methodName">Name of the method where the exception occured</param>
    ///<param name="lineNumber">Line number where the exception occured</param>
    ///<param name="fileName">File name where the exception occured</param>
    ///<param name="reason">The reason why the exception occured</param>
    ///</summary>
    ///
    CNullReferenceException(CREFSTRING methodName, int lineNumber, CREFSTRING fileName, CREFSTRING reason) throw();

    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Destructor for a CNullReferenceException object
    ///</summary>
    ///<returns>Nothing</returns>
    virtual ~CNullReferenceException() throw();

    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Retrieve the formatted exception message.
    /// </summary>
    ///<returns>
    /// Formatted message string
    ///</returns>
    virtual STRING GetMessage() throw();

private:
    static const STRING MessageText;
};

} // End of namespace

#endif
