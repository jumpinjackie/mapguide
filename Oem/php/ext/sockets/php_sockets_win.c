/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2007 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   |          Sterling Hughes  <sterling@php.net>                         |
   |                                                                      |
   | WinSock: Daniel Beulshausen <daniel@php4win.de>                      |
   +----------------------------------------------------------------------+
 */

/* $Id: php_sockets_win.c,v 1.12.2.1.2.2 2007/07/22 23:01:20 jani Exp $ */


#ifdef PHP_WIN32

#include <stdio.h>
#include <fcntl.h>

#include "php.h"
#include "php_sockets.h"
#include "php_sockets_win.h"

int socketpair(int domain, int type, int protocol, SOCKET sock[2]) {
	struct sockaddr_in address;
	SOCKET redirect;
	int size = sizeof(address);

	if(domain != AF_INET) {
		set_errno(WSAENOPROTOOPT);
		return -1;
	}


	sock[0] = socket(domain, type, protocol);
	address.sin_addr.s_addr	= INADDR_ANY;
	address.sin_family		= AF_INET;
	address.sin_port		= 0;

	bind(sock[0], (struct sockaddr*)&address, sizeof(address));
	if(getsockname(sock[0], (struct sockaddr *)&address, &size) != 0) {

    }

	listen(sock[0], 2);
	sock[1] = socket(domain, type, protocol);	
	address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	connect(sock[1], (struct sockaddr*)&address, sizeof(address));
	redirect = accept(sock[0],(struct sockaddr*)&address, &size);

	close(sock[0]);
	sock[0] = redirect;

	if(sock[0] == INVALID_SOCKET ) {
		close(sock[0]);
		close(sock[1]);
		set_errno(WSAECONNABORTED);
		return -1;
	}
	
	return 0;
}

int inet_aton(const char *cp, struct in_addr *inp) {
  inp->s_addr = inet_addr(cp);

  if (inp->s_addr == INADDR_NONE) {
	  return 0;
  }

  return 1;
}
#endif
