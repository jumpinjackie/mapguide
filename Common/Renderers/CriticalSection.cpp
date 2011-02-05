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

#include "stdafx.h"
#include "CriticalSection.h"

///////////////////////////////////////////////////////////////////////////////
/// Construct the object. The critical section is initialized.
///
CustomThreadMutex::CustomThreadMutex()
{
#ifdef _WIN32
    ::InitializeCriticalSection(&m_criticalSection);
#else
    pthread_mutexattr_init(&m_mutexAttributes);
    pthread_mutexattr_settype(&m_mutexAttributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_criticalSection, &m_mutexAttributes);
    pthread_mutexattr_destroy(&m_mutexAttributes);
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// Destruct the object. The critical section is terminated.
///
CustomThreadMutex::~CustomThreadMutex()
{
#ifdef _WIN32
    ::DeleteCriticalSection(&m_criticalSection);
#else
    pthread_mutex_destroy(&m_criticalSection);
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// Enter a critical section.
///
void CustomThreadMutex::Lock()
{
#ifdef _WIN32
    ::EnterCriticalSection(&m_criticalSection);
#else
    pthread_mutex_lock(&m_criticalSection);
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// Exit a critical section.
///
void CustomThreadMutex::Unlock()
{
#ifdef _WIN32
    ::LeaveCriticalSection(&m_criticalSection);
#else
    pthread_mutex_unlock(&m_criticalSection);
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// Construct the object. The critical section is automatically entered.
///
AutoMutexLocker::AutoMutexLocker(CustomThreadMutex& mutex) :
    m_mutex(mutex)
{
    m_mutex.Lock();
}

///////////////////////////////////////////////////////////////////////////////
/// Destruct the object. The critical section is automatically exited.
///
AutoMutexLocker::~AutoMutexLocker()
{
    m_mutex.Unlock();
}
