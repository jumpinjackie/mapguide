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

#ifndef MGRESOURCEDATASTREAMMANAGER_H_
#define MGRESOURCEDATASTREAMMANAGER_H_

#include "ResourceDataManager.h"
#include "ResourceDatabase.h"

class MgResourceDataStreamManager : public MgResourceDataManager
{
/// Constructors/Destructor

public:

    explicit MgResourceDataStreamManager(
        MgApplicationRepositoryManager& repositoryMan);
    virtual ~MgResourceDataStreamManager();

private:

    // Unimplemented Constructors/Methods

    MgResourceDataStreamManager();
    MgResourceDataStreamManager(const MgResourceDataStreamManager&);
    MgResourceDataStreamManager& operator=(const MgResourceDataStreamManager&);

/// Methods

public:

    // Resource Data Management APIs

    void SetResourceData(const string& dataKey, CREFSTRING dataName,
        MgByteReader* byteReader);
    void DeleteResourceData(const string& dataKey, CREFSTRING dataName,
        bool strict = false);
    void CopyResourceData(const string& sourceDataKey,
        const string& destDataKey, CREFSTRING dataName);
    MgByteReader* GetResourceData(const string& dataKey,
        CREFSTRING dataName, CREFSTRING mimeType);

/// Data Members

protected:

    Db& m_database;
};

#endif
