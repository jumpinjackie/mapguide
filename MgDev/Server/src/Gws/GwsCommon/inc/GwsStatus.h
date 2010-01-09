//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef GWSSTATUS_H
#define GWSSTATUS_H

#include "GwsCommon.h"  // for class GWSStatus

/// <summary>
/// Principal base class for all Gws classes.
///  - diagnostics area
/// <summary>
class CGwsStatus : public GWSStatus
{
public:
    GWS_COMMON_API  CGwsStatus ();
    GWS_COMMON_API  CGwsStatus (EGwsStatus stat);
    GWS_COMMON_API  CGwsStatus (EGwsStatus stat, const WSTR & mess);
    GWS_COMMON_API  CGwsStatus (const CGwsStatus &);
    GWS_COMMON_API
    virtual         ~CGwsStatus ();

    GWS_COMMON_API
    EGwsStatus      Status  () const ;

    GWS_COMMON_API
    EGwsStatus      GetStatus  () const ;
    GWS_COMMON_API
    FdoString*      GetMessage    () const;

    // get diagnostincs parameters
    GWS_COMMON_API
    const           GwsDiagParameters & Parameters () const;

    // set diagnostics parameter
    GWS_COMMON_API
    void            SetParameter (const WSTR & name, const WSTR & value);
    GWS_COMMON_API
    void            SetParameter (FdoString *pName, FdoString *pValue);

    GWS_COMMON_API
    void            operator= (const CGwsStatus & other);
    GWS_COMMON_API
    bool            GetParameterAt(unsigned int index, FdoString*& pName, FdoString*& pValue) const;
    GWS_COMMON_API
    int             GetParameterCount () const;

protected:
    EGwsStatus          m_status;
    WSTR                m_message;
    GwsDiagParameters   m_parameters;
};


// array of CGwsStatus
typedef std::vector<CGwsStatus>     GWSSTATUS;

#endif
