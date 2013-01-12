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

#include "Layer.h"
#include "FSDSAX2Parser.h"
#include "Services/ServiceFactory.h"
#include "Services/ScrollableFeatureReader.h"

MG_IMPL_DYNCREATE(MgdLayer)

//////////////////////////////////////////////////////////////
// Creates a MgLayerBase object
//
MgdLayer::MgdLayer()
    : MgLayerBase()
{
    m_initIdProps = true;
}

//////////////////////////////////////////////////////////////
// Creates a MgLayerBase object given a reference to a LayerDefinition
//
MgdLayer::MgdLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService)
    : MgLayerBase(layerDefinition, resourceService)
{
    m_initIdProps = true;
    GetLayerInfoFromDefinition(resourceService);
}

//////////////////////////////////////////////////////////////
// Creates a MgLayerBase object given a reference to a LayerDefinition
//
MgdLayer::MgdLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService, bool initIdProps)
    : MgLayerBase(layerDefinition, resourceService)
{
    m_initIdProps = initIdProps;
    if (initIdProps)
    {
        GetLayerInfoFromDefinition(resourceService);
    }
}

//////////////////////////////////////////////////////////////////
/// Creates an MgLayerBase object from a layer definition, pulls identity properties if required, and init layer definition if required.
///
MgdLayer::MgdLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService, bool initIdProps, bool initLayerDefinition)
    : MgLayerBase(layerDefinition, resourceService, initLayerDefinition)
{
    m_initIdProps = initIdProps;
    if (initIdProps && initLayerDefinition)
    {
        GetLayerInfoFromDefinition(resourceService);
    }
}

//////////////////////////////////////////////////////////////
// Destruct a MgLayerBase object
//
MgdLayer::~MgdLayer()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return the map object.
///
MgMapBase* MgdLayer::GetMap()
{
    MgMapBase* baseMap = NULL;

    if (NULL != m_layers)
    {
        baseMap = m_layers->GetMap();
    }

    if (NULL == baseMap)
    {
        throw new MgNullReferenceException(L"MgdLayer.GetMap",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return baseMap;
}

//////////////////////////////////////////////////////////////
// Parse the layer definition XML and extracts scale ranges,
// feature source and feature class from it
//
void MgdLayer::GetLayerInfoFromDefinition(MgResourceService* resourceService)
{
    MgLayerBase::GetLayerInfoFromDefinition(resourceService);

    if(m_initIdProps && resourceService != NULL)
    {
        MG_TRY()

        // Generate Id field information for feature sources
        m_idProps.clear();
        if (!m_featureName.empty())
        {
            // If we cannot pull the identity properties, silently ignore it.
            try
            {
				/*
                //TODO: Pull site connection directly from resource service
                Ptr<MgUserInformation> userInfo = resourceService->GetUserInfo();
                Ptr<MgSiteConnection> conn = new MgSiteConnection();
                conn->Open(userInfo);
				*/
                Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
				Ptr<MgFeatureService> featureService = static_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
                Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(m_featureSourceId);

                // If the class name is fully qualified (prefixed with a schema name),
                // then use it to determine the schema name.
                STRING className;
                STRING schemaName;
                ParseFeatureName(featureService, className, schemaName);

                // Get the identity properties
                Ptr<MgStringCollection> classNames = new MgStringCollection();
                classNames->Add(className);
                Ptr<MgClassDefinitionCollection> classDefs = featureService->GetIdentityProperties(resId, schemaName, classNames);
                if (NULL != classDefs.p && classDefs->GetCount() == 1)
                {
                    Ptr<MgClassDefinition> classDef = classDefs->GetItem(0);
                    PopulateIdentityProperties(classDef);
                }
            }
            catch (MgException* e)
            {
                e->Release();
                // Do nothing here.  A failure to pull selection id's is not critical at this point
            }
        }
        MG_CATCH_AND_THROW(L"MgdLayer.GetLayerInfoFromDefinition")
    }
}

//////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgdLayer::Serialize(MgStream* stream)
{
    Ptr<MgStreamHelper> helper = stream->GetStreamHelper();
    stream->WriteObject(m_definition);

    helper->WriteString(m_name);
    helper->WriteString(m_objectId);
    helper->WriteUINT32(m_type);
    UINT8 flags = 0;
    if (m_visible) flags += 1;
    if (m_selectable) flags += 2;
    if (m_displayInLegend) flags += 4;
    if (m_expandInLegend) flags += 8;
    if (m_needRefresh) flags += 16;
    if (m_hasTooltips) flags += 32;
    helper->WriteUINT8(flags);
    helper->WriteString(m_legendLabel);
    helper->WriteBytes((const unsigned char*)&m_displayOrder, sizeof(double));

    UINT32 scaleValueCount = (UINT32) m_scaleRanges.size();
    helper->WriteUINT32(scaleValueCount);
    if(scaleValueCount > 0)
    {
        for(SCALERANGES::const_iterator it = m_scaleRanges.begin(); it != m_scaleRanges.end(); ++it)
            helper->WriteBytes((const unsigned char*)&(*it), sizeof(double));
    }
    helper->WriteString(m_featureSourceId);
    helper->WriteString(m_featureName);
    helper->WriteString(m_filter);
    helper->WriteString(m_schemaName);
    helper->WriteString(m_geometry);

    helper->WriteUINT32((UINT32)m_idProps.size());
    for (IdPropertyList::iterator ids = m_idProps.begin(); ids != m_idProps.end(); ++ids)
    {
        helper->WriteUINT16(ids->type);
        helper->WriteString(ids->name);
    }
}


//////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgdLayer::Deserialize(MgStream* stream)
{
    Ptr<MgStreamHelper> helper = stream->GetStreamHelper();

    m_definition = (MgResourceIdentifier*) stream->GetObject();

    helper->GetString(m_name);
    helper->GetString(m_objectId);
    UINT32 type;
    helper->GetUINT32(type);
    m_type = type;
    UINT8 flags = 0;
    helper->GetUINT8(flags);
    m_visible = (flags & 1) > 0;
    m_selectable = (flags & 2) > 0;
    m_displayInLegend = (flags & 4) > 0;
    m_expandInLegend = (flags & 8) > 0;
    m_needRefresh = (flags & 16) > 0;
    m_hasTooltips = (flags & 32) > 0;

    helper->GetString(m_legendLabel);
    helper->GetData((void*)&m_displayOrder, sizeof(double));

    UINT32 scaleValueCount = 0;
    helper->GetUINT32(scaleValueCount);
    for(UINT32 i = 0; i < scaleValueCount; i++)
    {
        double scaleValue;
        helper->GetData((void*)&scaleValue, sizeof(double));
        m_scaleRanges.push_back(scaleValue);
    }

    helper->GetString(m_featureSourceId);
    helper->GetString(m_featureName);
    helper->GetString(m_filter);
    helper->GetString(m_schemaName);
    helper->GetString(m_geometry);

    UINT32 idCount = 0;
    helper->GetUINT32(idCount);
    for (UINT32 n = 0; n < idCount; n++)
    {
        IdProperty idProp;
        UINT16 idType;
        helper->GetUINT16(idType);
        idProp.type = idType;
        helper->GetString(idProp.name);
        m_idProps.push_back(idProp);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class definition for the feature class of the layer.  If the
/// feature class of the layer is extended with properties from other feature
/// classes, then all those properties are also contained in the returned
/// class definition.
///
MgClassDefinition* MgdLayer::GetClassDefinition()
{
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);
    STRING schemaName;
    STRING className;
    MgUtil::ParseQualifiedClassName(m_featureName, schemaName, className);
    return featureService->GetClassDefinition(resourceId, schemaName, className);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Selects features from a feature source according to the
/// criteria set in the MgFeatureQueryOptions argument The
/// criteria are applied to all of the features in the feature
/// source. If you want to apply the criteria to a subset of the
/// features, use the MgFeatureService::SelectAggregate Method.
///
MgFeatureReader* MgdLayer::SelectFeatures(MgFeatureQueryOptions* options)
{
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);

    return featureService->SelectFeatures(resourceId, m_featureName, options);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Selects groups of features from a feature source and applies
/// filters to each of the groups according to the criteria set
/// in the MgFeatureAggregateOptions argument. If you want to
/// apply the criteria to all features without grouping them, use
/// the MgFeatureService::SelectFeatures Method.
///
MgDataReader* MgdLayer::SelectAggregate(MgFeatureAggregateOptions* options)
{
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);

    return featureService->SelectAggregate(resourceId, m_featureName, options);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Executes the MgDeleteFeatures, MgInsertFeatures,
/// MgUpdateFeatures, MgLockFeatures or MgUnlockFeatures commands
/// contained in the given MgFeatureCommandCollection object.
///
MgPropertyCollection* MgdLayer::UpdateFeatures(MgFeatureCommandCollection* commands)
{
    Ptr<MgPropertyCollection> propCol;

    MG_TRY()

    // Get the content of the feature source.
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
        GetMap()->GetService(MgServiceType::ResourceService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);

    Ptr<MgByteReader> byteReader = resourceService->GetResourceContent(resourceId);
    string xmlContent;
    byteReader->ToStringUtf8(xmlContent);

    // Create the feature source object.
    MdfParser::FSDSAX2Parser parser;
    parser.ParseString(xmlContent.c_str(), xmlContent.length() * sizeof(char));

    if (!parser.GetSucceeded())
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);
        //TODO: MgdInvalidFeatureSourceException
		throw new MgFeatureServiceException(L"MgdLayer::UpdateFeatures", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    auto_ptr<MdfModel::FeatureSource> featureSource;
    featureSource.reset(parser.DetachFeatureSource());

    // Get the provider name.
    STRING providerName = (STRING)featureSource->GetProvider();

    // Determine if the provider supports transactions.
    bool useTransaction = false;
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));

    byteReader = featureService->GetCapabilities(providerName);
    xmlContent.clear();
    byteReader->ToStringUtf8(xmlContent);

    const string openTag = "<SupportsTransactions>"; //NOXLATE
    const string closeTag = "</SupportsTransactions>"; //NOXLATE
    size_t startIndex = xmlContent.find(openTag);

    if (string::npos != startIndex)
    {
        size_t endIndex = xmlContent.find(closeTag, startIndex);

        if (string::npos == endIndex)
        {
            throw new MgXmlParserException(L"MgdLayer.UpdateFeatures",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            string tagValue = xmlContent.substr(startIndex + openTag.length(),
                endIndex - startIndex - openTag.length());
            useTransaction = MgUtil::StringToBoolean(tagValue);
        }
    }

    propCol = featureService->UpdateFeatures(resourceId, commands, useTransaction);

    MG_CATCH_AND_THROW(L"MgdLayer.UpdateFeatures")

    return propCol.Detach();
}

MgTransaction* MgdLayer::BeginTransaction()
{
    Ptr<MgTransaction> trans;

    MG_TRY()

    Ptr<MgdFeatureService> featSvc = static_cast<MgdFeatureService*>(GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(GetFeatureSourceId());

    trans = featSvc->BeginTransaction(fsId);

    MG_CATCH_AND_THROW(L"MgdLayer.BeginTransaction")

    return trans.Detach();
}

MgdScrollableFeatureReader* MgdLayer::SelectFeaturesExtended(MgFeatureQueryOptions* options)
{
    Ptr<MgdScrollableFeatureReader> reader;

    MG_TRY()

    Ptr<MgdFeatureService> featSvc = static_cast<MgdFeatureService*>(GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(GetFeatureSourceId());

    reader = featSvc->SelectFeaturesExtended(fsId, GetFeatureClassName(), options);

    MG_CATCH_AND_THROW(L"MgdLayer.SelectFeaturesExtended")

    return reader.Detach();
}

MgSpatialContextReader* MgdLayer::GetSpatialContexts(bool active)
{
	Ptr<MgSpatialContextReader> reader;

    MG_TRY()

    Ptr<MgdFeatureService> featSvc = static_cast<MgdFeatureService*>(GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(GetFeatureSourceId());

    reader = featSvc->GetSpatialContexts(fsId, active);

    MG_CATCH_AND_THROW(L"MgdLayer.GetSpatialContexts")

    return reader.Detach();
}

MgFeatureReader* MgdLayer::InsertFeatures(MgPropertyCollection* propertyValues)
{
    Ptr<MgFeatureReader> reader;

    MG_TRY()

    Ptr<MgdFeatureService> featSvc = static_cast<MgdFeatureService*>(GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(GetFeatureSourceId());

    reader = featSvc->InsertFeatures(fsId, GetFeatureClassName(), propertyValues);

    MG_CATCH_AND_THROW(L"MgdLayer.InsertFeatures")

    return reader.Detach();
}

int MgdLayer::UpdateFeatures(MgPropertyCollection* propertyValues, CREFSTRING filter)
{
    int updated = 0;

    MG_TRY()

    Ptr<MgdFeatureService> featSvc = static_cast<MgdFeatureService*>(GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(GetFeatureSourceId());

    updated = featSvc->UpdateFeatures(fsId, GetFeatureClassName(), propertyValues, filter);

    MG_CATCH_AND_THROW(L"MgdLayer.UpdateFeatures")

    return updated;
}

int MgdLayer::DeleteFeatures(CREFSTRING filter)
{
    int deleted = 0;

    MG_TRY()

    Ptr<MgdFeatureService> featSvc = static_cast<MgdFeatureService*>(GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(GetFeatureSourceId());

    deleted = featSvc->DeleteFeatures(fsId, GetFeatureClassName(), filter);

    MG_CATCH_AND_THROW(L"MgdLayer.DeleteFeatures")

    return deleted;
}

MgFeatureReader* MgdLayer::InsertFeatures(MgPropertyCollection* propertyValues, MgTransaction* trans)
{
    Ptr<MgFeatureReader> reader;

    MG_TRY()

    Ptr<MgdFeatureService> featSvc = static_cast<MgdFeatureService*>(GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(GetFeatureSourceId());

    reader = featSvc->InsertFeatures(fsId, GetFeatureClassName(), propertyValues);

    MG_CATCH_AND_THROW(L"MgdLayer.InsertFeatures")

    return reader.Detach();
}

int MgdLayer::UpdateFeatures(MgPropertyCollection* propertyValues, CREFSTRING filter, MgTransaction* trans)
{
    int updated = 0;

    MG_TRY()

    Ptr<MgdFeatureService> featSvc = static_cast<MgdFeatureService*>(GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(GetFeatureSourceId());

    updated = featSvc->UpdateFeatures(fsId, GetFeatureClassName(), propertyValues, filter, trans);

    MG_CATCH_AND_THROW(L"MgdLayer.UpdateFeatures")

    return updated;
}

int MgdLayer::DeleteFeatures(CREFSTRING filter, MgTransaction* trans)
{
    int deleted = 0;

    MG_TRY()

    Ptr<MgdFeatureService> featSvc = static_cast<MgdFeatureService*>(GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(GetFeatureSourceId());

    deleted = featSvc->DeleteFeatures(fsId, GetFeatureClassName(), filter, trans);

    MG_CATCH_AND_THROW(L"MgdLayer.DeleteFeatures")

    return deleted;
}

void MgdLayer::ParseFeatureName(MgFeatureService* featureService, REFSTRING className, REFSTRING schemaName)
{
    // If the class name is fully qualified (prefixed with a schema name),
    // then use it to determine the schema name.
    MgUtil::ParseQualifiedClassName(m_featureName, schemaName, className);
    if (!schemaName.empty())
    {
        m_schemaName = schemaName;
    }
    else
    {
        // non-qualified, pull schema from Fdo
        if (m_schemaName.empty())
        {
            //TODO:  How do we deal with different schemas?  Just use first one for now...
            Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(m_featureSourceId);
			if (resId->GetResourceType().compare(L"FeatureSource") == 0) //MgResourceType::FeatureSource gives LNK2001 (huh?)
            {
                try
                {
                    Ptr<MgStringCollection> schemaNames = featureService->GetSchemas(resId);
                    m_schemaName = schemaNames->GetItem(0);
                }
                catch (MgException* e)
                {
                    e->Release();
                }
            }
        }
    }
    schemaName = m_schemaName;
}

void MgdLayer::PopulateIdentityProperties(MgClassDefinition* classDef)
{
    Ptr<MgPropertyDefinitionCollection> idProps = classDef->GetIdentityProperties();

    assert(idProps != NULL);
    for (int nIds=0; nIds<idProps->GetCount(); nIds++)
    {
        Ptr<MgDataPropertyDefinition> prop = dynamic_cast<MgDataPropertyDefinition*>(idProps->GetItem(nIds));
        if (0 == (MgDataPropertyDefinition*)prop)
        {
            throw new MgInvalidCastException(L"MgdLayer.PopulateIdentityProperties",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        MgdLayer::IdProperty idProp;
        idProp.type = prop->GetDataType();
        idProp.name = prop->GetName();
        m_idProps.push_back(idProp);
    }
}

bool MgdLayer::IsPotentiallyVisibleAtScale(double scale, bool bConsiderParentGroupVisibility)
{
    //check visibility flags
    Ptr<MgLayerGroup> group = GetGroup();

    if(group && !group->IsVisible() && bConsiderParentGroupVisibility)
        return false;

    //check scale ranges
    if(m_scaleRanges.size() % 2)
        return false;   //something is wrong with the scale ranges...

    for(SCALERANGES::const_iterator it = m_scaleRanges.begin(); it != m_scaleRanges.end(); )
    {
        double minScale = *it ++;
        double maxScale = *it ++;

        if(scale >= minScale && scale < maxScale)
            return true;
    }
    return false;
}

bool MgdLayer::HasTooltips()
{
    return MgLayerBase::HasTooltips();
}