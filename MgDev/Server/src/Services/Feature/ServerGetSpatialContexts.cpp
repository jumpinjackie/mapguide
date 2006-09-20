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

#include "ServerFeatureServiceDefs.h"
#include "ServerGetSpatialContexts.h"
#include "ServerFeatureConnection.h"
#include "SpatialContextData.h"
#include "ServerFeatureUtil.h"

#define CHECKOPEN(methodName) \
    if (!IsConnectionOpen())  \
        throw new MgConnectionNotOpenException(methodName, __LINE__, __WFILE__, NULL, L"", NULL); \

MgServerGetSpatialContexts::MgServerGetSpatialContexts()
{
    m_spatialContextInfoMap.reset(NULL);
}

MgServerGetSpatialContexts::~MgServerGetSpatialContexts()
{
}

// Executes the describe schema command and serializes the schema to XML

MgSpatialContextReader* MgServerGetSpatialContexts::GetSpatialContexts(MgResourceIdentifier* resId, bool bActiveOnly)
{
    Ptr<MgSpatialContextReader> mgSpatialContextReader;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resId)
    {
        throw new MgNullArgumentException(L"MgServerGetSpatialContexts.GetSpatialContexts", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgFeatureServiceCache* featureServiceCache = MgFeatureServiceCache::GetInstance();
    mgSpatialContextReader = featureServiceCache->ContainsSpatialContext(resId, bActiveOnly);
    if(NULL == mgSpatialContextReader)
    {
        // Connect to provider
        MgServerFeatureConnection msfc(resId);

        // connection must be open to retrieve list of active contexts
        if ( !msfc.IsConnectionOpen() )
        {
            throw new MgConnectionFailedException(L"MgServerGetSpatialContexts::GetSpatialContexts()", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        GisPtr<FdoIConnection> fdoConn = msfc.GetConnection();
        m_providerName = msfc.GetProviderName();
        m_spatialContextInfoMap.reset(msfc.GetSpatialContextInfoMap());

        // Check whether command is supported by provider
        if (!msfc.SupportsCommand((INT32)FdoCommandType_GetSpatialContexts))
        {
            // TODO: specify which argument and message, once we have the mechanism
            STRING message = MgServerFeatureUtil::GetMessage(L"MgCommandNotSupported");
            throw new MgInvalidOperationException(L"MgServerGetSpatialContexts.GetSpatialContexts", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        GisPtr<FdoIGetSpatialContexts> fdoCommand = (FdoIGetSpatialContexts*)fdoConn->CreateCommand(FdoCommandType_GetSpatialContexts);
        CHECKNULL((FdoIGetSpatialContexts*)fdoCommand, L"MgServerGetSpatialContexts.GetSpatialContexts");

        // Execute the command
        GisPtr<FdoISpatialContextReader> spatialReader = fdoCommand->Execute();
        CHECKNULL((FdoISpatialContextReader*)spatialReader, L"MgServerGetSpatialContexts.GetSpatialContexts");

        mgSpatialContextReader = new MgSpatialContextReader();
        while (spatialReader->ReadNext())
        {
            // If only active spatial context is required skip all others
            if (bActiveOnly)
            {
                if (!spatialReader->IsActive())
                    continue;
            }

            // Set providername for which spatial reader is executed
            mgSpatialContextReader->SetProviderName(m_providerName);

            Ptr<MgSpatialContextData> spatialData = GetSpatialContextData(spatialReader);
            CHECKNULL((MgSpatialContextData*)spatialData, L"MgServerGetSpatialContexts.GetSpatialContexts");

            // TODO: Remove this workaround for the SHP FDO provider when the defect 
            // for the "Default" active spatial contexts being blank is fixed.
            // START WORKAROUND CODE
            bool bAddSpatialContext = true;

            STRING name = spatialData->GetName();
            STRING coordSys = spatialData->GetCoordinateSystem();
            STRING coordSyswkt = spatialData->GetCoordinateSystemWkt();
            if((name == L"Default") && (coordSys.empty()) && (coordSyswkt.empty()))
            {
                // The "Default" coordinate system WKT is empty so we want to skip this one
                bAddSpatialContext = false;
            }
            // END WORKAROUND CODE

            if(bAddSpatialContext)
            {
                // Add spatial data to the spatialcontext reader
                mgSpatialContextReader->AddSpatialData(spatialData);

                // If only active spatial context is required skip all others
                if (bActiveOnly)
                {
                    if (spatialReader->IsActive())
                        break;
                }
            }
        }

        featureServiceCache->AddSpatialContext(resId, bActiveOnly, mgSpatialContextReader);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetSpatialContexts.GetSpatialContexts")

    return mgSpatialContextReader.Detach();
}

MgSpatialContextData* MgServerGetSpatialContexts::GetSpatialContextData(FdoISpatialContextReader* spatialReader)
{
    Ptr<MgSpatialContextData> spatialData = new MgSpatialContextData();

    // Name must exist
    GisString* name = spatialReader->GetName();
    CHECKNULL((GisString*)name, L"MgServerGetSpatialContexts.GetSpatialContexts");
    spatialData->SetName(STRING(name));

    // Co-ordinate system
    GisString* csName = spatialReader->GetCoordinateSystem();
    if (csName != NULL)
    {
        if (wcslen(csName) <= 0)
        {
            if (m_spatialContextInfoMap.get() != NULL)
            {
                // Perform substitution of missing coordinate system with
                // the spatial context mapping defined in feature source document
                std::map<STRING, STRING>::const_iterator iter;
                iter = m_spatialContextInfoMap->find(name);
                if (iter != m_spatialContextInfoMap->end())
                {
                    csName = (iter->second).c_str();
                }
            }
        }
        spatialData->SetCoordinateSystem(STRING(csName));
    }

    // WKT for co-ordinate system
    GisString* csWkt = spatialReader->GetCoordinateSystemWkt();
    if (csWkt != NULL)
    {
        // This is a work around for MG298: WKT not set for WMS and 
        // WFS spatial contexts.
        STRING srsWkt = csWkt;
        if (srsWkt.empty())
        {
            try
            {
                Ptr<MgCoordinateSystem> csPtr = new MgCoordinateSystem();
                srsWkt = csPtr->ConvertCoordinateSystemCodeToWkt(STRING(csName));
            }
            catch (MgException* e)
            {
                SAFE_RELEASE(e);
            }
            catch(...)
            {
                // Just use the empty WKT. 
            }

            spatialData->SetCoordinateSystemWkt(srsWkt);
        }
        else
        {
            spatialData->SetCoordinateSystemWkt(STRING(csWkt));
        }
    }

    // Desc for spatial context
    GisString* desc = spatialReader->GetDescription();
    if (desc != NULL)
    {
        spatialData->SetDescription(STRING(desc));
    }

    // Extent type
    FdoSpatialContextExtentType extentType = spatialReader->GetExtentType();
    spatialData->SetExtentType((INT32)extentType);

    // Extent (Geometry data)
    GisPtr<GisByteArray> byteArray = spatialReader->GetExtent();

    if (byteArray.p != NULL)
    {
        INT32 size = (INT32)byteArray->GetCount();
        BYTE_ARRAY_IN bytes = (BYTE_ARRAY_IN)byteArray->GetData();
        Ptr<MgByte> extent = new MgByte(bytes, size);
        spatialData->SetExtent(extent);
    }

    // XY Tolerance
    double xyTol = spatialReader->GetXYTolerance();
    spatialData->SetXYTolerance(xyTol);

    // Z Tolerance
    double zTol = spatialReader->GetZTolerance();
    spatialData->SetXYTolerance(zTol);

    // Whether it is active or not
    bool isActive = spatialReader->IsActive();
    spatialData->SetActiveStatus(isActive);

    return spatialData.Detach();
}
