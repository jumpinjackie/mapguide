//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "DrawingOperationFactory.h"

#include "OpGetDrawing.h"
#include "OpDescribeDrawing.h"
#include "OpGetSectionResource.h"
#include "OpGetSection.h"
#include "OpEnumerateLayers.h"
#include "OpGetLayer.h"
#include "OpEnumerateSections.h"
#include "OpEnumerateSectionResources.h"
#include "OpGetCoordinateSpace.h"


///////////////////////////////////////////////////////////////////////////
/// <summary>
/// The default constructor for an MgDrawingOperationFactory object.  However, since
/// this function is protected, this object should never really be
/// constructed.  Rather, it is merely a wrapper class for other static
/// functions.
/// </summary>
MgDrawingOperationFactory::MgDrawingOperationFactory()
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
IMgOperationHandler* MgDrawingOperationFactory::GetOperation(
    ACE_UINT32 operationId, ACE_UINT32 operationVersion)
{
    auto_ptr<IMgOperationHandler> handler;

    MG_TRY()

    switch (operationId)
    {
        case MgDrawingServiceOpId::GetDrawing:
            switch (VERSION_NO_PHASE(operationVersion))
            {
            case VERSION_SUPPORTED(1,0):
                handler.reset(new MgOpGetDrawing());
                break;
            default:
                throw new MgInvalidOperationVersionException(
                    L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            break;

        case MgDrawingServiceOpId::DescribeDrawing:
            switch (VERSION_NO_PHASE(operationVersion))
            {
            case VERSION_SUPPORTED(1,0):
                handler.reset(new MgOpDescribeDrawing());
                break;
            default:
                throw new MgInvalidOperationVersionException(
                    L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            break;

        case MgDrawingServiceOpId::GetSection:
            switch (VERSION_NO_PHASE(operationVersion))
            {
            case VERSION_SUPPORTED(1,0):
                handler.reset(new MgOpGetSection());
                break;
            default:
                throw new MgInvalidOperationVersionException(
                    L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            break;

        case MgDrawingServiceOpId::GetSectionResource:
            switch (VERSION_NO_PHASE(operationVersion))
            {
            case VERSION_SUPPORTED(1,0):
                handler.reset(new MgOpGetSectionResource());
                break;
            default:
                throw new MgInvalidOperationVersionException(
                    L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            break;

        case MgDrawingServiceOpId::EnumerateLayers:
            switch (VERSION_NO_PHASE(operationVersion))
            {
            case VERSION_SUPPORTED(1,0):
                handler.reset(new MgOpEnumerateLayers());
                break;
            default:
                throw new MgInvalidOperationVersionException(
                    L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            break;

        case MgDrawingServiceOpId::GetLayer:
            switch (VERSION_NO_PHASE(operationVersion))
            {
            case VERSION_SUPPORTED(1,0):
                handler.reset(new MgOpGetLayer());
                break;
            default:
                throw new MgInvalidOperationVersionException(
                    L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            break;

        case MgDrawingServiceOpId::EnumerateSections:
            switch (VERSION_NO_PHASE(operationVersion))
            {
            case VERSION_SUPPORTED(1,0):
                handler.reset(new MgOpEnumerateSections());
                break;
            default:
                throw new MgInvalidOperationVersionException(
                    L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            break;

        case MgDrawingServiceOpId::EnumerateSectionResources:
            switch (VERSION_NO_PHASE(operationVersion))
            {
            case VERSION_SUPPORTED(1,0):
                handler.reset(new MgOpEnumerateSectionResources());
                break;
            default:
                throw new MgInvalidOperationVersionException(
                    L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            break;

        case MgDrawingServiceOpId::GetCoordinateSpace:
            switch (VERSION_NO_PHASE(operationVersion))
            {
            case VERSION_SUPPORTED(1,0):
                handler.reset(new MgOpGetCoordinateSpace());
                break;
            default:
                throw new MgInvalidOperationVersionException(
                    L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            break;

        default:
            throw new MgInvalidOperationException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgDrawingOperationFactory.GetOperation")

    return handler.release();
}
