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

#ifndef MGUNMANAGEDDATAMANAGER_H_
#define MGUNMANAGEDDATAMANAGER_H_

class MG_SERVER_MANAGER_API MgUnmanagedDataManager : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgUnmanagedDataManager)

/// Constructors/Destructor

public:

    virtual ~MgUnmanagedDataManager();

private:

    // Unimplemented copy constructor and assignment operator.

    MgUnmanagedDataManager();
    MgUnmanagedDataManager(const MgUnmanagedDataManager&);
    MgUnmanagedDataManager& operator=(const MgUnmanagedDataManager&);

    void GetFiles(MgStringCollection* files, CREFSTRING mappingName, CREFSTRING rootdir, CREFSTRING subdir, CREFSTRING dataTypeFilter, INT32 depth);
    void GetDirectories(MgStringCollection* dirs, CREFSTRING mappingName, CREFSTRING rootdir, CREFSTRING subdir, INT32 depth);

    bool IsValidDataTypeFilter(CREFSTRING dataTypeFilter);
    bool IsValidFileType(CREFSTRING file, CREFSTRING dataTypeFilter);

    bool IsSdfFile(CREFSTRING file);
    bool IsShpFile(CREFSTRING file);
    bool IsDwfFile(CREFSTRING file);
    bool IsRasterFile(CREFSTRING file);
    bool IsOdbcFile(CREFSTRING file);
    bool IsAnyValidFile(CREFSTRING file);

    STRING FormatMappingName(CREFSTRING name);
    STRING FormatSubdir(CREFSTRING subdir);

/// Methods

public:

    static MgUnmanagedDataManager* GetInstance();
    virtual void Dispose();

    MgStringCollection* EnumerateUnmanagedData(CREFSTRING mappingName, CREFSTRING dataTypeFilter, INT32 depth);
    MgStringCollection* EnumerateUnmanagedDataMappings();

/// Data Members

private:

    static Ptr<MgUnmanagedDataManager> sm_unmanagedDataManager;

    static const STRING Sdf;
    static const STRING Shp;
    static const STRING Dwf;
    static const STRING Raster;
    static const STRING Odbc;
    static const STRING Folder;
    static const STRING Jpg;
    static const STRING Jpeg;
    static const STRING Jpe;
    static const STRING Png;
    static const STRING Bmp;
    static const STRING Cal;
    static const STRING Tga;
    static const STRING Tif;
    static const STRING Tiff;
    static const STRING Sid;
    static const STRING Ecw;
    static const STRING Bil;
    static const STRING Mdb;
    static const STRING OpenSquareBracket;
    static const STRING ClosedSquareBracket;
};

#endif
