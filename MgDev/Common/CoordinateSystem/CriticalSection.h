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

#ifndef CRITICALSECTION_H_
#define CRITICALSECTION_H_

#include "SmartCriticalClass.h"

class CustomCriticalSection
{
public:
    CustomCriticalSection();
    ~CustomCriticalSection();

    void Initialize();

    void Enter();
    void Leave();

#ifdef _DEBUG

    bool IsEntered();

#endif

private:
    bool m_bInitialized;
};

//This is used to protect *all* library calls - make sure, all comilation units refer to the same instance
extern CustomCriticalSection CriticalClass;

#endif //CRITICALSECTION_H_
