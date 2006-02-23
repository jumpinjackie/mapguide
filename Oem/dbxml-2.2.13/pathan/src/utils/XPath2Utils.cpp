/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/framework/psvi/PSVIItem.hpp>
#include <xercesc/dom/DOMPSVITypeInfo.hpp>
#include <xercesc/dom/impl/DOMTypeInfoImpl.hpp>
#include <xercesc/dom/impl/DOMAttrImpl.hpp>
#include <xercesc/dom/impl/DOMElementNSImpl.hpp>

#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include "../exceptions/MiscException.hpp"
#include <pathan/Sequence.hpp>
#include <pathan/Node.hpp>

#include <pathan/internal/m_apm.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#define snprintf _snprintf
#endif

const XMLCh* XPath2Utils::escapeURI(const XMLCh* const str, bool escapeRes, XPath2MemoryManager* memMgr)
{

  if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(str) == 0) {
    return 0;
  }
	XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf(1023, memMgr);

  const XMLCh *cptr; 

  for (cptr = str; *cptr; cptr++) {
    // here we compare 16-bit XMLCh's to zero-padded 8-bit chars, so it 
    // magically works 
    if ((*cptr >= 'A' && *cptr <= 'Z') ||
        (*cptr >= 'a' && *cptr <= 'z') ||
        (*cptr >= '0' && *cptr <= '9') ||
        *cptr == '%' || *cptr == '#' || 
        *cptr == '-' || *cptr == '_' || *cptr == '.' || 
        *cptr == '!' || *cptr == '~' || *cptr == '*' ||
        *cptr == '\''|| *cptr == '(' || *cptr == ')' ||
        (!escapeRes && 
         (*cptr == ';' || *cptr == '/' || *cptr == '?' ||
          *cptr == ':' || *cptr == '@' || *cptr == '&' ||
          *cptr == '=' || *cptr == '+' || *cptr == '$' ||
          *cptr == ',' || *cptr == '[' || *cptr == ']' ))) {
      
      buf.append(*cptr);
      
    } else {
      
      buf.append(X("%"));
      // if the upper 8 bits of the XMLCh are less than 10 (covers all of 8-bit ascii)
      if ((*cptr >> 4) < 10)
        buf.append((XMLCh)('0' + (*cptr >> 4)));
      else // covers extended ascii
        buf.append((XMLCh)('A' - 10 + (*cptr >> 4)));

      // bitwise-and the 16-bit XMLCh with octal F, 11111111
      // we are checking the lower 8-bits
      if ((*cptr & 0xF) < 10)
        buf.append((XMLCh)('0' + (*cptr & 0xF)));
      else
        buf.append((XMLCh)('A' - 10 + (*cptr & 0xF)));
    }
      
  }//for  

  return memMgr->getPooledString(buf.getRawBuffer());
}

const XMLCh* XPath2Utils::concatStrings(const XMLCh* src1, const XMLCh src, XPath2MemoryManager* memMgr) {
  XMLCh dummy[2] = {src, XERCES_CPP_NAMESPACE_QUALIFIER chNull};
  return XPath2Utils::concatStrings(src1, dummy, memMgr);
}

const XMLCh* XPath2Utils::concatStrings(const XMLCh* src1, const XMLCh* src2, XPath2MemoryManager* memMgr) {

  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, memMgr);
  buffer.set(src1);
  buffer.append(src2);

  return memMgr->getPooledString(buffer.getRawBuffer());
}

const XMLCh* XPath2Utils::concatStrings(const XMLCh* src1, const XMLCh* src2, const XMLCh* src3, XPath2MemoryManager* memMgr) {

  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, memMgr);
  buffer.set(src1);
  buffer.append(src2);
  buffer.append(src3);

  return memMgr->getPooledString(buffer.getRawBuffer());
}

const XMLCh* XPath2Utils::asStr(const XMLCh ch, XPath2MemoryManager* memMgr) {

  XMLCh newStr[2] = {ch, XERCES_CPP_NAMESPACE_QUALIFIER chNull};
  return memMgr->getPooledString(newStr);
}

const XMLCh* XPath2Utils::subString(const XMLCh* src, unsigned int offset, unsigned int count, XPath2MemoryManager* memMgr) {

  if (src == 0) {
    DSLthrow(MiscException,X("XPath2Utils:subString"),X("Cannot take substring of null string"));
  }

	XMLCh *newStr = new XMLCh [ count + 1 ];
  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::subString(newStr, src, offset, offset + count);
  const XMLCh* retval=memMgr->getPooledString(newStr);
  delete[] newStr;
  return retval;
}

const XMLCh* XPath2Utils::insertData(const XMLCh* target, const XMLCh *src, unsigned int offset, XPath2MemoryManager* memMgr) {

  unsigned int targetLength = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(target);
  unsigned int srcLength = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(src);
  unsigned int newSize = targetLength + srcLength;

  XMLCh *newString = new XMLCh [newSize + 1];

  if(targetLength) {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::copyNString(newString, target, offset);
  }

  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::copyNString(newString + offset, src, srcLength);

  newString[newSize] = XERCES_CPP_NAMESPACE_QUALIFIER chNull;
  const XMLCh* retval=memMgr->getPooledString(newString);
  delete newString;
  return retval;
}

const XMLCh* XPath2Utils::insertData( const XMLCh* target, const XMLCh src, unsigned int offset, XPath2MemoryManager* memMgr) {
  XMLCh newStr[2] = {src, XERCES_CPP_NAMESPACE_QUALIFIER chNull};
  return insertData(target, newStr, offset, memMgr);
  }

const XMLCh* XPath2Utils::deleteData( const XMLCh* const target, unsigned int offset, unsigned int count, XPath2MemoryManager* memMgr) {

  if (target == 0) {
    return 0;
  }

  unsigned int targetSize = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(target); 
  unsigned int newSize =  targetSize - count;
  XMLCh *newString = new XMLCh [newSize + 1];

  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::copyNString(newString, target, offset);// * sizeof(XMLCh));
  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::copyNString(newString + offset, target + count, ( targetSize - offset - count));//*sizeof(XMLCh));
  newString[newSize] = 0;
  const XMLCh* retval=memMgr->getPooledString(newString);
  delete newString;
  return retval;
}

const XMLCh* XPath2Utils::toLower( const XMLCh* const target, XPath2MemoryManager* memMgr) {
  XMLCh *newStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(target);
  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::lowerCase(newStr);
  const XMLCh* retval=memMgr->getPooledString(newStr);
  delete [] newStr;
  return retval;
}

const XMLCh* XPath2Utils::toUpper( const XMLCh* const target, XPath2MemoryManager* memMgr) {
  XMLCh *newStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(target);
  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::upperCase(newStr);
  const XMLCh* retval=memMgr->getPooledString(newStr);
  delete [] newStr;
  return retval;
}

const XMLCh* XPath2Utils::toCollapsedWS(const XMLCh* const target, XPath2MemoryManager* memMgr) {
  XMLCh *newStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(target);
  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::collapseWS(newStr);
  const XMLCh* retval=memMgr->getPooledString(newStr);
  delete [] newStr;
  return retval;
}


std::vector<const XMLCh*> XPath2Utils::getVal(const XMLCh* values, XPath2MemoryManager* memMgr){

  std::vector<const XMLCh*> valList;
  bool munchWS = true; // munch initial WS
  unsigned int start = 0;

  
  /* XPath requires this bizarre WS separated splitting of the string, as the
     string can hold many id's. */
  int valuesLen = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(values);
  

  
  for(int i = 0; i < valuesLen; i++) {
    switch(values[i]) {
    case 0x0020:
    case 0x0009:
    case 0x000d:
    case 0x000a: {
      if(!munchWS) {
	      munchWS = true;
	      // Found the end of a value, so add it to a vector.
	      valList.push_back(subString(values, start, i-start, memMgr));
      }
      break;
    }
    default: {
      if(munchWS) {
	      //found the start of an ID
	      start = i;
	      munchWS = false;
      }
    }
    
    }
  }
  if(!munchWS) {
    // Found the end of a value, so add it to the list of values.
    valList.push_back(subString(values, start, valuesLen-start, memMgr));
  }
  return valList;
}

bool XPath2Utils::containsString(std::vector<const XMLCh*> values, const XMLCh* val) { 
  for (std::vector<const XMLCh*>::const_iterator it=values.begin();it!=values.end();it++)
    if (XPath2Utils::equals(val, *it))
      return true;
  return false;
}

int XPath2Utils::indexNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *indexNode)
{

  int index = -1;   

  switch(indexNode->getNodeType()) {
    
  case XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE:
  case XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::DOCUMENT_NODE:
    index = 1;
    break;
    
  case XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE: {
    const XMLCh* name = XPath2NSUtils::getLocalName( indexNode );
    const XMLCh* nspace = indexNode->getNamespaceURI();
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *parent = indexNode->getParentNode();

    if(parent == 0) {
      return 1; // if the node nas no parent, it's index is one.
    }

    XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *child = parent->getFirstChild();
    assert(child != 0);
    index = 1;
    
    while(child != indexNode) {
      if(child->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE
	 && XPath2Utils::equals(name, XPath2NSUtils::getLocalName( child ) )
	 && XPath2Utils::equals(nspace, child->getNamespaceURI() ) ) {
	index++;
      }
      child = child->getNextSibling();
      assert(child != 0);
    }
    break;
  }
  
  case XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TEXT_NODE:
  case XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::COMMENT_NODE: {
    short type = indexNode->getNodeType();
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *parent = indexNode->getParentNode();
    if(parent == 0) {
      return 1; // if a node has no parent, it's index is 1
    }

    XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *child = parent->getFirstChild();
    assert(child != 0);
    index = 1;
    
    while(child != indexNode) {
      if(child->getNodeType() == type) {
	index++;
      }
      child = child->getNextSibling();
      assert(child != 0);
    }
    break;
  }
  default:
    assert("n" == "Unhandled node type in XPath2Utils::indexNode");
    DSLthrow(MiscException,X("XPath2Utils::indexNode"), X("Unhandled node type"));
  }
  

  return index;
}

/*static*/ void XPath2Utils::setElementType(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* pDoc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node, const XMLCh* uri, const XMLCh* name)
{
    XERCES_CPP_NAMESPACE_QUALIFIER DOMTypeInfoImpl* pInfo=new (pDoc) XERCES_CPP_NAMESPACE_QUALIFIER DOMTypeInfoImpl(uri,name);
    ((XERCES_CPP_NAMESPACE_QUALIFIER DOMElementNSImpl*)node)->setTypeInfo(pInfo);
}

/*static*/ void XPath2Utils::copyElementType(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* destNode, const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* srcNode)
{
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl* pDoc=(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl*)doc;
    try
    {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo* psviType=(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo*)const_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(srcNode)->getInterface(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXercescInterfacePSVITypeInfo);
        if(psviType && psviType->getNumericProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Validity)==XERCES_CPP_NAMESPACE_QUALIFIER PSVIItem::VALIDITY_VALID)
        {
            XERCES_CPP_NAMESPACE_QUALIFIER DOMTypeInfoImpl* pTypeInfo=new (pDoc) XERCES_CPP_NAMESPACE_QUALIFIER DOMTypeInfoImpl;
            pTypeInfo->setNumericProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Validity, XERCES_CPP_NAMESPACE_QUALIFIER PSVIItem::VALIDITY_VALID);
            pTypeInfo->setNumericProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Nil, psviType->getNumericProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Nil));
            pTypeInfo->setStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Type_Definition_Namespace, 
                pDoc->getPooledString(psviType->getStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Type_Definition_Namespace)));
            pTypeInfo->setStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Type_Definition_Name, 
                pDoc->getPooledString(psviType->getStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Type_Definition_Name)));
            pTypeInfo->setStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Member_Type_Definition_Namespace, 
                pDoc->getPooledString(psviType->getStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Member_Type_Definition_Namespace)));
            pTypeInfo->setStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Member_Type_Definition_Name, 
                pDoc->getPooledString(psviType->getStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Member_Type_Definition_Name)));
            ((XERCES_CPP_NAMESPACE_QUALIFIER DOMElementNSImpl*)destNode)->setTypeInfo(pTypeInfo);
        }
    } catch(XERCES_CPP_NAMESPACE_QUALIFIER DOMException&) {
    }
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap* pDestAttributes=destNode->getAttributes();
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap* pSrcAttributes=srcNode->getAttributes();
    for(XMLSize_t i=0;i<pSrcAttributes->getLength();i++)
    {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMAttr* pSrcAttr=(XERCES_CPP_NAMESPACE_QUALIFIER DOMAttr*)pSrcAttributes->item(i);
        XERCES_CPP_NAMESPACE_QUALIFIER DOMAttr* pDstAttr=(XERCES_CPP_NAMESPACE_QUALIFIER DOMAttr*)pDestAttributes->getNamedItemNS(pSrcAttr->getNamespaceURI(), pSrcAttr->getLocalName());
        if(pDstAttr!=NULL)
            copyAttributeType(pDoc, pDstAttr, pSrcAttr);
    }
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* pDestChild=destNode->getFirstChild();
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* pSrcChild=srcNode->getFirstChild();
    while(pDestChild!=NULL && pSrcChild!=NULL)
    {
        if(pDestChild->getNodeType()==XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE && 
           pSrcChild->getNodeType()==XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
        {
            copyElementType(pDoc, (XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*)pDestChild, (XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*)pSrcChild);
        }
        pDestChild=pDestChild->getNextSibling();
        pSrcChild=pSrcChild->getNextSibling();
    }
}

/*static*/ void XPath2Utils::copyAttributeType(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMAttr* destNode, const XERCES_CPP_NAMESPACE_QUALIFIER DOMAttr* srcNode)
{
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl* pDoc=(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl*)doc;
    try
    {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo* psviType=(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo*)const_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMAttr*>(srcNode)->getInterface(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXercescInterfacePSVITypeInfo);
        if(psviType && psviType->getNumericProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Validity)==XERCES_CPP_NAMESPACE_QUALIFIER PSVIItem::VALIDITY_VALID)
        {
            XERCES_CPP_NAMESPACE_QUALIFIER DOMTypeInfoImpl* pTypeInfo=new (pDoc) XERCES_CPP_NAMESPACE_QUALIFIER DOMTypeInfoImpl;
            pTypeInfo->setNumericProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Validity, XERCES_CPP_NAMESPACE_QUALIFIER PSVIItem::VALIDITY_VALID);
            pTypeInfo->setStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Type_Definition_Namespace, 
                pDoc->getPooledString(psviType->getStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Type_Definition_Namespace)));
            pTypeInfo->setStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Type_Definition_Name, 
                pDoc->getPooledString(psviType->getStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Type_Definition_Name)));
            pTypeInfo->setStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Member_Type_Definition_Namespace, 
                pDoc->getPooledString(psviType->getStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Member_Type_Definition_Namespace)));
            pTypeInfo->setStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Member_Type_Definition_Name, 
                pDoc->getPooledString(psviType->getStringProperty(XERCES_CPP_NAMESPACE_QUALIFIER DOMPSVITypeInfo::PSVI_Member_Type_Definition_Name)));
            ((XERCES_CPP_NAMESPACE_QUALIFIER DOMAttrImpl*)destNode)->setTypeInfo(pTypeInfo);
        }
    } catch(XERCES_CPP_NAMESPACE_QUALIFIER DOMException&) {
    }
}
