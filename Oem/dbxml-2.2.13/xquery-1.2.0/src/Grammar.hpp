/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

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
     _XQUERY_VERSION_ = 276,
     _XML_COMMENT_ = 277,
     _CDATA_SECTION_ = 278,
     _PROCESSING_INSTRUCTION_CONTENT_ = 279,
     _NAMED_PROCESSING_INSTRUCTION_CONSTR_ = 280,
     _PRAGMA_NAME_ = 281,
     _PRAGMA_CONTENT_ = 282,
     _XQUERY_COMMENT_ = 283,
     _XQUERY_WHITESPACE_ = 284,
     _XQUERY_ERROR_ = 285,
     _PRAGMA_OPEN_ = 286,
     _PRAGMA_CLOSE_ = 287,
     _ESCAPE_APOS_ = 288,
     _ESCAPE_QUOT_ = 289,
     _COMMA_ = 290,
     _RETURN_ = 291,
     _FOR_ = 292,
     _IN_ = 293,
     _DOLLAR_SIGN_ = 294,
     _AT_KEYWORD_ = 295,
     _LET_ = 296,
     _COLON_EQ_ = 297,
     _WHERE_ = 298,
     _ORDER_BY_ = 299,
     _STABLE_ORDER_BY_ = 300,
     _ASCENDING_ = 301,
     _DESCENDING_ = 302,
     _EMPTY_GREATEST_ = 303,
     _EMPTY_LEAST_ = 304,
     _COLLATION_ = 305,
     _SOME_ = 306,
     _EVERY_ = 307,
     _SATISFIES_ = 308,
     _TYPESWITCH_ = 309,
     _DEFAULT_ = 310,
     _CASE_ = 311,
     _AS_ = 312,
     _IF_ = 313,
     _THEN_ = 314,
     _ELSE_ = 315,
     _OR_ = 316,
     _AND_ = 317,
     _INSTANCE_OF_ = 318,
     _CASTABLE_AS_ = 319,
     _TO_ = 320,
     _PLUS_ = 321,
     _MINUS_ = 322,
     _MULTIPLY_ = 323,
     _DIV_ = 324,
     _INTEGER_DIV_ = 325,
     _MOD_ = 326,
     _UNION_ = 327,
     _VERTICAL_BAR_ = 328,
     _INTERSECT_ = 329,
     _EXCEPT_ = 330,
     _SLASH_ = 331,
     _SLASHSLASH_ = 332,
     _LBRACK_ = 333,
     _RBRACK_ = 334,
     _VALIDATE_CONTENT_ = 335,
     _VALIDATE_MODE_LAX_ = 336,
     _VALIDATE_MODE_STRICT_ = 337,
     _CAST_AS_ = 338,
     _TREAT_AS_ = 339,
     _EQUALS_ = 340,
     _NOT_EQUALS_ = 341,
     _LT_ = 342,
     _LT_EQUALS_ = 343,
     _GT_ = 344,
     _GT_EQUALS_ = 345,
     _FORTRAN_EQ_ = 346,
     _FORTRAN_NE_ = 347,
     _FORTRAN_LT_ = 348,
     _FORTRAN_LE_ = 349,
     _FORTRAN_GT_ = 350,
     _FORTRAN_GE_ = 351,
     _IS_ = 352,
     _LT_LT_ = 353,
     _GT_GT_ = 354,
     _DOT_ = 355,
     _AT_ = 356,
     _DOT_DOT_ = 357,
     _AXIS_CHILD_ = 358,
     _AXIS_DESCENDANT_ = 359,
     _AXIS_ATTRIBUTE_ = 360,
     _AXIS_SELF_ = 361,
     _AXIS_DESCENDANT_OR_SELF_ = 362,
     _AXIS_PARENT_ = 363,
     _STAR_ = 364,
     _PROCESSING_INSTRUCTION_LPAR_ = 365,
     _COMMENT_LPAR_ = 366,
     _TEXT_LPAR_ = 367,
     _NODE_LPAR_ = 368,
     _LPAR_ = 369,
     _RPAR_ = 370,
     _START_TAG_OPEN_ = 371,
     _EMPTY_TAG_CLOSE_ = 372,
     _VALUE_INDICATOR_ = 373,
     _END_TAG_OPEN_ = 374,
     _TAG_CLOSE_ = 375,
     _DOCUMENT_CONSTR_ = 376,
     _ELEMENT_CONSTR_ = 377,
     _ATTRIBUTE_CONSTR_ = 378,
     _TEXT_CONSTR_ = 379,
     _PROCESSING_INSTRUCTION_START_ = 380,
     _LCURLY_BRACE_ESCAPE_ = 381,
     _RCURLY_BRACE_ESCAPE_ = 382,
     _OPEN_QUOT_ = 383,
     _CLOSE_QUOT_ = 384,
     _OPEN_APOS_ = 385,
     _CLOSE_APOS_ = 386,
     _LBRACE_ = 387,
     _RBRACE_ = 388,
     _PRESERVE_ = 389,
     _STRIP_ = 390,
     _NAMESPACE_ = 391,
     _EXPR_AS_ = 392,
     _EMPTY_ = 393,
     _ITEM_ = 394,
     _NILLABLE_ = 395,
     _IMPORT_SCHEMA_ = 396,
     _IMPORT_MODULE_ = 397,
     _DOCUMENT_NODE_LPAR_ = 398,
     _ATTRIBUTE_LPAR_ = 399,
     _ELEMENT_LPAR_ = 400,
     _EXTERNAL_ = 401,
     _MODULE_NAMESPACE_ = 402,
     _PROCESSING_INSTRUCTION_CONSTR_ = 403,
     _COMMENT_CONSTR_ = 404,
     _AXIS_ANCESTOR_OR_SELF_ = 405,
     _AXIS_ANCESTOR_ = 406,
     _AXIS_FOLLOWING_SIBLING_ = 407,
     _AXIS_FOLLOWING_ = 408,
     _AXIS_PRECEDING_SIBLING_ = 409,
     _AXIS_PRECEDING_ = 410,
     _SEMICOLON_ = 411,
     _DEFAULT_ELEMENT_ = 412,
     _SCHEMA_ELEMENT_LPAR_ = 413,
     _SCHEMA_ATTRIBUTE_LPAR_ = 414,
     _ORDERED_ = 415,
     _UNORDERED_ = 416,
     _CONSTRUCTION_PRESERVE_ = 417,
     _CONSTRUCTION_STRIP_ = 418,
     _ORDERING_UNORDERED_ = 419,
     _ORDERING_ORDERED_ = 420,
     _ZERO_OR_ONE_ = 421,
     _ONE_OR_MORE_ = 422,
     _ZERO_OR_MORE_ = 423,
     _ENCODING_ = 424,
     _NS_PRESERVE_ = 425,
     _NS_NO_PRESERVE_ = 426,
     _NS_INHERIT_ = 427,
     _NS_NO_INHERIT_ = 428,
     _DECLARE_ = 429,
     _BOUNDARYSPACE_ = 430,
     _DEFAULT_COLLATION_ = 431,
     _DEFAULT_FUNCTION_ = 432,
     _BASEURI_ = 433,
     _CONSTRUCTION_ = 434,
     _ORDERING_ = 435,
     _DEFAULT_ORDER_ = 436,
     _COPY_NAMESPACES_ = 437,
     _DECLARE_FUNCTION_ = 438,
     _DECLARE_VARIABLE_ = 439,
     _DECLARE_OPTION_ = 440,
     _EOF_ = 441
   };
#endif
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
#define _XQUERY_VERSION_ 276
#define _XML_COMMENT_ 277
#define _CDATA_SECTION_ 278
#define _PROCESSING_INSTRUCTION_CONTENT_ 279
#define _NAMED_PROCESSING_INSTRUCTION_CONSTR_ 280
#define _PRAGMA_NAME_ 281
#define _PRAGMA_CONTENT_ 282
#define _XQUERY_COMMENT_ 283
#define _XQUERY_WHITESPACE_ 284
#define _XQUERY_ERROR_ 285
#define _PRAGMA_OPEN_ 286
#define _PRAGMA_CLOSE_ 287
#define _ESCAPE_APOS_ 288
#define _ESCAPE_QUOT_ 289
#define _COMMA_ 290
#define _RETURN_ 291
#define _FOR_ 292
#define _IN_ 293
#define _DOLLAR_SIGN_ 294
#define _AT_KEYWORD_ 295
#define _LET_ 296
#define _COLON_EQ_ 297
#define _WHERE_ 298
#define _ORDER_BY_ 299
#define _STABLE_ORDER_BY_ 300
#define _ASCENDING_ 301
#define _DESCENDING_ 302
#define _EMPTY_GREATEST_ 303
#define _EMPTY_LEAST_ 304
#define _COLLATION_ 305
#define _SOME_ 306
#define _EVERY_ 307
#define _SATISFIES_ 308
#define _TYPESWITCH_ 309
#define _DEFAULT_ 310
#define _CASE_ 311
#define _AS_ 312
#define _IF_ 313
#define _THEN_ 314
#define _ELSE_ 315
#define _OR_ 316
#define _AND_ 317
#define _INSTANCE_OF_ 318
#define _CASTABLE_AS_ 319
#define _TO_ 320
#define _PLUS_ 321
#define _MINUS_ 322
#define _MULTIPLY_ 323
#define _DIV_ 324
#define _INTEGER_DIV_ 325
#define _MOD_ 326
#define _UNION_ 327
#define _VERTICAL_BAR_ 328
#define _INTERSECT_ 329
#define _EXCEPT_ 330
#define _SLASH_ 331
#define _SLASHSLASH_ 332
#define _LBRACK_ 333
#define _RBRACK_ 334
#define _VALIDATE_CONTENT_ 335
#define _VALIDATE_MODE_LAX_ 336
#define _VALIDATE_MODE_STRICT_ 337
#define _CAST_AS_ 338
#define _TREAT_AS_ 339
#define _EQUALS_ 340
#define _NOT_EQUALS_ 341
#define _LT_ 342
#define _LT_EQUALS_ 343
#define _GT_ 344
#define _GT_EQUALS_ 345
#define _FORTRAN_EQ_ 346
#define _FORTRAN_NE_ 347
#define _FORTRAN_LT_ 348
#define _FORTRAN_LE_ 349
#define _FORTRAN_GT_ 350
#define _FORTRAN_GE_ 351
#define _IS_ 352
#define _LT_LT_ 353
#define _GT_GT_ 354
#define _DOT_ 355
#define _AT_ 356
#define _DOT_DOT_ 357
#define _AXIS_CHILD_ 358
#define _AXIS_DESCENDANT_ 359
#define _AXIS_ATTRIBUTE_ 360
#define _AXIS_SELF_ 361
#define _AXIS_DESCENDANT_OR_SELF_ 362
#define _AXIS_PARENT_ 363
#define _STAR_ 364
#define _PROCESSING_INSTRUCTION_LPAR_ 365
#define _COMMENT_LPAR_ 366
#define _TEXT_LPAR_ 367
#define _NODE_LPAR_ 368
#define _LPAR_ 369
#define _RPAR_ 370
#define _START_TAG_OPEN_ 371
#define _EMPTY_TAG_CLOSE_ 372
#define _VALUE_INDICATOR_ 373
#define _END_TAG_OPEN_ 374
#define _TAG_CLOSE_ 375
#define _DOCUMENT_CONSTR_ 376
#define _ELEMENT_CONSTR_ 377
#define _ATTRIBUTE_CONSTR_ 378
#define _TEXT_CONSTR_ 379
#define _PROCESSING_INSTRUCTION_START_ 380
#define _LCURLY_BRACE_ESCAPE_ 381
#define _RCURLY_BRACE_ESCAPE_ 382
#define _OPEN_QUOT_ 383
#define _CLOSE_QUOT_ 384
#define _OPEN_APOS_ 385
#define _CLOSE_APOS_ 386
#define _LBRACE_ 387
#define _RBRACE_ 388
#define _PRESERVE_ 389
#define _STRIP_ 390
#define _NAMESPACE_ 391
#define _EXPR_AS_ 392
#define _EMPTY_ 393
#define _ITEM_ 394
#define _NILLABLE_ 395
#define _IMPORT_SCHEMA_ 396
#define _IMPORT_MODULE_ 397
#define _DOCUMENT_NODE_LPAR_ 398
#define _ATTRIBUTE_LPAR_ 399
#define _ELEMENT_LPAR_ 400
#define _EXTERNAL_ 401
#define _MODULE_NAMESPACE_ 402
#define _PROCESSING_INSTRUCTION_CONSTR_ 403
#define _COMMENT_CONSTR_ 404
#define _AXIS_ANCESTOR_OR_SELF_ 405
#define _AXIS_ANCESTOR_ 406
#define _AXIS_FOLLOWING_SIBLING_ 407
#define _AXIS_FOLLOWING_ 408
#define _AXIS_PRECEDING_SIBLING_ 409
#define _AXIS_PRECEDING_ 410
#define _SEMICOLON_ 411
#define _DEFAULT_ELEMENT_ 412
#define _SCHEMA_ELEMENT_LPAR_ 413
#define _SCHEMA_ATTRIBUTE_LPAR_ 414
#define _ORDERED_ 415
#define _UNORDERED_ 416
#define _CONSTRUCTION_PRESERVE_ 417
#define _CONSTRUCTION_STRIP_ 418
#define _ORDERING_UNORDERED_ 419
#define _ORDERING_ORDERED_ 420
#define _ZERO_OR_ONE_ 421
#define _ONE_OR_MORE_ 422
#define _ZERO_OR_MORE_ 423
#define _ENCODING_ 424
#define _NS_PRESERVE_ 425
#define _NS_NO_PRESERVE_ 426
#define _NS_INHERIT_ 427
#define _NS_NO_INHERIT_ 428
#define _DECLARE_ 429
#define _BOUNDARYSPACE_ 430
#define _DEFAULT_COLLATION_ 431
#define _DEFAULT_FUNCTION_ 432
#define _BASEURI_ 433
#define _CONSTRUCTION_ 434
#define _ORDERING_ 435
#define _DEFAULT_ORDER_ 436
#define _COPY_NAMESPACES_ 437
#define _DECLARE_FUNCTION_ 438
#define _DECLARE_VARIABLE_ 439
#define _DECLARE_OPTION_ 440
#define _EOF_ 441




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




