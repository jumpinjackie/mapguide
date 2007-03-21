dnl
dnl $Id: config.m4,v 1.9.2.2 2005/10/10 10:28:53 sniper Exp $
dnl

PHP_ARG_WITH(mssql,for MSSQL support via FreeTDS,
[  --with-mssql[=DIR]      Include MSSQL-DB support.  DIR is the FreeTDS home
                          directory, defaults to /usr/local/freetds])

if test "$PHP_MSSQL" != "no"; then

  if test "$PHP_MSSQL" = "yes"; then
    for i in /usr/local /usr; do
      if test -f $i/include/tds.h; then
        FREETDS_INSTALLATION_DIR=$i
        FREETDS_INCLUDE_DIR=$i/include
        break
      elif test -f $i/include/freetds/tds.h; then
        FREETDS_INSTALLATION_DIR=$i
        FREETDS_INCLUDE_DIR=$i/include/freetds
        break
      fi
    done

    if test -z "$FREETDS_INSTALLATION_DIR"; then
      AC_MSG_ERROR(Cannot find FreeTDS in known installation directories)
    fi

  elif test "$PHP_MSSQL" != "no"; then

    if test -f $PHP_MSSQL/include/tds.h; then
      FREETDS_INSTALLATION_DIR=$PHP_MSSQL
      FREETDS_INCLUDE_DIR=$PHP_MSSQL/include
    elif test -f $PHP_MSSQL/include/freetds/tds.h; then
      FREETDS_INSTALLATION_DIR=$PHP_MSSQL
      FREETDS_INCLUDE_DIR=$PHP_MSSQL/include/freetds
    else
      AC_MSG_ERROR(Directory $PHP_MSSQL is not a FreeTDS installation directory)
    fi
  fi  

  if test ! -r "$FREETDS_INSTALLATION_DIR/$PHP_LIBDIR/libtds.a" && test ! -r "$FREETDS_INSTALLATION_DIR/$PHP_LIBDIR/libtds.so"; then
     AC_MSG_ERROR(Could not find $FREETDS_INSTALLATION_DIR/$PHP_LIBDIR/libtds.[a|so])
  fi

  PHP_ADD_INCLUDE($FREETDS_INCLUDE_DIR)
  PHP_ADD_LIBRARY_WITH_PATH(sybdb, $FREETDS_INSTALLATION_DIR/$PHP_LIBDIR, MSSQL_SHARED_LIBADD)

  PHP_NEW_EXTENSION(mssql, php_mssql.c, $ext_shared)
  AC_CHECK_LIB(dnet_stub, dnet_addr,
     [ PHP_ADD_LIBRARY_WITH_PATH(dnet_stub,,MSSQL_SHARED_LIBADD)
        AC_DEFINE(HAVE_LIBDNET_STUB,1,[ ])
     ])
  AC_DEFINE(HAVE_MSSQL,1,[ ])
  AC_DEFINE(HAVE_FREETDS,1,[ ])
  PHP_SUBST(MSSQL_SHARED_LIBADD)
fi
