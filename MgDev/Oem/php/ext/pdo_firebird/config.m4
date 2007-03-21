dnl
dnl $Id: config.m4,v 1.8 2005/06/14 00:00:52 sniper Exp $
dnl

if test "$PHP_PDO" != "no"; then

PHP_ARG_WITH(pdo-firebird,for Firebird support for PDO,
[  --with-pdo-firebird[=DIR] PDO: Firebird support.  DIR is the Firebird base
                            install directory, defaults to /opt/firebird])

if test "$PHP_PDO_FIREBIRD" != "no"; then
  if test "$PHP_PDO_FIREBIRD" = "yes"; then
    FIREBIRD_INCDIR=/opt/firebird/include
    FIREBIRD_LIBDIR=/opt/firebird/lib
  else
    FIREBIRD_INCDIR=$PHP_PDO_FIREBIRD/include
    FIREBIRD_LIBDIR=$PHP_PDO_FIREBIRD/lib
  fi

  PHP_CHECK_LIBRARY(fbclient, isc_detach_database,
  [
    FIREBIRD_LIBNAME=fbclient
  ], [
    PHP_CHECK_LIBRARY(gds, isc_detach_database,
    [
      FIREBIRD_LIBNAME=gds
    ], [
      PHP_CHECK_LIBRARY(ib_util, isc_detach_database,
      [
        FIREBIRD_LIBNAME=ib_util
      ], [
        AC_MSG_ERROR([libfbclient, libgds or libib_util not found! Check config.log for more information.])
      ], [
        -L$FIREBIRD_LIBDIR
      ])
    ], [
      -L$FIREBIRD_LIBDIR
    ])
  ], [
    -L$FIREBIRD_LIBDIR
  ])
 
  PHP_CHECK_PDO_INCLUDES

  PHP_ADD_LIBRARY_WITH_PATH($FIREBIRD_LIBNAME, $FIREBIRD_LIBDIR, PDO_FIREBIRD_SHARED_LIBADD)
  PHP_ADD_INCLUDE($FIREBIRD_INCDIR)
  AC_DEFINE(HAVE_PDO_FIREBIRD,1,[ ])
  PHP_NEW_EXTENSION(pdo_firebird, pdo_firebird.c firebird_driver.c firebird_statement.c, $ext_shared,,-I$pdo_inc_path)
  PHP_SUBST(PDO_FIREBIRD_SHARED_LIBADD)
  PHP_ADD_EXTENSION_DEP(pdo_firebird, pdo)
fi

fi
