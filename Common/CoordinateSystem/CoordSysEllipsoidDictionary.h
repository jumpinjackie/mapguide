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

#ifndef _CCOORDINATESYSTEMELLIPSOIDDICTIONARY_H_
#define _CCOORDINATESYSTEMELLIPSOIDDICTIONARY_H_

namespace CSLibrary
{

class CCoordinateSystemEllipsoidDictionary : public MgCoordinateSystemEllipsoidDictionary
{
public:
    CCoordinateSystemEllipsoidDictionary(MgCoordinateSystemCatalog *pCatalog);
    virtual ~CCoordinateSystemEllipsoidDictionary();

    virtual MgCoordinateSystemEllipsoid* NewEllipsoid();

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
    virtual MgCoordinateSystemEllipsoid* GetEllipsoid(CREFSTRING sName);
    virtual bool Has(CREFSTRING sName);
    virtual MgCoordinateSystemEnum* GetEnum();

protected:
    //MgDisposable
    virtual void Dispose();

protected:
    //Data members
    STRING m_sFileName;
    CSystemNameDescriptionMap *m_pmapSystemNameDescription;
    INT32 m_lMagic;
    Ptr<MgCoordinateSystemCatalog> m_pCatalog;

    //Private member functions
    cs_Eldef_ * eldef(const char *kpName) const;
    CCoordinateSystemEnumEllipsoid* GetEnumImp();

    static MgDisposableCollection* ReadAllEllipsoids(MgCoordinateSystemDictionaryBase* targetDictionary, const std::vector<MgCoordinateSystemFilter*>* const filters);
    MgCoordinateSystemEllipsoid* GetEllipsoid(const cs_Eldef_* elDef, const std::vector<std::map<STRING,Ptr<MgDisposable> >*>* const /* unused */ = NULL);

private:
    //Unimplemented stuff
    CCoordinateSystemEllipsoidDictionary();
    CCoordinateSystemEllipsoidDictionary(const CCoordinateSystemEllipsoidDictionary&);
    CCoordinateSystemEllipsoidDictionary& operator=(const CCoordinateSystemEllipsoidDictionary&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMELLISPOIDDICTIONARY_H_
