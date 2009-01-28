//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef MGSIGNALHANDLER_H
#define MGSIGNALHANDLER_H

#include "MapGuideCommon.h"

class MgSignalHandler : public ACE_Event_Handler
{
    ///////////////////////////////////////////////////////
    /// Methods
public:
    MgSignalHandler(int nSignal);
    virtual ~MgSignalHandler();

    // ACE_Event_Handler methods
    virtual int handle_signal(int signum, siginfo_t * = 0, ucontext_t * = 0);
    virtual int handle_exception(ACE_HANDLE handle);

private:

    ///////////////////////////////////////////////////////
    /// Member data
private:
    int m_nSignal;
};

#endif
