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

#ifndef _CCOORDINATESYSTEMCATEGORY_H_
#define _CCOORDINATESYSTEMCATEGORY_H_

namespace CSLibrary
{
class CCoordinateSystemInformation;
typedef vector<CCoordinateSystemInformation*> CoordinateSystemInformationVector;

class CCoordinateSystemCategory : public MgCoordinateSystemCategory
{
public:
    CCoordinateSystemCategory(MgCoordinateSystemCatalog *pCatalog);
    virtual ~CCoordinateSystemCategory();

    // MgCoordinateSystemCategory interface
    virtual STRING GetName();
    virtual void SetName(CREFSTRING sName);
    virtual bool IsLegalName(CREFSTRING sName);
    virtual bool IsValid();
    virtual bool IsUsable(MgCoordinateSystemCatalog *pCatalog);
    virtual bool IsSameAs(MgGuardDisposable *pDef);
    virtual MgCoordinateSystemCategory* CreateClone();
    virtual UINT32 GetSize();
    virtual MgCoordinateSystemEnum* GetEnum();
    virtual void AddCoordinateSystem(CREFSTRING sName);
    virtual void RemoveCoordinateSystem(CREFSTRING sName);
    virtual bool HasCoordinateSystem(CREFSTRING sName);
    virtual void Clear();
    virtual MgStringCollection* GetCoordinateSystems();
    virtual MgCoordinateSystemCatalog* GetCatalog();


    // Helper methods
    void Add(CCoordinateSystemInformation* coordSysInfo);
    size_t ContainsCode(CREFSTRING coordSysCode);
    size_t ContainsProj4(CREFSTRING proj4Defn);
    CoordinateSystemInformationVector* GetCoordinateSystemsInfo();

protected:
    //MgDisposable
    virtual void Dispose();

protected:
    //Data members

    CCategoryName m_categoryName;
    CSystemNameList m_listCoordinateSystemNames;
    Ptr<MgCoordinateSystemCatalog> m_pCatalog;

private:
    //Unimplemented stuff
    CCoordinateSystemCategory();
    CCoordinateSystemCategory(const CCoordinateSystemCategory&);
    CCoordinateSystemCategory& operator=(const CCoordinateSystemCategory&);

    CoordinateSystemInformationVector* m_coordinateSystems;
};

} // End of namespace

#endif
