//
//  Copyright (C) 2003-2008 by Autodesk, Inc.  All rights reserved.
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted,
// provided that the above copyright notice appears in all copies and
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM 'AS IS' AND WITH ALL FAULTS.
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC.
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//
// $Header: /Components/Internal/DWF Toolkit/v7.1/develop/global/src/dwf/package/Property.cpp 7     10/31/05 10:08a Gopalas $
//


#include "dwf/package/Property.h"
#include "dwf/package/Constants.h"
#include "dwf/package/reader/PackageManifestReader.h"
#include "dwf/package/writer/DWFXMLSerializer.h"
using namespace DWFToolkit;


_DWFTK_API
DWFProperty::DWFProperty()
throw()
{
    ;
}

_DWFTK_API
DWFProperty::DWFProperty( const DWFString& zName,
                          const DWFString& zValue,
                          const DWFString& zCategory,
                          const DWFString& zType,
                          const DWFString& zUnits )
throw()
           : _zName( zName )
           , _zValue( zValue )
           , _zCategory( zCategory )
           , _zType( zType )
           , _zUnits( zUnits )
{
    ;
}

_DWFTK_API
DWFProperty::DWFProperty( const DWFProperty& rProperty )
throw()
           : _zName( rProperty._zName )
           , _zValue( rProperty._zValue )
           , _zCategory( rProperty._zCategory )
           , _zType( rProperty._zType )
           , _zUnits( rProperty._zUnits )

#ifndef DWFTK_READ_ONLY

           , _oAttributes( rProperty._oAttributes )

#endif
{
    ;
}

_DWFTK_API
DWFProperty&
DWFProperty::operator=( const DWFProperty& rProperty )
throw()
{
    _zName.assign( rProperty._zName );
    _zValue.assign( rProperty._zValue );
    _zCategory.assign( rProperty._zCategory );
    _zType.assign( rProperty._zType );
    _zUnits.assign( rProperty._zUnits );

#ifndef DWFTK_READ_ONLY

    _oAttributes = rProperty._oAttributes;

#endif

    return *this;
}

_DWFTK_API
DWFProperty::~DWFProperty()
throw()
{
    //
    // requirement of the DWFOwnable contract
    //
    DWFOwnable::_notifyDelete();
}

_DWFTK_API
void
DWFProperty::parseAttributeList( const char** ppAttributeList )
throw( DWFException )
{
    unsigned char nFound = 0;
    size_t iAttrib = 0;
    const char* pAttrib = NULL;

    for(; ppAttributeList[iAttrib]; iAttrib += 2)
    {
            //
            // skip over any acceptable prefixes in the element name
            //
        if (DWFCORE_COMPARE_MEMORY(DWFXML::kzNamespace_DWF, ppAttributeList[iAttrib], 4) == 0)
        {
            pAttrib = &ppAttributeList[iAttrib][4];
        } 
        else if (DWFCORE_COMPARE_MEMORY(DWFXML::kzNamespace_ECommon, ppAttributeList[iAttrib], 8) == 0)
        {
            pAttrib = &ppAttributeList[iAttrib][8];
        } 
        else if (DWFCORE_COMPARE_MEMORY(DWFXML::kzNamespace_EPlot, ppAttributeList[iAttrib], 6) == 0)
        {
            pAttrib = &ppAttributeList[iAttrib][6];
        }
        else if (DWFCORE_COMPARE_MEMORY(DWFXML::kzNamespace_EModel, ppAttributeList[iAttrib], 7) == 0)
        {
            pAttrib = &ppAttributeList[iAttrib][7];
        }
        else
        {
            pAttrib = &ppAttributeList[iAttrib][0];
        }

            //
            // set the name
            //
        if (!(nFound & 0x01) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Name) == 0))
        {
            nFound |= 0x01;

            _zName.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the value
            //
        else if (!(nFound & 0x02) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Value) == 0))
        {
            nFound |= 0x02;

            _zValue.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the type
            //
        else if (!(nFound & 0x04) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Type) == 0))
        {
            nFound |= 0x04;

            _zType.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the units
            //
        else if (!(nFound & 0x08) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Units) == 0))
        {
            nFound |= 0x08;

            _zUnits.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the category
            //
        else if (!(nFound & 0x10) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Category) == 0))
        {
            nFound |= 0x10;

            _zCategory.assign( ppAttributeList[iAttrib+1] );
        }
    }
}

#ifndef DWFTK_READ_ONLY

_DWFTK_API
void
DWFProperty::addXMLAttribute( const DWFXMLNamespace&    rNamespace,
                              const DWFString&          zName,
                              const DWFString&          zValue )
throw( DWFException )
{
        //
        // all parameters are required
        //
    if ((zName.chars() == 0) || (zValue.chars() == 0))
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, L"Attribute name and value parameters must be provided." );
    }

    _tAttributeMap::iterator iAttributeList = _oAttributes.find( rNamespace.prefix() );
    if (iAttributeList != _oAttributes.end())
    {
        iAttributeList->second.push_back( _tAttribute(zName, zValue) );
    }
    else
    {
        _tAttributeList oNewList;
        oNewList.push_back( _tAttribute(zName, zValue) );
        _oAttributes[rNamespace.prefix()] = oNewList;
    }
}

_DWFTK_API
void
DWFProperty::serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
throw( DWFException )
{
    DWFString zNamespace;

        //
        // namespace dictated by document and section type 
        //
    if (nFlags & DWFPackageWriter::eDescriptor)
    {
        zNamespace.assign( namespaceXML(nFlags) );
    }
        //
        // the dwf namespace is currently only used in the manifest
        //
    else if (nFlags & DWFPackageWriter::eManifest)
    {
        zNamespace.assign( DWFXML::kzNamespace_DWF );
    }


    rSerializer.startElement( DWFXML::kzElement_Property, zNamespace );

    rSerializer.addAttribute( DWFXML::kzAttribute_Name, _zName );
    rSerializer.addAttribute( DWFXML::kzAttribute_Value, _zValue );
    rSerializer.addAttribute( DWFXML::kzAttribute_Category, _zCategory );
    rSerializer.addAttribute( DWFXML::kzAttribute_Type, _zType );
    rSerializer.addAttribute( DWFXML::kzAttribute_Units, _zUnits );

    zNamespace.destroy();

    _tAttributeMap::iterator iAttributeList = _oAttributes.begin();
    for (; iAttributeList != _oAttributes.end(); iAttributeList++)
    {
        zNamespace.assign( iAttributeList->first );
        zNamespace.append( L":" );

        _tAttributeList& rAttributeList = iAttributeList->second;
        _tAttributeList::iterator iAttribute = rAttributeList.begin();
        for (; iAttribute != rAttributeList.end(); iAttribute++)
        {
            rSerializer.addAttribute( iAttribute->first, iAttribute->second, zNamespace );
        }
    }

    rSerializer.endElement();
}

#endif

