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

//
// DESCRIPTION:
// Implementation of CGwsException class
//  This class allows a chain of exceptions.  A user of this class could
// get and print each of the exception messages in the chain.  Exceptions
// have a reference count and should be released after use.
//
//  This follows somewhat the definition of FdoException.  There are some
// differences.
//  The description of the actions of GetExceptionMessage and ToString in
// FdoException indicate they return the message for the first exception
// in the chain which has a message are incorrect.  GetExceptionMessage
// return only the message for the current exception and ToString only
// returns NULL.
//  This class also returns the current exception message from these
// functions, note that FdoString is a const pointer by definition.



/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "GwsCommonImp.h"


//////////////////////////////////////////////////////////////////////
//
// class CGwsException
//
//////////////////////////////////////////////////////////////////////
CGwsException::CGwsException()
{
}

CGwsException::CGwsException(EGwsStatus stat)
: m_status (stat)
{
}

CGwsException::CGwsException(const CGwsException & other)
{
    * this = other;
}

CGwsException::CGwsException (const CGwsStatus & status)
: m_status  (status)
{
}


CGwsException::CGwsException(
    FdoString* pMessage,
    EGwsStatus type
)
:m_status (type, pMessage)
{
}


// constructor
// calls default constructor for FdoException and FdoIDisposable
// sets m_message and m_cause to null and ref count to 1
CGwsException::CGwsException(
    FdoString*      pMessage,
    EGwsStatus      type,
    FdoException*   pCause
)
:m_status (type, pMessage)
{
    SetCause (pCause);
}


CGwsException::~CGwsException()
{
    // Base class deletes string and releases the cause
    // void
}

void CGwsException::Dispose()
{
    delete this;
}


FdoString* CGwsException::GetExceptionMessage()
{
    return m_status.GetMessage ();
}

FdoString* CGwsException::ToString()
{
    return m_status.GetMessage ();
}



EGwsStatus CGwsException::GetStatus () const
{
    return m_status.GetStatus ();
}


bool CGwsException::GetParameterAt(
    unsigned int    index,
    FdoString*&     name,
    FdoString*&     value
) const
{
    return m_status.GetParameterAt (index, name, value);
}

int CGwsException::GetParameterCount () const
{
    return m_status.GetParameterCount ();
}

void CGwsException::SetParameter (FdoString *name, FdoString *value)
{
    m_status.SetParameter (name, value);
}


void CGwsException::PushStatus(const CGwsStatus& status)
{
    CGwsException * exception = new CGwsException (status);
    FdoException * rootcause = GetRootCause ();
    rootcause->SetCause (exception);
}

const CGwsException& CGwsException::operator =(const CGwsException& other)
{
    m_status = other.m_status;
    return * this;
}
