//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#ifndef MG_RESOURCE_MANAGER_H
#define MG_RESOURCE_MANAGER_H

#include "RepositoryManager.h"

class MgResourceManager
{
/// Constructors/Destructor

    public:

        explicit MgResourceManager(MgRepositoryManager& repositoryMan);
        virtual ~MgResourceManager();

    private:

        // Unimplemented Constructors/Methods

        MgResourceManager();
        MgResourceManager(const MgResourceManager&);
        MgResourceManager& operator=(const MgResourceManager&);

/// Methods

    public:

    protected:

        bool IsTransacted() const;

        DbTxn* GetDbTxn() const;
        XmlTransaction& GetXmlTxn() const;

/// Data Members

    protected:

        MgRepositoryManager& m_repositoryMan;
        MgDbEnvironment& m_environment;
        MgXmlSchemaResolver& m_schemaResolver;
};

/// Inline Methods

inline bool MgResourceManager::IsTransacted() const
{
    return m_repositoryMan.IsTransacted();
}

inline DbTxn* MgResourceManager::GetDbTxn() const
{
    return m_repositoryMan.GetDbTxn();
}

inline XmlTransaction& MgResourceManager::GetXmlTxn() const
{
    return m_repositoryMan.GetXmlTxn();
}

#endif
