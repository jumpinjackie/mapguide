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

#ifndef MGREPOSITORY_H_
#define MGREPOSITORY_H_

#include "DbEnvironment.h"
#include "ResourceContainer.h"

class MgRepository
{
/// Constructors/Destructor

    public:

        virtual ~MgRepository();

    protected:

        MgRepository();

    private:

        // Unimplemented Constructors/Methods

        MgRepository(const MgRepository&);
        MgRepository& operator=(const MgRepository&);

/// Methods

    public:

        static int VerifyAccess(CREFSTRING dirPath, CREFSTRING fileName,
            bool checkVersion);
        virtual void Initialize() = 0;

        bool IsTypeOf(CREFSTRING type) const;

        MgDbEnvironment* GetEnvironment() const;
        MgResourceContainer* GetResourceContentContainer() const;

        void PerformCheckpoint(UINT32 flags = 0);

    private:

        virtual void SetupIndices() = 0;

/// Data Members

    public:

        static const string LibraryResourceContentContainerName;
        static const string LibraryResourceHeaderContainerName;
        static const string LibraryResourceDataStreamDatabaseName;
        static const string SessionResourceContentContainerName;
        static const string SessionResourceDataStreamDatabaseName;
        static const string SiteResourceContentContainerName;
        static const string SessionResourceContentContainerExt;
        static const string SessionResourceDataStreamDatabaseExt;

    protected:

        int m_dbVersion;
        MgDbEnvironment* m_environment;
        MgResourceContainer* m_resourceContentContainer;
};

/// Inline Methods

inline MgDbEnvironment* MgRepository::GetEnvironment() const
{
    return m_environment;
}

inline MgResourceContainer* MgRepository::GetResourceContentContainer() const
{
    return m_resourceContentContainer;
}

#endif
