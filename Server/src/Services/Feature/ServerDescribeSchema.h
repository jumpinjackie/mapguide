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

#ifndef _MG_SERVER_DESCRIBE_SCHEMA_H
#define _MG_SERVER_DESCRIBE_SCHEMA_H

#include "MapGuideCommon.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
#include "Fdo.h"
#include "FSDSAX2Parser.h"

class MgServerDescribeSchema
{
public:
    MgServerDescribeSchema();
    ~MgServerDescribeSchema();
    MgFeatureSchemaCollection* DescribeSchema(MgResourceIdentifier* resource, CREFSTRING schemaName);
    STRING DescribeSchemaAsXml(MgResourceIdentifier* resource, CREFSTRING schemaName);
    MgStringCollection* GetSchemas(MgResourceIdentifier* resource);
    MgStringCollection* GetClasses(MgResourceIdentifier* resource, CREFSTRING schemaName);
    MgClassDefinition*  GetClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className);
    STRING SchemaToXml(MgFeatureSchemaCollection* schema);
    FdoFeatureSchemaCollection* GetFdoFeatureSchemaCollection(MgFeatureSchemaCollection* mgSchemaCol);
    FdoFeatureSchema* GetFdoFeatureSchema(MgFeatureSchema* mgSchema);
    void GetFdoClassCollection(FdoClassCollection* fdoClassCol, MgClassDefinitionCollection* mgClassDefCol);

    FdoClassDefinition* GetFdoClassDefinition(MgClassDefinition* mgClassDef, FdoClassCollection* fdoClassCol);
    void GetClassProperties(FdoPropertyDefinitionCollection* fdoPropDefCol,
                            MgPropertyDefinitionCollection* propDefCol,
                            FdoClassCollection* fdoClassCol);

    void GetClassProperties(FdoDataPropertyDefinitionCollection* fdoPropDefCol,
                            MgPropertyDefinitionCollection* mgPropDefCol);
    FdoPropertyDefinition* GetFdoPropertyDefinition(MgPropertyDefinition* mgPropDef, FdoClassCollection* fdoClassCol);
    FdoDataPropertyDefinition* GetDataPropertyDefinition(MgDataPropertyDefinition* mgPropDef);
    FdoGeometricPropertyDefinition* GetGeometricPropertyDefinition(MgGeometricPropertyDefinition* mgPropDef);
    FdoRasterPropertyDefinition* GetRasterPropertyDefinition(MgRasterPropertyDefinition* mgPropDef);
    FdoDataType GetFdoDataType(INT32 awPropType);
    MgFeatureSchemaCollection* XmlToSchema(CREFSTRING xml);
    FdoObjectPropertyDefinition* GetObjectPropertyDefinition(MgObjectPropertyDefinition* objPropDef, FdoClassCollection* fdoClassCol);
    MgPropertyDefinitionCollection* GetIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className);

private:
    bool FdoClassExist(const wchar_t* name, FdoClassCollection* clsCol);
    FdoFeatureSchemaCollection* ExecuteDescribeSchema(MgResourceIdentifier* resource, CREFSTRING schemaName);
    STRING GetSerializedXml(FdoFeatureSchemaCollection* fdoSchemaCol);

    void RetrieveFeatureSource(MgResourceIdentifier* resource, string& resourceContent);
    MdfModel::FeatureSource* GetFeatureSource(MgResourceIdentifier* resource);

    MdfModel::FeatureSource* m_featureSource;

};

#endif
