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
 * $Id: CodepointCollation.cpp,v 1.9 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../../config/xqilla_config.h"
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CodepointCollation::CodepointCollation()
{
}

static XMLCh codepointName[]= /* http://www.w3.org/2005/xpath-functions/collation/codepoint */
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
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,       XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,      
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
    XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,       XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

/*static*/ const XMLCh* CodepointCollation::getCodepointCollationName()
{
	return codepointName;
}

const XMLCh* CodepointCollation::getCollationName() const
{
  return getCodepointCollationName();
}

int CodepointCollation::compare(const XMLCh* string1, const XMLCh* string2) const
{
  if(string1 == string2) return 0;

  if(string1 == 0) return *string2 == 0 ? 0 : -1; // string2 == 0 is handled by the first line
  if(string2 == 0) return *string1 == 0 ? 0 : 1;  // string1 == 0 is handled by the first line

  int diff;
  while((diff = int(*string1) - int(*string2)) == 0) {
    // If either has ended, then they both ended, so equal
    if(*string1 == 0) break;

    // Move upwards for the next round
    ++string1;
    ++string2;
  }
  return diff < 0 ? -1 : (diff > 0 ? 1 : 0);
}
