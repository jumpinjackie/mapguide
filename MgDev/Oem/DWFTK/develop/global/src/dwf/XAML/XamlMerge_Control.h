//
//  Copyright (c) 2006 by Autodesk, Inc.
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
#if !defined XAML_MERGE_CONTROL_HEADER
#define XAML_MERGE_CONTROL_HEADER

///
///\file        XAML/XamlMerge_Control.h
///

#include "XAML/XamlCore.h"

#include "whiptk/merge_control.h"
#include "XAML/XamlXML.h"


/** \addtogroup groupRenditionAttributes
 *  @{
 */

/// Rendition attribute describing a merge_control - a relation to a group of objects.
/** \sa WT_Rendition
 *
 */
class XAMLTK_API WT_XAML_Merge_Control : public WT_Merge_Control
{
	friend class WT_XAML_Class_Factory;
	friend class WT_XAML_Rendition;

protected:

    //@{ \name Construction
    /// Constructs a WT_XAML_Merge_Control object.
    WT_XAML_Merge_Control()
        : WT_Merge_Control()        
    { }

    /// Constructs a WT_Merge_Control object with the given format.
    WT_XAML_Merge_Control(WT_Merge_Control::WT_Merge_Format merge)
        : WT_Merge_Control( merge )
    { }

	///Copy Constructor
	WT_XAML_Merge_Control (WT_Merge_Control const & merge_control)
		:WT_Merge_Control (merge_control)
	{}
    /// Constructs a WT_Merge_Control object.  Copy constructor
    WT_XAML_Merge_Control (WT_XAML_Merge_Control const & merge_control)
		: WT_Merge_Control(static_cast<WT_Merge_Control const &>(merge_control))
	{}

	virtual ~WT_XAML_Merge_Control()
	{}
    //@}
public:

	/// Assignment operator.  Sets the merge_control contents from another (copies its contents.)
    WT_XAML_Merge_Control const & operator=(WT_XAML_Merge_Control const & merge_control)
	{
		WT_Merge_Control::operator = (merge_control);
		return *this;
	}

	//@{ \name WT_Object and WT_Attribute virtual methods
    WT_Result        serialize (WT_File & file) const;
	/// partial materialization of shell from W2X
	WT_Result		 parseAttributeList(XamlXML::tAttributeMap& rMap, WT_XAML_File& rFile);
};
//@}

#endif //XAML_MERGE_CONTROL_HEADER
