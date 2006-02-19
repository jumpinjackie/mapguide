/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include "../interface/XPath2EvaluatorImpl.hpp"

#include "../interface/XPath2ExpressionImpl.hpp"
#include "../dom-extensions/PathanNSResolverIml.hpp"

#include <dom3-xpath/XPath2Expression.hpp>

#include <pathan/VariableStore.hpp>
#include <simpleVariables/VarStoreImpl.hpp>

#include <parser/XPath2ParserControl.hpp>

#include <pathan/DynamicContext.hpp>

#include <pathan/exceptions/DataItemException.hpp>
#include <pathan/exceptions/NavigationException.hpp>
#include <exceptions/XPathParseException.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/ContextException.hpp>
#include <pathan/functionAPI/FunctionLookup.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <stdio.h>
#include <string>

#include <pathan/XPath2Utils.hpp>

#define yyFlexLexer xxFlexLexer
#include <lexer/XPathFlexLexer.hpp>
#undef yyFlexLexer

//Bison hooks
extern int XPathyyparse(void *);
extern int XPathyydebug;


//Remove leading and trailing whitespace of the given string
const XMLCh *stripWhitespace(const XMLCh* inputString)
{
	unsigned int start = 0, end = inputString.length () - 1;

	//Get the position of the first non-whitespace character
	while (isspace ((char) inputString[start])) {
		start++;
	}//while

	//Get the position of the last non-whitespace character
	while (isspace ((char) inputString[end])) {
		end--;
	}//while

	return XPath2Utils::subString(inputString, start, end - start + 1);
}//stripWhitespace



int main(int argc, char* argv[]) {
	bool parserDebug = false;
	int exprIndex = 1;

	for(int i = 1; i < argc; i++) {
		if(argv[i][0] == '-' && argv[i][1] == 'd') {
			parserDebug = true;
			exprIndex++;
		}
	}

	try {
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
		cerr << "problem init xerces";
	}


	//cerr << "1";

	//cerr << "2" << argv[1];	

	cerr << "about to lex and parse " << argv[exprIndex] << endl; 

	const XMLCh *expression(argv[exprIndex]);

	//cerr << "3" << argv[1];
	//Lex expression
	LexerDOMStringStream lexerStream(stripWhitespace(expression));
	xxFlexLexer lexer(&lexerStream, &cout);

	//cerr << "4";


	//Parse expression
	XPathParserControl pcontrol;
	pcontrol.lexer = &lexer;

	//cerr << "5";

	DynamicContext* context = new DynamicContextImpl() ;
	pcontrol.context = context;
	//cerr << "6";

	if(parserDebug) {
		XPathyydebug = 1;
	}
	else {
		XPathyydebug = 0;		
	}


	try {
	cerr << "8";
		XPathyyparse((void *) &pcontrol);

	cerr << "9";
	}
	catch(const ContextException &e) {
    delete context;
		throw DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NAMESPACE_ERR, e.getError().c_str());
	}

	catch(const XPathParseException &e)
	{
		cerr << "XPathParseException between 8 & 9 was " << e.getError().c_str() << endl;
		string errString;
		errString += "The expression '";
		errString += XPath2Utils::DSLtranscode(expression);
		errString += "' is an invalid XPath expression";
    delete context;
		throw XPathException(XPathException::INVALID_EXPRESSION_ERR, errString);
	}
	catch(const FunctionException &fe)
	{
		string errString;
		errString += fe.getError();
    delete context;
		throw XPathException(XPathException::INVALID_EXPRESSION_ERR, errString);
	}
	catch(const DSLException &e) {
		string errString;
		errString += "The expression '";
		errString += XPath2Utils::DSLtranscode(expression);
		errString += "' isn't recognised as a valid XPath expression";

		cerr << errString << endl << endl << endl ;
    delete context;
		throw XPathException(XPathException::INVALID_EXPRESSION_ERR, errString);
	}
	
  delete context;
}
