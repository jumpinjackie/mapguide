dnl $Id: config.m4,v 1.9 2005/11/02 04:36:20 george Exp $
dnl config.m4 for extension dbxml

PHP_ARG_WITH(dbxml, whether to enable dbxml support,
[  --with-dbxml           Enable dbxml support])
PHP_ARG_WITH(berkeleydb, where to find Berkeley DB,
[  --with-berkeleydb      where to find the Berkeley DB installation])
PHP_ARG_WITH(xerces, where to find the Xerces-c library,
[  --with-xerces      where to find the Xerces-c library])
PHP_ARG_WITH(pathan, where to find the Pathan library,
[  --with-pathan      where to find the Pathan library])
PHP_ARG_WITH(xquery, where to find the XQuery library,
[  --with-xquery      where to find the XQuery library])

CC=g++
EXTRA_CXXFLAGS="-g -DHAVE_CONFIG_H -O2 -Wall"
if test "$PHP_DBXML" != "no"; then
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, DBXML_SHARED_LIBADD)

  PHP_ADD_LIBRARY_WITH_PATH(db_cxx, $PHP_BERKELEYDB/lib, DBXML_SHARED_LIBADD)
  PHP_ADD_INCLUDE($PHP_BERKELEYDB/include)
  PHP_ADD_INCLUDE(../../../db-4.3.29/php_db4)

  PHP_ADD_LIBRARY_WITH_PATH(dbxml-2, $PHP_DBXML/lib, DBXML_SHARED_LIBADD)
  PHP_ADD_INCLUDE($PHP_DBXML/include)
   
  PHP_ADD_LIBRARY_WITH_PATH(pathan, $PHP_PATHAN/lib, DBXML_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(xerces-c, $PHP_XERCES/lib, DBXML_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(xquery, $PHP_XQUERY/lib, DBXML_SHARED_LIBADD)

  PHP_NEW_EXTENSION(dbxml, php_dbxml.cpp php_dbxml_manager.cpp php_dbxml_resolver.cpp php_dbxml_container.cpp php_dbxml_inputstream.cpp php_dbxml_transaction.cpp php_dbxml_results.cpp php_dbxml_value.cpp php_dbxml_queryexpression.cpp php_dbxml_querycontext.cpp php_dbxml_updatecontext.cpp php_dbxml_modify.cpp php_dbxml_indexspecification.cpp php_dbxml_data.cpp php_dbxml_document.cpp php_dbxml_statistics.cpp php_dbxml_indexlookup.cpp, $ext_shared)
  PHP_SUBST(DBXML_SHARED_LIBADD)
fi
