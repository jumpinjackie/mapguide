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

#include "SiteServiceDefs.h"
#include "SiteServiceUtil.h"
#include "SiteOperationFactory.h"
#include "SiteOperation.h"

//  User Operations
#include "OpAuthenticate.h"
#include "OpEnumerateUsers.h"
#include "OpAddUser.h"
#include "OpUpdateUser.h"
#include "OpDeleteUsers.h"
#include "OpGrantRoleMembershipsToUsers.h"
#include "OpRevokeRoleMembershipsFromUsers.h"
#include "OpGrantGroupMembershipsToUsers.h"
#include "OpRevokeGroupMembershipsFromUsers.h"
#include "OpCreateSession.h"
#include "OpDestroySession.h"
#include "OpGetSessionTimeout.h"

//  Group Operations
#include "OpEnumerateGroups.h"
#include "OpAddGroup.h"
#include "OpUpdateGroup.h"
#include "OpDeleteGroups.h"
#include "OpGrantRoleMembershipsToGroups.h"
#include "OpRevokeRoleMembershipsFromGroups.h"

//  Role Operations
#include "OpEnumerateRoles.h"

//  Server Operations
#include "OpEnumerateServers.h"
#include "OpAddServer.h"
#include "OpRemoveServer.h"
#include "OpUpdateServer.h"
#include "OpRequestServer.h"


//-------------------------------------------------------------------------
//  Constructors/Destructor
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  The default constructor for an MgSiteOperationFactory object.  However, since
//  this function is protected, this object should never really be
//  constructed.  Rather, it is merely a wrapper class for other static
//  functions.
//  </summary>
MgSiteOperationFactory::MgSiteOperationFactory()
{
}


//-------------------------------------------------------------------------
//  Methods/Functions
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// This static method returns the IMgOperationHandler object that corresponds
/// to the given ID and Version parameters.
/// </summary>
///
/// <param name="operationId">
/// The ID of the requested operation.
/// </param>
//
/// <param name="operationVersion">
/// The version of the requested operation.
/// </param>
//
/// <returns>
/// Returns an IMgOperationHandler object corresponding to the given parameters.
/// Returns NULL if one cannot be found.
/// </returns>
//
/// <exceptions>
/// An MgException is thrown on failure.
/// </exceptions>
///
/// TODO:   handle different versions
IMgOperationHandler* MgSiteOperationFactory::GetOperation(
    ACE_UINT32 operationId, ACE_UINT32 operationVersion)
{
    auto_ptr<IMgOperationHandler> handler;

    MG_SITE_SERVICE_TRY()

    switch (operationId)
    {
    case MgSiteOpId::Authenticate:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpAuthenticate() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::EnumerateUsers:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpEnumerateUsers() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::AddUser:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpAddUser() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::UpdateUser:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpUpdateUser() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::DeleteUsers:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpDeleteUsers() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::GrantRoleMembershipsToUsers:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpGrantRoleMembershipsToUsers() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::RevokeRoleMembershipsFromUsers:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpRevokeRoleMembershipsFromUsers() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::GrantGroupMembershipsToUsers:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpGrantGroupMembershipsToUsers() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::RevokeGroupMembershipsFromUsers:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpRevokeGroupMembershipsFromUsers() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::EnumerateGroups:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpEnumerateGroups() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::AddGroup:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpAddGroup() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::UpdateGroup:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpUpdateGroup() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::DeleteGroups:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpDeleteGroups() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::GrantRoleMembershipsToGroups:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpGrantRoleMembershipsToGroups() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::RevokeRoleMembershipsFromGroups:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpRevokeRoleMembershipsFromGroups() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::EnumerateRoles:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpEnumerateRoles() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::EnumerateServers:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpEnumerateServers() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::AddServer:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpAddServer() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::RemoveServer:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpRemoveServer() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::UpdateServer:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpUpdateServer() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::RequestServer:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpRequestServer() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::CreateSession:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpCreateSession() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::DestroySession:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpDestroySession() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::GetSessionTimeout:
        switch(VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset( new MgOpGetSessionTimeout() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgSiteOpId::GetUserForSession:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(2,0):
            handler.reset( new MgOpGetUserForSession() );
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgDrawingOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    default:
        throw new MgInvalidOperationException(L"GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgSiteOperationFactory::GetOperation" );

    return handler.release();
}
