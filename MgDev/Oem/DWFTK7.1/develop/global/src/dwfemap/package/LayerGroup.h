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
#ifndef _DWFEMAPTK_LAYERGROUP_H
#define _DWFEMAPTK_LAYERGROUP_H

#include "../EMapToolkit.h"
#include "dwf/package/reader/PackageReader.h"
#include "dwf/package/utility/PropertyContainer.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    class DWFUIGraphic;

    //--------------------------------------------------------------------
    // DESCRIPTION
    // Class DWFEMapLayerGroup corresponds to the LayerGroup element in
    // the DWF EMap XML schema.
    // LayerGroup is used to define a collection of Layers for the Map.
    // This is primarily done so that a common set of attributes defined
    // in a LayerGroup can be propagated to the Layers that belong to that
    // group.  LayerGroups do not refer to the Layers that belong to them,
    // rather Layer's have their group property set to the unique name of
    // the LayerGroup to which it belongs.
    //--------------------------------------------------------------------
    class DWFEMapLayerGroup : public DWFXMLBuildable
                            , public DWFXMLCallback
                            , public DWFXMLSerializable
                            , public DWFPropertyContainer
    {
    public:
        // constructor used with XMLBuildable
        _DWFTK_API DWFEMapLayerGroup( DWFPackageReader* pPackageReader ) throw();

        // basic constructor
        _DWFTK_API DWFEMapLayerGroup( const DWFString& zName,
                                      const DWFString& zObjectID,
                                      const DWFString& zGroupObjectID = L"",
                                      bool bVisible = true,
                                      DWFUIGraphic* pUIGraphic = NULL ) throw();

        // acquire member data from attribute list (from XML parser)
        _DWFTK_API virtual void parseAttributeList( const char** ppAttributeList )
                                                    throw( DWFException );

        // destructor
        _DWFTK_API virtual ~DWFEMapLayerGroup() throw();

        // property accessors
        _DWFTK_API const DWFString& name() const throw();
        _DWFTK_API const DWFString& objectID() const throw();
        _DWFTK_API const DWFString& groupObjectID() const throw();
        _DWFTK_API bool visible() const throw();
        _DWFTK_API const DWFUIGraphic* const uiGraphic() const throw();

        // XMLCallback support
        _DWFTK_API void notifyStartElement( const char* zName,
                                            const char** ppAttributeList ) throw();
        _DWFTK_API void notifyEndElement( const char* zName ) throw();
        _DWFTK_API void notifyStartNamespace( const char* zPrefix,
                                              const char* zURI ) throw();
        _DWFTK_API void notifyEndNamespace( const char* zPrefix ) throw();
        _DWFTK_API void notifyCharacterData( const char* zCData, int nLength ) throw();

        // DWFXMLSerializable support
        _DWFTK_API void serializeXML( DWFXMLSerializer& rSerializer,
                                      unsigned int nFlags ) throw( DWFException );

    private:
        // data members
        DWFString       _zName;
        DWFString       _zObjectID;
        DWFString       _zGroupObjectID;
        bool            _bVisible;
        DWFUIGraphic*   _poUIGraphic;

        // helper variables for parsing the XML data
        DWFPackageReader* _poPackageReader;
        unsigned char _nFlag;
    };
}}

#endif // _DWFEMAPTK_LAYERGROUP_H
// End of file.
