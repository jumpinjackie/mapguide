//
//  Copyright (C) 2004-2017 by Autodesk, Inc.
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

#ifndef _MG_WFS_QUERY_ADAPTER_H_
#define _MG_WFS_QUERY_ADAPTER_H_

class MgServerFeatureService;

// This is a helper class that contains most of the boilerplate for translating WFS GetFeature request parameters
// to our feature service queries
class MgWfsQueryAdapter 
{
public:
    MgWfsQueryAdapter(MgServerFeatureService* featSvc);
    ~MgWfsQueryAdapter();
    bool SetOptions(MgResourceIdentifier* fs,
                    CREFSTRING featureClass,
                    MgStringCollection* propNames,
                    CREFSTRING srs,
                    CREFSTRING wfsFilter,
                    CREFSTRING sortCriteria);
    MgFeatureReader* GetWfsReader();
    MgCoordinateSystem* GetMapCs();
    MgCoordinateSystemTransform* GetTransform();
    MgClassDefinition* GetClassDefinition();
    INT32 GetTotal(INT32 maxFeatures);

private:
    Ptr<MgClassDefinition> m_classDef;
    Ptr<MgResourceIdentifier> m_fs;
    Ptr<MgFeatureQueryOptions> m_options;
    Ptr<MgServerFeatureService> m_featSvc;
    Ptr<MgCoordinateSystem> m_mapCs;
    Ptr<MgCoordinateSystemTransform> m_xform;
    STRING m_className;
    TransformCacheMap m_transformCache;
};

#endif