/* A Bison parser, made by GNU Bison 1.875.  */

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
     _QUESTION_ = 258,
     _LBRACK_ = 259,
     _RBRACK_ = 260,
     _PLUS_ = 261,
     _MINUS_ = 262,
     _STAR_ = 263,
     _NCNAME_COLON_STAR_ = 264,
     _STAR_COLON_NCNAME_ = 265,
     _DOT_ = 266,
     _DOT_DOT_ = 267,
     _STRING_LITERAL_ = 268,
     _SLASH_ = 269,
     _SLASHSLASH_ = 270,
     _AXIS_CHILD_ = 271,
     _AXIS_DESCENDANT_ = 272,
     _AXIS_PARENT_ = 273,
     _AXIS_ATTRIBUTE_ = 274,
     _AXIS_SELF_ = 275,
     _AXIS_DESCENDANT_OR_SELF_ = 276,
     _AXIS_ANCESTOR_ = 277,
     _AXIS_FOLLOWING_SIBLING_ = 278,
     _AXIS_PRECEDING_SIBLING_ = 279,
     _AXIS_FOLLOWING_ = 280,
     _AXIS_PRECEDING_ = 281,
     _AXIS_NAMESPACE_ = 282,
     _AXIS_ANCESTOR_OR_SELF_ = 283,
     _AT_ = 284,
     _CAST_AS_ = 285,
     _TREAT_AS_ = 286,
     _FOR_ = 287,
     _SOME_ = 288,
     _EVERY_ = 289,
     _COMMA_ = 290,
     _LPAR_ = 291,
     _TEXT_LPAR_ = 292,
     _COMMENT_LPAR_ = 293,
     _NODE_LPAR_ = 294,
     _PROCESSING_INSTRUCTION_LPAR_ = 295,
     _ELEMENT_LPAR_ = 296,
     _ATTRIBUTE_LPAR_ = 297,
     _SCHEMA_ELEMENT_LPAR_ = 298,
     _SCHEMA_ATTRIBUTE_LPAR_ = 299,
     _DOCUMENT_NODE_LPAR_ = 300,
     _IF_LPAR_ = 301,
     _QNAME_LPAR_ = 302,
     _RPAR_ = 303,
     _MULTIPLY_ = 304,
     _DIV_ = 305,
     _INTEGER_DIV_ = 306,
     _MOD_ = 307,
     _AND_ = 308,
     _OR_ = 309,
     _RETURN_ = 310,
     _THEN_ = 311,
     _ELSE_ = 312,
     _TO_ = 313,
     _UNION_ = 314,
     _INTERSECT_ = 315,
     _EXCEPT_ = 316,
     _EQUALS_ = 317,
     _IS_ = 318,
     _NOT_EQUALS_ = 319,
     _LT_EQUALS_ = 320,
     _GT_EQUALS_ = 321,
     _LT_ = 322,
     _GT_ = 323,
     _VERTICAL_BAR_ = 324,
     _LT_LT_ = 325,
     _GT_GT_ = 326,
     _FORTRAN_EQ_ = 327,
     _FORTRAN_NE_ = 328,
     _FORTRAN_GT_ = 329,
     _FORTRAN_GE_ = 330,
     _FORTRAN_LT_ = 331,
     _FORTRAN_LE_ = 332,
     _IN_ = 333,
     _SATISFIES_ = 334,
     _ITEM_ = 335,
     _EMPTY_ = 336,
     _INSTANCE_OF_ = 337,
     _CASTABLE_AS_ = 338,
     _NILLABLE_ = 339,
     _ZERO_OR_ONE_ = 340,
     _ONE_OR_MORE_ = 341,
     _ZERO_OR_MORE_ = 342,
     _VARNAME_ = 343,
     _QNAME_ = 344,
     _NCNAME_ = 345,
     _INTEGER_LITERAL_ = 346,
     _DECIMAL_LITERAL_ = 347,
     _DOUBLE_LITERAL_ = 348
   };
#endif
#define _QUESTION_ 258
#define _LBRACK_ 259
#define _RBRACK_ 260
#define _PLUS_ 261
#define _MINUS_ 262
#define _STAR_ 263
#define _NCNAME_COLON_STAR_ 264
#define _STAR_COLON_NCNAME_ 265
#define _DOT_ 266
#define _DOT_DOT_ 267
#define _STRING_LITERAL_ 268
#define _SLASH_ 269
#define _SLASHSLASH_ 270
#define _AXIS_CHILD_ 271
#define _AXIS_DESCENDANT_ 272
#define _AXIS_PARENT_ 273
#define _AXIS_ATTRIBUTE_ 274
#define _AXIS_SELF_ 275
#define _AXIS_DESCENDANT_OR_SELF_ 276
#define _AXIS_ANCESTOR_ 277
#define _AXIS_FOLLOWING_SIBLING_ 278
#define _AXIS_PRECEDING_SIBLING_ 279
#define _AXIS_FOLLOWING_ 280
#define _AXIS_PRECEDING_ 281
#define _AXIS_NAMESPACE_ 282
#define _AXIS_ANCESTOR_OR_SELF_ 283
#define _AT_ 284
#define _CAST_AS_ 285
#define _TREAT_AS_ 286
#define _FOR_ 287
#define _SOME_ 288
#define _EVERY_ 289
#define _COMMA_ 290
#define _LPAR_ 291
#define _TEXT_LPAR_ 292
#define _COMMENT_LPAR_ 293
#define _NODE_LPAR_ 294
#define _PROCESSING_INSTRUCTION_LPAR_ 295
#define _ELEMENT_LPAR_ 296
#define _ATTRIBUTE_LPAR_ 297
#define _SCHEMA_ELEMENT_LPAR_ 298
#define _SCHEMA_ATTRIBUTE_LPAR_ 299
#define _DOCUMENT_NODE_LPAR_ 300
#define _IF_LPAR_ 301
#define _QNAME_LPAR_ 302
#define _RPAR_ 303
#define _MULTIPLY_ 304
#define _DIV_ 305
#define _INTEGER_DIV_ 306
#define _MOD_ 307
#define _AND_ 308
#define _OR_ 309
#define _RETURN_ 310
#define _THEN_ 311
#define _ELSE_ 312
#define _TO_ 313
#define _UNION_ 314
#define _INTERSECT_ 315
#define _EXCEPT_ 316
#define _EQUALS_ 317
#define _IS_ 318
#define _NOT_EQUALS_ 319
#define _LT_EQUALS_ 320
#define _GT_EQUALS_ 321
#define _LT_ 322
#define _GT_ 323
#define _VERTICAL_BAR_ 324
#define _LT_LT_ 325
#define _GT_GT_ 326
#define _FORTRAN_EQ_ 327
#define _FORTRAN_NE_ 328
#define _FORTRAN_GT_ 329
#define _FORTRAN_GE_ 330
#define _FORTRAN_LT_ 331
#define _FORTRAN_LE_ 332
#define _IN_ 333
#define _SATISFIES_ 334
#define _ITEM_ 335
#define _EMPTY_ 336
#define _INSTANCE_OF_ 337
#define _CASTABLE_AS_ 338
#define _NILLABLE_ 339
#define _ZERO_OR_ONE_ 340
#define _ONE_OR_MORE_ 341
#define _ZERO_OR_MORE_ 342
#define _VARNAME_ 343
#define _QNAME_ 344
#define _NCNAME_ 345
#define _INTEGER_LITERAL_ 346
#define _DECIMAL_LITERAL_ 347
#define _DOUBLE_LITERAL_ 348




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif





