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

#ifndef MGLIBRARYREPOSITORY_H_
#define MGLIBRARYREPOSITORY_H_

#include "ApplicationRepository.h"

class MgLibraryRepository : public MgApplicationRepository
{
/// Constructors/Destructor

    public:

        MgLibraryRepository();
        virtual ~MgLibraryRepository();

    private:

        // Unimplemented Constructors/Methods

        MgLibraryRepository(const MgLibraryRepository&);
        MgLibraryRepository& operator=(const MgLibraryRepository&);

/// Methods

    public:

        static int VerifyAccess(CREFSTRING repositoryPath, CREFSTRING resourceDataFilePath);
        virtual void Initialize();

        MgResourceContainer* GetResourceHeaderContainer() const;

    private:

        virtual void SetupIndices();

/// Data Members

    private:

        MgResourceContainer* m_resourceHeaderContainer;
};

inline MgResourceContainer* MgLibraryRepository::GetResourceHeaderContainer() const
{
    return m_resourceHeaderContainer;
}

#endif
