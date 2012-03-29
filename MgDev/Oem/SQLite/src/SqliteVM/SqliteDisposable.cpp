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

#include "CommonDefs.h"
#include "SqliteDisposable.h"

INT32 SqliteDisposable::GetRefCount() const
{
    return m_refCount;
}

INT32 SqliteDisposable::AddRef()
{
    return ++m_refCount;
}

INT32 SqliteDisposable::Release()
{
    if (0 != --m_refCount )
    {
        return m_refCount;
    }
    else
    {
        Dispose();
        return 0;
    }
}

SqliteDisposable::SqliteDisposable()
: m_refCount(1)
{
}

SqliteDisposable::~SqliteDisposable()
{
}
