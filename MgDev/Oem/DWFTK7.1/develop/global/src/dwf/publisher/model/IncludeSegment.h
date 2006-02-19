//
//  Copyright (c) 1996-2005 by Autodesk, Inc.
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

#ifndef _DWFTK_INCLUDE_SEGMENT_H
#define _DWFTK_INCLUDE_SEGMENT_H

///
///\file        dwf/publisher/model/IncludeSegment.h
///\brief       This file contains the DWFIncludeSegment class declaration.
///


#ifndef DWFTK_READ_ONLY

#include "dwf/Toolkit.h"
#include "dwf/publisher/model/Segment.h"


namespace DWFToolkit
{

///
///\ingroup         dwfpublish3d
///
///\class           DWFIncludeSegment     dwf/publisher/model/IncludeSegment.h    "dwf/publisher/model/IncludeSegment.h"
///\brief           A segment class for defining visual elements that can be referenced throughout the scene.
///\since           7.0.1
///
///                 An include segment is simply a "regular" segment that is defined in the include library
///                 rather than the scene graph.  These segments are exactly like the scene segments except
///                 that they are not part of the scene directly.  Rather they must be referenced, or included,
///                 in the scene via the \a DWFSegment::include() methods. 
///
///                 This class, as part of the \ref dwfpublish3d also may contribute to the metadata of the DWF.
///                 Currently all include segments must be named; and this name is not arbitrary.  The include
///                 segment name may be used as the object node in the DWF object structure.
///                 When a segment is included in another segment that is named (that is, a segment that itself
///                 is an object node), the included segment contributes its graphics to the scene
///                 but only the properties of the included object are applied to the referencing node.  
///                 When a segment is included in another segment that is unnamed, the included segment
///                 creates a node in the object hierarchy.
///
class DWFIncludeSegment : public DWFSegment
{

public:

    ///
    ///\brief       The local include library for the scene graph.
    ///
    static const char* const kz_Include_Library;

public:

    ///
    ///             Constructor
    ///
    ///\param       rSegmentBuilder     An interface for acquiring the segment-specific op-code handlers.
    ///\param       rGeometryBuilder    An interface for acquiring the geometry-specific op-code handlers.
    ///\param       rFeatureBuilder     An interface for acquiring the feature-specific op-code handlers.
    ///\param       rAttributeBuilder   An interface for acquiring the attribute-specific op-code handlers.
    ///\param       rObjectFactory      An interface for building published objects.
    ///\throw       None
    ///
    _DWFTK_API
    DWFIncludeSegment( DWFSegmentHandlerBuilder&    rSegmentBuilder,
                       DWFGeometryHandlerBuilder&   rGeometryBuilder,
                       DWFFeatureHandlerBuilder&    rFeatureBuilder,
                       DWFAttributeHandlerBuilder&  rAttributeBuilder,
                       DWFPublishedObject::Factory& rObjectFactory )
        throw();

    ///
    ///             Copy Constructor
    ///
    ///\param       rSegment    The segment to clone.
    ///\throw       DWFException
    ///
    _DWFTK_API
    DWFIncludeSegment( const DWFIncludeSegment& rSegment )
        throw();

    ///
    ///             Assignment Operator
    ///
    ///\param       rSegment    The segment to clone.
    ///\throw       DWFException
    ///
    _DWFTK_API
    DWFIncludeSegment& operator=( const DWFIncludeSegment& rSegment )
        throw();

    ///
    ///             Destructor
    ///
    ///\throw       None
    ///
    _DWFTK_API
    virtual ~DWFIncludeSegment()
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
    ///\throw       DWFException
    ///
    _DWFTK_API
    void open( const DWFString* zName )
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
    ///\throw       DWFException
    ///
    _DWFTK_API
    void open( const DWFString& zName )
        throw( DWFException );

private:

    //
    // Not Implemented
    //

    DWFIncludeSegment();
};


    //
    // required for win32 dll external linkage
    //
#ifdef  _DWFCORE_WIN32_SYSTEM
#ifndef DWFTK_STATIC

_declspec(selectany) const char* const DWFIncludeSegment::kz_Include_Library = "?Include Library/";

#endif
#endif


}


#endif  
#endif
