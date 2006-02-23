//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef MGPACKAGEMANAGER_H
#define MGPACKAGEMANAGER_H

class MG_SERVER_MANAGER_API MgPackageManager : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgPackageManager)

public:
    virtual ~MgPackageManager();

    static MgPackageManager* GetInstance();

    // MgDisposable method
    virtual void Dispose();

    MgStringCollection* EnumeratePackages();
    void LoadPackage(CREFSTRING packageName);
    void DeletePackage(CREFSTRING packageName);
    STRING GetPackageStatus(CREFSTRING packageName);
    MgByteReader* GetPackageLog(CREFSTRING packageName);

private:
    // Constructor
    MgPackageManager();

    static Ptr<MgPackageManager> m_packageManager;

    STRING GetPackagesPath();
    MgStringCollection* GetInformationFromLog(CREFSTRING logPath);
};

#endif
