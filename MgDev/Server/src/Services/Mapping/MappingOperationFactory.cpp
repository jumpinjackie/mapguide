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

#include "ServerMappingServiceDefs.h"
#include "MappingOperationFactory.h"

#include "OpGenerateMap.h"
#include "OpGenerateMapUpdate.h"
#include "OpGeneratePlot.h"
#include "OpGenerateMultiPlot.h"
#include "OpGenerateLegendPlot.h"
#include "OpQueryFeatures.h"
#include "OpQueryFeaturesWms.h"
#include "OpGenerateLegendImage.h"


///----------------------------------------------------------------------------
/// <summary>
/// The default constructor for an MgMappingOperationFactory object. However,
/// since this function is protected, this object should never really be
/// constructed. Rather, it is merely a wrapper class for other static
/// functions.
/// </summary>
///----------------------------------------------------------------------------
MgMappingOperationFactory::MgMappingOperationFactory()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// This static method returns the IMgOperationHandler object that corresponds
/// to the given ID and Version parameters.
/// </summary>
///
/// <param name="operationId">
/// The ID of the requested operation.
/// </param>
///
/// <param name="operationVersion">
/// The version of the requested operation.
/// </param>
///
/// <returns>
/// Returns an IMgOperationHandler object corresponding to the given parameters.
/// Returns NULL if one cannot be found.
/// </returns>
///
/// <exceptions>
/// MgException
/// </exceptions>
///
/// TODO:   handle different versions
/// TODO:   set up ids and whatnot in a hash or map instead of hardcoding the ids here
///----------------------------------------------------------------------------
IMgOperationHandler* MgMappingOperationFactory::GetOperation(
    ACE_UINT32 operationId, ACE_UINT32 operationVersion)
{
    auto_ptr<IMgOperationHandler> handler;

    MG_TRY()
    switch (operationId)
    {
    case MgMappingService::opIdGenerateMap:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGenerateMap());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgMappingService::opIdGenerateMapUpdate:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGenerateMapUpdate());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgMappingService::opIdGeneratePlot:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGeneratePlot());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgMappingService::opIdGeneratePlot2:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGeneratePlot());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgMappingService::opIdGeneratePlot3:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGeneratePlot());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgMappingService::opIdGenerateMultiPlot:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGenerateMultiPlot());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgMappingService::opIdGenerateLegendPlot:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGenerateLegendPlot());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgMappingService::opIdGenerateLegendImage:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGenerateLegendImage());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgMappingService::opIdQueryFeatures:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpMQueryFeatures());
            break;
        default:
            throw new MgInvalidOperationVersionException(
            L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgMappingService::opIdQueryFeaturesWms:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpQueryFeaturesWms());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    default:
        throw new MgInvalidOperationException(
            L"MgMappingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgMappingOperationFactory.GetOperation")

    return handler.release();
}
