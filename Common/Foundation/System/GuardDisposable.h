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

#ifndef _MGGUARDDISPOSABLE_H_
#define _MGGUARDDISPOSABLE_H_

#include "ace/Process_Manager.h"

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// MgGuardDisposable is the base interface for all classes that implement this
/// standard interface for thread-safe reference counting and object destruction.
class MG_FOUNDATION_API MgGuardDisposable : public MgDisposable
{
EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default destructor for this class.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual ~MgGuardDisposable();

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
    /// Sets the refcount flag for this object to true
    ///
    /// \return
    /// Returns nothing
    ///
    void SetRefCountFlag();

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Create an instance of the MgGuardDisposable object
    ///
    /// \return
    /// Returns nothing
    ///
    MgGuardDisposable();

private:

    ACE_Recursive_Thread_Mutex m_mutex;
};
/// \endcond

#endif
