//
//  Copyright (C) 1996-2008 by Autodesk, Inc.
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

#ifndef _DWFTK_SEGMENT_H
#define _DWFTK_SEGMENT_H


///
///\file        dwf/publisher/model/Segment.h
///\brief       This file contains the DWFSegment class declaration.
///

#ifndef DWFTK_READ_ONLY


#include "dwfcore/String.h"
using namespace DWFCore;

#include "dwf/Toolkit.h"
#include "dwf/w3dtk/BStream.h"
#include "dwf/w3dtk/BOpcodeHandler.h"
#include "dwf/w3dtk/BOpcodeShell.h"
#include "dwf/package/utility/PropertyContainer.h"
#include "dwf/publisher/PublishedObject.h"
#include "dwf/publisher/model/AttributeHandlerBuilder.h"
#include "dwf/publisher/model/GeometryHandlerBuilder.h"
#include "dwf/publisher/model/FeatureHandlerBuilder.h"
#include "dwf/publisher/model/SegmentHandlerBuilder.h"



namespace DWFToolkit
{

//
// fwd decl
//
class DWFStyleSegment;
class DWFIncludeSegment;


///
///\ingroup         dwfpublish3d
///
///\class           DWFSegment     dwf/publisher/model/Segment.h    "dwf/publisher/model/Segment.h"
///\brief           Represents a unit in the scene to which geometry and/or attributes can be applied.
///\since           7.0.1
///
class DWFSegment : public DWFAttributeHandlerBuilder
                 , public DWFGeometryHandlerBuilder
                 , public DWFFeatureHandlerBuilder
                 , public DWFPropertyContainer
{


public:

    ///
    ///\brief       Indicates the segment represents some object of interest
    ///             that may have metadata and scope outside of the 3d graphic stream itself.
    ///
    static const char* const kz_User_Options_Node;
    ///
    ///\brief       Indicates the segment contains explicitly defined feature geometry.
    ///
    static const char* const kz_User_Options_Edges;	 
///
    ///\brief       Indicates the segment represents some object of interest
    ///             that may have metadata and scope outside of the 3d graphic stream itself;
    ///             and additionally contains explicitly defined feature edge geometry.
    ///
    static const char* const kz_User_Options_Hidden;
    static const char* const kz_User_Options_Transparent;

    static const char* const kz_User_Options_Hidden_Node;
    static const char* const kz_User_Options_Transparent_Node;
    
    static const char* const kz_User_Options_Node_and_Edges;
    static const char* const kz_User_Options_Hidden_Node_and_Edges;
    static const char* const kz_User_Options_Transparent_Node_and_Edges;

    ///
    ///\brief       Defines the data type used for segment keys.
    ///
    typedef DWFPublishedObject::tKey   tKey;

    //
    // 
    //
    typedef enum
    {
        eDefault,

        eHidden,
        eTransparent

    } tePublishedState;

public:

    ///
    ///             Constructor
    ///
    ///\param       rSegmentBuilder     An interface for acquiring the segment-specific op-code handlers.
    ///\param       rGeometryBuilder    An interface for acquiring the geometry-specific op-code handlers.
    ///\param       rFeatureBuilder     An interface for acquiring the feature-specific op-code handlers.
    ///\param       rAttributeBuilder   An interface for acquiring the attribute-specific op-code handlers.
    ///\param       rObjectFactory      An interface for building published objects.
    ///\param       pPublishedParent    An optional object defining a structural relationship between
    ///                                 itself and the published object that will be created for this segment.
    ///\throw       None
    ///
    _DWFTK_API
    DWFSegment( DWFSegmentHandlerBuilder&     rSegmentBuilder,
                DWFGeometryHandlerBuilder&    rGeometryBuilder,
                DWFFeatureHandlerBuilder&     rFeatureBuilder,
                DWFAttributeHandlerBuilder&   rAttributeBuilder,
                DWFPublishedObject::Factory&  rObjectFactory,
                DWFPublishedObject*           pPublishedParent = NULL )
        throw();

    ///
    ///             Copy Constructor
    ///
    ///\param       rSegment    The segment to clone.
    ///\throw       DWFException
    ///
    _DWFTK_API
    DWFSegment( const DWFSegment& rSegment )
        throw( DWFException );

    ///
    ///             Assignment Operator
    ///
    ///\param       rSegment    The segment to clone.
    ///\throw       DWFException
    ///
    _DWFTK_API
    DWFSegment& operator=( const DWFSegment& rSegment )
        throw( DWFException );

    ///
    ///             Destructor
    ///
    ///\throw       None
    ///
    _DWFTK_API
    virtual ~DWFSegment()
        throw();

    ///
    ///             Returns a unique identifier for the segment.
    ///
    ///\return      The segment key.  
    ///\throw       None
    ///
    _DWFTK_API
    tKey key() const
        throw();

    ///
    ///             Opens the segment for use.
    ///
    ///             The usage model for segments is:
    ///             -# Create or obtain a segment object.
    ///             -# Open the segment.
    ///             -# Add geometry, attributes, sub-segments, etc.
    ///             -# Close the segment.
    ///             -# Repeat. (The same segment object can be reused.)
    ///
    ///\param       pName               An optional name for the segment.
    ///                                 If a segment is named it will be marked as a node
    ///                                 and captured in the package metadata as a defined object and/or instance.
    ///
    ///\param       bPublishedEdges     Set this flag to \e true if feature edge geometry
    ///                                 will be added to this segment.
    ///\throw       DWFException
    ///
    _DWFTK_API
    virtual void open( const DWFString* zName = NULL,
                       bool             bPublishedEdges = false )
        throw( DWFException );

    ///
    ///             Opens the segment for use.
    ///
    ///             The usage model for segments is:
    ///             -# Create or obtain a segment object.
    ///             -# Open the segment.
    ///             -# Add geometry, attributes, sub-segments, etc.
    ///             -# Close the segment.
    ///             -# Repeat. (The same segment object can be reused.)
    ///
    ///\param       zName               An optional name for the segment.
    ///                                 If a segment is named it will be marked as a node
    ///                                 and captured in the package metadata as a defined object and/or instance.
    ///
    ///\param       bPublishedEdges     Set this flag to \e true if feature edge geometry
    ///                                 will be added to this segment.
    ///\throw       DWFException
    ///
    _DWFTK_API
    virtual void open( const DWFString& zName,
                       bool             bPublishedEdges = false )
        throw( DWFException );

    ///
    ///             Closes the segment completely and serializes any remaining
    ///             data and op-codes into the W3D stream.
    ///
    ///             This segment object will be completely cleaned out and
    ///             is available for reuse after this method returns.
    ///
    ///\throw       DWFException
    ///
    _DWFTK_API
    void close()
        throw( DWFException );

    ///
    ///             Creates a subsegment in the scene.
    ///         
    ///             Despite this method name, the subsegment object
    ///             must still be explicitly opened as it follows the 
    ///             general workflow detailed in \a open().
    ///
    ///\throw       DWFException
    ///
    _DWFTK_API
    DWFSegment openSegment()
        throw( DWFException );

    ///
    ///             Applies a style (via a styling segment).
    ///
    ///             Styling segments are bundles of attributes that can be
    ///             applied as a whole in the scene to affect visualization.
    ///
    ///\param       rSegment    The style to apply.
    ///\throw       DWFException
    ///
    _DWFTK_API
    void style( DWFStyleSegment& rSegment )
        throw( DWFException );

    ///
    ///             Capture a reference to a segment from the include library.
    ///
    ///             Use this method for reusing geometry and instancing objects and composites.
    ///
    ///             Since all segments in the include library currently must be named,
    ///             including such a segment will create parent-child relationship in the
    ///             meta-structure if this segment is <b>un</b>named. See DWFIncludeSegment
    ///             and \ref dwfpublish3d page for more details.
    ///
    ///\param       rSegment        The segment to include.
    ///\throw       DWFException
    ///
    _DWFTK_API
    virtual void include( DWFIncludeSegment& rSegment )
        throw( DWFException );

    ///
    ///             Capture a reference to a segment from the include library.
    ///
    ///             Use this method for reusing geometry and instancing objects and composites.
    ///
    ///             Since all segments in the include library currently must be named,
    ///             including such a segment will create parent-child relationship in the
    ///             meta-structure if this segment is <b>un</b>named. See DWFIncludeSegment
    ///             and \ref dwfpublish3d page for more details.
    ///
    ///\param       rSegment        The segment to include.
    ///\param       zInstanceName   Changes the name of the relationship in the meta-structure
    ///                             (from the name of the include segment.)
    ///
    ///\throw       DWFException
    ///
    _DWFTK_API
    void include( DWFIncludeSegment& rSegment,
                  const DWFString&   rInstanceName )
        throw( DWFException );

    ///
    ///             Capture a reference to a segment from the include library.
    ///
    ///             Use this method for reusing geometry and instancing objects and composites.
    ///
    ///             Since all segments in the include library currently must be named,
    ///             including such a segment will create parent-child relationship in the
    ///             meta-structure if this segment is <b>un</b>named. See DWFIncludeSegment
    ///             and \ref dwfpublish3d page for more details.
    ///
    ///\param       nSegmentKey     The key of the segment to include.
    ///\throw       DWFException
    ///
    _DWFTK_API
    virtual void include( tKey nSegmentKey )
        throw( DWFException );

    ///
    ///             Capture a reference to a segment from the include library.
    ///
    ///             Use this method for reusing geometry and instancing objects and composites.
    ///
    ///             Since all segments in the include library currently must be named,
    ///             including such a segment will create parent-child relationship in the
    ///             meta-structure if this segment is <b>un</b>named. See DWFIncludeSegment
    ///             and \ref dwfpublish3d page for more details.
    ///
    ///\param       nSegmentKey     The key of the segment to include.
    ///\param       zInstanceName   Changes the name of the relationship in the meta-structure
    ///                             (from the name of the include segment.)
    ///
    ///\throw       DWFException
    ///
    _DWFTK_API
    void include( tKey             nSegmentKey,
                  const DWFString& rInstanceName )
        throw( DWFException );

public:

    ///
    ///\copydoc DWFPropertyContainer::findProperty
    ///
    _DWFTK_API
    virtual const DWFProperty* const findProperty( const DWFString& zName,
                                                   const DWFString& zCategory = L"" ) const
        throw();

    ///
    ///\copydoc DWFPropertyContainer::getProperties
    ///
    _DWFTK_API
    virtual DWFProperty::tMap::Iterator* getProperties( const DWFString& zCategory = L"" )
        throw();

    ///
    ///\copydoc DWFPropertyContainer::addProperty
    ///
    _DWFTK_API
    virtual void addProperty( DWFProperty* pProperty )
        throw( DWFException );

    ///
    ///         Inserts a property to the container.
    ///
    ///\param   rProperty           The property to add to the container.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual void addProperty( const DWFProperty& rProperty )
        throw( DWFException );

    ///
    ///\copydoc DWFPropertyContainer::addPropertyContainer
    ///
    _DWFTK_API
    virtual void addPropertyContainer( DWFPropertyContainer* pContainer )
        throw( DWFException );
    
    ///
    ///\copydoc DWFPropertyContainer::referencePropertyContainer
    ///
    _DWFTK_API
    virtual void referencePropertyContainer( const DWFPropertyContainer& rContainer )
        throw( DWFException );

    ///
    ///\copydoc DWFPropertyContainer::removeOwnedPropertyContainers
    ///
    _DWFTK_API
    virtual void removeOwnedPropertyContainers( DWFPropertyContainer::tList& rContainerList,
                                                bool                         bMakeReferences = false )
        throw( DWFException );

    ///
    ///\copydoc DWFPropertyContainer::removeReferencedPropertyContainers
    ///
    _DWFTK_API
    virtual void removeReferencedPropertyContainers( DWFPropertyContainer::tList& rContainerList )
        throw( DWFException );

    ///
    ///\copydoc DWFPropertyContainer::removeAllPropertyContainers
    ///
    _DWFTK_API
    virtual void removeAllPropertyContainers( DWFPropertyContainer::tList& rContainerList )
        throw( DWFException );

    ///
    ///\copydoc DWFPropertyContainer::copyProperties
    ///
    _DWFTK_API
    virtual void copyProperties( DWFPropertyContainer& rContainer,
                                 bool                  bRemoveOwnership = false,
                                 bool                  bMakeReferences = false)
        throw( DWFException );

public:

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getAreaLightHandler
    ///
    _DWFTK_API
    TK_Area_Light& getAreaLightHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getCircleHandler
    ///
    _DWFTK_API
    TK_Circle& getCircleHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getCircularArcHandler
    ///
    _DWFTK_API
    TK_Circle& getCircularArcHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getCircularChordHandler
    ///
    _DWFTK_API
    TK_Circle& getCircularChordHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getCircularWedgeHandler
    ///
    _DWFTK_API
    TK_Circle& getCircularWedgeHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getCuttingPlaneHandler
    ///
    _DWFTK_API
    TK_Cutting_Plane& getCuttingPlaneHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getDistantLightHandler
    ///
    _DWFTK_API
    TK_Point& getDistantLightHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getEllipseHandler
    ///
    _DWFTK_API
    TK_Ellipse& getEllipseHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getEllipticalArcHandler
    ///
    _DWFTK_API
    TK_Ellipse& getEllipticalArcHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getGlyphHandler
    ///
    _DWFTK_API
    TK_Glyph_Definition& getGlyphDefinitionHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getImageHandler
    ///
    _DWFTK_API
    TK_Image& getImageHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getLineHandler
    ///
    _DWFTK_API
    TK_Line& getLineHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getLocalLightHandler
    ///
    _DWFTK_API
    TK_Point& getLocalLightHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getNURBSCurveHandler
    ///
    _DWFTK_API
    TK_NURBS_Curve& getNURBSCurveHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getNURBSSurfaceHandler
    ///
    _DWFTK_API
    TK_NURBS_Surface& getNURBSSurfaceHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getPolyCylinderHandler
    ///
    _DWFTK_API
    TK_PolyCylinder& getPolyCylinderHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getPolygonHandler
    ///
    _DWFTK_API
    TK_Polypoint& getPolygonHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getPolylineHandler
    ///
    _DWFTK_API
    TK_Polypoint& getPolylineHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getShellHandler
    ///
    _DWFTK_API
    TK_Shell& getShellHandler( bool bTriStripsOnly = false,
                               bool bDisableOptimization = false )
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getSpotLightHandler
    ///
    _DWFTK_API
    TK_Spot_Light& getSpotLightHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getTextHandler
    ///
    _DWFTK_API
    TK_Text& getTextHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFGeometryHandlerBuilder::getTextWithEncodingHandler
    ///
    _DWFTK_API
    TK_Text& getTextWithEncodingHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFFeatureHandlerBuilder::getTextureHandler
    ///
    _DWFTK_API
    TK_Texture& getTextureHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFFeatureHandlerBuilder::getTextureMatrixHandler
    ///
    _DWFTK_API
    TK_Matrix& getTextureMatrixHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getColorHandler
    ///
    _DWFTK_API
    TK_Color& getColorHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getColorMapHandler
    ///
    _DWFTK_API
    TK_Color_Map& getColorMapHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getColorRGBHandler
    ///
    _DWFTK_API
    TK_Color_RGB& getColorRGBHandler()
        throw( DWFException );

    _DWFTK_API
    TK_Named& getEdgePatternHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getEdgeWeightHandler
    ///
    _DWFTK_API
    TK_Size& getEdgeWeightHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getFacePatternHandler
    ///
    _DWFTK_API
    TK_Enumerated& getFacePatternHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getHandednessHandler
    ///
    _DWFTK_API
    TK_Enumerated& getHandednessHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getHeuristicsHandler
    ///
    _DWFTK_API
    TK_Heuristics& getHeuristicsHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getLinePatternHandler
    ///
    _DWFTK_API
    TK_Named& getLinePatternHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getLineStyleHandler
    ///
    _DWFTK_API
    TK_Line_Style& getLineStyleHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getLineWeightHandler
    ///
    _DWFTK_API
    TK_Size& getLineWeightHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getMarkerSizeHandler
    ///
    _DWFTK_API
    TK_Size& getMarkerSizeHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getMarkerSymbolHandler
    ///
    _DWFTK_API
    TK_Enumerated& getMarkerSymbolHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getModellingMatrixHandler
    ///
    _DWFTK_API
    TK_Matrix& getModellingMatrixHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getRenderingOptionsHandler
    ///
    _DWFTK_API
    TK_Rendering_Options& getRenderingOptionsHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getSelectabilityHandler
    ///
    _DWFTK_API
    TK_Selectability& getSelectabilityHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getTextAlignmentHandler
    ///
    _DWFTK_API
    TK_Enumerated& getTextAlignmentHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getTextFontHandler
    ///
    _DWFTK_API
    TK_Text_Font& getTextFontHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getTextPathHandler
    ///
    _DWFTK_API
    TK_Point& getTextPathHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getTextSpacingHandler
    ///
    _DWFTK_API
    TK_Size& getTextSpacingHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getUserOptionsHandler
    ///
    _DWFTK_API
    TK_User_Options& getUserOptionsHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getUnicodeOptionsHandler
    ///
    _DWFTK_API
    TK_Unicode_Options& getUnicodeOptionsHandler()
        throw( DWFException );

    ///
    ///\copydoc DWFAttributeHandlerBuilder::getVisibilityHandler
    ///
    _DWFTK_API
    TK_Visibility& getVisibilityHandler()
        throw( DWFException );

protected:

    DWFSegmentHandlerBuilder&       _rSegmentBuilder;
    DWFGeometryHandlerBuilder&      _rGeometryBuilder;
    DWFFeatureHandlerBuilder&       _rFeatureBuilder;
    DWFAttributeHandlerBuilder&     _rAttributeBuilder;
    DWFPublishedObject::Factory&    _rObjectFactory;
    
    DWFPublishedObject*             _pPublishedObject;
    DWFPublishedObject*             _pPublishedParent;

    DWFString*                      _pLibrary;

private:

    bool                            _bOpen;
    DWFPublishedObject::tKey        _nKey;
    static unsigned int             _knNextKey;

private:

    //
    // Not Implemented
    //

    DWFSegment();
};



    //
    // required for win32 dll external linkage
    //
#ifdef  _DWFCORE_WIN32_SYSTEM
#ifndef DWFTK_STATIC

_declspec(selectany) const char* const DWFSegment::kz_User_Options_Node                         = "node";
_declspec(selectany) const char* const DWFSegment::kz_User_Options_Edges                        = "edges";
_declspec(selectany) const char* const DWFSegment::kz_User_Options_Hidden                       = "hidden";
_declspec(selectany) const char* const DWFSegment::kz_User_Options_Transparent                  = "transparent";

_declspec(selectany) const char* const DWFSegment::kz_User_Options_Hidden_Node                  = "node,hidden";
_declspec(selectany) const char* const DWFSegment::kz_User_Options_Transparent_Node             = "node,transparent";

_declspec(selectany) const char* const DWFSegment::kz_User_Options_Node_and_Edges               = "node,edges";
_declspec(selectany) const char* const DWFSegment::kz_User_Options_Hidden_Node_and_Edges        = "node,edges,hidden";
_declspec(selectany) const char* const DWFSegment::kz_User_Options_Transparent_Node_and_Edges   = "node,edges,transparent";

#endif
#endif


}


#endif  
#endif

