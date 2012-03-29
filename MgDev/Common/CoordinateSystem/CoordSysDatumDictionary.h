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

#ifndef _CCOORDINATESYSTEMDATUMDICTIONARY_H_
#define _CCOORDINATESYSTEMDATUMDICTIONARY_H_

namespace CSLibrary
{

class CCoordinateSystemDatumDictionary : public MgCoordinateSystemDatumDictionary
{
public:
    CCoordinateSystemDatumDictionary(MgCoordinateSystemCatalog *pCatalog);
    virtual ~CCoordinateSystemDatumDictionary();

    virtual MgCoordinateSystemDatum* NewDatum();

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
    virtual MgCoordinateSystemDatum* GetDatum(CREFSTRING sName);
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
    cs_Dtdef_ * dtdef(const char *kpName) const;
    CCoordinateSystemEnumDatum* GetEnumImp();
    
    MgCoordinateSystemDatum* GetDatum(const cs_Dtdef_* datumDef,
        const std::vector<std::map<STRING,Ptr<MgDisposable> >*>* const ellipsoidInfos = NULL);
    
    static MgDisposableCollection* ReadAllDatums(MgCoordinateSystemDictionaryBase* targetDictionary,
        const std::vector<MgCoordinateSystemFilter*>* const filters);

private:
    //Unimplemented stuff
    CCoordinateSystemDatumDictionary();
    CCoordinateSystemDatumDictionary(const CCoordinateSystemDatumDictionary&);
    CCoordinateSystemDatumDictionary& operator=(const CCoordinateSystemDatumDictionary&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMDATUMDICTIONARY_H_
