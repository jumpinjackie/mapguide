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

#ifndef MGRESOURCEPACKAGELOADER_H_
#define MGRESOURCEPACKAGELOADER_H_

#include "ResourcePackageHandler.h"
#include "ZipFileReader.h"
#include "ResourcePackageManifestParser.h"

class MgResourcePackageLoader : public MgResourcePackageHandler
{
/// Constructors/Destructor

public:

    explicit MgResourcePackageLoader(MgLibraryRepositoryManager& repositoryManager);
    virtual ~MgResourcePackageLoader();

private:

    // Unimplemented Methods

    MgResourcePackageLoader();
    MgResourcePackageLoader(const MgResourcePackageLoader&);
    MgResourcePackageLoader& operator=(const MgResourcePackageLoader&);

/// Methods

public:

    void Start(CREFSTRING packagePathname, bool logActivities);
    virtual void End(MgException* except);

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

    auto_ptr<MgZipFileReader> m_zipFileReader;
    MgResourcePackageManifestParser m_manifestParser;
};

/// Inline Methods

#endif
