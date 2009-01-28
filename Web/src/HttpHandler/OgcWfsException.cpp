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

#include "OgcFramework.h"

#include "OgcWfsException.h"
#include "XmlElementEmitter.h"

CPSZ MgOgcWfsException::kpszOperationNotSupported = _("OperationNotSupported");
CPSZ MgOgcWfsException::kpszUnknownParameterValue  = _("UnknownParameterValue");
CPSZ MgOgcWfsException::kpszMissingRequestParameter  = _("MissingParameterValue");
CPSZ MgOgcWfsException::kpszProcessingError  = _("RequestProcessingError");


void MgOgcWfsException::WriteTo(CStream& Response) const
{
    SZBUF szTypeAttribute[64];
    // Rudimentary buffer overrun check.
    ASSERT(szlen(this->Type()) + 8 /* "type=''" + EOS */ < char_sizeof(szTypeAttribute));

    szsprintf(szTypeAttribute,sizeof(szTypeAttribute),_("type='%s'"),this->Type());

    MgXmlElementEmitter Error(Response,this->ElementName(),szTypeAttribute);
    if(this->Message() != NULL)
        Error.Write(this->Message());
}
