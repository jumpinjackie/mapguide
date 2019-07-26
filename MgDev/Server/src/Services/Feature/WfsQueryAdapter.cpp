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

#include "ServerFeatureServiceDefs.h"
#include "WfsQueryAdapter.h"

MgWfsQueryAdapter::MgWfsQueryAdapter(MgServerFeatureService* featSvc)
{
    m_featSvc = SAFE_ADDREF(featSvc);
}

MgWfsQueryAdapter::~MgWfsQueryAdapter()
{
    TransformCache::Clear(m_transformCache);
    m_classDef = NULL;
    m_fs = NULL;
    m_options = NULL;
    m_featSvc = NULL;
    m_mapCs = NULL;
    m_xform = NULL;
}

bool MgWfsQueryAdapter::SetOptions(MgResourceIdentifier* fs,
                                   CREFSTRING featureClass,
                                   MgStringCollection* propNames,
                                   CREFSTRING srs,
                                   CREFSTRING wfsFilter,
                                   CREFSTRING sortCriteria)
{
    m_fs = SAFE_ADDREF(fs);
    m_className = featureClass;
    STRING schemaName, className;
    MgUtil::ParseQualifiedClassName(m_className, schemaName, className);

    bool hashed = (0 == schemaName.find_first_of(MG_SCHEMA_NAME_HASH_PREFIX.c_str(), 0, MG_SCHEMA_NAME_HASH_PREFIX.length()));
    Ptr<MgStringCollection> classNames = new MgStringCollection();

    if (!className.empty())
    {
        classNames->Add(className);
    }

    // Find the needed class definition.
    Ptr<MgFeatureSchemaCollection> fsc = m_featSvc->DescribeSchema(m_fs, hashed ? L"" : schemaName, classNames);
    Ptr<MgFeatureSchema> schema;
    STRING schemaHash;

    m_featSvc->FindClassDefinition(fsc, schemaName, className, schemaHash, schema, m_classDef);

    if (hashed && NULL != schema.p)
    {
        MgUtil::FormatQualifiedClassName(schema->GetName(), className, m_className);
    }

    MgCoordinateSystemFactory fact;
    std::wstring wkt = srs;
    if (wkt.empty())
    {
        //If there is no coordinate system pass in, get the default one in resource header.
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        assert(NULL != serviceMan);
        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceMan->RequestService(MgServiceType::ResourceService));
        assert(resourceService != NULL);
        Ptr<MgByteReader> byteReaderHeader = resourceService->GetResourceHeader(m_fs);
        Ptr<MgByteSink> byteSinkHeader = new MgByteSink(byteReaderHeader);
        std::string resourceHeader;
        byteSinkHeader->ToStringUtf8(resourceHeader);
        //parse for default SRS of this WFS, the format is:
        //<Property xsi:noNamespaceSchemaLocation="Property-1.0.0.xsd">
        //  <Name>_PrimarySRS</Name>
        //  <Value>EPSG:4326</Value>
        //</Property>
        std::string primary("<Name>_PrimarySRS</Name>");
        std::size_t primaryPos = resourceHeader.find(primary);
        if (primaryPos != std::string::npos)
        {
            std::string begin("<Value>EPSG:");
            std::size_t beginPos = resourceHeader.find(begin, primaryPos);
            if (beginPos != std::string::npos)
            {
                std::size_t endPos = resourceHeader.find("</Value>", beginPos);
                if (endPos != std::string::npos)
                {
                    std::string primarySRS = resourceHeader.substr(beginPos + begin.length(), endPos - beginPos - begin.length());
                    int epsgCode = atoi(primarySRS.c_str());
                    wkt = fact.ConvertEpsgCodeToWkt(epsgCode);
                }
            }
        }
    }

    if (!wkt.empty())
    {
        MG_TRY();
        m_mapCs = fact.Create(wkt);
        MG_CATCH_AND_RELEASE();
    }

    //get a transform from feature space to mapping space
    TransformCache* item = TransformCache::GetLayerToMapTransform(m_transformCache, m_className, m_fs, m_mapCs, &fact, m_featSvc, false);
    m_xform = item ? item->GetMgTransform() : NULL;

    assert(m_classDef.p != NULL);
    if (NULL == m_classDef.p)
        return false;

    //execute the spatial query
    Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();

    //add the properties we need from FDO
    if (propNames)
    {
        for (int i = 0; i<propNames->GetCount(); i++)
            options->AddFeatureProperty(propNames->GetItem(i));
    }

    //convert the WFS filter to an FDO filter
    //and set it to the FDO feature query
    if (!wfsFilter.empty())
    {
        STRING GEOM_PROP_TAG = L"%MG_GEOM_PROP%"; //NOXLATE
        STRING fdoFilterString = L""; //NOXLATE

        Ptr<MgPropertyDefinitionCollection> properties = m_classDef->GetProperties();
        MgOgcFilterUtil u;

        for (int i = 0; i<properties->GetCount(); i++)
        {
            Ptr<MgPropertyDefinition> prop = properties->GetItem(i);
            if (prop->GetPropertyType() == MgFeaturePropertyType::GeometricProperty)
            {
                STRING ogcFilter = wfsFilter;

                if (wfsFilter.find(GEOM_PROP_TAG, 0) != STRING::npos)
                {
                    ogcFilter = MgUtil::ReplaceString(wfsFilter, GEOM_PROP_TAG.c_str(), prop->GetName().c_str());
                }
                if (!fdoFilterString.empty())
                {
                    fdoFilterString += L" OR "; //NOXLATE
                }
                TransformCache* itemFilter = TransformCache::GetLayerToMapTransform(m_transformCache, m_className, m_fs, m_mapCs, &fact, m_featSvc, true);
                Ptr<MgCoordinateSystemTransform> transFilter = itemFilter ? itemFilter->GetMgTransform() : NULL;
                fdoFilterString += u.Ogc2FdoFilter(ogcFilter, transFilter, prop->GetName(), properties);
            }
        }

        options->SetFilter(fdoFilterString);
    }

    if (!sortCriteria.empty())
    {
        Ptr<MgStringCollection> orderByProperties = new MgStringCollection();
        int orderOption = MgOrderingOption::Ascending;

        STRING sSortCriteria = sortCriteria;
        STRING::size_type pos = sSortCriteria.find_last_of(L" ");
        if (pos != STRING::npos)
        {
            STRING sSortByProperty = sSortCriteria.substr(0, pos);
            orderByProperties->Add(sSortByProperty);

            STRING sSortOption = MgUtil::ToUpper(sSortCriteria.substr(pos + 1));
            if (sSortOption == L"D")
            {
                orderOption = MgOrderingOption::Descending;
            }
        }
        else
        {
            orderByProperties->Add(sortCriteria);
        }

        options->SetOrderingFilter(orderByProperties, orderOption);
    }

    m_options = SAFE_ADDREF(options.p);

    return true;
}

MgClassDefinition* MgWfsQueryAdapter::GetClassDefinition()
{
    return SAFE_ADDREF(m_classDef.p);
}

MgCoordinateSystemTransform* MgWfsQueryAdapter::GetTransform()
{
    return SAFE_ADDREF(m_xform.p);
}

MgCoordinateSystem* MgWfsQueryAdapter::GetMapCs()
{
    return SAFE_ADDREF(m_mapCs.p);
}

MgFeatureReader* MgWfsQueryAdapter::GetWfsReader()
{
    Ptr<MgFeatureReader> fr;
    MG_FEATURE_SERVICE_TRY()
    // TODO: can FeatureName be an extension name rather than a FeatureClass?
    fr = m_featSvc->SelectFeatures(m_fs, m_className, m_options);
    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(m_fs, L"MgWfsQueryAdapter.GetWfsReader")
    return fr.Detach();
}

INT32 MgWfsQueryAdapter::GetTotal(INT32 maxFeatures)
{
    INT32 total = 0;

    MG_FEATURE_SERVICE_TRY()
    // TODO: can FeatureName be an extension name rather than a FeatureClass?
    Ptr<MgFeatureReader> fr = m_featSvc->SelectFeatures(m_fs, m_className, m_options);

    // You may think: Why are we raw spinning this reader? We have select aggregates (with COUNT()) 
    // and other performant shortcuts available, but the real stickler is the maxFeatures parameter
    // passed in. WFS queries allow for this to be passed in and if we get one, we must respect
    // the value. None of our performant shortcuts allow us to apply an upper limit on it. So given
    // these constraints, raw spinning the feature reader is the only viable option.
    while (fr->ReadNext())
    {
        total++;
        //Break loop if we hit max features
        if (maxFeatures > 0 && total >= maxFeatures)
            break;
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(m_fs, L"MgWfsQueryAdapter.GetWfsReader")
    return total;
}