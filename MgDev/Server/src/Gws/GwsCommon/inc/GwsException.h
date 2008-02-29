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

#ifndef CGWSEXCEPTION_H
#define CGWSEXCEPTION_H

#include "GwsStatus.h"

// Forward declarations

class CGwsException : public IGWSException
{
public:
            CGwsException   ();
            CGwsException   (EGwsStatus type);
            CGwsException   (const CGwsStatus & status);
            CGwsException   (FdoString* message, EGwsStatus type);
            CGwsException   (FdoString* message, EGwsStatus type, FdoException* cause);

    virtual ~CGwsException();

    virtual void Dispose();

    virtual const FdoString*    GetExceptionMessage();
    virtual FdoString*          ToString();
    virtual EGwsStatus          GetStatus () const;
    virtual bool                GetParameterAt(unsigned int index,
                                               FdoString*& name,
                                               FdoString*& value) const;
    virtual int                 GetParameterCount () const;
    virtual void                SetParameter (FdoString *pName, FdoString *pValue);

    virtual void                PushStatus(const CGwsStatus& status);

protected:
    CGwsStatus m_status;

private:
    // explicit
    CGwsException(const CGwsException& other);
    // explicit so we know for sure what is going on.
    const CGwsException& operator =(const CGwsException& rhs);
};

#endif
