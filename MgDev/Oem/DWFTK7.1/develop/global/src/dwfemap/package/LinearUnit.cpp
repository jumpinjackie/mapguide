//
//  Copyright (C) 2003-2010 by Autodesk, Inc.
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

#include "./LinearUnit.h"
#include "./EMapConstants.h"
#include "./EMapUtility.h"
using namespace DWFToolkit::DWFEMapExtension;


// constructor used with XMLBuildable
DWFLinearUnit::DWFLinearUnit( )
throw() : DWFXMLBuildable()
        , _nScaleFactor(1.0)
        , _zType("")
        , _nFlags(0)
{
}

// basic constructor
DWFLinearUnit::DWFLinearUnit( const DWFString& zType,
    double nScaleFactor ) throw() : DWFXMLBuildable()
                                  , _nScaleFactor(nScaleFactor)
                                  , _zType(zType)
                                  , _nFlags(0)
{
}

// destructor
DWFLinearUnit::~DWFLinearUnit() throw()
{
}


_DWFTK_API void DWFLinearUnit::parseAttributeList( const char** ppAttributeList )
throw( DWFException )
{
}

// property accessors
const DWFString& DWFLinearUnit::type() const throw()
{
     return _zType;
}

double DWFLinearUnit::scaleFactor() const throw()
{
    return _nScaleFactor;
}

// XMLCallback support
void DWFLinearUnit::notifyStartElement( const char*  zName,
                            const char** ppAttributeList ) throw()
{
    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    _nFlags = 0x00;
    if( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_Type ) == 0)
    {
        // clear any existing type
        _zType.destroy();
        _nFlags = 0x01;
    }
    else if( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                DWFEMAPXML::kzElement_ScaleFactor ) == 0)
    {
        _nFlags = 0x02;
    }
}

void DWFLinearUnit::notifyEndElement( const char* zName ) throw()
{
    // NA todo can test for proper XML formatting
}

void DWFLinearUnit::notifyStartNamespace( const char* zPrefix,
                            const char* zURI ) throw()
{
    // NA
}

void DWFLinearUnit::notifyEndNamespace( const char* zPrefix ) throw()
{
    // NA
}

void DWFLinearUnit::notifyCharacterData( const char* zCData, int nLength )
                                    throw()
{
    if( _nFlags == 0x01)
    {
        // expat will call this callback multiple times if the
        // original string contains XML-encoded bytes
        this->_zType.append( zCData, nLength );
    }
    else if ( _nFlags == 0x02)
    {
        DWFString zBuffer(zCData,nLength);
        this->_nScaleFactor = DWFString::StringToDouble(zBuffer);
    }
}

// DWFXMLSerializable support
void DWFLinearUnit::serializeXML( DWFXMLSerializer& rSerializer,
                    unsigned int nFlags ) throw( DWFException )
{
    wchar_t zTempBuffer[_DWFEMAPTK_TEMP_BUFFER_BYTES];

    // Element LinearUnit
    rSerializer.startElement(DWFEMAPXML::kzElement_Units, DWFEMAPXML::kzNamespace_EMap);
    {
        // Element Type
        rSerializer.startElement( DWFEMAPXML::kzElement_Type, DWFEMAPXML::kzNamespace_EMap);
        {
            rSerializer.addCData( this->_zType);
        }
        rSerializer.endElement();

        // Element ScaleFactor
        rSerializer.startElement( DWFEMAPXML::kzElement_ScaleFactor, DWFEMAPXML::kzNamespace_EMap);
        {
            _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _nScaleFactor );
            rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
        }
        rSerializer.endElement(); // End ScaleFactor
    }
    rSerializer.endElement(); // End element
}

// End of file.
