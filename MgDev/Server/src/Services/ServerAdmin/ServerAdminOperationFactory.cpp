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

#include "AceCommon.h"
#include "ServerAdminService.h"
#include "ServerAdminOperationFactory.h"
#include "ServerAdminOperation.h"
#include "LogManager.h"

#include "OpBringOnline.h"
#include "OpClearLog.h"
#include "OpDeleteLog.h"
#include "OpDeletePackage.h"
#include "OpEnableMaximumLogSize.h"
#include "OpEnumerateLogs.h"
#include "OpEnumeratePackages.h"
#include "OpGetConfigurationProperties.h"
#include "OpGetDocument.h"
#include "OpGetInformationProperties.h"
#include "OpGetLog.h"
#include "OpGetLogFile.h"
#include "OpGetPackageLog.h"
#include "OpGetPackageStatus.h"
#include "OpIsMaximumLogSizeEnabled.h"
#include "OpIsOnline.h"
#include "OpLoadPackage.h"
#include "OpNotifyResourcesChanged.h"
#include "OpRegisterServicesOnServers.h"
#include "OpRenameLog.h"
#include "OpSetConfigurationProperties.h"
#include "OpSetDocument.h"
#include "OpSetLogDelimiter.h"
#include "OpSetMaximumLogSize.h"
#include "OpTakeOffline.h"
#include "OpUnregisterServicesOnServers.h"

///////////////////////////////////////////////////////////////////////////
/// <summary>
/// The default constructor for an MgServerAdminOperationFactory object.  However, since
/// this function is protected, this object should never really be
/// constructed.  Rather, it is merely a wrapper class for other static
/// functions.
/// </summary>

MgServerAdminOperationFactory::MgServerAdminOperationFactory()
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

IMgOperationHandler* MgServerAdminOperationFactory::GetOperation(
    ACE_UINT32 operationId, ACE_UINT32 operationVersion)
{
    auto_ptr<IMgOperationHandler> handler;

    MG_TRY()

    switch (operationId)
    {
    case MgServerAdminServiceOpId::GetConfigurationProperties:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpGetConfigurationProperties());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::SetConfigurationProperties:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpSetConfigurationProperties());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::BringOnline:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpBringOnline());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::TakeOffline:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpTakeOffline());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::IsOnline:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpIsOnline());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::GetLog:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpGetLog());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::GetLogFile:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpGetLogFile());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::ClearLog:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpClearLog());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::DeleteLog:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpDeleteLog());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::RenameLog:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpRenameLog());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::EnumerateLogs:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpEnumerateLogs());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::DeletePackage:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpDeletePackage());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::EnumeratePackages:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpEnumeratePackages());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::GetPackageLog:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpGetPackageLog());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::GetPackageStatus:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpGetPackageStatus());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::LoadPackage:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpLoadPackage());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::GetInformationProperties:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpGetInformationProperties());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::RegisterServicesOnServers :
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpRegisterServicesOnServers());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::UnregisterServicesOnServers :
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpUnregisterServicesOnServers());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::NotifyResourcesChanged :
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpNotifyResourcesChanged());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::SetMaximumLogSize :
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpSetMaximumLogSize());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::SetLogsDelimiter :
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpSetLogDelimiter());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::EnableMaximumLogSize :
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpEnableMaximumLogSize());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::IsMaximumLogSizeEnabled :
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpIsMaximumLogSizeEnabled());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::GetDocument:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpGetDocument());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgServerAdminServiceOpId::SetDocument:
        switch (operationVersion)
        {
        case 1:
            handler.reset(new MgOpSetDocument());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    default:
        throw new MgInvalidOperationException(
            L"MgServerAdminOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgServerAdminOperationFactory.GetOperation")

    return handler.release();
}
