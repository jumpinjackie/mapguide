//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "ServerFeatureUtil.h"
#include "CacheManager.h"

MgServerGetSpatialContexts::MgServerGetSpatialContexts()
{
    m_featureServiceCache = MgCacheManager::GetInstance()->GetFeatureServiceCache();
}

MgServerGetSpatialContexts::~MgServerGetSpatialContexts()
{
}

// Executes the describe schema command and serializes the schema to XML

MgSpatialContextReader* MgServerGetSpatialContexts::GetSpatialContexts(MgResourceIdentifier* resId, bool bActiveOnly)
{
    Ptr<MgSpatialContextReader> mgSpatialContextReader;

    MG_FEATURE_SERVICE_TRY()

    //Get the Resource Service
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
    (MgServiceManager::GetInstance())->RequestService(MgServiceType::ResourceService));
    ACE_ASSERT(NULL != resourceService.p);

    mgSpatialContextReader = m_featureServiceCache->GetSpatialContextReader(resId, bActiveOnly);

    //if the reader exists
    if(NULL != mgSpatialContextReader.p) 
    {
        //check the permissions
        if(false == resourceService->HasPermission(resId, MgResourcePermission::ReadOnly))
        {
            MgStringCollection arguments;
            arguments.Add(resId->ToString());

            throw new MgPermissionDeniedException(
                L"MgServerGetSpatialContexts.GetSpatialContexts",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }
    else
    {
        // Connect to provider
        MgServerFeatureConnection msfc(resId);

        // connection must be open to retrieve list of active contexts
        if ( msfc.IsConnectionOpen() )
        {
            // The reference to the FDO connection from the MgServerFeatureConnection object must be cleaned up before the parent object
            // otherwise it leaves the FDO connection marked as still in use.
            FdoPtr<FdoIConnection> fdoConn = msfc.GetConnection();
            m_providerName = msfc.GetProviderName();

            MgCacheManager* cacheManager = MgCacheManager::GetInstance();
            Ptr<MgSpatialContextCacheItem> cacheItem = cacheManager->GetSpatialContextCacheItem(resId);
            MgSpatialContextInfo* spatialContextInfo = cacheItem->Get();

            // Check whether command is supported by provider
            if (!msfc.SupportsCommand((INT32)FdoCommandType_GetSpatialContexts))
            {
                // TODO: specify which argument and message, once we have the mechanism
                STRING message = MgServerFeatureUtil::GetMessage(L"MgCommandNotSupported");
                throw new MgInvalidOperationException(L"MgServerGetSpatialContexts.GetSpatialContexts", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            FdoPtr<FdoIGetSpatialContexts> fdoCommand = (FdoIGetSpatialContexts*)fdoConn->CreateCommand(FdoCommandType_GetSpatialContexts);
            CHECKNULL((FdoIGetSpatialContexts*)fdoCommand, L"MgServerGetSpatialContexts.GetSpatialContexts");

            // Execute the command
            FdoPtr<FdoISpatialContextReader> spatialReader = fdoCommand->Execute();
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

                Ptr<MgSpatialContextData> spatialData = GetSpatialContextData(spatialReader, spatialContextInfo);
                CHECKNULL((MgSpatialContextData*)spatialData, L"MgServerGetSpatialContexts.GetSpatialContexts");

                // Add spatial data to the spatialcontext reader
                mgSpatialContextReader->AddSpatialData(spatialData);

                // If only active spatial context is required skip all others
                if (bActiveOnly)
                {
                    if (spatialReader->IsActive())
                        break;
                }
            }

            m_featureServiceCache->SetSpatialContextReader(resId, bActiveOnly, mgSpatialContextReader.p);
        }
        else
        {
            throw new MgConnectionFailedException(L"MgServerGetSpatialContexts::GetSpatialContexts()", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetSpatialContexts.GetSpatialContexts")

    return mgSpatialContextReader.Detach();
}

MgSpatialContextData* MgServerGetSpatialContexts::GetSpatialContextData(
    FdoISpatialContextReader* spatialReader, MgSpatialContextInfo* spatialContextInfo)
{
    Ptr<MgSpatialContextData> spatialData = new MgSpatialContextData();

    // Name must exist
    FdoString* name = spatialReader->GetName();
    CHECKNULL((FdoString*)name, L"MgServerGetSpatialContexts.GetSpatialContexts");
    spatialData->SetName(STRING(name));

    FdoString* csName = spatialReader->GetCoordinateSystem();
    STRING coordSysName = STRING(csName);
    bool spatialContextDefined = !coordSysName.empty();
    bool coordSysOverridden = false;

    // look for coordinate system override
    if (NULL != spatialContextInfo)
    {
        // Perform substitution of missing coordinate system with
        // the spatial context mapping defined in feature source document
        MgSpatialContextInfo::const_iterator iter = spatialContextInfo->find(name);

        if (spatialContextInfo->end() != iter)
        {
            csName = (iter->second).c_str();
            coordSysOverridden = true;
        }
    }

    if (csName != NULL)
    {
        spatialData->SetCoordinateSystem(STRING(csName));
    }

    // WKT for co-ordinate system
    FdoString* csWkt = NULL;
    STRING srsWkt = L"";

    // Desc for spatial context
    STRING desc = L"";

    // Whether it is active or not
    bool isActive = spatialReader->IsActive();

    if (coordSysOverridden)
    {
        srsWkt = csName;
        desc = MgServerFeatureUtil::GetMessage(L"MgCoordinateSystemOverridden");
    }
    else if (spatialContextDefined && !coordSysOverridden)
    {
        csWkt = spatialReader->GetCoordinateSystemWkt();
        srsWkt = csWkt;

        if (srsWkt.empty())
        {
            // This is a work around for MG298: WKT not set for WMS and
            // WFS spatial contexts.

            try
            {
                Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
                Ptr<MgCoordinateSystem> csPtr = csFactory->CreateFromCode(STRING(csName));
                srsWkt = csPtr->ToString();
            }
            catch (MgException* e)
            {
                SAFE_RELEASE(e);
            }
            catch(...)
            {
                // Just use the empty WKT.
            }
        }

        FdoString* fdoDesc = spatialReader->GetDescription();
        desc = STRING(fdoDesc);
    }

    // retrieve other values from spatialReader
    FdoSpatialContextExtentType extentType = spatialReader->GetExtentType();
    FdoPtr<FdoByteArray> byteArray = spatialReader->GetExtent();
    double xyTol = spatialReader->GetXYTolerance();
    double zTol = spatialReader->GetZTolerance();

    spatialData->SetCoordinateSystemWkt(srsWkt);
    spatialData->SetDescription(desc);
    spatialData->SetExtentType((INT32)extentType);

    if (byteArray.p != NULL)
    {
        INT32 size = (INT32)byteArray->GetCount();
        BYTE_ARRAY_IN bytes = (BYTE_ARRAY_IN)byteArray->GetData();
        Ptr<MgByte> extent = new MgByte(bytes, size);
        spatialData->SetExtent(extent);
    }

    // XY Tolerance
    spatialData->SetXYTolerance(xyTol);

    // Z Tolerance
    spatialData->SetZTolerance(zTol);

    // Whether it is active or not
    spatialData->SetActiveStatus(isActive);

    return spatialData.Detach();
}
