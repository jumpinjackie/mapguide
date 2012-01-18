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
#include "Selection.h"
#include "SAX2Parser.h"

MG_IMPL_DYNCREATE(MgdSelection)

/////////////////////////////////////////
// Dummy constructor for Ptr<> template
//
MgdSelection::MgdSelection()
: MgSelectionBase()
{
}


// Initialize selection generation from a map
MgdSelection::MgdSelection(MgdMap* map)
: MgSelectionBase(map)
{
}


// Initialize selection from a map and xml selection
MgdSelection::MgdSelection(MgdMap* map, CREFSTRING xmlSelectionString)
: MgSelectionBase(map, xmlSelectionString)
{
}


// Destructor
MgdSelection::~MgdSelection()
{
}


/////////////////////////////////////////////////////////////////
// Get the selected feature data for the specified feature class.
//
MgFeatureReader* MgdSelection::GetSelectedFeatures(MgLayerBase* layer, CREFSTRING className, bool mappedOnly)
{
    CHECKNULL((MgLayerBase*)layer, L"MgdSelection.GetSelectedFeatures");

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

    MG_CATCH_AND_THROW(L"MgdSelection.GetSelectedFeatures")

    return NULL;
}

/////////////////////////////////////////////////////////////////
/// Get the selected feature data for the specified feature class.
///
MgFeatureReader* MgdSelection::GetSelectedFeatures(MgLayerBase* layer, CREFSTRING className, MgStringCollection* propertyNames)
{
    CHECKNULL((MgLayerBase*)layer, L"MgdSelection.GetSelectedFeatures");

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
void MgdSelection::Open(MgResourceService* resourceService, CREFSTRING mapName)
{
	/*
    STRING sessionId;
    Ptr<MgUserInformation> userInfo = resourceService->GetUserInfo();
    if (userInfo.p != NULL) sessionId = userInfo->GetMgSessionId();
    if (sessionId.empty())
    {
        throw new MgSessionExpiredException(L"MgdSelection.Open",__LINE__,__WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(GetResourceName(sessionId, mapName));
    MgResource::Open(resourceService, resId);
	*/
}


//////////////////////////////////////////////////////////////////
// Save the resource
//
void MgdSelection::Save(MgResourceService* resourceService, CREFSTRING mapName)
{
	/*
    STRING sessionId;
    Ptr<MgUserInformation> userInfo = resourceService->GetUserInfo();
    if (userInfo.p != NULL) sessionId = userInfo->GetMgSessionId();
    if (sessionId.empty())
    {
        throw new MgSessionExpiredException(L"MgdSelection.Save",__LINE__,__WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(GetResourceName(sessionId, mapName));
    MgResource::Save(resourceService, resId);
	*/
}


//////////////////////////////////////////////////////////////////
STRING MgdSelection::GetResourceName(CREFSTRING sessionId, CREFSTRING mapName)
{
    return L"Session:" + sessionId + L"//" + mapName + L".Selection"; //MgResourceType::Selection gives LNK2001 (huh?)
}

//////////////////////////////////////////////////////////////////
const char* MgdSelection::GetResourceTypeName()
{
    return "Selection";
}

void MgdSelection::Clear()
{
    m_selections.clear();
}