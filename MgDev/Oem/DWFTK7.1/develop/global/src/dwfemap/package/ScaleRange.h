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
#ifndef _DWFEMAPTK_SCALERANGE_H
#define _DWFEMAPTK_SCALERANGE_H

#include "../EMapToolkit.h"
#include "dwf/package/reader/PackageReader.h"
#include "dwf/package/writer/DWFXMLSerializer.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    class DWFUIGraphic;

    //--------------------------------------------------------------------
    // DESCRIPTION:
    // Class DWFScaleRange represents the ScaleRange element in the DWF EMap
    // XML schema.
    // The ScaleRange element is used to define the minimum and maximum Map scale
    // values for which its parent Layer is deemed visible.
    // A Layer can have multiple ScaleRange elements.
    // The ScaleRange element also has an optional collection of UIGraphic elements
    // that are to be used while displaying the ScaleRange.
    //--------------------------------------------------------------------
    class DWFScaleRange : public DWFXMLBuildable
                        , public DWFXMLCallback
                        , public DWFXMLSerializable
    {

    public:
        // constructor used with XMLBuildable
        _DWFTK_API DWFScaleRange( DWFPackageReader* pPackageReader ) throw();

        // basic constructor
        _DWFTK_API DWFScaleRange( const DWFString& zObjectID,
                                  double nMinScale,
                                  double nMaxScale ) throw();

        // acquire member data from attribute list (from XML parser)
        _DWFTK_API virtual void parseAttributeList( const char** ppAttributeList )
                                                    throw( DWFException );

        // destructor
        _DWFTK_API virtual ~DWFScaleRange() throw();

        // property accessors
        _DWFTK_API const DWFString& objectID() const throw();
        _DWFTK_API double minScale() const throw();
        _DWFTK_API double maxScale() const throw();

        // returns the UIGraphic at the specified index, or NULL if the
        // index value is out of range
        _DWFTK_API const DWFUIGraphic* getUIGraphic( size_t index ) const throw();

        // returns the number of UIGraphic objects in this scale range
        _DWFTK_API size_t uiGraphicCount() const throw();

        // adds a UI graphic to the scale range
        _DWFTK_API void addUIGraphic( DWFUIGraphic* pUIGraphic )
                                      throw( DWFException );

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
        DWFString _zObjectID;

        // MinScale
        double _nMinScale;

        // MaxScale
        double _nMaxScale;

        // Collection of UIGraphics
        typedef std::vector<DWFUIGraphic*> UIGraphicCollection;
        UIGraphicCollection _oUIGraphics;

        // cached PackageReader to support UIGraphics
        DWFPackageReader* _poCachedPackageReader;

        // helper pointer for parsing the collection of UIGraphic elements
        DWFXMLCallback* _poActiveChild;

        // flags handling XML parsing
        bool _bUIGraphics;
    };
}}

#endif // _DWFEMAPTK_SCALERANGE_H
// End of file.
