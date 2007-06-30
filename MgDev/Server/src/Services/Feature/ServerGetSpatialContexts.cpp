//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

        FdoPtr<FdoIConnection> fdoConn = msfc.GetConnection();
        m_providerName = msfc.GetProviderName();
        m_spatialContextInfoMap.reset(msfc.GetSpatialContextInfoMap());

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

            Ptr<MgSpatialContextData> spatialData = GetSpatialContextData(spatialReader);
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

        featureServiceCache->AddSpatialContext(resId, bActiveOnly, mgSpatialContextReader);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetSpatialContexts.GetSpatialContexts")

    return mgSpatialContextReader.Detach();
}

MgSpatialContextData* MgServerGetSpatialContexts::GetSpatialContextData(FdoISpatialContextReader* spatialReader)
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
    if (m_spatialContextInfoMap.get() != NULL)
    {
        // Perform substitution of missing coordinate system with
        // the spatial context mapping defined in feature source document
        std::map<STRING, STRING>::const_iterator iter;
        iter = m_spatialContextInfoMap->find(name);
        if (iter != m_spatialContextInfoMap->end())
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
