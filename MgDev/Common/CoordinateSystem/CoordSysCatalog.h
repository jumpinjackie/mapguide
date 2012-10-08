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

#ifndef _CCOORDINATESYSTEMCATALOG_H_
#define _CCOORDINATESYSTEMCATALOG_H_

namespace CSLibrary
{

    class CCoordinateSystemGeodeticTransformDefDictionary;
    class CCoordinateSystemDictionary;
    class CCoordinateSystemDatumDictionary;
    class CCoordinateSystemEllipsoidDictionary;
    class CCoordinateSystemCategoryDictionary;
    class CCoordinateSystemGeodeticPathDictionary;
    class CCoordinateSystemGeodeticTransformDefDictionary;

class CCoordinateSystemCatalog : public MgCoordinateSystemCatalog
{
EXTERNAL_API:
    CCoordinateSystemCatalog();
    virtual ~CCoordinateSystemCatalog();

    virtual void SetDefaultDictionaryDirAndFileNames();
    virtual STRING GetDefaultDictionaryDir();
    virtual void SetDictionaryDir(CREFSTRING sDirPath);
    virtual bool AreDictionaryFilesWritable();
    virtual STRING GetDictionaryDir();

    virtual STRING GetDefaultUserDictionaryDir();
    virtual void SetUserDictionaryDir(CREFSTRING sDirPath);
    virtual STRING GetUserDictionaryDir();

    virtual void SetProtectionMode(INT16 nMode);
    virtual INT16 GetProtectionMode();
    virtual MgCoordinateSystemCategoryDictionary* GetCategoryDictionary();
    virtual MgCoordinateSystemDictionary* GetCoordinateSystemDictionary();
    virtual MgCoordinateSystemDatumDictionary* GetDatumDictionary();
    virtual MgCoordinateSystemEllipsoidDictionary* GetEllipsoidDictionary();
    virtual MgCoordinateSystemGeodeticPathDictionary* GetGeodeticPathDictionary();
    virtual MgCoordinateSystemGeodeticTransformDefDictionary* GetGeodeticTransformDefDictionary();
    virtual MgDisposableCollection* GetGeodeticTransformations(MgCoordinateSystemDatum* pSource, MgCoordinateSystemDatum *pTarget);
    virtual MgCoordinateSystemMathComparator* GetMathComparator();
    virtual MgCoordinateSystemFormatConverter* GetFormatConverter();
    virtual MgCoordinateSystemProjectionInformation* GetProjectionInformation();
    virtual MgCoordinateSystemUnitInformation* GetUnitInformation();
    virtual MgCoordinateSystemDictionaryUtility* GetDictionaryUtility();

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the library status.
    ///
    virtual LibraryStatus GetLibraryStatus();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Prepares the catalog for disposal.
    ///
    virtual void PrepareForDispose();

protected:
    //MgDisposable
    virtual void Dispose();

protected:
    //Data members
    STRING m_sDir;
    STRING m_sUserDir;

    Ptr<CCoordinateSystemDictionary> m_pCsDict;
    Ptr<CCoordinateSystemDatumDictionary> m_pDtDict;
    Ptr<CCoordinateSystemEllipsoidDictionary> m_pElDict;
    Ptr<CCoordinateSystemCategoryDictionary> m_pCtDict;
    Ptr<CCoordinateSystemGeodeticPathDictionary> m_pGpDict;
    Ptr<CCoordinateSystemGeodeticTransformDefDictionary> m_pGxDict;

    LibraryStatus m_libraryStatus;

private:
    //Unimplemented stuff
    CCoordinateSystemCatalog(const CCoordinateSystemCatalog&);
    CCoordinateSystemCatalog& operator=(const CCoordinateSystemCatalog&);

    STRING SetDictionaryDir(CREFSTRING sDirPath, bool dirWriteAccess, int (*CsMapDirFunc)(const char *pszDirectoryPath));
};

} // End of namespace

#endif //_CCOORDINATESYSTEMCATALOG_H_
