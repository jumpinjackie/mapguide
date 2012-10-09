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

#ifndef MG_LOG_THREAD_H
#define MG_LOG_THREAD_H

#include "MgDesktop.h"
#include "System/ThreadBase.h"

class MgdLogThread : public MgdThreadBase
{
    DECLARE_CLASSNAME(MgdLogThread)

    ///////////////////////////////////////////////////////
    /// Methods
public:
    MgdLogThread(ACE_Thread_Manager &tm, INT32 nThreads);

    // ACE_Task methods
    virtual int svc();

    // Class methods
private:

    ///////////////////////////////////////////////////////
    /// Member data
private:
    bool m_bActive;
};

#endif
