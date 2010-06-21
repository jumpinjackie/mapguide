//
//  Copyright (C) 2004-2010 by Autodesk, Inc.  All rights reserved.
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


#include "dwf/package/Constants.h"
#include "dwf/package/writer/DWFXMLSerializer.h"
#include "dwf/package/utility/PropertyContainer.h"
using namespace DWFToolkit;



_DWFTK_API
DWFPropertyContainer::DWFPropertyContainer( const DWFString& zID )
throw()
                    : _zID( zID )
{
#ifndef DWFTK_READ_ONLY

    _oSerializer.is( this );

#endif
}

_DWFTK_API
DWFPropertyContainer::~DWFPropertyContainer()
throw()
{
    DWFProperty::tMap::Iterator* piProperty = _oProperties.iterator();

    if (piProperty)
    {
        for(; piProperty->valid(); piProperty->next())
        {
            if (piProperty->value()->owner() == this)
            {
                DWFCORE_FREE_OBJECT( piProperty->value() );
            }
        }
        DWFCORE_FREE_OBJECT( piProperty );
    }

    DWFPropertyContainer::tList::iterator iContainer = _oContainers.begin();
    for(; iContainer != _oContainers.end(); iContainer++)
    {
        DWFCORE_FREE_OBJECT( *iContainer );
    }
}

_DWFTK_API
void
DWFPropertyContainer::addProperty( DWFProperty* pProperty,
                                   bool         bOwnProperty )
throw( DWFException )
{
    if (pProperty == NULL)
    {
        _DWFCORE_THROW( DWFNullPointerException, L"Property was null" );
    }

        //
        // make a copy
        //
    if (bOwnProperty == false)
    {
        DWFProperty* pCopy = DWFCORE_ALLOC_OBJECT( DWFProperty(*pProperty) );

        if (pCopy == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new property" );
        }

        pProperty = pCopy;
    }

    //
    // claim ownership
    //
    pProperty->own( *this );

        //
        // store, erase duplicates
        //
    if (false == _oProperties.insert(pProperty->category(), pProperty->name(), pProperty, false))
    {
        //
        // It's not enough to just call erase.  That just frees the node in the list; it doesn't touch
        // the original value.  We need to free the original value by finding it, then storing off the 
        // contained pointer (becasue the erase will nuke the contents of ppExisting, erase the 
        // node and free the old value.
        //
        DWFProperty** ppExisting = _oProperties.find( pProperty->category(), pProperty->name() );
        DWFProperty* pExisting = (ppExisting) ? *ppExisting : NULL;
        if (true == _oProperties.erase(pProperty->category(), pProperty->name()))
        {
            if (pExisting != NULL)
            {
                DWFCORE_FREE_OBJECT( pExisting );
            }

            _oProperties.insert( pProperty->category(), pProperty->name(), pProperty );
        }
        else
        {
            //
            // This is the failsafe case.  We've failed to erase the duplicate, and thus cannot put the
            // property into the collection, but we might as well not leak the object.
            //
            DWFCORE_FREE_OBJECT( pProperty ); 
        }
    }
}

_DWFTK_API
void
DWFPropertyContainer::addPropertyContainer( DWFPropertyContainer* pContainer )
throw( DWFException )
{
    if (pContainer == NULL)
    {
        _DWFCORE_THROW( DWFNullPointerException, L"Property container was null" );
    }

    _oContainers.push_back( pContainer );
}
    
_DWFTK_API
DWFProperty::tMap::Iterator*
DWFPropertyContainer::getProperties( const DWFString& zCategory )
throw()
{
    if (zCategory.bytes() == 0)
    {
        return _oProperties.iterator();
    }
    else
    {
        return _oProperties.iterator( zCategory );
    }
}

const DWFProperty* const
DWFPropertyContainer::findProperty( const DWFString& zName,
                                    const DWFString& zCategory )
throw()
{
    DWFProperty** ppProp = _oProperties.find( zCategory, zName );
    return (ppProp ? *ppProp : NULL);
}

_DWFTK_API
void
DWFPropertyContainer::referencePropertyContainer( const DWFPropertyContainer& rContainer )
throw( DWFException )
{
    _oReferences.push_back( const_cast<DWFPropertyContainer*>(&rContainer) );
}

_DWFTK_API
void
DWFPropertyContainer::getOwnedPropertyContainers( DWFPropertyContainer::tList& rContainerList )
throw( DWFException )
{
    DWFPropertyContainer::tList::iterator iContainer = _oContainers.begin();
    for (; iContainer != _oContainers.end(); iContainer++)
    {
        rContainerList.push_back( *iContainer );
    }
}

_DWFTK_API
void
DWFPropertyContainer::getReferencedPropertyContainers( DWFPropertyContainer::tList& rContainerList )
throw( DWFException )
{
    DWFPropertyContainer::tList::iterator iReference = _oReferences.begin();
    for (; iReference != _oReferences.end(); iReference++)
    {
        rContainerList.push_back( *iReference );
    }
}

_DWFTK_API
void
DWFPropertyContainer::getAllPropertyContainers( DWFPropertyContainer::tList& rContainerList )
throw( DWFException )
{
    getOwnedPropertyContainers( rContainerList );
    getReferencedPropertyContainers( rContainerList );
}

_DWFTK_API
void
DWFPropertyContainer::removeOwnedPropertyContainers( DWFPropertyContainer::tList& rContainerList,
                                                     bool                         bMakeReferences )
throw( DWFException )
{
    DWFPropertyContainer::tList::iterator iContainer = _oContainers.begin();
    for (; iContainer != _oContainers.end(); iContainer++)
    {
        rContainerList.push_back( *iContainer );

        if (bMakeReferences)
        {
            _oReferences.push_back( *iContainer );
        }
    }

    _oContainers.clear();
}

_DWFTK_API
void
DWFPropertyContainer::removeReferencedPropertyContainers( DWFPropertyContainer::tList& rContainerList )
throw( DWFException )
{
    getReferencedPropertyContainers( rContainerList );
    _oReferences.clear();
}

_DWFTK_API
void
DWFPropertyContainer::removeAllPropertyContainers( DWFPropertyContainer::tList& rContainerList )
throw( DWFException )
{
    removeOwnedPropertyContainers( rContainerList );
    removeReferencedPropertyContainers( rContainerList );
}

_DWFTK_API
void
DWFPropertyContainer::copyProperties( DWFPropertyContainer& rContainer,
                                      bool                  bRemoveOwnership,
                                      bool                  bMakeReferences )
throw( DWFException )
{
        //
        // copy properties
        //
    DWFProperty::tMap::Iterator* piProperties = rContainer.getProperties();
    if (piProperties)
    {
        for(; piProperties->valid(); piProperties->next())
        {
            addProperty( piProperties->value(), false );
        }

        DWFCORE_FREE_OBJECT( piProperties );
    }

    //
    // copy containers
    //
    DWFPropertyContainer::tList             oContainers;
    DWFPropertyContainer::tList::iterator   iContainer;

        //
        // owned containers
        //
    if (bRemoveOwnership)
    {
        rContainer.removeOwnedPropertyContainers( oContainers, bMakeReferences );

            //
            // "own" these pointers
            //
        iContainer = oContainers.begin();
        for (; iContainer != oContainers.end(); iContainer++)
        {
            _oContainers.push_back( *iContainer );
        }

        //
        // empty the list
        //
        oContainers.clear();
    }
    else
    {
        rContainer.getOwnedPropertyContainers( oContainers );
    }

    //
    // referenced containers
    //
    rContainer.getReferencedPropertyContainers( oContainers );

    iContainer = oContainers.begin();
    for (; iContainer != oContainers.end(); iContainer++)
    {
        _oReferences.push_back( *iContainer );
    }
}

_DWFTK_API
void
DWFPropertyContainer::notifyOwnerChanged( DWFOwnable& /*rOwnable*/ )
throw( DWFException )
{
    ;
}

_DWFTK_API
void
DWFPropertyContainer::notifyOwnableDeletion( DWFOwnable& rOwnable )
throw( DWFException )
{
    DWFProperty* pProperty = dynamic_cast<DWFProperty*>(&rOwnable);

    if (pProperty)
    {
        _oProperties.erase( pProperty->category(), pProperty->name() );
    }
}

#ifndef DWFTK_READ_ONLY

void
DWFPropertyContainer::_Serializer::serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
throw( DWFException )
{
    DWFPropertyContainer::tList::iterator iContainer = _pContainer->_oContainers.begin();

    for (; iContainer != _pContainer->_oContainers.end(); iContainer++)
    {
        (((DWFPropertyContainer*)*iContainer)->getSerializable()).serializeXML( rSerializer, nFlags );
    }

    DWFProperty::tMap::Iterator* piProperty = _pContainer->_oProperties.iterator();
    DWFPropertyContainer::tList::iterator iReference = _pContainer->_oReferences.begin();

    if ((piProperty && piProperty->valid()) ||
        (iReference != _pContainer->_oReferences.end()))
    {
        DWFString zNamespace;

            //
            // namespace dictated by document and section type 
            //
        if (nFlags & DWFPackageWriter::eDescriptor)
        {
            zNamespace.assign( namespaceXML(nFlags) );
        }

        rSerializer.startElement( DWFXML::kzElement_Properties, zNamespace );
        {
            if (_pContainer->_zID.bytes() == 0)
            {
                _pContainer->_zID.assign( rSerializer.nextUUID(true) );
            }

            rSerializer.addAttribute( DWFXML::kzAttribute_ID, _pContainer->_zID );

            DWFString zReferences;
            DWFPropertyContainer* pReference = NULL;
            for (; iReference != _pContainer->_oReferences.end(); iReference++)
            {
                pReference = *iReference;
                if (pReference->id().bytes() == 0)
                {
                    pReference->identify( rSerializer.nextUUID(true) );
                }

                zReferences.append( pReference->id() );
                zReferences.append( /*NOXLATE*/L" " );
            }

            if (zReferences.bytes() > 0)
            {
                rSerializer.addAttribute( DWFXML::kzAttribute_Refs, zReferences );
            }

            if (piProperty)
            {
                for (; piProperty->valid(); piProperty->next())
                {            
                    piProperty->value()->serializeXML( rSerializer, nFlags );
                }
            }
        }

        rSerializer.endElement();
    }

    if (piProperty)
    {
        DWFCORE_FREE_OBJECT( piProperty );
    }
}

#endif

