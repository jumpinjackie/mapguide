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
 * $Id: FunctionLookupImpl.cpp,v 1.13.2.1 2007/01/12 18:04:46 gmfeinberg Exp $
 */

#include "../config/xqilla_config.h"
#include "FunctionLookupImpl.hpp"
#include <xqilla/functions/FuncFactory.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/framework/XPath2MemoryManagerImpl.hpp>

#define SECONDARY_KEY(func)(((func)->getMinArgs() << 16) | (func)->getMaxArgs())

FunctionLookupImpl *FunctionLookupImpl::g_globalFunctionTable = 0;
XPath2MemoryManagerImpl *FunctionLookupImpl::g_memMgr = 0;

FunctionLookupImpl::FunctionLookupImpl(XPath2MemoryManager* memMgr,
				       bool addGlobals) :
	_funcTable(197, false, memMgr)
{
	if (addGlobals)
		createTable(memMgr);
}

FunctionLookupImpl::~FunctionLookupImpl()
{
}

// static
void FunctionLookupImpl::initialize()
{
	/* global table is allocated via the memory manager, so
	   no need to delete it at this time
	if (g_globalFunctionTable)
	        delete g_globalFunctionTable;
	*/
	if (g_memMgr)
		delete g_memMgr;
	g_memMgr = new XPath2MemoryManagerImpl();
	g_globalFunctionTable = new (g_memMgr) FunctionLookupImpl(g_memMgr, true);
}

// static
void FunctionLookupImpl::terminate()
{
	if (g_memMgr) {
		delete g_memMgr;
		g_memMgr = 0;
		g_globalFunctionTable = 0;
	}
	/* no need to delete this -- memMgr cleans up
	if (g_globalFunctionTable) {
		delete g_globalFunctionTable;
		g_globalFunctionTable = 0;
	}
	*/
}

void FunctionLookupImpl::replaceFunction(FuncFactory *func)
{
    unsigned int secondaryKey = (unsigned int)SECONDARY_KEY(func);
    _funcTable.put((void*)func->getQName(), secondaryKey, func);
}

void FunctionLookupImpl::insertFunction(FuncFactory *func)
{
    unsigned int secondaryKey = (unsigned int)SECONDARY_KEY(func);

    // Use similar algorithm to lookup in order to detect overlaps
    // in argument numbers
    XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOfEnumerator<FuncFactory> iterator(const_cast<XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOf< FuncFactory >* >(&_funcTable));
    //
    // Walk the matches for the primary key (name) looking for overlaps:
    //   ensure func->max < min OR func->min > max
    //
    iterator.setPrimaryKey(func->getQName());
    while(iterator.hasMoreElements())
    {
        FuncFactory *entry= &(iterator.nextElement());
        if ((func->getMaxArgs() < entry->getMinArgs()) ||
	    (func->getMinArgs() > entry->getMaxArgs()))
		continue;
	// overlap -- throw exception
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf;
	buf.set(X("Multiple functions have the same QName and overlapping args {"));
	buf.append(func->getURI());
	buf.append(X("}"));
	buf.append(func->getName());
	buf.append(X("/"));
	XMLCh szInt[10];
	XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(secondaryKey,szInt,9,10);
	buf.append(szInt);
	buf.append(X(" [err:XQST0034]."));
	XQThrow2(StaticErrorException,X("FunctionLookupImpl::insertFunction"), buf.getRawBuffer());
    }
    // Ok to add function
    _funcTable.put((void*)func->getQName(), secondaryKey, func);
}

ASTNode* FunctionLookupImpl::lookUpFunction(const XMLCh* URI, const XMLCh* fname,
                                             const VectorOfASTNodes &args, XPath2MemoryManager* memMgr) const
{
    if (this != g_globalFunctionTable) {
	    ASTNode *ret = g_globalFunctionTable->lookUpFunction(
		    URI, fname, args, memMgr);
	    if (ret)
		    return ret;
    }

    XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOfEnumerator<FuncFactory> iterator(const_cast<XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOf< FuncFactory >* >(&_funcTable));
    //
    // Walk the matches for the primary key (name) looking for matches
    // based on allowable parameters
    //
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer key;
    key.set(fname);
    key.append(URI);
    iterator.setPrimaryKey(key.getRawBuffer());
    size_t nargs = args.size();
    while(iterator.hasMoreElements())
    {
        FuncFactory *entry= &(iterator.nextElement());
	if (entry->getMinArgs() <= nargs &&
	    entry->getMaxArgs() >= nargs)
	    return entry->createInstance(args, memMgr);
    }
    return NULL;
}

bool equalNsAndName1(const std::pair<const XMLCh*,const XMLCh*>& first, const std::pair<const XMLCh*,const XMLCh*>& second)
{
    return (XERCES_CPP_NAMESPACE::XMLString::equals(first.first, second.first) &&
           XERCES_CPP_NAMESPACE::XMLString::equals(first.second, second.second));
}

std::vector< std::pair<const XMLCh*,const XMLCh*> > FunctionLookupImpl::getFunctions() const
{
  std::vector< std::pair<const XMLCh*,const XMLCh*> > retVal;
  XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOfEnumerator<FuncFactory> iterator(const_cast<XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOf< FuncFactory >* >(&_funcTable));
  while(iterator.hasMoreElements())
  {
    FuncFactory& entry=iterator.nextElement();
    retVal.push_back(std::pair<const XMLCh*,const XMLCh*>(entry.getURI(), entry.getName()));
  }
  retVal.erase(std::unique(retVal.begin(), retVal.end(), equalNsAndName1), retVal.end());
  return retVal;
}

std::vector< FuncFactory* > FunctionLookupImpl::getFunctionFactories() const
{
  std::vector< FuncFactory* > retVal;
  XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOfEnumerator<FuncFactory> iterator(const_cast<XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOf< FuncFactory >* >(&_funcTable));
  while(iterator.hasMoreElements())
  {
    FuncFactory& entry=iterator.nextElement();
    retVal.push_back(&entry);
  }
  retVal.erase(std::unique(retVal.begin(), retVal.end()), retVal.end());
  return retVal;
}


//////////////////////////////////////////////////////////////////////////

#include "FuncFactoryTemplate.hpp"
#include <xqilla/functions/FunctionAbs.hpp>
#include <xqilla/functions/FunctionAvg.hpp>
#include <xqilla/functions/FunctionBaseURI.hpp>
#include <xqilla/functions/FunctionBoolean.hpp>
#include <xqilla/functions/FunctionCeiling.hpp>
#include <xqilla/functions/FunctionCollection.hpp>
#include <xqilla/functions/FunctionCompare.hpp>
#include <xqilla/functions/FunctionCodepointEqual.hpp>
#include <xqilla/functions/FunctionConcat.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/functions/FunctionContains.hpp>
#include <xqilla/functions/FunctionCount.hpp>
#include <xqilla/functions/FunctionCurrentDate.hpp>
#include <xqilla/functions/FunctionCurrentDateTime.hpp>
#include <xqilla/functions/FunctionCurrentTime.hpp>
#include <xqilla/functions/FunctionData.hpp>
#include <xqilla/functions/FunctionDateTime.hpp>
#include <xqilla/functions/FunctionDeepEqual.hpp>
#include <xqilla/functions/FunctionDefaultCollation.hpp>
#include <xqilla/functions/FunctionDistinctValues.hpp>
#include <xqilla/functions/FunctionDoc.hpp>
#include <xqilla/functions/FunctionDocAvailable.hpp>
#include <xqilla/functions/FunctionDocumentURI.hpp>
#include <xqilla/functions/FunctionEmpty.hpp>
#include <xqilla/functions/FunctionEncodeForUri.hpp>
#include <xqilla/functions/FunctionEndsWith.hpp>
#include <xqilla/functions/FunctionError.hpp>
#include <xqilla/functions/FunctionEscapeHtmlUri.hpp>
#include <xqilla/functions/FunctionExactlyOne.hpp>
#include <xqilla/functions/FunctionExists.hpp>
#include <xqilla/functions/FunctionQName.hpp>
#include <xqilla/functions/FunctionFalse.hpp>
#include <xqilla/functions/FunctionFloor.hpp>
#include <xqilla/functions/FunctionInScopePrefixes.hpp>
#include <xqilla/functions/FunctionPrefixFromQName.hpp>
#include <xqilla/functions/FunctionLocalNameFromQName.hpp>
#include <xqilla/functions/FunctionNamespaceURIFromQName.hpp>
#include <xqilla/functions/FunctionNamespaceURIForPrefix.hpp>
#include <xqilla/functions/FunctionId.hpp>
#include <xqilla/functions/FunctionIdref.hpp>
#include <xqilla/functions/FunctionImplicitTimezone.hpp>
#include <xqilla/functions/FunctionIndexOf.hpp>
#include <xqilla/functions/FunctionInsertBefore.hpp>
#include <xqilla/functions/FunctionIriToUri.hpp>
#include <xqilla/functions/FunctionLang.hpp>
#include <xqilla/functions/FunctionLast.hpp>
#include <xqilla/functions/FunctionLocalname.hpp>
#include <xqilla/functions/FunctionLowerCase.hpp>
#include <xqilla/functions/FunctionMatches.hpp>
#include <xqilla/functions/FunctionMax.hpp>
#include <xqilla/functions/FunctionMin.hpp>
#include <xqilla/functions/FunctionName.hpp>
#include <xqilla/functions/FunctionNamespaceUri.hpp>
#include <xqilla/functions/FunctionNilled.hpp>
#include <xqilla/functions/FunctionNodeName.hpp>
#include <xqilla/functions/FunctionNormalizeSpace.hpp>
#include <xqilla/functions/FunctionNormalizeUnicode.hpp>
#include <xqilla/functions/FunctionNot.hpp>
#include <xqilla/functions/FunctionNumber.hpp>
#include <xqilla/functions/FunctionOneOrMore.hpp>
#include <xqilla/functions/FunctionPosition.hpp>
#include <xqilla/functions/FunctionRemove.hpp>
#include <xqilla/functions/FunctionReplace.hpp>
#include <xqilla/functions/FunctionResolveQName.hpp>
#include <xqilla/functions/FunctionResolveURI.hpp>
#include <xqilla/functions/FunctionReverse.hpp>
#include <xqilla/functions/FunctionRoot.hpp>
#include <xqilla/functions/FunctionRound.hpp>
#include <xqilla/functions/FunctionRoundHalfToEven.hpp>
#include <xqilla/functions/FunctionStartsWith.hpp>
#include <xqilla/functions/FunctionStaticBaseURI.hpp>
#include <xqilla/functions/FunctionString.hpp>
#include <xqilla/functions/FunctionStringJoin.hpp>
#include <xqilla/functions/FunctionStringLength.hpp>
#include <xqilla/functions/FunctionStringToCodepoints.hpp>
#include <xqilla/functions/FunctionCodepointsToString.hpp>
#include <xqilla/functions/FunctionSubsequence.hpp>
#include <xqilla/functions/FunctionSubstring.hpp>
#include <xqilla/functions/FunctionSubstringAfter.hpp>
#include <xqilla/functions/FunctionSubstringBefore.hpp>
#include <xqilla/functions/FunctionSum.hpp>
#include <xqilla/functions/FunctionTokenize.hpp>
#include <xqilla/functions/FunctionTrace.hpp>
#include <xqilla/functions/FunctionTranslate.hpp>
#include <xqilla/functions/FunctionTrue.hpp>
#include <xqilla/functions/FunctionUnordered.hpp>
#include <xqilla/functions/FunctionUpperCase.hpp>
#include <xqilla/functions/FunctionZeroOrOne.hpp>
#include <xqilla/functions/FunctionYearsFromDuration.hpp>
#include <xqilla/functions/FunctionMonthsFromDuration.hpp>
#include <xqilla/functions/FunctionDaysFromDuration.hpp>
#include <xqilla/functions/FunctionHoursFromDuration.hpp>
#include <xqilla/functions/FunctionMinutesFromDuration.hpp>
#include <xqilla/functions/FunctionSecondsFromDuration.hpp>
#include <xqilla/functions/FunctionYearFromDateTime.hpp>
#include <xqilla/functions/FunctionMonthFromDateTime.hpp>
#include <xqilla/functions/FunctionDayFromDateTime.hpp>
#include <xqilla/functions/FunctionHoursFromDateTime.hpp>
#include <xqilla/functions/FunctionMinutesFromDateTime.hpp>
#include <xqilla/functions/FunctionSecondsFromDateTime.hpp>
#include <xqilla/functions/FunctionTimezoneFromDateTime.hpp>
#include <xqilla/functions/FunctionYearFromDate.hpp>
#include <xqilla/functions/FunctionMonthFromDate.hpp>
#include <xqilla/functions/FunctionDayFromDate.hpp>
#include <xqilla/functions/FunctionTimezoneFromDate.hpp>
#include <xqilla/functions/FunctionHoursFromTime.hpp>
#include <xqilla/functions/FunctionMinutesFromTime.hpp>
#include <xqilla/functions/FunctionSecondsFromTime.hpp>
#include <xqilla/functions/FunctionTimezoneFromTime.hpp>
#include <xqilla/functions/FunctionAdjustDateTimeToTimezone.hpp>
#include <xqilla/functions/FunctionAdjustDateToTimezone.hpp>
#include <xqilla/functions/FunctionAdjustTimeToTimezone.hpp>

void FunctionLookupImpl::createTable(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr)
{
  // From the XPath2 Function & Operators list

  // Accessors:
  //   fn:node-name
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNodeName>(memMgr));
  //   fn:nilled
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNilled>(memMgr));
  //   fn:string
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionString>(memMgr));
  //   fn:data
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionData>(memMgr));
  //   fn:base-uri
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionBaseURI>(memMgr));
  //   fn:document-uri
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDocumentURI>(memMgr));


  // Debug Functions:
  //   fn:error
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionError>(memMgr));
  //   fn:trace
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTrace>(memMgr));

  // Special Constructor Functions:
  //   fn:dateTime
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDateTime>(memMgr));

  // Functions on numeric values:
  //   fn:abs
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionAbs>(memMgr));
  //   fn:ceiling
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCeiling>(memMgr));
  //   fn:floor
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionFloor>(memMgr));
  //   fn:round
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionRound>(memMgr));
  //   fn:round-half-to-even
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionRoundHalfToEven>(memMgr));

  // Functions on strings
  //   fn:codepoints-to-string
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCodepointsToString>(memMgr));
  //   fn:string-to-codepoints
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionStringToCodepoints>(memMgr));
  //   fn:compare
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCompare>(memMgr));
  //   fn:codepoint-equal
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCodepointEqual>(memMgr));
  //   fn:concat
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionConcat>(memMgr));
  //   fn:string-join
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionStringJoin>(memMgr));
  //   fn:substring
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSubstring>(memMgr));
  //   fn:string-length
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionStringLength>(memMgr));
  //   fn:normalize-space
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNormalizeSpace>(memMgr));
  //   fn:normalize-unicode
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNormalizeUnicode>(memMgr));
  //   fn:upper-case
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionUpperCase>(memMgr));
  //   fn:lower-case
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionLowerCase>(memMgr));
  //   fn:translate
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTranslate>(memMgr));
  //   fn:encode-for-uri
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionEncodeForUri>(memMgr));
  //   fn:iri-to-uri
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionIriToUri>(memMgr));
  //   fn:escape-html-uri
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionEscapeHtmlUri>(memMgr));
  //   fn:contains
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionContains>(memMgr));
  //   fn:starts-with
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionStartsWith>(memMgr));
  //   fn:ends-with
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionEndsWith>(memMgr));
  //   fn:substring-before
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSubstringBefore>(memMgr));
  //   fn:substring-after
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSubstringAfter>(memMgr));

  //   fn:matches
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMatches>(memMgr));
  //   fn:replace
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionReplace>(memMgr));
  //   fn:tokenize
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTokenize>(memMgr));

  // Functions on boolean values
  //   fn:true
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTrue>(memMgr));
  //   fn:false
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionFalse>(memMgr));
  //   fn:not
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNot>(memMgr));

  // Functions on date values
  //   fn:years-from-duration
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionYearsFromDuration>(memMgr));
  //   fn:months-from-duration
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMonthsFromDuration>(memMgr));
  //   fn:days-from-duration
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDaysFromDuration>(memMgr));
  //   fn:hours-from-duration
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionHoursFromDuration>(memMgr));
  //   fn:minutes-from-duration
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMinutesFromDuration>(memMgr));
  //   fn:seconds-from-duration
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSecondsFromDuration>(memMgr));
  //   fn:year-from-dateTime
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionYearFromDateTime>(memMgr));
  //   fn:month-from-dateTime
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMonthFromDateTime>(memMgr));
  //   fn:day-from-dateTime
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDayFromDateTime>(memMgr));
  //   fn:hours-from-dateTime
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionHoursFromDateTime>(memMgr));
  //   fn:minutes-from-dateTime
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMinutesFromDateTime>(memMgr));
  //   fn:seconds-from-dateTime
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSecondsFromDateTime>(memMgr));
  //   fn:timezone-from-dateTime
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTimezoneFromDateTime>(memMgr));
  //   fn:year-from-date
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionYearFromDate>(memMgr));
  //   fn:month-from-date
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMonthFromDate>(memMgr));
  //   fn:day-from-date
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDayFromDate>(memMgr));
  //   fn:timezone-from-date
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTimezoneFromDate>(memMgr));
  //   fn:hours-from-time
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionHoursFromTime>(memMgr));
  //   fn:minutes-from-time
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMinutesFromTime>(memMgr));
  //   fn:seconds-from-time
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSecondsFromTime>(memMgr));
  //   fn:timezone-from-time
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTimezoneFromTime>(memMgr));
  //   fn:adjust-dateTime-to-timezone
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionAdjustDateTimeToTimezone>(memMgr));
  //   fn:adjust-date-to-timezone
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionAdjustDateToTimezone>(memMgr));
  //   fn:adjust-time-to-timezone
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionAdjustTimeToTimezone>(memMgr));

  // Functions on QName values
  //   fn:resolve-QName
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionResolveQName>(memMgr));
  //   fn:QName
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionQName>(memMgr));
  //   fn:prefix-from-QName
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionPrefixFromQName>(memMgr));
  //   fn:local-name-from-QName
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionLocalNameFromQName>(memMgr));
  //   fn:namespace-uri-from-QName
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNamespaceURIFromQName>(memMgr));
  //   fn:namespace-uri-for-prefix
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNamespaceURIForPrefix>(memMgr));
  //   fn:in-scope-prefixes
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionInScopePrefixes>(memMgr));

  // Functions on anyURI values
  //   fn:resolve-URI
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionResolveURI>(memMgr));

  // Functions on nodes
  //   fn:name
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionName>(memMgr));
  //   fn:local-name
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionLocalname>(memMgr));
  //   fn:namespace-uri
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNamespaceUri>(memMgr));
  //   fn:number
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNumber>(memMgr));
  //   fn:lang
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionLang>(memMgr));
  //   fn:root
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionRoot>(memMgr));

  // Functions on sequences
  //   fn::zero-or-one
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionZeroOrOne>(memMgr));
  //   fn::one-or-more
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionOneOrMore>(memMgr));
  //   fn::exactly-one
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionExactlyOne>(memMgr));
  //   fn:boolean
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionBoolean>(memMgr));
  //   fn:index-of
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionIndexOf>(memMgr));
  //   fn:empty
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionEmpty>(memMgr));
  //   fn:exists
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionExists>(memMgr));
  //   fn:distinct-values
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDistinctValues>(memMgr));
  //   fn:insert-before
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionInsertBefore>(memMgr));
  //   fn:remove
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionRemove>(memMgr));
  //   fn:reverse
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionReverse>(memMgr));
  //   fn:subsequence
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSubsequence>(memMgr));
  //   fn:unordered
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionUnordered>(memMgr));
  
  //   fn:deep-equal
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDeepEqual>(memMgr));

  //   fn:count
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCount>(memMgr));
  //   fn:avg
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionAvg>(memMgr));
  //   fn:max
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMax>(memMgr));
  //   fn:min
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMin>(memMgr));
  //   fn:sum
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSum>(memMgr));

  //   fn:id
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionId>(memMgr));
  //   fn:idref
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionIdref>(memMgr));
  //   fn:doc
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDoc>(memMgr));
  //   fn:doc-available
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDocAvailable>(memMgr));
  //   fn:collection
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCollection>(memMgr));
  
  // Context functions
  //   fn:position
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionPosition>(memMgr));
  //   fn:last
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionLast>(memMgr));
  //   fn:current-dateTime
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCurrentDateTime>(memMgr));
  //   fn:current-date
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCurrentDate>(memMgr));
  //   fn:current-time
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCurrentTime>(memMgr));
  //   fn:implicit-timezone
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionImplicitTimezone>(memMgr));
  //   fn:default-collation
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDefaultCollation>(memMgr));
  //   fn:static-base-uri
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionStaticBaseURI>(memMgr));

}
