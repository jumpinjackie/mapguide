//
//  Copyright (C) 2010 by Autodesk, Inc.
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

#ifndef CRITICALSECTION_H_
#define CRITICALSECTION_H_

#ifndef _WIN32
#  include <pthread.h>
#  ifndef __bsdi__
#    include <semaphore.h>
#  endif
#endif

// Critical Section Class
class MgdMutex
{
public:

    MgdMutex()
    {
        m_bInitialized = false;
        Initialize();
    }

#ifdef _WIN32

    ~MgdMutex()
    {
        ::DeleteCriticalSection(&m_CritSect);
    }

    void Initialize()
    {
        if(!m_bInitialized)
        {
            m_bInitialized = true;
            ::InitializeCriticalSection(&m_CritSect);
        }
    }

    void Enter()
    {
        ::EnterCriticalSection(&m_CritSect);
    }

    void Leave()
    {
        ::LeaveCriticalSection(&m_CritSect);
    }

private:
    CRITICAL_SECTION m_CritSect;

#else //LINUX

    ~MgdMutex()
    {
        pthread_MgdMutex_destroy(&m_CritSect);
    }

    void Initialize()
    {
        if(!m_bInitialized)
        {
            m_bInitialized = true;
            pthread_MgdMutexattr_init(&m_attributes);
            pthread_MgdMutexattr_settype(&m_attributes, PTHREAD_MgdMutex_RECURSIVE);
            pthread_MgdMutex_init(&m_CritSect, &m_attributes);
            pthread_MgdMutexattr_destroy(&m_attributes);
        }
    }

    void Enter()
    {
        pthread_MgdMutex_lock(&m_CritSect);
    }

    void Leave()
    {
        pthread_MgdMutex_unlock(&m_CritSect);
    }

private:
    pthread_MgdMutex_t m_CritSect;
    pthread_MgdMutexattr_t m_attributes;

#endif //_WIN32

private:
    bool m_bInitialized;
};


class ScopedLock
{
public:
    ScopedLock(MgdMutex& MgdMutex):m_MgdMutex(MgdMutex)
    {
        m_MgdMutex.Enter();
    }
    ~ScopedLock()
    {
        m_MgdMutex.Leave();
    }
private:
    ScopedLock();
    MgdMutex& m_MgdMutex;
};

#endif