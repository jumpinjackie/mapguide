//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _MGCOORDINATESYSTEMCATEGORY_H_
#define _MGCOORDINATESYSTEMCATEGORY_H_

class MgCoordinateSystemCatalog;
class MgCoordinateSystemEnum;

class MgCoordinateSystemCategory : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemCategory)

PUBLISHED_API:
    virtual STRING GetName()=0;
    virtual void SetName(CREFSTRING sName)=0;
    virtual bool IsLegalName(CREFSTRING sName)=0;
    virtual bool IsValid()=0;
    virtual bool IsUsable(MgCoordinateSystemCatalog *pCatalog)=0;
    virtual bool IsSameAs(MgGuardDisposable *pDef)=0;
    virtual MgCoordinateSystemCategory* CreateClone()=0;
    virtual UINT32 GetSize()=0;
    virtual MgCoordinateSystemEnum* GetEnum()=0;
    virtual void AddCoordinateSystem(CREFSTRING sName)=0;
    virtual void RemoveCoordinateSystem(CREFSTRING sName)=0;
    virtual bool HasCoordinateSystem(CREFSTRING sName)=0;
    virtual void Clear()=0;
    virtual MgStringCollection* GetCoordinateSystems()=0;
    virtual MgCoordinateSystemCatalog* GetCatalog()=0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemCategory;
};

#endif //_MGCOORDINATESYSTEMCATEGORY_H_
