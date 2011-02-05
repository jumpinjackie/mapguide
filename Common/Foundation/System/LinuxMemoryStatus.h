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

#ifndef _MG_LINUXMEMORYSTATUS_H_
#define _MG_LINUXMEMORYSTATUS_H_

/// \cond INTERNAL
///////////////////////////////////////////////////////////
/// \brief
/// A structure to store Linux memory status
struct MgLinuxMemoryStatus
{
    INT64 m_memTotal;
    INT64 m_memUsed;
    INT64 m_memFree;
    INT64 m_memShared;
    INT64 m_memBuffers;
    INT64 m_memCached;

    INT64 m_swapTotal;
    INT64 m_swapUsed;
    INT64 m_swapFree;
};
/// \endcond

#endif
