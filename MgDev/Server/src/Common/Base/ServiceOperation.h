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

#ifndef MGSERVICEOPERATION_H_
#define MGSERVICEOPERATION_H_

#include "ServerBaseDllExport.h"
#include "MapGuideCommon.h"

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

    virtual void Initialize(MgStreamData* data, const MgOperationPacket& packet);
    virtual void Execute() = 0;
    virtual bool HandleException(MgException* except);

protected:

    virtual void Validate() = 0;
    void CheckLicense();
    void AuthenticateCurrentUser();

    MgStringCollection* GetAdministratorRole() const;
    MgStringCollection* GetAuthorRole() const;
    MgStringCollection* GetOgcRole() const;

    virtual MgStringCollection* GetRoles() const;

    virtual bool IsOverheadOperation() const;

    void BeginExecution();

    void EndExecution();
    void EndExecution(bool value);
    void EndExecution(INT32 value);
    void EndExecution(INT64 value);
    void EndExecution(STRING value);
    void EndExecution(MgSerializable* obj);
    void EndExecution(MgStringCollection* stringCollection);

private:

    void EndExecution(MgException* except);

/// Data Members

protected:

    MgConnection* m_currConnection;
    Ptr<MgStream> m_stream;
    Ptr<MgResourceService> m_resourceService;
    bool m_argsRead;

private:

    bool m_opCompleted;
};

/// Inline Methods

inline bool MgServiceOperation::IsOverheadOperation() const
{
    return false;
}

#endif
