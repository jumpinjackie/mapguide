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
 * $Id: XQParser.y,v 1.15.2.1 2007/01/03 19:19:51 jpcs Exp $
 */

%{

#if defined(WIN32) && !defined(__CYGWIN__)
// turn off the warnings "switch statement contains 'default' but no 'case' labels"
//                       "'yyerrlab1' : unreferenced label"
#pragma warning(disable : 4065 4102)
#endif

#include <iostream>
#include <xqilla/utils/UTF8Str.hpp>

#include "../lexer/XQLexer.hpp"

#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/ast/XQDocumentConstructor.hpp>
#include <xqilla/ast/XQElementConstructor.hpp>
#include <xqilla/ast/XQAttributeConstructor.hpp>
#include <xqilla/ast/XQPIConstructor.hpp>
#include <xqilla/ast/XQCommentConstructor.hpp>
#include <xqilla/ast/XQTextConstructor.hpp>
#include <xqilla/ast/XQFLWOR.hpp>
#include <xqilla/ast/XQQuantified.hpp>
#include <xqilla/ast/XQTypeswitch.hpp>
#include <xqilla/ast/XQValidate.hpp>
#include <xqilla/ast/XQGlobalVariable.hpp>
#include <xqilla/ast/XQDebugHook.hpp>
#include <xqilla/ast/XQFunctionCall.hpp>
#include <xqilla/ast/XQOrderingChange.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#include <xqilla/items/AnyAtomicTypeConstructor.hpp>

#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/ast/XQParenthesizedExpr.hpp>
#include <xqilla/ast/XQNav.hpp>
#include <xqilla/ast/XQStep.hpp>
#include <xqilla/ast/XQLiteral.hpp>
#include <xqilla/ast/XQVariable.hpp>
#include <xqilla/ast/XQInstanceOf.hpp>
#include <xqilla/ast/XQCastAs.hpp>
#include <xqilla/ast/XQCastableAs.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/XQIf.hpp>
#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/ast/XQPredicate.hpp>

#include <xqilla/fulltext/FTContains.hpp>
#include <xqilla/fulltext/FTOr.hpp>
#include <xqilla/fulltext/FTAnd.hpp>
#include <xqilla/fulltext/FTMildnot.hpp>
#include <xqilla/fulltext/FTUnaryNot.hpp>
#include <xqilla/fulltext/FTOrder.hpp>
#include <xqilla/fulltext/FTDistance.hpp>
#include <xqilla/fulltext/FTScope.hpp>
#include <xqilla/fulltext/FTContent.hpp>
#include <xqilla/fulltext/FTWindow.hpp>

#include <xqilla/parser/QName.hpp>

#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/operators/Plus.hpp>
#include <xqilla/operators/Minus.hpp>
#include <xqilla/operators/Multiply.hpp>
#include <xqilla/operators/Divide.hpp>
#include <xqilla/operators/IntegerDivide.hpp>
#include <xqilla/operators/Mod.hpp>
#include <xqilla/operators/UnaryMinus.hpp>
#include <xqilla/operators/NodeComparison.hpp>
#include <xqilla/operators/OrderComparison.hpp>
#include <xqilla/operators/Intersect.hpp>
#include <xqilla/operators/Except.hpp>
#include <xqilla/operators/Union.hpp>
#include <xqilla/operators/Or.hpp>
#include <xqilla/operators/And.hpp>
#include <xqilla/operators/Range.hpp>

#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/functions/FunctionRoot.hpp>

#include <xqilla/axis/NodeTest.hpp>

#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/NamespaceLookupException.hpp>
#include <xqilla/exceptions/ContextException.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include "../config/xqilla_config.h"

#define YYPARSE_PARAM qp
#define YYDEBUG 1
#define YYERROR_VERBOSE

// this removes a memory leak occurring when an error is found in the query (we throw an exception from inside
// yyerror, preventing the bison-generated code from cleaning up the memory); by defining this macro we use 
// stack-based memory instead of the heap
#define YYSTACK_USE_ALLOCA	1
#if HAVE_ALLOCA_H
#include <alloca.h>
#elif defined __GNUC__
#undef alloca
#define alloca __builtin_alloca
#elif defined _AIX
#define alloca __alloca
#elif defined _MSC_VER
#include <malloc.h>
#else
#include <stddef.h>
#ifdef __cplusplus
extern "C"
#endif
void *alloca (size_t);
#endif

#define QP						((XQParserArgs*)qp)
#define CONTEXT					(QP->_context)
#define LANGUAGE					(QP->_lexer->getLanguage())
#define MEMMGR					(CONTEXT->getMemoryManager())

#define REJECT_NOT_XQUERY(where,pos) if(!QP->_lexer->isXQuery()) { yyerror(LANGUAGE, #where, (pos).first_line, (pos).first_column); }
#define REJECT_NOT_XPATH(where,pos) if(!QP->_lexer->isXPath()) { yyerror(LANGUAGE, #where, (pos).first_line, (pos).first_column); }
#define REJECT_NOT_FULLTEXT(where,pos) if(!QP->_lexer->isFullText()) { yyerror(LANGUAGE, #where, (pos).first_line, (pos).first_column); }

#define WRAP(pos,object)		(wrapForDebug((QP), (object), NULL, (pos).first_line, (pos).first_column))
#define FNWRAP(pos,name,object)	(wrapForDebug((QP),(object),(name),(pos).first_line, (pos).first_column))

#define LOCATION(pos,name) LocationInfo name(QP->_query->getFile(), (pos).first_line, (pos).first_column)

#define BIT_ORDERING_SPECIFIED	0
#define BIT_BOUNDARY_SPECIFIED	                1
#define BIT_COLLATION_SPECIFIED	                2
#define BIT_BASEURI_SPECIFIED	                3
#define BIT_CONSTRUCTION_SPECIFIED	            4
#define BIT_EMPTYORDERING_SPECIFIED             5
#define BIT_COPYNAMESPACE_SPECIFIED             6
#define BIT_DEFAULTELEMENTNAMESPACE_SPECIFIED   7
#define BIT_DEFAULTFUNCTIONNAMESPACE_SPECIFIED  8
#define BIT_DECLARE_SECOND_STEP                 9

#undef yylex
#define yylex QP->_lexer->yylex
#undef yyerror
#define yyerror QP->_lexer->error

static XMLCh szQuote[]=		 { XERCES_CPP_NAMESPACE_QUALIFIER chDoubleQuote, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szApos[]=		 { XERCES_CPP_NAMESPACE_QUALIFIER chSingleQuote, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szCurlyOpen[]=  { XERCES_CPP_NAMESPACE_QUALIFIER chOpenCurly, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szCurlyClose[]= { XERCES_CPP_NAMESPACE_QUALIFIER chCloseCurly, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szFLWOR[] =     { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_L, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_W, 
                               XERCES_CPP_NAMESPACE_QUALIFIER chLatin_O, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_R, XERCES_CPP_NAMESPACE_QUALIFIER chNull }; 
static XMLCh sz1_0[]=        { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, 
                               XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szTrue[]=       { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szFalse[]=      { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static inline bool isAllSpaces(const XMLCh* str)
{
	while(*str)
	{
		if(*str!='\r' && *str != '\n' && *str != ' ' && *str != '\t')
			return false;
		str++;
	}
	return true;
}

static inline XQNav* getNavigation(ASTNode *possibleNav, XPath2MemoryManager * expr)
{
	ASTNode* originalObj=possibleNav;
	if((unsigned int)possibleNav->getType()==ASTNode::DEBUG_HOOK)
		possibleNav=((XQDebugHook*)possibleNav)->m_impl;
	if(possibleNav->getType()==ASTNode::NAVIGATION)
		return (XQNav*)possibleNav;
	else
	{
		XQNav* nav=new (expr) XQNav(expr);
		nav->addStep(originalObj);
		return nav;
	}
}

static inline VectorOfASTNodes packageArgs(ASTNode *arg1Impl, ASTNode *arg2Impl, XPath2MemoryManager* memMgr)
{
	VectorOfASTNodes args=VectorOfASTNodes(2,(ASTNode*)NULL,XQillaAllocator<ASTNode*>(memMgr));
	args[0]=arg1Impl;
	args[1]=arg2Impl;

	return args;
}

static void merge_strings(DynamicContext* context, VectorOfASTNodes* vec, XMLCh* toBeAdded)
{
	if(vec->size()>0 && vec->back()!=0 && vec->back()->getType()==ASTNode::LITERAL)
	{
		XQLiteral *lit = (XQLiteral*)vec->back();
		const XMLCh* string=lit->getItemConstructor()->createItem(context)->asString(context);
		string=XPath2Utils::concatStrings(string,toBeAdded,context->getMemoryManager());

		AnyAtomicTypeConstructor *ic = new (context->getMemoryManager())
    		AnyAtomicTypeConstructor(
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
            string, AnyAtomicType::STRING);
    	lit->setItemConstructor(ic);
	}
	else
	{
    if(vec->size()>0 && vec->back()==0)
			vec->pop_back();
    	AnyAtomicTypeConstructor *ic = new (context->getMemoryManager())
      		AnyAtomicTypeConstructor(
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
				toBeAdded, AnyAtomicType::STRING);

		vec->push_back(new (context->getMemoryManager())
                   XQLiteral(ic, context->getMemoryManager()));
	}
}

template<typename TYPE>
TYPE *wrapForDebug(XQParserArgs *qp, TYPE* pObjToWrap,
                   const XMLCh* fnName, unsigned int line, unsigned int column)
{
  if(pObjToWrap->getLine() == 0)
    pObjToWrap->setLocationInfo(QP->_query->getFile(), line, column);
  return pObjToWrap;
}

template<>
ASTNode *wrapForDebug(XQParserArgs *qp, ASTNode* pObjToWrap,
                      const XMLCh* fnName, unsigned int line, unsigned int column)
{
  if(pObjToWrap->getLine() == 0)
    pObjToWrap->setLocationInfo(QP->_query->getFile(), line, column);

  if(!CONTEXT->isDebuggingEnabled() && !CONTEXT->getDebugCallback())
    return pObjToWrap;
  if(fnName==NULL && (unsigned int)pObjToWrap->getType()==ASTNode::DEBUG_HOOK)
    return pObjToWrap;
  return new (MEMMGR) XQDebugHook(QP->_query->getFile(), line, column, pObjToWrap, fnName, MEMMGR);
}

namespace XQParser {

%}

/* this gives a nice name to the $end token */
%token MYEOF 0										"<end of file>"

/* str */
%token <str> _QNAME_								"<qualified name>"
%token <str> _FUNCTION_CALL_						"<function>("
%token <str> _TAG_NAME_								"<tag name>"
%token <str> _ATTRIBUTE_NAME_						"<attribute name>"
%token <str> _STRING_LITERAL_						"'...'"
%token <str> _VARIABLE_								"<variable name>"
%token <str> _CHAR_									"<char>"
%token <str> _CHAR_REF_								"&#XXX;"
%token <str> _PREDEFINED_ENTITY_REF_				"<predefined entity ref>"
%token <str> _NCNAME_COLON_STAR_					"<ns>:*"
%token <str> _STAR_COLON_NCNAME_					"*:<local name>"
%token <str> _NCNAME_								"<local name>"
%token <str> _PI_TARGET_							"<processing instruction target>"
%token <str> _NAMED_ATTRIBUTE_CONSTR_				"attribute <name> {"
%token <str> _NAMED_ELEMENT_CONSTR_					"element <name> {"
%token <str> _INTEGER_NUMBER_						"<integer number>"
%token <str> _DECIMAL_NUMBER_						"<decimal number>"
%token <str> _DOUBLE_NUMBER_						"<double number>"
%token <str> _XML_COMMENT_							"<!-- ... -->"
%token <str> _CDATA_SECTION_						"<![CDATA[...]]>"
%token <str> _PROCESSING_INSTRUCTION_CONTENT_		"...?>"
%token <str> _NAMED_PROCESSING_INSTRUCTION_CONSTR_  "processing-instruction <pi> {"
%token <str> _PRAGMA_NAME_							"<pragma name>"
%token <str> _PRAGMA_CONTENT_						"<pragma content>"

/* tok */
%token _XQUERY_COMMENT_					"(: comment :)"
%token _XQUERY_WHITESPACE_ 
%token _XQUERY_ERROR_ 
%token _PRAGMA_OPEN_					"(#"
%token _PRAGMA_CLOSE_					"#)"
%token _ESCAPE_APOS_					"''"
%token _ESCAPE_QUOT_					"\"\""
%token _COMMA_							","
%token _RETURN_							"return"
%token _FOR_							"for"
%token _IN_								"in"
%token _DOLLAR_SIGN_					"$"
%token _AT_KEYWORD_						"at"
%token _LET_							"let"
%token _COLON_EQ_						":="
%token _WHERE_							"where"
%token _BY_		        				"by"
%token _ORDER_  						"order"
%token _STABLE_         				"stable"
%token _ASCENDING_						"ascending"
%token _DESCENDING_						"descending"
%token _EMPTY_KEYWORD_                  "empty"
%token _GREATEST_					    "greatest"
%token _LEAST_					        "least"
%token _COLLATION_						"collation"
%token _SOME_							"some"
%token _EVERY_							"every"
%token _SATISFIES_						"satisfies"
%token _TYPESWITCH_						"typeswitch"
%token _CASE_							"case"
%token _AS_								"as"
%token _IF_								"if"
%token _THEN_							"then"
%token _ELSE_							"else"
%token _OR_								"or"
%token _AND_							"and"
%token _INSTANCE_   					"instance"
%token _OF_            					"of"
%token _CASTABLE_					    "castable"
%token _TO_								"to"
%token _PLUS_							"+ (arithmetic operator)"
%token _MINUS_							"- (arithmetic operator)"
%token _MULTIPLY_						"* (arithmetic operator)"
%token _DIV_							"div"
%token _INTEGER_DIV_					"idiv"
%token _MOD_							"mod"
%token _UNION_							"union"
%token _VERTICAL_BAR_					"|"
%token _INTERSECT_						"intersect"
%token _EXCEPT_							"except"
%token _SLASH_							"/"
%token _SLASHSLASH_						"//"
%token _LBRACK_							"["
%token _RBRACK_							"]"
%token _VALIDATE_       				"validate"
%token _MODE_LAX_				        "lax"
%token _MODE_STRICT_			        "strict"
%token _CAST_						    "cast"
%token _TREAT_  						"treat"
%token _EQUALS_							"= (comparison operator)"
%token _NOT_EQUALS_						"!="
%token _LT_								"< (comparison operator)"
%token _LT_EQUALS_						"<="
%token _GT_								"> (comparison operator)"
%token _GT_EQUALS_						">="
%token _FORTRAN_EQ_						"eq"
%token _FORTRAN_NE_						"ne"
%token _FORTRAN_LT_						"lt"
%token _FORTRAN_LE_						"le"
%token _FORTRAN_GT_						"gt"
%token _FORTRAN_GE_						"ge"
%token _IS_								"is"
%token _LT_LT_							"<<"
%token _GT_GT_							">>"
%token _DOT_							"."
%token _AT_								"@"
%token _DOT_DOT_						".."
%token _AXIS_CHILD_						"child::"
%token _AXIS_DESCENDANT_				"descendant::"
%token _AXIS_ATTRIBUTE_					"attribute::"
%token _AXIS_SELF_						"self::"
%token _AXIS_DESCENDANT_OR_SELF_		"descendant-or-self::"
%token _AXIS_PARENT_					"parent::"
%token _AXIS_NAMESPACE_					"namespace::"
%token _STAR_							"* (wildcard)"
%token _PROCESSING_INSTRUCTION_LPAR_	"processing-instruction("
%token _COMMENT_LPAR_					"comment("
%token _TEXT_LPAR_						"text("
%token _NODE_LPAR_						"node("
%token _LPAR_							"("
%token _RPAR_							")"
%token _START_TAG_OPEN_					"<"
%token _EMPTY_TAG_CLOSE_				"/>"
%token _VALUE_INDICATOR_				"="
%token _END_TAG_OPEN_					">"
%token _TAG_CLOSE_						"</"
%token _DOCUMENT_CONSTR_				"document {"
%token _ELEMENT_CONSTR_					"element {"
%token _ATTRIBUTE_CONSTR_				"attribute {"
%token _TEXT_CONSTR_					"text {"
%token _PROCESSING_INSTRUCTION_START_	"<?"
%token _LCURLY_BRACE_ESCAPE_			"{{"
%token _RCURLY_BRACE_ESCAPE_			"}}"
%token _OPEN_QUOT_						"\" (open)"
%token _CLOSE_QUOT_						"\" (close)"
%token _OPEN_APOS_						"' (open)"
%token _CLOSE_APOS_						"' (close)"
%token _LBRACE_							"{"
%token _RBRACE_							"}"
%token _PRESERVE_						"preserve"
%token _STRIP_							"strip"
%token _NAMESPACE_						"namespace"
%token _EXPR_AS_						") as"
%token _EMPTY_							"empty-sequence"
%token _ITEM_							"item"
%token _NILLABLE_						"?"
%token _DOCUMENT_NODE_LPAR_				"document-node("
%token _ATTRIBUTE_LPAR_					"attribute("
%token _ELEMENT_LPAR_					"element("
%token _EXTERNAL_						"external"
%token _PROCESSING_INSTRUCTION_CONSTR_	"processing-instruction {"
%token _COMMENT_CONSTR_					"comment {"
%token _AXIS_ANCESTOR_OR_SELF_			"ancestor-or-self::"
%token _AXIS_ANCESTOR_					"ancestor::"
%token _AXIS_FOLLOWING_SIBLING_			"following-sibling::"
%token _AXIS_FOLLOWING_					"following::"
%token _AXIS_PRECEDING_SIBLING_			"preceding-sibling::"
%token _AXIS_PRECEDING_					"preceding::"
%token _SEMICOLON_						";"
%token _SCHEMA_ELEMENT_LPAR_			"schema-element("
%token _SCHEMA_ATTRIBUTE_LPAR_			"schema-attribute("
%token _ORDERED_						"ordered {"
%token _UNORDERED_						"unordered {"
%token _ORDERING_UNORDERED_				"unordered"
%token _ORDERING_ORDERED_				"ordered"
%token _ZERO_OR_ONE_					"? (occurrence)"
%token _ONE_OR_MORE_					"+ (occurrence)"
%token _ZERO_OR_MORE_					"* (occurrence)"
%token _ENCODING_						"encoding"
%token _NO_PRESERVE_					"no-preserve"
%token _INHERIT_						"inherit"
%token _NO_INHERIT_						"no-inherit"
%token _DECLARE_						"declare"
%token _BOUNDARYSPACE_					"boundary-space"
%token _BASEURI_						"base-uri"
%token _CONSTRUCTION_					"construction"
%token _ORDERING_						"ordering"
%token _DEFAULT_					"default"
%token _COPY_NAMESPACES_				"copy-namespaces"
%token _VARIABLE_KEYWORD_				"variable"
%token _OPTION_					        "option"
%token _XQUERY_							"xquery"
%token _VERSION_						"version"
%token _IMPORT_						    "import"
%token _SCHEMA_					        "schema"
%token _MODULE_					        "module"
%token _ELEMENT_				        "element"
%token _FUNCTION_				        "function"
%token _SCORE_                                          "score"
%token _FTCONTAINS_                                     "ftcontains"
%token _WEIGHT_                                         "weight"
%token _DECLARE_FT_OPTION_                              "declare ft-option"
%token _DOUBLE_VERTICAL_BAR_                            "||"
%token _DOUBLE_AMPERSAND_                               "&&"
%token _NOT_IN_                                         "not in"
%token _BANG_                                           "!"
%token _WINDOW_                                         "window"
%token _DISTANCE_                                       "distance"
%token _OCCURS_                                         "occurs"
%token _TIMES_                                          "times"
%token _SAME_                                           "same"
%token _DIFFERENT_                                      "different"
%token _AT_START_                                       "at start"
%token _AT_END_                                         "at end"
%token _ENTIRE_CONTENT_                                 "entire content"
%token _LOWERCASE_                                      "lowercase"
%token _UPPERCASE_                                      "uppercase"
%token _CASE_INSENSITIVE_                               "case insensitive"
%token _CASE_SENSITIVE_                                 "case sensitive"
%token _WITH_DIACRITICS_                                "with diacritics"
%token _WITHOUT_DIACRITICS_                             "without diacritics"
%token _DIACRITICS_SENSITIVE_                           "diacritics sensitive"
%token _DIACRITICS_INSENSITIVE_                         "diacritics insensitive"
%token _WITH_STEMMING_                                  "with stemming"
%token _WITHOUT_STEMMING_                               "without stemming"
%token _WITH_THESAURUS_                                 "with thesaurus"
%token _WITHOUT_THESAURUS_                              "without thesaurus"
%token _RELATIONSHIP_                                   "relationship"
%token _LEVELS_                                         "levels"
%token _WITH_STOP_WORDS_                                "with stop words"
%token _WITHOUT_STOP_WORDS_                             "without stop words"
%token _WITH_DEFAULT_STOP_WORDS_                        "with default stop words"
%token _LANGUAGE_                                       "language"
%token _WITH_WILDCARDS_                                 "with wildcards"
%token _WITHOUT_WILDCARDS_                              "without wildcards"
%token _ANY_                                            "any"
%token _ANY_WORD_                                       "any word"
%token _ALL_                                            "all"
%token _ALL_WORDS_                                      "all words"
%token _PHRASE_                                         "phrase"
%token _EXACTLY_                                        "exactly"
%token _AT_LEAST_                                       "at least"
%token _AT_MOST_                                        "at most"
%token _FROM_                                           "from"
%token _WORDS_                                          "words"
%token _SENTENCES_                                      "sentences"
%token _PARAGRAPHS_                                     "paragraphs"
%token _SENTENCE_                                       "sentence"
%token _PARAGRAPH_                                      "paragraph"
%token _WITHOUT_CONTENT_                                "without content"
%token _EOF_

%type <functDecl>			FunctionDecl 
%type <funcParam>			Param
%type <funcParams>			ParamList
%type <astNode>			Expr ExprSingle OrExpr AndExpr EnclosedExpr FLWORExpr IfExpr ComparisonExpr DecimalLiteral VarRef
%type <astNode>			RangeExpr AdditiveExpr MultiplicativeExpr UnionExpr QuantifiedExpr StringLiteral Literal ContextItemExpr
%type <astNode>			UnaryExpr ValidateExpr CastExpr TreatExpr IntersectExceptExpr ParenthesizedExpr PrimaryExpr FunctionCall
%type <astNode>			RelativePathExpr StepExpr AxisStep FilterExpr TypeswitchExpr ValueExpr PathExpr NumericLiteral IntegerLiteral 
%type <astNode>			CastableExpr Constructor ComputedConstructor DirElemConstructor DirCommentConstructor DirPIConstructor  
%type <astNode>			CompElemConstructor CompTextConstructor CompPIConstructor CompCommentConstructor OrderedExpr UnorderedExpr
%type <astNode>			CompAttrConstructor WhereClause CompDocConstructor DoubleLiteral InstanceofExpr DirectConstructor 
%type <astNode>			ContentExpr ExtensionExpr CdataSection FTContainsExpr FTIgnoreOption
%type <astNode>      		ForwardStep ReverseStep AbbrevForwardStep AbbrevReverseStep

%type <ftselection>     FTSelection FTWords FTWordsSelection FTUnaryNot FTMildnot FTAnd FTOr
%type <ftoption>        FTProximity
%type <ftoptionlist>    FTSelectionOptions
%type <ftanyalloption>  FTAnyallOption
%type <ftrange>         FTRange
%type <ftunit>          FTUnit FTBigUnit

%type <itemList>			DirElementContent DirAttributeList QuotAttrValueContent AposAttrValueContent DirAttributeValue FunctionCallArgumentList
%type <predicates>    PredicateList
%type <axis>          		ForwardAxis ReverseAxis
%type <nodeTest>			NodeTest NameTest KindTest AttributeTest SchemaAttributeTest
%type <nodeTest>			Wildcard PITest CommentTest TextTest AnyKindTest ElementTest DocumentTest SchemaElementTest
%type <qName>				QName AtomicType TypeName ElementName AttributeName ElementNameOrWildcard AttribNameOrWildcard AttributeDeclaration ElementDeclaration
%type <sequenceType>		SequenceType TypeDeclaration SingleType
%type <occurrence>			OccurrenceIndicator
%type <itemType>			ItemType 
%type <variableBinding>		ForBinding LetBinding QuantifyBinding
%type <variableBindingList> FlworExprForLetList ForOrLetClause ForClause LetClause ForBindingList LetBindingList QuantifyBindingList
%type <clause>				CaseClause
%type <clauseList>			CaseClauseList
%type <sortModifier>		OrderDirection EmptyHandling
%type <sortSpecList>		OrderSpecList 
%type <sortSpec>			OrderSpec
%type <sort>				OrderByClause
%type <stringList>			ResourceLocations
%type <str>					PositionalVar SchemaPrefix CommonContent URILiteral FTScoreVar
%type <str>                 PreserveMode InheritMode

%start Module

%pure_parser

%%

// Parser rules go here.

// [1]    Module    ::=      VersionDecl? (MainModule | LibraryModule)
Module:
	VersionDecl MainModule
	| VersionDecl LibraryModule
	| MainModule
	| LibraryModule
	;

// [2]    VersionDecl    ::=    <"xquery" "version" StringLiteral> ("encoding" StringLiteral)? Separator
VersionDecl:
	_XQUERY_ _VERSION_ _STRING_LITERAL_ Separator
	{
    REJECT_NOT_XQUERY(VersionDecl, @1);

		if(!XPath2Utils::equals($3,sz1_0))
			yyerror(@2, "This XQuery processor only supports version 1.0 of the specs [err:XQST0031]");
	}
	| _XQUERY_ _VERSION_ _STRING_LITERAL_ _ENCODING_ _STRING_LITERAL_ Separator
	{
    REJECT_NOT_XQUERY(VersionDecl, @1);

		if(!XPath2Utils::equals($3,sz1_0))
			yyerror(@2, "This XQuery processor only supports version 1.0 of the specs [err:XQST0031]");
        bool bValidEnc=false;
        if(($5[0] >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A && $5[0] <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_Z) ||
           ($5[0] >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a && $5[0] <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z))
        {
            bValidEnc=true;
            int nLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen($5);
            for(int i=1;i<nLen;i++)
            {
                if(($5[i] >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A && $5[i] <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_Z) ||
                   ($5[i] >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a && $5[i] <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z) ||
                   ($5[i] >= XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0 && $5[i] <= XERCES_CPP_NAMESPACE_QUALIFIER chDigit_9) ||
                   $5[i] == XERCES_CPP_NAMESPACE_QUALIFIER chPeriod ||
                   $5[i] == XERCES_CPP_NAMESPACE_QUALIFIER chDash)
                {
                    continue;
                }
                bValidEnc=false;
                break;
            }
        }
        if(!bValidEnc)
          yyerror(@5, "The specified encoding does not conform to the definition of EncName [err:XQST0087]");
		// TODO: store the encoding somewhere
	}
	;

// [3]    MainModule    ::=    Prolog QueryBody 
MainModule:
	Prolog QueryBody
	{
	}
	;

// [4]    LibraryModule    ::=    ModuleDecl Prolog 
LibraryModule:
	ModuleDecl Prolog
	{
	}
	;

// [5]    ModuleDecl    ::=    <"module" "namespace"> NCName "=" URILiteral Separator
ModuleDecl:
	_MODULE_ _NAMESPACE_ _NCNAME_ _EQUALS_ URILiteral Separator
	{
		if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen($5)==0)
			yyerror(@5, "The literal that specifies the namespace of a module must not be of zero length [err:XQST0088]");
		QP->_query->setIsLibraryModule();
		QP->_query->setModuleTargetNamespace($5);
		CONTEXT->setNamespaceBinding($3,$5);
	}
	;

// [6]    Prolog    ::=    ((Setter | Import | NamespaceDecl | DefaultNamespaceDecl) Separator)* 
//						   ((VarDecl | FunctionDecl | OptionDecl | FTOptionDecl) Separator)*
Prolog:
	/* empty */
	| Prolog Setter Separator
    {
		if(QP->_flags.get(BIT_DECLARE_SECOND_STEP))
			yyerror(@2, "Prolog contains a setter declaration after a variable, function or option declaration");
    }
	| Prolog Import Separator
    {
		if(QP->_flags.get(BIT_DECLARE_SECOND_STEP))
			yyerror(@2, "Prolog contains an import declaration after a variable, function or option declaration");
    }
	| Prolog NamespaceDecl Separator
    {
		if(QP->_flags.get(BIT_DECLARE_SECOND_STEP))
			yyerror(@2, "Prolog contains a namespace declaration after a variable, function or option declaration");
    }
	| Prolog DefaultNamespaceDecl Separator
    {
		if(QP->_flags.get(BIT_DECLARE_SECOND_STEP))
			yyerror(@2, "Prolog contains a default namespace declaration after a variable, function or option declaration");
    }
	| Prolog VarDecl Separator
    {
		QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    }
	| Prolog FunctionDecl Separator
	{
		QP->_flags.set(BIT_DECLARE_SECOND_STEP);
		XQUserFunction* decl=$2;
		CONTEXT->addCustomFunction(decl);
		QP->_query->addFunction(decl);
	}
	| Prolog OptionDecl Separator
    {
		QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    }
	| Prolog FTOptionDecl Separator
  {
		QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
;

// [7]    Setter    ::=   BoundarySpaceDecl  
//						| DefaultCollationDecl 
//						| BaseURIDecl 
//						| ConstructionDecl 
//						| OrderingModeDecl
//						| EmptyOrderDecl 
//						| CopyNamespacesDecl 
Setter:
	BoundarySpaceDecl  
	| DefaultCollationDecl 
	| BaseURIDecl 
	| ConstructionDecl 
	| OrderingModeDecl 
	| EmptyOrderDecl 
	| CopyNamespacesDecl 
;	

// [8]    Import    ::=    SchemaImport | ModuleImport 
Import:
	SchemaImport
	| ModuleImport
;

// [9]    Separator    ::=    ";" 
Separator:
	_SEMICOLON_
	;

// [10]    NamespaceDecl    ::=    <"declare" "namespace"> NCName "=" URILiteral 
NamespaceDecl:
	  _DECLARE_ _NAMESPACE_ _NCNAME_ _EQUALS_ URILiteral 
		{

          REJECT_NOT_XQUERY(NamespaceDecl, @1);
          // if it has already bound, report an error
          if(QP->_namespaceDecls.containsKey($3))
            yyerror(@3, "Namespace prefix has already been bound to a namespace [err:XQST0033]");
          QP->_namespaceDecls.put($3,NULL);
          CONTEXT->setNamespaceBinding($3,$5);
		}
	;

// [11]    BoundarySpaceDecl     ::=    <"declare" "boundary-space"> ("preserve" |  "strip") 
BoundarySpaceDecl :
	  _DECLARE_ _BOUNDARYSPACE_ _PRESERVE_
		{
      REJECT_NOT_XQUERY(BoundarySpaceDecl, @1);

		    if(QP->_flags.get(BIT_BOUNDARY_SPECIFIED))
			    yyerror(@1, "Prolog contains more than one boundary space declaration [err:XQST0068]");
		    QP->_flags.set(BIT_BOUNDARY_SPECIFIED);
			CONTEXT->setPreserveBoundarySpace(true);
		}
	| _DECLARE_ _BOUNDARYSPACE_ _STRIP_
		{
      REJECT_NOT_XQUERY(BoundarySpaceDecl, @1);

		    if(QP->_flags.get(BIT_BOUNDARY_SPECIFIED))
			    yyerror(@1, "Prolog contains more than one boundary space declaration [err:XQST0068]");
		    QP->_flags.set(BIT_BOUNDARY_SPECIFIED);
			CONTEXT->setPreserveBoundarySpace(false);
		}
	;

// [12]    DefaultNamespaceDecl    ::=    (<"declare" "default" "element"> |  <"declare" "default" "function">) "namespace" URILiteral 
DefaultNamespaceDecl:
	  _DECLARE_ _DEFAULT_ _ELEMENT_ _NAMESPACE_ URILiteral
		{
      REJECT_NOT_XQUERY(DefaultNamespaceDecl, @1);

		    if(QP->_flags.get(BIT_DEFAULTELEMENTNAMESPACE_SPECIFIED))
			    yyerror(@1, "Prolog contains more than one default element namespace declaration [err:XQST0066]");
		    QP->_flags.set(BIT_DEFAULTELEMENTNAMESPACE_SPECIFIED);
			CONTEXT->setDefaultElementAndTypeNS($5);
		}
	| _DECLARE_ _DEFAULT_ _FUNCTION_ _NAMESPACE_ URILiteral
		{ 
      REJECT_NOT_XQUERY(DefaultNamespaceDecl, @1);

		    if(QP->_flags.get(BIT_DEFAULTFUNCTIONNAMESPACE_SPECIFIED))
			    yyerror(@1, "Prolog contains more than one default function namespace declaration [err:XQST0066]");
		    QP->_flags.set(BIT_DEFAULTFUNCTIONNAMESPACE_SPECIFIED);
			CONTEXT->setDefaultFuncNS($5);
		}
	;

// [13]   	OptionDecl	   ::=   	<"declare" "option"> QName StringLiteral
OptionDecl:
	  _DECLARE_ _OPTION_ _QNAME_ _STRING_LITERAL_
      {
      REJECT_NOT_XQUERY(OptionDecl, @1);

        // validate the QName
		QualifiedName qName($3);
        const XMLCh* prefix=qName.getPrefix();
        if(prefix==NULL || *prefix==0)
			yyerror(@3, "The option name must have a prefix [err:XPST0081]");

		try
		{
      LOCATION(@3, loc);
			CONTEXT->getUriBoundToPrefix(prefix, &loc);
		}
		catch(NamespaceLookupException&)
		{
			yyerror(@3, "The option name is using an undefined namespace prefix [err:XPST0081]");
		}
      }
	;

// [14]    FTOptionDecl    ::=    "declare" "ft-option" FTMatchOption
FTOptionDecl:
	  _DECLARE_FT_OPTION_ FTMatchOption
	{
      REJECT_NOT_XQUERY(FTOptionDecl, @1);
      REJECT_NOT_FULLTEXT(FTOptionDecl, @1);
	}
	;

// [15]    OrderingModeDecl    ::=    <"declare" "ordering"> ("ordered" | "unordered") 
OrderingModeDecl:
	_DECLARE_ _ORDERING_ _ORDERING_ORDERED_
	{
    REJECT_NOT_XQUERY(OrderingModeDecl, @1);

		if(QP->_flags.get(BIT_ORDERING_SPECIFIED))
			yyerror(@1, "Prolog contains more than one ordering mode declaration [err:XQST0065]");
		QP->_flags.set(BIT_ORDERING_SPECIFIED);
		CONTEXT->setNodeSetOrdering(StaticContext::ORDERING_ORDERED);
	}
	| _DECLARE_ _ORDERING_ _ORDERING_UNORDERED_
	{
    REJECT_NOT_XQUERY(OrderingModeDecl, @1);

		if(QP->_flags.get(BIT_ORDERING_SPECIFIED))
			yyerror(@1, "Prolog contains more than one ordering mode declaration [err:XQST0065]");
		QP->_flags.set(BIT_ORDERING_SPECIFIED);
		CONTEXT->setNodeSetOrdering(StaticContext::ORDERING_UNORDERED);
	}
	;

// [16]    EmptyOrderDecl    ::=    "declare" "default" "order" "empty" ("greatest" | "least")
EmptyOrderDecl:
	_DECLARE_ _DEFAULT_ _ORDER_ _EMPTY_KEYWORD_ _GREATEST_
	{ 
    REJECT_NOT_XQUERY(EmptyOrderDecl, @1);

		if(QP->_flags.get(BIT_EMPTYORDERING_SPECIFIED))
			yyerror(@1, "Prolog contains more than one empty ordering mode declaration [err:XQST0069]");
		QP->_flags.set(BIT_EMPTYORDERING_SPECIFIED);
		CONTEXT->setDefaultFLWOROrderingMode(StaticContext::FLWOR_ORDER_EMPTY_GREATEST);
	}
	| _DECLARE_ _DEFAULT_ _ORDER_ _EMPTY_KEYWORD_ _LEAST_
	{ 
    REJECT_NOT_XQUERY(EmptyOrderDecl, @1);

		if(QP->_flags.get(BIT_EMPTYORDERING_SPECIFIED))
			yyerror(@1, "Prolog contains more than one empty ordering mode declaration [err:XQST0069]");
		QP->_flags.set(BIT_EMPTYORDERING_SPECIFIED);
		CONTEXT->setDefaultFLWOROrderingMode(StaticContext::FLWOR_ORDER_EMPTY_LEAST);
	}
	;

// [17]    CopyNamespacesDecl    ::=     <"declare" "copy-namespaces"> PreserveMode "," InheritMode
CopyNamespacesDecl:
	_DECLARE_ _COPY_NAMESPACES_ PreserveMode _COMMA_ InheritMode
    {
    REJECT_NOT_XQUERY(CopyNamespacesDecl, @1);

		if(QP->_flags.get(BIT_COPYNAMESPACE_SPECIFIED))
			yyerror(@1, "Prolog contains more than one copy namespace declaration [err:XQST0055]");
		QP->_flags.set(BIT_COPYNAMESPACE_SPECIFIED);
		CONTEXT->setPreserveNamespaces(XPath2Utils::equals($3,szTrue));
		CONTEXT->setInheritNamespaces(XPath2Utils::equals($5,szTrue));
    }
;

// [18]   	PreserveMode	   ::=   	"preserve" | "no-preserve"
PreserveMode:
	  _PRESERVE_
	{
        $$ = szTrue;
	}
	| _NO_PRESERVE_
	{
        $$ = szFalse;
	}
	;

// [19]   	InheritMode	   ::=   	"inherit" | "no-inherit"
InheritMode:
	  _INHERIT_
	{
        $$ = szTrue;
	}
	| _NO_INHERIT_
	{
        $$ = szFalse;
	}
	;

// [20]    DefaultCollationDecl    ::=    <"declare" "default" "collation"> URILiteral 
DefaultCollationDecl:
		_DECLARE_ _DEFAULT_ _COLLATION_ URILiteral
		{
      REJECT_NOT_XQUERY(DefaultCollationDecl, @1);

		    if(QP->_flags.get(BIT_COLLATION_SPECIFIED))
			    yyerror(@1, "Prolog contains more than one default collation declaration [err:XQST0038]");
		    QP->_flags.set(BIT_COLLATION_SPECIFIED);
            try
            {
              LOCATION(@4, loc);
                CONTEXT->getCollation($4, &loc);
            }
            catch(ContextException&)
            {
			    yyerror(@4, "The specified collation does not exist [err:XQST0038]");
            }
			CONTEXT->setDefaultCollation($4);
		}
	  ;

// [21]    BaseURIDecl    ::=    <"declare" "base-uri"> URILiteral
BaseURIDecl:
		_DECLARE_ _BASEURI_ URILiteral
		{
      REJECT_NOT_XQUERY(BaseURIDecl, @1);

		    if(QP->_flags.get(BIT_BASEURI_SPECIFIED))
			    yyerror(@1, "Prolog contains more than one base URI declaration [err:XQST0032]");
		    QP->_flags.set(BIT_BASEURI_SPECIFIED);
			CONTEXT->setBaseURI($3);
		}
	  ;

// [22]    SchemaImport    ::=    <"import" "schema"> SchemaPrefix? URILiteral (<"at" URILiteral> ("," URILiteral)*)?
SchemaImport:
        _IMPORT_ _SCHEMA_ SchemaPrefix URILiteral
		{
      REJECT_NOT_XQUERY(SchemaImport, @1);

			if(XPath2Utils::equals($3, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString))
				CONTEXT->setDefaultElementAndTypeNS($4);
			else if(XPath2Utils::equals($4, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString))
				yyerror(@1, "A schema that has no target namespace cannot be bound to non-empty prefix [err:XQST0057]");
			else
				CONTEXT->setNamespaceBinding($3,$4);
            try {
			  CONTEXT->addSchemaLocation($4,NULL);
            } catch(XQException& e) {
              if(e.getXQueryLine() == 0)
                e.setXQueryPosition(QP->_query->getFile(), @1.first_line, @1.first_column);
              throw e;
            }
		}
	  | _IMPORT_ _SCHEMA_ SchemaPrefix URILiteral ResourceLocations
		{
      REJECT_NOT_XQUERY(SchemaImport, @1);

			if(XPath2Utils::equals($3, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString))
				CONTEXT->setDefaultElementAndTypeNS($4);
			else if(XPath2Utils::equals($4, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString))
				yyerror(@1, "A schema that has no target namespace cannot be bound to non-empty prefix [err:XQST0057]");
			else
				CONTEXT->setNamespaceBinding($3,$4);
            try {
			  CONTEXT->addSchemaLocation($4,$5);
            } catch(XQException& e) {
              if(e.getXQueryLine() == 0)
                e.setXQueryPosition(QP->_query->getFile(), @1.first_line, @1.first_column);
              throw e;
            }
		}
	  | _IMPORT_ _SCHEMA_ URILiteral
		{ 
      REJECT_NOT_XQUERY(SchemaImport, @1);

            try {
			  CONTEXT->addSchemaLocation($3,NULL);
            } catch(XQException& e) {
              if(e.getXQueryLine() == 0)
                e.setXQueryPosition(QP->_query->getFile(), @1.first_line, @1.first_column);
              throw e;
            }
		}
	  | _IMPORT_ _SCHEMA_ URILiteral ResourceLocations
		{
      REJECT_NOT_XQUERY(SchemaImport, @1);

            try {
			  CONTEXT->addSchemaLocation($3,$4);
            } catch(XQException& e) {
              if(e.getXQueryLine() == 0)
                e.setXQueryPosition(QP->_query->getFile(), @1.first_line, @1.first_column);
              throw e;
            }
		}
	  ;

ResourceLocations:
	_AT_KEYWORD_ URILiteral
	{
		$$ = new (MEMMGR) VectorOfStrings(XQillaAllocator<const XMLCh*>(MEMMGR));
		$$->push_back($2);
	}
	| ResourceLocations _COMMA_ URILiteral
	{
		$1->push_back($3);
		$$ = $1;
	}
;

// [23]    SchemaPrefix    ::=    ("namespace" NCName "=") |  (<"default" "element"> "namespace") 
SchemaPrefix:
	  _NAMESPACE_ _NCNAME_ _EQUALS_
		{
			$$ = $2;
		}
	| _DEFAULT_ _ELEMENT_ _NAMESPACE_
		{
			$$ = (XMLCh*)XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
		}
	;

// [24]    ModuleImport    ::=    <"import" "module"> ("namespace" NCName "=")? URILiteral (<"at" URILiteral> ("," URILiteral)*)?
ModuleImport:
	_IMPORT_ _MODULE_ _NAMESPACE_ _NCNAME_ _EQUALS_ URILiteral ResourceLocations
	{
    REJECT_NOT_XQUERY(ModuleImport, @1);

		if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen($6)==0)
			yyerror(@6, "The literal that specifies the target namespace in a module import must not be of zero length [err:XQST0088]");
		CONTEXT->setNamespaceBinding($4,$6);
        try {
		  QP->_query->importModule(LANGUAGE,$6,$7,CONTEXT);
        } catch(XQException& e) {
          if(e.getXQueryLine() == 0)
            e.setXQueryPosition(QP->_query->getFile(), @1.first_line, @1.first_column);
          throw e;
        }
	}
	| _IMPORT_ _MODULE_ _NAMESPACE_ _NCNAME_ _EQUALS_ URILiteral 
	{
    REJECT_NOT_XQUERY(ModuleImport, @1);

		if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen($6)==0)
			yyerror(@6, "The literal that specifies the target namespace in a module import must not be of zero length [err:XQST0088]");
		CONTEXT->setNamespaceBinding($4,$6);
        try {
		  QP->_query->importModule(LANGUAGE,$6,NULL,CONTEXT);
        } catch(XQException& e) {
          if(e.getXQueryLine() == 0)
            e.setXQueryPosition(QP->_query->getFile(), @1.first_line, @1.first_column);
          throw e;
        }
	}
	| _IMPORT_ _MODULE_ URILiteral ResourceLocations
	{
    REJECT_NOT_XQUERY(ModuleImport, @1);

		if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen($3)==0)
			yyerror(@3, "The literal that specifies the target namespace in a module import must not be of zero length [err:XQST0088]");
        try {
		  QP->_query->importModule(LANGUAGE,$3,$4,CONTEXT);
        } catch(XQException& e) {
          if(e.getXQueryLine() == 0)
            e.setXQueryPosition(QP->_query->getFile(), @1.first_line, @1.first_column);
          throw e;
        }
	}
	| _IMPORT_ _MODULE_ URILiteral 
	{
    REJECT_NOT_XQUERY(ModuleImport, @1);

		if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen($3)==0)
			yyerror(@3, "The literal that specifies the target namespace in a module import must not be of zero length [err:XQST0088]");
        try {
		  QP->_query->importModule(LANGUAGE,$3,NULL,CONTEXT);
        } catch(XQException& e) {
          if(e.getXQueryLine() == 0)
            e.setXQueryPosition(QP->_query->getFile(), @1.first_line, @1.first_column);
          throw e;
        }
	}
	;

// [25]    VarDecl    ::=    <"declare" "variable" "$"> VarName TypeDeclaration? ((":=" ExprSingle) | "external")
VarDecl:
	_DECLARE_ _VARIABLE_KEYWORD_ _DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration _COLON_EQ_ ExprSingle
	{
    REJECT_NOT_XQUERY(VarDecl, @1);

		XQGlobalVariable* var=WRAP(@1, new (MEMMGR) XQGlobalVariable($4,$5, WRAP(@7, $7),MEMMGR));
		QP->_query->addVariable(var);
	}
	| _DECLARE_ _VARIABLE_KEYWORD_ _DOLLAR_SIGN_ _VARIABLE_ _COLON_EQ_ ExprSingle
	{
    REJECT_NOT_XQUERY(VarDecl, @1);

		XQGlobalVariable* var=WRAP(@1, new (MEMMGR) XQGlobalVariable($4,WRAP(@1, new (MEMMGR) SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR)),
                                                            WRAP(@6, $6),MEMMGR));
		QP->_query->addVariable(var);
	}
	| _DECLARE_ _VARIABLE_KEYWORD_ _DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration _EXTERNAL_
	{
    REJECT_NOT_XQUERY(VarDecl, @1);

		XQGlobalVariable* var=WRAP(@1, new (MEMMGR) XQGlobalVariable($4,$5,NULL,MEMMGR));
		QP->_query->addVariable(var);
	}
	| _DECLARE_ _VARIABLE_KEYWORD_ _DOLLAR_SIGN_ _VARIABLE_ _EXTERNAL_
	{
    REJECT_NOT_XQUERY(VarDecl, @1);

		XQGlobalVariable* var=WRAP(@1, new (MEMMGR) XQGlobalVariable($4,WRAP(@1, new (MEMMGR) SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR)),
                                                            NULL,MEMMGR));
		QP->_query->addVariable(var);
	}
	;

// [26]    ConstructionDecl    ::=    <"declare" "construction"> ("preserve" | "strip") 
ConstructionDecl:
	_DECLARE_ _CONSTRUCTION_ _PRESERVE_
	{
    REJECT_NOT_XQUERY(ConstructionDecl, @1);

		if(QP->_flags.get(BIT_CONSTRUCTION_SPECIFIED))
			yyerror(@1, "Prolog contains more than one construction mode declaration [err:XQST0067]");
		QP->_flags.set(BIT_CONSTRUCTION_SPECIFIED);
		CONTEXT->setConstructionMode(StaticContext::CONSTRUCTION_MODE_PRESERVE);
	}
	| _DECLARE_ _CONSTRUCTION_ _STRIP_
	{
    REJECT_NOT_XQUERY(ConstructionDecl, @1);

		if(QP->_flags.get(BIT_CONSTRUCTION_SPECIFIED))
			yyerror(@1, "Prolog contains more than one construction mode declaration [err:XQST0067]");
		QP->_flags.set(BIT_CONSTRUCTION_SPECIFIED);
		CONTEXT->setConstructionMode(StaticContext::CONSTRUCTION_MODE_STRIP);
	}
	;

// [27]    FunctionDecl    ::=    <"declare" "function"> <QName "("> ParamList? (")" |  (<")" "as"> SequenceType)) 
//										(EnclosedExpr | "external")
FunctionDecl:
	  _DECLARE_ _FUNCTION_ _FUNCTION_CALL_ ParamList _RPAR_ EnclosedExpr
		{
      REJECT_NOT_XQUERY(FunctionDecl, @1);

			$$ = WRAP(@1, new (MEMMGR) XQUserFunction($3,$4,WRAP(@6, $6),NULL, CONTEXT));
		}
	| _DECLARE_ _FUNCTION_ _FUNCTION_CALL_ _RPAR_ EnclosedExpr
		{
      REJECT_NOT_XQUERY(FunctionDecl, @1);

			$$ = WRAP(@1, new (MEMMGR) XQUserFunction($3,NULL,WRAP(@5, $5),NULL, CONTEXT));
		}
	| _DECLARE_ _FUNCTION_ _FUNCTION_CALL_ ParamList _EXPR_AS_ SequenceType EnclosedExpr
		{
      REJECT_NOT_XQUERY(FunctionDecl, @1);

			$$ = WRAP(@1, new (MEMMGR) XQUserFunction($3,$4,WRAP(@7, $7),$6, CONTEXT));
		}
	| _DECLARE_ _FUNCTION_ _FUNCTION_CALL_ _EXPR_AS_ SequenceType EnclosedExpr
		{
      REJECT_NOT_XQUERY(FunctionDecl, @1);

			$$ = WRAP(@1, new (MEMMGR) XQUserFunction($3,NULL,WRAP(@6, $6),$5, CONTEXT));
		}
	| _DECLARE_ _FUNCTION_ _FUNCTION_CALL_ ParamList _RPAR_ _EXTERNAL_
		{
      REJECT_NOT_XQUERY(FunctionDecl, @1);

			$$ = WRAP(@1, new (MEMMGR) XQUserFunction($3,$4,NULL,NULL, CONTEXT));
		}
	| _DECLARE_ _FUNCTION_ _FUNCTION_CALL_ _RPAR_ _EXTERNAL_
		{
      REJECT_NOT_XQUERY(FunctionDecl, @1);

			$$ = WRAP(@1, new (MEMMGR) XQUserFunction($3,NULL,NULL,NULL, CONTEXT));
		}
	| _DECLARE_ _FUNCTION_ _FUNCTION_CALL_ ParamList _EXPR_AS_ SequenceType _EXTERNAL_
		{
      REJECT_NOT_XQUERY(FunctionDecl, @1);

			$$ = WRAP(@1, new (MEMMGR) XQUserFunction($3,$4,NULL,$6, CONTEXT));
		}
	| _DECLARE_ _FUNCTION_ _FUNCTION_CALL_ _EXPR_AS_ SequenceType _EXTERNAL_
		{
      REJECT_NOT_XQUERY(FunctionDecl, @1);

			$$ = WRAP(@1, new (MEMMGR) XQUserFunction($3,NULL,NULL,$5, CONTEXT));
		}
	;

// [28]    ParamList    ::=    Param ("," Param)* 
ParamList:
        ParamList _COMMA_ Param
		{
			$1->push_back($3);
			$$ = $1;
		}
      | Param
		{
			XQUserFunction::VectorOfFunctionParameters* paramList = new (MEMMGR) XQUserFunction::VectorOfFunctionParameters(XQillaAllocator<XQUserFunction::XQFunctionParameter*>(MEMMGR));
			paramList->push_back($1);
			$$ = paramList;
		}
      ;

// [29]    Param    ::=    "$" VarName TypeDeclaration? 
Param:
        _DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration
		{
			$$ = new (MEMMGR) XQUserFunction::XQFunctionParameter($2,$3,MEMMGR);
		}
	  | _DOLLAR_SIGN_ _VARIABLE_
		{
			$$ = new (MEMMGR) XQUserFunction::XQFunctionParameter($2,WRAP(@1, new (MEMMGR) SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR)) ,MEMMGR);
		}
      ;

// [30]    EnclosedExpr    ::=    "{" Expr "}" 
EnclosedExpr:
		_LBRACE_ Expr _RBRACE_
		{ 
			$$ = WRAP(@2, $2); 
		}
      ;

// [31]    QueryBody    ::=    Expr
QueryBody:
	Expr
	{
	    QP->_query->setQueryBody($1);
	}
	;

// [32]    Expr    ::=    ExprSingle ("," ExprSingle)* 
Expr:
		Expr _COMMA_ ExprSingle
		{
			ASTNode* prevExpr=$1;
			if((unsigned int)prevExpr->getType()==ASTNode::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()==ASTNode::PARENTHESIZED)
			{
				((XQParenthesizedExpr *)prevExpr)->addItem($3);
				$$ = $1;
			}
			else
			{
				XQParenthesizedExpr *dis = new (MEMMGR) XQParenthesizedExpr(MEMMGR);
				dis->addItem($1);
				dis->addItem($3);
				$$ = dis;
			}
		}
	  | ExprSingle
		{
			$$ = $1;
		}
	  ;

// [33]     ExprSingle    ::=    FLWORExpr |  QuantifiedExpr |  TypeswitchExpr |  IfExpr |  OrExpr 
ExprSingle:
		FLWORExpr
	  | QuantifiedExpr
	  | TypeswitchExpr
	  | IfExpr
	  | OrExpr
	  ;

// [34]    FLWORExpr    ::=    (ForClause |  LetClause)+ WhereClause? OrderByClause? "return" ExprSingle 
FLWORExpr:
	    FlworExprForLetList WhereClause OrderByClause _RETURN_ ExprSingle
		{
			$$ = FNWRAP(@1, szFLWOR, new (MEMMGR) XQFLWOR($1, $2, $3, WRAP(@4, $5), MEMMGR));
		}
	  | FlworExprForLetList WhereClause _RETURN_ ExprSingle
		{
			$$ = FNWRAP(@1, szFLWOR, new (MEMMGR) XQFLWOR($1, $2, NULL, WRAP(@3, $4), MEMMGR));
		}
	  | FlworExprForLetList OrderByClause _RETURN_ ExprSingle
		{
			$$ = FNWRAP(@1, szFLWOR, new (MEMMGR) XQFLWOR($1, NULL, $2, WRAP(@3, $4), MEMMGR));
		}
	  | FlworExprForLetList _RETURN_ ExprSingle
		{
			$$ = FNWRAP(@1, szFLWOR, new (MEMMGR) XQFLWOR($1, NULL, NULL, WRAP(@2, $3), MEMMGR));
		}
	;

FlworExprForLetList:
		FlworExprForLetList ForOrLetClause
		{
			$$ = $1;
			for (VectorOfVariableBinding::iterator it = $2->begin(); it != $2->end (); ++it) 
				$1->push_back(*it);
		}
	  | ForOrLetClause
      ;

ForOrLetClause:
		ForClause
      | LetClause
      ;

// [35]    ForClause    ::=    "for" "$" VarName TypeDeclaration? PositionalVar? FTScoreVar? "in" ExprSingle
//                                        ("," "$" VarName TypeDeclaration? PositionalVar? FTScoreVar? "in" ExprSingle)*
ForClause:
		_FOR_ ForBindingList
		{
			$$ = $2;
		}
	  ;

ForBindingList:
		ForBindingList _COMMA_ ForBinding
		{
			$$->push_back($3);
		}
	  | ForBinding
		{
			$$ = new (MEMMGR) VectorOfVariableBinding(XQillaAllocator<XQVariableBinding*>(MEMMGR));
			$$->push_back($1);
		}
	  ;

ForBinding:
	    _DOLLAR_SIGN_ _VARIABLE_ _IN_ ExprSingle 
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$4));
		}
	  | _DOLLAR_SIGN_ _VARIABLE_ FTScoreVar _IN_ ExprSingle 
		{
      // TBD FTScoreVar
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$5));
		}
	  | _DOLLAR_SIGN_ _VARIABLE_ PositionalVar _IN_ ExprSingle 
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$5,$3));
		}
	  | _DOLLAR_SIGN_ _VARIABLE_ PositionalVar FTScoreVar _IN_ ExprSingle 
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$6,$3));
		}
	  |	_DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration _IN_ ExprSingle 
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$5,NULL,$3));
		}
	  |	_DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration FTScoreVar _IN_ ExprSingle 
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$6,NULL,$3));
		}
	  |	_DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration PositionalVar _IN_ ExprSingle 
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$6,$4,$3));
		}
	  |	_DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration PositionalVar FTScoreVar _IN_ ExprSingle 
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$7,$4,$3));
		}
	  ;

// [36]    PositionalVar    ::=    "at" "$" VarName 
PositionalVar:
      _AT_KEYWORD_ _DOLLAR_SIGN_ _VARIABLE_
		{ 
      REJECT_NOT_XQUERY(PositionalVar, @1);

			$$ = $3; 
		}
    ;

// [37]    FTScoreVar    ::=    "score" "$" VarName
FTScoreVar:
    _SCORE_ _DOLLAR_SIGN_ _VARIABLE_
    {
      REJECT_NOT_FULLTEXT(FTScoreVar, @1);

      $$ = $3;
    }
    ;

// [38]    LetClause    ::= (("let" "$" VarName TypeDeclaration? FTScoreVar?) |
//                               ("let" "score" "$" VarName)) ":=" ExprSingle
//                               ("," (("$" VarName TypeDeclaration? FTScoreVar?) | FTScoreVar) ":=" ExprSingle)*
LetClause:
		_LET_ LetBindingList
		{
      REJECT_NOT_XQUERY(LetClause, @1);

			$$ = $2;
		}
	  ;

LetBindingList:
      LetBindingList _COMMA_ LetBinding
		{
			$1->push_back($3);
			$$ = $1;
		}
    | LetBinding
		{
			$$ = new (MEMMGR) VectorOfVariableBinding(XQillaAllocator<XQVariableBinding*>(MEMMGR));
			$$->push_back($1);
		}
    ;

LetBinding:
        _DOLLAR_SIGN_ _VARIABLE_ _COLON_EQ_ ExprSingle
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::letBinding,$2,$4));
		}
        | _DOLLAR_SIGN_ _VARIABLE_ FTScoreVar _COLON_EQ_ ExprSingle
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::letBinding,$2,$5));
		}
	| _DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration _COLON_EQ_ ExprSingle
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::letBinding,$2,$5,NULL,$3));
		}
	| _DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration FTScoreVar _COLON_EQ_ ExprSingle
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::letBinding,$2,$6,NULL,$3));
		}
	| FTScoreVar _COLON_EQ_ ExprSingle
		{
		}
    ;

// [39]    WhereClause    ::=    "where" ExprSingle 
WhereClause:
      _WHERE_ ExprSingle
		{ 
      REJECT_NOT_XQUERY(WhereClause, @1);

			$$ = WRAP(@1, $2);
		}
    ;

// [40]    OrderByClause    ::=    (<"order" "by"> |  <"stable" "order" "by">) OrderSpecList 
OrderByClause:
	  _ORDER_ _BY_ OrderSpecList
		{
      REJECT_NOT_XQUERY(OrderByClause, @1);

			$$=new (MEMMGR) XQSort(XQSort::unstable,$3);
		}
	| _STABLE_ _ORDER_ _BY_ OrderSpecList
		{
      REJECT_NOT_XQUERY(OrderByClause, @1);

			$$=new (MEMMGR) XQSort(XQSort::stable,$4);
		}
	;

// [41]    OrderSpecList    ::=    OrderSpec ("," OrderSpec)* 
OrderSpecList:
	  OrderSpecList _COMMA_ OrderSpec
		{
			$1->push_back($3);
			$$ = $1;
		}
	| OrderSpec
		{
			$$ = new (MEMMGR) XQSort::VectorOfSortSpec(XQillaAllocator<XQSort::SortSpec*>(MEMMGR));
			$$->push_back($1);
		}
	;

// [42]    OrderSpec    ::=    ExprSingle OrderModifier 
// [43]    OrderModifier    ::=    ("ascending" |  "descending")? (<"empty" "greatest"> |  <"empty" "least">)? ("collation" URILiteral)? 
OrderSpec:
	  ExprSingle OrderDirection EmptyHandling
		{
			$$ = WRAP(@1, new (MEMMGR) XQSort::SortSpec($1,$2+$3,NULL));
		}
	| ExprSingle OrderDirection EmptyHandling _COLLATION_ URILiteral
		{
            try
            {
              LOCATION(@5, loc);
                CONTEXT->getCollation($5, &loc);
            }
            catch(ContextException&)
            {
			    yyerror(@5, "The specified collation does not exist [err:XQST0076]");
            }
			$$ = WRAP(@1, new (MEMMGR) XQSort::SortSpec($1,$2+$3,$5));
		}
	;

OrderDirection:
	  /* empty */
		{ $$ = XQSort::SortSpec::ascending; }
	|  _ASCENDING_
		{ $$ = XQSort::SortSpec::ascending; }
    | _DESCENDING_
		{ $$ = XQSort::SortSpec::descending; }
	;

EmptyHandling:
	  /* empty */
		{ 
			switch(CONTEXT->getDefaultFLWOROrderingMode())
			{
			case StaticContext::FLWOR_ORDER_EMPTY_LEAST:
				$$ = XQSort::SortSpec::empty_least; break;
			case StaticContext::FLWOR_ORDER_EMPTY_GREATEST:
				$$ = XQSort::SortSpec::empty_greatest; break;
			}
		}
    | _EMPTY_KEYWORD_ _GREATEST_
		{ $$ = XQSort::SortSpec::empty_greatest; }
    | _EMPTY_KEYWORD_ _LEAST_
		{ $$ = XQSort::SortSpec::empty_least; }
	;

// [44]    QuantifiedExpr    ::=    (<"some" "$"> |  <"every" "$">) VarName TypeDeclaration? "in" ExprSingle 
//										("," "$" VarName TypeDeclaration? "in" ExprSingle)* "satisfies" ExprSingle 
QuantifiedExpr:
	_SOME_ QuantifyBindingList _SATISFIES_ ExprSingle
	{
		$$ = new (MEMMGR) XQQuantified(XQQuantified::some, $2, WRAP(@3, $4), MEMMGR);
	}
	| _EVERY_ QuantifyBindingList _SATISFIES_ ExprSingle
	{
		$$ = new (MEMMGR) XQQuantified(XQQuantified::every, $2, WRAP(@3, $4), MEMMGR);
	}
	;

QuantifyBindingList:
		QuantifyBindingList _COMMA_ QuantifyBinding
		{
			$$->push_back($3);
		}
	  | QuantifyBinding
		{
			$$ = new (MEMMGR) VectorOfVariableBinding(XQillaAllocator<XQVariableBinding*>(MEMMGR));
			$$->push_back($1);
		}
	  ;

QuantifyBinding:
	    _DOLLAR_SIGN_ _VARIABLE_ _IN_ ExprSingle 
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$4));
		}
	  |	_DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration _IN_ ExprSingle 
		{
			$$ = WRAP(@1, new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$5,NULL,$3));
		}
	  ;

// [45]    TypeswitchExpr    ::=    <"typeswitch" "("> Expr ")" CaseClause+ "default" ("$" VarName)? "return" ExprSingle 
TypeswitchExpr:
		_TYPESWITCH_ _LPAR_ Expr _RPAR_ CaseClauseList _DEFAULT_ _DOLLAR_SIGN_ _VARIABLE_ _RETURN_ ExprSingle
		{
      REJECT_NOT_XQUERY(TypeswitchExpr, @1);

			XQTypeswitch::Clause* defClause=WRAP(@6, new (MEMMGR) XQTypeswitch::Clause(NULL,$10,MEMMGR->getPooledString($8)));
			$$ = new (MEMMGR) XQTypeswitch( WRAP(@1, $3), $5, defClause, MEMMGR);
		}
	  | _TYPESWITCH_ _LPAR_ Expr _RPAR_ CaseClauseList _DEFAULT_ _RETURN_ ExprSingle
		{
      REJECT_NOT_XQUERY(TypeswitchExpr, @1);

			XQTypeswitch::Clause* defClause=WRAP(@6, new (MEMMGR) XQTypeswitch::Clause(NULL,$8,NULL));
			$$ = new (MEMMGR) XQTypeswitch( WRAP(@1, $3), $5, defClause, MEMMGR);
		}
	  ;

CaseClauseList:
		CaseClauseList CaseClause
		{
			$1->push_back($2);
			$$=$1;
		}
	  | CaseClause
		{
			$$=new (MEMMGR) XQTypeswitch::VectorOfClause(XQillaAllocator<XQTypeswitch::Clause*>(MEMMGR));
			$$->push_back($1);
		}
	  ;

// [46]    CaseClause    ::=    "case" ("$" VarName "as")? SequenceType "return" ExprSingle
CaseClause:
	  _CASE_ SequenceType _RETURN_ ExprSingle
		{ 
			$$ = WRAP(@1, new (MEMMGR) XQTypeswitch::Clause($2, $4, NULL));
		}
	|  _CASE_ _DOLLAR_SIGN_ _VARIABLE_ _AS_ SequenceType _RETURN_ ExprSingle
		{ 
			$$ = WRAP(@1, new (MEMMGR) XQTypeswitch::Clause($5, $7, MEMMGR->getPooledString($3)));
		}
	;

// [47]    IfExpr    ::=    <"if" "("> Expr ")" "then" ExprSingle "else" ExprSingle 
IfExpr:
	  _IF_ _LPAR_ Expr _RPAR_ _THEN_ ExprSingle _ELSE_ ExprSingle
		{ 
	        $$ = WRAP(@1, new (MEMMGR) XQIf($3, $6, $8, MEMMGR));
		}
	;

// [48]    OrExpr    ::=    AndExpr ( "or"  AndExpr )* 
OrExpr:
		OrExpr _OR_ AndExpr
		{
			ASTNode* prevExpr=$1;
			if((unsigned int)prevExpr->getType()==ASTNode::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()==ASTNode::OPERATOR && 
			   XPath2Utils::equals(((XQOperator*)prevExpr)->getOperatorName(),Or::name))
			{
				((Or*)prevExpr)->addArgument($3);
				$$ = prevExpr;
			}
			else
				$$ = WRAP(@1, new (MEMMGR) Or(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
	  | AndExpr
	  ;

// [49]    AndExpr    ::=    ComparisonExpr ( "and" ComparisonExpr )* 
AndExpr:
		AndExpr _AND_ ComparisonExpr
		{
			ASTNode* prevExpr=$1;
			if((unsigned int)prevExpr->getType()==ASTNode::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()==ASTNode::OPERATOR && 
			   XPath2Utils::equals(((XQOperator*)prevExpr)->getOperatorName(),And::name))
			{
				((And*)prevExpr)->addArgument($3);
				$$ = prevExpr;
			}
			else
				$$ = WRAP(@1, new (MEMMGR) And(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
	  | ComparisonExpr
      ;

// [50]    ComparisonExpr    ::=    FTContainsExpr ( (ValueComp 
//									|  GeneralComp 
//									|  NodeComp)  FTContainsExpr )? 
// [63]    GeneralComp    ::=    "=" |  "!=" |  "<" |  "<=" |  ">" |  ">=" 
// [64]    ValueComp    ::=    "eq" |  "ne" |  "lt" |  "le" |  "gt" |  "ge" 
// [65]    NodeComp    ::=    "is" |  "<<" |  ">>" 
ComparisonExpr:
	  FTContainsExpr _EQUALS_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::EQUAL,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _NOT_EQUALS_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::NOT_EQUAL,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _LT_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::LESS_THAN,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _LT_EQUALS_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::LESS_THAN_EQUAL,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _GT_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::GREATER_THAN,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _GT_EQUALS_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::GREATER_THAN_EQUAL,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _FORTRAN_EQ_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) Equals(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _FORTRAN_NE_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) NotEquals(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _FORTRAN_LT_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) LessThan(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _FORTRAN_LE_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) LessThanEqual(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _FORTRAN_GT_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GreaterThan(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _FORTRAN_GE_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GreaterThanEqual(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| FTContainsExpr _IS_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) NodeComparison(packageArgs($1, $3, MEMMGR), MEMMGR));
		}
	| FTContainsExpr _LT_LT_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) OrderComparison(packageArgs($1, $3, MEMMGR), true, MEMMGR));
		}
	| FTContainsExpr _GT_GT_ FTContainsExpr
		{
			$$ = WRAP(@2, new (MEMMGR) OrderComparison(packageArgs($1, $3, MEMMGR), false,MEMMGR));
		}
	| FTContainsExpr
	;

// [51]    FTContainsExpr    ::=    RangeExpr ( "ftcontains" FTSelection FTIgnoreOption? )?
FTContainsExpr:
      RangeExpr _FTCONTAINS_ FTSelection
      {
        REJECT_NOT_FULLTEXT(FTContainsExpr, @1);

        $$ = WRAP(@2, new (MEMMGR) FTContains($1, $3, NULL, MEMMGR));
      }
      | RangeExpr _FTCONTAINS_ FTSelection FTIgnoreOption
      {
        REJECT_NOT_FULLTEXT(FTContainsExpr, @1);

        $$ = WRAP(@2, new (MEMMGR) FTContains($1, $3, $4, MEMMGR));
      }
      | RangeExpr
      ;

// [52]    RangeExpr    ::=    AdditiveExpr ( "to"  AdditiveExpr )?
RangeExpr:
      AdditiveExpr _TO_ AdditiveExpr
		{
			$$ = WRAP(@2, new (MEMMGR) Range(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
	| AdditiveExpr
    ;

// [53]    AdditiveExpr    ::=    MultiplicativeExpr ( ("+" |  "-")  MultiplicativeExpr )* 
AdditiveExpr:
      AdditiveExpr _PLUS_ MultiplicativeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) Plus(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
    | AdditiveExpr _MINUS_ MultiplicativeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) Minus(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
	| MultiplicativeExpr
    ;

// [54]    MultiplicativeExpr    ::=    UnionExpr ( ("*" |  "div" |  "idiv" |  "mod")  UnionExpr )* 
MultiplicativeExpr:
      MultiplicativeExpr _MULTIPLY_ UnionExpr 
		{
			$$ = WRAP(@2, new (MEMMGR) Multiply(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
    | MultiplicativeExpr _DIV_ UnionExpr 
		{
			$$ = WRAP(@2, new (MEMMGR) Divide(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
    | MultiplicativeExpr _INTEGER_DIV_ UnionExpr 
		{
			$$ = WRAP(@2, new (MEMMGR) IntegerDivide(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
    | MultiplicativeExpr _MOD_ UnionExpr 
		{
			$$ = WRAP(@2, new (MEMMGR) Mod(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
	| UnionExpr 
    ;

// [55]    UnionExpr    ::=    IntersectExceptExpr ( ("union" |  "|")  IntersectExceptExpr )* 
UnionExpr:
      UnionExpr _VERTICAL_BAR_ IntersectExceptExpr
		{
			$$ = WRAP(@2, new (MEMMGR) Union(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
    | UnionExpr _UNION_ IntersectExceptExpr
		{
			$$ = WRAP(@2, new (MEMMGR) Union(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
	| IntersectExceptExpr
    ;

// [56]    IntersectExceptExpr    ::=    InstanceofExpr ( ("intersect" |  "except")  InstanceofExpr )* 
IntersectExceptExpr:
      IntersectExceptExpr _INTERSECT_ InstanceofExpr
		{
			$$ = WRAP(@2, new (MEMMGR) Intersect(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
    | IntersectExceptExpr _EXCEPT_ InstanceofExpr
		{
			$$ = WRAP(@2, new (MEMMGR) Except(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
	| InstanceofExpr
    ;

// [57]    InstanceofExpr    ::=    TreatExpr ( <"instance" "of"> SequenceType )? 
InstanceofExpr:
	TreatExpr _INSTANCE_ _OF_ SequenceType
	{
		$$ = WRAP(@2, new (MEMMGR) XQInstanceOf($1,$4,MEMMGR));
	}
	| TreatExpr
	;

// [58]    TreatExpr    ::=    CastableExpr ( <"treat" "as"> SequenceType )? 
TreatExpr:
	CastableExpr _TREAT_ _AS_ SequenceType
	{
        XQTreatAs* treatAs = new (MEMMGR) XQTreatAs($1,$4,MEMMGR);
        treatAs->setIsTreatAsStatement(true);
		$$ = WRAP(@2, treatAs);
	}
	| CastableExpr
	;

// [59]    CastableExpr    ::=    CastExpr ( <"castable" "as"> SingleType )? 
CastableExpr:
	CastExpr _CASTABLE_ _AS_ SingleType
	{
		$$ = WRAP(@2, new (MEMMGR) XQCastableAs($1,$4,MEMMGR));
	}
	| CastExpr
	;

// [60]    CastExpr    ::=    UnaryExpr (<"cast" "as"> SingleType)?
CastExpr:
	UnaryExpr _CAST_ _AS_ SingleType
	{
		$$ = WRAP(@1, new (MEMMGR) XQCastAs($1,$4,MEMMGR));
	}
	| UnaryExpr
	;

// [61]    UnaryExpr    ::=    ("-" |  "+")* ValueExpr 
UnaryExpr:
      _MINUS_ UnaryExpr
		{
			VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
			args.push_back($2);
			$$ = WRAP(@1, new (MEMMGR) UnaryMinus(/*positive*/false, args, MEMMGR));
		}
    | _PLUS_ UnaryExpr
		{
			VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
			args.push_back($2);
			$$ = WRAP(@1, new (MEMMGR) UnaryMinus(/*positive*/true, args, MEMMGR));
		}
	| ValueExpr 
    ;

// [62]    ValueExpr    ::=    ValidateExpr | PathExpr | ExtensionExpr
ValueExpr:
	  ValidateExpr
	| PathExpr
	| ExtensionExpr
	;

// [66]    ValidateExpr    ::=    (<"validate" "{"> |  
//								   (<"validate" ValidationMode> "{")
//								  ) Expr "}" 
// [67]    ValidationMode    ::=    "lax" | "strict"
ValidateExpr:
	  _VALIDATE_ _LBRACE_ Expr _RBRACE_
		{
			$$ = WRAP(@1, new (MEMMGR) XQValidate($3,DocumentCache::VALIDATION_STRICT,MEMMGR));
		}
	| _VALIDATE_ _MODE_LAX_ _LBRACE_ Expr _RBRACE_
		{
			$$ = WRAP(@1, new (MEMMGR) XQValidate($4,DocumentCache::VALIDATION_LAX,MEMMGR));
		}
	| _VALIDATE_ _MODE_STRICT_ _LBRACE_ Expr _RBRACE_
		{
			$$ = WRAP(@1, new (MEMMGR) XQValidate($4,DocumentCache::VALIDATION_STRICT,MEMMGR));
		}
	;

// [68]   	ExtensionExpr	   ::=   	Pragma+ "{" Expr? "}"
ExtensionExpr:
	  PragmaList _LBRACE_ _RBRACE_
	{
    REJECT_NOT_XQUERY(ExtensionExpr, @1);

		// we don't support any pragma
		yyerror(@1, "This pragma is not recognized, and no alternative expression is specified [err:XQST0079]");
	}
	| PragmaList _LBRACE_ Expr _RBRACE_
	{
    REJECT_NOT_XQUERY(ExtensionExpr, @1);

		// we don't support any pragma
		$$ = $3;
	}
	;

PragmaList:
	  Pragma
	| PragmaList Pragma
	;

// [69]   	Pragma	   ::=   	"(#" S? QName PragmaContents "#)"
// [70]   	PragmaContents	   ::=   	(Char* - (Char* '#)' Char*))
Pragma:
	  _PRAGMA_OPEN_ _PRAGMA_NAME_ _PRAGMA_CONTENT_ _PRAGMA_CLOSE_
      {
        // validate the QName
		QualifiedName qName($2);
        const XMLCh* prefix=qName.getPrefix();
        if(prefix==NULL || *prefix==0)
			yyerror(@2, "The pragma name must have a prefix [err:XPST0081]");

		try
		{
			LOCATION(@2, loc);
			CONTEXT->getUriBoundToPrefix(prefix, &loc);
		}
		catch(NamespaceLookupException&)
		{
			yyerror(@2, "The pragma name is using an undefined namespace prefix [err:XPST0081]");
		}
      }
    |  _PRAGMA_OPEN_ _PRAGMA_NAME_ _PRAGMA_CLOSE_
      {
        // validate the QName
		QualifiedName qName($2);
        const XMLCh* prefix=qName.getPrefix();
        if(prefix==NULL || *prefix==0)
			yyerror(@2, "The pragma name must have a prefix [err:XPST0081]");

		try
		{
			LOCATION(@2, loc);
			CONTEXT->getUriBoundToPrefix(prefix, &loc);
		}
		catch(NamespaceLookupException&)
		{
			yyerror(@2, "The pragma name is using an undefined namespace prefix [err:XPST0081]");
		}
      }
	;

// [71]    PathExpr    ::=    ("/" RelativePathExpr?) |  ("//" RelativePathExpr) |  RelativePathExpr 
PathExpr:
	  _SLASH_
  		{
			XQNav *nav = new (MEMMGR) XQNav(MEMMGR);

      VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
      FunctionRoot *root = WRAP(@1, new (MEMMGR) FunctionRoot(args, MEMMGR));

      SequenceType *documentNode = WRAP(@1, new (MEMMGR)
        SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT)));

      XQTreatAs *treat = WRAP(@1, new (MEMMGR) XQTreatAs(root, documentNode, MEMMGR));
      nav->addStepFront(treat);

			$$ = nav;
		}
	| _SLASH_ RelativePathExpr
  		{
			XQNav* nav=getNavigation($2,MEMMGR);

      VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
      FunctionRoot *root = WRAP(@1, new (MEMMGR) FunctionRoot(args, MEMMGR));

      SequenceType *documentNode = WRAP(@1, new (MEMMGR)
        SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT)));

      XQTreatAs *treat = WRAP(@1, new (MEMMGR) XQTreatAs(root, documentNode, MEMMGR));
      nav->addStepFront(treat);

			$$ = nav;
		}
	| _SLASHSLASH_ RelativePathExpr
  		{
			XQNav *nav = getNavigation($2,MEMMGR);

      NodeTest *step = new (MEMMGR) NodeTest();
      step->setTypeWildcard();
      step->setNameWildcard();
      step->setNamespaceWildcard();
      nav->addStepFront(WRAP(@1, new (MEMMGR) XQStep(XQStep::DESCENDANT_OR_SELF, step, MEMMGR)));

      VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
      FunctionRoot *root = WRAP(@1, new (MEMMGR) FunctionRoot(args, MEMMGR));

      SequenceType *documentNode = WRAP(@1, new (MEMMGR)
        SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT)));

      XQTreatAs *treat = WRAP(@1, new (MEMMGR) XQTreatAs(root, documentNode, MEMMGR));
      nav->addStepFront(treat);

			$$ = nav;
		}
	| RelativePathExpr
	;

// [72]    RelativePathExpr    ::=    StepExpr (("/" |  "//") StepExpr)* 
RelativePathExpr:
	  RelativePathExpr _SLASH_ StepExpr
  		{
			XQNav *nav = getNavigation($1,MEMMGR);
			nav->addStep($3);
			$$ = nav;
		}
	| RelativePathExpr _SLASHSLASH_ StepExpr
  		{
			XQNav *nav = getNavigation($1,MEMMGR);

            NodeTest *step = new (MEMMGR) NodeTest();
            step->setTypeWildcard();
            step->setNameWildcard();
            step->setNamespaceWildcard();
            nav->addStep(WRAP(@2, new (MEMMGR) XQStep(XQStep::DESCENDANT_OR_SELF, step, MEMMGR)));
			nav->addStep($3);

			$$ = nav;
		}
	| StepExpr
	;

// [73]    StepExpr    ::=    AxisStep |  FilterExpr 
StepExpr:
	  AxisStep
	| FilterExpr
	;

// [74]    AxisStep    ::=    (ForwardStep |  ReverseStep) PredicateList 
AxisStep:
	  ForwardStep PredicateList
		{
      $$ = XQPredicate::addPredicates($1, $2);
		}
	| ReverseStep PredicateList
		{
      $$ = XQPredicate::addPredicates($1, $2);
      $$ = WRAP(@1, new (MEMMGR) XQDocumentOrder($$, MEMMGR));
		}
	;

// [75]    ForwardStep    ::=    (ForwardAxis NodeTest) |  AbbrevForwardStep 
ForwardStep:
	  ForwardAxis NodeTest
		{
      if(!$2->isNodeTypeSet()) {
        switch($1) {
        case XQStep::NAMESPACE: $2->setNodeType(Node::namespace_string); break;
        case XQStep::ATTRIBUTE: $2->setNodeType(Node::attribute_string); break;
        default: $2->setNodeType(Node::element_string); break;
        }
      }

			$$ = WRAP(@1, new (MEMMGR) XQStep($1,$2,MEMMGR));
		}
	| AbbrevForwardStep
	;

// [76]    ForwardAxis    ::=    <"child" "::">
//								|  <"descendant" "::">
//								|  <"attribute" "::">
//								|  <"self" "::">
//								|  <"descendant-or-self" "::"> 
//								|  <"following-sibling" "::">
//								|  <"following" "::">
ForwardAxis:
	  _AXIS_CHILD_ 
		{
      $$ = XQStep::CHILD;
		}
	| _AXIS_DESCENDANT_ 
		{
      $$ = XQStep::DESCENDANT;
		}
	| _AXIS_ATTRIBUTE_
		{
      $$ = XQStep::ATTRIBUTE;
		}
	| _AXIS_SELF_
		{
      $$ = XQStep::SELF;
		}
	| _AXIS_DESCENDANT_OR_SELF_ 
		{
      $$ = XQStep::DESCENDANT_OR_SELF;
		}
	| _AXIS_FOLLOWING_SIBLING_ 
		{
      $$ = XQStep::FOLLOWING_SIBLING;
		}
	| _AXIS_FOLLOWING_ 
		{
      $$ = XQStep::FOLLOWING;
		}
    | _AXIS_NAMESPACE_
        {
        REJECT_NOT_XPATH(AxisNamespace, @1);
      $$ = XQStep::NAMESPACE;
        }
	;

// [77]    AbbrevForwardStep    ::=    "@"? NodeTest
AbbrevForwardStep:
	_AT_ NodeTest
		{
      if(!$2->isNodeTypeSet()) {
        $2->setNodeType(Node::attribute_string);
      }

      $$ = WRAP(@1, new (MEMMGR) XQStep(XQStep::ATTRIBUTE, $2, MEMMGR));
		}
	| NodeTest
		{
      XQStep::Axis axis = XQStep::CHILD;
      SequenceType::ItemType *itemtype = $1->getItemType();
      if(itemtype != 0 &&
         itemtype->getItemTestType() == SequenceType::ItemType::TEST_ATTRIBUTE) {
        axis = XQStep::ATTRIBUTE;
      }
      else if(!$1->isNodeTypeSet()) {
        $1->setNodeType(Node::element_string);
      }

      $$ = WRAP(@1, new (MEMMGR) XQStep(axis, $1, MEMMGR));
		}
	;

// [78]    ReverseStep    ::=    (ReverseAxis NodeTest) |  AbbrevReverseStep 
ReverseStep:
	  ReverseAxis NodeTest
		{
      if(!$2->isNodeTypeSet()) {
        $2->setNodeType(Node::element_string);
      }

      $$ = WRAP(@1, new (MEMMGR) XQStep($1, $2, MEMMGR));
		}
	| AbbrevReverseStep 
	;

// [79]    ReverseAxis    ::=    <"parent" "::"> 
//								| <"ancestor" "::">
//								| <"preceding-sibling" "::">
//								| <"preceding" "::">
//								| <"ancestor-or-self" "::">
ReverseAxis:
	  _AXIS_PARENT_
		{
      $$ = XQStep::PARENT;
		}
	| _AXIS_ANCESTOR_
		{
      $$ = XQStep::ANCESTOR;
		}
	| _AXIS_PRECEDING_SIBLING_
		{
      $$ = XQStep::PRECEDING_SIBLING;
		}
	| _AXIS_PRECEDING_
		{
      $$ = XQStep::PRECEDING;
		}
	| _AXIS_ANCESTOR_OR_SELF_
		{
      $$ = XQStep::ANCESTOR_OR_SELF;
		}
	;

// [80]    AbbrevReverseStep    ::=    ".." 
AbbrevReverseStep:
	  _DOT_DOT_
		{
			NodeTest *step = new (MEMMGR) NodeTest();
			step->setNameWildcard();
			step->setNamespaceWildcard();
			step->setTypeWildcard();
			$$ = WRAP(@1, new (MEMMGR) XQStep(XQStep::PARENT, step, MEMMGR));
		}	
	;

// [81]    NodeTest    ::=    KindTest |  NameTest 
NodeTest:
	  KindTest 
	| NameTest
	;

// [82]    NameTest    ::=    QName |  Wildcard 
NameTest:
	  QName
		{
			NodeTest *step = new (MEMMGR) NodeTest();
      step->setNodePrefix($1->getPrefix());
			step->setNodeName($1->getName());
			$$ = step;
		}
	| Wildcard
	;

// [83]    Wildcard    ::=    "*" |  <NCName ":" "*"> |  <"*" ":" NCName> 
Wildcard:
      _STAR_
		{
			NodeTest *step = new (MEMMGR) NodeTest();
			step->setNameWildcard();
			step->setNamespaceWildcard();
			$$ = step;
		}
    | _NCNAME_COLON_STAR_
		{
			NodeTest *step = new (MEMMGR) NodeTest();
      step->setNodePrefix($1);
			step->setNameWildcard();
			$$ = step;
		}
	| _STAR_COLON_NCNAME_
		{
			NodeTest *step = new (MEMMGR) NodeTest();
			step->setNodeName($1);
			step->setNamespaceWildcard();
			$$ = step;
		}
	;

// [84]    FilterExpr    ::=    PrimaryExpr PredicateList 
FilterExpr:
	  PrimaryExpr PredicateList
		{
      $$ = XQPredicate::addPredicates($1, $2);
		}
	;

// [85]    PredicateList    ::=    Predicate* 
// [86]    Predicate    ::=    "[" Expr "]" 
PredicateList:
	  /* empty */
		{
	        $$ = new (MEMMGR) VectorOfPredicates(MEMMGR);
		}
	| PredicateList _LBRACK_ Expr _RBRACK_
		{
      XQPredicate *pred = WRAP(@2, new (MEMMGR) XQPredicate($3, MEMMGR));
			$1->push_back(pred);
			$$ = $1; 
		}
	;

// [87]    PrimaryExpr    ::=    Literal |  VarRef | ParenthesizedExpr | ContextItemExpr | FunctionCall | Constructor | OrderedExpr | UnorderedExpr
PrimaryExpr:
	   Literal 
	|  VarRef
	|  ParenthesizedExpr 
	|  ContextItemExpr
	|  FunctionCall 
	|  Constructor
	|  OrderedExpr 
	|  UnorderedExpr
	;

// [88]    Literal    ::=    NumericLiteral |  StringLiteral 
Literal:
	  NumericLiteral 
	| StringLiteral 
	;

// [89]    NumericLiteral    ::=    IntegerLiteral |  DecimalLiteral |  DoubleLiteral 
NumericLiteral:
	  IntegerLiteral 
	| DecimalLiteral 
	| DoubleLiteral 
	;

// [90]    VarRef    ::=    "$" VarName 
// [91]    VarName    ::=    QName
VarRef:
	_DOLLAR_SIGN_ _VARIABLE_
		{
		    int nColon=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf($2,':');
			if(nColon!=-1)
  				$$ = WRAP(@1, new (MEMMGR) XQVariable($2,MEMMGR));
			else
				$$ = WRAP(@1, new (MEMMGR) XQVariable(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, $2,MEMMGR));
		}
	;
		
// [92]    ParenthesizedExpr    ::=    "(" Expr? ")" 
ParenthesizedExpr:
      _LPAR_ Expr _RPAR_
		{ 
			ASTNode* prevExpr=$2;
			if((unsigned int)prevExpr->getType()==ASTNode::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()!=ASTNode::PARENTHESIZED)
			{
				XQParenthesizedExpr *dis = new (MEMMGR) XQParenthesizedExpr(MEMMGR);
				dis->addItem($2);
				$$ = WRAP(@2, dis);
			}
			else
				$$ = WRAP(@2, $2);
		}
    | _LPAR_ _RPAR_
		{ 
			$$ = WRAP(@1, new (MEMMGR) XQSequence(MEMMGR));
		}
    ;

// [93]    ContextItemExpr    ::=    "." 
ContextItemExpr:
	  _DOT_
		{
			$$ = WRAP(@1, new (MEMMGR) XQContextItem(MEMMGR));
		}
	;

// [94]    OrderedExpr    ::=    <"ordered" "{"> Expr "}" 
OrderedExpr:
	_ORDERED_ _LBRACE_ Expr _RBRACE_
		{
      REJECT_NOT_XQUERY(OrderedExpr, @1);

			$$ = WRAP(@1, new (MEMMGR) XQOrderingChange(StaticContext::ORDERING_ORDERED, $3, MEMMGR));
		}
	;

// [95]    UnorderedExpr    ::=    <"unordered" "{"> Expr "}" 
UnorderedExpr:
	_UNORDERED_ _LBRACE_ Expr _RBRACE_
		{
      REJECT_NOT_XQUERY(UnorderedExpr, @1);

			$$ = WRAP(@1, new (MEMMGR) XQOrderingChange(StaticContext::ORDERING_UNORDERED, $3, MEMMGR));
		}
	;

// [96]    FunctionCall    ::=    <QName "("> (ExprSingle ("," ExprSingle)*)? ")" 
FunctionCall:
	  _FUNCTION_CALL_ _RPAR_
		{
			VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
			$$ = FNWRAP(@1, $1, new (MEMMGR) XQFunctionCall(new (MEMMGR) QualifiedName($1, MEMMGR), args, MEMMGR));
		}
	| _FUNCTION_CALL_ FunctionCallArgumentList _RPAR_
		{
			$$ = FNWRAP(@1, $1, new (MEMMGR) XQFunctionCall(new (MEMMGR) QualifiedName($1, MEMMGR), *$2,MEMMGR));
		}
	;

FunctionCallArgumentList:
	  FunctionCallArgumentList _COMMA_ ExprSingle
		{
			$1->push_back($3);
			$$ = $1;
		}
	| ExprSingle
		{
			$$ = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			$$->push_back($1);
		}	
	;

// [97]    Constructor    ::=    DirectConstructor | ComputedConstructor 
Constructor:
	  DirectConstructor
    {
      REJECT_NOT_XQUERY(Constructor, @1);
    }
	| ComputedConstructor
    {
      REJECT_NOT_XQUERY(Constructor, @1);
    }
	;

// [98]    DirectConstructor    ::=    DirElemConstructor
//									 | DirCommentConstructor
//									 | DirPIConstructor 
DirectConstructor:
	  DirElemConstructor
	| DirCommentConstructor
	| DirPIConstructor
	;

// [99]    DirElemConstructor    ::=    "<" QName DirAttributeList ("/>" |  (">" DirElementContent* "</" QName S? ">")) 
DirElemConstructor:
      _START_TAG_OPEN_ _TAG_NAME_ DirAttributeList _EMPTY_TAG_CLOSE_
		{ 
			VectorOfASTNodes* content=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQElementConstructor(
								new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										$2, AnyAtomicType::QNAME),
										MEMMGR), 
								$3, content, MEMMGR));
		}
	| _START_TAG_OPEN_ _TAG_NAME_ DirAttributeList _TAG_CLOSE_ DirElementContent _END_TAG_OPEN_ _TAG_NAME_ _TAG_CLOSE_
		{ 
			if(!XPath2Utils::equals($2,$7))
				yyerror(@7, "Close tag does not match open tag");
			// if we are requested to strip whitespace-only nodes, check if the last element content should be removed
			VectorOfASTNodes* elemContent=$5;
			if(elemContent->size()>0)
			{
				if(elemContent->back()==0)
					elemContent->pop_back();
				else if(!CONTEXT->getPreserveBoundarySpace() &&
					    elemContent->back()->getType()==ASTNode::LITERAL)
				{
                    ASTNode* last=elemContent->back();
				    Item::Ptr litVal = ((XQLiteral*)last)->getItemConstructor()->createItem(CONTEXT);
					if(((AnyAtomicType*)(const Item*)litVal)->getPrimitiveTypeIndex()==AnyAtomicType::STRING &&
					   isAllSpaces(litVal->asString(CONTEXT)))
                    {
						elemContent->pop_back();
                        // special case: if the previous node was a CDATA, put it back
                        if(elemContent->size()>0 && elemContent->back()->getType()==ASTNode::DOM_CONSTRUCTOR &&
                           ((XQDOMConstructor*)elemContent->back())->getNodeType()==Node::cdata_string)
                        {
                            elemContent->push_back(last);
                        }
                    }
				}
			}
			$$ = WRAP(@1, new (MEMMGR) XQElementConstructor(
							  new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										$2, AnyAtomicType::QNAME),
										MEMMGR), 
							  $3, elemContent,MEMMGR));
		}
	;

// [100]    DirAttributeList    ::=    (S (QName S? "=" S? DirAttributeValue)?)* 
DirAttributeList: 
		/* empty */
		{
			$$ = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
		}
      | DirAttributeList _ATTRIBUTE_NAME_ _VALUE_INDICATOR_ DirAttributeValue
        {
            $$ = $1;
            bool bInsertAtFront=false;
            if(XPath2Utils::equals($2, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLNSString) ||
               XERCES_CPP_NAMESPACE_QUALIFIER XMLString::startsWith($2, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLNSColonString))
            {
                // check that the value of an xmlns attribute is a stirng literal
                if($4->size()>1 || ($4->size()==1 && $4->front()->getType()!=ASTNode::LITERAL))
                    yyerror(@4, "Namespace URI of a namespace declaration must be a literal [err:XQST0022]");
                bInsertAtFront=true;
            }
            if($4->size()>0 && $4->back()==0)
                $4->pop_back();
            ASTNode* attrItem=WRAP(@2, new (MEMMGR) XQAttributeConstructor(
                                            new (MEMMGR) XQLiteral(
                                                new (MEMMGR) AnyAtomicTypeConstructor(
                                                    XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										            $2, AnyAtomicType::QNAME),
                                                MEMMGR), 
                                            $4,MEMMGR));
            if(bInsertAtFront)
                $$->insert($$->begin(), attrItem);
            else
                $$->push_back(attrItem);
        }
	  ;

// [101]    DirAttributeValue    ::=    ('"' (EscapeQuot |  QuotAttrValueContent)* '"')
//								 |  ("'" (EscapeApos |  AposAttrValueContent)* "'") 
DirAttributeValue:
		_OPEN_QUOT_ QuotAttrValueContent _CLOSE_QUOT_
		{ 
			$$ = $2;
		}
	  | _OPEN_APOS_ AposAttrValueContent _CLOSE_APOS_
		{ 
			$$ = $2;
		}
	;

// [102]    QuotAttrValueContent    ::=    QuotAttContentChar
//										|  CommonContent
QuotAttrValueContent:
		/* empty */
		{ 
			$$ = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
		}
      | QuotAttrValueContent EnclosedExpr
		{
			$$ = $1;
            if($$->size()>0 && $$->back()==0)
			    $$->pop_back();
			$$->push_back($2);
			$$->push_back(0);
		}
      | QuotAttrValueContent _ESCAPE_QUOT_
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,szQuote);
		}
	  | QuotAttrValueContent _CHAR_
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,$2);
		}
	  | QuotAttrValueContent CommonContent
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,$2);
		}
	;

// [103]    AposAttrValueContent    ::=    AposAttContentChar
//										|  CommonContent
AposAttrValueContent:
		/* empty */
		{ 
			$$ = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
		}
      | AposAttrValueContent EnclosedExpr
		{
			$$ = $1;
            if($$->size()>0 && $$->back()==0)
			    $$->pop_back();
			$$->push_back($2);
			$$->push_back(0);
		}
      | AposAttrValueContent _ESCAPE_APOS_
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,szApos);
		}
	  | AposAttrValueContent _CHAR_
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,$2);
		}
      | AposAttrValueContent CommonContent
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,$2);
		}
	  ;

// [104]    DirElementContent    ::=    DirectConstructor 
//									|  ElementContentChar
//									|  CdataSection 
//									|  CommonContent

// As literal strings generated by entity references or enclosed expressions should not be merged with real literal strings,
// I add a NULL pointer after them that has to be removed by the next fragment or by the parent rule
DirElementContent:
	  /* empty */
		{
			$$ = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR)); 
		}
	  |	DirElementContent DirectConstructor
		{
			$$ = $1;
			// if the last token was a string literal made of whitespace and
			// we are adding a node constructor, and the context tells us to strip whitespace, remove it
			if($$->size()>0)
			{
				if($$->back()==0)
					$$->pop_back();
				else if(!CONTEXT->getPreserveBoundarySpace() && $$->back()->getType()==ASTNode::LITERAL)
				{
					const XMLCh* lastString=NULL;
					Item::Ptr litVal = ((XQLiteral*)$$->back())->getItemConstructor()->createItem(CONTEXT);
					if(((AnyAtomicType*)(const Item*)litVal)->getPrimitiveTypeIndex()==AnyAtomicType::STRING)
						lastString=litVal->asString(CONTEXT);
					if(lastString!=NULL && *lastString!=0 && isAllSpaces(lastString))
						$$->pop_back();
				}
			}
			$$->push_back($2);
		}
	  | DirElementContent _CHAR_
		{
			$$ = $1;
			if($$->size()>0 && $$->back() && $$->back()->getType()==ASTNode::LITERAL)
			{
				XQLiteral *lit = (XQLiteral*)$$->back();
				const XMLCh* string=lit->getItemConstructor()->createItem(CONTEXT)->asString(CONTEXT);
				string=XPath2Utils::concatStrings(string,$2,MEMMGR);

				AnyAtomicTypeConstructor *ic = new (MEMMGR)
    				AnyAtomicTypeConstructor(
								XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
								XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
					string, AnyAtomicType::STRING);
    			lit->setItemConstructor(ic);
			}
			else
			{
				if($$->size()>0 && $$->back()==0)
					$$->pop_back();
    			AnyAtomicTypeConstructor *ic = new (MEMMGR)
      				AnyAtomicTypeConstructor(
								XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
								XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
						$2, AnyAtomicType::STRING);

				$$->push_back(new (MEMMGR) XQLiteral(ic, MEMMGR));
			}
		}
      | DirElementContent CdataSection
		{
			$$ = $1;
			if($$->size()>0 && $$->back()==0)
				$$->pop_back();

			// if the last token was a string literal made of whitespace and
			// we are adding a node constructor, and the context tells us to strip whitespace, remove it
			if(!CONTEXT->getPreserveBoundarySpace() && $$->size()>0 && $$->back()->getType()==ASTNode::LITERAL)
			{
				const XMLCh* lastString=NULL;
				Item::Ptr litVal = ((XQLiteral*)$$->back())->getItemConstructor()->createItem(CONTEXT);
				if(((AnyAtomicType*)(const Item*)litVal)->getPrimitiveTypeIndex()==AnyAtomicType::STRING)
					lastString=litVal->asString(CONTEXT);
				if(lastString!=NULL && *lastString!=0 && isAllSpaces(lastString))
					$$->pop_back();
			}
			$$->push_back($2);
		}
	  | DirElementContent EnclosedExpr
		{
			$$ = $1;
			// if the last token was a string literal made of whitespace and
			// we are adding an enclosed expression, and the context tells us to strip whitespace, remove it
			if($$->size()>0)
			{
				if($$->back()==0)
					$$->pop_back();
				else if(!CONTEXT->getPreserveBoundarySpace() && $$->back()->getType()==ASTNode::LITERAL)
				{
					const XMLCh* lastString=NULL;
					Item::Ptr litVal = ((XQLiteral*)$$->back())->getItemConstructor()->createItem(CONTEXT);
					if(((AnyAtomicType*)(const Item*)litVal)->getPrimitiveTypeIndex()==AnyAtomicType::STRING)
						lastString=litVal->asString(CONTEXT);
					if(lastString!=NULL && *lastString!=0 && isAllSpaces(lastString))
						$$->pop_back();
				}
			}
			$$->push_back($2);
			$$->push_back(0);
		}
	  | DirElementContent CommonContent
		{
			$$ = $1;
			if($$->size()>0 && $$->back()==0)
				$$->pop_back();

    		AnyAtomicTypeConstructor *ic = new (MEMMGR)
      			AnyAtomicTypeConstructor(
							XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
							XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
					$2, AnyAtomicType::STRING);

			$$->push_back(new (MEMMGR) XQLiteral(ic, MEMMGR));
		}
	;

// [105]    CommonContent    ::=    PredefinedEntityRef | CharRef | "{{" | "}}" | EnclosedExpr 
// we choose to remove the EnclosedExpr, so that the type can be a string
CommonContent:
	_PREDEFINED_ENTITY_REF_
	| _CHAR_REF_
	| _LCURLY_BRACE_ESCAPE_
	{
		$$ = szCurlyOpen;
	} 
	| _RCURLY_BRACE_ESCAPE_ 
	{
		$$ = szCurlyClose;
	} 
	;

// [106]    DirCommentConstructor    ::=    "<!--" DirCommentContents "-->"
// [107]    DirCommentContents    ::=    ((Char - '-') | <'-' (Char - '-')>)* 
DirCommentConstructor:
	_XML_COMMENT_
		{
			ASTNode *value = WRAP(@1, new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, AnyAtomicType::STRING),
										MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQCommentConstructor(value, MEMMGR));
		}
	;

// [108]    DirPIConstructor    ::=    "<?" PITarget (S DirPIContents)? "?>"
// [109]    DirPIContents    ::=    (Char* - (Char* '?>' Char*)) 
DirPIConstructor:
	_PROCESSING_INSTRUCTION_START_ _PI_TARGET_ _PROCESSING_INSTRUCTION_CONTENT_
		{
			if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareIString($2, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLString)==0)
			  yyerror(@2, "The target for the processing instruction must not be 'XML'");
			ASTNode *value = WRAP(@3, new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$3, AnyAtomicType::STRING),
										MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQPIConstructor(
								      WRAP(@2, new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$2, AnyAtomicType::STRING),
										MEMMGR)), 
									  value, MEMMGR));
		}
	;

// [110]    CDataSection    ::=    "<![CDATA[" CDataSectionContents "]]>"
// [111]    CDataSectionContents    ::=    (Char* - (Char* ']]>' Char*)) 
CdataSection:
	  _CDATA_SECTION_
		{
			ASTNode *value = new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, AnyAtomicType::STRING),
										MEMMGR);
			$$ = WRAP(@1, new (MEMMGR) XQTextConstructor(/*isCDATA*/true, value, MEMMGR));
		}
	;

// [112]    ComputedConstructor    ::=   CompDocConstructor
//									   | CompElemConstructor
//									   | CompAttrConstructor
//									   | CompTextConstructor
//									   | CompCommentConstructor
//									   | CompPIConstructor 
ComputedConstructor:
	CompDocConstructor
	| CompElemConstructor
	| CompAttrConstructor
	| CompTextConstructor
	| CompCommentConstructor
	| CompPIConstructor 
	;

// [113]    CompDocConstructor    ::=    <"document" "{"> Expr "}" 
CompDocConstructor:
	  _DOCUMENT_CONSTR_ _LBRACE_ Expr _RBRACE_
		{
			$$ = WRAP(@1, new (MEMMGR) XQDocumentConstructor(WRAP(@3, $3),MEMMGR));
		}
	;

// [114]    CompElemConstructor    ::=    (<"element" QName "{"> |  (<"element" "{"> Expr "}" "{")) ContentExpr? "}" 
CompElemConstructor:
	  _NAMED_ELEMENT_CONSTR_ _LBRACE_ ContentExpr _RBRACE_ 
		{
			VectorOfASTNodes* content=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			content->push_back(WRAP(@3, $3));
			$$ = WRAP(@1, new (MEMMGR) XQElementConstructor(
								  new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										$1, AnyAtomicType::QNAME),
										MEMMGR), 
								  empty, content,MEMMGR));
		}
	| _NAMED_ELEMENT_CONSTR_ _LBRACE_ _RBRACE_ 
		{
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQElementConstructor(
								  new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										$1, AnyAtomicType::QNAME),
										MEMMGR), 
								  empty, empty,MEMMGR));
		}
	| _ELEMENT_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ ContentExpr _RBRACE_ 
		{
			VectorOfASTNodes* content=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			content->push_back(WRAP(@6, $6));
			$$ = WRAP(@1, new (MEMMGR) XQElementConstructor(
								  WRAP(@3, $3), 
								  empty, content, MEMMGR));
		}
	| _ELEMENT_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ _RBRACE_ 
		{
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQElementConstructor(
								  WRAP(@3, $3), 
								  empty, empty, MEMMGR));
		}
	;

// [115]    ContentExpr    ::=    Expr
ContentExpr:
	Expr
	;

// [116]    CompAttrConstructor    ::=    (<"attribute" QName "{"> |  (<"attribute" "{"> Expr "}" "{")) Expr? "}" 
CompAttrConstructor:
	  _NAMED_ATTRIBUTE_CONSTR_ _LBRACE_ Expr _RBRACE_ 
		{
			VectorOfASTNodes* content=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			content->push_back(WRAP(@3, $3));
			$$ = WRAP(@1, new (MEMMGR) XQAttributeConstructor(
								      new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										$1, AnyAtomicType::QNAME),
										MEMMGR), 
									  content,MEMMGR));
		}
	| _NAMED_ATTRIBUTE_CONSTR_ _LBRACE_ _RBRACE_ 
		{
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQAttributeConstructor(
								      new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										$1, AnyAtomicType::QNAME),
										MEMMGR), 
									  empty,MEMMGR));
		}
	| _ATTRIBUTE_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ Expr _RBRACE_ 
		{
			VectorOfASTNodes* content=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			content->push_back(WRAP(@6, $6));
			$$ = WRAP(@1, new (MEMMGR) XQAttributeConstructor(
									  WRAP(@3, $3), 
									  content, MEMMGR));
		}
	| _ATTRIBUTE_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ _RBRACE_ 
		{
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQAttributeConstructor(
									  WRAP(@3, $3), 
									  empty, MEMMGR));
		}
	;

// [117]    CompTextConstructor    ::=    <"text" "{"> Expr "}" 
CompTextConstructor:
	  _TEXT_CONSTR_ _LBRACE_ Expr _RBRACE_
		{
			$$ = WRAP(@1, new (MEMMGR) XQTextConstructor(/*isCDATA*/false, WRAP(@3, $3), MEMMGR));
		}
	;

// [118]    CompCommentConstructor    ::=    <"comment" "{"> Expr "}" 
CompCommentConstructor:
	  _COMMENT_CONSTR_ _LBRACE_ Expr _RBRACE_
		{
			$$ = WRAP(@1, new (MEMMGR) XQCommentConstructor(WRAP(@3, $3), MEMMGR));
		}
	;

// [119]    CompPIConstructor    ::=    (<"processing-instruction" NCName "{"> | (<"processing-instruction" "{"> Expr "}" "{")) Expr? "}" 
CompPIConstructor:
	  _NAMED_PROCESSING_INSTRUCTION_CONSTR_ _LBRACE_ Expr _RBRACE_
	  {
			$$ = WRAP(@1, new (MEMMGR) XQPIConstructor(
								      new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, AnyAtomicType::STRING),
										MEMMGR), 
									  WRAP(@3, $3), MEMMGR));
	  }
	| _NAMED_PROCESSING_INSTRUCTION_CONSTR_ _LBRACE_ _RBRACE_
	  {
			$$ = WRAP(@1, new (MEMMGR) XQPIConstructor(
								      new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, AnyAtomicType::STRING),
										MEMMGR), 
									  new (MEMMGR) XQSequence(MEMMGR), MEMMGR));
	  }
	| _PROCESSING_INSTRUCTION_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ Expr _RBRACE_
	  {
			$$ = WRAP(@1, new (MEMMGR) XQPIConstructor(
									  WRAP(@3, $3), 
									  WRAP(@6, $6), MEMMGR));
	  }
	| _PROCESSING_INSTRUCTION_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ _RBRACE_
	  {
			$$ = WRAP(@1, new (MEMMGR) XQPIConstructor(
									  WRAP(@3, $3), 
									  new (MEMMGR) XQSequence(MEMMGR), MEMMGR));
	  }
	;

// [120]    SingleType    ::=    AtomicType "?"? 
SingleType:
		AtomicType _ZERO_OR_ONE_
	    {
			SequenceType* seq=WRAP(@1, new (MEMMGR) SequenceType());
			seq->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE,NULL,$1));
			seq->setOccurrence(SequenceType::QUESTION_MARK);
			$$ = seq;
		}
	  | AtomicType
	    {
			SequenceType* seq=WRAP(@1, new (MEMMGR) SequenceType());
			seq->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE,NULL,$1));
			seq->setOccurrence(SequenceType::EXACTLY_ONE);
			$$ = seq;
		}
	;

// [121]    TypeDeclaration    ::=    "as" SequenceType 
TypeDeclaration:
	  _AS_ SequenceType
		{
      REJECT_NOT_XQUERY(TypeDeclaration, @1);

			$$ = $2;
		}
	;

// [122]    SequenceType    ::=    (ItemType OccurrenceIndicator?) |  <" empty-sequence" "(" ")">
SequenceType:
	    ItemType OccurrenceIndicator
		{
			SequenceType* seq=WRAP(@1, new (MEMMGR) SequenceType());
			seq->setItemType($1);
			seq->setOccurrence($2);
			$$ = seq;
		}
	  | ItemType 
		{
			SequenceType* seq=WRAP(@1, new (MEMMGR) SequenceType());
			seq->setItemType($1);
			seq->setOccurrence(SequenceType::EXACTLY_ONE);
			$$ = seq;
		}
	  | _EMPTY_ _LPAR_ _RPAR_
		{ 
			$$ = WRAP(@1, new (MEMMGR) SequenceType()); 
		}
	  ;


// [123]    OccurrenceIndicator    ::=    "*" |  "+" |  "?"
OccurrenceIndicator:
	_ZERO_OR_MORE_
	{ $$ = SequenceType::STAR; }
	| _ONE_OR_MORE_ 
	{ $$ = SequenceType::PLUS; }
	| _ZERO_OR_ONE_ 
	{ $$ = SequenceType::QUESTION_MARK; }
	;

// [124]    ItemType    ::=    AtomicType | KindTest | <"item" "(" ")"> 
ItemType:
	AtomicType 
	{
        $$ = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE, NULL, $1);
	}
	| KindTest 
	{
		$$ = $1->getItemType();
		$1->setItemType(NULL);
	}
	| _ITEM_ _LPAR_ _RPAR_
	{
		$$ = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING);
	}
	;

// [125]    AtomicType    ::=    QName 
AtomicType:
	QName
	{
		$$ = $1;
	}
	;

// [126]    KindTest    ::=    DocumentTest
//							|  ElementTest
//							|  AttributeTest
//							|  SchemaElementTest
//							|  SchemaAttributeTest
//							|  PITest 
//							|  CommentTest 
//							|  TextTest 
//							|  AnyKindTest 
KindTest:
	   DocumentTest
	|  ElementTest
	|  AttributeTest
	|  SchemaElementTest
	|  SchemaAttributeTest
	|  PITest
	|  CommentTest
	|  TextTest
	|  AnyKindTest
	;

// [127]    AnyKindTest    ::=    <"node" "("> ")" 
AnyKindTest:
	_NODE_LPAR_ _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_NODE));
		$$ = step;
	}
	;

// [128]    DocumentTest    ::=    <"document-node" "("> (ElementTest | SchemaElementTest)? ")"
DocumentTest:
	_DOCUMENT_NODE_LPAR_ _RPAR_ 
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT));
		$$ = step;
	}
	| _DOCUMENT_NODE_LPAR_ ElementTest _RPAR_ 
	{
		NodeTest *step = new (MEMMGR) NodeTest();
		SequenceType::ItemType* elemTest=$2->getItemType();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT,elemTest->getName(),elemTest->getType()));
		elemTest->setName(NULL);
		elemTest->setType(NULL);
		$$ = step;
	}
	| _DOCUMENT_NODE_LPAR_ SchemaElementTest _RPAR_ 
	{
		NodeTest *step = new (MEMMGR) NodeTest();
		SequenceType::ItemType* elemTest=$2->getItemType();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT,elemTest->getName(),elemTest->getType()));
		elemTest->setName(NULL);
		elemTest->setType(NULL);
		$$ = step;
	}
	;
	
// [129]    TextTest    ::=    <"text" "("> ")" 
TextTest:
	_TEXT_LPAR_ _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_TEXT));
		$$ = step;
	}
	;

// [130]    CommentTest    ::=    <"comment" "("> ")" 
CommentTest: 
	_COMMENT_LPAR_ _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
		step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_COMMENT));
		$$ = step;
	}
	;

// [131]    PITest    ::=    <"processing-instruction" "("> (NCName | StringLiteral)? ")" 
PITest:
	_PROCESSING_INSTRUCTION_LPAR_ _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_PI));
		$$ = step;
	}
	| _PROCESSING_INSTRUCTION_LPAR_ _NCNAME_ _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_PI, new (MEMMGR) QualifiedName($2)));
		$$ = step;
	}
	| _PROCESSING_INSTRUCTION_LPAR_ _STRING_LITERAL_ _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_PI, new (MEMMGR) QualifiedName($2)));
		$$ = step;
	}
	;

// [132]    AttributeTest    ::=    <"attribute" "("> (AttribNameOrWildcard ("," TypeName)?)? ")" 
AttributeTest:
	_ATTRIBUTE_LPAR_ _RPAR_ 
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATTRIBUTE));
		$$ = step;
	}
	| _ATTRIBUTE_LPAR_ AttribNameOrWildcard _RPAR_ 
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATTRIBUTE,$2));
		$$ = step;
	}
	| _ATTRIBUTE_LPAR_ AttribNameOrWildcard _COMMA_ TypeName _RPAR_ 
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATTRIBUTE,$2,$4));
		$$ = step;
	}
	;

// [133]    AttribNameOrWildcard    ::=    AttributeName | "*" 
AttribNameOrWildcard:
	AttributeName
	| _STAR_
	{
		$$ = NULL;
	}
	;

// [134]    SchemaAttributeTest    ::=    <"schema-attribute" "("> AttributeDeclaration ")" 
SchemaAttributeTest:
	_SCHEMA_ATTRIBUTE_LPAR_ AttributeDeclaration _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_SCHEMA_ATTRIBUTE,$2));
		$$ = step;
	}
	;

// [135]    AttributeDeclaration    ::=    AttributeName 
AttributeDeclaration:
	AttributeName 
	;

// [136]    ElementTest    ::=    <"element" "("> (ElementNameOrWildcard ("," TypeName "?"?)?)? ")" 
ElementTest:
	_ELEMENT_LPAR_ _RPAR_ 
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT));
		$$ = step;
	}
	| _ELEMENT_LPAR_ ElementNameOrWildcard _RPAR_ 
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT,$2));
		$$ = step;
	}
	| _ELEMENT_LPAR_ ElementNameOrWildcard _COMMA_ TypeName _RPAR_ 
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT,$2,$4));
		$$ = step;
	}
	| _ELEMENT_LPAR_ ElementNameOrWildcard _COMMA_ TypeName _NILLABLE_ _RPAR_ 
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        SequenceType::ItemType* pType=new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT,$2,$4);
		pType->setAllowNilled(true);
        step->setItemType(pType);
		$$ = step;
	}
	;

// [137]    ElementNameOrWildcard    ::=    ElementName | "*" 
ElementNameOrWildcard:
	ElementName
	| _STAR_
	{
		$$ = NULL;
	}
	;

// [138]    SchemaElementTest    ::=    <"schema-element" "("> ElementDeclaration ")" 
SchemaElementTest:
	_SCHEMA_ELEMENT_LPAR_ ElementDeclaration _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_SCHEMA_ELEMENT,$2));
		$$ = step;
	}
	;

// [139]    ElementDeclaration    ::=    ElementName 
ElementDeclaration:
	ElementName 
	;

// [140]    AttributeName    ::=    QName
AttributeName:
	QName
	;

// [141]    ElementName    ::=    QName
ElementName:
	QName
	;

// [142]    TypeName    ::=    QName
TypeName:
	QName
	;

// [143]   	URILiteral	   ::=   	StringLiteral
URILiteral:
	_STRING_LITERAL_
	{
		// the string must be whitespace-normalized
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::collapseWS($1, MEMMGR);
        if($1 && *$1 && !XPath2Utils::isValidURI($1, MEMMGR))
          yyerror(@1, "The URI literal is not valid [err:XQST0046]");
		$$ = $1;
	}
	;

// [144]    FTSelection    ::=    FTOr (FTMatchOption | FTProximity)* ("weight" DecimalLiteral)?
FTSelection:
	FTOr FTSelectionOptions _WEIGHT_ DecimalLiteral
	{
    // TBD FTSelectionOptions and weight
    $$ = $1;

    for(VectorOfFTOptions::iterator i = $2->begin();
        i != $2->end(); ++i) {
      (*i)->setArgument($$);
      $$ = *i;
    }
    delete $2;
	}
	| FTOr FTSelectionOptions
	{
    $$ = $1;

    for(VectorOfFTOptions::iterator i = $2->begin();
        i != $2->end(); ++i) {
      (*i)->setArgument($$);
      $$ = *i;
    }
    delete $2;
	}
	;

FTSelectionOptions:
	/* empty */
	{
    $$ = new (MEMMGR) VectorOfFTOptions(XQillaAllocator<FTOption*>(MEMMGR));
	}
	| FTSelectionOptions FTMatchOption
	{
    $$ = $1;
	}
	| FTSelectionOptions FTProximity
	{
    if($2 != NULL)
      $1->push_back($2);
    $$ = $1;
	}
	;

// [145]    FTOr    ::=    FTAnd ( "||" FTAnd )*
FTOr:
	FTOr _DOUBLE_VERTICAL_BAR_ FTAnd
	{
    if($1->getType() == FTSelection::OR) {
      FTOr *op = (FTOr*)$1;
      op->addArg($3);
      $$ = op;
    }
    else {
      $$ = WRAP(@2, new (MEMMGR) FTOr($1, $3, MEMMGR));
    }
	}
	| FTAnd
	;

// [146]    FTAnd    ::=    FTMildnot ( "&&" FTMildnot )*
FTAnd:
	FTAnd _DOUBLE_AMPERSAND_ FTMildnot
	{
    if($1->getType() == FTSelection::AND) {
      FTAnd *op = (FTAnd*)$1;
      op->addArg($3);
      $$ = op;
    }
    else {
      $$ = WRAP(@2, new (MEMMGR) FTAnd($1, $3, MEMMGR));
    }
	}
	| FTMildnot
	;

// [147]    FTMildnot    ::=    FTUnaryNot ( "not" "in" FTUnaryNot )*
FTMildnot:
	FTMildnot _NOT_IN_ FTUnaryNot
	{
    $$ = WRAP(@2, new (MEMMGR) FTMildnot($1, $3, MEMMGR));
	}
	| FTUnaryNot
	;

// [148]    FTUnaryNot    ::=    ("!")? FTWordsSelection
FTUnaryNot:
	_BANG_ FTWordsSelection
	{
    $$ = WRAP(@1, new (MEMMGR) FTUnaryNot($2, MEMMGR));
	}
	| FTWordsSelection
	;

// [149]    FTWordsSelection    ::=    FTWords | ("(" FTSelection ")")
FTWordsSelection:
	_LPAR_ FTSelection _RPAR_
	{
    $$ = $2;
	}
	| FTWords
	;

// [150]    FTWords    ::=    (Literal | VarRef | ContextItemExpr | FunctionCall | ("{" Expr "}")) FTAnyallOption?
FTWords:
	Literal FTAnyallOption
	{
    $$ = WRAP(@1, new (MEMMGR) FTWords($1, $2, MEMMGR));
	}
	| VarRef FTAnyallOption
	{
    $$ = WRAP(@1, new (MEMMGR) FTWords($1, $2, MEMMGR));
	}
	| ContextItemExpr FTAnyallOption
	{
    $$ = WRAP(@1, new (MEMMGR) FTWords($1, $2, MEMMGR));
	}
	| FunctionCall FTAnyallOption
	{
    $$ = WRAP(@1, new (MEMMGR) FTWords($1, $2, MEMMGR));
	}
	| _LBRACE_ Expr _RBRACE_ FTAnyallOption
	{
    $$ = WRAP(@2, new (MEMMGR) FTWords($2, $4, MEMMGR));
	}
	;

// [151]    FTProximity    ::=    FTOrderedIndicator | FTWindow | FTDistance | FTTimes | FTScope | FTContent
// [152]    FTOrderedIndicator    ::=    "ordered"
// [164]    FTContent    ::=    ("at" "start") | ("at" "end") | ("entire" "content")
// [167]    FTDistance    ::=    "distance" FTRange FTUnit
// [168]    FTWindow    ::=    "window" UnionExpr FTUnit
// [169]    FTTimes    ::=    "occurs" FTRange "times"
// [170]    FTScope    ::=    ("same" | "different") FTBigUnit
FTProximity:
	_ORDERING_ORDERED_
	{
    $$ = WRAP(@1, new (MEMMGR) FTOrder(MEMMGR));
	}
	| _WINDOW_ UnionExpr FTUnit
	{
    $$ = WRAP(@1, new (MEMMGR) FTWindow($2, $3, MEMMGR));
	}
	| _DISTANCE_ FTRange FTUnit
	{
    $$ = WRAP(@1, new (MEMMGR) FTDistance($2, $3, MEMMGR));
	}
	| _OCCURS_ FTRange _TIMES_
	{
    std::cerr << "occurs" << std::endl;
    $$ = NULL;
	}
	| _SAME_ FTBigUnit
	{
    $$ = WRAP(@1, new (MEMMGR) FTScope(FTScope::SAME, $2, MEMMGR));
	}
	| _DIFFERENT_ FTBigUnit
	{
    $$ = WRAP(@1, new (MEMMGR) FTScope(FTScope::DIFFERENT, $2, MEMMGR));
	}
	| _AT_START_
	{
    $$ = WRAP(@1, new (MEMMGR) FTContent(FTContent::AT_START, MEMMGR));
	}
	| _AT_END_
	{
    $$ = WRAP(@1, new (MEMMGR) FTContent(FTContent::AT_END, MEMMGR));
	}
	| _ENTIRE_CONTENT_
	{
    $$ = WRAP(@1, new (MEMMGR) FTContent(FTContent::ENTIRE_CONTENT, MEMMGR));
	}
	;

// [153]    FTMatchOption    ::=    FTCaseOption
//                                  | FTDiacriticsOption
//                                  | FTStemOption
//                                  | FTThesaurusOption
//                                  | FTStopwordOption
//                                  | FTLanguageOption
//                                  | FTWildCardOption
FTMatchOption:
	FTCaseOption
	| FTDiacriticsOption
	| FTStemOption
	| FTThesaurusOption
	| FTStopwordOption
	| FTLanguageOption
	| FTWildCardOption
	;

// [154]    FTCaseOption    ::=    "lowercase"
//                                 | "uppercase"
//                                 | ("case" "sensitive")
//                                 | ("case" "insensitive")
FTCaseOption:
	_LOWERCASE_
	{
    std::cerr << "lowercase" << std::endl;
	}
	| _UPPERCASE_
	{
    std::cerr << "uppercase" << std::endl;
	}
	| _CASE_SENSITIVE_
	{
    std::cerr << "case sensitive" << std::endl;
	}
	| _CASE_INSENSITIVE_
	{
    std::cerr << "case insensitive" << std::endl;
	}
	;

// [155]    FTDiacriticsOption    ::=    ("with" "diacritics")
//                                       | ("without" "diacritics")
//                                       | ("diacritics" "sensitive")
//                                       | ("diacritics" "insensitive")
FTDiacriticsOption:
	_WITH_DIACRITICS_
	{
    std::cerr << "with diacritics" << std::endl;
	}
	| _WITHOUT_DIACRITICS_
	{
    std::cerr << "without diacritics" << std::endl;
	}
	| _DIACRITICS_SENSITIVE_
	{
    std::cerr << "diacritics sensitive" << std::endl;
	}
	| _DIACRITICS_INSENSITIVE_
	{
    std::cerr << "diacritics insensitive" << std::endl;
	}
	;

//      [156]    FTStemOption    ::=    ("with" "stemming") | ("without" "stemming")
FTStemOption:
	_WITH_STEMMING_
	{
    std::cerr << "with stemming" << std::endl;
	}
	| _WITHOUT_STEMMING_
	{
    std::cerr << "without stemming" << std::endl;
	}
	;

// [157]    FTThesaurusOption    ::=    ("with" "thesaurus" (FTThesaurusID | "default"))
//                                    | ("with" "thesaurus" "(" (FTThesaurusID | "default") ("," FTThesaurusID)* ")")
//                                    | ("without" "thesaurus")
FTThesaurusOption:
	_WITH_THESAURUS_ FTThesaurusID
	{
    std::cerr << "with thesaurus" << std::endl;
	}
	| _WITH_THESAURUS_ _DEFAULT_
	{
    std::cerr << "with thesaurus default" << std::endl;
	}
	| _WITH_THESAURUS_ _LPAR_ FTThesaurusID FTThesaurusIDList _RPAR_
	{
    std::cerr << "with thesaurus ()" << std::endl;
	}
	| _WITH_THESAURUS_ _LPAR_ _DEFAULT_ FTThesaurusIDList _RPAR_
	{
    std::cerr << "with thesaurus (default)" << std::endl;
	}
	| _WITHOUT_THESAURUS_
	{
    std::cerr << "without thesaurus" << std::endl;
	}
	;

FTThesaurusIDList:
	/* empty */
	{
	}
	| FTThesaurusIDList _COMMA_ FTThesaurusID
	{
	}
	;

// [158]    FTThesaurusID    ::=    "at" StringLiteral ("relationship" StringLiteral)? (FTRange "levels")?
FTThesaurusID:
	_AT_KEYWORD_ StringLiteral
	{
    std::cerr << "at StringLiteral" << std::endl;
	}
	| _AT_KEYWORD_ StringLiteral _RELATIONSHIP_ StringLiteral
	{
    std::cerr << "at StringLiteral relationship StringLiteral" << std::endl;
	}
	| _AT_KEYWORD_ StringLiteral FTRange _LEVELS_
	{
    std::cerr << "at StringLiteral levels" << std::endl;
	}
	| _AT_KEYWORD_ StringLiteral _RELATIONSHIP_ StringLiteral FTRange _LEVELS_
	{
    std::cerr << "at StringLiteral relationship StringLiteral levels" << std::endl;
	}
	;

// [159]    FTStopwordOption    ::=    ("with" "stop" "words" FTRefOrList FTInclExclStringLiteral*)
//                                   | ("without" "stop" "words")
//                                   | ("with" "default" "stop" "words" FTInclExclStringLiteral*)
FTStopwordOption:
	_WITH_STOP_WORDS_ FTRefOrList FTInclExclStringLiteralList
	{
    std::cerr << "with stop words" << std::endl;
	}
	| _WITHOUT_STOP_WORDS_
	{
    std::cerr << "without stop words" << std::endl;
	}
	| _WITH_DEFAULT_STOP_WORDS_ FTInclExclStringLiteralList
	{
    std::cerr << "with default stop words" << std::endl;
	}
	;

FTInclExclStringLiteralList:
	/* empty */
	{
	}
	| FTInclExclStringLiteralList FTInclExclStringLiteral
	{
	}
	;

// [160]    FTRefOrList    ::=    ("at" StringLiteral)
//                              | ("(" StringLiteral ("," StringLiteral)* ")")
FTRefOrList:
	_AT_KEYWORD_ StringLiteral
	{
    std::cerr << "at StringLiteral" << std::endl;
	}
	| _LPAR_ FTRefOrListStringList _RPAR_
	{
    std::cerr << "()" << std::endl;
	}
	;

FTRefOrListStringList:
	StringLiteral
	{
    std::cerr << "StringLiteral" << std::endl;
	}
	| FTRefOrListStringList _COMMA_ StringLiteral
	{
    std::cerr << ", StringLiteral" << std::endl;
	}
	;

// [161]    FTInclExclStringLiteral    ::=    ("union" | "except") FTRefOrList
FTInclExclStringLiteral:
	_UNION_ FTRefOrList
	{
    std::cerr << "union" << std::endl;
	}
	| _EXCEPT_ FTRefOrList
	{
    std::cerr << "except" << std::endl;
	}
	;

// [162]    FTLanguageOption    ::=    "language" StringLiteral
FTLanguageOption:
	_LANGUAGE_ StringLiteral
	{
    std::cerr << "language StringLiteral" << std::endl;
	}
	;

// [163]    FTWildCardOption    ::=    ("with" "wildcards") | ("without" "wildcards")
FTWildCardOption:
	_WITH_WILDCARDS_
	{
    std::cerr << "with wildcards" << std::endl;
	}
	| _WITHOUT_WILDCARDS_
	{
    std::cerr << "without wildcards" << std::endl;
	}
	;

// [165]    FTAnyallOption    ::=    ("any" "word"?) | ("all" "words"?) | "phrase"
FTAnyallOption:
	/* empty */
	{
    $$ = FTWords::ANY;
	}
	| _ANY_
	{
    $$ = FTWords::ANY;
	}
	| _ANY_WORD_
	{
    $$ = FTWords::ANY_WORD;
	}
	| _ALL_
	{
    $$ = FTWords::ALL;
	}
	| _ALL_WORDS_
	{
    $$ = FTWords::ALL_WORDS;
	}
	| _PHRASE_
	{
    $$ = FTWords::PHRASE;
	}
	;

// [166]    FTRange    ::=    ("exactly" UnionExpr)
//                          | ("at" "least" UnionExpr)
//                          | ("at" "most" UnionExpr)
//                          | ("from" UnionExpr "to" UnionExpr)
FTRange:
	_EXACTLY_ UnionExpr
	{
    $$.type = FTRange::EXACTLY;
    $$.arg1 = $2;
    $$.arg2 = 0;
	}
	| _AT_LEAST_ UnionExpr
	{
    $$.type = FTRange::AT_LEAST;
    $$.arg1 = $2;
    $$.arg2 = 0;
	}
	| _AT_MOST_ UnionExpr
	{
    $$.type = FTRange::AT_MOST;
    $$.arg1 = $2;
    $$.arg2 = 0;
	}
	| _FROM_ UnionExpr _TO_ UnionExpr
	{
    $$.type = FTRange::FROM_TO;
    $$.arg1 = $2;
    $$.arg2 = $4;
	}
	;

// [171]    FTUnit    ::=    "words" | "sentences" | "paragraphs"
FTUnit:
	_WORDS_
	{
    $$ = FTOption::WORDS;
	}
	| _SENTENCES_
	{
    $$ = FTOption::SENTENCES;
	}
	| _PARAGRAPHS_
	{
    $$ = FTOption::PARAGRAPHS;
	}
	;

// [172]    FTBigUnit    ::=    "sentence" | "paragraph"
FTBigUnit:
	_SENTENCE_
	{
    $$ = FTOption::SENTENCES;
	}
	| _PARAGRAPH_
	{
    $$ = FTOption::PARAGRAPHS;
	}
	;

// [173]    FTIgnoreOption    ::=    "without" "content" UnionExpr
FTIgnoreOption:
	_WITHOUT_CONTENT_ UnionExpr
	{
    $$ = $2;
	}
	;

// [174]   	IntegerLiteral	   ::=   	Digits
IntegerLiteral:
      _INTEGER_NUMBER_
		{
			$$ = new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER,
										$1, AnyAtomicType::DECIMAL),
										MEMMGR);
		}
	;

// [175]   	DecimalLiteral	   ::=   	("." Digits) | (Digits "." [0-9]*)
DecimalLiteral:
      _DECIMAL_NUMBER_
		{
			$$ = new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL,
										$1, AnyAtomicType::DECIMAL),
										MEMMGR);
		}
	;

// [176]   	DoubleLiteral	   ::=   	(("." Digits) | (Digits ("." [0-9]*)?)) [eE] [+-]? Digits
DoubleLiteral:
      _DOUBLE_NUMBER_
		{
			$$ = new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE,
										$1, AnyAtomicType::DOUBLE),
										MEMMGR);
		}
	;

// [177]   	StringLiteral	   ::=   	('"' (PredefinedEntityRef | CharRef | ('"' '"') | [^"&])* '"') | 
//										("'" (PredefinedEntityRef | CharRef | ("'" "'") | [^'&])* "'")
StringLiteral:
      _STRING_LITERAL_
		{
			$$ = new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, AnyAtomicType::STRING),
										MEMMGR);
		}
	;


QName:
	  _QNAME_
		{
			$$ = new (MEMMGR) QualifiedName($1,MEMMGR);
		}
	;
%%

}	// namespace XQParser


