//
//  Copyright (C) 2003-2011 by Autodesk, Inc.
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

#include "./Bounds.h"
#include "./EMapConstants.h"
#include "./EMapUtility.h"
using namespace DWFToolkit::DWFEMapExtension;


// constructor used with XMLBuildable
DWFBounds::DWFBounds(  ) throw()
    :DWFXMLBuildable()
    ,_nMinX(0.0)
    ,_nMinY(0.0)
    ,_nMaxX(0.0)
    ,_nMaxY(0.0)
    ,_nFlags(0x00)
{
}

// basic constructor
DWFBounds::DWFBounds( double nMinX, double nMaxX, double nMinY,
                    double nMaxY ) throw()
                    :DWFXMLBuildable()
                    ,_nMinX(nMinX)
                    ,_nMinY(nMinY)
                    ,_nMaxX(nMaxX)
                    ,_nMaxY(nMaxY)
                    ,_nFlags(0x00)
{
}


// destructor
DWFBounds::~DWFBounds() throw()
{
}


_DWFTK_API void DWFBounds::parseAttributeList( const char** ppAttributeList )
    throw( DWFException )
{
}

// property accessors
double DWFBounds::minX() const throw()
{
    return this->_nMinX;
}

double DWFBounds::maxX() const throw()
{
    return this->_nMaxX;
}

double DWFBounds::minY() const throw()
{
    return this->_nMinY;
}

double DWFBounds::maxY() const throw()
{
    return this->_nMaxY;
}

// XMLCallback support
void DWFBounds::notifyStartElement( const char*  zName,
                                const char** ppAttributeList ) throw()
{
    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    _nFlags = 0x00;
    if( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_MinX ) == 0)
    {
        _nFlags = 0x01;
    }
    else if( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_MaxX ) == 0)
    {
        _nFlags = 0x02;
    }
    else if( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_MinY ) == 0)
    {
        _nFlags = 0x03;
    }
    else if (DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_MaxY ) == 0)
    {
        _nFlags = 0x04;
    }
}

void DWFBounds::notifyEndElement( const char* zName ) throw()
{
    // NA : todo - this method can be used to validate the child
    //      elements for this element.
}

void DWFBounds::notifyStartNamespace( const char* zPrefix,
                                const char* zURI ) throw()
{
    // NA
}

void DWFBounds::notifyEndNamespace( const char* zPrefix ) throw()
{
    // NA
}

void DWFBounds::notifyCharacterData( const char* zCData, int nLength )
                                        throw()
{
    if( _nFlags > 0x00 )
    {
        DWFString zBuffer(zCData,nLength);
        double nValue = DWFString::StringToDouble(zBuffer);
        switch( _nFlags)
        {
        case 0x01:
            {
                this->_nMinX = nValue;
                break;
            }
        case 0x02:
            {
                this->_nMaxX = nValue;
                break;
            }
        case 0x03:
            {
                this->_nMinY = nValue;
                break;
            }
        case 0x04:
            {
                this->_nMaxY = nValue;
                break;
            }
        }
    }
}


// DWFXMLSerializable support
void DWFBounds::serializeXML( DWFXMLSerializer& rSerializer,
                        unsigned int nFlags ) throw( DWFException )
{
    wchar_t zTempBuffer[_DWFEMAPTK_TEMP_BUFFER_BYTES];
    rSerializer.startElement(DWFEMAPXML::kzElement_Bounds, DWFEMAPXML::kzNamespace_EMap);
    {
        // MinX element
        rSerializer.startElement(DWFEMAPXML::kzElement_MinX, DWFEMAPXML::kzNamespace_EMap);
        {
            _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _nMinX );
            rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
        }
        rSerializer.endElement();

        // MaxX element
        rSerializer.startElement(DWFEMAPXML::kzElement_MaxX, DWFEMAPXML::kzNamespace_EMap);
        {
            _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _nMaxX );
            rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
        }
        rSerializer.endElement();

        // MinY element
        rSerializer.startElement(DWFEMAPXML::kzElement_MinY, DWFEMAPXML::kzNamespace_EMap);
        {
            _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _nMinY );
            rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
        }
        rSerializer.endElement();

        // MaxY element
        rSerializer.startElement(DWFEMAPXML::kzElement_MaxY, DWFEMAPXML::kzNamespace_EMap);
        {
            _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _nMaxY );
            rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
        }
        rSerializer.endElement();
    }
    rSerializer.endElement(); //end Bounds element
}

// End of file.
