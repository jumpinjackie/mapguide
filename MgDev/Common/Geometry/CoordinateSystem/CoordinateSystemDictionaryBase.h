//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _MGCOORDINATESYSTEMDICTIONARYBASE_H_
#define _MGCOORDINATESYSTEMDICTIONARYBASE_H_

/// \defgroup MgCoordinateSystemDictionaryBase MgCoordinateSystemDictionaryBase
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// Basic functionality for all kinds of dictionaries
///

class MgCoordinateSystemDictionaryBase : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemDictionaryBase)

PUBLISHED_API:
    virtual MgCoordinateSystemCatalog* GetCatalog()=0;
    virtual STRING GetDefaultFileName()=0;
    virtual STRING GetFileName()=0;
    virtual void SetFileName(CREFSTRING sFileName)=0;
    virtual STRING GetPath()=0;
    virtual UINT32 GetSize()=0;
    virtual void Add(MgGuardDisposable *pDefinition)=0;
    virtual void Remove(CREFSTRING sName)=0;
    virtual void Modify(MgGuardDisposable *pDefinition)=0;
    virtual MgGuardDisposable* Get(CREFSTRING sName)=0; // TODO - this needs to be removed!
    virtual bool Has(CREFSTRING sName)=0;
    virtual MgCoordinateSystemEnum* GetEnum()=0;
};
/// \}

#endif //_MGCOORDINATESYSTEMDICTIONARYBASE_H_
