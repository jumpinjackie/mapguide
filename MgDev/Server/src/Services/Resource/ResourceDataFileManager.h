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

#ifndef MGRESOURCEDATAFILEMANAGER_H_
#define MGRESOURCEDATAFILEMANAGER_H_

#include "ResourceDataManager.h"

class MgResourceDataFileManager : public MgResourceDataManager
{
/// Constructors/Destructor

    public:

        explicit MgResourceDataFileManager(
            MgApplicationRepositoryManager& repositoryMan);
        virtual ~MgResourceDataFileManager();

    private:

        // Unimplemented Constructors/Methods

        MgResourceDataFileManager();
        MgResourceDataFileManager(const MgResourceDataFileManager&);
        MgResourceDataFileManager& operator=(const MgResourceDataFileManager&);

/// Methods

    public:

        CREFSTRING GetResourceDataFilePath() const;

        // Resource Data Management APIs

        void SetResourceData(CREFSTRING pathname, MgByteReader* byteReader);
        void DeleteResourceData(CREFSTRING pathname, bool strict = false);
        void RenameResourceData(CREFSTRING path, CREFSTRING oldFileName,
            CREFSTRING newFileName, bool overwrite);
        void CopyResourceData(CREFSTRING sourcePathname,
            CREFSTRING destPathname, bool overwrite);
        MgByteReader* GetResourceData(CREFSTRING pathname, CREFSTRING mimeType);

/// Data Members

    private:

        STRING m_resourceDataFilePath;
};

/// Inline Methods

inline CREFSTRING MgResourceDataFileManager::GetResourceDataFilePath() const
{
    return m_resourceDataFilePath;
}

#endif
