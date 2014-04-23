//
//  Copyright (C) 2001-2011 by Autodesk, Inc.
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




#if !defined CONTRAST_COLOR_HEADER
#define CONTRAST_COLOR_HEADER

#include "whiptk/whipcore.h"


/** \addtogroup groupRenditionAttributes
 *  @{
 *  The BackgroundColor rendition attribute is a straightforward 32 bit color declaration 
 *  ( RGBA) that is applied to drawables that require a background color. 
 *  Note: Unlike the color attribute it does not work with a color map and requires
 *  that every definition of Background color have its entire RGBA (Red Green Blue Alpha)
 *  definition
 */


/// Attribute class for specifying a Background color 
class WHIPTK_API WT_Contrast_Color : public WT_Attribute
{
public:
    //@{ \name Construction
	/// Default Constructor 
    WT_Contrast_Color()
        :m_color(0,0,0,0)
    {}
    /// Constructor with  WT_RGBA32 structure
    WT_Contrast_Color(WT_RGBA32 color)
        :m_color(color)
    {}
    
    /// Constructor with individual color and alpha values
    WT_Contrast_Color(
        int red, /**< Red coponent of RGBA color. */
        int green, /**< Green coponent of RGBA color. */
        int blue, /**< Blue coponent of RGBA color. */
        int alpha = 255) /**< Alpha component of RGBA color (0 = transparent, 255 = opaque.) */
        :m_color(red,green,blue,alpha)
    {}
    
    // returns the background color
    WT_RGBA32 color() const 
    { return m_color;}

    //@{ \name WT_Object and WT_Attribute virtual methods
    WT_ID            object_id() const;
    WT_Result        materialize(WT_Opcode const & opcode, WT_File & file);
    WT_Result        process(WT_File & file);
    WT_Result        skip_operand(WT_Opcode const & opcode, WT_File & file);
    WT_Result        serialize (WT_File & file) const;
    WT_Result        sync (WT_File & file) const;
    WT_Boolean       operator== (WT_Attribute const & attrib) const;
    //@}
    
    //@{ \name Inspection methods
    /// Returns WD_True if the given pattern is equal to this one (compares ID and pattern scale.)
    WT_Boolean operator== (WT_Contrast_Color const & color) const;
    /// Returns WD_True if the given pattern is NOT equal to this one (compares ID and pattern scale.)
    WT_Boolean operator!= (WT_Contrast_Color const & color) const;
    //@}
    
    /// Provides a default action handler for this attribute (e.g. sets the current rendition's corresponding attribute.)
    /** \warning This is used by the framework and should not be called by client code.
     */
    static WT_Result default_process(
        WT_Contrast_Color & item, /**< The attribute to process. */
        WT_File & file /**< The file being read. */
        );
private:
    // Data members
    WT_RGBA32 m_color;
};

#endif // CONTRAST_COLOR_HEADER
