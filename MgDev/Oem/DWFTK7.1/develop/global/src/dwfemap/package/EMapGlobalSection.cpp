//
//  Copyright (C) 2003-2009 by Autodesk, Inc.
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

#include "./EMapGlobalSection.h"
#include "./EMapUtility.h"
#include "../EMapVersion.h"
using namespace DWFToolkit::DWFEMapExtension;


// Reader Constructor. Called by the PackageReader when materializing this section.
DWFEMapGlobalSection::DWFEMapGlobalSection( const DWFString&  zName,
                                            const DWFString&  zTitle,
                                            DWFPackageReader* pPackageReader )
throw() : DWFGlobalSection( _DWF_FORMAT_EMAP_GLOBAL_TYPE_STRING, 
                            zName,
                            zTitle,
                            pPackageReader )
{
    ;
}

// Standard Constructor.
DWFEMapGlobalSection::DWFEMapGlobalSection( const DWFString& zTitle,
                                            const DWFSource& rSource )
throw() : DWFGlobalSection( _DWF_FORMAT_EMAP_GLOBAL_TYPE_STRING,
                            _DWF_FORMAT_EMAP_GLOBAL_TYPE_STRING,
                            zTitle,
                            _DWF_FORMAT_EMAP_VERSION_CURRENT_FLOAT,
                            rSource )
{
    ;
}

// Destructor
DWFEMapGlobalSection::~DWFEMapGlobalSection() throw()
{
    ;
}

// XMLSerializable Interface Override method 
// This method handles the serialization of this GlobalSection.
void DWFEMapGlobalSection::serializeXML( DWFXMLSerializer& rSerializer,
                                         unsigned int nFlags )
throw( DWFException )
{
    if (nFlags & DWFPackageWriter::eDescriptor)
    {
        wchar_t     zTempBuffer[_DWFEMAPTK_TEMP_BUFFER_BYTES] = {0};
        DWFString   zTempString;

        // root element
        rSerializer.startElement( DWFXML::kzElement_Global, DWFXML::kzNamespace_DWF );
        {
            // attributes
            {
                _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%#0.2g", _nVersion );

                // DWF namespace addition
                zTempString.assign( L"dwf:" );
                zTempString.append( _DWF_FORMAT_VERSION_CURRENT_WIDE_STRING );
                rSerializer.addAttribute( L"dwf", zTempString, L"xmlns:" );

                rSerializer.addAttribute( DWFXML::kzAttribute_Version, zTempBuffer );
                rSerializer.addAttribute( DWFXML::kzAttribute_ObjectID, _zObjectID );
                rSerializer.addAttribute( DWFXML::kzAttribute_Name, _zName );
            }

            // base global section for the rest
            DWFGlobalSection::serializeXML( rSerializer, nFlags );
        }
        rSerializer.endElement();
    }
    // base class for manifest
    else
    {
        DWFGlobalSection::serializeXML( rSerializer, nFlags );
    }
}

// Internal Factory class Implementation

// Internal class Factory constructor
DWFEMapGlobalSection::Factory::Factory() throw()
    : DWFGlobalSection::Factory( _DWF_FORMAT_EMAP_GLOBAL_TYPE_STRING )
{
    ;
}

// Internal class Factory destructor
DWFEMapGlobalSection::Factory::~Factory()
throw()
{
    ;
}

// Internal class Factory Build method . This method is used by the ManifestReader to construct the
// appropriate section should thir types match
DWFSection* DWFEMapGlobalSection::Factory::build( const DWFString&  zName,
                                                  const DWFString&  zTitle,
                                                  DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFEMapGlobalSection* pSection = DWFCORE_ALLOC_OBJECT( DWFEMapGlobalSection(zName, zTitle, pPackageReader) );

    if (pSection == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new section" );
    }

    return pSection;
}
