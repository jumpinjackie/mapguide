//
//  Copyright (C) 1996-2011 by Autodesk, Inc.
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



#if !defined DELINEATE_HEADER
#define DELINEATE_HEADER

#include "whiptk/whipcore.h"
#include "whiptk/attribute.h"



class WHIPTK_API WT_Delineate : public WT_Attribute
{
public:
    WT_Delineate();
    WT_Delineate(WT_Boolean delineate_state);
    
    /// Sets the fill state.
    WT_Boolean delineate()const;
    virtual ~WT_Delineate();

    /// Assignment operator.  Sets the fill state.
    WT_Delineate const & operator=(WT_Boolean const & fill_state);
    
    
     //@{ \name WT_Object and WT_Attribute virtual methods
    WT_ID            object_id() const;
    WT_Result        materialize(WT_Opcode const & opcode, WT_File & file);
    WT_Result        process(WT_File & file);
    WT_Result        skip_operand(WT_Opcode const & opcode, WT_File & file);
    WT_Result        serialize (WT_File & file) const;
    WT_Result        sync (WT_File & file) const;
    WT_Boolean       operator== (WT_Attribute const & attrib) const;
    //@}

    /// Provides a default action handler for this attribute (e.g. sets the current rendition's corresponding attribute.)
    /** \warning This is used by the framework and should not be called by client code.
     */
    static WT_Result default_process(
        WT_Delineate & item, /**< The attribute to process. */
        WT_File & file /**< The file being read. */
        );
private:
    WT_Boolean m_delineate;

};

#endif // DELINEATE_HEADER
// End of file.
