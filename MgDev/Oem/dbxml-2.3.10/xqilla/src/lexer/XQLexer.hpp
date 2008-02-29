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
 * $Id: XQLexer.hpp,v 1.5 2006/11/01 16:37:21 jpcs Exp $
 */

#ifndef XQLEXER_H
#define XQLEXER_H

// #define XQ_DEBUG_LEXER 1

#include "XQillaLexer.hpp"
#ifndef yyFlexLexer
#define yyFlexLexer xqFlexLexer
#include "FlexLexer.h"
#endif

#include <xercesc/util/BitSet.hpp>
#include <xercesc/util/RefHashTableOf.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/context/DynamicContext.hpp>

#ifndef YYSTYPE
#include <xqilla/ast/ASTNode.hpp>
#include <xqilla/ast/XQNav.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/parser/QName.hpp>
#include <xqilla/functions/XQUserFunction.hpp>
#include <xqilla/ast/XQFLWOR.hpp>
#include <xqilla/ast/XQVariableBinding.hpp>
#include <xqilla/ast/XQTypeswitch.hpp>
#include <xqilla/ast/XQPredicate.hpp>
#include <xqilla/fulltext/FTSelection.hpp>
#include <xqilla/fulltext/FTWords.hpp>
#include <xqilla/fulltext/FTOption.hpp>
#include <xqilla/fulltext/FTRange.hpp>

typedef union {
  XMLCh* str;
  ASTNode* astNode;
  XQUserFunction* functDecl;
  XQUserFunction::XQFunctionParameter* funcParam;
  XQUserFunction::VectorOfFunctionParameters* funcParams;
  NodeTest *nodeTest;
  XQStep::Axis axis;
  QualifiedName *qName;
  SequenceType* sequenceType;
  SequenceType::OccurrenceIndicator occurrence;
  SequenceType::ItemType* itemType;
  VectorOfASTNodes* itemList;
  VectorOfPredicates* predicates;
  XQVariableBinding* variableBinding;
  VectorOfVariableBinding* variableBindingList;
  XQTypeswitch::Clause* clause;
  XQTypeswitch::VectorOfClause* clauseList;
  XQSort::SortSpec::sortModifier sortModifier;
  XQSort::VectorOfSortSpec* sortSpecList;
  XQSort::SortSpec* sortSpec;
  XQSort* sort;
  VectorOfStrings* stringList;
  FTSelection *ftselection;
  FTWords::FTAnyallOption ftanyalloption;
  FTOption *ftoption;
  VectorOfFTOptions *ftoptionlist;
  FTRange ftrange;
  FTOption::FTUnit ftunit;
} yystype;
#define YYSTYPE yystype
#define YYSTYPE_IS_DECLARED 1
#define YYSTYPE_IS_TRIVIAL 1
#endif  // #ifndef YYSTYPE

#include "../parser/XQParser.hpp"  // to be included *after* defining YYSTYPE and YYLTYPE

class XQILLA_API XQLexer : public CXQillaLexer, public yyFlexLexer
{
public:
  XQLexer(XPath2MemoryManager* memMgr, const XMLCh *queryFile, const XMLCh* query, XQilla::Language lang)
    :CXQillaLexer(memMgr, queryFile, query, lang)
  {
  }

  // Implemented in XQLexer.cpp, output of XQLexer.l
  virtual int yylex();

  int yylex(YYSTYPE* pYYLVAL, YYLTYPE* pYYLOC)
  {
    int nextToken=yylex();
    memcpy(pYYLVAL,&yylval,sizeof(YYSTYPE));
    memcpy(pYYLOC,&m_yyloc,sizeof(YYLTYPE));
    if(nextToken==0 && !CXQillaLexer::isCommentClosed())
      LexerError("Unterminated comment");
    return nextToken;
  }

  int yylex(YYSTYPE* pYYLVAL)
  {
    int nextToken=yylex();
    memcpy(pYYLVAL,&yylval,sizeof(YYSTYPE));
    if(nextToken==0 && !CXQillaLexer::isCommentClosed())
      LexerError("Unterminated comment");
    return nextToken;
  }

protected:
  virtual int LexerInput( YY_CHAR* buf, int max_size )
  {
    return CXQillaLexer::Read(buf, max_size);
  }

  virtual void LexerOutput( const YY_CHAR* buf, int size )
  {
    // do nothing
  }

  virtual void LexerError( const char* msg )
  {
    CXQillaLexer::error(msg);
  }

  virtual void yy_pop_state()
  {
    if(yy_start_stack_ptr==0)
      {
        XMLCh szMsg[256];
        XERCES_CPP_NAMESPACE_QUALIFIER XMLString::copyString(szMsg, X("Unbalanced '"));
        XERCES_CPP_NAMESPACE_QUALIFIER XMLString::catString(szMsg, (XMLCh*)yytext);
        XERCES_CPP_NAMESPACE_QUALIFIER XMLString::catString(szMsg, X("' token"));
        XQSimpleThrow(szMsg, NULL, m_lineno, m_columnno);
      }
    yyFlexLexer::yy_pop_state();
  }

  virtual bool next_tokens(int state, int tok1, int tok2=-1, int tok3=-1)
  {
    XQLexer lookAhead(m_memMgr, m_szQueryFile, m_szQuery+m_index, m_language);
    lookAhead.setGenerateEOF(m_bGenerateEOF);
    lookAhead.setGenerateErrorException(false);
    lookAhead.yy_start=1 + 2 * state;

    int nextToken1 = lookAhead.yylex();
    if(tok1!=nextToken1)
      return false;
    int nextToken2 = -1;
    if(tok2!=-1)
      nextToken2 = lookAhead.yylex();
    if(tok2!=nextToken2)
      return false;
    int nextToken3 = -1;
    if(tok3!=-1)
      nextToken3 = lookAhead.yylex();

    return (tok3==nextToken3);
  }

  virtual int next_token(int state)
  {
    XQLexer lookAhead(m_memMgr, m_szQueryFile, m_szQuery+m_index, m_language);
    lookAhead.setGenerateEOF(m_bGenerateEOF);
    lookAhead.setGenerateErrorException(false);
    lookAhead.yy_start=1 + 2 * state;

    return lookAhead.yylex();
  }
    
  YYSTYPE yylval;
};

class XQParserArgs
{
public:
  XQParserArgs()
    : _lexer(0), _context(0), _query(0), _flags(32), _namespaceDecls(13) {}

  XQLexer* _lexer;
  DynamicContext* _context;
  XQQuery* _query;
  XERCES_CPP_NAMESPACE_QUALIFIER BitSet _flags;
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XMLCh> _namespaceDecls;
};

namespace XQParser {
  extern int yyparse(void *);
  extern int yydebug;
}


#endif

