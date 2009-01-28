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

#include "./TransactionSection.h"
#include "./Command.h"
#include "../EMapConstants.h"
#include "../EMapUtility.h"
#include "../../EMapVersion.h"
using namespace DWFToolkit::DWFEMapExtension;


// Construction , Destruction
DWFEMapTransactionSection::DWFEMapTransactionSection(
                                        const DWFString& zName,
                                        const DWFString& zTitle,
                                        DWFPackageReader* pPackageReader )
throw() : DWFSection( _DWF_FORMAT_EMAP_TRANSACTION_TYPE_STRING,
                                            zName,
                                            zTitle,
                                            pPackageReader )
        , _zMapObjectID("")
        , _nSequenceID(0)
        , _nFlag(0x11)
        , _poCommand(NULL)
        , _poPackageReader(pPackageReader)
{
}


DWFEMapTransactionSection::DWFEMapTransactionSection(
                                const DWFString& zTitle,
                                const DWFString& zObjectID,
                                const DWFSource& rSource,
                                unsigned int nSequenceID,
                                const DWFString& zMapObjectID)
throw() : DWFSection( _DWF_FORMAT_EMAP_TRANSACTION_TYPE_STRING,
                      zTitle,
                      zObjectID,
                      _DWF_FORMAT_EMAP_VERSION_CURRENT_FLOAT,
                      0,   // Plot order is not needed for EMapTransaction
                      rSource )
        , DWFEMapTransactionSectionDescriptorReader()
        , _zMapObjectID(zMapObjectID)
        , _nSequenceID(nSequenceID)
        , _nFlag(0x11)
        , _poCommand(NULL)
        , _poPackageReader(NULL)
{
}


DWFEMapTransactionSection::~DWFEMapTransactionSection(void)
throw()
{
    // Delete Command collection
    for( CommandCollection::iterator itr = _oCommands.begin();
                                itr != _oCommands.end() ; itr++ )
    {
        DWFCORE_FREE_OBJECT(*itr );
    }
}


// Operations
unsigned int DWFEMapTransactionSection::sequenceID() const
throw()
{
    return this->_nSequenceID;
}

const DWFString& DWFEMapTransactionSection::mapObjectID() const
throw()
{
    return _zMapObjectID;
}

// access all the commands in the EMapTransaction section
const DWFEMapCommand* DWFEMapTransactionSection::getCommand( size_t index ) const
throw()
{
    DWFEMapCommand* pRetValue = NULL;

    if( index < _oCommands.size() )
    {
        pRetValue = _oCommands.at( index );
    }

    return pRetValue;
}

size_t DWFEMapTransactionSection::commandCount() const throw()
{
    return _oCommands.size();
}
// adds a command to the EMapTransaction section
void DWFEMapTransactionSection::addCommand(DWFEMapCommand* pCommand) throw(DWFException)
{
    if (pCommand != NULL)
    {
        _oCommands.push_back(pCommand);
    }
}


// XMLCallback Support
void DWFEMapTransactionSection::notifyStartElement( const char*  zName,
                                                    const char** ppAttributeList )
throw()
{
    // Parsing is invalid if PackageReader is NULL
    if ( _poPackageReader == NULL )
    {
        return;
    }

    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    switch (_nFlag)
    {
    case 0x11:  // default
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                            DWFEMAPXML::kzElement_MapTransaction ) == 0)
            {
                _nFlag = 0x00; // can start processing other elements
                this->_processAttributes( ppAttributeList);
            }
            break;
        }
    case 0x00:  // at the MapTransaction element
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                            DWFEMAPXML::kzElement_Commands ) == 0)
            {
                _nFlag = 0x01;
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                            DWFXML::kzElement_Properties ) == 0)
            {
                _nFlag = 0x02;
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                            DWFXML::kzElement_Resources ) == 0)
            {
                _nFlag = 0x03;
            }
            break;
        }
    case 0x01:  // in the Commands collection
        {
            DWFEMapCommand* pCommand = DWFEMapCommand::ConstructCommand( zElement,
                                                                         ppAttributeList,
                                                                         _poPackageReader );
            if ( pCommand != NULL ) // it is a start element
            {
                _oCommands.push_back(pCommand);
                _poCommand = pCommand;
            }
            // it is not a start element but there is an active Command
            else if ( _poCommand != NULL )
            {
                _poCommand->notifyStartElement(zName, ppAttributeList);
            }
            break;
        }
    case 0x02:  // in the Properties collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                            DWFXML::kzElement_Property ) == 0)
            {
                 DWFProperty* pProperty = DWFCORE_ALLOC_OBJECT( DWFProperty() );
                 pProperty->parseAttributeList( ppAttributeList );
                 this->addProperty( pProperty, true );
            }
            break;
        }
    case 0x03:  // in the Resources collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFXML::kzElement_Resource) == 0 )
            {
                DWFResource* pResource = buildResource( ppAttributeList, _poPackageReader );
                this->addResource( pResource, true );
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFXML::kzElement_GraphicResource) == 0 )
            {
                DWFGraphicResource* pResource = buildGraphicResource( ppAttributeList, _poPackageReader );
                this->addResource( (DWFResource*)pResource, true );
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFXML::kzElement_ImageResource) == 0 )
            {
                DWFImageResource* pResource = buildImageResource( ppAttributeList,_poPackageReader );
                this->addResource( (DWFResource*)pResource, true );
            }
            break;
        }
    }
}


void DWFEMapTransactionSection::notifyEndElement( const char* zName )
throw()
{
    if( _poPackageReader == NULL)
    {
        return;
    }

    const char* zElement = DWFEMapUtility::offsetKnownPrefix( zName );

    switch( _nFlag )
    {
    case 0x00:  // in the MapTransaction element
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                            DWFEMAPXML::kzElement_MapTransaction ) == 0)
            {
                _nFlag = 0x11;
            }
            break;
        }
    case 0x01:  // in the Commands collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                            DWFEMAPXML::kzElement_Commands ) == 0)
            {
                _nFlag = 0x00;  // reset the flag
                _poCommand = NULL;
            }
            else if ( _poCommand != NULL )  // redundant check : _poCommand is never NULL
            {
                _poCommand->notifyEndElement( zName );
            }
            break;
        }
    case 0x02:  // in the Properties collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                            DWFXML::kzElement_Properties ) == 0)
            {
                _nFlag = 0x00;  // reset the flag
            }
            break;
        }
    case 0x03:  // in the Resources collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                            DWFXML::kzElement_Resources ) == 0)
            {
                _nFlag = 0x00;  // reset the flag
            }
            break;
        }
    }
}

void DWFEMapTransactionSection::notifyCharacterData( const char* zCData,
                                                    int nLength )
                                                    throw()
{
    // only Commands need to be processed
    if ( _nFlag == 0x01 && _poCommand != NULL )
    {
        _poCommand->notifyCharacterData( zCData, nLength );
    }
}


void DWFEMapTransactionSection::notifyStartNamespace( const char* zPrefix,
                                                      const char* zURI )
                                                      throw()
{
    // NA
}


void DWFEMapTransactionSection::notifyEndNamespace( const char* zPrefix )
throw()
{
    // NA
}

const DWFResource& DWFEMapTransactionSection::readDescriptor(
    DWFSectionDescriptorReader* pSectionDescriptorReader ) const
throw( DWFException )
{
    return DWFSection::readDescriptor( pSectionDescriptorReader
                                       ? pSectionDescriptorReader
                                       : ( DWFEMapTransactionSectionDescriptorReader* )this );
}

// DWFXMLSerializable Support
void DWFEMapTransactionSection::serializeXML( DWFXMLSerializer& rSerializer,
                                              unsigned int nFlags )
throw( DWFException )
{
    if( nFlags & DWFPackageWriter::eDescriptor )
    {
        wchar_t zTempBuffer[_DWFEMAPTK_TEMP_BUFFER_BYTES];
        DWFString zTempString;
        rSerializer.startElement( DWFEMAPXML::kzElement_MapTransaction,
                                  DWFEMAPXML::kzNamespace_EMap );
        {
            // attributes
            {
                _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%#0.2g", _nVersion );

                // EMap namespace
                zTempString.assign( L"DWF-eMap:" );
                zTempString.append( DWFString::RepairDecimalSeparators(zTempBuffer) );

                rSerializer.addAttribute( L"eMap", zTempString, L"xmlns:" );

                // DWF namespace addition
                zTempString.assign( L"dwf:" );
                zTempString.append( _DWF_FORMAT_VERSION_CURRENT_WIDE_STRING );

                rSerializer.addAttribute( L"dwf", zTempString, L"xmlns:" );

                // Attribute version        :Optional
                rSerializer.addAttribute( DWFXML::kzAttribute_Version, zTempBuffer );

                // Attribute sequenceId     :Required
                _DWFCORE_SWPRINTF( zTempBuffer,
                                   _DWFEMAPTK_TEMP_BUFFER_BYTES,
                                   L"%d",
                                   _nSequenceID);

                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_SequenceID,
                                          zTempBuffer,
                                          DWFEMAPXML::kzNamespace_EMap );

                // Attribute mapObjectId    :Required
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_MapObjectID,
                                          _zMapObjectID,
                                          DWFEMAPXML::kzNamespace_EMap);
            }

            // Element Commands
            if( _oCommands.size() > 0)
            {
                rSerializer.startElement( DWFEMAPXML::kzElement_Commands,
                                                 DWFEMAPXML::kzNamespace_EMap );
                {
                    for( CommandCollection::iterator itr = _oCommands.begin();
                         itr != _oCommands.end(); itr++ )
                    {
                        ( *itr )->serializeXML( rSerializer,nFlags );
                    }
                }
                rSerializer.endElement(); // end Commands element
            }

            //
            // base implementation
            //
            DWFSection::serializeXML( rSerializer, nFlags );
        }
        rSerializer.endElement(); // end element MapTransaction
    }
    else if( nFlags & DWFPackageWriter::eManifest ) // manifest : let the base class handle it
    {
        DWFSection::serializeXML( rSerializer, nFlags );
    }
}


// private methods
void DWFEMapTransactionSection::_processAttributes ( const char** ppAttributeList )
throw()
{
    unsigned char nFlag = 0;
    size_t iAttrib = 0;
    const char* pAttrib = NULL;

    // process attributes
    for(;ppAttributeList[iAttrib]!= NULL; iAttrib +=2)
    {
        pAttrib = DWFEMapUtility::offsetKnownPrefix(ppAttributeList[iAttrib]);

        if(!(nFlag & 0x01) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_SequenceID ) == 0)
        {
            nFlag |= 0x01;
            this->_nSequenceID = (unsigned int)::atoi(ppAttributeList[iAttrib + 1]);
        }
        else if(!(nFlag & 0x02) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                               DWFEMAPXML::kzAttribute_MapObjectID ) == 0)
        {
            nFlag |= 0x02;
            this->_zMapObjectID = ppAttributeList[iAttrib + 1];
        }
        else if(!(nFlag & 0x04) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                        DWFXML::kzAttribute_Version ) == 0)
        {
            nFlag |= 0x04;
            this->_nVersion = DWFString::StringToDouble(ppAttributeList[iAttrib + 1]);
        }
    }
}

// Implementation of the Factory Inner class
DWFEMapTransactionSection::Factory::Factory()
throw() : DWFSection::Factory( _DWF_FORMAT_EMAP_TRANSACTION_TYPE_STRING )
{
}


DWFEMapTransactionSection::Factory::~Factory()
throw()
{
}


DWFSection* DWFEMapTransactionSection::Factory::build( const DWFString&  zName,
                                                       const DWFString&  zTitle,
                                                       DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFSection* pSection = DWFCORE_ALLOC_OBJECT(
                            DWFEMapTransactionSection( zName,
                                                       zTitle,
                                                       pPackageReader ) );
    return pSection;
}

// End of file.
