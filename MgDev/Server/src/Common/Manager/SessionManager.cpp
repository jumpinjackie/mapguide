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

#include "SessionManager.h"
#include "LogManager.h"

ACE_Recursive_Thread_Mutex MgSessionManager::sm_mutex;
Ptr<MgSessionCache> MgSessionManager::sm_sessionCache = new MgSessionCache;

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgSessionManager::MgSessionManager()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgSessionManager::~MgSessionManager()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Clean up expired sessions in the session cache.
///
void MgSessionManager::CleanUpSessions(INT32 sessionTimeout,
    MgStringCollection* expiredSessions)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    sm_sessionCache->CleanUpSessions(sessionTimeout, expiredSessions);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Add the specified session to the session cache.
///
void MgSessionManager::AddSession(CREFSTRING session, CREFSTRING user)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    sm_sessionCache->AddSession(session, user);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Remove the specified session from the session cache.
///
void MgSessionManager::RemoveSession(CREFSTRING session)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    sm_sessionCache->RemoveSession(session);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return the name of the user in the specified session.
///
STRING MgSessionManager::GetUserName(CREFSTRING session)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, L""));

    MgSessionInfo* sessionInfo = sm_sessionCache->GetSessionInfo(session);
    assert(NULL != sessionInfo);

    return sessionInfo->GetUser();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Update the last accessed time, then return the name of the user in the
/// specified session.
///
STRING MgSessionManager::UpdateLastAccessedTime(CREFSTRING session)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, L""));

    MgSessionInfo* sessionInfo = sm_sessionCache->GetSessionInfo(session);
    assert(NULL != sessionInfo);

    sessionInfo->SetAccessedTime(ACE_High_Res_Timer::gettimeofday());

    return sessionInfo->GetUser();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Update the specified session with the current operation information.
///
void MgSessionManager::UpdateCurrentOperationInfo(const MgConnection& connection)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_TRY()

    MgSessionInfo* sessionInfo = sm_sessionCache->GetSessionInfo(connection.GetSessionId());
    assert(NULL != sessionInfo);

    sessionInfo->SetOperationInfo(connection);

    MG_CATCH_AND_RELEASE()
}
