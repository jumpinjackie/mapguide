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

#ifndef _CCOORDINATESYSTEMCATALOG_H_
#define _CCOORDINATESYSTEMCATALOG_H_

namespace CSLibrary
{
class CCoordinateSystemCategoryCollection;
class CCoordinateSystemCategory;

class CCoordinateSystemCatalog : public MgCoordinateSystemCatalog
{
EXTERNAL_API:
    CCoordinateSystemCatalog();
    virtual ~CCoordinateSystemCatalog();

    virtual void SetDefaultDictionaryDirAndFileNames();
    virtual STRING GetDefaultDictionaryDir();
    virtual void SetDictionaryDir(CREFSTRING sDirPath);
    virtual STRING GetDictionaryDir();

    virtual void SetProtectionMode(INT16 nMode);
    virtual INT16 GetProtectionMode();
    virtual MgCoordinateSystemCategoryDictionary* GetCategoryDictionary();
    virtual MgCoordinateSystemDictionary* GetCoordinateSystemDictionary();
    virtual MgCoordinateSystemDatumDictionary* GetDatumDictionary();
    virtual MgCoordinateSystemEllipsoidDictionary* GetEllipsoidDictionary();
    virtual MgDisposableCollection* GetGeodeticTransformations(MgCoordinateSystemDatum* pSource, MgCoordinateSystemDatum *pTarget);
    virtual MgCoordinateSystemMathComparator* GetMathComparator();
    virtual MgCoordinateSystemFormatConverter* GetFormatConverter();
    virtual MgCoordinateSystemProjectionInformation* GetProjectionInformation();
    virtual MgCoordinateSystemUnitInformation* GetUnitInformation();
    virtual MgCoordinateSystemDictionaryUtility* GetDictionaryUtility();
    virtual bool AreDictionaryFilesWritable();

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

public:
    // Helper methods
    CCoordinateSystemCategoryCollection* GetCoordinateSystemCategories();
    CCoordinateSystemCategory* GetCoordinateSystemCategory(CREFSTRING categoryName);

private:
    // Helper methods
    void ReadCategoryDictionary(CREFSTRING fileName);
    void ReadCategoryCoordinateSystems(CREFSTRING fileName, CCoordinateSystemCategory* category);
    STRING ReadString(FILE* file, int size);

protected:
    //MgDisposable
    virtual void Dispose();

protected:
    //Data members
    STRING m_sDir;
    STRING m_sEllipsoidDictFileName;
    STRING m_sDatumDictFileName;
    STRING m_sCoordinateSystemDictFileName;
    STRING m_sCategoryDictFileName;

    Ptr<CCoordinateSystemDictionary> m_pCsDict;
    Ptr<CCoordinateSystemDatumDictionary> m_pDtDict;
    Ptr<CCoordinateSystemEllipsoidDictionary> m_pElDict;
    Ptr<CCoordinateSystemCategoryDictionary> m_pCtDict;

    LibraryStatus m_libraryStatus;
    CCoordinateSystemCategoryCollection* m_categories;

private:
    //Unimplemented stuff
    CCoordinateSystemCatalog(const CCoordinateSystemCatalog&);
    CCoordinateSystemCatalog& operator=(const CCoordinateSystemCatalog&);
};
} // End of namespace

#endif
