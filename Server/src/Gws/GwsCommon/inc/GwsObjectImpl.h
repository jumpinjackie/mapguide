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

#ifndef GWSOBJECTIMPL_H
#define GWSOBJECTIMPL_H

#include "GwsCommon.h"  // for class GWSStatus

// forward definitions
class FdoXmlWriter;
class FdoXmlReader;

/// <summary>
/// Principal base class for all Gws classes.
///  - diagnostics area
///  - xml in and out
/// <summary>
class CGwsObject : public FdoXmlSaxHandler
{
protected:
    GWS_COMMON_API      CGwsObject ();
    GWS_COMMON_API      CGwsObject (const CGwsObject&);
    GWS_COMMON_API
    virtual             ~CGwsObject ();

public:
    // returns number of conditions in the diagnostics area
    GWS_COMMON_API
    int                 NumStatuses       () const;

    // returns status and message
    GWS_COMMON_API
    const               CGwsStatus &  Status  (int idx) const ;

    GWS_COMMON_API
    void                CopyStatus (const CGwsObject & other);

    // xml filing in/out
    // this methods may throw FdoXmlException
    GWS_COMMON_API
    virtual void        Write (FdoXmlWriter * writer);

    // push GwsStatus
    GWS_COMMON_API
    void                PushStatus (const CGwsStatus & stat) const;

    // push FdoException
    GWS_COMMON_API
    void                PushFdoException (const CGwsStatus & stat,
                                      FdoException * fdoEx) const;

    // remove condition
    GWS_COMMON_API
    void                RemoveStatusAt   (int idx);
    // clear diagnostics area
    GWS_COMMON_API
    void                Clear      () const;

    GWS_COMMON_API
    static bool         IsError    (EGwsStatus stat);
    GWS_COMMON_API
    static bool         IsSuccess  (EGwsStatus stat);
    GWS_COMMON_API
    static bool         IsWarning  (EGwsStatus stat);

protected:
    GWSSTATUS       m_statuses;
};

// throw gws error status
inline void eGwsOkThrow(EGwsStatus fdoes)
{
    if (CGwsObject::IsError (fdoes))
        throw fdoes;
}

#endif /* GWSOBJECTIMPL_H */
