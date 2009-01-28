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

#ifndef _MGCOORDINATESYSTEMCATALOG_H_
#define _MGCOORDINATESYSTEMCATALOG_H_

class MgCoordinateSystemDictionary;
class MgCoordinateSystemDatumDictionary;
class MgCoordinateSystemEllipsoidDictionary;
class MgCoordinateSystemCategoryDictionary;
class MgCoordinateSystemGeodeticTransformation;
class MgCoordinateSystemMathComparator;
class MgCoordinateSystemFormatConverter;
class MgCoordinateSystemProjectionInformation;
class MgCoordinateSystemUnitInformation;
class MgCoordinateSystemTransform;

enum LibraryStatus
{
    lsInitialized    = 0,
    lsInitializationFailed,
    lsLoadFailed
};

class MgCoordinateSystemCatalog : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemCatalog)

PUBLISHED_API:
    virtual void SetDefaultDictionaryDirAndFileNames()=0;
    virtual STRING GetDefaultDictionaryDir()=0;
    virtual void SetDictionaryDir(CREFSTRING sDirPath)=0;
    virtual STRING GetDictionaryDir()=0;

    virtual void SetProtectionMode(INT16 nMode)=0;
    virtual INT16 GetProtectionMode()=0;

    virtual MgCoordinateSystemCategoryDictionary* GetCategoryDictionary()=0;
    virtual MgCoordinateSystemDictionary* GetCoordinateSystemDictionary()=0;
    virtual MgCoordinateSystemDatumDictionary* GetDatumDictionary()=0;
    virtual MgCoordinateSystemEllipsoidDictionary* GetEllipsoidDictionary()=0;
    virtual MgDisposableCollection* GetGeodeticTransformations(MgCoordinateSystemDatum* pSource, MgCoordinateSystemDatum *pTarget)=0;
    virtual MgCoordinateSystemMathComparator* GetMathComparator()=0;
    virtual MgCoordinateSystemFormatConverter* GetFormatConverter()=0;
    virtual MgCoordinateSystemProjectionInformation* GetProjectionInformation()=0;
    virtual MgCoordinateSystemUnitInformation* GetUnitInformation()=0;
    virtual MgCoordinateSystemDictionaryUtility* GetDictionaryUtility()=0;
    virtual bool AreDictionaryFilesWritable()=0;

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the library status.
    ///
    virtual LibraryStatus GetLibraryStatus()=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Prepares the catalog for disposal.
    ///
    virtual void PrepareForDispose()=0;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId(){return m_cls_id;};

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemCatalog;
};

#endif //_MGCOORDINATESYSTEMCATALOG_H_
