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

#ifndef MGPACKAGEMANAGER_H_
#define MGPACKAGEMANAGER_H_

class MG_SERVER_MANAGER_API MgPackageManager : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgPackageManager)

/// Constructors/Destructor

public:

    virtual ~MgPackageManager();

private:

    // Unimplemented copy constructor and assignment operator.

    MgPackageManager();
    MgPackageManager(const MgPackageManager&);
    MgPackageManager& operator=(const MgPackageManager&);

/// Methods

public:

    static MgPackageManager* GetInstance();
    virtual void Dispose();

    static STRING GetPackagePath();
    static STRING GetPackageName(CREFSTRING packagePathname);
    static STRING GetPackagePathname(CREFSTRING packageName);
    static STRING GetLogPathname(CREFSTRING packageName);

    MgStringCollection* EnumeratePackages();
    void LoadPackage(CREFSTRING packageName);
    void MakePackage(MgResourceIdentifier* resource, CREFSTRING packageName,
        CREFSTRING packageDescription);
    void DeletePackage(CREFSTRING packageName);
    MgPackageStatusInformation* GetPackageStatus(CREFSTRING packageName);
    MgByteReader* GetPackageLog(CREFSTRING packageName);

/// Data Members

private:

    static Ptr<MgPackageManager> sm_packageManager;
};

#endif
