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

#include "./View.h"
#include "./EMapConstants.h"
#include "./EMapUtility.h"
using namespace DWFToolkit::DWFEMapExtension;


// constructor used with XMLBuildable
DWFView::DWFView(  )
throw(): DWFXMLBuildable( )
       , _nCenterX(0.0)
       , _nCenterY(0.0)
       , _nScale(1.0)
       , _nFlags(0x00)
{
}

// basic constructor
DWFView::DWFView( double nCenterX, double nCenterY, double nScale )
throw(): DWFXMLBuildable( )
       , _nCenterX(nCenterX)
       , _nCenterY(nCenterY)
       , _nScale(nScale)
       , _nFlags(0x00)
{
}


// destructor
DWFView::~DWFView() throw()
{
}


_DWFTK_API void DWFView::parseAttributeList( const char** ppAttributeList )
    throw( DWFException )
{
}


// property accessors
double DWFView::centerX() const throw()
{
    return _nCenterX;
}

double DWFView::centerY() const throw()
{
    return _nCenterY;
}

double DWFView::scale() const throw()
{
    return _nScale;
}


// XMLCallback support
void DWFView::notifyStartElement( const char*  zName,
                                const char** ppAttributeList ) throw()
{
    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    if (DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFEMAPXML::kzElement_CenterX ) == 0)
    {
        _nFlags = 0x01;
    }
    else if (DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFEMAPXML::kzElement_CenterY ) == 0)
    {
        _nFlags = 0x02;
    }
    else if (DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFEMAPXML::kzElement_Scale ) == 0)
    {
        _nFlags = 0x03;
    }
}

void DWFView::notifyEndElement( const char* zName ) throw()
{
    //NA
}

void DWFView::notifyStartNamespace( const char* zPrefix,
                                const char* zURI ) throw()
{
    //NA
}

void DWFView::notifyEndNamespace( const char* zPrefix ) throw()
{
    //NA
}

void DWFView::notifyCharacterData( const char* zCData, int nLength )
                                                  throw()
{
    if(_nFlags > 0x00)
    {
        DWFString zBuffer(zCData,nLength);
        double nValue = DWFString::StringToDouble(zBuffer);
        switch(_nFlags)
        {
        case 0x01:
            {
                _nCenterX = nValue;
                break;
            }
        case 0x02:
            {
                _nCenterY = nValue;
                break;
            }
        case 0x03:
            {
                _nScale = nValue;
                break;
            }
        } // end switch
    } // end if
}


// DWFXMLSerializable support
void DWFView::serializeXML( DWFXMLSerializer& rSerializer,
                        unsigned int nFlags ) throw( DWFException )
{
    wchar_t zTempBuffer[_DWFEMAPTK_TEMP_BUFFER_BYTES];

    // Element View
    rSerializer.startElement(DWFEMAPXML::kzElement_InitialView, DWFEMAPXML::kzNamespace_EMap);
    {
        // Element CenterX
        rSerializer.startElement(DWFEMAPXML::kzElement_CenterX, DWFEMAPXML::kzNamespace_EMap);
        {
            _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _nCenterX );
            rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
        }
        rSerializer.endElement();

        // Element CenterY
        rSerializer.startElement(DWFEMAPXML::kzElement_CenterY, DWFEMAPXML::kzNamespace_EMap);
        {
            _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _nCenterY );
            rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
        }
        rSerializer.endElement();

        // Element Scale
        rSerializer.startElement(DWFEMAPXML::kzElement_Scale, DWFEMAPXML::kzNamespace_EMap);
        {
            _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _nScale );
            rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
        }
        rSerializer.endElement();
    }
    rSerializer.endElement(); //End Element View
}

// End of file.
