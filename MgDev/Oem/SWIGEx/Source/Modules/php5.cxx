/*
 * PHP5 Support
 *
 * Alain Lebatard
 * Jun 2004
 *
 * Based on the PHP4 module
 *
 */

#include "swigmod.h"

#ifndef MACSWIG
#include "swigconfig.h"
#endif

#include "ExceptionClass.h"

#include <ctype.h>
#include <map>
#include <list>
#include <vector>
#include <string>

#define IS_NULL		0
#define IS_LONG		1
#define IS_DOUBLE	2
#define IS_STRING	3
#define IS_OBJECT	5
#define IS_BOOL		6

using namespace std;

static const char *usage = (char*)"\
PHP5 Options (available with -php)\n\
     -ldflags		 - Print runtime libraries to link with\n\
     -cppext         - cpp file extension (default to .cpp)\n\
     -noproxy        - Don't generate proxy classes.\n\
     -dlname <name>  - Set module prefix to <name>\n\
     -make           - Create simple makefile\n\
     -phpfull        - Create full make files\n\
     -withincs <libs>- With -phpfull writes needed incs in config.m4\n\
     -withlibs <libs>- With -phpfull writes needed libs in config.m4\n\
     -withc <libs>   - With -phpfull makes extra c files in Makefile.in\n\
     -withcxx <libs> - With -phpfull makes extra c++ files in Makefile.in\n\
\n";

static int constructors=0;
static String *NOTCLASS=NewString("Not a class");
static Node *classnode=0;
static String *module = 0;
static String *cap_module = 0;
static String *dlname = 0;
static String *withlibs = 0;
static String *withincs = 0;
static String *withc = 0;
static String *withcxx = 0;
static bool emitedThrowFunction = false;

//static char	*package = 0;	// Name of the package
static char *shadow_classname;

static Wrapper  *f_php;
static int	gen_extra = 0;
static int	gen_make = 0;

static File	  *f_runtime = 0;
static File	  *f_h = 0;
static File	  *f_phpcode = 0;
static String	  *phpfilename =0;

static String	  *s_header;
static String	  *s_wrappers;
static String	  *s_init;
static String	  *s_vinit;
static String	  *s_vdecl;
static String	  *s_cinit;
static String	  *s_oinit;
static String	  *s_entry;
static String	  *cs_entry;
static String	  *all_cs_entry;
static String	  *pragma_incl;
static String	  *pragma_code;
static String	  *pragma_phpinfo;

/* Variables for using PHP classes */
static String	  *class_name = 0;
static String	  *realpackage = 0;
static String	  *package = 0;

static Hash	*shadow_get_vars;
static Hash	*shadow_set_vars;
static String	*shadow_classdef;
static String 	*shadow_code;
static int	have_default_constructor = 0;
#define NATIVE_CONSTRUCTOR 1
#define ALTERNATIVE_CONSTRUCTOR 2
static int	native_constructor=0;
static int	destructor=0;
static int	enum_flag = 0; // Set to 1 when wrapping an enum
static int	static_flag = 0; // Set to 1 when wrapping a static functions or member variables
static int	const_flag = 0; // Set to 1 when wrapping a const member variables
static int	variable_wrapper_flag = 0; // Set to 1 when wrapping a member variable/enum/const
static int	wrapping_member = 0;
static Hash	*zend_types = 0;

static String *shadow_enum_code = 0;
static String *php_enum_code = 0;
static String *all_shadow_extra_code = 0; 
		//Extra code for all shadow classes from %pragma
static String *this_shadow_extra_code = 0; 
		//Extra Code for current single shadow class freom %pragma
static String *all_shadow_import = 0; 
		//import for all shadow classes from %pragma
static String *this_shadow_import = 0; 
		//import for current shadow classes from %pragma
static String *module_baseclass = 0; 
		//inheritance for module class from %pragma
static String *all_shadow_baseclass = 0; 
		//inheritence for all shadow classes from %pragma
static String *this_shadow_baseclass = 0; 
		//inheritance for shadow class from %pragma and cpp_inherit
static String *this_shadow_multinherit = 0;
static int	  shadow	= 1;
static map<string, bool> wrapped;
static map<string, bool> methodEntries;

extern map<string, int> clsIds;
extern "C" bool			enableRuntimeTypeChecking;
extern ExceptionClass*	rootException;
extern String*			baseException;
extern String*          disposeCode;
extern String*          rethrowCode;

class Parameter
{
public:
    Parameter(int type, string typeStr) 
    {
        this->type = type;
        this->typeStr = typeStr;
    }
    int     type;
    string  typeStr;
};

class OverloadedFunc
{
public:
	OverloadedFunc(string mangledName, int argcount, vector<Parameter*>* parms) 
	{ 
		this->mangledName = mangledName; this->argcount = argcount; this->parms = parms; 
		this->done = false;
	}
	string mangledName;
	int argcount;
	vector<Parameter*>* parms;
	bool done;
} ;

map<string, vector<OverloadedFunc*>*> overloads;

extern "C" {
static void (*r_prevtracefunc)(SwigType *t, String *mangled, String *clientdata) = 0;
}

static const char *php_header =
"/*"
"\n  +----------------------------------------------------------------------+"
"\n  | PHP version 5.0                                                      |"
"\n  +----------------------------------------------------------------------+"
"\n  | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |"
"\n  +----------------------------------------------------------------------+"
"\n  | This source file is subject to version 2.02 of the PHP license,      |"
"\n  | that is bundled with this package in the file LICENSE, and is        |"
"\n  | available at through the world-wide-web at                           |"
"\n  | http://www.php.net/license/2_02.txt.                                 |"
"\n  | If you did not receive a copy of the PHP license and are unable to   |"
"\n  | obtain it through the world-wide-web, please send a note to          |"
"\n  | license@php.net so we can mail you a copy immediately.               |"
"\n  +----------------------------------------------------------------------+"
"\n  | Authors:                                                             |"
"\n  |                                                                      |"
"\n  +----------------------------------------------------------------------+"
"\n */\n";

void emitThrowFunctionPrototype()
{
	if(baseException != NULL)
		Printf(s_header, "void ThrowPHPExceptionWrapper(%s* e TSRMLS_DC);\n\n", baseException);
}


void
SwigPHP5_emit_resource_registrations() {
  DOH *key;
  Iterator ki;
  String *destructor=0;
  String *classname=0;
  String *shadow_classname=0;

  if (!zend_types) return;

  //emit the throw function
 emitThrowFunctionPrototype();


  ki = First(zend_types);
  if (ki.key) Printf(s_oinit,"\n/* Register resource destructors for pointer types */\n");
  while (ki.key) if (1 /* is pointer type*/) {
    key = ki.key;
    Node *class_node;
    if ((class_node=Getattr(zend_types,key))) {
      // Write out destructor function header
      Printf(s_wrappers,"/* NEW Destructor style */\nstatic ZEND_RSRC_DTOR_FUNC(_wrap_destroy%s) {\n",key);

      // write out body
      if ((class_node!=NOTCLASS)) {
        classname = Getattr(class_node,"name");
        if (! (shadow_classname = Getattr(class_node,"sym:name"))) shadow_classname=classname;
        // Do we have a known destructor for this type?
        if ((destructor = Getattr(class_node,"destructor"))) {
          Printf(s_wrappers,"/* has destructor: %s */\n",destructor);
          Printf(s_wrappers,"%s(rsrc, SWIGTYPE%s->name TSRMLS_CC);\n",destructor,key);
        } else {
          Printf(s_wrappers,"/* bah! No destructor for this wrapped class!! */\n");
        }
      } else {
          Printf(s_wrappers,"/* bah! No destructor for this simple type!! */\n");
      }

      // close function
      Printf(s_wrappers,"}\n");

      // declare le_swig_<mangled> to store php registration
      Printf(s_vdecl,"static int le_swig_%s=0; /* handle for %s */\n", key, shadow_classname);

      // register with php
      Printf(s_oinit,"le_swig_%s=zend_register_list_destructors_ex"
	     "(_wrap_destroy%s,NULL,(char *)(SWIGTYPE%s->name),module_number);\n",
	     key,key,key);

      // store php type in class struct
      Printf(s_oinit,"SWIG_TypeClientData(SWIGTYPE%s,&le_swig_%s);\n",
           key,key);
    }
    ki = Next(ki);
  }
}

class PHP5 : public Language {
public:

	PHP5()
	{
		this->emitExCount = 0;
	}

  /* Test to see if a type corresponds to something wrapped with a shadow class. */
  
  String *is_shadow(SwigType *t) {
    String *r = 0;
    Node *n = classLookup(t);
    if (n) {
      r = Getattr(n,"php:proxy");   // Set by classDeclaration()
      if (!r) {
	r = Getattr(n,"sym:name");      // Not seen by classDeclaration yet, but this is the name
      }
    }
    return r;
  }

  void emitThrowFunctionDefinition()
  {
	  if(baseException == NULL)
		  return;

	  Printf(s_wrappers, "\n\nvoid ThrowPHPExceptionWrapper(%s* e TSRMLS_DC)\n", baseException);
	  Printf(s_wrappers, "{\n");

      if(rethrowCode)
      {
	    Printf(s_wrappers, "\ttry\n");
	    Printf(s_wrappers, "\t{\n");
	    
        Printf(s_wrappers, "\t\t%s\n", rethrowCode);

	    Printf(s_wrappers, "\t}\n");
	    emitCatchSequence(rootException);
      }
      else
      {
	    this->emitExCount = 0;
	    emitCatchSequence(rootException);
      }

	  Printf(s_wrappers, "}\n\n");
  }

  void emitCatchSequence(ExceptionClass* e)
  {
	  if(e->derivedClasses.size() > 0)
	  {
		  list<ExceptionClass*>::const_iterator iter = e->derivedClasses.begin();
		  while(iter != e->derivedClasses.end())
		  {
			  emitCatchSequence(*iter);
			  iter++;
		  }
	  }

      if(rethrowCode)
      {
    	  Printf(s_wrappers, "\tcatch(%s* e)\n", e->name.c_str());
      }
      else
      {
	    if(this->emitExCount++ == 0)
		    Printf(s_wrappers, "\tif(typeid(*e) == typeid(%s))\n", e->name.c_str());
	    else
		    Printf(s_wrappers, "\telse if(typeid(*e) == typeid(%s))\n", e->name.c_str());
      }

	  Printf(s_wrappers, "\t{\n");
      Printf(s_wrappers, "\t\tzval *obj, *_cPtr;\n");
      Printf(s_wrappers, "\t\tMAKE_STD_ZVAL(obj);\n");
      Printf(s_wrappers, "\t\tMAKE_STD_ZVAL(_cPtr);\n");
      Printf(s_wrappers, "\t\tSWIG_SetPointerZval(_cPtr, (void *)e, SWIGTYPE_p_%s, 1);\n", e->name.c_str());
      Printf(s_wrappers, "\t\tobject_init_ex(obj,ptr_ce_swig_%s);\n", e->name.c_str());
      Printf(s_wrappers, "\t\tadd_property_zval(obj,\"_cPtr\",_cPtr);\n");
      Printf(s_wrappers, "\t\tEG(exception) = obj;\n");

	  Printf(s_wrappers, "\t}\n");
  }

    bool isException(ExceptionClass* ex, string name)
    {
        if(!ex)
            return false;
        if(ex->name == name)
            return true;
        for(list<ExceptionClass*>::const_iterator it = ex->derivedClasses.begin(); it != ex->derivedClasses.end(); it++)
        {
            if(isException(*it, name))
                return true;
        }
        return false;
    }




  /* -----------------------------------------------------------------------------
   * get_pointer()
   * ----------------------------------------------------------------------------- */
  void
  get_pointer(char *iname, char *srcname, char *src, char *dest,
	      SwigType *t, String *f, char *ret) {

    SwigType_remember(t);
    SwigType *lt = SwigType_ltype(t);
    Printv(f, "if (SWIG_ConvertPtr(", src, ",(void **) ", dest, ",", NIL);
    
    /* If we're passing a void pointer, we give the pointer conversion a NULL
       pointer, otherwise pass in the expected type. */
    
    if (Cmp(lt,"p.void") == 0) {
      Printf(f, " 0 ) < 0) {\n");
    } else {
      Printv(f, "SWIGTYPE", SwigType_manglestr(t), ") < 0) {\n",NIL);
    }
    
    Printv(f,
	   "zend_error(E_ERROR, \"Type error in ", srcname, " of ", iname,
	   " Expected %s\", SWIGTYPE", SwigType_manglestr(t), "->name);\n", ret,
	   ";\n",
	   "}\n",
	   NIL);
    Delete(lt);
  }

  /* ------------------------------------------------------------
   * main()
   * ------------------------------------------------------------ */

  virtual void main(int argc, char *argv[]) {
    int i;
    SWIG_library_directory("php5");
    SWIG_config_cppext("cpp");

    for(i = 1; i < argc; i++) {
      if (argv[i]) {
	if(strcmp(argv[i], "-phpfull") == 0) {
	  gen_extra = 1;
	  Swig_mark_arg(i);
	} else if(strcmp(argv[i], "-dlname") == 0) {
	  if (argv[i+1]) {
	    dlname = NewString(argv[i+1]);
	    Swig_mark_arg(i);
	    Swig_mark_arg(i+1);
	    i++;
	  } else {
	    Swig_arg_error();
	  }
	} else if(strcmp(argv[i], "-baseexception") == 0) {
	  if (argv[i+1]) {
	    baseException = NewString(argv[i+1]);
	    Swig_mark_arg(i);
	    Swig_mark_arg(i+1);
	    i++;
	  } else {
	    Swig_arg_error();
	  }
	} else if(strcmp(argv[i], "-withlibs") == 0) {
	  if (argv[i+1]) {
	    withlibs = NewString(argv[i+1]);
	    Swig_mark_arg(i);
	    Swig_mark_arg(i+1);
	    i++;
	  } else {
	    Swig_arg_error();
	  }
	} else if(strcmp(argv[i], "-withincs") == 0) {
	  if (argv[i+1]) {
	    withincs = NewString(argv[i+1]);
	    Swig_mark_arg(i);
	    Swig_mark_arg(i+1);
	    i++;
	  } else {
	    Swig_arg_error();
	  }
	} else if(strcmp(argv[i], "-withc") == 0) {
	  if (argv[i+1]) {
	    withc = NewString(argv[i+1]);
	    Swig_mark_arg(i);
	    Swig_mark_arg(i+1);
	    i++;
	  } else {
	    Swig_arg_error();
	  }
	} else if(strcmp(argv[i], "-withcxx") == 0) {
	  if (argv[i+1]) {
	    withcxx = NewString(argv[i+1]);
	    Swig_mark_arg(i);
	    Swig_mark_arg(i+1);
	    i++;
	  } else {
	    Swig_arg_error();
	  }
	} else if(strcmp(argv[i], "-cppext") == 0) {
	  if (argv[i+1]) {
	    SWIG_config_cppext(argv[i+1]);
	    Swig_mark_arg(i);
	    Swig_mark_arg(i+1);
	    i++;
	  } else {
	    Swig_arg_error();
	  }
	}  else if((strcmp(argv[i], "-noshadow") == 0) || (strcmp(argv[i],"-noproxy") == 0)) {
	  shadow = 0;
	  Swig_mark_arg(i);
	} else if(strcmp(argv[i], "-make") == 0) {
	  gen_make = 1;
	  Swig_mark_arg(i);
	} else if(strcmp(argv[i], "-help") == 0) {
	  fputs(usage, stderr);
	} else if (strcmp (argv[i], "-ldflags") == 0) {
	  printf("%s\n", SWIG_PHP_RUNTIME);
	  SWIG_exit (EXIT_SUCCESS);
	}
      }
    }
    
    Preprocessor_define((void *) "SWIGPHP 1", 0);
    Preprocessor_define((void *) "SWIGPHP5 1", 0);
    Preprocessor_define ("SWIG_NO_OVERLOAD 1", 0);    
    SWIG_typemap_lang("php5");
    /* DB: Suggest using a language configuration file */
    SWIG_config_file("php5.swg");

	allow_overloading();
  }
  
  void create_simple_make(void) {
    File *f_make;
    
    f_make = NewFile((void *)"makefile", "w");
    if(CPlusPlus)
      Printf(f_make, "CC=g++\n");
    else
      Printf(f_make, "CC=gcc\n");
    
    Printf(f_make,
	   "OBJS=%s_wrap.o\n"
	   "PROG=lib%s.so\n"
	   "CFLAGS=-fpic\n"
	   "LDFLAGS=-shared\n"
	   "PHP_INC=`php-config --includes`\n"
	   "EXTRA_INC=\n"
	   "EXTRA_LIB=\n\n",
	   module, module);
    
    Printf(f_make,
	   "$(PROG): $(OBJS)\n"
	   "\t$(CC) $(LDFLAGS) $(OBJS) -o $(PROG) $(EXTRA_LIB)\n\n"
	   "%%.o: %%.%s\n"
	   "\t$(CC) $(EXTRA_INC) $(PHP_INC) $(CFLAGS) -c $<\n",
	   (CPlusPlus?"cpp":"c"));
    
    Close(f_make);
  }
  
  void create_extra_files(String *outfile) {
    File *f_extra;
    
    static String *configm4=0;
    static String *makefilein=0;
    static String *credits=0;
    
    configm4=NewString("");
    Printv(configm4, SWIG_output_directory(), "config.m4", NIL);
    
    makefilein=NewString("");
    Printv(makefilein, SWIG_output_directory(), "Makefile.in", NIL);
    
    credits=NewString("");
    Printv(credits, SWIG_output_directory(), "CREDITS", NIL);
    
    // are we a --with- or --enable-
    int with=(withincs || withlibs)?1:0;
    
    // Note makefile.in only copes with one source file
    // also withincs and withlibs only take one name each now
    // the code they generate should be adapted to take multiple lines
    
    if(gen_extra) {
      /* Write out Makefile.in */
      f_extra = NewFile(makefilein, "w");
      if (!f_extra) {
	Printf(stderr,"Unable to open %s\n",makefilein);
	SWIG_exit(EXIT_FAILURE);
      }
      
      Printf(f_extra,
	     "# $Id: php5.cxx $\n\n"
	     "LTLIBRARY_NAME          = php_%s.la\n",
	     module);
      
      // CPP has more and different entires to C in Makefile.in
      if (! CPlusPlus) Printf(f_extra,"LTLIBRARY_SOURCES       = %s %s\n"
                                      "LTLIBRARY_SOURCES_CPP   = %s\n",Swig_file_filename(outfile),withc,withcxx);
      else Printf(f_extra,"LTLIBRARY_SOURCES       = %s\n"
		  "LTLIBRARY_SOURCES_CPP   = %s %s\n"
		  "LTLIBRARY_OBJECTS_X = $(LTLIBRARY_SOURCES_CPP:.cpp=.lo) $(LTLIBRARY_SOURCES_CPP:.cxx=.lo)\n"
		  ,withc,Swig_file_filename(outfile),withcxx);
      
      Printf(f_extra,"LTLIBRARY_SHARED_NAME   = php_%s.la\n"
	     "LTLIBRARY_SHARED_LIBADD = $(%(upper)s_SHARED_LIBADD)\n\n"
	     "include $(top_srcdir)/build/dynlib.mk\n",
	     module,module);

      Printf(f_extra,"\n# patch in .cxx support to php build system to work like .cpp\n"
                     ".SUFFIXES: .cxx\n\n"
                     ".cxx.o:\n"
                     "	$(CXX_COMPILE) -c $<\n\n"
                     ".cxx.lo:\n"
                     "	$(CXX_PHP_COMPILE)\n\n"
                     ".cxx.slo:\n"
                     "	$(CXX_SHARED_COMPILE)\n\n");

      Printf(f_extra,"\n# make it easy to test module\n"
                     "testmodule:\n"
                     "	php -q -d extension_dir=modules %s\n\n",Swig_file_filename(phpfilename));
      Close(f_extra);
      
      /* Now config.m4 */
      // Note: # comments are OK in config.m4 if you don't mind them
      // appearing in the final ./configure file 
      // (which can help with ./configure debugging)
      
      // NOTE2: phpize really ought to be able to write out a sample
      // config.m4 based on some simple data, I'll take this up with
      // the php folk!
      f_extra = NewFile(configm4, "w");
      if (!f_extra) {
	Printf(stderr, "Unable to open %s\n",configm4);
	SWIG_exit(EXIT_FAILURE);
      }
      
      Printf(f_extra,
	     "dnl $Id: php5.cxx $\n"
	     "dnl ***********************************************************************\n"
	     "dnl ** THIS config.m4 is provided for PHPIZE and PHP's consumption NOT\n"
	     "dnl ** for any part of the rest of the %s build system\n"
	     "dnl ***********************************************************************\n\n"
	     ,module);
      
      if (! with) { // must be enable then
	Printf(f_extra,
	       "PHP_ARG_ENABLE(%s, whether to enable %s support,\n"
	       "[  --enable-%s             Enable %s support])\n\n",
	       module,module,module,module);
      } else {
	Printf(f_extra,
	       "PHP_ARG_WITH(%s, for %s support,\n"
	       "[  --with-%s[=DIR]             Include %s support.])\n\n",
	       module,module,module,module);
	// These tests try and file the library we need
	Printf(f_extra,"dnl THESE TESTS try and find the library and header files\n"
	       "dnl your new php module needs. YOU MAY NEED TO EDIT THEM\n"
	       "dnl as written they assume your header files are all in the same place\n\n");
	
	Printf(f_extra,"dnl ** are we looking for %s_lib.h or something else?\n",module);
	if (withincs) Printf(f_extra,"HNAMES=\"%s\"\n\n",withincs);
	else Printf(f_extra,"HNAMES=\"\"; # %s_lib.h ?\n\n",module);
	
	Printf(f_extra,"dnl ** Are we looking for lib%s.a or lib%s.so or something else?\n",module,module);
	if (withlibs) Printf(f_extra,"LIBNAMES=\"%s\"\n\n",withlibs);
	else Printf(f_extra,"LIBNAMES=\"\"; # lib_%s.so ?\n\n",withlibs);
	Printf(f_extra,"dnl IF YOU KNOW one of the symbols in the library and you\n"
	       "dnl specify it below then we can have a link test to see if it works\n"
	       "LIBSYMBOL=\"\"\n\n");
      }
      
      // Now write out tests to find thing.. they may need to extend tests
      Printf(f_extra,"if test \"$PHP_%(upper)s\" != \"no\"; then\n\n",module);
      
      // Ready for when we add libraries as we find them
      Printf(f_extra,"  PHP_SUBST(%(upper)s_SHARED_LIBADD)\n\n",module);
      
      if (withlibs) { // find more than one library
	Printf(f_extra,"  for LIBNAME in $LIBNAMES ; do\n");
	Printf(f_extra,"    LIBDIR=\"\"\n");
	// For each path element to try...
	Printf(f_extra,"    for i in $PHP_%(upper)s $PHP_%(upper)s/lib /usr/lib /usr/local/lib ; do\n",module,module);
	Printf(f_extra,"      if test -r $i/lib$LIBNAME.a -o -r $i/lib$LIBNAME.so ; then\n"
	       "        LIBDIR=\"$i\"\n"
	       "        break\n"
	       "      fi\n"
	       "    done\n\n");
	Printf(f_extra,"    dnl ** and $LIBDIR should be the library path\n"
	       "    if test \"$LIBNAME\" != \"\" -a -z \"$LIBDIR\" ; then\n"
	       "      AC_MSG_RESULT(Library files $LIBNAME not found)\n"
	       "      AC_MSG_ERROR(Is the %s distribution installed properly?)\n"
	       "    else\n"
	       "      AC_MSG_RESULT(Library files $LIBNAME found in $LIBDIR)\n"
	       "      PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LIBDIR, %(upper)s_SHARED_LIBADD)\n"
	       "    fi\n",module,module);
	Printf(f_extra,"  done\n\n");
      }
      
      if (withincs) {  // Find more than once include
	Printf(f_extra,"  for HNAME in $HNAMES ; do\n");
	Printf(f_extra,"    INCDIR=\"\"\n");
	// For each path element to try...
	Printf(f_extra,"    for i in $PHP_%(upper)s $PHP_%(upper)s/include $PHP_%(upper)s/includes $PHP_%(upper)s/inc $PHP_%(upper)s/incs /usr/local/include /usr/include; do\n",module,module,module,module,module);
	// Try and find header files
	Printf(f_extra,"      if test \"$HNAME\" != \"\" -a -r $i/$HNAME ; then\n"
	       "        INCDIR=\"$i\"\n"
	       "        break\n"
	       "      fi\n"
	       "    done\n\n");
	
	Printf(f_extra,
	       "    dnl ** Now $INCDIR should be the include file path\n"
	       "    if test \"$HNAME\" != \"\" -a -z \"$INCDIR\" ; then\n"
	       "      AC_MSG_RESULT(Include files $HNAME not found)\n"
	       "      AC_MSG_ERROR(Is the %s distribution installed properly?)\n"
	       "    else\n"
	       "      AC_MSG_RESULT(Include files $HNAME found in $INCDIR)\n"
	       "      PHP_ADD_INCLUDE($INCDIR)\n"
	       "    fi\n\n",module);
	Printf(f_extra,"  done\n\n");
      }
      
      if (CPlusPlus) Printf(f_extra,
			    "  # As this is a C++ module..\n"
			    "  PHP_REQUIRE_CXX\n"
			    "  AC_CHECK_LIB(stdc++, cin)\n");
      
      if (with) {
	Printf(f_extra,"  if test \"$LIBSYMBOL\" != \"\" ; then\n"
	       "    old_LIBS=\"$LIBS\"\n"
	       "    LIBS=\"$LIBS -L$TEST_DIR/lib -lm -ldl\"\n"
	       "    AC_CHECK_LIB($LIBNAME, $LIBSYMBOL, [AC_DEFINE(HAVE_TESTLIB,1,  [ ])],\n"
	       "    [AC_MSG_ERROR(wrong test lib version or lib not found)])\n"
	       "    LIBS=\"$old_LIBS\"\n"
	       "  fi\n\n");
      }
      
      Printf(f_extra,"  AC_DEFINE(HAVE_%(upper)s, 1, [ ])\n",module);
      Printf(f_extra,"dnl  AC_DEFINE_UNQUOTED(PHP_%(upper)s_DIR, \"$%(upper)s_DIR\", [ ])\n",module,module);
      Printf(f_extra,"  PHP_EXTENSION(%s, $ext_shared)\n",module);
      
      // and thats all!
      Printf(f_extra,"fi\n");
      
      Close(f_extra);
      
      /*  CREDITS */
      f_extra = NewFile(credits, "w");
      if (!f_extra) {
	Printf(stderr,"Unable to open %s\n",credits);
	SWIG_exit(EXIT_FAILURE);
      }
      Printf(f_extra, "%s\n", module);
      Close(f_extra);
    }
  }

  /* ------------------------------------------------------------
   * top()
   * ------------------------------------------------------------ */

  virtual int top(Node *n) {

    String *filen;
    String *s_type;

#if 0  //  TODO: move to main
	if(baseException != NULL)
	{
		if(!buildExceptionGraph(n))
			return SWIG_ERROR;
	}
#endif

    /* Initialize all of the output files */
    String *outfile = Getattr(n,"outfile");
    
    /* main output file */
    f_runtime = NewFile(outfile,"w");
    if (!f_runtime) {
      Printf(stderr,"*** Can't open '%s'\n", outfile);
      SWIG_exit(EXIT_FAILURE);
    }
    
    Swig_banner(f_runtime);
    
    /* sections of the output file */
    s_init = NewString("/* init section */\n");
    s_header = NewString("/* header section */\n");
    s_wrappers = NewString("/* wrapper section */\n");
    s_type = NewString("");
    /* subsections of the init section */
    s_vinit = NewString("/* vinit subsection */\n");
    s_vdecl = NewString("/* vdecl subsection */\n");
    s_cinit = NewString("/* cinit subsection */\n");
    s_oinit = NewString("/* oinit subsection */\n");
    pragma_phpinfo = NewString("");
    
    /* Register file targets with the SWIG file handler */
    Swig_register_filebyname("runtime",f_runtime);
    Swig_register_filebyname("init",s_init);
    Swig_register_filebyname("header",s_header);
    Swig_register_filebyname("wrapper",s_wrappers);
    
    shadow_classdef = NewString("");
    shadow_code = NewString("");
    php_enum_code = NewString("");
    module_baseclass = NewString("");
    all_shadow_extra_code = NewString("");
    all_shadow_import = NewString("");
    all_shadow_baseclass = NewString("");
    
    /* Set the module name */
    module = Copy(Getattr(n,"name"));
    cap_module = NewStringf("%(upper)s",module);
    
    if(shadow) {
      realpackage = module;
      package = NewStringf("%sc", module);
    }
    
    /* Set the dlname */
    if (!dlname) {
#if defined(_WIN32) || defined(__WIN32__)
      dlname = NewStringf("%s.dll", module);
#else
      dlname = NewStringf("%s.so", module);
#endif
    }

    /* PHP module file */
    filen = NewString("");
    Printv(filen, SWIG_output_directory(), module, ".php", NIL);
    phpfilename = NewString(filen);
    
    f_phpcode = NewFile(filen, "w");
    if (!f_phpcode) {
      Printf(stderr, "*** Can't open '%s'\n", filen);
      SWIG_exit(EXIT_FAILURE);
    }
    
    Printf(f_phpcode, "<?php\n\n");
    
    Swig_banner(f_phpcode);
    
    Printf(f_phpcode,
	   "global $%s_LOADED__;\n"
	   "if ($%s_LOADED__) return;\n"
	   "$%s_LOADED__ = true;\n\n"
	   "/* if our extension has not been loaded, do what we can */\n"
	   "if (!extension_loaded(\"php_%s\")) {\n"
	   "	if (!dl(\"php_%s\")) return;\n"
	   "}\n\n", cap_module, cap_module, cap_module, module, dlname);
    
    
    /* sub-sections of the php file */
    pragma_code = NewString("");
    pragma_incl = NewString("");
    
    /* Initialize the rest of the module */
    
    f_php = NewWrapper();// wots this used for now?
    
    /* start the header section */
    Printf(s_header, php_header);
    Printf(s_header,
	   "#define SWIG_init	init%s\n\n"
	   "#define SWIG_name	\"%s\"\n"
	   /*	   "#ifdef HAVE_CONFIG_H\n"
	   "#include \"config.h\"\n"
	   "#endif\n\n"
	   */
	   "#ifdef __cplusplus\n"
	   "extern \"C\" {\n"
	   "#endif\n"
	   "#include \"php.h\"\n"
	   "#include \"php_ini.h\"\n"
	   "#include \"ext/standard/info.h\"\n"
	   "#include \"php_%s.h\"\n"
	   "#ifdef __cplusplus\n"
	   "}\n"
	   "#endif\n\n",
	   module, module, module);
    
    /* Create the .h file too */
    filen = NewString("");
    Printv(filen, SWIG_output_directory(), "php_", module, ".h", NIL);
    f_h = NewFile(filen, "w");
    if (!f_h) {
      Printf(stderr,"Unable to open %s\n", filen);
      SWIG_exit(EXIT_FAILURE);
    }
    
    Swig_banner(f_h);
    Printf(f_h, php_header);
    
    Printf(f_h, "\n\n"
	   "#ifndef PHP_%s_H\n"
	   "#define PHP_%s_H\n\n"
	   "extern zend_module_entry %s_module_entry;\n"
	   "#define phpext_%s_ptr &%s_module_entry\n\n"
	   "#ifdef PHP_WIN32\n"
	   "# define PHP_%s_API __declspec(dllexport)\n"
	   "#else\n"
	   "# define PHP_%s_API\n"
	   "#endif\n\n"
	   "PHP_MINIT_FUNCTION(%s);\n"
	   "PHP_MSHUTDOWN_FUNCTION(%s);\n"
	   "PHP_RINIT_FUNCTION(%s);\n"
	   "PHP_RSHUTDOWN_FUNCTION(%s);\n"
	   "PHP_MINFO_FUNCTION(%s);\n\n",
	   cap_module, cap_module, module, module, module, cap_module, cap_module,
	   module, module, module, module, module);
    
    /* start the function entry section */
    s_entry = NewString("/* entry subsection */\n");
    /* holds all the per-class function entry sections */
    all_cs_entry = NewString("/* class entry subsection */\n");
    cs_entry = NULL;
    
    Printf(s_entry,"/* Every non-class user visible function must have an entry here */\n");
    Printf(s_entry,"function_entry %s_functions[] = {\n", module);
    
    /* start the init section */
    if (gen_extra)
      Printf(s_init,"#ifdef COMPILE_DL_%s\n", cap_module);
    Printf(s_init,
	   "#ifdef __cplusplus\n"
	   "extern \"C\" {\n"
	   "#endif\n"
	   "ZEND_GET_MODULE(%s)\n"
	   "#ifdef __cplusplus\n"
	   "}\n"
	   "#endif\n\n",
	   module);
    if (gen_extra)
      Printf(s_init,"#endif\n\n");
    
    Printf(s_init,
	   "PHP_MSHUTDOWN_FUNCTION(%s)\n{\n"
	   "    return SUCCESS;\n"
	   "}\n",
	   module);
    
    /* We have to register the constants before they are (possibly) used
     * by the pointer typemaps. This all needs re-arranging really as
     * things are being called in the wrong order
     */
    
    Printf(s_init,"PHP_MINIT_FUNCTION(%s)\n{\n", module);
    Printf(s_init,
	   "    int i;\n"
	   "    for (i = 0; swig_types_initial[i]; i++) {\n"
	   "        swig_types[i] = SWIG_TypeRegister(swig_types_initial[i]);\n"
	   "    }\n");
    /* Emit all of the code */
    Language::top(n);

    SwigPHP5_emit_resource_registrations();    
//    Printv(s_init,s_resourcetypes,NIL);
    /* We need this after all classes written out by ::top */
    Printf(s_oinit, "CG(active_class_entry) = NULL;\n");	
    Printf(s_oinit, "/* end oinit subsection */\n");
    Printf(s_init, "%s\n", s_oinit);
    
    /* Constants generated during top call */
    // But save them for RINIT
    Printf(s_cinit, "/* end cinit subsection */\n");
    
    /* finish our init section which will have been used by class wrappers */
    Printf(s_vinit, "/* end vinit subsection */\n");
    
    Printf(s_init, "    return SUCCESS;\n");
    Printf(s_init,"}\n");
    
    // Now do REQUEST init which holds cinit and vinit
    Printf(s_init,
	   "PHP_RINIT_FUNCTION(%s)\n{\n",
	   module);
    
    Printf(s_init, "%s\n", s_cinit); 
    Clear(s_cinit);
    
    Printf(s_init, "%s\n", s_vinit);
    Clear(s_vinit);
    
    Printf(s_init,
	   "    return SUCCESS;\n"
	   "}\n");
    
    Delete(s_cinit);
    Delete(s_vinit);
    
    Printf(s_init,
	   "PHP_RSHUTDOWN_FUNCTION(%s)\n{\n"
	   "    return SUCCESS;\n"
	   "}\n",
	   module);
    
    Printf(s_init,
	   "PHP_MINFO_FUNCTION(%s)\n{\n"
	   "%s"
	   "}\n"
	   "/* end init section */\n",
	   module, pragma_phpinfo);
    
    /* Complete header file */
    
    Printf(f_h,
	   "/*If you declare any globals in php_%s.h uncomment this:\n"
	   "ZEND_BEGIN_MODULE_GLOBALS(%s)\n"
	   "ZEND_END_MODULE_GLOBALS(%s)\n"
	   "*/\n",
	   module, module, module);
    
    Printf(f_h,
	   "#ifdef ZTS\n"
	   "#define %s_D  zend_%s_globals *%s_globals\n"
	   "#define %s_DC  , %s_D\n"
	   "#define %s_C  %s_globals\n"
	   "#define %s_CC  , %s_C\n"
	   "#define %s_SG(v)  (%s_globals->v)\n"
	   "#define %s_FETCH()  zend_%s_globals *%s_globals "
	   "= ts_resource(%s_globals_id)\n"
	   "#else\n"
	   "#define %s_D\n"
	   "#define %s_DC\n"
	   "#define %s_C\n"
	   "#define %s_CC\n"
	   "#define %s_SG(v)  (%s_globals.v)\n"
	   "#define %s_FETCH()\n"
	   "#endif\n\n"
	   "#endif /* PHP_%s_H */\n",
	   cap_module, module, module, cap_module, cap_module, cap_module, module,
	   cap_module, cap_module, cap_module, module, cap_module, module, module,
	   module, cap_module, cap_module, cap_module, cap_module, cap_module, module,
	   cap_module, cap_module);
    
    Close(f_h);
    
    Printf(s_header, "%s\n\n",all_cs_entry);
    Printf(s_header, 
	   "%s"
	   "	{NULL, NULL, NULL}\n};\n\n"
	   "zend_module_entry %s_module_entry = {\n"
	   "#if ZEND_MODULE_API_NO > 20010900\n"
	   "    STANDARD_MODULE_HEADER,\n"
	   "#endif\n"
	   "    \"%s\",\n"
	   "    %s_functions,\n"
	   "    PHP_MINIT(%s),\n"
	   "    PHP_MSHUTDOWN(%s),\n"
	   "    PHP_RINIT(%s),\n"
	   "    PHP_RSHUTDOWN(%s),\n"
	   "    PHP_MINFO(%s),\n"
	   "#if ZEND_MODULE_API_NO > 20010900\n"
	   "    NO_VERSION_YET,\n"
	   "#endif\n"
	   "    STANDARD_MODULE_PROPERTIES\n"
	   "};\nzend_module_entry* SWIG_module_entry = &%s_module_entry;\n\n",
	   s_entry, module, module, module, module, module, module, module,module,module);
    
    String *type_table = NewString("");
    SwigType_emit_type_table(f_runtime,type_table);
    Printf(s_header,"%s",type_table);
    Delete(type_table);
    
    /* Oh dear, more things being called in the wrong order. This whole
     * function really needs totally redoing.
     */
    
    Printv(f_runtime, s_header, NIL);
    
    //  Wrapper_print(f_c, s_wrappers);
    Wrapper_print(f_php, s_wrappers);
    
    Printf(s_header, "/* end header section */\n");
    Printf(s_wrappers, "/* end wrapper section */\n");
    Printf(s_vdecl, "/* end vdecl subsection */\n");

	extern String* getIdFilename, *catchAllCodeFilename;
	if(getIdFilename)
		Swig_insert_file(getIdFilename, s_vdecl);
	if(catchAllCodeFilename)
		Swig_insert_file(catchAllCodeFilename, s_vdecl);

	emitThrowFunctionDefinition();

    Printv(f_runtime, s_vdecl, s_wrappers, s_init, NIL);
    Delete(s_header);
    Delete(s_wrappers);
    Delete(s_init);
    Delete(s_vdecl);
    Close(f_runtime);
    Printf(f_phpcode, "%s\n%s\n?>\n", pragma_incl, pragma_code);
    Close(f_phpcode); 
    
    create_extra_files(outfile);
    
    if(!gen_extra && gen_make)
      create_simple_make();

    
    return SWIG_OK;
  }
  
/* Just need to append function names to function table to register with
   PHP
*/

  void create_command(char *cname, char *iname) {
//    char *lower_cname = strdup(cname);
//    char *c;
    
//    for(c = lower_cname; *c != '\0'; c++) {
//      if(*c >= 'A' && *c <= 'Z')
//	*c = *c + 32;
//    }
    
    Printf(f_h, "ZEND_NAMED_FUNCTION(%s);\n", iname);
    
    // This is for the single main function_entry record
    if (! cs_entry) 
	{
		//now that we allow overloading, avoid more than one entry with the same name
		string wrapName = string(iname);
		if(methodEntries.find(wrapName) == methodEntries.end())
		{
			Printf(s_entry,
				"	ZEND_NAMED_FE(%(lower)s,\n"
				"		%s, NULL)\n", cname,iname);
			methodEntries[wrapName] = true;
		}
	}
    
//    free(lower_cname);
  }

  /* ------------------------------------------------------------
   * functionWrapper()
   * ------------------------------------------------------------ */
virtual int functionWrapper(Node *n) {
//  virtual int wrapMethod(Node *n, char* methodName) {
    char *name = GetChar(n,"name");
    char *iname = GetChar(n,"sym:name");
    SwigType *d = Getattr(n,"type");
    ParmList *l = Getattr(n,"parms");
    int newobject = (Getattr(n,"feature:new"))?1:0;
    Parm *p;
    char source[256],target[256],temp[256],argnum[32],args[32];
    int i,numopt;
    String *tm;
    Wrapper *f;
    int num_saved = (Getattr(n,"feature:new"))?1:0;
    String *cleanup, *outarg;
    bool mvr=(shadow && variable_wrapper_flag && !enum_flag);
    bool mvrset=0;

//    if (!addSymbol(iname,n)) return SWIG_ERROR;
    mvrset=(mvr && (strcmp(iname, Char(Swig_name_set(Swig_name_member(shadow_classname, name)))) == 0));

    // if shadow and variable wrapper we want to snag the main contents
    // of this function to stick in to the property handler....    
    if (mvr) { //shadow && variable_wrapper_flag && !enum_flag) {
      String *member_function_name = NewString("");
      String *php_function_name = NewString(iname);
      if(strcmp(iname, Char(Swig_name_set(Swig_name_member(shadow_classname, name)))) == 0) {
	Setattr(shadow_set_vars, php_function_name, name);
      }
      if(strcmp(iname, Char(Swig_name_get(Swig_name_member(shadow_classname, name)))) == 0) {
	Setattr(shadow_get_vars, php_function_name, name);
      }
      Putc(toupper((int )*iname), member_function_name);
      Printf(member_function_name, "%s", iname+1);

      cpp_func(Char(member_function_name), d, l, php_function_name);
     
      Delete(php_function_name);
      Delete(member_function_name);
    }
    
    outarg = cleanup = NULL;
    f 	= NewWrapper();
    numopt = 0;
    
    outarg = NewString("");
    cleanup = NewString("");
    
    // Special action for shadowing destructors under php.
    // The real destructor is the resource list destructor, this is
    // merely the thing that actually knows how to destroy...

    if (destructor) {
      String *destructorname=NewString("");
      Printf(destructorname,"_%s",Swig_name_wrapper(iname));
      Setattr(classnode,"destructor",destructorname);

      Wrapper *df = NewWrapper();
      Printf(df->def,"/* This function is designed to be called by the zend list destructors to typecast and do the actual destruction */\n"
                     "void %s(zend_rsrc_list_entry *rsrc, const char *type_name TSRMLS_DC) {\n",destructorname);

      Wrapper_add_localv(df, "value", "swig_object_wrapper *value=(swig_object_wrapper *) rsrc->ptr", NIL);
      Wrapper_add_localv(df, "ptr", "void *ptr=value->ptr", NIL);
      Wrapper_add_localv(df, "newobject", "int newobject=value->newobject", NIL);
      // Magic spell nicked from further down.
      emit_args(d, l, df);
      emit_attach_parmmaps(l,f);

      // Get type of first arg, thing to be destructed
      // Skip ignored arguments
      {
        p=l;
        //while (Getattr(p,"tmap:ignore")) {p = Getattr(p,"tmap:ignore:next");}
	while (checkAttribute(p,"tmap:in:numinputs","0")) {
	  p = Getattr(p,"tmap:in:next");
	}

        SwigType *pt = Getattr(p,"type");

        Printf(df->code,
             "  efree(value);\n"
             "  if (! newobject) return; /* can't delete it! */\n"
	     "  SWIG_ZTS_ConvertResourceData(ptr,rsrc->type,type_name,(void **) &arg1,SWIGTYPE%s TSRMLS_CC);\n"
	     "  if (! arg1) zend_error(E_ERROR, \"%s resource already free'd\");\n"
	     ,SwigType_manglestr(pt), shadow_classname);
      }

      emit_action(n,df);

      if(disposeCode)
      {
          Replaceall(df->code, "delete arg1", disposeCode);
      }

      Printf(df->code,"}\n");

      Wrapper_print(df,s_wrappers);
    }

    if (mvr) { // do prop[gs]et header
      if (mvrset) Printf(f->def, "static int _wrap_%s(zend_property_reference *property_reference, pval *value) {\n",iname);
      else Printf(f->def, "static pval _wrap_%s(zend_property_reference *property_reference) {\n",iname);
    } else { // regular header

	string methodname = string(iname);
	if(Getattr(n,"sym:overloaded"))
	{
		vector<Parameter*>* parms = new vector<Parameter*>;
		int argcount = 0;
		for(Parm* pm = l; pm != NULL; pm = nextSibling(pm))
		{
            String* typeStr = Getattr(pm, "type");
			int type = SwigType_type(typeStr);
			String* pname = Getattr(pm, "name");
			if(pname != NULL && !Strcmp(pname, "self"))
				continue;
			parms->push_back(new Parameter(type, string(Char(typeStr))));
			argcount++;
		}

		string qualifiedName = Char(Swig_name_wrapper(iname));
		map<string, vector<OverloadedFunc*>*>::const_iterator it = overloads.find(qualifiedName);
		vector<OverloadedFunc*>* lstOver = it != overloads.end()? it->second: NULL;
		if(lstOver == NULL)
		{
			lstOver = new vector<OverloadedFunc*>;
			overloads[qualifiedName] = lstOver;
		}
		char idx[10];
		sprintf(idx, "%d", lstOver->size() + 1);
		string mangledName = string(Char(Swig_name_wrapper(iname))) + "_" + idx;
		OverloadedFunc* fi = new OverloadedFunc(mangledName, argcount, parms);
		lstOver->push_back(fi);

		methodname = mangledName;
	}
	else
		methodname = string(Char(Swig_name_wrapper(iname)));


      create_command(iname, Char(Swig_name_wrapper(iname)));
      //Printv(f->def, "ZEND_NAMED_FUNCTION(" , Swig_name_wrapper(methodName), ") {\n", NIL);
	  Printv(f->def, "ZEND_NAMED_FUNCTION(" , methodname.c_str(), ") {\n", NIL);
    }

    emit_args(d, l, f);
    /* Attach standard typemaps */

    emit_attach_parmmaps(l,f);

    int num_arguments = emit_num_arguments(l);
    int num_required  = emit_num_required(l);
    numopt = num_arguments - num_required;
    
    // we do +1 because we are going to push in this_ptr as arg0 if present
    // or do we need to?

    sprintf(args, "%s[%d]", "zval **args", num_arguments+1); 

    Wrapper_add_local(f, "args",args);    
    Wrapper_add_localv(f, "argbase", "int argbase=0", NIL);
    // This generated code may be called 
    // 1) as an object method, or
    // 2) as a class-method/function (without a "this_ptr")
    // Option (1) has "this_ptr" for "this", option (2) needs it as
    // first parameter
    // NOTE: possible we ignore this_ptr as a param for native constructor

    if (native_constructor) {
      if (native_constructor==NATIVE_CONSTRUCTOR) Printf(f->code, "/* NATIVE Constructor */\nint self_constructor=1;\n");
      else Printf(f->code, "/* ALTERNATIVE Constructor */\n");
    }

    if (mvr && ! mvrset) {
      Wrapper_add_local(f, "_return_value", "zval _return_value");
      Wrapper_add_local(f, "return_value", "zval *return_value=&_return_value");
    };

    // only let this_ptr count as arg[-1] if we are not a constructor
    // if we are a constructor and this_ptr is null we are called as a class
    // method and can make one of us
    if (! mvr && native_constructor==0) Printf(f->code,
				      "if (this_ptr && this_ptr->type==IS_OBJECT) {\n"
				      "  /* fake this_ptr as first arg (till we can work out how to do it better */\n"
				      "  argbase++;\n"
				      "}\n");
    
    // I'd like to write out:
    //"  //args[argbase++]=&this_ptr;\n"
    // but zend_get_parameters_array_ex can't then be told to leave
    // the first slot alone, so we have to check whether or not to access
    // this_ptr explicitly in each case where we normally just read args[]
    
    if(numopt > 0) { // membervariable wrappers do not have optional args
      Wrapper_add_local(f, "arg_count", "int arg_count");
      Printf(f->code,
	     "arg_count = ZEND_NUM_ARGS();\n"
	     "if(arg_count<(%d-argbase) || arg_count>(%d-argbase))\n"
	     "\tWRONG_PARAM_COUNT;\n\n",
	     num_required, num_arguments);
      
      /* Verified args, retrieve them... */
      Printf(f->code,
	     "if(zend_get_parameters_array_ex(arg_count-argbase,args)!=SUCCESS)"
	     "\n\t\tWRONG_PARAM_COUNT;\n\n");
      
    } else if (!mvr) {
      Printf(f->code, 
	     "if(((ZEND_NUM_ARGS() + argbase )!= %d) || (zend_get_parameters_array_ex(%d-argbase, args)"
	     "!= SUCCESS)) {\n"
	     "WRONG_PARAM_COUNT;\n}\n\n",
	     num_arguments, num_arguments);
    }
    
    /* Now convert from php to C variables */
    // At this point, argcount if used is the number of deliberatly passed args
    // not including this_ptr even if it is used.
    // It means error messages may be out by argbase with error
    // reports.  We can either take argbase into account when raising 
    // errors, or find a better way of dealing with _thisptr
    // I would like, if objects are wrapped, to assume _thisptr is always
    // _this and the and not the first argument
    // This may mean looking at Lang::memberfunctionhandler

    for (i = 0, p = l; i < num_arguments; i++) {
      /* Skip ignored arguments */
      //while (Getattr(p,"tmap:ignore")) { p = Getattr(p,"tmap:ignore:next");}
      while (checkAttribute(p,"tmap:in:numinputs","0")) {
        p = Getattr(p,"tmap:in:next");
      }

      SwigType *pt = Getattr(p,"type");

 
      if (mvr) { // do we assert that numargs=2, that i<2
        if (i==0) sprintf(source,"&(property_reference->object)");
        else sprintf(source,"&value");
      } else {
        // Do we fake this_ptr as arg0, or just possibly shift other args by 1 if we did fake?
        if (i==0) sprintf(source, "((%d<argbase)?(&this_ptr):(args[%d-argbase]))", i, i);
        else sprintf(source, "args[%d-argbase]", i); } 
        sprintf(target, "%s", Char(Getattr(p,"lname")));
        sprintf(argnum, "%d", i+1);
      
        /* Check if optional */
      
        if(i>= (num_required))
	  Printf(f->code,"\tif(arg_count > %d) {\n", i);
      
        Setattr(p,"emit:input", source);
      
        if ((tm = Getattr(p,"tmap:in"))) {
	  Replaceall(tm,"$target",target);
	  Replaceall(tm,"$source",source);
	  Replaceall(tm,"$input", source);
	  Printf(f->code,"%s\n",tm);
	  p = Getattr(p,"tmap:in:next");
	  if (i >= num_required) {
	    Printf(f->code,"}\n");
	  }
	  continue;
        } else {
	  Printf(stderr,"%s : Line %d, Unable to use type %s as a function argument.\n", input_file, line_number, SwigType_str(pt,0));
      }
      if (i>= num_required)
	Printf(f->code,"\t}\n");
    }
    
    /* Insert constraint checking code */
    for (p = l; p;) {
      if ((tm = Getattr(p,"tmap:check"))) {
	Replaceall(tm,"$target",Getattr(p,"lname"));
	Printv(f->code,tm,"\n",NIL);
	p = Getattr(p,"tmap:check:next");
      } else {
	p = nextSibling(p);
      }
    }

    /* Insert cleanup code */
    for (i = 0, p = l; p; i++) {
      if ((tm = Getattr(p,"tmap:freearg"))) {
	Replaceall(tm,"$source",Getattr(p,"lname"));
	Printv(cleanup,tm,"\n",NIL);
	p = Getattr(p,"tmap:freearg:next");
      } else {
	p = nextSibling(p);
      }
    }
    
    /* Insert argument output code */
    num_saved = 0;
    for (i=0,p = l; p;i++) {
      if ((tm = Getattr(p,"tmap:argout"))) {
	Replaceall(tm,"$source",Getattr(p,"lname"));
	Replaceall(tm,"$input",Getattr(p,"lname"));
	Replaceall(tm,"$target","return_value");
	Replaceall(tm,"$result","return_value");

	String *in = Getattr(p,"emit:input");
	if (in) {
	  sprintf(temp,"_saved[%d]", num_saved);
	  Replaceall(tm,"$arg",temp);
	  Printf(f->code,"_saved[%d] = %s;\n", num_saved, in);
	  num_saved++;
	}
	Printv(outarg,tm,"\n",NIL);
	p = Getattr(p,"tmap:argout:next");
      } else {
	p = nextSibling(p);
      }
    }

    // These are saved for argout again...    
    if(num_saved) {
      sprintf(temp, "_saved[%d]",num_saved);
      // Used to be zval *, perhaps above we should use * %s
      Wrapper_add_localv(f,"_saved","zval **",temp,NIL);
    }
    
	  if(baseException != NULL)
	  {
		Printf(f->code, "try\n");
		Printf(f->code, "{\n");
	  }

    /* emit function call*/
    if (destructor) {
      // If it is a registered resource (and it always should be)
      // then destroy it the resource way
      
        Printf(f->code,
            "if (this_ptr->type==IS_OBJECT) {\n"
            "zval ** _cPtr;\n"
            "if (zend_hash_find(HASH_OF(this_ptr),\"_cPtr\",sizeof(\"_cPtr\"),(void**)&_cPtr)==SUCCESS) {\n"
            "if ((*_cPtr)->type==IS_RESOURCE) {\n"
            "zend_list_delete(Z_LVAL_PP(_cPtr));\n"
            "} } }\n",name,name);
 
      // but leave the old way in for as long as we accept strings as swig objects
      emit_action(n,f);

      // Never delete directly.  Always let the resource destruction mechanism
      // do this.  Do not emit any code but put a placeholder here just in case
      // this change breaks something else.
      if(disposeCode)
      {
          Replaceall(f->code, "delete arg1", "/* delete arg1; //Resource destructor should handle this*/" /*disposeCode*/);
      }

      Printf(f->code,"/*}*/\n");
    } else {
      emit_action(n,f);
    }
    


    if((tm = Swig_typemap_lookup((char*)"out",d,iname,(char*)"result",(char*)"result",(char*)"return_value",0))) {
      Replaceall(tm, "$input", "result");
      Replaceall(tm, "$source", "result");
      Replaceall(tm, "$target", "return_value");
      Replaceall(tm, "$result", "return_value");
      Replaceall(tm,"$owner", newobject ? "1" : "0");

	  bool returnObject = is_shadow(d) && SwigType_type(d) != T_ARRAY && native_constructor!= NATIVE_CONSTRUCTOR;

	  if(returnObject && enableRuntimeTypeChecking) 
	  {
	    //TODO see if we can check if this base class has derived class. If no derived class
		// we could generate the usual code (currently in tm) without trying to instantiate a derived class wrapper
   		String *tpname = NewString("");
		SwigToPhpType(d, iname, tpname, shadow);

        Printf(f->code, "ClsInfo* clsInfo=NULL;if(result!=NULL){clsInfo=classMap[getClassId(result)];\n");
		Printf(f->code, "if(clsInfo == NULL)\nzend_error(E_ERROR,\"The class %s (or one of its derived classes) has no runtime type id\");\n", tpname);
        // All classes are "new" as far as PHP is concerned and require eventual destruction
        Printf(f->code, "else SWIG_SetPointerZval(return_value, (void *)result, clsInfo->pTypeInfo, 1);} else{ZVAL_LONG(return_value,0); return;} \n");
	  }
	  else
	  {
		Printf(f->code, "%s\n", tm);
	  }
      // are we returning a wrapable object?
      // I don't know if this test is comlete, I nicked it
      if(is_shadow(d) && (SwigType_type(d) != T_ARRAY)) {
	Printf(f->code,"/* Wrap this return value */\n");
	if (native_constructor==NATIVE_CONSTRUCTOR) {
	  Printf(f->code, "if (this_ptr) {\n/* NATIVE Constructor, use this_ptr */\n");
	  Printf(f->code,"zval *_cPtr; MAKE_STD_ZVAL(_cPtr);\n"
		 "*_cPtr = *return_value;\n"
		 "INIT_ZVAL(*return_value);\n"
		 "add_property_zval(this_ptr,\"_cPtr\",_cPtr);\n"
		 "} else if (! this_ptr) ",shadow_classname);
	}
	{ // THIS CODE only really needs writing out if the object to be returned
	  // Is being shadow-wrap-thingied
	  Printf(f->code, "{\n/* ALTERNATIVE Constructor, make an object wrapper */\n");
	  // Make object 
	  String *shadowrettype = NewString("");
	  SwigToPhpType(d, iname, shadowrettype, shadow);
	  
	  Printf(f->code, 
		 "zval *obj, *_cPtr;\n"
		 "MAKE_STD_ZVAL(obj);\n"
		 "MAKE_STD_ZVAL(_cPtr);\n"
		 "*_cPtr = *return_value;\n"
		 "INIT_ZVAL(*return_value);\n");
	  
	  if (! shadow) {
	    Printf(f->code,
		   "*return_value=*_cPtr;\n");
	  } else {
	    if(returnObject && enableRuntimeTypeChecking)
			Printf(f->code, 
				"object_init_ex(obj,clsInfo? clsInfo->pCls: ptr_ce_swig_%s);\n"
			"add_property_zval(obj,\"_cPtr\",_cPtr);\n"
			"*return_value=*obj;\n",
			shadowrettype);
		else
			Printf(f->code, 
				"object_init_ex(obj,ptr_ce_swig_%s);\n"
				"add_property_zval(obj,\"_cPtr\",_cPtr);\n"
				"*return_value=*obj;\n",
				shadowrettype);
	  }
	  Printf(f->code, "}\n");
	}
      } // end of if-shadow lark
    } else {
      Printf(stderr,"%s: Line %d, Unable to use return type %s in function %s.\n", input_file, line_number, SwigType_str(d,0), name);
    }
    
    if(outarg)
      Printv(f->code,outarg,NIL);
    
    if(cleanup)
      Printv(f->code,cleanup,NIL);
    
    // Whats this bit for?
    if((tm = Swig_typemap_lookup((char*)"ret",d,iname,(char *)"result", (char*)"result",(char*)"",0))) {
      Printf(f->code,"%s\n", tm);
    }
    
    Replaceall(f->code,"$cleanup",cleanup);
    Replaceall(f->code,"$symname",iname);

	if(baseException != NULL)
	{
		Printf(f->code, "}\n");
		Printf(f->code, "catch(%s *e)\n", baseException);
		Printf(f->code, "{\n");
		Printf(f->code, "\tThrowPHPExceptionWrapper(e TSRMLS_CC);\n");
        Printf(f->code, "} catch(...) { onCatchAll(\"%s\" TSRMLS_CC); }\n", iname);
	}

    if (mvr) {
      if (! mvrset) Printf(f->code,"return _return_value;\n");    
      else Printf(f->code,"return SUCCESS;\n");
    }

    Printf(f->code, "}\n");    
    Wrapper_print(f,s_wrappers);

    return SWIG_OK;
  }
  
  /* ------------------------------------------------------------
   * variableWrapper()
   * ------------------------------------------------------------ */

  virtual int OLDvariableWrapper(Node *n) {
    char *name = GetChar(n,"name");
    char *iname = GetChar(n,"sym:name");
    SwigType *t = Getattr(n,"type");
    String *tm;

    if (!addSymbol(iname,n)) return SWIG_ERROR;

    SwigType_remember(t);

    /* First link C variables to PHP */

    tm = Swig_typemap_lookup_new("varinit", n, name, 0);
    if(tm) {
      Replaceall(tm, "$target", name);
      Printf(s_vinit, "%s\n", tm);
    } else {
      Printf(stderr,"%s: Line %d, Unable to link with type %s\n", 
	     input_file, line_number, SwigType_str(t,0), name);
    }

    /* Now generate PHP -> C sync blocks */
    tm = Swig_typemap_lookup_new("varin", n, name, 0);
    /*
  if(tm) {
	Replaceall(tm, "$symname", iname);
	Printf(f_c->code, "%s\n", tm);
  } else {
	Printf(stderr,"%s: Line %d, Unable to link with type %s\n", 
		input_file, line_number, SwigType_str(t, 0), name);
  }
*/
  /* Now generate C -> PHP sync blocks */
/*
  if(!Getattr(n,"feature:immutable")) {

	tm = Swig_typemap_lookup_new("varout", n, name, 0);
	if(tm) {
		Replaceall(tm, "$symname", iname);
		Printf(f_php->code, "%s\n", tm);
	} else {
		Printf(stderr,"%s: Line %d, Unable to link with type %s\n", 
			input_file, line_number, SwigType_str(t, 0), name);
	}
  }
*/
    return SWIG_OK;
  }

  /* ------------------------------------------------------------
   * constantWrapper()
   * ------------------------------------------------------------ */
  
  virtual int constantWrapper(Node *n) {
    char *name = GetChar(n,"name");
    char *iname = GetChar(n,"sym:name");
    SwigType *type = Getattr(n,"type");
    char *value = GetChar(n,"value");

    if (!addSymbol(iname,n)) return SWIG_ERROR;

    String *rval;
    String *tm;

    SwigType_remember(type);

    switch(SwigType_type(type)) {
    case T_STRING:
      rval = NewStringf("\"%s\"", value);
      break;
    case T_CHAR:
      rval = NewStringf("\'%s\'", value);
      break;
    default:
      rval = NewString(value);
    }

    if((tm = Swig_typemap_lookup_new("consttab", n, name, 0))) {
      Replaceall(tm, "$source", value);
      Replaceall(tm, "$target", name);
      Replaceall(tm, "$value", value);
      Printf(s_oinit, "%s\n", tm);
    }
    return SWIG_OK;
  }

  /*
   * PHP5::pragma()
   *
   * Pragma directive.
   *
   * %pragma(php5) code="String"         # Includes a string in the .php file
   * %pragma(php5) include="file.pl"     # Includes a file in the .php file
   */
  
  virtual int pragmaDirective(Node *n) {
    if (!ImportMode) {
      String *lang = Getattr(n,"lang");
      String *type = Getattr(n,"name");
      String *value = Getattr(n,"value");

      if (Strcmp(lang,"php") == 0) {
	
	if (Strcmp(type, "code") == 0) {
	  if (value)
	    Printf(pragma_code, "%s\n", value);
	} else if (Strcmp(type, "include") == 0) {
	  if (value)
	    Printf(pragma_incl, "include \"%s\";\n", value);
	} else if (Strcmp(type, "phpinfo") == 0) {
	  if (value)
	    Printf(pragma_phpinfo, "%s\n", value);
	} else {
	  Printf(stderr, "%s : Line %d. Unrecognized pragma.\n",
		 input_file, line_number);
	}
      }
    }
    return Language::pragmaDirective(n);
  }

  /* ------------------------------------------------------------
   * classDeclaration()
   * ------------------------------------------------------------ */

  virtual int classDeclaration(Node *n) {
    String *symname = Getattr(n,"sym:name");
    Setattr(n,"php:proxy",symname);
    return Language::classDeclaration(n);
  }

  int typeToPhpType(int type)
  {
		switch(type)
		{
		case T_BOOL:
			return IS_BOOL;
		case T_CHAR:
		case T_SCHAR:
		case T_UCHAR:
		case T_SHORT:
		case T_USHORT:
		case T_INT:
		case T_UINT:
		case T_LONG:
		case T_ULONG:
		case T_LONGLONG:
		case T_ULONGLONG:
			return IS_LONG;
		case T_FLOAT:
		case T_DOUBLE:
			return IS_DOUBLE;
		case T_STRING:
			return IS_STRING;
		case T_USER:
		case T_POINTER:
			return IS_OBJECT;
		default:
			return IS_NULL;
		}
  }

  string getClassNameFromType(string& type)
  {
      const char* sep = strrchr(type.c_str(), '.');
	  if(sep == NULL)
		return string("");
	  return string(sep + 1);
  }

  string getZendClassNameFromType(string& type)
  {
      string retVal = getClassNameFromType(type);
      for (int i = 0; i < retVal.size(); i++)
          retVal[i] = ::tolower(retVal[i]);
      return retVal;
  }
  
  /* ------------------------------------------------------------
   * classHandler()
   * ------------------------------------------------------------ */

  virtual int classHandler(Node *n) {
    constructors=0;
    //SwigType *t = Getattr(n, "classtype");
    if(class_name) free(class_name);
    class_name = Swig_copy_string(GetChar(n, "name"));
    // String *use_class_name=SwigType_manglestr(SwigType_ltype(t));
      List *baselist = Getattr(n, "bases");
      Iterator base;

	  overloads.clear();

    if(shadow) {
      char *rename = GetChar(n, "sym:name");
      if (!addSymbol(rename,n)) return SWIG_ERROR;
      shadow_classname = Swig_copy_string(rename);
      cs_entry = NewString("");
      Printf(cs_entry,"/* Function entries for %s */\n"
	     "static zend_function_entry %s_functions[] = {\n"
	     ,shadow_classname, shadow_classname);

      if(Strcmp(shadow_classname, module) == 0) {
	Printf(stderr, "class name cannot be equal to module name: %s\n", shadow_classname);
	SWIG_exit(1);
      }

      Clear(shadow_classdef);
      Clear(shadow_code);

      have_default_constructor = 0;
      shadow_enum_code = NewString("");
      this_shadow_baseclass = NewString("");
      this_shadow_multinherit = NewString("");
      this_shadow_extra_code = NewString("");
      this_shadow_import = NewString("");

      shadow_get_vars = NewHash();
      shadow_set_vars = NewHash();

      /* Deal with inheritance */
      List *baselist = Getattr(n, "bases");

      if(baselist) {
        int class_count = 0;
	Iterator base = First(baselist);
	while(base.item && Getattr(base.item,"feature:ignore")) base = Next(base);

	if (base.item && is_shadow(Getattr(base.item, "name"))) {
	  class_count++;
	  Printf(this_shadow_baseclass, "%s", Getattr(base.item, "name"));
	}

	if (base.item) for(base = Next(base); base.item; base = Next(base)) {
	  if (Getattr(base.item,"feature:ignore")) continue;
	  if(is_shadow(Getattr(base.item, "name"))) {
	    class_count++;
	    Printf(this_shadow_multinherit, "%s ", Getattr(base.item, "name"));
	  }
	}

	if(class_count > 1) Printf(stderr, "Error: %s inherits from multiple base classes(%s %s). SWIG doesn;t support multiple inheritance.\n", shadow_classname, base.item, this_shadow_multinherit);
      }

      /* Write out class init code */
      Printf(s_vdecl,"static zend_class_entry ce_swig_%s;\n",shadow_classname);
      Printf(s_vdecl,"static zend_class_entry* ptr_ce_swig_%s=NULL;\n",shadow_classname);
    }

    classnode=n;
    Language::classHandler(n);
    classnode=0;

	for(map<string, vector<OverloadedFunc*>*>::const_iterator it = overloads.begin(); it != overloads.end(); it++)
	{
		string qualifiedName = it->first;
		vector<OverloadedFunc*>* listOver = it->second;

		Printv(s_wrappers, "ZEND_NAMED_FUNCTION(", qualifiedName.c_str(), ")\n{\n", NIL);
		Printf(s_wrappers, "\tint numArgs = ZEND_NUM_ARGS();\n");

		map<int, vector<OverloadedFunc*>*> identicalArgCount;
		list<OverloadedFunc*> simpleFuncs;

		bool isIdenticalCount = false;
		int maxArgs = -1;
		int count1 = listOver->size();
		for(vector<OverloadedFunc*>::const_iterator it1 = listOver->begin(); it1 != listOver->end(); it1++)
		{
			OverloadedFunc* of = *it1;
			if(of->argcount > maxArgs)
				maxArgs = of->argcount;
			bool uniqueNumber = true;
			for(vector<OverloadedFunc*>::const_iterator it2 = listOver->begin(); it2 != listOver->end(); it2++)
			{
				OverloadedFunc* of2 = *it2;
				if(of2 != of && of->argcount == of2->argcount)
				{
					vector<OverloadedFunc*>* sameArgs = identicalArgCount[of->argcount];
					if(sameArgs == NULL)
					{
						sameArgs = new vector<OverloadedFunc*>;
						identicalArgCount[of->argcount] = sameArgs;
					}
					else
						isIdenticalCount = true;
					sameArgs->push_back(of);
					uniqueNumber = false;
					break;
				}
			}
			if(uniqueNumber)
				simpleFuncs.push_back(of);
		}

		if(isIdenticalCount)
		{
			Printf(s_wrappers, "\tzval **args[%d];\n", maxArgs);
			Printf(s_wrappers, "\tif(zend_get_parameters_array_ex(numArgs, args)!= SUCCESS)\n");
			Printf(s_wrappers, "\t\tWRONG_PARAM_COUNT;\n\n");
		}
		Printf(s_wrappers, "\tswitch(numArgs)\n\t{\n");

		//first the simple functions
		for(list<OverloadedFunc*>::const_iterator it = simpleFuncs.begin(); it != simpleFuncs.end(); it++)
		{
			OverloadedFunc* of = *it;
			Printf(s_wrappers, "\tcase %d:\n\t\t%s(INTERNAL_FUNCTION_PARAM_PASSTHRU);\n\t\tbreak;\n", of->argcount, of->mangledName.c_str());
		}


		//go through methods with the same number of arguments, by groups
		for(map<int, vector<OverloadedFunc*>*>::const_iterator it = identicalArgCount.begin(); it != identicalArgCount.end(); it++)
		{
			int argcount = it->first;
			vector<OverloadedFunc*>* overloads = it->second;
			Printf(s_wrappers, "\tcase %d:\n\t{\n", argcount);
				
			if(argcount == 0)
			{
				Printf(stderr, "Two overloads with no parameters (%s)", (*overloads->begin())->mangledName.c_str());
				return SWIG_ERROR;
			}
			for(int arg = 0; arg < argcount; arg ++)
			{
				vector<OverloadedFunc*> objectArgsFunc;
				for(vector<OverloadedFunc*>::const_iterator it1 = overloads->begin(); it1 != overloads->end(); it1++)
				{
					OverloadedFunc* m = *it1;
					if(m->done)
						continue;
					Parameter* p = m->parms->at(arg);
                    int type = p->type;
					int phpType = typeToPhpType(type);
					bool sameType = false;
					int count = 0;
					bool multipleObjectParam = false;
					map<OverloadedFunc*, bool> done;
					for(vector<OverloadedFunc*>::const_iterator it2 = overloads->begin(); it2 != overloads->end(); it2++, count ++)
					{
						OverloadedFunc* m2 = *it2;
						if(m != m2)
						{
							Parameter* p2 = m2->parms->at(arg);
                            int type2 = p2->type;
							int phpType2 = typeToPhpType(type2);
							if(phpType == phpType2)
							{
								if(phpType == IS_OBJECT)
								{
									//type are the same if classes are the same
                                    if(getClassNameFromType(p->typeStr) == getClassNameFromType(p2->typeStr))
										sameType = true;
									multipleObjectParam = true;
								}
								else
									sameType = true;
								break;
							}
						}
					}
					if(!sameType)
					{
						switch(phpType)
						{
						case IS_NULL:
							Printf(stderr, "Unknown overloaded type for argument %d (%s)", arg, (*overloads->begin())->mangledName.c_str());
							return SWIG_ERROR;
						case IS_LONG:
							Printf(s_wrappers, "\t\tif((*args[%d])->type == IS_LONG)\n\t\t{\n\t\t\t%s(INTERNAL_FUNCTION_PARAM_PASSTHRU);\n\t\t\treturn;\n\t\t}\n", arg, m->mangledName.c_str());
							break;
						case IS_DOUBLE:
							Printf(s_wrappers, "\t\tif((*args[%d])->type == IS_DOUBLE)\n\t\t{\n\t\t\t%s(INTERNAL_FUNCTION_PARAM_PASSTHRU);\n\t\t\treturn;\n\t\t}\n", arg, m->mangledName.c_str());
							break;
						case IS_STRING:
							Printf(s_wrappers, "\t\tif((*args[%d])->type == IS_STRING)\n\t\t{\n\t\t\t%s(INTERNAL_FUNCTION_PARAM_PASSTHRU);\n\t\t\treturn;\n\t\t}\n", arg, m->mangledName.c_str());
							break;
						case IS_OBJECT:
						{
							if(!multipleObjectParam)
								Printf(s_wrappers, "\t\tif((*args[%d])->type == IS_OBJECT)\n\t\t{\n\t\t\t%s(INTERNAL_FUNCTION_PARAM_PASSTHRU);\n\t\t\treturn;\n\t\t}\n", arg, m->mangledName.c_str());
							else
								objectArgsFunc.push_back(m);
							break;
						}
						case IS_BOOL:
							Printf(s_wrappers, "\t\tif((*args[%d])->type == IS_BOOL)\n\t\t{\n\t\t\t%s(INTERNAL_FUNCTION_PARAM_PASSTHRU);\n\t\t\treturn;\n\t\t}\n", arg, m->mangledName.c_str());
							break;
						}
						m->done = true;

					}
					else
					{
						if(arg == argcount - 1)
						{
							Printf(stderr, "Couldn't distinguish between to overloads based on type of parameters (%s)", qualifiedName.c_str());
							return SWIG_ERROR;
						}
					} 
				}

				for(int idx = 0; idx < (int)overloads->size(); )
				{
					if(overloads->at(idx)->done)
					{
						overloads->erase(overloads->begin() + idx);
					}
					else
						++idx;
				}

				if(objectArgsFunc.size())
				{
					Printf(s_wrappers, "\t\tif((*args[%d])->type == IS_OBJECT)\n", arg);
					Printf(s_wrappers, "\t\t{\n");
					Printf(s_wrappers, "\t\t\tzval ** _cPtr;\n");
					Printf(s_wrappers, "\t\t\tif (zend_hash_find(HASH_OF(*args[%d]),\"_cPtr\",sizeof(\"_cPtr\"),(void**)&_cPtr)!=SUCCESS)\n", arg);
					Printf(s_wrappers, "\t\t\t\tzend_error(E_ERROR, \"Error in getting C++ pointer (%s)\");\n", qualifiedName.c_str());
					Printf(s_wrappers, "\t\t\tvoid *p;\n");
					Printf(s_wrappers, "\t\t\tint type;\n");
					Printf(s_wrappers, "\t\t\tswig_object_wrapper *value = (swig_object_wrapper *) zend_list_find((*_cPtr)->value.lval,&type);\n");
					Printf(s_wrappers, "\t\t\tp = value->ptr;\n");
					Printf(s_wrappers, "\t\t\tint clsId = getClassId(p);\n");
					Printf(s_wrappers, "\t\t\tClsInfo* clsInfo = classMap[getClassId(p)];\n");
					Printf(s_wrappers, "\t\t\tif(clsInfo == NULL)\n");
					Printf(s_wrappers, "\t\t\t\tzend_error(E_ERROR, \"Unknown C++ pointer type (passed to %s)\");\n", qualifiedName.c_str());
					Printf(s_wrappers, "\t\t\telse\n");
					Printf(s_wrappers, "\t\t\t{\n");

					bool first = false;
					for(vector<OverloadedFunc*>::const_iterator it = objectArgsFunc.begin(); it != objectArgsFunc.end(); it++)
					{
						OverloadedFunc* m = *it;
                        Printf(s_wrappers, "\t\t\t\tif(!strcmp(clsInfo->pTypeInfo->name, \"_p_%s\"))\n", getClassNameFromType(m->parms->at(arg)->typeStr).c_str());
						Printf(s_wrappers, "\t\t\t\t{\n");
						Printf(s_wrappers, "\t\t\t\t\t%s(INTERNAL_FUNCTION_PARAM_PASSTHRU);\n", m->mangledName.c_str());
						Printf(s_wrappers, "\t\t\t\t\treturn;\n");
						Printf(s_wrappers, "\t\t\t\t}\n");

                        Printf(s_wrappers, "\t\t\t\telse if(clsInfo->pCls->parent && !strcmp(clsInfo->pCls->parent->name, \"%s\"))\n", getZendClassNameFromType(m->parms->at(arg)->typeStr).c_str());
						Printf(s_wrappers, "\t\t\t\t{\n");
						Printf(s_wrappers, "\t\t\t\t\t%s(INTERNAL_FUNCTION_PARAM_PASSTHRU);\n", m->mangledName.c_str());
						Printf(s_wrappers, "\t\t\t\t\treturn;\n");
						Printf(s_wrappers, "\t\t\t\t}\n");
						
					}
					Printf(s_wrappers, "\t\t\t}\n");
					Printf(s_wrappers, "\t\t}\n");
				}

			}

			Printf(s_wrappers, "\t\tzend_error(E_ERROR, \"Arguments passed did not match any overload for method %s\");\n", qualifiedName.c_str());

            Printf(s_wrappers, "\t\tbreak;\n\t}\n");
		}

		//default case, the number of argument does not match any overloaded methods
		Printf(s_wrappers, "\tdefault:\n\t\tzend_error(E_ERROR, \"No overload of %s take %%d arguments)\", numArgs);\n\t\tbreak;\n", qualifiedName.c_str());

		Printf(s_wrappers, "\t}\n");	//end of switch
		Printf(s_wrappers, "}\n\n");	//end of function
	}

    if(shadow) {
      String	  *s_propget=NewString("");
      String	  *s_propset=NewString("");
      List *baselist = Getattr(n, "bases");


      // If no constructor was generated (abstract class) we had better
      // generate a constructor that raises an error about instantiating
      // abstract classes
      if (! constructors || Getattr(n,"abstract")) {
        // have to write out fake constructor which raises an error when called
        abstractConstructorHandler(n);
      }

      Printf(s_oinit,"/* Define class %s */\n"
	     "INIT_OVERLOADED_CLASS_ENTRY(ce_swig_%s,\"%(lower)s\",%s_functions,"
	     "NULL,NULL,NULL);\n",
	     shadow_classname,shadow_classname,shadow_classname,
	     shadow_classname);

      // Save class in class table
      if (baselist) base=First(baselist);
      else base.item=NULL;
      while(base.item && Getattr(base.item,"feature:ignore")) base = Next(base);

      if (base.item) {
        Printf(s_oinit,"if (! (ptr_ce_swig_%s=zend_register_internal_class_ex(&ce_swig_%s,ptr_ce_swig_%s,NULL TSRMLS_CC))) zend_error(E_ERROR,\"Error registering wrapper for class %s\");\n",
          shadow_classname,shadow_classname,GetChar(base.item, "sym:name"), shadow_classname);
      } else {
        Printf(s_oinit,"if (! (ptr_ce_swig_%s=zend_register_internal_class_ex(&ce_swig_%s,NULL,NULL TSRMLS_CC))) zend_error(E_ERROR,\"Error registering wrapper for class %s\");\n",
          shadow_classname,shadow_classname, shadow_classname);
      }
	  if(enableRuntimeTypeChecking)
	  {
		map<string, int>::const_iterator iter = clsIds.find(Char(class_name));
		if(iter != clsIds.end())
		{
			Printf(s_oinit,"classMap[%d] = new ClsInfo(SWIGTYPE_p_%s, ptr_ce_swig_%s);\n", iter->second, class_name, class_name);
		}
	  }

      Printf(s_oinit,"\n");

	  // Write the enum initialisation code in a static block
      // These are all the enums defined withing the c++ class.

      // PHP Needs to handle shadow enums properly still
      if(strlen(Char(shadow_enum_code)) != 0 ) Printv(f_phpcode, "{\n /* enum */\n", shadow_enum_code, " }\n", NIL);

      free(shadow_classname);
      shadow_classname = NULL;

      Delete(shadow_enum_code); shadow_enum_code = NULL;
      Delete(this_shadow_baseclass); this_shadow_baseclass = NULL;
      Delete(this_shadow_extra_code); this_shadow_extra_code = NULL;
      Delete(this_shadow_import); this_shadow_import = NULL;
      Delete(shadow_set_vars); shadow_set_vars = NULL;
      Delete(shadow_get_vars); shadow_get_vars = NULL;
      Delete(this_shadow_multinherit); this_shadow_multinherit = NULL;

      Printf(all_cs_entry,"%s	{ NULL, NULL, NULL}\n};\n",cs_entry);
      //??delete cs_entry;
      cs_entry=NULL;
    }
    return SWIG_OK;
  }

  /* ------------------------------------------------------------
   * memberfunctionHandler()
   * ------------------------------------------------------------ */

  virtual int memberfunctionHandler(Node *n) {
    char *name = GetChar(n, "name");
    char *iname = GetChar(n, "sym:name");
    SwigType *t = Getattr(n, "type");
    ParmList *l = Getattr(n, "parms");

    this->Language::memberfunctionHandler(n);

    if(shadow) {
      char *realname = iname ? iname : name;
      String *php_function_name = Swig_name_member(shadow_classname, realname);

      cpp_func(iname, t, l, realname, php_function_name);

    }
    return SWIG_OK;
  }

  /* ------------------------------------------------------------
   * membervariableHandler()
   * ------------------------------------------------------------ */

  virtual int membervariableHandler(Node *n) {

    wrapping_member = 1;
    variable_wrapper_flag = 1;
    Language::membervariableHandler(n);
    wrapping_member = 0;
    variable_wrapper_flag = 0;

    return SWIG_OK;
  }

  /* ------------------------------------------------------------
   * staticmemberfunctionHandler()
   * ------------------------------------------------------------ */

  virtual int staticmemberfunctionHandler(Node *n) {
    char *name = GetChar(n, "name");
    char *iname = GetChar(n, "sym:name");

    Language::staticmemberfunctionHandler(n);

    if(shadow) {
      String *symname = Getattr(n, "sym:name");
      static_flag = 1;      
      char *realname = iname ? iname : name;
      String *php_function_name = Swig_name_member(shadow_classname, realname);
      cpp_func(Char(symname), Getattr(n, "type"), Getattr(n, "parms"), symname, php_function_name);
      static_flag = 0;
    }

    return SWIG_OK;
  }

  /* ------------------------------------------------------------
   * staticmembervariableHandler()
   * ------------------------------------------------------------ */

  virtual int staticmembervariableHandler(Node *n) {
    SwigType *d = Getattr(n, "type");
    char *iname = GetChar(n, "sym:name");
    char *name = GetChar(n, "name");
    String *static_name = NewStringf("%s::%s", class_name, name);
//    String *use_class_name=SwigType_manglestr(SwigType_ltype(t));
    Wrapper *f;

  /* A temporary(!) hack for static member variables.
   * Php currently supports class functions, but not class variables.
   * Until it does, we convert a class variable to a class function
   * that returns the current value of the variable. E.g.
   *
   * class Example {
   * 	public:
   * 		static int ncount;
   * };
   *
   * would be available in php as Example::ncount() 
   */
    static_flag = 1;
    if(Getattr(n,"feature:immutable")) {
      const_flag = 1;
    }
    cpp_func(iname, d, 0, iname);
    static_flag = 0;

    create_command(iname, Char(Swig_name_wrapper(iname)));

    f = NewWrapper();

    Printv(f->def, "ZEND_NAMED_FUNCTION(", Swig_name_wrapper(iname), ") {\n", NIL);

	/* If a argument is given we set the variable. Then we return
	 * the current value
	*/

    Printf(f->code, 
	   "zval **args[1];\n"
	   "int argcount;\n\n"
	   "argcount = ZEND_NUM_ARGS();\n"
	   "if(argcount > %d) WRONG_PARAM_COUNT;\n\n", (const_flag? 0 : 1));

    if(!const_flag) {
      Printf(f->code, "if(argcount) {\n");

      Printf(f->code, "if(zend_get_parameters_array_ex(argcount, args) != SUCCESS) WRONG_PARAM_COUNT;\n");

      switch(SwigType_type(d)) {
      case T_BOOL:
      case T_INT:
      case T_SHORT:
      case T_LONG:
      case T_SCHAR:
      case T_UINT:
      case T_USHORT:
      case T_ULONG:
      case T_UCHAR:
	Printf(f->code, 
	       "convert_to_long_ex(args[0]);\n"
	       "%s = Z_LVAL_PP(args[0]);\n", static_name);
	break;
      case T_CHAR:
	Printf(f->code, 
	       "convert_to_string_ex(args[0]);\n"
	       "%s = estrdup(Z_STRVAL(args[0]));\n", static_name);
	break;
      case T_DOUBLE:
      case T_FLOAT:
	Printf(f->code, 
	       "convert_to_double_ex(args[0]);\n"
	       "%s = Z_DVAL_PP(args[0]);\n", 
	       static_name);
	break;
      case T_VOID:
	break;
      case T_USER:
	Printf(f->code, "convert_to_string_ex(args[0]);\n");
	get_pointer(Char(iname), (char*)"variable", (char*)"args[0]", Char(static_name), d, f->code, (char *)"RETURN_FALSE");
	break;
      case T_POINTER:
      case T_ARRAY:
      case T_REFERENCE:
	Printf(f->code, "convert_to_string_ex(args[0]);\n");
	get_pointer(Char(iname), (char*)"variable", (char*)"args[0]", Char(static_name), d, f->code, (char*)"RETURN_FALSE");
	break;
      default:
	Printf(stderr,"%s : Line %d, Unable to use type %s as a class variable.\n", input_file, line_number, SwigType_str(d,0));
	break;
      }
		
      Printf(f->code, "}\n\n");
	
    } /* end of const_flag */

    switch(SwigType_type(d)) {
    case T_BOOL:
    case T_INT:
    case T_SHORT:
    case T_LONG:
    case T_SCHAR:
    case T_UINT:
    case T_USHORT:
    case T_ULONG:
    case T_UCHAR:
      Printf(f->code, 
	     "RETURN_LONG(%s);\n", static_name);
      break;
    case T_DOUBLE:
    case T_FLOAT:
      Printf(f->code, 
	     "RETURN_DOUBLE(%s);\n", static_name);
      break;
    case T_CHAR:
      Printf(f->code,
	     "{\nchar ctemp[2];\n"
	     "ctemp[0] = %s;\n"
	     "ctemp[1] = 0;\n"
	     "RETURN_STRING(ctemp, 1);\n}\n",
	     static_name);
      break;

    case T_USER:
    case T_POINTER:
      Printf(f->code, 
	     "SWIG_SetPointerZval(return_value, (void *)%s, "
	     "SWIGTYPE%s);\n", static_name, SwigType_manglestr(d));
      break;
    case  T_STRING:
      Printf(f->code, "RETURN_STRING(%s, 1);\n", static_name);
      break;
    }


    Printf(f->code, "}\n");

    const_flag = 0;

    Wrapper_print(f, s_wrappers);

    return SWIG_OK;
  }

  
  void SwigToPhpType(SwigType *t, String_or_char *pname, String* php_type, int shadow_flag) {
    char *ptype = 0;

    if(shadow_flag)
      ptype = PhpTypeFromTypemap((char*)"pstype", t, pname,(char*)"");
    if(!ptype)
      ptype = PhpTypeFromTypemap((char*)"ptype",t,pname,(char*)"");


    if(ptype) {
      Printf(php_type, ptype);
      free(ptype);
    }
    else {
      /* Map type here */
      switch(SwigType_type(t)) {
      case T_CHAR:
      case T_SCHAR:
      case T_UCHAR:
      case T_SHORT:
      case T_USHORT:
      case T_INT:
      case T_UINT:
      case T_LONG:
      case T_ULONG:
      case T_FLOAT:
      case T_DOUBLE:
      case T_BOOL:
      case T_STRING:
      case T_VOID:
	Printf(php_type, "");
	break;
      case T_POINTER:
      case T_REFERENCE:
      case T_USER:
	if(shadow_flag && is_shadow(t))
	  Printf(php_type, Char(is_shadow(t)));
	else
	  Printf(php_type, "");
	break;
      case T_ARRAY:
				/* TODO */
	break;
      default:
	Printf(stderr, "SwigToPhpType: unhandled data type: %s\n", SwigType_str(t,0));
	break;
      }
    }
  }


  char *PhpTypeFromTypemap(char *op, SwigType *t, String_or_char *pname, String_or_char *lname) {
    String *tms;
    char bigbuf[1024];
    char *tm;
    char *c = bigbuf;
    if(!(tms = Swig_typemap_lookup(op, t, pname, lname, (char*)"", (char*)"", NULL))) return NULL;

    tm = Char(tms);
    while(*tm && (isspace(*tm) || *tm == '{')) tm++;
    while(*tm && *tm != '}') *c++ = *tm++;
    *c='\0';
    return Swig_copy_string(bigbuf);
  }

  int abstractConstructorHandler(Node *n) {
    char *iname = GetChar(n, "sym:name");
    if (shadow) {
      Wrapper *f;
      f   = NewWrapper();

      // constructor header
      if (cs_entry) Printf(cs_entry,
			   "	ZEND_NAMED_FE(%(lower)s,\n"
			   "		_wrap_new_%s, NULL)\n", iname,iname);
      // now constructor body
      Printf(f_h, "ZEND_NAMED_FUNCTION(_wrap_new_%s);\n",iname);
      Printf(f->def, "ZEND_NAMED_FUNCTION(_wrap_new_%s) {\n"
                     "zend_error(E_ERROR,\"Cannot create swig object type: %s as the underlying object is abstract\");\n"
                     "}\n\n", iname, iname);
      Wrapper_print(f,s_wrappers);
      DelWrapper(f);
    }
    return SWIG_OK;
  }
  /* ------------------------------------------------------------
   * constructorHandler()
   * ------------------------------------------------------------ */

  virtual int constructorHandler(Node *n) {
    char *iname = GetChar(n, "sym:name");

    if (shadow) native_constructor = (strcmp(iname, shadow_classname) == 0)?\
		  NATIVE_CONSTRUCTOR:ALTERNATIVE_CONSTRUCTOR;
    else native_constructor=0;
    constructors++;
    Language::constructorHandler(n);

    if(shadow) {
      // But we also need one per wrapped-class
		//now that we allow overloading, avoid more than one entry with the same name
		string wrapName = string(iname);
		if(methodEntries.find(wrapName) == methodEntries.end())
		{
			if (cs_entry) Printf(cs_entry,
					"	ZEND_NAMED_FE(%(lower)s,\n"
					"		_wrap_new_%s, NULL)\n", iname,iname);
			methodEntries[wrapName] = true;
		}
    }

    native_constructor = 0;
    return SWIG_OK;
  }

  /* ------------------------------------------------------------
   * destructorHandler()
   * ------------------------------------------------------------ */

  virtual int destructorHandler(Node *n) {
    char *iname = GetChar(n, "sym:name");

    destructor=1;
    Language::destructorHandler(n);
    destructor=0;

    // Users should never access destructors.  PHP's reference
    // counting mechanism  will automatically call this code once
    // the object is no longer referenced.  This is a bit of a risk since
    // we do expose __destruct.  If we do not document it, we should be ok.
    //
    // Create a __destruct for the class which removes the resource.
    // Destruction is two phase.  _wrap_delete removes the resource
    // from the internally managed list and __wrap_delete is called by Zend
    // when it removes the resource.  __wrap_delete in turn decrements
    // the C++ reference count.  This mechanism is refcount based.  Our exposed
    // objects will be deleted as soon as they are no longer referenced.
    // PHP does not wait to "garbage collect"
    //
    // Eventually, we may be able to replace the object+resource reference
    // mechanism to use just objects.  However, that will require a signficant
    // amount of rework to the pre-existing resource based infrastructure.
    if(shadow) {
      // But we also need one per wrapped-class
      if (cs_entry) Printf(cs_entry,
			   "	ZEND_NAMED_FE(__destruct,\n"
			   "		_wrap_delete_%s, NULL)\n", iname,iname);
    }
    
    return SWIG_OK;
  }
  
  /* ------------------------------------------------------------
   * memberconstantHandler()
   * ------------------------------------------------------------ */

  virtual int memberconstantHandler(Node *n) {
    wrapping_member = 1;
    Language::memberconstantHandler(n);
    wrapping_member = 0;
    return SWIG_OK;
  }

  // This method is quite stale and ought to be factored out
  void cpp_func(char *iname, SwigType *t, ParmList *l, String *php_function_name, String *handler_name = NULL) {
    if(!shadow) return;

	// if they didn't provide a handler name, use the realname
    if (! handler_name) handler_name=php_function_name;

    if(l) {
      if(SwigType_type(Getattr(l, "type")) == T_VOID) {
	l = nextSibling(l);
      }
    }

    // But we also need one per wrapped-class
    //        Printf(f_h, "x ZEND_NAMED_FUNCTION(%s);\n", Swig_name_wrapper(handler_name));
    if (cs_entry && !(variable_wrapper_flag && shadow)) 
	{
		//now that we allow overloading, avoid more than one entry with the same name
		string wrapName = string(Char(Swig_name_wrapper(handler_name)));
		if(methodEntries.find(wrapName) == methodEntries.end())
		{
			Printf(cs_entry,
				"	ZEND_NAMED_FE(%(lower)s,\n"
				"		%s, NULL)\n", php_function_name,wrapName.c_str());
			methodEntries[wrapName] = true;
		}
	}

    if(variable_wrapper_flag)  { return; }

    /* Workaround to overcome Getignore(p) not working - p does not always
	 * have the Getignore attribute set. Noticeable when cpp_func is called
	 * from cpp_member_func()
	*/

    Wrapper *f = NewWrapper();
    emit_args(NULL, l, f);
    DelWrapper(f);

    /*Workaround end */

  }

	int emitExCount;


};  /* class PHP5 */

/* -----------------------------------------------------------------------------
 * swig_php()    - Instantiate module
 * ----------------------------------------------------------------------------- */

static PHP5 *maininstance=0;

// We use this function to be able to write out zend_register_list_destructor_ex
// lines for most things in the type table
// NOTE: it's a function NOT A PHP5::METHOD
extern "C"
void typetrace5(SwigType *ty, String *mangled, String *clientdata) {
  Node *class_node;
  if (!zend_types) {
    zend_types=NewHash();
  }
  // we want to know if the type which reduced to this has a constructor
  if ((class_node=maininstance->classLookup(ty))) {
    if (! Getattr(zend_types,mangled)) {
      // OK it may have been set before by a different SwigType but it would
      // have had the same underlying class node I think
      // - it is certainly required not to have different originating class
      // nodes for the same SwigType
      Setattr(zend_types,mangled,class_node);
    }
  } else { // a non-class pointer
    Setattr(zend_types,mangled,NOTCLASS);
  }
  if (r_prevtracefunc) (*r_prevtracefunc)(ty, mangled, (String *) clientdata);
}

extern "C" Language *
swig_php5(void) {
  maininstance=new PHP5();
  if (! r_prevtracefunc) {
    r_prevtracefunc=SwigType_remember_trace(typetrace5);
  } else {
    Printf(stderr,"php5 Typetrace vector already saved!\n");
    assert(0);
  }
  return maininstance;
}

