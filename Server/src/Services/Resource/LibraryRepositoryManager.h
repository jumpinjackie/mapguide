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

#ifndef MGLIBRARYREPOSITORYMANAGER_H_
#define MGLIBRARYREPOSITORYMANAGER_H_

#include "ApplicationRepositoryManager.h"
#include "LibraryRepository.h"
#include "LibraryResourceContentManager.h"

class MgResourceHeaderManager;

class MgLibraryRepositoryManager : public MgApplicationRepositoryManager
{
/// Constructors/Destructor

public:

    explicit MgLibraryRepositoryManager(MgLibraryRepository& repository);
    virtual ~MgLibraryRepositoryManager();

private:

    // Unimplemented Constructors/Methods

    MgLibraryRepositoryManager();
    MgLibraryRepositoryManager(const MgLibraryRepositoryManager&);
    MgLibraryRepositoryManager& operator=(const MgLibraryRepositoryManager&);

/// Methods

public:

    virtual void CommitTransaction();

    // Repository Management APIs

    virtual MgByteReader* GetRepositoryHeader(
        MgResourceIdentifier* resource);
    void ApplyResourcePackage(MgByteReader* packageStream);
    void LoadResourcePackage(CREFSTRING packagePathname, bool logActivities);
    void MakeResourcePackage(MgResourceIdentifier* resource,
        CREFSTRING packagePathname, CREFSTRING packageDescription, bool logActivities);

    // Resource Management APIs

    virtual bool ResourceExists(MgResourceIdentifier* resource);
    virtual MgByteReader* EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type, INT32 properties,
        CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren);
    STRING EnumerateResourceDocuments(MgStringCollection* resources,
        CREFSTRING type, INT32 properties);
    virtual MgByteReader* GetResourceHeader(
        MgResourceIdentifier* resource);
    MgDateTime* GetResourceModifiedDate(MgResourceIdentifier* resource);
    void ChangeResourceOwner(MgResourceIdentifier* resource,
        CREFSTRING owner, bool includeDescendants);
    void InheritPermissionsFrom(MgResourceIdentifier* resource);

    // Helper Methods

    virtual MgResourceContentManager* GetResourceContentManager();
    virtual MgResourceHeaderManager* GetResourceHeaderManager();

protected:

    virtual void UpdateDateModifiedResourceSet(CREFSTRING resource);

/// Data Members

private:

    MgLibraryResourceContentManager* m_resourceContentMan;
    MgResourceHeaderManager* m_resourceHeaderMan;

    set<STRING> m_dateModifiedResources;
};

/// Inline Methods

inline MgResourceContentManager* MgLibraryRepositoryManager::GetResourceContentManager()
{
    return m_resourceContentMan;
}

inline MgResourceHeaderManager* MgLibraryRepositoryManager::GetResourceHeaderManager()
{
    return m_resourceHeaderMan;
}

#endif
