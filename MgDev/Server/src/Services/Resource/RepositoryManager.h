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

#ifndef MGREPOSITORYMANAGER_H_
#define MGREPOSITORYMANAGER_H_

#include "SecurityManager.h"
#include "Repository.h"

class MgResourceContentManager;
class MgResourceHeaderManager;

class MgRepositoryManager
{
/// Constructors/Destructor

public:

    explicit MgRepositoryManager(MgRepository& repository);
    virtual ~MgRepositoryManager();

private:

    // Unimplemented Constructors/Methods

    MgRepositoryManager();
    MgRepositoryManager(const MgRepositoryManager&);
    MgRepositoryManager& operator=(const MgRepositoryManager&);

/// Methods

private:

    static MgStringCollection* GetFiles(CREFSTRING repositoryType,
        DbEnv& dbEnv, u_int32_t flags);

public:

    static MgStringCollection* GetDatabaseFiles(CREFSTRING repositoryType,
        CREFSTRING repositoryPath, bool fullPath);
    static MgStringCollection* GetDatabaseFiles(CREFSTRING repositoryType,
        DbEnv& dbEnv, bool fullPath);
    static MgStringCollection* GetLogFiles(CREFSTRING repositoryType,
        DbEnv& dbEnv, bool fullPath);

    static void CleanRepository(CREFSTRING repositoryType);

    void Initialize(bool transacted);
    void Terminate();

    bool IsTransacted() const;

    XmlValue GetAccessedTime() const;
    MgRepository& GetRepository() const;

    DbTxn* GetDbTxn() const;
    XmlTransaction& GetXmlTxn();

    MgUserInformation* GetCurrentUserInfo() const;

    void ThrowResourceNotFoundException(MgResourceIdentifier& resource,
        CREFSTRING methodName, UINT32 lineNumber, CREFSTRING fileName);
    void ThrowDuplicateResourceException(MgResourceIdentifier& resource,
        CREFSTRING methodName, UINT32 lineNumber, CREFSTRING fileName);

    // Repository Management APIs

    virtual MgByteReader* EnumerateRepositories();
    virtual void CreateRepository(MgResourceIdentifier* resource,
        MgByteReader* content, MgByteReader* header);
    virtual void DeleteRepository(MgResourceIdentifier* resource);

    // Resource Management APIs

    virtual bool ResourceExists(MgResourceIdentifier* resource);
    void AddResource(MgResourceIdentifier* resource,
        MgByteReader* content, MgByteReader* header);
    void SetResource(MgResourceIdentifier* resource,
        MgByteReader* content, MgByteReader* header);
    void DeleteResource(MgResourceIdentifier* resource);
    void UpdateResource(MgResourceIdentifier* resource,
        MgByteReader* content, MgByteReader* header);
    virtual MgByteReader* GetResourceContent(
        MgResourceIdentifier* resource, CREFSTRING preProcessTags) = 0;

    // Resource Data Management APIs

    virtual void DeleteResourceData(CREFSTRING resourceTags, MgResourceIdentifier* resource);
    virtual void CopyResourceData(CREFSTRING sourceResourceTags,
        REFSTRING destResourceTags, bool overwrite);

    // Helper Methods

    MgRepositoryManager* GetSourceRepositoryManager();

    virtual MgResourceContentManager* GetResourceContentManager() = 0;
    virtual MgResourceHeaderManager* GetResourceHeaderManager() = 0;

protected:

    void CreateTransaction();
    void ValidateTransaction();
    virtual void CommitTransaction();
    void AbortTransaction();

    virtual void UpdateDateModifiedResourceSet(CREFSTRING resource) {};
    virtual void NotifyResourceChanged(MgResourceIdentifier& resource) {};

/// Data Members

protected:

    auto_ptr<MgSecurityManager> m_securityMan;
    Ptr<MgUserInformation> m_currUserInfo;

    bool m_currUserIsAdmin;
    bool m_currUserIsAuthor;

    MgRepositoryManager* m_sourceRepositoryMan;

    DbTxn* m_dbTxn;
    auto_ptr<XmlTransaction> m_xmlTxn;

    time_t m_accessedTime;

private:

    friend class MgResourceDefinitionManager;
    friend class MgResourceHeaderManager;
    friend class MgResourceContentManager;
    friend class MgApplicationResourceContentManager;
    friend class MgLibraryResourceContentManager;
    friend class MgSessionResourceContentManager;
    friend class MgResourceDataFileManager;
    friend class MgResourceDataStreamManager;

    MgRepository& m_repository;
    bool m_transacted;
};

/// Inline Methods

inline bool MgRepositoryManager::IsTransacted() const
{
    return m_transacted;
}

inline MgRepositoryManager* MgRepositoryManager::GetSourceRepositoryManager()
{
    return m_sourceRepositoryMan;
}

inline MgRepository& MgRepositoryManager::GetRepository() const
{
    return m_repository;
}

inline DbTxn* MgRepositoryManager::GetDbTxn() const
{
    return m_dbTxn;
}

inline XmlTransaction& MgRepositoryManager::GetXmlTxn()
{
    assert(NULL != m_xmlTxn.get());
    return *m_xmlTxn;
}

inline MgUserInformation* MgRepositoryManager::GetCurrentUserInfo() const
{
    return SAFE_ADDREF((MgUserInformation*)m_currUserInfo);
}

#endif
