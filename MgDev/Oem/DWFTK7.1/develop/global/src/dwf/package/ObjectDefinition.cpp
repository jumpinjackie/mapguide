//
//  Copyright (C) 2003-2008 by Autodesk, Inc.
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



#include "dwf/package/ObjectDefinition.h"
using namespace DWFToolkit;


_DWFTK_API
DWFObjectDefinition::DWFObjectDefinition( DWFPackageReader* pPackageReader )
throw()
                   : DWFObjectDefinitionReader( pPackageReader )
                   , _nVersion(0)
{
    ;
}

_DWFTK_API
DWFObjectDefinition::~DWFObjectDefinition()
throw()
{
        //
        // clean properties
        //
    DWFProperty::tList* pPropertyList = NULL;
    DWFProperty::tList::iterator iProperty;

    tPropertySetMap::Iterator* piSet = _oPropertySets.iterator();
    if (piSet)
    {
        for (; piSet->valid(); piSet->next())
        {
            pPropertyList = piSet->value()->pList;
            for (iProperty = pPropertyList->begin();
                 iProperty!= pPropertyList->end();
                 iProperty++)
            {
                DWFCORE_FREE_OBJECT( *iProperty );
            }

            DWFCORE_FREE_OBJECT( pPropertyList );
            DWFCORE_FREE_OBJECT( piSet->value() );
        }

        DWFCORE_FREE_OBJECT( piSet );
    }

        //
        // clean objects
        //
    DWFDefinedObject::tMap::Iterator* piObj = _oObjects.iterator();
    if (piObj)
    {
        for (; piObj->valid(); piObj->next())
        {
            DWFCORE_FREE_OBJECT( piObj->value() );
        }

        DWFCORE_FREE_OBJECT( piObj );
    }

        //
        // clean instances
        //
    DWFDefinedObjectInstance::tMap::Iterator* piInst = _oInstances.iterator();
    if (piInst)
    {
        for (; piInst->valid(); piInst->next())
        {
            DWFCORE_FREE_OBJECT( piInst->value() );
        }

        DWFCORE_FREE_OBJECT( piInst );
    }
}

_DWFTK_API
double
DWFObjectDefinition::provideVersion( double nDecimalVersion )
throw()
{
    _nVersion = nDecimalVersion;

    return nDecimalVersion;
}

_DWFTK_API
void
DWFObjectDefinition::provideProperties( const DWFString&    zID,
                                        vector<DWFString>*  pPropertyRefs,
                                        DWFProperty::tList* pPropertyList )
throw()
{
        //
        // sanity check
        //
    if (pPropertyList == NULL)
    {
        if (pPropertyRefs) DWFCORE_FREE_OBJECT( pPropertyRefs );
        return;
    }

    tPropertySet* pCurrentSet = NULL;

    //
    // see if the set already exists
    // (this happens if a referring set was processed first)
    //
    tPropertySet** ppSet = _oPropertySets.find( zID );

    if (ppSet)
    {
        pCurrentSet = *ppSet;
    }
        //
        // nope, allocate a new set
        //
    else
    {
        pCurrentSet = DWFCORE_ALLOC_OBJECT( tPropertySet );
    }

        //
        // fail silently... yuck
        // but release incoming pointers on the way out
        //
    if (pCurrentSet == NULL)
    {
        DWFCORE_FREE_OBJECT( pPropertyList );
        if (pPropertyRefs) DWFCORE_FREE_OBJECT( pPropertyRefs );
        return;
    }

    //
    // capture the properties
    //
    pCurrentSet->pList = pPropertyList;

        //
        // process set references
        //
    if (pPropertyRefs)
    {
        tPropertySet* pRefSet = NULL;

        vector<DWFString>::iterator iRef = pPropertyRefs->begin();
        for (; iRef != pPropertyRefs->end(); iRef++)
        {
            //
            // resolve existing reference
            //
            ppSet = _oPropertySets.find( *iRef );

                //
                // referenced set exists
                //
            if (ppSet)
            {
                //
                // capture the pointer directly (no look ups later)
                //
                pRefSet = *ppSet;
            }
                //
                // create placeholder
                //
            else
            {
                pRefSet = DWFCORE_ALLOC_OBJECT( tPropertySet );

                    //
                    // add into the main map for later resolution
                    //
                if (pRefSet)
                {
                    _oPropertySets.insert( *iRef, pRefSet );
                }
            }

                //
                // add set pointer reference
                //
            if (pRefSet)
            {
                pCurrentSet->oRefs.push_back( pRefSet );
            }
        }

        //
        // clean up
        //
        DWFCORE_FREE_OBJECT( pPropertyRefs );
    }

    //
    // finally, add the current (new) set into the map
    //
    _oPropertySets.insert( zID, pCurrentSet );
}

_DWFTK_API
DWFDefinedObject*
DWFObjectDefinition::provideObject( DWFDefinedObject* pObject )
throw()
{
    if (pObject)
    {
        _oObjects.insert( pObject->id(), pObject );
    }

    return pObject;
}

_DWFTK_API
DWFDefinedObjectInstance*
DWFObjectDefinition::provideInstance( DWFDefinedObjectInstance* pInstance )
throw()
{
    if (pInstance)
    {
        _oInstances.insert( pInstance->id(), pInstance );
    }

    return pInstance;
}

_DWFTK_API
void
DWFObjectDefinition::getRootInstances( DWFDefinedObjectInstance::tList& rRootInstances )
throw( DWFException )
{
    DWFDefinedObject**          ppObject = NULL;
    DWFDefinedObjectInstance*   pInstance = NULL;
    DWFDefinedObjectInstance**  ppChildInstance = NULL;

    DWFDefinedObjectInstance::tMap::Iterator* piInstance = _oInstances.iterator();
    for (; piInstance->valid(); piInstance->next())
    {
        pInstance = piInstance->value();

            //
            // resolve the object
            //
        ppObject = _oObjects.find( pInstance->object() );
        if (ppObject)
        {
            pInstance->resolveObject( *ppObject );
        }

        //
        // resolve children
        //
        DWFIterator<DWFString>* piChildren = pInstance->children();

        if (piChildren)
        {
            for (;piChildren->valid(); piChildren->next())
            {
                ppChildInstance = _oInstances.find( piChildren->get() );
                if (ppChildInstance)
                {
                    (*ppChildInstance)->resolveParent( pInstance );
                    pInstance->resolveChild( *ppChildInstance );
                }
            }
            DWFCORE_FREE_OBJECT( piChildren );
        }
    }

    for (piInstance->reset(); piInstance->valid(); piInstance->next())
    {
        pInstance = piInstance->value();

        if (pInstance->resolvedParent() == NULL)
        {
            rRootInstances.push_back( pInstance );
        }
    }

    DWFCORE_FREE_OBJECT( piInstance );
}

_DWFTK_API
DWFPropertyContainer*
DWFObjectDefinition::getInstanceProperties( const DWFString& zInstanceID )
throw( DWFException )
{
    DWFDefinedObjectInstance** ppInstance = _oInstances.find( zInstanceID );
    if (ppInstance == NULL)
    {
        _DWFCORE_THROW( DWFDoesNotExistException, L"Unknown instance ID" );
    }

    return getInstanceProperties( **ppInstance );
}

_DWFTK_API
DWFPropertyContainer*
DWFObjectDefinition::getInstanceProperties( DWFDefinedObjectInstance& rInstance )
throw( DWFException )
{
    DWFPropertyContainer* pContainer = DWFCORE_ALLOC_OBJECT( DWFPropertyContainer );
    if (pContainer == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate container" );
    }

        //
        // start the with associated object
        //
    tPropertySet** ppPropSet = NULL;
    DWFDefinedObject* pObject = rInstance.resolvedObject();
    if (pObject)
    {
        DWFIterator<DWFString>* piPropRefs = pObject->getPropertyRefs();
        if (piPropRefs)
        {
                //
                // insert any set references first
                //
            for (;piPropRefs->valid(); piPropRefs->next())
            {
                //
                // look up this reference in our map
                //
                ppPropSet = _oPropertySets.find( piPropRefs->get() );
        
                    //
                    // it really should be here...
                    //
                if (ppPropSet)
                {
                    //
                    // this will resolve all other set references
                    //
                    _resolvePropertyRefs( *ppPropSet, pContainer );
                }
            }
            DWFCORE_FREE_OBJECT( piPropRefs );
        }
            //
            // now process the object properties
            //
        DWFProperty::tMap::Iterator* piProp = pObject->getProperties();
        for (; piProp->valid(); piProp->next())
        {
            //
            // false -> do not transfer ownership of the property
            //
            pContainer->addProperty( piProp->value(), false );
        }
        DWFCORE_FREE_OBJECT( piProp );
    }

    //
    // now resolve any references on the instance
    //
    DWFIterator<DWFString>* piPropRefs = rInstance.getPropertyRefs();

    if (piPropRefs)
    {
            //
            // insert any set references
            //
        for (;piPropRefs->valid(); piPropRefs->next())
        {
            //
            // look up this reference in our map
            //
            ppPropSet = _oPropertySets.find( piPropRefs->get() );
        
                //
                // it really should be here...
                //
            if (ppPropSet)
            {
                //
                // this will resolve all other set references
                //
                _resolvePropertyRefs( *ppPropSet, pContainer );
            }
        }
        DWFCORE_FREE_OBJECT( piPropRefs );
    }

        //
        // finally add the instance level properties
        // these always go last to override any object level props
        //
    DWFProperty::tMap::Iterator* piProp = rInstance.getProperties();
    for (; piProp->valid(); piProp->next())
    {
        //
        // false -> do not transfer ownership of the property
        //
        pContainer->addProperty( piProp->value(), false );
    }
    DWFCORE_FREE_OBJECT( piProp );

    return pContainer;
}

void
DWFObjectDefinition::_resolvePropertyRefs( tPropertySet*         pSet,
                                           DWFPropertyContainer* pContainer )
throw()
{
    vector<struct _tPropertySet*>::const_iterator iRefSet = pSet->oRefs.begin();

    for (; iRefSet!= pSet->oRefs.end(); iRefSet++)
    {
        //
        // recurse through other set references
        //
        _resolvePropertyRefs( *iRefSet, pContainer );
    }

        //
        // add the current set properties
        //
    DWFProperty::tList::const_iterator iProp = pSet->pList->begin();
    for (; iProp != pSet->pList->end(); iProp++)
    {
        //
        // false -> do not transfer ownership
        //
        pContainer->addProperty( *iProp, false );
    }
}
