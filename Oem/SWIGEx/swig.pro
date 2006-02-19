TEMPLATE = app 
CONFIG += warn_on 

SOURCES += Source/CParse/cscanner.c \
           Source/CParse/parser.c \
           Source/CParse/templ.c \
           Source/CParse/util.c \
           Source/DOH/base.c \
           Source/DOH/file.c \
           Source/DOH/fio.c \
           Source/DOH/hash.c \
           Source/DOH/list.c \
           Source/DOH/memory.c \
           Source/DOH/string.c \
           Source/DOH/void.c \
           Source/Modules/allocate.cxx \
           Source/Modules/browser.cxx \
           Source/Modules/chicken.cxx \
           Source/Modules/contract.cxx \
           Source/Modules/csharp.cxx \
           Source/Modules/directors.cxx \
           Source/Modules/emit.cxx \
           Source/Modules/guile.cxx \
           Source/Modules/java.cxx \
           Source/Modules/lang.cxx \
           Source/Modules/main.cxx \
           Source/Modules/module.cxx \
           Source/Modules/mzscheme.cxx \
           Source/Modules/ocaml.cxx \
           Source/Modules/overload.cxx \
           Source/Modules/perl5.cxx \
           Source/Modules/php4.cxx \
           Source/Modules/php5.cxx \
           Source/Modules/pike.cxx \
           Source/Modules/python.cxx \
           Source/Modules/ruby.cxx \
           Source/Modules/s-exp.cxx \
           Source/Modules/swigmain.cxx \
           Source/Modules/tcl8.cxx \
           Source/Modules/typepass.cxx \
           Source/Modules/utils.cxx \
           Source/Modules/xml.cxx \
           Source/Swig/cwrap.c \
           Source/Swig/error.c \
           Source/Swig/fragment.c \
           Source/Swig/getopt.c \
           Source/Swig/include.c \
           Source/Swig/misc.c \
           Source/Swig/naming.c \
           Source/Swig/parms.c \
           Source/Swig/scanner.c \
           Source/Swig/stype.c \
           Source/Swig/symbol.c \
           Source/Swig/tree.c \
           Source/Swig/typemap.c \
           Source/Swig/typeobj.c \
           Source/Swig/typesys.c \
           Source/Swig/warn.c \
           Source/Swig/wrapfunc.c \
           Source/Preprocessor/cpp.c \
           Source/Preprocessor/expr.c
           
           
HEADERS += Source/CParse/cparse.h \
           Source/CParse/parser.h \
           Source/DOH/doh.h \
           Source/DOH/dohint.h \
           Source/Include/swigconfig.h \
           Source/Include/swigwarn.h \
           Source/Modules/swigmod.h \
           Source/Modules/ExceptionClass.h \
           Source/Swig/swig.h \
           Source/Preprocessor/preprocessor.h
 
INCLUDEPATH += Source/CParse \
           Source/DOH \
           Source/Include \
           Source/Swig \
           Source/Modules \
           Source/Preprocessor

#########################################
#                                       #
#      Debug specific configuration     #
#                                       #
#########################################
debug {
DESTDIR = ./Linux
TARGET = swig
OBJECTS_DIR = ./Linux/obj/debug 
DEFINES = _DEBUG
}

#########################################
#                                       #
#    Release specific configuration     #
#                                       #
#########################################
release {
DESTDIR = ./Linux
TARGET = swig
OBJECTS_DIR = ./Linux/obj/release 
DEFINES = NDEBUG
}
