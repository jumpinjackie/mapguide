//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef MGRESOURCEPACKAGEMANAGER_H_
#define MGRESOURCEPACKAGEMANAGER_H_

#include "LibraryRepositoryManager.h"
#include "ZipFileHandler.h"
#include "ResourcePackageManifestParser.h"
#include "OperationInfo.h"
#include "PackageStatusLogManager.h"

class MgResourcePackageManager
{
/// Constructors/Destructor

    public:

        MgResourcePackageManager(MgLibraryRepositoryManager& repositoryManager,
            CREFSTRING manifest, bool logActivities = false);
        virtual ~MgResourcePackageManager();

    private:

        // Unimplemented Methods

        MgResourcePackageManager();
        MgResourcePackageManager(const MgResourcePackageManager&);
        MgResourcePackageManager& operator=(const MgResourcePackageManager&);

/// Methods

    public:

        void PerformOperations();

    protected:

        void PerformOperation(const MgOperationInfo& opInfo);

        void UpdateRepository(const MgOperationInfo& opInfo);
        void SetResource(const MgOperationInfo& opInfo);
        void DeleteResource(const MgOperationInfo& opInfo);
        void MoveResource(const MgOperationInfo& opInfo);
        void CopyResource(const MgOperationInfo& opInfo);
        void ChangeResourceOwner(const MgOperationInfo& opInfo);
        void InheritPermissionsFrom(const MgOperationInfo& opInfo);
        void SetResourceData(const MgOperationInfo& opInfo);
        void DeleteResourceData(const MgOperationInfo& opInfo);
        void RenameResourceData(const MgOperationInfo& opInfo);

    private:

        MgResourceIdentifier* CreateResourceIdentifier(
            const MgOperationParameter& opParam) const;
        MgByteReader* CreateByteReader(
            const MgOperationParameter& opParam, bool direct = true) const;

/// Data Members

    private:

        MgLibraryRepositoryManager& m_repositoryManager;
        auto_ptr<MgZipFileHandler> m_zipFileHandler;
        MgResourcePackageManifestParser m_manifestParser;
        string m_packageManifest;

        // Needed for logging
        Ptr<MgPackageStatusLogManager> m_packageLogManager;
};

/// Inline Methods

#endif
