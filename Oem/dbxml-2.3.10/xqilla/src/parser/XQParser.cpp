/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     MYEOF = 0,
     _QNAME_ = 258,
     _FUNCTION_CALL_ = 259,
     _TAG_NAME_ = 260,
     _ATTRIBUTE_NAME_ = 261,
     _STRING_LITERAL_ = 262,
     _VARIABLE_ = 263,
     _CHAR_ = 264,
     _CHAR_REF_ = 265,
     _PREDEFINED_ENTITY_REF_ = 266,
     _NCNAME_COLON_STAR_ = 267,
     _STAR_COLON_NCNAME_ = 268,
     _NCNAME_ = 269,
     _PI_TARGET_ = 270,
     _NAMED_ATTRIBUTE_CONSTR_ = 271,
     _NAMED_ELEMENT_CONSTR_ = 272,
     _INTEGER_NUMBER_ = 273,
     _DECIMAL_NUMBER_ = 274,
     _DOUBLE_NUMBER_ = 275,
     _XML_COMMENT_ = 276,
     _CDATA_SECTION_ = 277,
     _PROCESSING_INSTRUCTION_CONTENT_ = 278,
     _NAMED_PROCESSING_INSTRUCTION_CONSTR_ = 279,
     _PRAGMA_NAME_ = 280,
     _PRAGMA_CONTENT_ = 281,
     _XQUERY_COMMENT_ = 282,
     _XQUERY_WHITESPACE_ = 283,
     _XQUERY_ERROR_ = 284,
     _PRAGMA_OPEN_ = 285,
     _PRAGMA_CLOSE_ = 286,
     _ESCAPE_APOS_ = 287,
     _ESCAPE_QUOT_ = 288,
     _COMMA_ = 289,
     _RETURN_ = 290,
     _FOR_ = 291,
     _IN_ = 292,
     _DOLLAR_SIGN_ = 293,
     _AT_KEYWORD_ = 294,
     _LET_ = 295,
     _COLON_EQ_ = 296,
     _WHERE_ = 297,
     _BY_ = 298,
     _ORDER_ = 299,
     _STABLE_ = 300,
     _ASCENDING_ = 301,
     _DESCENDING_ = 302,
     _EMPTY_KEYWORD_ = 303,
     _GREATEST_ = 304,
     _LEAST_ = 305,
     _COLLATION_ = 306,
     _SOME_ = 307,
     _EVERY_ = 308,
     _SATISFIES_ = 309,
     _TYPESWITCH_ = 310,
     _CASE_ = 311,
     _AS_ = 312,
     _IF_ = 313,
     _THEN_ = 314,
     _ELSE_ = 315,
     _OR_ = 316,
     _AND_ = 317,
     _INSTANCE_ = 318,
     _OF_ = 319,
     _CASTABLE_ = 320,
     _TO_ = 321,
     _PLUS_ = 322,
     _MINUS_ = 323,
     _MULTIPLY_ = 324,
     _DIV_ = 325,
     _INTEGER_DIV_ = 326,
     _MOD_ = 327,
     _UNION_ = 328,
     _VERTICAL_BAR_ = 329,
     _INTERSECT_ = 330,
     _EXCEPT_ = 331,
     _SLASH_ = 332,
     _SLASHSLASH_ = 333,
     _LBRACK_ = 334,
     _RBRACK_ = 335,
     _VALIDATE_ = 336,
     _MODE_LAX_ = 337,
     _MODE_STRICT_ = 338,
     _CAST_ = 339,
     _TREAT_ = 340,
     _EQUALS_ = 341,
     _NOT_EQUALS_ = 342,
     _LT_ = 343,
     _LT_EQUALS_ = 344,
     _GT_ = 345,
     _GT_EQUALS_ = 346,
     _FORTRAN_EQ_ = 347,
     _FORTRAN_NE_ = 348,
     _FORTRAN_LT_ = 349,
     _FORTRAN_LE_ = 350,
     _FORTRAN_GT_ = 351,
     _FORTRAN_GE_ = 352,
     _IS_ = 353,
     _LT_LT_ = 354,
     _GT_GT_ = 355,
     _DOT_ = 356,
     _AT_ = 357,
     _DOT_DOT_ = 358,
     _AXIS_CHILD_ = 359,
     _AXIS_DESCENDANT_ = 360,
     _AXIS_ATTRIBUTE_ = 361,
     _AXIS_SELF_ = 362,
     _AXIS_DESCENDANT_OR_SELF_ = 363,
     _AXIS_PARENT_ = 364,
     _AXIS_NAMESPACE_ = 365,
     _STAR_ = 366,
     _PROCESSING_INSTRUCTION_LPAR_ = 367,
     _COMMENT_LPAR_ = 368,
     _TEXT_LPAR_ = 369,
     _NODE_LPAR_ = 370,
     _LPAR_ = 371,
     _RPAR_ = 372,
     _START_TAG_OPEN_ = 373,
     _EMPTY_TAG_CLOSE_ = 374,
     _VALUE_INDICATOR_ = 375,
     _END_TAG_OPEN_ = 376,
     _TAG_CLOSE_ = 377,
     _DOCUMENT_CONSTR_ = 378,
     _ELEMENT_CONSTR_ = 379,
     _ATTRIBUTE_CONSTR_ = 380,
     _TEXT_CONSTR_ = 381,
     _PROCESSING_INSTRUCTION_START_ = 382,
     _LCURLY_BRACE_ESCAPE_ = 383,
     _RCURLY_BRACE_ESCAPE_ = 384,
     _OPEN_QUOT_ = 385,
     _CLOSE_QUOT_ = 386,
     _OPEN_APOS_ = 387,
     _CLOSE_APOS_ = 388,
     _LBRACE_ = 389,
     _RBRACE_ = 390,
     _PRESERVE_ = 391,
     _STRIP_ = 392,
     _NAMESPACE_ = 393,
     _EXPR_AS_ = 394,
     _EMPTY_ = 395,
     _ITEM_ = 396,
     _NILLABLE_ = 397,
     _DOCUMENT_NODE_LPAR_ = 398,
     _ATTRIBUTE_LPAR_ = 399,
     _ELEMENT_LPAR_ = 400,
     _EXTERNAL_ = 401,
     _PROCESSING_INSTRUCTION_CONSTR_ = 402,
     _COMMENT_CONSTR_ = 403,
     _AXIS_ANCESTOR_OR_SELF_ = 404,
     _AXIS_ANCESTOR_ = 405,
     _AXIS_FOLLOWING_SIBLING_ = 406,
     _AXIS_FOLLOWING_ = 407,
     _AXIS_PRECEDING_SIBLING_ = 408,
     _AXIS_PRECEDING_ = 409,
     _SEMICOLON_ = 410,
     _SCHEMA_ELEMENT_LPAR_ = 411,
     _SCHEMA_ATTRIBUTE_LPAR_ = 412,
     _ORDERED_ = 413,
     _UNORDERED_ = 414,
     _ORDERING_UNORDERED_ = 415,
     _ORDERING_ORDERED_ = 416,
     _ZERO_OR_ONE_ = 417,
     _ONE_OR_MORE_ = 418,
     _ZERO_OR_MORE_ = 419,
     _ENCODING_ = 420,
     _NO_PRESERVE_ = 421,
     _INHERIT_ = 422,
     _NO_INHERIT_ = 423,
     _DECLARE_ = 424,
     _BOUNDARYSPACE_ = 425,
     _BASEURI_ = 426,
     _CONSTRUCTION_ = 427,
     _ORDERING_ = 428,
     _DEFAULT_ = 429,
     _COPY_NAMESPACES_ = 430,
     _VARIABLE_KEYWORD_ = 431,
     _OPTION_ = 432,
     _XQUERY_ = 433,
     _VERSION_ = 434,
     _IMPORT_ = 435,
     _SCHEMA_ = 436,
     _MODULE_ = 437,
     _ELEMENT_ = 438,
     _FUNCTION_ = 439,
     _SCORE_ = 440,
     _FTCONTAINS_ = 441,
     _WEIGHT_ = 442,
     _DECLARE_FT_OPTION_ = 443,
     _DOUBLE_VERTICAL_BAR_ = 444,
     _DOUBLE_AMPERSAND_ = 445,
     _NOT_IN_ = 446,
     _BANG_ = 447,
     _WINDOW_ = 448,
     _DISTANCE_ = 449,
     _OCCURS_ = 450,
     _TIMES_ = 451,
     _SAME_ = 452,
     _DIFFERENT_ = 453,
     _AT_START_ = 454,
     _AT_END_ = 455,
     _ENTIRE_CONTENT_ = 456,
     _LOWERCASE_ = 457,
     _UPPERCASE_ = 458,
     _CASE_INSENSITIVE_ = 459,
     _CASE_SENSITIVE_ = 460,
     _WITH_DIACRITICS_ = 461,
     _WITHOUT_DIACRITICS_ = 462,
     _DIACRITICS_SENSITIVE_ = 463,
     _DIACRITICS_INSENSITIVE_ = 464,
     _WITH_STEMMING_ = 465,
     _WITHOUT_STEMMING_ = 466,
     _WITH_THESAURUS_ = 467,
     _WITHOUT_THESAURUS_ = 468,
     _RELATIONSHIP_ = 469,
     _LEVELS_ = 470,
     _WITH_STOP_WORDS_ = 471,
     _WITHOUT_STOP_WORDS_ = 472,
     _WITH_DEFAULT_STOP_WORDS_ = 473,
     _LANGUAGE_ = 474,
     _WITH_WILDCARDS_ = 475,
     _WITHOUT_WILDCARDS_ = 476,
     _ANY_ = 477,
     _ANY_WORD_ = 478,
     _ALL_ = 479,
     _ALL_WORDS_ = 480,
     _PHRASE_ = 481,
     _EXACTLY_ = 482,
     _AT_LEAST_ = 483,
     _AT_MOST_ = 484,
     _FROM_ = 485,
     _WORDS_ = 486,
     _SENTENCES_ = 487,
     _PARAGRAPHS_ = 488,
     _SENTENCE_ = 489,
     _PARAGRAPH_ = 490,
     _WITHOUT_CONTENT_ = 491,
     _EOF_ = 492
   };
#endif
/* Tokens.  */
#define MYEOF 0
#define _QNAME_ 258
#define _FUNCTION_CALL_ 259
#define _TAG_NAME_ 260
#define _ATTRIBUTE_NAME_ 261
#define _STRING_LITERAL_ 262
#define _VARIABLE_ 263
#define _CHAR_ 264
#define _CHAR_REF_ 265
#define _PREDEFINED_ENTITY_REF_ 266
#define _NCNAME_COLON_STAR_ 267
#define _STAR_COLON_NCNAME_ 268
#define _NCNAME_ 269
#define _PI_TARGET_ 270
#define _NAMED_ATTRIBUTE_CONSTR_ 271
#define _NAMED_ELEMENT_CONSTR_ 272
#define _INTEGER_NUMBER_ 273
#define _DECIMAL_NUMBER_ 274
#define _DOUBLE_NUMBER_ 275
#define _XML_COMMENT_ 276
#define _CDATA_SECTION_ 277
#define _PROCESSING_INSTRUCTION_CONTENT_ 278
#define _NAMED_PROCESSING_INSTRUCTION_CONSTR_ 279
#define _PRAGMA_NAME_ 280
#define _PRAGMA_CONTENT_ 281
#define _XQUERY_COMMENT_ 282
#define _XQUERY_WHITESPACE_ 283
#define _XQUERY_ERROR_ 284
#define _PRAGMA_OPEN_ 285
#define _PRAGMA_CLOSE_ 286
#define _ESCAPE_APOS_ 287
#define _ESCAPE_QUOT_ 288
#define _COMMA_ 289
#define _RETURN_ 290
#define _FOR_ 291
#define _IN_ 292
#define _DOLLAR_SIGN_ 293
#define _AT_KEYWORD_ 294
#define _LET_ 295
#define _COLON_EQ_ 296
#define _WHERE_ 297
#define _BY_ 298
#define _ORDER_ 299
#define _STABLE_ 300
#define _ASCENDING_ 301
#define _DESCENDING_ 302
#define _EMPTY_KEYWORD_ 303
#define _GREATEST_ 304
#define _LEAST_ 305
#define _COLLATION_ 306
#define _SOME_ 307
#define _EVERY_ 308
#define _SATISFIES_ 309
#define _TYPESWITCH_ 310
#define _CASE_ 311
#define _AS_ 312
#define _IF_ 313
#define _THEN_ 314
#define _ELSE_ 315
#define _OR_ 316
#define _AND_ 317
#define _INSTANCE_ 318
#define _OF_ 319
#define _CASTABLE_ 320
#define _TO_ 321
#define _PLUS_ 322
#define _MINUS_ 323
#define _MULTIPLY_ 324
#define _DIV_ 325
#define _INTEGER_DIV_ 326
#define _MOD_ 327
#define _UNION_ 328
#define _VERTICAL_BAR_ 329
#define _INTERSECT_ 330
#define _EXCEPT_ 331
#define _SLASH_ 332
#define _SLASHSLASH_ 333
#define _LBRACK_ 334
#define _RBRACK_ 335
#define _VALIDATE_ 336
#define _MODE_LAX_ 337
#define _MODE_STRICT_ 338
#define _CAST_ 339
#define _TREAT_ 340
#define _EQUALS_ 341
#define _NOT_EQUALS_ 342
#define _LT_ 343
#define _LT_EQUALS_ 344
#define _GT_ 345
#define _GT_EQUALS_ 346
#define _FORTRAN_EQ_ 347
#define _FORTRAN_NE_ 348
#define _FORTRAN_LT_ 349
#define _FORTRAN_LE_ 350
#define _FORTRAN_GT_ 351
#define _FORTRAN_GE_ 352
#define _IS_ 353
#define _LT_LT_ 354
#define _GT_GT_ 355
#define _DOT_ 356
#define _AT_ 357
#define _DOT_DOT_ 358
#define _AXIS_CHILD_ 359
#define _AXIS_DESCENDANT_ 360
#define _AXIS_ATTRIBUTE_ 361
#define _AXIS_SELF_ 362
#define _AXIS_DESCENDANT_OR_SELF_ 363
#define _AXIS_PARENT_ 364
#define _AXIS_NAMESPACE_ 365
#define _STAR_ 366
#define _PROCESSING_INSTRUCTION_LPAR_ 367
#define _COMMENT_LPAR_ 368
#define _TEXT_LPAR_ 369
#define _NODE_LPAR_ 370
#define _LPAR_ 371
#define _RPAR_ 372
#define _START_TAG_OPEN_ 373
#define _EMPTY_TAG_CLOSE_ 374
#define _VALUE_INDICATOR_ 375
#define _END_TAG_OPEN_ 376
#define _TAG_CLOSE_ 377
#define _DOCUMENT_CONSTR_ 378
#define _ELEMENT_CONSTR_ 379
#define _ATTRIBUTE_CONSTR_ 380
#define _TEXT_CONSTR_ 381
#define _PROCESSING_INSTRUCTION_START_ 382
#define _LCURLY_BRACE_ESCAPE_ 383
#define _RCURLY_BRACE_ESCAPE_ 384
#define _OPEN_QUOT_ 385
#define _CLOSE_QUOT_ 386
#define _OPEN_APOS_ 387
#define _CLOSE_APOS_ 388
#define _LBRACE_ 389
#define _RBRACE_ 390
#define _PRESERVE_ 391
#define _STRIP_ 392
#define _NAMESPACE_ 393
#define _EXPR_AS_ 394
#define _EMPTY_ 395
#define _ITEM_ 396
#define _NILLABLE_ 397
#define _DOCUMENT_NODE_LPAR_ 398
#define _ATTRIBUTE_LPAR_ 399
#define _ELEMENT_LPAR_ 400
#define _EXTERNAL_ 401
#define _PROCESSING_INSTRUCTION_CONSTR_ 402
#define _COMMENT_CONSTR_ 403
#define _AXIS_ANCESTOR_OR_SELF_ 404
#define _AXIS_ANCESTOR_ 405
#define _AXIS_FOLLOWING_SIBLING_ 406
#define _AXIS_FOLLOWING_ 407
#define _AXIS_PRECEDING_SIBLING_ 408
#define _AXIS_PRECEDING_ 409
#define _SEMICOLON_ 410
#define _SCHEMA_ELEMENT_LPAR_ 411
#define _SCHEMA_ATTRIBUTE_LPAR_ 412
#define _ORDERED_ 413
#define _UNORDERED_ 414
#define _ORDERING_UNORDERED_ 415
#define _ORDERING_ORDERED_ 416
#define _ZERO_OR_ONE_ 417
#define _ONE_OR_MORE_ 418
#define _ZERO_OR_MORE_ 419
#define _ENCODING_ 420
#define _NO_PRESERVE_ 421
#define _INHERIT_ 422
#define _NO_INHERIT_ 423
#define _DECLARE_ 424
#define _BOUNDARYSPACE_ 425
#define _BASEURI_ 426
#define _CONSTRUCTION_ 427
#define _ORDERING_ 428
#define _DEFAULT_ 429
#define _COPY_NAMESPACES_ 430
#define _VARIABLE_KEYWORD_ 431
#define _OPTION_ 432
#define _XQUERY_ 433
#define _VERSION_ 434
#define _IMPORT_ 435
#define _SCHEMA_ 436
#define _MODULE_ 437
#define _ELEMENT_ 438
#define _FUNCTION_ 439
#define _SCORE_ 440
#define _FTCONTAINS_ 441
#define _WEIGHT_ 442
#define _DECLARE_FT_OPTION_ 443
#define _DOUBLE_VERTICAL_BAR_ 444
#define _DOUBLE_AMPERSAND_ 445
#define _NOT_IN_ 446
#define _BANG_ 447
#define _WINDOW_ 448
#define _DISTANCE_ 449
#define _OCCURS_ 450
#define _TIMES_ 451
#define _SAME_ 452
#define _DIFFERENT_ 453
#define _AT_START_ 454
#define _AT_END_ 455
#define _ENTIRE_CONTENT_ 456
#define _LOWERCASE_ 457
#define _UPPERCASE_ 458
#define _CASE_INSENSITIVE_ 459
#define _CASE_SENSITIVE_ 460
#define _WITH_DIACRITICS_ 461
#define _WITHOUT_DIACRITICS_ 462
#define _DIACRITICS_SENSITIVE_ 463
#define _DIACRITICS_INSENSITIVE_ 464
#define _WITH_STEMMING_ 465
#define _WITHOUT_STEMMING_ 466
#define _WITH_THESAURUS_ 467
#define _WITHOUT_THESAURUS_ 468
#define _RELATIONSHIP_ 469
#define _LEVELS_ 470
#define _WITH_STOP_WORDS_ 471
#define _WITHOUT_STOP_WORDS_ 472
#define _WITH_DEFAULT_STOP_WORDS_ 473
#define _LANGUAGE_ 474
#define _WITH_WILDCARDS_ 475
#define _WITHOUT_WILDCARDS_ 476
#define _ANY_ 477
#define _ANY_WORD_ 478
#define _ALL_ 479
#define _ALL_WORDS_ 480
#define _PHRASE_ 481
#define _EXACTLY_ 482
#define _AT_LEAST_ 483
#define _AT_MOST_ 484
#define _FROM_ 485
#define _WORDS_ 486
#define _SENTENCES_ 487
#define _PARAGRAPHS_ 488
#define _SENTENCE_ 489
#define _PARAGRAPH_ 490
#define _WITHOUT_CONTENT_ 491
#define _EOF_ 492




/* Copy the first part of user declarations.  */
#line 14 "../src/parser/XQParser.y"


#if defined(WIN32) && !defined(__CYGWIN__)
// turn off the warnings "switch statement contains 'default' but no 'case' labels"
//                       "'yyerrlab1' : unreferenced label"
#pragma warning(disable : 4065 4102)
#endif

#include <iostream>
#include <xqilla/utils/UTF8Str.hpp>

#include "../lexer/XQLexer.hpp"

#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/ast/XQDocumentConstructor.hpp>
#include <xqilla/ast/XQElementConstructor.hpp>
#include <xqilla/ast/XQAttributeConstructor.hpp>
#include <xqilla/ast/XQPIConstructor.hpp>
#include <xqilla/ast/XQCommentConstructor.hpp>
#include <xqilla/ast/XQTextConstructor.hpp>
#include <xqilla/ast/XQFLWOR.hpp>
#include <xqilla/ast/XQQuantified.hpp>
#include <xqilla/ast/XQTypeswitch.hpp>
#include <xqilla/ast/XQValidate.hpp>
#include <xqilla/ast/XQGlobalVariable.hpp>
#include <xqilla/ast/XQDebugHook.hpp>
#include <xqilla/ast/XQFunctionCall.hpp>
#include <xqilla/ast/XQOrderingChange.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#include <xqilla/items/AnyAtomicTypeConstructor.hpp>

#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/ast/XQParenthesizedExpr.hpp>
#include <xqilla/ast/XQNav.hpp>
#include <xqilla/ast/XQStep.hpp>
#include <xqilla/ast/XQLiteral.hpp>
#include <xqilla/ast/XQVariable.hpp>
#include <xqilla/ast/XQInstanceOf.hpp>
#include <xqilla/ast/XQCastAs.hpp>
#include <xqilla/ast/XQCastableAs.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/XQIf.hpp>
#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/ast/XQPredicate.hpp>

#include <xqilla/fulltext/FTContains.hpp>
#include <xqilla/fulltext/FTOr.hpp>
#include <xqilla/fulltext/FTAnd.hpp>
#include <xqilla/fulltext/FTMildnot.hpp>
#include <xqilla/fulltext/FTUnaryNot.hpp>
#include <xqilla/fulltext/FTOrder.hpp>
#include <xqilla/fulltext/FTDistance.hpp>
#include <xqilla/fulltext/FTScope.hpp>
#include <xqilla/fulltext/FTContent.hpp>
#include <xqilla/fulltext/FTWindow.hpp>

#include <xqilla/parser/QName.hpp>

#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/operators/Plus.hpp>
#include <xqilla/operators/Minus.hpp>
#include <xqilla/operators/Multiply.hpp>
#include <xqilla/operators/Divide.hpp>
#include <xqilla/operators/IntegerDivide.hpp>
#include <xqilla/operators/Mod.hpp>
#include <xqilla/operators/UnaryMinus.hpp>
#include <xqilla/operators/NodeComparison.hpp>
#include <xqilla/operators/OrderComparison.hpp>
#include <xqilla/operators/Intersect.hpp>
#include <xqilla/operators/Except.hpp>
#include <xqilla/operators/Union.hpp>
#include <xqilla/operators/Or.hpp>
#include <xqilla/operators/And.hpp>
#include <xqilla/operators/Range.hpp>

#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/functions/FunctionRoot.hpp>

#include <xqilla/axis/NodeTest.hpp>

#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/NamespaceLookupException.hpp>
#include <xqilla/exceptions/ContextException.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include "../config/xqilla_config.h"

#define YYPARSE_PARAM qp
#define YYDEBUG 1
#define YYERROR_VERBOSE

// this removes a memory leak occurring when an error is found in the query (we throw an exception from inside
// yyerror, preventing the bison-generated code from cleaning up the memory); by defining this macro we use 
// stack-based memory instead of the heap
#define YYSTACK_USE_ALLOCA	1
#if HAVE_ALLOCA_H
#include <alloca.h>
#elif defined __GNUC__
#undef alloca
#define alloca __builtin_alloca
#elif defined _AIX
#define alloca __alloca
#elif defined _MSC_VER
#include <malloc.h>
#else
#include <stddef.h>
#ifdef __cplusplus
extern "C"
#endif
void *alloca (size_t);
#endif

#define QP						((XQParserArgs*)qp)
#define CONTEXT					(QP->_context)
#define LANGUAGE					(QP->_lexer->getLanguage())
#define MEMMGR					(CONTEXT->getMemoryManager())

#define REJECT_NOT_XQUERY(where,pos) if(!QP->_lexer->isXQuery()) { yyerror(LANGUAGE, #where, (pos).first_line, (pos).first_column); }
#define REJECT_NOT_XPATH(where,pos) if(!QP->_lexer->isXPath()) { yyerror(LANGUAGE, #where, (pos).first_line, (pos).first_column); }
#define REJECT_NOT_FULLTEXT(where,pos) if(!QP->_lexer->isFullText()) { yyerror(LANGUAGE, #where, (pos).first_line, (pos).first_column); }

#define WRAP(pos,object)		(wrapForDebug((QP), (object), NULL, (pos).first_line, (pos).first_column))
#define FNWRAP(pos,name,object)	(wrapForDebug((QP),(object),(name),(pos).first_line, (pos).first_column))

#define LOCATION(pos,name) LocationInfo name(QP->_query->getFile(), (pos).first_line, (pos).first_column)

#define BIT_ORDERING_SPECIFIED	0
#define BIT_BOUNDARY_SPECIFIED	                1
#define BIT_COLLATION_SPECIFIED	                2
#define BIT_BASEURI_SPECIFIED	                3
#define BIT_CONSTRUCTION_SPECIFIED	            4
#define BIT_EMPTYORDERING_SPECIFIED             5
#define BIT_COPYNAMESPACE_SPECIFIED             6
#define BIT_DEFAULTELEMENTNAMESPACE_SPECIFIED   7
#define BIT_DEFAULTFUNCTIONNAMESPACE_SPECIFIED  8
#define BIT_DECLARE_SECOND_STEP                 9

#undef yylex
#define yylex QP->_lexer->yylex
#undef yyerror
#define yyerror QP->_lexer->error

static XMLCh szQuote[]=		 { XERCES_CPP_NAMESPACE_QUALIFIER chDoubleQuote, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szApos[]=		 { XERCES_CPP_NAMESPACE_QUALIFIER chSingleQuote, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szCurlyOpen[]=  { XERCES_CPP_NAMESPACE_QUALIFIER chOpenCurly, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szCurlyClose[]= { XERCES_CPP_NAMESPACE_QUALIFIER chCloseCurly, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szFLWOR[] =     { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_L, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_W, 
                               XERCES_CPP_NAMESPACE_QUALIFIER chLatin_O, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_R, XERCES_CPP_NAMESPACE_QUALIFIER chNull }; 
static XMLCh sz1_0[]=        { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, 
                               XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szTrue[]=       { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
static XMLCh szFalse[]=      { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static inline bool isAllSpaces(const XMLCh* str)
{
	while(*str)
	{
		if(*str!='\r' && *str != '\n' && *str != ' ' && *str != '\t')
			return false;
		str++;
	}
	return true;
}

static inline XQNav* getNavigation(ASTNode *possibleNav, XPath2MemoryManager * expr)
{
	ASTNode* originalObj=possibleNav;
	if((unsigned int)possibleNav->getType()==ASTNode::DEBUG_HOOK)
		possibleNav=((XQDebugHook*)possibleNav)->m_impl;
	if(possibleNav->getType()==ASTNode::NAVIGATION)
		return (XQNav*)possibleNav;
	else
	{
		XQNav* nav=new (expr) XQNav(expr);
		nav->addStep(originalObj);
		return nav;
	}
}

static inline VectorOfASTNodes packageArgs(ASTNode *arg1Impl, ASTNode *arg2Impl, XPath2MemoryManager* memMgr)
{
	VectorOfASTNodes args=VectorOfASTNodes(2,(ASTNode*)NULL,XQillaAllocator<ASTNode*>(memMgr));
	args[0]=arg1Impl;
	args[1]=arg2Impl;

	return args;
}

static void merge_strings(DynamicContext* context, VectorOfASTNodes* vec, XMLCh* toBeAdded)
{
	if(vec->size()>0 && vec->back()!=0 && vec->back()->getType()==ASTNode::LITERAL)
	{
		XQLiteral *lit = (XQLiteral*)vec->back();
		const XMLCh* string=lit->getItemConstructor()->createItem(context)->asString(context);
		string=XPath2Utils::concatStrings(string,toBeAdded,context->getMemoryManager());

		AnyAtomicTypeConstructor *ic = new (context->getMemoryManager())
    		AnyAtomicTypeConstructor(
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
            string, AnyAtomicType::STRING);
    	lit->setItemConstructor(ic);
	}
	else
	{
    if(vec->size()>0 && vec->back()==0)
			vec->pop_back();
    	AnyAtomicTypeConstructor *ic = new (context->getMemoryManager())
      		AnyAtomicTypeConstructor(
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
						XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
				toBeAdded, AnyAtomicType::STRING);

		vec->push_back(new (context->getMemoryManager())
                   XQLiteral(ic, context->getMemoryManager()));
	}
}

template<typename TYPE>
TYPE *wrapForDebug(XQParserArgs *qp, TYPE* pObjToWrap,
                   const XMLCh* fnName, unsigned int line, unsigned int column)
{
  if(pObjToWrap->getLine() == 0)
    pObjToWrap->setLocationInfo(QP->_query->getFile(), line, column);
  return pObjToWrap;
}

template<>
ASTNode *wrapForDebug(XQParserArgs *qp, ASTNode* pObjToWrap,
                      const XMLCh* fnName, unsigned int line, unsigned int column)
{
  if(pObjToWrap->getLine() == 0)
    pObjToWrap->setLocationInfo(QP->_query->getFile(), line, column);

  if(!CONTEXT->isDebuggingEnabled() && !CONTEXT->getDebugCallback())
    return pObjToWrap;
  if(fnName==NULL && (unsigned int)pObjToWrap->getType()==ASTNode::DEBUG_HOOK)
    return pObjToWrap;
  return new (MEMMGR) XQDebugHook(QP->_query->getFile(), line, column, pObjToWrap, fnName, MEMMGR);
}

namespace XQParser {



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined (YYLTYPE) && ! defined (YYLTYPE_IS_DECLARED)
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */
#line 840 "../src/parser/XQParser.cpp"

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYLTYPE_IS_TRIVIAL) && YYLTYPE_IS_TRIVIAL \
             && defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2651

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  238
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  180
/* YYNRULES -- Number of rules. */
#define YYNRULES  465
/* YYNRULES -- Number of states. */
#define YYNSTATES  829

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   492

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,     9,    11,    13,    18,    25,    28,
      31,    38,    39,    43,    47,    51,    55,    59,    63,    67,
      71,    73,    75,    77,    79,    81,    83,    85,    87,    89,
      91,    97,   101,   105,   111,   117,   122,   125,   129,   133,
     139,   145,   151,   153,   155,   157,   159,   164,   168,   173,
     179,   183,   188,   191,   195,   199,   203,   211,   218,   223,
     227,   235,   242,   249,   255,   259,   263,   270,   276,   284,
     291,   298,   304,   312,   319,   323,   325,   329,   332,   336,
     338,   342,   344,   346,   348,   350,   352,   354,   360,   365,
     370,   374,   377,   379,   381,   383,   386,   390,   392,   397,
     403,   409,   416,   422,   429,   436,   444,   448,   452,   455,
     459,   461,   466,   472,   478,   485,   489,   492,   496,   501,
     505,   507,   511,   517,   518,   520,   522,   523,   526,   529,
     534,   539,   543,   545,   550,   556,   567,   576,   579,   581,
     586,   594,   603,   607,   609,   613,   615,   619,   623,   627,
     631,   635,   639,   643,   647,   651,   655,   659,   663,   667,
     671,   675,   677,   681,   686,   688,   692,   694,   698,   702,
     704,   708,   712,   716,   720,   722,   726,   730,   732,   736,
     740,   742,   747,   749,   754,   756,   761,   763,   768,   770,
     773,   776,   778,   780,   782,   784,   789,   795,   801,   805,
     810,   812,   815,   820,   824,   826,   829,   832,   834,   838,
     842,   844,   846,   848,   851,   854,   857,   859,   861,   863,
     865,   867,   869,   871,   873,   875,   878,   880,   883,   885,
     887,   889,   891,   893,   895,   897,   899,   901,   903,   905,
     907,   909,   911,   914,   915,   920,   922,   924,   926,   928,
     930,   932,   934,   936,   938,   940,   942,   944,   946,   949,
     953,   956,   958,   963,   968,   971,   975,   979,   981,   983,
     985,   987,   989,   991,   996,  1005,  1006,  1011,  1015,  1019,
    1020,  1023,  1026,  1029,  1032,  1033,  1036,  1039,  1042,  1045,
    1046,  1049,  1052,  1055,  1058,  1061,  1063,  1065,  1067,  1069,
    1071,  1075,  1077,  1079,  1081,  1083,  1085,  1087,  1089,  1094,
    1099,  1103,  1111,  1118,  1120,  1125,  1129,  1137,  1144,  1149,
    1154,  1159,  1163,  1171,  1178,  1181,  1183,  1186,  1189,  1191,
    1195,  1197,  1199,  1201,  1203,  1205,  1209,  1211,  1213,  1215,
    1217,  1219,  1221,  1223,  1225,  1227,  1229,  1232,  1235,  1239,
    1243,  1246,  1249,  1252,  1256,  1260,  1263,  1267,  1273,  1275,
    1277,  1281,  1283,  1286,  1290,  1296,  1303,  1305,  1307,  1311,
    1313,  1315,  1317,  1319,  1321,  1326,  1329,  1330,  1333,  1336,
    1340,  1342,  1346,  1348,  1352,  1354,  1357,  1359,  1363,  1365,
    1368,  1371,  1374,  1377,  1382,  1384,  1388,  1392,  1396,  1399,
    1402,  1404,  1406,  1408,  1410,  1412,  1414,  1416,  1418,  1420,
    1422,  1424,  1426,  1428,  1430,  1432,  1434,  1436,  1438,  1440,
    1442,  1445,  1448,  1454,  1460,  1462,  1463,  1467,  1470,  1475,
    1480,  1487,  1491,  1493,  1496,  1497,  1500,  1503,  1507,  1509,
    1513,  1516,  1519,  1522,  1524,  1526,  1527,  1529,  1531,  1533,
    1535,  1537,  1540,  1543,  1546,  1551,  1553,  1555,  1557,  1559,
    1561,  1564,  1566,  1568,  1570,  1572
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     239,     0,    -1,   240,   241,    -1,   240,   242,    -1,   241,
      -1,   242,    -1,   178,   179,     7,   247,    -1,   178,   179,
       7,   165,     7,   247,    -1,   244,   270,    -1,   243,   244,
      -1,   182,   138,    14,    86,   384,   247,    -1,    -1,   244,
     245,   247,    -1,   244,   246,   247,    -1,   244,   248,   247,
      -1,   244,   250,   247,    -1,   244,   264,   247,    -1,   244,
     266,   247,    -1,   244,   251,   247,    -1,   244,   252,   247,
      -1,   249,    -1,   258,    -1,   259,    -1,   265,    -1,   253,
      -1,   254,    -1,   255,    -1,   260,    -1,   263,    -1,   155,
      -1,   169,   138,    14,    86,   384,    -1,   169,   170,   136,
      -1,   169,   170,   137,    -1,   169,   174,   183,   138,   384,
      -1,   169,   174,   184,   138,   384,    -1,   169,   177,     3,
       7,    -1,   188,   394,    -1,   169,   173,   161,    -1,   169,
     173,   160,    -1,   169,   174,    44,    48,    49,    -1,   169,
     174,    44,    48,    50,    -1,   169,   175,   256,    34,   257,
      -1,   136,    -1,   166,    -1,   167,    -1,   168,    -1,   169,
     174,    51,   384,    -1,   169,   171,   384,    -1,   180,   181,
     262,   384,    -1,   180,   181,   262,   384,   261,    -1,   180,
     181,   384,    -1,   180,   181,   384,   261,    -1,    39,   384,
      -1,   261,    34,   384,    -1,   138,    14,    86,    -1,   174,
     183,   138,    -1,   180,   182,   138,    14,    86,   384,   261,
      -1,   180,   182,   138,    14,    86,   384,    -1,   180,   182,
     384,   261,    -1,   180,   182,   384,    -1,   169,   176,    38,
       8,   362,    41,   272,    -1,   169,   176,    38,     8,    41,
     272,    -1,   169,   176,    38,     8,   362,   146,    -1,   169,
     176,    38,     8,   146,    -1,   169,   172,   136,    -1,   169,
     172,   137,    -1,   169,   184,     4,   267,   117,   269,    -1,
     169,   184,     4,   117,   269,    -1,   169,   184,     4,   267,
     139,   363,   269,    -1,   169,   184,     4,   139,   363,   269,
      -1,   169,   184,     4,   267,   117,   146,    -1,   169,   184,
       4,   117,   146,    -1,   169,   184,     4,   267,   139,   363,
     146,    -1,   169,   184,     4,   139,   363,   146,    -1,   267,
      34,   268,    -1,   268,    -1,    38,     8,   362,    -1,    38,
       8,    -1,   134,   271,   135,    -1,   271,    -1,   271,    34,
     272,    -1,   272,    -1,   273,    -1,   290,    -1,   293,    -1,
     296,    -1,   297,    -1,   274,   284,   285,    35,   272,    -1,
     274,   284,    35,   272,    -1,   274,   285,    35,   272,    -1,
     274,    35,   272,    -1,   274,   275,    -1,   275,    -1,   276,
      -1,   281,    -1,    36,   277,    -1,   277,    34,   278,    -1,
     278,    -1,    38,     8,    37,   272,    -1,    38,     8,   280,
      37,   272,    -1,    38,     8,   279,    37,   272,    -1,    38,
       8,   279,   280,    37,   272,    -1,    38,     8,   362,    37,
     272,    -1,    38,     8,   362,   280,    37,   272,    -1,    38,
       8,   362,   279,    37,   272,    -1,    38,     8,   362,   279,
     280,    37,   272,    -1,    39,    38,     8,    -1,   185,    38,
       8,    -1,    40,   282,    -1,   282,    34,   283,    -1,   283,
      -1,    38,     8,    41,   272,    -1,    38,     8,   280,    41,
     272,    -1,    38,     8,   362,    41,   272,    -1,    38,     8,
     362,   280,    41,   272,    -1,   280,    41,   272,    -1,    42,
     272,    -1,    44,    43,   286,    -1,    45,    44,    43,   286,
      -1,   286,    34,   287,    -1,   287,    -1,   272,   288,   289,
      -1,   272,   288,   289,    51,   384,    -1,    -1,    46,    -1,
      47,    -1,    -1,    48,    49,    -1,    48,    50,    -1,    52,
     291,    54,   272,    -1,    53,   291,    54,   272,    -1,   291,
      34,   292,    -1,   292,    -1,    38,     8,    37,   272,    -1,
      38,     8,   362,    37,   272,    -1,    55,   116,   271,   117,
     294,   174,    38,     8,    35,   272,    -1,    55,   116,   271,
     117,   294,   174,    35,   272,    -1,   294,   295,    -1,   295,
      -1,    56,   363,    35,   272,    -1,    56,    38,     8,    57,
     363,    35,   272,    -1,    58,   116,   271,   117,    59,   272,
      60,   272,    -1,   297,    61,   298,    -1,   298,    -1,   298,
      62,   299,    -1,   299,    -1,   300,    86,   300,    -1,   300,
      87,   300,    -1,   300,    88,   300,    -1,   300,    89,   300,
      -1,   300,    90,   300,    -1,   300,    91,   300,    -1,   300,
      92,   300,    -1,   300,    93,   300,    -1,   300,    94,   300,
      -1,   300,    95,   300,    -1,   300,    96,   300,    -1,   300,
      97,   300,    -1,   300,    98,   300,    -1,   300,    99,   300,
      -1,   300,   100,   300,    -1,   300,    -1,   301,   186,   385,
      -1,   301,   186,   385,   412,    -1,   301,    -1,   302,    66,
     302,    -1,   302,    -1,   302,    67,   303,    -1,   302,    68,
     303,    -1,   303,    -1,   303,    69,   304,    -1,   303,    70,
     304,    -1,   303,    71,   304,    -1,   303,    72,   304,    -1,
     304,    -1,   304,    74,   305,    -1,   304,    73,   305,    -1,
     305,    -1,   305,    75,   306,    -1,   305,    76,   306,    -1,
     306,    -1,   307,    63,    64,   363,    -1,   307,    -1,   308,
      85,    57,   363,    -1,   308,    -1,   309,    65,    57,   361,
      -1,   309,    -1,   310,    84,    57,   361,    -1,   310,    -1,
      68,   310,    -1,    67,   310,    -1,   311,    -1,   312,    -1,
     316,    -1,   313,    -1,    81,   134,   271,   135,    -1,    81,
      82,   134,   271,   135,    -1,    81,    83,   134,   271,   135,
      -1,   314,   134,   135,    -1,   314,   134,   271,   135,    -1,
     315,    -1,   314,   315,    -1,    30,    25,    26,    31,    -1,
      30,    25,    31,    -1,    77,    -1,    77,   317,    -1,    78,
     317,    -1,   317,    -1,   317,    77,   318,    -1,   317,    78,
     318,    -1,   318,    -1,   319,    -1,   329,    -1,   320,   330,
      -1,   323,   330,    -1,   321,   326,    -1,   322,    -1,   104,
      -1,   105,    -1,   106,    -1,   107,    -1,   108,    -1,   151,
      -1,   152,    -1,   110,    -1,   102,   326,    -1,   326,    -1,
     324,   326,    -1,   325,    -1,   109,    -1,   150,    -1,   153,
      -1,   154,    -1,   149,    -1,   103,    -1,   367,    -1,   327,
      -1,   417,    -1,   328,    -1,   111,    -1,    12,    -1,    13,
      -1,   331,   330,    -1,    -1,   330,    79,   271,    80,    -1,
     332,    -1,   334,    -1,   335,    -1,   336,    -1,   339,    -1,
     341,    -1,   337,    -1,   338,    -1,   333,    -1,   416,    -1,
     413,    -1,   414,    -1,   415,    -1,    38,     8,    -1,   116,
     271,   117,    -1,   116,   117,    -1,   101,    -1,   158,   134,
     271,   135,    -1,   159,   134,   271,   135,    -1,     4,   117,
      -1,     4,   340,   117,    -1,   340,    34,   272,    -1,   272,
      -1,   342,    -1,   353,    -1,   343,    -1,   350,    -1,   351,
      -1,   118,     5,   344,   119,    -1,   118,     5,   344,   122,
     348,   121,     5,   122,    -1,    -1,   344,     6,   120,   345,
      -1,   130,   346,   131,    -1,   132,   347,   133,    -1,    -1,
     346,   269,    -1,   346,    33,    -1,   346,     9,    -1,   346,
     349,    -1,    -1,   347,   269,    -1,   347,    32,    -1,   347,
       9,    -1,   347,   349,    -1,    -1,   348,   342,    -1,   348,
       9,    -1,   348,   352,    -1,   348,   269,    -1,   348,   349,
      -1,    11,    -1,    10,    -1,   128,    -1,   129,    -1,    21,
      -1,   127,    15,    23,    -1,    22,    -1,   354,    -1,   355,
      -1,   357,    -1,   358,    -1,   359,    -1,   360,    -1,   123,
     134,   271,   135,    -1,    17,   134,   356,   135,    -1,    17,
     134,   135,    -1,   124,   134,   271,   135,   134,   356,   135,
      -1,   124,   134,   271,   135,   134,   135,    -1,   271,    -1,
      16,   134,   271,   135,    -1,    16,   134,   135,    -1,   125,
     134,   271,   135,   134,   271,   135,    -1,   125,   134,   271,
     135,   134,   135,    -1,   126,   134,   271,   135,    -1,   148,
     134,   271,   135,    -1,    24,   134,   271,   135,    -1,    24,
     134,   135,    -1,   147,   134,   271,   135,   134,   271,   135,
      -1,   147,   134,   271,   135,   134,   135,    -1,   366,   162,
      -1,   366,    -1,    57,   363,    -1,   365,   364,    -1,   365,
      -1,   140,   116,   117,    -1,   164,    -1,   163,    -1,   162,
      -1,   366,    -1,   367,    -1,   141,   116,   117,    -1,   417,
      -1,   369,    -1,   377,    -1,   373,    -1,   379,    -1,   375,
      -1,   372,    -1,   371,    -1,   370,    -1,   368,    -1,   115,
     117,    -1,   143,   117,    -1,   143,   377,   117,    -1,   143,
     379,   117,    -1,   114,   117,    -1,   113,   117,    -1,   112,
     117,    -1,   112,    14,   117,    -1,   112,     7,   117,    -1,
     144,   117,    -1,   144,   374,   117,    -1,   144,   374,    34,
     383,   117,    -1,   381,    -1,   111,    -1,   157,   376,   117,
      -1,   381,    -1,   145,   117,    -1,   145,   378,   117,    -1,
     145,   378,    34,   383,   117,    -1,   145,   378,    34,   383,
     142,   117,    -1,   382,    -1,   111,    -1,   156,   380,   117,
      -1,   382,    -1,   417,    -1,   417,    -1,   417,    -1,     7,
      -1,   387,   386,   187,   414,    -1,   387,   386,    -1,    -1,
     386,   394,    -1,   386,   393,    -1,   387,   189,   388,    -1,
     388,    -1,   388,   190,   389,    -1,   389,    -1,   389,   191,
     390,    -1,   390,    -1,   192,   391,    -1,   391,    -1,   116,
     385,   117,    -1,   392,    -1,   332,   408,    -1,   334,   408,
      -1,   336,   408,    -1,   339,   408,    -1,   134,   271,   135,
     408,    -1,   161,    -1,   193,   304,   410,    -1,   194,   409,
     410,    -1,   195,   409,   196,    -1,   197,   411,    -1,   198,
     411,    -1,   199,    -1,   200,    -1,   201,    -1,   395,    -1,
     396,    -1,   397,    -1,   398,    -1,   401,    -1,   406,    -1,
     407,    -1,   202,    -1,   203,    -1,   205,    -1,   204,    -1,
     206,    -1,   207,    -1,   208,    -1,   209,    -1,   210,    -1,
     211,    -1,   212,   400,    -1,   212,   174,    -1,   212,   116,
     400,   399,   117,    -1,   212,   116,   174,   399,   117,    -1,
     213,    -1,    -1,   399,    34,   400,    -1,    39,   416,    -1,
      39,   416,   214,   416,    -1,    39,   416,   409,   215,    -1,
      39,   416,   214,   416,   409,   215,    -1,   216,   403,   402,
      -1,   217,    -1,   218,   402,    -1,    -1,   402,   405,    -1,
      39,   416,    -1,   116,   404,   117,    -1,   416,    -1,   404,
      34,   416,    -1,    73,   403,    -1,    76,   403,    -1,   219,
     416,    -1,   220,    -1,   221,    -1,    -1,   222,    -1,   223,
      -1,   224,    -1,   225,    -1,   226,    -1,   227,   304,    -1,
     228,   304,    -1,   229,   304,    -1,   230,   304,    66,   304,
      -1,   231,    -1,   232,    -1,   233,    -1,   234,    -1,   235,
      -1,   236,   304,    -1,    18,    -1,    19,    -1,    20,    -1,
       7,    -1,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   563,   563,   564,   565,   566,   571,   578,   612,   619,
     626,   638,   640,   645,   650,   655,   660,   664,   671,   675,
     689,   690,   691,   692,   693,   694,   695,   700,   701,   706,
     711,   725,   734,   747,   756,   769,   793,   802,   811,   824,
     833,   846,   860,   864,   872,   876,   884,   906,   919,   937,
     955,   967,   982,   987,   996,  1000,  1008,  1023,  1038,  1052,
    1070,  1077,  1085,  1092,  1104,  1113,  1127,  1133,  1139,  1145,
    1151,  1157,  1163,  1169,  1179,  1184,  1194,  1198,  1206,  1214,
    1222,  1240,  1248,  1249,  1250,  1251,  1252,  1257,  1261,  1265,
    1269,  1276,  1282,  1286,  1287,  1293,  1300,  1304,  1312,  1316,
    1321,  1325,  1329,  1333,  1337,  1341,  1349,  1359,  1371,  1380,
    1385,  1393,  1397,  1401,  1405,  1409,  1416,  1426,  1432,  1442,
    1447,  1457,  1461,  1478,  1479,  1481,  1487,  1496,  1498,  1505,
    1509,  1516,  1520,  1528,  1532,  1540,  1547,  1557,  1562,  1571,
    1575,  1583,  1591,  1605,  1610,  1624,  1634,  1638,  1642,  1646,
    1650,  1654,  1658,  1662,  1666,  1670,  1674,  1678,  1682,  1686,
    1690,  1694,  1699,  1705,  1711,  1716,  1720,  1725,  1729,  1733,
    1738,  1742,  1746,  1750,  1754,  1759,  1763,  1767,  1772,  1776,
    1780,  1785,  1789,  1794,  1800,  1805,  1809,  1814,  1818,  1823,
    1829,  1835,  1840,  1841,  1842,  1850,  1854,  1858,  1866,  1873,
    1883,  1884,  1890,  1908,  1930,  1945,  1960,  1981,  1986,  1992,
    2005,  2010,  2011,  2016,  2020,  2029,  2041,  2052,  2056,  2060,
    2064,  2068,  2072,  2076,  2080,  2089,  2097,  2115,  2123,  2132,
    2136,  2140,  2144,  2148,  2156,  2168,  2169,  2174,  2181,  2186,
    2193,  2200,  2211,  2221,  2224,  2234,  2235,  2236,  2237,  2238,
    2239,  2240,  2241,  2246,  2247,  2252,  2253,  2254,  2260,  2272,
    2286,  2294,  2302,  2312,  2322,  2327,  2334,  2339,  2348,  2352,
    2362,  2363,  2364,  2369,  2381,  2423,  2426,  2458,  2462,  2472,
    2475,  2483,  2488,  2493,  2504,  2507,  2515,  2520,  2525,  2541,
    2544,  2565,  2594,  2613,  2635,  2654,  2655,  2656,  2660,  2669,
    2684,  2708,  2727,  2728,  2729,  2730,  2731,  2732,  2737,  2745,
    2759,  2771,  2780,  2791,  2796,  2809,  2821,  2829,  2840,  2848,
    2856,  2867,  2878,  2884,  2894,  2901,  2912,  2922,  2929,  2936,
    2945,  2947,  2949,  2955,  2959,  2964,  2972,  2988,  2989,  2990,
    2991,  2992,  2993,  2994,  2995,  2996,  3001,  3011,  3017,  3026,
    3039,  3049,  3059,  3065,  3071,  3081,  3087,  3093,  3103,  3104,
    3112,  3122,  3127,  3133,  3139,  3145,  3157,  3158,  3166,  3176,
    3181,  3186,  3191,  3196,  3208,  3220,  3235,  3238,  3242,  3252,
    3263,  3268,  3279,  3284,  3288,  3293,  3297,  3302,  3306,  3311,
    3315,  3319,  3323,  3327,  3341,  3345,  3349,  3353,  3358,  3362,
    3366,  3370,  3374,  3388,  3389,  3390,  3391,  3392,  3393,  3394,
    3402,  3406,  3410,  3414,  3425,  3429,  3433,  3437,  3445,  3449,
    3459,  3463,  3467,  3471,  3475,  3483,  3485,  3492,  3496,  3500,
    3504,  3514,  3518,  3522,  3530,  3532,  3540,  3544,  3551,  3555,
    3563,  3567,  3575,  3583,  3587,  3596,  3599,  3603,  3607,  3611,
    3615,  3626,  3632,  3638,  3644,  3654,  3658,  3662,  3670,  3674,
    3682,  3690,  3703,  3716,  3730,  3743
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "\"<end of file>\"", "error", "$undefined", "\"<qualified name>\"",
  "\"<function>(\"", "\"<tag name>\"", "\"<attribute name>\"", "\"'...'\"",
  "\"<variable name>\"", "\"<char>\"", "\"&#XXX;\"",
  "\"<predefined entity ref>\"", "\"<ns>:*\"", "\"*:<local name>\"",
  "\"<local name>\"", "\"<processing instruction target>\"",
  "\"attribute <name> {\"", "\"element <name> {\"", "\"<integer number>\"",
  "\"<decimal number>\"", "\"<double number>\"", "\"<!-- ... -->\"",
  "\"<![CDATA[...]]>\"", "\"...?>\"", "\"processing-instruction <pi> {\"",
  "\"<pragma name>\"", "\"<pragma content>\"", "\"(: comment :)\"",
  "_XQUERY_WHITESPACE_", "_XQUERY_ERROR_", "\"(#\"", "\"#)\"", "\"''\"",
  "\"\\\"\\\"\"", "\",\"", "\"return\"", "\"for\"", "\"in\"", "\"$\"",
  "\"at\"", "\"let\"", "\":=\"", "\"where\"", "\"by\"", "\"order\"",
  "\"stable\"", "\"ascending\"", "\"descending\"", "\"empty\"",
  "\"greatest\"", "\"least\"", "\"collation\"", "\"some\"", "\"every\"",
  "\"satisfies\"", "\"typeswitch\"", "\"case\"", "\"as\"", "\"if\"",
  "\"then\"", "\"else\"", "\"or\"", "\"and\"", "\"instance\"", "\"of\"",
  "\"castable\"", "\"to\"", "\"+ (arithmetic operator)\"",
  "\"- (arithmetic operator)\"", "\"* (arithmetic operator)\"", "\"div\"",
  "\"idiv\"", "\"mod\"", "\"union\"", "\"|\"", "\"intersect\"",
  "\"except\"", "\"/\"", "\"//\"", "\"[\"", "\"]\"", "\"validate\"",
  "\"lax\"", "\"strict\"", "\"cast\"", "\"treat\"",
  "\"= (comparison operator)\"", "\"!=\"", "\"< (comparison operator)\"",
  "\"<=\"", "\"> (comparison operator)\"", "\">=\"", "\"eq\"", "\"ne\"",
  "\"lt\"", "\"le\"", "\"gt\"", "\"ge\"", "\"is\"", "\"<<\"", "\">>\"",
  "\".\"", "\"@\"", "\"..\"", "\"child::\"", "\"descendant::\"",
  "\"attribute::\"", "\"self::\"", "\"descendant-or-self::\"",
  "\"parent::\"", "\"namespace::\"", "\"* (wildcard)\"",
  "\"processing-instruction(\"", "\"comment(\"", "\"text(\"", "\"node(\"",
  "\"(\"", "\")\"", "\"<\"", "\"/>\"", "\"=\"", "\">\"", "\"</\"",
  "\"document {\"", "\"element {\"", "\"attribute {\"", "\"text {\"",
  "\"<?\"", "\"{{\"", "\"}}\"", "\"\\\" (open)\"", "\"\\\" (close)\"",
  "\"' (open)\"", "\"' (close)\"", "\"{\"", "\"}\"", "\"preserve\"",
  "\"strip\"", "\"namespace\"", "\") as\"", "\"empty-sequence\"",
  "\"item\"", "\"?\"", "\"document-node(\"", "\"attribute(\"",
  "\"element(\"", "\"external\"", "\"processing-instruction {\"",
  "\"comment {\"", "\"ancestor-or-self::\"", "\"ancestor::\"",
  "\"following-sibling::\"", "\"following::\"", "\"preceding-sibling::\"",
  "\"preceding::\"", "\";\"", "\"schema-element(\"",
  "\"schema-attribute(\"", "\"ordered {\"", "\"unordered {\"",
  "\"unordered\"", "\"ordered\"", "\"? (occurrence)\"",
  "\"+ (occurrence)\"", "\"* (occurrence)\"", "\"encoding\"",
  "\"no-preserve\"", "\"inherit\"", "\"no-inherit\"", "\"declare\"",
  "\"boundary-space\"", "\"base-uri\"", "\"construction\"", "\"ordering\"",
  "\"default\"", "\"copy-namespaces\"", "\"variable\"", "\"option\"",
  "\"xquery\"", "\"version\"", "\"import\"", "\"schema\"", "\"module\"",
  "\"element\"", "\"function\"", "\"score\"", "\"ftcontains\"",
  "\"weight\"", "\"declare ft-option\"", "\"||\"", "\"&&\"", "\"not in\"",
  "\"!\"", "\"window\"", "\"distance\"", "\"occurs\"", "\"times\"",
  "\"same\"", "\"different\"", "\"at start\"", "\"at end\"",
  "\"entire content\"", "\"lowercase\"", "\"uppercase\"",
  "\"case insensitive\"", "\"case sensitive\"", "\"with diacritics\"",
  "\"without diacritics\"", "\"diacritics sensitive\"",
  "\"diacritics insensitive\"", "\"with stemming\"",
  "\"without stemming\"", "\"with thesaurus\"", "\"without thesaurus\"",
  "\"relationship\"", "\"levels\"", "\"with stop words\"",
  "\"without stop words\"", "\"with default stop words\"", "\"language\"",
  "\"with wildcards\"", "\"without wildcards\"", "\"any\"", "\"any word\"",
  "\"all\"", "\"all words\"", "\"phrase\"", "\"exactly\"", "\"at least\"",
  "\"at most\"", "\"from\"", "\"words\"", "\"sentences\"",
  "\"paragraphs\"", "\"sentence\"", "\"paragraph\"", "\"without content\"",
  "_EOF_", "$accept", "Module", "VersionDecl", "MainModule",
  "LibraryModule", "ModuleDecl", "Prolog", "Setter", "Import", "Separator",
  "NamespaceDecl", "BoundarySpaceDecl", "DefaultNamespaceDecl",
  "OptionDecl", "FTOptionDecl", "OrderingModeDecl", "EmptyOrderDecl",
  "CopyNamespacesDecl", "PreserveMode", "InheritMode",
  "DefaultCollationDecl", "BaseURIDecl", "SchemaImport",
  "ResourceLocations", "SchemaPrefix", "ModuleImport", "VarDecl",
  "ConstructionDecl", "FunctionDecl", "ParamList", "Param", "EnclosedExpr",
  "QueryBody", "Expr", "ExprSingle", "FLWORExpr", "FlworExprForLetList",
  "ForOrLetClause", "ForClause", "ForBindingList", "ForBinding",
  "PositionalVar", "FTScoreVar", "LetClause", "LetBindingList",
  "LetBinding", "WhereClause", "OrderByClause", "OrderSpecList",
  "OrderSpec", "OrderDirection", "EmptyHandling", "QuantifiedExpr",
  "QuantifyBindingList", "QuantifyBinding", "TypeswitchExpr",
  "CaseClauseList", "CaseClause", "IfExpr", "OrExpr", "AndExpr",
  "ComparisonExpr", "FTContainsExpr", "RangeExpr", "AdditiveExpr",
  "MultiplicativeExpr", "UnionExpr", "IntersectExceptExpr",
  "InstanceofExpr", "TreatExpr", "CastableExpr", "CastExpr", "UnaryExpr",
  "ValueExpr", "ValidateExpr", "ExtensionExpr", "PragmaList", "Pragma",
  "PathExpr", "RelativePathExpr", "StepExpr", "AxisStep", "ForwardStep",
  "ForwardAxis", "AbbrevForwardStep", "ReverseStep", "ReverseAxis",
  "AbbrevReverseStep", "NodeTest", "NameTest", "Wildcard", "FilterExpr",
  "PredicateList", "PrimaryExpr", "Literal", "NumericLiteral", "VarRef",
  "ParenthesizedExpr", "ContextItemExpr", "OrderedExpr", "UnorderedExpr",
  "FunctionCall", "FunctionCallArgumentList", "Constructor",
  "DirectConstructor", "DirElemConstructor", "DirAttributeList",
  "DirAttributeValue", "QuotAttrValueContent", "AposAttrValueContent",
  "DirElementContent", "CommonContent", "DirCommentConstructor",
  "DirPIConstructor", "CdataSection", "ComputedConstructor",
  "CompDocConstructor", "CompElemConstructor", "ContentExpr",
  "CompAttrConstructor", "CompTextConstructor", "CompCommentConstructor",
  "CompPIConstructor", "SingleType", "TypeDeclaration", "SequenceType",
  "OccurrenceIndicator", "ItemType", "AtomicType", "KindTest",
  "AnyKindTest", "DocumentTest", "TextTest", "CommentTest", "PITest",
  "AttributeTest", "AttribNameOrWildcard", "SchemaAttributeTest",
  "AttributeDeclaration", "ElementTest", "ElementNameOrWildcard",
  "SchemaElementTest", "ElementDeclaration", "AttributeName",
  "ElementName", "TypeName", "URILiteral", "FTSelection",
  "FTSelectionOptions", "FTOr", "FTAnd", "FTMildnot", "FTUnaryNot",
  "FTWordsSelection", "FTWords", "FTProximity", "FTMatchOption",
  "FTCaseOption", "FTDiacriticsOption", "FTStemOption",
  "FTThesaurusOption", "FTThesaurusIDList", "FTThesaurusID",
  "FTStopwordOption", "FTInclExclStringLiteralList", "FTRefOrList",
  "FTRefOrListStringList", "FTInclExclStringLiteral", "FTLanguageOption",
  "FTWildCardOption", "FTAnyallOption", "FTRange", "FTUnit", "FTBigUnit",
  "FTIgnoreOption", "IntegerLiteral", "DecimalLiteral", "DoubleLiteral",
  "StringLiteral", "QName", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short int yyr1[] =
{
       0,   238,   239,   239,   239,   239,   240,   240,   241,   242,
     243,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     245,   245,   245,   245,   245,   245,   245,   246,   246,   247,
     248,   249,   249,   250,   250,   251,   252,   253,   253,   254,
     254,   255,   256,   256,   257,   257,   258,   259,   260,   260,
     260,   260,   261,   261,   262,   262,   263,   263,   263,   263,
     264,   264,   264,   264,   265,   265,   266,   266,   266,   266,
     266,   266,   266,   266,   267,   267,   268,   268,   269,   270,
     271,   271,   272,   272,   272,   272,   272,   273,   273,   273,
     273,   274,   274,   275,   275,   276,   277,   277,   278,   278,
     278,   278,   278,   278,   278,   278,   279,   280,   281,   282,
     282,   283,   283,   283,   283,   283,   284,   285,   285,   286,
     286,   287,   287,   288,   288,   288,   289,   289,   289,   290,
     290,   291,   291,   292,   292,   293,   293,   294,   294,   295,
     295,   296,   297,   297,   298,   298,   299,   299,   299,   299,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   299,
     299,   299,   300,   300,   300,   301,   301,   302,   302,   302,
     303,   303,   303,   303,   303,   304,   304,   304,   305,   305,
     305,   306,   306,   307,   307,   308,   308,   309,   309,   310,
     310,   310,   311,   311,   311,   312,   312,   312,   313,   313,
     314,   314,   315,   315,   316,   316,   316,   316,   317,   317,
     317,   318,   318,   319,   319,   320,   320,   321,   321,   321,
     321,   321,   321,   321,   321,   322,   322,   323,   323,   324,
     324,   324,   324,   324,   325,   326,   326,   327,   327,   328,
     328,   328,   329,   330,   330,   331,   331,   331,   331,   331,
     331,   331,   331,   332,   332,   333,   333,   333,   334,   335,
     335,   336,   337,   338,   339,   339,   340,   340,   341,   341,
     342,   342,   342,   343,   343,   344,   344,   345,   345,   346,
     346,   346,   346,   346,   347,   347,   347,   347,   347,   348,
     348,   348,   348,   348,   348,   349,   349,   349,   349,   350,
     351,   352,   353,   353,   353,   353,   353,   353,   354,   355,
     355,   355,   355,   356,   357,   357,   357,   357,   358,   359,
     360,   360,   360,   360,   361,   361,   362,   363,   363,   363,
     364,   364,   364,   365,   365,   365,   366,   367,   367,   367,
     367,   367,   367,   367,   367,   367,   368,   369,   369,   369,
     370,   371,   372,   372,   372,   373,   373,   373,   374,   374,
     375,   376,   377,   377,   377,   377,   378,   378,   379,   380,
     381,   382,   383,   384,   385,   385,   386,   386,   386,   387,
     387,   388,   388,   389,   389,   390,   390,   391,   391,   392,
     392,   392,   392,   392,   393,   393,   393,   393,   393,   393,
     393,   393,   393,   394,   394,   394,   394,   394,   394,   394,
     395,   395,   395,   395,   396,   396,   396,   396,   397,   397,
     398,   398,   398,   398,   398,   399,   399,   400,   400,   400,
     400,   401,   401,   401,   402,   402,   403,   403,   404,   404,
     405,   405,   406,   407,   407,   408,   408,   408,   408,   408,
     408,   409,   409,   409,   409,   410,   410,   410,   411,   411,
     412,   413,   414,   415,   416,   417
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     2,     1,     1,     4,     6,     2,     2,
       6,     0,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       5,     3,     3,     5,     5,     4,     2,     3,     3,     5,
       5,     5,     1,     1,     1,     1,     4,     3,     4,     5,
       3,     4,     2,     3,     3,     3,     7,     6,     4,     3,
       7,     6,     6,     5,     3,     3,     6,     5,     7,     6,
       6,     5,     7,     6,     3,     1,     3,     2,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     5,     4,     4,
       3,     2,     1,     1,     1,     2,     3,     1,     4,     5,
       5,     6,     5,     6,     6,     7,     3,     3,     2,     3,
       1,     4,     5,     5,     6,     3,     2,     3,     4,     3,
       1,     3,     5,     0,     1,     1,     0,     2,     2,     4,
       4,     3,     1,     4,     5,    10,     8,     2,     1,     4,
       7,     8,     3,     1,     3,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     3,     4,     1,     3,     1,     3,     3,     1,
       3,     3,     3,     3,     1,     3,     3,     1,     3,     3,
       1,     4,     1,     4,     1,     4,     1,     4,     1,     2,
       2,     1,     1,     1,     1,     4,     5,     5,     3,     4,
       1,     2,     4,     3,     1,     2,     2,     1,     3,     3,
       1,     1,     1,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     0,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       2,     1,     4,     4,     2,     3,     3,     1,     1,     1,
       1,     1,     1,     4,     8,     0,     4,     3,     3,     0,
       2,     2,     2,     2,     0,     2,     2,     2,     2,     0,
       2,     2,     2,     2,     2,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     4,     4,
       3,     7,     6,     1,     4,     3,     7,     6,     4,     4,
       4,     3,     7,     6,     2,     1,     2,     2,     1,     3,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     3,     3,
       2,     2,     2,     3,     3,     2,     3,     5,     1,     1,
       3,     1,     2,     3,     5,     6,     1,     1,     3,     1,
       1,     1,     1,     1,     4,     2,     0,     2,     2,     3,
       1,     3,     1,     3,     1,     2,     1,     3,     1,     2,
       2,     2,     2,     4,     1,     3,     3,     3,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     5,     5,     1,     0,     3,     2,     4,     4,
       6,     3,     1,     2,     0,     2,     2,     3,     1,     3,
       2,     2,     2,     1,     1,     0,     1,     1,     1,     1,
       1,     2,     2,     2,     4,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
      11,     0,     0,     0,    11,     4,     5,    11,     0,     0,
       0,     1,     2,     3,     9,   465,     0,   464,   240,   241,
       0,     0,   461,   462,   463,   299,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   204,     0,     0,
     261,     0,   234,   217,   218,   219,   220,   221,   229,   224,
     239,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   233,   230,   222,
     223,   231,   232,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    20,     0,     0,     0,    24,    25,    26,
      21,    22,    27,    28,     0,    23,     0,     8,    79,    81,
      82,     0,    92,    93,    94,    83,    84,    85,    86,   143,
     145,   161,   164,   166,   169,   174,   177,   180,   182,   184,
     186,   188,   191,   192,   194,     0,   200,   193,   207,   210,
     211,   243,     0,   216,   243,     0,   228,   226,   236,   238,
     212,   243,   245,   253,   246,   247,   248,   251,   252,   249,
     250,   268,   270,   271,   272,   269,   302,   303,   304,   305,
     306,   307,   235,   345,   337,   344,   343,   342,   339,   341,
     338,   340,   255,   256,   257,   254,   237,     0,     0,   264,
     267,     0,     0,     0,     0,     0,     0,    95,    97,   258,
       0,     0,     0,   108,   110,     0,     0,   132,     0,     0,
       0,   190,   189,   205,   206,     0,     0,     0,   225,     0,
       0,   352,   351,   350,   346,   260,     0,   275,     0,     0,
       0,     0,     0,   347,     0,     0,   359,   355,     0,   358,
     370,   367,   362,     0,   366,   371,     0,     0,     0,   369,
       0,   361,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   410,   411,   413,   412,
     414,   415,   416,   417,   418,   419,     0,   424,     0,   432,
     434,     0,   443,   444,    36,   403,   404,   405,   406,   407,
     408,   409,    29,    12,    13,    14,    15,    18,    19,    16,
      17,     0,     0,     0,     0,     0,    91,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   201,     0,     0,   213,   215,   214,   227,
     242,     0,     6,     0,     0,   265,   315,     0,   310,   313,
       0,   321,     0,     0,   203,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     354,   353,   259,     0,     0,     0,     0,     0,   300,   348,
     349,     0,   356,     0,   363,     0,     0,   368,   360,     0,
       0,     0,    31,    32,   373,    47,    64,    65,    38,    37,
       0,     0,     0,     0,    42,    43,     0,     0,     0,     0,
       0,     0,     0,    50,     0,    59,     0,     0,   421,   420,
       0,     0,   434,   433,   442,    80,    90,   116,     0,     0,
       0,     0,     0,   142,   144,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
       0,     0,     0,   445,   445,   445,   445,   162,   376,   380,
     382,   384,   386,   388,   165,   167,   168,   170,   171,   172,
     173,   176,   175,   178,   179,     0,     0,     0,     0,   198,
       0,   208,   209,     0,     0,     0,   266,   314,   309,   320,
     202,     0,     0,     0,     0,     0,     0,    96,     0,     0,
       0,   107,   115,   109,     0,     0,   131,   129,   130,     0,
       0,     0,     0,   195,     0,   273,   289,   308,     0,     0,
     318,     0,   372,     0,     0,   319,   262,   263,     0,     0,
      46,     0,     0,     0,     0,    35,     0,     0,     0,     0,
      75,     0,     0,    48,     0,    51,     0,    58,   427,   425,
     425,   436,     0,   438,   431,     0,     0,   435,   123,   117,
     120,     0,    88,     0,    89,     0,     0,   385,   446,   447,
     448,   449,   450,   389,   390,   391,   392,     0,   163,     0,
     375,     0,     0,     0,     0,   181,   328,   333,   334,   336,
     183,   185,   325,   187,   199,     0,     7,    10,    98,     0,
     326,     0,     0,     0,     0,     0,     0,   111,     0,     0,
       0,   133,     0,     0,     0,   138,     0,   196,   197,     0,
       0,     0,     0,   357,   364,     0,     0,    30,    39,    40,
      33,    34,    44,    45,    41,     0,    63,     0,    77,     0,
      71,    67,     0,     0,     0,     0,    54,    55,    49,    52,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   437,   440,   441,   124,   125,   126,     0,   118,    87,
     387,   445,   460,   379,   394,     0,     0,     0,     0,     0,
       0,   400,   401,   402,   378,   377,   381,   383,     0,     0,
     332,   331,   330,   327,   324,   244,   106,   100,     0,    99,
     102,     0,     0,     0,   112,   113,     0,   134,     0,     0,
       0,   137,     0,   279,   284,   276,   291,   296,   295,   301,
       0,   297,   298,   293,   290,   294,   292,   312,     0,   317,
       0,   365,   323,     0,    61,     0,    62,    76,     0,    73,
      69,    74,    70,    66,     0,    53,    57,   428,   451,   452,
     453,     0,   429,     0,   423,   422,   439,     0,   121,   119,
     393,   374,     0,     0,     0,   458,   459,   398,   399,   329,
     335,   101,   104,     0,   103,   114,     0,     0,     0,     0,
       0,     0,     0,     0,   311,   316,   322,    60,    78,    72,
      68,    56,     0,     0,   426,   127,   128,     0,   455,   456,
     457,   395,   396,   397,   105,     0,   139,   136,     0,   141,
     282,   281,   277,   280,   283,   287,   286,   278,   285,   288,
     274,   430,   454,   122,     0,     0,     0,   135,   140
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     3,     4,     5,     6,     7,     8,    80,    81,   283,
      82,    83,    84,    85,    86,    87,    88,    89,   406,   634,
      90,    91,    92,   545,   412,    93,    94,    95,    96,   539,
     540,   641,    97,   349,    99,   100,   101,   102,   103,   187,
     188,   494,   192,   104,   193,   194,   297,   298,   559,   560,
     666,   758,   105,   196,   197,   106,   614,   615,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   336,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   181,   150,   151,   152,   373,   715,   781,   782,
     620,   725,   153,   154,   726,   155,   156,   157,   350,   158,
     159,   160,   161,   591,   496,   585,   693,   586,   587,   162,
     163,   164,   165,   166,   167,   168,   228,   169,   240,   170,
     233,   171,   238,   229,   234,   521,   395,   457,   580,   458,
     459,   460,   461,   462,   463,   684,   274,   275,   276,   277,
     278,   658,   419,   279,   423,   422,   552,   557,   280,   281,
     573,   657,   801,   767,   578,   172,   173,   174,   175,   176
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -593
static const short int yypact[] =
{
     -48,  -111,   -57,    87,   -78,  -593,  -593,  -593,   591,   128,
     285,  -593,  -593,  -593,   -69,  -593,   765,  -593,  -593,  -593,
       9,    93,  -593,  -593,  -593,  -593,   196,   307,   283,   330,
     -11,   306,   306,   234,   244,  2335,  2335,  2492,  2492,   -12,
    -593,    77,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,
    -593,    55,   249,   272,   309,   922,   358,   265,   298,   316,
     320,   378,   -25,   124,   142,   323,   340,  -593,  -593,  -593,
    -593,  -593,  -593,   392,   392,   347,   349,   296,   193,   322,
     252,   252,   252,  -593,   252,   252,   252,  -593,  -593,  -593,
    -593,  -593,  -593,  -593,   252,  -593,   252,  -593,   376,  -593,
    -593,   411,  -593,  -593,  -593,  -593,  -593,  -593,   383,   386,
    -593,   474,   360,   312,   366,    83,   293,  -593,   401,   391,
     420,   414,  -593,  -593,  -593,    68,  -593,  -593,   353,  -593,
    -593,  -593,    77,  -593,  -593,    77,  -593,  -593,  -593,  -593,
    -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,
    -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,
    -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,
    -593,  -593,  -593,  -593,  -593,  -593,  -593,   -60,   458,  -593,
    -593,    11,  1079,  1236,  1393,   294,   550,   547,  -593,  -593,
     576,   548,   556,   565,  -593,   618,    48,  -593,    62,  2178,
    2178,  -593,  -593,   353,   353,   494,   496,  2178,  -593,   515,
     517,  -593,  -593,  -593,  -593,  -593,    41,  -593,  2178,  2178,
    2178,  2178,   612,  -593,   519,   520,  -593,  -593,    65,  -593,
    -593,  -593,  -593,   247,  -593,  -593,  2178,  2178,   521,  -593,
     522,  -593,  2178,  2178,   626,   325,   634,   342,   326,    12,
     -75,   604,   642,   643,    30,    39,  -593,  -593,  -593,  -593,
    -593,  -593,  -593,  -593,  -593,  -593,    54,  -593,   207,  -593,
    -593,   641,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,
    -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,
    -593,  2178,  2178,  2178,   607,   608,  -593,   314,   619,  2335,
    2335,  2335,  2335,  2335,  2335,  2335,  2335,  2335,  2335,  2335,
    2335,  2335,  2335,  2335,  2335,  2335,    46,  2335,  2335,  2335,
    2335,  2335,  2335,  2335,  2335,  2335,  2335,  2335,   589,   598,
     599,   600,  1550,  -593,  2492,  2492,   581,  -593,   581,  -593,
     581,   654,  -593,   634,  2178,  -593,  -593,    74,  -593,   376,
     527,  -593,    75,   632,  -593,    15,   283,    26,   656,  2178,
     -11,    60,   306,  2178,  2178,   255,   279,  2178,  2178,   216,
    -593,  -593,  -593,   120,   230,   232,   235,   236,  -593,  -593,
    -593,   392,  -593,   392,  -593,   238,   246,  -593,  -593,   253,
     256,   579,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,
     622,   634,   529,   533,  -593,  -593,   639,   658,   667,   126,
     661,   493,   634,   638,   664,   638,   641,    34,  -593,  -593,
     641,   641,  -593,   260,  -593,  -593,  -593,  -593,  2178,   636,
    2178,   645,  2178,   386,  -593,  -593,  -593,  -593,  -593,  -593,
    -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,
      46,  2178,   299,   327,   327,   327,   327,   445,   495,   498,
     492,  -593,  -593,  -593,   449,   366,   366,    83,    83,    83,
      83,   293,   293,  -593,  -593,   104,   104,   392,   392,  -593,
     257,  -593,  -593,  2178,   252,   252,  -593,  -593,  -593,  -593,
    -593,  2178,   648,   104,   -19,   652,    18,  -593,  2178,   650,
      47,  -593,  -593,  -593,  2178,   671,  -593,  -593,  -593,   655,
     651,   259,   273,  -593,   592,  -593,  -593,  -593,   586,   587,
    -593,   596,  -593,   -41,   588,  -593,  -593,  -593,   634,   487,
    -593,   634,   634,   387,     2,  -593,   715,   -28,   104,    -2,
    -593,   640,   590,   638,   634,   690,   644,   690,   213,  -593,
    -593,  -593,   280,  -593,   260,   207,   207,  -593,   510,   691,
    -593,  2178,  -593,  2178,  -593,   610,   276,  -593,  -593,  -593,
    -593,  -593,  -593,  -593,  -593,  -593,  -593,  2335,  -593,    46,
     302,    46,    46,   613,   615,  -593,   223,  -593,  -593,  -593,
    -593,  -593,   570,  -593,  -593,    43,  -593,  -593,  -593,   725,
    -593,  2178,   700,  2178,  2178,    14,   709,  -593,  2178,  2178,
     711,  -593,  2178,   111,   -15,  -593,  2178,  -593,  -593,   179,
     331,  1707,  1864,  -593,  -593,   637,  2021,  -593,  -593,  -593,
    -593,  -593,  -593,  -593,  -593,  2178,  -593,    33,   696,  2178,
    -593,  -593,   -22,   717,   -21,   104,  -593,  -593,   690,  -593,
     634,   634,   641,  2335,  2335,  2335,  2335,   541,   281,   288,
     641,  -593,  -593,  -593,  -593,  -593,   710,  2178,   691,  -593,
    -593,   327,    83,   498,  -593,   740,  2335,   263,   263,   341,
     341,  -593,  -593,  -593,  -593,  -593,   492,  -593,   646,   649,
    -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  2178,  -593,
    -593,  2178,   728,  2178,  -593,  -593,  2178,  -593,   759,   735,
      98,  -593,   713,  -593,  -593,  -593,  -593,  -593,  -593,  -593,
     769,  -593,  -593,  -593,  -593,  -593,  -593,  -593,   653,  -593,
     278,  -593,  -593,   282,  -593,  2178,  -593,  -593,   290,  -593,
    -593,  -593,  -593,  -593,    -5,  -593,   638,   263,    83,    83,
      83,   310,  -593,   736,  -593,  -593,  -593,   528,   729,  -593,
    -593,  -593,   -26,   187,   580,  -593,  -593,  -593,  -593,  -593,
    -593,  -593,  -593,  2178,  -593,  -593,   730,  2178,  2178,   782,
    2178,   275,   129,   669,  -593,  -593,  -593,  -593,  -593,  -593,
    -593,   690,   577,  2335,  -593,  -593,  -593,   634,  -593,  -593,
    -593,  -593,  -593,  -593,  -593,   104,  -593,  -593,   758,  -593,
    -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,
    -593,  -593,    83,  -593,   761,  2178,  2178,  -593,  -593
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -593,  -593,  -593,   790,   793,  -593,   791,  -593,  -593,   -56,
    -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,
    -593,  -593,  -593,  -411,  -593,  -593,  -593,  -593,  -593,  -593,
     156,  -584,  -593,    -6,   -16,  -593,  -593,   699,  -593,  -593,
     446,   308,  -313,  -593,  -593,   447,  -593,   509,   250,   141,
    -593,  -593,  -593,   780,   451,  -593,  -593,   200,  -593,  -593,
     516,   524,   546,  -593,   499,   261,  -299,   258,   287,  -593,
    -593,  -593,   554,  -593,  -593,  -593,  -593,   694,  -593,   564,
     271,  -593,  -593,  -593,  -593,  -593,  -593,  -593,   199,  -593,
    -593,  -593,   -31,  -593,  -308,  -593,  -285,  -593,  -281,  -593,
    -593,  -277,  -593,  -593,   201,  -593,  -593,  -593,  -593,  -593,
    -593,  -164,  -593,  -593,  -593,  -593,  -593,  -593,   204,  -593,
    -593,  -593,  -593,   344,  -350,  -459,  -593,  -593,   145,  -460,
    -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,  -593,   764,
    -593,   766,  -593,   753,   756,   448,  -249,   380,  -593,  -593,
     262,   254,   303,   382,  -593,  -593,   264,  -593,  -593,  -593,
    -593,   286,  -414,  -593,   415,    69,  -593,  -593,  -593,  -593,
    -442,  -592,    76,   158,  -593,  -593,   165,  -593,  -270,   -54
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short int yytable[] =
{
     180,   424,    98,   550,   547,   413,   415,   500,   453,   230,
     235,   505,   574,   575,   576,   588,   588,   590,   601,   235,
     230,   467,   468,   469,   470,   284,   285,   190,   286,   287,
     288,   454,   643,   588,   600,   455,   723,   394,   289,   456,
     290,   613,   495,   635,   499,   344,   394,   324,   325,   216,
      16,   701,   491,    17,   492,   604,   400,   492,   740,   493,
     743,   404,   209,   401,    22,    23,    24,   498,     9,   210,
     205,   206,   493,   416,   735,   291,   624,   291,   588,   642,
      15,    10,   362,   493,    29,   763,   764,    11,   609,    18,
      19,   405,   223,   416,   485,   282,   362,   504,    27,   381,
      77,   625,   363,   338,     2,   341,   639,    15,   291,   291,
     340,    78,   639,   639,    15,   644,   364,   493,   640,    79,
      64,   342,   207,   695,   739,   742,   514,    15,   345,   639,
       1,    73,   648,   778,     2,   177,   779,   645,   815,   717,
     718,   789,   453,   182,   453,    15,   548,    40,   636,   708,
     551,   553,   530,   588,   709,   792,   324,   325,   372,   710,
     790,   816,   450,   543,   536,   454,   191,   454,   410,   455,
     417,   455,   211,   456,   191,   456,   347,   414,   352,   736,
     451,   602,   382,   606,   637,   588,   744,   610,    50,    51,
      52,    53,    54,   365,   366,   402,   403,   813,   818,   191,
     191,   369,   332,   191,   411,   798,   799,   800,   549,   487,
     489,   191,   374,   375,   376,   377,    51,    52,    53,    54,
      62,    63,    64,    51,    52,    53,    54,   183,   418,   760,
     385,   386,   191,    73,    74,   226,   389,   390,   452,   515,
     208,   227,   516,   537,   583,   584,   420,    62,    63,    64,
     291,   583,   584,   231,    62,    63,    64,   721,   722,   232,
      73,    74,   817,   639,   291,   538,   291,    73,    74,   291,
     291,   453,   291,   453,   453,   425,   426,   427,   672,   627,
     291,   383,   630,   631,   810,   717,   718,   291,   737,   291,
     291,   291,   702,   291,   454,   649,   454,   454,   455,   178,
     455,   455,   456,    16,   456,   456,    17,   291,   811,   713,
     291,   714,   291,   291,   660,   753,   291,    22,    23,    24,
     353,   186,   753,   421,   291,   354,   480,   522,   486,   522,
     184,   337,   185,   555,   339,   791,   556,    29,   189,   794,
     716,   717,   718,   502,   195,   588,   824,   507,   508,   430,
     199,   513,    25,   719,   748,   749,   750,   751,   294,   295,
     200,   511,   512,   217,   384,   517,   212,   518,   326,   327,
     519,   520,   509,   524,   254,   255,   793,   762,   317,   318,
     319,   525,   747,   324,   325,   690,   691,   692,   526,   213,
     756,   527,   594,   222,   617,    15,   510,   661,   754,   218,
      40,   745,   746,   721,   722,   755,   812,   282,   618,   639,
     291,   671,   558,   785,   562,   450,   564,   786,   798,   799,
     800,   589,   589,   589,   589,   788,   214,   652,   596,   597,
     334,   335,   219,   451,   244,   320,   321,   322,   323,   589,
     653,   654,   655,   656,   299,   566,   292,    28,   300,    56,
     220,    30,   720,   293,   221,   294,   295,   236,    61,   721,
     722,   392,   393,   674,   328,   639,   245,   246,   247,   248,
     249,   250,   251,   252,   237,   598,   329,   595,   396,   397,
     253,   242,   607,   243,   589,   330,   398,   399,   611,   675,
     653,   654,   655,   656,   822,   676,   677,   678,   331,   679,
     680,   681,   682,   683,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   318,   319,   268,   269,
     270,   271,   272,   273,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   628,   629,   268,   269,
     270,   271,   272,   273,   343,   558,   316,   669,   823,   568,
     569,   570,   571,   572,   632,   633,   664,   665,   355,   589,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   765,   766,   795,   796,   465,
     466,   356,   471,   472,   357,   697,   358,   699,   700,   201,
     202,   589,   704,   705,    15,    16,   707,   359,    17,   360,
     712,   203,   204,    18,    19,   481,   482,    20,    21,    22,
      23,    24,    25,   473,   474,    26,   730,   814,   819,   734,
     733,    27,   592,   592,   662,   663,   361,    28,   367,    29,
     368,    30,   370,   738,   371,   378,   379,   380,   387,   388,
     391,   394,   407,    31,    32,   408,    33,   409,    17,    34,
     428,   558,   429,   475,   432,   476,   477,   478,    35,    36,
     483,   484,   488,   490,   501,   528,   534,   531,    37,    38,
     529,   532,    39,   533,   535,   541,   542,   544,   546,   561,
     563,   577,   771,   582,   579,   772,   599,   774,   581,   603,
     775,   608,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,   612,    56,
     616,   613,   619,   623,    57,    58,    59,    60,    61,   787,
     621,   622,   626,   638,   650,   667,   646,   670,   647,   688,
     651,   689,   694,   696,    62,    63,    64,   698,    65,    66,
      67,    68,    69,    70,    71,    72,   703,    73,    74,    75,
      76,   589,   706,   493,   731,   536,   752,   804,   757,    23,
      77,   806,   807,   769,   809,   773,   770,   776,    15,    16,
     777,    78,    17,   780,   783,   416,   803,    18,    19,    79,
     797,    20,    21,    22,    23,    24,    25,   805,   784,    26,
     808,   820,   821,   825,    12,    27,   826,    13,    14,   741,
     296,    28,   497,    29,   605,    30,   431,   503,   759,   827,
     828,   668,   198,   506,   711,   433,   464,    31,    32,   333,
      33,   724,   593,    34,   434,   728,   224,   241,   225,   239,
     565,   523,    35,    36,   567,   686,   659,   554,   768,   802,
     761,   673,    37,    38,   685,     0,    39,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,     0,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,   179,    56,     0,   687,     0,     0,    57,    58,
      59,    60,    61,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    62,    63,
      64,     0,    65,    66,    67,    68,    69,    70,    71,    72,
       0,    73,    74,    75,    76,    15,    16,     0,     0,    17,
       0,     0,     0,     0,    18,    19,     0,     0,    20,    21,
      22,    23,    24,    25,     0,     0,    26,     0,     0,     0,
       0,     0,    27,     0,     0,     0,     0,     0,    28,     0,
      29,     0,    30,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    31,    32,     0,    33,     0,     0,
      34,     0,     0,     0,     0,     0,     0,     0,     0,    35,
      36,     0,     0,     0,     0,     0,     0,     0,     0,    37,
      38,     0,     0,    39,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,   215,
      56,     0,     0,     0,     0,    57,    58,    59,    60,    61,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    62,    63,    64,     0,    65,
      66,    67,    68,    69,    70,    71,    72,     0,    73,    74,
      75,    76,    15,    16,     0,     0,    17,     0,     0,     0,
       0,    18,    19,     0,     0,    20,    21,    22,    23,    24,
      25,     0,     0,    26,     0,     0,     0,     0,     0,    27,
       0,     0,     0,     0,     0,    28,     0,    29,     0,    30,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    32,     0,    33,     0,     0,    34,     0,     0,
       0,     0,     0,     0,     0,     0,    35,    36,     0,     0,
       0,     0,     0,     0,     0,     0,    37,    38,     0,     0,
      39,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,     0,    56,     0,     0,
       0,     0,    57,    58,    59,    60,    61,     0,     0,     0,
       0,     0,     0,     0,   346,     0,     0,     0,     0,     0,
       0,     0,    62,    63,    64,     0,    65,    66,    67,    68,
      69,    70,    71,    72,     0,    73,    74,    75,    76,    15,
      16,     0,     0,    17,     0,     0,     0,     0,    18,    19,
       0,     0,    20,    21,    22,    23,    24,    25,     0,     0,
      26,     0,     0,     0,     0,     0,    27,     0,     0,     0,
       0,     0,    28,     0,    29,     0,    30,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
       0,    33,     0,     0,    34,     0,     0,     0,     0,     0,
       0,     0,     0,    35,    36,     0,     0,     0,     0,     0,
       0,     0,     0,    37,    38,     0,     0,    39,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,     0,    56,     0,     0,     0,     0,    57,
      58,    59,    60,    61,     0,     0,     0,     0,     0,     0,
       0,   348,     0,     0,     0,     0,     0,     0,     0,    62,
      63,    64,     0,    65,    66,    67,    68,    69,    70,    71,
      72,     0,    73,    74,    75,    76,    15,    16,     0,     0,
      17,     0,     0,     0,     0,    18,    19,     0,     0,    20,
      21,    22,    23,    24,    25,     0,     0,    26,     0,     0,
       0,     0,     0,    27,     0,     0,     0,     0,     0,    28,
       0,    29,     0,    30,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,     0,    33,     0,
       0,    34,     0,     0,     0,     0,     0,     0,     0,     0,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
      37,    38,     0,     0,    39,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
       0,    56,     0,     0,     0,     0,    57,    58,    59,    60,
      61,     0,     0,     0,     0,     0,     0,     0,   351,     0,
       0,     0,     0,     0,     0,     0,    62,    63,    64,     0,
      65,    66,    67,    68,    69,    70,    71,    72,     0,    73,
      74,    75,    76,    15,    16,     0,     0,    17,     0,     0,
       0,     0,    18,    19,     0,     0,    20,    21,    22,    23,
      24,    25,     0,     0,    26,     0,     0,     0,     0,     0,
      27,     0,     0,     0,     0,     0,    28,     0,    29,     0,
      30,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    31,    32,     0,    33,     0,     0,    34,     0,
       0,     0,     0,     0,     0,     0,     0,    35,    36,     0,
       0,     0,     0,     0,     0,     0,     0,    37,    38,     0,
       0,    39,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,     0,    56,     0,
       0,     0,     0,    57,    58,    59,    60,    61,     0,     0,
       0,     0,     0,     0,     0,   479,     0,     0,     0,     0,
       0,     0,     0,    62,    63,    64,     0,    65,    66,    67,
      68,    69,    70,    71,    72,     0,    73,    74,    75,    76,
      15,    16,     0,     0,    17,     0,     0,     0,     0,    18,
      19,     0,     0,    20,    21,    22,    23,    24,    25,     0,
       0,    26,     0,     0,     0,     0,     0,    27,     0,     0,
       0,     0,     0,    28,     0,    29,     0,    30,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    31,
      32,     0,    33,     0,     0,    34,     0,     0,     0,     0,
       0,     0,     0,     0,    35,    36,     0,     0,     0,     0,
       0,     0,     0,     0,    37,    38,     0,     0,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,     0,    56,     0,     0,     0,     0,
      57,    58,    59,    60,    61,     0,     0,     0,     0,     0,
       0,     0,   727,     0,     0,     0,     0,     0,     0,     0,
      62,    63,    64,     0,    65,    66,    67,    68,    69,    70,
      71,    72,     0,    73,    74,    75,    76,    15,    16,     0,
       0,    17,     0,     0,     0,     0,    18,    19,     0,     0,
      20,    21,    22,    23,    24,    25,     0,     0,    26,     0,
       0,     0,     0,     0,    27,     0,     0,     0,     0,     0,
      28,     0,    29,     0,    30,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    31,    32,     0,    33,
       0,     0,    34,     0,     0,     0,     0,     0,     0,     0,
       0,    35,    36,     0,     0,     0,     0,     0,     0,     0,
       0,    37,    38,     0,     0,    39,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,     0,    56,     0,     0,     0,     0,    57,    58,    59,
      60,    61,     0,     0,     0,     0,     0,     0,     0,   729,
       0,     0,     0,     0,     0,     0,     0,    62,    63,    64,
       0,    65,    66,    67,    68,    69,    70,    71,    72,     0,
      73,    74,    75,    76,    15,    16,     0,     0,    17,     0,
       0,     0,     0,    18,    19,     0,     0,    20,    21,    22,
      23,    24,    25,     0,     0,    26,     0,     0,     0,     0,
       0,    27,     0,     0,     0,     0,     0,    28,     0,    29,
       0,    30,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,     0,    33,     0,     0,    34,
       0,     0,     0,     0,     0,     0,     0,     0,    35,    36,
       0,     0,     0,     0,     0,     0,     0,     0,    37,    38,
       0,     0,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,     0,    56,
       0,     0,     0,     0,    57,    58,    59,    60,    61,     0,
       0,     0,     0,     0,     0,     0,   732,     0,     0,     0,
       0,     0,     0,     0,    62,    63,    64,     0,    65,    66,
      67,    68,    69,    70,    71,    72,     0,    73,    74,    75,
      76,    15,    16,     0,     0,    17,     0,     0,     0,     0,
      18,    19,     0,     0,    20,    21,    22,    23,    24,    25,
       0,     0,    26,     0,     0,     0,     0,     0,    27,     0,
       0,     0,     0,     0,    28,     0,    29,     0,    30,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      31,    32,     0,    33,     0,     0,    34,     0,     0,     0,
       0,     0,     0,     0,     0,    35,    36,     0,     0,     0,
       0,     0,     0,     0,     0,    37,    38,     0,     0,    39,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,     0,    56,     0,     0,     0,
       0,    57,    58,    59,    60,    61,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    62,    63,    64,     0,    65,    66,    67,    68,    69,
      70,    71,    72,     0,    73,    74,    75,    76,    15,    16,
       0,     0,    17,     0,     0,     0,     0,    18,    19,     0,
       0,    20,    21,    22,    23,    24,    25,     0,     0,    26,
       0,     0,     0,     0,     0,    27,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    35,    36,     0,     0,     0,     0,     0,     0,
       0,     0,    37,    38,     0,     0,    39,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,     0,    56,     0,     0,     0,     0,    57,    58,
      59,    60,    61,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    62,    63,
      64,     0,    65,    66,    67,    68,    69,    70,    71,    72,
       0,    73,    74,    75,    76,    15,    16,     0,     0,    17,
       0,     0,     0,     0,    18,    19,     0,     0,    20,    21,
      22,    23,    24,    25,     0,     0,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,     0,
      56,     0,     0,     0,     0,    57,    58,    59,    60,    61,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    62,    63,    64,     0,    65,
      66,    67,    68,    69,    70,    71,    72,     0,    73,    74,
      75,    76
};

static const short int yycheck[] =
{
      16,   271,     8,   417,   415,   254,   255,   357,   316,    63,
      64,   361,   454,   455,   456,   475,   476,   476,    37,    73,
      74,   320,   321,   322,   323,    81,    82,    38,    84,    85,
      86,   316,    34,   493,   493,   316,   620,     7,    94,   316,
      96,    56,   355,    41,   357,    34,     7,    73,    74,    55,
       4,    37,    37,     7,    39,    37,    44,    39,   642,    57,
     644,   136,     7,    51,    18,    19,    20,    41,   179,    14,
      82,    83,    57,    39,    41,    34,   117,    34,   538,   538,
       3,   138,    34,    57,    38,   677,   678,     0,    41,    12,
      13,   166,   117,    39,   343,   155,    34,    37,    30,    34,
     169,   142,    54,   134,   182,   165,   134,     3,    34,    34,
     141,   180,   134,   134,     3,   117,    54,    57,   146,   188,
     145,   177,   134,    80,   146,   146,     6,     3,   117,   134,
     178,   156,   543,    35,   182,     7,    38,   139,     9,    10,
      11,   146,   450,   134,   452,     3,   416,   101,   146,    38,
     420,   421,   401,   613,   613,   747,    73,    74,   117,   174,
     744,    32,   116,   412,    38,   450,   185,   452,   138,   450,
     116,   452,   117,   450,   185,   452,   182,   138,   184,   146,
     134,   494,   117,   496,   534,   645,   645,   500,   111,   112,
     113,   114,   115,   199,   200,   183,   184,   781,   782,   185,
     185,   207,   134,   185,   174,   231,   232,   233,   174,   135,
     135,   185,   218,   219,   220,   221,   112,   113,   114,   115,
     143,   144,   145,   112,   113,   114,   115,   134,   174,   671,
     236,   237,   185,   156,   157,   111,   242,   243,   192,   119,
      41,   117,   122,   117,   140,   141,    39,   143,   144,   145,
      34,   140,   141,   111,   143,   144,   145,   128,   129,   117,
     156,   157,   133,   134,    34,   139,    34,   156,   157,    34,
      34,   579,    34,   581,   582,   291,   292,   293,   577,   528,
      34,    34,   531,   532,     9,    10,    11,    34,   638,    34,
      34,    34,   605,    34,   579,   544,   581,   582,   579,    14,
     581,   582,   579,     4,   581,   582,     7,    34,    33,   130,
      34,   132,    34,    34,    34,    34,    34,    18,    19,    20,
      26,    38,    34,   116,    34,    31,   332,   381,   344,   383,
     134,   132,    25,    73,   135,   746,    76,    38,     8,   753,
       9,    10,    11,   359,    38,   805,   805,   363,   364,    35,
     116,   135,    21,    22,   653,   654,   655,   656,    44,    45,
     116,   367,   368,     5,   117,   135,   117,   135,    75,    76,
     135,   135,   117,   135,   181,   182,    66,   676,    66,    67,
      68,   135,   652,    73,    74,   162,   163,   164,   135,   117,
     660,   135,   135,    15,   135,     3,   117,   117,   117,   134,
     101,   650,   651,   128,   129,   117,   131,   155,   135,   134,
      34,   135,   428,   135,   430,   116,   432,   135,   231,   232,
     233,   475,   476,   477,   478,   135,   117,   214,   484,   485,
      77,    78,   134,   134,   138,    69,    70,    71,    72,   493,
     227,   228,   229,   230,    61,   451,    35,    36,    62,   118,
     134,    40,   121,    42,   134,    44,    45,   134,   127,   128,
     129,   136,   137,   161,    63,   134,   170,   171,   172,   173,
     174,   175,   176,   177,   134,   491,    85,   483,   136,   137,
     184,   134,   498,   134,   538,    65,   160,   161,   504,   187,
     227,   228,   229,   230,   793,   193,   194,   195,    84,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,    67,    68,   216,   217,
     218,   219,   220,   221,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,    49,    50,   216,   217,
     218,   219,   220,   221,    86,   561,   186,   563,   797,   222,
     223,   224,   225,   226,   167,   168,    46,    47,     8,   613,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   234,   235,    49,    50,   318,
     319,    34,   324,   325,     8,   601,    38,   603,   604,    35,
      36,   645,   608,   609,     3,     4,   612,    41,     7,    34,
     616,    37,    38,    12,    13,   334,   335,    16,    17,    18,
      19,    20,    21,   326,   327,    24,   622,   781,   782,   635,
     626,    30,   477,   478,   555,   556,     8,    36,   134,    38,
     134,    40,   117,   639,   117,    23,   117,   117,   117,   117,
      14,     7,    38,    52,    53,     3,    55,     4,     7,    58,
      43,   667,    44,    64,    35,    57,    57,    57,    67,    68,
      79,     7,   135,    31,     8,    86,     8,   138,    77,    78,
      48,   138,    81,    34,     7,    14,   183,    39,    14,    43,
      35,   236,   698,   191,   189,   701,    38,   703,   190,    37,
     706,    41,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,    37,   118,
      59,    56,   120,   117,   123,   124,   125,   126,   127,   735,
     134,   134,   134,     8,    34,    34,    86,   117,   138,   116,
      86,   116,   162,     8,   143,   144,   145,    37,   147,   148,
     149,   150,   151,   152,   153,   154,    37,   156,   157,   158,
     159,   805,    41,    57,   117,    38,   215,   773,    48,    19,
     169,   777,   778,   117,   780,    37,   117,     8,     3,     4,
      35,   180,     7,    60,     5,    39,   196,    12,    13,   188,
      51,    16,    17,    18,    19,    20,    21,    57,   135,    24,
       8,   122,   215,    35,     4,    30,    35,     4,     7,   643,
     101,    36,   356,    38,   496,    40,   297,   360,   667,   825,
     826,   561,    32,   362,   614,   299,   317,    52,    53,   125,
      55,   620,   478,    58,   300,   621,    62,    74,    62,    73,
     450,   383,    67,    68,   452,   581,   550,   422,   680,   763,
     675,   579,    77,    78,   580,    -1,    81,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,    -1,    -1,    -1,    -1,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,    -1,   582,    -1,    -1,   123,   124,
     125,   126,   127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,   144,
     145,    -1,   147,   148,   149,   150,   151,   152,   153,   154,
      -1,   156,   157,   158,   159,     3,     4,    -1,    -1,     7,
      -1,    -1,    -1,    -1,    12,    13,    -1,    -1,    16,    17,
      18,    19,    20,    21,    -1,    -1,    24,    -1,    -1,    -1,
      -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,    36,    -1,
      38,    -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    52,    53,    -1,    55,    -1,    -1,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,
      78,    -1,    -1,    81,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    -1,    -1,    -1,    -1,   123,   124,   125,   126,   127,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   143,   144,   145,    -1,   147,
     148,   149,   150,   151,   152,   153,   154,    -1,   156,   157,
     158,   159,     3,     4,    -1,    -1,     7,    -1,    -1,    -1,
      -1,    12,    13,    -1,    -1,    16,    17,    18,    19,    20,
      21,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,    30,
      -1,    -1,    -1,    -1,    -1,    36,    -1,    38,    -1,    40,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    53,    -1,    55,    -1,    -1,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    77,    78,    -1,    -1,
      81,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,    -1,   118,    -1,    -1,
      -1,    -1,   123,   124,   125,   126,   127,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   135,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   143,   144,   145,    -1,   147,   148,   149,   150,
     151,   152,   153,   154,    -1,   156,   157,   158,   159,     3,
       4,    -1,    -1,     7,    -1,    -1,    -1,    -1,    12,    13,
      -1,    -1,    16,    17,    18,    19,    20,    21,    -1,    -1,
      24,    -1,    -1,    -1,    -1,    -1,    30,    -1,    -1,    -1,
      -1,    -1,    36,    -1,    38,    -1,    40,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    53,
      -1,    55,    -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    77,    78,    -1,    -1,    81,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,    -1,   118,    -1,    -1,    -1,    -1,   123,
     124,   125,   126,   127,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   135,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,
     144,   145,    -1,   147,   148,   149,   150,   151,   152,   153,
     154,    -1,   156,   157,   158,   159,     3,     4,    -1,    -1,
       7,    -1,    -1,    -1,    -1,    12,    13,    -1,    -1,    16,
      17,    18,    19,    20,    21,    -1,    -1,    24,    -1,    -1,
      -1,    -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,    36,
      -1,    38,    -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    52,    53,    -1,    55,    -1,
      -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      77,    78,    -1,    -1,    81,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
      -1,   118,    -1,    -1,    -1,    -1,   123,   124,   125,   126,
     127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   143,   144,   145,    -1,
     147,   148,   149,   150,   151,   152,   153,   154,    -1,   156,
     157,   158,   159,     3,     4,    -1,    -1,     7,    -1,    -1,
      -1,    -1,    12,    13,    -1,    -1,    16,    17,    18,    19,
      20,    21,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,
      30,    -1,    -1,    -1,    -1,    -1,    36,    -1,    38,    -1,
      40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    52,    53,    -1,    55,    -1,    -1,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,    78,    -1,
      -1,    81,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,    -1,   118,    -1,
      -1,    -1,    -1,   123,   124,   125,   126,   127,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   135,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   143,   144,   145,    -1,   147,   148,   149,
     150,   151,   152,   153,   154,    -1,   156,   157,   158,   159,
       3,     4,    -1,    -1,     7,    -1,    -1,    -1,    -1,    12,
      13,    -1,    -1,    16,    17,    18,    19,    20,    21,    -1,
      -1,    24,    -1,    -1,    -1,    -1,    -1,    30,    -1,    -1,
      -1,    -1,    -1,    36,    -1,    38,    -1,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,
      53,    -1,    55,    -1,    -1,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    77,    78,    -1,    -1,    81,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,    -1,   118,    -1,    -1,    -1,    -1,
     123,   124,   125,   126,   127,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   135,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     143,   144,   145,    -1,   147,   148,   149,   150,   151,   152,
     153,   154,    -1,   156,   157,   158,   159,     3,     4,    -1,
      -1,     7,    -1,    -1,    -1,    -1,    12,    13,    -1,    -1,
      16,    17,    18,    19,    20,    21,    -1,    -1,    24,    -1,
      -1,    -1,    -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,
      36,    -1,    38,    -1,    40,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    52,    53,    -1,    55,
      -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    77,    78,    -1,    -1,    81,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,    -1,   118,    -1,    -1,    -1,    -1,   123,   124,   125,
     126,   127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,   144,   145,
      -1,   147,   148,   149,   150,   151,   152,   153,   154,    -1,
     156,   157,   158,   159,     3,     4,    -1,    -1,     7,    -1,
      -1,    -1,    -1,    12,    13,    -1,    -1,    16,    17,    18,
      19,    20,    21,    -1,    -1,    24,    -1,    -1,    -1,    -1,
      -1,    30,    -1,    -1,    -1,    -1,    -1,    36,    -1,    38,
      -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    53,    -1,    55,    -1,    -1,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,    78,
      -1,    -1,    81,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,    -1,   118,
      -1,    -1,    -1,    -1,   123,   124,   125,   126,   127,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   135,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   143,   144,   145,    -1,   147,   148,
     149,   150,   151,   152,   153,   154,    -1,   156,   157,   158,
     159,     3,     4,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      12,    13,    -1,    -1,    16,    17,    18,    19,    20,    21,
      -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,    30,    -1,
      -1,    -1,    -1,    -1,    36,    -1,    38,    -1,    40,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    53,    -1,    55,    -1,    -1,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    67,    68,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    77,    78,    -1,    -1,    81,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,    -1,   118,    -1,    -1,    -1,
      -1,   123,   124,   125,   126,   127,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   143,   144,   145,    -1,   147,   148,   149,   150,   151,
     152,   153,   154,    -1,   156,   157,   158,   159,     3,     4,
      -1,    -1,     7,    -1,    -1,    -1,    -1,    12,    13,    -1,
      -1,    16,    17,    18,    19,    20,    21,    -1,    -1,    24,
      -1,    -1,    -1,    -1,    -1,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    77,    78,    -1,    -1,    81,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,    -1,   118,    -1,    -1,    -1,    -1,   123,   124,
     125,   126,   127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,   144,
     145,    -1,   147,   148,   149,   150,   151,   152,   153,   154,
      -1,   156,   157,   158,   159,     3,     4,    -1,    -1,     7,
      -1,    -1,    -1,    -1,    12,    13,    -1,    -1,    16,    17,
      18,    19,    20,    21,    -1,    -1,    24,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,    -1,
     118,    -1,    -1,    -1,    -1,   123,   124,   125,   126,   127,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   143,   144,   145,    -1,   147,
     148,   149,   150,   151,   152,   153,   154,    -1,   156,   157,
     158,   159
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short int yystos[] =
{
       0,   178,   182,   239,   240,   241,   242,   243,   244,   179,
     138,     0,   241,   242,   244,     3,     4,     7,    12,    13,
      16,    17,    18,    19,    20,    21,    24,    30,    36,    38,
      40,    52,    53,    55,    58,    67,    68,    77,    78,    81,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   118,   123,   124,   125,
     126,   127,   143,   144,   145,   147,   148,   149,   150,   151,
     152,   153,   154,   156,   157,   158,   159,   169,   180,   188,
     245,   246,   248,   249,   250,   251,   252,   253,   254,   255,
     258,   259,   260,   263,   264,   265,   266,   270,   271,   272,
     273,   274,   275,   276,   281,   290,   293,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     341,   342,   343,   350,   351,   353,   354,   355,   357,   358,
     359,   360,   367,   368,   369,   370,   371,   372,   373,   375,
     377,   379,   413,   414,   415,   416,   417,     7,    14,   117,
     272,   340,   134,   134,   134,    25,    38,   277,   278,     8,
      38,   185,   280,   282,   283,    38,   291,   292,   291,   116,
     116,   310,   310,   317,   317,    82,    83,   134,   326,     7,
      14,   117,   117,   117,   117,   117,   271,     5,   134,   134,
     134,   134,    15,   117,   377,   379,   111,   117,   374,   381,
     417,   111,   117,   378,   382,   417,   134,   134,   380,   382,
     376,   381,   134,   134,   138,   170,   171,   172,   173,   174,
     175,   176,   177,   184,   181,   182,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   216,   217,
     218,   219,   220,   221,   394,   395,   396,   397,   398,   401,
     406,   407,   155,   247,   247,   247,   247,   247,   247,   247,
     247,    34,    35,    42,    44,    45,   275,   284,   285,    61,
      62,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   186,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    63,    85,
      65,    84,   134,   315,    77,    78,   330,   326,   330,   326,
     330,   165,   247,    86,    34,   117,   135,   271,   135,   271,
     356,   135,   271,    26,    31,     8,    34,     8,    38,    41,
      34,     8,    34,    54,    54,   271,   271,   134,   134,   271,
     117,   117,   117,   344,   271,   271,   271,   271,    23,   117,
     117,    34,   117,    34,   117,   271,   271,   117,   117,   271,
     271,    14,   136,   137,     7,   384,   136,   137,   160,   161,
      44,    51,   183,   184,   136,   166,   256,    38,     3,     4,
     138,   174,   262,   384,   138,   384,    39,   116,   174,   400,
      39,   116,   403,   402,   416,   272,   272,   272,    43,    44,
      35,   285,    35,   298,   299,   300,   300,   300,   300,   300,
     300,   300,   300,   300,   300,   300,   300,   300,   300,   300,
     116,   134,   192,   332,   334,   336,   339,   385,   387,   388,
     389,   390,   391,   392,   302,   303,   303,   304,   304,   304,
     304,   305,   305,   306,   306,    64,    57,    57,    57,   135,
     271,   318,   318,    79,     7,   384,   272,   135,   135,   135,
      31,    37,    39,    57,   279,   280,   362,   278,    41,   280,
     362,     8,   272,   283,    37,   362,   292,   272,   272,   117,
     117,   271,   271,   135,     6,   119,   122,   135,   135,   135,
     135,   383,   417,   383,   135,   135,   135,   135,    86,    48,
     384,   138,   138,    34,     8,     7,    38,   117,   139,   267,
     268,    14,   183,   384,    39,   261,    14,   261,   416,   174,
     400,   416,   404,   416,   402,    73,    76,   405,   272,   286,
     287,    43,   272,    35,   272,   385,   271,   391,   222,   223,
     224,   225,   226,   408,   408,   408,   408,   236,   412,   189,
     386,   190,   191,   140,   141,   363,   365,   366,   367,   417,
     363,   361,   366,   361,   135,   271,   247,   247,   272,    38,
     363,    37,   280,    37,    37,   279,   280,   272,    41,    41,
     280,   272,    37,    56,   294,   295,    59,   135,   135,   120,
     348,   134,   134,   117,   117,   142,   134,   384,    49,    50,
     384,   384,   167,   168,   257,    41,   146,   362,     8,   134,
     146,   269,   363,    34,   117,   139,    86,   138,   261,   384,
      34,    86,   214,   227,   228,   229,   230,   409,   399,   399,
      34,   117,   403,   403,    46,    47,   288,    34,   286,   272,
     117,   135,   304,   388,   161,   187,   193,   194,   195,   197,
     198,   199,   200,   201,   393,   394,   389,   390,   116,   116,
     162,   163,   164,   364,   162,    80,     8,   272,    37,   272,
     272,    37,   280,    37,   272,   272,    41,   272,    38,   363,
     174,   295,   272,   130,   132,   345,     9,    10,    11,    22,
     121,   128,   129,   269,   342,   349,   352,   135,   356,   135,
     271,   117,   135,   271,   272,    41,   146,   362,   271,   146,
     269,   268,   146,   269,   363,   384,   384,   416,   304,   304,
     304,   304,   215,    34,   117,   117,   416,    48,   289,   287,
     408,   414,   304,   409,   409,   234,   235,   411,   411,   117,
     117,   272,   272,    37,   272,   272,     8,    35,    35,    38,
      60,   346,   347,     5,   135,   135,   135,   272,   135,   146,
     269,   261,   409,    66,   400,    49,    50,    51,   231,   232,
     233,   410,   410,   196,   272,    57,   272,   272,     8,   272,
       9,    33,   131,   269,   349,     9,    32,   133,   269,   349,
     122,   215,   304,   384,   363,    35,    35,   272,   272
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value, Location);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;
  (void) yylocationp;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");

# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;
  (void) yylocationp;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()
    ;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended. */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;
#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
  *++yylsp = yylloc;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, yylsp - yylen, yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 6:
#line 572 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(VersionDecl, (yylsp[-3]));

		if(!XPath2Utils::equals((yyvsp[-1].str),sz1_0))
			yyerror((yylsp[-2]), "This XQuery processor only supports version 1.0 of the specs [err:XQST0031]");
	}
    break;

  case 7:
#line 579 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(VersionDecl, (yylsp[-5]));

		if(!XPath2Utils::equals((yyvsp[-3].str),sz1_0))
			yyerror((yylsp[-4]), "This XQuery processor only supports version 1.0 of the specs [err:XQST0031]");
        bool bValidEnc=false;
        if(((yyvsp[-1].str)[0] >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A && (yyvsp[-1].str)[0] <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_Z) ||
           ((yyvsp[-1].str)[0] >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a && (yyvsp[-1].str)[0] <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z))
        {
            bValidEnc=true;
            int nLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((yyvsp[-1].str));
            for(int i=1;i<nLen;i++)
            {
                if(((yyvsp[-1].str)[i] >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A && (yyvsp[-1].str)[i] <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_Z) ||
                   ((yyvsp[-1].str)[i] >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a && (yyvsp[-1].str)[i] <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z) ||
                   ((yyvsp[-1].str)[i] >= XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0 && (yyvsp[-1].str)[i] <= XERCES_CPP_NAMESPACE_QUALIFIER chDigit_9) ||
                   (yyvsp[-1].str)[i] == XERCES_CPP_NAMESPACE_QUALIFIER chPeriod ||
                   (yyvsp[-1].str)[i] == XERCES_CPP_NAMESPACE_QUALIFIER chDash)
                {
                    continue;
                }
                bValidEnc=false;
                break;
            }
        }
        if(!bValidEnc)
          yyerror((yylsp[-1]), "The specified encoding does not conform to the definition of EncName [err:XQST0087]");
		// TODO: store the encoding somewhere
	}
    break;

  case 8:
#line 613 "../src/parser/XQParser.y"
    {
	}
    break;

  case 9:
#line 620 "../src/parser/XQParser.y"
    {
	}
    break;

  case 10:
#line 627 "../src/parser/XQParser.y"
    {
		if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((yyvsp[-1].str))==0)
			yyerror((yylsp[-1]), "The literal that specifies the namespace of a module must not be of zero length [err:XQST0088]");
		QP->_query->setIsLibraryModule();
		QP->_query->setModuleTargetNamespace((yyvsp[-1].str));
		CONTEXT->setNamespaceBinding((yyvsp[-3].str),(yyvsp[-1].str));
	}
    break;

  case 12:
#line 641 "../src/parser/XQParser.y"
    {
		if(QP->_flags.get(BIT_DECLARE_SECOND_STEP))
			yyerror((yylsp[-1]), "Prolog contains a setter declaration after a variable, function or option declaration");
    }
    break;

  case 13:
#line 646 "../src/parser/XQParser.y"
    {
		if(QP->_flags.get(BIT_DECLARE_SECOND_STEP))
			yyerror((yylsp[-1]), "Prolog contains an import declaration after a variable, function or option declaration");
    }
    break;

  case 14:
#line 651 "../src/parser/XQParser.y"
    {
		if(QP->_flags.get(BIT_DECLARE_SECOND_STEP))
			yyerror((yylsp[-1]), "Prolog contains a namespace declaration after a variable, function or option declaration");
    }
    break;

  case 15:
#line 656 "../src/parser/XQParser.y"
    {
		if(QP->_flags.get(BIT_DECLARE_SECOND_STEP))
			yyerror((yylsp[-1]), "Prolog contains a default namespace declaration after a variable, function or option declaration");
    }
    break;

  case 16:
#line 661 "../src/parser/XQParser.y"
    {
		QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    }
    break;

  case 17:
#line 665 "../src/parser/XQParser.y"
    {
		QP->_flags.set(BIT_DECLARE_SECOND_STEP);
		XQUserFunction* decl=(yyvsp[-1].functDecl);
		CONTEXT->addCustomFunction(decl);
		QP->_query->addFunction(decl);
	}
    break;

  case 18:
#line 672 "../src/parser/XQParser.y"
    {
		QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    }
    break;

  case 19:
#line 676 "../src/parser/XQParser.y"
    {
		QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
    break;

  case 30:
#line 712 "../src/parser/XQParser.y"
    {

          REJECT_NOT_XQUERY(NamespaceDecl, (yylsp[-4]));
          // if it has already bound, report an error
          if(QP->_namespaceDecls.containsKey((yyvsp[-2].str)))
            yyerror((yylsp[-2]), "Namespace prefix has already been bound to a namespace [err:XQST0033]");
          QP->_namespaceDecls.put((yyvsp[-2].str),NULL);
          CONTEXT->setNamespaceBinding((yyvsp[-2].str),(yyvsp[0].str));
		}
    break;

  case 31:
#line 726 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(BoundarySpaceDecl, (yylsp[-2]));

		    if(QP->_flags.get(BIT_BOUNDARY_SPECIFIED))
			    yyerror((yylsp[-2]), "Prolog contains more than one boundary space declaration [err:XQST0068]");
		    QP->_flags.set(BIT_BOUNDARY_SPECIFIED);
			CONTEXT->setPreserveBoundarySpace(true);
		}
    break;

  case 32:
#line 735 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(BoundarySpaceDecl, (yylsp[-2]));

		    if(QP->_flags.get(BIT_BOUNDARY_SPECIFIED))
			    yyerror((yylsp[-2]), "Prolog contains more than one boundary space declaration [err:XQST0068]");
		    QP->_flags.set(BIT_BOUNDARY_SPECIFIED);
			CONTEXT->setPreserveBoundarySpace(false);
		}
    break;

  case 33:
#line 748 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(DefaultNamespaceDecl, (yylsp[-4]));

		    if(QP->_flags.get(BIT_DEFAULTELEMENTNAMESPACE_SPECIFIED))
			    yyerror((yylsp[-4]), "Prolog contains more than one default element namespace declaration [err:XQST0066]");
		    QP->_flags.set(BIT_DEFAULTELEMENTNAMESPACE_SPECIFIED);
			CONTEXT->setDefaultElementAndTypeNS((yyvsp[0].str));
		}
    break;

  case 34:
#line 757 "../src/parser/XQParser.y"
    { 
      REJECT_NOT_XQUERY(DefaultNamespaceDecl, (yylsp[-4]));

		    if(QP->_flags.get(BIT_DEFAULTFUNCTIONNAMESPACE_SPECIFIED))
			    yyerror((yylsp[-4]), "Prolog contains more than one default function namespace declaration [err:XQST0066]");
		    QP->_flags.set(BIT_DEFAULTFUNCTIONNAMESPACE_SPECIFIED);
			CONTEXT->setDefaultFuncNS((yyvsp[0].str));
		}
    break;

  case 35:
#line 770 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(OptionDecl, (yylsp[-3]));

        // validate the QName
		QualifiedName qName((yyvsp[-1].str));
        const XMLCh* prefix=qName.getPrefix();
        if(prefix==NULL || *prefix==0)
			yyerror((yylsp[-1]), "The option name must have a prefix [err:XPST0081]");

		try
		{
      LOCATION((yylsp[-1]), loc);
			CONTEXT->getUriBoundToPrefix(prefix, &loc);
		}
		catch(NamespaceLookupException&)
		{
			yyerror((yylsp[-1]), "The option name is using an undefined namespace prefix [err:XPST0081]");
		}
      }
    break;

  case 36:
#line 794 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(FTOptionDecl, (yylsp[-1]));
      REJECT_NOT_FULLTEXT(FTOptionDecl, (yylsp[-1]));
	}
    break;

  case 37:
#line 803 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(OrderingModeDecl, (yylsp[-2]));

		if(QP->_flags.get(BIT_ORDERING_SPECIFIED))
			yyerror((yylsp[-2]), "Prolog contains more than one ordering mode declaration [err:XQST0065]");
		QP->_flags.set(BIT_ORDERING_SPECIFIED);
		CONTEXT->setNodeSetOrdering(StaticContext::ORDERING_ORDERED);
	}
    break;

  case 38:
#line 812 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(OrderingModeDecl, (yylsp[-2]));

		if(QP->_flags.get(BIT_ORDERING_SPECIFIED))
			yyerror((yylsp[-2]), "Prolog contains more than one ordering mode declaration [err:XQST0065]");
		QP->_flags.set(BIT_ORDERING_SPECIFIED);
		CONTEXT->setNodeSetOrdering(StaticContext::ORDERING_UNORDERED);
	}
    break;

  case 39:
#line 825 "../src/parser/XQParser.y"
    { 
    REJECT_NOT_XQUERY(EmptyOrderDecl, (yylsp[-4]));

		if(QP->_flags.get(BIT_EMPTYORDERING_SPECIFIED))
			yyerror((yylsp[-4]), "Prolog contains more than one empty ordering mode declaration [err:XQST0069]");
		QP->_flags.set(BIT_EMPTYORDERING_SPECIFIED);
		CONTEXT->setDefaultFLWOROrderingMode(StaticContext::FLWOR_ORDER_EMPTY_GREATEST);
	}
    break;

  case 40:
#line 834 "../src/parser/XQParser.y"
    { 
    REJECT_NOT_XQUERY(EmptyOrderDecl, (yylsp[-4]));

		if(QP->_flags.get(BIT_EMPTYORDERING_SPECIFIED))
			yyerror((yylsp[-4]), "Prolog contains more than one empty ordering mode declaration [err:XQST0069]");
		QP->_flags.set(BIT_EMPTYORDERING_SPECIFIED);
		CONTEXT->setDefaultFLWOROrderingMode(StaticContext::FLWOR_ORDER_EMPTY_LEAST);
	}
    break;

  case 41:
#line 847 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(CopyNamespacesDecl, (yylsp[-4]));

		if(QP->_flags.get(BIT_COPYNAMESPACE_SPECIFIED))
			yyerror((yylsp[-4]), "Prolog contains more than one copy namespace declaration [err:XQST0055]");
		QP->_flags.set(BIT_COPYNAMESPACE_SPECIFIED);
		CONTEXT->setPreserveNamespaces(XPath2Utils::equals((yyvsp[-2].str),szTrue));
		CONTEXT->setInheritNamespaces(XPath2Utils::equals((yyvsp[0].str),szTrue));
    }
    break;

  case 42:
#line 861 "../src/parser/XQParser.y"
    {
        (yyval.str) = szTrue;
	}
    break;

  case 43:
#line 865 "../src/parser/XQParser.y"
    {
        (yyval.str) = szFalse;
	}
    break;

  case 44:
#line 873 "../src/parser/XQParser.y"
    {
        (yyval.str) = szTrue;
	}
    break;

  case 45:
#line 877 "../src/parser/XQParser.y"
    {
        (yyval.str) = szFalse;
	}
    break;

  case 46:
#line 885 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(DefaultCollationDecl, (yylsp[-3]));

		    if(QP->_flags.get(BIT_COLLATION_SPECIFIED))
			    yyerror((yylsp[-3]), "Prolog contains more than one default collation declaration [err:XQST0038]");
		    QP->_flags.set(BIT_COLLATION_SPECIFIED);
            try
            {
              LOCATION((yylsp[0]), loc);
                CONTEXT->getCollation((yyvsp[0].str), &loc);
            }
            catch(ContextException&)
            {
			    yyerror((yylsp[0]), "The specified collation does not exist [err:XQST0038]");
            }
			CONTEXT->setDefaultCollation((yyvsp[0].str));
		}
    break;

  case 47:
#line 907 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(BaseURIDecl, (yylsp[-2]));

		    if(QP->_flags.get(BIT_BASEURI_SPECIFIED))
			    yyerror((yylsp[-2]), "Prolog contains more than one base URI declaration [err:XQST0032]");
		    QP->_flags.set(BIT_BASEURI_SPECIFIED);
			CONTEXT->setBaseURI((yyvsp[0].str));
		}
    break;

  case 48:
#line 920 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(SchemaImport, (yylsp[-3]));

			if(XPath2Utils::equals((yyvsp[-1].str), XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString))
				CONTEXT->setDefaultElementAndTypeNS((yyvsp[0].str));
			else if(XPath2Utils::equals((yyvsp[0].str), XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString))
				yyerror((yylsp[-3]), "A schema that has no target namespace cannot be bound to non-empty prefix [err:XQST0057]");
			else
				CONTEXT->setNamespaceBinding((yyvsp[-1].str),(yyvsp[0].str));
            try {
			  CONTEXT->addSchemaLocation((yyvsp[0].str),NULL);
            } catch(XQException& e) {
              if(e.getXQueryLine() == 0)
                e.setXQueryPosition(QP->_query->getFile(), (yylsp[-3]).first_line, (yylsp[-3]).first_column);
              throw e;
            }
		}
    break;

  case 49:
#line 938 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(SchemaImport, (yylsp[-4]));

			if(XPath2Utils::equals((yyvsp[-2].str), XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString))
				CONTEXT->setDefaultElementAndTypeNS((yyvsp[-1].str));
			else if(XPath2Utils::equals((yyvsp[-1].str), XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString))
				yyerror((yylsp[-4]), "A schema that has no target namespace cannot be bound to non-empty prefix [err:XQST0057]");
			else
				CONTEXT->setNamespaceBinding((yyvsp[-2].str),(yyvsp[-1].str));
            try {
			  CONTEXT->addSchemaLocation((yyvsp[-1].str),(yyvsp[0].stringList));
            } catch(XQException& e) {
              if(e.getXQueryLine() == 0)
                e.setXQueryPosition(QP->_query->getFile(), (yylsp[-4]).first_line, (yylsp[-4]).first_column);
              throw e;
            }
		}
    break;

  case 50:
#line 956 "../src/parser/XQParser.y"
    { 
      REJECT_NOT_XQUERY(SchemaImport, (yylsp[-2]));

            try {
			  CONTEXT->addSchemaLocation((yyvsp[0].str),NULL);
            } catch(XQException& e) {
              if(e.getXQueryLine() == 0)
                e.setXQueryPosition(QP->_query->getFile(), (yylsp[-2]).first_line, (yylsp[-2]).first_column);
              throw e;
            }
		}
    break;

  case 51:
#line 968 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(SchemaImport, (yylsp[-3]));

            try {
			  CONTEXT->addSchemaLocation((yyvsp[-1].str),(yyvsp[0].stringList));
            } catch(XQException& e) {
              if(e.getXQueryLine() == 0)
                e.setXQueryPosition(QP->_query->getFile(), (yylsp[-3]).first_line, (yylsp[-3]).first_column);
              throw e;
            }
		}
    break;

  case 52:
#line 983 "../src/parser/XQParser.y"
    {
		(yyval.stringList) = new (MEMMGR) VectorOfStrings(XQillaAllocator<const XMLCh*>(MEMMGR));
		(yyval.stringList)->push_back((yyvsp[0].str));
	}
    break;

  case 53:
#line 988 "../src/parser/XQParser.y"
    {
		(yyvsp[-2].stringList)->push_back((yyvsp[0].str));
		(yyval.stringList) = (yyvsp[-2].stringList);
	}
    break;

  case 54:
#line 997 "../src/parser/XQParser.y"
    {
			(yyval.str) = (yyvsp[-1].str);
		}
    break;

  case 55:
#line 1001 "../src/parser/XQParser.y"
    {
			(yyval.str) = (XMLCh*)XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
		}
    break;

  case 56:
#line 1009 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(ModuleImport, (yylsp[-6]));

		if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((yyvsp[-1].str))==0)
			yyerror((yylsp[-1]), "The literal that specifies the target namespace in a module import must not be of zero length [err:XQST0088]");
		CONTEXT->setNamespaceBinding((yyvsp[-3].str),(yyvsp[-1].str));
        try {
		  QP->_query->importModule(LANGUAGE,(yyvsp[-1].str),(yyvsp[0].stringList),CONTEXT);
        } catch(XQException& e) {
          if(e.getXQueryLine() == 0)
            e.setXQueryPosition(QP->_query->getFile(), (yylsp[-6]).first_line, (yylsp[-6]).first_column);
          throw e;
        }
	}
    break;

  case 57:
#line 1024 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(ModuleImport, (yylsp[-5]));

		if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((yyvsp[0].str))==0)
			yyerror((yylsp[0]), "The literal that specifies the target namespace in a module import must not be of zero length [err:XQST0088]");
		CONTEXT->setNamespaceBinding((yyvsp[-2].str),(yyvsp[0].str));
        try {
		  QP->_query->importModule(LANGUAGE,(yyvsp[0].str),NULL,CONTEXT);
        } catch(XQException& e) {
          if(e.getXQueryLine() == 0)
            e.setXQueryPosition(QP->_query->getFile(), (yylsp[-5]).first_line, (yylsp[-5]).first_column);
          throw e;
        }
	}
    break;

  case 58:
#line 1039 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(ModuleImport, (yylsp[-3]));

		if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((yyvsp[-1].str))==0)
			yyerror((yylsp[-1]), "The literal that specifies the target namespace in a module import must not be of zero length [err:XQST0088]");
        try {
		  QP->_query->importModule(LANGUAGE,(yyvsp[-1].str),(yyvsp[0].stringList),CONTEXT);
        } catch(XQException& e) {
          if(e.getXQueryLine() == 0)
            e.setXQueryPosition(QP->_query->getFile(), (yylsp[-3]).first_line, (yylsp[-3]).first_column);
          throw e;
        }
	}
    break;

  case 59:
#line 1053 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(ModuleImport, (yylsp[-2]));

		if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen((yyvsp[0].str))==0)
			yyerror((yylsp[0]), "The literal that specifies the target namespace in a module import must not be of zero length [err:XQST0088]");
        try {
		  QP->_query->importModule(LANGUAGE,(yyvsp[0].str),NULL,CONTEXT);
        } catch(XQException& e) {
          if(e.getXQueryLine() == 0)
            e.setXQueryPosition(QP->_query->getFile(), (yylsp[-2]).first_line, (yylsp[-2]).first_column);
          throw e;
        }
	}
    break;

  case 60:
#line 1071 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(VarDecl, (yylsp[-6]));

		XQGlobalVariable* var=WRAP((yylsp[-6]), new (MEMMGR) XQGlobalVariable((yyvsp[-3].str),(yyvsp[-2].sequenceType), WRAP((yylsp[0]), (yyvsp[0].astNode)),MEMMGR));
		QP->_query->addVariable(var);
	}
    break;

  case 61:
#line 1078 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(VarDecl, (yylsp[-5]));

		XQGlobalVariable* var=WRAP((yylsp[-5]), new (MEMMGR) XQGlobalVariable((yyvsp[-2].str),WRAP((yylsp[-5]), new (MEMMGR) SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR)),
                                                            WRAP((yylsp[0]), (yyvsp[0].astNode)),MEMMGR));
		QP->_query->addVariable(var);
	}
    break;

  case 62:
#line 1086 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(VarDecl, (yylsp[-5]));

		XQGlobalVariable* var=WRAP((yylsp[-5]), new (MEMMGR) XQGlobalVariable((yyvsp[-2].str),(yyvsp[-1].sequenceType),NULL,MEMMGR));
		QP->_query->addVariable(var);
	}
    break;

  case 63:
#line 1093 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(VarDecl, (yylsp[-4]));

		XQGlobalVariable* var=WRAP((yylsp[-4]), new (MEMMGR) XQGlobalVariable((yyvsp[-1].str),WRAP((yylsp[-4]), new (MEMMGR) SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR)),
                                                            NULL,MEMMGR));
		QP->_query->addVariable(var);
	}
    break;

  case 64:
#line 1105 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(ConstructionDecl, (yylsp[-2]));

		if(QP->_flags.get(BIT_CONSTRUCTION_SPECIFIED))
			yyerror((yylsp[-2]), "Prolog contains more than one construction mode declaration [err:XQST0067]");
		QP->_flags.set(BIT_CONSTRUCTION_SPECIFIED);
		CONTEXT->setConstructionMode(StaticContext::CONSTRUCTION_MODE_PRESERVE);
	}
    break;

  case 65:
#line 1114 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(ConstructionDecl, (yylsp[-2]));

		if(QP->_flags.get(BIT_CONSTRUCTION_SPECIFIED))
			yyerror((yylsp[-2]), "Prolog contains more than one construction mode declaration [err:XQST0067]");
		QP->_flags.set(BIT_CONSTRUCTION_SPECIFIED);
		CONTEXT->setConstructionMode(StaticContext::CONSTRUCTION_MODE_STRIP);
	}
    break;

  case 66:
#line 1128 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(FunctionDecl, (yylsp[-5]));

			(yyval.functDecl) = WRAP((yylsp[-5]), new (MEMMGR) XQUserFunction((yyvsp[-3].str),(yyvsp[-2].funcParams),WRAP((yylsp[0]), (yyvsp[0].astNode)),NULL, CONTEXT));
		}
    break;

  case 67:
#line 1134 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(FunctionDecl, (yylsp[-4]));

			(yyval.functDecl) = WRAP((yylsp[-4]), new (MEMMGR) XQUserFunction((yyvsp[-2].str),NULL,WRAP((yylsp[0]), (yyvsp[0].astNode)),NULL, CONTEXT));
		}
    break;

  case 68:
#line 1140 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(FunctionDecl, (yylsp[-6]));

			(yyval.functDecl) = WRAP((yylsp[-6]), new (MEMMGR) XQUserFunction((yyvsp[-4].str),(yyvsp[-3].funcParams),WRAP((yylsp[0]), (yyvsp[0].astNode)),(yyvsp[-1].sequenceType), CONTEXT));
		}
    break;

  case 69:
#line 1146 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(FunctionDecl, (yylsp[-5]));

			(yyval.functDecl) = WRAP((yylsp[-5]), new (MEMMGR) XQUserFunction((yyvsp[-3].str),NULL,WRAP((yylsp[0]), (yyvsp[0].astNode)),(yyvsp[-1].sequenceType), CONTEXT));
		}
    break;

  case 70:
#line 1152 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(FunctionDecl, (yylsp[-5]));

			(yyval.functDecl) = WRAP((yylsp[-5]), new (MEMMGR) XQUserFunction((yyvsp[-3].str),(yyvsp[-2].funcParams),NULL,NULL, CONTEXT));
		}
    break;

  case 71:
#line 1158 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(FunctionDecl, (yylsp[-4]));

			(yyval.functDecl) = WRAP((yylsp[-4]), new (MEMMGR) XQUserFunction((yyvsp[-2].str),NULL,NULL,NULL, CONTEXT));
		}
    break;

  case 72:
#line 1164 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(FunctionDecl, (yylsp[-6]));

			(yyval.functDecl) = WRAP((yylsp[-6]), new (MEMMGR) XQUserFunction((yyvsp[-4].str),(yyvsp[-3].funcParams),NULL,(yyvsp[-1].sequenceType), CONTEXT));
		}
    break;

  case 73:
#line 1170 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(FunctionDecl, (yylsp[-5]));

			(yyval.functDecl) = WRAP((yylsp[-5]), new (MEMMGR) XQUserFunction((yyvsp[-3].str),NULL,NULL,(yyvsp[-1].sequenceType), CONTEXT));
		}
    break;

  case 74:
#line 1180 "../src/parser/XQParser.y"
    {
			(yyvsp[-2].funcParams)->push_back((yyvsp[0].funcParam));
			(yyval.funcParams) = (yyvsp[-2].funcParams);
		}
    break;

  case 75:
#line 1185 "../src/parser/XQParser.y"
    {
			XQUserFunction::VectorOfFunctionParameters* paramList = new (MEMMGR) XQUserFunction::VectorOfFunctionParameters(XQillaAllocator<XQUserFunction::XQFunctionParameter*>(MEMMGR));
			paramList->push_back((yyvsp[0].funcParam));
			(yyval.funcParams) = paramList;
		}
    break;

  case 76:
#line 1195 "../src/parser/XQParser.y"
    {
			(yyval.funcParam) = new (MEMMGR) XQUserFunction::XQFunctionParameter((yyvsp[-1].str),(yyvsp[0].sequenceType),MEMMGR);
		}
    break;

  case 77:
#line 1199 "../src/parser/XQParser.y"
    {
			(yyval.funcParam) = new (MEMMGR) XQUserFunction::XQFunctionParameter((yyvsp[0].str),WRAP((yylsp[-1]), new (MEMMGR) SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR)) ,MEMMGR);
		}
    break;

  case 78:
#line 1207 "../src/parser/XQParser.y"
    { 
			(yyval.astNode) = WRAP((yylsp[-1]), (yyvsp[-1].astNode)); 
		}
    break;

  case 79:
#line 1215 "../src/parser/XQParser.y"
    {
	    QP->_query->setQueryBody((yyvsp[0].astNode));
	}
    break;

  case 80:
#line 1223 "../src/parser/XQParser.y"
    {
			ASTNode* prevExpr=(yyvsp[-2].astNode);
			if((unsigned int)prevExpr->getType()==ASTNode::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()==ASTNode::PARENTHESIZED)
			{
				((XQParenthesizedExpr *)prevExpr)->addItem((yyvsp[0].astNode));
				(yyval.astNode) = (yyvsp[-2].astNode);
			}
			else
			{
				XQParenthesizedExpr *dis = new (MEMMGR) XQParenthesizedExpr(MEMMGR);
				dis->addItem((yyvsp[-2].astNode));
				dis->addItem((yyvsp[0].astNode));
				(yyval.astNode) = dis;
			}
		}
    break;

  case 81:
#line 1241 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = (yyvsp[0].astNode);
		}
    break;

  case 87:
#line 1258 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = FNWRAP((yylsp[-4]), szFLWOR, new (MEMMGR) XQFLWOR((yyvsp[-4].variableBindingList), (yyvsp[-3].astNode), (yyvsp[-2].sort), WRAP((yylsp[-1]), (yyvsp[0].astNode)), MEMMGR));
		}
    break;

  case 88:
#line 1262 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = FNWRAP((yylsp[-3]), szFLWOR, new (MEMMGR) XQFLWOR((yyvsp[-3].variableBindingList), (yyvsp[-2].astNode), NULL, WRAP((yylsp[-1]), (yyvsp[0].astNode)), MEMMGR));
		}
    break;

  case 89:
#line 1266 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = FNWRAP((yylsp[-3]), szFLWOR, new (MEMMGR) XQFLWOR((yyvsp[-3].variableBindingList), NULL, (yyvsp[-2].sort), WRAP((yylsp[-1]), (yyvsp[0].astNode)), MEMMGR));
		}
    break;

  case 90:
#line 1270 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = FNWRAP((yylsp[-2]), szFLWOR, new (MEMMGR) XQFLWOR((yyvsp[-2].variableBindingList), NULL, NULL, WRAP((yylsp[-1]), (yyvsp[0].astNode)), MEMMGR));
		}
    break;

  case 91:
#line 1277 "../src/parser/XQParser.y"
    {
			(yyval.variableBindingList) = (yyvsp[-1].variableBindingList);
			for (VectorOfVariableBinding::iterator it = (yyvsp[0].variableBindingList)->begin(); it != (yyvsp[0].variableBindingList)->end (); ++it) 
				(yyvsp[-1].variableBindingList)->push_back(*it);
		}
    break;

  case 95:
#line 1294 "../src/parser/XQParser.y"
    {
			(yyval.variableBindingList) = (yyvsp[0].variableBindingList);
		}
    break;

  case 96:
#line 1301 "../src/parser/XQParser.y"
    {
			(yyval.variableBindingList)->push_back((yyvsp[0].variableBinding));
		}
    break;

  case 97:
#line 1305 "../src/parser/XQParser.y"
    {
			(yyval.variableBindingList) = new (MEMMGR) VectorOfVariableBinding(XQillaAllocator<XQVariableBinding*>(MEMMGR));
			(yyval.variableBindingList)->push_back((yyvsp[0].variableBinding));
		}
    break;

  case 98:
#line 1313 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-3]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,(yyvsp[-2].str),(yyvsp[0].astNode)));
		}
    break;

  case 99:
#line 1317 "../src/parser/XQParser.y"
    {
      // TBD FTScoreVar
			(yyval.variableBinding) = WRAP((yylsp[-4]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,(yyvsp[-3].str),(yyvsp[0].astNode)));
		}
    break;

  case 100:
#line 1322 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-4]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,(yyvsp[-3].str),(yyvsp[0].astNode),(yyvsp[-2].str)));
		}
    break;

  case 101:
#line 1326 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-5]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,(yyvsp[-4].str),(yyvsp[0].astNode),(yyvsp[-3].str)));
		}
    break;

  case 102:
#line 1330 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-4]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,(yyvsp[-3].str),(yyvsp[0].astNode),NULL,(yyvsp[-2].sequenceType)));
		}
    break;

  case 103:
#line 1334 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-5]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,(yyvsp[-4].str),(yyvsp[0].astNode),NULL,(yyvsp[-3].sequenceType)));
		}
    break;

  case 104:
#line 1338 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-5]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,(yyvsp[-4].str),(yyvsp[0].astNode),(yyvsp[-2].str),(yyvsp[-3].sequenceType)));
		}
    break;

  case 105:
#line 1342 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-6]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,(yyvsp[-5].str),(yyvsp[0].astNode),(yyvsp[-3].str),(yyvsp[-4].sequenceType)));
		}
    break;

  case 106:
#line 1350 "../src/parser/XQParser.y"
    { 
      REJECT_NOT_XQUERY(PositionalVar, (yylsp[-2]));

			(yyval.str) = (yyvsp[0].str); 
		}
    break;

  case 107:
#line 1360 "../src/parser/XQParser.y"
    {
      REJECT_NOT_FULLTEXT(FTScoreVar, (yylsp[-2]));

      (yyval.str) = (yyvsp[0].str);
    }
    break;

  case 108:
#line 1372 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(LetClause, (yylsp[-1]));

			(yyval.variableBindingList) = (yyvsp[0].variableBindingList);
		}
    break;

  case 109:
#line 1381 "../src/parser/XQParser.y"
    {
			(yyvsp[-2].variableBindingList)->push_back((yyvsp[0].variableBinding));
			(yyval.variableBindingList) = (yyvsp[-2].variableBindingList);
		}
    break;

  case 110:
#line 1386 "../src/parser/XQParser.y"
    {
			(yyval.variableBindingList) = new (MEMMGR) VectorOfVariableBinding(XQillaAllocator<XQVariableBinding*>(MEMMGR));
			(yyval.variableBindingList)->push_back((yyvsp[0].variableBinding));
		}
    break;

  case 111:
#line 1394 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-3]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::letBinding,(yyvsp[-2].str),(yyvsp[0].astNode)));
		}
    break;

  case 112:
#line 1398 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-4]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::letBinding,(yyvsp[-3].str),(yyvsp[0].astNode)));
		}
    break;

  case 113:
#line 1402 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-4]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::letBinding,(yyvsp[-3].str),(yyvsp[0].astNode),NULL,(yyvsp[-2].sequenceType)));
		}
    break;

  case 114:
#line 1406 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-5]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::letBinding,(yyvsp[-4].str),(yyvsp[0].astNode),NULL,(yyvsp[-3].sequenceType)));
		}
    break;

  case 115:
#line 1410 "../src/parser/XQParser.y"
    {
		}
    break;

  case 116:
#line 1417 "../src/parser/XQParser.y"
    { 
      REJECT_NOT_XQUERY(WhereClause, (yylsp[-1]));

			(yyval.astNode) = WRAP((yylsp[-1]), (yyvsp[0].astNode));
		}
    break;

  case 117:
#line 1427 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(OrderByClause, (yylsp[-2]));

			(yyval.sort)=new (MEMMGR) XQSort(XQSort::unstable,(yyvsp[0].sortSpecList));
		}
    break;

  case 118:
#line 1433 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(OrderByClause, (yylsp[-3]));

			(yyval.sort)=new (MEMMGR) XQSort(XQSort::stable,(yyvsp[0].sortSpecList));
		}
    break;

  case 119:
#line 1443 "../src/parser/XQParser.y"
    {
			(yyvsp[-2].sortSpecList)->push_back((yyvsp[0].sortSpec));
			(yyval.sortSpecList) = (yyvsp[-2].sortSpecList);
		}
    break;

  case 120:
#line 1448 "../src/parser/XQParser.y"
    {
			(yyval.sortSpecList) = new (MEMMGR) XQSort::VectorOfSortSpec(XQillaAllocator<XQSort::SortSpec*>(MEMMGR));
			(yyval.sortSpecList)->push_back((yyvsp[0].sortSpec));
		}
    break;

  case 121:
#line 1458 "../src/parser/XQParser.y"
    {
			(yyval.sortSpec) = WRAP((yylsp[-2]), new (MEMMGR) XQSort::SortSpec((yyvsp[-2].astNode),(yyvsp[-1].sortModifier)+(yyvsp[0].sortModifier),NULL));
		}
    break;

  case 122:
#line 1462 "../src/parser/XQParser.y"
    {
            try
            {
              LOCATION((yylsp[0]), loc);
                CONTEXT->getCollation((yyvsp[0].str), &loc);
            }
            catch(ContextException&)
            {
			    yyerror((yylsp[0]), "The specified collation does not exist [err:XQST0076]");
            }
			(yyval.sortSpec) = WRAP((yylsp[-4]), new (MEMMGR) XQSort::SortSpec((yyvsp[-4].astNode),(yyvsp[-3].sortModifier)+(yyvsp[-2].sortModifier),(yyvsp[0].str)));
		}
    break;

  case 123:
#line 1478 "../src/parser/XQParser.y"
    { (yyval.sortModifier) = XQSort::SortSpec::ascending; }
    break;

  case 124:
#line 1480 "../src/parser/XQParser.y"
    { (yyval.sortModifier) = XQSort::SortSpec::ascending; }
    break;

  case 125:
#line 1482 "../src/parser/XQParser.y"
    { (yyval.sortModifier) = XQSort::SortSpec::descending; }
    break;

  case 126:
#line 1487 "../src/parser/XQParser.y"
    { 
			switch(CONTEXT->getDefaultFLWOROrderingMode())
			{
			case StaticContext::FLWOR_ORDER_EMPTY_LEAST:
				(yyval.sortModifier) = XQSort::SortSpec::empty_least; break;
			case StaticContext::FLWOR_ORDER_EMPTY_GREATEST:
				(yyval.sortModifier) = XQSort::SortSpec::empty_greatest; break;
			}
		}
    break;

  case 127:
#line 1497 "../src/parser/XQParser.y"
    { (yyval.sortModifier) = XQSort::SortSpec::empty_greatest; }
    break;

  case 128:
#line 1499 "../src/parser/XQParser.y"
    { (yyval.sortModifier) = XQSort::SortSpec::empty_least; }
    break;

  case 129:
#line 1506 "../src/parser/XQParser.y"
    {
		(yyval.astNode) = new (MEMMGR) XQQuantified(XQQuantified::some, (yyvsp[-2].variableBindingList), WRAP((yylsp[-1]), (yyvsp[0].astNode)), MEMMGR);
	}
    break;

  case 130:
#line 1510 "../src/parser/XQParser.y"
    {
		(yyval.astNode) = new (MEMMGR) XQQuantified(XQQuantified::every, (yyvsp[-2].variableBindingList), WRAP((yylsp[-1]), (yyvsp[0].astNode)), MEMMGR);
	}
    break;

  case 131:
#line 1517 "../src/parser/XQParser.y"
    {
			(yyval.variableBindingList)->push_back((yyvsp[0].variableBinding));
		}
    break;

  case 132:
#line 1521 "../src/parser/XQParser.y"
    {
			(yyval.variableBindingList) = new (MEMMGR) VectorOfVariableBinding(XQillaAllocator<XQVariableBinding*>(MEMMGR));
			(yyval.variableBindingList)->push_back((yyvsp[0].variableBinding));
		}
    break;

  case 133:
#line 1529 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-3]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,(yyvsp[-2].str),(yyvsp[0].astNode)));
		}
    break;

  case 134:
#line 1533 "../src/parser/XQParser.y"
    {
			(yyval.variableBinding) = WRAP((yylsp[-4]), new (MEMMGR) XQVariableBinding(MEMMGR, XQVariableBinding::forBinding,(yyvsp[-3].str),(yyvsp[0].astNode),NULL,(yyvsp[-2].sequenceType)));
		}
    break;

  case 135:
#line 1541 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(TypeswitchExpr, (yylsp[-9]));

			XQTypeswitch::Clause* defClause=WRAP((yylsp[-4]), new (MEMMGR) XQTypeswitch::Clause(NULL,(yyvsp[0].astNode),MEMMGR->getPooledString((yyvsp[-2].str))));
			(yyval.astNode) = new (MEMMGR) XQTypeswitch( WRAP((yylsp[-9]), (yyvsp[-7].astNode)), (yyvsp[-5].clauseList), defClause, MEMMGR);
		}
    break;

  case 136:
#line 1548 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(TypeswitchExpr, (yylsp[-7]));

			XQTypeswitch::Clause* defClause=WRAP((yylsp[-2]), new (MEMMGR) XQTypeswitch::Clause(NULL,(yyvsp[0].astNode),NULL));
			(yyval.astNode) = new (MEMMGR) XQTypeswitch( WRAP((yylsp[-7]), (yyvsp[-5].astNode)), (yyvsp[-3].clauseList), defClause, MEMMGR);
		}
    break;

  case 137:
#line 1558 "../src/parser/XQParser.y"
    {
			(yyvsp[-1].clauseList)->push_back((yyvsp[0].clause));
			(yyval.clauseList)=(yyvsp[-1].clauseList);
		}
    break;

  case 138:
#line 1563 "../src/parser/XQParser.y"
    {
			(yyval.clauseList)=new (MEMMGR) XQTypeswitch::VectorOfClause(XQillaAllocator<XQTypeswitch::Clause*>(MEMMGR));
			(yyval.clauseList)->push_back((yyvsp[0].clause));
		}
    break;

  case 139:
#line 1572 "../src/parser/XQParser.y"
    { 
			(yyval.clause) = WRAP((yylsp[-3]), new (MEMMGR) XQTypeswitch::Clause((yyvsp[-2].sequenceType), (yyvsp[0].astNode), NULL));
		}
    break;

  case 140:
#line 1576 "../src/parser/XQParser.y"
    { 
			(yyval.clause) = WRAP((yylsp[-6]), new (MEMMGR) XQTypeswitch::Clause((yyvsp[-2].sequenceType), (yyvsp[0].astNode), MEMMGR->getPooledString((yyvsp[-4].str))));
		}
    break;

  case 141:
#line 1584 "../src/parser/XQParser.y"
    { 
	        (yyval.astNode) = WRAP((yylsp[-7]), new (MEMMGR) XQIf((yyvsp[-5].astNode), (yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR));
		}
    break;

  case 142:
#line 1592 "../src/parser/XQParser.y"
    {
			ASTNode* prevExpr=(yyvsp[-2].astNode);
			if((unsigned int)prevExpr->getType()==ASTNode::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()==ASTNode::OPERATOR && 
			   XPath2Utils::equals(((XQOperator*)prevExpr)->getOperatorName(),Or::name))
			{
				((Or*)prevExpr)->addArgument((yyvsp[0].astNode));
				(yyval.astNode) = prevExpr;
			}
			else
				(yyval.astNode) = WRAP((yylsp[-2]), new (MEMMGR) Or(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 144:
#line 1611 "../src/parser/XQParser.y"
    {
			ASTNode* prevExpr=(yyvsp[-2].astNode);
			if((unsigned int)prevExpr->getType()==ASTNode::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()==ASTNode::OPERATOR && 
			   XPath2Utils::equals(((XQOperator*)prevExpr)->getOperatorName(),And::name))
			{
				((And*)prevExpr)->addArgument((yyvsp[0].astNode));
				(yyval.astNode) = prevExpr;
			}
			else
				(yyval.astNode) = WRAP((yylsp[-2]), new (MEMMGR) And(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 146:
#line 1635 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) GeneralComp(GeneralComp::EQUAL,packageArgs((yyvsp[-2].astNode),(yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 147:
#line 1639 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) GeneralComp(GeneralComp::NOT_EQUAL,packageArgs((yyvsp[-2].astNode),(yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 148:
#line 1643 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) GeneralComp(GeneralComp::LESS_THAN,packageArgs((yyvsp[-2].astNode),(yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 149:
#line 1647 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) GeneralComp(GeneralComp::LESS_THAN_EQUAL,packageArgs((yyvsp[-2].astNode),(yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 150:
#line 1651 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) GeneralComp(GeneralComp::GREATER_THAN,packageArgs((yyvsp[-2].astNode),(yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 151:
#line 1655 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) GeneralComp(GeneralComp::GREATER_THAN_EQUAL,packageArgs((yyvsp[-2].astNode),(yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 152:
#line 1659 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) Equals(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 153:
#line 1663 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) NotEquals(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 154:
#line 1667 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) LessThan(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 155:
#line 1671 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) LessThanEqual(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 156:
#line 1675 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) GreaterThan(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 157:
#line 1679 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) GreaterThanEqual(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 158:
#line 1683 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) NodeComparison(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR), MEMMGR));
		}
    break;

  case 159:
#line 1687 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) OrderComparison(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR), true, MEMMGR));
		}
    break;

  case 160:
#line 1691 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) OrderComparison(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR), false,MEMMGR));
		}
    break;

  case 162:
#line 1700 "../src/parser/XQParser.y"
    {
        REJECT_NOT_FULLTEXT(FTContainsExpr, (yylsp[-2]));

        (yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) FTContains((yyvsp[-2].astNode), (yyvsp[0].ftselection), NULL, MEMMGR));
      }
    break;

  case 163:
#line 1706 "../src/parser/XQParser.y"
    {
        REJECT_NOT_FULLTEXT(FTContainsExpr, (yylsp[-3]));

        (yyval.astNode) = WRAP((yylsp[-2]), new (MEMMGR) FTContains((yyvsp[-3].astNode), (yyvsp[-1].ftselection), (yyvsp[0].astNode), MEMMGR));
      }
    break;

  case 165:
#line 1717 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) Range(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 167:
#line 1726 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) Plus(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 168:
#line 1730 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) Minus(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 170:
#line 1739 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) Multiply(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 171:
#line 1743 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) Divide(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 172:
#line 1747 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) IntegerDivide(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 173:
#line 1751 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) Mod(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 175:
#line 1760 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) Union(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 176:
#line 1764 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) Union(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 178:
#line 1773 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) Intersect(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 179:
#line 1777 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) Except(packageArgs((yyvsp[-2].astNode), (yyvsp[0].astNode), MEMMGR),MEMMGR));
		}
    break;

  case 181:
#line 1786 "../src/parser/XQParser.y"
    {
		(yyval.astNode) = WRAP((yylsp[-2]), new (MEMMGR) XQInstanceOf((yyvsp[-3].astNode),(yyvsp[0].sequenceType),MEMMGR));
	}
    break;

  case 183:
#line 1795 "../src/parser/XQParser.y"
    {
        XQTreatAs* treatAs = new (MEMMGR) XQTreatAs((yyvsp[-3].astNode),(yyvsp[0].sequenceType),MEMMGR);
        treatAs->setIsTreatAsStatement(true);
		(yyval.astNode) = WRAP((yylsp[-2]), treatAs);
	}
    break;

  case 185:
#line 1806 "../src/parser/XQParser.y"
    {
		(yyval.astNode) = WRAP((yylsp[-2]), new (MEMMGR) XQCastableAs((yyvsp[-3].astNode),(yyvsp[0].sequenceType),MEMMGR));
	}
    break;

  case 187:
#line 1815 "../src/parser/XQParser.y"
    {
		(yyval.astNode) = WRAP((yylsp[-3]), new (MEMMGR) XQCastAs((yyvsp[-3].astNode),(yyvsp[0].sequenceType),MEMMGR));
	}
    break;

  case 189:
#line 1824 "../src/parser/XQParser.y"
    {
			VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
			args.push_back((yyvsp[0].astNode));
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) UnaryMinus(/*positive*/false, args, MEMMGR));
		}
    break;

  case 190:
#line 1830 "../src/parser/XQParser.y"
    {
			VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
			args.push_back((yyvsp[0].astNode));
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) UnaryMinus(/*positive*/true, args, MEMMGR));
		}
    break;

  case 195:
#line 1851 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-3]), new (MEMMGR) XQValidate((yyvsp[-1].astNode),DocumentCache::VALIDATION_STRICT,MEMMGR));
		}
    break;

  case 196:
#line 1855 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-4]), new (MEMMGR) XQValidate((yyvsp[-1].astNode),DocumentCache::VALIDATION_LAX,MEMMGR));
		}
    break;

  case 197:
#line 1859 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-4]), new (MEMMGR) XQValidate((yyvsp[-1].astNode),DocumentCache::VALIDATION_STRICT,MEMMGR));
		}
    break;

  case 198:
#line 1867 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(ExtensionExpr, (yylsp[-2]));

		// we don't support any pragma
		yyerror((yylsp[-2]), "This pragma is not recognized, and no alternative expression is specified [err:XQST0079]");
	}
    break;

  case 199:
#line 1874 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(ExtensionExpr, (yylsp[-3]));

		// we don't support any pragma
		(yyval.astNode) = (yyvsp[-1].astNode);
	}
    break;

  case 202:
#line 1891 "../src/parser/XQParser.y"
    {
        // validate the QName
		QualifiedName qName((yyvsp[-2].str));
        const XMLCh* prefix=qName.getPrefix();
        if(prefix==NULL || *prefix==0)
			yyerror((yylsp[-2]), "The pragma name must have a prefix [err:XPST0081]");

		try
		{
			LOCATION((yylsp[-2]), loc);
			CONTEXT->getUriBoundToPrefix(prefix, &loc);
		}
		catch(NamespaceLookupException&)
		{
			yyerror((yylsp[-2]), "The pragma name is using an undefined namespace prefix [err:XPST0081]");
		}
      }
    break;

  case 203:
#line 1909 "../src/parser/XQParser.y"
    {
        // validate the QName
		QualifiedName qName((yyvsp[-1].str));
        const XMLCh* prefix=qName.getPrefix();
        if(prefix==NULL || *prefix==0)
			yyerror((yylsp[-1]), "The pragma name must have a prefix [err:XPST0081]");

		try
		{
			LOCATION((yylsp[-1]), loc);
			CONTEXT->getUriBoundToPrefix(prefix, &loc);
		}
		catch(NamespaceLookupException&)
		{
			yyerror((yylsp[-1]), "The pragma name is using an undefined namespace prefix [err:XPST0081]");
		}
      }
    break;

  case 204:
#line 1931 "../src/parser/XQParser.y"
    {
			XQNav *nav = new (MEMMGR) XQNav(MEMMGR);

      VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
      FunctionRoot *root = WRAP((yylsp[0]), new (MEMMGR) FunctionRoot(args, MEMMGR));

      SequenceType *documentNode = WRAP((yylsp[0]), new (MEMMGR)
        SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT)));

      XQTreatAs *treat = WRAP((yylsp[0]), new (MEMMGR) XQTreatAs(root, documentNode, MEMMGR));
      nav->addStepFront(treat);

			(yyval.astNode) = nav;
		}
    break;

  case 205:
#line 1946 "../src/parser/XQParser.y"
    {
			XQNav* nav=getNavigation((yyvsp[0].astNode),MEMMGR);

      VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
      FunctionRoot *root = WRAP((yylsp[-1]), new (MEMMGR) FunctionRoot(args, MEMMGR));

      SequenceType *documentNode = WRAP((yylsp[-1]), new (MEMMGR)
        SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT)));

      XQTreatAs *treat = WRAP((yylsp[-1]), new (MEMMGR) XQTreatAs(root, documentNode, MEMMGR));
      nav->addStepFront(treat);

			(yyval.astNode) = nav;
		}
    break;

  case 206:
#line 1961 "../src/parser/XQParser.y"
    {
			XQNav *nav = getNavigation((yyvsp[0].astNode),MEMMGR);

      NodeTest *step = new (MEMMGR) NodeTest();
      step->setTypeWildcard();
      step->setNameWildcard();
      step->setNamespaceWildcard();
      nav->addStepFront(WRAP((yylsp[-1]), new (MEMMGR) XQStep(XQStep::DESCENDANT_OR_SELF, step, MEMMGR)));

      VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
      FunctionRoot *root = WRAP((yylsp[-1]), new (MEMMGR) FunctionRoot(args, MEMMGR));

      SequenceType *documentNode = WRAP((yylsp[-1]), new (MEMMGR)
        SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT)));

      XQTreatAs *treat = WRAP((yylsp[-1]), new (MEMMGR) XQTreatAs(root, documentNode, MEMMGR));
      nav->addStepFront(treat);

			(yyval.astNode) = nav;
		}
    break;

  case 208:
#line 1987 "../src/parser/XQParser.y"
    {
			XQNav *nav = getNavigation((yyvsp[-2].astNode),MEMMGR);
			nav->addStep((yyvsp[0].astNode));
			(yyval.astNode) = nav;
		}
    break;

  case 209:
#line 1993 "../src/parser/XQParser.y"
    {
			XQNav *nav = getNavigation((yyvsp[-2].astNode),MEMMGR);

            NodeTest *step = new (MEMMGR) NodeTest();
            step->setTypeWildcard();
            step->setNameWildcard();
            step->setNamespaceWildcard();
            nav->addStep(WRAP((yylsp[-1]), new (MEMMGR) XQStep(XQStep::DESCENDANT_OR_SELF, step, MEMMGR)));
			nav->addStep((yyvsp[0].astNode));

			(yyval.astNode) = nav;
		}
    break;

  case 213:
#line 2017 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = XQPredicate::addPredicates((yyvsp[-1].astNode), (yyvsp[0].predicates));
		}
    break;

  case 214:
#line 2021 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = XQPredicate::addPredicates((yyvsp[-1].astNode), (yyvsp[0].predicates));
      (yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) XQDocumentOrder((yyval.astNode), MEMMGR));
		}
    break;

  case 215:
#line 2030 "../src/parser/XQParser.y"
    {
      if(!(yyvsp[0].nodeTest)->isNodeTypeSet()) {
        switch((yyvsp[-1].axis)) {
        case XQStep::NAMESPACE: (yyvsp[0].nodeTest)->setNodeType(Node::namespace_string); break;
        case XQStep::ATTRIBUTE: (yyvsp[0].nodeTest)->setNodeType(Node::attribute_string); break;
        default: (yyvsp[0].nodeTest)->setNodeType(Node::element_string); break;
        }
      }

			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) XQStep((yyvsp[-1].axis),(yyvsp[0].nodeTest),MEMMGR));
		}
    break;

  case 217:
#line 2053 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::CHILD;
		}
    break;

  case 218:
#line 2057 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::DESCENDANT;
		}
    break;

  case 219:
#line 2061 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::ATTRIBUTE;
		}
    break;

  case 220:
#line 2065 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::SELF;
		}
    break;

  case 221:
#line 2069 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::DESCENDANT_OR_SELF;
		}
    break;

  case 222:
#line 2073 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::FOLLOWING_SIBLING;
		}
    break;

  case 223:
#line 2077 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::FOLLOWING;
		}
    break;

  case 224:
#line 2081 "../src/parser/XQParser.y"
    {
        REJECT_NOT_XPATH(AxisNamespace, (yylsp[0]));
      (yyval.axis) = XQStep::NAMESPACE;
        }
    break;

  case 225:
#line 2090 "../src/parser/XQParser.y"
    {
      if(!(yyvsp[0].nodeTest)->isNodeTypeSet()) {
        (yyvsp[0].nodeTest)->setNodeType(Node::attribute_string);
      }

      (yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) XQStep(XQStep::ATTRIBUTE, (yyvsp[0].nodeTest), MEMMGR));
		}
    break;

  case 226:
#line 2098 "../src/parser/XQParser.y"
    {
      XQStep::Axis axis = XQStep::CHILD;
      SequenceType::ItemType *itemtype = (yyvsp[0].nodeTest)->getItemType();
      if(itemtype != 0 &&
         itemtype->getItemTestType() == SequenceType::ItemType::TEST_ATTRIBUTE) {
        axis = XQStep::ATTRIBUTE;
      }
      else if(!(yyvsp[0].nodeTest)->isNodeTypeSet()) {
        (yyvsp[0].nodeTest)->setNodeType(Node::element_string);
      }

      (yyval.astNode) = WRAP((yylsp[0]), new (MEMMGR) XQStep(axis, (yyvsp[0].nodeTest), MEMMGR));
		}
    break;

  case 227:
#line 2116 "../src/parser/XQParser.y"
    {
      if(!(yyvsp[0].nodeTest)->isNodeTypeSet()) {
        (yyvsp[0].nodeTest)->setNodeType(Node::element_string);
      }

      (yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) XQStep((yyvsp[-1].axis), (yyvsp[0].nodeTest), MEMMGR));
		}
    break;

  case 229:
#line 2133 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::PARENT;
		}
    break;

  case 230:
#line 2137 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::ANCESTOR;
		}
    break;

  case 231:
#line 2141 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::PRECEDING_SIBLING;
		}
    break;

  case 232:
#line 2145 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::PRECEDING;
		}
    break;

  case 233:
#line 2149 "../src/parser/XQParser.y"
    {
      (yyval.axis) = XQStep::ANCESTOR_OR_SELF;
		}
    break;

  case 234:
#line 2157 "../src/parser/XQParser.y"
    {
			NodeTest *step = new (MEMMGR) NodeTest();
			step->setNameWildcard();
			step->setNamespaceWildcard();
			step->setTypeWildcard();
			(yyval.astNode) = WRAP((yylsp[0]), new (MEMMGR) XQStep(XQStep::PARENT, step, MEMMGR));
		}
    break;

  case 237:
#line 2175 "../src/parser/XQParser.y"
    {
			NodeTest *step = new (MEMMGR) NodeTest();
      step->setNodePrefix((yyvsp[0].qName)->getPrefix());
			step->setNodeName((yyvsp[0].qName)->getName());
			(yyval.nodeTest) = step;
		}
    break;

  case 239:
#line 2187 "../src/parser/XQParser.y"
    {
			NodeTest *step = new (MEMMGR) NodeTest();
			step->setNameWildcard();
			step->setNamespaceWildcard();
			(yyval.nodeTest) = step;
		}
    break;

  case 240:
#line 2194 "../src/parser/XQParser.y"
    {
			NodeTest *step = new (MEMMGR) NodeTest();
      step->setNodePrefix((yyvsp[0].str));
			step->setNameWildcard();
			(yyval.nodeTest) = step;
		}
    break;

  case 241:
#line 2201 "../src/parser/XQParser.y"
    {
			NodeTest *step = new (MEMMGR) NodeTest();
			step->setNodeName((yyvsp[0].str));
			step->setNamespaceWildcard();
			(yyval.nodeTest) = step;
		}
    break;

  case 242:
#line 2212 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = XQPredicate::addPredicates((yyvsp[-1].astNode), (yyvsp[0].predicates));
		}
    break;

  case 243:
#line 2221 "../src/parser/XQParser.y"
    {
	        (yyval.predicates) = new (MEMMGR) VectorOfPredicates(MEMMGR);
		}
    break;

  case 244:
#line 2225 "../src/parser/XQParser.y"
    {
      XQPredicate *pred = WRAP((yylsp[-2]), new (MEMMGR) XQPredicate((yyvsp[-1].astNode), MEMMGR));
			(yyvsp[-3].predicates)->push_back(pred);
			(yyval.predicates) = (yyvsp[-3].predicates); 
		}
    break;

  case 258:
#line 2261 "../src/parser/XQParser.y"
    {
		    int nColon=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf((yyvsp[0].str),':');
			if(nColon!=-1)
  				(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) XQVariable((yyvsp[0].str),MEMMGR));
			else
				(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) XQVariable(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, (yyvsp[0].str),MEMMGR));
		}
    break;

  case 259:
#line 2273 "../src/parser/XQParser.y"
    { 
			ASTNode* prevExpr=(yyvsp[-1].astNode);
			if((unsigned int)prevExpr->getType()==ASTNode::DEBUG_HOOK)
				prevExpr=((XQDebugHook*)prevExpr)->m_impl;
			if(prevExpr->getType()!=ASTNode::PARENTHESIZED)
			{
				XQParenthesizedExpr *dis = new (MEMMGR) XQParenthesizedExpr(MEMMGR);
				dis->addItem((yyvsp[-1].astNode));
				(yyval.astNode) = WRAP((yylsp[-1]), dis);
			}
			else
				(yyval.astNode) = WRAP((yylsp[-1]), (yyvsp[-1].astNode));
		}
    break;

  case 260:
#line 2287 "../src/parser/XQParser.y"
    { 
			(yyval.astNode) = WRAP((yylsp[-1]), new (MEMMGR) XQSequence(MEMMGR));
		}
    break;

  case 261:
#line 2295 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[0]), new (MEMMGR) XQContextItem(MEMMGR));
		}
    break;

  case 262:
#line 2303 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(OrderedExpr, (yylsp[-3]));

			(yyval.astNode) = WRAP((yylsp[-3]), new (MEMMGR) XQOrderingChange(StaticContext::ORDERING_ORDERED, (yyvsp[-1].astNode), MEMMGR));
		}
    break;

  case 263:
#line 2313 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(UnorderedExpr, (yylsp[-3]));

			(yyval.astNode) = WRAP((yylsp[-3]), new (MEMMGR) XQOrderingChange(StaticContext::ORDERING_UNORDERED, (yyvsp[-1].astNode), MEMMGR));
		}
    break;

  case 264:
#line 2323 "../src/parser/XQParser.y"
    {
			VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
			(yyval.astNode) = FNWRAP((yylsp[-1]), (yyvsp[-1].str), new (MEMMGR) XQFunctionCall(new (MEMMGR) QualifiedName((yyvsp[-1].str), MEMMGR), args, MEMMGR));
		}
    break;

  case 265:
#line 2328 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = FNWRAP((yylsp[-2]), (yyvsp[-2].str), new (MEMMGR) XQFunctionCall(new (MEMMGR) QualifiedName((yyvsp[-2].str), MEMMGR), *(yyvsp[-1].itemList),MEMMGR));
		}
    break;

  case 266:
#line 2335 "../src/parser/XQParser.y"
    {
			(yyvsp[-2].itemList)->push_back((yyvsp[0].astNode));
			(yyval.itemList) = (yyvsp[-2].itemList);
		}
    break;

  case 267:
#line 2340 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			(yyval.itemList)->push_back((yyvsp[0].astNode));
		}
    break;

  case 268:
#line 2349 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(Constructor, (yylsp[0]));
    }
    break;

  case 269:
#line 2353 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(Constructor, (yylsp[0]));
    }
    break;

  case 273:
#line 2370 "../src/parser/XQParser.y"
    { 
			VectorOfASTNodes* content=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			(yyval.astNode) = WRAP((yylsp[-3]), new (MEMMGR) XQElementConstructor(
								new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										(yyvsp[-2].str), AnyAtomicType::QNAME),
										MEMMGR), 
								(yyvsp[-1].itemList), content, MEMMGR));
		}
    break;

  case 274:
#line 2382 "../src/parser/XQParser.y"
    { 
			if(!XPath2Utils::equals((yyvsp[-6].str),(yyvsp[-1].str)))
				yyerror((yylsp[-1]), "Close tag does not match open tag");
			// if we are requested to strip whitespace-only nodes, check if the last element content should be removed
			VectorOfASTNodes* elemContent=(yyvsp[-3].itemList);
			if(elemContent->size()>0)
			{
				if(elemContent->back()==0)
					elemContent->pop_back();
				else if(!CONTEXT->getPreserveBoundarySpace() &&
					    elemContent->back()->getType()==ASTNode::LITERAL)
				{
                    ASTNode* last=elemContent->back();
				    Item::Ptr litVal = ((XQLiteral*)last)->getItemConstructor()->createItem(CONTEXT);
					if(((AnyAtomicType*)(const Item*)litVal)->getPrimitiveTypeIndex()==AnyAtomicType::STRING &&
					   isAllSpaces(litVal->asString(CONTEXT)))
                    {
						elemContent->pop_back();
                        // special case: if the previous node was a CDATA, put it back
                        if(elemContent->size()>0 && elemContent->back()->getType()==ASTNode::DOM_CONSTRUCTOR &&
                           ((XQDOMConstructor*)elemContent->back())->getNodeType()==Node::cdata_string)
                        {
                            elemContent->push_back(last);
                        }
                    }
				}
			}
			(yyval.astNode) = WRAP((yylsp[-7]), new (MEMMGR) XQElementConstructor(
							  new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										(yyvsp[-6].str), AnyAtomicType::QNAME),
										MEMMGR), 
							  (yyvsp[-5].itemList), elemContent,MEMMGR));
		}
    break;

  case 275:
#line 2423 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
		}
    break;

  case 276:
#line 2427 "../src/parser/XQParser.y"
    {
            (yyval.itemList) = (yyvsp[-3].itemList);
            bool bInsertAtFront=false;
            if(XPath2Utils::equals((yyvsp[-2].str), XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLNSString) ||
               XERCES_CPP_NAMESPACE_QUALIFIER XMLString::startsWith((yyvsp[-2].str), XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLNSColonString))
            {
                // check that the value of an xmlns attribute is a stirng literal
                if((yyvsp[0].itemList)->size()>1 || ((yyvsp[0].itemList)->size()==1 && (yyvsp[0].itemList)->front()->getType()!=ASTNode::LITERAL))
                    yyerror((yylsp[0]), "Namespace URI of a namespace declaration must be a literal [err:XQST0022]");
                bInsertAtFront=true;
            }
            if((yyvsp[0].itemList)->size()>0 && (yyvsp[0].itemList)->back()==0)
                (yyvsp[0].itemList)->pop_back();
            ASTNode* attrItem=WRAP((yylsp[-2]), new (MEMMGR) XQAttributeConstructor(
                                            new (MEMMGR) XQLiteral(
                                                new (MEMMGR) AnyAtomicTypeConstructor(
                                                    XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										            (yyvsp[-2].str), AnyAtomicType::QNAME),
                                                MEMMGR), 
                                            (yyvsp[0].itemList),MEMMGR));
            if(bInsertAtFront)
                (yyval.itemList)->insert((yyval.itemList)->begin(), attrItem);
            else
                (yyval.itemList)->push_back(attrItem);
        }
    break;

  case 277:
#line 2459 "../src/parser/XQParser.y"
    { 
			(yyval.itemList) = (yyvsp[-1].itemList);
		}
    break;

  case 278:
#line 2463 "../src/parser/XQParser.y"
    { 
			(yyval.itemList) = (yyvsp[-1].itemList);
		}
    break;

  case 279:
#line 2472 "../src/parser/XQParser.y"
    { 
			(yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
		}
    break;

  case 280:
#line 2476 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
            if((yyval.itemList)->size()>0 && (yyval.itemList)->back()==0)
			    (yyval.itemList)->pop_back();
			(yyval.itemList)->push_back((yyvsp[0].astNode));
			(yyval.itemList)->push_back(0);
		}
    break;

  case 281:
#line 2484 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
			merge_strings(CONTEXT,(yyval.itemList),szQuote);
		}
    break;

  case 282:
#line 2489 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
			merge_strings(CONTEXT,(yyval.itemList),(yyvsp[0].str));
		}
    break;

  case 283:
#line 2494 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
			merge_strings(CONTEXT,(yyval.itemList),(yyvsp[0].str));
		}
    break;

  case 284:
#line 2504 "../src/parser/XQParser.y"
    { 
			(yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
		}
    break;

  case 285:
#line 2508 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
            if((yyval.itemList)->size()>0 && (yyval.itemList)->back()==0)
			    (yyval.itemList)->pop_back();
			(yyval.itemList)->push_back((yyvsp[0].astNode));
			(yyval.itemList)->push_back(0);
		}
    break;

  case 286:
#line 2516 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
			merge_strings(CONTEXT,(yyval.itemList),szApos);
		}
    break;

  case 287:
#line 2521 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
			merge_strings(CONTEXT,(yyval.itemList),(yyvsp[0].str));
		}
    break;

  case 288:
#line 2526 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
			merge_strings(CONTEXT,(yyval.itemList),(yyvsp[0].str));
		}
    break;

  case 289:
#line 2541 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR)); 
		}
    break;

  case 290:
#line 2545 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
			// if the last token was a string literal made of whitespace and
			// we are adding a node constructor, and the context tells us to strip whitespace, remove it
			if((yyval.itemList)->size()>0)
			{
				if((yyval.itemList)->back()==0)
					(yyval.itemList)->pop_back();
				else if(!CONTEXT->getPreserveBoundarySpace() && (yyval.itemList)->back()->getType()==ASTNode::LITERAL)
				{
					const XMLCh* lastString=NULL;
					Item::Ptr litVal = ((XQLiteral*)(yyval.itemList)->back())->getItemConstructor()->createItem(CONTEXT);
					if(((AnyAtomicType*)(const Item*)litVal)->getPrimitiveTypeIndex()==AnyAtomicType::STRING)
						lastString=litVal->asString(CONTEXT);
					if(lastString!=NULL && *lastString!=0 && isAllSpaces(lastString))
						(yyval.itemList)->pop_back();
				}
			}
			(yyval.itemList)->push_back((yyvsp[0].astNode));
		}
    break;

  case 291:
#line 2566 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
			if((yyval.itemList)->size()>0 && (yyval.itemList)->back() && (yyval.itemList)->back()->getType()==ASTNode::LITERAL)
			{
				XQLiteral *lit = (XQLiteral*)(yyval.itemList)->back();
				const XMLCh* string=lit->getItemConstructor()->createItem(CONTEXT)->asString(CONTEXT);
				string=XPath2Utils::concatStrings(string,(yyvsp[0].str),MEMMGR);

				AnyAtomicTypeConstructor *ic = new (MEMMGR)
    				AnyAtomicTypeConstructor(
								XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
								XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
					string, AnyAtomicType::STRING);
    			lit->setItemConstructor(ic);
			}
			else
			{
				if((yyval.itemList)->size()>0 && (yyval.itemList)->back()==0)
					(yyval.itemList)->pop_back();
    			AnyAtomicTypeConstructor *ic = new (MEMMGR)
      				AnyAtomicTypeConstructor(
								XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
								XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
						(yyvsp[0].str), AnyAtomicType::STRING);

				(yyval.itemList)->push_back(new (MEMMGR) XQLiteral(ic, MEMMGR));
			}
		}
    break;

  case 292:
#line 2595 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
			if((yyval.itemList)->size()>0 && (yyval.itemList)->back()==0)
				(yyval.itemList)->pop_back();

			// if the last token was a string literal made of whitespace and
			// we are adding a node constructor, and the context tells us to strip whitespace, remove it
			if(!CONTEXT->getPreserveBoundarySpace() && (yyval.itemList)->size()>0 && (yyval.itemList)->back()->getType()==ASTNode::LITERAL)
			{
				const XMLCh* lastString=NULL;
				Item::Ptr litVal = ((XQLiteral*)(yyval.itemList)->back())->getItemConstructor()->createItem(CONTEXT);
				if(((AnyAtomicType*)(const Item*)litVal)->getPrimitiveTypeIndex()==AnyAtomicType::STRING)
					lastString=litVal->asString(CONTEXT);
				if(lastString!=NULL && *lastString!=0 && isAllSpaces(lastString))
					(yyval.itemList)->pop_back();
			}
			(yyval.itemList)->push_back((yyvsp[0].astNode));
		}
    break;

  case 293:
#line 2614 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
			// if the last token was a string literal made of whitespace and
			// we are adding an enclosed expression, and the context tells us to strip whitespace, remove it
			if((yyval.itemList)->size()>0)
			{
				if((yyval.itemList)->back()==0)
					(yyval.itemList)->pop_back();
				else if(!CONTEXT->getPreserveBoundarySpace() && (yyval.itemList)->back()->getType()==ASTNode::LITERAL)
				{
					const XMLCh* lastString=NULL;
					Item::Ptr litVal = ((XQLiteral*)(yyval.itemList)->back())->getItemConstructor()->createItem(CONTEXT);
					if(((AnyAtomicType*)(const Item*)litVal)->getPrimitiveTypeIndex()==AnyAtomicType::STRING)
						lastString=litVal->asString(CONTEXT);
					if(lastString!=NULL && *lastString!=0 && isAllSpaces(lastString))
						(yyval.itemList)->pop_back();
				}
			}
			(yyval.itemList)->push_back((yyvsp[0].astNode));
			(yyval.itemList)->push_back(0);
		}
    break;

  case 294:
#line 2636 "../src/parser/XQParser.y"
    {
			(yyval.itemList) = (yyvsp[-1].itemList);
			if((yyval.itemList)->size()>0 && (yyval.itemList)->back()==0)
				(yyval.itemList)->pop_back();

    		AnyAtomicTypeConstructor *ic = new (MEMMGR)
      			AnyAtomicTypeConstructor(
							XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
							XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
					(yyvsp[0].str), AnyAtomicType::STRING);

			(yyval.itemList)->push_back(new (MEMMGR) XQLiteral(ic, MEMMGR));
		}
    break;

  case 297:
#line 2657 "../src/parser/XQParser.y"
    {
		(yyval.str) = szCurlyOpen;
	}
    break;

  case 298:
#line 2661 "../src/parser/XQParser.y"
    {
		(yyval.str) = szCurlyClose;
	}
    break;

  case 299:
#line 2670 "../src/parser/XQParser.y"
    {
			ASTNode *value = WRAP((yylsp[0]), new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										(yyvsp[0].str), AnyAtomicType::STRING),
										MEMMGR));
			(yyval.astNode) = WRAP((yylsp[0]), new (MEMMGR) XQCommentConstructor(value, MEMMGR));
		}
    break;

  case 300:
#line 2685 "../src/parser/XQParser.y"
    {
			if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareIString((yyvsp[-1].str), XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLString)==0)
			  yyerror((yylsp[-1]), "The target for the processing instruction must not be 'XML'");
			ASTNode *value = WRAP((yylsp[0]), new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										(yyvsp[0].str), AnyAtomicType::STRING),
										MEMMGR));
			(yyval.astNode) = WRAP((yylsp[-2]), new (MEMMGR) XQPIConstructor(
								      WRAP((yylsp[-1]), new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										(yyvsp[-1].str), AnyAtomicType::STRING),
										MEMMGR)), 
									  value, MEMMGR));
		}
    break;

  case 301:
#line 2709 "../src/parser/XQParser.y"
    {
			ASTNode *value = new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										(yyvsp[0].str), AnyAtomicType::STRING),
										MEMMGR);
			(yyval.astNode) = WRAP((yylsp[0]), new (MEMMGR) XQTextConstructor(/*isCDATA*/true, value, MEMMGR));
		}
    break;

  case 308:
#line 2738 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-3]), new (MEMMGR) XQDocumentConstructor(WRAP((yylsp[-1]), (yyvsp[-1].astNode)),MEMMGR));
		}
    break;

  case 309:
#line 2746 "../src/parser/XQParser.y"
    {
			VectorOfASTNodes* content=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			content->push_back(WRAP((yylsp[-1]), (yyvsp[-1].astNode)));
			(yyval.astNode) = WRAP((yylsp[-3]), new (MEMMGR) XQElementConstructor(
								  new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										(yyvsp[-3].str), AnyAtomicType::QNAME),
										MEMMGR), 
								  empty, content,MEMMGR));
		}
    break;

  case 310:
#line 2760 "../src/parser/XQParser.y"
    {
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			(yyval.astNode) = WRAP((yylsp[-2]), new (MEMMGR) XQElementConstructor(
								  new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										(yyvsp[-2].str), AnyAtomicType::QNAME),
										MEMMGR), 
								  empty, empty,MEMMGR));
		}
    break;

  case 311:
#line 2772 "../src/parser/XQParser.y"
    {
			VectorOfASTNodes* content=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			content->push_back(WRAP((yylsp[-1]), (yyvsp[-1].astNode)));
			(yyval.astNode) = WRAP((yylsp[-6]), new (MEMMGR) XQElementConstructor(
								  WRAP((yylsp[-4]), (yyvsp[-4].astNode)), 
								  empty, content, MEMMGR));
		}
    break;

  case 312:
#line 2781 "../src/parser/XQParser.y"
    {
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			(yyval.astNode) = WRAP((yylsp[-5]), new (MEMMGR) XQElementConstructor(
								  WRAP((yylsp[-3]), (yyvsp[-3].astNode)), 
								  empty, empty, MEMMGR));
		}
    break;

  case 314:
#line 2797 "../src/parser/XQParser.y"
    {
			VectorOfASTNodes* content=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			content->push_back(WRAP((yylsp[-1]), (yyvsp[-1].astNode)));
			(yyval.astNode) = WRAP((yylsp[-3]), new (MEMMGR) XQAttributeConstructor(
								      new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										(yyvsp[-3].str), AnyAtomicType::QNAME),
										MEMMGR), 
									  content,MEMMGR));
		}
    break;

  case 315:
#line 2810 "../src/parser/XQParser.y"
    {
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			(yyval.astNode) = WRAP((yylsp[-2]), new (MEMMGR) XQAttributeConstructor(
								      new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
										(yyvsp[-2].str), AnyAtomicType::QNAME),
										MEMMGR), 
									  empty,MEMMGR));
		}
    break;

  case 316:
#line 2822 "../src/parser/XQParser.y"
    {
			VectorOfASTNodes* content=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			content->push_back(WRAP((yylsp[-1]), (yyvsp[-1].astNode)));
			(yyval.astNode) = WRAP((yylsp[-6]), new (MEMMGR) XQAttributeConstructor(
									  WRAP((yylsp[-4]), (yyvsp[-4].astNode)), 
									  content, MEMMGR));
		}
    break;

  case 317:
#line 2830 "../src/parser/XQParser.y"
    {
			VectorOfASTNodes* empty=new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
			(yyval.astNode) = WRAP((yylsp[-5]), new (MEMMGR) XQAttributeConstructor(
									  WRAP((yylsp[-3]), (yyvsp[-3].astNode)), 
									  empty, MEMMGR));
		}
    break;

  case 318:
#line 2841 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-3]), new (MEMMGR) XQTextConstructor(/*isCDATA*/false, WRAP((yylsp[-1]), (yyvsp[-1].astNode)), MEMMGR));
		}
    break;

  case 319:
#line 2849 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-3]), new (MEMMGR) XQCommentConstructor(WRAP((yylsp[-1]), (yyvsp[-1].astNode)), MEMMGR));
		}
    break;

  case 320:
#line 2857 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-3]), new (MEMMGR) XQPIConstructor(
								      new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										(yyvsp[-3].str), AnyAtomicType::STRING),
										MEMMGR), 
									  WRAP((yylsp[-1]), (yyvsp[-1].astNode)), MEMMGR));
	  }
    break;

  case 321:
#line 2868 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-2]), new (MEMMGR) XQPIConstructor(
								      new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										(yyvsp[-2].str), AnyAtomicType::STRING),
										MEMMGR), 
									  new (MEMMGR) XQSequence(MEMMGR), MEMMGR));
	  }
    break;

  case 322:
#line 2879 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-6]), new (MEMMGR) XQPIConstructor(
									  WRAP((yylsp[-4]), (yyvsp[-4].astNode)), 
									  WRAP((yylsp[-1]), (yyvsp[-1].astNode)), MEMMGR));
	  }
    break;

  case 323:
#line 2885 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = WRAP((yylsp[-5]), new (MEMMGR) XQPIConstructor(
									  WRAP((yylsp[-3]), (yyvsp[-3].astNode)), 
									  new (MEMMGR) XQSequence(MEMMGR), MEMMGR));
	  }
    break;

  case 324:
#line 2895 "../src/parser/XQParser.y"
    {
			SequenceType* seq=WRAP((yylsp[-1]), new (MEMMGR) SequenceType());
			seq->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE,NULL,(yyvsp[-1].qName)));
			seq->setOccurrence(SequenceType::QUESTION_MARK);
			(yyval.sequenceType) = seq;
		}
    break;

  case 325:
#line 2902 "../src/parser/XQParser.y"
    {
			SequenceType* seq=WRAP((yylsp[0]), new (MEMMGR) SequenceType());
			seq->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE,NULL,(yyvsp[0].qName)));
			seq->setOccurrence(SequenceType::EXACTLY_ONE);
			(yyval.sequenceType) = seq;
		}
    break;

  case 326:
#line 2913 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(TypeDeclaration, (yylsp[-1]));

			(yyval.sequenceType) = (yyvsp[0].sequenceType);
		}
    break;

  case 327:
#line 2923 "../src/parser/XQParser.y"
    {
			SequenceType* seq=WRAP((yylsp[-1]), new (MEMMGR) SequenceType());
			seq->setItemType((yyvsp[-1].itemType));
			seq->setOccurrence((yyvsp[0].occurrence));
			(yyval.sequenceType) = seq;
		}
    break;

  case 328:
#line 2930 "../src/parser/XQParser.y"
    {
			SequenceType* seq=WRAP((yylsp[0]), new (MEMMGR) SequenceType());
			seq->setItemType((yyvsp[0].itemType));
			seq->setOccurrence(SequenceType::EXACTLY_ONE);
			(yyval.sequenceType) = seq;
		}
    break;

  case 329:
#line 2937 "../src/parser/XQParser.y"
    { 
			(yyval.sequenceType) = WRAP((yylsp[-2]), new (MEMMGR) SequenceType()); 
		}
    break;

  case 330:
#line 2946 "../src/parser/XQParser.y"
    { (yyval.occurrence) = SequenceType::STAR; }
    break;

  case 331:
#line 2948 "../src/parser/XQParser.y"
    { (yyval.occurrence) = SequenceType::PLUS; }
    break;

  case 332:
#line 2950 "../src/parser/XQParser.y"
    { (yyval.occurrence) = SequenceType::QUESTION_MARK; }
    break;

  case 333:
#line 2956 "../src/parser/XQParser.y"
    {
        (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE, NULL, (yyvsp[0].qName));
	}
    break;

  case 334:
#line 2960 "../src/parser/XQParser.y"
    {
		(yyval.itemType) = (yyvsp[0].nodeTest)->getItemType();
		(yyvsp[0].nodeTest)->setItemType(NULL);
	}
    break;

  case 335:
#line 2965 "../src/parser/XQParser.y"
    {
		(yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING);
	}
    break;

  case 336:
#line 2973 "../src/parser/XQParser.y"
    {
		(yyval.qName) = (yyvsp[0].qName);
	}
    break;

  case 346:
#line 3002 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_NODE));
		(yyval.nodeTest) = step;
	}
    break;

  case 347:
#line 3012 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT));
		(yyval.nodeTest) = step;
	}
    break;

  case 348:
#line 3018 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
		SequenceType::ItemType* elemTest=(yyvsp[-1].nodeTest)->getItemType();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT,elemTest->getName(),elemTest->getType()));
		elemTest->setName(NULL);
		elemTest->setType(NULL);
		(yyval.nodeTest) = step;
	}
    break;

  case 349:
#line 3027 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
		SequenceType::ItemType* elemTest=(yyvsp[-1].nodeTest)->getItemType();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT,elemTest->getName(),elemTest->getType()));
		elemTest->setName(NULL);
		elemTest->setType(NULL);
		(yyval.nodeTest) = step;
	}
    break;

  case 350:
#line 3040 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_TEXT));
		(yyval.nodeTest) = step;
	}
    break;

  case 351:
#line 3050 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
		step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_COMMENT));
		(yyval.nodeTest) = step;
	}
    break;

  case 352:
#line 3060 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_PI));
		(yyval.nodeTest) = step;
	}
    break;

  case 353:
#line 3066 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_PI, new (MEMMGR) QualifiedName((yyvsp[-1].str))));
		(yyval.nodeTest) = step;
	}
    break;

  case 354:
#line 3072 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_PI, new (MEMMGR) QualifiedName((yyvsp[-1].str))));
		(yyval.nodeTest) = step;
	}
    break;

  case 355:
#line 3082 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATTRIBUTE));
		(yyval.nodeTest) = step;
	}
    break;

  case 356:
#line 3088 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATTRIBUTE,(yyvsp[-1].qName)));
		(yyval.nodeTest) = step;
	}
    break;

  case 357:
#line 3094 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATTRIBUTE,(yyvsp[-3].qName),(yyvsp[-1].qName)));
		(yyval.nodeTest) = step;
	}
    break;

  case 359:
#line 3105 "../src/parser/XQParser.y"
    {
		(yyval.qName) = NULL;
	}
    break;

  case 360:
#line 3113 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_SCHEMA_ATTRIBUTE,(yyvsp[-1].qName)));
		(yyval.nodeTest) = step;
	}
    break;

  case 362:
#line 3128 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT));
		(yyval.nodeTest) = step;
	}
    break;

  case 363:
#line 3134 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT,(yyvsp[-1].qName)));
		(yyval.nodeTest) = step;
	}
    break;

  case 364:
#line 3140 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT,(yyvsp[-3].qName),(yyvsp[-1].qName)));
		(yyval.nodeTest) = step;
	}
    break;

  case 365:
#line 3146 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        SequenceType::ItemType* pType=new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT,(yyvsp[-4].qName),(yyvsp[-2].qName));
		pType->setAllowNilled(true);
        step->setItemType(pType);
		(yyval.nodeTest) = step;
	}
    break;

  case 367:
#line 3159 "../src/parser/XQParser.y"
    {
		(yyval.qName) = NULL;
	}
    break;

  case 368:
#line 3167 "../src/parser/XQParser.y"
    {
		NodeTest *step = new (MEMMGR) NodeTest();
        step->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_SCHEMA_ELEMENT,(yyvsp[-1].qName)));
		(yyval.nodeTest) = step;
	}
    break;

  case 373:
#line 3197 "../src/parser/XQParser.y"
    {
		// the string must be whitespace-normalized
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::collapseWS((yyvsp[0].str), MEMMGR);
        if((yyvsp[0].str) && *(yyvsp[0].str) && !XPath2Utils::isValidURI((yyvsp[0].str), MEMMGR))
          yyerror((yylsp[0]), "The URI literal is not valid [err:XQST0046]");
		(yyval.str) = (yyvsp[0].str);
	}
    break;

  case 374:
#line 3209 "../src/parser/XQParser.y"
    {
    // TBD FTSelectionOptions and weight
    (yyval.ftselection) = (yyvsp[-3].ftselection);

    for(VectorOfFTOptions::iterator i = (yyvsp[-2].ftoptionlist)->begin();
        i != (yyvsp[-2].ftoptionlist)->end(); ++i) {
      (*i)->setArgument((yyval.ftselection));
      (yyval.ftselection) = *i;
    }
    delete (yyvsp[-2].ftoptionlist);
	}
    break;

  case 375:
#line 3221 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = (yyvsp[-1].ftselection);

    for(VectorOfFTOptions::iterator i = (yyvsp[0].ftoptionlist)->begin();
        i != (yyvsp[0].ftoptionlist)->end(); ++i) {
      (*i)->setArgument((yyval.ftselection));
      (yyval.ftselection) = *i;
    }
    delete (yyvsp[0].ftoptionlist);
	}
    break;

  case 376:
#line 3235 "../src/parser/XQParser.y"
    {
    (yyval.ftoptionlist) = new (MEMMGR) VectorOfFTOptions(XQillaAllocator<FTOption*>(MEMMGR));
	}
    break;

  case 377:
#line 3239 "../src/parser/XQParser.y"
    {
    (yyval.ftoptionlist) = (yyvsp[-1].ftoptionlist);
	}
    break;

  case 378:
#line 3243 "../src/parser/XQParser.y"
    {
    if((yyvsp[0].ftoption) != NULL)
      (yyvsp[-1].ftoptionlist)->push_back((yyvsp[0].ftoption));
    (yyval.ftoptionlist) = (yyvsp[-1].ftoptionlist);
	}
    break;

  case 379:
#line 3253 "../src/parser/XQParser.y"
    {
    if((yyvsp[-2].ftselection)->getType() == FTSelection::OR) {
      FTOr *op = (FTOr*)(yyvsp[-2].ftselection);
      op->addArg((yyvsp[0].ftselection));
      (yyval.ftselection) = op;
    }
    else {
      (yyval.ftselection) = WRAP((yylsp[-1]), new (MEMMGR) FTOr((yyvsp[-2].ftselection), (yyvsp[0].ftselection), MEMMGR));
    }
	}
    break;

  case 381:
#line 3269 "../src/parser/XQParser.y"
    {
    if((yyvsp[-2].ftselection)->getType() == FTSelection::AND) {
      FTAnd *op = (FTAnd*)(yyvsp[-2].ftselection);
      op->addArg((yyvsp[0].ftselection));
      (yyval.ftselection) = op;
    }
    else {
      (yyval.ftselection) = WRAP((yylsp[-1]), new (MEMMGR) FTAnd((yyvsp[-2].ftselection), (yyvsp[0].ftselection), MEMMGR));
    }
	}
    break;

  case 383:
#line 3285 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[-1]), new (MEMMGR) FTMildnot((yyvsp[-2].ftselection), (yyvsp[0].ftselection), MEMMGR));
	}
    break;

  case 385:
#line 3294 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[-1]), new (MEMMGR) FTUnaryNot((yyvsp[0].ftselection), MEMMGR));
	}
    break;

  case 387:
#line 3303 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = (yyvsp[-1].ftselection);
	}
    break;

  case 389:
#line 3312 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[-1]), new (MEMMGR) FTWords((yyvsp[-1].astNode), (yyvsp[0].ftanyalloption), MEMMGR));
	}
    break;

  case 390:
#line 3316 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[-1]), new (MEMMGR) FTWords((yyvsp[-1].astNode), (yyvsp[0].ftanyalloption), MEMMGR));
	}
    break;

  case 391:
#line 3320 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[-1]), new (MEMMGR) FTWords((yyvsp[-1].astNode), (yyvsp[0].ftanyalloption), MEMMGR));
	}
    break;

  case 392:
#line 3324 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[-1]), new (MEMMGR) FTWords((yyvsp[-1].astNode), (yyvsp[0].ftanyalloption), MEMMGR));
	}
    break;

  case 393:
#line 3328 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[-2]), new (MEMMGR) FTWords((yyvsp[-2].astNode), (yyvsp[0].ftanyalloption), MEMMGR));
	}
    break;

  case 394:
#line 3342 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[0]), new (MEMMGR) FTOrder(MEMMGR));
	}
    break;

  case 395:
#line 3346 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[-2]), new (MEMMGR) FTWindow((yyvsp[-1].astNode), (yyvsp[0].ftunit), MEMMGR));
	}
    break;

  case 396:
#line 3350 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[-2]), new (MEMMGR) FTDistance((yyvsp[-1].ftrange), (yyvsp[0].ftunit), MEMMGR));
	}
    break;

  case 397:
#line 3354 "../src/parser/XQParser.y"
    {
    std::cerr << "occurs" << std::endl;
    (yyval.ftoption) = NULL;
	}
    break;

  case 398:
#line 3359 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[-1]), new (MEMMGR) FTScope(FTScope::SAME, (yyvsp[0].ftunit), MEMMGR));
	}
    break;

  case 399:
#line 3363 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[-1]), new (MEMMGR) FTScope(FTScope::DIFFERENT, (yyvsp[0].ftunit), MEMMGR));
	}
    break;

  case 400:
#line 3367 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[0]), new (MEMMGR) FTContent(FTContent::AT_START, MEMMGR));
	}
    break;

  case 401:
#line 3371 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[0]), new (MEMMGR) FTContent(FTContent::AT_END, MEMMGR));
	}
    break;

  case 402:
#line 3375 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[0]), new (MEMMGR) FTContent(FTContent::ENTIRE_CONTENT, MEMMGR));
	}
    break;

  case 410:
#line 3403 "../src/parser/XQParser.y"
    {
    std::cerr << "lowercase" << std::endl;
	}
    break;

  case 411:
#line 3407 "../src/parser/XQParser.y"
    {
    std::cerr << "uppercase" << std::endl;
	}
    break;

  case 412:
#line 3411 "../src/parser/XQParser.y"
    {
    std::cerr << "case sensitive" << std::endl;
	}
    break;

  case 413:
#line 3415 "../src/parser/XQParser.y"
    {
    std::cerr << "case insensitive" << std::endl;
	}
    break;

  case 414:
#line 3426 "../src/parser/XQParser.y"
    {
    std::cerr << "with diacritics" << std::endl;
	}
    break;

  case 415:
#line 3430 "../src/parser/XQParser.y"
    {
    std::cerr << "without diacritics" << std::endl;
	}
    break;

  case 416:
#line 3434 "../src/parser/XQParser.y"
    {
    std::cerr << "diacritics sensitive" << std::endl;
	}
    break;

  case 417:
#line 3438 "../src/parser/XQParser.y"
    {
    std::cerr << "diacritics insensitive" << std::endl;
	}
    break;

  case 418:
#line 3446 "../src/parser/XQParser.y"
    {
    std::cerr << "with stemming" << std::endl;
	}
    break;

  case 419:
#line 3450 "../src/parser/XQParser.y"
    {
    std::cerr << "without stemming" << std::endl;
	}
    break;

  case 420:
#line 3460 "../src/parser/XQParser.y"
    {
    std::cerr << "with thesaurus" << std::endl;
	}
    break;

  case 421:
#line 3464 "../src/parser/XQParser.y"
    {
    std::cerr << "with thesaurus default" << std::endl;
	}
    break;

  case 422:
#line 3468 "../src/parser/XQParser.y"
    {
    std::cerr << "with thesaurus ()" << std::endl;
	}
    break;

  case 423:
#line 3472 "../src/parser/XQParser.y"
    {
    std::cerr << "with thesaurus (default)" << std::endl;
	}
    break;

  case 424:
#line 3476 "../src/parser/XQParser.y"
    {
    std::cerr << "without thesaurus" << std::endl;
	}
    break;

  case 425:
#line 3483 "../src/parser/XQParser.y"
    {
	}
    break;

  case 426:
#line 3486 "../src/parser/XQParser.y"
    {
	}
    break;

  case 427:
#line 3493 "../src/parser/XQParser.y"
    {
    std::cerr << "at StringLiteral" << std::endl;
	}
    break;

  case 428:
#line 3497 "../src/parser/XQParser.y"
    {
    std::cerr << "at StringLiteral relationship StringLiteral" << std::endl;
	}
    break;

  case 429:
#line 3501 "../src/parser/XQParser.y"
    {
    std::cerr << "at StringLiteral levels" << std::endl;
	}
    break;

  case 430:
#line 3505 "../src/parser/XQParser.y"
    {
    std::cerr << "at StringLiteral relationship StringLiteral levels" << std::endl;
	}
    break;

  case 431:
#line 3515 "../src/parser/XQParser.y"
    {
    std::cerr << "with stop words" << std::endl;
	}
    break;

  case 432:
#line 3519 "../src/parser/XQParser.y"
    {
    std::cerr << "without stop words" << std::endl;
	}
    break;

  case 433:
#line 3523 "../src/parser/XQParser.y"
    {
    std::cerr << "with default stop words" << std::endl;
	}
    break;

  case 434:
#line 3530 "../src/parser/XQParser.y"
    {
	}
    break;

  case 435:
#line 3533 "../src/parser/XQParser.y"
    {
	}
    break;

  case 436:
#line 3541 "../src/parser/XQParser.y"
    {
    std::cerr << "at StringLiteral" << std::endl;
	}
    break;

  case 437:
#line 3545 "../src/parser/XQParser.y"
    {
    std::cerr << "()" << std::endl;
	}
    break;

  case 438:
#line 3552 "../src/parser/XQParser.y"
    {
    std::cerr << "StringLiteral" << std::endl;
	}
    break;

  case 439:
#line 3556 "../src/parser/XQParser.y"
    {
    std::cerr << ", StringLiteral" << std::endl;
	}
    break;

  case 440:
#line 3564 "../src/parser/XQParser.y"
    {
    std::cerr << "union" << std::endl;
	}
    break;

  case 441:
#line 3568 "../src/parser/XQParser.y"
    {
    std::cerr << "except" << std::endl;
	}
    break;

  case 442:
#line 3576 "../src/parser/XQParser.y"
    {
    std::cerr << "language StringLiteral" << std::endl;
	}
    break;

  case 443:
#line 3584 "../src/parser/XQParser.y"
    {
    std::cerr << "with wildcards" << std::endl;
	}
    break;

  case 444:
#line 3588 "../src/parser/XQParser.y"
    {
    std::cerr << "without wildcards" << std::endl;
	}
    break;

  case 445:
#line 3596 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::ANY;
	}
    break;

  case 446:
#line 3600 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::ANY;
	}
    break;

  case 447:
#line 3604 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::ANY_WORD;
	}
    break;

  case 448:
#line 3608 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::ALL;
	}
    break;

  case 449:
#line 3612 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::ALL_WORDS;
	}
    break;

  case 450:
#line 3616 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::PHRASE;
	}
    break;

  case 451:
#line 3627 "../src/parser/XQParser.y"
    {
    (yyval.ftrange).type = FTRange::EXACTLY;
    (yyval.ftrange).arg1 = (yyvsp[0].astNode);
    (yyval.ftrange).arg2 = 0;
	}
    break;

  case 452:
#line 3633 "../src/parser/XQParser.y"
    {
    (yyval.ftrange).type = FTRange::AT_LEAST;
    (yyval.ftrange).arg1 = (yyvsp[0].astNode);
    (yyval.ftrange).arg2 = 0;
	}
    break;

  case 453:
#line 3639 "../src/parser/XQParser.y"
    {
    (yyval.ftrange).type = FTRange::AT_MOST;
    (yyval.ftrange).arg1 = (yyvsp[0].astNode);
    (yyval.ftrange).arg2 = 0;
	}
    break;

  case 454:
#line 3645 "../src/parser/XQParser.y"
    {
    (yyval.ftrange).type = FTRange::FROM_TO;
    (yyval.ftrange).arg1 = (yyvsp[-2].astNode);
    (yyval.ftrange).arg2 = (yyvsp[0].astNode);
	}
    break;

  case 455:
#line 3655 "../src/parser/XQParser.y"
    {
    (yyval.ftunit) = FTOption::WORDS;
	}
    break;

  case 456:
#line 3659 "../src/parser/XQParser.y"
    {
    (yyval.ftunit) = FTOption::SENTENCES;
	}
    break;

  case 457:
#line 3663 "../src/parser/XQParser.y"
    {
    (yyval.ftunit) = FTOption::PARAGRAPHS;
	}
    break;

  case 458:
#line 3671 "../src/parser/XQParser.y"
    {
    (yyval.ftunit) = FTOption::SENTENCES;
	}
    break;

  case 459:
#line 3675 "../src/parser/XQParser.y"
    {
    (yyval.ftunit) = FTOption::PARAGRAPHS;
	}
    break;

  case 460:
#line 3683 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[0].astNode);
	}
    break;

  case 461:
#line 3691 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER,
										(yyvsp[0].str), AnyAtomicType::DECIMAL),
										MEMMGR);
		}
    break;

  case 462:
#line 3704 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL,
										(yyvsp[0].str), AnyAtomicType::DECIMAL),
										MEMMGR);
		}
    break;

  case 463:
#line 3717 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE,
										(yyvsp[0].str), AnyAtomicType::DOUBLE),
										MEMMGR);
		}
    break;

  case 464:
#line 3731 "../src/parser/XQParser.y"
    {
			(yyval.astNode) = new (MEMMGR) XQLiteral(
                    new (MEMMGR) AnyAtomicTypeConstructor(
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
										XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
										(yyvsp[0].str), AnyAtomicType::STRING),
										MEMMGR);
		}
    break;

  case 465:
#line 3744 "../src/parser/XQParser.y"
    {
			(yyval.qName) = new (MEMMGR) QualifiedName((yyvsp[0].str),MEMMGR);
		}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 6514 "../src/parser/XQParser.cpp"

  yyvsp -= yylen;
  yyssp -= yylen;
  yylsp -= yylen;

  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  yylsp -= yylen;
  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping", yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though. */
  YYLLOC_DEFAULT (yyloc, yyerror_range - 1, 2);
  *++yylsp = yyloc;

  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 3748 "../src/parser/XQParser.y"


}	// namespace XQParser



