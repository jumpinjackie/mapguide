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
#include "DrawingServiceHandler.h"
#include "DrawingOperationFactory.h"
#include "ServerManager.h"
#include "LogManager.h"

//-------------------------------------------------------------------------
//  Constructors/Destructor
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//
//  <summary>
//  Constructs a MgDrawingServiceHandler object with the given parameters.
//  </summary>
//
//  <param name = "data">
//  A pointer the the StreamData object that encapsulates the incoming
//  stream information.
//  </param>
//
//  <param name = "packet">
//  The Operation Packet that has already been read from the incoming
//  data stream.
//  </param>
MgDrawingServiceHandler::MgDrawingServiceHandler(MgStreamData* data,
    const MgOperationPacket& packet) :
    IMgServiceHandler(data, packet)
{
}

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  The destructor for the MgDrawingServiceHandler object.
//  </summary>
MgDrawingServiceHandler::~MgDrawingServiceHandler( void )
{
}

//-------------------------------------------------------------------------
//  Methods/Functions
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Processes the appropriate operation using the Operation Packet and
//  StreamData objects given during construction.
//  Overrides IServiceHander::ProcessOperation().
//  </summary>
IMgServiceHandler::MgProcessStatus MgDrawingServiceHandler::ProcessOperation()
{
    IMgServiceHandler::MgProcessStatus status = IMgServiceHandler::mpsError;

    try
    {
        auto_ptr<IMgOperationHandler> handler(
            MgDrawingOperationFactory::GetOperation(
            m_packet.m_OperationID, m_packet.m_OperationVersion));

        if (0 != handler.get())
        {
            handler->Init(m_data, m_packet);
            handler->Execute();

            status = IMgServiceHandler::mpsDone;
        }
    }
    catch (MgOperationProcessingException* e )
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(e->GetMessage(pServerManager->GetDefaultLocale()).c_str(), e->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        SAFE_RELEASE(e);
    }
    catch (MgException* e)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(e->GetMessage(pServerManager->GetDefaultLocale()).c_str(), e->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        //  set handler status to mpsDone as we assume we can still propess operations
        status = IMgServiceHandler::mpsDone;

        SAFE_RELEASE(e);
    }
    catch (...)
    {
    }

    return status;
}
