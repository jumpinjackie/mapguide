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

#ifndef _MGCOORDINATESYSTEMDATUM_H_
#define _MGCOORDINATESYSTEMDATUM_H_

class MgCoordinateSystemEllipsoid;
class MgCoordinateSystemGeodeticTransformation;

class MgCoordinateSystemDatum : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemDatum)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the CS-Map coordinate system datum code
    ///
    /// \return
    /// The CS-Map coordinate system datum code
    ///
    /// \remarks
    /// This method used to be called GetCode prior to MapGuide Open Source 2.2 / MapGuide Enterprise 2011 / AutoCAD Map3D 2011
    ///
    virtual STRING GetDtCode()=0;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the CS-Map coordinate system datum code for this datum
    ///
    /// \param sCode (String/string)
    /// The CS-Map coordinate system datum code
    ///
    /// \remarks
    /// This method used to be called SetCode prior to MapGuide Open Source 2.2 / MapGuide Enterprise 2011 / AutoCAD Map3D 2011
    ///
    virtual void SetDtCode(CREFSTRING sCode)=0;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether the specified CS-Map coordinate system datum code is a legal one
    ///
    /// \return
    /// true if the given code is a legal one, false otherwise
    ///
    /// \remarks
    /// This method used to be called IsLegalCode prior to MapGuide Open Source 2.2 / MapGuide Enterprise 2011 / AutoCAD Map3D 2011
    ///
    virtual bool IsLegalDtCode(CREFSTRING sCode)=0;
    virtual bool IsValid()=0;
    virtual bool IsUsable(MgCoordinateSystemCatalog *pCatalog)=0;
    virtual bool IsSameAs(MgGuardDisposable *pDef)=0;
    virtual STRING GetDescription()=0;
    virtual void SetDescription(CREFSTRING sDesc)=0;
    virtual bool IsLegalDescription(CREFSTRING sDesc)=0;
    virtual STRING GetGroup()=0;
    virtual void SetGroup(CREFSTRING sGroup)=0;
    virtual bool IsLegalGroup(CREFSTRING sGroup)=0;
    virtual STRING GetSource()=0;
    virtual void SetSource(CREFSTRING sSource)=0;
    virtual INT16 GetEpsgCode()=0;
    virtual void SetEpsgCode(INT16 epsgCode)=0;
    virtual bool IsLegalSource(CREFSTRING sSource)=0;
    virtual bool IsProtected()=0;
    virtual INT16 GetAge()=0;
    virtual void SetProtectMode(bool bIsProtected)=0;
    virtual bool IsEncrypted()=0;
    virtual void SetEncryptMode(bool bIsEncrypted)=0;
    virtual STRING GetLocation()=0;
    virtual void SetLocation(CREFSTRING sLoc)=0;
    virtual bool IsLegalLocation(CREFSTRING sLoc)=0;
    virtual STRING GetCountryOrState()=0;
    virtual void SetCountryOrState(CREFSTRING sCountryOrState)=0;
    virtual bool IsLegalCountryOrState(CREFSTRING sCountryOrState)=0;
    virtual STRING GetEllipsoid()=0;
    virtual void SetEllipsoid(CREFSTRING sEllipsoid)=0;
    virtual MgCoordinateSystemEllipsoid* GetEllipsoidDefinition()=0;
    virtual void SetEllipsoidDefinition(MgCoordinateSystemEllipsoid *pDef)=0;
    virtual MgCoordinateSystemDatum* CreateClone()=0;
    virtual MgDisposableCollection* GetGeodeticTransformations(MgCoordinateSystemDatum *pTarget)=0;
    virtual MgCoordinateSystemCatalog* GetCatalog()=0;

INTERNAL_API:
    virtual UINT8* SerializeFrom(UINT8* pStream)=0;
    virtual UINT8* SerializeTo(UINT8* pStream)=0;
    virtual UINT32 GetSizeSerialized()=0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemDatum;
};

#endif //_MGCOORDINATESYSTEMDATUM_H_
