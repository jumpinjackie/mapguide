/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

// $Header: /a/CVSROOT/pathan/src/lexer/FlexLexer.hpp,v 1.3 2005/06/03 20:52:25 alby Exp $

// FlexLexer.h -- define interfaces for lexical analyzer classes generated
//		  by flex

// Copyright (c) 1993 The Regents of the University of California.
// All rights reserved.
//
// This code is derived from software contributed to Berkeley by
// Kent Williams and Tom Epperly.
//
// Redistribution and use in source and binary forms are permitted provided
// that: (1) source distributions retain this entire copyright notice and
// comment, and (2) distributions including binaries display the following
// acknowledgement:  ``This product includes software developed by the
// University of California, Berkeley and its contributors'' in the
// documentation or other materials provided with the distribution and in
// all advertising materials mentioning features or use of this software.
// Neither the name of the University nor the names of its contributors may
// be used to endorse or promote products derived from this software without
// specific prior written permission.
// THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

// This file defines FlexLexer, an abstract class which specifies the
// external interface provided to flex C++ lexer objects, and yyFlexLexer,
// which defines a particular lexer class.
//
// If you want to create multiple lexer classes, you use the -P flag
// to rename each yyFlexLexer to some other xxFlexLexer.  You then
// include <FlexLexer.h> in your other sources once per lexer class:
//
//	#undef yyFlexLexer
//	#define yyFlexLexer xxFlexLexer
//	#include <FlexLexer.h>
//
//	#undef yyFlexLexer
//	#define yyFlexLexer zzFlexLexer
//	#include <FlexLexer.h>
//	...
//
// Since this header is generic for all sizes of flex scanners, you must
// define the type YY_CHAR before including it:
//
//	typedef xxx YY_CHAR;
//	#include <FlexLexer.h>
//	...
//
// where xxx = char for 7-bit scanners, unsigned char for 8-bit, and
// wchar_t for 16-bit.

#ifndef __FLEX_LEXER_H
// Never included before - need to define base class.
#define __FLEX_LEXER_H

#include <pathan/Pathan.hpp>
#include <iostream>

#define YY_CHAR unsigned short

extern "C++" {

#include "LexerDOMStringStream.hpp"

struct yy_buffer_state;
typedef int yy_state_type;

class PATHAN_EXPORT FlexLexer {
public:
	virtual ~FlexLexer()	{ }

	const YY_CHAR* YYText()	{ return yytext; }
	int YYLeng()		{ return yyleng; }

	virtual void
		yy_switch_to_buffer( struct yy_buffer_state* new_buffer ) = 0;
	virtual struct yy_buffer_state*
		yy_create_buffer( LexerDOMStringStream* s, int size ) = 0;
	virtual void yy_delete_buffer( struct yy_buffer_state* b ) = 0;
	virtual void yyrestart( LexerDOMStringStream* s ) = 0;

	virtual int yylex() = 0;

	// Call yylex with new input/output sources.
	int yylex( LexerDOMStringStream* new_in, std::ostream* new_out = 0 )
		{
		switch_streams( new_in, new_out );
		return yylex();
		}

	// Switch to new input/output streams.  A nil stream pointer
	// indicates "keep the current one".
	virtual void switch_streams( LexerDOMStringStream* new_in = 0,
      std::ostream* new_out = 0 ) = 0;

	int lineno() const		{ return yylineno; }

	int debug() const		{ return yy_flex_debug; }
	void set_debug( int flag )	{ yy_flex_debug = flag; }

protected:
	YY_CHAR* yytext;
	int yyleng;
	int yylineno;		// only maintained if you use %option yylineno
	int yy_flex_debug;	// only has effect with -d or "%option debug"
};
}

#endif


