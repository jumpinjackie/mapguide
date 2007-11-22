/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: XQLexer.l,v 1.5 2006/11/01 16:37:21 jpcs Exp $
 */

%{

#include "XQLexer.hpp"
#include "../parser/XQParser.hpp"

#define YY_STACK_USED 1
#ifdef XQ_DEBUG_LEXER

#include <iostream>

#define CHANGE_STATE(state) BEGIN((state)); std::cerr << "Begin: " << xqGetStateName((state)) << ", "
#define RECOGNIZE(token)	{ std::cerr << "Recognize: "#token << std::endl; return token; }
// yy_push_state pushes the current state and does a transition to the specified one;
// the semantic of the PUSH/PUSH_CURRENT methods in the specs is that the specified state is pushed
#define PUSH(state)			{ int curState=YYSTATE; CHANGE_STATE(state); yy_push_state(curState); std::cerr << "Push: " << xqGetStateName((state)) << ", "; }
#define PUSH_CURRENT()		PUSH(YYSTATE)
#define POP()				yy_pop_state(); std::cerr << "Pop: " << xqGetStateName(YYSTATE) << ", "

#else

#define CHANGE_STATE(state) BEGIN((state))
#define RECOGNIZE(token)	return token
// yy_push_state pushes the current state and does a transition to the specified one;
// the semantic of the PUSH/PUSH_CURRENT methods in the specs is that the specified state is pushed
#define PUSH(state)			{ int curState=YYSTATE; CHANGE_STATE(state); yy_push_state(curState); }
#define PUSH_CURRENT()		PUSH(YYSTATE)
#define POP()				yy_pop_state()

#endif

static char szDelimiters[]       ="\t\n\r <>?*$)({/=!|@[],;%\"'&}:.-+";

#define REJECT_IF_NOT_TERMINATED    	{ if(yy_hold_char!=YY_END_OF_BUFFER_CHAR && strchr(szDelimiters,yy_hold_char)==NULL) { undoUserAction(); REJECT; } }
#define REJECT_IF_COLON			    	{ if(yy_hold_char!=YY_END_OF_BUFFER_CHAR && yy_hold_char==':') { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED1(state, tok1)    		{ if(!next_tokens(state, tok1)) { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED2(state, tok1, tok2)    	{ if(!next_tokens(state, tok1,tok2)) { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED3(state, tok1, tok2, tok3)    { if(!next_tokens(state, tok1, tok2, tok3)) { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED_CHOICE2(state, tok1, tok2)    	                                            { int tok=next_token(state); if(tok!=_EOF_ && tok!=tok1 && tok!=tok2) { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED_CHOICE3(state, tok1, tok2, tok3)                                             { int tok=next_token(state); if(tok!=_EOF_ && tok!=tok1 && tok!=tok2 && tok!=tok3) { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED_CHOICE4(state, tok1, tok2, tok3, tok4)                                       { int tok=next_token(state); if(tok!=_EOF_ && tok!=tok1 && tok!=tok2 && tok!=tok3 && tok!=tok4) { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED_CHOICE5(state, tok1, tok2, tok3, tok4, tok5)                                 { int tok=next_token(state); if(tok!=_EOF_ && tok!=tok1 && tok!=tok2 && tok!=tok3 && tok!=tok4 && tok!=tok5) { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED_CHOICE6(state, tok1, tok2, tok3, tok4, tok5, tok6)                           { int tok=next_token(state); if(tok!=_EOF_ && tok!=tok1 && tok!=tok2 && tok!=tok3 && tok!=tok4 && tok!=tok5 && tok!=tok6) { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED_CHOICE7(state, tok1, tok2, tok3, tok4, tok5, tok6, tok7)                     { int tok=next_token(state); if(tok!=_EOF_ && tok!=tok1 && tok!=tok2 && tok!=tok3 && tok!=tok4 && tok!=tok5 && tok!=tok6 && tok!=tok7) { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED_CHOICE8(state, tok1, tok2, tok3, tok4, tok5, tok6, tok7, tok8)               { int tok=next_token(state); if(tok!=_EOF_ && tok!=tok1 && tok!=tok2 && tok!=tok3 && tok!=tok4 && tok!=tok5 && tok!=tok6 && tok!=tok7 && tok!=tok8) { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED_CHOICE9(state, tok1, tok2, tok3, tok4, tok5, tok6, tok7, tok8, tok9)         { int tok=next_token(state); if(tok!=_EOF_ && tok!=tok1 && tok!=tok2 && tok!=tok3 && tok!=tok4 && tok!=tok5 && tok!=tok6 && tok!=tok7 && tok!=tok8 && tok!=tok9) { undoUserAction(); REJECT; } }
#define REJECT_IF_NOT_FOLLOWED_CHOICE10(state, tok1, tok2, tok3, tok4, tok5, tok6, tok7, tok8, tok9, tok10) { int tok=next_token(state); if(tok!=_EOF_ && tok!=tok1 && tok!=tok2 && tok!=tok3 && tok!=tok4 && tok!=tok5 && tok!=tok6 && tok!=tok7 && tok!=tok8 && tok!=tok9 && tok!=tok10) { undoUserAction(); REJECT; } }

#define REJECT_NOT_XQUERY { if(!isXQuery()) { undoUserAction(); REJECT; } }
#define REJECT_NOT_FULLTEXT { if(!isFullText()) { undoUserAction(); REJECT; } }
#define REJECT_XQUERY { if(isXQuery()) { undoUserAction(); REJECT; } }
#define REJECT_IF_XPATH { if(getLanguage() == XQilla::XPATH2) { undoUserAction(); REJECT; } }

%}

/* -------------- declaration section -------------- */

/* UTF16 Surrogates, for chars above 0xFFFF */
HighSurrogates ([\xD800-\xDB7F])
HighPrivateUseSurrogates ([\xDB80-\xDBFF])
LowSurrogates ([\xDC00-\xDFFF])
SurrogatePairs {HighSurrogates}|{HighPrivateUseSurrogates}|{LowSurrogates}
NonPrivateUseSurrogates {HighSurrogates}|{LowSurrogates}

/* from XML spec */
Char				([\x0009]|[\x000D]|[\x000A]|[\x0020-\xD7FF]|[\xE000-\xFFFD]|{SurrogatePairs})
WhitespaceChar		([\x0009]|[\x000D]|[\x000A]|[\x0020]) 
S					{WhitespaceChar}+
BaseChar1			([\x0041-\x005A]|[\x0061-\x007A]|[\x00C0-\x00D6]|[\x00D8-\x00F6]|[\x00F8-\x00FF]|[\x0100-\x0131]|[\x0134-\x013E]|[\x0141-\x0148]|[\x014A-\x017E]|[\x0180-\x01C3]|[\x01CD-\x01F0]|[\x01F4-\x01F5]|[\x01FA-\x0217]|[\x0250-\x02A8]|[\x02BB-\x02C1]|[\x0386]|[\x0388-\x038A]|[\x038C]|[\x038E-\x03A1]|[\x03A3-\x03CE]|[\x03D0-\x03D6]|[\x03DA]|[\x03DC]|[\x03DE]|[\x03E0]|[\x03E2-\x03F3]|[\x0401-\x040C]|[\x040E-\x044F]|[\x0451-\x045C]|[\x045E-\x0481]|[\x0490-\x04C4]|[\x04C7-\x04C8]|[\x04CB-\x04CC]|[\x04D0-\x04EB]|[\x04EE-\x04F5]|[\x04F8-\x04F9]|[\x0531-\x0556]|[\x0559]|[\x0561-\x0586]|[\x05D0-\x05EA]|[\x05F0-\x05F2]|[\x0621-\x063A]|[\x0641-\x064A]|[\x0671-\x06B7]|[\x06BA-\x06BE]|[\x06C0-\x06CE]|[\x06D0-\x06D3]|[\x06D5]|[\x06E5-\x06E6]|[\x0905-\x0939]|[\x093D]|[\x0958-\x0961]|[\x0985-\x098C]|[\x098F-\x0990]|[\x0993-\x09A8]|[\x09AA-\x09B0]|[\x09B2]|[\x09B6-\x09B9]|[\x09DC-\x09DD]|[\x09DF-\x09E1]|[\x09F0-\x09F1]|[\x0A05-\x0A0A]|[\x0A0F-\x0A10]|[\x0A13-\x0A28]|[\x0A2A-\x0A30]|[\x0A32-\x0A33]|[\x0A35-\x0A36]|[\x0A38-\x0A39]|[\x0A59-\x0A5C]|[\x0A5E]|[\x0A72-\x0A74]|[\x0A85-\x0A8B]|[\x0A8D]|[\x0A8F-\x0A91]|[\x0A93-\x0AA8]|[\x0AAA-\x0AB0]|[\x0AB2-\x0AB3]|[\x0AB5-\x0AB9]|[\x0ABD]|[\x0AE0]|[\x0B05-\x0B0C]|[\x0B0F-\x0B10]|[\x0B13-\x0B28]|[\x0B2A-\x0B30]|[\x0B32-\x0B33]|[\x0B36-\x0B39]|[\x0B3D]|[\x0B5C-\x0B5D]|[\x0B5F-\x0B61]|[\x0B85-\x0B8A]|[\x0B8E-\x0B90]|[\x0B92-\x0B95]|[\x0B99-\x0B9A]|[\x0B9C]|[\x0B9E-\x0B9F]|[\x0BA3-\x0BA4]|[\x0BA8-\x0BAA]|[\x0BAE-\x0BB5]|[\x0BB7-\x0BB9]|[\x0C05-\x0C0C]|[\x0C0E-\x0C10]|[\x0C12-\x0C28]|[\x0C2A-\x0C33]|[\x0C35-\x0C39]|[\x0C60-\x0C61]|[\x0C85-\x0C8C]|[\x0C8E-\x0C90]|[\x0C92-\x0CA8]|[\x0CAA-\x0CB3]|[\x0CB5-\x0CB9]|[\x0CDE]|[\x0CE0-\x0CE1]|[\x0D05-\x0D0C])
BaseChar2			([\x0D0E-\x0D10]|[\x0D12-\x0D28]|[\x0D2A-\x0D39]|[\x0D60-\x0D61]|[\x0E01-\x0E2E]|[\x0E30]|[\x0E32-\x0E33]|[\x0E40-\x0E45]|[\x0E81-\x0E82]|[\x0E84]|[\x0E87-\x0E88]|[\x0E8A]|[\x0E8D]|[\x0E94-\x0E97]|[\x0E99-\x0E9F]|[\x0EA1-\x0EA3]|[\x0EA5]|[\x0EA7]|[\x0EAA-\x0EAB]|[\x0EAD-\x0EAE]|[\x0EB0]|[\x0EB2-\x0EB3]|[\x0EBD]|[\x0EC0-\x0EC4]|[\x0F40-\x0F47]|[\x0F49-\x0F69]|[\x10A0-\x10C5]|[\x10D0-\x10F6]|[\x1100]|[\x1102-\x1103]|[\x1105-\x1107]|[\x1109]|[\x110B-\x110C]|[\x110E-\x1112]|[\x113C]|[\x113E]|[\x1140]|[\x114C]|[\x114E]|[\x1150]|[\x1154-\x1155]|[\x1159]|[\x115F-\x1161]|[\x1163]|[\x1165]|[\x1167]|[\x1169]|[\x116D-\x116E]|[\x1172-\x1173]|[\x1175]|[\x119E]|[\x11A8]|[\x11AB]|[\x11AE-\x11AF]|[\x11B7-\x11B8]|[\x11BA]|[\x11BC-\x11C2]|[\x11EB]|[\x11F0]|[\x11F9]|[\x1E00-\x1E9B]|[\x1EA0-\x1EF9]|[\x1F00-\x1F15]|[\x1F18-\x1F1D]|[\x1F20-\x1F45]|[\x1F48-\x1F4D]|[\x1F50-\x1F57]|[\x1F59]|[\x1F5B]|[\x1F5D]|[\x1F5F-\x1F7D]|[\x1F80-\x1FB4]|[\x1FB6-\x1FBC]|[\x1FBE]|[\x1FC2-\x1FC4]|[\x1FC6-\x1FCC]|[\x1FD0-\x1FD3]|[\x1FD6-\x1FDB]|[\x1FE0-\x1FEC]|[\x1FF2-\x1FF4]|[\x1FF6-\x1FFC]|[\x2126]|[\x212A-\x212B]|[\x212E]|[\x2180-\x2182]|[\x3041-\x3094]|[\x30A1-\x30FA]|[\x3105-\x312C]|[\xAC00-\xD7A3])
BaseChar			{BaseChar1}|{BaseChar2}
Ideographic			([\x4E00-\x9FA5]|[\x3007]|[\x3021-\x3029])
Digit				([\x0030-\x0039]|[\x0660-\x0669]|[\x06F0-\x06F9]|[\x0966-\x096F]|[\x09E6-\x09EF]|[\x0A66-\x0A6F]|[\x0AE6-\x0AEF]|[\x0B66-\x0B6F]|[\x0BE7-\x0BEF]|[\x0C66-\x0C6F]|[\x0CE6-\x0CEF]|[\x0D66-\x0D6F]|[\x0E50-\x0E59]|[\x0ED0-\x0ED9]|[\x0F20-\x0F29]) 
CombiningChar		([\x0300-\x0345]|[\x0360-\x0361]|[\x0483-\x0486]|[\x0591-\x05A1]|[\x05A3-\x05B9]|[\x05BB-\x05BD]|[\x05BF]|[\x05C1-\x05C2]|[\x05C4]|[\x064B-\x0652]|[\x0670]|[\x06D6-\x06DC]|[\x06DD-\x06DF]|[\x06E0-\x06E4]|[\x06E7-\x06E8]|[\x06EA-\x06ED]|[\x0901-\x0903]|[\x093C]|[\x093E-\x094C]|[\x094D]|[\x0951-\x0954]|[\x0962-\x0963]|[\x0981-\x0983]|[\x09BC]|[\x09BE]|[\x09BF]|[\x09C0-\x09C4]|[\x09C7-\x09C8]|[\x09CB-\x09CD]|[\x09D7]|[\x09E2-\x09E3]|[\x0A02]|[\x0A3C]|[\x0A3E]|[\x0A3F]|[\x0A40-\x0A42]|[\x0A47-\x0A48]|[\x0A4B-\x0A4D]|[\x0A70-\x0A71]|[\x0A81-\x0A83]|[\x0ABC]|[\x0ABE-\x0AC5]|[\x0AC7-\x0AC9]|[\x0ACB-\x0ACD]|[\x0B01-\x0B03]|[\x0B3C]|[\x0B3E-\x0B43]|[\x0B47-\x0B48]|[\x0B4B-\x0B4D]|[\x0B56-\x0B57]|[\x0B82-\x0B83]|[\x0BBE-\x0BC2]|[\x0BC6-\x0BC8]|[\x0BCA-\x0BCD]|[\x0BD7]|[\x0C01-\x0C03]|[\x0C3E-\x0C44]|[\x0C46-\x0C48]|[\x0C4A-\x0C4D]|[\x0C55-\x0C56]|[\x0C82-\x0C83]|[\x0CBE-\x0CC4]|[\x0CC6-\x0CC8]|[\x0CCA-\x0CCD]|[\x0CD5-\x0CD6]|[\x0D02-\x0D03]|[\x0D3E-\x0D43]|[\x0D46-\x0D48]|[\x0D4A-\x0D4D]|[\x0D57]|[\x0E31]|[\x0E34-\x0E3A]|[\x0E47-\x0E4E]|[\x0EB1]|[\x0EB4-\x0EB9]|[\x0EBB-\x0EBC]|[\x0EC8-\x0ECD]|[\x0F18-\x0F19]|[\x0F35]|[\x0F37]|[\x0F39]|[\x0F3E]|[\x0F3F]|[\x0F71-\x0F84]|[\x0F86-\x0F8B]|[\x0F90-\x0F95]|[\x0F97]|[\x0F99-\x0FAD]|[\x0FB1-\x0FB7]|[\x0FB9]|[\x20D0-\x20DC]|[\x20E1]|[\x302A-\x302F]|[\x3099]|[\x309A])
Extender			([\x00B7]|[\x02D0]|[\x02D1]|[\x0387]|[\x0640]|[\x0E46]|[\x0EC6]|[\x3005]|[\x3031-\x3035]|[\x309D-\x309E]|[\x30FC-\x30FE])
Letter				{BaseChar}|{Ideographic}
/* [20]    CData    ::=    (Char* - (Char* ']]>' Char*))  */
CharMinusBrack		([\x0009]|[\x000D]|[\x000A]|[\x0020-\x005C]|[\x005E-\xD7FF]|[\xE000-\xFFFD])
CharMinusGT			([\x0009]|[\x000D]|[\x000A]|[\x0020-\x003D]|[\x003F-\xD7FF]|[\xE000-\xFFFD])
CDATAContent		(({CharMinusBrack}|("]"{CharMinusBrack})|("]]"{CharMinusGT}))*)
/* [16]    PI    ::=    '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>' */
CharMinusQuestion	([\x0009]|[\x000D]|[\x000A]|[\x0020-\x003E]|[\x0040-\xD7FF]|[\xE000-\xFFFD])
PIContent			(({CharMinusQuestion}|("?"{CharMinusGT}))*)
/* [66]    PragmaContents	   ::=   	(Char* - (Char* '#)' Char*)) */
CharMinusSharp		([\x0009]|[\x000D]|[\x000A]|[\x0020-\x0022]|[\x0024-\xD7FF]|[\xE000-\xFFFD])
CharMinusClosePar	([\x0009]|[\x000D]|[\x000A]|[\x0020-\x0028]|[\x002A-\xD7FF]|[\xE000-\xFFFD])
PragmaContents		(({CharMinusSharp}|("#"{CharMinusClosePar}))*)

/* from Namespace spec */
NCNameChar			{Letter}|{Digit}|"."|"-"|"_"|{CombiningChar}|{Extender}
NCName				({Letter}|"_"){NCNameChar}*

Prefix				{NCName}
LocalPart			{NCName}
QName           	({Prefix}":")?{LocalPart}

/* from XQuery spec */
IntegerLiteral		{Digits}
DecimalLiteral		("."{Digits})|({Digits}"."[0-9]*)
DoubleLiteral		(("."{Digits})|({Digits}("."[0-9]*)?))("e"|"E")("+"|"-")?{Digits}
StringLiteral		(["]({PredefinedEntityRef}|{CharRef}|(["]["])|[^"&])*["])|([']({PredefinedEntityRef}|{CharRef}|([']['])|[^'&])*[']) 

Digits				[0-9]+
EscapeQuot			["]["]
PredefinedEntityRef	"&"("lt"|"gt"|"amp"|"quot"|"apos")";"
HexDigits			([0-9]|[a-f]|[A-F])+ 
CharRef				"&#"({Digits}|("x"{HexDigits}))";"
EscapeApos			"''"
PITarget			{NCName}

/* CommentChar := {Char} - "-" */
CommentChar			([\x0009]|[\x000D]|[\x000A]|[\x0020-\x002C]|[\x002E-\xD7FF]|[\xE000-\xFFFD])
CommentContent		(({CommentChar}|("-"{CommentChar}))*)

NotColon			[^:]

%option never-interactive
%option noyywrap
%option yyclass="XQLexer"
%option c++

%x ELEMENT_CONTENT
%x PROCESSING_INSTRUCTION
%x PROCESSING_INSTRUCTION_CONTENT 
%x QUOT_ATTRIBUTE_CONTENT
%x APOS_ATTRIBUTE_CONTENT
%x START_TAG
%x END_TAG
%x OPERATOR
%x ITEMTYPE
%x NAMESPACEDECL
%x NAMESPACEKEYWORD
%x BOUNDARYSPACE_DECL
%x COPYNAMESPACE_DECL
%x VARNAME
%x ATTRIBUTE_DECL
%x KINDTEST
%x KINDTESTFORPI
%x CLOSEKINDTEST
%x EXPR_COMMENT
%x OCCURRENCEINDICATOR
%x SINGLETYPE
%x PRAGMA_NAME
%x PRAGMA_CONTENT
%x XQUERYDECL
%x IMPORT
%x DEFAULT_KEYWORD
%x DECLARE_KEYWORD
%x DEFAULT_ORDER
%x VALIDATE
%x ORDER
%x EMPTY_PARENS
%x THESAURUS1
%x THESAURUS2
%x STOPWORDS

%{
char *xqGetStateName(int state)
{
  switch(state) {
  case INITIAL: return "INITIAL";
  case ELEMENT_CONTENT: return "ELEMENT_CONTENT";
  case PROCESSING_INSTRUCTION: return "PROCESSING_INSTRUCTION";
  case PROCESSING_INSTRUCTION_CONTENT: return "PROCESSING_INSTRUCTION_CONTENT";
  case QUOT_ATTRIBUTE_CONTENT: return "QUOT_ATTRIBUTE_CONTENT";
  case APOS_ATTRIBUTE_CONTENT: return "APOS_ATTRIBUTE_CONTENT";
  case START_TAG: return "START_TAG";
  case END_TAG: return "END_TAG";
  case OPERATOR: return "OPERATOR";
  case ITEMTYPE: return "ITEMTYPE";
  case NAMESPACEDECL: return "NAMESPACEDECL";
  case NAMESPACEKEYWORD: return "NAMESPACEKEYWORD";
  case BOUNDARYSPACE_DECL: return "BOUNDARYSPACE_DECL";
  case COPYNAMESPACE_DECL: return "COPYNAMESPACE_DECL";
  case VARNAME: return "VARNAME";
  case ATTRIBUTE_DECL: return "ATTRIBUTE_DECL";
  case KINDTEST: return "KINDTEST";
  case KINDTESTFORPI: return "KINDTESTFORPI";
  case CLOSEKINDTEST: return "CLOSEKINDTEST";
  case EXPR_COMMENT: return "EXPR_COMMENT";
  case OCCURRENCEINDICATOR: return "OCCURRENCEINDICATOR";
  case SINGLETYPE: return "SINGLETYPE";
  case PRAGMA_NAME: return "PRAGMA_NAME";
  case PRAGMA_CONTENT: return "PRAGMA_CONTENT";
  case XQUERYDECL: return "XQUERYDECL";
  case IMPORT: return "IMPORT";
  case DEFAULT_KEYWORD: return "DEFAULT_KEYWORD";
  case DECLARE_KEYWORD: return "DECLARE_KEYWORD";
  case DEFAULT_ORDER: return "DEFAULT_ORDER";
  case VALIDATE: return "VALIDATE";
  case ORDER: return "ORDER";
  case EMPTY_PARENS: return "EMPTY_PARENS";
  case THESAURUS1: return "THESAURUS1";
  case THESAURUS2: return "THESAURUS2";
  case STOPWORDS: return "STOPWORDS";
  }
  return "unknown";
}
%}

%%

<INITIAL,OPERATOR,BOUNDARYSPACE_DECL,COPYNAMESPACE_DECL,XQUERYDECL,NAMESPACEDECL,NAMESPACEKEYWORD,KINDTEST,KINDTESTFORPI,CLOSEKINDTEST,ITEMTYPE,SINGLETYPE,ATTRIBUTE_DECL,IMPORT,DECLARE_KEYWORD,DEFAULT_KEYWORD,DEFAULT_ORDER,VALIDATE,ORDER,EMPTY_PARENS,OCCURRENCEINDICATOR,THESAURUS1,THESAURUS2,STOPWORDS>{S}		if(m_bReportWhiteSpace) RECOGNIZE(_XQUERY_WHITESPACE_);
<INITIAL,OPERATOR,XQUERYDECL,NAMESPACEDECL,NAMESPACEKEYWORD,BOUNDARYSPACE_DECL,COPYNAMESPACE_DECL,SINGLETYPE,ITEMTYPE,KINDTEST,KINDTESTFORPI,CLOSEKINDTEST,OCCURRENCEINDICATOR,VARNAME,EXPR_COMMENT,IMPORT,DECLARE_KEYWORD,DEFAULT_KEYWORD,DEFAULT_ORDER,VALIDATE,ORDER,EMPTY_PARENS,THESAURUS1,THESAURUS2,STOPWORDS>"(:"						beginComment(); PUSH_CURRENT(); BEGIN(EXPR_COMMENT); if(m_bReportWhiteSpace) RECOGNIZE(_XQUERY_COMMENT_);

<INITIAL>{DecimalLiteral}									REJECT_IF_NOT_TERMINATED; yylval.str=allocate_string((XMLCh*)yytext); CHANGE_STATE(OPERATOR); RECOGNIZE(_DECIMAL_NUMBER_);
<INITIAL>{DoubleLiteral}									REJECT_IF_NOT_TERMINATED; yylval.str=allocate_string((XMLCh*)yytext); CHANGE_STATE(OPERATOR); RECOGNIZE(_DOUBLE_NUMBER_);
<INITIAL>{IntegerLiteral}									REJECT_IF_NOT_TERMINATED; yylval.str=allocate_string((XMLCh*)yytext); CHANGE_STATE(OPERATOR); RECOGNIZE(_INTEGER_NUMBER_);
<INITIAL>"."												CHANGE_STATE(OPERATOR); RECOGNIZE(_DOT_);
<INITIAL>".."												CHANGE_STATE(OPERATOR); RECOGNIZE(_DOT_DOT_);
<INITIAL>{NCName}":*"										{
																yylval.str=allocate_string((XMLCh*)yytext,XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((XMLCh*)yytext)-2);
																CHANGE_STATE(OPERATOR);
																RECOGNIZE(_NCNAME_COLON_STAR_);
															}
<INITIAL,OPERATOR,KINDTEST,KINDTESTFORPI,CLOSEKINDTEST,THESAURUS2,STOPWORDS>")"	{
																switch(YYSTATE)
																{
																case THESAURUS2:
																case INITIAL:
																case OPERATOR:		CHANGE_STATE(OPERATOR); break;
																case KINDTEST:
																case KINDTESTFORPI:
																case CLOSEKINDTEST:	POP(); break;
																case STOPWORDS:	break;
																}
																RECOGNIZE(_RPAR_);
															}
<INITIAL>"*:"{NCName}										{
																yylval.str=allocate_string((XMLCh*)(yytext+2));	// skip the *:
																CHANGE_STATE(OPERATOR);
																RECOGNIZE(_STAR_COLON_NCNAME_);
															}
<INITIAL,OPERATOR,OCCURRENCEINDICATOR,KINDTEST>"*"			{
																switch(YYSTATE)
																{
																case INITIAL:				CHANGE_STATE(OPERATOR); RECOGNIZE(_STAR_);
																case OCCURRENCEINDICATOR:	CHANGE_STATE(OPERATOR); RECOGNIZE(_ZERO_OR_MORE_);
																case OPERATOR:				CHANGE_STATE(INITIAL); RECOGNIZE(_MULTIPLY_);
																case KINDTEST:				CHANGE_STATE(CLOSEKINDTEST); RECOGNIZE(_STAR_);
																}
															}
<INITIAL,OPERATOR,XQUERYDECL,NAMESPACEDECL,NAMESPACEKEYWORD,KINDTESTFORPI,THESAURUS1,THESAURUS2,STOPWORDS>{StringLiteral}	{
																							yylval.str=allocate_string_and_unescape(*yytext,(XMLCh*)(yytext+1),XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((XMLCh*)yytext)-2);
																							switch(YYSTATE)
																							{
																							case INITIAL:
																							case OPERATOR:			CHANGE_STATE(OPERATOR); break;
																							case STOPWORDS:
																							case THESAURUS1:
																							case THESAURUS2:
																							case KINDTESTFORPI:
																							case XQUERYDECL: break;
																							case NAMESPACEDECL:
																							case NAMESPACEKEYWORD:	CHANGE_STATE(INITIAL); break;
																							}
																							RECOGNIZE(_STRING_LITERAL_);
																						}

<INITIAL>"declare"											REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED_CHOICE10(DECLARE_KEYWORD, _OPTION_, _VARIABLE_KEYWORD_, _FUNCTION_, _CONSTRUCTION_, _ORDERING_, _DEFAULT_, _COPY_NAMESPACES_, _BASEURI_, _BOUNDARYSPACE_, _NAMESPACE_); CHANGE_STATE(DECLARE_KEYWORD); RECOGNIZE(_DECLARE_);
<DECLARE_KEYWORD>"option"                                   REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_OPTION_);
<DECLARE_KEYWORD>"variable"                                 REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_VARIABLE_KEYWORD_);
<DECLARE_KEYWORD>"function"                                 REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_FUNCTION_);
<DECLARE_KEYWORD>"construction"                             REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_CONSTRUCTION_);
<DECLARE_KEYWORD>"ordering"                                 REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_ORDERING_);
<DECLARE_KEYWORD>"copy-namespaces"							REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(COPYNAMESPACE_DECL); RECOGNIZE(_COPY_NAMESPACES_);
<DECLARE_KEYWORD>"base-uri"									REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(NAMESPACEDECL); RECOGNIZE(_BASEURI_);
<DECLARE_KEYWORD>"boundary-space"							REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(BOUNDARYSPACE_DECL); RECOGNIZE(_BOUNDARYSPACE_);
<DECLARE_KEYWORD>"namespace"								REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(NAMESPACEDECL); RECOGNIZE(_NAMESPACE_);

<DECLARE_KEYWORD,NAMESPACEKEYWORD>"default"          	    {
                                                                REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; 
																switch(YYSTATE)
																{
																case DECLARE_KEYWORD:		REJECT_IF_NOT_FOLLOWED_CHOICE4(DEFAULT_KEYWORD, _ORDER_, _COLLATION_, _ELEMENT_, _FUNCTION_); break;
																case NAMESPACEKEYWORD:	    REJECT_IF_NOT_FOLLOWED1(DEFAULT_KEYWORD, _ELEMENT_); break;
                                                                }
                                                                CHANGE_STATE(DEFAULT_KEYWORD); 
                                                                RECOGNIZE(_DEFAULT_);
                                                            }

<DEFAULT_KEYWORD>"order"                                    REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_ORDER_);
<DEFAULT_KEYWORD>"collation"                                REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(NAMESPACEDECL); RECOGNIZE(_COLLATION_);
<DEFAULT_KEYWORD>"element"                                  REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(NAMESPACEKEYWORD); RECOGNIZE(_ELEMENT_);
<DEFAULT_KEYWORD>"function"                                 REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(NAMESPACEKEYWORD); RECOGNIZE(_FUNCTION_);

<OPERATOR>"empty"                                           REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED_CHOICE2(DEFAULT_ORDER, _GREATEST_, _LEAST_); CHANGE_STATE(DEFAULT_ORDER); RECOGNIZE(_EMPTY_KEYWORD_);
<DEFAULT_ORDER>"greatest"								    REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_GREATEST_);
<DEFAULT_ORDER>"least"									    REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_LEAST_);

<INITIAL>"module"           								REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(NAMESPACEKEYWORD, _NAMESPACE_); CHANGE_STATE(NAMESPACEKEYWORD); RECOGNIZE(_MODULE_);
<INITIAL>"import"								            REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED_CHOICE2(IMPORT, _SCHEMA_, _MODULE_); CHANGE_STATE(IMPORT); RECOGNIZE(_IMPORT_);

<IMPORT>"schema"                                            REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(NAMESPACEKEYWORD); RECOGNIZE(_SCHEMA_);
<IMPORT>"module"                                            REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(NAMESPACEKEYWORD); RECOGNIZE(_MODULE_);

<INITIAL,OPERATOR,ITEMTYPE>"$"								CHANGE_STATE(VARNAME); RECOGNIZE(_DOLLAR_SIGN_);
<INITIAL,OPERATOR>"for"										REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(YYSTATE, _DOLLAR_SIGN_); RECOGNIZE(_FOR_);
<INITIAL,OPERATOR>"let"										REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(YYSTATE, _DOLLAR_SIGN_); RECOGNIZE(_LET_);
<INITIAL,OPERATOR>"score"										REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(YYSTATE, _DOLLAR_SIGN_); RECOGNIZE(_SCORE_);
<INITIAL>"some"												REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(YYSTATE, _DOLLAR_SIGN_); RECOGNIZE(_SOME_);
<INITIAL>"every"											REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(YYSTATE, _DOLLAR_SIGN_); RECOGNIZE(_EVERY_);

<INITIAL,OPERATOR,ITEMTYPE>")"{WhitespaceChar}*"as"			REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(ITEMTYPE); RECOGNIZE(_EXPR_AS_);

<INITIAL,ITEMTYPE,KINDTEST>"element"{WhitespaceChar}*"("	{
																REJECT_IF_COLON;
																switch(YYSTATE)
																{
																case INITIAL:  PUSH(OPERATOR); break;
																case ITEMTYPE: PUSH(OCCURRENCEINDICATOR); break;
																case KINDTEST: PUSH(KINDTEST); break;
																}
																CHANGE_STATE(KINDTEST); 
																RECOGNIZE(_ELEMENT_LPAR_);
															}
<INITIAL,ITEMTYPE>"attribute"{WhitespaceChar}*"("			{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																CHANGE_STATE(KINDTEST);
																RECOGNIZE(_ATTRIBUTE_LPAR_);
															}
<INITIAL,ITEMTYPE,KINDTEST>"schema-element"{WhitespaceChar}*"("	{
																	REJECT_IF_COLON;
																	switch(YYSTATE)
																	{
																	case INITIAL:  PUSH(OPERATOR); break;
																	case ITEMTYPE: PUSH(OCCURRENCEINDICATOR); break;
																	case KINDTEST: PUSH(KINDTEST); break;
																	}
																	CHANGE_STATE(KINDTEST); 
																	RECOGNIZE(_SCHEMA_ELEMENT_LPAR_);
																}
<INITIAL,ITEMTYPE>"schema-attribute"{WhitespaceChar}*"("	{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																CHANGE_STATE(KINDTEST);
																RECOGNIZE(_SCHEMA_ATTRIBUTE_LPAR_);
															}
<INITIAL,ITEMTYPE>"comment"{WhitespaceChar}*"("				{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																CHANGE_STATE(KINDTEST);
																RECOGNIZE(_COMMENT_LPAR_);
															}
<INITIAL,ITEMTYPE>"text"{WhitespaceChar}*"("				{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																CHANGE_STATE(KINDTEST);
																RECOGNIZE(_TEXT_LPAR_);
															}
<INITIAL,ITEMTYPE>"node"{WhitespaceChar}*"("				{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																CHANGE_STATE(KINDTEST);
																RECOGNIZE(_NODE_LPAR_);
															}
<INITIAL,ITEMTYPE>"document-node"{WhitespaceChar}*"("		{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																CHANGE_STATE(KINDTEST);
																RECOGNIZE(_DOCUMENT_NODE_LPAR_);
															}
<INITIAL,ITEMTYPE>"processing-instruction"{WhitespaceChar}*"("	{
																	REJECT_IF_COLON;
																	int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																	PUSH(stateToPush);
																	CHANGE_STATE(KINDTESTFORPI);
																	RECOGNIZE(_PROCESSING_INSTRUCTION_LPAR_);
																}

<INITIAL,ELEMENT_CONTENT>"<!--"{CommentContent}"-->"		{
																REJECT_NOT_XQUERY;
																yylval.str=allocate_string((XMLCh*)(yytext+4),XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((XMLCh*)(yytext+4))-3); 
																if(YYSTATE==INITIAL)
																	CHANGE_STATE(OPERATOR);
																RECOGNIZE(_XML_COMMENT_);
															}

<INITIAL,ELEMENT_CONTENT>"<?"								{
																REJECT_NOT_XQUERY;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:YYSTATE;
																PUSH(stateToPush);
																CHANGE_STATE(PROCESSING_INSTRUCTION); 
																RECOGNIZE(_PROCESSING_INSTRUCTION_START_);
															}

<INITIAL,ELEMENT_CONTENT>"<![CDATA["{CDATAContent}"]]>"		{
																REJECT_NOT_XQUERY;
																yylval.str=allocate_string((XMLCh*)(yytext+9),XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((XMLCh*)(yytext+9))-3); 
																if(YYSTATE==INITIAL)
																	CHANGE_STATE(OPERATOR);
																RECOGNIZE(_CDATA_SECTION_);
															}

<INITIAL,OPERATOR,ITEMTYPE,ELEMENT_CONTENT>"<"				{
																switch(YYSTATE)
																{
																case INITIAL:
																	REJECT_NOT_XQUERY; PUSH(OPERATOR); CHANGE_STATE(START_TAG); RECOGNIZE(_START_TAG_OPEN_);
																case OPERATOR:
																case ITEMTYPE:
																	CHANGE_STATE(INITIAL); RECOGNIZE(_LT_);
																case ELEMENT_CONTENT:
																	PUSH_CURRENT(); CHANGE_STATE(START_TAG); RECOGNIZE(_START_TAG_OPEN_);
																}
															}

<INITIAL,OPERATOR>"}"										POP(); RECOGNIZE(_RBRACE_);

<INITIAL>"validate"											REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED_CHOICE3(VALIDATE, _LBRACE_, _MODE_LAX_, _MODE_STRICT_); CHANGE_STATE(VALIDATE); RECOGNIZE(_VALIDATE_);
<VALIDATE>"{"									            REJECT_NOT_XQUERY; PUSH(OPERATOR); CHANGE_STATE(INITIAL); RECOGNIZE(_LBRACE_);
<VALIDATE>"lax"									            REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(VALIDATE, _LBRACE_); RECOGNIZE(_MODE_LAX_);
<VALIDATE>"strict"								            REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(VALIDATE, _LBRACE_); RECOGNIZE(_MODE_STRICT_);

<INITIAL>"typeswitch"										REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(YYSTATE, _LPAR_); RECOGNIZE(_TYPESWITCH_);

<INITIAL>"element"/{WhitespaceChar}*"{"						REJECT_NOT_XQUERY; RECOGNIZE(_ELEMENT_CONSTR_);
<INITIAL>"attribute"/{WhitespaceChar}*"{"					REJECT_NOT_XQUERY; RECOGNIZE(_ATTRIBUTE_CONSTR_);

<INITIAL>"attribute"{S}{QName}/{WhitespaceChar}*"{"			{
																REJECT_NOT_XQUERY;
																XMLCh* beginning=(XMLCh*)(yytext+9);
																while(iswspace(*beginning)) beginning++;
																yylval.str=allocate_string(beginning); 
																RECOGNIZE(_NAMED_ATTRIBUTE_CONSTR_);
															}
<INITIAL>"element"{S}{QName}/{WhitespaceChar}*"{"			{
																REJECT_NOT_XQUERY;
																XMLCh* beginning=(XMLCh*)(yytext+7);
																while(iswspace(*beginning)) beginning++;
																yylval.str=allocate_string(beginning); 
																RECOGNIZE(_NAMED_ELEMENT_CONSTR_);
															}
<INITIAL>"document"/{WhitespaceChar}*"{"					REJECT_NOT_XQUERY; RECOGNIZE(_DOCUMENT_CONSTR_);
<INITIAL>"text"/{WhitespaceChar}*"{"						REJECT_NOT_XQUERY; RECOGNIZE(_TEXT_CONSTR_);
<INITIAL>"processing-instruction"/{WhitespaceChar}*"{"		REJECT_NOT_XQUERY; RECOGNIZE(_PROCESSING_INSTRUCTION_CONSTR_);
<INITIAL>"processing-instruction"{S}{NCName}/{WhitespaceChar}*"{"	{
    																REJECT_NOT_XQUERY;
																		XMLCh* beginning=(XMLCh*)(yytext+22);
																		while(iswspace(*beginning)) beginning++;
																		yylval.str=allocate_string(beginning); 
																		RECOGNIZE(_NAMED_PROCESSING_INSTRUCTION_CONSTR_);
																	}
<INITIAL>"comment"/{WhitespaceChar}*"{"						REJECT_NOT_XQUERY; RECOGNIZE(_COMMENT_CONSTR_);

<INITIAL>"(#"{WhitespaceChar}*				 REJECT_NOT_XQUERY; BEGIN(PRAGMA_NAME); RECOGNIZE(_PRAGMA_OPEN_);

<PRAGMA_NAME>{QName}                   REJECT_NOT_XQUERY; yylval.str=allocate_string((XMLCh*)yytext); CHANGE_STATE(PRAGMA_CONTENT); RECOGNIZE(_PRAGMA_NAME_);
<PRAGMA_CONTENT>{S}{PragmaContents}    REJECT_NOT_XQUERY; RECOGNIZE(_PRAGMA_CONTENT_);
<PRAGMA_CONTENT>"#)"									 REJECT_NOT_XQUERY; CHANGE_STATE(INITIAL); RECOGNIZE(_PRAGMA_CLOSE_);


<INITIAL,OPERATOR,ITEMTYPE,XQUERYDECL>";"					REJECT_NOT_XQUERY; CHANGE_STATE(INITIAL); RECOGNIZE(_SEMICOLON_);

<INITIAL,OPERATOR,KINDTEST,CLOSEKINDTEST,ELEMENT_CONTENT,QUOT_ATTRIBUTE_CONTENT,APOS_ATTRIBUTE_CONTENT>"{"	{
																switch(YYSTATE)
																{
																case ELEMENT_CONTENT:
																case QUOT_ATTRIBUTE_CONTENT:
																case APOS_ATTRIBUTE_CONTENT:	PUSH_CURRENT(); CHANGE_STATE(INITIAL); break;
																case INITIAL:
																case OPERATOR:
																case KINDTEST:
																case CLOSEKINDTEST:				PUSH(OPERATOR); CHANGE_STATE(INITIAL); break;
																}
																RECOGNIZE(_LBRACE_);
															}

<INITIAL>"ordered"/{WhitespaceChar}*"{"						REJECT_NOT_XQUERY; PUSH(OPERATOR); CHANGE_STATE(INITIAL); RECOGNIZE(_ORDERED_);
<INITIAL>"unordered"/{WhitespaceChar}*"{"					REJECT_NOT_XQUERY; PUSH(OPERATOR); CHANGE_STATE(INITIAL); RECOGNIZE(_UNORDERED_);

<INITIAL>"xquery"											REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(XQUERYDECL, _VERSION_); CHANGE_STATE(XQUERYDECL); RECOGNIZE(_XQUERY_);
<XQUERYDECL>"version"										REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; RECOGNIZE(_VERSION_);

<XQUERYDECL>"encoding"											REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; RECOGNIZE(_ENCODING_);

<INITIAL>"@"												RECOGNIZE(_AT_);

<INITIAL>"ancestor-or-self"{WhitespaceChar}*"::"			RECOGNIZE(_AXIS_ANCESTOR_OR_SELF_);
<INITIAL>"ancestor"{WhitespaceChar}*"::"					RECOGNIZE(_AXIS_ANCESTOR_);
<INITIAL>"attribute"{WhitespaceChar}*"::"					RECOGNIZE(_AXIS_ATTRIBUTE_);
<INITIAL>"child"{WhitespaceChar}*"::"						RECOGNIZE(_AXIS_CHILD_);
<INITIAL>"descendant-or-self"{WhitespaceChar}*"::"			RECOGNIZE(_AXIS_DESCENDANT_OR_SELF_);
<INITIAL>"descendant"{WhitespaceChar}*"::"					RECOGNIZE(_AXIS_DESCENDANT_);
<INITIAL>"following-sibling"{WhitespaceChar}*"::"			RECOGNIZE(_AXIS_FOLLOWING_SIBLING_);
<INITIAL>"following"{WhitespaceChar}*"::"					RECOGNIZE(_AXIS_FOLLOWING_);
<INITIAL>"parent"{WhitespaceChar}*"::"						RECOGNIZE(_AXIS_PARENT_);
<INITIAL>"preceding-sibling"{WhitespaceChar}*"::"			RECOGNIZE(_AXIS_PRECEDING_SIBLING_);
<INITIAL>"preceding"{WhitespaceChar}*"::"					RECOGNIZE(_AXIS_PRECEDING_);
<INITIAL>"self"{WhitespaceChar}*"::"						RECOGNIZE(_AXIS_SELF_);
<INITIAL>"namespace"{WhitespaceChar}*"::"					REJECT_XQUERY; RECOGNIZE(_AXIS_NAMESPACE_);

<INITIAL,OPERATOR,ITEMTYPE,CLOSEKINDTEST,THESAURUS2,STOPWORDS>","				{
																switch(YYSTATE) {
																case CLOSEKINDTEST: CHANGE_STATE(KINDTEST); break;
																case THESAURUS2:
																case STOPWORDS: break;
																default: CHANGE_STATE(INITIAL); break;
																}
																RECOGNIZE(_COMMA_);
															}
<INITIAL>"if"												REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(YYSTATE, _LPAR_);RECOGNIZE(_IF_);
<INITIAL,ITEMTYPE,THESAURUS1,STOPWORDS>"("										{
                     switch(YYSTATE) {
                     case INITIAL:
                     case ITEMTYPE: CHANGE_STATE(INITIAL); break;
                     case THESAURUS1: CHANGE_STATE(THESAURUS2); break;
                     case STOPWORDS: break;
                     }
                     RECOGNIZE(_LPAR_);
                   }
<INITIAL,OPERATOR,ITEMTYPE>"-"								CHANGE_STATE(INITIAL); RECOGNIZE(_MINUS_);
<INITIAL,OPERATOR,ITEMTYPE,THESAURUS1,THESAURUS2,STOPWORDS>"at"	{
							REJECT_IF_XPATH;
							REJECT_IF_NOT_TERMINATED;
              switch(YYSTATE) {
              case INITIAL:
              case OPERATOR:
              case ITEMTYPE: CHANGE_STATE(INITIAL); break;
              case THESAURUS1: CHANGE_STATE(OPERATOR); break;
              case STOPWORDS:
              case THESAURUS2: break;
              }
              RECOGNIZE(_AT_KEYWORD_);
            }
<INITIAL,OPERATOR,OCCURRENCEINDICATOR>"+"					{
																if(YYSTATE==OCCURRENCEINDICATOR)
																{
																	CHANGE_STATE(OPERATOR); RECOGNIZE(_ONE_OR_MORE_);
																}
																else
																{
																	CHANGE_STATE(INITIAL); RECOGNIZE(_PLUS_);
																}
															}
<INITIAL,OPERATOR>"//"										CHANGE_STATE(INITIAL); RECOGNIZE(_SLASHSLASH_);
<INITIAL,OPERATOR>"/"										CHANGE_STATE(INITIAL); RECOGNIZE(_SLASH_);

<OPERATOR,ITEMTYPE>"then"									REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_THEN_);
<OPERATOR,ITEMTYPE>"else"									REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_ELSE_);
<OPERATOR,ITEMTYPE>"external"								REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_EXTERNAL_);
<OPERATOR,ITEMTYPE>"and"									REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_AND_);
<OPERATOR,ITEMTYPE>":="										REJECT_NOT_XQUERY; CHANGE_STATE(INITIAL); RECOGNIZE(_COLON_EQ_);
<OPERATOR,ITEMTYPE>"div"									REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_DIV_);
<OPERATOR,ITEMTYPE>"="										CHANGE_STATE(INITIAL); RECOGNIZE(_EQUALS_);
<OPERATOR,ITEMTYPE,STOPWORDS>"except" {
									REJECT_IF_NOT_TERMINATED;
									switch(YYSTATE) {
									case OPERATOR:
									case ITEMTYPE: CHANGE_STATE(INITIAL); break;
									case STOPWORDS: break;
									}
									RECOGNIZE(_EXCEPT_);
								}
<OPERATOR,ITEMTYPE>"eq"										REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_FORTRAN_EQ_);
<OPERATOR,ITEMTYPE>"ge"										REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_FORTRAN_GE_);
<OPERATOR,ITEMTYPE>"gt"										REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_FORTRAN_GT_);
<OPERATOR,ITEMTYPE>"le"										REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_FORTRAN_LE_);
<OPERATOR,ITEMTYPE>"lt"										REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_FORTRAN_LT_);
<OPERATOR,ITEMTYPE>"ne"										REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_FORTRAN_NE_);
<OPERATOR,ITEMTYPE>">="										CHANGE_STATE(INITIAL); RECOGNIZE(_GT_EQUALS_);
<OPERATOR,ITEMTYPE>">>"										CHANGE_STATE(INITIAL); RECOGNIZE(_GT_GT_);
<OPERATOR,ITEMTYPE>">"										CHANGE_STATE(INITIAL); RECOGNIZE(_GT_);
<OPERATOR,ITEMTYPE>"idiv"									REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_INTEGER_DIV_);
<OPERATOR,ITEMTYPE>"intersect"								REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_INTERSECT_);
<OPERATOR,ITEMTYPE>"in"										REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_IN_);
<OPERATOR,ITEMTYPE>"is"										REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_IS_);
<OPERATOR,ITEMTYPE>"["										CHANGE_STATE(INITIAL); RECOGNIZE(_LBRACK_);
<OPERATOR,ITEMTYPE>"<="										CHANGE_STATE(INITIAL); RECOGNIZE(_LT_EQUALS_);
<OPERATOR,ITEMTYPE>"<<"										CHANGE_STATE(INITIAL); RECOGNIZE(_LT_LT_);
<OPERATOR,ITEMTYPE>"mod"									REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_MOD_);
<OPERATOR,ITEMTYPE>"!="										CHANGE_STATE(INITIAL); RECOGNIZE(_NOT_EQUALS_);
<OPERATOR,ITEMTYPE>"order"      					REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(ORDER, _BY_); CHANGE_STATE(ORDER); RECOGNIZE(_ORDER_);
<OPERATOR,ITEMTYPE>"stable"               REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(YYSTATE, _ORDER_); RECOGNIZE(_STABLE_);
<ORDER>"by"				                        REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_BY_);
<OPERATOR,ITEMTYPE>"or"										REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_OR_);
<OPERATOR,ITEMTYPE>"return"									REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_RETURN_);
<OPERATOR,ITEMTYPE>"satisfies"								REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_SATISFIES_);
<OPERATOR,ITEMTYPE>"to"										REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_TO_);

<OPERATOR,ITEMTYPE>"ftcontains"						REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_FTCONTAINS_);
<OPERATOR,ITEMTYPE>"||"										REJECT_NOT_FULLTEXT; CHANGE_STATE(INITIAL); RECOGNIZE(_DOUBLE_VERTICAL_BAR_);
<OPERATOR,ITEMTYPE>"&&"										REJECT_NOT_FULLTEXT; CHANGE_STATE(INITIAL); RECOGNIZE(_DOUBLE_AMPERSAND_);
<OPERATOR,ITEMTYPE>"not"{S}"in"						REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_NOT_IN_);
<INITIAL>"!"                  						REJECT_NOT_FULLTEXT; CHANGE_STATE(INITIAL); RECOGNIZE(_BANG_);
<OPERATOR>"lowercase"											REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_LOWERCASE_);
<OPERATOR>"uppercase"											REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_UPPERCASE_);
<OPERATOR>"case"{S}"sensitive"						REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_CASE_SENSITIVE_);
<OPERATOR>"case"{S}"insensitive"					REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_CASE_INSENSITIVE_);
<OPERATOR>"with"{S}"diacritics" 					REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_WITH_DIACRITICS_);
<OPERATOR>"without"{S}"diacritics"				REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_WITHOUT_DIACRITICS_);
<OPERATOR>"diacritics"{S}"sensitive"			REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_DIACRITICS_SENSITIVE_);
<OPERATOR>"diacritics"{S}"insensitive"		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_DIACRITICS_INSENSITIVE_);
<OPERATOR>"with"{S}"stemming"         		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_WITH_STEMMING_);
<OPERATOR>"without"{S}"stemming"       		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_WITHOUT_STEMMING_);
<OPERATOR>"language"                  		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_LANGUAGE_);
<OPERATOR>"with"{S}"wildcards"         		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_WITH_WILDCARDS_);
<OPERATOR>"without"{S}"wildcards"      		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_WITHOUT_WILDCARDS_);
<OPERATOR>"at"{S}"start"              		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_AT_START_);
<OPERATOR>"at"{S}"end"                		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_AT_END_);
<OPERATOR>"entire"{S}"content"         		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_ENTIRE_CONTENT_);
<OPERATOR>"any"{S}"word"              		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_ANY_WORD_);
<OPERATOR>"any"                       		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_ANY_);
<OPERATOR>"all"{S}"words"              		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_ALL_WORDS_);
<OPERATOR>"all"                       		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_ALL_);
<OPERATOR>"phrase"                     		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_PHRASE_);
<OPERATOR>"distance"                   		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_DISTANCE_);
<OPERATOR>"window"                    		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_WINDOW_);
<OPERATOR>"occurs"                    		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_OCCURS_);
<OPERATOR>"same"                      		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_SAME_);
<OPERATOR>"different"                  		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_DIFFERENT_);
<OPERATOR>"sentence"                  		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_SENTENCE_);
<OPERATOR>"paragraph"                  		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_PARAGRAPH_);
<OPERATOR>"without"{S}"content"        		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_WITHOUT_CONTENT_);

<OPERATOR>"with"{S}"thesaurus"        		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(THESAURUS1); RECOGNIZE(_WITH_THESAURUS_);
<OPERATOR>"without"{S}"thesaurus"      		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_WITHOUT_THESAURUS_);
<OPERATOR,THESAURUS2>"relationship"       REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; RECOGNIZE(_RELATIONSHIP_);
<OPERATOR,THESAURUS2>"levels"             REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; RECOGNIZE(_LEVELS_);

<OPERATOR>"with"{S}"stop"{S}"words"    		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(STOPWORDS); RECOGNIZE(_WITH_STOP_WORDS_);
<OPERATOR>"without"{S}"stop"{S}"words" 		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_WITHOUT_STOP_WORDS_);
<OPERATOR>"with"{S}"default"{S}"stop"{S}"words" 		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(STOPWORDS); RECOGNIZE(_WITH_DEFAULT_STOP_WORDS_);

<OPERATOR>"exactly"                   		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_EXACTLY_);
<OPERATOR>"at"{S}"least"               		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_AT_LEAST_);
<OPERATOR>"at"{S}"most"               		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_AT_MOST_);
<OPERATOR>"from"                      		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_FROM_);

<OPERATOR>"times"                     		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_TIMES_);
<OPERATOR>"words"                     		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_WORDS_);
<OPERATOR>"sentences"                  		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_SENTENCES_);
<OPERATOR>"paragraphs"                 		REJECT_NOT_FULLTEXT; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_PARAGRAPHS_);

<OPERATOR,ITEMTYPE,STOPWORDS>"union" {
       			REJECT_IF_NOT_TERMINATED;
       			switch(YYSTATE) {
       			case OPERATOR:
       			case ITEMTYPE: CHANGE_STATE(INITIAL); break;
       			case STOPWORDS: break;
       			}
       			RECOGNIZE(_UNION_);
       		}
<OPERATOR,ITEMTYPE>"|"										CHANGE_STATE(INITIAL); RECOGNIZE(_VERTICAL_BAR_);
<OPERATOR,ITEMTYPE>"where"									REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_WHERE_);
<OPERATOR>"preserve"										REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_PRESERVE_);
<OPERATOR>"strip"											REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_STRIP_);
<OPERATOR>"ordered"											REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(OPERATOR); RECOGNIZE(_ORDERING_ORDERED_);
<OPERATOR>"unordered"										REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_ORDERING_UNORDERED_);

<OPERATOR,ITEMTYPE>"castable"						        REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(SINGLETYPE, _AS_); CHANGE_STATE(SINGLETYPE); RECOGNIZE(_CASTABLE_);
<OPERATOR,ITEMTYPE>"cast"       							REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(SINGLETYPE, _AS_); CHANGE_STATE(SINGLETYPE); RECOGNIZE(_CAST_);
<OPERATOR,ITEMTYPE>"treat"      							REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(YYSTATE, _AS_); CHANGE_STATE(ITEMTYPE); RECOGNIZE(_TREAT_);
<OPERATOR,ITEMTYPE,SINGLETYPE>"as"							{
                                                                REJECT_IF_NOT_TERMINATED; 
																if(YYSTATE!=SINGLETYPE)
																{
                                  REJECT_NOT_XQUERY;
																	CHANGE_STATE(ITEMTYPE);
																}
                                                                RECOGNIZE(_AS_);
															}

<OPERATOR,ITEMTYPE>"instance"       						REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(ITEMTYPE, _OF_); CHANGE_STATE(ITEMTYPE); RECOGNIZE(_INSTANCE_);
<ITEMTYPE>"of"                                              REJECT_IF_NOT_TERMINATED; RECOGNIZE(_OF_);
<OPERATOR,ITEMTYPE>"case"									REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(ITEMTYPE); RECOGNIZE(_CASE_);

<OPERATOR,OCCURRENCEINDICATOR>"?"							CHANGE_STATE(OPERATOR); RECOGNIZE(_ZERO_OR_ONE_);

<OPERATOR>"]"												RECOGNIZE(_RBRACK_);
<OPERATOR>"collation"										REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; RECOGNIZE(_COLLATION_);
<OPERATOR>"ascending"										REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; RECOGNIZE(_ASCENDING_);
<OPERATOR>"descending"										REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; RECOGNIZE(_DESCENDING_);
<OPERATOR,THESAURUS1,THESAURUS2>"default"	{
										REJECT_IF_XPATH;
										REJECT_IF_NOT_TERMINATED;
                    if(YYSTATE==THESAURUS1) CHANGE_STATE(OPERATOR);
                    RECOGNIZE(_DEFAULT_);
                 }

<NAMESPACEDECL>"="											RECOGNIZE(_EQUALS_);
<NAMESPACEDECL,KINDTESTFORPI>{NCName}						REJECT_IF_NOT_TERMINATED; yylval.str=allocate_string((XMLCh*)yytext); RECOGNIZE(_NCNAME_);

<NAMESPACEKEYWORD>"namespace"								REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(NAMESPACEDECL); RECOGNIZE(_NAMESPACE_);

<BOUNDARYSPACE_DECL>"preserve"								REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_PRESERVE_);
<BOUNDARYSPACE_DECL>"strip"									REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_STRIP_);

<COPYNAMESPACE_DECL>"preserve"								REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; RECOGNIZE(_PRESERVE_);
<COPYNAMESPACE_DECL>"no-preserve"							REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; RECOGNIZE(_NO_PRESERVE_);
<COPYNAMESPACE_DECL>","										RECOGNIZE(_COMMA_);
<COPYNAMESPACE_DECL>"inherit"								REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_INHERIT_);
<COPYNAMESPACE_DECL>"no-inherit"							REJECT_NOT_XQUERY; REJECT_IF_NOT_TERMINATED; CHANGE_STATE(INITIAL); RECOGNIZE(_NO_INHERIT_);

<ITEMTYPE>"empty-sequence"                                  REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(EMPTY_PARENS, _LPAR_); CHANGE_STATE(EMPTY_PARENS); RECOGNIZE(_EMPTY_);
<ITEMTYPE>"item"                                        	REJECT_IF_NOT_TERMINATED; REJECT_IF_NOT_FOLLOWED1(EMPTY_PARENS, _LPAR_); CHANGE_STATE(EMPTY_PARENS); RECOGNIZE(_ITEM_);
<EMPTY_PARENS>"("                                           RECOGNIZE(_LPAR_);
<EMPTY_PARENS>")"                                           CHANGE_STATE(OCCURRENCEINDICATOR); RECOGNIZE(_RPAR_);

<CLOSEKINDTEST>"?"											RECOGNIZE(_NILLABLE_);

<OCCURRENCEINDICATOR,STOPWORDS>{Char}									{
																undoUserAction();
																CHANGE_STATE(OPERATOR); 
																yyless(0);
															}

<ATTRIBUTE_DECL>"/>"										POP(); RECOGNIZE(_EMPTY_TAG_CLOSE_);

<ATTRIBUTE_DECL>">"											CHANGE_STATE(ELEMENT_CONTENT); RECOGNIZE(_TAG_CLOSE_);

<ATTRIBUTE_DECL,QUOT_ATTRIBUTE_CONTENT>["]					{
																if(YYSTATE==ATTRIBUTE_DECL)
																{
																	CHANGE_STATE(QUOT_ATTRIBUTE_CONTENT); RECOGNIZE(_OPEN_QUOT_);
																}
																else if(YYSTATE==QUOT_ATTRIBUTE_CONTENT)
																{
																	CHANGE_STATE(ATTRIBUTE_DECL); RECOGNIZE(_CLOSE_QUOT_);
																}
															}
<ATTRIBUTE_DECL,APOS_ATTRIBUTE_CONTENT>[']					{
																if(YYSTATE==ATTRIBUTE_DECL)
																{
																	CHANGE_STATE(APOS_ATTRIBUTE_CONTENT); RECOGNIZE(_OPEN_APOS_);
																}
																else if(YYSTATE==APOS_ATTRIBUTE_CONTENT)
																{
																	CHANGE_STATE(ATTRIBUTE_DECL); RECOGNIZE(_CLOSE_APOS_);
																}
															}

<ATTRIBUTE_DECL>"="											RECOGNIZE(_VALUE_INDICATOR_);

<ELEMENT_CONTENT>"</"										CHANGE_STATE(END_TAG); RECOGNIZE(_END_TAG_OPEN_);

<ELEMENT_CONTENT,QUOT_ATTRIBUTE_CONTENT,APOS_ATTRIBUTE_CONTENT>{PredefinedEntityRef}	yylval.str=allocate_string((XMLCh*)yytext, yyleng); RECOGNIZE(_PREDEFINED_ENTITY_REF_); 
<ELEMENT_CONTENT,QUOT_ATTRIBUTE_CONTENT,APOS_ATTRIBUTE_CONTENT>{CharRef}				checkCharRef((XMLCh*)yytext, yyleng); yylval.str=allocate_string((XMLCh*)yytext, yyleng); RECOGNIZE(_CHAR_REF_);
<ELEMENT_CONTENT,QUOT_ATTRIBUTE_CONTENT,APOS_ATTRIBUTE_CONTENT>"{{"						RECOGNIZE(_LCURLY_BRACE_ESCAPE_);
<ELEMENT_CONTENT,QUOT_ATTRIBUTE_CONTENT,APOS_ATTRIBUTE_CONTENT>"}}"						RECOGNIZE(_RCURLY_BRACE_ESCAPE_);

<END_TAG>{WhitespaceChar}*">"								POP(); RECOGNIZE(_TAG_CLOSE_);

<EXPR_COMMENT>":)"											endComment(); POP(); if(m_bReportWhiteSpace) RECOGNIZE(_XQUERY_COMMENT_);

<PROCESSING_INSTRUCTION>{PITarget}							{
																yylval.str=allocate_string((XMLCh*)yytext); 
																CHANGE_STATE(PROCESSING_INSTRUCTION_CONTENT); RECOGNIZE(_PI_TARGET_); 
															}

<PROCESSING_INSTRUCTION_CONTENT>"?>"						{
																yylval.str=(XMLCh*)XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString; 
																POP(); 
																RECOGNIZE(_PROCESSING_INSTRUCTION_CONTENT_);
															}
<PROCESSING_INSTRUCTION_CONTENT>{S}{PIContent}"?>"			{
																yylval.str=allocate_string((XMLCh*)yytext,XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((XMLCh*)yytext)-2); 
																while(iswspace(*yylval.str)) yylval.str++;
																POP(); 
																RECOGNIZE(_PROCESSING_INSTRUCTION_CONTENT_);
															}


<QUOT_ATTRIBUTE_CONTENT>{EscapeQuot}						RECOGNIZE(_ESCAPE_QUOT_);

<APOS_ATTRIBUTE_CONTENT>{EscapeApos}						RECOGNIZE(_ESCAPE_APOS_);

<EXPR_COMMENT>{Char}										if(m_bReportWhiteSpace) RECOGNIZE(_XQUERY_COMMENT_);
<ELEMENT_CONTENT,QUOT_ATTRIBUTE_CONTENT,APOS_ATTRIBUTE_CONTENT>{Char}	{
																			if(*yytext=='&' || *yytext=='<' || *yytext=='{' || *yytext=='}')
																				return error("Character not allowed in content");
																			if(YYSTATE==QUOT_ATTRIBUTE_CONTENT && *yytext=='"')
																				return error("Character not allowed in content");
																			if(YYSTATE==APOS_ATTRIBUTE_CONTENT && *yytext=='\'')
																				return error("Character not allowed in content");
																			yylval.str=allocate_string((XMLCh*)yytext); 
																			RECOGNIZE(_CHAR_);
																		}

<INITIAL>{QName}{WhitespaceChar}*"("						{
																REJECT_IF_COLON;

                                                                XMLCh* beginning=yylval.str=allocate_string((XMLCh*)yytext);
																while(*beginning!='(' && !iswspace(*beginning)) beginning++;
																*beginning=0;

																static const XMLCh attribute_str[] = { 'a', 't', 't', 'r', 'i', 'b', 'u', 't', 'e', 0 };
																static const XMLCh comment_str[] = { 'c', 'o', 'm', 'm', 'e', 'n', 't', 0 };
																static const XMLCh document_node_str[] = { 'd', 'o', 'c', 'u', 'm', 'e', 'n', 't', '-', 'n', 'o', 'd', 'e', 0 };
																static const XMLCh element_str[] = { 'e', 'l', 'e', 'm', 'e', 'n', 't', 0 };
																static const XMLCh item_str[] = { 'i', 't', 'e', 'm', 0 };
																static const XMLCh if_str[] = { 'i', 'f', 0 };
																static const XMLCh node_str[] = { 'n', 'o', 'd', 'e', 0 };
																static const XMLCh processing_instruction_str[] = { 'p', 'r', 'o', 'c', 'e', 's', 's', 'i', 'n', 'g', '-', 'i', 'n', 's', 't', 'r', 'u', 'c', 't', 'i', 'o', 'n', 0 };
																static const XMLCh schema_attribute_str[] = { 's', 'c', 'h', 'e', 'm', 'a', '-', 'a', 't', 't', 'r', 'i', 'b', 'u', 't', 'e', 0 };
																static const XMLCh schema_element_str[] = { 's', 'c', 'h', 'e', 'm', 'a', '-', 'e', 'l', 'e', 'm', 'e', 'n', 't', 0 };
																static const XMLCh text_str[] = { 't', 'e', 'x', 't', 0 };
																static const XMLCh typeswitch_str[] = { 't', 'y', 'p', 'e', 's', 'w', 'i', 't', 'c', 'h', 0 };
																static const XMLCh void_str[] = { 'v', 'o', 'i', 'd', 0 };

                                                                // reject reserved names
																if(XPath2Utils::equals(yylval.str,attribute_str) ||
																   XPath2Utils::equals(yylval.str,comment_str) ||
																   XPath2Utils::equals(yylval.str,document_node_str) ||
																   XPath2Utils::equals(yylval.str,element_str) ||
																   XPath2Utils::equals(yylval.str,item_str) ||
																   XPath2Utils::equals(yylval.str,if_str) ||
																   XPath2Utils::equals(yylval.str,node_str) ||
																   XPath2Utils::equals(yylval.str,processing_instruction_str) ||
																   XPath2Utils::equals(yylval.str,schema_attribute_str) ||
																   XPath2Utils::equals(yylval.str,schema_element_str) ||
																   XPath2Utils::equals(yylval.str,text_str) ||
																   XPath2Utils::equals(yylval.str,typeswitch_str) ||
																   XPath2Utils::equals(yylval.str,void_str))
																{
																	undoUserAction(); 
																	REJECT;
																}

																RECOGNIZE(_FUNCTION_CALL_);
															}
<INITIAL,SINGLETYPE,ITEMTYPE,KINDTEST,VARNAME,START_TAG,ATTRIBUTE_DECL,END_TAG>{QName}		{
																											REJECT_IF_NOT_TERMINATED; 
																											yylval.str=allocate_string((XMLCh*)yytext); 
																											switch(YYSTATE)
																											{
																												case INITIAL:
																												case SINGLETYPE:		CHANGE_STATE(OPERATOR); RECOGNIZE(_QNAME_);
																												case ITEMTYPE:			CHANGE_STATE(OCCURRENCEINDICATOR); RECOGNIZE(_QNAME_);
																												case KINDTEST:			CHANGE_STATE(CLOSEKINDTEST); RECOGNIZE(_QNAME_);
																												case VARNAME:			CHANGE_STATE(OPERATOR); RECOGNIZE(_VARIABLE_);
																												case START_TAG:			CHANGE_STATE(ATTRIBUTE_DECL); RECOGNIZE(_TAG_NAME_);
																												case ATTRIBUTE_DECL:	RECOGNIZE(_ATTRIBUTE_NAME_);
																												case END_TAG:			RECOGNIZE(_TAG_NAME_);
																											}
																										}

<*><<EOF>>				{ 
							if(m_bGenerateEOF) 
							{
								m_bGenerateEOF=false;
								RECOGNIZE(_EOF_); 
							}
							return 0;
						}

<*>[\x0000-\xFFFF]		{ 
							char buff[128]; 
							sprintf(buff,"Unrecognized character '%c' (0x%X)",yytext[0],yytext[0]); 
							return error(buff); 
						}

%%
