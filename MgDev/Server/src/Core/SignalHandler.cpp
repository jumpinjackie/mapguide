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
#include "SignalHandler.h"
#include "LogManager.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgSignalHandler::MgSignalHandler(int nSignal) :
    m_nSignal(nSignal)
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgSignalHandler::~MgSignalHandler()
{
}

int MgSignalHandler::handle_signal(int signum, siginfo_t * psiginfo, ucontext_t * pucontext)
{
    ACE_UNUSED_ARG(psiginfo);
    ACE_UNUSED_ARG(pucontext);

    if(MgLogManager::GetInstance()->IsTraceLogEnabled())
    {
        wchar_t buffer[255];
        STRING temp;
        swprintf(buffer, 255, L"MgSignalHandler::handle_signal() - Received Signal: %d", signum);
        temp = buffer;
        MG_LOG_TRACE_ENTRY(temp);
    }

    // Check to ensure that this signal is meant for us
    ACE_ASSERT(signum == m_nSignal);

    ACE_Reactor::instance()->notify(this);

    return 0;
}

int MgSignalHandler::handle_exception(ACE_HANDLE handle)
{
    ACE_UNUSED_ARG(handle);

    MG_LOG_TRACE_ENTRY(L"MgSignalHandler::handle_exception() - Processing signal...shutting down the Reactor.");
    ACE_Reactor::instance()->end_reactor_event_loop();

    return 0;
}
