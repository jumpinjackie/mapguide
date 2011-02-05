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

#ifndef _MGCOORDINATESYSTEMFORMATCONVERTER_H_
#define _MGCOORDINATESYSTEMFORMATCONVERTER_H_

class MgCoordinateSystemFormatConverter : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemFormatConverter)

PUBLISHED_API:
    //WKT methods
    //use MgCoordinateSystemWktFlavor values for the WKT format
    //use MgCoordinateSystemCodeFormat values for the code format
    virtual STRING DefinitionToWkt(MgCoordinateSystem* pSource, INT32 nWktFlavor)=0;
    virtual MgCoordinateSystem* WktToDefinition(INT32 nWktFlavor, CREFSTRING sWkt)=0;
    virtual STRING CodeToWkt(INT32 nFormatSource, CREFSTRING sCodeSource, INT32 nWktFlavor)=0;
    virtual STRING WktToCode(INT32 nWktFlavor, CREFSTRING sWkt, INT32 nFormatDestination)=0;

    //code format conversion
    virtual STRING DefinitionToCode(MgCoordinateSystem* pSource, INT32 nFormatDestination)=0;
    virtual MgCoordinateSystem* CodeToDefinition(INT32 nFormatSource, CREFSTRING sCodeSource)=0;
    virtual STRING CodeToCode(INT32 nFormatSource, CREFSTRING sCodeSource, INT32 nFormatDestination)=0;
    virtual bool IsCodeInDictionary(INT32 nFormat, CREFSTRING sCode)=0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemFormatConverter;
};

#endif //_MGCOORDINATESYSTEMFORMATCONVERTER_H_
