//
//  Copyright (C) 2004-2019 by Autodesk, Inc.
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
#include "MetatileLockUtil.h"

std::map<STRING, ACE_Condition<ACE_Recursive_Thread_Mutex>*> MgMetatileLockUtil::sm_lockMap;
ACE_Recursive_Thread_Mutex MgMetatileLockUtil::sm_tileMutex;

MgMetatileLockUtil::MgMetatileLockUtil() { }

MgMetatileLockUtil::~MgMetatileLockUtil() { }

void MgMetatileLockUtil::WaitForLock(CREFSTRING tilePathname)
{
    // protect test of lockmap entry with a guard
    ACE_Condition<ACE_Recursive_Thread_Mutex> *tileLock = 0;
    // look for lock and wait if found
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_tileMutex));
    if (tileLock = sm_lockMap[tilePathname])
    {   // lock has been found
#ifdef _DEBUG
        std::wstringstream text;
        text << L"(" << ACE_OS::thr_self() << L") METATILE: WaitForLock1("
            << tilePathname << L")\n";
        ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
        //                ACE_Time_Value waitTime(METATILEWAIT);
        int ret = tileLock->wait();    // returns -1 if timed out which is not supposed to happen
        if (-1 == ret) // so the wait might never come back!!! and the thread gets cleaned out by the service manager
        {
#ifdef _DEBUG
            std::wstringstream text;
            text << L"(" << ACE_OS::thr_self() << L") METATILE: WaitForLockTimedOut1(" << tilePathname << L")\n";
            ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
            MgStringCollection arguments;
            arguments.Add(tilePathname);
            throw new MgFileIoException(L"MgServerTileService.GetTile",
                __LINE__, __WFILE__, &arguments, L"MgWaitForLockTimedOut", NULL);
        }
#ifdef _DEBUG
        {
            std::wstringstream text;
            text << L"(" << ACE_OS::thr_self() << L") METATILE: LockReleased(" << tilePathname << L")\n";
            ACE_DEBUG((LM_DEBUG, text.str().c_str()));
        }
#endif
    }
}

void MgMetatileLockUtil::ClearLock(CREFSTRING tilePathname)
{
    try
    {   // synchronize access to map
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_tileMutex));
        ACE_Condition<ACE_Recursive_Thread_Mutex> *lock = sm_lockMap[tilePathname];
        if (lock)
        {
            int result = lock->broadcast();    // notify waiters of finished tile
#ifdef _DEBUG
            std::wstringstream text;
            text << L"(" << ACE_OS::thr_self() << L") METATILE: Broadcast(" << tilePathname
                << L") returns: " << result << L"\n";
            ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
            sm_lockMap.erase(tilePathname); // erase only existing lock: this destroys the ace condition
        }
        else
        {
#ifdef _DEBUG
            std::wstringstream text;
            text << L"(" << ACE_OS::thr_self() << L") METATILE: NoLockFor(" << tilePathname << L")\n";
            ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
        }
    }
    catch (MgException* e)
    {
#ifdef _DEBUG
        std::wstringstream text;
        text << L"(" << ACE_OS::thr_self() << L") METATILE: Broadcastcrash (" << tilePathname
            << L")\n" << e->GetExceptionMessage() << L")\n";
        ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
        throw e;
    }
}

void MgMetatileLockUtil::LockTile(CREFSTRING tilePathname)
{
    // create ace condition for this tile as we are going to make it
    ACE_Condition<ACE_Recursive_Thread_Mutex> *tileLock = new ACE_Condition<ACE_Recursive_Thread_Mutex>(sm_tileMutex);
    // keep reference to lock (ace condition) in our map
    sm_lockMap[tilePathname] = tileLock;

#ifdef _DEBUG
    std::wstringstream text;
    text << L"(" << ACE_OS::thr_self() << L") METATILE: CreateLock(" << tilePathname << L"\n";
    ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
}