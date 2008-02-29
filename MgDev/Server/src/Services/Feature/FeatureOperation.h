//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef MGFEATUREOPERATION_H_
#define MGFEATUREOPERATION_H_

#include "ServiceOperation.h"
#include "ServerFeatureService.h"

class MgFeatureOperation : public MgServiceOperation
{
/// Constructors/Destructor

public:
    virtual ~MgFeatureOperation();

protected:
    MgFeatureOperation();

/// Methods

public:
    virtual MgService* GetService();
    virtual void Initialize(MgStreamData* data, const MgOperationPacket& packet);

protected:
    virtual void Validate();

/// Member Variables

protected:
    Ptr<MgFeatureService> m_service;
};


// Inline Methods

inline MgService* MgFeatureOperation::GetService()
{
    // To avoid overheads and maintain thread safety,
    // do not assign this returned object to a Ptr object.
    return m_service.p;
}

#endif
