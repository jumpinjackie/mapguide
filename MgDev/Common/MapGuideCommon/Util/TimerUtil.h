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

#ifndef MGTIMERUTIL_H_
#define MGTIMERUTIL_H_

#ifdef WIN32   
#include <windows.h>
#else          
#include <sys/time.h>
#endif

/// \cond INTERNAL
class MG_MAPGUIDE_API MgTimerUtil
{
    DECLARE_CLASSNAME(MgTimerUtil)

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs the object.
    ///
    ///

    MgTimerUtil();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructs the object.
    ///
    ///

    virtual ~MgTimerUtil();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieves the current time in milliseconds
    ///
    /// \return
    /// current time in milliseconds
    ///
    ///

    static double GetTime();

private:

    /// Unimplemented Methods

    MgTimerUtil(const MgTimerUtil& util);
    MgTimerUtil& operator=(const MgTimerUtil& util);

    /// Helper Methods

#ifdef WIN32
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Invoke the QueryPerformanceFrequency to initialize frequency.
    /// \param pFrequency
    /// Pointer which idicates the frequncy.
    ///

    static void GetFrequency(LARGE_INTEGER* pFrequency);
#endif

private:

#ifdef WIN32
    static LARGE_INTEGER frequency;
    static LARGE_INTEGER count;
#else
    static timeval count;
#endif
};
/// \endcond

/// Inline Methods

#endif
