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

#include "ThreadBase.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgdThreadBase::MgdThreadBase (ACE_Thread_Manager &tm, INT32 nThreads) :
    ACE_Task<ACE_MT_SYNCH> (&tm)
{
    if(nThreads <= 0)
    {
        nThreads = 5;
    }

    m_nThreads = nThreads;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Initialization. This activates the thread(s).
/// </summary>
INT32 MgdThreadBase::Activate()
{
    INT32 nResult = 0;

    if (activate(THR_DETACHED | THR_NEW_LWP, m_nThreads) == -1)
    {
        ACE_ERROR ((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("MgdThreadBase::activate")));
        nResult = -1;
    }

    return nResult;
}
