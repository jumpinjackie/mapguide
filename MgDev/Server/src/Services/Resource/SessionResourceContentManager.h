//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef MGSESSIONRESOURCECONTENTMANAGER_H_
#define MGSESSIONRESOURCECONTENTMANAGER_H_

#include "ApplicationResourceContentManager.h"

class MgSessionRepositoryManager;

class MgSessionResourceContentManager : public MgApplicationResourceContentManager
{
/// Constructors/Destructor

public:

    explicit MgSessionResourceContentManager(
        MgSessionRepositoryManager& repositoryManager);
    virtual ~MgSessionResourceContentManager();

private:

    // Unimplemented Constructors/Methods

    MgSessionResourceContentManager();
    MgSessionResourceContentManager(const MgSessionResourceContentManager&);
    MgSessionResourceContentManager& operator=(const MgSessionResourceContentManager&);

/// Methods

public:

    // Repository Management APIs

    virtual MgByteReader* EnumerateRepositories();
    void DeleteRepository(MgResourceIdentifier* resource);

protected:

    // Resource Permission related methods

    virtual bool CheckPermission(MgResourceIdentifier& resource,
        CREFSTRING permission, bool strict = true);
    virtual bool CheckParentPermission(MgResourceIdentifier& resource,
        CREFSTRING permission, bool strict = true);

/// Data Members

private:

};

#endif
