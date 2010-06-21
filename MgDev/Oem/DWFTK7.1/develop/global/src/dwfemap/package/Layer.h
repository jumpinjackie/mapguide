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
#ifndef _DWFEMAPTK_LAYER_H
#define _DWFEMAPTK_LAYER_H

#include "../EMapToolkit.h"
#include "dwf/package/reader/PackageReader.h"
#include "dwf/package/utility/PropertyContainer.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    class DWFScaleRange;
    class DWFUIGraphic;

    //--------------------------------------------------------------------
    // DESCRIPTION:
    // Class DWFEMapLayer corresponds to the Layer element in the DWF EMap
    // XML schema.
    // A Layer element is a related collection of data contained in a Map.
    // Layers have their visibility defined for a given Map scale value
    // by their collection of scale ranges.
    //--------------------------------------------------------------------
    class DWFEMapLayer : public DWFXMLBuildable
                       , public DWFXMLCallback
                       , public DWFXMLSerializable
                       , public DWFPropertyContainer
    {
    public:
        // constructor used with XMLBuildable
        _DWFTK_API DWFEMapLayer( DWFPackageReader* pPackageReader ) throw();

        // basic constructor
        _DWFTK_API DWFEMapLayer( const DWFString& zName,
                                 const DWFString& zObjectID,
                                 const DWFString& zGroupObjectID = L"",
                                 bool bVisible = true,
                                 bool bSelectable = true,
                                 bool bEditable = true,
                                 DWFUIGraphic* pUIGraphic = NULL ) throw();

        // acquire member data from attribute list (from XML parser)
        _DWFTK_API virtual void parseAttributeList( const char** ppAttributeList )
                                                    throw( DWFException );

        // destructor
        _DWFTK_API virtual ~DWFEMapLayer() throw();

        // property accessors
        _DWFTK_API const DWFString& name() const throw();
        _DWFTK_API const DWFString& objectID() const throw();
        _DWFTK_API const DWFString& groupObjectID() const throw();
        _DWFTK_API bool visible() const throw();
        _DWFTK_API bool selectable() const throw();
        _DWFTK_API bool editable() const throw();
        _DWFTK_API const DWFUIGraphic* const uiGraphic() const throw();

        // finds a scale range by its object ID
        _DWFTK_API const DWFScaleRange* const findScaleRangeByObjectID(const DWFString& zObjectID) const throw();

        // returns the scale range at the specified index, or NULL if the
        // index value is out of range
        _DWFTK_API const DWFScaleRange* getScaleRange( size_t index ) const throw();

        // returns the number of scale ranges in this layer
        _DWFTK_API size_t scaleRangeCount() const throw();

        // adds a scale range to the layer
        _DWFTK_API void addScaleRange( DWFScaleRange* pScaleRange ) throw( DWFException );

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

    private :
        // hidden copy constructor and assignment operator
        DWFEMapLayer( const DWFEMapLayer& );
        void operator=( const DWFEMapLayer& );

        // data members
        DWFString _zName;
        DWFString _zObjectID;
        DWFString _zGroupObjectID;
        bool _bVisible;
        bool _bSelectable;
        bool _bEditable;

        // UIGraphic
        DWFUIGraphic* _poUIGraphic;

        // collection of ScaleRanges
        typedef std::vector<DWFScaleRange*> ScaleRangeCollection;
        ScaleRangeCollection _oScaleRanges;

        // non-persisted variables used for parsing the associated XML
        DWFPackageReader* _poPackageReader;
        DWFXMLCallback* _poCallback;
        unsigned char _nFlag;
    };
}}

#endif // _DWFEMAPTK_LAYER_H
// End of file.
