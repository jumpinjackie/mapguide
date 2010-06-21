//
//  Copyright (C) 1996-2010 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE CORRECTNESS
//  OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE IT. AUTODESK
//  PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY DISCLAIMS ANY
//  LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//


#ifndef _DWFTK_PROPERTY_H
#define _DWFTK_PROPERTY_H


///
///\file        dwf/package/Property.h
///\brief       This file contains the DWFProperty class declaration.
///


#include "dwfcore/STL.h"
#include "dwfcore/Owner.h"
#include "dwfcore/SkipList.h"
using namespace DWFCore;

#include "dwf/Toolkit.h"
#include "dwf/package/XML.h"
#include "dwf/package/reader/PackageReader.h"
#include "dwf/package/writer/PackageWriter.h"


namespace DWFToolkit
{

///
///\ingroup     dwfpackage
///
///\class       DWFProperty   dwf/package/Property.h     "dwf/package/Property.h"
///\brief       This class represents a categorized name-value data pair.
///\since       7.0.1
///
///             Properties are available for use with the DWFPropertyContainer utility class
///             and those that specialize it.
///
class DWFProperty : public DWFXMLBuildable

#ifndef DWFTK_READ_ONLY
                  , public DWFXMLSerializable
#endif
                  , public DWFOwnable
                  _DWFTK_EXPORT_API_MEMORY_CONTROL_BASE_MEMBER
{

public:

    ///
    ///\brief   This type defines a list of DWFProperty pointers.
    ///
    typedef vector<DWFProperty*>                        tList;
    ///
    ///\brief   This type defines a mapped collection of DWFProperty pointers.
    ///
    typedef DWFStringKeyChainedSkipList<DWFProperty*>   tMap;

public:

    ///
    ///         Constructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    DWFProperty()
        throw();

    ///
    ///         Constructor
    ///
    ///\param   zName       Identifies the property within its \a zCategory.
    ///\param   zValue      The data associated with the property.
    ///\param   zCategory   An optional grouping of properties.
    ///\param   zType       An optional type defining the data.
    ///\param   zUnits      An optional type defining the data.
    ///\throw   None
    ///
    _DWFTK_API
    DWFProperty( const DWFString& zName,
                 const DWFString& zValue,
                 const DWFString& zCategory = L"",
                 const DWFString& zType = L"",
                 const DWFString& zUnits = L"" )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFProperty()
        throw();

    ///
    ///         Copy Constructor
    ///
    ///\param   rProperty   The source property from which to copy.
    ///\throw   None
    ///
    _DWFTK_API
    DWFProperty( const DWFProperty& rProperty )
        throw();

    ///
    ///         Assignment Operator
    ///
    ///\param   rProperty   The source property from which to copy.
    ///\throw   None
    ///
    _DWFTK_API
    DWFProperty& operator=( const DWFProperty& rProperty )
        throw();

    ///
    ///         Returns the property name.
    ///
    ///\return  A string identifying the property with a \a category.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& name() const
        throw()
    {
        return _zName;
    }

    ///
    ///         Modifies the property name.
    ///
    ///\param   zName       The new name.
    ///\throw   None
    ///
    _DWFTK_API
    void setName( const DWFString& zName )
        throw()
    {
        _zName.assign( zName );
    }

    ///
    ///         Returns the property value.
    ///
    ///\return  A string identifying the property data.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& value() const
        throw()
    {
        return _zValue;
    }

    ///
    ///         Modifies the property value.
    ///
    ///\param   zValue  The new value.
    ///\throw   None
    ///
    _DWFTK_API
    void setValue( const DWFString& zValue )
        throw()
    {
        _zValue.assign( zValue );
    }

    ///
    ///         Returns the property category.
    ///
    ///\return  A string identifying an optional grouping.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& category() const
        throw()
    {
        return _zCategory;
    }

    ///
    ///         Modifies the property category.
    ///
    ///\param   zCategory   The new category.
    ///\throw   None
    ///
    _DWFTK_API
    void setCategory( const DWFString& zCategory )
        throw()
    {
        _zCategory.assign( zCategory );
    }

    ///
    ///         Returns the property value type.
    ///
    ///\return  A string identifying the value type.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& type() const
        throw()
    {
        return _zType;
    }

    ///
    ///         Modifies the property value type.
    ///
    ///\param   zType       The new type.
    ///\throw   None
    ///
    _DWFTK_API
    void setType( const DWFString& zType )
        throw()
    {
        _zType.assign( zType );
    }

    ///
    ///         Returns the property value units.
    ///
    ///\return  A string identifying the data units.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& units() const
        throw()
    {
        return _zUnits;
    }

    ///
    ///         Modifies the property value units.
    ///
    ///\param   zUnits      The new units.
    ///\throw   None
    ///
    _DWFTK_API
    void setUnits( const DWFString& zUnits )
        throw()
    {
        _zUnits.assign( zUnits );
    }

    ///
    ///\copydoc DWFXMLBuildable::parseAttributeList()
    ///
    _DWFTK_API
    virtual void parseAttributeList( const char** ppAttributeList )
        throw( DWFException );

#ifndef DWFTK_READ_ONLY

    ///
    ///         This method can be used by publishers to add extra data
    ///         to property elements.  These data are not part of the 
    ///         DWF format schema and must therefore be scoped by a unique
    ///         namespace.  
    ///
    ///\param   rNamespace      The publisher-specific (non DWF) namespace for the attribute.
    ///                         This object is obtained from only those classes that support
    ///                         and implement the DWFXMLSerializable::addNamespace() method.
    ///                         This parameter is required; this method will thrown an exception
    ///                         if this parameter is not provided.
    ///\param   zName           The name of the attribute.
    ///                         This parameter is required; this method will thrown an exception
    ///                         if this parameter is not provided.
    ///\param   zName           The attribute value.
    ///                         This parameter is required; this method will thrown an exception
    ///                         if this parameter is not provided.
    ///
    _DWFTK_API
    void addXMLAttribute( const DWFXMLNamespace&    rNamespace,
                          const DWFString&          zName,
                          const DWFString&          zValue )
       throw( DWFException );

    ///
    ///\copydoc DWFXMLSerializable::serializeXML()
    ///
    _DWFTK_API
    void serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
        throw( DWFException );

#endif


private:

    DWFString _zName;
    DWFString _zValue;
    DWFString _zCategory;
    DWFString _zType;
    DWFString _zUnits;

#ifndef DWFTK_READ_ONLY

    typedef pair<DWFString, DWFString>      _tAttribute;
    typedef vector<_tAttribute>             _tAttributeList;
    typedef map<DWFString, _tAttributeList> _tAttributeMap;
    _tAttributeMap                          _oAttributes;

#endif

};

}

#endif
