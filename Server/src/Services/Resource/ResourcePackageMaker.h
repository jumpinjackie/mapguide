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

#ifndef MGRESOURCEPACKAGEMAKER_H_
#define MGRESOURCEPACKAGEMAKER_H_

#include "ResourcePackageHandler.h"
#include "ZipFileWriter.h"
#include "ResourcePackageManifestSerializer.h"

typedef std::multimap<STRING, MgOperationInfo*> MgResourceOperationMap;

class MgResourcePackageMaker : public MgResourcePackageHandler
{
/// Constructors/Destructor

public:

    explicit MgResourcePackageMaker(MgLibraryRepositoryManager& repositoryManager);
    virtual ~MgResourcePackageMaker();

private:

    // Unimplemented Methods

    MgResourcePackageMaker();
    MgResourcePackageMaker(const MgResourcePackageMaker&);
    MgResourcePackageMaker& operator=(const MgResourcePackageMaker&);

/// Methods

public:

    void Start(MgResourceIdentifier& resource,
        CREFSTRING packagePathname, CREFSTRING packageDescription,
        bool logActivities);
    virtual void End(MgException* except);

    void PackageResourceHeader(MgResourceIdentifier& resource,
        const XmlDocument& xmlDoc);
    bool PackageResourceContent(MgResourceIdentifier& resource,
        const XmlDocument& xmlDoc);
    void PackageResourceData(MgResourceIdentifier& resource,
        MgByteReader* byteReader, CREFSTRING dataName, CREFSTRING dataType);

private:

    void GeneratePathnames(MgResourceIdentifier& resource, CREFSTRING postfix,
        REFSTRING resourcePathname, REFSTRING archivePathname) const;

/// Data Members

private:

    auto_ptr<MgZipFileWriter> m_zipFileWriter;
    MgResourcePackageManifestSerializer m_manifestSerializer;

    MgResourceOperationMap m_resourceOperationMap;
};

/// Inline Methods

#endif
