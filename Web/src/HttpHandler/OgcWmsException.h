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

#ifndef _OgcWmsException_h
#define _OgcWmsException_h

#include "OgcException.h"

class MgOgcWmsException: public MgOgcException
{
public:
    MgOgcWmsException(CPSZ pszType,CPSZ pszMessage)
    : MgOgcException(_("ServiceException"),pszType,pszMessage)
    {
    }

    void WriteTo(CStream& Response) const;

/*
Exception Code        Meaning
--------------------- -------------------------------------------------------------
InvalidFormat         Request contains a Format not offered by the server.
InvalidCRS            Request contains a CRS not offered by the server for one or
                      more of the Layers in the request.
MissingCRS            Request does not contain a required CRS (or SRS) parameter.
LayerNotDefined       GetMap request is for a Layer not offered by the server, or
                      GetFeatureInfo request is for a Layer not shown on the map.
StyleNotDefined       Request is for a Layer in a Style not offered by the server.
LayerNotQueryable     GetFeatureInfo request is applied to a Layer which is not
                      declared queryable.
InvalidPoint          GetFeatureInfo request contains invalid X or Y value.
CurrentUpdateSequence Value of (optional) UpdateSequence parameter in GetCapabilities
                      request is equal to current value of service metadata update
                      sequence number.
InvalidUpdateSequence Value of (optional) UpdateSequence parameter in GetCapabilities
                      request is greater than current value of service metadata update
                      sequence number.
MissingDimensionValue Request does not include a sample dimension value, and the server
                      did not declare a default value for that dimension.
InvalidDimensionValue Request contains an invalid sample dimension value.
OperationNotSupported Request is for an optional operation that is not supported by the
                      server.
MissingBoundingBox    Request does not contain a required bounding box parameter.
*/

    static CPSZ kpszInvalidFormat;
    static CPSZ kpszInvalidCRS;
    static CPSZ kpszMissingCRS;
    static CPSZ kpszLayerNotDefined;
    static CPSZ kpszStyleNotDefined;
    static CPSZ kpszLayerNotQueryable;
    static CPSZ kpszInvalidPoint;
    static CPSZ kpszCurrentUpdateSequence;
    static CPSZ kpszInvalidUpdateSequence;
    static CPSZ kpszMissingDimensionValue;
    static CPSZ kpszInvalidDimensionValue;
    static CPSZ kpszOperationNotSupported;
    static CPSZ kpszMissingBoundingBox;
};

#endif//_OgcWmsException_h
