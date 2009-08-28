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

#ifndef MGAPPLICATIONREPOSITORYMANAGER_H_
#define MGAPPLICATIONREPOSITORYMANAGER_H_

#include "RepositoryManager.h"
#include "ApplicationRepository.h"

class MgApplicationResourceContentManager;
class MgResourceDataStreamManager;
class MgResourceDataFileManager;

class MgApplicationRepositoryManager : public MgRepositoryManager
{
/// Constructors/Destructor

public:

    explicit MgApplicationRepositoryManager(MgApplicationRepository& repository);
    virtual ~MgApplicationRepositoryManager();

private:

    // Unimplemented Constructors/Methods

    MgApplicationRepositoryManager();
    MgApplicationRepositoryManager(const MgApplicationRepositoryManager&);
    MgApplicationRepositoryManager& operator=(const MgApplicationRepositoryManager&);

/// Methods

public:

    // Repository Management APIs

    void UpdateRepository(MgResourceIdentifier* resource,
        MgByteReader* content, MgByteReader* header);
    MgByteReader* GetRepositoryContent(
        MgResourceIdentifier* resource);
    virtual MgByteReader* GetRepositoryHeader(
        MgResourceIdentifier* resource);

    // Resource Management APIs

    virtual MgByteReader* EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type, INT32 properties,
        CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren);
    virtual MgByteReader* GetResourceContent(
        MgResourceIdentifier* resource, CREFSTRING preProcessTags);
    virtual MgByteReader* GetResourceHeader(
        MgResourceIdentifier* resource);
    void MoveResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite, bool cascade);
    void CopyResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite);
    MgByteReader* EnumerateReferences(MgResourceIdentifier* resource);
    void EnumerateParentMapDefinitions(
        set<string>& childResources, set<STRING>& parentResources);

    // Resource Data Management APIs

    MgByteReader* EnumerateResourceData(
        MgResourceIdentifier* resource);
    void SetResourceData(MgResourceIdentifier* resource,
        CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data);
    void DeleteResourceData(MgResourceIdentifier* resource,
        CREFSTRING dataName);
    void RenameResourceData(MgResourceIdentifier* resource,
        CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite);
    MgByteReader* GetResourceData(MgResourceIdentifier* resource,
        CREFSTRING dataName, CREFSTRING preProcessTags);

    // Helper Methods

    void DeleteResourceData(CREFSTRING resourceTags, MgResourceIdentifier* resource);
    void CopyResourceData(CREFSTRING sourceResourceTags,
        REFSTRING destResourceTags, bool overwrite);

    MgApplicationResourceContentManager* GetApplicationResourceContentManager();

    MgResourceDataStreamManager* GetResourceDataStreamManager();
    MgResourceDataFileManager* GetResourceDataFileManager();

    const set<STRING>& GetChangedResources() const;

protected:

    virtual void NotifyResourceChanged(MgResourceIdentifier& resource);

private:

    static MgApplicationRepositoryManager* Create(MgResourceIdentifier* resource);

/// Data Members

protected:

    MgResourceDataStreamManager* m_dataStreamMan;
    MgResourceDataFileManager* m_dataFileMan;

private:

    set<STRING> m_changedResources;
};

/// Inline Methods

inline MgResourceDataStreamManager* MgApplicationRepositoryManager::GetResourceDataStreamManager()
{
    return m_dataStreamMan;
}

inline MgResourceDataFileManager* MgApplicationRepositoryManager::GetResourceDataFileManager()
{
    return m_dataFileMan;
}

inline const set<STRING>& MgApplicationRepositoryManager::GetChangedResources() const
{
    return m_changedResources;
}

#endif
