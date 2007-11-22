#!/usr/bin/env python
#
# Setup script for Berkeley DB XML package

import os, sys
from distutils.core import setup, Extension

#
# Posix:
#
# db_home, xqilla_home, and xerces_home are
# set based on the arguments  passed to the configure script.
# This can be overridden with the --with-berkeleydb, --with-xqilla, 
# and --with-xerces arguments.
#
# Windows: may require further editing to reflect site specifics.
#
if os.name == "posix":
  db_home = "@DB_DIR@"
  xerces_home = "@XERCES_DIR@"
  xqilla_home = "@XQILLA_DIR@"
  LFLAGS = os.environ.get('LFLAGS', [])
else:
  xerces_home = "../../../xerces-c-src"
  xqilla_home = "../../../xqilla"
  db_home = "../../../db-4.5.20"

for arg in sys.argv:
  if arg.startswith('--with-berkeleydb='):
    db_home = arg.split('=')[1]
  elif arg.startswith('--with-xerces='):
    xerces_home = arg.split('=')[1]
  elif arg.startswith('--with-xqilla='):
    xqillahome = arg.split('=')[1]
  elif arg.startswith('--lflags='):
    LFLAGS = arg.split('=')[1].split()
    sys.argv.remove(arg)

lflags_arg = []

sys.argv = filter(lambda x: not x.startswith("--with-"), sys.argv)

debug = "--debug" in sys.argv or "-g" in sys.argv

# setup complains when passed debug flags for install
if "install" in sys.argv and not "build" in sys.argv:
  sys.argv = filter(lambda x: (x != "--debug") and (x != "-g"), sys.argv)

if os.name == "posix":
  INCLUDES =    ["../../include",
                 os.path.join(db_home, "include")]

  LIBDIRS =     ["../../build_unix/.libs",
                 os.path.join(db_home, "lib"),
                 os.path.join(xqilla_home, "lib"),
                 os.path.join(xerces_home, "lib")]

  LIBS =        ["dbxml-2",
                 "db_cxx-4",
                 "xqilla", "xerces-c"]

  DATAFILES =   []

  if LFLAGS:
        lflags_arg = LFLAGS + LIBS

  # Now run with whatever settings we've got
  setup(name = "dbxml",
              version = "2.3.10",
              description = "Berkeley DB XML Python API",
              long_description = """\
              This module provides a complete wrapping of the C++ API
              to the Berkeley DB XML (BDB XML) XML database.  BDB
              XML can be used to store, retrieve, manage, and query (using
              XQuery) XML documents.""",
              author = "Oracle",
              author_email = "berkeleydb-info_us@oracle.com",
              url = "http://www.oracle.com",
              py_modules = ["dbxml"],
              ext_modules = [Extension("_dbxml", ["dbxml_python_wrap.cpp"],
                                       include_dirs = INCLUDES,
                                       library_dirs = LIBDIRS,
                                       runtime_library_dirs = LIBDIRS,
                                       libraries = LIBS,
                                       extra_link_args = lflags_arg
                                       )],
              data_files = DATAFILES)
elif os.name == "nt":
  INCLUDES =    ["../../include",
                 os.path.join(db_home, "build_windows"),
                 os.path.join(db_home, "dbinc")]

  if debug:
    LIBDIRS =   ["../../../lib",
                 "../../build_windows/Debug",
                 os.path.join(db_home, "build_windows/Debug"),
                 os.path.join(xqilla_home, "lib"),
                 os.path.join(xerces_home, "Build/Win32/VC7")]
  
    LIBS =      ["libdbxml23D",
                 "libdb45D", "xqilla10d",
                 "xerces-c_2D"]

    DATAFILES = [("", ["../../../bin/debug/libdbxml23D.dll",
                       "../../../bin/debug/libdb45D.dll",
                       "../../../bin/debug/xqilla10d.dll",
                       "../../../bin/debug/xerces-c_2_7D.dll"])]
  else:
    LIBDIRS =   ["../../../lib",
                 "../../build_windows/Release",
                 os.path.join(db_home, "build_windows/Release"),
                 os.path.join(xqilla_home, "lib"),
                 os.path.join(xerces_home, "Build/Win32/VC7")]
  
    LIBS =      ["libdbxml23",
                 "libdb45", "xqilla10",
                 "xerces-c_2"]

    DATAFILES = [("", ["../../../bin/libdbxml23.dll",
                       "../../../bin/libdb45.dll",
                       "../../../bin/xqilla10.dll",
                       "../../../bin/xerces-c_2_7.dll"])]

    # Now run with whatever settings we've got
    setup(name = "dbxml",
          version = "2.3.10",
          description = "Berkeley DB XML Python API",
          long_description = """\
          This module provides a complete wrapping of the C++ API
          to the Berkeley DB XML (BDB XML) XML database.  BDB
          XML can be used to store, retrieve, manage, and query (using
          XQuery) XML documents.""",
          author = "Oracle",
          author_email = "berkeleydb-info_us@oracle.com",
          url = "http://www.oracle.com",
          py_modules = ["dbxml"],
          ext_modules = [Extension("_dbxml", ["dbxml_python_wrap.cpp"],
                                   include_dirs = INCLUDES,
                                   library_dirs = LIBDIRS,
                                   libraries = LIBS,
                                   )],
          data_files = DATAFILES)
    
else:
  print "I don't know anything about your platform '%s'." % os.name
  print "Please check the build instructions for more information"
  sys.exit(1)
