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

#ifndef WMSMAPUTILH_
#define WMSMAPUTILH_

// TODO: Rename this class, or restructure things, since it seems like
// Wfs also accesses this for the SrsToWkt mapping.
class MgWmsMapUtil
{
public:

    static MgStringCollection* GetLayerDefinitionIds(CREFSTRING layerList);

    static MgMap* GetMap(MgOgcWmsServer& wms,
        MgStringCollection* layerDefIds, CREFSTRING bbox, CREFSTRING sSRS,
        INT32 width, INT32 height, MgResourceService* resourceService, MgFeatureService* featureService, CREFSTRING sSession);

    static MgEnvelope* GetExtents(CREFSTRING bbox);

    static MgColor* GetBackgroundColor(CREFSTRING bgColor, bool transparent);

    static bool GetDocument(const wchar_t* pszDoc, REFSTRING sRet);

    // Converts srs (ostensibly an "EPSG:xxxx" string) into WKT.
    // Note: may throw an exception if underlying library doesn't have a mapping.
    static void SrsToWktMapping(MgOgcServer& oWms,STRING sSrs,REFSTRING sWkt);
    // Converts srs (ostensibly an "EPSG:xxxx" string) into WKT.
    static bool UserDefinedSrsToWktMapping(MgOgcServer& oWms,STRING sSrs,REFSTRING sWkt);
    // Handle the CRS(SRS) which have their axis orientation changed
    static void ProcessBoundingBoxAxes(STRING sSrs,REFSTRING bbox);
    // Help method to swap coordinates in boundingbox
    static void SwapCoords(double(& coord)[4]);
    // Help method to make coordinates about-turn
    static void ReverseCoords(double& coord1, double& coord2);
    // Help method to convert URN SRS(urn:ogc:def:crs:EPSG::4326) to EPSG SRS (EPSG:4326)
    static void ProcessURNSrs(REFSTRING sSRS);
    // Help method to get user defined WMS boundingbox with particular SRS
    static MgStringCollection* GetWMSlayerBoundingbox(STRING sSrs, STRING layerBounds);
};

#endif  // _FS_WMS_MAP_UTIL_H
