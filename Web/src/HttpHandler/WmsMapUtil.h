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

#ifndef _FS_WMS_MAP_UTIL_H
#define _FS_WMS_MAP_UTIL_H

class MgWmsMapUtil
{
public:

    static MgMap* GetMap(CREFSTRING layerList, CREFSTRING bbox, CREFSTRING crs,
        INT32 width, INT32 height, MgResourceService* resourceService);

    static MgEnvelope* GetExtents(CREFSTRING bbox);

    static MgColor* GetBackgroundColor(CREFSTRING bgColor, bool transparent);

    static bool GetDocument(const wchar_t* pszDoc, REFSTRING sRet);

    static STRING SrsToWkt(CREFSTRING srs);
};

#endif  // _FS_WMS_MAP_UTIL_H
