/* -----------------------------------------------------------------------------
 * main.cxx
 *
 *     Main entry point to the SWIG core.
 *
 * Author(s) : David Beazley (beazley@cs.uchicago.edu)
 *
 * Copyright (C) 1998-2000.  The University of Chicago
 * Copyright (C) 1995-1998.  The University of Utah and The Regents of the
 *                           University of California.
 *
 * See the file LICENSE for information on usage and redistribution.
 * ----------------------------------------------------------------------------- */

char cvsroot_main_cxx[] = "$Header: /cvsroot/SWIG/Source/Modules/main.cxx,v 1.27 2003/12/16 03:43:51 beazley Exp $";

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "swigmod.h"
#ifndef MACSWIG
#include "swigconfig.h"
#endif

#include "swigwarn.h"

extern "C" {
#include "preprocessor.h"
}

#include "ExceptionClass.h"

#include <ctype.h>
#include <string>
#include <map>
#include <list>

using namespace std;

// Global variables

    char       LibDir[512];                      // Library directory
    Language  *lang;                            // Language method
    int        CPlusPlus = 0;
    int        Extend = 0;                      // Extend flag
    int        ForceExtern = 0;                 // Force extern mode
    int        GenerateDefault = 1;             // Generate default constructors
    char      *Config = 0;
    int        NoInclude = 0;
    int        Verbose = 0;
    int        NoExtern = 0;
    int        NoExcept = 0;
    char      *SwigLib;
	map<string, int> clsIds;
	map<string, ExceptionClass*> exmap;
	extern "C" bool		enableRuntimeTypeChecking = false;	// C linkage because it's accessed from C code
	ExceptionClass* rootException = NULL;
	String     *baseException = 0;
	String     *disposeCode = 0;
    String     *rethrowCode = 0;
	String	   *getIdFilename = 0;
	String	   *catchAllCodeFilename = 0;
	bool       noConstants = false;


extern "C" {
extern String  *ModuleName;
}

static char *usage = (char*)"\
\nGeneral Options\n\
	 -baseexception <name>  - Name of the exception base class\n\
     -c++            - Enable C++ processing\n\
	 -clsiddata <member> - Data member used as class identifier\n\
	 -clsidcode <file> - Name of the file implementing getClassId()\n\
	 -catchallcode <file> - Name of the file implementing onCatchAll()\n\
     -co             - Check a file out of the SWIG library\n\
     -dispose <code> - Code to call to replace 'delete arg1'\n\
     -dirprot        - Turn on wrapping of protected members for director classes\n\
     -D<symbol>      - Define a symbol <symbol> (for conditional compilation)\n\
     -E              - Preprocess only, does not generate wrapper code\n\
     -fcompact       - Compile in compact mode\n\
     -fvirtual       - Compile in virtual elimination mode\n\
     -help           - This output\n\
     -I<dir>         - Look for SWIG files in <dir>\n\
     -ignoremissing  - Ignore missing include files\n\
     -importall      - Follow all #include statements as imports\n\
     -includeall     - Follow all #include statements\n\
     -l<ifile>       - Include SWIG library file <ifile>\n\
     -lib <folder>   - absolute or relative library folder\n\
     -M              - List all dependencies \n\
     -MM             - List dependencies, but omit files in SWIG library\n\
     -makedefault    - Create default constructors/destructors (the default)\n\
     -module <name>  - Set module name to <name>\n\
     -noconstants    - Turn off enum and constant wrapping \n\
     -nocontract     - Turn off contract checking \n\
     -nodefault      - Do not generate constructors/destructors\n\
     -nodirprot      - Do not wrap director protected members\n\
     -noexcept       - Do not wrap exception specifiers\n"
"     -noextern       - Do not generate extern declarations\n\
     -noruntime      - Do not include SWIG runtime code\n\
     -o <outfile>    - Set name of the output file to <outfile>\n\
     -outdir <dir>   - Set language specific files output directory\n\
     -runtime        - Make the runtime support code globally visible.\n\
     -small          - Compile in virtual elimination & compact mode\n\
     -swiglib        - Report location of SWIG library and exit\n\
     -v              - Run in verbose mode\n\
     -version        - Print SWIG version number\n\
     -Wall           - Enable all warning messages\n\
     -Wallkw         - Enable warning messages for all the languages keywords\n\
     -Werror         - Force to treat the warnings as errors\n\
     -w<n>           - Suppress warning number <n>\n\
\n";

// Local variables
static int     freeze = 0;
static String  *lang_config = 0;
static char    *cpp_extension = (char *) "cxx";
static String  *outdir = 0;
static String  *clsIdMember = 0;

bool buildExceptionGraph(Node* n)
{
	Node* tmpNode = firstChild(n);
	while(tmpNode) 
	{
		char *tag = Char(nodeType(tmpNode));
		if(!strcmp(tag, "include") && Getattr(tmpNode, "module"))
		{
			//first pass, collect all classes
			tmpNode = firstChild(tmpNode);
			while(tmpNode) 
			{
				if(!Strcmp(nodeType(tmpNode), "class"))
				{
					char* parentName = NULL;
					ExceptionClass* classObj = new ExceptionClass();
					classObj->name = Char(Getattr(tmpNode, "name"));
					exmap[classObj->name] = classObj;

					List *baselist = Getattr(tmpNode, "bases");

					if(baselist) 
					{
						Iterator base = First(baselist);
						while(base.item)
						{
							if(!classObj->parentName.empty())
							{
								Printf(stderr, "class %s already has a base class [%s]", classObj->name.c_str(), classObj->parentName.c_str());
								return false;
							}
							classObj->parentName = Char(Getattr(base.item, "name"));
							base = Next(base);
						}
					}
				}
				tmpNode = nextSibling(tmpNode);
			}
			break;
		}

		tmpNode = nextSibling(tmpNode);
	}


    //find the root exception, 
    bool bFoundRootException = false;
    map<string, ExceptionClass*>::const_iterator iterRootException = exmap.find(Char(baseException));
    if (iterRootException != exmap.end())
    {
        rootException = iterRootException->second;
        bFoundRootException = true;
    }
    else
    {
        rootException = new ExceptionClass();
        rootException->name = Char(baseException);
    }
    
	//second pass, link the classes together to form the graph
	map<string, ExceptionClass*>::const_iterator iter = exmap.begin();
	while(iter != exmap.end())
	{
		ExceptionClass* exClass = iter->second;
		if(Strcmp(baseException, exClass->name.c_str()))
		{
			map<string, ExceptionClass*>::const_iterator iterBase = exmap.find(exClass->parentName);
			if(iterBase != exmap.end())
    			iterBase->second->addChild(exClass);

            if(!bFoundRootException && !Strcmp(baseException, exClass->parentName.c_str()))
                rootException->addChild(exClass);
		}
		iter ++;
	}
	//check that the root exception class was found
	if(rootException == NULL)
	{
		Printf(stderr, "Root class exception %s was not found", baseException);
		return false;
	}
	return true;
}
  
// -----------------------------------------------------------------------------
// check_suffix(char *name)
//
// Checks the suffix of a file to see if we should emit extern declarations.
// -----------------------------------------------------------------------------

int
check_suffix(char *name) {
  char *c;
  if (!name) return 0;
  c = Swig_file_suffix(name);
  if ((strcmp(c,".c") == 0) ||
      (strcmp(c,".C") == 0) ||
      (strcmp(c,".cc") == 0) ||
      (strcmp(c,".cxx") == 0) ||
      (strcmp(c,".c++") == 0) ||
      (strcmp(c,".cpp") == 0)) {
    return 1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
// install_opts(int argc, char *argv[])
// Install all command line options as preprocessor symbols
// ----------------------------------------------------------------------------- 

static void
install_opts(int argc, char *argv[]) {
  int i;
  int noopt = 0;
  char *c;
  for (i = 1; i < (argc-1); i++) {
    if (argv[i]) {
      if ((*argv[i] == '-') && (!isupper(*(argv[i]+1)))) {
	String *opt = NewStringf("SWIGOPT%(upper)s", argv[i]);
	Replaceall(opt,"-","_");
	c = Char(opt);
	noopt = 0;
	while (*c) {
	  if (!(isalnum(*c) || (*c == '_'))) {
	    noopt = 1;
	    break;
	  }
	  c++;
	}
	if (((i+1) < (argc-1)) && (argv[i+1]) && (*argv[i+1] != '-')) {
	  Printf(opt," %s", argv[i+1]);
	  i++;
	} else {
	  Printf(opt," 1");
	}
	if (!noopt) {
	  /*	  Printf(stdout,"%s\n", opt); */
	  Preprocessor_define(opt, 0);
	}
      }
    }
  }
}

// -----------------------------------------------------------------------------
// Sets the output directory for language specific (proxy) files if not set and 
// adds trailing file separator if necessary.
// ----------------------------------------------------------------------------- 

static void set_outdir(const String *c_wrapper_file_dir) {

    // Add file delimiter if not present in output directory name
    if (outdir && Len(outdir) != 0) {
        const char* outd = Char(outdir);
        if (strcmp(outd + strlen(outd) - strlen(SWIG_FILE_DELIMETER), SWIG_FILE_DELIMETER) != 0)
            Printv(outdir, SWIG_FILE_DELIMETER, NIL);
    }
    // Use the C wrapper file's directory if the output directory has not been set by user
    if (!outdir)
        outdir = NewString(c_wrapper_file_dir);
}

//-----------------------------------------------------------------
// main()
//
// Main program.    Initializes the files and starts the parser.
//-----------------------------------------------------------------

/* This function sets the name of the configuration file */

void SWIG_config_file(const String_or_char *filename) {
  lang_config = NewString(filename);
}

void SWIG_library_directory(const char *filename) {
  strcpy(LibDir,filename);
}

// Returns the directory for generating language specific files (non C/C++ files)
const String *SWIG_output_directory() {
    assert(outdir);
    return outdir;
}

void SWIG_config_cppext(const char *ext) {
  cpp_extension = (char *) ext;
}

extern  "C" Node *Swig_cparse(File *);
extern  "C" void  Swig_cparse_cplusplus(int);
extern  "C" void  Swig_cparse_debug_templates(int);
extern  void Wrapper_virtual_elimination_mode_set(int);
extern  void Wrapper_director_protected_mode_set(int);

extern void Swig_contracts(Node *n);
extern void Swig_contract_mode_set(int flag);
extern int  Swig_contract_mode_get();

static int simpleIntegerExpression(char* strvalue)
{
    //so far, accept only + operator, no parenthesis
    //if need be, we'll implement here a real expression parser
    char expr[2048];
    strncpy(expr, strvalue, sizeof(expr)); expr[sizeof(expr)-1] = '\0';
    char* token = strtok(expr, "+");
    int exprValue = 0;
    while(token)
    {
        exprValue += atoi(token);
        token = strtok(NULL, "+");
    }
    return exprValue;
}

static void buildClassIdMap(Hash *classes, String* clsIdMember)
{
	if(!classes || !clsIdMember)
		return;
	for (Iterator ki = First(classes); ki.key; ki = Next(ki)) 
	{
		Node* nclass = ki.item;
		for(Node* child = firstChild(nclass); child != NULL; child = nextSibling(child))
		{
			char* name = Char(Getattr(child, "name"));
			if(name && !Strcmp(name, clsIdMember))
			{
                char* strvalue = Char(Getattr(child, "value"));
                if(strvalue)
                {
				    int value = simpleIntegerExpression(strvalue);
				    clsIds[Char(Getattr(nclass, "name"))] = value;
                }
			}
		}
	}
	//we verify that no id is duplicated
	map<int, string> ids;
	for(map<string, int>::const_iterator it = clsIds.begin(); it != clsIds.end(); it++)
	{
		map<int, string>::const_iterator it1 = ids.find(it->second);
		if(it1 != ids.end())
		{
			printf("\nDuplicate ID: %d in classes: %s and %s\n\n", it->second, it->first.c_str(), it1->second.c_str());
			SWIG_exit(EXIT_FAILURE);
		}
		ids[it->second] = it->first;
	}

	if(clsIds.size() == 0)
		Printf(stderr,"\nWarning: -clsiddata was specified but no classes where found with class ids.\n\n");

}

int SWIG_main(int argc, char *argv[], Language *l) {
  int    i;
  char   *c;
  char    temp[512];
  char   *outfile_name = 0;
  int     help = 0;
  int     checkout = 0;
  int     cpp_only = 0;
  int     tm_debug = 0;
  char   *includefiles[256];
  int     includecount = 0;
  extern  int check_suffix(char *);
  int     dump_tags = 0;
  int     dump_tree = 0;
  int     browse = 0;
  int     dump_typedef = 0;
  int     dump_classes = 0;
  int     werror = 0;
  int     depend = 0;
  int     memory_debug = 0;
  int     allkw = 0;

  DOH    *libfiles = 0;
  DOH    *cpps = 0 ;
  extern void Swig_browser(Node *n, int);
  extern void Swig_default_allocators(Node *n);
  extern void Swig_process_types(Node *n);


  /* Initialize the SWIG core */
  Swig_init();
  
  /* Suppress warning messages for private inheritance, preprocessor evaluation, 
     might be abstract, and overloaded const */

  Swig_warnfilter("202,309,403,512,321",1);

  // Initialize the preprocessor
  Preprocessor_init();

  lang = l;

  // Set up some default symbols (available in both SWIG interface files
  // and C files)

  Preprocessor_define((DOH *) "SWIG 1", 0);
  Preprocessor_define((DOH *) "__STDC__", 0);
#ifdef MACSWIG
  Preprocessor_define((DOH *) "SWIGMAC 1", 0);
#endif
#ifdef SWIGWIN32
  Preprocessor_define((DOH *) "SWIGWIN32 1", 0);
#endif

  // Set the SWIG version value in format 0xAABBCC from package version expected to be in format A.B.C
  String *package_version = NewString(PACKAGE_VERSION);
  char *token = strtok(Char(package_version), ".");
  String *vers = NewString("SWIG_VERSION 0x");
  int count = 0;
  while (token) {
    int len = strlen(token);
    assert(len == 1 || len == 2);
    Printf(vers, "%s%s", (len == 1) ? "0" : "", token);
    token = strtok(NULL, ".");
    count++;
  }
  Delete(package_version);
  assert(count == 3); // Check version format is correct

  /* Turn on contracts */

  Swig_contract_mode_set(1);
  Preprocessor_define(vers,0);

  /* Turn on director protected mode */
  Wrapper_director_protected_mode_set(0);


    // Check for SWIG_LIB environment variable

    if ((c = getenv("SWIG_LIB")) == (char *) 0) {
    #if defined(_WIN32)
        char buf[MAX_PATH];
        char *p, *p1;
        if (GetModuleFileName(0, buf, MAX_PATH) == 0
            || (p = strrchr(buf, '\\')) == 0
            || (*p = 0)
            || (p1 = strrchr(buf, '\\')) == 0
            ) {
        Printf(stderr, "Warning: Could not determine SWIG library location. Assuming " SWIG_LIB "\n");
        sprintf(LibDir,"%s",SWIG_LIB);    // Build up search paths
        } else {
        strcpy(p1+1, "Lib");
        strcpy(LibDir, buf);
        }
    #else
        sprintf(LibDir,"%s",SWIG_LIB);    // Build up search paths
    #endif                                        
    } else {
        strcpy(LibDir,c);
    }

  libfiles = NewList();

  /* Check for SWIG_FEATURES environment variable */
  if ((c = getenv("SWIG_FEATURES"))) {
    while (*c!='\0') {
      while ((*c)==' ') {
	c++;
      }
      i = 0;
      while ((*c!='\0') && (*c!=' ')) {
	temp[i] = *c;
	c++; i++;
      }
      temp[i]='\0';
      
      if (strcmp(temp, "-fcompact") == 0) {
	Wrapper_compact_print_mode_set(1);
      } else if (strcmp(temp, "-fvirtual") == 0) {
	Wrapper_virtual_elimination_mode_set(1);
      } else if (strcmp(temp,"-dirprot") == 0) {
	  Wrapper_director_protected_mode_set(1);
      } else if (strcmp(temp,"-nodirprot") == 0) {
	  Wrapper_director_protected_mode_set(0);
      } else if (strcmp(temp, "-small") == 0) {
	Wrapper_compact_print_mode_set(1);
	Wrapper_virtual_elimination_mode_set(1);
      }
    }
  }
  
  // Get options
  for (i = 1; i < argc; i++) {
      if (argv[i]) {
	  if (strncmp(argv[i],"-I",2) == 0) {
	    // Add a new directory search path
	    includefiles[includecount++] = Swig_copy_string(argv[i]+2);
	    Swig_mark_arg(i);
	  } else if (strncmp(argv[i],"-D",2) == 0) {
	    DOH *d = NewString(argv[i]+2);
	    Replace(d,(char*)"=",(char*)" ", DOH_REPLACE_ANY | DOH_REPLACE_FIRST);
	    Preprocessor_define((DOH *) d,0);
	    // Create a symbol
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-E") == 0) {
	    cpp_only = 1;
	    Swig_mark_arg(i);
	  } else if ((strcmp(argv[i],"-verbose") == 0) ||
		     (strcmp(argv[i],"-v") == 0)) {
	      Verbose = 1;
	      Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-c++") == 0) {
	      CPlusPlus=1;
	      Preprocessor_define((DOH *) "__cplusplus 1", 0);
	      Swig_cparse_cplusplus(1);
	      Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-clsiddata") == 0) {
	      Swig_mark_arg(i);
	      if (argv[i+1]) {
		clsIdMember = Swig_copy_string(argv[i+1]);
		Swig_mark_arg(i+1);
		i++;
	      } else {
		Swig_arg_error();
	      }
	  } else if (strcmp(argv[i],"-clsidcode") == 0) {
	      Swig_mark_arg(i);
	      if (argv[i+1]) {
		getIdFilename = Swig_copy_string(argv[i+1]);
		Swig_mark_arg(i+1);
		i++;
	      } else {
		Swig_arg_error();
	      }
	  } else if (strcmp(argv[i],"-catchallcode") == 0) {
	      Swig_mark_arg(i);
	      if (argv[i+1]) {
		catchAllCodeFilename = Swig_copy_string(argv[i+1]);
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
		} else if(strcmp(argv[i], "-dispose") == 0) {
			if (argv[i+1]) {
				disposeCode = NewString(argv[i+1]);
				Swig_mark_arg(i);
				Swig_mark_arg(i+1);
				i++;
			} else {
				Swig_arg_error();
			}
		} else if(strcmp(argv[i], "-rethrow") == 0) {
			if (argv[i+1]) {
				rethrowCode = NewString(argv[i+1]);
				Swig_mark_arg(i);
				Swig_mark_arg(i+1);
				i++;
			} else {
				Swig_arg_error();
			}
		} else if(strcmp(argv[i], "-lib") == 0) {
			if (argv[i+1]) {
                //override libdir
                strcpy(LibDir, argv[i+1]);
				Swig_mark_arg(i);
				Swig_mark_arg(i+1);
				i++;
			} else {
				Swig_arg_error();
			}
	  } else if (strcmp(argv[i],"-fcompact") == 0) {
	    Wrapper_compact_print_mode_set(1);
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-fvirtual") == 0) {
	    Wrapper_virtual_elimination_mode_set(1);
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-dirprot") == 0) {
	    Wrapper_director_protected_mode_set(1);
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-nodirprot") == 0) {
	    Wrapper_director_protected_mode_set(0);
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-small") == 0) {
	    Wrapper_compact_print_mode_set(1);
	    Wrapper_virtual_elimination_mode_set(1);
	    Swig_mark_arg(i);
	  } else if ((strcmp(argv[i],"-noruntime") == 0) || (strcmp(argv[i],"-c") == 0)) {
	      NoInclude=1;
	      Preprocessor_define((DOH *) "SWIG_NOINCLUDE 1", 0);
	      Swig_mark_arg(i);
	      if (strcmp(argv[i],"-c") == 0) {
		Swig_warning(WARN_DEPRECATED_OPTC, "SWIG",1, "-c command line option is deprecated. Use -noruntime instead.\n");
	      }
	  } else if ((strcmp(argv[i],"-runtime") == 0)) {
	    Preprocessor_define((String *) "SWIG_RUNTIME_MODE 1", 0);
	    Swig_mark_arg(i);
          } else if ((strcmp(argv[i],"-make_default") == 0) || (strcmp(argv[i],"-makedefault") == 0)) {
	    GenerateDefault = 1;
	    Swig_mark_arg(i);
          } else if ((strcmp(argv[i],"-no_default") == 0) || (strcmp(argv[i],"-nodefault") == 0)) {
	    GenerateDefault = 0;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-noconstants") == 0) {
	    noConstants = true;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-noexcept") == 0) {
	    NoExcept = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-noextern") == 0) {
	    NoExtern = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-show_templates") == 0) {
	    Swig_cparse_debug_templates(1);
	    Swig_mark_arg(i);
          } else if (strcmp(argv[i],"-swiglib") == 0) {
	    printf("%s\n", LibDir);
	    SWIG_exit (EXIT_SUCCESS);
	  } else if (strcmp(argv[i],"-o") == 0) {
	      Swig_mark_arg(i);
	      if (argv[i+1]) {
		outfile_name = Swig_copy_string(argv[i+1]);
		Swig_mark_arg(i+1);
		i++;
	      } else {
		Swig_arg_error();
	      }
	  } else if (strcmp(argv[i],"-version") == 0) {
 	      fprintf(stderr,"\nSWIG Version %s\n", PACKAGE_VERSION);
	      fprintf(stderr,"Copyright (c) 1995-1998\n");
	      fprintf(stderr,"University of Utah and the Regents of the University of California\n");
              fprintf(stderr,"Copyright (c) 1998-2003\n");
	      fprintf(stderr,"University of Chicago\n");
	      fprintf(stderr,"Compiled with %s [%s]\n", SWIG_CXX, SWIG_PLATFORM);
	      fprintf(stderr,"\nPlease see %s for reporting bugs and further information\n", PACKAGE_BUGREPORT);
	      SWIG_exit (EXIT_SUCCESS);
	  } else if (strncmp(argv[i],"-l",2) == 0) {
	    // Add a new directory search path
	    Append(libfiles,argv[i]+2);
	    Swig_mark_arg(i);
          } else if (strcmp(argv[i],"-co") == 0) {
	    checkout = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-freeze") == 0) {
	    freeze = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-includeall") == 0) {
	    Preprocessor_include_all(1);
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-importall") == 0) {
	    Preprocessor_import_all(1);
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-ignoremissing") == 0) {
	    Preprocessor_ignore_missing(1);
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-tm_debug") == 0) {
	    tm_debug = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-module") == 0) {
	    Swig_mark_arg(i);
	    if (argv[i+1]) {
	      ModuleName = NewString(argv[i+1]);	    
	      Swig_mark_arg(i+1);
	    } else {
	      Swig_arg_error();
	    }
	  } else if (strcmp(argv[i],"-M") == 0) {
	    depend = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-MM") == 0) {
	    depend = 2;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-outdir") == 0) {
	    Swig_mark_arg(i);
	    if (argv[i+1]) {
	      outdir = NewString(argv[i+1]);	    
	      Swig_mark_arg(i+1);
	    } else {
	      Swig_arg_error();
	    }
	  } else if (strcmp(argv[i],"-Wall") == 0) {
	    Swig_mark_arg(i);
	    Swig_warnall();
	  } else if (strcmp(argv[i],"-Wallkw") == 0) {
	    allkw = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-Werror") == 0) {
	    werror = 1;
	    Swig_mark_arg(i);
	  } else if (strncmp(argv[i],"-w",2) == 0) {
	    Swig_mark_arg(i);
	    Swig_warnfilter(argv[i]+2,1);
	  } else if (strcmp(argv[i],"-dump_tags") == 0) {
	    dump_tags = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-dump_tree") == 0) {
	    dump_tree = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-nocontract") == 0) {
	    Swig_mark_arg(i);
	    Swig_contract_mode_set(0);
	  } else if (strcmp(argv[i],"-browse") == 0) {
	    browse = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-dump_typedef") == 0) {
	    dump_typedef = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-dump_classes") == 0) {
	    dump_classes = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-dump_memory") == 0) {
	    memory_debug =1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-help") == 0) {
	    fputs(usage,stderr);
	    Swig_mark_arg(i);
	    help = 1;
	  }
      }
  }

  SwigLib = Swig_copy_string(LibDir);        // Make a copy of the real library location
  

  for (i = 0; i < includecount; i++) {
    Swig_add_directory((DOH *) includefiles[i]);
  }

  if(getIdFilename == NULL)
  {
	if(clsIdMember != NULL)
	{
	    Printf(stderr,"\n-clsidcode must be specified when -clsiddata is.\n\n");
	    SWIG_exit (EXIT_FAILURE); 
	}
  }

  if(catchAllCodeFilename == NULL)
  {
    Printf(stderr,"\n-catchallcode must be specified.\n\n");
    SWIG_exit (EXIT_FAILURE); 
  }

  enableRuntimeTypeChecking = clsIdMember != NULL;

  // Define the __cplusplus symbol
  if (CPlusPlus)
    Preprocessor_define((DOH *) "__cplusplus 1", 0);

  // Parse language dependent options
  lang->main(argc,argv);

  
  if (help) {
    Printf(stderr,"\nNote: 'swig -<lang> -help' displays options for a specific target language.\n\n");
    SWIG_exit (EXIT_SUCCESS);    // Exit if we're in help mode
  }

  // Check all of the options to make sure we're cool.
  Swig_check_options();

  install_opts(argc, argv);

  // Add language dependent directory to the search path
  {
    DOH *rl = NewString("");
    Printf(rl,"%s%s%s", SwigLib, SWIG_FILE_DELIMETER, LibDir);
    Swig_add_directory(rl);
    rl = NewString("");
    Printf(rl,".%sswig_lib%s%s", SWIG_FILE_DELIMETER, SWIG_FILE_DELIMETER, LibDir);
    Swig_add_directory(rl);
  }

  sprintf(temp,"%s%sconfig", SwigLib, SWIG_FILE_DELIMETER);
  Swig_add_directory((DOH *) temp);
  Swig_add_directory((DOH *) "." SWIG_FILE_DELIMETER "swig_lib" SWIG_FILE_DELIMETER "config");
  Swig_add_directory((DOH *) SwigLib);
  Swig_add_directory((DOH *) "." SWIG_FILE_DELIMETER "swig_lib");

  if (Verbose) {
    printf ("LibDir: %s\n", LibDir);
    List *sp = Swig_search_path();
    Iterator s;
    for (s = First(sp); s.item; s = Next(s)) {
      Printf(stdout,"   %s\n", s.item);
    }
  }

  // If we made it this far, looks good. go for it....

  input_file = argv[argc-1];

  // If the user has requested to check out a file, handle that
  if (checkout) {
    DOH *s;
    char *outfile = input_file;
    if (outfile_name)
      outfile = outfile_name;

    if (Verbose)
      printf ("Handling checkout...\n");

    s = Swig_include(input_file);
    if (!s) {
      fprintf(stderr,"Unable to locate '%s' in the SWIG library.\n", input_file);
    } else {
      FILE *f = fopen(outfile,"r");
      if (f) {
	fclose(f);
	fprintf(stderr,"File '%s' already exists. Checkout aborted.\n", outfile);
      } else {
	f = fopen(outfile,"w");
	if (!f) {
	  fprintf(stderr,"Unable to create file '%s'\n", outfile);
	} else {
	  fprintf(stderr,"'%s' checked out from the SWIG library.\n", input_file);
	  fputs(Char(s),f);
	  fclose(f);
	}
      }
    }
  } else {
    // Check the suffix for a .c file.  If so, we're going to
    // declare everything we see as "extern"

    ForceExtern = check_suffix(input_file);

    // Run the preprocessor
    if (Verbose)
      printf ("Preprocessing...\n");
    {
      int i;
      String *fs = NewString("");
      FILE *df = Swig_open(input_file);
      if (!df) {
	Printf(stderr,"Unable to find '%s'\n", input_file);
	SWIG_exit (EXIT_FAILURE);
      }
      fclose(df);
      Printf(fs,"%%include \"swig.swg\"\n");
      if (allkw) {
	Printf(fs,"%%include \"allkw.swg\"\n");
      }
      if (lang_config) {
	Printf(fs,"\n%%include \"%s\"\n", lang_config);
      }
      Printf(fs,"%%include \"%s\"\n", Swig_last_file());
      for (i = 0; i < Len(libfiles); i++) {
	Printf(fs,"\n%%include \"%s\"\n", Getitem(libfiles,i));
      }
      Seek(fs,0,SEEK_SET);
      cpps = Preprocessor_parse(fs);
      if (Swig_error_count()) {
	SWIG_exit(EXIT_FAILURE);
      }
      if (cpp_only) {
	Printf(stdout,"%s", cpps);
	while (freeze);
	SWIG_exit (EXIT_SUCCESS);
      }
      if (depend) {
	String *outfile;
	if (!outfile_name) {
	  if (CPlusPlus) {
	    outfile = NewStringf("%s_wrap.%s", Swig_file_basename(input_file),cpp_extension);
	  } else {
	    outfile = NewStringf("%s_wrap.c", Swig_file_basename(input_file));
	  }
	} else {
	  outfile = NewString(outfile_name);
	}
	Printf(stdout,"%s: ", outfile);
	List *files = Preprocessor_depend();
	for (int i = 0; i < Len(files); i++) {
	  if ((depend != 2) || ((depend == 2) && (Strncmp(Getitem(files,i),SwigLib, Len(SwigLib)) != 0))) {
	    Printf(stdout,"\\\n %s ", Getitem(files,i));
	  }
	}
	Printf(stdout,"\n");
	SWIG_exit(EXIT_SUCCESS);
      }
      Seek(cpps, 0, SEEK_SET);
    }

    /* Register a null file with the file handler */
    Swig_register_filebyname("null", NewString(""));

    // Pass control over to the specific language interpreter
    if (Verbose) {
      fprintf (stdout, "Starting language-specific parse...\n");
      fflush (stdout);
    }

    Node *top = Swig_cparse(cpps);


    if (Verbose) {
      Printf(stdout,"Processing types...\n");
    }
    Swig_process_types(top);

    if (Verbose) {
      Printf(stdout,"C++ analysis...\n");
    }
    Swig_default_allocators(top);

    if (Verbose) {
      Printf(stdout,"Generating wrappers...\n");
    }

    if (dump_classes) {
      Hash *classes = Getattr(top,"classes");
      if (classes) {
	Printf(stdout,"Classes\n");
	Printf(stdout,"------------\n");
	Iterator ki;
	for (ki = First(classes); ki.key; ki = Next(ki)) {
	  Printf(stdout,"%s\n", ki.key);
	}
      }	       
    }

    if (dump_typedef) {
      SwigType_print_scope(0);
    }

    if (dump_tags) {
      Swig_print_tags(top,0);
    }
    if (top) {
      
	  buildClassIdMap(Getattr(top,"classes"), clsIdMember);
	  
      if (!Getattr(top,"name")) {
	Printf(stderr,"*** No module name specified using %%module or -module.\n");
	SWIG_exit(EXIT_FAILURE);
      } else {
	/* Set some filename information on the object */
	Setattr(top,"infile", input_file);
	if (!outfile_name) {
	  if (CPlusPlus) {
	    Setattr(top,"outfile", NewStringf("%s_wrap.%s", Swig_file_basename(input_file),cpp_extension));
	  } else {
	    Setattr(top,"outfile", NewStringf("%s_wrap.c", Swig_file_basename(input_file)));
	  }
	  Setattr(top,"outfile_h", NewStringf("%s_wrap.h", Swig_file_basename(input_file)));
	} else {
	  char *header = strdup(outfile_name);
	  char *ext = header + strlen(header);
	  while (ext > header && *ext != '.') ext--;
	  if (*ext == '.') *ext = 0;
	  Setattr(top,"outfile", outfile_name);
	  Setattr(top,"outfile_h", NewStringf("%s.h", header));
	  free(header);
	}
        set_outdir(Swig_file_dirname(Getattr(top,"outfile")));
	if (Swig_contract_mode_get()) {
	  Swig_contracts(top);
	}

	if(baseException)
		if(!buildExceptionGraph(top))
			SWIG_exit(EXIT_FAILURE);

	lang->top(top);
	if (browse) {
	  Swig_browser(top,0);
	}
      }
    }
    if (dump_tree) {
      Swig_print_tree(top);
    }
  }
  if (tm_debug) Swig_typemap_debug();
  if (memory_debug) DohMemoryDebug();
  while (freeze);


  if ((werror) && (Swig_warn_count())) {
    return Swig_warn_count();
  }
  return Swig_error_count();
}

// --------------------------------------------------------------------------
// SWIG_exit(int exit_code)
//
// Cleanup and either freeze or exit
// --------------------------------------------------------------------------

void SWIG_exit(int exit_code) {
  while (freeze);
  exit (exit_code);
}

