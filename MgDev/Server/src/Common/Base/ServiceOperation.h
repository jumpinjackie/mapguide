//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef MGSERVICEOPERATION_H_
#define MGSERVICEOPERATION_H_

#include "ServerBaseDllExport.h"
#include "AceCommon.h"
#include "BaseService.h"

class MgConnection;

class MG_SERVER_BASE_API MgServiceOperation : public IMgOperationHandler
{
/// Constructors/Destructor

public:

    MgServiceOperation();
    virtual ~MgServiceOperation();

private:

    // Unimplemented copy constructor and assignment operator.

    MgServiceOperation(const MgServiceOperation&);
    MgServiceOperation& operator=(const MgServiceOperation&);

/// Methods

public:

    virtual MgService* GetService() = 0;
    virtual void Execute() = 0;
    virtual void Init(MgStreamData* data, const MgOperationPacket& packet);

protected:

    void Authenticate();

    MgStringCollection* GetAdministratorRole() const;
    MgStringCollection* GetAuthorRole() const;
    MgStringCollection* GetOgcRole() const;

    virtual MgStringCollection* GetRoles() const;

    virtual bool IsOverheadOperation() const;

    void WriteResponseStream(MgStream& stream);
    void WriteResponseStream(MgStream& stream, bool value);
    void WriteResponseStream(MgStream& stream, INT32 value);
    void WriteResponseStream(MgStream& stream, INT64 value);
    void WriteResponseStream(MgStream& stream, STRING value);
    void WriteResponseStream(MgStream& stream, MgSerializable* obj);
    void WriteResponseStream(MgStream& stream, MgStringCollection* stringCollection);
    void WriteResponseStream(MgStream& stream, MgException* except);

/// Data Members

protected:

    MgConnection* m_currConnection;
};

/// Inline Methods

inline bool MgServiceOperation::IsOverheadOperation() const
{
    return false;
}

#endif
