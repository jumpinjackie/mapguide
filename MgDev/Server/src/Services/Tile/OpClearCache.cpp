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

#include "OpClearCache.h"
#include "LogManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpClearCache::MgOpClearCache()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpClearCache::~MgOpClearCache()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Executes the operation.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgOpClearCache::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpClearCache::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"ClearCache");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (1 == m_packet.m_NumArguments)
    {
        Ptr<MgSerializable> obj = (MgSerializable*)m_stream->GetObject();
        if (obj->IsOfClass(MapGuide_MapLayer_Map))
        {
            Ptr<MgMap> map = SAFE_ADDREF((MgMap*)obj.p);
            Ptr<MgResourceIdentifier> resource = map->GetResourceId();
            map->SetDelayedLoadResourceService(m_resourceService);

            BeginExecution();

            MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
            MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
            MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

            Validate();

            m_service->ClearCache(map);

            EndExecution();
        }
        else if (obj->IsOfClass(PlatformBase_ResourceService_ResourceIdentifier))
        {
            Ptr<MgResourceIdentifier> resource = SAFE_ADDREF((MgResourceIdentifier*)obj.p);

            BeginExecution();

            MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
            MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
            MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

            Validate();

            m_service->ClearCache(resource);

            EndExecution();
        }
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpClearCache.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgOpClearCache.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()
}
