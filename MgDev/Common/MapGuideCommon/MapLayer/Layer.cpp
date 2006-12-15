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

#include "MapGuideCommon.h"

IMPLEMENT_CREATE_OBJECT(MgLayer)

//////////////////////////////////////////////////////////////
// Creates a MgLayerBase object
//
MgLayer::MgLayer()
    : MgLayerBase()
{
}



//////////////////////////////////////////////////////////////
// Creates a MgLayerBase object given a reference to a LayerDefinition
//
MgLayer::MgLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService)
    : MgLayerBase(layerDefinition, resourceService)
{
    GetLayerInfoFromDefinition(resourceService);
}


//////////////////////////////////////////////////////////////
// Destruct a MgLayerBase object
//
MgLayer::~MgLayer()
{
}



//////////////////////////////////////////////////////////////
// Parse the layer definition XML and extracts scale ranges,
// feature source and feature class from it
//
void MgLayer::GetLayerInfoFromDefinition(MgResourceService* resourceService)
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
            STRING schemaName;
            STRING className;
            size_t pfxSep = m_featureName.find_first_of(L':');
            if (pfxSep != STRING::npos)
            {
                // fully qualified
                schemaName = m_featureName.substr(0, pfxSep);
                className = m_featureName.substr(pfxSep + 1, m_featureName.length() - pfxSep - 1);
            }
            else
            {
                // non-qualified
                className = m_featureName;

                //TODO:  How do we deal with different schemas?  Just use first one for now...
                Ptr<MgStringCollection> schemaNames = featureService->GetSchemas(resId);
                schemaName = schemaNames->GetItem(0);
            }

            // Get the identity properties
            Ptr<MgPropertyDefinitionCollection> idProps = featureService->GetIdentityProperties(resId,
                                                                                                schemaName,
                                                                                                className);
            assert(idProps != NULL);
            for (int nIds=0; nIds<idProps->GetCount(); nIds++)
            {
                Ptr<MgDataPropertyDefinition> prop = dynamic_cast<MgDataPropertyDefinition*>(idProps->GetItem(nIds));
                if (0 == (MgDataPropertyDefinition*)prop)
                {
                    throw new MgInvalidCastException(L"MgLayer::GetLayerInfoFromDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
                }
                MgLayer::IdProperty idProp;
                idProp.type = prop->GetDataType();
                idProp.name = prop->GetName();
                m_idProps.push_back(idProp);
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
    helper->WriteUINT8(flags);
    helper->WriteString(m_legendLabel);
    helper->WriteBytes((const unsigned char*)&m_displayOrder, sizeof(double));

    UINT32 scaleValueCount = (UINT32) m_scaleRanges.size();
    helper->WriteUINT32(scaleValueCount);
    if(scaleValueCount > 0)
    {
        for(SCALERANGES::const_iterator it = m_scaleRanges.begin(); it != m_scaleRanges.end(); it++)
            helper->WriteBytes((const unsigned char*)&(*it), sizeof(double));
    }
    helper->WriteString(m_featureSourceId);
    helper->WriteString(m_featureName);
    helper->WriteString(m_geometry);

    helper->WriteUINT32((UINT32)m_idProps.size());
    for (IdPropertyList::iterator ids = m_idProps.begin(); ids != m_idProps.end(); ids++)
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

