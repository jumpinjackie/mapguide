dnl
dnl $Id: config.m4,v 1.17.6.1 2006/10/27 07:23:44 thetaphi Exp $
dnl

AC_MSG_CHECKING(for NSAPI support)
AC_ARG_WITH(nsapi,
[  --with-nsapi=DIR        Build PHP as NSAPI module for Netscape/iPlanet/Sun Webserver],[
  PHP_NSAPI=$withval
],[
  PHP_NSAPI=no
])
AC_MSG_RESULT($PHP_NSAPI)

if test "$PHP_NSAPI" != "no"; then
  if test ! -d $PHP_NSAPI/bin ; then
    AC_MSG_ERROR(Please specify the path to the root of your Netscape/iPlanet/Sun Webserver using --with-nsapi=DIR)
  fi
  AC_MSG_CHECKING(for NSAPI include files)
  if test -d $PHP_NSAPI/include ; then
    NSAPI_INC_DIR="$PHP_NSAPI/include"
    AC_MSG_RESULT(Netscape 3.x / Sun 7.x style)
    AC_CHECK_HEADERS([$NSAPI_INC_DIR/nsapi.h])
    NSAPI_INCLUDE="-I$NSAPI_INC_DIR"
  fi
  if test -d $PHP_NSAPI/plugins/include ; then
    NSAPI_INC_DIR="$PHP_NSAPI/plugins/include"
    AC_MSG_RESULT(iPlanet 4.x / Sun 6.x style)
    AC_CHECK_HEADERS([$NSAPI_INC_DIR/nsapi.h])
    NSAPI_INCLUDE="$NSAPI_INCLUDE -I$NSAPI_INC_DIR"
  fi
  if test "$NSAPI_INCLUDE" = ""; then
    AC_MSG_ERROR(Please check you have nsapi.h in either $PHP_NSAPI/include or $PHP_NSAPI/plugins/include)
  fi

  PHP_EVAL_INCLINE($NSAPI_INCLUDE)
  PHP_BUILD_THREAD_SAFE
  AC_DEFINE(HAVE_NSAPI,1,[Whether you have a Netscape/iPlanet/Sun Webserver])
  PHP_SELECT_SAPI(nsapi, shared, nsapi.c)
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$PHP_NSAPI/bin/"
fi


dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
