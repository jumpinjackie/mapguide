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
#include "ServerConnectionImp.h"
#include "ServerConnectionStack.h"
#include "ServerConnectionPool.h"
#include "ServerConnection.h"
#include "ServerConnectionEventHandler.h"

MgServerConnectionEventHandler::MgServerConnectionEventHandler()
{
}

MgServerConnectionEventHandler::~MgServerConnectionEventHandler()
{
}

int MgServerConnectionEventHandler::handle_timeout(const ACE_Time_Value& currentTime, const void* arg)
{
    MG_TRY()

    ACE_UNUSED_ARG(currentTime);
    ACE_UNUSED_ARG(arg);

    ACE_Time_Value now = ACE_High_Res_Timer::gettimeofday();
    MgServerConnectionPool* pool = MgServerConnectionPool::GetInstance();
    if (NULL != pool)
    {
        pool->CloseStaleConnections(&now);
    }

    MG_CATCH_AND_RELEASE()
    return 0;
}
