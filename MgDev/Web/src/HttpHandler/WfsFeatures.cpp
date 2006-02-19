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

#include "OgcFramework.h"
#include "WfsFeatures.h"

MgWfsFeatures::MgWfsFeatures(WfsGetFeatureParams* pRequestParams, MgFeatureService* pFeatureService):
    m_pFeatureService(pFeatureService),
    m_pRequestParams(pRequestParams),
    m_bOk(true),
    m_bDone(false)
{
}

void MgWfsFeatures::RetrieveFeatures()
{
    // Add the opening FeatureCollection element to the response
    string responseString = "";

    // Loop through the requested feature types
    if(m_pRequestParams != NULL)
    {
        Ptr<MgStringCollection> featureTypeList = m_pRequestParams->GetFeatureTypes();
        if(featureTypeList != NULL && featureTypeList->GetCount() > 0)
        {
            int numFeaturesRetrieved = 0;
            int maxFeatures = m_pRequestParams->GetMaxFeatures();
            for(int i = 0; i < featureTypeList->GetCount(); i++)
            {
                // Check to see if we've already retrieved the maximum
                // number of features
                if(maxFeatures > 0 && numFeaturesRetrieved >= maxFeatures)
                {
                    break;
                }

                STRING featureType = featureTypeList->GetItem(i);
                int delimPos = (int)featureType.find_last_of(L":");
                if(delimPos != wstring::npos)
                {
                    STRING featureSource = featureType.substr(0, delimPos);
                    STRING fullFeatureSource = L"Library://" + featureSource + L".FeatureSource";
                    STRING featureClass = featureType.substr(delimPos + 1);

                    Ptr<MgResourceIdentifier> featureSourceId = new MgResourceIdentifier(fullFeatureSource);

                    Ptr<MgStringCollection> requiredProperties;
                    requiredProperties = NULL;
                    Ptr<MgStringCollection> requiredPropertiesList = m_pRequestParams->GetRequiredProperties();
                    if(requiredPropertiesList != NULL && requiredPropertiesList->GetCount() > i)
                    {
                        STRING requiredPropertiesString = requiredPropertiesList->GetItem(i);
                        if(requiredPropertiesString.length() > 0)
                        {
                            requiredProperties = MgStringCollection::ParseCollection(requiredPropertiesString, L",");
                        }
                    }

                    STRING filter = L"";
                    Ptr<MgStringCollection> filterList = m_pRequestParams->GetFilterStrings();
                    if(filterList->GetCount() > i)
                    {
                        filter = filterList->GetItem(i);
                    }
                    else if(filterList->GetCount() == 1)
                    {
                        // If there's only one filter in the list, use it for all typenames
                        filter = filterList->GetItem(0);
                    }

                    // Calculate the max remaining features to retrieve
                    int numFeaturesToRetrieve = maxFeatures;
                    if(maxFeatures > 0)
                    {
                        numFeaturesToRetrieve = maxFeatures - numFeaturesRetrieved;
                        if(numFeaturesToRetrieve <= 0)
                        {
                            // We have all the features we need, so break out of the loop
                            break;
                        }
                    }

                    // Call the C++ API
                    Ptr<MgByteReader> resultReader = m_pFeatureService->GetWfsFeature(featureSourceId, featureClass,
                        requiredProperties, m_pRequestParams->GetSrs(), filter, numFeaturesToRetrieve);

                    // TODO How to determine number of features retrieved...?
                    // numFeaturesRetrieved += ?

                    // Write the byte reader's data into our response data object
                    string thisResponseString;
                    resultReader->ToStringUtf8(thisResponseString);

                    // Strip off anything before the first member element, or after the last one
                    int firstMember = thisResponseString.find(startMemberElement);
                    if(firstMember != string::npos)
                    {
                        int lastMember = thisResponseString.rfind(endMemberElement);
                        if(lastMember != string::npos)
                        {
                            responseString.append(thisResponseString, firstMember, lastMember - firstMember + endMemberElement.length());
                        }
                    }
                }
            }
        }
    }
    if(responseString.length() > 0)
    {
        m_responseString = MgUtil::MultiByteToWideChar(responseString);
        m_bOk = true;
    }
    else
    {
        m_bOk = false;
    }
}


MgWfsFeatures::~MgWfsFeatures()
{
}

bool MgWfsFeatures::Next()
{
    if(m_bOk)
    {
        if(m_bDone)
        {
            m_bOk = false;
        }
    }
    return m_bOk;
}

void MgWfsFeatures::GenerateDefinitions(MgUtilDictionary& Dictionary)
{
    if(m_bOk)
    {
        Dictionary.AddDefinition(L"GetFeature.FeatureCollection", m_responseString.c_str());
        m_bDone = true;
    }
}

