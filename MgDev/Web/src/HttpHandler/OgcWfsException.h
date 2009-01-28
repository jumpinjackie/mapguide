//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _OgcWfsException_h
#define _OgcWfsException_h

#include "OgcException.h"

class MgOgcWfsException: public MgOgcException
{
public:
    MgOgcWfsException(CPSZ pszType,CPSZ pszMessage)
    : MgOgcException(_("ServiceException"),pszType,pszMessage)
    {
    }

    void WriteTo(CStream& Response) const;

/*
Exception Code        Meaning
--------------------- -------------------------------------------------------------
*/
    static CPSZ kpszOperationNotSupported;   // Requested a Request= operation that is unknown
    static CPSZ kpszUnknownParameterValue;   // Specified a parameter value that is unknown.
    static CPSZ kpszMissingRequestParameter; // A required request parameter was not specified.
    static CPSZ kpszProcessingError;         // An error occurred while processing the request
};

#endif//_OgcWfsException_h
