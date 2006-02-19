typedef union {
  char  *id;
  List  *bases;
  struct Define {
    String *val;
    String *rawval;
    int     type;
    String *qualifier;
    String *bitfield;
    Parm   *throws;
  } dtype;
  struct {
    char *type;
    char *filename;
    int   line;
  } loc;
  struct {
    char      *id;
    SwigType  *type;
    String    *defarg;
    ParmList  *parms;
    short      have_parms;
    ParmList  *throws;
  } decl;
  Parm         *tparms;
  struct {
    String     *op;
    Hash       *kwargs;
  } tmap;
  struct {
    String     *type;
    String     *us;
  } ptype;
  SwigType     *type;
  String       *str;
  Parm         *p;
  ParmList     *pl;
  int           ivalue;
  Node         *node;
} YYSTYPE;
#define	ID	257
#define	HBLOCK	258
#define	POUND	259
#define	STRING	260
#define	INCLUDE	261
#define	IMPORT	262
#define	INSERT	263
#define	CHARCONST	264
#define	NUM_INT	265
#define	NUM_FLOAT	266
#define	NUM_UNSIGNED	267
#define	NUM_LONG	268
#define	NUM_ULONG	269
#define	NUM_LONGLONG	270
#define	NUM_ULONGLONG	271
#define	TYPEDEF	272
#define	TYPE_INT	273
#define	TYPE_UNSIGNED	274
#define	TYPE_SHORT	275
#define	TYPE_LONG	276
#define	TYPE_FLOAT	277
#define	TYPE_DOUBLE	278
#define	TYPE_CHAR	279
#define	TYPE_VOID	280
#define	TYPE_SIGNED	281
#define	TYPE_BOOL	282
#define	TYPE_TYPEDEF	283
#define	TYPE_RAW	284
#define	LPAREN	285
#define	RPAREN	286
#define	COMMA	287
#define	SEMI	288
#define	EXTERN	289
#define	INIT	290
#define	LBRACE	291
#define	RBRACE	292
#define	PERIOD	293
#define	CONST	294
#define	VOLATILE	295
#define	STRUCT	296
#define	UNION	297
#define	EQUAL	298
#define	SIZEOF	299
#define	MODULE	300
#define	LBRACKET	301
#define	RBRACKET	302
#define	ILLEGAL	303
#define	CONSTANT	304
#define	NAME	305
#define	RENAME	306
#define	NAMEWARN	307
#define	EXTEND	308
#define	PRAGMA	309
#define	FEATURE	310
#define	VARARGS	311
#define	ENUM	312
#define	CLASS	313
#define	TYPENAME	314
#define	PRIVATE	315
#define	PUBLIC	316
#define	PROTECTED	317
#define	COLON	318
#define	STATIC	319
#define	VIRTUAL	320
#define	FRIEND	321
#define	THROW	322
#define	CATCH	323
#define	USING	324
#define	NAMESPACE	325
#define	NATIVE	326
#define	INLINE	327
#define	TYPEMAP	328
#define	EXCEPT	329
#define	ECHO	330
#define	APPLY	331
#define	CLEAR	332
#define	SWIGTEMPLATE	333
#define	FRAGMENT	334
#define	WARN	335
#define	LESSTHAN	336
#define	GREATERTHAN	337
#define	MODULO	338
#define	DELETE	339
#define	TYPES	340
#define	PARMS	341
#define	NONID	342
#define	DSTAR	343
#define	DCNOT	344
#define	TEMPLATE	345
#define	OPERATOR	346
#define	COPERATOR	347
#define	PARSETYPE	348
#define	PARSEPARM	349
#define	CAST	350
#define	LOR	351
#define	LAND	352
#define	OR	353
#define	XOR	354
#define	AND	355
#define	LSHIFT	356
#define	RSHIFT	357
#define	PLUS	358
#define	MINUS	359
#define	STAR	360
#define	SLASH	361
#define	UMINUS	362
#define	NOT	363
#define	LNOT	364
#define	DCOLON	365


extern YYSTYPE yylval;
