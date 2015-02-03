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

#include "MapGuideCommon.h"
#include "FSDSAX2Parser.h"
#include "AreaTypeStyle.h"
#include "LineTypeStyle.h"
#include "PointTypeStyle.h"
#include "CompositeTypeStyle.h"
#include <set>

MG_IMPL_DYNCREATE(MgLayer)

//////////////////////////////////////////////////////////////
// Creates a MgLayerBase object
//
MgLayer::MgLayer()
    : MgLayerBase()
{
    m_initIdProps = true;
}

//////////////////////////////////////////////////////////////
// Creates a MgLayerBase object given a reference to a LayerDefinition
//
MgLayer::MgLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService)
    : MgLayerBase(layerDefinition, resourceService)
{
    m_initIdProps = true;
    GetLayerInfoFromDefinition(resourceService);
}

//////////////////////////////////////////////////////////////
// Creates a MgLayerBase object given a reference to a LayerDefinition
//
MgLayer::MgLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService, bool initIdProps)
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
MgLayer::MgLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService, bool initIdProps, bool initLayerDefinition)
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
MgLayer::~MgLayer()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return the map object.
///
MgMapBase* MgLayer::GetMap()
{
    MgMapBase* baseMap = NULL;

    if (NULL != m_layers)
    {
        baseMap = m_layers->GetMap();
    }

    CHECKNULL(baseMap, L"MgLayer.GetMap");

    return baseMap;
}

//////////////////////////////////////////////////////////////
// Parse the layer definition XML and extracts scale ranges,
// feature source and feature class from it
//
void MgLayer::GetLayerInfoFromDefinition(MgResourceService* resourceService)
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
                //TODO: Pull site connection directly from resource service
                Ptr<MgUserInformation> userInfo = resourceService->GetUserInfo();
                Ptr<MgSiteConnection> conn = new MgSiteConnection();
                conn->Open(userInfo);

                Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(conn->CreateService(MgServiceType::FeatureService));
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
        MG_CATCH_AND_THROW(L"MgLayer.GetLayerInfoFromDefinition")
    }
}

//////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgLayer::Serialize(MgStream* stream)
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
void MgLayer::Deserialize(MgStream* stream)
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
MgClassDefinition* MgLayer::GetClassDefinition()
{
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);

    return featureService->GetClassDefinition(resourceId, m_schemaName, m_featureName);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Selects features from a feature source according to the
/// criteria set in the MgFeatureQueryOptions argument The
/// criteria are applied to all of the features in the feature
/// source. If you want to apply the criteria to a subset of the
/// features, use the MgFeatureService::SelectAggregate Method.
///
MgFeatureReader* MgLayer::SelectFeatures(MgFeatureQueryOptions* options)
{
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);

    return featureService->SelectFeatures(resourceId, m_featureName, options);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Selects features from a feature source according to the
/// criteria set in the MgFeatureQueryOptions argument The
/// criteria are applied to all of the features in the feature
/// source. If you want to apply the criteria to a subset of the
/// features, use the MgFeatureService::SelectAggregate Method.
///
MgFeatureReader* MgLayer::SelectFeatures(MgFeatureQueryOptions* options, CREFSTRING coordinateSystem)
{
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);

    return featureService->SelectFeatures(resourceId, m_featureName, options, coordinateSystem);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Selects groups of features from a feature source and applies
/// filters to each of the groups according to the criteria set
/// in the MgFeatureAggregateOptions argument. If you want to
/// apply the criteria to all features without grouping them, use
/// the MgFeatureService::SelectFeatures Method.
///
MgDataReader* MgLayer::SelectAggregate(MgFeatureAggregateOptions* options)
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
MgPropertyCollection* MgLayer::UpdateFeatures(MgFeatureCommandCollection* commands)
{
    Ptr<MgPropertyCollection> propCol;

    MG_TRY()

    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);
    STRING providerName = GetProviderName();
    // Determine if the provider supports transactions.
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));

    bool useTransaction = UseTransaction(featureService, providerName);

    propCol = featureService->UpdateFeatures(resourceId, commands, useTransaction);

    MG_CATCH_AND_THROW(L"MgLayer.UpdateFeatures")

    return propCol.Detach();
}

STRING MgLayer::GetProviderName()
{
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
        throw new MgInvalidFeatureSourceException(L"MgLayer::UpdateFeatures", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    auto_ptr<MdfModel::FeatureSource> featureSource;
    featureSource.reset(parser.DetachFeatureSource());

    // Get the provider name.
    return (STRING)featureSource->GetProvider();
}

bool MgLayer::UseTransaction(MgFeatureService* featureService, CREFSTRING providerName)
{
    bool useTransaction = false;
    Ptr<MgByteReader> byteReader = featureService->GetCapabilities(providerName);
    std::string xmlContent;
    byteReader->ToStringUtf8(xmlContent);

    const string openTag = "<SupportsTransactions>"; //NOXLATE
    const string closeTag = "</SupportsTransactions>"; //NOXLATE
    size_t startIndex = xmlContent.find(openTag);

    if (string::npos != startIndex)
    {
        size_t endIndex = xmlContent.find(closeTag, startIndex);

        if (string::npos == endIndex)
        {
            throw new MgXmlParserException(L"MgLayer.UpdateFeatures",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            string tagValue = xmlContent.substr(startIndex + openTag.length(),
                endIndex - startIndex - openTag.length());
            useTransaction = MgUtil::StringToBoolean(tagValue);
        }
    }
    return useTransaction;
}

void MgLayer::ParseFeatureName(MgFeatureService* featureService, REFSTRING className, REFSTRING schemaName)
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
            if (resId->GetResourceType().compare(MgResourceType::FeatureSource) == 0)
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

void MgLayer::PopulateIdentityProperties(MgClassDefinition* classDef)
{
    Ptr<MgPropertyDefinitionCollection> idProps = classDef->GetIdentityProperties();

    assert(idProps != NULL);
    for (int nIds=0; nIds<idProps->GetCount(); nIds++)
    {
        Ptr<MgDataPropertyDefinition> prop = dynamic_cast<MgDataPropertyDefinition*>(idProps->GetItem(nIds));
        if (0 == (MgDataPropertyDefinition*)prop)
        {
            throw new MgInvalidCastException(L"MgLayer.PopulateIdentityProperties",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        MgLayer::IdProperty idProp;
        idProp.type = prop->GetDataType();
        idProp.name = prop->GetName();
        m_idProps.push_back(idProp);
    }
}

MgTransaction* MgLayer::BeginTransaction()
{
    Ptr<MgTransaction> ret;

    MG_TRY()

    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));

    ret = featureService->BeginTransaction(resourceId);

    MG_CATCH_AND_THROW(L"MgLayer.BeginTransaction")

    return ret.Detach();
}

MgPropertyCollection* MgLayer::UpdateFeatures(MgFeatureCommandCollection* commands, MgTransaction* transaction)
{
    Ptr<MgPropertyCollection> propCol;

    MG_TRY()

    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);
    // Determine if the provider supports transactions.
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));

    propCol = featureService->UpdateFeatures(resourceId, commands, transaction);

    MG_CATCH_AND_THROW(L"MgLayer.UpdateFeatures")

    return propCol.Detach();
}

MgFeatureReader* MgLayer::InsertFeatures(MgPropertyCollection* properties, MgTransaction* transaction)
{
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);

    Ptr<MgFeatureReader> ret;
    MG_TRY()

    ret = featureService->InsertFeatures(resourceId, m_featureName, properties, transaction);

    MG_CATCH_AND_THROW(L"MgLayer.InsertFeatures")

    return ret.Detach();
}

MgFeatureReader* MgLayer::InsertFeatures(MgBatchPropertyCollection* properties, MgTransaction* transaction)
{
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);

    Ptr<MgFeatureReader> ret;
    MG_TRY()

    ret = featureService->InsertFeatures(resourceId, m_featureName, properties, transaction);

    MG_CATCH_AND_THROW(L"MgLayer.InsertFeatures")

    return ret.Detach();
}

INT32 MgLayer::UpdateMatchingFeatures(MgPropertyCollection* properties, CREFSTRING filter, MgTransaction* transaction)
{
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);

    INT32 updated = -1;
    MG_TRY()

    updated = featureService->UpdateMatchingFeatures(resourceId, m_featureName, properties, filter, transaction);
    
    MG_CATCH_AND_THROW(L"MgLayer.UpdateMatchingFeatures")

    return updated;
}

INT32 MgLayer::DeleteFeatures(CREFSTRING filter, MgTransaction* transaction)
{
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(m_featureSourceId);

    INT32 deleted = -1;
    MG_TRY()

    deleted = featureService->DeleteFeatures(resourceId, m_featureName, filter, transaction);

    MG_CATCH_AND_THROW(L"MgLayer.DeleteFeatures")

    return deleted;
}

MgIntCollection* MgLayer::GetGeometryTypeStyles()
{
    Ptr<MgIntCollection> ret;

    MG_TRY()

    ret = GetGeometryTypeStyles(GetMap()->GetViewScale());

    MG_CATCH_AND_THROW(L"MgLayer.GetGeometryTypeStyles");

    return ret.Detach();
}

INT32 MgLayer::GetThemeCategoryCount(INT32 geomType)
{
    return GetThemeCategoryCount(GetMap()->GetViewScale(), geomType);
}

MgByteReader* MgLayer::GenerateLegendImage(INT32 width, INT32 height, CREFSTRING format, INT32 geomType, INT32 themeCategory)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    ret = GenerateLegendImage(GetMap()->GetViewScale(), width, height, format, geomType, themeCategory);

    MG_CATCH_AND_THROW(L"MgLayer.GenerateLegendImage");

    return ret.Detach();
}

MgIntCollection* MgLayer::GetGeometryTypeStyles(double scale)
{
    Ptr<MgIntCollection> ret;

    MG_TRY()

    Ptr<MgResourceService> resSvc = dynamic_cast<MgResourceService*>(GetMap()->GetService(MgServiceType::ResourceService));
    std::auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(resSvc, m_definition));
    if (ldf.get() != NULL)
    {
        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
        if(vl != NULL)
        {
            MdfModel::VectorScaleRangeCollection* scaleRanges = vl->GetScaleRanges();
            if (scaleRanges != NULL)
            {
                for (INT32 i = 0; i < scaleRanges->GetCount(); i++)
                {
                    MdfModel::VectorScaleRange* vsr = scaleRanges->GetAt(i);
                    if (scale >= vsr->GetMinScale() && scale < vsr->GetMaxScale())
                    {
                        MdfModel::FeatureTypeStyleCollection* ftsc = vsr->GetFeatureTypeStyles();

                        ret = new MgIntCollection();

                        for (INT32 j = 0; j < ftsc->GetCount(); j++)
                        {
                            MdfModel::PointTypeStyle* pts = dynamic_cast<MdfModel::PointTypeStyle*>(ftsc->GetAt(j));
                            MdfModel::LineTypeStyle* lts = dynamic_cast<MdfModel::LineTypeStyle*>(ftsc->GetAt(j));
                            MdfModel::AreaTypeStyle* ats = dynamic_cast<MdfModel::AreaTypeStyle*>(ftsc->GetAt(j));
                            MdfModel::CompositeTypeStyle* cts = dynamic_cast<MdfModel::CompositeTypeStyle*>(ftsc->GetAt(j));

                            if (pts != NULL)
                            {
                                if (ret->IndexOf(1) < 0)
                                    ret->Add(1);
                            }
                            else if (lts != NULL)
                            {
                                if (ret->IndexOf(2) < 0)
                                    ret->Add(2);
                            }
                            else if (ats != NULL)
                            {
                                if (ret->IndexOf(3) < 0)
                                    ret->Add(3);
                            }
                            else if (cts != NULL)
                            {
                                ret->Add(4);
                            }
                        }
                    }
                }
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgLayer.GenerateLegendImage");

    return ret.Detach();
}

INT32 MgLayer::GetThemeCategoryCount(double scale, INT32 geomType)
{
    INT32 ret = -1;

    Ptr<MgResourceService> resSvc = dynamic_cast<MgResourceService*>(GetMap()->GetService(MgServiceType::ResourceService));
    std::auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(resSvc, m_definition));
    if (ldf.get() != NULL)
    {
        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
        if(vl != NULL)
        {
            MdfModel::VectorScaleRangeCollection* scaleRanges = vl->GetScaleRanges();
            if (scaleRanges != NULL)
            {
                for (INT32 i = 0; i < scaleRanges->GetCount(); i++)
                {
                    MdfModel::VectorScaleRange* vsr = scaleRanges->GetAt(i);
                    if (scale >= vsr->GetMinScale() && scale < vsr->GetMaxScale())
                    {
                        MdfModel::FeatureTypeStyleCollection* ftsc = vsr->GetFeatureTypeStyles();
                        for (INT32 j = 0; j < ftsc->GetCount(); j++)
                        {
                            MdfModel::PointTypeStyle* pts = dynamic_cast<MdfModel::PointTypeStyle*>(ftsc->GetAt(j));
                            MdfModel::LineTypeStyle* lts = dynamic_cast<MdfModel::LineTypeStyle*>(ftsc->GetAt(j));
                            MdfModel::AreaTypeStyle* ats = dynamic_cast<MdfModel::AreaTypeStyle*>(ftsc->GetAt(j));
                            MdfModel::CompositeTypeStyle* cts = dynamic_cast<MdfModel::CompositeTypeStyle*>(ftsc->GetAt(j));

                            if (pts != NULL && geomType == 1)
                            {
                                ret = pts->GetRules()->GetCount();
                                break;
                            }
                            else if (lts != NULL && geomType == 2)
                            {
                                ret = lts->GetRules()->GetCount();
                                break;
                            }
                            else if (ats != NULL && geomType == 3)
                            {
                                ret = ats->GetRules()->GetCount();
                                break;
                            }
                            else if (cts != NULL && geomType == 4)
                            {
                                ret = cts->GetRules()->GetCount();
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return ret;
}

INT32 MgLayer::GetCompositeThemeCategoryCount(INT32 compositeOffset)
{
    return GetCompositeThemeCategoryCount(GetMap()->GetViewScale(), compositeOffset);
}

INT32 MgLayer::GetCompositeThemeCategoryCount(double scale, INT32 compositeOffset)
{
    INT32 ret = -1;

    Ptr<MgResourceService> resSvc = dynamic_cast<MgResourceService*>(GetMap()->GetService(MgServiceType::ResourceService));
    std::auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(resSvc, m_definition));
    if (ldf.get() != NULL)
    {
        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
        if(vl != NULL)
        {
            MdfModel::VectorScaleRangeCollection* scaleRanges = vl->GetScaleRanges();
            if (scaleRanges != NULL)
            {
                for (INT32 i = 0; i < scaleRanges->GetCount(); i++)
                {
                    MdfModel::VectorScaleRange* vsr = scaleRanges->GetAt(i);
                    if (scale >= vsr->GetMinScale() && scale < vsr->GetMaxScale())
                    {
                        MdfModel::FeatureTypeStyleCollection* ftsc = vsr->GetFeatureTypeStyles();
                        //NOTE: If a Layer Definition has basic and composite types, then this offset will probably be wrong
                        //but such layers are technically illegal and we shouldn't try to be catering to such layers
                        if (compositeOffset < ftsc->GetCount())
                        {
                            MdfModel::CompositeTypeStyle* cts = dynamic_cast<MdfModel::CompositeTypeStyle*>(ftsc->GetAt(compositeOffset));
                            if (cts != NULL)
                            {
                                ret = cts->GetRules()->GetCount();
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return ret;
}

MgByteReader* MgLayer::GenerateLegendImage(double scale, INT32 width, INT32 height, CREFSTRING format, INT32 geomType, INT32 themeCategory)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    Ptr<MgMappingService> svcMapping = dynamic_cast<MgMappingService*>(GetMap()->GetService(MgServiceType::MappingService));
    ret = svcMapping->GenerateLegendImage(m_definition, scale, width, height, format, geomType, themeCategory);

    MG_CATCH_AND_THROW(L"MgLayer.GenerateLegendImage");

    return ret.Detach();
}

MgSpatialContextReader* MgLayer::GetSpatialContexts(bool bActiveOnly)
{
    Ptr<MgSpatialContextReader> ret;

    MG_TRY()

    Ptr<MgFeatureService> svcFeature = dynamic_cast<MgFeatureService*>(GetMap()->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(m_featureSourceId);
    ret = svcFeature->GetSpatialContexts(fsId, bActiveOnly);

    MG_CATCH_AND_THROW(L"MgLayer.GetSpatialContexts");

    return ret.Detach();
}