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

#ifndef _MGDISPOSABLE_H_
#define _MGDISPOSABLE_H_

#ifdef _DEBUG
#include "ace/Process_Manager.h"
#endif

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// MgDisposable is the base interface for all classes that implement this
/// standard interface for reference counting and object destruction.
class MgDisposable;
template class MG_FOUNDATION_API Ptr<MgDisposable>;

class MG_FOUNDATION_API MgDisposable : public MgObject
{
EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default destructor for this class.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual ~MgDisposable();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the reference count.
    ///
    /// \return
    /// Returns the current reference count (value for debugging use only).
    ///
    virtual INT32 GetRefCount();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Increase the reference count.
    ///
    /// \return
    /// Returns the new reference count (value for debugging use only).
    ///
    virtual INT32 AddRef();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Decrease the reference count.
    ///
    /// \return
    /// Returns the new reference count (value for debugging use only).
    ///
    virtual INT32 Release();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the ref count flag for this object to true.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void SetRefCountFlag();

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Create an instance of the MgDisposable object
    ///
    /// \return
    /// Returns nothing
    ///
    MgDisposable();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose() = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Resets the ref count flag for this object to false.
    ///
    /// \return
    /// Returns nothing
    ///
    void ResetRefCountFlag();

private:

    // hidden copy constructor and assignment operator
    MgDisposable(const MgDisposable&);
    MgDisposable& operator=(const MgDisposable&);

private:

    friend class MgGuardDisposable;

    INT32 m_refCount;
    bool m_refCountFlag;

#ifdef _DEBUG
    ACE_Recursive_Thread_Mutex m_dbg_mutex;
#endif
};
/// \endcond

#endif
