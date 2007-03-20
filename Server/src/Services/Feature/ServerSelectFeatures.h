//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef _MG_SERVER_SELECT_FEATURES_H_
#define _MG_SERVER_SELECT_FEATURES_H_

#include "MapGuideCommon.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
#include "Fdo.h"
#include "ServerGwsFeatureReader.h"

const STRING CLASSNAME_QUALIFIER = L":";  /* NOXLATE */

class MgFeatureQueryOptions;
class MgFeatureServiceCommand;

class MgServerSelectFeatures
{
public:
    MgServerSelectFeatures();
    ~MgServerSelectFeatures();
    MgReader* SelectFeatures(MgResourceIdentifier* resource,
                             CREFSTRING className,
                             MgFeatureQueryOptions* options,
                             bool isSelectAggregate);

private:
    void  ApplyQueryOptions(bool isSelectAggregate);
    void  ApplyClassProperties();
    void  ApplyComputedProperties();
    void  ApplyFilter();
    // void  ApplySpatialFilter();
    void  ApplyOrderingOptions();

    //bool HasCustomProperty() { return m_customPropertyFound; }
    //FdoFunction* GetCustomFunction() { return FDO_SAFE_ADDREF(m_customFunction); }
    //STRING GetCustomPropertyName() { return m_customPropertyName; }

    bool IsFdoSupportedFunction(FdoIConnection* connection, FdoFunction* fdoFunc);
    bool ContainsUdf(FdoExpression* expression);
    bool IsCustomFunction(FdoFunction* fdoFunc);
    void AddFdoComputedProperty(CREFSTRING aliasName, FdoExpression* expression);
    void AddCustomComputedProperty(CREFSTRING aliasName, FdoExpression* expression);
    void ValidateConstraintsOnCustomFunctions();
    void ApplyAggregateOptions(bool isSelectAggregate);
    void CreateCommand(MgResourceIdentifier* resource, bool isSelectAggregate);
    void ValidateParam(MgResourceIdentifier* resource, CREFSTRING className);
    void ApplyFdoGroupingProperties(MgStringCollection* propertyNames);
    bool ContainsCustomFunction(MgFeatureQueryOptions* options);
    MgReader* GetCustomReader(MgReader* reader);

    // STRING                  m_providerName;
    STRING                  className;
    Ptr<MgStringCollection> properties;
    Ptr<MgFeatureQueryOptions> m_options;
    STRING                  filterText;
    Ptr<MgFeatureServiceCommand>    m_command;

    MdfModel::FeatureSource* m_featureSource;

    // Only one custom property is supported. No nesting of custom properties allowed.
    STRING                  m_customPropertyName;
    bool                    m_customPropertyFound;
    FdoFunction*            m_customFunction;

    // Methods for Feature Join
    bool FindFeatureJoinProperties(MgResourceIdentifier* resourceId, CREFSTRING extensionName);
    MgServerGwsFeatureReader* JoinFeatures(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName, FdoFilter* filter);
    void ParseQualifiedClassName(CREFSTRING qualifiedClassName, STRING& schemaName, STRING& className);
    MgResourceIdentifier* GetSecondaryResourceIdentifier(MgResourceIdentifier* primResId, CREFSTRING extensionName, CREFSTRING relationName);

    MdfModel::FeatureSource* GetFeatureSource(MgResourceIdentifier* resource);

    // This setting limits the batch size used by the join query algorithm
    INT32 m_nJoinQueryBatchSize;
};

#endif
