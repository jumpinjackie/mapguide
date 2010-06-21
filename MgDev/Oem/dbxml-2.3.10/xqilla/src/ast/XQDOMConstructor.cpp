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
 * $Id: XQDOMConstructor.cpp,v 1.39 2006/11/03 17:04:20 jpcs Exp $
 */

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQDOMConstructor.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/ASTException.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/XMLChar.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XQDOMConstructor::XQDOMConstructor(XPath2MemoryManager* mm)
  : ASTNodeImpl(mm)
{
}

bool XQDOMConstructor::getStringValue(const VectorOfASTNodes* m_children, XMLBuffer &value, DynamicContext *context)
{
  bool bSomethingFound=false;
  for(VectorOfASTNodes::const_iterator itCont = m_children->begin();
      itCont != m_children->end (); ++itCont) {
    if(getStringValue(*itCont, value, context))
      bSomethingFound = true;
  }
  return bSomethingFound;
}

bool XQDOMConstructor::getStringValue(const ASTNode *child, XMLBuffer &value, DynamicContext *context)
{
  bool bSomethingFound=false;
  Result childList = child->collapseTree(context);
  Item::Ptr item;
  bool addSpace = false;
  while((item = childList->next(context)) != NULLRCP) {
    if(addSpace) value.append(' ');
    else addSpace = true;
    value.append(item->asString(context));
    bSomethingFound=true;
  }
  return bSomethingFound;
}

void XQDOMConstructor::unescapeEntities(XMLBuffer& buff) const
{
  const XMLCh* value=buff.getRawBuffer();
  unsigned int len=buff.getLen();
	int j=0;
	XMLCh* dst=(XMLCh*)value;
	for(unsigned int i=0;i<len;i++)
	{
		if(value[i]==chAmpersand) // entity reference
		{
      if(XMLString::compareNString(value+i+1,XMLUni::fgAmp,3)==0 && *(value+i+1+3)==chSemiColon) {
				dst[j++]=chAmpersand; i+=4;
      } else if(XMLString::compareNString(value+i+1,XMLUni::fgQuot,4)==0 && *(value+i+1+4)==chSemiColon) {
				dst[j++]=chDoubleQuote; i+=5;
			} else if(XMLString::compareNString(value+i+1,XMLUni::fgApos,4)==0 && *(value+i+1+4)==chSemiColon) {
				dst[j++]=chSingleQuote; i+=5;
			} else if(XMLString::compareNString(value+i+1,XMLUni::fgLT,2)==0 && *(value+i+1+2)==chSemiColon) {
				dst[j++]=chOpenAngle; i+=3;
      } else if(XMLString::compareNString(value+i+1,XMLUni::fgGT,2)==0 && *(value+i+1+2)==chSemiColon) {
				dst[j++]=chCloseAngle; i+=3;
			} else if(*(value+i+1)==chPound) {
        unsigned int number = 0;
        unsigned int radix = 10;
				i+=2;
        if(*(value+i)==chLatin_x)
        {
          i++;
          radix=16;
        }
				unsigned int k=i;
				while(k<len && value[k]!=chSemiColon) k++;
				if(k==len)
					XQThrow(ASTException,X("DOM Constructor"),X("Unterminated entity reference [err:XPST0003]."));
        for(unsigned int q=i;q<k;q++)
        {
          unsigned int nextVal;
          XMLCh nextCh=*(value+q);
          if ((nextCh >= chDigit_0) && (nextCh <= chDigit_9))
            nextVal = (unsigned int)(nextCh - chDigit_0);
          else if ((nextCh >= chLatin_A) && (nextCh <= chLatin_F))
            nextVal= (unsigned int)(10 + (nextCh - chLatin_A));
          else if ((nextCh >= chLatin_a) && (nextCh <= chLatin_f))
            nextVal = (unsigned int)(10 + (nextCh - chLatin_a));
          else
            XQThrow(ASTException,X("DOM Constructor"),X("Unterminated entity reference [err:XPST0003]."));
          if (nextVal >= radix)
            XQThrow(ASTException,X("DOM Constructor"),X("Invalid digit inside entity reference [err:XPST0003]."));
          else
            number = (number * radix) + nextVal;
        }
        if(!XMLChar1_0::isXMLChar(number))
          XQThrow(ASTException,X("DOM Constructor"),X("Entity reference is not a valid XML character [err:XPST0003]."));
        if (number <= 0xFFFD)
          dst[j++]=number;
        else if (number >= 0x10000 && number <= 0x10FFFF)
        {
          number -= 0x10000;
          dst[j++]= XMLCh((number >> 10) + 0xD800);
          dst[j++]= XMLCh((number & 0x3FF) + 0xDC00);
        }
				i=k;
			} else
        XQThrow(ASTException,X("DOM Constructor"),X("Invalid entity reference [err:XPST0003]."));
		}
    else
      dst[j++]=value[i];
	}
	dst[j++]=0;
}
