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
 * $Id: XQQuery.hpp,v 1.16.2.1 2007/01/03 19:19:51 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQQuery.h: interface for the XQQuery class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQUERY_H__FAA9933A_2F10_49A4_93A2_857E2A8C48E3__INCLUDED_)
#define AFXQ_XQUERY_H__FAA9933A_2F10_49A4_93A2_857E2A8C48E3__INCLUDED_

#include <xqilla/framework/XQillaExport.hpp>
#include <vector>
#include <string>
#include <xercesc/util/XMemory.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xqilla/runtime/ResultImpl.hpp>
#include <xqilla/runtime/LazySequenceResult.hpp>
#include <xqilla/context/StaticContext.hpp>
#include <xqilla/simple-api/XQilla.hpp>

class DynamicContext;
class XQUserFunction;
class XQGlobalVariable;
class XQQuery;

typedef std::vector<XQUserFunction*, XQillaAllocator<XQUserFunction*> > UserFunctions;
typedef std::vector<XQGlobalVariable*, XQillaAllocator<XQGlobalVariable*> > GlobalVariables;
typedef std::vector<XQQuery*, XQillaAllocator<XQQuery*> > ImportedModules;

/**
 * Encapsulates a query expression. XQQuery objects are thread safe, and can be
 * executed from multiple threads at the same time - provided a DynamicContext is
 * used per thread.
 *
 * Instances of this class can be created by using the parsing methods on the XQilla
 * object.
 *
 * This class is dependant on the DynamicContext that was used to create it. This
 * context must not be deleted before the XQQuery object. Normally this is acheived
 * by the XQQuery adopting the DynamicContext and deleting it when it is deleted.
 * However, this behaviour can be disabled by using the XQilla::NO_ADOPT_CONTEXT
 * flag when creating the XQQuery.
 */
class XQILLA_API XQQuery : public XERCES_CPP_NAMESPACE_QUALIFIER XMemory
{
public:
  /// Destructor.
  ~XQQuery();

	/** @name Runtime Methods */
	// @{

  /**
   * Creates a DynamicContext based on the static context used to parse this query.
   * This DynamicContext will be suitable for using to call execute().
   *
   * @param memMgr The memory manager used to create the DynamicContext.
   *
   * @return An implementation of DynamicContext
   */
  DynamicContext *createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr =
                                       XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager) const;

  /**
   * Executes the query using the given DynamicContext.
   *
   * @param context The DynamicContext to use to execute the
   * query. This contains runtime values for variables, the value
   * of the context item, and other relevent information.
   *
   * @return The Result object - a lazy iterator over the query
   * results.
   *
   * @exception XQException If a runtime error occurs
   */
  Result execute(DynamicContext* context) const;

  /**
   * Perform static resolution on the query. This operation is not
   * thread safe, and should only be performed once.
   *
   * Static reolution resolves prefixes to namespace URIs, variable
   * names to variables, function names to functions, and performs
   * some basic query optimisation.
   *
   * Normally this is done as part of parsing the query expression.
   * However, if the XQilla::NO_STATIC_RESOLUTION flag was specified
   * when the query was parsed, this method will have to be called to
   * perform this step manually.
   *
   * @param context If specified, the StaticContext to use to statically
   * resolve this object. By default, the context used to parse the query
   * is used. This should not need to be changed under normal usage.
   *
   * @exception XQException If a static resolution time error occurs
   */
  void staticResolution(StaticContext *context = 0);

  /**
   * Perform type calculation and related optimizations. This operation
   * is safe to call multiple times.
   *
   * Normally this step is done as part of static resolution.
   *
   * @param context If specified, the StaticContext to use to statically
   * resolve this object. By default, the context used to parse the query
   * is used. This should not need to be changed under normal usage.
   *
   * @exception XQException If a static resolution time error occurs
   */
  void staticTyping(StaticContext *context = 0);

	//@}

	/** @name Query Information */
	// @{

  /// Returns the expression that was parsed to create this XQQuery object.
  const XMLCh* getQueryText() const;

  /// Returns the query plan as XML. This is designed for debug and informative purposes only.
  std::string getQueryPlan() const;

	//@}

	/** @name Getters and Setters */
	// @{

  /// Get the query body as an ASTNode
  ASTNode* getQueryBody() const;
  /// Set the query body to an ASTNode
  void setQueryBody(ASTNode* query);

  /// Adds an XQUserFunction to the query
  void addFunction(XQUserFunction* fnDef);
  /// Returns a vector of all XQUserFunction objects from the query
  const UserFunctions &getFunctions() const { return m_userDefFns; }

  /// Adds a XQGlobalVariable to the query
  void addVariable(XQGlobalVariable* varDef);
  /// Returns a vector of all XQGlobalVariable objects from the query
  const GlobalVariables &getVariables() const { return m_userDefVars; }

  /// Returns a vector of all XQGlobalVariable objects from the query
  const ImportedModules &getImportedModules() const { return m_importedModules; }

  /// Returns the name of the file that this query was parsed from.
  const XMLCh* getFile() const;
  /// Sets the name of the file that this query was parsed from.
  void setFile(const XMLCh* file);

  const DynamicContext *getStaticContext() const { return m_context; }

	//@}

	/** @name Module Methods */
	// @{

  /// Sets whether this query is a module or not
  void setIsLibraryModule(bool bIsModule=true);
  /// Returns whether this query is a module or not
  bool getIsLibraryModule() const;
  /// Sets the module target namespace for this query
  void setModuleTargetNamespace(const XMLCh* uri);
  /// Gets the module taget namespace for this query
  const XMLCh* getModuleTargetNamespace() const;
  /// Performs a module import from the given target namespace and locations
  void importModule(XQilla::Language language, const XMLCh* szUri,
                    VectorOfStrings* locations, StaticContext* context);

	//@}

private:
  /// Top level lazy result iterator
  class QueryResult : public ResultImpl
  {
  public:
    QueryResult(const XQQuery *query);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const XQQuery *_query;
    Result _parent;
    bool _toDo;
  };

  /// Top level debug lazy result iterator
  class DebugResult : public LazySequenceResult
  {
  public:
    DebugResult(const XQQuery *query, DynamicContext *context);

    void getResult(Sequence &toFill, DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const XQQuery *_query;
  };

private:
  /// Private constructor - Can be accessed by an XQilla object, as it is a friend.
  XQQuery(const XMLCh* queryText, DynamicContext *context, bool contextOwned = false,
          XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr =
          XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager);

  XQQuery(const XQQuery &);
  XQQuery &operator=(const XQQuery &);

private:
  /// The memory manager used to create this object
  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* m_memMgr;

  /// The context used to create this query
  DynamicContext *m_context;
  bool m_contextOwned;

  ASTNode* m_query;

  bool m_bIsLibraryModule;
  const XMLCh* m_szTargetNamespace;
  const XMLCh* m_szQueryText;
  const XMLCh* m_szCurrentFile;

  UserFunctions m_userDefFns;
  GlobalVariables m_userDefVars;
  ImportedModules m_importedModules;

  friend class QueryResult;
  friend class DebugResult;
  friend class XQilla;
};

#endif // !defined(AFXQ_XQUERY_H__FAA9933A_2F10_49A4_93A2_857E2A8C48E3__INCLUDED_)
