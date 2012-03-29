//
//  Copyright (C) 2011 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "TimerUtil.h"

#ifdef WIN32
LARGE_INTEGER MgTimerUtil::frequency = {0};
LARGE_INTEGER MgTimerUtil::count = {0};
#else
timeval MgTimerUtil::count = {0};
#endif

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgTimerUtil::MgTimerUtil()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgTimerUtil::~MgTimerUtil()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves the current time in milliseconds.
/// </summary>
///----------------------------------------------------------------------------

double MgTimerUtil::GetTime()
{
#ifdef WIN32
    if(0 == frequency.QuadPart)
    {
        GetFrequency(&frequency);
    }
    QueryPerformanceCounter(&count);

    return count.QuadPart* (1000.0 / frequency.QuadPart);
#else
    gettimeofday(&count, NULL);
    return (count.tv_sec * 1000.0) + (count.tv_usec / 1000.0);
#endif
}

#ifdef WIN32
///----------------------------------------------------------------------------
/// <summary>
/// Invoke the QueryPerformanceFrequency to initialize class member frequency.
/// </summary>
///----------------------------------------------------------------------------

void MgTimerUtil::GetFrequency(LARGE_INTEGER* pFrequency)
{
    if(!QueryPerformanceFrequency(pFrequency))
        throw new MgUnclassifiedException(L"MgTimerUtil.GetFrequency", __LINE__, __WFILE__, NULL, L"", NULL);
}
#endif
