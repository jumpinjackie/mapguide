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

#ifndef _MGCOORDINATESYSTEMCATEGORY_H_
#define _MGCOORDINATESYSTEMCATEGORY_H_

class MgCoordinateSystemCatalog;
class MgCoordinateSystemEnum;

///////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Defines a category of coordinate systems
///
class MgCoordinateSystemCategory : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemCategory)

PUBLISHED_API:

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of this coordinate system category
    ///
    /// \return
    /// The name of this coordinate system category
    ///
    virtual STRING GetName()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of this coordinate system category
    ///
    /// \param sName (String/string)
    /// The name of this coordinate system category
    ///
    virtual void SetName(CREFSTRING sName)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether the given name is legal for this category
    ///
    /// \return
    /// true if the given name is legal. false otherwise
    /// 
    virtual bool IsLegalName(CREFSTRING sName)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this category is valid
    ///
    /// \return
    /// true if this category is valid
    /// 
    virtual bool IsValid()=0;
    virtual bool IsUsable(MgCoordinateSystemCatalog *pCatalog)=0;
    virtual bool IsSameAs(MgGuardDisposable *pDef)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a cloned copy of this category
    ///
    /// \return
    /// A cloned copy of this category
    /// 
    virtual MgCoordinateSystemCategory* CreateClone()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the size of this category
    ///
    /// \return
    /// The size of this category
    /// 
    virtual UINT32 GetSize()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system enumerator for this category
    ///
    /// \return
    /// The coordinate system enumerator
    /// 
    virtual MgCoordinateSystemEnum* GetEnum()=0;
    virtual void AddCoordinateSystem(CREFSTRING sName)=0;
    virtual void RemoveCoordinateSystem(CREFSTRING sName)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether this category has the specified coordinate system name
    ///
    /// \param sName (String/string)
    /// The coordinate system name
    ///
    /// \return
    /// true if the specified name exists. false otherwise
    /// 
    virtual bool HasCoordinateSystem(CREFSTRING sName)=0;
    virtual void Clear()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system in this category
    ///
    /// \return
    /// A MgStringCollection containing the names of all coordinate systems in this category
    /// 
    virtual MgStringCollection* GetCoordinateSystems()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the associated coordinate system catalog
    ///
    /// \return
    /// The associated coordinate system catalog
    /// 
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
