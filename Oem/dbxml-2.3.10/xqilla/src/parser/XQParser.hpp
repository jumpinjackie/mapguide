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




