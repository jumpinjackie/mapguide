//
//  Copyright (C) 1996-2007 by Autodesk, Inc.
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

#if !defined POLYMARK_HEADER
#define POLYMARK_HEADER

#include "whiptk/whipcore.h"
#include "whiptk/file.h"
#include "whiptk/drawable.h"
#include "whiptk/pointset.h"


/** \addtogroup groupDrawable
 *  @{
 */

/// A drawable describing a polymarker (a set of points).
/** \ex Polymarker.cpp */
class WHIPTK_API WT_Polymarker : public WT_Drawable, public WT_Point_Set
{
public:

    /// Constructs a WT_Polymarker object.
    WT_Polymarker ()
    {}

    /// Constructs a WT_Polymarker object from the given data.
    WT_Polymarker(
        int                         count, /**< The number of points in the array. */
        WT_Logical_Point const *    points, /**< Pointer to the array of points. */
        WT_Boolean                  copy /**< Whether the points should be copied or if their addresses should be used directly from the array. */
        )
        : WT_Point_Set(count, points, copy)
    { }

    /// Destroys a WT_Polymarker object.
    ~WT_Polymarker()
    { }

    //@{ \name WT_Object and WT_Drawable virtual methods
    virtual WT_Result   delay(WT_File & file) const;
    virtual WT_Result   dump(WT_File & file) const;
    WT_ID            object_id() const;
    WT_Result        materialize(WT_Opcode const & opcode, WT_File & file);
    virtual WT_Boolean  merge(WT_Drawable const & current);
    WT_Result        process(WT_File & file);
    WT_Result        serialize(WT_File & file) const;
    WT_Result        skip_operand(WT_Opcode const & opcode, WT_File & file);
    virtual void     update_bounds(WT_File * file);
    //@}

    /// Provides a default action handler for this object.
    /** \warning This is used by the framework and should not be called by client code.
     */
    static WT_Result default_process(
        WT_Polymarker & item, /**< The object to process. */
        WT_File & file /**< The file being read. */
        );

};
//@}

#endif // POLYMARK_HEADER
