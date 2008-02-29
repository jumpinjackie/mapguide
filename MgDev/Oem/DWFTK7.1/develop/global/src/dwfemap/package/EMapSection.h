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
#ifndef _DWFEMAPTK_EMAPSECTION_H
#define _DWFEMAPTK_EMAPSECTION_H

#include "../EMapToolkit.h"
#include "./EMapSectionDescriptorReader.h"
#include "dwf/package/Section.h"
#include <vector>
using namespace DWFCore;
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    class DWFBackground;
    class DWFCoordinateSpace;
    class DWFEMapLayer;
    class DWFEMapLayerGroup;

    //--------------------------------------------------------------------
    // DESCRIPTION:
    // The class DWFEMapSection represents the root document level element
    // of an EMap Section. As such, it offers unique functionality amongst
    // the EMap classes. In addition to being the root object in an EMap
    // document it also is a container for all the Resources 
    // required by any EMap schema element.
    //--------------------------------------------------------------------
    class DWFEMapSection : public DWFSection,
                           public DWFEMapSectionDescriptorReader
    {
    public:
        // Inner class for defining the Factory that will construct this
        // EMapSection.
        class Factory : public DWFSection::Factory
        {
        public:
            // constructor / destructor
            _DWFTK_API Factory() throw();
            _DWFTK_API virtual ~Factory() throw();

            // inherited from DWFSection::Factory
            _DWFTK_API virtual DWFSection* build( const DWFString& zName,
                                                  const DWFString& zTitle,
                                                  DWFPackageReader* pPackageReader)
                                                  throw( DWFException );
        };

    public:
        // basic constructors
        _DWFTK_API DWFEMapSection( const DWFString& zName,
                                   const DWFString& zTitle,
                                   DWFPackageReader* pPackageReader ) throw();

        _DWFTK_API DWFEMapSection( const DWFString& zTitle,
                                   const DWFString& zObjectID,
                                   const DWFSource& rSource,
                                   const DWFBackground& background,
                                   const DWFCoordinateSpace& space,
                                   const DWFString& zUri = L"" ) throw();

        // destructor
        _DWFTK_API virtual ~DWFEMapSection() throw();

        // background
        _DWFTK_API const DWFBackground* const background() throw();

        // coordinate space
        _DWFTK_API const DWFCoordinateSpace* const coordinateSpace() throw();

        // uri to the server that served up this map
        _DWFTK_API const DWFString& uri() const throw();

        // access a layer by its name
        _DWFTK_API const DWFEMapLayer* const findLayerByName(
            const DWFString& zName) throw();

        // access a layer by its object ID
        _DWFTK_API const DWFEMapLayer* const findLayerByObjectID(
            const DWFString& zObjectID) throw();

        // returns the layer at the specified index, or NULL if the
        // index value is out of range
        _DWFTK_API const DWFEMapLayer* getLayer( size_t index ) const
            throw();

        // returns the number of layers in the map
        _DWFTK_API size_t layerCount() const throw();

        // adds a layer to the EMap section
        _DWFTK_API void addLayer( DWFEMapLayer* pLayer )
            throw( DWFException );

        // access a layer group by its name
        _DWFTK_API const DWFEMapLayerGroup* const findLayerGroupByName(
            const DWFString& zName ) const throw();

        // access a layer group by its object ID
        _DWFTK_API const DWFEMapLayerGroup* const findLayerGroupByObjectID(
            const DWFString& zObjectID ) const throw();

        // returns the layer group at the specified index, or NULL if the
        // index value is out of range
        _DWFTK_API const DWFEMapLayerGroup* getLayerGroup( size_t index )
            const throw();

        // returns the number of layer groups in the map
        _DWFTK_API size_t layerGroupCount() const throw();

        // adds a layer group to the EMap section
        _DWFTK_API void addLayerGroup( DWFEMapLayerGroup* pLayerGroup )
            throw( DWFException );

        // XMLCallback support
        _DWFTK_API void notifyStartElement( const char* zName,
                                            const char** ppAttributeList ) throw();
        _DWFTK_API void notifyEndElement( const char* zName ) throw();
        _DWFTK_API void notifyStartNamespace( const char* zPrefix,
                                              const char* zURI ) throw();
        _DWFTK_API void notifyEndNamespace( const char* zPrefix ) throw();
        _DWFTK_API void notifyCharacterData( const char* zCData, int nLength ) throw();

        // This method will locate the one descriptor resource in the section
        // and parse it accordingly with the reader provided or the default reader
        // for the section (this object).  In order to use another reader as a filter
        // first call setFilter() on this EMapSection object.
        _DWFTK_API const DWFResource& readDescriptor(
            DWFSectionDescriptorReader* pSectionDescriptorReader = NULL ) const
            throw( DWFException );

        // DWFXMLSerializable support
        _DWFTK_API void serializeXML( DWFXMLSerializer& rSerializer,
                                      unsigned int nFlags ) throw( DWFException );
    
    private:
        // hidden copy constructor and assignment operator
        DWFEMapSection( const DWFEMapSection& );
        void operator=( const DWFEMapSection& );

        // methods
        void _processAttributes( const char** ppAttributeList ) throw();

        // data members
        DWFString _zUri;
        DWFBackground* _poBackground;
        DWFCoordinateSpace* _poCoordinateSpace;

        // template collections for DWFEMapLayer and DWFEMapLayerGroup objects
        typedef std::vector<DWFEMapLayer*>  LayerCollection;
        LayerCollection _oLayers;

        typedef std::vector<DWFEMapLayerGroup*> LayerGroupCollection;
        LayerGroupCollection _oLayerGroups;

        // helpers for parsing XML
        unsigned char _nFlag;
        DWFXMLCallback* _poCallback;
        DWFPackageReader* _poPackageReader;
    };
}}

#endif // _DWFEMAPTK_EMAPSECTION_H
// End of file.
