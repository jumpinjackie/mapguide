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

#include "HttpHandler.h"
#include "HttpSelectFeaturesSpatially.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpSelectFeaturesSpatially)

/// <summary>
/// Initializes the common parameters and parameters specific to this request.
/// </summary>
/// <param name="name">Input
/// MgHttpRequest
/// This contains all the parameters of the request.
/// </param>
/// <returns>
/// nothing
/// </returns>
MgHttpSelectFeaturesSpatially::MgHttpSelectFeaturesSpatially(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    m_resId = params->GetParameterValue(MgHttpResourceStrings::reqFeatResourceId);
    m_className = params->GetParameterValue(MgHttpResourceStrings::reqFeatClass);
    m_geometryClass = params->GetParameterValue(MgHttpResourceStrings::reqFeatGeomProperty);

    STRING spatialOp = params->GetParameterValue(MgHttpResourceStrings::reqFeatSpatialOp);
    if(spatialOp.length() == 0)
        m_operation = MgFeatureSpatialOperations::Within;
    else
        m_operation = atoi(MgUtil::WideCharToMultiByte(spatialOp).c_str());

    m_geometry = params->GetParameterValue(MgHttpResourceStrings::reqFeatGeometry);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpSelectFeaturesSpatially::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    MgResourceIdentifier resId(m_resId);

    // Create Proxy Feature Service instance
    Ptr<MgFeatureService> service = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));

    Ptr<MgHttpRequestParam> params = m_hRequest->GetRequestParam();

    //get addtional params, determine which overload to call and make the call
    STRING filterText = params->GetParameterValue(MgHttpResourceStrings::reqFeatFilter);
    STRING propsStr = params->GetParameterValue(MgHttpResourceStrings::reqFeatProperties);

    STRING computedProperties = params->GetParameterValue(MgHttpResourceStrings::reqFeatComputedProperties);
    STRING computedAliases = params->GetParameterValue(MgHttpResourceStrings::reqFeatComputedAliases);

    Ptr<MgStringCollection> featureProps = MgStringCollection::ParseCollection(propsStr, L"\t");
    Ptr<MgStringCollection> computedProps = MgStringCollection::ParseCollection(computedProperties, L"\t");
    Ptr<MgStringCollection> aliases = MgStringCollection::ParseCollection(computedAliases, L"\t");

    Ptr<MgFeatureAggregateOptions> qryOptions = new MgFeatureAggregateOptions();
    qryOptions->SetFilter(filterText);

    if (featureProps != NULL)
    {
        INT32 cnt = featureProps->GetCount();
        for (INT32 i=0; i < cnt; i++)
        {
            qryOptions->AddFeatureProperty(featureProps->GetItem(i));
        }
    }

    if ((computedProps != NULL) && (aliases != NULL))
    {
        INT32 aliasCnt = aliases->GetCount();
        INT32 computedCnt = computedProps->GetCount();

        if (aliasCnt == computedCnt)
        {
            for (INT32 j=0; j < aliasCnt; j++)
            {
                qryOptions->AddComputedProperty(aliases->GetItem(j), computedProps->GetItem(j));
            }
        }
        else
        {
            // throw exception here
        }
    }

    Ptr<MgDataReader> dataReader = service->SelectAggregate(&resId, m_className, qryOptions);
    Ptr<MgByteReader> byteReader = dataReader->ToXml();

    //Convert to alternate response format, if necessary
    ProcessFormatConversion(byteReader);

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpSelectFeaturesSpatially.Execute")
}
