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

#include "./Background.h"
#include "./EMapConstants.h"
#include "./EMapUtility.h"
using namespace DWFToolkit::DWFEMapExtension;


// basic constructor
DWFBackground::DWFBackground(  )
throw() : DWFXMLBuildable()
        , _nColorARGB(0)
        , _bIsColor(false)
{
}

// basic constructor
DWFBackground::DWFBackground( unsigned int nColorARGB )
throw() : DWFXMLBuildable()
        , _nColorARGB(nColorARGB)
        , _bIsColor(false)
{
}

// destructor
DWFBackground::~DWFBackground() throw()
{
}


_DWFTK_API void DWFBackground::parseAttributeList( const char** ppAttributeList )
    throw( DWFException )
{
}


// property accessors
const unsigned int DWFBackground::color() const throw()
{
    return this->_nColorARGB;
}

// XMLCallback support
void DWFBackground::notifyStartElement( const char*  zName,
                            const char** ppAttributeList ) throw()
{
    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    _bIsColor = false;
    if( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_Color) == 0)
    {
        _bIsColor = true;
    }
}

void DWFBackground::notifyEndElement( const char* zName ) throw()
{
    _bIsColor = false;
}

void DWFBackground::notifyStartNamespace( const char* zPrefix,
                            const char* zURI ) throw()
{
    // NA
}

void DWFBackground::notifyEndNamespace( const char* zPrefix ) throw()
{
    // NA
}

void DWFBackground::notifyCharacterData( const char* zCData, int nLength )
                                    throw()
{
    if( _bIsColor)
    {
        // Construct buffer
        char* pzBuffer = new char[nLength+1];

        // Set the Null terminator
        pzBuffer[nLength] = '\0';

        // copy string to buffer
        memcpy(pzBuffer,zCData,nLength);

        // transparency is ignored
        char* pState;
        char* pToken = DWFCORE_ASCII_STRING_TOKENIZE( pzBuffer, " ", &pState );
        _nColorARGB = ((unsigned char)::atoi(pToken) << 24); // ALPHA

        pToken = DWFCORE_ASCII_STRING_TOKENIZE( NULL, " ", &pState );
        _nColorARGB |= ((unsigned char)::atoi(pToken) << 16); // RED

        pToken = DWFCORE_ASCII_STRING_TOKENIZE( NULL, " ", &pState );
        _nColorARGB |= ((unsigned char)::atoi(pToken) << 8);  // GREEN

        pToken = DWFCORE_ASCII_STRING_TOKENIZE( NULL, " ", &pState );
        _nColorARGB |= (unsigned char)::atoi(pToken);         // BLUE

        delete[] pzBuffer;
    }
}


// DWFXMLSerializable support
void DWFBackground::serializeXML( DWFXMLSerializer& rSerializer,
                    unsigned int nFlags ) throw( DWFException )
{
    wchar_t zTempBuffer[_DWFEMAPTK_TEMP_BUFFER_BYTES];

    // write out element Background
    rSerializer.startElement( DWFEMAPXML::kzElement_Background, DWFEMAPXML::kzNamespace_EMap);
    {
        // Element Color
        rSerializer.startElement(DWFEMAPXML::kzElement_Color, DWFEMAPXML::kzNamespace_EMap);
        {
            _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES,
                               L"%d %d %d %d",
                               // (unsigned char)((_nColorARGB & 0xff000000)>>24),      with mask
                               (unsigned char)(_nColorARGB >> 24),      // ALPHA
                               (unsigned char)(_nColorARGB >> 16),      // RED
                               (unsigned char)(_nColorARGB >>  8),      // GREEN
                               (unsigned char)(_nColorARGB      ) );    // BLUE

            rSerializer.addCData(zTempBuffer);
        }
        rSerializer.endElement(); // end Color
    }
    rSerializer.endElement(); //end Background element
}

// End of file.
