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

#include "MapGuideCommon.h"
#include "ProfilingServiceHandler.h"
#include "ProfilingOperationFactory.h"
#include "ServerManager.h"
#include "LogManager.h"

//-------------------------------------------------------------------------
//  Constructors/Destructor
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//
//  <summary>
//  Constructs a MgProfilingServiceHandler object with the given parameters.
//  </summary>
//
//  <param name = "data">
//  A pointer the the MgStreamData object that encapsulates the incoming
//  stream information.
//  </param>
//
//  <param name = "packet">
//  The Operation Packet that has already been read from the incoming
//  data stream.
//  </param>
MgProfilingServiceHandler::MgProfilingServiceHandler(MgStreamData* data,
    const MgOperationPacket& packet) :
    IMgServiceHandler(data, packet)
{
}

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  The destructor for the MgProfilingServiceHandler object.
//  </summary>
MgProfilingServiceHandler::~MgProfilingServiceHandler( void )
{
}

//-------------------------------------------------------------------------
//  Methods/Functions
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Processes the appropriate operation using the Operation Packet and
//  MgStreamData objects given during construction.
//  Overrides IServiceHander::ProcessOperation().
//  </summary>
IMgServiceHandler::MgProcessStatus MgProfilingServiceHandler::ProcessOperation()
{
    IMgServiceHandler::MgProcessStatus status = IMgServiceHandler::mpsError;
    auto_ptr<IMgOperationHandler> handler;

    MG_TRY()

    handler.reset(MgProfilingOperationFactory::GetOperation(
        m_packet.m_OperationID, m_packet.m_OperationVersion));
    assert(NULL != handler.get());

    handler->Initialize(m_data, m_packet);
    handler->Execute();

    status = IMgServiceHandler::mpsDone;

    MG_CATCH(L"MgProfilingServiceHandler.ProcessOperation")

    if (mgException != NULL && NULL != handler.get())
    {
        status = (handler.get()->HandleException(mgException) ?
            IMgServiceHandler::mpsDone : IMgServiceHandler::mpsError);
    }

    if (IMgServiceHandler::mpsDone != status)
    {
        MG_THROW();
    }

    return status;
}
