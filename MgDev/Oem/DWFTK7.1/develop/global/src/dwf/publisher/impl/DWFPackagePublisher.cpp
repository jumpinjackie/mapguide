//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESSED OR IMPLIED,
//  AS TO THE CORRECTNESS OF THIS CODE OR ANY DERIVATIVE
//  WORKS WHICH INCORPORATE IT.
//
//  AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS
//  AND EXPLICITLY DISCLAIMS ANY LIABILITY, INCLUDING
//  CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//



#ifndef DWFTK_READ_ONLY


#include "dwfcore/MIME.h"

#include "dwf/package/Constants.h"
#include "dwf/package/EPlotSection.h"
#include "dwf/package/EModelSection.h"
#include "dwf/package/writer/extensions/6.01/PackageVersionExtension.h"
#include "dwf/publisher/impl/DWFPackagePublisher.h"
using namespace DWFToolkit;

#if defined(DWFTK_STATIC) || !defined(_DWFCORE_WIN32_SYSTEM)

//DNT_Start
const wchar_t* const DWFPackagePublisher::kz_PropName_PolygonHandedness =   L"_PolygonHandedness";
const wchar_t* const DWFPackagePublisher::kz_PropName_UseDefaultLighting =  L"_UseDefaultLighting";
const wchar_t* const DWFPackagePublisher::kz_PropName_UseSilhouetteEdges =  L"_UseSilhouetteEdges";
const wchar_t* const DWFPackagePublisher::kz_PropName_EdgeColor =           L"_EdgeColor";

const wchar_t* const DWFPackagePublisher::kz_NamedView_Default =            L"default";
const wchar_t* const DWFPackagePublisher::kz_PropSetName_DefaultView =      L"_DefaultView";
const wchar_t* const DWFPackagePublisher::kz_PropName_CameraProjection =    L"_CameraProjection";
const wchar_t* const DWFPackagePublisher::kz_PropName_CameraPosition =      L"_CameraPosition";
const wchar_t* const DWFPackagePublisher::kz_PropName_CameraTarget =        L"_CameraTarget";
const wchar_t* const DWFPackagePublisher::kz_PropName_CameraUpVector =      L"_CameraUpVector";
const wchar_t* const DWFPackagePublisher::kz_PropName_CameraField =         L"_CameraField";

const wchar_t* const DWFPackagePublisher::kz_PropName_Published_Object =    L"_name";
const wchar_t* const DWFPackagePublisher::kz_PropCategory_Hidden =          L"hidden";
//DNT_End

#endif


//DNT_Start

_DWFTK_API
DWFPackagePublisher::DWFPackagePublisher( const DWFFile&   rFile,
                                          const DWFString& zPassword,
                                          const DWFString& zSourceProductVendor,
                                          const DWFString& zSourceProductName,
                                          const DWFString& zSourceProductVersion,
                                          const DWFString& zPublisherProductVendor,
                                          const DWFString& zPublisherProductVersion )
throw()
                   : DWFPublishedObject::Visitor( NULL )
                   , DWFPropertyVisitor( NULL )
                   , DWFEmbeddedFontVisitor( NULL )
                   , _nModels( 0 )
                   , _nNextInstanceID( 1 )     // MUST BE NON-ZERO
                   , _nPlots( 0 )
                   , _zSourceProductVendor( zSourceProductVendor )
                   , _zSourceProductName( zSourceProductName )
                   , _zSourceProductVersion( zSourceProductVersion )
                   , _zPublisherProductVendor( zPublisherProductVendor )
                   , _zPublisherProductVersion( zPublisherProductVersion )
                   , _oPackageWriter( rFile, zPassword )
                   , _pCurrentSection( NULL )
                   , _pCurrentObjectDefinition( NULL )
{
    ;
}

_DWFTK_API
DWFPackagePublisher::~DWFPackagePublisher()
throw()
{
    ;
}

_DWFTK_API
void
DWFPackagePublisher::publish()
throw( DWFException )
{
    _oPackageWriter.write( _zSourceProductVendor,
                           _zSourceProductName,
                           _zSourceProductVersion,
                           _zPublisherProductVendor,
                           _zPublisherProductVersion );
}

_DWFTK_API
void
DWFPackagePublisher::visitPublishedObject( DWFPublishedObject& rPublishedObject )
throw( DWFException )
{
        //
        // objects and instances are inserted directly into the section
        // there had better be one
        //
    if (_pCurrentSection == NULL)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"No current section" );
    }

    //
    // acquire the object defining this published object
    //
    DWFDefinedObject* pDefinedObject = _findDefinedObject( rPublishedObject );
    DWFDefinedObjectInstance* pDefinedInstance = NULL;

        //
        // this is the first time we have seen this published object
        //
    if (pDefinedObject == NULL)
    {
        //
        // create a definition of this published object
        //
        _definePublishedObject( rPublishedObject, &pDefinedObject, &pDefinedInstance );
    }

        //
        // we will need the instance of the defined object to
        // process any references, so create one if needed
        //
    if (pDefinedInstance == NULL)
    {
        wchar_t zNode[12];
        _DWFCORE_SWPRINTF( zNode, 12, L"%u", rPublishedObject.key() );
        
        pDefinedInstance = pDefinedObject->instance( zNode );
    }

    //
    // now we can process any references to this published object
    //
    DWFPublishedObject::tReference* pReference = NULL;
    DWFPublishedObject::tReferenceList& rReferences = rPublishedObject.references();
    DWFPublishedObject::tReferenceList::const_iterator iReference = rReferences.begin();

    for (; iReference != rReferences.end(); iReference++)
    {
        pReference = *iReference;

            //
            // sanity check
            //
        if (pReference == NULL)
        {
            _DWFCORE_THROW( DWFNullPointerException, L"Encounted NULL reference" );
        }

        //
        // acquire this published object's definition
        //
        DWFDefinedObject* pReferenceObject = _findDefinedObject( *(pReference->pObj) );

            //
            // this is the first time we have seen this published object
            //
        if (pReferenceObject == NULL)
        {
            //
            // create a definition of the published reference object
            //
            _definePublishedObject( *(pReference->pObj), &pReferenceObject, NULL );
        }

        //
        // the reference relationship is captured hierarchically
        //
        pDefinedObject->addChild( pReferenceObject, pReference->pObj->key() );

        //
        // NOTE: do not add object to the container here - it already exists
        //

        //
        // the published reference object will not have had a matching instance
        // defined for it.  we must handle that here using the index from the
        // reference structure rather than the referenced published object
        //
        // the index of the reference structure corresponds to the
        // graphics directly related to this object instance.
        // this is the instance node
        //
        wchar_t zNode[12] = {0};
        _DWFCORE_SWPRINTF( zNode, 12, L"%u", pReference->nIndex );

        //
        // now create an instance of the defined reference object
        //
        DWFDefinedObjectInstance* pReferenceInstance = pReferenceObject->instance( zNode );

            //
            // if a name was provided for the reference
            // we add it as a property
            //
        if (pReference->zName.bytes() > 0)
        {
            pReferenceInstance->addProperty( DWFCORE_ALLOC_OBJECT(DWFProperty(kz_PropName_Published_Object,
                                                                              pReference->zName,
                                                                              kz_PropCategory_Hidden)),
                                             true );
        }

        //
        // assign the instance ID for the object
        //
        pReference->pObj->setInstance( _nNextInstanceID++ );

        //
        // the reference instance becomes a child of the defined instance
        //
        pDefinedInstance->addChild( pReferenceInstance, pReference->nIndex );

        //
        // capture the instance
        //
        _pCurrentObjectDefinition->addInstance( pReferenceInstance, pReference->nIndex );
    }

        //
        // invoke base implementation for composites
        //
    DWFPublishedObject::Visitor* pBaseVisitor = DWFPublisher::getPublishedObjectVisitor();
    if (pBaseVisitor)
    {
        pBaseVisitor->visitPublishedObject( rPublishedObject );
    }
}

DWFDefinedObject*
DWFPackagePublisher::_findDefinedObject( DWFPublishedObject& rPublishedObject )
const
throw()
{
    //
    // the key of the published object is also the ID of the defined object
    //
    wchar_t zObjectID[12] = {0};
    _DWFCORE_SWPRINTF( zObjectID, 12, L"%u", rPublishedObject.key() );

    return _pCurrentObjectDefinition->findObject( zObjectID );
}

void
DWFPackagePublisher::_definePublishedObject( DWFPublishedObject&        rPublishedObject,
                                             DWFDefinedObject**         ppDefinedObject,
                                             DWFDefinedObjectInstance** ppDefinedInstance )
throw( DWFException )
{
    if (ppDefinedObject == NULL)
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, L"ppDefinedObject cannot be NULL" );
    }

    //
    // the key of the published object corresponds to ID of the defined object
    //
    wchar_t zObjectID[12] = {0};
    _DWFCORE_SWPRINTF( zObjectID, 12, L"%u", rPublishedObject.key() );

    //
    // allocate a new defined object
    //
    *ppDefinedObject = DWFCORE_ALLOC_OBJECT( DWFDefinedObject(zObjectID) );
    if (*ppDefinedObject == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new object" );
    }

    //
    // name the object with a hidden property
    // this will show up in the model navigator as browseable object
    //
    (*ppDefinedObject)->addProperty( DWFCORE_ALLOC_OBJECT(DWFProperty(kz_PropName_Published_Object,
                                                                      rPublishedObject.name(),
                                                                      kz_PropCategory_Hidden)),
                                     true );

    //
    // before adding the defined object to the container,
    // check for a published parent
    //
    DWFDefinedObject* pDefinedParent = NULL;
    DWFPublishedObject* pPublishedParent = rPublishedObject.parent();

        //
        // if the published object has a parent
        // does the published parent have a defined object?
        //
    if (pPublishedParent)
    {
        pDefinedParent = _findDefinedObject( *pPublishedParent );

            //
            // if there is no defined object for the published parent
            // then we need to realize one before proceeding
            //
        if (pDefinedParent == NULL)
        {
            //
            // create a definition of this published object
            //
            _definePublishedObject( *pPublishedParent, &pDefinedParent, NULL );
        }
    }

        //
        // if the parent exists, add this child
        //
    if (pDefinedParent)
    {
        pDefinedParent->addChild( *ppDefinedObject, rPublishedObject.key() );
    }

    //
    // capture the object
    //
    _pCurrentObjectDefinition->addObject( *ppDefinedObject, rPublishedObject.key() );

        //
        // if this published object is NOT referenced
        // we need to have a defined instance as well
        //
    if (rPublishedObject.isReferenced() == false)
    {
        //
        // the index of the published object corresponds to the
        // graphics directly related to this object instance.
        // this is the instance node
        //
        wchar_t zNode[12] = {0};
        _DWFCORE_SWPRINTF( zNode, 12, L"%u", rPublishedObject.index() );

        //
        // create an instance of this defined object
        //
        DWFDefinedObjectInstance* pDefinedInstance = (*ppDefinedObject)->instance( zNode );
        DWFDefinedObjectInstance* pParentInstance = NULL;

            //
            // we already know the state of this published object's parent
            // if the parent exists, and we are here, then it must also have
            // a defined instance at this node
            //
        if (pDefinedParent)
        {
            //
            // look up the parent instance by the index of the published parent
            //
            _DWFCORE_SWPRINTF( zNode, 12, L"%u", pPublishedParent->index() );

            pParentInstance = pDefinedParent->getInstance( zNode );

                //
                // this had better be there
                //
            if (pParentInstance == NULL)
            {
                _DWFCORE_THROW( DWFDoesNotExistException, L"Inconsistent structure - expected matching parent instance" );
            }
        }

        //
        // acquire the published properties
        //
        pDefinedInstance->copyProperties( rPublishedObject );

            //
            //
            //
        if (pParentInstance)
        {
            pParentInstance->addChild( pDefinedInstance, rPublishedObject.index() );
        }

        //
        // now we can add the instance to the container
        //
        _pCurrentObjectDefinition->addInstance( pDefinedInstance, rPublishedObject.index() );

            //
            // return this pointer if possible
            //
        if (ppDefinedInstance)
        {
            *ppDefinedInstance = pDefinedInstance;
        }

        //
        // assign the instance ID for the object
        //
        rPublishedObject.setInstance( _nNextInstanceID++ );
    }
        //
        // referenced objects get the properies
        //
    else
    {
        //
        // acquire the published properties
        //
        (*ppDefinedObject)->copyProperties( rPublishedObject );
    }
}

_DWFTK_API
void
DWFPackagePublisher::visitProperty( DWFProperty& rProperty )
throw( DWFException )
{
        //
        // invoke base implementation for composites
        //
    DWFPropertyVisitor* pBaseVisitor = DWFPublisher::getPropertyVisitor();
    if (pBaseVisitor)
    {
        pBaseVisitor->visitProperty( rProperty );
    }
}

_DWFTK_API
void
DWFPackagePublisher::visitPropertyContainer( DWFPropertyContainer& rPropertyContainer )
throw( DWFException )
{
    //
    // when we visited the published object, we made sure to copy the properties
    // and container references, what has not happened though is the actual
    // definition of those property sets. 
    //

    //
    // remove the owned containers from the object but be sure
    // to leave references to them
    //
    DWFPropertyContainer::tList oContainerList;
    rPropertyContainer.removeOwnedPropertyContainers( oContainerList, true );

    //
    // now add each container to the object properties of the current section
    //
    DWFPropertyContainer::tList::const_iterator iContainer = oContainerList.begin();
    for (; iContainer != oContainerList.end(); iContainer++)
    {
        _pCurrentObjectDefinition->getObjectProperties().addPropertyContainer( *iContainer );
    }

        //
        // invoke base implementation for composites
        //
    DWFPropertyVisitor* pBaseVisitor = DWFPublisher::getPropertyVisitor();
    if (pBaseVisitor)
    {
        pBaseVisitor->visitPropertyContainer( rPropertyContainer );
    }
}

_DWFTK_API
void
DWFPackagePublisher::visitEmbeddedFont( const DWFEmbeddedFont& rFont )
throw( DWFException )
{
    //
    // blindly cache for now
    // TODO: be smart about which sections use which fonts
    //
    _oEmbeddedFonts.push_back( const_cast<DWFEmbeddedFont*>(&rFont) );

        //
        // invoke base implementation for composites
        //
    DWFEmbeddedFontVisitor* pBaseVisitor = DWFPublisher::getEmbeddedFontVisitor();
    if (pBaseVisitor)
    {
        pBaseVisitor->visitEmbeddedFont( rFont );
    }
}

_DWFTK_API
void
DWFPackagePublisher::preprocess( DWFPublishable* pContent )
throw( DWFException )
{
        //
        // 3D
        //
    DWFModel* pW3DModel = dynamic_cast<DWFModel*>(pContent);
    if (pW3DModel)
    {
        _preprocessModel( pW3DModel );
    }
    else
    {
            //
            // 2D
            //
        DWFPlot* pW2DPlot = dynamic_cast<DWFPlot*>(pContent);
        if (pW2DPlot)
        {
            _preprocessPlot( pW2DPlot );
        }
    }
}

_DWFTK_API
void
DWFPackagePublisher::postprocess( DWFPublishable* pContent )
throw( DWFException )
{
        //
        // 3D
        //
    DWFModel* pW3DModel = dynamic_cast<DWFModel*>(pContent);
    if (pW3DModel)
    {
        _postprocessModel( pW3DModel );
    }
    else
    {
            //
            // 2D
            //
        DWFPlot* pW2DPlot = dynamic_cast<DWFPlot*>(pContent);
        if (pW2DPlot)
        {
            _postprocessPlot( pW2DPlot );
        }
    }

    //
    // reset
    //
    _pCurrentSection = NULL;
    _pCurrentObjectDefinition = NULL;
}

void
DWFPackagePublisher::_preprocessModel( DWFModel* pW3DModel )
throw( DWFException )
{
    //
    // create a new EModelSection from the DWFModel
    //
    DWFSource oModelSource( L"",
                            pW3DModel->getSource(),
                            pW3DModel->getSourceID() );

    DWFUnits oModelUnits( pW3DModel->getUnits() );

    DWFEModelSection* pModelSection = 
        DWFCORE_ALLOC_OBJECT( DWFEModelSection(pW3DModel->getTitle(), 
                                               L"",
                                              (double)_nModels++, 
                                               oModelSource,
                                              &oModelUnits) );

        //
        // add the section to the writer
        //
    if (pModelSection)
    {
        _oPackageWriter.addSection( pModelSection );
    }
    else
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new section" );
    }

    //
    // going to need this...
    //
    _pCurrentSection = pModelSection;

    //
    // create a new object definition resource for this section
    //
    _pCurrentObjectDefinition = 
        DWFCORE_ALLOC_OBJECT( DWFObjectDefinitionResource(DWFXML::kzElement_SpaceObjectDefinition, DWFXML::kzRole_ObjectDefinition) );

    if (_pCurrentObjectDefinition == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate object definition resource" );
    }

}

void
DWFPackagePublisher::_preprocessPlot( DWFPlot* pW2DPlot )
throw( DWFException )
{
    //
    // create a new EPlotSection from the DWFPlot
    //
    DWFSource oPlotSource( L"",
                            pW2DPlot->getSource(),
                            pW2DPlot->getSourceID() );

    DWFPaper* pPaper = pW2DPlot->getPaper();

    DWFEPlotSection* pPlotSection = 
        DWFCORE_ALLOC_OBJECT( DWFEPlotSection(pW2DPlot->getTitle(), 
                                               L"123",
                                              (double)_nPlots++,
                                              oPlotSource,
                                               0, // not used
                                              pPaper) );

        //
        // add the section to the writer
        //
    if (pPlotSection)
    {
        _oPackageWriter.addSection( pPlotSection );
    }
    else
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new section" );
    }

    //
    // going to need this...
    //
    _pCurrentSection = pPlotSection;

    //
    // create a new object definition resource for this section
    //
    /*
    _pCurrentObjectDefinition = 
        DWFCORE_ALLOC_OBJECT( DWFObjectDefinitionResource(DWFXML::kzElement_SpaceObjectDefinition, DWFXML::kzRole_ObjectDefinition) );

    if (_pCurrentObjectDefinition == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate object definition resource" );
    }
    */
}

void
DWFPackagePublisher::_postprocessModel( DWFModel* pW3DModel )
throw( DWFException )
{
    DWFEModelSection* pModelSection = dynamic_cast<DWFEModelSection*>(_pCurrentSection);

    if (pModelSection == NULL)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"No current section" );
    }


    //
    // Create a DWF resource to represent our W3D graphics stream
    //
    DWFGraphicResource* pW3D = 
        DWFCORE_ALLOC_OBJECT( DWFGraphicResource(pW3DModel->getTitle(),
                                                 DWFXML::kzRole_Graphics3d,
                                                 DWFMIME::kzMIMEType_W3D) );

    if (pW3D == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new section" );
    }

    double anTransform[16] = {0.0f};
    pW3DModel->getTransform( anTransform );

    vector<double> anExtents;
    pW3DModel->getBoundingVolume( anExtents );
    double* pExtents = NULL;

    if (anExtents.size() > 0)
    {
        pExtents = DWFCORE_ALLOC_MEMORY( double, anExtents.size() );
        
        if (pExtents == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate memory for extents" );
        }

        unsigned long i = 0;
        for (; i < (unsigned long)anExtents.size(); i++)
        {
            pExtents[i] = anExtents[i];
        }
    }
    
    pW3D->configureGraphic( anTransform, pExtents );

    if (pExtents)
    {
        DWFCORE_FREE_MEMORY( pExtents );
    }

    //
    // Properties
    //
    //
    DWFProperty oModelProp( L"", L"", kz_PropCategory_Hidden );

    //
    // tell the viewer how to light the scene
    //
    oModelProp.setName( kz_PropName_UseDefaultLighting );
    oModelProp.setValue( (pW3DModel->useModelLighting() ? L"1" : L"0") );
    pW3D->addProperty( &oModelProp, false );

    //
    // tell the viewer about our polygons
    //
    {
        const wchar_t* zValue = NULL;
        switch (pW3DModel->getPolygonHandedness())
        {
            case DWFModel::eHandednessLeft:
            {
                zValue = L"left";
                break;
            }
            case DWFModel::eHandednessRight:
            {
                zValue = L"right";
                break;
            }
            default:
            {
                zValue = L"none";
            }
        };

        oModelProp.setName( kz_PropName_PolygonHandedness );
        oModelProp.setValue( zValue );
        pW3D->addProperty( &oModelProp, false );
    }

    //
    // tell the viewer about silhouette edges
    //
    oModelProp.setName( kz_PropName_UseSilhouetteEdges );
    oModelProp.setValue( (pW3DModel->useSilhouetteEdges() ? L"1" : L"0") );
    pW3D->addProperty( &oModelProp, false );

        //
        // tell the viewer about any edge color
        //
    const float* const pEdgeColor = pW3DModel->getEdgeColor();
    if (pEdgeColor)
    {
        wchar_t zValues[64] = {0};
        _DWFCORE_SWPRINTF( zValues, 64, L"%.7G %.7G %.7G\0", pEdgeColor[0], pEdgeColor[1], pEdgeColor[2] );

        oModelProp.setName( kz_PropName_EdgeColor );
        oModelProp.setValue( DWFString::RepairDecimalSeparators(zValues) );
        pW3D->addProperty( &oModelProp, false );
    }


        //
        // define the default view in the XML if it's not in the stream
        //
    const W3DCamera* pCamera = pW3DModel->getDefaultView();
    if (pCamera)
    {
        wchar_t zValues[64];
        float anValues[3];

        //
        // projection
        //
        oModelProp.setName( kz_PropName_CameraProjection );
        oModelProp.setValue( (pCamera->getProjection() == W3DCamera::ePerspective ? L"1" : L"0") );
        pW3D->addProperty( &oModelProp, false );

        //
        // position
        //
        pCamera->getPosition( anValues );
        _DWFCORE_SWPRINTF( zValues, 64, L"%.7G %.7G %.7G\0", anValues[0], anValues[1], anValues[2] );

        oModelProp.setName( kz_PropName_CameraPosition );
        oModelProp.setValue( DWFString::RepairDecimalSeparators(zValues) );
        pW3D->addProperty( &oModelProp, false );

        //
        // target
        //
        pCamera->getTarget( anValues );
        _DWFCORE_SWPRINTF( zValues, 64, L"%.7G %.7G %.7G\0", anValues[0], anValues[1], anValues[2] );

        oModelProp.setName( kz_PropName_CameraTarget );
        oModelProp.setValue( DWFString::RepairDecimalSeparators(zValues) );
        pW3D->addProperty( &oModelProp, false );

        //
        // up vector
        //
        pCamera->getUpVector( anValues );
        _DWFCORE_SWPRINTF( zValues, 64, L"%.7G %.7G %.7G\0", anValues[0], anValues[1], anValues[2] );

        oModelProp.setName( kz_PropName_CameraUpVector );
        oModelProp.setValue( DWFString::RepairDecimalSeparators(zValues) );
        pW3D->addProperty( &oModelProp, false );

        //
        // field
        //
        pCamera->getField( anValues );
        _DWFCORE_SWPRINTF( zValues, 64, L"%.7G %.7G %.7G\0", anValues[0], anValues[1], anValues[2] );

        oModelProp.setName( kz_PropName_CameraField );
        oModelProp.setValue( DWFString::RepairDecimalSeparators(zValues) );
        pW3D->addProperty( &oModelProp, false );
    }

    //
    // bind the streams
    //
    pW3D->setInputStream( pW3DModel->getInputStream() );

    //
    // add the graphics resource to section
    //
    pModelSection->addResource( pW3D, true );

    //
    // Publish associated resources
    //

    //
    // get the list of resources from the model
    //
    DWFPublishableResource* pResource = NULL;
    DWFIterator<DWFPublishableResource*>* piResources = pW3DModel->getResources();

    DWFImage* pImage = NULL;

    if (piResources)
    {
        for (; piResources->valid(); piResources->next())
        {
            pResource = piResources->get();

            //
            // first try images (thumbnails, previews...)
            //
            pImage = dynamic_cast<DWFImage*>(pResource);

            if (pImage)
            {
                DWFString zRole = (pImage->type() == DWFImage::eTexture)    ? DWFXML::kzRole_Texture    :
                                  (pImage->type() == DWFImage::eThumbnail)  ? DWFXML::kzRole_Thumbnail  :
                                                                              DWFXML::kzRole_Preview;

                //
                // If the image is a texture, capture the title
                //
                DWFString zTitle;
                DWFTexture* pTexture = dynamic_cast<DWFTexture*>(pImage);
                if (pTexture)
                {
                    zTitle.assign( pTexture->name() );
                }

                DWFImageResource* pImageRes = DWFCORE_ALLOC_OBJECT( DWFImageResource(zTitle, zRole, pImage->getMIMEType()) );

                if (pImageRes == NULL)
                {
                    _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate image resource" );
                }

                double anExtents[4] = { 0.0, 0.0, pImage->width(), pImage->height() };

                pImageRes->configureGraphic( NULL, anExtents, pImage->clip() );
                pImageRes->configureImage( pImage->depth() );
                pImageRes->setInputStream( pImage->getInputStream() );

                //
                // add image to section
                // 1. section owns the resource (true)
                // 2. replace with the resource if applicable (true)
                // 3. deleted replaced resource if applicable (true)
                //
                pModelSection->addResource( pImageRes, true, true, true, pW3D );
            }
        }

        DWFCORE_FREE_OBJECT( piResources );
    }

    //
    // Embed Fonts
    //

    unsigned long iFont = 0;
    unsigned long nFonts = static_cast<unsigned long>(_oEmbeddedFonts.size());
    DWFEmbeddedFont* pFont = NULL;

        //
        // embed each font as a section resource
        //
    for (; iFont < nFonts; iFont++)
    {
        pFont = const_cast<DWFEmbeddedFont*>(_oEmbeddedFonts[iFont]);

        //
        // make sure the data is available
        //
        pFont->embed();

        //
        // create a new font resource
        //
        DWFFontResource* pFontRes = 
            DWFCORE_ALLOC_OBJECT( DWFFontResource(pFont->getRequest(),
                                                  pFont->getPrivilege(),
                                                  pFont->getCharacterCode(),
                                                  pFont->getFaceName(),
                                                  pFont->getLogfontName()) );

        if (pFontRes == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate font resource" );
        }

        //
        // set the data stream
        //
        pFontRes->setInputStream( pFont->getInputStream() );

        //
        // add font to section
        //
        pModelSection->addResource( pFontRes, true );
    }

    //
    // prepare for next section
    //
    _oEmbeddedFonts.clear();

    //
    // Object Definition
    //
    // 1. section owns the resource (true)
    // 2. replace with the resource if applicable (true)
    // 3. deleted replaced resource if applicable (true)
    //
    pModelSection->addResource( _pCurrentObjectDefinition, true, true, true, pW3D );

    //
    // MOVE all resources from the DWFModel to the DWFEModelSection
    //
    DWFResource* pDWFResource = NULL;
    DWFKVIterator<const wchar_t*, DWFResource*>* piAllResources = pW3DModel->getResourcesByRole();

    if (piAllResources)
    {
        for (; piAllResources->valid(); piAllResources->next())
        {
            pDWFResource = piAllResources->value();
            pModelSection->addResource(pDWFResource, false);
        }

        DWFCORE_FREE_OBJECT( piAllResources );
    }
    //
    // Model Properties
    //
    // MOVE all properties from the DWFModel to the DWFEModelSection
    //
    pModelSection->copyProperties( *pW3DModel, true );

}


void
DWFPackagePublisher::_postprocessPlot( DWFPlot* pW2DPlot )
throw( DWFException )
{
    DWFEPlotSection* pPlotSection = dynamic_cast<DWFEPlotSection*>(_pCurrentSection);

    if (pPlotSection == NULL)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"No current section" );
    }


    //
    // Create a DWF resource to represent our W2D graphics stream
    //
    DWFGraphicResource* pW2D = 
        DWFCORE_ALLOC_OBJECT( DWFGraphicResource(pW2DPlot->getTitle(),
                                                 DWFXML::kzRole_Graphics2d,
                                                 DWFMIME::kzMIMEType_W2D) );

    if (pW2D == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new section" );
    }


    DWFUUID oObjectID;
    pW2D->setObjectID(oObjectID.uuid(true));

    double anTransform[16] = {0.0f};
    pW2DPlot->getTransform( anTransform );

    vector<double> anExtents;
    pW2DPlot->getPlotExtents( anExtents );
    double* pExtents = NULL;

    if (anExtents.size() > 0)
    {
        pExtents = DWFCORE_ALLOC_MEMORY( double, anExtents.size() );
        
        if (pExtents == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate memory for extents" );
        }

        unsigned long i = 0;
        for (; i < (unsigned long)anExtents.size(); i++)
        {
            pExtents[i] = anExtents[i];
        }
    }
    
    vector<double> anClip;
    pW2DPlot->getPlotClip( anClip );
    double* pClip = NULL;

    if (anClip.size() > 0)
    {
        pClip = DWFCORE_ALLOC_MEMORY( double, anClip.size() );
        
        if (pClip == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate memory for Clip" );
        }

        unsigned long i = 0;
        for (; i < (unsigned long)anClip.size(); i++)
        {
            pClip[i] = anClip[i];
        }
    }
    
    pW2D->configureGraphic( anTransform, pExtents, pClip );

    if (pExtents)
    {
        DWFCORE_FREE_MEMORY( pExtents );
    }

    if (pClip)
    {
        DWFCORE_FREE_MEMORY( pClip );
    }

    //
    // bind the streams
    //
    pW2D->setInputStream( pW2DPlot->getInputStream() );

    //
    // add the graphics resource to section
    //
    pPlotSection->addResource( pW2D, true );

    //
    // Publish associated resources
    //

    //
    // get the list of resources from the plot
    //
    DWFPublishableResource* pResource = NULL;
    DWFIterator<DWFPublishableResource*>* piResources = pW2DPlot->getResources();

    DWFImage* pImage = NULL;

    if (piResources)
    {
        for (; piResources->valid(); piResources->next())
        {
            pResource = piResources->get();

            //
            // first try images (thumbnails, previews...)
            //
            pImage = dynamic_cast<DWFImage*>(pResource);

            if (pImage)
            {
                DWFString zRole = (pImage->type() == DWFImage::eThumbnail)  ? DWFXML::kzRole_Thumbnail  :
                                                                              DWFXML::kzRole_Preview;


                DWFImageResource* pImageRes = DWFCORE_ALLOC_OBJECT( DWFImageResource(L"", zRole, pImage->getMIMEType()) );

                if (pImageRes == NULL)
                {
                    _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate image resource" );
                }

                double anExtents[4] = { 0.0, 0.0, pImage->width(), pImage->height() };

                pImageRes->configureGraphic( NULL, anExtents, pImage->clip() );
                pImageRes->configureImage( pImage->depth() );
                pImageRes->setInputStream( pImage->getInputStream() );

                //
                // add image to section
                // 1. section owns the resource (true)
                // 2. replace with the resource if applicable (true)
                // 3. deleted replaced resource if applicable (true)
                //
                pPlotSection->addResource( pImageRes, true, true, true, pW2D );
            }
        }

        DWFCORE_FREE_OBJECT( piResources );
    }

    //
    // Embed Fonts
    //

    unsigned long iFont = 0;
    unsigned long nFonts = static_cast<unsigned long>(_oEmbeddedFonts.size());
    DWFEmbeddedFont* pFont = NULL;

        //
        // embed each font as a section resource
        //
    for (; iFont < nFonts; iFont++)
    {
        pFont = const_cast<DWFEmbeddedFont*>(_oEmbeddedFonts[iFont]);

        //
        // make sure the data is available
        //
        pFont->embed();

        //
        // create a new font resource
        //
        DWFFontResource* pFontRes = 
            DWFCORE_ALLOC_OBJECT( DWFFontResource(pFont->getRequest(),
                                                  pFont->getPrivilege(),
                                                  pFont->getCharacterCode(),
                                                  pFont->getFaceName(),
                                                  pFont->getLogfontName()) );

        if (pFontRes == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate font resource" );
        }

        //
        // set the data stream
        //
        pFontRes->setInputStream( pFont->getInputStream() );

        //
        // add font to section
        //
        pPlotSection->addResource( pFontRes, true );
    }

    //
    // prepare for next section
    //
    _oEmbeddedFonts.clear();

    //
    // MOVE all resources from the DWFPlot to the DWFEPlotSection
    //
    DWFResource* pDWFResource = NULL;
    DWFKVIterator<const wchar_t*, DWFResource*>* piAllResources = pW2DPlot->getResourcesByRole();

    if (piAllResources)
    {
        for (; piAllResources->valid(); piAllResources->next())
        {
            pDWFResource = piAllResources->value();
            if (pDWFResource->role() == DWFXML::kzRole_ObjectDefinition
                || pDWFResource->role() == DWFXML::kzRole_MarkupObjectDefinition)
            {
                pDWFResource->setParentResource(pW2D);
            }
            pPlotSection->addResource(pDWFResource, false);
        }

        DWFCORE_FREE_OBJECT( piAllResources );
    }

    //
    // Model Properties
    //
    // MOVE all properties from the DWFPlot to the DWFPlotSection
    //
    pPlotSection->copyProperties( *pW2DPlot, true );

}


/******************************************************************
void
DWFPackagePublisher::_preprocessGeneric( DWFGeneric* pGeneric )
throw( DWFException )
{
    ;
}

void
DWFPackagePublisher::_postprocessGeneric( DWFGeneric* )
throw( DWFException )
{
    ;
}
******************************************************************/

//DNT_End

#endif
