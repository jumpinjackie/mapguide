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

#ifndef MG_SERVER_DESCRIBE_SCHEMA_H_
#define MG_SERVER_DESCRIBE_SCHEMA_H_

#include "MapGuideCommon.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
#include "Fdo.h"
#include "FSDSAX2Parser.h"
#include "FeatureServiceCache.h"

class MgCacheManager;

class MgServerDescribeSchema
{
/// Constructors/Destructor

public:

    MgServerDescribeSchema();
    ~MgServerDescribeSchema();

/// Methods

public:

    MgFeatureSchemaCollection* DescribeSchema(MgResourceIdentifier* resource,
        CREFSTRING schemaName, MgStringCollection* classNames, bool serialize = true);
    STRING DescribeSchemaAsXml(MgResourceIdentifier* resource,
        CREFSTRING schemaName, MgStringCollection* classNames);
    MgStringCollection* GetSchemas(MgResourceIdentifier* resource);
    MgStringCollection* GetClasses(MgResourceIdentifier* resource, CREFSTRING schemaName);
    MgClassDefinition*  GetClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, bool serialize = true);
    STRING SchemaToXml(MgFeatureSchemaCollection* schema);
    STRING SchemaToXml(MgFeatureSchemaCollection* schema, CREFSTRING namespacePrefix, CREFSTRING namespaceUrl);
    MgFeatureSchemaCollection* XmlToSchema(CREFSTRING xml);
    MgClassDefinitionCollection* GetIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames);

private:
    FdoFeatureSchemaCollection* DescribeFdoSchema(MgResourceIdentifier* resource,
        CREFSTRING schemaName, MgStringCollection* classNames, bool& classNameHintUsed);
    STRING GetSerializedXml(FdoFeatureSchemaCollection* fdoSchemaCol);
    STRING GetSerializedXml(FdoFeatureSchemaCollection* fdoSchemaCol, FdoXmlFlags* flags);
    bool GetIdentityProperties(CREFSTRING className,
        FdoClassCollection* classCol, MgPropertyDefinitionCollection* idProps);

    bool IsClassNameHintUsed(FdoIDescribeSchema* fdoCommand);
    MgStringCollection* GetSchemaNames(MgFeatureSchemaCollection* schemas);
    MgStringCollection* GetClassNames(MgFeatureSchemaCollection* schemas, CREFSTRING schemaName);
    MgClassDefinition* GetClassDefinition(MgFeatureSchemaCollection* schemas,
        CREFSTRING schemaName, CREFSTRING className);
    MgPropertyDefinitionCollection* GetIdentityProperties(FdoFeatureSchemaCollection* schemas,
        MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className);

    bool CheckExtendedFeatureClass(MgResourceIdentifier* resource, CREFSTRING className);
    bool CheckExtendedFeatureClasses(MgResourceIdentifier* resource, MgStringCollection* classNames);

/// Data Members

private:

    MgCacheManager* m_cacheManager;
    MgFeatureServiceCache* m_featureServiceCache;
    Ptr<MgFeatureSourceCacheItem> m_featureSourceCacheItem;
};

#endif
