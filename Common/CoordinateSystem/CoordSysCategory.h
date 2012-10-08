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

#ifndef _CCOORDINATESYSTEMCATEGORY_H_
#define _CCOORDINATESYSTEMCATEGORY_H_

struct cs_Ctdef_;

namespace CSLibrary
{

class CCoordinateSystemCategory : public MgCoordinateSystemCategory
{
public:
    CCoordinateSystemCategory(MgCoordinateSystemCatalog *pCatalog);
    CCoordinateSystemCategory(MgCoordinateSystemCatalog *pCatalog, cs_Ctdef_* pCategory);
    CCoordinateSystemCategory(CCoordinateSystemCategory const&);
    CCoordinateSystemCategory& operator=(const CCoordinateSystemCategory&);

    virtual ~CCoordinateSystemCategory();

    void CopyFrom(CCoordinateSystemCategory const* pDef);

    //MgCoordinateSystemCategory
    virtual STRING GetName();
    virtual void SetName(CREFSTRING sName);
    virtual bool IsLegalName(CREFSTRING sName);
    virtual bool IsValid();
    virtual bool IsUsable(MgCoordinateSystemCatalog *pCatalog);
    virtual bool IsSameAs(MgGuardDisposable *pDef);
    virtual MgCoordinateSystemCategory* CreateClone();
    virtual UINT32 GetSize();
    virtual MgCoordinateSystemEnum* GetEnum();
    virtual MgStringCollection* GetCoordinateSystems();
    virtual void AddCoordinateSystem(CREFSTRING sName);
    virtual void RemoveCoordinateSystem(CREFSTRING sName);
    virtual bool HasCoordinateSystem(CREFSTRING sName);
    virtual void Clear();
    virtual MgCoordinateSystemCatalog* GetCatalog();

    cs_Ctdef_ const* GetCategoryDef() const;

protected:
    //MgDisposable
    virtual void Dispose();

protected:
    //Data members
    std::vector<STRING> m_listCoordinateSystemNames;
    Ptr<MgCoordinateSystemCatalog> m_pCatalog;

    //Private member functions
    static bool IsLegalName(const char *kpName);

private:

    std::vector<STRING>& GetAllCsNames();
    void ClearAllCsNames();

    cs_Ctdef_ * mp_ctDef;
    static void CtorInit(CCoordinateSystemCategory* pToInitialize, MgCoordinateSystemCatalog *pCatalog, cs_Ctdef_* pSourceCtDef = NULL, bool copyDef = true);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMCATEGORY_H_
