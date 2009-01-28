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

#ifndef MGRESOURCEPACKAGEMANIFESTHANDLER_H_
#define MGRESOURCEPACKAGEMANIFESTHANDLER_H_

#include "System/XmlUtil.h"

class MgResourcePackageManifestHandler
{
/// Constructors/Destructor

public:

    MgResourcePackageManifestHandler();
    virtual ~MgResourcePackageManifestHandler();

private:

    // Unimplemented Methods

    MgResourcePackageManifestHandler(const MgResourcePackageManifestHandler&);
    MgResourcePackageManifestHandler& operator=(
        const MgResourcePackageManifestHandler&);

/// Methods

public:

    CREFSTRING GetDescription() const;
    void SetDescription(CREFSTRING description);

/// Data Members

public:

    static const STRING sm_manifestFileName;

protected:

    auto_ptr<MgXmlUtil> m_xmlUtil;
    STRING m_description;
};

/// Inline Methods

inline CREFSTRING MgResourcePackageManifestHandler::GetDescription() const
{
    return m_description;
}

#endif
