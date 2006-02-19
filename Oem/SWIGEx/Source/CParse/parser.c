
/*  A Bison parser, made from parser.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

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

#line 1 "parser.y"

/* -----------------------------------------------------------------------------
 * parser.y
 *
 *     YACC parser for SWIG.   The grammar is a somewhat broken subset of C/C++.
 *     This file is a bit of a mess and probably needs to be rewritten at
 *     some point.  Beware.
 *
 * Author(s) : David Beazley (beazley@cs.uchicago.edu)
 *
 * Copyright (C) 1998-2001.  The University of Chicago
 * Copyright (C) 1995-1998.  The University of Utah and The Regents of the
 *                           University of California.
 *
 * See the file LICENSE for information on usage and redistribution.
 * ----------------------------------------------------------------------------- */

#define yylex yylex

char cvsroot_parser_y[] = "$Header: /cvsroot/SWIG/Source/CParse/parser.y,v 1.37 2003/12/11 03:59:17 mmatus Exp $";

#include "cparse.h"
#include "preprocessor.h"
#include <ctype.h>

/* We do this for portability */
#undef alloca
#define alloca malloc

/* -----------------------------------------------------------------------------
 *                               Externals
 * ----------------------------------------------------------------------------- */

extern int   yylex();
extern void  yyerror (const char *e);

/* scanner.cxx */

extern int  cparse_line;
extern int  cparse_start_line;
extern void skip_balanced(int startchar, int endchar);
extern void skip_decl(void);
extern void scanner_check_typedef(void);
extern void scanner_ignore_typedef(void);
extern void scanner_last_id(int);
extern void scanner_clear_rename(void);
extern void start_inline(char *, int);
extern String *scanner_ccode;
extern int Swig_cparse_template_expand(Node *n, String *rname, ParmList *tparms);
extern Node *Swig_cparse_template_locate(String *name, ParmList *tparms);

/* NEW Variables */

extern void generate_all(Node *);

static Node    *top = 0;      /* Top of the generated parse tree */
static int      unnamed = 0;  /* Unnamed datatype counter */
static Hash    *extendhash = 0;     /* Hash table of added methods */
static Hash    *classes = 0;        /* Hash table of classes */
static Symtab  *prev_symtab = 0;
static Node    *current_class = 0;
       String  *ModuleName = 0;
static Node    *module_node = 0;
static String  *Classprefix = 0;  
static String  *Namespaceprefix = 0;
static int      inclass = 0;
static char    *last_cpptype = 0;
static int      inherit_list = 0;
static Parm    *template_parameters = 0;
static int      extendmode   = 0;
static int      dirprot_mode  = 0;

/* -----------------------------------------------------------------------------
 *                            Assist Functions
 * ----------------------------------------------------------------------------- */

/* Called by the parser (yyparse) when an error is found.*/
void yyerror (const char *e) {
}

static Node *new_node(const String_or_char *tag) {
  Node *n = NewHash();
  set_nodeType(n,tag);
  Setfile(n,cparse_file);
  Setline(n,cparse_line);
  return n;
}

/* Copies a node.  Does not copy tree links or symbol table data (except for
   sym:name) */

static Node *copy_node(Node *n) {
  Node *nn;
  String *key;
  Iterator k;
  nn = NewHash();
  Setfile(nn,Getfile(n));
  Setline(nn,Getline(n));
  for (k = First(n); k.key; k = Next(k)) {
    key = k.key;
    if ((Strcmp(key,"nextSibling") == 0) ||
	(Strcmp(key,"previousSibling") == 0) ||
	(Strcmp(key,"parentNode") == 0) ||
	(Strcmp(key,"lastChild") == 0)) {
      continue;
    }
    if (Strncmp(key,"csym:",5) == 0) continue;
    /* We do copy sym:name.  For templates */
    if ((Strcmp(key,"sym:name") == 0) || 
	(Strcmp(key,"sym:weak") == 0) ||
	(Strcmp(key,"sym:typename") == 0)) {
      Setattr(nn,key, Copy(k.item));
      continue;
    }
    if (Strcmp(key,"sym:symtab") == 0) {
      Setattr(nn,"sym:needs_symtab", "1");
    }
    /* We don't copy any other symbol table attributes */
    if (Strncmp(key,"sym:",4) == 0) {
      continue;
    }
    /* If children.  We copy them recursively using this function */
    if (Strcmp(key,"firstChild") == 0) {
      /* Copy children */
      Node *cn = k.item;
      while (cn) {
	appendChild(nn,copy_node(cn));
	cn = nextSibling(cn);
      }
      continue;
    }
    /* We don't copy the symbol table.  But we drop an attribute 
       requires_symtab so that functions know it needs to be built */

    if (Strcmp(key,"symtab") == 0) {
      /* Node defined a symbol table. */
      Setattr(nn,"requires_symtab","1");
      continue;
    }
    /* Can't copy nodes */
    if (Strcmp(key,"node") == 0) {
      continue;
    }
    if ((Strcmp(key,"parms") == 0) || (Strcmp(key,"pattern") == 0) || (Strcmp(key,"throws") == 0))  {
      Setattr(nn,key,CopyParmList(k.item));
      continue;
    }
    /* Looks okay.  Just copy the data using Copy */
    Setattr(nn, key, Copy(k.item));
  }
  return nn;
}

/* -----------------------------------------------------------------------------
 *                              Variables
 * ----------------------------------------------------------------------------- */

      char  *typemap_lang = 0;    /* Current language setting */

static int cplus_mode  = 0;
static String  *class_rename = 0;

/* C++ modes */

#define  CPLUS_PUBLIC    1
#define  CPLUS_PRIVATE   2
#define  CPLUS_PROTECTED 3

void SWIG_typemap_lang(const char *tm_lang) {
  typemap_lang = Swig_copy_string(tm_lang);
}

/* -----------------------------------------------------------------------------
 *                           Assist functions
 * ----------------------------------------------------------------------------- */

/* Perform type-promotion for binary operators */
static int promote(int t1, int t2) {
  return t1 > t2 ? t1 : t2;
}

static String *yyrename = 0;

/* Forward renaming operator */
static Hash   *rename_hash = 0;
static Hash   *namewarn_hash = 0;
static Hash   *features_hash = 0;

static String *feature_identifier_fix(String *s) {
  if (SwigType_istemplate(s)) {
    String *tp, *ts, *ta, *tq;
    tp = SwigType_templateprefix(s);
    ts = SwigType_templatesuffix(s);
    ta = SwigType_templateargs(s);
    tq = Swig_symbol_type_qualify(ta,0);
    Append(tp,tq);
    Append(tp,ts);
    Delete(ts);
    Delete(ta);
    Delete(tq);
    return tp;
  } else {
    return NewString(s);
  }
}

static void
rename_add(char *name, SwigType *decl, char *newname) {
  String *nname;
  if (!rename_hash) rename_hash = NewHash();
  if (Namespaceprefix) {
    nname = NewStringf("%s::%s",Namespaceprefix, name);
  } else {
    nname = NewString(name);
  }
  Swig_name_object_set(rename_hash,nname,decl,NewString(newname));
  Delete(nname);
}

static void
namewarn_add(char *name, SwigType *decl, char *warning) {
  String *nname;
  if (!namewarn_hash) namewarn_hash = NewHash();
  if (Namespaceprefix) {
    nname = NewStringf("%s::%s",Namespaceprefix, name);
  } else {
    nname = NewString(name);
  }

  Swig_name_object_set(namewarn_hash,nname,decl,NewString(warning));
  Delete(nname);
}

static void
rename_inherit(String *base, String *derived) {
  /*  Printf(stdout,"base = '%s', derived = '%s'\n", base, derived); */
  Swig_name_object_inherit(rename_hash,base,derived);
  Swig_name_object_inherit(namewarn_hash,base,derived);
  Swig_name_object_inherit(features_hash,base,derived);
}

/* Generate the symbol table name for an object */
/* This is a bit of a mess. Need to clean up */
static String *add_oldname = 0;

static String *make_name(String *name,SwigType *decl) {
  String *rn = 0;
  String *origname = name;
  int     destructor = 0;

  if (name && (*(Char(name)) == '~')) {
    destructor = 1;
  }
  if (yyrename) {
    String *s = yyrename;
    yyrename = 0;
    if (destructor) {
      Insert(s,0,"~");
    }
    return s;
  }


  if (!name) return 0;
  /* Check to see if the name is in the hash */
  if (!rename_hash) {
    if (add_oldname) return Copy(add_oldname);
    return origname;
  }
  rn = Swig_name_object_get(rename_hash, Namespaceprefix, name, decl);
  if (!rn) {
    if (add_oldname) return Copy(add_oldname);
    return name;
  }
  if (destructor) {
    if (Strcmp(rn,"$ignore") != 0) {
      String *s = NewStringf("~%s", rn);
      return s;
    }
  }
  return Copy(rn);
}

/* Generate an unnamed identifier */
static String *make_unnamed() {
  unnamed++;
  return NewStringf("$unnamed%d$",unnamed);
}

/* Generate the symbol table name for an object */
static String *name_warning(String *name,SwigType *decl) {
  String *rn = 0;
  if (!name) return 0;

  /* Check to see if the name is in the hash */
  if (!namewarn_hash) return 0;
  rn = Swig_name_object_get(namewarn_hash, Namespaceprefix,name,decl);
  if (!rn) return 0;
  return rn;
}

/* Add declaration list to symbol table */
static int  add_only_one = 0;

extern void cparse_normalize_void(Node *);
extern int need_protected(Node *n, int dirprot_mode);

static void add_symbols(Node *n) {
  String *decl;
  String *wrn = 0;

  if (inclass) {
    cparse_normalize_void(n);
  }

  /* Don't add symbols for private/protected members */
  if (inclass && (cplus_mode == CPLUS_PRIVATE)) {
    while (n) {
      Swig_symbol_add(0, n);       /* Add to C symbol table */
      Setattr(n,"access", "private");
      if (add_only_one) break;
      n = nextSibling(n);
    }
    return;
  }
  while (n) {
    String *symname;
    if (inclass && (cplus_mode == CPLUS_PROTECTED)) {
      Setattr(n,"access", "protected");
      if (!need_protected(n, dirprot_mode)) {
	/* Only add to C symbol table and continue */
	Swig_symbol_add(0, n); 
	if (add_only_one) break;
	n = nextSibling(n);
	continue;
      }
    }
    if (Getattr(n,"sym:name")) {
      n = nextSibling(n);
      continue;
    }
    decl = Getattr(n,"decl");
    if (!SwigType_isfunction(decl)) {
      symname = make_name(Getattr(n,"name"),0);
      if (!symname) {
	symname = Getattr(n,"unnamed");
      }
      if (symname) {
	wrn = name_warning(symname,0);
	Swig_features_get(features_hash, Namespaceprefix, Getattr(n,"name"), 0, n);
      }
    } else {
      SwigType *fdecl = Copy(decl);
      SwigType *fun = SwigType_pop_function(fdecl);
      symname = make_name(Getattr(n,"name"),fun);
      wrn = name_warning(symname,fun);
      
      Swig_features_get(features_hash,Namespaceprefix,Getattr(n,"name"),fun,n);
      Delete(fdecl);
      Delete(fun);
    }
    if (!symname) {
      n = nextSibling(n);
      continue;
    }
    if (strncmp(Char(symname),"$ignore",7) == 0) {
      char *c = Char(symname)+7;
      Setattr(n,"feature:ignore","1");
      if (strlen(c)) {
	Swig_warning(0,Getfile(n), Getline(n), "%s\n",c+1);
      }
      Swig_symbol_add(0, n);
    } else {
      Node *c;
      if ((wrn) && (Len(wrn))) {
	Swig_warning(0,Getfile(n),Getline(n), "%s\n", wrn);
      }
      if (Strcmp(nodeType(n),"enum") != 0) {
	c = Swig_symbol_add(symname,n);
	if (c != n) {
	  if (Getattr(n,"sym:weak")) {
	    Setattr(n,"sym:name",symname);
	  } else if ((Strcmp(nodeType(n),"template") == 0) && (Strcmp(Getattr(n,"templatetype"),"cdecl") == 0)) {
	    Setattr(n,"sym:name",symname);
	  } else {
	    String *e = NewString("");
	    Printf(e,"Identifier '%s' redeclared (ignored).", symname);
	    if (Cmp(symname,Getattr(n,"name"))) {
	      Printf(e," (Renamed from '%s')", SwigType_namestr(Getattr(n,"name")));
	    }
	    Printf(e,"\n%s:%d: Previous declaration of '%s'", Getfile(c),Getline(c),symname);
	    if (Cmp(symname,Getattr(c,"name"))) {
	      Printf(e," (Renamed from '%s')", SwigType_namestr(Getattr(c,"name")));
	    }
	    Swig_warning(WARN_PARSE_REDEFINED,Getfile(n), Getline(n),"%s\n", e);
	    Setattr(n,"error",e);
	  }
	}
      } else {
	Setattr(n,"sym:name", symname);
      }
    }
    if (add_only_one) return;
    n = nextSibling(n);
  }
}


/* add symbols a parse tree node copy */

void add_symbols_copy(Node *n) {
  String *name;
  int    emode = 0;

  while (n) {

    if (Strcmp(nodeType(n),"access") == 0) {
      String *kind = Getattr(n,"kind");
      if (Strcmp(kind,"public") == 0) {
	cplus_mode = CPLUS_PUBLIC;
      } else if (Strcmp(kind,"private") == 0) {
	cplus_mode = CPLUS_PRIVATE;
      } else if (Strcmp(kind,"protected") == 0) {
	cplus_mode = CPLUS_PROTECTED;
      }
      n = nextSibling(n);
      continue;
    }

    add_oldname = Getattr(n,"sym:name");
    if ((add_oldname) || (Getattr(n,"sym:needs_symtab"))) {
      if (add_oldname) {
	DohIncref(add_oldname);
	/* If already renamed, we used that name */
	if (Strcmp(add_oldname, Getattr(n,"name")) != 0) {
	  yyrename = add_oldname;
	}
      }
      Delattr(n,"sym:needs_symtab");
      Delattr(n,"sym:name");

      add_only_one = 1;
      add_symbols(n);

      if (Getattr(n,"partialargs")) {
	Swig_symbol_cadd(Getattr(n,"partialargs"),n);
      }
      add_only_one = 0;
      name = Getattr(n,"name");
      if (Getattr(n,"requires_symtab")) {
	Swig_symbol_newscope();
	Swig_symbol_setscopename(name);
	Namespaceprefix = Swig_symbol_qualifiedscopename(0);
      }
      if (Strcmp(nodeType(n),"class") == 0) {
	inclass = 1;
	if (Strcmp(Getattr(n,"kind"),"class") == 0) {
	  cplus_mode = CPLUS_PRIVATE;
	} else {
	  cplus_mode = CPLUS_PUBLIC;
	}
      }
      if (Strcmp(nodeType(n),"extend") == 0) {
	emode = cplus_mode;
	cplus_mode = CPLUS_PUBLIC;
      }
      add_symbols_copy(firstChild(n));
      if (Strcmp(nodeType(n),"extend") == 0) {
	cplus_mode = emode;
      }
      if (Getattr(n,"requires_symtab")) {
	Setattr(n,"symtab", Swig_symbol_popscope());
	Delattr(n,"requires_symtab");
	Namespaceprefix = Swig_symbol_qualifiedscopename(0);
      }
      if (add_oldname) {
	Delete(add_oldname);
      }
      if (Strcmp(nodeType(n),"class") == 0) {
	inclass = 0;
      }
      add_oldname = 0;
    } else {
      if (Strcmp(nodeType(n),"extend") == 0) {
	emode = cplus_mode;
	cplus_mode = CPLUS_PUBLIC;
      }
      add_symbols_copy(firstChild(n));
      if (Strcmp(nodeType(n),"extend") == 0) {
	cplus_mode = emode;
      }
    }
    n = nextSibling(n);
  }
}

/* Extension merge.  This function is used to handle the %extend directive
   when it appears before a class definition.   To handle this, the %extend
   actually needs to take precedence.  Therefore, we will selectively nuke symbols
   from the current symbol table, replacing them with the added methods */

static void merge_extensions(Node *cls, Node *am) {
  Node *n;
  Node *csym;

  n = firstChild(am);
  while (n) {
    String *symname;
    if (Strcmp(nodeType(n),"constructor") == 0) {
      symname = Getattr(n,"sym:name");
      if (symname) {
	if (Strcmp(symname,Getattr(n,"name")) == 0) {
	  /* If the name and the sym:name of a constructor are the same,
             then it hasn't been renamed.  However---the name of the class
             itself might have been renamed so we need to do a consistency
             check here */
	  if (Getattr(cls,"sym:name")) {
	    Setattr(n,"sym:name", Getattr(cls,"sym:name"));
	  }
	}
      } 
    }

    symname = Getattr(n,"sym:name");
    DohIncref(symname);
    if ((symname) && (!Getattr(n,"error"))) {
      /* Remove node from its symbol table */
      Swig_symbol_remove(n);
      csym = Swig_symbol_add(symname,n);
      if (csym != n) {
	/* Conflict with previous definition.  Nuke previous definition */
	String *e = NewString("");
	Printf(e,"Identifier '%s' redeclared (ignored).\n", symname);
	Printf(e,"%s:%d: Previous definition of tag '%s'", Getfile(n),Getline(n), symname);
	Swig_warning(WARN_PARSE_REDEFINED,Getfile(csym), Getline(csym), "%s\n", e);
	Setattr(csym,"error",e);
	Swig_symbol_remove(csym);              /* Remove class definition */
	Swig_symbol_add(symname,n);            /* Insert extend definition */
      }
    }
    n = nextSibling(n);
  }
}

/* Check for unused %extend.  Special case, don't report unused
   extensions for templates */
 
 static void check_extensions() {
   Iterator ki;

   if (!extendhash) return;
   for (ki = First(extendhash); ki.key; ki = Next(ki)) {
     if (!Strstr(ki.key,"<")) {
       Swig_warning(WARN_PARSE_EXTEND_UNDEF,Getfile(ki.item), Getline(ki.item), "%%extend defined for an undeclared class %s.\n", ki.key);
     }
   }
 }

/* Check a set of declarations to see if any are pure-abstract */

 static List *pure_abstract(Node *n) {
   List *abs = 0;
   while (n) {
     if (Cmp(nodeType(n),"cdecl") == 0) {
       String *decl = Getattr(n,"decl");
       if (SwigType_isfunction(decl)) {
	 String *init = Getattr(n,"value");
	 if (Cmp(init,"0") == 0) {
	   if (!abs) {
	     abs = NewList();
	   }
	   Append(abs,n);
	   Setattr(n,"abstract","1");
	 }
       }
     } else if (Cmp(nodeType(n),"destructor") == 0) {
       if (Cmp(Getattr(n,"value"),"0") == 0) {
	 if (!abs) {
	   abs = NewList();
	 }
	 Append(abs,n);
	 Setattr(n,"abstract","1");
       }
     }
     n = nextSibling(n);
   }
   return abs;
 }

 /* Make a classname */

 static String *make_class_name(String *name) {
   String *nname = 0;
   if (Namespaceprefix) {
     nname= NewStringf("%s::%s", Namespaceprefix, name);
   } else {
     nname = NewString(name);
   }
   if (SwigType_istemplate(nname)) {
     String *prefix, *args, *qargs;
     prefix = SwigType_templateprefix(nname);
     args   = SwigType_templateargs(nname);
     qargs  = Swig_symbol_type_qualify(args,0);
     Append(prefix,qargs);
     Delete(nname);
     nname = prefix;
   }
   return nname;
 }

 static List *make_inherit_list(String *clsname, List *names) {
   int i;
   String *derived;
   List *bases = NewList();

   if (Namespaceprefix) derived = NewStringf("%s::%s", Namespaceprefix,clsname);
   else derived = NewString(clsname);

   for (i = 0; i < Len(names); i++) {
     Node *s;
     String *base;
     String *n = Getitem(names,i);
     /* Try to figure out where this symbol is */
     s = Swig_symbol_clookup(n,0);
     if (s) {
       while (s && (Strcmp(nodeType(s),"class") != 0)) {
	 /* Not a class.  Could be a typedef though. */
	 String *storage = Getattr(s,"storage");
	 if (storage && (Strcmp(storage,"typedef") == 0)) {
	   String *nn = Getattr(s,"type");
	   s = Swig_symbol_clookup(nn,Getattr(s,"sym:symtab"));
	 } else {
	   break;
	 }
       }
       if (s && ((Strcmp(nodeType(s),"class") == 0) || (Strcmp(nodeType(s),"template") == 0))) {
	 String *q = Swig_symbol_qualified(s);
	 Append(bases,s);
	 if (q) {
	   base = NewStringf("%s::%s", q, Getattr(s,"name"));
	 } else {
	   base = NewString(Getattr(s,"name"));
	 }
       } else {
	 base = NewString(n);
       }
     } else {
       base = NewString(n);
     }
     if (base) {
       rename_inherit(base,derived);
       Delete(base);
     }
   }
   return bases;
 }

/* Structures for handling code fragments built for nested classes */

typedef struct Nested {
  String   *code;        /* Associated code fragment */
  int      line;         /* line number where it starts */
  char     *name;        /* Name associated with this nested class */
  char     *kind;        /* Kind of class */
  SwigType *type;        /* Datatype associated with the name */
  struct Nested   *next;        /* Next code fragment in list */
} Nested;

/* Some internal variables for saving nested class information */

static Nested      *nested_list = 0;

/* Add a function to the nested list */

static void add_nested(Nested *n) {
  Nested *n1;
  if (!nested_list) nested_list = n;
  else {
    n1 = nested_list;
    while (n1->next) n1 = n1->next;
    n1->next = n;
  }
}

/* Dump all of the nested class declarations to the inline processor
 * However.  We need to do a few name replacements and other munging
 * first.  This function must be called before closing a class! */

static Node *dump_nested(char *parent) {
  Nested *n,*n1;
  Node *ret = 0;
  n = nested_list;
  if (!parent) {
    nested_list = 0;
    return 0;
  }
  while (n) {
    char temp[256];
    Node *retx;
    /* Token replace the name of the parent class */
    Replace(n->code, "$classname", parent, DOH_REPLACE_ANY);
    /* Fix up the name of the datatype (for building typedefs and other stuff) */
    sprintf(temp,"%s_%s", parent,n->name);

    Append(n->type,parent);
    Append(n->type,"_");
    Append(n->type,n->name);

    /* Add the appropriate declaration to the C++ processor */
    retx = new_node("cdecl");
    Setattr(retx,"name",n->name);
    Setattr(retx,"type",Copy(n->type));
    Setattr(retx,"nested",parent);
    add_symbols(retx);
    if (ret) {
      set_nextSibling(retx,ret);
    }
    ret = retx;

    /* Insert a forward class declaration */
    /* Disabled: [ 597599 ] union in class: incorrect scope 
    retx = new_node("classforward");
    Setattr(retx,"kind",n->kind);
    Setattr(retx,"name",Copy(n->type));
    Setattr(retx,"sym:name", make_name(n->type,0));
    set_nextSibling(retx,ret);
    ret = retx; 
    */

    /* Make all SWIG created typedef structs/unions/classes unnamed else 
       redefinition errors occur - nasty hack alert.*/

    {
      const char* types_array[3] = {"struct", "union", "class"};
      int i;
      for (i=0; i<3; i++) {
	char* code_ptr = Char(n->code);
      while (code_ptr) {
        /* Replace struct name (as in 'struct name {' ) with whitespace
           name will be between struct and { */
	
        code_ptr = strstr(code_ptr, types_array[i]);
        if (code_ptr) {
	  char *open_bracket_pos;
          code_ptr += strlen(types_array[i]);
          open_bracket_pos = strstr(code_ptr, "{");
          if (open_bracket_pos) { 
            /* Make sure we don't have something like struct A a; */
            char* semi_colon_pos = strstr(code_ptr, ";");
            if (!(semi_colon_pos && (semi_colon_pos < open_bracket_pos)))
              while (code_ptr < open_bracket_pos)
                *code_ptr++ = ' ';
          }
        }
      }
      }
    }
    
    {
      /* Remove SWIG directive %constant which may be left in the SWIG created typedefs */
      char* code_ptr = Char(n->code);
      while (code_ptr) {
	code_ptr = strstr(code_ptr, "%constant");
	if (code_ptr) {
	  char* directive_end_pos = strstr(code_ptr, ";");
	  if (directive_end_pos) { 
            while (code_ptr <= directive_end_pos)
              *code_ptr++ = ' ';
	  }
	}
      }
    }
    {
      Node *head;
      head = new_node("insert");
      Setattr(head,"code",NewStringf("\n%s\n",n->code));
      set_nextSibling(head,ret);
      ret = head;
    }
      
    /* Dump the code to the scanner */
    start_inline(Char(n->code),n->line);

    n1 = n->next;
    Delete(n->code);
    free(n);
    n = n1;
  }
  nested_list = 0;
  return ret;
}

Node *Swig_cparse(File *f) {
  extern void scanner_file(File *);
  extern int yyparse();
  scanner_file(f);
  top = 0;
  yyparse();
  return top;
}


#line 804 "parser.y"
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
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		815
#define	YYFLAG		-32768
#define	YYNTBASE	112

#define YYTRANSLATE(x) ((unsigned)(x) <= 365 ? yytranslate[x] : 248)

static const char yytranslate[] = {     0,
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
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
    87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
    97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
   107,   108,   109,   110,   111
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,     9,    13,    16,    19,    21,    23,    25,
    27,    29,    31,    33,    36,    38,    40,    42,    44,    46,
    48,    50,    52,    54,    56,    58,    60,    62,    64,    66,
    68,    70,    72,    74,    76,    78,    79,    87,    93,    97,
   103,   109,   113,   116,   119,   125,   128,   134,   137,   145,
   153,   154,   162,   164,   166,   169,   172,   174,   180,   186,
   192,   196,   201,   205,   213,   222,   228,   232,   234,   236,
   240,   242,   247,   255,   262,   264,   266,   274,   284,   290,
   298,   300,   302,   308,   316,   318,   322,   329,   336,   345,
   347,   350,   354,   356,   359,   363,   370,   376,   386,   389,
   391,   393,   399,   405,   407,   412,   414,   416,   419,   425,
   432,   440,   449,   456,   458,   460,   462,   464,   466,   468,
   469,   479,   480,   489,   491,   494,   499,   500,   507,   511,
   513,   515,   517,   519,   521,   523,   527,   532,   533,   540,
   541,   547,   553,   556,   557,   564,   566,   567,   571,   573,
   575,   577,   579,   581,   583,   585,   587,   591,   593,   595,
   597,   599,   601,   603,   605,   612,   619,   627,   636,   645,
   653,   659,   662,   665,   668,   669,   677,   678,   685,   687,
   689,   691,   693,   695,   697,   699,   701,   703,   705,   707,
   709,   711,   714,   717,   720,   725,   728,   734,   736,   739,
   741,   743,   745,   747,   749,   751,   754,   756,   760,   762,
   765,   772,   776,   778,   781,   783,   787,   789,   791,   793,
   795,   798,   802,   805,   808,   810,   813,   816,   818,   820,
   822,   824,   827,   831,   833,   836,   840,   845,   851,   856,
   858,   861,   865,   870,   876,   880,   885,   890,   892,   895,
   900,   905,   911,   915,   920,   925,   927,   930,   933,   937,
   939,   942,   944,   947,   951,   956,   960,   965,   968,   972,
   976,   981,   985,   989,   992,   995,   997,   999,  1002,  1004,
  1006,  1008,  1011,  1013,  1015,  1017,  1019,  1022,  1025,  1027,
  1030,  1032,  1035,  1037,  1039,  1042,  1044,  1046,  1048,  1050,
  1052,  1054,  1056,  1058,  1059,  1062,  1064,  1066,  1068,  1072,
  1074,  1076,  1080,  1082,  1084,  1086,  1088,  1090,  1096,  1098,
  1100,  1104,  1109,  1115,  1121,  1128,  1130,  1132,  1134,  1136,
  1138,  1140,  1142,  1146,  1150,  1154,  1158,  1162,  1166,  1170,
  1174,  1178,  1182,  1186,  1189,  1192,  1195,  1198,  1201,  1203,
  1204,  1208,  1210,  1212,  1216,  1219,  1224,  1226,  1228,  1230,
  1232,  1234,  1236,  1238,  1240,  1242,  1244,  1249,  1255,  1257,
  1261,  1265,  1270,  1275,  1279,  1282,  1284,  1286,  1290,  1293,
  1297,  1299,  1301,  1303,  1305,  1307,  1310,  1315,  1317,  1321,
  1323,  1327,  1331,  1334,  1337,  1340,  1343,  1346,  1351,  1353,
  1357,  1359,  1363,  1367,  1370,  1373,  1376,  1379,  1381,  1383,
  1385,  1387,  1391,  1393,  1397,  1403,  1405,  1409,  1411,  1413
};

static const short yyrhs[] = {   113,
     0,    94,   190,    34,     0,    94,     1,     0,    95,   190,
    34,     0,    95,     1,     0,   113,   114,     0,   247,     0,
   115,     0,   149,     0,   155,     0,    34,     0,     1,     0,
   154,     0,     1,    93,     0,   116,     0,   118,     0,   119,
     0,   120,     0,   121,     0,   122,     0,   123,     0,   124,
     0,   127,     0,   128,     0,   129,     0,   130,     0,   131,
     0,   132,     0,   135,     0,   137,     0,   139,     0,   141,
     0,   146,     0,   147,     0,   148,     0,     0,    54,   244,
   237,    37,   117,   169,    38,     0,    77,   145,    37,   143,
    38,     0,    78,   143,    34,     0,    50,     3,    44,   212,
    34,     0,    50,   206,   198,   195,    34,     0,    50,     1,
    34,     0,    76,     4,     0,    76,   242,     0,    75,    31,
     3,    32,    37,     0,    75,    37,     0,    75,    31,     3,
    32,    34,     0,    75,    34,     0,    80,    31,   235,    33,
   235,    32,     4,     0,    80,    31,   235,    33,   235,    32,
    37,     0,     0,   126,   244,   242,    47,   125,   113,    48,
     0,     7,     0,     8,     0,    73,     4,     0,    73,    37,
     0,     4,     0,     9,    31,   235,    32,   242,     0,     9,
    31,   235,    32,     4,     0,     9,    31,   235,    32,    37,
     0,    46,   244,   235,     0,    51,    31,   235,    32,     0,
    51,    31,    32,     0,    72,    31,     3,    32,   186,     3,
    34,     0,    72,    31,     3,    32,   186,   206,   198,    34,
     0,    55,   134,     3,    44,   133,     0,    55,   134,     3,
     0,   242,     0,     4,     0,    31,     3,    32,     0,   247,
     0,   136,   198,   235,    34,     0,   136,    31,   235,    32,
   198,   229,    34,     0,   136,    31,   235,    32,   242,    34,
     0,    52,     0,    53,     0,    56,    31,   235,    32,   198,
   229,   138,     0,    56,    31,   235,    33,   235,    32,   198,
   229,    34,     0,    56,    31,   235,    32,   138,     0,    56,
    31,   235,    33,   235,    32,    34,     0,   243,     0,    34,
     0,    87,    31,   187,    32,    34,     0,    57,    31,   140,
    32,   198,   229,    34,     0,   187,     0,    11,    33,   190,
     0,    74,    31,   142,    32,   143,   243,     0,    74,    31,
   142,    32,   143,    34,     0,    74,    31,   142,    32,   143,
    44,   145,    34,     0,   245,     0,   145,   144,     0,    33,
   145,   144,     0,   247,     0,   206,   197,     0,    31,   187,
    32,     0,    31,   187,    32,    31,   187,    32,     0,    86,
    31,   187,    32,    34,     0,    79,    31,   236,    32,   240,
    82,   191,    83,    34,     0,    81,   242,     0,   150,     0,
   153,     0,    35,   242,    37,   113,    38,     0,   186,   206,
   198,   152,   151,     0,    34,     0,    33,   198,   152,   151,
     0,    37,     0,   195,     0,   204,   195,     0,    68,    31,
   187,    32,   195,     0,   204,    68,    31,   187,    32,   195,
     0,   186,    58,   214,    37,   215,    38,    34,     0,   186,
    58,   214,    37,   215,    38,   198,   151,     0,   186,   206,
    31,   187,    32,   230,     0,   156,     0,   160,     0,   161,
     0,   165,     0,   166,     0,   176,     0,     0,   186,   227,
   237,   221,    37,   157,   169,    38,   159,     0,     0,   186,
   227,    37,   158,   169,    38,   198,   151,     0,    34,     0,
   198,   151,     0,   186,   227,   237,    34,     0,     0,    91,
    82,   164,    83,   162,   163,     0,    91,   227,   237,     0,
   150,     0,   156,     0,   173,     0,   161,     0,   160,     0,
   188,     0,    70,   237,    34,     0,    70,    71,   237,    34,
     0,     0,    71,   237,    37,   167,   113,    38,     0,     0,
    71,    37,   168,   113,    38,     0,    71,     3,    44,   237,
    34,     0,   172,   169,     0,     0,    54,    37,   170,   169,
    38,   169,     0,   247,     0,     0,     1,   171,   169,     0,
   149,     0,   173,     0,   174,     0,   177,     0,   182,     0,
   175,     0,   160,     0,   178,     0,   186,   237,    34,     0,
   165,     0,   161,     0,   176,     0,   147,     0,   148,     0,
   185,     0,    34,     0,   186,   206,    31,   187,    32,   230,
     0,   109,   239,    31,   187,    32,   183,     0,    66,   109,
   239,    31,   187,    32,   184,     0,   186,    93,   206,   203,
    31,   187,    32,   184,     0,   186,    93,   206,   101,    31,
   187,    32,   184,     0,   186,    93,   206,    31,   187,    32,
   184,     0,    69,    31,   187,    32,    37,     0,    62,    64,
     0,    61,    64,     0,    63,    64,     0,     0,   186,   227,
     3,    37,   179,   181,    34,     0,     0,   186,   227,    37,
   180,   181,    34,     0,   198,     0,   247,     0,   132,     0,
   120,     0,   130,     0,   135,     0,   137,     0,   139,     0,
   128,     0,   141,     0,   118,     0,   119,     0,   121,     0,
   229,    34,     0,   229,    37,     0,   229,    34,     0,   229,
    44,   212,    34,     0,   229,    37,     0,   186,   206,    64,
    11,    34,     0,    35,     0,    35,   242,     0,    65,     0,
    18,     0,    66,     0,    67,     0,   247,     0,   188,     0,
   190,   189,     0,   247,     0,    33,   190,   189,     0,   247,
     0,   207,   196,     0,    91,    82,   227,    83,   227,   237,
     0,    39,    39,    39,     0,   192,     0,   194,   193,     0,
   247,     0,    33,   194,   193,     0,   247,     0,   190,     0,
   219,     0,     6,     0,    44,   212,     0,    44,   101,   198,
     0,    44,    37,     0,    64,    11,     0,   247,     0,   198,
   195,     0,   201,   195,     0,   195,     0,   198,     0,   201,
     0,   247,     0,   203,   199,     0,   203,   101,   199,     0,
   200,     0,   101,   199,     0,   237,    89,   199,     0,   203,
   237,    89,   199,     0,   203,   237,    89,   101,   199,     0,
   237,    89,   101,   199,     0,   237,     0,   109,   237,     0,
    31,   237,    32,     0,    31,   203,   199,    32,     0,    31,
   237,    89,   199,    32,     0,   199,    47,    48,     0,   199,
    47,   218,    48,     0,   199,    31,   187,    32,     0,   237,
     0,   109,   237,     0,    31,   203,   200,    32,     0,    31,
   101,   200,    32,     0,    31,   237,    89,   200,    32,     0,
   200,    47,    48,     0,   200,    47,   218,    48,     0,   200,
    31,   187,    32,     0,   203,     0,   203,   202,     0,   203,
   101,     0,   203,   101,   202,     0,   202,     0,   101,   202,
     0,   101,     0,   237,    89,     0,   203,   237,    89,     0,
   203,   237,    89,   202,     0,   202,    47,    48,     0,   202,
    47,   218,    48,     0,    47,    48,     0,    47,   218,    48,
     0,    31,   201,    32,     0,   202,    31,   187,    32,     0,
    31,   187,    32,     0,   106,   204,   203,     0,   106,   203,
     0,   106,   204,     0,   106,     0,   205,     0,   205,   204,
     0,    40,     0,    41,     0,   207,     0,   204,   208,     0,
   208,     0,   209,     0,    28,     0,    26,     0,    29,   234,
     0,    58,   237,     0,    30,     0,   208,   204,     0,   237,
     0,   227,   237,     0,   210,     0,   211,     0,   211,   210,
     0,    19,     0,    21,     0,    22,     0,    25,     0,    23,
     0,    24,     0,    27,     0,    20,     0,     0,   213,   218,
     0,    10,     0,     3,     0,   247,     0,   215,    33,   216,
     0,   216,     0,     3,     0,     3,    44,   217,     0,   247,
     0,   218,     0,    10,     0,   219,     0,   242,     0,    45,
    31,   206,   196,    32,     0,   220,     0,   206,     0,    31,
   218,    32,     0,    31,   218,    32,   218,     0,    31,   218,
   203,    32,   218,     0,    31,   218,   101,    32,   218,     0,
    31,   218,   203,   101,    32,   218,     0,    11,     0,    12,
     0,    13,     0,    14,     0,    15,     0,    16,     0,    17,
     0,   218,   104,   218,     0,   218,   105,   218,     0,   218,
   106,   218,     0,   218,   107,   218,     0,   218,   101,   218,
     0,   218,    99,   218,     0,   218,   100,   218,     0,   218,
   102,   218,     0,   218,   103,   218,     0,   218,    98,   218,
     0,   218,    97,   218,     0,   105,   218,     0,   104,   218,
     0,   109,   218,     0,   110,   218,     0,   206,    31,     0,
   222,     0,     0,    64,   223,   224,     0,   247,     0,   225,
     0,   224,    33,   225,     0,   228,   237,     0,   228,   226,
   228,   237,     0,    62,     0,    61,     0,    63,     0,    59,
     0,    42,     0,    43,     0,    60,     0,    66,     0,   247,
     0,   204,     0,    68,    31,   187,    32,     0,   204,    68,
    31,   187,    32,     0,   247,     0,   229,   231,    34,     0,
   229,   231,    37,     0,    31,   187,    32,    34,     0,    31,
   187,    32,    37,     0,    44,   212,    34,     0,    64,   232,
     0,   247,     0,   233,     0,   232,    33,   233,     0,   237,
    31,     0,    82,   191,    83,     0,   247,     0,     3,     0,
   242,     0,   235,     0,   247,     0,   239,   238,     0,    88,
   111,   239,   238,     0,   239,     0,    88,   111,   239,     0,
    92,     0,    88,   111,    92,     0,   111,   239,   238,     0,
   111,   239,     0,   111,    92,     0,    90,   239,     0,     3,
   234,     0,     3,   241,     0,    88,   111,     3,   241,     0,
     3,     0,    88,   111,     3,     0,    92,     0,    88,   111,
    92,     0,   111,     3,   241,     0,   111,     3,     0,   111,
    92,     0,    90,     3,     0,   242,     6,     0,     6,     0,
   242,     0,    37,     0,     4,     0,    31,   245,    32,     0,
   247,     0,   235,    44,   246,     0,   235,    44,   246,    33,
   245,     0,   235,     0,   235,    33,   245,     0,   242,     0,
   219,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   946,   958,   961,   964,   967,   972,   976,   981,   982,   983,
   984,   985,   997,  1013,  1023,  1024,  1025,  1026,  1027,  1028,
  1029,  1030,  1031,  1032,  1033,  1034,  1035,  1036,  1037,  1038,
  1039,  1040,  1041,  1042,  1043,  1050,  1078,  1122,  1132,  1143,
  1161,  1182,  1193,  1202,  1221,  1227,  1233,  1238,  1248,  1254,
  1270,  1275,  1297,  1298,  1305,  1325,  1352,  1356,  1366,  1371,
  1386,  1402,  1406,  1418,  1424,  1450,  1456,  1463,  1464,  1467,
  1468,  1476,  1487,  1531,  1542,  1545,  1558,  1605,  1657,  1668,
  1681,  1682,  1683,  1688,  1724,  1725,  1754,  1766,  1774,  1787,
  1809,  1815,  1819,  1822,  1830,  1835,  1847,  1857,  2102,  2112,
  2118,  2122,  2138,  2191,  2195,  2214,  2220,  2225,  2230,  2235,
  2247,  2255,  2303,  2350,  2351,  2352,  2353,  2354,  2355,  2358,
  2416,  2495,  2507,  2593,  2594,  2606,  2624,  2624,  2837,  2843,
  2846,  2849,  2852,  2855,  2860,  2892,  2898,  2925,  2942,  2952,
  2963,  2972,  2999,  3013,  3017,  3023,  3024,  3033,  3044,  3045,
  3059,  3060,  3061,  3062,  3063,  3064,  3065,  3066,  3067,  3068,
  3069,  3070,  3071,  3072,  3081,  3102,  3118,  3151,  3165,  3182,
  3200,  3207,  3214,  3222,  3245,  3246,  3274,  3275,  3301,  3302,
  3308,  3311,  3315,  3318,  3319,  3320,  3321,  3322,  3323,  3324,
  3325,  3328,  3331,  3334,  3341,  3348,  3358,  3364,  3365,  3373,
  3374,  3375,  3376,  3377,  3384,  3395,  3403,  3406,  3410,  3414,
  3424,  3429,  3437,  3450,  3458,  3461,  3465,  3469,  3495,  3501,
  3509,  3519,  3547,  3555,  3562,  3571,  3575,  3579,  3586,  3603,
  3620,  3628,  3636,  3645,  3649,  3658,  3669,  3681,  3691,  3704,
  3711,  3719,  3735,  3743,  3754,  3765,  3776,  3795,  3803,  3820,
  3828,  3835,  3846,  3857,  3868,  3887,  3893,  3899,  3906,  3915,
  3918,  3927,  3934,  3941,  3951,  3962,  3973,  3984,  3991,  3998,
  4001,  4018,  4028,  4035,  4041,  4046,  4052,  4056,  4062,  4063,
  4069,  4075,  4079,  4082,  4085,  4086,  4087,  4088,  4089,  4090,
  4095,  4098,  4103,  4128,  4131,  4173,  4177,  4181,  4185,  4189,
  4193,  4197,  4201,  4207,  4207,  4226,  4243,  4244,  4249,  4262,
  4271,  4277,  4290,  4293,  4302,  4313,  4314,  4318,  4323,  4324,
  4348,  4355,  4361,  4368,  4375,  4385,  4386,  4387,  4388,  4389,
  4390,  4391,  4394,  4398,  4402,  4406,  4410,  4414,  4418,  4422,
  4426,  4430,  4434,  4438,  4442,  4446,  4450,  4454,  4465,  4470,
  4470,  4471,  4474,  4479,  4485,  4495,  4507,  4508,  4509,  4513,
  4517,  4521,  4525,  4531,  4532,  4535,  4539,  4543,  4547,  4553,
  4559,  4565,  4572,  4579,  4586,  4587,  4590,  4591,  4594,  4600,
  4606,  4609,  4610,  4613,  4614,  4617,  4622,  4626,  4629,  4632,
  4635,  4640,  4644,  4647,  4654,  4660,  4669,  4674,  4678,  4681,
  4684,  4687,  4692,  4696,  4699,  4702,  4708,  4713,  4716,  4719,
  4723,  4728,  4741,  4745,  4750,  4756,  4760,  4767,  4770,  4775
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","ID","HBLOCK",
"POUND","STRING","INCLUDE","IMPORT","INSERT","CHARCONST","NUM_INT","NUM_FLOAT",
"NUM_UNSIGNED","NUM_LONG","NUM_ULONG","NUM_LONGLONG","NUM_ULONGLONG","TYPEDEF",
"TYPE_INT","TYPE_UNSIGNED","TYPE_SHORT","TYPE_LONG","TYPE_FLOAT","TYPE_DOUBLE",
"TYPE_CHAR","TYPE_VOID","TYPE_SIGNED","TYPE_BOOL","TYPE_TYPEDEF","TYPE_RAW",
"LPAREN","RPAREN","COMMA","SEMI","EXTERN","INIT","LBRACE","RBRACE","PERIOD",
"CONST","VOLATILE","STRUCT","UNION","EQUAL","SIZEOF","MODULE","LBRACKET","RBRACKET",
"ILLEGAL","CONSTANT","NAME","RENAME","NAMEWARN","EXTEND","PRAGMA","FEATURE",
"VARARGS","ENUM","CLASS","TYPENAME","PRIVATE","PUBLIC","PROTECTED","COLON","STATIC",
"VIRTUAL","FRIEND","THROW","CATCH","USING","NAMESPACE","NATIVE","INLINE","TYPEMAP",
"EXCEPT","ECHO","APPLY","CLEAR","SWIGTEMPLATE","FRAGMENT","WARN","LESSTHAN",
"GREATERTHAN","MODULO","DELETE","TYPES","PARMS","NONID","DSTAR","DCNOT","TEMPLATE",
"OPERATOR","COPERATOR","PARSETYPE","PARSEPARM","CAST","LOR","LAND","OR","XOR",
"AND","LSHIFT","RSHIFT","PLUS","MINUS","STAR","SLASH","UMINUS","NOT","LNOT",
"DCOLON","program","interface","declaration","swig_directive","extend_directive",
"@1","apply_directive","clear_directive","constant_directive","echo_directive",
"except_directive","fragment_directive","include_directive","@2","includetype",
"inline_directive","insert_directive","module_directive","name_directive","native_directive",
"pragma_directive","pragma_arg","pragma_lang","rename_directive","rename_namewarn",
"feature_directive","stringbracesemi","varargs_directive","varargs_parms","typemap_directive",
"typemap_type","tm_list","tm_tail","typemap_parm","types_directive","template_directive",
"warn_directive","c_declaration","c_decl","c_decl_tail","initializer","c_enum_decl",
"c_constructor_decl","cpp_declaration","cpp_class_decl","@3","@4","cpp_opt_declarators",
"cpp_forward_class_decl","cpp_template_decl","@5","cpp_temp_possible","template_parms",
"cpp_using_decl","cpp_namespace_decl","@6","@7","cpp_members","@8","@9","cpp_member",
"cpp_constructor_decl","cpp_destructor_decl","cpp_conversion_operator","cpp_catch_decl",
"cpp_protection_decl","cpp_nested","@10","@11","nested_decl","cpp_swig_directive",
"cpp_end","cpp_vend","anonymous_bitfield","storage_class","parms","rawparms",
"ptail","parm","valparms","rawvalparms","valptail","valparm","def_args","parameter_declarator",
"typemap_parameter_declarator","declarator","notso_direct_declarator","direct_declarator",
"abstract_declarator","direct_abstract_declarator","pointer","type_qualifier",
"type_qualifier_raw","type","rawtype","type_right","primitive_type","primitive_type_list",
"type_specifier","definetype","@12","ename","enumlist","edecl","etype","expr",
"exprnum","exprcompound","inherit","raw_inherit","@13","base_list","base_specifier",
"access_specifier","cpptype","opt_virtual","cpp_const","ctor_end","ctor_initializer",
"mem_initializer_list","mem_initializer","template_decl","idstring","idstringopt",
"idcolon","idcolontail","idtemplate","idcolonnt","idcolontailnt","string","stringbrace",
"options","kwargs","stringnum","empty", NULL
};
#endif

static const short yyr1[] = {     0,
   112,   112,   112,   112,   112,   113,   113,   114,   114,   114,
   114,   114,   114,   114,   115,   115,   115,   115,   115,   115,
   115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
   115,   115,   115,   115,   115,   117,   116,   118,   119,   120,
   120,   120,   121,   121,   122,   122,   122,   122,   123,   123,
   125,   124,   126,   126,   127,   127,   128,   128,   128,   128,
   129,   130,   130,   131,   131,   132,   132,   133,   133,   134,
   134,   135,   135,   135,   136,   136,   137,   137,   137,   137,
   138,   138,   138,   139,   140,   140,   141,   141,   141,   142,
   143,   144,   144,   145,   145,   145,   146,   147,   148,   149,
   149,   149,   150,   151,   151,   151,   152,   152,   152,   152,
   153,   153,   154,   155,   155,   155,   155,   155,   155,   157,
   156,   158,   156,   159,   159,   160,   162,   161,   161,   163,
   163,   163,   163,   163,   164,   165,   165,   167,   166,   168,
   166,   166,   169,   170,   169,   169,   171,   169,   172,   172,
   172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
   172,   172,   172,   172,   173,   174,   174,   175,   175,   175,
   176,   177,   177,   177,   179,   178,   180,   178,   181,   181,
   182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
   182,   183,   183,   184,   184,   184,   185,   186,   186,   186,
   186,   186,   186,   186,   187,   188,   188,   189,   189,   190,
   190,   190,   191,   192,   192,   193,   193,   194,   194,   194,
   195,   195,   195,   195,   195,   196,   196,   196,   197,   197,
   197,   198,   198,   198,   198,   198,   198,   198,   198,   199,
   199,   199,   199,   199,   199,   199,   199,   200,   200,   200,
   200,   200,   200,   200,   200,   201,   201,   201,   201,   201,
   201,   201,   201,   201,   201,   202,   202,   202,   202,   202,
   202,   202,   203,   203,   203,   203,   204,   204,   205,   205,
   206,   207,   207,   208,   208,   208,   208,   208,   208,   208,
   208,   208,   209,   210,   210,   211,   211,   211,   211,   211,
   211,   211,   211,   213,   212,   212,   214,   214,   215,   215,
   216,   216,   216,   217,   217,   218,   218,   218,   218,   218,
   218,   218,   218,   218,   218,   219,   219,   219,   219,   219,
   219,   219,   220,   220,   220,   220,   220,   220,   220,   220,
   220,   220,   220,   220,   220,   220,   220,   220,   221,   223,
   222,   222,   224,   224,   225,   225,   226,   226,   226,   227,
   227,   227,   227,   228,   228,   229,   229,   229,   229,   230,
   230,   230,   230,   230,   231,   231,   232,   232,   233,   234,
   234,   235,   235,   236,   236,   237,   237,   237,   237,   237,
   237,   238,   238,   238,   238,   239,   240,   240,   240,   240,
   240,   240,   241,   241,   241,   241,   242,   242,   243,   243,
   243,   244,   244,   245,   245,   245,   245,   246,   246,   247
};

static const short yyr2[] = {     0,
     1,     3,     2,     3,     2,     2,     1,     1,     1,     1,
     1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     0,     7,     5,     3,     5,
     5,     3,     2,     2,     5,     2,     5,     2,     7,     7,
     0,     7,     1,     1,     2,     2,     1,     5,     5,     5,
     3,     4,     3,     7,     8,     5,     3,     1,     1,     3,
     1,     4,     7,     6,     1,     1,     7,     9,     5,     7,
     1,     1,     5,     7,     1,     3,     6,     6,     8,     1,
     2,     3,     1,     2,     3,     6,     5,     9,     2,     1,
     1,     5,     5,     1,     4,     1,     1,     2,     5,     6,
     7,     8,     6,     1,     1,     1,     1,     1,     1,     0,
     9,     0,     8,     1,     2,     4,     0,     6,     3,     1,
     1,     1,     1,     1,     1,     3,     4,     0,     6,     0,
     5,     5,     2,     0,     6,     1,     0,     3,     1,     1,
     1,     1,     1,     1,     1,     1,     3,     1,     1,     1,
     1,     1,     1,     1,     6,     6,     7,     8,     8,     7,
     5,     2,     2,     2,     0,     7,     0,     6,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     2,     2,     2,     4,     2,     5,     1,     2,     1,
     1,     1,     1,     1,     1,     2,     1,     3,     1,     2,
     6,     3,     1,     2,     1,     3,     1,     1,     1,     1,
     2,     3,     2,     2,     1,     2,     2,     1,     1,     1,
     1,     2,     3,     1,     2,     3,     4,     5,     4,     1,
     2,     3,     4,     5,     3,     4,     4,     1,     2,     4,
     4,     5,     3,     4,     4,     1,     2,     2,     3,     1,
     2,     1,     2,     3,     4,     3,     4,     2,     3,     3,
     4,     3,     3,     2,     2,     1,     1,     2,     1,     1,
     1,     2,     1,     1,     1,     1,     2,     2,     1,     2,
     1,     2,     1,     1,     2,     1,     1,     1,     1,     1,
     1,     1,     1,     0,     2,     1,     1,     1,     3,     1,
     1,     3,     1,     1,     1,     1,     1,     5,     1,     1,
     3,     4,     5,     5,     6,     1,     1,     1,     1,     1,
     1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     2,     2,     2,     2,     2,     1,     0,
     3,     1,     1,     3,     2,     4,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     4,     5,     1,     3,
     3,     4,     4,     3,     2,     1,     1,     3,     2,     3,
     1,     1,     1,     1,     1,     2,     4,     1,     3,     1,
     3,     3,     2,     2,     2,     2,     2,     4,     1,     3,
     1,     3,     3,     2,     2,     2,     2,     1,     1,     1,
     1,     3,     1,     3,     5,     1,     3,     1,     1,     0
};

static const short yydefact[] = {   420,
     0,     0,     0,     7,     3,   420,   296,   303,   297,   298,
   300,   301,   299,   286,   302,   285,   420,   289,     0,   279,
   280,   361,   362,     0,   360,   363,     0,     0,   390,     0,
     0,   277,   420,   283,   284,   293,   294,     0,   291,   388,
     5,     0,    12,    57,    53,    54,     0,   201,    11,   198,
   420,     0,     0,    75,    76,   420,   420,     0,     0,   200,
   202,   203,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     6,     8,    15,
    16,    17,    18,    19,    20,    21,    22,   420,    23,    24,
    25,    26,    27,    28,    29,     0,    30,    31,    32,    33,
    34,    35,     9,   100,   101,    13,    10,   114,   115,   116,
   117,   118,   119,     0,   204,   420,   396,   381,   287,     0,
   288,     0,     0,     2,   282,   278,   420,   304,     0,     0,
   262,   276,     0,   228,   210,   420,   234,   420,   260,   256,
   248,   225,   290,   295,   292,     0,     0,   386,     4,    14,
     0,   408,   199,     0,     0,   413,     0,   420,     0,   281,
     0,     0,     0,     0,    71,     0,   420,   420,     0,     0,
   420,   140,     0,     0,    55,    56,     0,     0,    48,    46,
    43,    44,   420,     0,   420,     0,   420,   420,     0,    99,
   420,   420,     0,     0,     0,     0,     0,     0,   248,   420,
     0,     0,   220,   326,   327,   328,   329,   330,   331,   332,
   218,     0,   213,   420,   219,   215,   212,   391,   389,     0,
   420,   262,     0,   205,   420,     0,   256,   291,   207,   306,
   223,     0,   221,     0,     0,     0,   268,     0,     0,     0,
     0,   320,     0,   316,   319,   317,   224,   420,     0,   235,
   261,   240,   274,   275,   249,   226,   420,     0,   227,   420,
     0,   258,   232,   257,   240,   263,   395,   394,   393,   382,
     0,   383,   407,   420,   416,     0,    61,    42,   304,     0,
   420,    63,     0,     0,     0,    67,     0,     0,     0,    85,
     0,     0,   136,     0,   420,   138,     0,     0,    90,     0,
     0,     0,    94,   229,   230,   231,    39,     0,    91,    93,
   384,     0,   385,     0,     0,     0,   135,   129,     0,   420,
     0,     0,     0,     0,     0,     0,     0,   240,     0,   420,
     0,   308,   420,   420,   122,   292,   380,     0,   214,   217,
   387,     0,   262,   256,   291,     0,   248,   272,     0,   206,
   209,   270,   258,     0,   248,   263,   222,   305,     0,     0,
   345,   344,   346,   347,   348,   269,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   256,   291,   241,
   420,     0,   273,     0,   253,     0,     0,   266,     0,   233,
   259,   264,     0,   236,   392,     0,     0,     0,     0,   412,
     0,     0,    62,    36,    70,     0,     0,     0,     0,     0,
     0,   137,     0,     0,   420,   420,     0,     0,    95,     0,
   420,     0,     0,     0,   127,    51,     0,     0,     0,     0,
    72,     0,   420,     0,   291,     0,     0,   107,   420,     0,
   126,   350,     0,   349,   352,   420,     0,     0,   263,   251,
   420,   250,   264,     0,   321,     0,   276,     0,   420,   343,
   342,   338,   339,   337,   340,   341,   333,   334,   335,   336,
     0,   240,   242,   263,     0,   245,     0,   255,   254,   271,
   267,     0,   237,   265,   239,    59,    60,    58,   102,   417,
   419,   418,   414,    40,    41,     0,    69,    66,    68,   411,
    82,   410,     0,    79,   420,   409,    81,     0,    86,   420,
   171,   142,   141,     0,   198,     0,     0,    47,    45,   420,
    38,    92,   399,     0,   401,     0,     0,    97,   420,   420,
   420,     0,     0,   311,     0,   310,   313,   420,   420,     0,
   104,   106,   103,     0,   108,   147,   164,     0,     0,     0,
     0,   202,     0,   189,   190,   182,   191,   187,   183,   181,
   184,   185,   186,   188,   161,   162,   149,   155,   159,   158,
     0,     0,   150,   151,   154,   160,   152,   156,   153,   163,
     0,   204,   420,   120,   216,   211,   208,   252,   322,     0,
   275,     0,     0,     0,   243,     0,   247,   246,   238,     0,
     0,   420,     0,   366,     0,   369,     0,     0,   139,   199,
   420,     0,    88,     0,    87,     0,     0,     0,   397,     0,
   420,     0,   130,   131,   134,   133,   128,   132,     0,     0,
     0,    74,     0,   420,     0,   420,   304,   420,   113,     0,
   420,   420,     0,   144,   173,   172,   174,     0,     0,     0,
   143,     0,     0,     0,   291,   364,   351,   353,     0,   365,
     0,   324,   323,     0,   318,   244,   415,    37,     0,   420,
     0,    77,    80,   420,    84,    64,     0,     0,    96,   406,
   404,   405,   400,   402,     0,    49,    50,     0,    52,    73,
   315,   312,   314,   309,   111,     0,     0,     0,     0,     0,
   376,   420,     0,     0,   148,     0,     0,   420,     0,     0,
   420,     0,   420,   177,   292,   157,   420,   358,   357,   359,
   420,   355,     0,   325,     0,     0,   420,     0,    65,    89,
   403,   398,     0,   112,     0,   374,   375,   377,     0,   370,
   371,   109,   105,   420,     0,   420,     0,   123,   420,     0,
     0,     0,     0,   175,   420,   354,     0,     0,    83,   367,
     0,    78,    98,   372,   373,     0,   379,   110,     0,     0,
   420,     0,   420,   420,   420,   197,   420,     0,   179,   180,
   356,   124,   121,     0,   368,   378,   145,   420,   166,     0,
   420,     0,     0,   165,     0,   178,   125,   167,     0,   192,
   193,   170,   420,   420,   176,   194,   196,   304,   169,   168,
     0,   195,     0,     0,     0
};

static const short yydefgoto[] = {   813,
     3,    78,    79,    80,   496,   554,   555,   556,   557,    85,
    86,    87,   530,    88,    89,   558,    91,   559,    93,   560,
   498,   164,   561,    96,   562,   504,   563,   289,   564,   298,
   186,   309,   187,   100,   565,   566,   567,   104,   543,   437,
   105,   106,   107,   108,   661,   440,   783,   568,   569,   529,
   627,   316,   570,   112,   415,   295,   571,   706,   643,   572,
   573,   574,   575,   576,   577,   578,   777,   755,   778,   579,
   789,   798,   580,   581,   223,   224,   350,   225,   212,   213,
   339,   214,   134,   135,   303,   334,   250,   137,   226,   139,
   198,    31,    32,   242,   160,    34,    35,    36,    37,   233,
   234,   331,   535,   536,   692,   464,   244,   245,   443,   444,
   583,   657,   658,   721,    38,   659,   799,   639,   700,   737,
   738,   117,   275,   312,    39,   148,    40,   526,   619,   246,
   507,   155,   276,   493,   229
};

static const short yypact[] = {   304,
  2862,  2905,  2437,-32768,-32768,   146,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   146,-32768,    92,-32768,
-32768,-32768,-32768,    76,-32768,-32768,    81,   152,-32768,   233,
  2393,   492,   202,   492,-32768,-32768,  1093,    76,-32768,   180,
-32768,   237,   192,-32768,-32768,-32768,   265,-32768,-32768,   308,
   297,  2948,   307,-32768,-32768,   297,   314,   335,   339,-32768,
-32768,-32768,   362,   150,   169,   380,   227,   388,   341,   511,
  3200,  3200,   397,   446,   308,   463,   220,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   297,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   587,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,  3242,-32768,  3074,-32768,-32768,-32768,   468,
-32768,    69,   407,-32768,   492,-32768,  1304,    49,  1540,   500,
   296,   183,    76,-32768,-32768,   268,   310,   268,   320,    40,
   433,-32768,-32768,-32768,-32768,   523,    71,-32768,-32768,-32768,
   384,-32768,   245,   384,   384,-32768,   507,   -14,   623,-32768,
   224,    76,   543,   550,-32768,   384,  3116,  3158,    76,   534,
    26,-32768,   533,   583,-32768,-32768,   384,   607,-32768,-32768,
-32768,   611,  3158,   588,    98,   593,   600,   384,   384,   611,
  3158,  3158,    76,   308,    90,   628,   384,   247,   553,   148,
   750,   312,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   572,-32768,   627,-32768,-32768,-32768,-32768,   180,   579,
  2288,   496,   635,-32768,   639,   646,    88,   595,-32768,-32768,
-32768,   623,-32768,  2245,  2245,   661,-32768,  2245,  2245,  2245,
  2245,   668,  1151,-32768,-32768,   611,-32768,  2288,    76,   437,
   320,-32768,-32768,   598,-32768,-32768,  3158,  1640,-32768,  3158,
  1740,   296,   437,   320,   618,   564,-32768,-32768,   180,-32768,
   676,   611,-32768,-32768,   178,   678,-32768,-32768,   702,    33,
   268,-32768,   682,   681,   687,   679,   526,   689,   693,-32768,
   696,   700,-32768,    76,-32768,-32768,   698,   704,-32768,   707,
   709,  3200,-32768,-32768,-32768,-32768,-32768,  3200,-32768,-32768,
-32768,   710,-32768,   712,   715,   665,-32768,-32768,   207,    58,
   783,   783,   717,   663,   110,   722,   628,   670,   564,    -5,
   714,-32768,  2342,   890,-32768,   325,-32768,  3074,-32768,-32768,
-32768,   407,   455,   576,   671,   332,-32768,-32768,  3158,-32768,
-32768,-32768,   455,   349,   672,   783,-32768,  1504,  1119,  3284,
-32768,-32768,-32768,-32768,-32768,-32768,  2245,  2245,  2245,  2245,
  2245,  2245,  2245,  2245,  2245,  2245,  2245,   803,    16,-32768,
  3158,  1840,-32768,   725,-32768,  1179,   732,-32768,  1610,   437,
   320,   814,   628,   437,-32768,   118,  2523,   384,   986,-32768,
   731,   733,-32768,-32768,-32768,   590,   781,   384,  3158,   623,
   735,-32768,   734,  2609,-32768,   336,  3200,   289,   739,   742,
   600,   153,   384,   749,-32768,-32768,   241,   783,   628,    29,
-32768,   876,   787,   760,   663,   763,   548,-32768,    62,  1449,
-32768,-32768,   761,-32768,-32768,   627,    76,   672,-32768,-32768,
   639,-32768,   455,   451,  2245,  1940,  2040,   -12,   202,   910,
  1059,  1106,  1026,   946,   540,   540,   530,   530,-32768,-32768,
   469,   672,-32768,   628,   767,-32768,  1710,-32768,-32768,-32768,
-32768,   628,   437,   320,   437,-32768,-32768,   611,-32768,-32768,
-32768,   611,   772,-32768,-32768,  1449,-32768,-32768,   611,-32768,
-32768,-32768,   777,-32768,   342,   611,-32768,   784,-32768,   342,
-32768,-32768,-32768,  2695,   308,  3326,   484,-32768,-32768,  3158,
-32768,-32768,   235,   699,-32768,   727,   788,-32768,   883,-32768,
   342,   267,   628,   775,   136,-32768,-32768,   560,  3158,   623,
-32768,-32768,-32768,   792,-32768,-32768,-32768,   789,   765,   766,
   768,   716,   523,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   797,  1449,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  2991,   799,   774,-32768,-32768,-32768,-32768,-32768,  1504,  2245,
  1004,  2245,   795,   809,-32768,   567,-32768,-32768,   437,   384,
   805,  3158,   813,   786,   475,-32768,   252,   812,-32768,   611,
    83,   623,-32768,  3200,-32768,   817,   849,    91,-32768,   109,
  3074,   228,-32768,-32768,-32768,-32768,-32768,-32768,  3284,  2781,
   832,-32768,  2145,   787,   285,  3158,   702,   806,-32768,   835,
   890,  3158,  1449,-32768,-32768,-32768,-32768,   523,   841,   623,
-32768,  3284,   212,   460,   840,-32768,   843,-32768,    22,-32768,
  1449,  1504,  1504,  2245,-32768,-32768,-32768,-32768,   845,  3158,
   850,-32768,-32768,   342,-32768,-32768,   846,   851,-32768,-32768,
   235,-32768,   235,-32768,   810,-32768,-32768,   808,-32768,-32768,
-32768,-32768,  1504,-32768,-32768,   548,   856,   863,    76,   458,
-32768,   268,   548,   866,-32768,  1449,   868,  3158,   548,    -9,
  2342,   892,    78,-32768,   871,-32768,   774,-32768,-32768,-32768,
   774,-32768,   870,  1504,   879,   884,  3158,   885,-32768,-32768,
-32768,-32768,   886,-32768,   517,-32768,   888,-32768,   894,-32768,
-32768,-32768,-32768,   268,   889,  3158,   903,-32768,  3158,   895,
   905,   906,   907,-32768,   623,-32768,    76,   321,-32768,-32768,
   908,-32768,-32768,-32768,-32768,    76,-32768,-32768,  1449,   911,
   342,   913,  3158,  3158,   560,-32768,   623,   912,-32768,-32768,
-32768,-32768,-32768,   548,-32768,-32768,-32768,   342,-32768,   578,
   342,   915,   919,-32768,   918,-32768,-32768,-32768,   569,-32768,
-32768,-32768,   342,   342,-32768,-32768,-32768,   702,-32768,-32768,
   923,-32768,   953,   959,-32768
};

static const short yypgoto[] = {-32768,
  -253,-32768,-32768,-32768,-32768,     9,    23,    28,    41,-32768,
-32768,-32768,-32768,-32768,-32768,    46,-32768,    50,-32768,    51,
-32768,-32768,    55,-32768,    57,   355,    59,-32768,    60,-32768,
  -294,   541,   -69,-32768,    61,    64,    73,   434,  -576,   324,
-32768,-32768,-32768,   440,-32768,-32768,-32768,    20,    21,-32768,
-32768,-32768,    75,-32768,-32768,-32768,  -477,-32768,-32768,-32768,
   442,-32768,-32768,    79,-32768,-32768,-32768,-32768,   193,-32768,
-32768,  -300,-32768,    10,   441,   790,   525,    36,   357,-32768,
   535,   641,  -122,   527,-32768,   -15,   404,  -175,    -6,   -85,
   -28,   -25,-32768,   505,    27,   -30,-32768,   951,-32768,  -268,
-32768,-32768,-32768,   356,-32768,   865,  -101,-32768,-32768,-32768,
-32768,-32768,   276,-32768,   -73,   275,  -465,   229,-32768,-32768,
   239,  1002,   -86,-32768,   662,  -121,  -112,-32768,  -508,   778,
   503,   147,  -138,-32768,     0
};


#define	YYLAST		3418


static const short yytable[] = {     4,
   125,   184,   115,   193,   140,   118,   126,   420,   143,   219,
   401,    81,   114,   256,   215,   259,   118,   136,   601,   592,
   397,   749,   109,   110,     6,    82,   138,    33,    33,   279,
    83,  -307,   142,   267,   269,     6,    30,    42,   299,   605,
   202,   414,     6,    84,   608,   251,   346,   473,    90,   220,
   156,   354,    92,    94,   264,   156,   165,    95,   230,    97,
   473,    98,    99,   101,   271,   631,   102,   116,   277,   294,
   248,     6,   638,     6,   283,   103,   116,   111,     6,   287,
   197,   113,   718,   719,   720,   231,   129,   156,   593,  -382,
     6,   750,   320,   681,   651,   152,   132,   341,   227,   143,
     6,   311,   314,   253,   474,   128,   254,   116,   323,    27,
   326,   683,     6,    29,   754,   216,   676,   533,   127,   734,
    27,   486,   517,   152,    29,   130,   743,    27,   127,   544,
   120,    29,   748,   321,   129,   142,   251,   142,   132,   116,
   262,   264,    33,   281,   129,   346,   354,   395,   249,   232,
   330,   211,     6,    33,   487,   523,   140,   118,   402,   116,
   218,   514,   268,    27,   116,   705,   322,    29,   634,   304,
   118,   171,   731,   635,   732,    27,   391,    27,   305,    29,
   454,    29,   682,   723,   306,    27,   310,   313,   353,    29,
   321,   122,   344,    33,    33,   132,   133,    27,   131,   332,
   684,    29,   162,   132,     6,   172,   133,   797,   728,    33,
   398,   438,   273,   340,     6,   132,   357,    33,    33,   378,
   169,   399,    20,    21,   351,   383,   270,   116,   745,   152,
   175,   686,   127,   123,   194,    27,   215,    27,   421,    29,
   524,    29,   711,     6,   525,   128,   152,    33,   129,     6,
   273,   322,   454,   426,     6,   282,    27,   251,   264,   490,
    29,    22,    23,   176,   687,   130,   124,   391,   447,   146,
   149,   280,   273,     4,    33,   712,   630,   325,    25,    26,
   142,   274,   280,    33,   150,   673,    33,     6,   132,    27,
   147,   787,   264,    29,     4,   151,   429,   491,     6,    27,
   632,   192,   131,    29,   322,   790,   484,   132,   439,   638,
   133,   128,   196,   152,     6,   280,   545,   132,   695,   118,
   133,   508,   518,     6,   617,   519,   248,   154,    27,   118,
   458,   130,    29,   142,    27,   445,   527,   161,    29,    27,
   257,   196,   129,    29,   163,   618,   132,   327,   335,   133,
   260,   280,   196,    48,   782,   249,   258,   132,   441,    33,
   133,  -420,   257,   450,    33,   166,   261,   484,   698,   167,
   515,   178,    27,   211,   179,    33,    29,   180,   258,   257,
   452,    20,    21,    27,   451,   196,   270,    29,   442,   152,
   132,   505,   168,   133,   510,   258,   115,     1,     2,    27,
    60,    61,    62,    29,   249,    81,   114,    33,    27,   603,
   174,   531,    29,   115,     4,   115,   109,   110,   177,    82,
   310,   196,    81,   114,    83,   516,   132,   188,   253,   133,
   140,   591,   537,   109,   110,    33,    82,    84,   142,   582,
   649,    83,    90,   136,   509,   340,    92,    94,    22,    23,
   351,    95,   138,    97,    84,    98,    99,   101,   142,    90,
   102,   667,   713,    92,    94,    25,    26,   381,    95,   103,
    97,   111,    98,    99,   101,   113,   189,   102,   500,   604,
   152,   257,   588,   382,   604,   221,   103,   500,   111,   152,
   802,   740,   113,   191,   741,   582,   714,   258,     6,   381,
   595,   129,   809,   810,   606,   604,   217,   654,   501,   606,
   247,   502,   604,   115,   181,   382,   152,   613,   438,   215,
   502,   266,    81,   114,   641,     6,   127,   614,   115,     4,
   606,    20,    21,   109,   110,   707,    82,   606,   629,   811,
   278,    83,   129,   263,   678,   285,    33,    27,   625,   626,
   764,    29,   286,   765,    84,   202,   159,   407,   408,    90,
   125,   503,   383,    92,    94,    33,     6,   293,    95,   296,
    97,   582,    98,    99,   101,   185,   185,   102,     6,   742,
   540,   541,   660,    27,   542,   297,   103,    29,   111,     6,
   636,   674,   113,   497,   325,   152,   677,   381,   666,    20,
    21,   263,   806,   637,   133,   807,   221,   290,   291,   300,
   118,   800,   808,   382,   801,   439,   273,   195,   201,   696,
   216,   768,   129,   301,   302,     6,   307,   603,    33,   115,
     6,   315,   308,   537,   709,   376,   377,   701,    81,   114,
   142,   329,   582,   374,   375,   376,   377,    33,   604,   109,
   110,    27,    82,   280,   337,    29,   211,    83,   325,   338,
   582,   342,    33,    27,   393,   390,   348,    29,    33,   394,
    84,   349,   249,   606,    27,    90,   353,   352,    29,    92,
    94,   751,   322,   356,    95,   121,    97,   196,    98,    99,
   101,   360,   132,   102,   141,   133,    33,   384,   365,   145,
   387,   142,   103,   132,   111,   582,   392,   396,   113,   400,
    27,   230,   118,   403,    29,    27,   660,   404,   405,    29,
   660,   409,   406,   196,   410,   170,   173,   411,   132,   416,
   390,   133,   394,   412,    33,   417,   249,    33,   418,   779,
   419,   422,   784,   142,   423,   604,   424,   425,   427,   604,
   433,   428,     6,    33,   780,   431,   478,   199,   432,   449,
   453,   779,   604,   480,   494,   604,   495,   512,   582,   520,
   606,   511,    33,   434,   606,    33,   780,   604,   604,   521,
   333,   471,   528,     6,   500,     6,   152,   606,   228,   534,
   606,   538,   252,   539,   255,   483,   485,   584,   597,    33,
    33,   265,   606,   606,   600,     6,   185,   602,   621,   620,
     6,   280,   185,   280,   501,   607,     6,   502,   633,   622,
   199,   475,   642,   284,   648,   644,   664,   153,   645,   646,
   292,   647,   471,   248,   650,   483,  -146,    27,   711,   656,
   665,    29,   668,   670,   248,   675,   141,   182,   679,   129,
   196,   680,   190,   671,   318,   132,   324,   252,   133,   328,
   129,   121,   199,   336,   459,   690,   702,   503,    27,   699,
    27,   708,    29,   716,    29,   717,   725,   596,     6,   729,
   727,   196,   345,   347,   730,   599,   132,   735,   355,   133,
    27,   133,   733,   199,    29,    27,   736,   744,   746,    29,
    48,    27,   753,   353,   441,    29,   325,   758,   196,   379,
   380,   249,   759,   132,   482,   760,   133,   515,   762,   763,
   766,   185,   249,   252,   767,   773,   769,   252,   272,    20,
    21,   272,   272,   128,   771,   774,   596,   775,   272,   785,
   776,   324,   788,   272,   791,   796,   803,    60,    61,    62,
   804,   805,   814,   130,   272,   413,   812,   436,   815,   672,
   616,   522,   623,    27,   703,   272,   272,    29,   624,   795,
   628,   319,   272,    77,   272,   587,   482,   685,   446,   640,
   585,   317,   347,   347,   249,   594,   430,   144,   252,   694,
   252,   152,   756,   243,   435,   757,   204,   205,   206,   207,
   208,   209,   210,   794,   786,   448,     6,   368,   369,   370,
   371,   372,   373,   374,   375,   376,   377,   347,   119,   615,
   612,     0,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,     0,     0,     0,     0,     0,   472,
     0,     0,   669,     0,     0,    22,    23,   372,   373,   374,
   375,   376,   377,   252,   252,     0,     0,     0,     0,     0,
     0,    24,    25,    26,     0,     0,     0,     0,   199,     0,
     0,   199,     0,     0,     0,     0,   697,     0,     0,     0,
     0,     0,   704,     0,     0,   653,     0,     0,   199,   347,
   252,    27,     0,   252,     0,    29,     0,     0,   358,   359,
     0,     0,   361,   362,   363,   364,     0,     0,   586,   132,
   726,     7,     8,     9,    10,    11,    12,    13,   185,    15,
   141,     0,   386,     0,     0,   389,   371,   372,   373,   374,
   375,   376,   377,   688,     0,   252,     0,     0,     0,     0,
     0,     0,     0,   252,     0,     0,     0,     0,   747,     0,
   455,   752,     0,     0,     0,     0,   710,   369,   370,   371,
   372,   373,   374,   375,   376,   377,     0,   761,     0,     0,
     0,     0,     0,   488,     0,   272,   492,     0,     0,     0,
     0,     0,     0,   499,   506,   272,   770,     0,     0,   772,
     0,     0,     0,     0,   252,     0,     0,     0,   366,     0,
   272,   199,     0,     0,   532,   370,   371,   372,   373,   374,
   375,   376,   377,   792,   793,   367,   368,   369,   370,   456,
   372,   373,   374,   375,   457,   377,   479,     0,     0,     0,
     0,   460,   461,   462,   463,     0,   465,   466,   467,   468,
   469,   470,   655,     0,     0,     0,   477,   367,   368,   369,
   370,   371,   372,   373,   374,   375,   376,   377,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   199,     0,
     0,     0,     0,   199,     0,   367,   368,   369,   370,   371,
   372,   373,   374,   375,   376,   377,     0,     0,     0,     0,
     0,     0,   610,     0,   506,     0,   199,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     6,     0,     0,     0,
     0,   199,     0,     0,   199,   715,     0,     0,     0,   589,
   722,   469,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,   221,     0,     0,     0,     0,     0,
     0,     0,    19,    20,    21,    22,    23,     0,     0,   199,
   129,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   739,    24,    25,    26,     0,     0,     0,     0,     0,     0,
     0,     0,   435,     0,     0,     0,     0,   272,     0,     0,
     0,     0,   506,     0,     0,     0,     0,     0,     0,     0,
     0,    27,     0,     0,    28,    29,     0,     0,     0,     0,
     0,     0,     0,     0,   222,     0,     0,     0,     0,   132,
     0,     0,     0,     0,     0,     0,   199,     0,   781,   199,
     0,     0,     0,     0,     0,     0,     0,   739,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   199,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   546,
     0,  -420,    44,     0,   662,     0,   663,    47,     0,     0,
     0,     0,     0,     0,     0,     0,    48,  -420,  -420,  -420,
  -420,  -420,  -420,  -420,  -420,  -420,  -420,  -420,  -420,     0,
     0,     0,   547,    50,     0,     0,  -420,     0,  -420,  -420,
  -420,  -420,     0,     0,     0,     0,     0,   693,    52,    53,
    54,    55,   548,    57,    58,    59,  -420,  -420,  -420,   549,
   550,   551,     0,    60,   552,    62,     0,    63,    64,     0,
     0,     0,    68,     0,    70,    71,    72,    73,   724,    75,
     0,     0,     0,     0,     0,     0,  -420,     0,     0,    77,
  -420,  -420,     6,     0,     0,   152,     0,     0,     0,     0,
   204,   205,   206,   207,   208,   209,   210,   553,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
   235,     0,     0,     0,     0,     0,     0,     0,     0,    20,
    21,    22,    23,     0,   236,     0,     0,   237,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    24,    25,    26,
   367,   368,   369,   370,   371,   372,   373,   374,   375,   376,
   377,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    27,     0,     0,
     0,    29,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     6,   238,   239,   152,     0,     0,   240,   241,
   204,   205,   206,   207,   208,   209,   210,   481,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
   235,     0,     0,     0,     0,     0,     0,     0,     0,    20,
    21,    22,    23,     0,   236,     0,     0,   385,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    24,    25,    26,
     0,     0,     0,     0,     0,     0,   367,   368,   369,   370,
   371,   372,   373,   374,   375,   376,   377,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    27,     0,     0,
     0,    29,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     6,   238,   239,   152,     0,     0,   240,   241,
   204,   205,   206,   207,   208,   209,   210,   598,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
   235,     0,     0,     0,     0,     0,     0,     0,     0,    20,
    21,    22,    23,     0,   236,     0,     0,   388,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    24,    25,    26,
     0,     0,     0,     0,     0,     0,   367,   368,   369,   370,
   371,   372,   373,   374,   375,   376,   377,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    27,     0,     0,
     0,    29,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     6,   238,   239,   152,     0,     0,   240,   241,
   204,   205,   206,   207,   208,   209,   210,     0,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
   235,     0,     0,     0,     0,     0,     0,     0,     0,    20,
    21,    22,    23,     0,   236,     0,     0,   476,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    24,    25,    26,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    27,     0,     0,
     0,    29,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     6,   238,   239,   152,     0,     0,   240,   241,
   204,   205,   206,   207,   208,   209,   210,     0,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
   235,   590,     0,     0,     0,     0,     0,     0,     0,    20,
    21,    22,    23,     0,   236,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    24,    25,    26,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    27,     0,     0,
     0,    29,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     6,   238,   239,   152,     0,     0,   240,   241,
   204,   205,   206,   207,   208,   209,   210,     0,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
   235,     0,     0,     0,     0,     0,     0,     0,     0,    20,
    21,    22,    23,     0,   236,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    24,    25,    26,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    27,     0,     0,
     0,    29,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   238,   239,   132,     0,     6,   240,   241,
   152,     0,     0,     0,   691,   204,   205,   206,   207,   208,
   209,   210,     0,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,   235,     0,     0,     0,     0,
     0,     0,     0,     0,    20,    21,    22,    23,     0,   236,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    24,    25,    26,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    27,     0,     0,     0,    29,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     6,   238,   239,
   152,     0,     0,   240,   241,   204,   205,   206,   207,   208,
   209,   210,     0,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,   235,     0,     0,     0,     0,
     0,     0,     0,     0,    20,    21,    22,    23,     0,   236,
     6,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    24,    25,    26,     0,     7,     8,     9,    10,
    11,    12,    13,    14,    15,    16,    17,    18,   221,     0,
     0,     0,     0,     0,     0,     0,    19,    20,    21,    22,
    23,     0,    27,     0,   129,     0,    29,     0,     0,     0,
     0,     0,     0,     0,     6,    24,    25,    26,   238,   239,
     0,     0,     0,   240,   241,     0,     0,     0,     0,     0,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,     0,     0,     0,    27,     0,     0,    28,    29,
    19,    20,    21,    22,    23,     0,     0,     0,   343,     0,
     0,     0,     0,   132,     0,     6,     0,     0,     0,    24,
    25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,     0,     0,     0,     0,     0,     0,    27,
     0,     0,    28,    29,    22,    23,    -1,    43,     0,  -420,
    44,     0,   321,    45,    46,    47,     0,   132,     0,     0,
    24,    25,    26,     0,    48,  -420,  -420,  -420,  -420,  -420,
  -420,  -420,  -420,  -420,  -420,  -420,  -420,     0,     0,     0,
    49,    50,     0,     0,     0,     0,  -420,  -420,  -420,  -420,
    27,     0,    51,     0,    29,     0,    52,    53,    54,    55,
    56,    57,    58,    59,  -420,  -420,  -420,     0,     0,     0,
     0,    60,    61,    62,     0,    63,    64,    65,    66,    67,
    68,    69,    70,    71,    72,    73,    74,    75,     0,     0,
     0,     0,    76,    43,  -420,  -420,    44,    77,  -420,    45,
    46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
    48,  -420,  -420,  -420,  -420,  -420,  -420,  -420,  -420,  -420,
  -420,  -420,  -420,     0,     0,     0,    49,    50,     0,     0,
   489,     0,  -420,  -420,  -420,  -420,     0,     0,    51,     0,
     0,     0,    52,    53,    54,    55,    56,    57,    58,    59,
  -420,  -420,  -420,     0,     0,     0,     0,    60,    61,    62,
     0,    63,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,     0,     0,     0,     0,    76,    43,
  -420,  -420,    44,    77,  -420,    45,    46,    47,     0,     0,
     0,     0,     0,     0,     0,     0,    48,  -420,  -420,  -420,
  -420,  -420,  -420,  -420,  -420,  -420,  -420,  -420,  -420,     0,
     0,     0,    49,    50,     0,     0,   513,     0,  -420,  -420,
  -420,  -420,     0,     0,    51,     0,     0,     0,    52,    53,
    54,    55,    56,    57,    58,    59,  -420,  -420,  -420,     0,
     0,     0,     0,    60,    61,    62,     0,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
     0,     0,     0,     0,    76,    43,  -420,  -420,    44,    77,
  -420,    45,    46,    47,     0,     0,     0,     0,     0,     0,
     0,     0,    48,  -420,  -420,  -420,  -420,  -420,  -420,  -420,
  -420,  -420,  -420,  -420,  -420,     0,     0,     0,    49,    50,
     0,     0,   609,     0,  -420,  -420,  -420,  -420,     0,     0,
    51,     0,     0,     0,    52,    53,    54,    55,    56,    57,
    58,    59,  -420,  -420,  -420,     0,     0,     0,     0,    60,
    61,    62,     0,    63,    64,    65,    66,    67,    68,    69,
    70,    71,    72,    73,    74,    75,     0,     0,     0,     0,
    76,    43,  -420,  -420,    44,    77,  -420,    45,    46,    47,
     0,     0,     0,     0,     0,     0,     0,     0,    48,  -420,
  -420,  -420,  -420,  -420,  -420,  -420,  -420,  -420,  -420,  -420,
  -420,     0,     0,     0,    49,    50,     0,     0,     0,     0,
  -420,  -420,  -420,  -420,     0,     0,    51,     0,   689,     0,
    52,    53,    54,    55,    56,    57,    58,    59,  -420,  -420,
  -420,     0,     0,     0,     0,    60,    61,    62,     0,    63,
    64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
    74,    75,     5,     0,     6,     0,    76,     0,  -420,     0,
     0,    77,  -420,     0,     0,     0,     0,     0,     0,     0,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,     0,     0,     0,     0,     0,     0,     0,     0,
    19,    20,    21,    22,    23,    41,     0,     6,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
    25,    26,     0,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,     0,     0,     0,     0,     0,
     0,     0,     0,    19,    20,    21,    22,    23,   157,    27,
   158,     0,    28,    29,     0,     0,     0,     0,     0,     0,
     0,     0,    24,    25,    26,     0,     7,     8,     9,    10,
    11,    12,    13,    14,    15,    16,    17,    18,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    20,    21,    22,
    23,     0,    27,     6,     0,    28,    29,     0,     0,     0,
     0,     0,     0,     0,     0,    24,    25,    26,     0,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    20,    21,    22,    23,     0,    27,     0,     0,     0,    29,
     0,     0,     0,     0,     0,     0,     0,     0,   200,    25,
    26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     6,     0,    27,   203,
     0,     0,    29,   652,   204,   205,   206,   207,   208,   209,
   210,     0,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,     0,     0,     0,     0,     0,     0,
     0,     0,    19,    20,    21,    22,    23,     0,     6,     0,
     0,     0,     0,     0,     0,     0,   288,     0,     0,     0,
     0,    24,    25,    26,     7,     8,     9,    10,    11,    12,
    13,    14,    15,    16,    17,    18,     0,     0,     0,     0,
     0,     0,     0,     0,    19,    20,    21,    22,    23,     0,
     6,    27,     0,     0,    28,    29,     0,     0,     0,     0,
     0,     0,     0,    24,    25,    26,     7,     8,     9,    10,
    11,    12,    13,    14,    15,    16,    17,    18,     0,     0,
     0,     0,     0,     0,     0,     0,    19,    20,    21,    22,
    23,     0,     6,    27,     0,     0,    28,    29,     0,     0,
     0,     0,     0,     0,     0,    24,    25,    26,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
   183,     0,     0,     0,     0,     0,     0,     0,     0,    20,
    21,    22,    23,     0,     6,    27,     0,     0,    28,    29,
     0,     0,     0,     0,     0,     0,     0,    24,    25,    26,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    20,    21,    22,    23,     0,     6,    27,     0,     0,
     0,    29,     0,     0,     0,     0,     0,     0,     0,   200,
    25,    26,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    20,    21,    22,    23,     0,   611,    27,
     0,     0,     0,    29,     0,     0,     0,     0,     0,     0,
     0,    24,    25,    26,     7,     8,     9,    10,    11,    12,
    13,    14,    15,    16,    17,    18,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    20,    21,    22,    23,     0,
     0,    27,     0,     0,     0,    29,     0,     0,     0,     0,
     0,     0,     0,    24,    25,    26,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,     0,    29
};

static const short yycheck[] = {     0,
    31,    71,     3,    77,    33,     6,    32,   302,    34,   122,
   279,     3,     3,   136,   116,   138,    17,    33,   496,    32,
   274,    31,     3,     3,     3,     3,    33,     1,     2,    44,
     3,    37,    33,   146,   147,     3,     1,     2,   177,   505,
   114,   295,     3,     3,   510,   131,   222,    32,     3,   123,
    51,   227,     3,     3,   140,    56,    57,     3,    10,     3,
    32,     3,     3,     3,   151,   531,     3,    82,   155,    44,
    31,     3,   538,     3,   161,     3,    82,     3,     3,   166,
    96,     3,    61,    62,    63,    37,    47,    88,   101,    32,
     3,   101,     3,     3,   572,     6,   106,   219,   127,   125,
     3,   188,   189,   132,    89,    44,   132,    82,   195,    88,
   197,     3,     3,    92,    37,   116,    34,    89,    31,   696,
    88,     4,   417,     6,    92,    64,   703,    88,    31,    68,
    39,    92,   709,   101,    47,   136,   222,   138,   106,    82,
   101,   227,   116,   159,    47,   321,   322,   269,   109,   101,
     3,   116,     3,   127,    37,     3,   185,   158,   281,    82,
    92,   415,    92,    88,    82,   643,   195,    92,    33,   185,
   171,     3,   681,    38,   683,    88,   262,    88,   185,    92,
   356,    92,    92,   661,   185,    88,   187,   188,   101,    92,
   101,   111,   221,   167,   168,   106,   109,    88,   101,   200,
    92,    92,    56,   106,     3,    37,   109,   784,   674,   183,
    33,   334,     6,   214,     3,   106,   232,   191,   192,   248,
    71,    44,    40,    41,   225,   254,     3,    82,   706,     6,
     4,     4,    31,    82,    88,    88,   338,    88,   308,    92,
    88,    92,    31,     3,    92,    44,     6,   221,    47,     3,
     6,   280,   428,    47,     3,    32,    88,   343,   344,   398,
    92,    42,    43,    37,    37,    64,    34,   353,   342,    90,
    34,    31,     6,   274,   248,    64,   530,    31,    59,    60,
   281,    37,    31,   257,    93,    34,   260,     3,   106,    88,
   111,   769,   378,    92,   295,    31,   325,   399,     3,    88,
    34,    82,   101,    92,   333,   771,   392,   106,   334,   775,
   109,    44,   101,     6,     3,    31,   439,   106,    34,   320,
   109,   408,    34,     3,    90,    37,    31,    31,    88,   330,
   359,    64,    92,   334,    88,   336,   423,    31,    92,    88,
    31,   101,    47,    92,    31,   111,   106,   101,    37,   109,
    31,    31,   101,    18,    34,   109,    47,   106,    34,   333,
   109,    37,    31,    32,   338,    31,    47,   453,   637,    31,
    35,    31,    88,   338,    34,   349,    92,    37,    47,    31,
    32,    40,    41,    88,   349,   101,     3,    92,    64,     6,
   106,   407,    31,   109,   410,    47,   397,    94,    95,    88,
    65,    66,    67,    92,   109,   397,   397,   381,    88,    68,
    31,   427,    92,   414,   415,   416,   397,   397,    31,   397,
   421,   101,   414,   414,   397,   416,   106,    31,   457,   109,
   459,   457,   433,   414,   414,   409,   414,   397,   439,   440,
   553,   414,   397,   459,   409,   446,   397,   397,    42,    43,
   451,   397,   459,   397,   414,   397,   397,   397,   459,   414,
   397,   600,     3,   414,   414,    59,    60,    31,   414,   397,
   414,   397,   414,   414,   414,   397,    31,   414,     4,   505,
     6,    31,    32,    47,   510,    31,   414,     4,   414,     6,
   791,    34,   414,    31,    37,   496,    37,    47,     3,    31,
    32,    47,   803,   804,   505,   531,    39,   581,    34,   510,
    11,    37,   538,   514,     4,    47,     6,    34,   641,   621,
    37,    89,   514,   514,   540,     3,    31,    44,   529,   530,
   531,    40,    41,   514,   514,   648,   514,   538,   529,   808,
    34,   514,    47,   140,   614,     3,   520,    88,   529,   529,
    34,    92,     3,    37,   514,   629,    52,    32,    33,   514,
   591,    87,   591,   514,   514,   539,     3,    34,   514,    37,
   514,   572,   514,   514,   514,    71,    72,   514,     3,   702,
    33,    34,   583,    88,    37,     3,   514,    92,   514,     3,
    31,   607,   514,     4,    31,     6,   612,    31,    32,    40,
    41,   198,    34,    44,   109,    37,    31,   167,   168,     3,
   611,    34,    44,    47,    37,   641,     6,    31,   114,   635,
   621,   744,    47,   183,    37,     3,    34,    68,   602,   630,
     3,   191,    33,   634,   650,   106,   107,   638,   630,   630,
   641,    89,   643,   104,   105,   106,   107,   621,   674,   630,
   630,    88,   630,    31,    83,    92,   621,   630,    31,    33,
   661,    83,   636,    88,   101,   262,    32,    92,   642,   266,
   630,    33,   109,   674,    88,   630,   101,    32,    92,   630,
   630,   710,   711,    89,   630,    24,   630,   101,   630,   630,
   630,    31,   106,   630,    33,   109,   670,   257,    31,    38,
   260,   702,   630,   106,   630,   706,    89,    32,   630,    32,
    88,    10,   713,    32,    92,    88,   717,    37,    32,    92,
   721,    33,    44,   101,    32,    64,    65,    32,   106,    32,
   327,   109,   329,    34,   708,    32,   109,   711,    32,   755,
    32,    32,   758,   744,    33,   771,    32,    83,    32,   775,
    37,    89,     3,   727,   755,    34,    32,    96,    89,    89,
    89,   777,   788,    32,    34,   791,    34,    34,   769,    31,
   771,    37,   746,   333,   775,   749,   777,   803,   804,    38,
    31,   378,    34,     3,     4,     3,     6,   788,   127,     3,
   791,    32,   131,    31,   133,   392,   393,    37,    32,   773,
   774,   140,   803,   804,    33,     3,   302,    31,    82,   111,
     3,    31,   308,    31,    34,    32,     3,    37,    44,    32,
   159,   381,    31,   162,   109,    37,    32,    50,    64,    64,
   169,    64,   429,    31,    38,   432,    38,    88,    31,    66,
    32,    92,    38,    31,    31,    34,   185,    70,    32,    47,
   101,     3,    75,    68,   193,   106,   195,   196,   109,   198,
    47,   200,   201,   202,   360,    34,    32,    87,    88,    64,
    88,    31,    92,    34,    92,    33,    32,   474,     3,    34,
    31,   101,   221,   222,    34,   482,   106,    32,   227,   109,
    88,   109,    83,   232,    92,    88,    34,    32,    31,    92,
    18,    88,    11,   101,    34,    92,    31,    38,   101,   248,
   249,   109,    34,   106,   101,    32,   109,    35,    34,    34,
    33,   417,   109,   262,    31,    31,    38,   266,   151,    40,
    41,   154,   155,    44,    32,    31,   533,    32,   161,    32,
    34,   280,    32,   166,    32,    34,    32,    65,    66,    67,
    32,    34,     0,    64,   177,   294,    34,    68,     0,   605,
   520,   421,   529,    88,   641,   188,   189,    92,   529,   777,
   529,   194,   195,    91,   197,   451,   101,   621,   338,   539,
   446,   192,   321,   322,   109,   459,   325,    37,   327,   634,
   329,     6,   717,   129,   333,   721,    11,    12,    13,    14,
    15,    16,    17,   775,   766,   344,     3,    98,    99,   100,
   101,   102,   103,   104,   105,   106,   107,   356,    17,   517,
   516,    -1,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,   378,
    -1,    -1,   602,    -1,    -1,    42,    43,   102,   103,   104,
   105,   106,   107,   392,   393,    -1,    -1,    -1,    -1,    -1,
    -1,    58,    59,    60,    -1,    -1,    -1,    -1,   407,    -1,
    -1,   410,    -1,    -1,    -1,    -1,   636,    -1,    -1,    -1,
    -1,    -1,   642,    -1,    -1,   581,    -1,    -1,   427,   428,
   429,    88,    -1,   432,    -1,    92,    -1,    -1,   234,   235,
    -1,    -1,   238,   239,   240,   241,    -1,    -1,   447,   106,
   670,    19,    20,    21,    22,    23,    24,    25,   614,    27,
   459,    -1,   258,    -1,    -1,   261,   101,   102,   103,   104,
   105,   106,   107,   629,    -1,   474,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   482,    -1,    -1,    -1,    -1,   708,    -1,
    32,   711,    -1,    -1,    -1,    -1,   652,    99,   100,   101,
   102,   103,   104,   105,   106,   107,    -1,   727,    -1,    -1,
    -1,    -1,    -1,   396,    -1,   398,   399,    -1,    -1,    -1,
    -1,    -1,    -1,   406,   407,   408,   746,    -1,    -1,   749,
    -1,    -1,    -1,    -1,   533,    -1,    -1,    -1,    48,    -1,
   423,   540,    -1,    -1,   427,   100,   101,   102,   103,   104,
   105,   106,   107,   773,   774,    97,    98,    99,   100,   101,
   102,   103,   104,   105,   106,   107,    48,    -1,    -1,    -1,
    -1,   367,   368,   369,   370,    -1,   372,   373,   374,   375,
   376,   377,   581,    -1,    -1,    -1,   382,    97,    98,    99,
   100,   101,   102,   103,   104,   105,   106,   107,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   607,    -1,
    -1,    -1,    -1,   612,    -1,    97,    98,    99,   100,   101,
   102,   103,   104,   105,   106,   107,    -1,    -1,    -1,    -1,
    -1,    -1,   515,    -1,   517,    -1,   635,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,    -1,    -1,
    -1,   650,    -1,    -1,   653,   654,    -1,    -1,    -1,   455,
   659,   457,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    39,    40,    41,    42,    43,    -1,    -1,   688,
    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   699,    58,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   711,    -1,    -1,    -1,    -1,   600,    -1,    -1,
    -1,    -1,   605,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    88,    -1,    -1,    91,    92,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,    -1,   106,
    -1,    -1,    -1,    -1,    -1,    -1,   755,    -1,   757,   758,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   766,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   777,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,
    -1,     3,     4,    -1,   590,    -1,   592,     9,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
    -1,    -1,    34,    35,    -1,    -1,    38,    -1,    40,    41,
    42,    43,    -1,    -1,    -1,    -1,    -1,   633,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
    62,    63,    -1,    65,    66,    67,    -1,    69,    70,    -1,
    -1,    -1,    74,    -1,    76,    77,    78,    79,   664,    81,
    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,    91,
    92,    93,     3,    -1,    -1,     6,    -1,    -1,    -1,    -1,
    11,    12,    13,    14,    15,    16,    17,   109,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
    41,    42,    43,    -1,    45,    -1,    -1,    48,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    59,    60,
    97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
   107,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,
    -1,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,     3,   104,   105,     6,    -1,    -1,   109,   110,
    11,    12,    13,    14,    15,    16,    17,    48,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
    41,    42,    43,    -1,    45,    -1,    -1,    48,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    59,    60,
    -1,    -1,    -1,    -1,    -1,    -1,    97,    98,    99,   100,
   101,   102,   103,   104,   105,   106,   107,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,
    -1,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,     3,   104,   105,     6,    -1,    -1,   109,   110,
    11,    12,    13,    14,    15,    16,    17,    48,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
    41,    42,    43,    -1,    45,    -1,    -1,    48,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    59,    60,
    -1,    -1,    -1,    -1,    -1,    -1,    97,    98,    99,   100,
   101,   102,   103,   104,   105,   106,   107,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,
    -1,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,     3,   104,   105,     6,    -1,    -1,   109,   110,
    11,    12,    13,    14,    15,    16,    17,    -1,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
    41,    42,    43,    -1,    45,    -1,    -1,    48,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    59,    60,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,
    -1,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,     3,   104,   105,     6,    -1,    -1,   109,   110,
    11,    12,    13,    14,    15,    16,    17,    -1,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
    41,    42,    43,    -1,    45,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    59,    60,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,
    -1,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,     3,   104,   105,     6,    -1,    -1,   109,   110,
    11,    12,    13,    14,    15,    16,    17,    -1,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
    41,    42,    43,    -1,    45,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    59,    60,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,
    -1,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   104,   105,   106,    -1,     3,   109,   110,
     6,    -1,    -1,    -1,    10,    11,    12,    13,    14,    15,
    16,    17,    -1,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    40,    41,    42,    43,    -1,    45,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    58,    59,    60,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    88,    -1,    -1,    -1,    92,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,   104,   105,
     6,    -1,    -1,   109,   110,    11,    12,    13,    14,    15,
    16,    17,    -1,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    40,    41,    42,    43,    -1,    45,
     3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    58,    59,    60,    -1,    19,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    39,    40,    41,    42,
    43,    -1,    88,    -1,    47,    -1,    92,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,     3,    58,    59,    60,   104,   105,
    -1,    -1,    -1,   109,   110,    -1,    -1,    -1,    -1,    -1,
    19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    -1,    -1,    -1,    88,    -1,    -1,    91,    92,
    39,    40,    41,    42,    43,    -1,    -1,    -1,   101,    -1,
    -1,    -1,    -1,   106,    -1,     3,    -1,    -1,    -1,    58,
    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    88,
    -1,    -1,    91,    92,    42,    43,     0,     1,    -1,     3,
     4,    -1,   101,     7,     8,     9,    -1,   106,    -1,    -1,
    58,    59,    60,    -1,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    -1,    -1,    -1,
    34,    35,    -1,    -1,    -1,    -1,    40,    41,    42,    43,
    88,    -1,    46,    -1,    92,    -1,    50,    51,    52,    53,
    54,    55,    56,    57,    58,    59,    60,    -1,    -1,    -1,
    -1,    65,    66,    67,    -1,    69,    70,    71,    72,    73,
    74,    75,    76,    77,    78,    79,    80,    81,    -1,    -1,
    -1,    -1,    86,     1,    88,     3,     4,    91,    92,     7,
     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    -1,    -1,    -1,    34,    35,    -1,    -1,
    38,    -1,    40,    41,    42,    43,    -1,    -1,    46,    -1,
    -1,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    59,    60,    -1,    -1,    -1,    -1,    65,    66,    67,
    -1,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    -1,    -1,    -1,    -1,    86,     1,
    88,     3,     4,    91,    92,     7,     8,     9,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
    -1,    -1,    34,    35,    -1,    -1,    38,    -1,    40,    41,
    42,    43,    -1,    -1,    46,    -1,    -1,    -1,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
    -1,    -1,    -1,    65,    66,    67,    -1,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
    -1,    -1,    -1,    -1,    86,     1,    88,     3,     4,    91,
    92,     7,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    -1,    -1,    -1,    34,    35,
    -1,    -1,    38,    -1,    40,    41,    42,    43,    -1,    -1,
    46,    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    -1,    -1,    -1,    -1,    65,
    66,    67,    -1,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    81,    -1,    -1,    -1,    -1,
    86,     1,    88,     3,     4,    91,    92,     7,     8,     9,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    -1,    -1,    -1,    34,    35,    -1,    -1,    -1,    -1,
    40,    41,    42,    43,    -1,    -1,    46,    -1,    48,    -1,
    50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
    60,    -1,    -1,    -1,    -1,    65,    66,    67,    -1,    69,
    70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
    80,    81,     1,    -1,     3,    -1,    86,    -1,    88,    -1,
    -1,    91,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    39,    40,    41,    42,    43,     1,    -1,     3,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,
    59,    60,    -1,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    39,    40,    41,    42,    43,     1,    88,
     3,    -1,    91,    92,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    58,    59,    60,    -1,    19,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    42,
    43,    -1,    88,     3,    -1,    91,    92,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    58,    59,    60,    -1,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    40,    41,    42,    43,    -1,    88,    -1,    -1,    -1,    92,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    59,
    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,    88,     6,
    -1,    -1,    92,    93,    11,    12,    13,    14,    15,    16,
    17,    -1,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    39,    40,    41,    42,    43,    -1,     3,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    11,    -1,    -1,    -1,
    -1,    58,    59,    60,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    39,    40,    41,    42,    43,    -1,
     3,    88,    -1,    -1,    91,    92,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    58,    59,    60,    19,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    39,    40,    41,    42,
    43,    -1,     3,    88,    -1,    -1,    91,    92,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    58,    59,    60,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
    41,    42,    43,    -1,     3,    88,    -1,    -1,    91,    92,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    59,    60,
    19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    40,    41,    42,    43,    -1,     3,    88,    -1,    -1,
    -1,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,
    59,    60,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    40,    41,    42,    43,    -1,     3,    88,
    -1,    -1,    -1,    92,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    58,    59,    60,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    -1,
    -1,    88,    -1,    -1,    -1,    92,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    58,    59,    60,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    88,    -1,    -1,    -1,    92
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 946 "parser.y"
{
		   Setattr(yyvsp[0].node,"classes",classes); 
		   Setattr(yyvsp[0].node,"name",ModuleName);
		   
		   if ((!module_node) && ModuleName) {
		     module_node = new_node("module");
		     Setattr(module_node,"name",ModuleName);
		   }
		   Setattr(yyvsp[0].node,"module",module_node);
		   check_extensions();
	           top = yyvsp[0].node;
               ;
    break;}
case 2:
#line 958 "parser.y"
{
                 top = Getattr(yyvsp[-1].p,"type");
               ;
    break;}
case 3:
#line 961 "parser.y"
{
                 top = 0;
               ;
    break;}
case 4:
#line 964 "parser.y"
{
                 top = yyvsp[-1].p;
               ;
    break;}
case 5:
#line 967 "parser.y"
{
                 top = 0;
               ;
    break;}
case 6:
#line 972 "parser.y"
{  
                   appendChild(yyvsp[-1].node,yyvsp[0].node);
                   yyval.node = yyvsp[-1].node;
               ;
    break;}
case 7:
#line 976 "parser.y"
{
                   yyval.node = new_node("top");
               ;
    break;}
case 8:
#line 981 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 9:
#line 982 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 10:
#line 983 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 11:
#line 984 "parser.y"
{ yyval.node = 0; ;
    break;}
case 12:
#line 985 "parser.y"
{
                  yyval.node = 0;
		  if (!Swig_error_count()) {
		    static int last_error_line = -1;
		    if (last_error_line != cparse_line) {
		      Swig_error(cparse_file, cparse_line,"Syntax error in input.\n");
		      last_error_line = cparse_line;
		      skip_decl();
		    }
		  }
               ;
    break;}
case 13:
#line 997 "parser.y"
{ 
                  if (yyval.node) {
   		      add_symbols(yyval.node);
                  }
                  yyval.node = yyvsp[0].node; 
	       ;
    break;}
case 14:
#line 1013 "parser.y"
{
                  yyval.node = 0;
                  skip_decl();
               ;
    break;}
case 15:
#line 1023 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 16:
#line 1024 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 17:
#line 1025 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 18:
#line 1026 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 19:
#line 1027 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 20:
#line 1028 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 21:
#line 1029 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 22:
#line 1030 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 23:
#line 1031 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 24:
#line 1032 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 25:
#line 1033 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 26:
#line 1034 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 27:
#line 1035 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 28:
#line 1036 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 29:
#line 1037 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 30:
#line 1038 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 31:
#line 1039 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 32:
#line 1040 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 33:
#line 1041 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 34:
#line 1042 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 35:
#line 1043 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 36:
#line 1050 "parser.y"
{
               Node *cls;
	       String *clsname;
	       cplus_mode = CPLUS_PUBLIC;
	       if (!classes) classes = NewHash();
	       if (!extendhash) extendhash = NewHash();
	       clsname = make_class_name(yyvsp[-1].str);
	       cls = Getattr(classes,clsname);
	       if (!cls) {
		 /* No previous definition. Create a new scope */
		 Node *am = Getattr(extendhash,clsname);
		 if (!am) {
		   Swig_symbol_newscope();
		   Swig_symbol_setscopename(yyvsp[-1].str);
		   prev_symtab = 0;
		 } else {
		   prev_symtab = Swig_symbol_setscope(Getattr(am,"symtab"));
		 }
		 current_class = 0;
	       } else {
		 /* Previous class definition.  Use its symbol table */
		 prev_symtab = Swig_symbol_setscope(Getattr(cls,"symtab"));
		 current_class = cls;
		 extendmode = 1;
	       }
	       Classprefix = NewString(yyvsp[-1].str);
	       Namespaceprefix= Swig_symbol_qualifiedscopename(0);
	       Delete(clsname);
	     ;
    break;}
case 37:
#line 1078 "parser.y"
{
               String *clsname;
	       extendmode = 0;
               yyval.node = new_node("extend");
	       Setattr(yyval.node,"symtab",Swig_symbol_popscope());
	       if (prev_symtab) {
		 Swig_symbol_setscope(prev_symtab);
	       }
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
               clsname = make_class_name(yyvsp[-4].str);
	       Setattr(yyval.node,"name",clsname);

	       /* Mark members as extend */

	       Swig_tag_nodes(yyvsp[-1].node,"feature:extend",(char*) "1");
	       if (current_class) {
		 /* We add the extension to the previously defined class */
		 appendChild(yyval.node,yyvsp[-1].node);
		 appendChild(current_class,yyval.node);
	       } else {
		 /* We store the extensions in the extensions hash */
		 Node *am = Getattr(extendhash,clsname);
		 if (am) {
		   /* Append the members to the previous extend methods */
		   appendChild(am,yyvsp[-1].node);
		 } else {
		   appendChild(yyval.node,yyvsp[-1].node);
		   Setattr(extendhash,clsname,yyval.node);
		 }
	       }
	       current_class = 0;
	       Delete(Classprefix);
	       Delete(clsname);
	       Classprefix = 0;
	       prev_symtab = 0;
	       yyval.node = 0;

	     ;
    break;}
case 38:
#line 1122 "parser.y"
{
                    yyval.node = new_node("apply");
                    Setattr(yyval.node,"pattern",Getattr(yyvsp[-3].p,"pattern"));
		    appendChild(yyval.node,yyvsp[-1].p);
               ;
    break;}
case 39:
#line 1132 "parser.y"
{
		 yyval.node = new_node("clear");
		 appendChild(yyval.node,yyvsp[-1].p);
               ;
    break;}
case 40:
#line 1143 "parser.y"
{
		   if ((yyvsp[-1].dtype.type != T_ERROR) && (yyvsp[-1].dtype.type != T_SYMBOL)) {
		     yyval.node = new_node("constant");
		     Setattr(yyval.node,"name",yyvsp[-3].id);
		     Setattr(yyval.node,"type",NewSwigType(yyvsp[-1].dtype.type));
		     Setattr(yyval.node,"value",yyvsp[-1].dtype.val);
		     Setattr(yyval.node,"storage","%constant");
		     Setattr(yyval.node,"feature:immutable","1");
		     add_symbols(yyval.node);
		   } else {
		     if (yyvsp[-1].dtype.type == T_ERROR) {
		       Swig_warning(WARN_PARSE_UNSUPPORTED_VALUE,cparse_file,cparse_line,"Unsupported constant value (ignored)\n");
		     }
		     yyval.node = 0;
		   }

	       ;
    break;}
case 41:
#line 1161 "parser.y"
{
		 if ((yyvsp[-1].dtype.type != T_ERROR) && (yyvsp[-1].dtype.type != T_SYMBOL)) {
		   SwigType_push(yyvsp[-3].type,yyvsp[-2].decl.type);
		   /* Sneaky callback function trick */
		   if (SwigType_isfunction(yyvsp[-3].type)) {
		     SwigType_add_pointer(yyvsp[-3].type);
		   }
		   yyval.node = new_node("constant");
		   Setattr(yyval.node,"name",yyvsp[-2].decl.id);
		   Setattr(yyval.node,"type",yyvsp[-3].type);
		   Setattr(yyval.node,"value",yyvsp[-1].dtype.val);
		   Setattr(yyval.node,"storage","%constant");
		   Setattr(yyval.node,"feature:immutable","1");
		   add_symbols(yyval.node);
		 } else {
		     if (yyvsp[-1].dtype.type == T_ERROR) {
		       Swig_warning(WARN_PARSE_UNSUPPORTED_VALUE,cparse_file,cparse_line,"Unsupported constant value\n");
		     }
		   yyval.node = 0;
		 }
               ;
    break;}
case 42:
#line 1182 "parser.y"
{
		 Swig_warning(WARN_PARSE_BAD_VALUE,cparse_file,cparse_line,"Bad constant value (ignored).\n");
		 yyval.node = 0;
	       ;
    break;}
case 43:
#line 1193 "parser.y"
{
		 char temp[64];
		 Replace(yyvsp[0].str,"$file",cparse_file, DOH_REPLACE_ANY);
		 sprintf(temp,"%d", cparse_line);
		 Replace(yyvsp[0].str,"$line",temp,DOH_REPLACE_ANY);
		 Printf(stderr,"%s\n", yyvsp[0].str);
		 Delete(yyvsp[0].str);
                 yyval.node = 0;
	       ;
    break;}
case 44:
#line 1202 "parser.y"
{
		 char temp[64];
		 String *s = NewString(yyvsp[0].id);
		 Replace(s,"$file",cparse_file, DOH_REPLACE_ANY);
		 sprintf(temp,"%d", cparse_line);
		 Replace(s,"$line",temp,DOH_REPLACE_ANY);
		 Printf(stderr,"%s\n", s);
		 Delete(s);
                 yyval.node = 0;
               ;
    break;}
case 45:
#line 1221 "parser.y"
{
                    skip_balanced('{','}');
		    yyval.node = 0;
		    Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
	       ;
    break;}
case 46:
#line 1227 "parser.y"
{
                    skip_balanced('{','}');
		    yyval.node = 0;
		    Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
               ;
    break;}
case 47:
#line 1233 "parser.y"
{
		 yyval.node = 0;
		 Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
               ;
    break;}
case 48:
#line 1238 "parser.y"
{
		 yyval.node = 0;
		 Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
	       ;
    break;}
case 49:
#line 1248 "parser.y"
{
                 yyval.node = new_node("fragment");
                 Setattr(yyval.node,"section", yyvsp[-2].id);
                 Setattr(yyval.node,"name",yyvsp[-4].id);
                 Setattr(yyval.node,"code",yyvsp[0].str);
                 ;
    break;}
case 50:
#line 1254 "parser.y"
{
                   skip_balanced('{','}');
		   yyval.node = new_node("fragment");
		   Setattr(yyval.node,"section",yyvsp[-2].id);
		   Setattr(yyval.node,"name",yyvsp[-4].id);
		   Delitem(scanner_ccode,0);
		   Delitem(scanner_ccode,DOH_END);
		   Setattr(yyval.node,"code",Copy(scanner_ccode));
		 ;
    break;}
case 51:
#line 1270 "parser.y"
{
                     yyvsp[-3].loc.filename = Swig_copy_string(cparse_file);
		     yyvsp[-3].loc.line = cparse_line;
		     cparse_file = Swig_copy_string(yyvsp[-1].id);
		     cparse_line = 0;
               ;
    break;}
case 52:
#line 1275 "parser.y"
{
                     yyval.node = yyvsp[-1].node;
		     cparse_file = yyvsp[-6].loc.filename;
		     cparse_line = yyvsp[-6].loc.line;
		     if (strcmp(yyvsp[-6].loc.type,"include") == 0) set_nodeType(yyval.node,"include");
		     if (strcmp(yyvsp[-6].loc.type,"import") == 0) set_nodeType(yyval.node,"import");
		     Setattr(yyval.node,"name",yyvsp[-4].id);
		     /* Search for the module (if any) */
		     {
			 Node *n = firstChild(yyval.node);
			 while (n) {
			     if (Strcmp(nodeType(n),"module") == 0) {
				 Setattr(yyval.node,"module",Getattr(n,"name"));
				 break;
			     }
			     n = nextSibling(n);
			 }
		     }
		     Setattr(yyval.node,"options",yyvsp[-5].node);
               ;
    break;}
case 53:
#line 1297 "parser.y"
{ yyval.loc.type = (char *) "include"; ;
    break;}
case 54:
#line 1298 "parser.y"
{ yyval.loc.type = (char *) "import"; ;
    break;}
case 55:
#line 1305 "parser.y"
{
                 String *cpps;
		 if (Namespaceprefix) {
		   Swig_error(cparse_file, cparse_start_line, "Error. %%inline directive inside a namespace is disallowed.\n");

		   yyval.node = 0;
		 } else {
		   yyval.node = new_node("insert");
		   Setattr(yyval.node,"code",yyvsp[0].str);
		   /* Need to run through the preprocessor */
		   Setline(yyvsp[0].str,cparse_start_line);
		   Setfile(yyvsp[0].str,cparse_file);
		   Seek(yyvsp[0].str,0,SEEK_SET);
		   cpps = Preprocessor_parse(yyvsp[0].str);
		   start_inline(Char(cpps), cparse_start_line);
		   Delete(yyvsp[0].str);
		   Delete(cpps);
		 }
		 
	       ;
    break;}
case 56:
#line 1325 "parser.y"
{
                 String *cpps;
		 skip_balanced('{','}');
		 if (Namespaceprefix) {
		   Swig_error(cparse_file, cparse_start_line, "Error. %%inline directive inside a namespace is disallowed.\n");
		   
		   yyval.node = 0;
		 } else {
                   yyval.node = new_node("insert");
		   Delitem(scanner_ccode,0);
		   Delitem(scanner_ccode,DOH_END);
		   Setattr(yyval.node,"code", Copy(scanner_ccode));
		   cpps=Copy(scanner_ccode);
		   start_inline(Char(cpps), cparse_start_line);
		   Delete(cpps);
		 }
               ;
    break;}
case 57:
#line 1352 "parser.y"
{
                 yyval.node = new_node("insert");
		 Setattr(yyval.node,"code",yyvsp[0].str);
	       ;
    break;}
case 58:
#line 1356 "parser.y"
{
		 String *code = NewString("");
		 yyval.node = new_node("insert");
		 Setattr(yyval.node,"section",yyvsp[-2].id);
		 Setattr(yyval.node,"code",code);
		 if (Swig_insert_file(yyvsp[0].id,code) < 0) {
		   Swig_error(cparse_file, cparse_line, "Couldn't find '%s'.\n", yyvsp[0].id);
		   yyval.node = 0;
		 } 
               ;
    break;}
case 59:
#line 1366 "parser.y"
{
		 yyval.node = new_node("insert");
		 Setattr(yyval.node,"section",yyvsp[-2].id);
		 Setattr(yyval.node,"code",yyvsp[0].str);
               ;
    break;}
case 60:
#line 1371 "parser.y"
{
                 skip_balanced('{','}');
		 yyval.node = new_node("insert");
		 Setattr(yyval.node,"section",yyvsp[-2].id);
		 Delitem(scanner_ccode,0);
		 Delitem(scanner_ccode,DOH_END);
		 Setattr(yyval.node,"code", Copy(scanner_ccode));
	       ;
    break;}
case 61:
#line 1386 "parser.y"
{
                 yyval.node = new_node("module");
		 Setattr(yyval.node,"name",yyvsp[0].id);
		 if (yyvsp[-1].node) Setattr(yyval.node,"options",yyvsp[-1].node);
		 if (yyvsp[-1].node && Getattr(yyvsp[-1].node,"directors") && Getattr(yyvsp[-1].node,"dirprot"))
		   dirprot_mode = 1;
		 if (!ModuleName) ModuleName = NewString(yyvsp[0].id);
		 if (!module_node) module_node = yyval.node;
	       ;
    break;}
case 62:
#line 1402 "parser.y"
{
                 yyrename = NewString(yyvsp[-1].id);
		 yyval.node = 0;
               ;
    break;}
case 63:
#line 1406 "parser.y"
{
                   yyval.node = 0;
		   Swig_error(cparse_file,cparse_line,"Missing argument to %%name directive.\n");
	       ;
    break;}
case 64:
#line 1418 "parser.y"
{
                 yyval.node = new_node("native");
		 Setattr(yyval.node,"name",yyvsp[-4].id);
		 Setattr(yyval.node,"wrap:name",yyvsp[-1].id);
	         add_symbols(yyval.node);
	       ;
    break;}
case 65:
#line 1424 "parser.y"
{
		 if (!SwigType_isfunction(yyvsp[-1].decl.type)) {
		   Swig_error(cparse_file,cparse_line,"%%native declaration '%s' is not a function.\n", yyvsp[-1].decl.id);
		   yyval.node = 0;
		 } else {
		     Delete(SwigType_pop_function(yyvsp[-1].decl.type));
		     /* Need check for function here */
		     SwigType_push(yyvsp[-2].type,yyvsp[-1].decl.type);
		     yyval.node = new_node("native");
	             Setattr(yyval.node,"name",yyvsp[-5].id);
		     Setattr(yyval.node,"wrap:name",yyvsp[-1].decl.id);
		     Setattr(yyval.node,"type",yyvsp[-2].type);
		     Setattr(yyval.node,"parms",yyvsp[-1].decl.parms);
		     Setattr(yyval.node,"decl",yyvsp[-1].decl.type);
		 }
	         add_symbols(yyval.node);
	       ;
    break;}
case 66:
#line 1450 "parser.y"
{
                 yyval.node = new_node("pragma");
		 Setattr(yyval.node,"lang",yyvsp[-3].id);
		 Setattr(yyval.node,"name",yyvsp[-2].id);
		 Setattr(yyval.node,"value",yyvsp[0].str);
	       ;
    break;}
case 67:
#line 1456 "parser.y"
{
		yyval.node = new_node("pragma");
		Setattr(yyval.node,"lang",yyvsp[-1].id);
		Setattr(yyval.node,"name",yyvsp[0].id);
	      ;
    break;}
case 68:
#line 1463 "parser.y"
{ yyval.str = NewString(yyvsp[0].id); ;
    break;}
case 69:
#line 1464 "parser.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 70:
#line 1467 "parser.y"
{ yyval.id = yyvsp[-1].id; ;
    break;}
case 71:
#line 1468 "parser.y"
{ yyval.id = (char *) "swig"; ;
    break;}
case 72:
#line 1476 "parser.y"
{
                    SwigType *t = yyvsp[-2].decl.type;
		    if (!Len(t)) t = 0;
		    if (yyvsp[-3].ivalue) {
		      rename_add(yyvsp[-2].decl.id,t,yyvsp[-1].id);
		    } else {
		      namewarn_add(yyvsp[-2].decl.id,t,yyvsp[-1].id);
		    }
		    yyval.node = 0;
		    scanner_clear_rename();
              ;
    break;}
case 73:
#line 1487 "parser.y"
{
		String *fixname;
		SwigType *t = yyvsp[-2].decl.type;
		fixname = feature_identifier_fix(yyvsp[-2].decl.id);
		if (!Len(t)) t = 0;
		/* Special declarator check */
		if (t) {
		  if (yyvsp[-1].dtype.qualifier) SwigType_push(t,yyvsp[-1].dtype.qualifier);
		  if (SwigType_isfunction(t)) {
		    SwigType *decl = SwigType_pop_function(t);
		    if (SwigType_ispointer(t)) {
		      String *nname = NewStringf("*%s",fixname);
		      if (yyvsp[-6].ivalue) {
			rename_add(Char(nname),decl,yyvsp[-4].id);
		      } else {
			namewarn_add(Char(nname),decl,yyvsp[-4].id);
		      }
		      Delete(nname);
		    } else {
		      if (yyvsp[-6].ivalue) {
			rename_add(Char(fixname),decl,yyvsp[-4].id);
		      } else {
			namewarn_add(Char(fixname),decl,yyvsp[-4].id);
		      }
		    }
		  } else if (SwigType_ispointer(t)) {
		    String *nname = NewStringf("*%s",fixname);
		    if (yyvsp[-6].ivalue) {
		      rename_add(Char(nname),0,yyvsp[-4].id);
		    } else {
		      namewarn_add(Char(nname),0,yyvsp[-4].id);
		    }
		    Delete(nname);
		  }
		} else {
		  if (yyvsp[-6].ivalue) {
		    rename_add(Char(fixname),0,yyvsp[-4].id);
		  } else {
		    namewarn_add(Char(fixname),0,yyvsp[-4].id);
		  }
		}
                yyval.node = 0;
		scanner_clear_rename();
              ;
    break;}
case 74:
#line 1531 "parser.y"
{
		if (yyvsp[-5].ivalue) {
		  rename_add(yyvsp[-1].id,0,yyvsp[-3].id);
		} else {
		  namewarn_add(yyvsp[-1].id,0,yyvsp[-3].id);
		}
		yyval.node = 0;
		scanner_clear_rename();
              ;
    break;}
case 75:
#line 1542 "parser.y"
{
		    yyval.ivalue = 1;
                ;
    break;}
case 76:
#line 1545 "parser.y"
{
                    yyval.ivalue = 0;
                ;
    break;}
case 77:
#line 1558 "parser.y"
{
                 String *fname;
                 String *val;
		 String *name;
		 String *fixname;
		 SwigType *t;
                 if (!features_hash) features_hash = NewHash();
		 fname = NewStringf("feature:%s",yyvsp[-4].id);
		 fixname = feature_identifier_fix(yyvsp[-2].decl.id);
		 if (Namespaceprefix) {
		   name = NewStringf("%s::%s",Namespaceprefix, fixname);
		 } else {
		   name = fixname;
		 }
		 val = yyvsp[0].str ? NewString(yyvsp[0].str) : NewString("1");
		 if (yyvsp[-2].decl.parms) {
		   Setmeta(val,"parms",yyvsp[-2].decl.parms);
		 }
		 t = yyvsp[-2].decl.type;
		 if (yyvsp[-2].decl.parms) Setmeta(val,"parms",yyvsp[-2].decl.parms);
		 if (!Len(t)) t = 0;
		 if (t) {
		   if (yyvsp[-1].dtype.qualifier) SwigType_push(t,yyvsp[-1].dtype.qualifier);
		   if (SwigType_isfunction(t)) {
		     SwigType *decl = SwigType_pop_function(t);
		     if (SwigType_ispointer(t)) {
		       String *nname = NewStringf("*%s",name);
		       Swig_feature_set(features_hash, nname, decl, fname, val);
		       Delete(nname);
		     } else {
		       Swig_feature_set(features_hash, name, decl, fname, val);
		     }
		   } else if (SwigType_ispointer(t)) {
		     String *nname = NewStringf("*%s",name);
		     Swig_feature_set(features_hash,nname,0,fname,val);
		     Delete(nname);
		   }
		 } else {
		   Swig_feature_set(features_hash,name,0,fname,val);
		 }
		 Delete(fname);
		 Delete(name);
		 yyval.node = 0;
              ;
    break;}
case 78:
#line 1605 "parser.y"
{
                 String *fname;
                 String *val;
		 String *name;
		 String *fixname;
		 SwigType *t;

                 if (!features_hash) features_hash = NewHash();
		 fname = NewStringf("feature:%s",yyvsp[-6].id);
		 fixname = feature_identifier_fix(yyvsp[-2].decl.id);
		 if (Namespaceprefix) {
		   name = NewStringf("%s::%s",Namespaceprefix, fixname);
		 } else {
		   name = fixname;
		 }
		 if (Len(yyvsp[-4].id)) {
		   val = NewString(yyvsp[-4].id);
		 } else {
		   val = 0;
		 }
		 if (yyvsp[-2].decl.parms) {
		   Setmeta(val,"parms",yyvsp[-2].decl.parms);
		 }
		 t = yyvsp[-2].decl.type;
		 if (yyvsp[-2].decl.parms) Setmeta(val,"parms",yyvsp[-2].decl.parms);
		 if (!Len(t)) t = 0;
		 if (t) {
		   if (yyvsp[-1].dtype.qualifier) SwigType_push(t,yyvsp[-1].dtype.qualifier);
		   if (SwigType_isfunction(t)) {
		     SwigType *decl = SwigType_pop_function(t);
		     if (SwigType_ispointer(t)) {
		       String *nname = NewStringf("*%s",name);
		       Swig_feature_set(features_hash, nname, decl, fname, val);
		       Delete(nname);
		     } else {
		       Swig_feature_set(features_hash, name, decl, fname, val);
		     }
		   } else if (SwigType_ispointer(t)) {
		     String *nname = NewStringf("*%s",name);
		     Swig_feature_set(features_hash,nname,0,fname,val);
		     Delete(nname);
		   }
		 } else {
		   Swig_feature_set(features_hash,name,0,fname,val);
		 }
		 Delete(fname);
		 Delete(name);
		 yyval.node = 0;
              ;
    break;}
case 79:
#line 1657 "parser.y"
{
		String *name;
		String *fname = NewStringf("feature:%s",yyvsp[-2].id);
		if (!features_hash) features_hash = NewHash();
		if (Namespaceprefix) name = NewStringf("%s::", Namespaceprefix);
		else name = NewString("");
		Swig_feature_set(features_hash,name,0,fname,(yyvsp[0].str ? NewString(yyvsp[0].str) : NewString("1")));
		Delete(name);
		Delete(fname);
		yyval.node = 0;
              ;
    break;}
case 80:
#line 1668 "parser.y"
{
		String *name;
		String *fname = NewStringf("feature:%s",yyvsp[-4].id);
		if (!features_hash) features_hash = NewHash();
		if (Namespaceprefix) name = NewStringf("%s::", Namespaceprefix);
		else name = NewString("");
		Swig_feature_set(features_hash,name,0,fname,(Len(yyvsp[-2].id) ? NewString(yyvsp[-2].id) : 0));
		Delete(name);
		Delete(fname);
		yyval.node = 0;
              ;
    break;}
case 81:
#line 1681 "parser.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 82:
#line 1682 "parser.y"
{ yyval.str = 0; ;
    break;}
case 83:
#line 1683 "parser.y"
{ yyval.str = yyvsp[-2].pl; ;
    break;}
case 84:
#line 1688 "parser.y"
{
                 Parm *val;
		 String *name;
		 SwigType *t;
                 if (!features_hash) features_hash = NewHash();
		 if (Namespaceprefix) name = NewStringf("%s::%s", Namespaceprefix, yyvsp[-2].decl.id);
		 else name = NewString(yyvsp[-2].decl.id);
		 val = yyvsp[-4].pl;
		 if (yyvsp[-2].decl.parms) {
		   Setmeta(val,"parms",yyvsp[-2].decl.parms);
		 }
		 t = yyvsp[-2].decl.type;
		 if (!Len(t)) t = 0;
		 if (t) {
		   if (yyvsp[-1].dtype.qualifier) SwigType_push(t,yyvsp[-1].dtype.qualifier);
		   if (SwigType_isfunction(t)) {
		     SwigType *decl = SwigType_pop_function(t);
		     if (SwigType_ispointer(t)) {
		       String *nname = NewStringf("*%s",name);
		       Swig_feature_set(features_hash, nname, decl, "feature:varargs", val);
		       Delete(nname);
		     } else {
		       Swig_feature_set(features_hash, name, decl, "feature:varargs", val);
		     }
		   } else if (SwigType_ispointer(t)) {
		     String *nname = NewStringf("*%s",name);
		     Swig_feature_set(features_hash,nname,0,"feature:varargs",val);
		     Delete(nname);
		   }
		 } else {
		   Swig_feature_set(features_hash,name,0,"feature:varargs",val);
		 }
		 Delete(name);
		 yyval.node = 0;
              ;
    break;}
case 85:
#line 1724 "parser.y"
{ yyval.pl = yyvsp[0].pl; ;
    break;}
case 86:
#line 1725 "parser.y"
{ 
		  int i;
		  int n;
		  Parm *p;
		  n = atoi(Char(yyvsp[-2].dtype.val));
		  if (n <= 0) {
		    Swig_error(cparse_file, cparse_line,"Argument count in %%varargs must be positive.\n");
		    yyval.pl = 0;
		  } else {
		    yyval.pl = Copy(yyvsp[0].p);
		    Setattr(yyval.pl,"name","VARARGS_SENTINEL");
		    for (i = 0; i < n; i++) {
		      p = Copy(yyvsp[0].p);
		      set_nextSibling(p,yyval.pl);
		      yyval.pl = p;
		    }
		  }
                ;
    break;}
case 87:
#line 1754 "parser.y"
{
		   yyval.node = 0;
		   if (yyvsp[-3].tmap.op) {
		     yyval.node = new_node("typemap");
		     Setattr(yyval.node,"method",yyvsp[-3].tmap.op);
		     Setattr(yyval.node,"code",NewString(yyvsp[0].str));
		     if (yyvsp[-3].tmap.kwargs) {
		       Setattr(yyval.node,"kwargs", yyvsp[-3].tmap.kwargs);
		     }
		     appendChild(yyval.node,yyvsp[-1].p);
		   }
	       ;
    break;}
case 88:
#line 1766 "parser.y"
{
		 yyval.node = 0;
		 if (yyvsp[-3].tmap.op) {
		   yyval.node = new_node("typemap");
		   Setattr(yyval.node,"method",yyvsp[-3].tmap.op);
		   appendChild(yyval.node,yyvsp[-1].p);
		 }
	       ;
    break;}
case 89:
#line 1774 "parser.y"
{
		   yyval.node = 0;
		   if (yyvsp[-5].tmap.op) {
		     yyval.node = new_node("typemapcopy");
		     Setattr(yyval.node,"method",yyvsp[-5].tmap.op);
		     Setattr(yyval.node,"pattern", Getattr(yyvsp[-1].p,"pattern"));
		     appendChild(yyval.node,yyvsp[-3].p);
		   }
	       ;
    break;}
case 90:
#line 1787 "parser.y"
{
		 Hash *p;
		 String *name;
		 p = nextSibling(yyvsp[0].node);
		 if (p && (!Getattr(p,"value"))) {
		   /* two argument typemap form */
		   name = Getattr(yyvsp[0].node,"name");
		   if (!name || (Strcmp(name,typemap_lang))) {
		     yyval.tmap.op = 0;
		     yyval.tmap.kwargs = 0;
		   } else {
		     yyval.tmap.op = Getattr(p,"name");
		     yyval.tmap.kwargs = nextSibling(p);
		   }
		 } else {
		   /* one-argument typemap-form */
		   yyval.tmap.op = Getattr(yyvsp[0].node,"name");
		   yyval.tmap.kwargs = p;
		 }
                ;
    break;}
case 91:
#line 1809 "parser.y"
{
                 yyval.p = yyvsp[-1].p;
		 set_nextSibling(yyval.p,yyvsp[0].p);
		;
    break;}
case 92:
#line 1815 "parser.y"
{
                 yyval.p = yyvsp[-1].p;
		 set_nextSibling(yyval.p,yyvsp[0].p);
                ;
    break;}
case 93:
#line 1819 "parser.y"
{ yyval.p = 0;;
    break;}
case 94:
#line 1822 "parser.y"
{
		  SwigType_push(yyvsp[-1].type,yyvsp[0].decl.type);
		  yyval.p = new_node("typemapitem");
		  Setattr(yyval.p,"pattern",NewParm(yyvsp[-1].type,yyvsp[0].decl.id));
		  Setattr(yyval.p,"parms", yyvsp[0].decl.parms);
		  /*		  $$ = NewParm($1,$2.id);
				  Setattr($$,"parms",$2.parms); */
                ;
    break;}
case 95:
#line 1830 "parser.y"
{
                  yyval.p = new_node("typemapitem");
		  Setattr(yyval.p,"pattern",yyvsp[-1].pl);
		  /*		  Setattr($$,"multitype",$2); */
               ;
    break;}
case 96:
#line 1835 "parser.y"
{
		 yyval.p = new_node("typemapitem");
		 Setattr(yyval.p,"pattern", yyvsp[-4].pl);
		 /*                 Setattr($$,"multitype",$2); */
		 Setattr(yyval.p,"parms",yyvsp[-1].pl);
               ;
    break;}
case 97:
#line 1847 "parser.y"
{
                   yyval.node = new_node("types");
		   Setattr(yyval.node,"parms",yyvsp[-2].pl);
               ;
    break;}
case 98:
#line 1857 "parser.y"
{
                  Parm *p, *tp;
		  Node *n;
		  Node *nspace = 0, *nspace_inner = 0;
		  Node *tnode = 0;
		  Symtab *tscope = 0;
		  int     specialized = 0;
		  yyval.node = 0;

		  tscope = Swig_symbol_current();          /* Get the current scope */

		  /* If the template name is qualified.  We need to create or lookup namespace entries */
		  if (Swig_scopename_check(yyvsp[-4].str)) {
		    String *prefix, *base;
		    Node   *ns;
		    prefix = Swig_scopename_prefix(yyvsp[-4].str);
		    base = Swig_scopename_last(yyvsp[-4].str);
		    
		    /* Try to locate the scope */
		    ns = Swig_symbol_clookup(prefix,0);
		    if (!ns) {
		      Swig_error(cparse_file,cparse_line,"Undefined scope '%s'\n", prefix);
		    } else {
		      if (Strcmp(nodeType(ns),"namespace") != 0) {
			Swig_error(cparse_file,cparse_line,"'%s' is not defined as namespace.\n", prefix);
			ns = 0;
		      } else {
			/*			Swig_symbol_setscope(Getattr(ns,"symtab"));
						Namespaceprefix = Swig_symbol_qualifiedscopename(0); */
		      }
		    }

		    if (ns && Namespaceprefix) {
		      Swig_error(cparse_file,cparse_line,
				 "Can't instantiate template '%s' inside namespace '%s'.\n"
				 "Suggest moving %%template outside the namespace.\n", yyvsp[-4].str, Namespaceprefix);

		    }
		    
		    /* Create namespace nodes to enclose the template declaration */
		    if (ns) {
		      List *scopes;
		      String *sname;
		      Iterator si;
		      String *name = NewString(prefix);
		      scopes = NewList();
		      while (name) {
			String *tprefix;
			String *base = Swig_scopename_last(name);
			Insert(scopes,0,base);
			tprefix = Swig_scopename_prefix(name);
			Delete(name);
			name = tprefix;
		      }
		      for (si = First(scopes); si.item; si = Next(si)) {
			Node *ns1,*ns2;
			sname = si.item;
			ns1 = Swig_symbol_clookup(sname,0);
			assert(ns1);
			if (Strcmp(nodeType(ns1),"namespace") == 0) {
			  if (Getattr(ns1,"alias")) {
			    ns1 = Getattr(ns1,"namespace");
			  }
			} else {
			  assert(0);
			}
			ns2 = new_node("namespace");
			Setattr(ns2,"name",sname);
			Setattr(ns2,"symtab", Getattr(ns1,"symtab"));
			add_symbols(ns2);
			Swig_symbol_setscope(Getattr(ns1,"symtab"));
			Namespaceprefix = Swig_symbol_qualifiedscopename(0);
			if (nspace_inner) {
			  appendChild(nspace_inner,ns2);
			}
			nspace_inner = ns2;
			if (!nspace) nspace = ns2;
		      }
		      yyvsp[-4].str = base;
		    }
		  }

		  n = Swig_cparse_template_locate(yyvsp[-4].str,yyvsp[-2].p);

		  /* Patch the argument types to respect namespaces */
		  p = yyvsp[-2].p;
		  while (p) {
		    if (!Getattr(p,"value")) {
		      SwigType *ty = Getattr(p,"type");
		      if (ty) {
			ty = Swig_symbol_type_qualify(ty,0);
			/*			ty = Swig_symbol_typedef_reduce(ty,0); */
			Setattr(p,"type",ty);
		      }
		    }
		    p = nextSibling(p);
		  }
		  /* Look for the template */

		  if (n && (Strcmp(nodeType(n),"template") == 0)) {
		    Parm *tparms = Getattr(n,"templateparms");
		    if (!tparms) {
		      specialized = 1;
		    }
		    if (!specialized && ((ParmList_len(yyvsp[-2].p) > ParmList_len(tparms)))) {
		      Swig_error(cparse_file, cparse_line, "Too many template parameters. Maximum of %d.\n", ParmList_len(tparms));
		    } else if (!specialized && ((ParmList_len(yyvsp[-2].p) < ParmList_numrequired(tparms)))) {
		      Swig_error(cparse_file, cparse_line, "Not enough template parameters specified. %d required.\n", ParmList_numrequired(tparms));
		    } else {
		      int  def_supplied = 0;
		      /* Expand the template */
		      ParmList *temparms;
		      if (specialized) temparms = CopyParmList(yyvsp[-2].p);
		      else temparms = CopyParmList(tparms);

		      /* Create typedef's and arguments */
		      p = yyvsp[-2].p;
		      tp = temparms;
		      while (p) {
			String *value = Getattr(p,"value");
			if (def_supplied) {
			  Setattr(p,"default","1");
			}
			if (value) {
			  Setattr(tp,"value",value);
			} else {
			  SwigType *ty = Getattr(p,"type");
			  if (ty) {
			    Setattr(tp,"type",ty);
			  }
			  Delattr(tp,"value");
			}
			p = nextSibling(p);
			tp = nextSibling(tp);
			if (!p && tp) {
			  p = tp;
			  def_supplied = 1;
			}
		      }

		      yyval.node = copy_node(n);
		      /* We need to set the node name based on name used to instantiate */
		      Setattr(yyval.node,"name",Copy(yyvsp[-4].str));
		      if (!specialized) {
			Delattr(yyval.node,"sym:typename");
		      } else {
			Setattr(yyval.node,"sym:typename","1");
		      }
		      if (yyvsp[-6].id) {
			Swig_cparse_template_expand(yyval.node,yyvsp[-6].id,temparms);
			Setattr(yyval.node,"sym:name",yyvsp[-6].id);
		      } else {
			static int cnt = 0;
			String *nname = NewStringf("__dummy_%d__", cnt++);
			Swig_cparse_template_expand(yyval.node,nname,temparms);
			Setattr(yyval.node,"sym:name",nname);
			Setattr(yyval.node,"feature:ignore","1");
		      }
		      Delattr(yyval.node,"templatetype");
		      Setattr(yyval.node,"template",n);
		      tnode = yyval.node;
		      Setfile(yyval.node,cparse_file);
		      Setline(yyval.node,cparse_line);
		      Delete(temparms);
		      
		      add_symbols_copy(yyval.node);
		      if (Strcmp(nodeType(yyval.node),"class") == 0) {

			/* Identify pure abstract methods */
			Setattr(yyval.node,"abstract", pure_abstract(firstChild(yyval.node)));
			
                        /* Set up inheritance in symbol table */
			{
			  Symtab  *csyms;
 			  List *baselist = Getattr(yyval.node,"baselist");
			  csyms = Swig_symbol_current();
			  Swig_symbol_setscope(Getattr(yyval.node,"symtab"));
			  if (baselist) {
			    List *bases = make_inherit_list(Getattr(yyval.node,"name"),baselist);
			    if (bases) {
			      Iterator s;
			      for (s = First(bases); s.item; s = Next(s)) {
				Symtab *st = Getattr(s.item,"symtab");
				if (st) {
				  Swig_symbol_inherit(st);
				}
			      }
			    }
			  }
			  Swig_symbol_setscope(csyms);
			}

			/* Merge in addmethods for this class */
			
			/* !!! This may be broken.  We may have to  add the addmethods at the beginning of
			   the class */
			
			if (extendhash) {
			  String *clsname;
			  Node *am;
			  if (Namespaceprefix) {
			    clsname = NewStringf("%s::%s", Namespaceprefix, Getattr(yyval.node,"name"));
			  } else {
			    clsname = Getattr(yyval.node,"name");
			  }
			  am = Getattr(extendhash,clsname);
			  if (am) {
			    Symtab *st = Swig_symbol_current();
			    Swig_symbol_setscope(Getattr(yyval.node,"symtab"));
			    /*			    Printf(stdout,"%s: %s %x %x\n", Getattr($$,"name"), clsname, Swig_symbol_current(), Getattr($$,"symtab")); */
			    merge_extensions(yyval.node,am);
			    Swig_symbol_setscope(st);
			    appendChild(yyval.node,am);
			    Delattr(extendhash,clsname);
			  }
			}
			/* Add to classes hash */
			if (!classes) classes = NewHash();

			{
			  if (Namespaceprefix) {
			    String *temp = NewStringf("%s::%s", Namespaceprefix, Getattr(yyval.node,"name"));
			    Setattr(classes,temp,yyval.node);
			  } else {
			    Setattr(classes,Swig_symbol_qualifiedscopename(yyval.node),yyval.node);
			  }
			}
		      }
		    }
		    if (yyval.node && nspace) {
		      appendChild(nspace_inner,yyval.node);
		      yyval.node = nspace;
		    }
		  }
   	          Swig_symbol_setscope(tscope);
		  Namespaceprefix = Swig_symbol_qualifiedscopename(0);

                ;
    break;}
case 99:
#line 2102 "parser.y"
{
		  Swig_warning(0,cparse_file, cparse_line,"%s\n", yyvsp[0].id);
		  yyval.node = 0;
               ;
    break;}
case 100:
#line 2112 "parser.y"
{
                    yyval.node = yyvsp[0].node; 
                    if (yyval.node) {
   		      add_symbols(yyval.node);
   	            }
                ;
    break;}
case 101:
#line 2118 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 102:
#line 2122 "parser.y"
{ 
		  if (Strcmp(yyvsp[-3].id,"C") == 0) {
		    yyval.node = new_node("extern");
		    Setattr(yyval.node,"name",yyvsp[-3].id);
		    appendChild(yyval.node,firstChild(yyvsp[-1].node));
		  } else {
		     Swig_warning(WARN_PARSE_UNDEFINED_EXTERN,cparse_file, cparse_line,"Unrecognized extern type \"%s\" (ignored).\n", yyvsp[-3].id);
		     yyval.node = 0;
		  }
                ;
    break;}
case 103:
#line 2138 "parser.y"
{
              yyval.node = new_node("cdecl");
	      if (yyvsp[-1].dtype.qualifier) SwigType_push(yyvsp[-2].decl.type,yyvsp[-1].dtype.qualifier);
	      Setattr(yyval.node,"type",yyvsp[-3].type);
	      Setattr(yyval.node,"storage",yyvsp[-4].id);
	      Setattr(yyval.node,"name",yyvsp[-2].decl.id);
	      Setattr(yyval.node,"decl",yyvsp[-2].decl.type);
	      Setattr(yyval.node,"parms",yyvsp[-2].decl.parms);
	      Setattr(yyval.node,"value",yyvsp[-1].dtype.val);
	      Setattr(yyval.node,"throws",yyvsp[-1].dtype.throws);
	      if (!yyvsp[0].node) {
		if (Len(scanner_ccode)) {
		  Setattr(yyval.node,"code",Copy(scanner_ccode));
		}
	      } else {
		Node *n = yyvsp[0].node;
		/* Inherit attributes */
		while (n) {
		  Setattr(n,"type",Copy(yyvsp[-3].type));
		  Setattr(n,"storage",yyvsp[-4].id);
		  n = nextSibling(n);
		}
	      }
	      if (yyvsp[-1].dtype.bitfield) {
		Setattr(yyval.node,"bitfield", yyvsp[-1].dtype.bitfield);
	      }

	      /* Look for "::" declarations (ignored) */
	      if (Strstr(yyvsp[-2].decl.id,"::")) {
		if (Namespaceprefix) {
		  /* This is a special case. If the scope name of the declaration exactly
                     matches that of the declaration, then we will allow it. Otherwise, delete. */
		  String *p = Swig_scopename_prefix(yyvsp[-2].decl.id);
		  if (Strcmp(p,Namespaceprefix) == 0) {
		    Setattr(yyval.node,"name",Swig_scopename_last(yyvsp[-2].decl.id));
		    set_nextSibling(yyval.node,yyvsp[0].node);
		  } else {
		    Delete(yyval.node);
		    yyval.node = yyvsp[0].node;
		  }
		  Delete(p);
		} else {
		  Delete(yyval.node);
		  yyval.node = yyvsp[0].node;
		}
	      } else {
		set_nextSibling(yyval.node,yyvsp[0].node);
	      }
           ;
    break;}
case 104:
#line 2191 "parser.y"
{ 
                   yyval.node = 0;
                   Clear(scanner_ccode); 
               ;
    break;}
case 105:
#line 2195 "parser.y"
{
		 yyval.node = new_node("cdecl");
		 if (yyvsp[-1].dtype.qualifier) SwigType_push(yyvsp[-2].decl.type,yyvsp[-1].dtype.qualifier);
		 Setattr(yyval.node,"name",yyvsp[-2].decl.id);
		 Setattr(yyval.node,"decl",yyvsp[-2].decl.type);
		 Setattr(yyval.node,"parms",yyvsp[-2].decl.parms);
		 Setattr(yyval.node,"value",yyvsp[-1].dtype.val);
		 Setattr(yyval.node,"throws",yyvsp[-1].dtype.throws);
		 if (yyvsp[-1].dtype.bitfield) {
		   Setattr(yyval.node,"bitfield", yyvsp[-1].dtype.bitfield);
		 }
		 if (!yyvsp[0].node) {
		   if (Len(scanner_ccode)) {
		     Setattr(yyval.node,"code",Copy(scanner_ccode));
		   }
		 } else {
		   set_nextSibling(yyval.node,yyvsp[0].node);
		 }
	       ;
    break;}
case 106:
#line 2214 "parser.y"
{ 
                   skip_balanced('{','}');
                   yyval.node = 0;
               ;
    break;}
case 107:
#line 2220 "parser.y"
{ 
                   yyval.dtype = yyvsp[0].dtype; 
                   yyval.dtype.qualifier = 0;
		   yyval.dtype.throws = 0;
              ;
    break;}
case 108:
#line 2225 "parser.y"
{ 
                   yyval.dtype = yyvsp[0].dtype; 
		   yyval.dtype.qualifier = yyvsp[-1].str;
		   yyval.dtype.throws = 0;
	      ;
    break;}
case 109:
#line 2230 "parser.y"
{ 
		   yyval.dtype = yyvsp[0].dtype; 
                   yyval.dtype.qualifier = 0;
		   yyval.dtype.throws = yyvsp[-2].pl;
              ;
    break;}
case 110:
#line 2235 "parser.y"
{ 
                   yyval.dtype = yyvsp[0].dtype; 
                   yyval.dtype.qualifier = yyvsp[-5].str;
		   yyval.dtype.throws = yyvsp[-2].pl;
              ;
    break;}
case 111:
#line 2247 "parser.y"
{
                  yyval.node = new_node("enum");
		  Setattr(yyval.node,"name",yyvsp[-4].id);
		  appendChild(yyval.node,yyvsp[-2].node);
		  add_symbols(yyval.node);           /* Add to tag space */
		  add_symbols(yyvsp[-2].node);       /* Add enum values to id space */
	       ;
    break;}
case 112:
#line 2255 "parser.y"
{
		 Node *n;
		 SwigType *ty = 0;
		 String   *unnamed = 0;

		 yyval.node = new_node("enum");
		 if (yyvsp[-5].id) {
		   Setattr(yyval.node,"name",yyvsp[-5].id);
		   ty = NewStringf("enum %s", yyvsp[-5].id);
		 } else if (yyvsp[-1].decl.id){
		   unnamed = make_unnamed();
		   ty = NewStringf("enum %s", unnamed);
		   Setattr(yyval.node,"unnamed",unnamed);
		   /* WF 20/12/2001: Cannot get sym:name and symtab set without setting name - fix!
		      // I don't think sym:name should be set. */
		   Setattr(yyval.node,"name",yyvsp[-1].decl.id);
		   Setattr(yyval.node,"tdname",yyvsp[-1].decl.id);
		   Setattr(yyval.node,"storage",yyvsp[-7].id);
		 }
		 appendChild(yyval.node,yyvsp[-3].node);
		 n = new_node("cdecl");
		 Setattr(n,"type",ty);
		 Setattr(n,"name",yyvsp[-1].decl.id);
		 Setattr(n,"storage",yyvsp[-7].id);
		 Setattr(n,"decl",yyvsp[-1].decl.type);
		 Setattr(n,"parms",yyvsp[-1].decl.parms);
		 Setattr(n,"unnamed",unnamed);
		 if (yyvsp[0].node) {
		   Node *p = yyvsp[0].node;
		   set_nextSibling(n,p);
		   while (p) {
		     Setattr(p,"type",Copy(ty));
		     Setattr(p,"unnamed",unnamed);
		     Setattr(p,"storage",yyvsp[-7].id);
		     p = nextSibling(p);
		   }
		 } else {
		   if (Len(scanner_ccode)) {
		     Setattr(n,"code",Copy(scanner_ccode));
		   }
		 }
		 add_symbols(yyval.node);        /* Add enum to tag space */
		 set_nextSibling(yyval.node,n);
		 add_symbols(yyvsp[-3].node);       /* Add to id space */
	         add_symbols(n);
	       ;
    break;}
case 113:
#line 2303 "parser.y"
{
                   /* This is a sick hack.  If the ctor_end has parameters,
                      and the parms paremeter only has 1 parameter, this
                      could be a declaration of the form:

                         type (id)(parms)

			 Otherwise it's an error. */
                    int err = 0;
                    yyval.node = 0;

		    if ((ParmList_len(yyvsp[-2].pl) == 1) && (!Swig_scopename_check(yyvsp[-4].type))) {
		      SwigType *ty = Getattr(yyvsp[-2].pl,"type");
		      String *name = Getattr(yyvsp[-2].pl,"name");
		      err = 1;
		      if (!name) {
			yyval.node = new_node("cdecl");
			Setattr(yyval.node,"type",yyvsp[-4].type);
			Setattr(yyval.node,"storage",yyvsp[-5].id);
			Setattr(yyval.node,"name",ty);

			if (yyvsp[0].decl.have_parms) {
			  SwigType *decl = NewString("");
			  SwigType_add_function(decl,yyvsp[0].decl.parms);
			  Setattr(yyval.node,"decl",decl);
			  Setattr(yyval.node,"parms",yyvsp[0].decl.parms);
			  if (Len(scanner_ccode)) {
			    Setattr(yyval.node,"code",Copy(scanner_ccode));
			  }
			}
			if (yyvsp[0].decl.defarg) {
			  Setattr(yyval.node,"value",yyvsp[0].decl.defarg);
			}
			Setattr(yyval.node,"throws",yyvsp[0].decl.throws);
			err = 0;
		      }
		    }
		    if (err) {
		      Swig_error(cparse_file,cparse_line,"Syntax error in input.\n");
		    }
                ;
    break;}
case 114:
#line 2350 "parser.y"
{  yyval.node = yyvsp[0].node; ;
    break;}
case 115:
#line 2351 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 116:
#line 2352 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 117:
#line 2353 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 118:
#line 2354 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 119:
#line 2355 "parser.y"
{ yyval.node = 0; ;
    break;}
case 120:
#line 2361 "parser.y"
{
                   List *bases = 0;
                   class_rename = make_name(yyvsp[-2].str,0);
		   Classprefix = NewString(yyvsp[-2].str);
		   /* Deal with inheritance  */
		   if (yyvsp[-1].bases) {
		     bases = make_inherit_list(yyvsp[-2].str,yyvsp[-1].bases);
		   }
		   if (SwigType_istemplate(yyvsp[-2].str)) {
		     String *fbase, *tbase, *prefix;
		     prefix = SwigType_templateprefix(yyvsp[-2].str);
		     if (Namespaceprefix) {
		       fbase = NewStringf("%s::%s", Namespaceprefix,yyvsp[-2].str);
		       tbase = NewStringf("%s::%s", Namespaceprefix, prefix);
		     } else {
		       fbase = Copy(yyvsp[-2].str);
		       tbase = Copy(prefix);
		     }
		     rename_inherit(tbase,fbase);
		     Delete(fbase);
		     Delete(tbase);
		     Delete(prefix);
		   }
                   if (strcmp(yyvsp[-3].id,"class") == 0) {
		     cplus_mode = CPLUS_PRIVATE;
		   } else {
		     cplus_mode = CPLUS_PUBLIC;
		   }
		   Swig_symbol_newscope();
		   Swig_symbol_setscopename(yyvsp[-2].str);
		   if (bases) {
		     Iterator s;
		     for (s = First(bases); s.item; s = Next(s)) {
		       Symtab *st = Getattr(s.item,"symtab");
		       if (st) {
			 Swig_symbol_inherit(st); 
		       }
		     }
		   }
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		   cparse_start_line = cparse_line;

		   /* If there are active template parameters, we need to make sure they are
                      placed in the class symbol table so we can catch shadows */

		   if (template_parameters) {
		     Parm *tp = template_parameters;
		     while(tp) {
		       Node *tn = new_node("templateparm");
		       Setattr(tn,"name",Getattr(tp,"name"));
		       Swig_symbol_cadd(Copy(Getattr(tp,"name")),tn);
		       tp = nextSibling(tp);
		     }
		   }
		   inclass = 1;
               ;
    break;}
case 121:
#line 2416 "parser.y"
{
		 Node *p;
		 SwigType *ty;
		 inclass = 0;
		 yyval.node = new_node("class");
		 Setline(yyval.node,cparse_start_line);
		 Setattr(yyval.node,"name",yyvsp[-6].str);
		 Setattr(yyval.node,"kind",yyvsp[-7].id);
		 Setattr(yyval.node,"baselist",yyvsp[-5].bases);
		 Setattr(yyval.node,"allows_typedef","1");
		 /* Check for pure-abstract class */
		 Setattr(yyval.node,"abstract", pure_abstract(yyvsp[-2].node));
		 
		 /* This bit of code merges in a previously defined %extend directive (if any) */
		 if (extendhash) {
		   String *clsname = Swig_symbol_qualifiedscopename(0);
		   Node *am = Getattr(extendhash,clsname);
		   if (am) {
		     merge_extensions(yyval.node,am);
		     appendChild(yyval.node,am);
		     Delattr(extendhash,clsname);
		   }
		   Delete(clsname);
		 }
		 if (!classes) classes = NewHash();
		 Setattr(classes,Swig_symbol_qualifiedscopename(0),yyval.node);

		 appendChild(yyval.node,yyvsp[-2].node);
		 p = yyvsp[0].node;
		 if (p) {
		   set_nextSibling(yyval.node,p);
		 }
		 
		 if (cparse_cplusplus) {
		   ty = NewString(yyvsp[-6].str);
		 } else {
		   ty = NewStringf("%s %s", yyvsp[-7].id,yyvsp[-6].str);
		 }
		 while (p) {
		   Setattr(p,"storage",yyvsp[-8].id);
		   Setattr(p,"type",ty);
		   p = nextSibling(p);
		 }
		 /* Dump nested classes */
		 {
		   String *name = yyvsp[-6].str;
		   if (yyvsp[0].node) {
		     SwigType *decltype = Getattr(yyvsp[0].node,"decl");
		     if (Cmp(yyvsp[-8].id,"typedef") == 0) {
		       if (!decltype || !Len(decltype)) {
			 name = Getattr(yyvsp[0].node,"name");
			 Setattr(yyval.node,"tdname",Copy(name));

			 /* Use typedef name as class name */
			 if (class_rename && (Strcmp(class_rename,yyvsp[-6].str) == 0)) {
			   class_rename = NewString(name);
			 }
			 if (!Getattr(classes,name)) {
			   Setattr(classes,name,yyval.node);
			 }
			 Setattr(yyval.node,"decl",decltype);
		       }
		     }
		   }
		   appendChild(yyval.node,dump_nested(Char(name)));
		 }
		 Setattr(yyval.node,"symtab",Swig_symbol_popscope());

		 yyrename = NewString(class_rename);
		 Classprefix = 0;
		 Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		 add_symbols(yyval.node);
		 if (yyvsp[0].node)
		   add_symbols(yyvsp[0].node);

	       ;
    break;}
case 122:
#line 2495 "parser.y"
{
	       class_rename = make_name(0,0);
	       if (strcmp(yyvsp[-1].id,"class") == 0) {
		 cplus_mode = CPLUS_PRIVATE;
	       } else {
		 cplus_mode = CPLUS_PUBLIC;
	       }
	       Swig_symbol_newscope();
	       cparse_start_line = cparse_line;
	       inclass = 1;
	       Classprefix = NewString("");
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
             ;
    break;}
case 123:
#line 2507 "parser.y"
{
	       String *unnamed;
	       Node *n, *p, *pp = 0;
	       Classprefix = 0;
	       inclass = 0;
	       unnamed = make_unnamed();
	       yyval.node = new_node("class");
	       Setline(yyval.node,cparse_start_line);
	       Setattr(yyval.node,"kind",yyvsp[-6].id);
	       Setattr(yyval.node,"storage",yyvsp[-7].id);
	       Setattr(yyval.node,"unnamed",unnamed);
	       Setattr(yyval.node,"allows_typedef","1");

	       /* Check for pure-abstract class */
	       Setattr(yyval.node,"abstract", pure_abstract(yyvsp[-3].node));

	       n = new_node("cdecl");
	       Setattr(n,"name",yyvsp[-1].decl.id);
	       Setattr(n,"unnamed",unnamed);
	       Setattr(n,"type",unnamed);
	       Setattr(n,"decl",yyvsp[-1].decl.type);
	       Setattr(n,"parms",yyvsp[-1].decl.parms);
	       Setattr(n,"storage",yyvsp[-7].id);
	       pp = n;
	       if (yyvsp[0].node) {
		 set_nextSibling(n,yyvsp[0].node);
		 p = yyvsp[0].node;
		 while (p) {
		   pp = p;
		   Setattr(p,"unnamed",unnamed);
		   Setattr(p,"type",Copy(unnamed));
		   Setattr(p,"storage",yyvsp[-7].id);
		   p = nextSibling(p);
		 }
	       }
	       set_nextSibling(yyval.node,n);
	       {
		 /* If a proper typedef name was given, we'll use it to set the scope name */
		 String *name = 0;
		 if (yyvsp[-7].id && (strcmp(yyvsp[-7].id,"typedef") == 0)) {
		   if (!Len(yyvsp[-1].decl.type)) {	
		     name = yyvsp[-1].decl.id;
		     Setattr(yyval.node,"tdname",name);
		     Setattr(yyval.node,"name",name);
		     /* if (!class_rename) class_rename = NewString(name); */
		     Swig_symbol_setscopename(name);

		     /* If a proper name given, we use that as the typedef, not unnamed */
		     Clear(unnamed);
		     Append(unnamed, name);
		     
		     n = nextSibling(n);
		     set_nextSibling(yyval.node,n);

		     /* Check for previous extensions */
		     if (extendhash) {
		       String *clsname = Swig_symbol_qualifiedscopename(0);
		       Node *am = Getattr(extendhash,clsname);
		       if (am) {
			 /* Merge the extension into the symbol table */
			 merge_extensions(yyval.node,am);
			 appendChild(yyval.node,am);
			 Delattr(extendhash,clsname);
		       }
		       Delete(clsname);
		     }
		     if (!classes) classes = NewHash();
		     Setattr(classes,Swig_symbol_qualifiedscopename(0),yyval.node);
		   } else {
		     Swig_symbol_setscopename((char*)"<unnamed>");
		   }
		 }
		 appendChild(yyval.node,yyvsp[-3].node);
		 appendChild(yyval.node,dump_nested(Char(name)));
	       }
	       /* Pop the scope */
	       Setattr(yyval.node,"symtab",Swig_symbol_popscope());
	       if (class_rename) {
		 yyrename = NewString(class_rename);
	       }
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	       add_symbols(yyval.node);
	       add_symbols(n);
              ;
    break;}
case 124:
#line 2593 "parser.y"
{ yyval.node = 0; ;
    break;}
case 125:
#line 2594 "parser.y"
{
                        yyval.node = new_node("cdecl");
                        Setattr(yyval.node,"name",yyvsp[-1].decl.id);
                        Setattr(yyval.node,"decl",yyvsp[-1].decl.type);
                        Setattr(yyval.node,"parms",yyvsp[-1].decl.parms);
			set_nextSibling(yyval.node,yyvsp[0].node);
                    ;
    break;}
case 126:
#line 2606 "parser.y"
{
              if (yyvsp[-3].id && (Strcmp(yyvsp[-3].id,"friend") == 0)) {
		/* Ignore */
                yyval.node = 0; 
	      } else {
		yyval.node = new_node("classforward");
		Setattr(yyval.node,"kind",yyvsp[-2].id);
		Setattr(yyval.node,"name",yyvsp[-1].str);
		Setattr(yyval.node,"sym:weak", "1");
		add_symbols(yyval.node);
	      }
             ;
    break;}
case 127:
#line 2624 "parser.y"
{ template_parameters = yyvsp[-1].tparms; ;
    break;}
case 128:
#line 2624 "parser.y"
{
		      String *tname = 0;
		      int     error = 0;

                      template_parameters = 0;
                      yyval.node = yyvsp[0].node;
		      if (yyval.node) tname = Getattr(yyval.node,"name");
		      
		      /* Check if the class is a template specialization */
		      if ((yyval.node) && (Strstr(tname,"<")) && (Strncmp(tname,"operator ",9) != 0)) {
			/* If a specialization.  Check if defined. */
			Node *tempn = 0;
			{
			  String *tbase = SwigType_templateprefix(tname);
			  tempn = Swig_symbol_clookup_local(tbase,0);
			  if (!tempn || (Strcmp(nodeType(tempn),"template") != 0)) {
			    Swig_warning(WARN_PARSE_TEMPLATE_SP_UNDEF, Getfile(yyval.node),Getline(yyval.node),"Specialization of non-template '%s'.\n", tbase);
			    tempn = 0;
			    error = 1;
			  }
			  Delete(tbase);
			}
			Setattr(yyval.node,"specialization","1");
			Setattr(yyval.node,"templatetype",nodeType(yyval.node));
			set_nodeType(yyval.node,"template");
			/* Template partial specialization */
			if (tempn && (yyvsp[-3].tparms) && (yyvsp[0].node)) {
			  List   *tlist;
			  String *targs = SwigType_templateargs(tname);
			  tlist = SwigType_parmlist(targs);
			  /*			  Printf(stdout,"targs = '%s' %s\n", targs, tlist); */
			  if (!Getattr(yyval.node,"sym:weak")) {
			    Setattr(yyval.node,"sym:typename","1");
			  }
			  
			  if (Len(tlist) != ParmList_len(Getattr(tempn,"templateparms"))) {
			    Swig_error(Getfile(yyval.node),Getline(yyval.node),"Inconsistent argument count in template partial specialization. %d %d\n", Len(tlist), ParmList_len(Getattr(tempn,"templateparms")));
			    
			  } else {

			  /* This code builds the argument list for the partial template
                             specialization.  This is a little hairy, but the idea is as
                             follows:

                             $3 contains a list of arguments supplied for the template.
                             For example template<class T>.

                             tlist is a list of the specialization arguments--which may be
                             different.  For example class<int,T>.

                             tp is a copy of the arguments in the original template definition.
     
                             The patching algorithm walks through the list of supplied
                             arguments ($3), finds the position in the specialization arguments
                             (tlist), and then patches the name in the argument list of the
                             original template.
			  */

			  {
			    String *pn;
			    Parm *p, *p1;
			    int i, nargs;
			    Parm *tp = CopyParmList(Getattr(tempn,"templateparms"));
			    nargs = Len(tlist);
			    p = yyvsp[-3].tparms;
			    while (p) {
			      for (i = 0; i < nargs; i++){
				pn = Getattr(p,"name");
				if (Strcmp(pn,SwigType_base(Getitem(tlist,i))) == 0) {
				  int j;
				  Parm *p1 = tp;
				  for (j = 0; j < i; j++) {
				    p1 = nextSibling(p1);
				  }
				  Setattr(p1,"name",pn);
				  Setattr(p1,"partialarg","1");
				}
			      }
			      p = nextSibling(p);
			    }
			    p1 = tp;
			    i = 0;
			    while (p1) {
			      if (!Getattr(p1,"partialarg")) {
				Delattr(p1,"name");
				Setattr(p1,"type", Getitem(tlist,i));
			      } 
			      i++;
			      p1 = nextSibling(p1);
			    }
			    Setattr(yyval.node,"templateparms",tp);
			  }
#if 0
			  /* Patch the parameter list */
			  if (tempn) {
			    Parm *p,*p1;
			    ParmList *tp = CopyParmList(Getattr(tempn,"templateparms"));
			    p = yyvsp[-3].tparms;
			    p1 = tp;
			    while (p && p1) {
			      String *pn = Getattr(p,"name");
			      Printf(stdout,"pn = '%s'\n", pn);
			      if (pn) Setattr(p1,"name",pn);
			      else Delattr(p1,"name");
			      pn = Getattr(p,"type");
			      if (pn) Setattr(p1,"type",pn);
			      p = nextSibling(p);
			      p1 = nextSibling(p1);
			    }
			    Setattr(yyval.node,"templateparms",tp);
			  } else {
			    Setattr(yyval.node,"templateparms",yyvsp[-3].tparms);
			  }
#endif
			  Delattr(yyval.node,"specialization");
			  Setattr(yyval.node,"partialspecialization","1");
			  /* Create a specialized name for matching */
			  {
			    Parm *p = yyvsp[-3].tparms;
			    String *fname = NewString(Getattr(yyval.node,"name"));
			    String *ffname = 0;

			    char   tmp[32];
			    int    i;
			    while (p) {
			      String *n = Getattr(p,"name");
			      if (!n) {
				p = nextSibling(p);
				continue;
			      }
			      for (i = 0; i < Len(tlist); i++) {
				if (Strstr(Getitem(tlist,i),n)) {
				  sprintf(tmp,"$%d",i+1);
				  Replaceid(fname,n,tmp);
				}
			      }
			      p = nextSibling(p);
			    }
			    /* Patch argument names with typedef */
			    {
			      Iterator tt;
			      List *tparms = SwigType_parmlist(fname);
			      ffname = SwigType_templateprefix(fname);
			      Append(ffname,"<(");
			      for (tt = First(tparms); tt.item; ) {
				SwigType *ttr = Swig_symbol_typedef_reduce(tt.item,0);
				ttr = Swig_symbol_type_qualify(ttr,0);
				Append(ffname,ttr);
				tt = Next(tt);
				if (tt.item) Putc(',',ffname);
			      }
			      Append(ffname,")>");
			    }
			    {
			      String *partials = Getattr(tempn,"partials");
			      if (!partials) {
				partials = NewList();
				Setattr(tempn,"partials",partials);
			      }
			      /*			      Printf(stdout,"partial: fname = '%s', '%s'\n", fname, Swig_symbol_typedef_reduce(fname,0)); */
			      Append(partials,ffname);
			    }
			    Setattr(yyval.node,"partialargs",ffname);
			    Swig_symbol_cadd(ffname,yyval.node);
			  }
			  }
			  Delete(tlist);
			  Delete(targs);
			} else {
			  /* Need to resolve exact specialization name */
			  /* This needs to be rewritten */
			  List *tparms;
			  String *fname;
			  Iterator tt;
			  fname = SwigType_templateprefix(tname);
			  tparms = SwigType_parmlist(tname);
			  Append(fname,"<(");
			  for (tt = First(tparms); tt.item; ) {
			    SwigType *ttr = Swig_symbol_typedef_reduce(tt.item,0);
			    ttr = Swig_symbol_type_qualify(ttr,0);
			    Append(fname,ttr);
			    tt = Next(tt);
			    if (tt.item) Putc(',',fname);
			  }
			  Append(fname,")>");
			  Swig_symbol_cadd(fname,yyval.node);
			}
		      }  else if (yyval.node) {
			Setattr(yyval.node,"templatetype",nodeType(yyvsp[0].node));
			set_nodeType(yyval.node,"template");
			Setattr(yyval.node,"templateparms", yyvsp[-3].tparms);
			if (!Getattr(yyval.node,"sym:weak")) {
			  Setattr(yyval.node,"sym:typename","1");
			}
			add_symbols(yyval.node);
			/* We also place a fully parameterized version in the symbol table */
			{
			  Parm *p;
			  String *fname = NewStringf("%s<(",Getattr(yyval.node,"name"));
			  p = yyvsp[-3].tparms;
			  while (p) {
			    String *n = Getattr(p,"name");
			    if (!n) n = Getattr(p,"type");
			    Printf(fname,"%s", n);
			    p = nextSibling(p);
			    if (p) Putc(',',fname);
			  }
			  Printf(fname,")>");
			  Swig_symbol_cadd(fname,yyval.node);
			}
		      }
		      if (error) yyval.node = 0;
                  ;
    break;}
case 129:
#line 2837 "parser.y"
{
		  Swig_warning(WARN_PARSE_EXPLICIT_TEMPLATE, cparse_file, cparse_line, "Explicit template instantiation ignored.\n");
                   yyval.node = 0; 
                ;
    break;}
case 130:
#line 2843 "parser.y"
{
		  yyval.node = yyvsp[0].node;
                ;
    break;}
case 131:
#line 2846 "parser.y"
{
                   yyval.node = yyvsp[0].node;
                ;
    break;}
case 132:
#line 2849 "parser.y"
{
                   yyval.node = yyvsp[0].node;
                ;
    break;}
case 133:
#line 2852 "parser.y"
{
		  yyval.node = 0;
                ;
    break;}
case 134:
#line 2855 "parser.y"
{
                  yyval.node = yyvsp[0].node;
                ;
    break;}
case 135:
#line 2860 "parser.y"
{
		   /* Rip out the parameter names */
		  Parm *p = yyvsp[0].pl;
		  yyval.tparms = yyvsp[0].pl;

		  while (p) {
		    String *name = Getattr(p,"name");
		    if (!name) {
		      /* Hmmm. Maybe it's a 'class T' parameter */
		      char *type = Char(Getattr(p,"type"));
		      /* Template template parameter */
		      if (strncmp(type,"template<class> ",16) == 0) {
			type += 16;
		      }
		      if ((strncmp(type,"class ",6) == 0) || (strncmp(type,"typename ", 9) == 0)) {
			char *t = strchr(type,' ');
			Setattr(p,"name", t+1);
		      } else {
			/*
			 Swig_error(cparse_file, cparse_line, "Missing template parameter name\n");
			 $$.rparms = 0;
			 $$.parms = 0;
			 break; */
		      }
		    }
		    p = nextSibling(p);
		  }
                 ;
    break;}
case 136:
#line 2892 "parser.y"
{
                  yyval.node = new_node("using");
		  Setattr(yyval.node,"uname",yyvsp[-1].str);
		  Setattr(yyval.node,"name", Swig_scopename_last(yyvsp[-1].str));
		  add_symbols(yyval.node);
             ;
    break;}
case 137:
#line 2898 "parser.y"
{
	       Node *n = Swig_symbol_clookup(yyvsp[-1].str,0);
	       if (!n) {
		 Swig_error(cparse_file, cparse_line, "Nothing known about namespace '%s'\n", yyvsp[-1].str);
		 yyval.node = 0;
	       } else {

		 while (Strcmp(nodeType(n),"using") == 0) {
		   n = Getattr(n,"node");
		 }
		 if (n) {
		   if (Strcmp(nodeType(n),"namespace") == 0) {
		     yyval.node = new_node("using");
		     Setattr(yyval.node,"node",n);
		     Setattr(yyval.node,"namespace", yyvsp[-1].str);
		     Swig_symbol_inherit(Getattr(n,"symtab"));
		   } else {
		     Swig_error(cparse_file, cparse_line, "'%s' is not a namespace.\n", yyvsp[-1].str);
		     yyval.node = 0;
		   }
		 } else {
		   yyval.node = 0;
		 }
	       }
             ;
    break;}
case 138:
#line 2925 "parser.y"
{ 
                Hash *h;
                yyvsp[-2].node = Swig_symbol_current();
		h = Swig_symbol_clookup(yyvsp[-1].str,0);
		if (h && (Strcmp(nodeType(h),"namespace") == 0)) {
		  if (Getattr(h,"alias")) {
		    h = Getattr(h,"namespace");
		    Swig_warning(WARN_PARSE_NAMESPACE_ALIAS, cparse_file, cparse_line, "Namespace alias '%s' not allowed here. Assuming '%s'\n",
				 yyvsp[-1].str, Getattr(h,"name"));
		    yyvsp[-1].str = Getattr(h,"name");
		  }
		  Swig_symbol_setscope(Getattr(h,"symtab"));
		} else {
		  Swig_symbol_newscope();
		  Swig_symbol_setscopename(yyvsp[-1].str);
		}
		Namespaceprefix = Swig_symbol_qualifiedscopename(0);
             ;
    break;}
case 139:
#line 2942 "parser.y"
{
                Node *n = yyvsp[-1].node;
		set_nodeType(n,"namespace");
		Setattr(n,"name",yyvsp[-4].str);
                Setattr(n,"symtab", Swig_symbol_popscope());
		Swig_symbol_setscope(yyvsp[-5].node);
		yyval.node = n;
		Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		add_symbols(yyval.node);
             ;
    break;}
case 140:
#line 2952 "parser.y"
{
	       Hash *h;
	       yyvsp[-1].node = Swig_symbol_current();
	       h = Swig_symbol_clookup("",0);
	       if (h && (Strcmp(nodeType(h),"namespace") == 0)) {
		 Swig_symbol_setscope(Getattr(h,"symtab"));
	       } else {
		 Swig_symbol_newscope();
		 Swig_symbol_setscopename("__unnamed__");
	       }
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
             ;
    break;}
case 141:
#line 2963 "parser.y"
{
	       yyval.node = yyvsp[-1].node;
	       set_nodeType(yyval.node,"namespace");
	       Setattr(yyval.node,"unnamed","1");
	       Setattr(yyval.node,"symtab", Swig_symbol_popscope());
	       Swig_symbol_setscope(yyvsp[-4].node);
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	       add_symbols(yyval.node);
             ;
    break;}
case 142:
#line 2972 "parser.y"
{
	       /* Namespace alias */
	       Node *n;
	       yyval.node = new_node("namespace");
	       Setattr(yyval.node,"name",yyvsp[-3].id);
	       Setattr(yyval.node,"alias",yyvsp[-1].str);
	       n = Swig_symbol_clookup(yyvsp[-1].str,0);
	       if (!n) {
		 Swig_error(cparse_file, cparse_line, "Unknown namespace '%s'\n", yyvsp[-1].str);
		 yyval.node = 0;
	       } else {
		 if (Strcmp(nodeType(n),"namespace") != 0) {
		   Swig_error(cparse_file, cparse_line, "'%s' is not a namespace\n",yyvsp[-1].str);
		   yyval.node = 0;
		 } else {
		   while (Getattr(n,"alias")) {
		     n = Getattr(n,"namespace");
		   }
		   Setattr(yyval.node,"namespace",n);
		   add_symbols(yyval.node);
		   /* Set up a scope alias */
		   Swig_symbol_alias(yyvsp[-3].id,Getattr(n,"symtab"));
		 }
	       }
             ;
    break;}
case 143:
#line 2999 "parser.y"
{
                   yyval.node = yyvsp[-1].node;
		   if (yyval.node) {
		     Node *p = yyval.node;
		     Node *pp =0;
		     while (p) {
		       pp = p;
		       p = nextSibling(p);
		     }
		     set_nextSibling(pp,yyvsp[0].node);
		   } else {
		     yyval.node = yyvsp[0].node;
		   }
             ;
    break;}
case 144:
#line 3013 "parser.y"
{ 
                  if (cplus_mode != CPLUS_PUBLIC) {
		     Swig_error(cparse_file,cparse_line,"%%extend can only be used in a public section\n");
		  }
             ;
    break;}
case 145:
#line 3017 "parser.y"
{
	       yyval.node = new_node("extend");
	       Swig_tag_nodes(yyvsp[-2].node,"feature:extend",(char*) "1");
	       appendChild(yyval.node,yyvsp[-2].node);
	       set_nextSibling(yyval.node,yyvsp[0].node);
	     ;
    break;}
case 146:
#line 3023 "parser.y"
{ yyval.node = 0;;
    break;}
case 147:
#line 3024 "parser.y"
{
	       skip_decl();
		   {
		     static int last_error_line = -1;
		     if (last_error_line != cparse_line) {
		       Swig_error(cparse_file, cparse_line,"Syntax error in input.\n");
		       last_error_line = cparse_line;
		     }
		   }
	     ;
    break;}
case 148:
#line 3033 "parser.y"
{ 
                yyval.node = yyvsp[0].node;
             ;
    break;}
case 149:
#line 3044 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 150:
#line 3045 "parser.y"
{ 
                 yyval.node = yyvsp[0].node; 
		 if (extendmode) {
		   String *symname;
		   symname= make_name(Getattr(yyval.node,"name"), Getattr(yyval.node,"decl"));
		   if (Strcmp(symname,Getattr(yyval.node,"name")) == 0) {
		     /* No renaming operation.  Set name to class name */
		     yyrename = NewString(Getattr(current_class,"sym:name"));
		   } else {
		     yyrename = symname;
		   }
		 }
		 add_symbols(yyval.node);
             ;
    break;}
case 151:
#line 3059 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 152:
#line 3060 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 153:
#line 3061 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 154:
#line 3062 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 155:
#line 3063 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 156:
#line 3064 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 157:
#line 3065 "parser.y"
{ yyval.node = 0; ;
    break;}
case 158:
#line 3066 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 159:
#line 3067 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 160:
#line 3068 "parser.y"
{ yyval.node = 0; ;
    break;}
case 161:
#line 3069 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 162:
#line 3070 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 163:
#line 3071 "parser.y"
{ yyval.node = 0; ;
    break;}
case 164:
#line 3072 "parser.y"
{ yyval.node = 0; ;
    break;}
case 165:
#line 3081 "parser.y"
{
              if (Classprefix) {
		 SwigType *decl = NewString("");
		 yyval.node = new_node("constructor");
		 Setattr(yyval.node,"name",yyvsp[-4].type);
		 Setattr(yyval.node,"parms",yyvsp[-2].pl);
		 SwigType_add_function(decl,yyvsp[-2].pl);
		 Setattr(yyval.node,"decl",decl);
		 Setattr(yyval.node,"throws",yyvsp[0].decl.throws);
		 if (Len(scanner_ccode)) {
		   Setattr(yyval.node,"code",Copy(scanner_ccode));
		 }
		 Setattr(yyval.node,"feature:new","1");
	      } else {
		yyval.node = 0;
              }
              ;
    break;}
case 166:
#line 3102 "parser.y"
{
               yyval.node = new_node("destructor");
	       Setattr(yyval.node,"name",NewStringf("~%s",yyvsp[-4].str));
	       if (Len(scanner_ccode)) {
		 Setattr(yyval.node,"code",Copy(scanner_ccode));
	       }
	       {
		 String *decl = NewString("");
		 SwigType_add_function(decl,yyvsp[-2].pl);
		 Setattr(yyval.node,"decl",decl);
	       }
	       add_symbols(yyval.node);
	      ;
    break;}
case 167:
#line 3118 "parser.y"
{
		yyval.node = new_node("destructor");
	       /* Check for template names.  If the class is a template
		  and the constructor is missing the template part, we
		  add it */
	       {
		 char *c = Strstr(Classprefix,"<");
		 if (c) {
		   if (!Strstr(yyvsp[-4].str,"<")) {
		     yyvsp[-4].str = NewStringf("%s%s",yyvsp[-4].str,c);
		   }
		 }
	       }
		Setattr(yyval.node,"storage","virtual");
		Setattr(yyval.node,"name",NewStringf("~%s",yyvsp[-4].str));
		if (yyvsp[0].dtype.val) {
		  Setattr(yyval.node,"value","0");
		}
		if (Len(scanner_ccode)) {
		  Setattr(yyval.node,"code",Copy(scanner_ccode));
		}
		{
		  String *decl = NewString("");
		  SwigType_add_function(decl,yyvsp[-2].pl);
		  Setattr(yyval.node,"decl",decl);
		}

		add_symbols(yyval.node);
	      ;
    break;}
case 168:
#line 3151 "parser.y"
{
                 yyval.node = new_node("cdecl");
                 Setattr(yyval.node,"type",yyvsp[-5].type);
		 Setattr(yyval.node,"name",yyvsp[-6].str);

		 SwigType_add_function(yyvsp[-4].type,yyvsp[-2].pl);
		 if (yyvsp[0].dtype.qualifier) {
		   SwigType_push(yyvsp[-4].type,yyvsp[0].dtype.qualifier);
		 }
		 Setattr(yyval.node,"decl",yyvsp[-4].type);
		 Setattr(yyval.node,"parms",yyvsp[-2].pl);
		 Setattr(yyval.node,"conversion_operator","1");
		 add_symbols(yyval.node);
              ;
    break;}
case 169:
#line 3165 "parser.y"
{
		 SwigType *decl;
                 yyval.node = new_node("cdecl");
                 Setattr(yyval.node,"type",yyvsp[-5].type);
		 Setattr(yyval.node,"name",yyvsp[-6].str);
		 decl = NewString("");
		 SwigType_add_reference(decl);
		 SwigType_add_function(decl,yyvsp[-2].pl);
		 if (yyvsp[0].dtype.qualifier) {
		   SwigType_push(decl,yyvsp[0].dtype.qualifier);
		 }
		 Setattr(yyval.node,"decl",decl);
		 Setattr(yyval.node,"parms",yyvsp[-2].pl);
		 Setattr(yyval.node,"conversion_operator","1");
		 add_symbols(yyval.node);
	       ;
    break;}
case 170:
#line 3182 "parser.y"
{
		String *t = NewString("");
		yyval.node = new_node("cdecl");
		Setattr(yyval.node,"type",yyvsp[-4].type);
		Setattr(yyval.node,"name",yyvsp[-5].str);
		SwigType_add_function(t,yyvsp[-2].pl);
		if (yyvsp[0].dtype.qualifier) {
		  SwigType_push(t,yyvsp[0].dtype.qualifier);
		}
		Setattr(yyval.node,"decl",t);
		Setattr(yyval.node,"parms",yyvsp[-2].pl);
		Setattr(yyval.node,"conversion_operator","1");
		add_symbols(yyval.node);
              ;
    break;}
case 171:
#line 3200 "parser.y"
{
                 skip_balanced('{','}');
                 yyval.node = 0;
               ;
    break;}
case 172:
#line 3207 "parser.y"
{ 
                yyval.node = new_node("access");
		Setattr(yyval.node,"kind","public");
                cplus_mode = CPLUS_PUBLIC;
              ;
    break;}
case 173:
#line 3214 "parser.y"
{ 
                yyval.node = new_node("access");
                Setattr(yyval.node,"kind","private");
		cplus_mode = CPLUS_PRIVATE;
	      ;
    break;}
case 174:
#line 3222 "parser.y"
{ 
		yyval.node = new_node("access");
		Setattr(yyval.node,"kind","protected");
		cplus_mode = CPLUS_PROTECTED;
	      ;
    break;}
case 175:
#line 3245 "parser.y"
{ cparse_start_line = cparse_line; skip_balanced('{','}');
	      ;
    break;}
case 176:
#line 3246 "parser.y"
{
	        yyval.node = 0;
		if (cplus_mode == CPLUS_PUBLIC) {
		  if (yyvsp[-1].decl.id) {
		    if (strcmp(yyvsp[-5].id,"class") == 0) {
		      Swig_warning(WARN_PARSE_NESTED_CLASS, cparse_file, cparse_line, "Nested classes not currently supported (ignored).\n");
		      /* Generate some code for a new class */
		    } else {
		      Nested *n = (Nested *) malloc(sizeof(Nested));
		      n->code = NewString("");
		      Printv(n->code, "typedef ", yyvsp[-5].id, " ",
			     Char(scanner_ccode), " $classname_", yyvsp[-1].decl.id, ";\n", NIL);

		      n->name = Swig_copy_string(yyvsp[-1].decl.id);
		      n->line = cparse_start_line;
		      n->type = NewString("");
		      n->kind = yyvsp[-5].id;
		      SwigType_push(n->type, yyvsp[-1].decl.type);
		      n->next = 0;
		      add_nested(n);
		    }
		  } else {
		    Swig_warning(WARN_PARSE_NESTED_CLASS, cparse_file, cparse_line, "Nested %s not currently supported (ignored).\n", yyvsp[-5].id);
		  }
		}
	      ;
    break;}
case 177:
#line 3274 "parser.y"
{ cparse_start_line = cparse_line; skip_balanced('{','}');
              ;
    break;}
case 178:
#line 3275 "parser.y"
{
	        yyval.node = 0;
		if (cplus_mode == CPLUS_PUBLIC) {
		  if (strcmp(yyvsp[-4].id,"class") == 0) {
		    Swig_warning(WARN_PARSE_NESTED_CLASS,cparse_file, cparse_line,"Nested class not currently supported (ignored)\n");
		    /* Generate some code for a new class */
		  } else if (yyvsp[-1].decl.id) {
		    /* Generate some code for a new class */
		    Nested *n = (Nested *) malloc(sizeof(Nested));
		    n->code = NewString("");
		    Printv(n->code, "typedef ", yyvsp[-4].id, " " ,
			    Char(scanner_ccode), " $classname_", yyvsp[-1].decl.id, ";\n",NIL);
		    n->name = Swig_copy_string(yyvsp[-1].decl.id);
		    n->line = cparse_start_line;
		    n->type = NewString("");
		    n->kind = yyvsp[-4].id;
		    SwigType_push(n->type,yyvsp[-1].decl.type);
		    n->next = 0;
		    add_nested(n);
		  } else {
		    Swig_warning(WARN_PARSE_NESTED_CLASS, cparse_file, cparse_line, "Nested %s not currently supported (ignored).\n", yyvsp[-4].id);
		  }
		}
	      ;
    break;}
case 179:
#line 3301 "parser.y"
{ yyval.decl = yyvsp[0].decl;;
    break;}
case 180:
#line 3302 "parser.y"
{ yyval.decl.id = 0; ;
    break;}
case 181:
#line 3308 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 182:
#line 3311 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 183:
#line 3315 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 184:
#line 3318 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 185:
#line 3319 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 186:
#line 3320 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 187:
#line 3321 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 188:
#line 3322 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 189:
#line 3323 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 190:
#line 3324 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 191:
#line 3325 "parser.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 192:
#line 3328 "parser.y"
{
	            Clear(scanner_ccode);
               ;
    break;}
case 193:
#line 3331 "parser.y"
{ skip_balanced('{','}'); ;
    break;}
case 194:
#line 3334 "parser.y"
{ 
                     Clear(scanner_ccode);
                     yyval.dtype.val = 0;
                     yyval.dtype.qualifier = yyvsp[-1].dtype.qualifier;
                     yyval.dtype.bitfield = 0;
                     yyval.dtype.throws = yyvsp[-1].dtype.throws;
                ;
    break;}
case 195:
#line 3341 "parser.y"
{ 
                     Clear(scanner_ccode);
                     yyval.dtype.val = yyvsp[-1].dtype.val;
                     yyval.dtype.qualifier = yyvsp[-3].dtype.qualifier;
                     yyval.dtype.bitfield = 0;
                     yyval.dtype.throws = yyvsp[-3].dtype.throws; 
               ;
    break;}
case 196:
#line 3348 "parser.y"
{ 
                     skip_balanced('{','}');
                     yyval.dtype.val = 0;
                     yyval.dtype.qualifier = yyvsp[-1].dtype.qualifier;
                     yyval.dtype.bitfield = 0;
                     yyval.dtype.throws = yyvsp[-1].dtype.throws; 
               ;
    break;}
case 197:
#line 3358 "parser.y"
{ ;
    break;}
case 198:
#line 3364 "parser.y"
{ yyval.id = "extern"; ;
    break;}
case 199:
#line 3365 "parser.y"
{ 
                   if (strcmp(yyvsp[0].id,"C") == 0) {
		     yyval.id = "externc";
		   } else {
		     Swig_warning(WARN_PARSE_UNDEFINED_EXTERN,cparse_file, cparse_line,"Unrecognized extern type \"%s\" (ignored).\n", yyvsp[0].id);
		     yyval.id = 0;
		   }
               ;
    break;}
case 200:
#line 3373 "parser.y"
{ yyval.id = "static"; ;
    break;}
case 201:
#line 3374 "parser.y"
{ yyval.id = "typedef"; ;
    break;}
case 202:
#line 3375 "parser.y"
{ yyval.id = "virtual"; ;
    break;}
case 203:
#line 3376 "parser.y"
{ yyval.id = "friend"; ;
    break;}
case 204:
#line 3377 "parser.y"
{ yyval.id = 0; ;
    break;}
case 205:
#line 3384 "parser.y"
{
                 Parm *p;
		 yyval.pl = yyvsp[0].pl;
		 p = yyvsp[0].pl;
                 while (p) {
		   Replace(Getattr(p,"type"),"typename ", "", DOH_REPLACE_ANY);
		   p = nextSibling(p);
                 }
               ;
    break;}
case 206:
#line 3395 "parser.y"
{
		  if (1) { 
		    set_nextSibling(yyvsp[-1].p,yyvsp[0].pl);
		    yyval.pl = yyvsp[-1].p;
		  } else {
		    yyval.pl = yyvsp[0].pl;
		  }
		;
    break;}
case 207:
#line 3403 "parser.y"
{ yyval.pl = 0; ;
    break;}
case 208:
#line 3406 "parser.y"
{
                 set_nextSibling(yyvsp[-1].p,yyvsp[0].pl);
		 yyval.pl = yyvsp[-1].p;
                ;
    break;}
case 209:
#line 3410 "parser.y"
{ yyval.pl = 0; ;
    break;}
case 210:
#line 3414 "parser.y"
{
                   SwigType_push(yyvsp[-1].type,yyvsp[0].decl.type);
		   yyval.p = NewParm(yyvsp[-1].type,yyvsp[0].decl.id);
		   Setfile(yyval.p,cparse_file);
		   Setline(yyval.p,cparse_line);
		   if (yyvsp[0].decl.defarg) {
		     Setattr(yyval.p,"value",yyvsp[0].decl.defarg);
		   }
		;
    break;}
case 211:
#line 3424 "parser.y"
{
                  yyval.p = NewParm(NewStringf("template<class> %s %s", yyvsp[-1].id,yyvsp[0].str), 0);
		  Setfile(yyval.p,cparse_file);
		  Setline(yyval.p,cparse_line);
                ;
    break;}
case 212:
#line 3429 "parser.y"
{
		  SwigType *t = NewString("v(...)");
		  yyval.p = NewParm(t, 0);
		  Setfile(yyval.p,cparse_file);
		  Setline(yyval.p,cparse_line);
		;
    break;}
case 213:
#line 3437 "parser.y"
{
                 Parm *p;
		 yyval.p = yyvsp[0].p;
		 p = yyvsp[0].p;
                 while (p) {
		   if (Getattr(p,"type")) {
		     Replace(Getattr(p,"type"),"typename ", "", DOH_REPLACE_ANY);
		   }
		   p = nextSibling(p);
                 }
               ;
    break;}
case 214:
#line 3450 "parser.y"
{
		  if (1) { 
		    set_nextSibling(yyvsp[-1].p,yyvsp[0].p);
		    yyval.p = yyvsp[-1].p;
		  } else {
		    yyval.p = yyvsp[0].p;
		  }
		;
    break;}
case 215:
#line 3458 "parser.y"
{ yyval.p = 0; ;
    break;}
case 216:
#line 3461 "parser.y"
{
                 set_nextSibling(yyvsp[-1].p,yyvsp[0].p);
		 yyval.p = yyvsp[-1].p;
                ;
    break;}
case 217:
#line 3465 "parser.y"
{ yyval.p = 0; ;
    break;}
case 218:
#line 3469 "parser.y"
{
		  yyval.p = yyvsp[0].p;
		  {
		    /* We need to make a possible adjustment for integer parameters. */
		    SwigType *type;
		    Node     *n = 0;

		    while (!n) {
		      type = Getattr(yyvsp[0].p,"type");
		      n = Swig_symbol_clookup(type,0);     /* See if we can find a node that matches the typename */
		      if ((n) && (Strcmp(nodeType(n),"cdecl") == 0)) {
			SwigType *decl = Getattr(n,"decl");
			if (!SwigType_isfunction(decl)) {
			  String *value = Getattr(n,"value");
			  if (value) {
			    Setattr(yyvsp[0].p,"type",Copy(value));
			    n = 0;
			  }
			}
		      } else {
			break;
		      }
		    }
		  }
		  
               ;
    break;}
case 219:
#line 3495 "parser.y"
{
                  yyval.p = NewParm(0,0);
                  Setfile(yyval.p,cparse_file);
		  Setline(yyval.p,cparse_line);
		  Setattr(yyval.p,"value",yyvsp[0].dtype.val);
               ;
    break;}
case 220:
#line 3501 "parser.y"
{
                  yyval.p = NewParm(0,0);
                  Setfile(yyval.p,cparse_file);
		  Setline(yyval.p,cparse_line);
		  Setattr(yyval.p,"value",NewString(yyvsp[0].id));
               ;
    break;}
case 221:
#line 3509 "parser.y"
{ 
                  yyval.dtype = yyvsp[0].dtype; 
		  if (yyvsp[0].dtype.type == T_ERROR) {
		    Swig_warning(WARN_PARSE_BAD_DEFAULT,cparse_file, cparse_line, "Can't set default argument (ignored)\n");
		    yyval.dtype.val = 0;
		    yyval.dtype.rawval = 0;
		    yyval.dtype.bitfield = 0;
		    yyval.dtype.throws = 0;
		  }
               ;
    break;}
case 222:
#line 3519 "parser.y"
{
		 Node *n = Swig_symbol_clookup(yyvsp[0].decl.id,0);
		 if (n) {
		   String *q = Swig_symbol_qualified(n);
		   if (Getattr(n,"access")) {
		     if (cplus_mode == CPLUS_PUBLIC) {
		       Swig_warning(WARN_PARSE_PRIVATE, cparse_file, cparse_line,"'%s' is private in this context.\n", yyvsp[0].decl.id);
		       Swig_warning(WARN_PARSE_BAD_DEFAULT, cparse_file, cparse_line,"Can't set default argument value (ignored)\n");
		     }
		     yyval.dtype.val = 0;
		   } else {
		     if (q) {
		       String *temp = NewStringf("%s::%s", q, Getattr(n,"name"));
		       yyval.dtype.val = NewStringf("&%s", SwigType_str(yyvsp[0].decl.type,temp));
		       Delete(q);
		       Delete(temp);
		     } else {
		       yyval.dtype.val = NewStringf("&%s", SwigType_str(yyvsp[0].decl.type,yyvsp[0].decl.id));
		     }
		   }
		 } else {
		   yyval.dtype.val = NewStringf("&%s",SwigType_str(yyvsp[0].decl.type,yyvsp[0].decl.id));
		 }
		 yyval.dtype.rawval = 0;
		 yyval.dtype.type = T_USER;
		 yyval.dtype.bitfield = 0;
		 yyval.dtype.throws = 0;
	       ;
    break;}
case 223:
#line 3547 "parser.y"
{
		 skip_balanced('{','}');
		 yyval.dtype.val = 0;
		 yyval.dtype.rawval = 0;
                 yyval.dtype.type = T_INT;
		 yyval.dtype.bitfield = 0;
		 yyval.dtype.throws = 0;
	       ;
    break;}
case 224:
#line 3555 "parser.y"
{ 
		 yyval.dtype.val = 0;
		 yyval.dtype.rawval = 0;
		 yyval.dtype.type = 0;
		 yyval.dtype.bitfield = yyvsp[0].dtype.val;
		 yyval.dtype.throws = 0;
	       ;
    break;}
case 225:
#line 3562 "parser.y"
{
                 yyval.dtype.val = 0;
                 yyval.dtype.rawval = 0;
                 yyval.dtype.type = T_INT;
		 yyval.dtype.bitfield = 0;
		 yyval.dtype.throws = 0;
               ;
    break;}
case 226:
#line 3571 "parser.y"
{
                 yyval.decl = yyvsp[-1].decl;
		 yyval.decl.defarg = yyvsp[0].dtype.rawval ? yyvsp[0].dtype.rawval : yyvsp[0].dtype.val;
            ;
    break;}
case 227:
#line 3575 "parser.y"
{
              yyval.decl = yyvsp[-1].decl;
	      yyval.decl.defarg = yyvsp[0].dtype.rawval ? yyvsp[0].dtype.rawval : yyvsp[0].dtype.val;
            ;
    break;}
case 228:
#line 3579 "parser.y"
{
   	      yyval.decl.type = 0;
              yyval.decl.id = 0;
	      yyval.decl.defarg = yyvsp[0].dtype.rawval ? yyvsp[0].dtype.rawval : yyvsp[0].dtype.val;
            ;
    break;}
case 229:
#line 3586 "parser.y"
{
                 yyval.decl = yyvsp[0].decl;
		 if (SwigType_isfunction(yyvsp[0].decl.type)) {
		   Delete(SwigType_pop_function(yyvsp[0].decl.type));
		 } else if (SwigType_isarray(yyvsp[0].decl.type)) {
		   SwigType *ta = SwigType_pop_arrays(yyvsp[0].decl.type);
		   if (SwigType_isfunction(yyvsp[0].decl.type)) {
		     Delete(SwigType_pop_function(yyvsp[0].decl.type));
		   } else {
		     yyval.decl.parms = 0;
		   }
		   SwigType_push(yyvsp[0].decl.type,ta);
		   Delete(ta);
		 } else {
		   yyval.decl.parms = 0;
		 }
            ;
    break;}
case 230:
#line 3603 "parser.y"
{
              yyval.decl = yyvsp[0].decl;
	      if (SwigType_isfunction(yyvsp[0].decl.type)) {
		Delete(SwigType_pop_function(yyvsp[0].decl.type));
	      } else if (SwigType_isarray(yyvsp[0].decl.type)) {
		SwigType *ta = SwigType_pop_arrays(yyvsp[0].decl.type);
		if (SwigType_isfunction(yyvsp[0].decl.type)) {
		  Delete(SwigType_pop_function(yyvsp[0].decl.type));
		} else {
		  yyval.decl.parms = 0;
		}
		SwigType_push(yyvsp[0].decl.type,ta);
		Delete(ta);
	      } else {
		yyval.decl.parms = 0;
	      }
            ;
    break;}
case 231:
#line 3620 "parser.y"
{
   	      yyval.decl.type = 0;
              yyval.decl.id = 0;
	      yyval.decl.parms = 0;
	      ;
    break;}
case 232:
#line 3628 "parser.y"
{
              yyval.decl = yyvsp[0].decl;
	      if (yyval.decl.type) {
		SwigType_push(yyvsp[-1].type,yyval.decl.type);
		Delete(yyval.decl.type);
	      }
	      yyval.decl.type = yyvsp[-1].type;
           ;
    break;}
case 233:
#line 3636 "parser.y"
{
              yyval.decl = yyvsp[0].decl;
	      SwigType_add_reference(yyvsp[-2].type);
              if (yyval.decl.type) {
		SwigType_push(yyvsp[-2].type,yyval.decl.type);
		Delete(yyval.decl.type);
	      }
	      yyval.decl.type = yyvsp[-2].type;
           ;
    break;}
case 234:
#line 3645 "parser.y"
{
              yyval.decl = yyvsp[0].decl;
	      if (!yyval.decl.type) yyval.decl.type = NewString("");
           ;
    break;}
case 235:
#line 3649 "parser.y"
{ 
	     yyval.decl = yyvsp[0].decl;
	     yyval.decl.type = NewString("");
	     SwigType_add_reference(yyval.decl.type);
	     if (yyvsp[0].decl.type) {
	       SwigType_push(yyval.decl.type,yyvsp[0].decl.type);
	       Delete(yyvsp[0].decl.type);
	     }
           ;
    break;}
case 236:
#line 3658 "parser.y"
{ 
	     SwigType *t = NewString("");

	     yyval.decl = yyvsp[0].decl;
	     SwigType_add_memberpointer(t,yyvsp[-2].str);
	     if (yyval.decl.type) {
	       SwigType_push(t,yyval.decl.type);
	       Delete(yyval.decl.type);
	     }
	     yyval.decl.type = t;
	     ;
    break;}
case 237:
#line 3669 "parser.y"
{ 
	     SwigType *t = NewString("");
	     yyval.decl = yyvsp[0].decl;
	     SwigType_add_memberpointer(t,yyvsp[-2].str);
	     SwigType_push(yyvsp[-3].type,t);
	     if (yyval.decl.type) {
	       SwigType_push(yyvsp[-3].type,yyval.decl.type);
	       Delete(yyval.decl.type);
	     }
	     yyval.decl.type = yyvsp[-3].type;
	     Delete(t);
	   ;
    break;}
case 238:
#line 3681 "parser.y"
{ 
	     yyval.decl = yyvsp[0].decl;
	     SwigType_add_memberpointer(yyvsp[-4].type,yyvsp[-3].str);
	     SwigType_add_reference(yyvsp[-4].type);
	     if (yyval.decl.type) {
	       SwigType_push(yyvsp[-4].type,yyval.decl.type);
	       Delete(yyval.decl.type);
	     }
	     yyval.decl.type = yyvsp[-4].type;
	   ;
    break;}
case 239:
#line 3691 "parser.y"
{ 
	     SwigType *t = NewString("");
	     yyval.decl = yyvsp[0].decl;
	     SwigType_add_memberpointer(t,yyvsp[-3].str);
	     SwigType_add_reference(t);
	     if (yyval.decl.type) {
	       SwigType_push(t,yyval.decl.type);
	       Delete(yyval.decl.type);
	     } 
	     yyval.decl.type = t;
	   ;
    break;}
case 240:
#line 3704 "parser.y"
{
                /* Note: This is non-standard C.  Template declarator is allowed to follow an identifier */
                 yyval.decl.id = Char(yyvsp[0].str);
		 yyval.decl.type = 0;
		 yyval.decl.parms = 0;
		 yyval.decl.have_parms = 0;
                  ;
    break;}
case 241:
#line 3711 "parser.y"
{
                  yyval.decl.id = Char(NewStringf("~%s",yyvsp[0].str));
                  yyval.decl.type = 0;
                  yyval.decl.parms = 0;
                  yyval.decl.have_parms = 0;
                  ;
    break;}
case 242:
#line 3719 "parser.y"
{
                  yyval.decl.id = Char(yyvsp[-1].str);
                  yyval.decl.type = 0;
                  yyval.decl.parms = 0;
                  yyval.decl.have_parms = 0;
                  ;
    break;}
case 243:
#line 3735 "parser.y"
{
		    yyval.decl = yyvsp[-1].decl;
		    if (yyval.decl.type) {
		      SwigType_push(yyvsp[-2].type,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = yyvsp[-2].type;
                  ;
    break;}
case 244:
#line 3743 "parser.y"
{
		    SwigType *t;
		    yyval.decl = yyvsp[-1].decl;
		    t = NewString("");
		    SwigType_add_memberpointer(t,yyvsp[-3].str);
		    if (yyval.decl.type) {
		      SwigType_push(t,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = t;
		    ;
    break;}
case 245:
#line 3754 "parser.y"
{ 
		    SwigType *t;
		    yyval.decl = yyvsp[-2].decl;
		    t = NewString("");
		    SwigType_add_array(t,(char*)"");
		    if (yyval.decl.type) {
		      SwigType_push(t,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = t;
                  ;
    break;}
case 246:
#line 3765 "parser.y"
{ 
		    SwigType *t;
		    yyval.decl = yyvsp[-3].decl;
		    t = NewString("");
		    SwigType_add_array(t,yyvsp[-1].dtype.val);
		    if (yyval.decl.type) {
		      SwigType_push(t,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = t;
                  ;
    break;}
case 247:
#line 3776 "parser.y"
{
		    SwigType *t;
                    yyval.decl = yyvsp[-3].decl;
		    t = NewString("");
		    SwigType_add_function(t,yyvsp[-1].pl);
		    if (!yyval.decl.have_parms) {
		      yyval.decl.parms = yyvsp[-1].pl;
		      yyval.decl.have_parms = 1;
		    }
		    if (!yyval.decl.type) {
		      yyval.decl.type = t;
		    } else {
		      SwigType_push(t, yyval.decl.type);
		      Delete(yyval.decl.type);
		      yyval.decl.type = t;
		    }
		  ;
    break;}
case 248:
#line 3795 "parser.y"
{
                /* Note: This is non-standard C.  Template declarator is allowed to follow an identifier */
                 yyval.decl.id = Char(yyvsp[0].str);
		 yyval.decl.type = 0;
		 yyval.decl.parms = 0;
		 yyval.decl.have_parms = 0;
                  ;
    break;}
case 249:
#line 3803 "parser.y"
{
                  yyval.decl.id = Char(NewStringf("~%s",yyvsp[0].str));
                  yyval.decl.type = 0;
                  yyval.decl.parms = 0;
                  yyval.decl.have_parms = 0;
                  ;
    break;}
case 250:
#line 3820 "parser.y"
{
		    yyval.decl = yyvsp[-1].decl;
		    if (yyval.decl.type) {
		      SwigType_push(yyvsp[-2].type,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = yyvsp[-2].type;
                  ;
    break;}
case 251:
#line 3828 "parser.y"
{
                    yyval.decl = yyvsp[-1].decl;
		    if (!yyval.decl.type) {
		      yyval.decl.type = NewString("");
		    }
		    SwigType_add_reference(yyval.decl.type);
                  ;
    break;}
case 252:
#line 3835 "parser.y"
{
		    SwigType *t;
		    yyval.decl = yyvsp[-1].decl;
		    t = NewString("");
		    SwigType_add_memberpointer(t,yyvsp[-3].str);
		    if (yyval.decl.type) {
		      SwigType_push(t,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = t;
		    ;
    break;}
case 253:
#line 3846 "parser.y"
{ 
		    SwigType *t;
		    yyval.decl = yyvsp[-2].decl;
		    t = NewString("");
		    SwigType_add_array(t,(char*)"");
		    if (yyval.decl.type) {
		      SwigType_push(t,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = t;
                  ;
    break;}
case 254:
#line 3857 "parser.y"
{ 
		    SwigType *t;
		    yyval.decl = yyvsp[-3].decl;
		    t = NewString("");
		    SwigType_add_array(t,yyvsp[-1].dtype.val);
		    if (yyval.decl.type) {
		      SwigType_push(t,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = t;
                  ;
    break;}
case 255:
#line 3868 "parser.y"
{
		    SwigType *t;
                    yyval.decl = yyvsp[-3].decl;
		    t = NewString("");
		    SwigType_add_function(t,yyvsp[-1].pl);
		    if (!yyval.decl.have_parms) {
		      yyval.decl.parms = yyvsp[-1].pl;
		      yyval.decl.have_parms = 1;
		    }
		    if (!yyval.decl.type) {
		      yyval.decl.type = t;
		    } else {
		      SwigType_push(t, yyval.decl.type);
		      Delete(yyval.decl.type);
		      yyval.decl.type = t;
		    }
		  ;
    break;}
case 256:
#line 3887 "parser.y"
{
		    yyval.decl.type = yyvsp[0].type;
                    yyval.decl.id = 0;
		    yyval.decl.parms = 0;
		    yyval.decl.have_parms = 0;
                  ;
    break;}
case 257:
#line 3893 "parser.y"
{ 
                     yyval.decl = yyvsp[0].decl;
                     SwigType_push(yyvsp[-1].type,yyvsp[0].decl.type);
		     yyval.decl.type = yyvsp[-1].type;
		     Delete(yyvsp[0].decl.type);
                  ;
    break;}
case 258:
#line 3899 "parser.y"
{
		    yyval.decl.type = yyvsp[-1].type;
		    SwigType_add_reference(yyval.decl.type);
		    yyval.decl.id = 0;
		    yyval.decl.parms = 0;
		    yyval.decl.have_parms = 0;
		  ;
    break;}
case 259:
#line 3906 "parser.y"
{
		    yyval.decl = yyvsp[0].decl;
		    SwigType_add_reference(yyvsp[-2].type);
		    if (yyval.decl.type) {
		      SwigType_push(yyvsp[-2].type,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = yyvsp[-2].type;
                  ;
    break;}
case 260:
#line 3915 "parser.y"
{
		    yyval.decl = yyvsp[0].decl;
                  ;
    break;}
case 261:
#line 3918 "parser.y"
{
		    yyval.decl = yyvsp[0].decl;
		    yyval.decl.type = NewString("");
		    SwigType_add_reference(yyval.decl.type);
		    if (yyvsp[0].decl.type) {
		      SwigType_push(yyval.decl.type,yyvsp[0].decl.type);
		      Delete(yyvsp[0].decl.type);
		    }
                  ;
    break;}
case 262:
#line 3927 "parser.y"
{ 
                    yyval.decl.id = 0;
                    yyval.decl.parms = 0;
		    yyval.decl.have_parms = 0;
                    yyval.decl.type = NewString("");
		    SwigType_add_reference(yyval.decl.type);
                  ;
    break;}
case 263:
#line 3934 "parser.y"
{ 
		    yyval.decl.type = NewString("");
                    SwigType_add_memberpointer(yyval.decl.type,yyvsp[-1].str);
                    yyval.decl.id = 0;
                    yyval.decl.parms = 0;
		    yyval.decl.have_parms = 0;
      	          ;
    break;}
case 264:
#line 3941 "parser.y"
{ 
		    SwigType *t = NewString("");
                    yyval.decl.type = yyvsp[-2].type;
		    yyval.decl.id = 0;
		    yyval.decl.parms = 0;
		    yyval.decl.have_parms = 0;
		    SwigType_add_memberpointer(t,yyvsp[-1].str);
		    SwigType_push(yyval.decl.type,t);
		    Delete(t);
                  ;
    break;}
case 265:
#line 3951 "parser.y"
{ 
		    yyval.decl = yyvsp[0].decl;
		    SwigType_add_memberpointer(yyvsp[-3].type,yyvsp[-2].str);
		    if (yyval.decl.type) {
		      SwigType_push(yyvsp[-3].type,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = yyvsp[-3].type;
                  ;
    break;}
case 266:
#line 3962 "parser.y"
{ 
		    SwigType *t;
		    yyval.decl = yyvsp[-2].decl;
		    t = NewString("");
		    SwigType_add_array(t,(char*)"");
		    if (yyval.decl.type) {
		      SwigType_push(t,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = t;
                  ;
    break;}
case 267:
#line 3973 "parser.y"
{ 
		    SwigType *t;
		    yyval.decl = yyvsp[-3].decl;
		    t = NewString("");
		    SwigType_add_array(t,yyvsp[-1].dtype.val);
		    if (yyval.decl.type) {
		      SwigType_push(t,yyval.decl.type);
		      Delete(yyval.decl.type);
		    }
		    yyval.decl.type = t;
                  ;
    break;}
case 268:
#line 3984 "parser.y"
{ 
		    yyval.decl.type = NewString("");
		    yyval.decl.id = 0;
		    yyval.decl.parms = 0;
		    yyval.decl.have_parms = 0;
		    SwigType_add_array(yyval.decl.type,(char*)"");
                  ;
    break;}
case 269:
#line 3991 "parser.y"
{ 
		    yyval.decl.type = NewString("");
		    yyval.decl.id = 0;
		    yyval.decl.parms = 0;
		    yyval.decl.have_parms = 0;
		    SwigType_add_array(yyval.decl.type,yyvsp[-1].dtype.val);
		  ;
    break;}
case 270:
#line 3998 "parser.y"
{
                    yyval.decl = yyvsp[-1].decl;
		  ;
    break;}
case 271:
#line 4001 "parser.y"
{
		    SwigType *t;
                    yyval.decl = yyvsp[-3].decl;
		    t = NewString("");
                    SwigType_add_function(t,yyvsp[-1].pl);
		    if (!yyval.decl.type) {
		      yyval.decl.type = t;
		    } else {
		      SwigType_push(t,yyval.decl.type);
		      Delete(yyval.decl.type);
		      yyval.decl.type = t;
		    }
		    if (!yyval.decl.have_parms) {
		      yyval.decl.parms = yyvsp[-1].pl;
		      yyval.decl.have_parms = 1;
		    }
		  ;
    break;}
case 272:
#line 4018 "parser.y"
{
                    yyval.decl.type = NewString("");
                    SwigType_add_function(yyval.decl.type,yyvsp[-1].pl);
		    yyval.decl.parms = yyvsp[-1].pl;
		    yyval.decl.have_parms = 1;
		    yyval.decl.id = 0;
                  ;
    break;}
case 273:
#line 4028 "parser.y"
{ 
               yyval.type = NewString("");
               SwigType_add_pointer(yyval.type);
	       SwigType_push(yyval.type,yyvsp[-1].str);
	       SwigType_push(yyval.type,yyvsp[0].type);
	       Delete(yyvsp[0].type);
           ;
    break;}
case 274:
#line 4035 "parser.y"
{
	     yyval.type = NewString("");
	     SwigType_add_pointer(yyval.type);
	     SwigType_push(yyval.type,yyvsp[0].type);
	     Delete(yyvsp[0].type);
	     ;
    break;}
case 275:
#line 4041 "parser.y"
{ 
	     	yyval.type = NewString("");	
		SwigType_add_pointer(yyval.type);
	        SwigType_push(yyval.type,yyvsp[0].str);
           ;
    break;}
case 276:
#line 4046 "parser.y"
{
	      yyval.type = NewString("");
	      SwigType_add_pointer(yyval.type);
           ;
    break;}
case 277:
#line 4052 "parser.y"
{ 
                  yyval.str = NewString("");
	          SwigType_add_qualifier(yyval.str,yyvsp[0].id);
               ;
    break;}
case 278:
#line 4056 "parser.y"
{ 
                  yyval.str = yyvsp[0].str; 
                  SwigType_add_qualifier(yyval.str,yyvsp[-1].id);
               ;
    break;}
case 279:
#line 4062 "parser.y"
{ yyval.id = "const"; ;
    break;}
case 280:
#line 4063 "parser.y"
{ yyval.id = "volatile"; ;
    break;}
case 281:
#line 4069 "parser.y"
{
                   yyval.type = yyvsp[0].type;
                   Replace(yyval.type,"typename ","", DOH_REPLACE_ANY);
                ;
    break;}
case 282:
#line 4075 "parser.y"
{
                   yyval.type = yyvsp[0].type;
	           SwigType_push(yyval.type,yyvsp[-1].str);
               ;
    break;}
case 283:
#line 4079 "parser.y"
{ yyval.type = yyvsp[0].type; ;
    break;}
case 284:
#line 4082 "parser.y"
{ yyval.type = yyvsp[0].type;
                  /* Printf(stdout,"primitive = '%s'\n", $$);*/
                ;
    break;}
case 285:
#line 4085 "parser.y"
{ yyval.type = yyvsp[0].type; ;
    break;}
case 286:
#line 4086 "parser.y"
{ yyval.type = yyvsp[0].type; ;
    break;}
case 287:
#line 4087 "parser.y"
{ yyval.type = NewStringf("%s%s",yyvsp[-1].type,yyvsp[0].id); ;
    break;}
case 288:
#line 4088 "parser.y"
{ yyval.type = NewStringf("enum %s", yyvsp[0].str); ;
    break;}
case 289:
#line 4089 "parser.y"
{ yyval.type = yyvsp[0].type; ;
    break;}
case 290:
#line 4090 "parser.y"
{
		  yyval.type = yyvsp[-1].type;
	          SwigType_push(yyval.type,yyvsp[0].str);
     	       ;
    break;}
case 291:
#line 4095 "parser.y"
{
		  yyval.type = yyvsp[0].str;
               ;
    break;}
case 292:
#line 4098 "parser.y"
{ 
		 yyval.type = NewStringf("%s %s", yyvsp[-1].id, yyvsp[0].str);
               ;
    break;}
case 293:
#line 4103 "parser.y"
{
		 if (!yyvsp[0].ptype.type) yyvsp[0].ptype.type = NewString("int");
		 if (yyvsp[0].ptype.us) {
		   yyval.type = NewStringf("%s %s", yyvsp[0].ptype.us, yyvsp[0].ptype.type);
		   Delete(yyvsp[0].ptype.us);
                   Delete(yyvsp[0].ptype.type);
		 } else {
                   yyval.type = yyvsp[0].ptype.type;
		 }
		 if (Cmp(yyval.type,"signed int") == 0) {
		   Delete(yyval.type);
		   yyval.type = NewString("int");
                 } else if (Cmp(yyval.type,"signed long") == 0) {
		   Delete(yyval.type);
                   yyval.type = NewString("long");
                 } else if (Cmp(yyval.type,"signed short") == 0) {
		   Delete(yyval.type);
		   yyval.type = NewString("short");
		 } else if (Cmp(yyval.type,"signed long long") == 0) {
		   Delete(yyval.type);
		   yyval.type = NewString("long long");
		 }
               ;
    break;}
case 294:
#line 4128 "parser.y"
{ 
                 yyval.ptype = yyvsp[0].ptype;
               ;
    break;}
case 295:
#line 4131 "parser.y"
{
                    if (yyvsp[-1].ptype.us && yyvsp[0].ptype.us) {
		      Swig_error(cparse_file, cparse_line, "Extra %s specifier.\n", yyvsp[0].ptype.us);
		    }
                    yyval.ptype = yyvsp[0].ptype;
                    if (yyvsp[-1].ptype.us) yyval.ptype.us = yyvsp[-1].ptype.us;
		    if (yyvsp[-1].ptype.type) {
		      if (!yyvsp[0].ptype.type) yyval.ptype.type = yyvsp[-1].ptype.type;
		      else {
			int err = 0;
			if ((Cmp(yyvsp[-1].ptype.type,"long") == 0)) {
			  if ((Cmp(yyvsp[0].ptype.type,"long") == 0) || (Cmp(yyvsp[0].ptype.type,"double") == 0)) {
			    yyval.ptype.type = NewStringf("long %s", yyvsp[0].ptype.type);
			  } else if (Cmp(yyvsp[0].ptype.type,"int") == 0) {
			    yyval.ptype.type = yyvsp[-1].ptype.type;
			  } else {
			    err = 1;
			  }
			} else if ((Cmp(yyvsp[-1].ptype.type,"short")) == 0) {
			  if (Cmp(yyvsp[0].ptype.type,"int") == 0) {
			    yyval.ptype.type = yyvsp[-1].ptype.type;
			  } else {
			    err = 1;
			  }
			} else if (Cmp(yyvsp[-1].ptype.type,"int") == 0) {
			  yyval.ptype.type = yyvsp[0].ptype.type;
			} else if (Cmp(yyvsp[-1].ptype.type,"double") == 0) {
			  if (Cmp(yyvsp[0].ptype.type,"long") == 0) {
			    yyval.ptype.type = NewString("long double");
			  } else {
			    err = 1;
			  }
			}
			if (err) {
			  Swig_error(cparse_file, cparse_line, "Extra %s specifier.\n", yyvsp[-1].ptype.type);
			}
		      }
		    }
               ;
    break;}
case 296:
#line 4173 "parser.y"
{ 
		    yyval.ptype.type = NewString("int");
                    yyval.ptype.us = 0;
               ;
    break;}
case 297:
#line 4177 "parser.y"
{ 
                    yyval.ptype.type = NewString("short");
                    yyval.ptype.us = 0;
                ;
    break;}
case 298:
#line 4181 "parser.y"
{ 
                    yyval.ptype.type = NewString("long");
                    yyval.ptype.us = 0;
                ;
    break;}
case 299:
#line 4185 "parser.y"
{ 
                    yyval.ptype.type = NewString("char");
                    yyval.ptype.us = 0;
                ;
    break;}
case 300:
#line 4189 "parser.y"
{ 
                    yyval.ptype.type = NewString("float");
                    yyval.ptype.us = 0;
                ;
    break;}
case 301:
#line 4193 "parser.y"
{ 
                    yyval.ptype.type = NewString("double");
                    yyval.ptype.us = 0;
                ;
    break;}
case 302:
#line 4197 "parser.y"
{ 
                    yyval.ptype.us = NewString("signed");
                    yyval.ptype.type = 0;
                ;
    break;}
case 303:
#line 4201 "parser.y"
{ 
                    yyval.ptype.us = NewString("unsigned");
                    yyval.ptype.type = 0;
                ;
    break;}
case 304:
#line 4207 "parser.y"
{ /* scanner_check_typedef(); */ ;
    break;}
case 305:
#line 4207 "parser.y"
{
                   yyval.dtype = yyvsp[0].dtype;
		   if (yyval.dtype.type == T_STRING) {
		     yyval.dtype.rawval = NewStringf("\"%(escape)s\"",yyval.dtype.val);
		   } else {
		     yyval.dtype.rawval = 0;
		   }
		   yyval.dtype.bitfield = 0;
		   yyval.dtype.throws = 0;
		   scanner_ignore_typedef();
                ;
    break;}
case 306:
#line 4226 "parser.y"
{
                   yyval.dtype.val = NewString(yyvsp[0].str);
		   /*		   $$.rawval = NewStringf("\'%(escape)s\'",$$.val); */
		   /*		   Printf(stdout,"rawval = '%s'\n", $$.rawval); */
		   if (Len(yyval.dtype.val)) {
		     yyval.dtype.rawval = NewStringf("\'%(escape)s\'", yyval.dtype.val);
		   } else {
		     yyval.dtype.rawval = NewString("\'\\0'");
		   }
		   yyval.dtype.type = T_CHAR;
		   yyval.dtype.bitfield = 0;
		   yyval.dtype.throws = 0;
		 ;
    break;}
case 307:
#line 4243 "parser.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 308:
#line 4244 "parser.y"
{ yyval.id = (char *) 0;;
    break;}
case 309:
#line 4249 "parser.y"
{ 
                   Node *leftSibling = Getattr(yyvsp[-2].node,"_last");
		   if (!leftSibling) {
                     leftSibling=yyvsp[-2].node;
		   }
                   set_nextSibling(leftSibling,yyvsp[0].node);
                   Setattr(yyvsp[-2].node,"_last",yyvsp[0].node);
                   if (yyvsp[0].node && !Getattr(yyvsp[0].node, "enumvalue")) {
                     /* There is no explicit enum value given, so make one. */
                     Setattr(yyvsp[0].node,"enumvalue", NewStringf("%s+1", Getattr(leftSibling,"name")));
                   }
		   yyval.node = yyvsp[-2].node;
               ;
    break;}
case 310:
#line 4262 "parser.y"
{ 
                   yyval.node = yyvsp[0].node; 
                   if (!Getattr(yyvsp[0].node, "enumvalue")) {
                     /* first enum item value defaults to 0 */
                     Setattr(yyvsp[0].node,"enumvalue", "0");
                   }
               ;
    break;}
case 311:
#line 4271 "parser.y"
{
		   yyval.node = new_node("enumitem");
		   Setattr(yyval.node,"name",yyvsp[0].id);
		   Setattr(yyval.node,"type",NewSwigType(T_INT));
		   Setattr(yyval.node,"feature:immutable","1");
		 ;
    break;}
case 312:
#line 4277 "parser.y"
{
		   yyval.node = new_node("enumitem");
		   Setattr(yyval.node,"name",yyvsp[-2].id);
		   Setattr(yyval.node,"enumvalue", yyvsp[0].dtype.val);
	           if (yyvsp[0].dtype.type == T_CHAR) {
		     Setattr(yyval.node,"value",yyvsp[0].dtype.val);
		     Setattr(yyval.node,"type",NewSwigType(T_CHAR));
		   } else {
		     Setattr(yyval.node,"value",yyvsp[-2].id);
		     Setattr(yyval.node,"type",NewSwigType(T_INT));
		   }
		   Setattr(yyval.node,"feature:immutable","1");
                 ;
    break;}
case 313:
#line 4290 "parser.y"
{ yyval.node = 0; ;
    break;}
case 314:
#line 4293 "parser.y"
{
                   yyval.dtype = yyvsp[0].dtype;
		   if ((yyval.dtype.type != T_INT) && (yyval.dtype.type != T_UINT) &&
		       (yyval.dtype.type != T_LONG) && (yyval.dtype.type != T_ULONG) &&
		       (yyval.dtype.type != T_SHORT) && (yyval.dtype.type != T_USHORT) &&
		       (yyval.dtype.type != T_SCHAR) && (yyval.dtype.type != T_UCHAR)) {
		     Swig_error(cparse_file,cparse_line,"Type error. Expecting an int\n");
		   }
                ;
    break;}
case 315:
#line 4302 "parser.y"
{
                   yyval.dtype.val  = NewString(yyvsp[0].str);
		   yyval.dtype.type = T_INT;
		 ;
    break;}
case 316:
#line 4313 "parser.y"
{ yyval.dtype = yyvsp[0].dtype; ;
    break;}
case 317:
#line 4314 "parser.y"
{ 
		    yyval.dtype.val = NewString(yyvsp[0].id); 
                    yyval.dtype.type = T_STRING; 
               ;
    break;}
case 318:
#line 4318 "parser.y"
{
  		  SwigType_push(yyvsp[-2].type,yyvsp[-1].decl.type);
		  yyval.dtype.val = NewStringf("sizeof(%s)",SwigType_str(yyvsp[-2].type,0));
		  yyval.dtype.type = T_INT;
               ;
    break;}
case 319:
#line 4323 "parser.y"
{ yyval.dtype = yyvsp[0].dtype; ;
    break;}
case 320:
#line 4324 "parser.y"
{
		 Node *n;
		 yyval.dtype.val = yyvsp[0].type;
		 yyval.dtype.type = T_INT;
		 /* Check if value is in scope */
		 n = Swig_symbol_clookup(yyvsp[0].type,0);
		 if (n) {
		   if (Getattr(n,"access") && (cplus_mode == CPLUS_PUBLIC)) {
		     Swig_warning(WARN_PARSE_PRIVATE,cparse_file, cparse_line, "'%s' is private in this context.\n", yyvsp[0].type);
		     yyval.dtype.type = T_ERROR;
		   } else {
		     /* A band-aid for enum values used in expressions. */
		     if (Strcmp(nodeType(n),"enumitem") == 0) {
		       String *q = Swig_symbol_qualified(n);
		       if (q) {
			 yyval.dtype.val = NewStringf("%s::%s", q, Getattr(n,"name"));
			 Delete(q);
		       }
		     }
		   }
		 }
               ;
    break;}
case 321:
#line 4348 "parser.y"
{
   	            yyval.dtype.val = NewStringf("(%s)",yyvsp[-1].dtype.val);
		    yyval.dtype.type = yyvsp[-1].dtype.type;
   	       ;
    break;}
case 322:
#line 4355 "parser.y"
{
                 yyval.dtype = yyvsp[0].dtype;
		 if (yyvsp[0].dtype.type != T_STRING) {
		   yyval.dtype.val = NewStringf("(%s) %s", SwigType_str(yyvsp[-2].dtype.val,0), yyvsp[0].dtype.val);
		 }
 	       ;
    break;}
case 323:
#line 4361 "parser.y"
{
                 yyval.dtype = yyvsp[0].dtype;
		 if (yyvsp[0].dtype.type != T_STRING) {
		   SwigType_push(yyvsp[-3].dtype.val,yyvsp[-2].type);
		   yyval.dtype.val = NewStringf("(%s) %s", SwigType_str(yyvsp[-3].dtype.val,0), yyvsp[0].dtype.val);
		 }
 	       ;
    break;}
case 324:
#line 4368 "parser.y"
{
                 yyval.dtype = yyvsp[0].dtype;
		 if (yyvsp[0].dtype.type != T_STRING) {
		   SwigType_add_reference(yyvsp[-3].dtype.val);
		   yyval.dtype.val = NewStringf("(%s) %s", SwigType_str(yyvsp[-3].dtype.val,0), yyvsp[0].dtype.val);
		 }
 	       ;
    break;}
case 325:
#line 4375 "parser.y"
{
                 yyval.dtype = yyvsp[0].dtype;
		 if (yyvsp[0].dtype.type != T_STRING) {
		   SwigType_push(yyvsp[-4].dtype.val,yyvsp[-3].type);
		   SwigType_add_reference(yyvsp[-4].dtype.val);
		   yyval.dtype.val = NewStringf("(%s) %s", SwigType_str(yyvsp[-4].dtype.val,0), yyvsp[0].dtype.val);
		 }
 	       ;
    break;}
case 326:
#line 4385 "parser.y"
{ yyval.dtype = yyvsp[0].dtype; ;
    break;}
case 327:
#line 4386 "parser.y"
{ yyval.dtype = yyvsp[0].dtype; ;
    break;}
case 328:
#line 4387 "parser.y"
{ yyval.dtype = yyvsp[0].dtype; ;
    break;}
case 329:
#line 4388 "parser.y"
{ yyval.dtype = yyvsp[0].dtype; ;
    break;}
case 330:
#line 4389 "parser.y"
{ yyval.dtype = yyvsp[0].dtype; ;
    break;}
case 331:
#line 4390 "parser.y"
{ yyval.dtype = yyvsp[0].dtype; ;
    break;}
case 332:
#line 4391 "parser.y"
{ yyval.dtype = yyvsp[0].dtype; ;
    break;}
case 333:
#line 4394 "parser.y"
{
		 yyval.dtype.val = NewStringf("%s+%s",yyvsp[-2].dtype.val,yyvsp[0].dtype.val);
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
	       ;
    break;}
case 334:
#line 4398 "parser.y"
{
		 yyval.dtype.val = NewStringf("%s-%s",yyvsp[-2].dtype.val,yyvsp[0].dtype.val);
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
	       ;
    break;}
case 335:
#line 4402 "parser.y"
{
		 yyval.dtype.val = NewStringf("%s*%s",yyvsp[-2].dtype.val,yyvsp[0].dtype.val);
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
	       ;
    break;}
case 336:
#line 4406 "parser.y"
{
		 yyval.dtype.val = NewStringf("%s/%s",yyvsp[-2].dtype.val,yyvsp[0].dtype.val);
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
	       ;
    break;}
case 337:
#line 4410 "parser.y"
{
		 yyval.dtype.val = NewStringf("%s&%s",yyvsp[-2].dtype.val,yyvsp[0].dtype.val);
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
	       ;
    break;}
case 338:
#line 4414 "parser.y"
{
		 yyval.dtype.val = NewStringf("%s|%s",yyvsp[-2].dtype.val,yyvsp[0].dtype.val);
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
	       ;
    break;}
case 339:
#line 4418 "parser.y"
{
		 yyval.dtype.val = NewStringf("%s^%s",yyvsp[-2].dtype.val,yyvsp[0].dtype.val);
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
	       ;
    break;}
case 340:
#line 4422 "parser.y"
{
		 yyval.dtype.val = NewStringf("%s<<%s",yyvsp[-2].dtype.val,yyvsp[0].dtype.val);
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
	       ;
    break;}
case 341:
#line 4426 "parser.y"
{
		 yyval.dtype.val = NewStringf("%s>>%s",yyvsp[-2].dtype.val,yyvsp[0].dtype.val);
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
	       ;
    break;}
case 342:
#line 4430 "parser.y"
{
		 yyval.dtype.val = NewStringf("%s&&%s",yyvsp[-2].dtype.val,yyvsp[0].dtype.val);
		 yyval.dtype.type = T_INT;
	       ;
    break;}
case 343:
#line 4434 "parser.y"
{
		 yyval.dtype.val = NewStringf("%s||%s",yyvsp[-2].dtype.val,yyvsp[0].dtype.val);
		 yyval.dtype.type = T_INT;
	       ;
    break;}
case 344:
#line 4438 "parser.y"
{
		 yyval.dtype.val = NewStringf("-%s",yyvsp[0].dtype.val);
		 yyval.dtype.type = yyvsp[0].dtype.type;
	       ;
    break;}
case 345:
#line 4442 "parser.y"
{
                 yyval.dtype.val = NewStringf("+%s",yyvsp[0].dtype.val);
		 yyval.dtype.type = yyvsp[0].dtype.type;
  	       ;
    break;}
case 346:
#line 4446 "parser.y"
{
		 yyval.dtype.val = NewStringf("~%s",yyvsp[0].dtype.val);
		 yyval.dtype.type = yyvsp[0].dtype.type;
	       ;
    break;}
case 347:
#line 4450 "parser.y"
{
                 yyval.dtype.val = NewStringf("!%s",yyvsp[0].dtype.val);
		 yyval.dtype.type = T_INT;
	       ;
    break;}
case 348:
#line 4454 "parser.y"
{
                 skip_balanced('(',')');
		 if (SwigType_istemplate(yyvsp[-1].type)) {
		   yyvsp[-1].type = SwigType_namestr(yyvsp[-1].type);
		 }
		 yyval.dtype.val = NewStringf("%s%s",yyvsp[-1].type,scanner_ccode);
		 Clear(scanner_ccode);
		 yyval.dtype.type = T_INT;
               ;
    break;}
case 349:
#line 4465 "parser.y"
{
		 yyval.bases = yyvsp[0].bases;
               ;
    break;}
case 350:
#line 4470 "parser.y"
{ inherit_list = 1; ;
    break;}
case 351:
#line 4470 "parser.y"
{ yyval.bases = yyvsp[0].bases; inherit_list = 0; ;
    break;}
case 352:
#line 4471 "parser.y"
{ yyval.bases = 0; ;
    break;}
case 353:
#line 4474 "parser.y"
{
	           yyval.bases = NewList();
	           if (yyvsp[0].node) Append(yyval.bases,yyvsp[0].node);
               ;
    break;}
case 354:
#line 4479 "parser.y"
{
                   yyval.bases = yyvsp[-2].bases;
                   if (yyvsp[0].node) Append(yyval.bases,yyvsp[0].node);
               ;
    break;}
case 355:
#line 4485 "parser.y"
{
                 if (last_cpptype && (Strcmp(last_cpptype,"struct") != 0)) {
                     Swig_warning(WARN_PARSE_NO_ACCESS,cparse_file, cparse_line,"No access specifier given for base class %s (ignored).\n",yyvsp[0].str);
   		     yyval.node = (char *) 0;
                 } else {
		   yyval.node = yyvsp[0].str;
		   Setfile(yyval.node,cparse_file);
		   Setline(yyval.node,cparse_line);
		 }
               ;
    break;}
case 356:
#line 4495 "parser.y"
{
		 yyval.node = 0;
	         if (strcmp(yyvsp[-2].id,"public") == 0) {
		   yyval.node = yyvsp[0].str;
		   Setfile(yyval.node, cparse_file);
		   Setline(yyval.node, cparse_line);
		 } else {
		   Swig_warning(WARN_PARSE_PRIVATE_INHERIT, cparse_file, cparse_line, "%s inheritance ignored.\n", yyvsp[-2].id);
		 }
               ;
    break;}
case 357:
#line 4507 "parser.y"
{ yyval.id = (char*)"public"; ;
    break;}
case 358:
#line 4508 "parser.y"
{ yyval.id = (char*)"private"; ;
    break;}
case 359:
#line 4509 "parser.y"
{ yyval.id = (char*)"protected"; ;
    break;}
case 360:
#line 4513 "parser.y"
{ 
                   yyval.id = (char*)"class"; 
		   if (!inherit_list) last_cpptype = yyval.id;
               ;
    break;}
case 361:
#line 4517 "parser.y"
{ 
                   yyval.id = (char*)"struct"; 
		   if (!inherit_list) last_cpptype = yyval.id;
               ;
    break;}
case 362:
#line 4521 "parser.y"
{
                   yyval.id = (char*)"union"; 
		   if (!inherit_list) last_cpptype = yyval.id;
               ;
    break;}
case 363:
#line 4525 "parser.y"
{ 
                   yyval.id = (char *)"typename"; 
		   if (!inherit_list) last_cpptype = yyval.id;
               ;
    break;}
case 366:
#line 4535 "parser.y"
{
                    yyval.dtype.qualifier = yyvsp[0].str;
                    yyval.dtype.throws = 0;
               ;
    break;}
case 367:
#line 4539 "parser.y"
{
                    yyval.dtype.qualifier = 0;
                    yyval.dtype.throws = yyvsp[-1].pl;
               ;
    break;}
case 368:
#line 4543 "parser.y"
{
                    yyval.dtype.qualifier = yyvsp[-4].str;
                    yyval.dtype.throws = yyvsp[-1].pl;
               ;
    break;}
case 369:
#line 4547 "parser.y"
{ 
                    yyval.dtype.qualifier = 0; 
                    yyval.dtype.throws = 0;
               ;
    break;}
case 370:
#line 4553 "parser.y"
{ 
                    Clear(scanner_ccode); 
                    yyval.decl.have_parms = 0; 
                    yyval.decl.defarg = 0; 
		    yyval.decl.throws = yyvsp[-2].dtype.throws;
               ;
    break;}
case 371:
#line 4559 "parser.y"
{ 
                    skip_balanced('{','}'); 
                    yyval.decl.have_parms = 0; 
                    yyval.decl.defarg = 0; 
                    yyval.decl.throws = yyvsp[-2].dtype.throws;
               ;
    break;}
case 372:
#line 4565 "parser.y"
{ 
                    Clear(scanner_ccode); 
                    yyval.decl.parms = yyvsp[-2].pl; 
                    yyval.decl.have_parms = 1; 
                    yyval.decl.defarg = 0; 
		    yyval.decl.throws = 0;
               ;
    break;}
case 373:
#line 4572 "parser.y"
{
                    skip_balanced('{','}'); 
                    yyval.decl.parms = yyvsp[-2].pl; 
                    yyval.decl.have_parms = 1; 
                    yyval.decl.defarg = 0; 
                    yyval.decl.throws = 0;
               ;
    break;}
case 374:
#line 4579 "parser.y"
{ 
                    yyval.decl.have_parms = 0; 
                    yyval.decl.defarg = yyvsp[-1].dtype.val; 
                    yyval.decl.throws = 0;
               ;
    break;}
case 379:
#line 4594 "parser.y"
{
	            skip_balanced('(',')');
                    Clear(scanner_ccode);
            	;
    break;}
case 380:
#line 4600 "parser.y"
{ 
                     String *s = NewString("");
                     SwigType_add_template(s,yyvsp[-1].p);
                     yyval.id = Char(s);
		     scanner_last_id(1);
                 ;
    break;}
case 381:
#line 4606 "parser.y"
{ yyval.id = (char*)"";  ;
    break;}
case 382:
#line 4609 "parser.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 383:
#line 4610 "parser.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 384:
#line 4613 "parser.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 385:
#line 4614 "parser.y"
{ yyval.id = 0; ;
    break;}
case 386:
#line 4617 "parser.y"
{ 
                  yyval.str = 0;
		  if (!yyval.str) yyval.str = NewStringf("%s%s", yyvsp[-1].str,yyvsp[0].str);
      	          Delete(yyvsp[0].str);
               ;
    break;}
case 387:
#line 4622 "parser.y"
{ 
		 yyval.str = NewStringf("::%s%s",yyvsp[-1].str,yyvsp[0].str);
                 Delete(yyvsp[0].str);
               ;
    break;}
case 388:
#line 4626 "parser.y"
{
		 yyval.str = NewString(yyvsp[0].str);
   	       ;
    break;}
case 389:
#line 4629 "parser.y"
{
		 yyval.str = NewStringf("::%s",yyvsp[0].str);
               ;
    break;}
case 390:
#line 4632 "parser.y"
{
                 yyval.str = NewString(yyvsp[0].str);
	       ;
    break;}
case 391:
#line 4635 "parser.y"
{
                 yyval.str = NewStringf("::%s",yyvsp[0].str);
               ;
    break;}
case 392:
#line 4640 "parser.y"
{
                   yyval.str = NewStringf("::%s%s",yyvsp[-1].str,yyvsp[0].str);
		   Delete(yyvsp[0].str);
               ;
    break;}
case 393:
#line 4644 "parser.y"
{
                   yyval.str = NewStringf("::%s",yyvsp[0].str);
               ;
    break;}
case 394:
#line 4647 "parser.y"
{
                   yyval.str = NewStringf("::%s",yyvsp[0].str);
               ;
    break;}
case 395:
#line 4654 "parser.y"
{
		 yyval.str = NewStringf("::~%s",yyvsp[0].str);
               ;
    break;}
case 396:
#line 4660 "parser.y"
{
                  yyval.str = NewStringf("%s%s",yyvsp[-1].id,yyvsp[0].id);
		  /*		  if (Len($2)) {
		    scanner_last_id(1);
		    } */
              ;
    break;}
case 397:
#line 4669 "parser.y"
{ 
                  yyval.str = 0;
		  if (!yyval.str) yyval.str = NewStringf("%s%s", yyvsp[-1].id,yyvsp[0].str);
      	          Delete(yyvsp[0].str);
               ;
    break;}
case 398:
#line 4674 "parser.y"
{ 
		 yyval.str = NewStringf("::%s%s",yyvsp[-1].id,yyvsp[0].str);
                 Delete(yyvsp[0].str);
               ;
    break;}
case 399:
#line 4678 "parser.y"
{
		 yyval.str = NewString(yyvsp[0].id);
   	       ;
    break;}
case 400:
#line 4681 "parser.y"
{
		 yyval.str = NewStringf("::%s",yyvsp[0].id);
               ;
    break;}
case 401:
#line 4684 "parser.y"
{
                 yyval.str = NewString(yyvsp[0].str);
	       ;
    break;}
case 402:
#line 4687 "parser.y"
{
                 yyval.str = NewStringf("::%s",yyvsp[0].str);
               ;
    break;}
case 403:
#line 4692 "parser.y"
{
                   yyval.str = NewStringf("::%s%s",yyvsp[-1].id,yyvsp[0].str);
		   Delete(yyvsp[0].str);
               ;
    break;}
case 404:
#line 4696 "parser.y"
{
                   yyval.str = NewStringf("::%s",yyvsp[0].id);
               ;
    break;}
case 405:
#line 4699 "parser.y"
{
                   yyval.str = NewStringf("::%s",yyvsp[0].str);
               ;
    break;}
case 406:
#line 4702 "parser.y"
{
		 yyval.str = NewStringf("::~%s",yyvsp[0].id);
               ;
    break;}
case 407:
#line 4708 "parser.y"
{ 
                   yyval.id = (char *) malloc(strlen(yyvsp[-1].id)+strlen(yyvsp[0].id)+1);
                   strcpy(yyval.id,yyvsp[-1].id);
                   strcat(yyval.id,yyvsp[0].id);
               ;
    break;}
case 408:
#line 4713 "parser.y"
{ yyval.id = yyvsp[0].id;;
    break;}
case 409:
#line 4716 "parser.y"
{
		 yyval.str = NewString(yyvsp[0].id);
               ;
    break;}
case 410:
#line 4719 "parser.y"
{
                  skip_balanced('{','}');
		  yyval.str = NewString(scanner_ccode);
               ;
    break;}
case 411:
#line 4723 "parser.y"
{
		 yyval.str = yyvsp[0].str;
              ;
    break;}
case 412:
#line 4728 "parser.y"
{
                  Hash *n;
                  yyval.node = NewHash();
                  n = yyvsp[-1].node;
                  while(n) {
                     String *name, *value;
                     name = Getattr(n,"name");
                     value = Getattr(n,"value");
		     if (!value) value = (String *) "1";
                     Setattr(yyval.node,name, value);
		     n = nextSibling(n);
		  }
               ;
    break;}
case 413:
#line 4741 "parser.y"
{ yyval.node = 0; ;
    break;}
case 414:
#line 4745 "parser.y"
{
		 yyval.node = NewHash();
		 Setattr(yyval.node,"name",yyvsp[-2].id);
		 Setattr(yyval.node,"value",yyvsp[0].id);
               ;
    break;}
case 415:
#line 4750 "parser.y"
{
		 yyval.node = NewHash();
		 Setattr(yyval.node,"name",yyvsp[-4].id);
		 Setattr(yyval.node,"value",yyvsp[-2].id);
		 set_nextSibling(yyval.node,yyvsp[0].node);
               ;
    break;}
case 416:
#line 4756 "parser.y"
{
                 yyval.node = NewHash();
                 Setattr(yyval.node,"name",yyvsp[0].id);
	       ;
    break;}
case 417:
#line 4760 "parser.y"
{
                 yyval.node = NewHash();
                 Setattr(yyval.node,"name",yyvsp[-2].id);
                 set_nextSibling(yyval.node,yyvsp[0].node);
               ;
    break;}
case 418:
#line 4767 "parser.y"
{
		 yyval.id = yyvsp[0].id;
               ;
    break;}
case 419:
#line 4770 "parser.y"
{
                 yyval.id = yyvsp[0].dtype.val;
               ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 4777 "parser.y"


SwigType *Swig_cparse_type(String *s) {
   String *ns;
   extern void scanner_file(File *);
   extern int yyparse();
   extern void scanner_next_token(int);
   ns = NewStringf("%s;",s);
   Seek(ns,0,SEEK_SET);
   scanner_file(ns);
   top = 0;
   scanner_next_token(PARSETYPE);
   yyparse();
   /*   Printf(stdout,"typeparse: '%s' ---> '%s'\n", s, top); */
   return top;
}


Parm *Swig_cparse_parm(String *s) {
   String *ns;
   extern void scanner_file(File *);
   extern int yyparse();
   extern void scanner_next_token(int);
   ns = NewStringf("%s;",s);
   Seek(ns,0,SEEK_SET);
   scanner_file(ns);
   top = 0;
   scanner_next_token(PARSEPARM);
   yyparse();
   /*   Printf(stdout,"typeparse: '%s' ---> '%s'\n", s, top); */
   return top;
}









