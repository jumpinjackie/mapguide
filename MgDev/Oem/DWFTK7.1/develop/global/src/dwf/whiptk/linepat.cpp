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
// $Header: /Components/Internal/DWF Toolkit/v7.1/develop/global/src/dwf/whiptk/linepat.cpp 3     5/25/05 1:53p Hainese $
#include "whiptk/pch.h"

///////////////////////////////////////////////////////////////////////////
const char* WT_Line_Pattern::m_names[Count] =
{
    "",            // Index zero is not a legal line pattern
    "Solid",
    "Dashed",
    "Dotted",
    "Dash_Dot",
    "Short_Dash",
    "Medium_Dash",
    "Long_Dash",
    "Short_Dash_X2",
    "Medium_Dash_X2",
    "Long_Dash_X2",
    "Medium_Long_Dash",
    "Medium_Dash_Short_Dash_Short_Dash",
    "Long_Dash_Short_Dash",
    "Long_Dash_Dot_Dot",
    "Long_Dash_Dot",
    "Medium_Dash_Dot_Short_Dash_Dot",
    "Sparse_Dot",
    "ISO_Dash",
    "ISO_Dash_Space",
    "ISO_Long_Dash_Dot",
    "ISO_Long_Dash_Double_Dot",
    "ISO_Long_Dash_Triple_Dot",
    "ISO_Dot",
    "ISO_Long_Dash_Short_Dash",
    "ISO_Long_Dash_Double_Short_Dash",
    "ISO_Dash_Dot",
    "ISO_Double_Dash_Dot",
    "ISO_Dash_Double_Dot",
    "ISO_Double_Dash_Double_Dot",
    "ISO_Dash_Triple_Dot",
    "ISO_Double_Dash_Triple_Dot",
    "Decorated_Tracks",
    "Decorated_Wide_Tracks",
    "Decorated_Circle_Fence",
    "Decorated_Square_Fence"
};

///////////////////////////////////////////////////////////////////////////
// AutoSketch 6.0 used the old spec. WHIP! 4.0 says their DWF files
// are corrupt. An alternative list that is also checked would avoid
// this. I left the number of table entries the same so the indexes
// would line up. Patterns not found in the original spec, e.g. the
// '-..' pattern were included because they were found in sample
// DWF files generated by AutoSketch 6.0.
const char* WT_Line_Pattern::m_alternate_names[Count] =
{
    "",            // Index zero is not a legal line pattern
    "----",
    "- -",
    "....",
    "-.-.",
    "- - ",
    "Medium_Dash",
    "Long_Dash",
    "...",
    "Medium_Dash_X2",
    "-- --",
    "Medium_Long_Dash",
    "-...",
    "Long_Dash_Short_Dash",
    "-..-..",
    "-..",
    "center",
    "phantom",
    "__ __",
    "__    __",
    "____ . ____",
    "____ .. ____",
    "____ ... ____",
    ". . . . . . .",
    "____ __ ____",
    "____ __ __ ____",
    "__ . __",
    "__ __ . __ __",
    "__ . . __ . .",
    "__ __ . . __ __",
    "__ . . . __ . . .",
    "__ __ . . . __ __",
    "",  /* tracks, wide tracks, square, circle; no legacy, just need to init memory */
    "",
    "",
    ""
};

///////////////////////////////////////////////////////////////////////////
WT_Boolean WT_Line_Pattern::operator== ( WT_Attribute const & atref ) const
{
    if( atref.object_id() != Line_Pattern_ID )
        return WD_False;
    if( m_id != ((WT_Line_Pattern const &)atref).m_id )
        return WD_False;
    return WD_True;
}

///////////////////////////////////////////////////////////////////////////
#if DESIRED_CODE(WHIP_OUTPUT)
WT_Result WT_Line_Pattern::sync( WT_File& file ) const
{
    WD_Assert( (file.file_mode() == WT_File::File_Write)   ||
               (file.file_mode() == WT_File::Block_Append) ||
               (file.file_mode() == WT_File::Block_Write) );

    if( *this != file.rendition().line_pattern() )
    {   file.rendition().line_pattern() = *this;
        return serialize(file);
    }
    return WT_Result::Success;
}
#else
WT_Result WT_Line_Pattern::sync( WT_File&) const
{
    return WT_Result::Success;
}
#endif  // DESIRED_CODE()

///////////////////////////////////////////////////////////////////////////
WT_Result WT_Line_Pattern::process( WT_File& file )
{
    WD_Assert( file.line_pattern_action() );
    return (file.line_pattern_action())(*this,file);
}

///////////////////////////////////////////////////////////////////////////
WT_Result WT_Line_Pattern::default_process(
    WT_Line_Pattern& item,
    WT_File& file )
{
    file.rendition().dash_pattern() = WT_Dash_Pattern::kNull;
    file.rendition().line_pattern() = item;
    return WT_Result::Success;
}

///////////////////////////////////////////////////////////////////////////
WT_Result WT_Line_Pattern::skip_operand(
    WT_Opcode const & opcode,
    WT_File& file )
{
    switch( opcode.type() )
    {
    case WT_Opcode::Single_Byte:
        return materialize(opcode, file);

    case WT_Opcode::Extended_ASCII:
        return opcode.skip_past_matching_paren(file);

    case WT_Opcode::Extended_Binary:
    default:
        return WT_Result::Opcode_Not_Valid_For_This_Object;
    }
}

///////////////////////////////////////////////////////////////////////////
WT_Result WT_Line_Pattern::materialize(
    WT_Opcode const & opcode,
    WT_File& file )
{
    switch( opcode.type() )
    {
    case WT_Opcode::Extended_ASCII:
        return materialize_ascii(opcode,file);
    case WT_Opcode::Single_Byte:
        return materialize_single_byte(opcode,file);
    case WT_Opcode::Extended_Binary:
    default:
        return WT_Result::Opcode_Not_Valid_For_This_Object;
    }
}

///////////////////////////////////////////////////////////////////////////
WT_Result WT_Line_Pattern::materialize_ascii(
    WT_Opcode const & opcode,
    WT_File& file )
{
    char* string = 0;
    switch( m_stage )
    {
    case Eating_Initial_Whitespace:
        WD_CHECK( file.eat_whitespace() );
        m_stage = Getting_Pattern_ID;
        // No Break Here

    case Getting_Pattern_ID:
        WD_CHECK( file.read(string,65536) );
        WD_CHECK( interpret(string,m_id) );
        delete [] string;
        m_stage = Eating_End_Whitespace;
        // No Break here

    case Eating_End_Whitespace:
        WD_CHECK( opcode.skip_past_matching_paren(file) );
        m_stage = Eating_Initial_Whitespace;
        break;

    default:
        return WT_Result::Internal_Error;
    }
    m_materialized = WD_True;
    return WT_Result::Success;
}

///////////////////////////////////////////////////////////////////////////
WT_Result WT_Line_Pattern::materialize_single_byte(
    WT_Opcode const & opcode,
    WT_File& file )
{
    if( opcode.token()[0] != 0xCC )
        return WT_Result::Corrupt_File_Error;
    WT_Integer32 tmp;
    WD_CHECK( file.read_count(tmp) );
    m_id = (WT_Pattern_ID)tmp;
    m_materialized = WD_True;
    return WT_Result::Success;
}

///////////////////////////////////////////////////////////////////////////
#if DESIRED_CODE(WHIP_OUTPUT)
WT_Result WT_Line_Pattern::serialize( WT_File& file ) const
{
    WD_CHECK( file.dump_delayed_drawable() );

    file.desired_rendition().blockref();
    WD_CHECK(file.desired_rendition().sync(file, WT_Rendition::BlockRef_Bit));

    WD_CHECK( file.write_tab_level() );

    if (m_id > WT_Line_Pattern::Solid)
        WD_CHECK (file.desired_rendition().sync(file, WT_Rendition::Line_Style_Bit));

    WD_Assert (m_id > 0);

    if (file.heuristics().allow_binary_data())
    {
        WD_CHECK( file.write((WT_Byte)0xCC) );
        return file.write_count(m_id);
    }
    else
    {
        WD_CHECK( file.write("(LinePattern ") );
        const char* pName = m_names[m_id];
        WD_CHECK( file.write_quoted_string(pName) );
        return file.write(")");
    }
}
#else
WT_Result WT_Line_Pattern::serialize( WT_File&) const
{
    return WT_Result::Success;
}
#endif  // DESIRED_CODE()

///////////////////////////////////////////////////////////////////////////
WT_Result WT_Line_Pattern::interpret(
    char const *   string,
    WT_Pattern_ID& value)
{
    // Match the text string defining the line
    // pattern with the names (normal and alternate)
    // in our lists of valid pattern names.
    for (int i = 0; i < Count; i++)
    {
        if (!strcmp(string,m_names[i]) ||
            !strcmp(string,m_alternate_names[i]))
        {
            value = (WT_Pattern_ID)i;
            return WT_Result::Success;
        }
    }

    // We used to return
    //     WT_Result::Corrupt_File_Error
    // when we didn't find a match. The result
    // was that programs like AutoSketch 6.0 that
    // implemented line patterns via an old spec
    // generated DWF files that WHIP! viewed as
    // corrupt. Now we just ignore the line pattern
    // and use solid.
    value = (WT_Pattern_ID)1;
    return WT_Result::Success;
}
