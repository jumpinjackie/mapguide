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


#ifndef DWFTK_READ_ONLY


#include "dwfcore/MIME.h"
using namespace DWFCore;

#include "dwf/Version.h"
#include "dwf/package/XML.h"
#include "dwf/package/Manifest.h"
#include "dwf/package/Constants.h"
#include "dwf/package/EPlotSection.h"
#include "dwf/package/EModelSection.h"
#include "dwf/package/GlobalSection.h"
#include "dwf/package/CustomSection.h"
#include "dwf/package/utility/PropertyContainer.h"
#include "dwf/package/writer/extensions/6.0/PackageVersionExtension.h"
#include "dwf/package/writer/extensions/6.01/PackageVersionExtension.h"
using namespace DWFToolkit;




    //
    // used for temporary buffer to transfer bytes
    // from a resource to the dwf archive
    //
#define _DWFTK_PACKAGE_WRITER_RESOURCE_STREAM_BUFFER_BYTES  16384

//DNT_Start

_DWFTK_API
DWFPackageWriter::DWFPackageWriter( const DWFFile&              rDWFPackageFile,
                                    const DWFString&            zDWFPackagePassword,
                                    DWFPackageVersionExtension* pVersionExtension )
throw()
                : _rDWFPackage( rDWFPackageFile )
                , _zDWFPassword( zDWFPackagePassword )
                , _pXMLSerializer( NULL )
                , _pVersionExtension( pVersionExtension )
                , _pPackageDescriptor( NULL )
                , _pPackageManifest( NULL )
                , _nNextPlot( 1.0 )
                , _bEPlotGlobalAdded( false )
                , _bEModelGlobalAdded( false )
                , _nEPlotSectionCount( 0 )
                , _nEModelSectionCount( 0 )
{
}

_DWFTK_API
DWFPackageWriter::~DWFPackageWriter()
throw()
{
    if (_pVersionExtension)
    {
        DWFCORE_FREE_OBJECT( _pVersionExtension );
    }

    if (_pXMLSerializer)
    {
        DWFCORE_FREE_OBJECT( _pXMLSerializer );
    }

    if (_pPackageDescriptor)
    {
        DWFCORE_FREE_OBJECT( _pPackageDescriptor );
    }

    if (_pPackageManifest)
    {
        DWFCORE_FREE_OBJECT( _pPackageManifest );
    }
}

_DWFTK_API
void
DWFPackageWriter::addSection( DWFSection*   pSection,
                              DWFInterface* pInterface )
throw( DWFException )
{
    if (pSection == NULL)
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, L"No section provided" );
    }

        //
        // always route through the version extension if it exists
        // and return immediately if instructed (ext. will return false)
        //
    if (_pVersionExtension && (_pVersionExtension->addSection(pSection, pInterface) == false))
    {
        return;
    }

        //
        // trap global sections
        //
    {
        DWFGlobalSection* pGlobal = dynamic_cast<DWFGlobalSection*>(pSection);
        if (pGlobal)
        {
            return addGlobalSection( pGlobal );
        }
    }

    //
    //
    //
    _ensureManifest();

        //
        // ask the section for it's interface (it may not exist)
        //
    if (pInterface == NULL)
    {
        pInterface = pSection->buildInterface();
    }

        //
        // 
        //
    if (pInterface)
    {
        //
        // add to the manifest
        //
        _pPackageManifest->provideInterface( pInterface );
    }

    //
    // a custom section can still act like a known dwf section
    //
    DWFCustomSection* pCustomSection = dynamic_cast<DWFCustomSection*>(pSection);
    DWFCustomSection::tBehavior tSectionBehavior = {0};

    if (pCustomSection)
    {
        tSectionBehavior = pCustomSection->behavior();
    }

        //
        // modify the section before caching
        //
    if ((pCustomSection == NULL) || tSectionBehavior.bRenameOnPublish)
    {
        pSection->rename( _oUUID.next(true) );
    }

        //
        // manage section descriptor resource
        //
    if ((pCustomSection == NULL) || tSectionBehavior.bPublishDescriptor)
    {
        pSection->reorder( _nNextPlot++ );

        //
        // the HREF of this descriptor
        //
        DWFString zHREF( pSection->name() );
        zHREF.append( L"\\descriptor.xml" );

        //
        // create the descriptor resource
        //
        DWFResource* pDescriptor = DWFCORE_ALLOC_OBJECT( DWFResource(L"",
                                                            DWFXML::kzRole_Descriptor,
                                                            DWFMIME::kzMIMEType_XML,
                                                            zHREF) );
        if (pDescriptor == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate desriptor resource" );
        }

        //
        // remove any previous descriptor
        // and have it destroyed if the section owns it
        //
        pSection->removeResourcesByRole( DWFXML::kzRole_Descriptor, true );

        //
        // add a descriptor resource to the section
        //
        pSection->addResource( pDescriptor, true );
    }

    //
    // add the section to the manifest
    //
    _pPackageManifest->provideSection( pSection );

    //
    // cache the section
    //
    _oSections.push_back( pSection );

    //
    // increment the section specific count based on the section type
    //
    const DWFString& zSectionType = pSection->type();

    if (zSectionType == _DWF_FORMAT_EPLOT_TYPE_STRING)
    {
        _nEPlotSectionCount++;
    }
    else if (zSectionType == _DWF_FORMAT_EMODEL_TYPE_STRING)
    {
        _nEModelSectionCount++;
    }
}

_DWFTK_API
void
DWFPackageWriter::addGlobalSection( DWFGlobalSection* pSection )
throw( DWFException )
{
    if (pSection == NULL)
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, L"No section provided" );
    }

        //
        // always route through the version extension if it exists
        // and return immediately if instructed (ext. will return false)
        //
    if (_pVersionExtension && (_pVersionExtension->addGlobalSection(pSection) == false))
    {
        return;
    }

    //
    //
    //
    _ensureManifest();

        //
        // ensure only one global section per type
        //
    if (pSection->type() == _DWF_FORMAT_EPLOT_GLOBAL_TYPE_STRING)
    {
        if (_bEPlotGlobalAdded)
        {
            DWFString zEx( L"Global section already exists [" );
                      zEx.append( _DWF_FORMAT_EPLOT_GLOBAL_TYPE_STRING );
                      zEx.append( L"]" );

            _DWFCORE_THROW( DWFUnexpectedException, zEx );
        }

        _bEPlotGlobalAdded = true;
    }
    else if (pSection->type() == _DWF_FORMAT_EMODEL_GLOBAL_TYPE_STRING)
    {
        if (_bEModelGlobalAdded)
        {
            DWFString zEx( L"Global section already exists [" );
                      zEx.append( _DWF_FORMAT_EMODEL_GLOBAL_TYPE_STRING );
                      zEx.append( L"]" );

            _DWFCORE_THROW( DWFUnexpectedException, zEx );
        }

        _bEModelGlobalAdded = true;
    }

    //
    // the HREF of this descriptor
    //
    DWFString zHREF( pSection->name() );
    zHREF.append( L"\\descriptor.xml" );

    //
    // create the descriptor resource
    //
    DWFResource* pDescriptor = DWFCORE_ALLOC_OBJECT( DWFResource(L"",
                                                        DWFXML::kzRole_Descriptor,
                                                        DWFMIME::kzMIMEType_XML,
                                                        zHREF) );
    if (pDescriptor == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate desriptor resource" );
    }

    //
    // remove any previous descriptor
    // and have it destroyed if the section owns it
    //
    pSection->removeResourcesByRole( DWFXML::kzRole_Descriptor, true );

    //
    // add a descriptor resource to the section
    //
    pSection->addResource( pDescriptor, true );

    //
    // add the section to the manifest
    //
    _pPackageManifest->provideSection( pSection );

    //
    // cache the section
    //
    _oGlobalSections.push_back( pSection );
}

_DWFTK_API
DWFInterface*
DWFPackageWriter::addInterface( DWFInterface* pInterface )
throw( DWFException )
{
    if (pInterface == NULL)
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, L"Interface must be specified" );
    }

    //
    //
    //
    _ensureManifest();

    //
    // add to the manifest
    //
    return _pPackageManifest->provideInterface( pInterface );
}

_DWFTK_API
DWFPropertyContainer&
DWFPackageWriter::getManifestProperties()
throw( DWFException )
{
    //
    //
    //
    _ensureManifest();

    return dynamic_cast<DWFPropertyContainer&>(*_pPackageManifest);
}
    
_DWFTK_API
void
DWFPackageWriter::write( const DWFString&                   zSourceProductVendor,
                         const DWFString&                   zSourceProductName,
                         const DWFString&                   zSourceProductVersion,
                         const DWFString&                   zDWFProductVendor,
                         const DWFString&                   zDWFProductVersion,
                         DWFZipFileDescriptor::teFileMode   eCompressionMode )
throw( DWFException )
{
        //
        // if no version extension has been provided hook in the appropriate
        // one based on the section type count
        //
    if (_pVersionExtension == NULL)
    {
            //
            // if no eModel sections have been added, use the 6.0 extension
            //
        if (_nEModelSectionCount == 0)
        {
            _pVersionExtension = DWFCORE_ALLOC_OBJECT( DWFPackageVersion60Extension() );
        }

            //
            // if only one eModel section has been added, use the TypeInfo extension
            //
        else if (_nEPlotSectionCount == 0 && _nEModelSectionCount == 1)
        {    
            _pVersionExtension = DWFCORE_ALLOC_OBJECT( DWFPackageVersionTypeInfoExtension(DWFPackageVersionTypeInfoExtension::kzDocumentType_EModel) );
        }
    }
        //
        // always route through the version extension if it exists
        // and return immediately if instructed (ext. will return false)
        //
    if (_pVersionExtension && 
       (_pVersionExtension->write(zSourceProductVendor,
                                  zSourceProductName,
                                  zSourceProductVersion,
                                  zDWFProductVendor,
                                  zDWFProductVersion,
                                  eCompressionMode) == false))
    {
        return;
    }

    if (_pPackageManifest == NULL)
    {
        _DWFCORE_THROW( DWFNullPointerException, L"Cannot write package - no manifest exists" );
    }

    if (_pXMLSerializer == NULL)
    {
        _pXMLSerializer = DWFCORE_ALLOC_OBJECT( DWFXMLSerializer(_oUUID) );
        if (_pXMLSerializer == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate serializer" );
        }
    }

        //
        // open the package
        //
    if (_pPackageDescriptor == NULL)
    {
        if (_pVersionExtension)
        {
            _pPackageDescriptor = DWFCORE_ALLOC_OBJECT( DWFPackageFileDescriptor(_rDWFPackage, eCompressionMode, _pVersionExtension->major(), _pVersionExtension->minor()) );
        }
        else
        {
            _pPackageDescriptor = DWFCORE_ALLOC_OBJECT( DWFPackageFileDescriptor(_rDWFPackage, eCompressionMode) );
        }

        if (_pPackageDescriptor == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate file descriptor" );
        }

        _pPackageDescriptor->open();
    }

        //
        // if no version extension has been hooked in then write out a typeinfo file
        // 
    if (_pVersionExtension == NULL)
    {
            //
            // if no ePlot sections have been added write out eModel typeinfo
            // else write out the ePlot typeinfo
            //
        DWFString zTypeInfo;
        if (_nEPlotSectionCount == 0)
        {
            zTypeInfo = DWFInterface::kzEModel_ID;
        }
        else
        {
            if (_nEModelSectionCount != 0)
            {
                zTypeInfo = DWFInterface::kzEPlot_ID;
            }
        }

        if (zTypeInfo.chars())
        {
            zTypeInfo.append( L".TYPEINFO" );
        
            //
            // create the file
            //
            DWFOutputStream* pFilestream = _pPackageDescriptor->zip( zTypeInfo, _zDWFPassword );

            //
            // this file is empty, so just delete the stream (and close the file)
            //
            DWFCORE_FREE_OBJECT( pFilestream );
        }
    }

        //
        // give extension chance to write a file before the manifest
        //
    if (_pVersionExtension)
    {
       _pVersionExtension->prewriteManifest( *this, *_pPackageDescriptor, *_pXMLSerializer, _zDWFPassword );
    }
    
    //
    // create the MANIFEST
    //
    DWFOutputStream* pFilestream = _pPackageDescriptor->zip( L"manifest.xml", _zDWFPassword );

    //
    // just before writing out the manifest,
    // add any document-wide properties to it
    //
    DWFProperty* pStandardProperty = NULL;

    if (zSourceProductVendor.bytes() > 0)
    {
        pStandardProperty = DWFCORE_ALLOC_OBJECT( DWFProperty(L"SourceProductVendor", zSourceProductVendor) );
        if (pStandardProperty == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate property" );
        }

        _pPackageManifest->addProperty( pStandardProperty, true );
    }

    if (zSourceProductName.bytes() > 0)
    {
        pStandardProperty = DWFCORE_ALLOC_OBJECT( DWFProperty(L"SourceProductName", zSourceProductName) );
        if (pStandardProperty == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate property" );
        }

        _pPackageManifest->addProperty( pStandardProperty, true );
    }

    if (zSourceProductVersion.bytes() > 0)
    {
        pStandardProperty = DWFCORE_ALLOC_OBJECT( DWFProperty(L"SourceProductVersion", zSourceProductVersion) );
        if (pStandardProperty == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate property" );
        }

        _pPackageManifest->addProperty( pStandardProperty, true );
    }

    if (zDWFProductVendor.bytes() > 0)
    {
        pStandardProperty = DWFCORE_ALLOC_OBJECT( DWFProperty(L"DWFProductVendor", zDWFProductVendor) );
        if (pStandardProperty == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate property" );
        }

        _pPackageManifest->addProperty( pStandardProperty, true );
    }

    if (zDWFProductVersion.bytes() > 0)
    {
        pStandardProperty = DWFCORE_ALLOC_OBJECT( DWFProperty(L"DWFProductVersion", zDWFProductVersion) );
        if (pStandardProperty == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate property" );
        }
        _pPackageManifest->addProperty( pStandardProperty, true );
    }

    pStandardProperty = DWFCORE_ALLOC_OBJECT( DWFProperty(L"DWFToolkitVersion", _DWFTK_VERSION_STRING) );
    if (pStandardProperty == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate property" );
    }
    _pPackageManifest->addProperty( pStandardProperty, true );

    //
    // bind the stream to the writer, this will directly
    // populate the archive as the manifest serializes itself
    //
    _pXMLSerializer->attach( *pFilestream );

    //
    // tell the manifest to serialize himself as an XML document
    //
    _pPackageManifest->serializeXML( *_pXMLSerializer, eManifest );

    //
    // unbind the stream and finalize the file in the archive
    //
    _pXMLSerializer->detach();

    //
    // release the file stream
    //
    DWFCORE_FREE_OBJECT( pFilestream );

        //
        // give extension chance to write a file after the manifest
        //
    if (_pVersionExtension)
    {
       _pVersionExtension->postwriteManifest( *this, *_pPackageDescriptor, *_pXMLSerializer, _zDWFPassword );
    }

    //
    // Add any global sections first
    //
    DWFString zSection;
    DWFSection* pSection = NULL;
    DWFGlobalSection::tList::const_iterator iGlobalSection;

    for (iGlobalSection = _oGlobalSections.begin();
         iGlobalSection!= _oGlobalSections.end();
         iGlobalSection++)
    {
        pSection = *iGlobalSection;

        //
        // name the new descriptor
        //
        zSection.assign( pSection->name() );
        zSection.append( L"\\descriptor.xml" );

        //
        // start with the descriptor
        //
        pFilestream = _pPackageDescriptor->zip( zSection, _zDWFPassword );

        //
        // bind the stream to the writer, this will directly
        // populate the archive as the page descriptor serializes itself
        //
        _pXMLSerializer->attach( *pFilestream );

        //
        // tell the page to serialize itself into the descriptor XML document
        //
        pSection->serializeXML( *_pXMLSerializer, eDescriptor );

        //
        // unbind the stream and finalize the file in the archive
        //
        _pXMLSerializer->detach();

        //
        // release the file stream
        //
        DWFCORE_FREE_OBJECT( pFilestream );

        //
        // resources
        //
        DWFResource* pResource = NULL;
        DWFKVIterator<const wchar_t*, DWFResource*>* piAllResources = pSection->getResourcesByRole();

        if (piAllResources)
        {
            for (; piAllResources->valid(); piAllResources->next())
            {
                pResource = piAllResources->value();

                if (pResource->role() != DWFXML::kzRole_Descriptor)
                {
                    _serializeResource( pSection->name(), *pResource );
                }
            }

            DWFCORE_FREE_OBJECT( piAllResources );
        }
    }

    //
    // a custom section can still act like a known dwf section
    //
    DWFCustomSection* pCustomSection = NULL;
    DWFCustomSection::tBehavior tSectionBehavior = {0};

        //
        // Add the non-global sections
        //
    DWFSection::tList::const_iterator iSection;
    for (iSection = _oSections.begin();
         iSection!= _oSections.end();
         iSection++)
    {
        pSection = *iSection;

            //
            // a custom section can still act like a known dwf section
            //
        pCustomSection = dynamic_cast<DWFCustomSection*>(pSection);
        if (pCustomSection)
        {
            tSectionBehavior = pCustomSection->behavior();
        }

            //
            // manage section descriptor
            //
        if ((pCustomSection == NULL) || tSectionBehavior.bPublishDescriptor)
        {
            //
            // name the new descriptor
            //
            zSection.assign( pSection->name() );
            zSection.append( L"\\descriptor.xml" );

            //
            // start with the descriptor
            //
            pFilestream = _pPackageDescriptor->zip( zSection, _zDWFPassword );

            //
            // bind the stream to the writer, this will directly
            // populate the archive as the page descriptor serializes itself
            //
            _pXMLSerializer->attach( *pFilestream );

            //
            // tell the page to serialize itself into the descriptor XML document
            //
            pSection->serializeXML( *_pXMLSerializer, eDescriptor );

            //
            // unbind the stream and finalize the file in the archive
            //
            _pXMLSerializer->detach();

            //
            // release the file stream
            //
            DWFCORE_FREE_OBJECT( pFilestream );
        }

        //
        // iterator for finders
        //
        DWFIterator<DWFResource*>* piResources = NULL;

            //
            // thumbnails
            //
        piResources = pSection->findResourcesByRole( DWFXML::kzRole_Thumbnail );
        if (piResources)
        {
            for (; piResources->valid(); piResources->next())
            {
                _serializeResource( pSection->name(), *(piResources->get()) );
            }

            //
            // clean up
            //
            DWFCORE_FREE_OBJECT( piResources );
            piResources = NULL;
        }

            //
            // fonts
            //
        piResources = pSection->findResourcesByRole( DWFXML::kzRole_Font );
        if (piResources)
        {
            for (; piResources->valid(); piResources->next())
            {
                _serializeResource( pSection->name(), *(piResources->get()) );
            }

            //
            // clean up
            //
            DWFCORE_FREE_OBJECT( piResources );
            piResources = NULL;
        }

            //
            // w2d
            //
        piResources = pSection->findResourcesByRole( DWFXML::kzRole_Graphics2d );
        if (piResources)
        {
            for (; piResources->valid(); piResources->next())
            {
                _serializeResource( pSection->name(), *(piResources->get()) );
            }

            //
            // clean up
            //
            DWFCORE_FREE_OBJECT( piResources );
            piResources = NULL;
        }

            //
            // w3d
            //
        piResources = pSection->findResourcesByRole( DWFXML::kzRole_Graphics3d );
        if (piResources)
        {
            for (; piResources->valid(); piResources->next())
            {
                _serializeResource( pSection->name(), *(piResources->get()) );
            }

            //
            // clean up
            //
            DWFCORE_FREE_OBJECT( piResources );
            piResources = NULL;
        }

            //
            // textures
            //
        piResources = pSection->findResourcesByRole( DWFXML::kzRole_Texture );
        if (piResources)
        {
            for (; piResources->valid(); piResources->next())
            {
                _serializeResource( pSection->name(), *(piResources->get()) );
            }

            //
            // clean up
            //
            DWFCORE_FREE_OBJECT( piResources );
            piResources = NULL;
        }

        //
        // remaining resources
        //
        DWFResource* pResource = NULL;
        DWFKVIterator<const wchar_t*, DWFResource*>* piAllResources = pSection->getResourcesByRole();

        if (piAllResources)
        {
            for (; piAllResources->valid(); piAllResources->next())
            {
                pResource = piAllResources->value();

                    //
                    // if someone can think of a faster, cleaner way of doing this, please fix it
                    //
                if ((pResource->role() != DWFXML::kzRole_Descriptor)    &&
                    (pResource->role() != DWFXML::kzRole_Thumbnail)     &&
                    (pResource->role() != DWFXML::kzRole_Font)          &&
                    (pResource->role() != DWFXML::kzRole_Graphics2d)    &&
                    (pResource->role() != DWFXML::kzRole_Graphics3d)    &&
                    (pResource->role() != DWFXML::kzRole_Texture))
                {
                    _serializeResource( pSection->name(), *pResource );
                }
            }

            DWFCORE_FREE_OBJECT( piAllResources );
        }
    }

        //
        // give extension chance to write a file after the sections
        //
    if (_pVersionExtension)
    {
       _pVersionExtension->postwriteSections( *this, *_pPackageDescriptor, *_pXMLSerializer, _zDWFPassword );
    }

    //
    // all done
    //
    _pPackageDescriptor->close();
}

void
DWFPackageWriter::_serializeResource( const DWFString&  zSection,
                                      DWFResource&      rResource )
throw( DWFException )
{
    if (_pPackageDescriptor == NULL)
    {
        _DWFCORE_THROW( DWFNullPointerException, L"No package descriptor exists" );
    }

    size_t  nBytesRead = 0;
    char    aBuffer[_DWFTK_PACKAGE_WRITER_RESOURCE_STREAM_BUFFER_BYTES];

    //
    // open the resource file in the archive
    //
    DWFString zHREF = rResource.publishedIdentity();

        //
        //
        //
    if (zHREF.bytes() == 0)
    {
        zHREF.assign( zSection );
        zHREF.append( L"\\" );
        zHREF.append( rResource.href() );
    }

    DWFPointer<DWFOutputStream> apFilestream( _pPackageDescriptor->zip(zHREF, _zDWFPassword), false );

    //
    // open the stream to acquire the resource bytes
    //
    DWFPointer<DWFInputStream>  apResourceStream( rResource.getInputStream(), false );

        //
        // sanity check
        //
    if (apFilestream.isNull() || apResourceStream.isNull())
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Error acquiring resource streams" );
    }

        //
        // stream the bytes from the resource into the archive
        //
    while (apResourceStream->available() > 0)
    {
        nBytesRead = apResourceStream->read( aBuffer, _DWFTK_PACKAGE_WRITER_RESOURCE_STREAM_BUFFER_BYTES );
        apFilestream->write( aBuffer, nBytesRead );
    }

    //
    // release the file stream
    //
    apFilestream->flush();
}

void
DWFPackageWriter::_ensureManifest()
throw( DWFException )
{
        //
        // create the manifest if necessary
        //
    if (_pPackageManifest == NULL)
    {
        //
        // when the manifest is created it is assigned a new UUID for it's object ID
        // all subsequent object IDs should be acquired via the next() method of the UUID generator
        //
        _pPackageManifest = DWFCORE_ALLOC_OBJECT( DWFManifest(_oUUID.uuid(true)) );
        if (_pPackageManifest == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to create manifest" );
        }
    }
}

//DNT_End

#endif
