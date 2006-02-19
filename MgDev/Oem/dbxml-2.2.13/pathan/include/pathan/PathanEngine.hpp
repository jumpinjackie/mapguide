/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __PATHANENGINE_HPP
#define __PATHANENGINE_HPP

#include <pathan/Pathan.hpp>

#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMXPathException.hpp>
#include <xercesc/dom/DOMXPathEvaluator.hpp>
#include <xercesc/util/XMemory.hpp>
#include <xercesc/util/PlatformUtils.hpp>

class XPath2MemoryManager;
class XPath2Result;
class DynamicContext;
class PathanExpression;
class PathanNSResolver;
class DataItem;

XERCES_CPP_NAMESPACE_BEGIN 
class DOMDocumentType;
class DOMXPathEvaluator;
class DOMNode;
class DOMDocument;
class XMLGrammarPool;
XERCES_CPP_NAMESPACE_END 


/**
 * The <code>PathanEngine</code> class provides functions to create expressions and evaluate with
 * a given context, and shortcuts to creating your own context.
 */
class PATHAN_EXPORT PathanEngine : public XERCES_CPP_NAMESPACE_QUALIFIER XMemory
{
private:
    // -----------------------------------------------------------------------
    //  Constructor
    // -----------------------------------------------------------------------
    /** @name Constructor */
    //@{    
    PathanEngine();
    //@}

public:

  /** @name XPath 2 Factory Methods */
  //@{
  
  /**
   * Creates a parsed XPath2 expression with resolved namespaces. This is useful
   * when an expression will be reused in an application since it makes it 
   * possible to compile the expression string into a more efficient internal 
   * form and preresolve all namespace prefixes which occur within the expression.
   * @param expression of type XMLCh - The XPath expression string to be parsed.
   * code NAMESPACE_ERR.
   * @param context of type <code>DynamicContext</code> - The static context to use during parsing.
   * The context must not change or be deleted during the lifetime of the DataItem. If you want
   * to alter the context to evaluate the expression, obtain a new context using the
   * StaticContext::createDynamicContext() method.
   * @param memMgr of type <code>MemoryManager</code> - The memory manager used to create the
   * <code>PathanExpression</code>. Defaults to the xerces default memory manager.
   * @return <code>PathanExpression</code> The compiled form of the XPath2 expression.
   * @exception XPathException
   * INVALID_EXPRESSION_ERR: Raised if the expression is not legal according to the 
   * rules of the <code>DOMXPathEvaluator</code>.
   * @exception DOMException
   * NAMESPACE_ERR: Raised if the expression contains namespace prefixes which cannot
   * be resolved by the specified <code>XPathNSResolver</code>.
   */
	static PathanExpression *createExpression(const XMLCh* expression, DynamicContext* context,
                                            XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager)
    throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);

  /**
   * Creates a context suitable for parsing an expression with.
   * @param memMgr of type <code>MemoryManager</code> - The memory manager used to create the
   * <code>DynamicContext</code>.
   * @return An implementation of <code>DynamicContext</code>
   */
  static DynamicContext *createContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager);

  /**
   * Creates a context suitable for parsing an expression with.
   * @param xmlGP of type <code>XMLGrammarPool</code> - The grammar to set. If the parameter is null, no grammar will
   * be set in the context.
   * @param memMgr of type <code>MemoryManager</code> - The memory manager used to create the
   * <code>DynamicContext</code>.
   * @return An implementation of <code>DynamicContext</code>
   */
  static DynamicContext *createContext(XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *xmlGP,
                                       XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager);

  /**
   * Creates a context suitable for parsing an expression with.
   * @param document of type <code>DOMDocument</code> - the document, that must be from the Pathan
   * implementation of xerces, to get the grammar from. If the parameter is null, no grammar wil
   * be set in the context.
   * @param memMgr of type <code>MemoryManager</code> - The memory manager used to create the
   * <code>DynamicContext</code>.
   * @return An implementation of <code>DynamicContext</code>
   */
  static DynamicContext *createContext(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *document,
                                       XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager);

  /**
   * Creates a parsed XPath2 expression, using the given context. This method returns a DataItem,
   * which is the root of the executable parse tree for the expression, and is more flexible than
   * the PathanExpression returned above.
   * @param expression of type XMLCh - The XPath expression string to be parsed.
   * @param context of type <code>DynamicContext</code> - The static context to use during parsing.
   * The context must not change or be deleted during the lifetime of the DataItem. If you want
   * to alter the context to evaluate the expression, obtain a new context using the
   * StaticContext::createDynamicContext() method.
   * @param memMgr of type <code>XPath2MemoryManager</code> - The memory manager used to create the
   * <code>DataItem</code> tree.
   * @param staticallyResolve of type bool - If set to true, the DataItem returned will have been
   * statically resolved using the DataItem::staticResolution() method.
   * @return <code>DataItem</code> The compiled form of the XPath2 expression.
   * @exception XPathException
   * INVALID_EXPRESSION_ERR: Raised if the expression is not legal according to the 
   * rules of the <code>DOMXPathEvaluator</code>.
   * @exception DOMException
   * NAMESPACE_ERR: Raised if the expression contains namespace prefixes which cannot
   * be resolved by the specified <code>XPathNSResolver</code>.
   */
  static DataItem* createDataItem(const XMLCh* expression, DynamicContext* context,
                                  XPath2MemoryManager *memMgr, bool staticallyResolve = true)
    throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);

  /**
   * Create a PathanNSResolver
   * @param memMgr of type <code>XPath2MemoryManager</code> - The memory manager used to create the
   * <code>PathanNSResolver</code>.
   * @return An implementation of <code>PathanNSResolver</code>
   */
  static PathanNSResolver *createNSResolver(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* nodeResolver,
                                            XPath2MemoryManager *memMgr);

  /**
   * Create a new XPath2MemoryManager object. This
   * object is allocated on the heap, and should be deleted
   * at the end of it's life. However, deleting an
   * XPath2MemoryManager deletes all objects allocated using
   * it.
   */
  static XPath2MemoryManager *createMemoryManager();

  //@}
};


#endif //__PATHANENGINE_HPP

