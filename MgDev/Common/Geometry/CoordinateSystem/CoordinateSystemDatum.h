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

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this coordinate system datum is valid
    ///
    /// \return
    /// true if this datum is valid. false otherwise
    ///
    virtual bool IsValid()=0;
    virtual bool IsUsable(MgCoordinateSystemCatalog *pCatalog)=0;
    virtual bool IsSameAs(MgGuardDisposable *pDef)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the description of this datum
    ///
    /// \return
    /// The description of this datum
    ///
    virtual STRING GetDescription()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the description of this datum
    ///
    /// \param sDesc (String/string)
    /// The description of this datum
    ///
    virtual void SetDescription(CREFSTRING sDesc)=0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether the given description is legal for this datum
    ///
    /// \return
    /// true if the given description is legal. false otherwise
    ///
    virtual bool IsLegalDescription(CREFSTRING sDesc)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the group for this datum
    ///
    /// \return
    /// The group for this datum 
    ///
    virtual STRING GetGroup()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the group for this datum 
    ///
    /// \param sGroup (String/string)
    /// The group for this datum
    ///
    virtual void SetGroup(CREFSTRING sGroup)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether the specified group is legal for this datum 
    ///
    /// \return
    /// true if the specified group is legal. false otherwise
    ///
    virtual bool IsLegalGroup(CREFSTRING sGroup)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the source of this datum 
    ///
    /// \return
    /// The source of this datum
    ///
    virtual STRING GetSource()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the source of this datum
    ///
    /// \param sSource (String/string)
    /// The source of this datum
    ///
    virtual void SetSource(CREFSTRING sSource)=0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the EPSG code for this datum
    ///
    /// \return
    /// The EPSG code for this datum
    /// 
    /// \since 2.3
    ///
    virtual INT16 GetEpsgCode()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the EPSG code for this datum
    ///
    /// \param epsgCode (short/int)
    /// The EPSG code for this datum
    ///
    virtual void SetEpsgCode(INT16 epsgCode)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether the given source is legal for this datum
    ///
    /// \return
    /// true if the given source is legal. false otherwise
    ///
    virtual bool IsLegalSource(CREFSTRING sSource)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this datum is protected
    ///
    /// \return
    /// true if this datum is protected. false otherwise
    ///
    virtual bool IsProtected()=0;
    virtual INT16 GetAge()=0;
    virtual void SetProtectMode(bool bIsProtected)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this datum is encrypted
    ///
    /// \return
    /// true if this datum is encrypted. false otherwise
    ///
    virtual bool IsEncrypted()=0;
    virtual void SetEncryptMode(bool bIsEncrypted)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the location of this datum
    ///
    /// \return
    /// The location of this datum
    ///
    virtual STRING GetLocation()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the location of this datum
    ///
    /// \param sLoc (String/string)
    /// The location of this datum
    ///
    virtual void SetLocation(CREFSTRING sLoc)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether the given location is valid for this datum
    ///
    /// \return
    /// true if the given location is valid. false otherwise
    ///
    virtual bool IsLegalLocation(CREFSTRING sLoc)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the country or state for this datum
    ///
    /// \return
    /// The country or state for this datum
    ///
    virtual STRING GetCountryOrState()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the country or state for this datum
    ///
    /// \param sCountryOrState (String/string)
    /// The country or state for this datum
    ///
    virtual void SetCountryOrState(CREFSTRING sCountryOrState)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether the given country or state is legal for this datum
    ///
    /// \return
    /// true if the given country or state is legal. false otherwise
    ///
    virtual bool IsLegalCountryOrState(CREFSTRING sCountryOrState)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the ellipsoid for this datum
    ///
    /// \return
    /// The ellipsoid for this datum
    ///
    virtual STRING GetEllipsoid()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the ellipsoid for this datum
    ///
    /// \param sEllipsoid (String/string)
    /// The ellipsoid
    ///
    virtual void SetEllipsoid(CREFSTRING sEllipsoid)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the ellipsoid definition for this datum
    ///
    /// \return
    /// The MgCoordinateSystemEllipsoid object
    ///
    virtual MgCoordinateSystemEllipsoid* GetEllipsoidDefinition()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the ellipsoid definition for this datum
    ///
    /// \param pDef (MgCoordinateSystemEllipsoid)
    /// The coordinate system ellipsoid
    ///
    virtual void SetEllipsoidDefinition(MgCoordinateSystemEllipsoid *pDef)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a cloned copy of this datum 
    ///
    /// \return
    /// A cloned copy of this datum
    ///
    virtual MgCoordinateSystemDatum* CreateClone()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a collection of geodetic transformation between this datum and the specified target datum
    ///
    /// \return
    /// A collection of MgCoordinateSystemGeodeticTransformation objects
    ///
    virtual MgDisposableCollection* GetGeodeticTransformations(MgCoordinateSystemDatum *pTarget)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the associated coordinate system catalog
    ///
    /// \return
    /// The associated coordinate system catalog
    ///
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
