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

#ifndef _CCOORDINATE_SYSTEM_COMPUTATION_FAILED_EXCEPTION_H_
#define _CCOORDINATE_SYSTEM_COMPUTATION_FAILED_EXCEPTION_H_

namespace CSLibrary
{
//////////////////////////////////////////////////////////////////
///<summary>
/// The exception is thrown when the coordinate system computation
/// fails.
///</summary>
class COORDINATE_SYSTEM_API CCoordinateSystemComputationFailedException : public CException
{
    ///////////////////////////////////////////////////////
    /// Methods
public:
    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Construct a CCoordinateSystemComputationFailedException object
    ///<param name="methodName">Name of the method where the exception occured</param>
    ///<param name="lineNumber">Line number where the exception occured</param>
    ///<param name="fileName">File name where the exception occured</param>
    ///</summary>
    ///
    CCoordinateSystemComputationFailedException(CREFSTRING methodName, int lineNumber, CREFSTRING fileName, CREFSTRING reason) throw();

    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Destructor for a CCoordinateSystemConversionFailedException object
    ///</summary>
    ///<returns>Nothing</returns>
    virtual ~CCoordinateSystemComputationFailedException() throw();

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
