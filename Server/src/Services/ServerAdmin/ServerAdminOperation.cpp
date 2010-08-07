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
#include "ServerAdminService.h"
#include "ServerAdminOperation.h"
#include "ServiceManager.h"
#include "LogManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
MgServerAdminOperation::MgServerAdminOperation()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgServerAdminOperation::~MgServerAdminOperation()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Initialize the operation with the given stream data and operation packet.
/// </summary>
///----------------------------------------------------------------------------
void MgServerAdminOperation::Initialize(MgStreamData* data, const MgOperationPacket& packet)
{
    // Let the base class do its things so that current user information can be
    // saved into the thread local storage.
    MgServiceOperation::Initialize(data, packet);

    // Initialize the service.
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    assert(NULL != serviceManager);

    m_service = dynamic_cast<MgServerAdminService*>(
        serviceManager->RequestService(MgServiceType::ServerAdminService));
    assert(m_service != NULL);
}


///----------------------------------------------------------------------------
/// <summary>
/// Gets the role(s) required to perform this operation.
/// </summary>
///----------------------------------------------------------------------------
MgStringCollection* MgServerAdminOperation::GetRoles() const
{
    return GetAdministratorRole();
}


///----------------------------------------------------------------------------
/// <summary>
/// Perform validation on the operation
/// </summary>
///----------------------------------------------------------------------------
void MgServerAdminOperation::Validate()
{
    AuthenticateCurrentUser();
}