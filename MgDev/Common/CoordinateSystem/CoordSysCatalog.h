//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

class CCoordinateSystemCatalog : public MgCoordinateSystemCatalog
{
EXTERNAL_API:
    CCoordinateSystemCatalog();
    virtual ~CCoordinateSystemCatalog();

    virtual void SetDefaultDictionaryDirAndFileNames();
    virtual void SetDictionaryDir(CREFSTRING sDirPath);
    virtual void SetDictionaryFileNames(CREFSTRING sEllipsoidDictFileName, CREFSTRING sDatumDictFileName, CREFSTRING sCoordinateSystemDictFileName, CREFSTRING sCategoryDictFileName);
    virtual void GetDefaultDictionaryDirAndFileNames(REFSTRING sDir, REFSTRING sEllipsoidDictFileName, REFSTRING sDatumDictFileName, REFSTRING sCoordinateSystemDictFileName, REFSTRING sCategoryDictFileName);

    virtual STRING GetDictionaryDir();
    virtual STRING GetCategoryFileName();
    virtual STRING GetCoordinateSystemFileName();
    virtual STRING GetDatumFileName();
    virtual STRING GetEllipsoidFileName();

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

	CCoordinateSystemDictionary *m_pCsDict;
	CCoordinateSystemDatumDictionary *m_pDtDict;
	CCoordinateSystemEllipsoidDictionary *m_pElDict;
	CCoordinateSystemCategoryDictionary *m_pCtDict;

    LibraryStatus m_libraryStatus;

private:
	//Unimplemented stuff
	CCoordinateSystemCatalog(const CCoordinateSystemCatalog&);
	CCoordinateSystemCatalog& operator=(const CCoordinateSystemCatalog&);
};

} // End of namespace

#endif
