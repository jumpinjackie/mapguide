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

#ifndef _CCOORDINATESYSTEMDATUM_H_
#define _CCOORDINATESYSTEMDATUM_H_

namespace CSLibrary
{

class CCoordinateSystemDatum : public MgCoordinateSystemDatum
{
public:
    CCoordinateSystemDatum(MgCoordinateSystemCatalog* pCatalog);
    virtual ~CCoordinateSystemDatum();

    virtual STRING GetCode();
    virtual void SetCode(CREFSTRING sCode);
    virtual bool IsLegalCode(CREFSTRING sCode);
    virtual bool IsValid();
    virtual bool IsUsable(MgCoordinateSystemCatalog *pCatalog);
    virtual bool IsSameAs(MgGuardDisposable *pDef);
    virtual STRING GetDescription();
    virtual void SetDescription(CREFSTRING sDesc);
    virtual bool IsLegalDescription(CREFSTRING sDesc);
    virtual STRING GetGroup();
    virtual void SetGroup(CREFSTRING sGroup);
    virtual bool IsLegalGroup(CREFSTRING sGroup);
    virtual STRING GetSource();
    virtual void SetSource(CREFSTRING sSource);
    virtual bool IsLegalSource(CREFSTRING sSource);
    virtual bool IsProtected();
    virtual INT16 GetAge();
    virtual void SetProtectMode(bool bIsProtected);
    virtual bool IsEncrypted();
    virtual void SetEncryptMode(bool bIsEncrypted);
    virtual STRING GetLocation();
    virtual void SetLocation(CREFSTRING sLoc);
    virtual bool IsLegalLocation(CREFSTRING sLoc);
    virtual STRING GetCountryOrState();
    virtual void SetCountryOrState(CREFSTRING sCountryOrState);
    virtual bool IsLegalCountryOrState(CREFSTRING sCountryOrState);
    virtual STRING GetEllipsoid();
    virtual void SetEllipsoid(CREFSTRING sEllipsoid);
    virtual MgCoordinateSystemEllipsoid* GetEllipsoidDefinition();
    virtual void SetEllipsoidDefinition(MgCoordinateSystemEllipsoid *pDef);
    virtual MgCoordinateSystemDatum* CreateClone();
    virtual MgDisposableCollection* GetGeodeticTransformations(MgCoordinateSystemDatum *pTarget);
    virtual MgCoordinateSystemCatalog* GetCatalog();

    virtual UINT8* SerializeFrom(UINT8* pStream);
    virtual UINT8* SerializeTo(UINT8* pStream);
    virtual UINT32 GetSizeSerialized();

protected:
    //from MgDisposable
    virtual void Dispose();

protected:
    //Data members
    Ptr<MgCoordinateSystemCatalog> m_pCatalog;

    //Private member functions
    void SetCatalog(MgCoordinateSystemCatalog* pCatalog);

private:
    //Unimplemented stuff
    CCoordinateSystemDatum(const CCoordinateSystemDatum&);
    CCoordinateSystemDatum& operator=(const CCoordinateSystemDatum&);
    CCoordinateSystemDatum();
};

} // End of namespace

#endif //_CCOORDINATESYSTEMDATUM_H_
