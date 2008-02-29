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

#include "ResourceServiceDefs.h"
#include "ResourceOperationFactory.h"
// Repository Management APIs
#include "OpEnumerateRepositories.h"
#include "OpCreateRepository.h"
#include "OpDeleteRepository.h"
#include "OpUpdateRepository.h"
#include "OpGetRepositoryContent.h"
#include "OpGetRepositoryHeader.h"
#include "OpApplyResourcePackage.h"
// Resource Management APIs
#include "OpEnumerateResources.h"
#include "OpSetResource.h"
#include "OpDeleteResource.h"
#include "OpMoveResource.h"
#include "OpCopyResource.h"
#include "OpGetResourceContent.h"
#include "OpGetResourceHeader.h"
#include "OpGetResourceModifiedDate.h"
#include "OpEnumerateResourceReferences.h"
#include "OpEnumerateParentMapDefinitions.h"
#include "OpChangeResourceOwner.h"
#include "OpInheritPermissionsFrom.h"
// Resource Data Management APIs
#include "OpEnumerateResourceData.h"
#include "OpSetResourceData.h"
#include "OpDeleteResourceData.h"
#include "OpRenameResourceData.h"
#include "OpGetResourceData.h"
// Unmanaged Data APIs
#include "OpEnumerateUnmanagedData.h"


///----------------------------------------------------------------------------
/// <summary>
/// The default constructor for an MgResourceOperationFactory object. However,
/// since this function is protected, this object should never really be
/// constructed. Rather, it is merely a wrapper class for other static
/// functions.
/// </summary>
///----------------------------------------------------------------------------
MgResourceOperationFactory::MgResourceOperationFactory()
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
IMgOperationHandler* MgResourceOperationFactory::GetOperation(
    ACE_UINT32 operationId, ACE_UINT32 operationVersion)
{
    auto_ptr<IMgOperationHandler> handler;

    MG_RESOURCE_SERVICE_TRY()

    switch (operationId)
    {
    case MgResourceService::opIdEnumerateRepositories:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpEnumerateRepositories());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdCreateRepository:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpCreateRepository());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdDeleteRepository:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpDeleteRepository());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdUpdateRepository:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpUpdateRepository());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdGetRepositoryContent:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetRepositoryContent());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdGetRepositoryHeader:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetRepositoryHeader());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdApplyResourcePackage:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpApplyResourcePackage());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdEnumerateResources:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpEnumerateResources());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdSetResource:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpSetResource());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdDeleteResource:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpDeleteResource());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdMoveResource:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpMoveResource());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdCopyResource:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpCopyResource());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdGetResourceContent:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetResourceContent());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdGetResourceHeader:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetResourceHeader());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdGetResourceModifiedDate:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetResourceModifiedDate());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdEnumerateReferences:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpEnumerateResourceReferences());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdEnumerateParentMapDefinitions:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpEnumerateParentMapDefinitions());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdChangeResourceOwner:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpChangeResourceOwner());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdInheritPermissionsFrom:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpInheritPermissionsFrom());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdEnumerateResourceData:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpEnumerateResourceData());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdSetResourceData:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpSetResourceData());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdDeleteResourceData:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpDeleteResourceData());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdRenameResourceData:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpRenameResourceData());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdGetResourceData:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetResourceData());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgResourceService::opIdEnumerateUnmanagedData:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpEnumerateUnmanagedData());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    default:
        throw new MgInvalidOperationException(
            L"MgResourceOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceOperationFactory.GetOperation")

    return handler.release();
}
