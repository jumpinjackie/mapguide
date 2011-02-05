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

#ifndef MGMAPPINGOPERATION_H_
#define MGMAPPINGOPERATION_H_

class MgMappingOperation : public MgServiceOperation
{
/// Constructors/Destructor

public:
    virtual ~MgMappingOperation();

protected:
    MgMappingOperation();

/// Methods

public:
    virtual MgService* GetService();
    virtual void Initialize(MgStreamData* data, const MgOperationPacket& packet);

protected:
    virtual void Validate();

/// Member Variables

protected:
    Ptr<MgMappingService> m_service;
};


// Inline Methods

inline MgService* MgMappingOperation::GetService()
{
    // To avoid overheads and maintain thread safety,
    // do not assign this returned object to a Ptr object.
    return m_service.p;
}

#endif
