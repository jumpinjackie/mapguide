/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include "FunctionLookupImpl.hpp"
#include <pathan/internal/factory/FuncFactory.hpp>
#include <pathan/exceptions/StaticErrorException.hpp>

FunctionLookupImpl::FunctionLookupImpl(XPath2MemoryManager* memMgr) :
  _uriPool(17, memMgr),
  _funcTable(197, false, memMgr),
  _memMgr(memMgr)
{
    createTable();
}

FunctionLookupImpl::~FunctionLookupImpl()
{
}

void FunctionLookupImpl::insertFunction(FuncFactory *func)
{
    unsigned int nMax=func->getMaxArgs();
    unsigned int uriId=_uriPool.addOrFind(func->getURI());
    for(unsigned int i=func->getMinArgs(); i<=nMax; i++)
    {
        unsigned int secondaryKey=uriId | (i << 16);
        if(_funcTable.containsKey((void*)func->getName(), secondaryKey))
            DSLthrow(StaticErrorException,X("FunctionLookupImpl::insertFunction"), X("Multiple functions have the same expanded QName and the same number of arguments. [err:XQST0034]"));
        _funcTable.put((void*)func->getName(), secondaryKey, func);
    }
}

DataItem* FunctionLookupImpl::lookUpFunction(const XMLCh* URI, const XMLCh* fname,
                                             const VectorOfDataItems &args, XPath2MemoryManager* memMgr) const
{
    if(!_uriPool.exists(URI))
      return NULL;  
    unsigned int secondaryKey=_uriPool.getId(URI) | (args.size() << 16);
    const FuncFactory* pFactory=_funcTable.get((void*)fname, secondaryKey);
    if(pFactory)
        return pFactory->createInstance(args, memMgr);
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

#include "../factory/FuncFactoryTemplate.hpp"
#include <pathan/functions/FunctionAbs.hpp>
#include <pathan/functions/FunctionAvg.hpp>
#include <pathan/functions/FunctionBaseURI.hpp>
#include <pathan/functions/FunctionBoolean.hpp>
#include <pathan/functions/FunctionCeiling.hpp>
#include <pathan/functions/FunctionCollection.hpp>
#include <pathan/functions/FunctionCompare.hpp>
#include <pathan/functions/FunctionCodepointEqual.hpp>
#include <pathan/functions/FunctionConcat.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/functions/FunctionContains.hpp>
#include <pathan/functions/FunctionCount.hpp>
#include <pathan/functions/FunctionCurrentDate.hpp>
#include <pathan/functions/FunctionCurrentDateTime.hpp>
#include <pathan/functions/FunctionCurrentTime.hpp>
#include <pathan/functions/FunctionData.hpp>
#include <pathan/functions/FunctionDateTime.hpp>
#include <pathan/functions/FunctionDeepEqual.hpp>
#include <pathan/functions/FunctionDefaultCollation.hpp>
#include <pathan/functions/FunctionDistinctValues.hpp>
#include <pathan/functions/FunctionDoc.hpp>
#include <pathan/functions/FunctionDocAvailable.hpp>
#include <pathan/functions/FunctionDocumentURI.hpp>
#include <pathan/functions/FunctionEmpty.hpp>
#include <pathan/functions/FunctionEndsWith.hpp>
#include <pathan/functions/FunctionError.hpp>
#include <pathan/functions/FunctionEscapeURI.hpp>
#include <pathan/functions/FunctionExactlyOne.hpp>
#include <pathan/functions/FunctionExists.hpp>
#include <pathan/functions/FunctionQName.hpp>
#include <pathan/functions/FunctionFalse.hpp>
#include <pathan/functions/FunctionFloor.hpp>
#include <pathan/functions/FunctionInScopePrefixes.hpp>
#include <pathan/functions/FunctionPrefixFromQName.hpp>
#include <pathan/functions/FunctionLocalNameFromQName.hpp>
#include <pathan/functions/FunctionNamespaceURIFromQName.hpp>
#include <pathan/functions/FunctionNamespaceURIForPrefix.hpp>
#include <pathan/functions/FunctionId.hpp>
#include <pathan/functions/FunctionIdref.hpp>
#include <pathan/functions/FunctionInsertBefore.hpp>
#include <pathan/functions/FunctionImplicitTimezone.hpp>
#include <pathan/functions/FunctionIndexOf.hpp>
#include <pathan/functions/FunctionLang.hpp>
#include <pathan/functions/FunctionLast.hpp>
#include <pathan/functions/FunctionLocalname.hpp>
#include <pathan/functions/FunctionLowerCase.hpp>
#include <pathan/functions/FunctionMatches.hpp>
#include <pathan/functions/FunctionMax.hpp>
#include <pathan/functions/FunctionMin.hpp>
#include <pathan/functions/FunctionName.hpp>
#include <pathan/functions/FunctionNamespaceUri.hpp>
#include <pathan/functions/FunctionNilled.hpp>
#include <pathan/functions/FunctionNodeName.hpp>
#include <pathan/functions/FunctionNormalizeSpace.hpp>
#include <pathan/functions/FunctionNormalizeUnicode.hpp>
#include <pathan/functions/FunctionNot.hpp>
#include <pathan/functions/FunctionNumber.hpp>
#include <pathan/functions/FunctionOneOrMore.hpp>
#include <pathan/functions/FunctionPosition.hpp>
#include <pathan/functions/FunctionRemove.hpp>
#include <pathan/functions/FunctionReplace.hpp>
#include <pathan/functions/FunctionResolveQName.hpp>
#include <pathan/functions/FunctionResolveURI.hpp>
#include <pathan/functions/FunctionReverse.hpp>
#include <pathan/functions/FunctionRoot.hpp>
#include <pathan/functions/FunctionRound.hpp>
#include <pathan/functions/FunctionRoundHalfToEven.hpp>
#include <pathan/functions/FunctionStartsWith.hpp>
#include <pathan/functions/FunctionStaticBaseURI.hpp>
#include <pathan/functions/FunctionString.hpp>
#include <pathan/functions/FunctionStringJoin.hpp>
#include <pathan/functions/FunctionStringLength.hpp>
#include <pathan/functions/FunctionStringToCodepoints.hpp>
#include <pathan/functions/FunctionCodepointsToString.hpp>
#include <pathan/functions/FunctionSubsequence.hpp>
#include <pathan/functions/FunctionSubstring.hpp>
#include <pathan/functions/FunctionSubstringAfter.hpp>
#include <pathan/functions/FunctionSubstringBefore.hpp>
#include <pathan/functions/FunctionSum.hpp>
#include <pathan/functions/FunctionTokenize.hpp>
#include <pathan/functions/FunctionTrace.hpp>
#include <pathan/functions/FunctionTranslate.hpp>
#include <pathan/functions/FunctionTrue.hpp>
#include <pathan/functions/FunctionUnordered.hpp>
#include <pathan/functions/FunctionUpperCase.hpp>
#include <pathan/functions/FunctionZeroOrOne.hpp>
#include <pathan/functions/FunctionYearsFromDuration.hpp>
#include <pathan/functions/FunctionMonthsFromDuration.hpp>
#include <pathan/functions/FunctionDaysFromDuration.hpp>
#include <pathan/functions/FunctionHoursFromDuration.hpp>
#include <pathan/functions/FunctionMinutesFromDuration.hpp>
#include <pathan/functions/FunctionSecondsFromDuration.hpp>
#include <pathan/functions/FunctionYearFromDateTime.hpp>
#include <pathan/functions/FunctionMonthFromDateTime.hpp>
#include <pathan/functions/FunctionDayFromDateTime.hpp>
#include <pathan/functions/FunctionHoursFromDateTime.hpp>
#include <pathan/functions/FunctionMinutesFromDateTime.hpp>
#include <pathan/functions/FunctionSecondsFromDateTime.hpp>
#include <pathan/functions/FunctionTimezoneFromDateTime.hpp>
#include <pathan/functions/FunctionYearFromDate.hpp>
#include <pathan/functions/FunctionMonthFromDate.hpp>
#include <pathan/functions/FunctionDayFromDate.hpp>
#include <pathan/functions/FunctionTimezoneFromDate.hpp>
#include <pathan/functions/FunctionHoursFromTime.hpp>
#include <pathan/functions/FunctionMinutesFromTime.hpp>
#include <pathan/functions/FunctionSecondsFromTime.hpp>
#include <pathan/functions/FunctionTimezoneFromTime.hpp>
#include <pathan/functions/FunctionAdjustDateTimeToTimezone.hpp>
#include <pathan/functions/FunctionAdjustDateToTimezone.hpp>
#include <pathan/functions/FunctionAdjustTimeToTimezone.hpp>

void FunctionLookupImpl::createTable()
{
  // From the XPath2 Function & Operators list

  // Accessors:
  //   fn:node-name
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionNodeName>());
  //   fn:nilled
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionNilled>());
  //   fn:string
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionString>());
  //   fn:data
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionData>());
  //   fn:base-uri
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionBaseURI>());
  //   fn:document-uri
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionDocumentURI>());


  // Debug Functions:
  //   fn:error
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionError>());
  //   fn:trace
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionTrace>());

  // Special Constructor Functions:
  //   fn:dateTime
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionDateTime>());

  // Functions on numeric values:
  //   fn:abs
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionAbs>());
  //   fn:ceiling
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionCeiling>());
  //   fn:floor
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionFloor>());
  //   fn:round
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionRound>());
  //   fn:round-half-to-even
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionRoundHalfToEven>());

  // Functions on strings
  //   fn:codepoints-to-string
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionCodepointsToString>());
  //   fn:string-to-codepoints
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionStringToCodepoints>());
  //   fn:compare
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionCompare>());
  //   fn:codepoint-equal
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionCodepointEqual>());
  //   fn:concat
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionConcat>());
  //   fn:string-join
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionStringJoin>());
  //   fn:substring
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionSubstring>());
  //   fn:string-length
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionStringLength>());
  //   fn:normalize-space
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionNormalizeSpace>());
  //   fn:normalize-unicode
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionNormalizeUnicode>());
  //   fn:upper-case
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionUpperCase>());
  //   fn:lower-case
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionLowerCase>());
  //   fn:translate
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionTranslate>());
  //   fn:escape-URI
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionEscapeURI>());
  //   fn:contains
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionContains>());
  //   fn:starts-with
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionStartsWith>());
  //   fn:ends-with
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionEndsWith>());
  //   fn:substring-before
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionSubstringBefore>());
  //   fn:substring-after
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionSubstringAfter>());

  //   fn:matches
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionMatches>());
  //   fn:replace
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionReplace>());
  //   fn:tokenize
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionTokenize>());

  // Functions on boolean values
  //   fn:true
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionTrue>());
  //   fn:false
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionFalse>());
  //   fn:not
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionNot>());

  // Functions on date values
  //   fn:years-from-duration
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionYearsFromDuration>());
  //   fn:months-from-duration
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionMonthsFromDuration>());
  //   fn:days-from-duration
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionDaysFromDuration>());
  //   fn:hours-from-duration
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionHoursFromDuration>());
  //   fn:minutes-from-duration
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionMinutesFromDuration>());
  //   fn:seconds-from-duration
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionSecondsFromDuration>());
  //   fn:year-from-dateTime
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionYearFromDateTime>());
  //   fn:month-from-dateTime
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionMonthFromDateTime>());
  //   fn:day-from-dateTime
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionDayFromDateTime>());
  //   fn:hours-from-dateTime
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionHoursFromDateTime>());
  //   fn:minutes-from-dateTime
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionMinutesFromDateTime>());
  //   fn:seconds-from-dateTime
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionSecondsFromDateTime>());
  //   fn:timezone-from-dateTime
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionTimezoneFromDateTime>());
  //   fn:year-from-date
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionYearFromDate>());
  //   fn:month-from-date
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionMonthFromDate>());
  //   fn:day-from-date
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionDayFromDate>());
  //   fn:timezone-from-date
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionTimezoneFromDate>());
  //   fn:hours-from-time
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionHoursFromTime>());
  //   fn:minutes-from-time
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionMinutesFromTime>());
  //   fn:seconds-from-time
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionSecondsFromTime>());
  //   fn:timezone-from-time
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionTimezoneFromTime>());
  //   fn:adjust-dateTime-to-timezone
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionAdjustDateTimeToTimezone>());
  //   fn:adjust-date-to-timezone
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionAdjustDateToTimezone>());
  //   fn:adjust-time-to-timezone
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionAdjustTimeToTimezone>());

  // Functions on QName values
  //   fn:resolve-QName
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionResolveQName>());
  //   fn:QName
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionQName>());
  //   fn:prefix-from-QName
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionPrefixFromQName>());
  //   fn:local-name-from-QName
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionLocalNameFromQName>());
  //   fn:namespace-uri-from-QName
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionNamespaceURIFromQName>());
  //   fn:namespace-uri-for-prefix
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionNamespaceURIForPrefix>());
  //   fn:in-scope-prefixes
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionInScopePrefixes>());

  // Functions on anyURI values
  //   fn:resolve-URI
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionResolveURI>());

  // Functions on nodes
  //   fn:name
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionName>());
  //   fn:local-name
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionLocalname>());
  //   fn:namespace-uri
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionNamespaceUri>());
  //   fn:number
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionNumber>());
  //   fn:lang
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionLang>());
  //   fn:root
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionRoot>());

  // Functions on sequences
  //   fn::zero-or-one
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionZeroOrOne>());
  //   fn::one-or-more
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionOneOrMore>());
  //   fn::exactly-one
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionExactlyOne>());
  //   fn:boolean
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionBoolean>());
  //   fn:index-of
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionIndexOf>());
  //   fn:empty
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionEmpty>());
  //   fn:exists
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionExists>());
  //   fn:distinct-values
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionDistinctValues>());
  //   fn:insert-before
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionInsertBefore>());
  //   fn:remove
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionRemove>());
  //   fn:reverse
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionReverse>());
  //   fn:subsequence
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionSubsequence>());
  //   fn:unordered
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionUnordered>());
  
  //   fn:deep-equal
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionDeepEqual>());

  //   fn:count
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionCount>());
  //   fn:avg
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionAvg>());
  //   fn:max
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionMax>());
  //   fn:min
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionMin>());
  //   fn:sum
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionSum>());

  //   fn:id
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionId>());
  //   fn:idref
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionIdref>());
  //   fn:doc
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionDoc>());
  //   fn:doc-available
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionDocAvailable>());
  //   fn:collection
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionCollection>());
  
  // Context functions
  //   fn:position
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionPosition>());
  //   fn:last
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionLast>());
  //   fn:current-dateTime
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionCurrentDateTime>());
  //   fn:current-date
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionCurrentDate>());
  //   fn:current-time
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionCurrentTime>());
  //   fn:implicit-timezone
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionImplicitTimezone>());
  //   fn:default-collation
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionDefaultCollation>());
  //   fn:static-base-uri
  insertFunction(new (_memMgr) FuncFactoryTemplate<FunctionStaticBaseURI>());

}
