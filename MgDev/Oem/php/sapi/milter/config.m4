dnl
dnl $Id: config.m4,v 1.6 2003/10/03 05:24:31 sniper Exp $
dnl

AC_MSG_CHECKING(for Milter support)
AC_ARG_WITH(milter,
[  --with-milter[=DIR]     Build PHP as Milter application],[
  if test "$withval" = "yes"; then
    if test -f /usr/lib/libmilter.a ; then
      MILTERPATH=/usr/lib
    else
      if test -f /usr/lib/libmilter/libmilter.a ; then
        MILTERPATH=/usr/lib/libmilter
      else
        AC_MSG_ERROR(Unable to find libmilter.a)
      fi
    fi
  else
    MILTERPATH=$withval
  fi
  
  SAPI_MILTER_PATH=sapi/milter/php-milter
  PHP_SUBST(SAPI_MILTER_PATH)
  PHP_BUILD_THREAD_SAFE
  PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/milter/Makefile.frag)
  
  PHP_SELECT_SAPI(milter, program, php_milter.c getopt.c,,'$(SAPI_MILTER_PATH)') 
  PHP_ADD_LIBRARY_WITH_PATH(milter, $MILTERPATH,)
  
  BUILD_MILTER="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_SAPI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_MILTER_PATH)"

  INSTALL_IT="\$(INSTALL) -m 0755 \$(SAPI_MILTER_PATH) \$(bindir)/php-milter"
  RESULT=yes

  PHP_SUBST(BUILD_MILTER)
],[
  RESULT=no
])
AC_MSG_RESULT($RESULT)
