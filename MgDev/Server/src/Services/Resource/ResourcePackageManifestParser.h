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

#ifndef MGRESOURCEPACKAGEMANIFESTPARSER_H_
#define MGRESOURCEPACKAGEMANIFESTPARSER_H_

#include "ResourcePackageManifestHandler.h"
#include "OperationInfo.h"

typedef std::vector<MgOperationInfo> MgOpInfoVector;

class MgResourcePackageManifestParser : public MgResourcePackageManifestHandler
{
/// Constructors/Destructor

public:

    MgResourcePackageManifestParser();
    virtual ~MgResourcePackageManifestParser();

private:

    // Unimplemented Methods

    MgResourcePackageManifestParser(const MgResourcePackageManifestParser&);
    MgResourcePackageManifestParser& operator=(
        const MgResourcePackageManifestParser&);

/// Methods

public:

    const MgOpInfoVector& GetOperations() const;

    void Parse(const string& manifest);

/// Data Members

private:

    MgOpInfoVector m_operations;
};

/// Inline Methods

inline const MgOpInfoVector& MgResourcePackageManifestParser::GetOperations() const
{
    return m_operations;
}

#endif
