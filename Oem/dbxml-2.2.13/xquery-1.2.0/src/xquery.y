%{

#if defined(WIN32) && !defined(__CYGWIN__)
// turn off the warnings "switch statement contains 'default' but no 'case' labels"
//                       "'yyerrlab1' : unreferenced label"
#pragma warning(disable : 4065 4102)
#endif

#include "Scanner.hpp"

#include "xquery/XQQuery.hpp"
#include "xquery/functions/XQFunction.hpp"
#include "xquery/dataItem/XQDOMConstructor.hpp"
#include "xquery/dataItem/XQFLWOR.hpp"
#include "xquery/dataItem/XQQuantified.hpp"
#include "xquery/dataItem/XQTypeswitch.hpp"
#include "xquery/dataItem/XQValidate.hpp"
#include "xquery/dataItem/XQGlobalVariable.hpp"
#include "xquery/dataItem/XQDebugHook.hpp"
#include "xquery/dataItem/XQFunctionCall.hpp"
#include "xquery/dataItem/XQOrderingChange.hpp"

#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/dom/DOMXPathNamespace.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <pathan/AnyAtomicTypeConstructor.hpp>

#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/dataItem/DataItemParenthesizedExpr.hpp>
#include <pathan/dataItem/DataItemNav.hpp>
#include <pathan/dataItem/DataItemStep.hpp>
#include <pathan/dataItem/DataItemLiteral.hpp>
#include <pathan/dataItem/DataItemVariable.hpp>
#include <pathan/dataItem/DataItemQuantifiedExpr.hpp>
#include <pathan/dataItem/DataItemInstanceOf.hpp>
#include <pathan/dataItem/DataItemCastAs.hpp>
#include <pathan/dataItem/DataItemCastableAs.hpp>
#include <pathan/dataItem/DataItemTreatAs.hpp>
#include <pathan/dataItem/DataItemIf.hpp>
#include <pathan/dataItem/DataItemContextItem.hpp>

#include <pathan/internal/parser/QName.hpp>

#include <pathan/operators/Equals.hpp>
#include <pathan/operators/GeneralComp.hpp>
#include <pathan/operators/NotEquals.hpp>
#include <pathan/operators/LessThan.hpp>
#include <pathan/operators/GreaterThan.hpp>
#include <pathan/operators/LessThanEqual.hpp>
#include <pathan/operators/GreaterThanEqual.hpp>
#include <pathan/operators/Plus.hpp>
#include <pathan/operators/Minus.hpp>
#include <pathan/operators/Multiply.hpp>
#include <pathan/operators/Divide.hpp>
#include <pathan/operators/IntegerDivide.hpp>
#include <pathan/operators/Mod.hpp>
#include <pathan/operators/UnaryMinus.hpp>
#include <pathan/operators/NodeComparison.hpp>
#include <pathan/operators/OrderComparison.hpp>
#include <pathan/operators/Intersect.hpp>
#include <pathan/operators/Except.hpp>
#include <pathan/operators/Union.hpp>
#include <pathan/operators/Or.hpp>
#include <pathan/operators/And.hpp>
#include <pathan/operators/Range.hpp>

#include <pathan/internal/navigation/NodeTest.hpp>

#include <pathan/SequenceType.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/NamespaceLookupException.hpp>

#include <pathan/XPath2Utils.hpp>
#ifdef HAVE_CONFIG_H
#include "xquery_config.h"
#endif

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

#define QP						((XQueryParserArgs*)qp)
#define CONTEXT					(QP->_context)
#define MEMMGR					(CONTEXT->getMemoryManager())
#define WRAP(pos,object)		(wrapForDebug((QP),(object),NULL  ,(pos).first_line, (pos).first_column))
#define FNWRAP(pos,name,object)	(wrapForDebug((QP),(object),(name),(pos).first_line, (pos).first_column))

#undef yylex
#define yylex QP->_scanner->yylex
#undef yyerror
#define yyerror QP->_scanner->error

static XMLCh szQuote[]=		 { XERCES_CPP_NAMESPACE_QUALIFIER chDoubleQuote, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szApos[]=		 { XERCES_CPP_NAMESPACE_QUALIFIER chSingleQuote, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szCurlyOpen[]=  { XERCES_CPP_NAMESPACE_QUALIFIER chOpenCurly, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szCurlyClose[]= { XERCES_CPP_NAMESPACE_QUALIFIER chCloseCurly, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szFLWOR[] =     { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_L, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_W, 
                               XERCES_CPP_NAMESPACE_QUALIFIER chLatin_O, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_R, XERCES_CPP_NAMESPACE_QUALIFIER chNull }; 
static XMLCh sz1_0[]=        { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, 
                               XERCES_CPP_NAMESPACE_QUALIFIER chNull };

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

static inline DataItemNav* getNavigation(DataItem *possibleNav, XPath2MemoryManager * expr)
{
	DataItem* originalObj=possibleNav;
	if((unsigned int)possibleNav->getType()==XQContext::DEBUG_HOOK)
		possibleNav=((XQDebugHook*)possibleNav)->m_impl;
	if(possibleNav->getType()==DataItem::NAVIGATION)
		return (DataItemNav*)possibleNav;
	else
	{
		DataItemNav* nav=new (expr) DataItemNav(expr);
		nav->addStep(originalObj);
		return nav;
	}
}

static inline VectorOfDataItems packageArgs(DataItem *arg1Impl, DataItem *arg2Impl, XPath2MemoryManager* memMgr)
{
	VectorOfDataItems args=VectorOfDataItems(2,(DataItem*)NULL,PathanAllocator<DataItem*>(memMgr));
	args[0]=arg1Impl;
	args[1]=arg2Impl;

	return args;
}

static void merge_strings(DynamicContext* context, VectorOfDataItems* vec, XMLCh* toBeAdded)
{
	if(vec->size()>0 && vec->back()->getType()==DataItem::LITERAL)
	{
		DataItemLiteral *lit = (DataItemLiteral*)vec->back();
		const XMLCh* string=lit->getItemConstructor()->createItem(context)->asString(context);
		string=XPath2Utils::concatStrings(string,toBeAdded,context->getMemoryManager());

		AnyAtomicTypeConstructor *ic = new (context->getMemoryManager())
    		AnyAtomicTypeConstructor(
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
            string, /*isNumeric*/false);
    	lit->setItemConstructor(ic);
	}
	else
	{
    	AnyAtomicTypeConstructor *ic = new (context->getMemoryManager())
      		AnyAtomicTypeConstructor(
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
				toBeAdded, /*isNumeric*/false);

		vec->push_back(new (context->getMemoryManager())
                   DataItemLiteral(ic, context->getMemoryManager()));
	}
}

static DataItem* wrapForDebug(XQueryParserArgs *qp, DataItem* pObjToWrap,
                              const XMLCh* fnName, unsigned int line, unsigned int column)
{
  if(!CONTEXT->getDebugCallback())
    return pObjToWrap;
  if(fnName==NULL && (unsigned int)pObjToWrap->getType()==XQContext::DEBUG_HOOK)
    return pObjToWrap;
  return new (MEMMGR) XQDebugHook(QP->_query->getFile(), line, column, pObjToWrap, fnName, MEMMGR);
}

namespace XQuery {

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
%token <str> _XQUERY_VERSION_						"xquery version <ver>"
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
%token _ORDER_BY_						"order by"
%token _STABLE_ORDER_BY_				"stable order by"
%token _ASCENDING_						"ascending"
%token _DESCENDING_						"descending"
%token _EMPTY_GREATEST_					"empty greatest"
%token _EMPTY_LEAST_					"empty least"
%token _COLLATION_						"collation"
%token _SOME_							"some"
%token _EVERY_							"every"
%token _SATISFIES_						"satisfies"
%token _TYPESWITCH_						"typeswitch"
%token _DEFAULT_						"default"
%token _CASE_							"case"
%token _AS_								"as"
%token _IF_								"if"
%token _THEN_							"then"
%token _ELSE_							"else"
%token _OR_								"or"
%token _AND_							"and"
%token _INSTANCE_OF_					"instance of"
%token _CASTABLE_AS_					"castable as"
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
%token _VALIDATE_CONTENT_				"validate {"
%token _VALIDATE_MODE_LAX_				"validate lax"
%token _VALIDATE_MODE_STRICT_			"validate strict"
%token _CAST_AS_						"cast as"
%token _TREAT_AS_						"treat as"
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
%token _PRESERVE_						"preserve (xmlspace)"
%token _STRIP_							"strip (xmlspace)"
%token _NAMESPACE_						"namespace"
%token _EXPR_AS_						") as"
%token _EMPTY_							"empty()"
%token _ITEM_							"item()"
%token _NILLABLE_						"?"
%token _IMPORT_SCHEMA_					"import schema"
%token _IMPORT_MODULE_					"import module"
%token _DOCUMENT_NODE_LPAR_				"document-node("
%token _ATTRIBUTE_LPAR_					"attribute("
%token _ELEMENT_LPAR_					"element("
%token _EXTERNAL_						"external"
%token _MODULE_NAMESPACE_				"module namespace"
%token _PROCESSING_INSTRUCTION_CONSTR_	"processing-instruction {"
%token _COMMENT_CONSTR_					"comment {"
%token _AXIS_ANCESTOR_OR_SELF_			"ancestor-or-self::"
%token _AXIS_ANCESTOR_					"ancestor::"
%token _AXIS_FOLLOWING_SIBLING_			"following-sibling::"
%token _AXIS_FOLLOWING_					"following::"
%token _AXIS_PRECEDING_SIBLING_			"preceding-sibling::"
%token _AXIS_PRECEDING_					"preceding::"
%token _SEMICOLON_						";"
%token _DEFAULT_ELEMENT_				"default element"
%token _SCHEMA_ELEMENT_LPAR_			"schema-element("
%token _SCHEMA_ATTRIBUTE_LPAR_			"schema-attribute("
%token _ORDERED_						"ordered {"
%token _UNORDERED_						"unordered {"
%token _CONSTRUCTION_PRESERVE_			"preserve (construction)"
%token _CONSTRUCTION_STRIP_				"strip (construction)"
%token _ORDERING_UNORDERED_				"unordered"
%token _ORDERING_ORDERED_				"ordered"
%token _ZERO_OR_ONE_					"? (occurrence)"
%token _ONE_OR_MORE_					"+ (occurrence)"
%token _ZERO_OR_MORE_					"* (occurrence)"
%token _ENCODING_						"encoding"
%token _NS_PRESERVE_					"preserve (copy-namespaces)"
%token _NS_NO_PRESERVE_					"no-preserve"
%token _NS_INHERIT_						"inherit"
%token _NS_NO_INHERIT_					"no-inherit"
%token _DECLARE_						"declare"
%token _BOUNDARYSPACE_					"boundary-space"
%token _DEFAULT_COLLATION_				"default collation"
%token _DEFAULT_FUNCTION_				"default function"
%token _BASEURI_						"base-uri"
%token _CONSTRUCTION_					"construction"
%token _ORDERING_						"ordering"
%token _DEFAULT_ORDER_					"default order"
%token _COPY_NAMESPACES_				"copy-namespaces"
%token _DECLARE_FUNCTION_				"declare function"
%token _DECLARE_VARIABLE_				"declare variable"
%token _DECLARE_OPTION_					"declare option"
%token _EOF_

%type <functDecl>			FunctionDecl 
%type <funcParam>			Param
%type <funcParams>			ParamList
%type <dataItem>			Expr ExprSingle OrExpr AndExpr EnclosedExpr FLWORExpr IfExpr ComparisonExpr DecimalLiteral VarRef
%type <dataItem>			RangeExpr AdditiveExpr MultiplicativeExpr UnionExpr QuantifiedExpr StringLiteral Literal ContextItemExpr
%type <dataItem>			UnaryExpr ValidateExpr CastExpr TreatExpr IntersectExceptExpr ParenthesizedExpr PrimaryExpr FunctionCall
%type <dataItem>			RelativePathExpr StepExpr AxisStep FilterExpr TypeswitchExpr ValueExpr PathExpr NumericLiteral IntegerLiteral 
%type <dataItem>			CastableExpr Constructor ComputedConstructor DirElemConstructor DirCommentConstructor DirPIConstructor  
%type <dataItem>			CompElemConstructor CompTextConstructor CompPIConstructor CompCommentConstructor OrderedExpr UnorderedExpr
%type <dataItem>			CompAttrConstructor WhereClause CompDocConstructor DoubleLiteral InstanceofExpr DirectConstructor 
%type <dataItem>			ContentExpr ExtensionExpr 
%type <dataItem>      		ForwardStep ReverseStep AbbrevForwardStep AbbrevReverseStep
%type <itemList>			DirElementContent DirAttributeList QuotAttrValueContent AposAttrValueContent DirAttributeValue PredicateList FunctionCallArgumentList
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
%type <str>					PositionalVar SchemaPrefix CommonContent CdataSection URILiteral

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
	_XQUERY_VERSION_ Separator
	{
		if(!XPath2Utils::equals($1,sz1_0))
			yyerror("This XQuery processor only supports version 1.0 of the specs");
	}
	| _XQUERY_VERSION_ _ENCODING_ _STRING_LITERAL_ Separator
	{
		if(!XPath2Utils::equals($1,sz1_0))
			yyerror("This XQuery processor only supports version 1.0 of the specs");
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
	_MODULE_NAMESPACE_ _NCNAME_ _EQUALS_ URILiteral Separator
	{
		QP->_query->setIsLibraryModule();
		QP->_query->setModuleTargetNamespace($4);
		CONTEXT->setNamespaceBinding($2,$4);
	}
	;

// [6]    Prolog    ::=    ((Setter | Import | NamespaceDecl | DefaultNamespaceDecl) Separator)* 
//						   ((VarDecl | FunctionDecl | OptionDecl) Separator)*
Prolog:
	Setters VariablesAndFunctions
	;

Setters:
	/* empty */
	| Setters Setter Separator
	| Setters Import Separator
	| Setters NamespaceDecl Separator
	| Setters DefaultNamespaceDecl Separator
;

VariablesAndFunctions:
	/* empty */
	| VariablesAndFunctions VarDecl Separator
	| VariablesAndFunctions FunctionDecl Separator
	{
		XQFunction* decl=$2;
		CONTEXT->addCustomFunction(decl);
		QP->_query->addFunction(decl);
	}
	| VariablesAndFunctions OptionDecl Separator
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
			CONTEXT->setNamespaceBinding($3,$5);
		}
	;

// [11]    BoundarySpaceDecl     ::=    <"declare" "boundary-space"> ("preserve" |  "strip") 
BoundarySpaceDecl :
	  _DECLARE_ _BOUNDARYSPACE_ _PRESERVE_
		{
			CONTEXT->setPreserveBoundarySpace(true);
		}
	| _DECLARE_ _BOUNDARYSPACE_ _STRIP_
		{
			CONTEXT->setPreserveBoundarySpace(false);
		}
	;

// [12]    DefaultNamespaceDecl    ::=    (<"declare" "default" "element"> |  <"declare" "default" "function">) "namespace" URILiteral 
DefaultNamespaceDecl:
	  _DECLARE_ _DEFAULT_ELEMENT_ _NAMESPACE_ URILiteral
		{
			CONTEXT->setDefaultElementAndTypeNS($4);
		}
	| _DECLARE_ _DEFAULT_FUNCTION_ _NAMESPACE_ URILiteral
		{ 
			CONTEXT->setDefaultFuncNS($4);
		}
	;

// [13]   	OptionDecl	   ::=   	<"declare" "option"> QName StringLiteral
OptionDecl:
	  _DECLARE_OPTION_ _QNAME_ _STRING_LITERAL_
	;

// [14]    OrderingModeDecl    ::=    <"declare" "ordering"> ("ordered" | "unordered") 
OrderingModeDecl:
	_DECLARE_ _ORDERING_ _ORDERING_ORDERED_
	{
		CONTEXT->setNodeSetOrdering(StaticContext::ORDERING_ORDERED);
	}
	| _DECLARE_ _ORDERING_ _ORDERING_UNORDERED_
	{
		CONTEXT->setNodeSetOrdering(StaticContext::ORDERING_UNORDERED);
	}
	;

// [15]    EmptyOrderDecl    ::=    <"declare" "default" "order"> (<"empty" "greatest"> | <"empty" "least">) 
EmptyOrderDecl:
	_DECLARE_ _DEFAULT_ORDER_ _EMPTY_GREATEST_
	{ 
		CONTEXT->setDefaultFLWOROrderingMode(XQStaticContext::FLWOR_ORDER_EMPTY_GREATEST);
	}
	| _DECLARE_ _DEFAULT_ORDER_ _EMPTY_LEAST_
	{ 
		CONTEXT->setDefaultFLWOROrderingMode(XQStaticContext::FLWOR_ORDER_EMPTY_LEAST);
	}
	;

// [16]    CopyNamespacesDecl    ::=     <"declare" "copy-namespaces"> PreserveMode "," InheritMode
CopyNamespacesDecl:
	_DECLARE_ _COPY_NAMESPACES_ PreserveMode _COMMA_ InheritMode
;

// [17]   	PreserveMode	   ::=   	"preserve" | "no-preserve"
PreserveMode:
	  _NS_PRESERVE_
	{
		CONTEXT->setPreserveNamespaces(true);
	}
	| _NS_NO_PRESERVE_
	{
		CONTEXT->setPreserveNamespaces(false);
	}
	;

// [18]   	InheritMode	   ::=   	"inherit" | "no-inherit"
InheritMode:
	  _NS_INHERIT_
	{
		CONTEXT->setInheritNamespaces(true);
	}
	| _NS_NO_INHERIT_
	{
		CONTEXT->setInheritNamespaces(false);
	}
	;

// [19]    DefaultCollationDecl    ::=    <"declare" "default" "collation"> URILiteral 
DefaultCollationDecl:
		_DECLARE_ _DEFAULT_COLLATION_ URILiteral
		{
			CONTEXT->setDefaultCollation($3);
		}
	  ;

// [20]    BaseURIDecl    ::=    <"declare" "base-uri"> URILiteral
BaseURIDecl:
		_DECLARE_ _BASEURI_ URILiteral
		{
			CONTEXT->setBaseURI($3);
		}
	  ;

// [21]    SchemaImport    ::=    <"import" "schema"> SchemaPrefix? URILiteral (<"at" URILiteral> ("," URILiteral)*)?
SchemaImport:
        _IMPORT_SCHEMA_ SchemaPrefix URILiteral
		{
			CONTEXT->setNamespaceBinding($2,$3);
			CONTEXT->addSchemaLocation($3,NULL);
		}
	  | _IMPORT_SCHEMA_ SchemaPrefix URILiteral ResourceLocations
		{
			CONTEXT->addSchemaLocation($3,$4);
			CONTEXT->setNamespaceBinding($2,$3);
		}
	  | _IMPORT_SCHEMA_ URILiteral
		{ 
			CONTEXT->addSchemaLocation($2,NULL);
		}
	  | _IMPORT_SCHEMA_ URILiteral ResourceLocations
		{
			CONTEXT->addSchemaLocation($2,$3);
		}
	  ;

ResourceLocations:
	_AT_KEYWORD_ URILiteral
	{
		$$ = new (MEMMGR) VectorOfStrings(PathanAllocator<const XMLCh*>(MEMMGR));
		$$->push_back($2);
	}
	| ResourceLocations _COMMA_ URILiteral
	{
		$1->push_back($3);
		$$ = $1;
	}
;

// [22]    SchemaPrefix    ::=    ("namespace" NCName "=") |  (<"default" "element"> "namespace") 
SchemaPrefix:
	  _NAMESPACE_ _NCNAME_ _EQUALS_
		{
			$$ = $2;
		}
	| _DEFAULT_ELEMENT_ _NAMESPACE_
		{
			$$ = (XMLCh*)XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
		}
	;

// [23]    ModuleImport    ::=    <"import" "module"> ("namespace" NCName "=")? URILiteral (<"at" URILiteral> ("," URILiteral)*)?
ModuleImport:
	_IMPORT_MODULE_ _NAMESPACE_ _NCNAME_ _EQUALS_ URILiteral ResourceLocations
	{
		CONTEXT->setNamespaceBinding($3,$5);
		QP->_query->importModule($5,$6,CONTEXT);
	}
	| _IMPORT_MODULE_ _NAMESPACE_ _NCNAME_ _EQUALS_ URILiteral 
	{
		yyerror("Cannot locate module without the 'at <location>' keyword");
	}
	| _IMPORT_MODULE_ URILiteral ResourceLocations
	{
		QP->_query->importModule($2,$3,CONTEXT);
	}
	| _IMPORT_MODULE_ URILiteral 
	{
		yyerror("Cannot locate module without the 'at <location>' keyword");
	}
	;

// [24]    VarDecl    ::=    <"declare" "variable" "$"> VarName TypeDeclaration? ((":=" ExprSingle) | "external")
VarDecl:
	_DECLARE_VARIABLE_ _DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration _COLON_EQ_ ExprSingle
	{
		XQGlobalVariable* var=new (MEMMGR) XQGlobalVariable($3,$4,$6,MEMMGR);
		QP->_query->addVariable(var);
	}
	| _DECLARE_VARIABLE_ _DOLLAR_SIGN_ _VARIABLE_ _COLON_EQ_ ExprSingle
	{
		XQGlobalVariable* var=new (MEMMGR) XQGlobalVariable($3,new (MEMMGR) SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR),$5,MEMMGR);
		QP->_query->addVariable(var);
	}
	| _DECLARE_VARIABLE_ _DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration _EXTERNAL_
	{
		XQGlobalVariable* var=new (MEMMGR) XQGlobalVariable($3,$4,NULL,MEMMGR);
		QP->_query->addVariable(var);
	}
	| _DECLARE_VARIABLE_ _DOLLAR_SIGN_ _VARIABLE_ _EXTERNAL_
	{
		XQGlobalVariable* var=new (MEMMGR) XQGlobalVariable($3,new (MEMMGR) SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR),NULL,MEMMGR);
		QP->_query->addVariable(var);
	}
	;

// [25]    ConstructionDecl    ::=    <"declare" "construction"> ("preserve" | "strip") 
ConstructionDecl:
	_DECLARE_ _CONSTRUCTION_ _CONSTRUCTION_PRESERVE_
	{
		CONTEXT->setConstructionMode(XQStaticContext::CONSTRUCTION_MODE_PRESERVE);
	}
	| _DECLARE_ _CONSTRUCTION_ _CONSTRUCTION_STRIP_
	{
		CONTEXT->setConstructionMode(XQStaticContext::CONSTRUCTION_MODE_STRIP);
	}
	;

// [26]    FunctionDecl    ::=    <"declare" "function"> <QName "("> ParamList? (")" |  (<")" "as"> SequenceType)) 
//										(EnclosedExpr | "external")
FunctionDecl:
	  _DECLARE_FUNCTION_ _FUNCTION_CALL_ ParamList _RPAR_ EnclosedExpr
		{
			$$ = new (MEMMGR) XQFunction($2,$3,$5,NULL, MEMMGR); 
			$$->setURI(CONTEXT->getUriBoundToPrefix($$->getPrefix()));
		}
	| _DECLARE_FUNCTION_ _FUNCTION_CALL_ _RPAR_ EnclosedExpr
		{
			$$ = new (MEMMGR) XQFunction($2,NULL,$4,NULL, MEMMGR); 
			$$->setURI(CONTEXT->getUriBoundToPrefix($$->getPrefix()));
		}
	| _DECLARE_FUNCTION_ _FUNCTION_CALL_ ParamList _EXPR_AS_ SequenceType EnclosedExpr
		{
			$$ = new (MEMMGR) XQFunction($2,$3,$6,$5, MEMMGR); 
			$$->setURI(CONTEXT->getUriBoundToPrefix($$->getPrefix()));
		}
	| _DECLARE_FUNCTION_ _FUNCTION_CALL_ _EXPR_AS_ SequenceType EnclosedExpr
		{
			$$ = new (MEMMGR) XQFunction($2,NULL,$5,$4, MEMMGR); 
			$$->setURI(CONTEXT->getUriBoundToPrefix($$->getPrefix()));
		}
	| _DECLARE_FUNCTION_ _FUNCTION_CALL_ ParamList _RPAR_ _EXTERNAL_
		{
			$$ = new (MEMMGR) XQFunction($2,$3,NULL,NULL, MEMMGR); 
			$$->setURI(CONTEXT->getUriBoundToPrefix($$->getPrefix()));
		}
	| _DECLARE_FUNCTION_ _FUNCTION_CALL_ _RPAR_ _EXTERNAL_
		{
			$$ = new (MEMMGR) XQFunction($2,NULL,NULL,NULL, MEMMGR); 
			$$->setURI(CONTEXT->getUriBoundToPrefix($$->getPrefix()));
		}
	| _DECLARE_FUNCTION_ _FUNCTION_CALL_ ParamList _EXPR_AS_ SequenceType _EXTERNAL_
		{
			$$ = new (MEMMGR) XQFunction($2,$3,NULL,$5, MEMMGR); 
			$$->setURI(CONTEXT->getUriBoundToPrefix($$->getPrefix()));
		}
	| _DECLARE_FUNCTION_ _FUNCTION_CALL_ _EXPR_AS_ SequenceType _EXTERNAL_
		{
			$$ = new (MEMMGR) XQFunction($2,NULL,NULL,$4, MEMMGR); 
			$$->setURI(CONTEXT->getUriBoundToPrefix($$->getPrefix()));
		}
	;

// [27]    ParamList    ::=    Param ("," Param)* 
ParamList:
        ParamList _COMMA_ Param
		{
			$1->push_back($3);
			$$ = $1;
		}
      | Param
		{
			XQFunction::VectorOfFunctionParameters* paramList = new (MEMMGR) XQFunction::VectorOfFunctionParameters(PathanAllocator<XQFunction::XQFunctionParameter*>(MEMMGR));
			paramList->push_back($1);
			$$ = paramList;
		}
      ;

// [28]    Param    ::=    "$" VarName TypeDeclaration? 
Param:
        _DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration
		{
			$$ = new (MEMMGR) XQFunction::XQFunctionParameter($2,$3,MEMMGR);
		}
	  | _DOLLAR_SIGN_ _VARIABLE_
		{
			$$ = new (MEMMGR) XQFunction::XQFunctionParameter($2,new (MEMMGR) SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR) ,MEMMGR);
		}
      ;

// [29]    EnclosedExpr    ::=    "{" Expr "}" 
EnclosedExpr:
		_LBRACE_ Expr _RBRACE_
		{ 
			$$ = WRAP(@2, $2); 
		}
      ;

// [30]    QueryBody    ::=    Expr
QueryBody:
	Expr
	{
	    QP->_query->setQueryBody($1);
	}
	;

// [31]    Expr    ::=    ExprSingle ("," ExprSingle)* 
Expr:
		Expr _COMMA_ ExprSingle
		{
			DataItem* prevExpr=$1;
			if((unsigned int)prevExpr->getType()==XQContext::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()==DataItem::PARENTHESIZED)
			{
				((DataItemParenthesizedExpr *)prevExpr)->addItem($3);
				$$ = $1;
			}
			else
			{
				DataItemParenthesizedExpr *dis = new (MEMMGR) DataItemParenthesizedExpr(MEMMGR);
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

// [32]     ExprSingle    ::=    FLWORExpr |  QuantifiedExpr |  TypeswitchExpr |  IfExpr |  OrExpr 
ExprSingle:
		FLWORExpr
	  | QuantifiedExpr
	  | TypeswitchExpr
	  | IfExpr
	  | OrExpr
	  ;

// [33]    FLWORExpr    ::=    (ForClause |  LetClause)+ WhereClause? OrderByClause? "return" ExprSingle 
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

// [34]    ForClause    ::=    <"for" "$"> VarName TypeDeclaration? PositionalVar? "in" ExprSingle 
//								  ("," "$" VarName TypeDeclaration? PositionalVar? "in" ExprSingle)* 
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
			$$ = new (MEMMGR) VectorOfVariableBinding(PathanAllocator<XQVariableBinding*>(MEMMGR));
			$$->push_back($1);
		}
	  ;

ForBinding:
	    _DOLLAR_SIGN_ _VARIABLE_ _IN_ ExprSingle 
		{
			$$ = new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$4);
			$$->_line=@1.first_line;
			$$->_file=QP->_query->getFile();
		}
	  | _DOLLAR_SIGN_ _VARIABLE_ PositionalVar _IN_ ExprSingle 
		{
			$$ = new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$5,$3);
			$$->_line=@1.first_line;
			$$->_file=QP->_query->getFile();
		}
	  |	_DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration _IN_ ExprSingle 
		{
			$$ = new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$5,NULL,$3);
			$$->_line=@1.first_line;
			$$->_file=QP->_query->getFile();
		}
	  |	_DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration PositionalVar _IN_ ExprSingle 
		{
			$$ = new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$6,$4,$3);
			$$->_line=@1.first_line;
			$$->_file=QP->_query->getFile();
		}
	  ;

// [35]    PositionalVar    ::=    "at" "$" VarName 
PositionalVar:
      _AT_KEYWORD_ _DOLLAR_SIGN_ _VARIABLE_
		{ 
			$$ = $3; 
		}
    ;

// [36]    LetClause    ::=    <"let" "$"> VarName TypeDeclaration? ":=" ExprSingle 
//								  ("," "$" VarName TypeDeclaration? ":=" ExprSingle)* 
LetClause:
		_LET_ LetBindingList
		{
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
			$$ = new (MEMMGR) VectorOfVariableBinding(PathanAllocator<XQVariableBinding*>(MEMMGR));
			$$->push_back($1);
		}
    ;

LetBinding:
      _DOLLAR_SIGN_ _VARIABLE_ _COLON_EQ_ ExprSingle
		{
			$$ = new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::letBinding,$2,$4);
			$$->_line=@1.first_line;
			$$->_file=QP->_query->getFile();
		}
	| _DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration _COLON_EQ_ ExprSingle
		{
			$$ = new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::letBinding,$2,$5,NULL,$3);
			$$->_line=@1.first_line;
			$$->_file=QP->_query->getFile();
		}
    ;

// [37]    WhereClause    ::=    "where" ExprSingle 
WhereClause:
      _WHERE_ ExprSingle
		{ 
			$$ = WRAP(@1, $2);
		}
    ;

// [38]    OrderByClause    ::=    (<"order" "by"> |  <"stable" "order" "by">) OrderSpecList 
OrderByClause:
	  _ORDER_BY_ OrderSpecList
		{
			$$=new (MEMMGR) XQSort(XQSort::unstable,$2);
		}
	| _STABLE_ORDER_BY_ OrderSpecList
		{
			$$=new (MEMMGR) XQSort(XQSort::stable,$2);
		}
	;

// [39]    OrderSpecList    ::=    OrderSpec ("," OrderSpec)* 
OrderSpecList:
	  OrderSpecList _COMMA_ OrderSpec
		{
			$1->push_back($3);
			$$ = $1;
		}
	| OrderSpec
		{
			$$ = new (MEMMGR) XQSort::VectorOfSortSpec(PathanAllocator<XQSort::SortSpec*>(MEMMGR));
			$$->push_back($1);
		}
	;

// [40]    OrderSpec    ::=    ExprSingle OrderModifier 
// [41]    OrderModifier    ::=    ("ascending" |  "descending")? (<"empty" "greatest"> |  <"empty" "least">)? ("collation" URILiteral)? 
OrderSpec:
	  ExprSingle OrderDirection EmptyHandling
		{
			$$ = new (MEMMGR) XQSort::SortSpec($1,$2+$3,NULL);
		}
	| ExprSingle OrderDirection EmptyHandling _COLLATION_ URILiteral
		{
			$$ = new (MEMMGR) XQSort::SortSpec($1,$2+$3,$5);
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
			case XQStaticContext::FLWOR_ORDER_EMPTY_LEAST:
				$$ = XQSort::SortSpec::empty_least; break;
			case XQStaticContext::FLWOR_ORDER_EMPTY_GREATEST:
				$$ = XQSort::SortSpec::empty_greatest; break;
			}
		}
    | _EMPTY_GREATEST_
		{ $$ = XQSort::SortSpec::empty_greatest; }
    | _EMPTY_LEAST_
		{ $$ = XQSort::SortSpec::empty_least; }
	;

// [42]    QuantifiedExpr    ::=    (<"some" "$"> |  <"every" "$">) VarName TypeDeclaration? "in" ExprSingle 
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
			$$ = new (MEMMGR) VectorOfVariableBinding(PathanAllocator<XQVariableBinding*>(MEMMGR));
			$$->push_back($1);
		}
	  ;

QuantifyBinding:
	    _DOLLAR_SIGN_ _VARIABLE_ _IN_ ExprSingle 
		{
			$$ = new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$4);
			$$->_line=@1.first_line;
			$$->_file=QP->_query->getFile();
		}
	  |	_DOLLAR_SIGN_ _VARIABLE_ TypeDeclaration _IN_ ExprSingle 
		{
			$$ = new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,$2,$5,NULL,$3);
			$$->_line=@1.first_line;
			$$->_file=QP->_query->getFile();
		}
	  ;

// [43]    TypeswitchExpr    ::=    <"typeswitch" "("> Expr ")" CaseClause+ "default" ("$" VarName)? "return" ExprSingle 
TypeswitchExpr:
		_TYPESWITCH_ _LPAR_ Expr _RPAR_ CaseClauseList _DEFAULT_ _DOLLAR_SIGN_ _VARIABLE_ _RETURN_ ExprSingle
		{
			XQTypeswitch::Clause* defClause=new (MEMMGR) XQTypeswitch::Clause(NULL,WRAP(@6, $10),MEMMGR->getPooledString($8));
			$$ = new (MEMMGR) XQTypeswitch( WRAP(@1, $3), $5, defClause, MEMMGR);
		}
	  | _TYPESWITCH_ _LPAR_ Expr _RPAR_ CaseClauseList _DEFAULT_ _RETURN_ ExprSingle
		{
			XQTypeswitch::Clause* defClause=new (MEMMGR) XQTypeswitch::Clause(NULL,WRAP(@6, $8),NULL);
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
			$$=new (MEMMGR) XQTypeswitch::VectorOfClause(PathanAllocator<XQTypeswitch::Clause*>(MEMMGR));
			$$->push_back($1);
		}
	  ;

// [44]    CaseClause    ::=    "case" ("$" VarName "as")? SequenceType "return" ExprSingle
CaseClause:
	  _CASE_ SequenceType _RETURN_ ExprSingle
		{ 
			$$ = new (MEMMGR) XQTypeswitch::Clause($2, WRAP(@1, $4), NULL);
		}
	|  _CASE_ _DOLLAR_SIGN_ _VARIABLE_ _AS_ SequenceType _RETURN_ ExprSingle
		{ 
			$$ = new (MEMMGR) XQTypeswitch::Clause($5, WRAP(@1, $7), MEMMGR->getPooledString($3));
		}
	;

// [45]    IfExpr    ::=    <"if" "("> Expr ")" "then" ExprSingle "else" ExprSingle 
IfExpr:
	  _IF_ _LPAR_ Expr _RPAR_ _THEN_ ExprSingle _ELSE_ ExprSingle
		{ 
	        $$ = WRAP(@1, new (MEMMGR) DataItemIf($3, $6, $8, MEMMGR));
		}
	;

// [46]    OrExpr    ::=    AndExpr ( "or"  AndExpr )* 
OrExpr:
		OrExpr _OR_ AndExpr
		{
			DataItem* prevExpr=$1;
			if((unsigned int)prevExpr->getType()==XQContext::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()==DataItem::OPERATOR && 
			   XPath2Utils::equals(((DataItemOperator*)prevExpr)->getOperatorName(),Or::name))
			{
				((Or*)prevExpr)->addArgument($3);
				$$ = prevExpr;
			}
			else
				$$ = WRAP(@1, new (MEMMGR) Or(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
	  | AndExpr
	  ;

// [47]    AndExpr    ::=    ComparisonExpr ( "and" ComparisonExpr )* 
AndExpr:
		AndExpr _AND_ ComparisonExpr
		{
			DataItem* prevExpr=$1;
			if((unsigned int)prevExpr->getType()==XQContext::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()==DataItem::OPERATOR && 
			   XPath2Utils::equals(((DataItemOperator*)prevExpr)->getOperatorName(),And::name))
			{
				((And*)prevExpr)->addArgument($3);
				$$ = prevExpr;
			}
			else
				$$ = WRAP(@1, new (MEMMGR) And(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
	  | ComparisonExpr
      ;

// [48]    ComparisonExpr    ::=    RangeExpr ( (ValueComp 
//									|  GeneralComp 
//									|  NodeComp)  RangeExpr )? 
// [60]    GeneralComp    ::=    "=" |  "!=" |  "<" |  "<=" |  ">" |  ">=" 
// [61]    ValueComp    ::=    "eq" |  "ne" |  "lt" |  "le" |  "gt" |  "ge" 
// [62]    NodeComp    ::=    "is" |  "<<" |  ">>" 
ComparisonExpr:
	  RangeExpr _EQUALS_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::EQUAL,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _NOT_EQUALS_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::NOT_EQUAL,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _LT_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::LESS_THAN,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _LT_EQUALS_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::LESS_THAN_EQUAL,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _GT_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::GREATER_THAN,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _GT_EQUALS_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GeneralComp(GeneralComp::GREATER_THAN_EQUAL,packageArgs($1,$3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _FORTRAN_EQ_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) Equals(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _FORTRAN_NE_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) NotEquals(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _FORTRAN_LT_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) LessThan(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _FORTRAN_LE_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) LessThanEqual(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _FORTRAN_GT_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GreaterThan(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _FORTRAN_GE_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) GreaterThanEqual(packageArgs($1, $3, MEMMGR),MEMMGR));
		}	
	| RangeExpr _IS_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) NodeComparison(packageArgs($1, $3, MEMMGR), MEMMGR));
		}
	| RangeExpr _LT_LT_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) OrderComparison(packageArgs($1, $3, MEMMGR), true, MEMMGR));
		}
	| RangeExpr _GT_GT_ RangeExpr
		{
			$$ = WRAP(@2, new (MEMMGR) OrderComparison(packageArgs($1, $3, MEMMGR), false,MEMMGR));
		}
	| RangeExpr
	;

// [49]    RangeExpr    ::=    AdditiveExpr ( "to"  AdditiveExpr )?
RangeExpr:
      AdditiveExpr _TO_ AdditiveExpr
		{
			$$ = WRAP(@2, new (MEMMGR) Range(packageArgs($1, $3, MEMMGR),MEMMGR));
		}
	| AdditiveExpr
    ;

// [50]    AdditiveExpr    ::=    MultiplicativeExpr ( ("+" |  "-")  MultiplicativeExpr )* 
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

// [51]    MultiplicativeExpr    ::=    UnionExpr ( ("*" |  "div" |  "idiv" |  "mod")  UnionExpr )* 
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

// [52]    UnionExpr    ::=    IntersectExceptExpr ( ("union" |  "|")  IntersectExceptExpr )* 
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

// [53]    IntersectExceptExpr    ::=    InstanceofExpr ( ("intersect" |  "except")  InstanceofExpr )* 
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

// [54]    InstanceofExpr    ::=    TreatExpr ( <"instance" "of"> SequenceType )? 
InstanceofExpr:
	TreatExpr _INSTANCE_OF_ SequenceType
	{
		$$ = WRAP(@2, new (MEMMGR) DataItemInstanceOf($1,$3,MEMMGR));
	}
	| TreatExpr
	;

// [55]    TreatExpr    ::=    CastableExpr ( <"treat" "as"> SequenceType )? 
TreatExpr:
	CastableExpr _TREAT_AS_ SequenceType
	{
		$$ = WRAP(@2, new (MEMMGR) DataItemTreatAs($1,$3,MEMMGR));
	}
	| CastableExpr
	;

// [56]    CastableExpr    ::=    CastExpr ( <"castable" "as"> SingleType )? 
CastableExpr:
	CastExpr _CASTABLE_AS_ SingleType
	{
		$$ = WRAP(@2, new (MEMMGR) DataItemCastableAs($1,$3,MEMMGR));
	}
	| CastExpr
	;

// [57]    CastExpr    ::=    UnaryExpr (<"cast" "as"> SingleType)?
CastExpr:
	UnaryExpr _CAST_AS_ SingleType
	{
		$$ = WRAP(@1, new (MEMMGR) DataItemCastAs($1,$3,MEMMGR));
	}
	| UnaryExpr
	;

// [58]    UnaryExpr    ::=    ("-" |  "+")* ValueExpr 
UnaryExpr:
      _MINUS_ UnaryExpr
		{
			VectorOfDataItems args(PathanAllocator<DataItem*>(MEMMGR));
			args.push_back($2);
			$$ = new (MEMMGR) UnaryMinus(args, MEMMGR);
		}
    | _PLUS_ UnaryExpr
		{
			$$ = $2;
		}
	| ValueExpr 
    ;

// [59]    ValueExpr    ::=    ValidateExpr | PathExpr | ExtensionExpr
ValueExpr:
	  ValidateExpr
	| PathExpr
	| ExtensionExpr
	;

// [63]    ValidateExpr    ::=    (<"validate" "{"> |  
//								   (<"validate" ValidationMode> "{")
//								  ) Expr "}" 
ValidateExpr:
	  _VALIDATE_CONTENT_ _LBRACE_ Expr _RBRACE_
		{
			$$ = WRAP(@1, new (MEMMGR) XQValidate($3,DocumentCache::VALIDATION_STRICT,MEMMGR));
		}
	| _VALIDATE_MODE_LAX_ _LBRACE_ Expr _RBRACE_
		{
			$$ = WRAP(@1, new (MEMMGR) XQValidate($3,DocumentCache::VALIDATION_LAX,MEMMGR));
		}
	| _VALIDATE_MODE_STRICT_ _LBRACE_ Expr _RBRACE_
		{
			$$ = WRAP(@1, new (MEMMGR) XQValidate($3,DocumentCache::VALIDATION_STRICT,MEMMGR));
		}
	;

// [64]   	ExtensionExpr	   ::=   	Pragma+ "{" Expr? "}"
ExtensionExpr:
	  PragmaList _LBRACE_ _RBRACE_
	{
		// we don't support any pragma
		yyerror("This pragma is not recognized, and no alternative expression is specified [err:XQ0079]");
	}
	| PragmaList _LBRACE_ Expr _RBRACE_
	{
		// we don't support any pragma
		$$ = $3;
	}
	;

PragmaList:
	  Pragma
	| PragmaList Pragma
	;

// [65]   	Pragma	   ::=   	"(#" S? QName PragmaContents "#)"
// [66]   	PragmaContents	   ::=   	(Char* - (Char* '#)' Char*))
Pragma:
	  _PRAGMA_OPEN_ _PRAGMA_NAME_ _PRAGMA_CONTENT_ _PRAGMA_CLOSE_
	;

// [67]    PathExpr    ::=    ("/" RelativePathExpr?) |  ("//" RelativePathExpr) |  RelativePathExpr 
PathExpr:
	  _SLASH_
  		{
			DataItemNav *nav = new (MEMMGR) DataItemNav(MEMMGR);
			nav->setGotoRootFirst(true);
			$$ = nav;
		}
	| _SLASH_ RelativePathExpr
  		{
			DataItemNav* nav=getNavigation($2,MEMMGR);
			nav->setGotoRootFirst(true);
			$$ = nav;
		}
	| _SLASHSLASH_ RelativePathExpr
  		{
			DataItemNav *newNavigation = getNavigation($2,MEMMGR);
			newNavigation->setGotoRootFirst(true);

      NodeTest *step = new (MEMMGR) NodeTest();
      step->setTypeWildcard();
      step->setNameWildcard();
      step->setNamespaceWildcard();
      newNavigation->addStepFront(new (MEMMGR) DataItemStep(DataItemStep::DESCENDANT_OR_SELF, step, MEMMGR));        

			$$ = newNavigation;
		}
	| RelativePathExpr
	;

// [68]    RelativePathExpr    ::=    StepExpr (("/" |  "//") StepExpr)* 
RelativePathExpr:
	  RelativePathExpr _SLASH_ StepExpr
  		{
			DataItemNav *nav = getNavigation($1,MEMMGR);
			nav->addStep($3);
			$$ = nav;
		}
	| RelativePathExpr _SLASHSLASH_ StepExpr
  		{
			DataItemNav *nav = getNavigation($1,MEMMGR);

      NodeTest *step = new (MEMMGR) NodeTest();
      step->setTypeWildcard();
      step->setNameWildcard();
      step->setNamespaceWildcard();
      nav->addStep(DataItemStep::DESCENDANT_OR_SELF, step);
			nav->addStep($3);

			$$ = nav;
		}
	| StepExpr
	;

// [69]    StepExpr    ::=    AxisStep |  FilterExpr 
StepExpr:
	  AxisStep
	| FilterExpr
	;

// [70]    AxisStep    ::=    (ForwardStep |  ReverseStep) PredicateList 
AxisStep:
	  ForwardStep PredicateList
		{
			$1->addPredicates(*$2);
			$$=$1;
		}
	| ReverseStep PredicateList
		{
			$1->addPredicates(*$2);
			$$=$1;
		}
	;

// [71]    ForwardStep    ::=    (ForwardAxis NodeTest) |  AbbrevForwardStep 
ForwardStep:
	  ForwardAxis NodeTest
		{
      if(!$2->isNodeTypeSet()) {
        switch($1) {
        case DataItemStep::NAMESPACE: $2->setNodeType(Node::namespace_string); break;
        case DataItemStep::ATTRIBUTE: $2->setNodeType(Node::attribute_string); break;
        default: $2->setNodeType(Node::element_string); break;
        }
      }

			$$ = new (MEMMGR) DataItemStep($1,$2,MEMMGR);
		}
	| AbbrevForwardStep
	;

// [72]    ForwardAxis    ::=    <"child" "::">
//								|  <"descendant" "::">
//								|  <"attribute" "::">
//								|  <"self" "::">
//								|  <"descendant-or-self" "::"> 
//								|  <"following-sibling" "::">
//								|  <"following" "::">
ForwardAxis:
	  _AXIS_CHILD_ 
		{
      $$ = DataItemStep::CHILD;
		}
	| _AXIS_DESCENDANT_ 
		{
      $$ = DataItemStep::DESCENDANT;
		}
	| _AXIS_ATTRIBUTE_
		{
      $$ = DataItemStep::ATTRIBUTE;
		}
	| _AXIS_SELF_
		{
      $$ = DataItemStep::SELF;
		}
	| _AXIS_DESCENDANT_OR_SELF_ 
		{
      $$ = DataItemStep::DESCENDANT_OR_SELF;
		}
	| _AXIS_FOLLOWING_SIBLING_ 
		{
      $$ = DataItemStep::FOLLOWING_SIBLING;
		}
	| _AXIS_FOLLOWING_ 
		{
      $$ = DataItemStep::FOLLOWING;
		}
	;

// [73]    AbbrevForwardStep    ::=    "@"? NodeTest
AbbrevForwardStep:
	_AT_ NodeTest
		{
      if(!$2->isNodeTypeSet()) {
        $2->setNodeType(Node::attribute_string);
      }

      $$ = new (MEMMGR) DataItemStep(DataItemStep::ATTRIBUTE, $2, MEMMGR);
		}
	| NodeTest
		{
      DataItemStep::Axis axis = DataItemStep::CHILD;
      SequenceType::ItemType *itemtype = $1->getItemType();
      if(itemtype != 0 &&
         itemtype->getItemTestType() == SequenceType::ItemType::TEST_ATTRIBUTE) {
        axis = DataItemStep::ATTRIBUTE;
      }
      else if(!$1->isNodeTypeSet()) {
        $1->setNodeType(Node::element_string);
      }

      $$ = new (MEMMGR) DataItemStep(axis, $1, MEMMGR);
		}
	;

// [74]    ReverseStep    ::=    (ReverseAxis NodeTest) |  AbbrevReverseStep 
ReverseStep:
	  ReverseAxis NodeTest
		{
      if(!$2->isNodeTypeSet()) {
        $2->setNodeType(Node::element_string);
      }

      $$ = new (MEMMGR) DataItemStep($1, $2, MEMMGR);
		}
	| AbbrevReverseStep 
	;

// [75]    ReverseAxis    ::=    <"parent" "::"> 
//								| <"ancestor" "::">
//								| <"preceding-sibling" "::">
//								| <"preceding" "::">
//								| <"ancestor-or-self" "::">
ReverseAxis:
	  _AXIS_PARENT_
		{
      $$ = DataItemStep::PARENT;
		}
	| _AXIS_ANCESTOR_
		{
      $$ = DataItemStep::ANCESTOR;
		}
	| _AXIS_PRECEDING_SIBLING_
		{
      $$ = DataItemStep::PRECEDING_SIBLING;
		}
	| _AXIS_PRECEDING_
		{
      $$ = DataItemStep::PRECEDING;
		}
	| _AXIS_ANCESTOR_OR_SELF_
		{
      $$ = DataItemStep::ANCESTOR_OR_SELF;
		}
	;

// [76]    AbbrevReverseStep    ::=    ".." 
AbbrevReverseStep:
	  _DOT_DOT_
		{
			NodeTest *step = new (MEMMGR) NodeTest();
			step->setNameWildcard();
			step->setNamespaceWildcard();
			step->setTypeWildcard();
			$$ = new (MEMMGR) DataItemStep(DataItemStep::PARENT, step, MEMMGR);
		}	
	;

// [77]    NodeTest    ::=    KindTest |  NameTest 
NodeTest:
	  KindTest 
	| NameTest
	;

// [78]    NameTest    ::=    QName |  Wildcard 
NameTest:
	  QName
		{
			NodeTest *step = new (MEMMGR) NodeTest();
			try
			{
				const XMLCh* uri = CONTEXT->getUriBoundToPrefix($1->getPrefix());
				step->setNodeUri(uri);
			}
			catch(NamespaceLookupException&)
			{
				step->setNodePrefix($1->getPrefix());
			}
			step->setNodeName($1->getName());
			$$ = step;
		}
	| Wildcard
	;

// [79]    Wildcard    ::=    "*" |  <NCName ":" "*"> |  <"*" ":" NCName> 
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
			try
			{
				const XMLCh* uri = CONTEXT->getUriBoundToPrefix($1);
				step->setNodeUri(uri);
			}
			catch(NamespaceLookupException&)
			{
				step->setNodePrefix($1);
			}
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

// [80]    FilterExpr    ::=    PrimaryExpr PredicateList 
FilterExpr:
	  PrimaryExpr PredicateList
		{
			$1->addPredicates(*$2);
			$$=$1;
		}
	;

// [81]    PredicateList    ::=    Predicate* 
// [82]    Predicate    ::=    "[" Expr "]" 
PredicateList:
	  /* empty */
		{
	        $$ = new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
		}
	| PredicateList _LBRACK_ Expr _RBRACK_
		{
			$1->push_back($3);
			$$ = $1; 
		}
	;

// [83]    PrimaryExpr    ::=    Literal |  VarRef | ParenthesizedExpr | ContextItemExpr | FunctionCall | Constructor | OrderedExpr | UnorderedExpr
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

// [84]    Literal    ::=    NumericLiteral |  StringLiteral 
Literal:
	  NumericLiteral 
	| StringLiteral 
	;

// [85]    NumericLiteral    ::=    IntegerLiteral |  DecimalLiteral |  DoubleLiteral 
NumericLiteral:
	  IntegerLiteral 
	| DecimalLiteral 
	| DoubleLiteral 
	;

// [86]    VarRef    ::=    "$" VarName 
VarRef:
	_DOLLAR_SIGN_ _VARIABLE_
		{
		    int nColon=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf($2,':');
			if(nColon!=-1)
  				$$ = new (MEMMGR) DataItemVariable($2,MEMMGR);
			else
				$$ = new (MEMMGR) DataItemVariable(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, $2,MEMMGR);
		}
	;
		
// [87]    ParenthesizedExpr    ::=    "(" Expr? ")" 
ParenthesizedExpr:
      _LPAR_ Expr _RPAR_
		{ 
			DataItem* prevExpr=$2;
			if((unsigned int)prevExpr->getType()==XQContext::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()!=DataItem::PARENTHESIZED)
			{
				DataItemParenthesizedExpr *dis = new (MEMMGR) DataItemParenthesizedExpr(MEMMGR);
				dis->addItem($2);
				$$ = WRAP(@2, dis);
			}
			else
				$$ = WRAP(@2, $2);
		}
    | _LPAR_ _RPAR_
		{ 
			$$ = WRAP(@1, new (MEMMGR) DataItemSequence(MEMMGR));
		}
    ;

// [88]    ContextItemExpr    ::=    "." 
ContextItemExpr:
	  _DOT_
		{
			$$ = new (MEMMGR) DataItemContextItem(MEMMGR);
		}
	;

// [89]    OrderedExpr    ::=    <"ordered" "{"> Expr "}" 
OrderedExpr:
	_ORDERED_ _LBRACE_ Expr _RBRACE_
		{
			$$ = new (MEMMGR) XQOrderingChange(StaticContext::ORDERING_ORDERED, $3, MEMMGR);
		}
	;

// [90]    UnorderedExpr    ::=    <"unordered" "{"> Expr "}" 
UnorderedExpr:
	_UNORDERED_ _LBRACE_ Expr _RBRACE_
		{
			$$ = new (MEMMGR) XQOrderingChange(StaticContext::ORDERING_UNORDERED, $3, MEMMGR);
		}
	;

// [91]    FunctionCall    ::=    <QName "("> (ExprSingle ("," ExprSingle)*)? ")" 
FunctionCall:
	  _FUNCTION_CALL_ _RPAR_
		{
			VectorOfDataItems args(PathanAllocator<DataItem*>(MEMMGR));
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
			$$ = new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			$$->push_back($1);
		}	
	;

// [92]    Constructor    ::=    DirectConstructor | ComputedConstructor 
Constructor:
	  DirectConstructor
	| ComputedConstructor
	;

// [93]    DirectConstructor    ::=    DirElemConstructor
//									 | DirCommentConstructor
//									 | DirPIConstructor 
DirectConstructor:
	  DirElemConstructor
	| DirCommentConstructor
	| DirPIConstructor
	;

// [94]    DirElemConstructor    ::=    "<" QName DirAttributeList ("/>" |  (">" DirElementContent* "</" QName S? ">")) 
DirElemConstructor:
      _START_TAG_OPEN_ _TAG_NAME_ DirAttributeList _EMPTY_TAG_CLOSE_
		{ 
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::element_string,
								new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$2, /*isNumeric*/false),
										MEMMGR), 
								$3, content, MEMMGR));
		}
	| _START_TAG_OPEN_ _TAG_NAME_ DirAttributeList _TAG_CLOSE_ DirElementContent _END_TAG_OPEN_ _TAG_NAME_ _TAG_CLOSE_
		{ 
			if(!XPath2Utils::equals($2,$7))
				yyerror("Close tag does not match open tag");
			// if we are requested to strip whitespace-only nodes, check if the last element content should be removed
			VectorOfDataItems* elemContent=$5;
			if(!CONTEXT->getPreserveBoundarySpace() &&
			   elemContent->size()>0 && 
			   elemContent->back()->getType()==DataItem::LITERAL)
			{
                Item::Ptr litVal = ((DataItemLiteral*)elemContent->back())->getItemConstructor()->createItem(CONTEXT);
				if(((AnyAtomicType*)(const Item*)litVal)->getPrimitiveTypeIndex()==AnyAtomicType::STRING &&
                   isAllSpaces(litVal->asString(CONTEXT)))
					elemContent->pop_back();
			}
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::element_string,
							  new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$2, /*isNumeric*/false),
										MEMMGR), 
							  $3, elemContent,MEMMGR));
		}
	;

// [95]    DirAttributeList    ::=    (S (QName S? "=" S? DirAttributeValue)?)* 
DirAttributeList: 
		/* empty */
		{
			$$ = new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
		}
	  | DirAttributeList _ATTRIBUTE_NAME_ _VALUE_INDICATOR_ DirAttributeValue
		{
			$$ = $1;
			DataItem* attrItem=WRAP(@2, new (MEMMGR) XQDOMConstructor(Node::attribute_string,
										   new (MEMMGR) DataItemLiteral(
						new (MEMMGR) AnyAtomicTypeConstructor(
											XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
											XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
											$2, /*isNumeric*/false),
											MEMMGR), 
										  0, $4,MEMMGR));
			if(XPath2Utils::equals($2, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLNSString) ||
			   XERCES_CPP_NAMESPACE_QUALIFIER XMLString::startsWith($2, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLNSColonString))
			{
				$$->insert($$->begin(), attrItem);
			}
			else
			{
				$$->push_back(attrItem);
			}
		}
	  ;

// [96]    DirAttributeValue    ::=    ('"' (EscapeQuot |  QuotAttrValueContent)* '"')
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

// [97]    QuotAttrValueContent    ::=    QuotAttContentChar
//										|  CommonContent
QuotAttrValueContent:
		/* empty */
		{ 
			$$ = new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
		}
      | QuotAttrValueContent EnclosedExpr
		{
			$$ = $1;
			$$->push_back($2);
		}
      | QuotAttrValueContent _ESCAPE_QUOT_
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,szQuote);
		}
	  | QuotAttrValueContent CommonContent
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,$2);
		}
	;

// [98]    AposAttrValueContent    ::=    AposAttContentChar
//										|  CommonContent
AposAttrValueContent:
		/* empty */
		{ 
			$$ = new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
		}
      | AposAttrValueContent EnclosedExpr
		{
			$$ = $1;
			$$->push_back($2);
		}
      | AposAttrValueContent _ESCAPE_APOS_
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,szApos);
		}
      | AposAttrValueContent CommonContent
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,$2);
		}
	  ;

// [99]    DirElementContent    ::=    DirectConstructor 
//									|  ElementContentChar
//									|  CdataSection 
//									|  CommonContent
DirElementContent:
	  /* empty */
		{
			$$ = new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR)); 
		}
	  |	DirElementContent DirectConstructor
		{
			// if the last token was a string literal made of whitesapce and
			// we are adding a node constructor, and the context tells us to strip whitespace, remove it
			if($1->size()>0 && 
			   $1->back()->getType()==DataItem::LITERAL)
			{
				const XMLCh* lastString=NULL;
                Item::Ptr litVal = ((DataItemLiteral*)$1->back())->getItemConstructor()->createItem(CONTEXT);
				if(((AnyAtomicType*)(const Item*)litVal)->getPrimitiveTypeIndex()==AnyAtomicType::STRING)
					lastString=litVal->asString(CONTEXT);
				if(lastString!=NULL && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(lastString)>0 && isAllSpaces(lastString) && !CONTEXT->getPreserveBoundarySpace())
					$1->pop_back();
			}
			$$ = $1;
			$$->push_back($2);
		}
	  | DirElementContent EnclosedExpr
		{
			// if the last token was a string literal made of whitesapce and
			// we are adding an enclosed expression, and the context tells us to strip whitespace, remove it
			if($1->size()>0 && 
			   $1->back()->getType()==DataItem::LITERAL)
			{
				const XMLCh* lastString=NULL;
                Item::Ptr litVal = ((DataItemLiteral*)$1->back())->getItemConstructor()->createItem(CONTEXT);
				if(((AnyAtomicType*)(const Item*)litVal)->getPrimitiveTypeIndex()==AnyAtomicType::STRING)
					lastString=litVal->asString(CONTEXT);
				if(lastString!=NULL && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(lastString)>0 && isAllSpaces(lastString) && !CONTEXT->getPreserveBoundarySpace())
					$1->pop_back();
			}
			$$ = $1;
			$$->push_back($2);
		}
      | DirElementContent CdataSection
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,$2);
		}
	  | DirElementContent CommonContent
		{
			$$ = $1;
			merge_strings(CONTEXT,$$,$2);
		}
	;

// [100]    CommonContent    ::=    PredefinedEntityRef | CharRef | "{{" | "}}" | EnclosedExpr 
// we choose to add the _CHAR_ and remove the EnclosedExpr, so that the type can be a string
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
	| _CHAR_
	;

// [101]    DirCommentConstructor    ::=    "<!--" DirCommentContents "-->"
// [102]    DirCommentContents    ::=    ((Char - '-') | <'-' (Char - '-')>)* 
DirCommentConstructor:
	_XML_COMMENT_
		{
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			content->push_back(new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, /*isNumeric*/false),
										MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::comment_string, 0, 0, content, MEMMGR));
		}
	;

// [103]    DirPIConstructor    ::=    "<?" PITarget (S DirPIContents)? "?>"
// [104]    DirPIContents    ::=    (Char* - (Char* '?>' Char*)) 
DirPIConstructor:
	_PROCESSING_INSTRUCTION_START_ _PI_TARGET_ _PROCESSING_INSTRUCTION_CONTENT_
		{
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			VectorOfDataItems* empty=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			content->push_back(new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$3, /*isNumeric*/false),
										MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::processing_instruction_string,
								      new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$2, /*isNumeric*/false),
										MEMMGR), 
									  empty, content, MEMMGR));
		}
	;

// [105]    CDataSection    ::=    "<![CDATA[" CDataSectionContents "]]>"
// [106]    CDataSectionContents    ::=    (Char* - (Char* ']]>' Char*)) 
CdataSection:
	  _CDATA_SECTION_
		{
			$$ = $1;
		}
	;

// [107]    ComputedConstructor    ::=   CompDocConstructor
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

// [108]    CompDocConstructor    ::=    <"document" "{"> Expr "}" 
CompDocConstructor:
	  _DOCUMENT_CONSTR_ _LBRACE_ Expr _RBRACE_
		{
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			VectorOfDataItems* empty=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			content->push_back(WRAP(@3, $3));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::document_string,
								  new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, /*isNumeric*/false),
										MEMMGR), 
								  empty, content,MEMMGR));
		}
	;

// [109]    CompElemConstructor    ::=    (<"element" QName "{"> |  (<"element" "{"> Expr "}" "{")) ContentExpr? "}" 
CompElemConstructor:
	  _NAMED_ELEMENT_CONSTR_ _LBRACE_ ContentExpr _RBRACE_ 
		{
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			VectorOfDataItems* empty=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			content->push_back(WRAP(@3, $3));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::element_string,
								  new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, /*isNumeric*/false),
										MEMMGR), 
								  empty, content,MEMMGR));
		}
	| _NAMED_ELEMENT_CONSTR_ _LBRACE_ _RBRACE_ 
		{
			VectorOfDataItems* empty=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::element_string,
								  new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, /*isNumeric*/false),
										MEMMGR), 
								  empty, empty,MEMMGR));
		}
	| _ELEMENT_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ ContentExpr _RBRACE_ 
		{
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			VectorOfDataItems* empty=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			content->push_back(WRAP(@6, $6));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::element_string,
								  WRAP(@3, $3), 
								  empty, content, MEMMGR));
		}
	| _ELEMENT_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ _RBRACE_ 
		{
			VectorOfDataItems* empty=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::element_string,
								  WRAP(@3, $3), 
								  empty, empty, MEMMGR));
		}
	;

// [110]    ContentExpr    ::=    Expr
ContentExpr:
	Expr
	;

// [111]    CompAttrConstructor    ::=    (<"attribute" QName "{"> |  (<"attribute" "{"> Expr "}" "{")) Expr? "}" 
CompAttrConstructor:
	  _NAMED_ATTRIBUTE_CONSTR_ _LBRACE_ Expr _RBRACE_ 
		{
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			content->push_back(WRAP(@3, $3));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::attribute_string,
								      new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, /*isNumeric*/false),
										MEMMGR), 
									  0, content,MEMMGR));
		}
	| _NAMED_ATTRIBUTE_CONSTR_ _LBRACE_ _RBRACE_ 
		{
			VectorOfDataItems* empty=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::attribute_string,
								      new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, /*isNumeric*/false),
										MEMMGR), 
									  0, empty,MEMMGR));
		}
	| _ATTRIBUTE_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ Expr _RBRACE_ 
		{
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			content->push_back(WRAP(@6, $6));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::attribute_string,
									  WRAP(@3, $3), 
									  0, content, MEMMGR));
		}
	| _ATTRIBUTE_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ _RBRACE_ 
		{
			VectorOfDataItems* empty=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::attribute_string,
									  WRAP(@3, $3), 
									  0, empty, MEMMGR));
		}
	;

// [112]    CompTextConstructor    ::=    <"text" "{"> Expr "}" 
CompTextConstructor:
	  _TEXT_CONSTR_ _LBRACE_ Expr _RBRACE_
		{
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			content->push_back(WRAP(@3, $3));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::text_string, 0, 0, content, MEMMGR));
		}
	;

// [113]    CompCommentConstructor    ::=    <"comment" "{"> Expr "}" 
CompCommentConstructor:
	  _COMMENT_CONSTR_ _LBRACE_ Expr _RBRACE_
		{
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			content->push_back(WRAP(@3, $3));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::comment_string, 0, 0, content, MEMMGR));
		}
	;

// [114]    CompPIConstructor    ::=    (<"processing-instruction" NCName "{"> | (<"processing-instruction" "{"> Expr "}" "{")) Expr? "}" 
CompPIConstructor:
	  _NAMED_PROCESSING_INSTRUCTION_CONSTR_ _LBRACE_ Expr _RBRACE_
	  {
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			content->push_back(WRAP(@3, $3));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::processing_instruction_string,
								      new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, /*isNumeric*/false),
										MEMMGR), 
									  0, content, MEMMGR));
	  }
	| _NAMED_PROCESSING_INSTRUCTION_CONSTR_ _LBRACE_ _RBRACE_
	  {
			VectorOfDataItems* empty=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::processing_instruction_string,
								      new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, /*isNumeric*/false),
										MEMMGR), 
									  0, empty, MEMMGR));
	  }
	| _PROCESSING_INSTRUCTION_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ Expr _RBRACE_
	  {
			VectorOfDataItems* content=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			content->push_back(WRAP(@6, $6));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::processing_instruction_string,
									  WRAP(@3, $3), 
									  0, content, MEMMGR));
	  }
	| _PROCESSING_INSTRUCTION_CONSTR_ _LBRACE_ Expr _RBRACE_ _LBRACE_ _RBRACE_
	  {
			VectorOfDataItems* empty=new (MEMMGR) VectorOfDataItems(PathanAllocator<DataItem*>(MEMMGR));
			$$ = WRAP(@1, new (MEMMGR) XQDOMConstructor(Node::processing_instruction_string,
									  WRAP(@3, $3), 
									  0, empty, MEMMGR));
	  }
	;

// [115]    SingleType    ::=    AtomicType "?"? 
SingleType:
		AtomicType _ZERO_OR_ONE_
	    {
			SequenceType* seq=new (MEMMGR) SequenceType();
			seq->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE,NULL,$1));
			seq->setOccurrence(SequenceType::QUESTION_MARK);
			$$ = seq;
		}
	  | AtomicType
	    {
			SequenceType* seq=new (MEMMGR) SequenceType();
			seq->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE,NULL,$1));
			seq->setOccurrence(SequenceType::EXACTLY_ONE);
			$$ = seq;
		}
	;

// [116]    TypeDeclaration    ::=    "as" SequenceType 
TypeDeclaration:
	  _AS_ SequenceType
		{
			$$ = $2;
		}
	;

// [117]    SequenceType    ::=    (ItemType OccurrenceIndicator?) |  <"empty" "(" ")">
SequenceType:
	    ItemType OccurrenceIndicator
		{
			SequenceType* seq=new (MEMMGR) SequenceType();
			seq->setItemType($1);
			seq->setOccurrence($2);
			$$ = seq;
		}
	  | ItemType 
		{
			SequenceType* seq=new (MEMMGR) SequenceType();
			seq->setItemType($1);
			seq->setOccurrence(SequenceType::EXACTLY_ONE);
			$$ = seq;
		}
	  | _EMPTY_
		{ 
			$$ = new (MEMMGR) SequenceType(); 
		}
	  ;


// [118]    OccurrenceIndicator    ::=    "*" |  "+" |  "?"
OccurrenceIndicator:
	_ZERO_OR_MORE_
	{ $$ = SequenceType::STAR; }
	| _ONE_OR_MORE_ 
	{ $$ = SequenceType::PLUS; }
	| _ZERO_OR_ONE_ 
	{ $$ = SequenceType::QUESTION_MARK; }
	;

// [119]    ItemType    ::=    AtomicType | KindTest | <"item" "(" ")"> 
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
	| _ITEM_ 
	{
		$$ = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING);
	}
	;

// [120]    AtomicType    ::=    QName 
AtomicType:
	QName
	{
		$$ = $1;
	}
	;

// [121]    KindTest    ::=    DocumentTest
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

// [122]    AnyKindTest    ::=    <"node" "("> ")" 
AnyKindTest:
	_NODE_LPAR_ _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_NODE));
		$$ = step;
	}
	;

// [123]    DocumentTest    ::=    <"document-node" "("> (ElementTest | SchemaElementTest)? ")"
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
	
// [124]    TextTest    ::=    <"text" "("> ")" 
TextTest:
	_TEXT_LPAR_ _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_TEXT));
		$$ = step;
	}
	;

// [125]    CommentTest    ::=    <"comment" "("> ")" 
CommentTest: 
	_COMMENT_LPAR_ _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
		step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_COMMENT));
		$$ = step;
	}
	;

// [126]    PITest    ::=    <"processing-instruction" "("> (NCName | StringLiteral)? ")" 
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

// [127]    AttributeTest    ::=    <"attribute" "("> (AttribNameOrWildcard ("," TypeName)?)? ")" 
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

// [128]    AttribNameOrWildcard    ::=    AttributeName | "*" 
AttribNameOrWildcard:
	AttributeName
	| _STAR_
	{
		$$ = NULL;
	}
	;

// [129]    SchemaAttributeTest    ::=    <"schema-attribute" "("> AttributeDeclaration ")" 
SchemaAttributeTest:
	_SCHEMA_ATTRIBUTE_LPAR_ AttributeDeclaration _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_SCHEMA_ATTRIBUTE,$2));
		$$ = step;
	}
	;

// [130]    AttributeDeclaration    ::=    AttributeName 
AttributeDeclaration:
	AttributeName 
	;

// [131]    ElementTest    ::=    <"element" "("> (ElementNameOrWildcard ("," TypeName "?"?)?)? ")" 
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

// [132]    ElementNameOrWildcard    ::=    ElementName | "*" 
ElementNameOrWildcard:
	ElementName
	| _STAR_
	{
		$$ = NULL;
	}
	;

// [133]    SchemaElementTest    ::=    <"schema-element" "("> ElementDeclaration ")" 
SchemaElementTest:
	_SCHEMA_ELEMENT_LPAR_ ElementDeclaration _RPAR_
	{
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_SCHEMA_ELEMENT,$2));
		$$ = step;
	}
	;

// [134]    ElementDeclaration    ::=    ElementName 
ElementDeclaration:
	ElementName 
	;

// [135]    AttributeName    ::=    QName
AttributeName:
	QName
	;

// [136]    ElementName    ::=    QName
ElementName:
	QName
	;

// [137]    TypeName    ::=    QName
TypeName:
	QName
	;

// [138]   	IntegerLiteral	   ::=   	Digits
IntegerLiteral:
      _INTEGER_NUMBER_
		{
			$$ = new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER,
										$1, /*isNumeric*/true),
										MEMMGR);
		}
	;

// [139]   	DecimalLiteral	   ::=   	("." Digits) | (Digits "." [0-9]*)
DecimalLiteral:
      _DECIMAL_NUMBER_
		{
			$$ = new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL,
										$1, /*isNumeric*/true),
										MEMMGR);
		}
	;

// [140]   	DoubleLiteral	   ::=   	(("." Digits) | (Digits ("." [0-9]*)?)) [eE] [+-]? Digits
DoubleLiteral:
      _DOUBLE_NUMBER_
		{
			$$ = new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE,
										$1, /*isNumeric*/true),
										MEMMGR);
		}
	;

// [141]   	URILiteral	   ::=   	StringLiteral
URILiteral:
	_STRING_LITERAL_
	;

// [142]   	StringLiteral	   ::=   	('"' (PredefinedEntityRef | CharRef | ('"' '"') | [^"&])* '"') | 
//										("'" (PredefinedEntityRef | CharRef | ("'" "'") | [^'&])* "'")
StringLiteral:
      _STRING_LITERAL_
		{
			$$ = new (MEMMGR) DataItemLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										$1, /*isNumeric*/false),
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

}	// namespace XQuery


