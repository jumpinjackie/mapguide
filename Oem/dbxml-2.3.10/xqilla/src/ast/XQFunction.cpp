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
 * $Id: XQFunction.cpp,v 1.14.2.4 2007/01/12 18:04:46 gmfeinberg Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/utils/XPath2Utils.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLStringTokenizer.hpp>

#include <string>
#include <map>

#include <assert.h>

class SequenceTypeTable : public std::map< std::string, std::vector< SequenceType* >* >
{
public:
	~SequenceTypeTable()
	{
		for(iterator it=begin();it!=end();it++)
		{
			std::vector< SequenceType* >* vec=it->second;
			for(std::vector< SequenceType* >::iterator it2=vec->begin();it2!=vec->end();it2++)
				delete (*it2);
			delete vec;
		}
	}
};

/* http://www.w3.org/2005/xpath-functions */
const XMLCh XQFunction::XMLChFunctionURI[] =
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,       XERCES_CPP_NAMESPACE_QUALIFIER chColon,         XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, 
    XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,       XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3,       XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g,       XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2,       XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,       XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_5,       XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h,       XERCES_CPP_NAMESPACE_QUALIFIER chDash,          XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,       XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const size_t XQFunction::UNLIMITED = 10000; // A reasonably large number

XQFunction::XQFunction(const XMLCh* name, size_t argsFrom, size_t argsTo, const char* paramDecl,
                       const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
  _fName(name), _fURI(XMLChFunctionURI), 
  _nArgsFrom(argsFrom), _nArgsTo(argsTo),
  _paramDecl(XQillaAllocator<SequenceType*>(memMgr)), _args(args)
{
  setType(ASTNode::FUNCTION);
  parseParamDecl(paramDecl, memMgr);

  if((argsFrom != UNLIMITED && argsFrom > args.size()) ||
     (argsTo != UNLIMITED && args.size() > argsTo)) {

    //cerr << "argsFrom: " << argsFrom << endl;
    //cerr << "argsTo: " << argsTo << endl;
    //cerr << "number of args: " << args.size() << endl;

    XQThrow(FunctionException,X("XQFunction::XQFunction"), X("Wrong number of arguments"));
  }
  const XMLCh* paramString=memMgr->getPooledString(paramDecl);
  if(argsFrom==argsTo)
	  _signature=paramString;
  else
  {
	  // place the optional arguments between [ and ]
	  static const XMLCh comma[]={ XERCES_CPP_NAMESPACE_QUALIFIER chComma, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
	  static const XMLCh openSquare[]={ XERCES_CPP_NAMESPACE_QUALIFIER chSpace, XERCES_CPP_NAMESPACE_QUALIFIER chOpenSquare, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
	  static const XMLCh closSquare[]={ XERCES_CPP_NAMESPACE_QUALIFIER chCloseSquare, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
	  XERCES_CPP_NAMESPACE_QUALIFIER XMLStringTokenizer tokenizer(paramString,comma);
	  unsigned int i;
	  unsigned int nTokens=tokenizer.countTokens();
	  _signature=XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;

	  for(i=0;i<argsFrom;i++)
	  {
		  if(i==0)
			  _signature=XPath2Utils::concatStrings(_signature,tokenizer.nextToken(),memMgr);
		  else
			  _signature=XPath2Utils::concatStrings(_signature,comma,tokenizer.nextToken(),memMgr);
	  }
	  if(i<nTokens)
	  {
		  _signature=XPath2Utils::concatStrings(_signature,openSquare,memMgr);
		  for(;i<nTokens;i++)
			  if(i==0)
				  _signature=XPath2Utils::concatStrings(_signature,tokenizer.nextToken(),memMgr);
			  else
				  _signature=XPath2Utils::concatStrings(_signature,comma,tokenizer.nextToken(),memMgr);
		  _signature=XPath2Utils::concatStrings(_signature,closSquare,memMgr);
	  }
	  if(argsTo==UNLIMITED)
	  {
		  static const XMLCh ellipsis[]={ XERCES_CPP_NAMESPACE_QUALIFIER chComma, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
		  _signature=XPath2Utils::concatStrings(_signature,ellipsis,memMgr);
	  }
  }
}


XQFunction::~XQFunction()
{
}

const XMLCh* XQFunction::getFunctionURI()const {
  return _fURI;
}

const XMLCh* XQFunction::getFunctionName() const {
  return _fName;
}

const XMLCh* XQFunction::getFunctionSignature() const
{
  return _signature;
}

ASTNode *XQFunction::resolveArguments(StaticContext *context, bool checkTimezone, bool numericFunction)
{
  size_t paramNumber = 0;
  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = _paramDecl[paramNumber]->convertFunctionArg(*i, context, numericFunction, this);
    *i = (*i)->staticResolution(context);

    ++paramNumber;
    if(paramNumber >= _paramDecl.size()) {
      paramNumber = _paramDecl.size() - 1;
    }
  }

  return this;
}

ASTNode *XQFunction::calculateSRCForArguments(StaticContext *context, bool checkTimezone, bool numericFunction)
{
  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = (*i)->staticTyping(context);
    _src.add((*i)->getStaticResolutionContext());

    if(checkTimezone && (*i)->isDateOrTimeAndHasNoTimezone(context))
      _src.implicitTimezoneUsed(true);
  }

  if(!_src.isUsed()) {
    return constantFold(context);
  }
  return this;
}

Result XQFunction::getParamNumber(size_t number, DynamicContext* context, int flags) const
{
	assert(number > 0);
	assert(number <= getNumArgs());
	return _args[number - 1]->collapseTree(context, flags);
}

size_t XQFunction::getNumArgs() const
{
  return _args.size();
}

void XQFunction::parseParamDecl(const char* paramString, XPath2MemoryManager *mm)
{
  // Tokenise param string
  XMLCh* string = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(paramString);
  static XMLCh delimiters[]={ XERCES_CPP_NAMESPACE_QUALIFIER chComma, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
  XERCES_CPP_NAMESPACE_QUALIFIER XMLStringTokenizer tokenizer(string, delimiters);
  while(tokenizer.hasMoreTokens())
  {
    XMLCh* toParse=tokenizer.nextToken();
    //create SequenceType from curParam and append to vector
                                                                                                                                                              
    SequenceType *sequenceType=NULL;
    XMLCh* tmpCurParam = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(toParse);
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::collapseWS(tmpCurParam);
                                                                                                                                                              
    // get the OccurrenceIndicator
    SequenceType::OccurrenceIndicator occurrence=SequenceType::EXACTLY_ONE;
    unsigned int len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(tmpCurParam);
    if(len>0)
      switch(tmpCurParam[len-1])
      {
        case XERCES_CPP_NAMESPACE_QUALIFIER chQuestion: occurrence=SequenceType::QUESTION_MARK; tmpCurParam[len-1]=0; break;
        case XERCES_CPP_NAMESPACE_QUALIFIER chAsterisk: occurrence=SequenceType::STAR; tmpCurParam[len-1]=0; break;
        case XERCES_CPP_NAMESPACE_QUALIFIER chPlus:     occurrence=SequenceType::PLUS; tmpCurParam[len-1]=0; break;
      }
                                                                                                                                                              
    // check if it's a node(), element(), item() or empty()
    static XMLCh szNode[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chOpenParen, XERCES_CPP_NAMESPACE_QUALIFIER chCloseParen, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
    static XMLCh szElement[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chOpenParen, XERCES_CPP_NAMESPACE_QUALIFIER chCloseParen, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
    static XMLCh szItem[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chOpenParen, XERCES_CPP_NAMESPACE_QUALIFIER chCloseParen, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
    static XMLCh szEmpty[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y, XERCES_CPP_NAMESPACE_QUALIFIER chOpenParen, XERCES_CPP_NAMESPACE_QUALIFIER chCloseParen, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
    if(XPath2Utils::equals(tmpCurParam, szNode))
      sequenceType=new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_NODE), occurrence);
    else if(XPath2Utils::equals(tmpCurParam, szElement))
      sequenceType=new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT), occurrence);
    else if(XPath2Utils::equals(tmpCurParam, szItem))
      sequenceType=new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), occurrence);
    else if(XPath2Utils::equals(tmpCurParam, szEmpty))
      sequenceType=new (mm) SequenceType();
    else
    {
      SequenceType::ItemType* test=new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE,NULL,new (mm) QualifiedName(tmpCurParam, mm));
      if (XPath2Utils::equals(tmpCurParam, ATDurationOrDerived::fgDT_YEARMONTHDURATION) ||
          XPath2Utils::equals(tmpCurParam, ATDurationOrDerived::fgDT_DAYTIMEDURATION) ||
          XPath2Utils::equals(tmpCurParam, AnyAtomicType::fgDT_ANYATOMICTYPE) ||
          XPath2Utils::equals(tmpCurParam, ATUntypedAtomic::fgDT_UNTYPEDATOMIC) )
        // If yearMonthDuration, dayTimeDuration, anyAtomicType or untypedAtomic, we set the URI to be FunctionURI, and use the specified type
        test->setTypeURI(FunctionConstructor::XMLChXPath2DatatypesURI);
      else
        // otherwise it's a atomic type coming from the XMLSchema namespace
        test->setTypeURI(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
      sequenceType = new (mm) SequenceType(test, occurrence);
    }
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&tmpCurParam);
    _paramDecl.push_back(sequenceType);
  }
  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&string);
}

const VectorOfASTNodes &XQFunction::getArguments() const {
  return _args;
}

