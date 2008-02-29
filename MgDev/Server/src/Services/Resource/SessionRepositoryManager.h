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

#ifndef MGSESSIONREPOSITORYMANAGER_H_
#define MGSESSIONREPOSITORYMANAGER_H_

#include "ApplicationRepositoryManager.h"
#include "SessionRepository.h"
#include "SessionResourceContentManager.h"

class MgSessionRepositoryManager : public MgApplicationRepositoryManager
{
/// Constructors/Destructor

    public:

        explicit MgSessionRepositoryManager(MgSessionRepository& repository);
        virtual ~MgSessionRepositoryManager();

    private:

        // Unimplemented Constructors/Methods

        MgSessionRepositoryManager();
        MgSessionRepositoryManager(const MgSessionRepositoryManager&);
        MgSessionRepositoryManager& operator=(const MgSessionRepositoryManager&);

/// Methods

    public:

        // Repository Management APIs

        virtual MgByteReader* EnumerateRepositories();
        virtual void DeleteRepository(MgResourceIdentifier* resource);
         virtual MgResourceHeaderManager* GetResourceHeaderManager();

    protected:

        virtual MgResourceContentManager* GetResourceContentManager();

/// Data Members

    private:

        MgSessionResourceContentManager* m_resourceContentMan;
};

/// Inline Methods

inline MgResourceContentManager* MgSessionRepositoryManager::GetResourceContentManager()
{
    return m_resourceContentMan;
}

inline MgResourceHeaderManager* MgSessionRepositoryManager::GetResourceHeaderManager()
{
    return NULL;
}

#endif
