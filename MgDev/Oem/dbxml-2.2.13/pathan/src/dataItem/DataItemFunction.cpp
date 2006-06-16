/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/dataItem/DataItemFunction.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/ATUntypedAtomic.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/XPath2Utils.hpp>

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

static SequenceTypeTable g_SequenceTypeCache;

/* http://www.w3.org/2005/04/xpath-functions */
const XMLCh DataItemFunction::XMLChFunctionURI[] =
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,
    XERCES_CPP_NAMESPACE_QUALIFIER chColon, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_5, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_4, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chDash,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const unsigned int DataItemFunction::UNLIMITED = 51; // A reasonably large number

DataItemFunction::DataItemFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr),
  _fName(name), _fURI(XMLChFunctionURI), 
  _nArgsFrom(argsFrom), _nArgsTo(argsTo),
  _paramDecl(parseParamDecl(paramDecl)), _args(args)
{
  setType(DataItem::FUNCTION);

  if((argsFrom != UNLIMITED && argsFrom > args.size()) ||
     (argsTo != UNLIMITED && args.size() > argsTo)) {

    //cerr << "argsFrom: " << argsFrom << endl;
    //cerr << "argsTo: " << argsTo << endl;
    //cerr << "number of args: " << args.size() << endl;

    DSLthrow(FunctionException,X("DataItemFunction::DataItemFunction"), X("Wrong number of arguments"));
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


DataItemFunction::~DataItemFunction()
{
}

const XMLCh* DataItemFunction::getFunctionURI()const {
  return _fURI;
}

const XMLCh* DataItemFunction::getFunctionName() const {
  return _fName;
}

const XMLCh* DataItemFunction::getFunctionSignature() const
{
  return _signature;
}

Result DataItemFunction::getParamNumber(unsigned int number, DynamicContext* context, int flags) const
{
	return getParamNumber(number,number,context,flags);
}

Result DataItemFunction::getParamNumber(unsigned int paramNumber, unsigned int signatureNumber, DynamicContext* context, int flags) const
{
	assert(paramNumber > 0);
	assert(paramNumber <= getNumArgs());
	assert(signatureNumber > 0);
	assert(signatureNumber <= _paramDecl->size());

	return _args[paramNumber - 1]->collapseTree(context, flags).convertFunctionArg((*_paramDecl)[signatureNumber - 1],
    _args[paramNumber - 1]->getStaticResolutionContext().getStaticType(),context);
}

ATStringOrDerived::Ptr DataItemFunction::getStringParam(unsigned int number, DynamicContext* context, int flags) const
{
  Result paramSeq(getParamNumber(number, context, flags));

  const Item::Ptr first = paramSeq.next(context);
  const Item::Ptr second = paramSeq.next(context);

  if(second != NULLRCP) {
    DSLthrow(FunctionException,X("DataItemFunction::getStringParam"), X("getStringParam() called on a parameter which is a sequence of more than one item"));
  }

  return (const ATStringOrDerived::Ptr)first;
}

unsigned int DataItemFunction::getNumArgs() const
{
  return _args.size();
}

std::vector< SequenceType* >* DataItemFunction::parseParamDecl(const char* paramString)
{
  // check the cache, first
  SequenceTypeTable::iterator it=g_SequenceTypeCache.find(paramString);
  if(it!=g_SequenceTypeCache.end())
    return it->second;
                                                                                                                                                              
  // Note: we are going to use the global allocator, as we are going to store this data into a static variable
  std::vector< SequenceType* >* params=new std::vector< SequenceType* >;
                                                                                                                                                              
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
      sequenceType=new SequenceType(new SequenceType::ItemType(SequenceType::ItemType::TEST_NODE), occurrence);
    else if(XPath2Utils::equals(tmpCurParam, szElement))
      sequenceType=new SequenceType(new SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT), occurrence);
    else if(XPath2Utils::equals(tmpCurParam, szItem))
      sequenceType=new SequenceType(new SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), occurrence);
    else if(XPath2Utils::equals(tmpCurParam, szEmpty))
      sequenceType=new SequenceType();
    else
    {
      SequenceType::ItemType* test=new SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE,NULL,new QualifiedName(tmpCurParam));
      if (XPath2Utils::equals(tmpCurParam, ATDurationOrDerived::fgDT_YEARMONTHDURATION) ||
          XPath2Utils::equals(tmpCurParam, ATDurationOrDerived::fgDT_DAYTIMEDURATION) ||
          XPath2Utils::equals(tmpCurParam, AnyAtomicType::fgDT_ANYATOMICTYPE) ||
          XPath2Utils::equals(tmpCurParam, ATUntypedAtomic::fgDT_UNTYPEDATOMIC) )
        // If yearMonthDuration, dayTimeDuration, anyAtomicType or untypedAtomic, we set the URI to be FunctionURI, and use the specified type
        test->setTypeURI(FunctionConstructor::XMLChXPath2DatatypesURI);
      else
        // otherwise it's a atomic type coming from the XMLSchema namespace
        test->setTypeURI(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
      sequenceType = new SequenceType(test, occurrence);
    }
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&tmpCurParam);
    params->push_back(sequenceType);
  }
  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&string);
  g_SequenceTypeCache[paramString]=params;
                                                                                                                                                              
  return params;
}

const VectorOfDataItems &DataItemFunction::getArguments() const {
  return _args;
}

