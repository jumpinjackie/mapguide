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

#ifndef _CCOORDINATESYSTEMCATEGORYDICTIONARY_H_
#define _CCOORDINATESYSTEMCATEGORYDICTIONARY_H_

namespace CSLibrary
{

class CCoordinateSystemCategory;

//Magic number for category dictionary.  Caution:  Don't change this number
//unless the format of the dictionary file itself changes, since this number
//gets written to the dictionary file.
const long klCategoryMagic = 0x3372ED6C;

//Convenient typedefs

class CCoordinateSystemCategoryDictionary : public MgCoordinateSystemCategoryDictionary
{
public:
    CCoordinateSystemCategoryDictionary(MgCoordinateSystemCatalog *pCatalog);
    virtual ~CCoordinateSystemCategoryDictionary();

    virtual MgCoordinateSystemCategory* NewCategory();
    virtual void Rename(CREFSTRING sOldName, CREFSTRING sNewName);

    virtual MgCoordinateSystemCatalog* GetCatalog();
    virtual STRING GetDefaultFileName();
    virtual STRING GetFileName();
    virtual void SetFileName(CREFSTRING sFileName);
    virtual STRING GetPath();
    virtual UINT32 GetSize();
    virtual void Add(MgGuardDisposable *pDefinition);
    virtual void Remove(CREFSTRING sName);
    virtual void Modify(MgGuardDisposable *pDefinition);
    virtual MgGuardDisposable* Get(CREFSTRING sName);
    virtual MgCoordinateSystemCategory* GetCategory(CREFSTRING sName);
    virtual bool Has(CREFSTRING sName);
    virtual MgCoordinateSystemEnum* GetEnum();

protected:
    //MgDisposable
    virtual void Dispose();

private:
    //Data members
    STRING m_sFileName;
    CCategoryNameIndexMap m_index;
    CCategoryNameList m_list;
    Ptr<MgCoordinateSystemCatalog> m_pCatalog;
    bool m_bIndexDirty;

    //Private member functions
    void GenerateIndex();
    void InvalidateIndex();

    CCategoryNameIndexMap& Index();
    CCategoryNameList& List();

private:
    //Unimplemented stuff
    CCoordinateSystemCategoryDictionary();
    CCoordinateSystemCategoryDictionary(const CCoordinateSystemCategoryDictionary&);
    CCoordinateSystemCategoryDictionary& operator=(const CCoordinateSystemCategoryDictionary&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMCATEGORYDICTIONARY_H_
