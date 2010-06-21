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

#include "MapGuideCommon.h"
#include "RenderingOperationFactory.h"
#include "RenderingOperation.h"


///////////////////////////////////////////////////////////////////////////
/// <summary>
/// The default constructor for an MgRenderingOperationFactory object.  However, since
/// this function is protected, this object should never really be
/// constructed.  Rather, it is merely a wrapper class for other static
/// functions.
/// </summary>
MgRenderingOperationFactory::MgRenderingOperationFactory()
{
}


///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// This static method returns the IMgOperationHandler object that corresponds
/// to the given ID and Version parameters.
/// </summary>
/// <param name="operationId">
/// The ID of the requested operation.
/// </param>
/// <param name="operationVersion">
/// The version of the requested operation.
/// </param>
/// <returns>
/// Returns an IMgOperationHandler object corresponding to the given parameters.
/// Returns NULL if one cannot be found.
/// </returns>
/// <exceptions>
/// An MgException is thrown on failure.
/// </exceptions>
/// TODO:   handle different versions
/// TODO:   set up ids and whatnot in a hash or map instead of hardcoding the ids here
IMgOperationHandler* MgRenderingOperationFactory::GetOperation(
    ACE_UINT32 operationId, ACE_UINT32 operationVersion)
{
    auto_ptr<IMgOperationHandler> handler;

    MG_TRY()

    switch (operationId)
    {
    case MgRenderingServiceOpId::RenderTile:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpRenderTile());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgRenderingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgRenderingServiceOpId::RenderDynamicOverlay:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(2,0):
        case VERSION_SUPPORTED(2,1):
            handler.reset(new MgOpRenderDynamicOverlay());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgRenderingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgRenderingServiceOpId::RenderMap1:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpRenderMap());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgRenderingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgRenderingServiceOpId::RenderMap2:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpRenderMap());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgRenderingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgRenderingServiceOpId::RenderMap3:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpRenderMap());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgRenderingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgRenderingServiceOpId::RenderMap4:
        switch (VERSION_NO_PHASE(operationVersion))
        {
            // TODO: Should this only be supported in VERSION (2,0)? -ez
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpRenderMap());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgRenderingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgRenderingServiceOpId::QueryFeatures:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpQueryFeatures());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgRenderingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;
    case MgRenderingServiceOpId::QueryFeatureProperties:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpQueryFeatureProperties());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgRenderingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;
    case MgRenderingServiceOpId::RenderMapLegend:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpRenderMapLegend());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgRenderingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    default:
        throw new MgInvalidOperationException(
            L"MgRenderingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgRenderingOperationFactory.GetOperation")

    return handler.release();
}
