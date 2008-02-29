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

#ifndef MG_SYSTEM_REPOSITORY_MANAGER_H
#define MG_SYSTEM_REPOSITORY_MANAGER_H

#include "RepositoryManager.h"
#include "SystemRepository.h"

class MgSystemRepositoryManager : public MgRepositoryManager
{
/// Constructors/Destructor

    public:

        explicit MgSystemRepositoryManager(MgSystemRepository& repository);
        virtual ~MgSystemRepositoryManager();

    private:

        // Unimplemented Constructors/Methods

        MgSystemRepositoryManager();
        MgSystemRepositoryManager(const MgSystemRepositoryManager&);
        MgSystemRepositoryManager& operator=(const MgSystemRepositoryManager&);

/// Methods

    public:

        // Resource Management APIs

        virtual MgByteReader* GetResourceContent(
            MgResourceIdentifier* resource, CREFSTRING preProcessTags);

        virtual MgResourceHeaderManager* GetResourceHeaderManager();

    protected:


/// Data Members

    private:

};

/// Inline Methods

inline MgResourceHeaderManager* MgSystemRepositoryManager::GetResourceHeaderManager()
{
    return NULL;
}

#endif
