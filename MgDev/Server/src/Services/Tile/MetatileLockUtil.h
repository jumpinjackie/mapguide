//
//  Copyright (C) 2004-2019 by Autodesk, Inc.
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


#ifndef MG_METATILE_LOCK_UTIL_H
#define MG_METATILE_LOCK_UTIL_H

/// \brief
/// Metatile helper class for file-based locking
class MgMetatileLockUtil
{
public:
    MgMetatileLockUtil();
    ~MgMetatileLockUtil();

    void WaitForLock(CREFSTRING tilePathname);
    void ClearLock(CREFSTRING tilePathname);
    void LockTile(CREFSTRING tilePathname);

private:
    static std::map<STRING, ACE_Condition<ACE_Recursive_Thread_Mutex>*> sm_lockMap;
    static ACE_Recursive_Thread_Mutex sm_tileMutex;
};

#endif