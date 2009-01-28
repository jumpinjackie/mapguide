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

#ifndef MGSESSIONREPOSITORY_H_
#define MGSESSIONREPOSITORY_H_

#include "ApplicationRepository.h"

class MgSessionRepository : public MgApplicationRepository
{
/// Constructors/Destructor

    public:

        MgSessionRepository();
        virtual ~MgSessionRepository();

    private:

        // Unimplemented Constructors/Methods

        MgSessionRepository(const MgSessionRepository&);
        MgSessionRepository& operator=(const MgSessionRepository&);

/// Methods

    public:

        static int VerifyAccess(CREFSTRING repositoryPath);
        virtual void Initialize();

    private:

        virtual void SetupIndices();

/// Data Members

    private:

};

#endif
