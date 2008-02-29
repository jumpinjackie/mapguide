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

#ifndef MGSERVERSITESERVICE_H_
#define MGSERVERSITESERVICE_H_

#include "ServerSiteDllExport.h"
#include "ServerResourceService.h"

class MG_SERVER_SITE_API MgServerSiteService
    : public MgService
{
    DECLARE_CLASSNAME(MgServerSiteService)

/// Constructors/Destructor

public:
    MgServerSiteService();
    ~MgServerSiteService( void );
    DECLARE_CREATE_SERVICE()

private:

    void ValidateAuthorOrSelf(CREFSTRING user, CREFSTRING group);

/// Methods

public:
    //  User Methods
    virtual void AddUser( CREFSTRING userID, CREFSTRING userName,
        CREFSTRING password, CREFSTRING description );
    virtual void DeleteUsers( MgStringCollection* users );
    virtual MgByteReader* EnumerateUsers( CREFSTRING group,
        CREFSTRING role, bool includeGroups );
    virtual void GrantGroupMembershipsToUsers( MgStringCollection* groups,
        MgStringCollection* users );
    virtual void GrantRoleMembershipsToUsers( MgStringCollection* roles,
        MgStringCollection* users );
    virtual void RevokeGroupMembershipsFromUsers( MgStringCollection* groups,
        MgStringCollection* users );
    virtual void RevokeRoleMembershipsFromUsers( MgStringCollection* roles,
        MgStringCollection* users );
    virtual void UpdateUser( CREFSTRING userID, CREFSTRING newUserID,
        CREFSTRING newUserName, CREFSTRING newPassword,
        CREFSTRING newDescription );

    //  Group Methods
    virtual void AddGroup( CREFSTRING group, CREFSTRING description );
    virtual void DeleteGroups( MgStringCollection* groups );
    virtual MgByteReader* EnumerateGroups( CREFSTRING user, CREFSTRING role );
    virtual void GrantRoleMembershipsToGroups( MgStringCollection* roles,
        MgStringCollection* groups );
    virtual void RevokeRoleMembershipsFromGroups( MgStringCollection* roles,
        MgStringCollection* groups );
    virtual void UpdateGroup( CREFSTRING group, CREFSTRING newGroup,
        CREFSTRING newDescription );

    //  Role Methods
    virtual MgStringCollection* EnumerateRoles( CREFSTRING user,
        CREFSTRING group );

    // Server Management Methods

    MgByteReader* EnumerateServers();
    void AddServer(CREFSTRING name, CREFSTRING description,
        CREFSTRING address);
    void UpdateServer(CREFSTRING oldName, CREFSTRING newName,
        CREFSTRING newDescription, CREFSTRING newAddress);
    void RemoveServer(CREFSTRING name);
    STRING RequestServer(INT32 serviceType);

    //  Authentication
    virtual MgStringCollection* Authenticate(MgUserInformation* userInformation,
        MgStringCollection* requiredRoles, bool returnAssignedRoles);
    virtual STRING CreateSession();
    virtual void DestroySession(CREFSTRING session);
    virtual STRING GetUserForSession();

    //  Inherited from MgDisposable
    virtual void Dispose();

    // Internal use methods
    void SetConnectionProperties(MgConnectionProperties* connProp);

protected:

    MgServerResourceService& GetResourceService();

/// Data Members

private:

    Ptr<MgServerResourceService> m_resourceService;
};

#endif
