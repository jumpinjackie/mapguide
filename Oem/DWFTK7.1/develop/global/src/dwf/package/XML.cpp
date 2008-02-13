//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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


#include "dwf/package/XML.h"

#include "dwf/package/Bookmark.h"
#include "dwf/package/DefinedObject.h"
#include "dwf/package/Dependency.h"
#include "dwf/package/FontResource.h"
#include "dwf/package/GraphicResource.h"
#include "dwf/package/Interface.h"
#include "dwf/package/Paper.h"
#include "dwf/package/Property.h"
#include "dwf/package/Source.h"
#include "dwf/package/Units.h"
#include "dwf/package/reader/PackageReader.h"
using namespace DWFToolkit;



//////////

template<class T>
T* _build( T*& pT, const char** ppAttributeList, DWFPackageReader* pPackageReader )
throw( DWFException )
{
    pT = DWFCORE_ALLOC_OBJECT( T(pPackageReader) );

    if (pT == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate DWFXMLBuildable object" );
    }

    pT->parseAttributeList( ppAttributeList );
    return pT;
}

template<class T>
T* _build( T*& pT, const char** ppAttributeList )
throw( DWFException )
{
    pT = DWFCORE_ALLOC_OBJECT( T );

    if (pT == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate DWFXMLBuildable object" );
    }

    pT->parseAttributeList( ppAttributeList );
    return pT;
}

//////////



DWFXMLElementBuilder::DWFXMLElementBuilder()
throw()
{
    ;
}

DWFXMLElementBuilder::~DWFXMLElementBuilder()
throw()
{
    ;
}

DWFResource*
DWFXMLElementBuilder::buildResource( const char**      ppAttributeList,
                                     DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFResource* pElement = _build( pElement, ppAttributeList, pPackageReader );
    return pElement;
}

DWFFontResource*
DWFXMLElementBuilder::buildFontResource( const char**      ppAttributeList,
                                         DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFFontResource* pElement = _build( pElement, ppAttributeList, pPackageReader );
    return pElement;
}

DWFImageResource*
DWFXMLElementBuilder::buildImageResource( const char**      ppAttributeList,
                                          DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFImageResource* pElement = _build( pElement, ppAttributeList, pPackageReader );
    return pElement;
}

DWFGraphicResource*
DWFXMLElementBuilder::buildGraphicResource( const char**      ppAttributeList,
                                            DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFGraphicResource* pElement = _build( pElement, ppAttributeList, pPackageReader );
    return pElement;
}

DWFBookmark*
DWFXMLElementBuilder::buildBookmark( const char** ppAttributeList )
throw( DWFException )
{
    DWFBookmark* pElement = _build( pElement, ppAttributeList );
    return pElement;
}

DWFDefinedObject*
DWFXMLElementBuilder::buildDefinedObject( const char** ppAttributeList )
throw( DWFException )
{
    DWFDefinedObject* pElement = _build( pElement, ppAttributeList );
    return pElement;
}

DWFDefinedObjectInstance*
DWFXMLElementBuilder::buildDefinedObjectInstance( const char**  ppAttributeList,
                                                  unsigned long nSequence )
throw( DWFException )
{
    DWFDefinedObjectInstance* pElement = DWFCORE_ALLOC_OBJECT( DWFDefinedObjectInstance(nSequence) );

    if (pElement == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate DWFXMLBuildable object" );
    }

    pElement->parseAttributeList( ppAttributeList );
    return pElement;
}

DWFDependency*
DWFXMLElementBuilder::buildDependency( const char** ppAttributeList )
throw( DWFException )
{
    DWFDependency* pElement = _build( pElement, ppAttributeList );
    return pElement;
}

DWFInterface*
DWFXMLElementBuilder::buildInterface( const char** ppAttributeList )
throw( DWFException )
{
    DWFInterface* pElement = _build( pElement, ppAttributeList );
    return pElement;
}

DWFPaper*
DWFXMLElementBuilder::buildPaper( const char** ppAttributeList )
throw( DWFException )
{
    DWFPaper* pElement = _build( pElement, ppAttributeList );
    return pElement;
}

DWFProperty*
DWFXMLElementBuilder::buildProperty( const char** ppAttributeList )
throw( DWFException )
{
    DWFProperty* pElement = _build( pElement, ppAttributeList );
    return pElement;
}

DWFSource*
DWFXMLElementBuilder::buildSource( const char** ppAttributeList )
throw( DWFException )
{
    DWFSource* pElement = _build( pElement, ppAttributeList );
    return pElement;
}

DWFUnits*
DWFXMLElementBuilder::buildUnits( const char** ppAttributeList )
throw( DWFException )
{
    DWFUnits* pElement = _build( pElement, ppAttributeList );
    return pElement;
}

//////////

#ifndef DWFTK_READ_ONLY

DWFXMLSerializable::DWFXMLSerializable( const DWFString& zDefaultNamespace )
throw()
                  : _zDefaultNamespace( zDefaultNamespace )
{
    ;
}

DWFXMLSerializable::~DWFXMLSerializable()
throw()
{
    ;
}

DWFString
DWFXMLSerializable::namespaceXML( unsigned int nFlags ) const
throw()
{
    return (nFlags & DWFPackageWriter::eEPlot)  ? DWFXML::kzNamespace_EPlot  :
           (nFlags & DWFPackageWriter::eEModel) ? DWFXML::kzNamespace_EModel :
           (_zDefaultNamespace.bytes() > 0 )    ? _zDefaultNamespace         :
                                                   DWFXML::kzNamespace_DWF;
}

void
DWFXMLSerializable::setDefaultNamespace( const DWFString& zDefaultNamespace )
throw( DWFException )
{
    _zDefaultNamespace.assign( zDefaultNamespace );
}

_DWFTK_API
const DWFXMLNamespace&
DWFXMLSerializable::addNamespace( const DWFString& /*zNamespace*/, 
                                  const DWFString& /*zXMLNS*/ )
throw( DWFException )
{
    _DWFCORE_THROW( DWFNotImplementedException, L"This serializable entity does not support additional namespaces." );
}

///////////

_DWFTK_API
DWFXMLNamespace::DWFXMLNamespace()
throw()
{
    ;
}

_DWFTK_API
DWFXMLNamespace::DWFXMLNamespace( const DWFString& zNamespace, const DWFString& zXMLNS )
throw( DWFException )
            : _zPrefix( zNamespace )
            , _zXMLNS( zXMLNS )
{
        //
        // all parameters are required
        //
    if ((_zPrefix.chars() == 0) || (_zXMLNS.chars() == 0))
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, L"The full namespace definition must be provided." );
    }

        //
        // known DWF namespaces are prohibited
        //
    if ((_zPrefix == DWFXML::kzNamespace_DWF)    ||
        (_zPrefix == DWFXML::kzNamespace_EPlot)  ||
        (_zPrefix == DWFXML::kzNamespace_EModel) ||
        (_zPrefix == DWFXML::kzNamespace_ECommon))
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, L"Cannot provide a known DWF namespace to this method." );
    }
}


_DWFTK_API
DWFXMLNamespace::~DWFXMLNamespace()
throw()
{
    ;
}

_DWFTK_API
const DWFString&
DWFXMLNamespace::prefix() const
throw()
{
    return _zPrefix;
}

_DWFTK_API
const DWFString& 
DWFXMLNamespace::xmlns() const
throw()
{
    return _zXMLNS;
}


#endif
