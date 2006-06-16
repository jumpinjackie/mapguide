%{

#include "Scanner.hpp"
#include "Grammar.hpp"

#define YY_STACK_USED 1
#define RECOGNIZE(token)	return token
// yy_push_state pushes the current state and does a transition to the specified one;
// the semantic of the PUSH/PUSH_CURRENT methods in the specs is that the specified state is pushed
#define PUSH(state)			{ int curState=YYSTATE; BEGIN(state); yy_push_state(curState); }
#define PUSH_CURRENT()		PUSH(YYSTATE)
#define POP()				yy_pop_state()

static char szDelimiters[]       ="\t\n\r <>?*$)({/=!|@[],;%\"'&}:.-+";

#define REJECT_IF_NOT_TERMINATED    	{ if(yy_hold_char!=YY_END_OF_BUFFER_CHAR && strchr(szDelimiters,yy_hold_char)==NULL) { undoUserAction(); REJECT; } }
#define REJECT_IF_COLON			    	{ if(yy_hold_char!=YY_END_OF_BUFFER_CHAR && yy_hold_char==':') { undoUserAction(); REJECT; } }

%}

/* -------------- declaration section -------------- */

/* from XML spec */
Char				([\x0009]|[\x000D]|[\x000A]|[\x0020-\xD7FF]|[\xE000-\xFFFD])
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
%option yyclass="CXQueryScanner"
%option c++
%option outfile="Scanner.cpp"

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

%%

<INITIAL,OPERATOR,BOUNDARYSPACE_DECL,COPYNAMESPACE_DECL,NAMESPACEDECL,NAMESPACEKEYWORD,KINDTEST,KINDTESTFORPI,CLOSEKINDTEST,ITEMTYPE,SINGLETYPE,ATTRIBUTE_DECL,PRAGMA_NAME>{S}		if(m_bReportWhiteSpace) RECOGNIZE(_XQUERY_WHITESPACE_);

<INITIAL>{DecimalLiteral}									REJECT_IF_NOT_TERMINATED; yylval.str=allocate_string((XMLCh*)yytext); BEGIN(OPERATOR); RECOGNIZE(_DECIMAL_NUMBER_);
<INITIAL>{DoubleLiteral}									REJECT_IF_NOT_TERMINATED; yylval.str=allocate_string((XMLCh*)yytext); BEGIN(OPERATOR); RECOGNIZE(_DOUBLE_NUMBER_);
<INITIAL>{IntegerLiteral}									REJECT_IF_NOT_TERMINATED; yylval.str=allocate_string((XMLCh*)yytext); BEGIN(OPERATOR); RECOGNIZE(_INTEGER_NUMBER_);
<INITIAL>"."												BEGIN(OPERATOR); RECOGNIZE(_DOT_);
<INITIAL>".."												BEGIN(OPERATOR); RECOGNIZE(_DOT_DOT_);
<INITIAL>{NCName}":*"										{
																yylval.str=allocate_string((XMLCh*)yytext,XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((XMLCh*)yytext)-2);
																BEGIN(OPERATOR);
																RECOGNIZE(_NCNAME_COLON_STAR_);
															}
<INITIAL,OPERATOR,KINDTEST,KINDTESTFORPI,CLOSEKINDTEST>")"	{
																switch(YYSTATE)
																{
																case INITIAL:
																case OPERATOR:		BEGIN(OPERATOR); break;
																case KINDTEST:
																case KINDTESTFORPI:
																case CLOSEKINDTEST:	POP(); break;
																}
																RECOGNIZE(_RPAR_);
															}
<INITIAL>"*:"{NCName}										{
																yylval.str=allocate_string((XMLCh*)(yytext+2));	// skip the *:
																BEGIN(OPERATOR);
																RECOGNIZE(_STAR_COLON_NCNAME_);
															}
<INITIAL,OPERATOR,OCCURRENCEINDICATOR,KINDTEST>"*"			{
																switch(YYSTATE)
																{
																case INITIAL:				BEGIN(OPERATOR); RECOGNIZE(_STAR_);
																case OCCURRENCEINDICATOR:	BEGIN(OPERATOR); RECOGNIZE(_ZERO_OR_MORE_);
																case OPERATOR:				BEGIN(INITIAL); RECOGNIZE(_MULTIPLY_);
																case KINDTEST:				BEGIN(CLOSEKINDTEST); RECOGNIZE(_STAR_);
																}
															}
<INITIAL,OPERATOR,NAMESPACEDECL,NAMESPACEKEYWORD,KINDTESTFORPI>{StringLiteral}	{
																							yylval.str=allocate_string_and_unescape(*yytext,(XMLCh*)(yytext+1),XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((XMLCh*)yytext)-2);
																							switch(YYSTATE)
																							{
																							case INITIAL:
																							case OPERATOR:			BEGIN(OPERATOR); break;
																							case KINDTESTFORPI:		break;
																							case NAMESPACEDECL:
																							case NAMESPACEKEYWORD:	BEGIN(INITIAL); break;
																							}
																							RECOGNIZE(_STRING_LITERAL_);
																						}

<INITIAL>"declare"											REJECT_IF_NOT_TERMINATED; RECOGNIZE(_DECLARE_);
<INITIAL>"declare"{S}"option"								REJECT_IF_NOT_TERMINATED; RECOGNIZE(_DECLARE_OPTION_);
<INITIAL>"declare"{S}"variable"								REJECT_IF_NOT_TERMINATED; RECOGNIZE(_DECLARE_VARIABLE_);
<INITIAL>"declare"{S}"function"								REJECT_IF_NOT_TERMINATED; RECOGNIZE(_DECLARE_FUNCTION_);

<INITIAL>"construction"										REJECT_IF_NOT_TERMINATED; BEGIN(OPERATOR); RECOGNIZE(_CONSTRUCTION_);
<INITIAL>"ordering"											REJECT_IF_NOT_TERMINATED; BEGIN(OPERATOR); RECOGNIZE(_ORDERING_);
<INITIAL>"default"{S}"order"								REJECT_IF_NOT_TERMINATED; BEGIN(OPERATOR); RECOGNIZE(_DEFAULT_ORDER_);

<INITIAL>"copy-namespaces"									REJECT_IF_NOT_TERMINATED; BEGIN(COPYNAMESPACE_DECL); RECOGNIZE(_COPY_NAMESPACES_);

<INITIAL>"default"{S}"collation"							REJECT_IF_NOT_TERMINATED; BEGIN(NAMESPACEDECL); RECOGNIZE(_DEFAULT_COLLATION_);
<INITIAL>"namespace"										REJECT_IF_NOT_TERMINATED; BEGIN(NAMESPACEDECL); RECOGNIZE(_NAMESPACE_);
<INITIAL>"base-uri"											REJECT_IF_NOT_TERMINATED; BEGIN(NAMESPACEDECL); RECOGNIZE(_BASEURI_);
<INITIAL>"default"{S}"element"								REJECT_IF_NOT_TERMINATED; BEGIN(NAMESPACEKEYWORD); RECOGNIZE(_DEFAULT_ELEMENT_);
<INITIAL>"default"{S}"function"								REJECT_IF_NOT_TERMINATED; BEGIN(NAMESPACEKEYWORD); RECOGNIZE(_DEFAULT_FUNCTION_);

<INITIAL>"boundary-space"									REJECT_IF_NOT_TERMINATED; BEGIN(BOUNDARYSPACE_DECL); RECOGNIZE(_BOUNDARYSPACE_);

<INITIAL>"module"{S}"namespace"								REJECT_IF_NOT_TERMINATED; BEGIN(NAMESPACEDECL); RECOGNIZE(_MODULE_NAMESPACE_);
<INITIAL>"import"{S}"schema"								REJECT_IF_NOT_TERMINATED; BEGIN(NAMESPACEKEYWORD); RECOGNIZE(_IMPORT_SCHEMA_);
<INITIAL>"import"{S}"module"								REJECT_IF_NOT_TERMINATED; BEGIN(NAMESPACEKEYWORD); RECOGNIZE(_IMPORT_MODULE_);

<INITIAL,OPERATOR,ITEMTYPE>"$"								BEGIN(VARNAME); RECOGNIZE(_DOLLAR_SIGN_);
<INITIAL,OPERATOR>"for"										REJECT_IF_NOT_TERMINATED; RECOGNIZE(_FOR_);
<INITIAL,OPERATOR>"let"										REJECT_IF_NOT_TERMINATED; RECOGNIZE(_LET_);
<INITIAL>"some"												REJECT_IF_NOT_TERMINATED; RECOGNIZE(_SOME_);
<INITIAL>"every"											REJECT_IF_NOT_TERMINATED; RECOGNIZE(_EVERY_);

<INITIAL,OPERATOR,ITEMTYPE>")"{WhitespaceChar}*"as"			REJECT_IF_NOT_TERMINATED; BEGIN(ITEMTYPE); RECOGNIZE(_EXPR_AS_);

<INITIAL,ITEMTYPE,KINDTEST>"element"{WhitespaceChar}*"("	{
																REJECT_IF_COLON;
																switch(YYSTATE)
																{
																case INITIAL:  PUSH(OPERATOR); break;
																case ITEMTYPE: PUSH(OCCURRENCEINDICATOR); break;
																case KINDTEST: PUSH(KINDTEST); break;
																}
																BEGIN(KINDTEST); 
																RECOGNIZE(_ELEMENT_LPAR_);
															}
<INITIAL,ITEMTYPE>"attribute"{WhitespaceChar}*"("			{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																BEGIN(KINDTEST);
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
																	BEGIN(KINDTEST); 
																	RECOGNIZE(_SCHEMA_ELEMENT_LPAR_);
																}
<INITIAL,ITEMTYPE>"schema-attribute"{WhitespaceChar}*"("	{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																BEGIN(KINDTEST);
																RECOGNIZE(_SCHEMA_ATTRIBUTE_LPAR_);
															}
<INITIAL,ITEMTYPE>"comment"{WhitespaceChar}*"("				{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																BEGIN(KINDTEST);
																RECOGNIZE(_COMMENT_LPAR_);
															}
<INITIAL,ITEMTYPE>"text"{WhitespaceChar}*"("				{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																BEGIN(KINDTEST);
																RECOGNIZE(_TEXT_LPAR_);
															}
<INITIAL,ITEMTYPE>"node"{WhitespaceChar}*"("				{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																BEGIN(KINDTEST);
																RECOGNIZE(_NODE_LPAR_);
															}
<INITIAL,ITEMTYPE>"document-node"{WhitespaceChar}*"("		{
																REJECT_IF_COLON;
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																PUSH(stateToPush);
																BEGIN(KINDTEST);
																RECOGNIZE(_DOCUMENT_NODE_LPAR_);
															}
<INITIAL,ITEMTYPE>"processing-instruction"{WhitespaceChar}*"("	{
																	REJECT_IF_COLON;
																	int stateToPush=(YYSTATE==INITIAL)?OPERATOR:OCCURRENCEINDICATOR;
																	PUSH(stateToPush);
																	BEGIN(KINDTESTFORPI);
																	RECOGNIZE(_PROCESSING_INSTRUCTION_LPAR_);
																}

<INITIAL,ELEMENT_CONTENT>"<!--"{CommentContent}"-->"		{
																yylval.str=allocate_string((XMLCh*)(yytext+4),XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((XMLCh*)(yytext+4))-3); 
																if(YYSTATE==INITIAL)
																	BEGIN(OPERATOR);
																RECOGNIZE(_XML_COMMENT_);
															}

<INITIAL,ELEMENT_CONTENT>"<?"								{
																int stateToPush=(YYSTATE==INITIAL)?OPERATOR:YYSTATE;
																PUSH(stateToPush);
																BEGIN(PROCESSING_INSTRUCTION); 
																RECOGNIZE(_PROCESSING_INSTRUCTION_START_);
															}

<INITIAL,ELEMENT_CONTENT>"<![CDATA["{CDATAContent}"]]>"		{
																yylval.str=allocate_string((XMLCh*)(yytext+9),XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((XMLCh*)(yytext+9))-3); 
																if(YYSTATE==INITIAL)
																	BEGIN(OPERATOR);
																RECOGNIZE(_CDATA_SECTION_);
															}

<INITIAL,OPERATOR,ITEMTYPE,ELEMENT_CONTENT>"<"				{
																switch(YYSTATE)
																{
																case INITIAL:
																	PUSH(OPERATOR); BEGIN(START_TAG); RECOGNIZE(_START_TAG_OPEN_);
																case OPERATOR:
																case ITEMTYPE:
																	BEGIN(INITIAL); RECOGNIZE(_LT_);
																case ELEMENT_CONTENT:
																	PUSH_CURRENT(); BEGIN(START_TAG); RECOGNIZE(_START_TAG_OPEN_);
																}
															}

<INITIAL,OPERATOR>"}"										POP(); RECOGNIZE(_RBRACE_);

<INITIAL>"validate"											REJECT_IF_NOT_TERMINATED; RECOGNIZE(_VALIDATE_CONTENT_);
<INITIAL>"validate"{S}"lax"									PUSH(OPERATOR); BEGIN(INITIAL); RECOGNIZE(_VALIDATE_MODE_LAX_);
<INITIAL>"validate"{S}"strict"								PUSH(OPERATOR); BEGIN(KINDTEST); RECOGNIZE(_VALIDATE_MODE_STRICT_);

<INITIAL>"typeswitch"										REJECT_IF_NOT_TERMINATED; RECOGNIZE(_TYPESWITCH_);

<INITIAL>"element"/{WhitespaceChar}*"{"						RECOGNIZE(_ELEMENT_CONSTR_);
<INITIAL>"attribute"/{WhitespaceChar}*"{"					RECOGNIZE(_ATTRIBUTE_CONSTR_);

<INITIAL>"attribute"{S}{QName}/{WhitespaceChar}*"{"			{
																XMLCh* beginning=(XMLCh*)(yytext+9);
																while(iswspace(*beginning)) beginning++;
																yylval.str=allocate_string(beginning); 
																RECOGNIZE(_NAMED_ATTRIBUTE_CONSTR_);
															}
<INITIAL>"element"{S}{QName}/{WhitespaceChar}*"{"			{
																XMLCh* beginning=(XMLCh*)(yytext+7);
																while(iswspace(*beginning)) beginning++;
																yylval.str=allocate_string(beginning); 
																RECOGNIZE(_NAMED_ELEMENT_CONSTR_);
															}
<INITIAL>"document"/{WhitespaceChar}*"{"					RECOGNIZE(_DOCUMENT_CONSTR_);
<INITIAL>"text"/{WhitespaceChar}*"{"						RECOGNIZE(_TEXT_CONSTR_);
<INITIAL>"processing-instruction"/{WhitespaceChar}*"{"		RECOGNIZE(_PROCESSING_INSTRUCTION_CONSTR_);
<INITIAL>"processing-instruction"{S}{NCName}/{WhitespaceChar}*"{"	{
																		XMLCh* beginning=(XMLCh*)(yytext+22);
																		while(iswspace(*beginning)) beginning++;
																		yylval.str=allocate_string(beginning); 
																		RECOGNIZE(_NAMED_PROCESSING_INSTRUCTION_CONSTR_);
																	}
<INITIAL>"comment"/{WhitespaceChar}*"{"						RECOGNIZE(_COMMENT_CONSTR_);

<INITIAL,OPERATOR,NAMESPACEDECL,NAMESPACEKEYWORD,BOUNDARYSPACE_DECL,COPYNAMESPACE_DECL,SINGLETYPE,ITEMTYPE,KINDTEST,KINDTESTFORPI,CLOSEKINDTEST,OCCURRENCEINDICATOR,VARNAME,EXPR_COMMENT>"(:"						beginComment(); PUSH_CURRENT(); BEGIN(EXPR_COMMENT); if(m_bReportWhiteSpace) RECOGNIZE(_XQUERY_COMMENT_);

<INITIAL>"(#"{WhitespaceChar}*								BEGIN(PRAGMA_NAME); RECOGNIZE(_PRAGMA_OPEN_);

<INITIAL,OPERATOR,ITEMTYPE>";"								BEGIN(INITIAL); RECOGNIZE(_SEMICOLON_);

<INITIAL,OPERATOR,KINDTEST,CLOSEKINDTEST,ELEMENT_CONTENT,QUOT_ATTRIBUTE_CONTENT,APOS_ATTRIBUTE_CONTENT>"{"	{
																switch(YYSTATE)
																{
																case ELEMENT_CONTENT:
																case QUOT_ATTRIBUTE_CONTENT:
																case APOS_ATTRIBUTE_CONTENT:	PUSH_CURRENT(); BEGIN(INITIAL); break;
																case INITIAL:
																case OPERATOR:
																case KINDTEST:
																case CLOSEKINDTEST:				PUSH(OPERATOR); BEGIN(INITIAL); break;
																}
																RECOGNIZE(_LBRACE_);
															}

<INITIAL>"ordered"/{WhitespaceChar}*"{"						PUSH(OPERATOR); BEGIN(INITIAL); RECOGNIZE(_ORDERED_);
<INITIAL>"unordered"/{WhitespaceChar}*"{"					PUSH(OPERATOR); BEGIN(INITIAL); RECOGNIZE(_UNORDERED_);

<INITIAL>"xquery"{S}"version"{S}{StringLiteral}				{
																XMLCh* beginning=(XMLCh*)(yytext+6);
																while(iswspace(*beginning)) beginning++;
																beginning+=7;
																while(iswspace(*beginning)) beginning++;
																yylval.str=allocate_string_and_unescape(*beginning,beginning+1,XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(beginning)-2); 
																RECOGNIZE(_XQUERY_VERSION_);
															}
<INITIAL>"encoding"											REJECT_IF_NOT_TERMINATED; RECOGNIZE(_ENCODING_);

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
<INITIAL>"namespace"{WhitespaceChar}*"::"					return error("Axis 'namespace' is not part of the XQuery language");
<INITIAL,OPERATOR,ITEMTYPE,CLOSEKINDTEST>","				{
																if(YYSTATE==CLOSEKINDTEST)
																	BEGIN(KINDTEST); 
																else
																	BEGIN(INITIAL); 
																RECOGNIZE(_COMMA_);
															}
<INITIAL>"if"												REJECT_IF_NOT_TERMINATED; RECOGNIZE(_IF_);
<INITIAL,ITEMTYPE>"("										BEGIN(INITIAL); RECOGNIZE(_LPAR_);
<INITIAL,OPERATOR,ITEMTYPE>"-"								BEGIN(INITIAL); RECOGNIZE(_MINUS_);
<INITIAL,OPERATOR,ITEMTYPE>"at"								REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_AT_KEYWORD_);
<INITIAL,OPERATOR,OCCURRENCEINDICATOR>"+"					{
																if(YYSTATE==OCCURRENCEINDICATOR)
																{
																	BEGIN(OPERATOR); RECOGNIZE(_ONE_OR_MORE_);
																}
																else
																{
																	BEGIN(INITIAL); RECOGNIZE(_PLUS_);
																}
															}
<INITIAL,OPERATOR>"//"										BEGIN(INITIAL); RECOGNIZE(_SLASHSLASH_);
<INITIAL,OPERATOR>"/"										BEGIN(INITIAL); RECOGNIZE(_SLASH_);

<OPERATOR,ITEMTYPE>"then"									REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_THEN_);
<OPERATOR,ITEMTYPE>"else"									REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_ELSE_);
<OPERATOR,ITEMTYPE>"external"								REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_EXTERNAL_);
<OPERATOR,ITEMTYPE>"and"									REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_AND_);
<OPERATOR,ITEMTYPE>":="										BEGIN(INITIAL); RECOGNIZE(_COLON_EQ_);
<OPERATOR,ITEMTYPE>"div"									REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_DIV_);
<OPERATOR,ITEMTYPE>"="										BEGIN(INITIAL); RECOGNIZE(_EQUALS_);
<OPERATOR,ITEMTYPE>"except"									REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_EXCEPT_);
<OPERATOR,ITEMTYPE>"eq"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_FORTRAN_EQ_);
<OPERATOR,ITEMTYPE>"ge"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_FORTRAN_GE_);
<OPERATOR,ITEMTYPE>"gt"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_FORTRAN_GT_);
<OPERATOR,ITEMTYPE>"le"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_FORTRAN_LE_);
<OPERATOR,ITEMTYPE>"lt"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_FORTRAN_LT_);
<OPERATOR,ITEMTYPE>"ne"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_FORTRAN_NE_);
<OPERATOR,ITEMTYPE>">="										BEGIN(INITIAL); RECOGNIZE(_GT_EQUALS_);
<OPERATOR,ITEMTYPE>">>"										BEGIN(INITIAL); RECOGNIZE(_GT_GT_);
<OPERATOR,ITEMTYPE>">"										BEGIN(INITIAL); RECOGNIZE(_GT_);
<OPERATOR,ITEMTYPE>"idiv"									REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_INTEGER_DIV_);
<OPERATOR,ITEMTYPE>"intersect"								REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_INTERSECT_);
<OPERATOR,ITEMTYPE>"in"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_IN_);
<OPERATOR,ITEMTYPE>"is"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_IS_);
<OPERATOR,ITEMTYPE>"["										BEGIN(INITIAL); RECOGNIZE(_LBRACK_);
<OPERATOR,ITEMTYPE>"<="										BEGIN(INITIAL); RECOGNIZE(_LT_EQUALS_);
<OPERATOR,ITEMTYPE>"<<"										BEGIN(INITIAL); RECOGNIZE(_LT_LT_);
<OPERATOR,ITEMTYPE>"mod"									REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_MOD_);
<OPERATOR,ITEMTYPE>"!="										BEGIN(INITIAL); RECOGNIZE(_NOT_EQUALS_);
<OPERATOR,ITEMTYPE>"order"{S}"by"							REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_ORDER_BY_);
<OPERATOR,ITEMTYPE>"stable"{S}"order"{S}"by"				REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_STABLE_ORDER_BY_);
<OPERATOR,ITEMTYPE>"or"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_OR_);
<OPERATOR,ITEMTYPE>"return"									REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_RETURN_);
<OPERATOR,ITEMTYPE>"satisfies"								REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_SATISFIES_);
<OPERATOR,ITEMTYPE>"to"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_TO_);
<OPERATOR,ITEMTYPE>"union"									REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_UNION_);
<OPERATOR,ITEMTYPE>"|"										BEGIN(INITIAL); RECOGNIZE(_VERTICAL_BAR_);
<OPERATOR,ITEMTYPE>"where"									REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_WHERE_);
<OPERATOR>"preserve"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_CONSTRUCTION_PRESERVE_);
<OPERATOR>"strip"											REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_CONSTRUCTION_STRIP_);
<OPERATOR>"ordered"											REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_ORDERING_ORDERED_);
<OPERATOR>"unordered"										REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_ORDERING_UNORDERED_);

<OPERATOR,ITEMTYPE>"castable"{S}"as"						REJECT_IF_NOT_TERMINATED; BEGIN(SINGLETYPE); RECOGNIZE(_CASTABLE_AS_);
<OPERATOR,ITEMTYPE>"cast"{S}"as"							REJECT_IF_NOT_TERMINATED; BEGIN(SINGLETYPE); RECOGNIZE(_CAST_AS_);

<OPERATOR,ITEMTYPE>"instance"{S}"of"						REJECT_IF_NOT_TERMINATED; BEGIN(ITEMTYPE); RECOGNIZE(_INSTANCE_OF_);
<OPERATOR,ITEMTYPE>"treat"{S}"as"							REJECT_IF_NOT_TERMINATED; BEGIN(ITEMTYPE); RECOGNIZE(_TREAT_AS_);
<OPERATOR,ITEMTYPE>"case"									REJECT_IF_NOT_TERMINATED; BEGIN(ITEMTYPE); RECOGNIZE(_CASE_);
<OPERATOR,ITEMTYPE>"as"										REJECT_IF_NOT_TERMINATED; BEGIN(ITEMTYPE); RECOGNIZE(_AS_);

<OPERATOR,OCCURRENCEINDICATOR>"?"							BEGIN(OPERATOR); RECOGNIZE(_ZERO_OR_ONE_);

<OPERATOR>"]"												RECOGNIZE(_RBRACK_);
<OPERATOR>"collation"										REJECT_IF_NOT_TERMINATED; RECOGNIZE(_COLLATION_);
<OPERATOR>"ascending"										REJECT_IF_NOT_TERMINATED; RECOGNIZE(_ASCENDING_);
<OPERATOR>"descending"										REJECT_IF_NOT_TERMINATED; RECOGNIZE(_DESCENDING_);
<OPERATOR>"empty"{S}"greatest"								REJECT_IF_NOT_TERMINATED; RECOGNIZE(_EMPTY_GREATEST_);
<OPERATOR>"empty"{S}"least"									REJECT_IF_NOT_TERMINATED; RECOGNIZE(_EMPTY_LEAST_);
<OPERATOR>"default"											REJECT_IF_NOT_TERMINATED; RECOGNIZE(_DEFAULT_);

<NAMESPACEDECL>"="											RECOGNIZE(_EQUALS_);
<NAMESPACEDECL,KINDTESTFORPI>{NCName}						REJECT_IF_NOT_TERMINATED; yylval.str=allocate_string((XMLCh*)yytext); RECOGNIZE(_NCNAME_);

<NAMESPACEKEYWORD>"namespace"								REJECT_IF_NOT_TERMINATED; BEGIN(NAMESPACEDECL); RECOGNIZE(_NAMESPACE_);
<NAMESPACEKEYWORD>"default"{S}"element"						REJECT_IF_NOT_TERMINATED; RECOGNIZE(_DEFAULT_ELEMENT_);

<BOUNDARYSPACE_DECL>"preserve"								REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_PRESERVE_);
<BOUNDARYSPACE_DECL>"strip"									REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_STRIP_);

<COPYNAMESPACE_DECL>"preserve"								REJECT_IF_NOT_TERMINATED; RECOGNIZE(_NS_PRESERVE_);
<COPYNAMESPACE_DECL>"no-preserve"							REJECT_IF_NOT_TERMINATED; RECOGNIZE(_NS_NO_PRESERVE_);
<COPYNAMESPACE_DECL>","										RECOGNIZE(_COMMA_);
<COPYNAMESPACE_DECL>"inherit"								REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_NS_INHERIT_);
<COPYNAMESPACE_DECL>"no-inherit"							REJECT_IF_NOT_TERMINATED; BEGIN(INITIAL); RECOGNIZE(_NS_NO_INHERIT_);

<ITEMTYPE>"void"{WhitespaceChar}*"("{WhitespaceChar}*")"	BEGIN(OPERATOR); RECOGNIZE(_EMPTY_);

<ITEMTYPE>"item"{WhitespaceChar}*"("{WhitespaceChar}*")"	BEGIN(OCCURRENCEINDICATOR); RECOGNIZE(_ITEM_);

<CLOSEKINDTEST>"?"											RECOGNIZE(_NILLABLE_);

<OCCURRENCEINDICATOR>{Char}									{
																undoUserAction();
																BEGIN(OPERATOR); 
																yyless(0);
															}

<ATTRIBUTE_DECL>"/>"										POP(); RECOGNIZE(_EMPTY_TAG_CLOSE_);

<ATTRIBUTE_DECL>">"											BEGIN(ELEMENT_CONTENT); RECOGNIZE(_TAG_CLOSE_);

<ATTRIBUTE_DECL,QUOT_ATTRIBUTE_CONTENT>["]					{
																if(YYSTATE==ATTRIBUTE_DECL)
																{
																	BEGIN(QUOT_ATTRIBUTE_CONTENT); RECOGNIZE(_OPEN_QUOT_);
																}
																else if(YYSTATE==QUOT_ATTRIBUTE_CONTENT)
																{
																	BEGIN(ATTRIBUTE_DECL); RECOGNIZE(_CLOSE_QUOT_);
																}
															}
<ATTRIBUTE_DECL,APOS_ATTRIBUTE_CONTENT>[']					{
																if(YYSTATE==ATTRIBUTE_DECL)
																{
																	BEGIN(APOS_ATTRIBUTE_CONTENT); RECOGNIZE(_OPEN_APOS_);
																}
																else if(YYSTATE==APOS_ATTRIBUTE_CONTENT)
																{
																	BEGIN(ATTRIBUTE_DECL); RECOGNIZE(_CLOSE_APOS_);
																}
															}

<ATTRIBUTE_DECL>"="											RECOGNIZE(_VALUE_INDICATOR_);

<ELEMENT_CONTENT>"</"										BEGIN(END_TAG); RECOGNIZE(_END_TAG_OPEN_);

<ELEMENT_CONTENT,QUOT_ATTRIBUTE_CONTENT,APOS_ATTRIBUTE_CONTENT>{PredefinedEntityRef}	yylval.str=allocate_string_and_unescape(0, (XMLCh*)yytext, yyleng); RECOGNIZE(_PREDEFINED_ENTITY_REF_); 
<ELEMENT_CONTENT,QUOT_ATTRIBUTE_CONTENT,APOS_ATTRIBUTE_CONTENT>{CharRef}				yylval.str=allocate_string_and_unescape(0, (XMLCh*)yytext, yyleng); RECOGNIZE(_CHAR_REF_);
<ELEMENT_CONTENT,QUOT_ATTRIBUTE_CONTENT,APOS_ATTRIBUTE_CONTENT>"{{"						RECOGNIZE(_LCURLY_BRACE_ESCAPE_);
<ELEMENT_CONTENT,QUOT_ATTRIBUTE_CONTENT,APOS_ATTRIBUTE_CONTENT>"}}"						RECOGNIZE(_RCURLY_BRACE_ESCAPE_);

<END_TAG>{WhitespaceChar}*">"								POP(); RECOGNIZE(_TAG_CLOSE_);

<EXPR_COMMENT>":)"											endComment(); POP(); if(m_bReportWhiteSpace) RECOGNIZE(_XQUERY_COMMENT_);

<PRAGMA_CONTENT>{PragmaContents}							RECOGNIZE(_PRAGMA_CONTENT_);
<PRAGMA_CONTENT>"#)"										BEGIN(INITIAL); RECOGNIZE(_PRAGMA_CLOSE_);

<PROCESSING_INSTRUCTION>{PITarget}							{
																yylval.str=allocate_string((XMLCh*)yytext); 
																BEGIN(PROCESSING_INSTRUCTION_CONTENT); RECOGNIZE(_PI_TARGET_); 
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
<INITIAL,SINGLETYPE,ITEMTYPE,KINDTEST,VARNAME,START_TAG,ATTRIBUTE_DECL,END_TAG,PRAGMA_NAME>{QName}		{
																											REJECT_IF_NOT_TERMINATED; 
																											yylval.str=allocate_string((XMLCh*)yytext); 
																											switch(YYSTATE)
																											{
																												case INITIAL:
																												case SINGLETYPE:		BEGIN(OPERATOR); RECOGNIZE(_QNAME_);
																												case ITEMTYPE:			BEGIN(OCCURRENCEINDICATOR); RECOGNIZE(_QNAME_);
																												case KINDTEST:			BEGIN(CLOSEKINDTEST); RECOGNIZE(_QNAME_);
																												case VARNAME:			BEGIN(OPERATOR); RECOGNIZE(_VARIABLE_);
																												case START_TAG:			BEGIN(ATTRIBUTE_DECL); RECOGNIZE(_TAG_NAME_);
																												case ATTRIBUTE_DECL:	RECOGNIZE(_ATTRIBUTE_NAME_);
																												case END_TAG:			RECOGNIZE(_TAG_NAME_);
																												case PRAGMA_NAME:		BEGIN(PRAGMA_CONTENT); RECOGNIZE(_PRAGMA_NAME_);
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
