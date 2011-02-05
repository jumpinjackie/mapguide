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

#include "Foundation.h"


//////////////////////////////////////////////////////////////
INT32 MgDisposable::GetRefCount()
{
#ifdef _DEBUG
    // conditionally acquire the lock
    ACE_Guard<ACE_Recursive_Thread_Mutex> ace_mon(m_dbg_mutex, 0);

    // if this check fails then this object should be derived from MgGuardDisposable and not MgDisposable
    if (ace_mon.locked() == 0)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) ************ Concurrent refcount access in MgDisposable::GetRefCount(). Class Name: %W. This object should derive from MgGuardDisposable and not MgDisposable.\n"), GetClassName().c_str()));
        assert(false);
    }
#endif

    return m_refCount;
}


//////////////////////////////////////////////////////////////
INT32 MgDisposable::AddRef()
{
#ifdef _DEBUG
    // conditionally acquire the lock
    ACE_Guard<ACE_Recursive_Thread_Mutex> ace_mon(m_dbg_mutex, 0);

    // if this check fails then this object should be derived from MgGuardDisposable and not MgDisposable
    if (ace_mon.locked() == 0)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) ************ Concurrent refcount access in MgDisposable::AddRef(). Class Name: %W. This object should derive from MgGuardDisposable and not MgDisposable.\n"), GetClassName().c_str()));
        assert(false);
    }
#endif

    m_refCountFlag = true;

    if (0 == m_refCount)
    {
#ifdef _DEBUG
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) ************ Error in MgDisposable::Addref(). Class Name: %W. Called with Reference Count = 0.\n"), GetClassName().c_str()));
#endif

        throw new MgLogicException(L"MgDisposable.AddRef", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return ++m_refCount;
}


//////////////////////////////////////////////////////////////
INT32 MgDisposable::Release()
{
    {
#ifdef _DEBUG
        // conditionally acquire the lock
        ACE_Guard<ACE_Recursive_Thread_Mutex> ace_mon(m_dbg_mutex, 0);

        // if this check fails then this object should be derived from MgGuardDisposable and not MgDisposable
        if (ace_mon.locked() == 0)
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) ************ Concurrent refcount access in MgDisposable::Release(). Class Name: %W. This object should derive from MgGuardDisposable and not MgDisposable.\n"), GetClassName().c_str()));
            assert(false);
        }
#endif

        m_refCountFlag = true;

        if (0 >= m_refCount)
        {
#ifdef _DEBUG
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) ************ Error in MgDisposable::Release(). Class Name: %W. Called with Reference Count <= 0.\n"), GetClassName().c_str()));
#endif

            throw new MgLogicException(L"MgDisposable.Release", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        --m_refCount;

        if (0 != m_refCount)
        {
            return m_refCount;
        }
    }

    // if we get here, the reference count must be zero, so dispose of this object
    Dispose();
    return 0;
}


//////////////////////////////////////////////////////////////
void MgDisposable::SetRefCountFlag()
{
    m_refCountFlag = true;
}


//////////////////////////////////////////////////////////////
void MgDisposable::ResetRefCountFlag()
{
    m_refCountFlag = false;
}


//////////////////////////////////////////////////////////////
MgDisposable::MgDisposable() :
    m_refCount(1),
    m_refCountFlag(false)
{
}


//////////////////////////////////////////////////////////////
MgDisposable::~MgDisposable()
{
#ifdef _DEBUG
    if ((m_refCountFlag) && (m_refCount != 0))
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) ************ Error in MgDisposable::~MgDisposable(). Class Name: %W. Actual Reference Count: %d. Expected Reference Count: 0. This object should have been created on the heap instead of on the stack.\n"), GetClassName().c_str(), m_refCount));
    }
    else if ((!m_refCountFlag) && (m_refCount != 1))
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) ************ Error in MgDisposable::~MgDisposable(). Class Name: %W. Actual Reference Count: %d. Expected Reference Count: 1. This object should have been created on the stack instead of on the heap.\n"), GetClassName().c_str(), m_refCount));
    }
#endif

    ACE_ASSERT(m_refCountFlag ? (m_refCount == 0) : (m_refCount == 1));
}
