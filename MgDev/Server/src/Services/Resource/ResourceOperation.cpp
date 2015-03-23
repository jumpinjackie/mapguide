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

#include "ResourceServiceDefs.h"
#include "ResourceOperation.h"
#include "ServiceManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
MgResourceOperation::MgResourceOperation()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgResourceOperation::~MgResourceOperation()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Initialize the operation with the given stream data and operation packet.
/// </summary>
///----------------------------------------------------------------------------
void MgResourceOperation::Initialize(MgStreamData* data, const MgOperationPacket& packet)
{
    // Let the base class do its things so that current user information can be
    // saved into the thread local storage.
    MgServiceOperation::Initialize(data, packet);

    // Initialize the service.
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    assert(NULL != serviceManager);

    m_service = dynamic_cast<MgResourceService*>(
        serviceManager->RequestService(MgServiceType::ResourceService));
    assert(m_service != NULL);
}


///----------------------------------------------------------------------------
/// <summary>
/// Perform validation on the operation
/// </summary>
///----------------------------------------------------------------------------
void MgResourceOperation::Validate()
{
    CheckLicense();
    AuthenticateCurrentUser();
}
