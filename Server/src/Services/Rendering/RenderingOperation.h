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

#ifndef MGRENDERINGOPERATION_H_
#define MGRENDERINGOPERATION_H_

#include "ServiceOperation.h"

class MgRenderingOperation : public MgServiceOperation
{
/// Constructors/Destructor

public:
    virtual ~MgRenderingOperation();

protected:
    MgRenderingOperation();

/// Methods

public:
    virtual MgService* GetService();
    virtual void Initialize(MgStreamData* data, const MgOperationPacket& packet);

protected:
    virtual void Validate();

/// Member Variables

protected:
    Ptr<MgRenderingService> m_service;
};


// Inline Methods

inline MgService* MgRenderingOperation::GetService()
{
    // To avoid overheads and maintain thread safety,
    // do not assign this returned object to a Ptr object.
    return m_service.p;
}

#endif
