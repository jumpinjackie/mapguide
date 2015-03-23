//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef _SqliteDisposable_H_
#define _SqliteDisposable_H_

class SqliteDisposable
{

INTERNAL_API:
    /// <summary>Get the reference count.</summary>
    /// <returns>Returns the current reference count (value for debugging use only).</returns>
    INT32 GetRefCount() const;

    /// <summary>Increase the reference count.</summary>
    /// <returns>Returns the new reference count (value for debugging use only).</returns>
    INT32 AddRef();

    /// <summary>Decrease the reference count.</summary>
    /// <returns>Returns the new reference count (value for debugging use only).</returns>
    INT32 Release();

    virtual INT32 GetClassId() { return 0; };

    // <summary>Default destructor for this class.</summary>
    // <returns>Returns nothing</returns>
    virtual ~SqliteDisposable();

    // <summary>Dispose this object.</summary>
    // <returns>Returns nothing</returns>
    virtual void Dispose() = 0;

protected:
    // <summary>Create an instance of the IDisposable object</summary>
    // <returns>Returns nothing</returns>
    SqliteDisposable();

private:
    INT32 m_refCount;
};

#define SAFE_RELEASE(x) {if (x) (x)->Release(); (x) = NULL;}
#define SAFE_ADDREF(x)  ((x != NULL) ? ((x)->AddRef(), (x)) : NULL)

#endif
