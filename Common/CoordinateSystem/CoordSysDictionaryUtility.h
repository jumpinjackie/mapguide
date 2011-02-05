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

#ifndef _CCOORDINATESYSTEMDICTIONARYUTILITY_H_
#define _CCOORDINATESYSTEMDICTIONARYUTILITY_H_

namespace CSLibrary
{

class CCoordinateSystemDictionaryUtility : public MgCoordinateSystemDictionaryUtility
{
public:
    CCoordinateSystemDictionaryUtility(MgCoordinateSystemCatalog *pCatalog);
    virtual ~CCoordinateSystemDictionaryUtility();

    virtual MgCoordinateSystemCatalog* GetCatalog();
    virtual void CreateEllipsoidDictionary(CREFSTRING sDictPathName);
    virtual void CreateDatumDictionary(CREFSTRING sDictPathName);
    virtual void CreateCoordsysDictionary(CREFSTRING sDictPathName);
    virtual void CreateCategoryDictionary(CREFSTRING sDictPathName);

protected:
    //MgDisposable
    virtual void Dispose();

    void CreateDictionaryFile(const wchar_t* kpDict, const long klMagic);

protected:
    //Data members
    Ptr<MgCoordinateSystemCatalog> m_pCatalog;

private:
    //Unimplemented stuff
    CCoordinateSystemDictionaryUtility();
    CCoordinateSystemDictionaryUtility(const CCoordinateSystemDictionaryUtility&);
    CCoordinateSystemDictionaryUtility& operator=(const CCoordinateSystemDictionaryUtility&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMDICTIONARYUTILITY_H_
