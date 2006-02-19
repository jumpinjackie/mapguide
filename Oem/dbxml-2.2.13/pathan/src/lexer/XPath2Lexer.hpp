/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Interface to XPathLexer that gives a yylex function as
  required by Bison.
*/

#ifndef _XPATHLEXER_HPP
#define _XPATHLEXER_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFor.hpp>
#include <pathan/dataItem/DataItemStep.hpp>
#include <pathan/SequenceType.hpp>

#ifndef YY_CHAR
#define YY_CHAR unsigned short
#endif

class DataItem;
class NodeTest;
class QualifiedName;

typedef union {

  DataItem* dataItemImpl;
  XMLCh *wString;
  NodeTest *nodeTest;
  DataItemStep::Axis axis;
  QualifiedName *qName;
  VectorOfDataItems *dataItemStore;
  SequenceType* sequenceType;
  SequenceType::OccurrenceIndicator occurrence;
  SequenceType::ItemType* itemType;
  VectorOfBindings* variableBinding;
} yystype;

#define YYSTYPE yystype


/** Adapter for XPathLexer that gives a yylex function as required
    by Bison. Basically Bison expects an integer as the return type,
    with 0 meaning end of input. If the returned token type has an
    associated chunk of data, we should store it in lvalp. The
    pcontrol pointer is the only way to pass control data to the yylex
    function, and is cast back to a DSLPathParserControl structure,
    then the Lexer instance included in that is used to actually grab
    tokens. **/
int yylex(YYSTYPE *lvalp, void *pcontrol);

#endif    









