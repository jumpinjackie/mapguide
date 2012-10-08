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

#ifdef _WIN32
#   include <Windows.h>
#else
#  include <pthread.h>
#  ifndef __bsdi__
#    include <semaphore.h>
#  endif
#endif

#include "CriticalSection.h"

#ifdef _WIN32
    CRITICAL_SECTION CritSect;
#else //LINUX
    pthread_mutex_t CritSect;
    pthread_mutexattr_t CritSectAttributes;
#endif

//declare the "global" instance of our CustomCriticalSection object we're using throughout the whole process
CustomCriticalSection CriticalClass;

CustomCriticalSection::CustomCriticalSection()
{
    m_bInitialized = false;
    Initialize();
}

#ifdef _WIN32

CustomCriticalSection::~CustomCriticalSection()
{
    ::DeleteCriticalSection(&CritSect);
}

void CustomCriticalSection::Initialize()
{
    if(!m_bInitialized)
    {
        m_bInitialized = true;
        ::InitializeCriticalSection(&CritSect);
    }
}

void CustomCriticalSection::Enter()
{
    ::EnterCriticalSection(&CritSect);
}

void CustomCriticalSection::Leave()
{
    ::LeaveCriticalSection(&CritSect);
}

#ifdef _DEBUG
    bool CustomCriticalSection::IsEntered()
    {
        if (!::TryEnterCriticalSection(&CritSect))
            return false;

        bool wasHeld = (CritSect.RecursionCount > 0);
        ::LeaveCriticalSection(&CritSect);

        return wasHeld;
    }
#endif

#else //LINUX

CustomCriticalSection::~CustomCriticalSection()
{
    pthread_mutex_destroy(&CritSect);
}

void CustomCriticalSection::Initialize()
{
    if(!m_bInitialized)
    {
        m_bInitialized = true;
        pthread_mutexattr_init(&CritSectAttributes);
        pthread_mutexattr_settype(&CritSectAttributes, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&CritSect, &CritSectAttributes);
        pthread_mutexattr_destroy(&CritSectAttributes);
    }
}

void CustomCriticalSection::Enter()
{
    pthread_mutex_lock(&CritSect);
}

void CustomCriticalSection::Leave()
{
    pthread_mutex_unlock(&CritSect);
}

#ifdef _DEBUG
    bool CustomCriticalSection::IsEntered()
    {
        return true;
    }
#endif

#endif //_WIN32 / LINUX
