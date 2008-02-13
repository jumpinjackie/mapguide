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

#ifndef MGLIBRARYRESOURCECONTENTMANAGER_H_
#define MGLIBRARYRESOURCECONTENTMANAGER_H_

#include "ApplicationResourceContentManager.h"

class MgLibraryRepositoryManager;
class MgResourcePackageMaker;

class MgLibraryResourceContentManager : public MgApplicationResourceContentManager
{
/// Constructors/Destructor

public:

    explicit MgLibraryResourceContentManager(
        MgLibraryRepositoryManager& repositoryMan);
    virtual ~MgLibraryResourceContentManager();

private:

    // Unimplemented Constructors/Methods

    MgLibraryResourceContentManager();
    MgLibraryResourceContentManager(const MgLibraryResourceContentManager&);
    MgLibraryResourceContentManager& operator=(const MgLibraryResourceContentManager&);

/// Methods

protected:

    // Resource Permission related methods

    virtual bool CheckPermission(MgResourceIdentifier& resource,
        CREFSTRING permission, bool strict = true);
    virtual bool CheckParentPermission(MgResourceIdentifier& resource,
        CREFSTRING permission, bool strict = true);

    void PackageResource(MgResourceIdentifier& resource,
        MgResourcePackageMaker& packageMaker);

/// Data Members

private:

    friend class MgLibraryRepositoryManager;
};

#endif
