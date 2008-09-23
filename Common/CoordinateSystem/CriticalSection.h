//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
class CustomCriticalSection
{
public:

    CustomCriticalSection()
    {
        m_bInitialized = false;
        Initialize();
    }

#ifdef _WIN32

    ~CustomCriticalSection()
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

    ~CustomCriticalSection()
    {
        pthread_mutex_destroy(&m_CritSect);
    }

    void Initialize()
    {
        if(!m_bInitialized)
        {
            m_bInitialized = true;
            pthread_mutexattr_init(&m_attributes);
            pthread_mutexattr_settype(&m_attributes, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(&m_CritSect, &m_attributes);
            pthread_mutexattr_destroy(&m_attributes);
        }
    }

    void Enter()
    {
        pthread_mutex_lock(&m_CritSect);
    }

    void Leave()
    {
        pthread_mutex_unlock(&m_CritSect);
    }

private:
    pthread_mutex_t m_CritSect;
    pthread_mutexattr_t m_attributes;

#endif //_WIN32

private:
    bool m_bInitialized;
};

// This is used to protect library calls
static CustomCriticalSection CriticalClass;

class SmartCriticalClass
{
public:
    SmartCriticalClass(bool bEnter):m_bEntered(bEnter)
    {
        if (bEnter)
        {
            CriticalClass.Enter();
        }
    }
    void Enter()
    {
        m_bEntered=true;
        CriticalClass.Enter();
    }
    ~SmartCriticalClass()
    {
        if (m_bEntered)
        {
            CriticalClass.Leave();
        }
    }
private:
    SmartCriticalClass();
    bool m_bEntered;
};

#endif
