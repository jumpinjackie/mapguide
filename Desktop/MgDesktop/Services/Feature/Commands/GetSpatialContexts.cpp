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

#include "Services/Feature/FeatureDefs.h"
#include "GetSpatialContexts.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FeatureUtil.h"
#include "Services/Feature/FeatureServiceCache.h"
#include "GeometryCommon.h"

MgdGetSpatialContexts::MgdGetSpatialContexts()
{
    m_featureServiceCache = MgdFeatureServiceCache::GetInstance();
}

MgdGetSpatialContexts::~MgdGetSpatialContexts()
{
}

// Executes the GetSpatialContext command and creates a reader

MgSpatialContextReader* MgdGetSpatialContexts::GetSpatialContexts(MgResourceIdentifier* resId)
{
    Ptr<MgSpatialContextReader> mgSpatialContextReader;

    MG_FEATURE_SERVICE_TRY()

    mgSpatialContextReader = m_featureServiceCache->GetSpatialContextReader(resId);

    if (NULL == mgSpatialContextReader.p)
    {
        // Connect to provider
        Ptr<MgdFeatureConnection> msfc = new MgdFeatureConnection(resId);

        // Connection must be open to retrieve a list of available contexts.
        if ((NULL != msfc.p) && ( msfc->IsConnectionOpen() ))
        {
            // The reference to the FDO connection from the MgdFeatureConnection object must be cleaned up before the parent object
            // otherwise it leaves the FDO connection marked as still in use.
            FdoPtr<FdoIConnection> fdoConn = msfc->GetConnection();
            m_providerName = msfc->GetProviderName();

            Ptr<MgdSpatialContextCacheItem> cacheItem = m_featureServiceCache->GetSpatialContextInfo(resId);
            MgSpatialContextInfo* spatialContextInfo = cacheItem->Get();

            // Check whether command is supported by provider
            if (!msfc->SupportsCommand((INT32)FdoCommandType_GetSpatialContexts))
            {
                // TODO: specify which argument and message, once we have the mechanism
                STRING message = MgdFeatureUtil::GetMessage(L"MgCommandNotSupported");
                throw new MgInvalidOperationException(L"MgdGetSpatialContexts.GetSpatialContexts", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            FdoPtr<FdoIGetSpatialContexts> fdoCommand = (FdoIGetSpatialContexts*)fdoConn->CreateCommand(FdoCommandType_GetSpatialContexts);
            CHECKNULL((FdoIGetSpatialContexts*)fdoCommand, L"MgdGetSpatialContexts.GetSpatialContexts");

            // Execute the command
            FdoPtr<FdoISpatialContextReader> spatialReader = fdoCommand->Execute();
            CHECKNULL((FdoISpatialContextReader*)spatialReader, L"MgdGetSpatialContexts.GetSpatialContexts");

            mgSpatialContextReader = new MgSpatialContextReader();
            while (spatialReader->ReadNext())
            {
                // Set providername for which spatial reader is executed
                mgSpatialContextReader->SetProviderName(m_providerName);

                Ptr<MgSpatialContextData> spatialData = GetSpatialContextData(spatialReader, spatialContextInfo);
                CHECKNULL((MgSpatialContextData*)spatialData, L"MgdGetSpatialContexts.GetSpatialContexts");

                // Add spatial data to the spatialcontext reader
                mgSpatialContextReader->AddSpatialData(spatialData);
            }

            m_featureServiceCache->SetSpatialContextReader(resId, mgSpatialContextReader.p);
        }
        else
        {
            throw new MgdConnectionFailedException(L"MgdGetSpatialContexts.GetSpatialContexts()", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else
    {
        //MgCacheManager::GetInstance()->CheckPermission(resId, MgResourcePermission::ReadOnly);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resId, L"MgdGetSpatialContexts.GetSpatialContexts")

    return mgSpatialContextReader.Detach();
}

MgSpatialContextData* MgdGetSpatialContexts::GetSpatialContextData(
    FdoISpatialContextReader* spatialReader, MgSpatialContextInfo* spatialContextInfo)
{
    Ptr<MgSpatialContextData> spatialData = new MgSpatialContextData();

    // Name must exist
    FdoString* name = spatialReader->GetName();
    CHECKNULL((FdoString*)name, L"MgdGetSpatialContexts.GetSpatialContexts");
    spatialData->SetName(STRING(name));

    STRING coordSysName = L"";
    FdoString* csName = spatialReader->GetCoordinateSystem();

    Ptr<MgCoordinateSystemFactory> csFactory;
    // WKT for co-ordinate system
    FdoString* csWkt = NULL;
    STRING srsWkt = L"";

    bool haveValidCoordSys = false;
    if(NULL != csName && *csName != '\0')
    {
        coordSysName = STRING(csName);
    }
    else
    {
        csWkt = spatialReader->GetCoordinateSystemWkt();
        if (csWkt != NULL && *csWkt != '\0')
        {
            srsWkt = csWkt;
            try
            {
                csFactory = new MgCoordinateSystemFactory();
                coordSysName = csFactory->ConvertWktToCoordinateSystemCode(srsWkt);
                haveValidCoordSys = (coordSysName.size() != 0);
            }
            catch (MgException* e)
            {
                SAFE_RELEASE(e);
            }
            catch(...)
            {
            }
        }
    }

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

    if (csName != NULL && *csName != '\0')
    {
        spatialData->SetCoordinateSystem(STRING(csName));
    }

    // Desc for spatial context
    STRING desc = L"";

    // This flag is obsolete and will be deprecated.
    bool isActive = spatialReader->IsActive();

    if (coordSysOverridden)
    {
        srsWkt = csName;
        desc = MgdFeatureUtil::GetMessage(L"MgCoordinateSystemOverridden");
    }
    else if (spatialContextDefined && !coordSysOverridden)
    {
        // avoid looking one more time after CS in case we have a valid one...
        if (!haveValidCoordSys)
        {
            csWkt = spatialReader->GetCoordinateSystemWkt();
            if(NULL != csWkt && *csWkt != '\0')
                srsWkt = csWkt;

            if (srsWkt.empty())
            {
                try
                {
                    if (csFactory == NULL)
                        csFactory = new MgCoordinateSystemFactory();

                    // Check if the spatial context coordinate system data represents an EPSG
                    // code. If this is the case the WKT data for the EPSG code has to be
                    // retrieved.
                    if (IsEpsgCodeRepresentation(csName))
                    {
                        // This is an EPSG code
                        FdoString* p = NULL;
                        if ((csName[0] == L'E') || (csName[0] == L'e'))
                            p = csName+5;
                        else
                            p = csName;

                        INT32 epsgCode = (INT32)wcstol(p, NULL, 10);

                        // Convert the EPSG numerical code to WKT
                        srsWkt = csFactory->ConvertEpsgCodeToWkt(epsgCode);
                    }
                    else
                    {
                        srsWkt = csFactory->ConvertCoordinateSystemCodeToWkt(STRING(csName));
                    }
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
        }
        FdoString* fdoDesc = spatialReader->GetDescription();
        if(NULL != fdoDesc)
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

    // This flag is obsolete and will be deprecated.
    spatialData->SetActiveStatus(isActive);

    return spatialData.Detach();
}

bool MgdGetSpatialContexts::IsEpsgCodeRepresentation (FdoString *coordSysName)
{
    // If the given coordinate system name is NULL or not NULL but empty
    // return false as those cases do not represent an EPSG code.
    if ((coordSysName == NULL) || (coordSysName[0] == L'\0'))
        return false;

    // Check if the string contains a coded EPSG number ("EPSG:<num>"). If this
    // is the case return true;
    size_t coordSysNameLength  = wcslen(coordSysName);
    if (coordSysNameLength  > 5)
        if (((coordSysName[0] == L'E') || (coordSysName[0] == L'e')) &&
            ((coordSysName[1] == L'P') || (coordSysName[1] == L'p')) &&
            ((coordSysName[2] == L'S') || (coordSysName[2] == L's')) &&
            ((coordSysName[3] == L'G') || (coordSysName[3] == L'g')) &&
            (coordSysName[4] == L':')                                   )
            return true;

    // The string does not contain a coded EPSG number. Next, check if the
    // string represents a numeric value. If this is the case then the value
    // represents an EPSG code and the routine has to return true.
    for (size_t i = 0; i < coordSysNameLength; i++)
        if (!iswdigit(coordSysName[i]))
            return false;

    // This is an EPSG code. Indicate so.
    return true;
}
