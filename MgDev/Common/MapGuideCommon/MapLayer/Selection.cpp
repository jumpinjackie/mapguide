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

#include <algorithm>
#include "MapGuideCommon.h"

MG_IMPL_DYNCREATE(MgSelection)

/////////////////////////////////////////
// Dummy constructor for Ptr<> template
//
MgSelection::MgSelection()
: MgSelectionBase()
{
}


// Initialize selection generation from a map
MgSelection::MgSelection(MgMap* map)
: MgSelectionBase(map)
{
}


// Initialize selection from a map and xml selection
MgSelection::MgSelection(MgMap* map, CREFSTRING xmlSelectionString)
: MgSelectionBase(map, xmlSelectionString)
{
}


// Destructor
MgSelection::~MgSelection()
{
}


void MgSelection::Dispose()
{
    delete this;
}


/////////////////////////////////////////////////////////////////
// Get the selected feature data for the specified feature class.
//
MgFeatureReader* MgSelection::GetSelectedFeatures(MgLayerBase* layer, CREFSTRING className, bool mappedOnly)
{
    CHECKNULL((MgLayerBase*)layer, L"MgSelection.GetSelectedFeatures");

    if (!mappedOnly)
        return GetSelectedFeatures(layer, className, (MgStringCollection*)NULL);

    MG_TRY()

    Ptr<MgResourceIdentifier> layerid = layer->GetLayerDefinition();
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
        m_map->GetService(MgServiceType::ResourceService));
    auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(resourceService, layerid));
    Ptr<MgStringCollection> propertyNames = new MgStringCollection();
    if(ldf.get() != NULL)
    {
        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
        if(vl != NULL)
        {
            MdfModel::NameStringPairCollection* pmappings = vl->GetPropertyMappings();
            for (int j=0; j<pmappings->GetCount(); j++)
            {
                MdfModel::NameStringPair* m = pmappings->GetAt(j);
                propertyNames->Add(m->GetName());
            }
        }
    }
    return GetSelectedFeatures(layer, className, propertyNames);

    MG_CATCH_AND_THROW(L"MgSelection.GetSelectedFeatures")

    return NULL;
}

/////////////////////////////////////////////////////////////////
/// Get the selected feature data for the specified feature class.
///
MgFeatureReader* MgSelection::GetSelectedFeatures(MgLayerBase* layer, CREFSTRING className, MgStringCollection* propertyNames)
{
    CHECKNULL((MgLayerBase*)layer, L"MgSelection.GetSelectedFeatures");

    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        m_map->GetService(MgServiceType::FeatureService));
    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(layer->GetFeatureSourceId());

    // Ctor query filter
    STRING filter = GenerateFilter(layer, className);
    Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();
    options->SetFilter(filter);
    if (propertyNames)
    {
        for (INT32 i = 0; i < propertyNames->GetCount(); ++i)
            options->AddFeatureProperty(propertyNames->GetItem(i));
    }
    return featureService->SelectFeatures(resourceId, className, options);
}

/////////////////////////////////////////////////////////////////
// Open the resource
//
void MgSelection::Open(MgResourceService* resourceService, CREFSTRING mapName)
{
    STRING sessionId;
    Ptr<MgUserInformation> userInfo = resourceService->GetUserInfo();
    if (userInfo.p != NULL) sessionId = userInfo->GetMgSessionId();
    if (sessionId.empty())
    {
        throw new MgSessionExpiredException(L"MgSelection.Open",__LINE__,__WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(GetResourceName(sessionId, mapName));
    MgResource::Open(resourceService, resId);
}


//////////////////////////////////////////////////////////////////
// Save the resource
//
void MgSelection::Save(MgResourceService* resourceService, CREFSTRING mapName)
{
    STRING sessionId;
    Ptr<MgUserInformation> userInfo = resourceService->GetUserInfo();
    if (userInfo.p != NULL) sessionId = userInfo->GetMgSessionId();
    if (sessionId.empty())
    {
        throw new MgSessionExpiredException(L"MgSelection.Save",__LINE__,__WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(GetResourceName(sessionId, mapName));
    MgResource::Save(resourceService, resId);
}


//////////////////////////////////////////////////////////////////
STRING MgSelection::GetResourceName(CREFSTRING sessionId, CREFSTRING mapName)
{
    return L"Session:" + sessionId + L"//" + mapName + L"." + MgResourceType::Selection;
}

//////////////////////////////////////////////////////////////////
const char* MgSelection::GetResourceTypeName()
{
    return "Selection";
}
