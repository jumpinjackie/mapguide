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

#ifndef CRITICALSECTION_H_
#define CRITICALSECTION_H_

#ifdef _WIN32
#   include <windows.h>
#else
#   include <pthread.h>
#   ifndef __bsdi__
#      include <semaphore.h>
#   endif
#endif

class CustomThreadMutex
{
public:
    CustomThreadMutex();
    ~CustomThreadMutex();

    void Lock();
    void Unlock();

private:
    // Unimplemented copy constructor and assignment operator.
    CustomThreadMutex(const CustomThreadMutex&);
    CustomThreadMutex& operator=(const CustomThreadMutex&);

#ifdef _WIN32
    CRITICAL_SECTION m_criticalSection;
#else
    pthread_mutex_t m_criticalSection;
    pthread_mutexattr_t m_mutexAttributes;
#endif
};

class AutoMutexLocker
{
public:
    AutoMutexLocker(CustomThreadMutex& mutex);
    ~AutoMutexLocker();

private:
    // Unimplemented copy constructor and assignment operator.
    AutoMutexLocker();
    AutoMutexLocker(const AutoMutexLocker&);
    AutoMutexLocker& operator=(const AutoMutexLocker&);

    CustomThreadMutex& m_mutex;
};

#endif
