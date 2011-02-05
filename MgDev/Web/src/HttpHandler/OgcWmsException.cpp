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

#include "OgcFramework.h"

#include "OgcWmsException.h"
#include "XmlElementEmitter.h"

CPSZ MgOgcWmsException::kpszInvalidFormat          = _("InvalidFormat");
CPSZ MgOgcWmsException::kpszInvalidCRS             = _("InvalidCRS");
CPSZ MgOgcWmsException::kpszMissingCRS             = _("MissingCRS");
CPSZ MgOgcWmsException::kpszInvalidSRS             = _("InvalidSRS"); // Gotta duplicate these, for certification
CPSZ MgOgcWmsException::kpszMissingSRS             = _("MissingSRS"); //
CPSZ MgOgcWmsException::kpszLayerNotDefined        = _("LayerNotDefined");
CPSZ MgOgcWmsException::kpszStyleNotDefined        = _("StyleNotDefined");
CPSZ MgOgcWmsException::kpszLayerNotQueryable      = _("LayerNotQueryable");
CPSZ MgOgcWmsException::kpszInvalidPoint           = _("InvalidPoint");
CPSZ MgOgcWmsException::kpszCurrentUpdateSequence  = _("CurrentUpdateSequence");
CPSZ MgOgcWmsException::kpszInvalidUpdateSequence  = _("InvalidUpdateSequence");
CPSZ MgOgcWmsException::kpszMissingDimensionValue  = _("MissingDimensionValue");
CPSZ MgOgcWmsException::kpszInvalidDimensionValue  = _("InvalidDimensionValue");
CPSZ MgOgcWmsException::kpszOperationNotSupported  = _("OperationNotSupported");
CPSZ MgOgcWmsException::kpszMissingBoundingBox     = _("MissingBoundingBox");
CPSZ MgOgcWmsException::kpszInvalidBoundingBox     = _("InvalidBoundingBox");
CPSZ MgOgcWmsException::kpszMissingInfoFormat      = _("MissingInfoFormat");
CPSZ MgOgcWmsException::kpszMissingQueryLayers     = _("MissingQueryLayers");
CPSZ MgOgcWmsException::kpszMissingVersion         = _("MissingVersion");

// A catch-all for the innards being really unhealthy!
CPSZ MgOgcException::kpszInternalError          = _("InternalError");


void MgOgcWmsException::WriteTo(CStream& Response) const
{
    SZBUF szTypeAttribute[64];
    // Rudimentary buffer overrun check.
    ASSERT(szlen(this->Type()) + 8 /* "type=''" + EOS */ < char_sizeof(szTypeAttribute));

    szsprintf(szTypeAttribute,sizeof(szTypeAttribute),_("type='%s'"),this->Type());

    MgXmlElementEmitter Error(Response,this->ElementName(),szTypeAttribute);
    if(this->Message() != NULL)
        Error.Write(this->Message());
}
