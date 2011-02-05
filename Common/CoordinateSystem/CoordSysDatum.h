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

#ifndef _CCOORDINATESYSTEMDATUM_H_
#define _CCOORDINATESYSTEMDATUM_H_

namespace CSLibrary
{

class CCoordinateSystemDatum : public MgCoordinateSystemDatum
{
public:
    CCoordinateSystemDatum(MgCoordinateSystemCatalog* pCatalog);
    virtual ~CCoordinateSystemDatum();

    void InitFromCatalog(const cs_Dtdef_& def, const map<STRING, Ptr<MgDisposable> >* const ellipsoidMap = NULL);
    void GetMentorDef(cs_Dtdef_& def){def=m_DtDef;};

    virtual STRING GetDtCode();
    virtual void SetDtCode(CREFSTRING sCode);
    virtual bool IsLegalDtCode(CREFSTRING sCode);
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
    virtual INT16 GetEpsgCode();
    virtual void SetEpsgCode(INT16 epsgCode);
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
    bool m_bEncrypted;
    cs_Datum_ m_datum;
    cs_Dtdef_ m_DtDef; //we can't store everything in cs_Datum_ so we need this for source, country, group, ...
    cs_Eldef_ m_ElDef; //we can't store everything in cs_Datum_ so we need this for source, country, group, ...
    Ptr<MgCoordinateSystemCatalog> m_pCatalog;

    enum CCoordinateSystemDatumObjectVersions {
        kDtRelease0  = 0   // Initial Release
    };

    //Private member functions
    void SetString(CREFSTRING sSrc, char *pDest, UINT32 nMaxSize);
    bool Protected() const;
    bool IsInitialized() const;
    void Uninitialize();
    bool IsDatumValid();
    void SetCatalog(MgCoordinateSystemCatalog* pCatalog);

    //Friends
    friend class CCoordinateSystemGeodeticTransformation;

private:
    //Unimplemented stuff
    CCoordinateSystemDatum();
    CCoordinateSystemDatum(const CCoordinateSystemDatum&);
    CCoordinateSystemDatum& operator=(const CCoordinateSystemDatum&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMDATUM_H_
