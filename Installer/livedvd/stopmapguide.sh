#!/bin/bash
#
#  Copyright (C) 2010 by Autodesk, Inc.
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of version 2.1 of the GNU Lesser
#  General Public License as published by the Free Software Foundation.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# About:
# ===
# This script will start the MapGuide Server and Apache on the LiveDVD
# Usage (as user with sudo rights):
# ./stopmapguide.sh
#

MGPID=`ps -C mgserver -o pid=`
sudo /bin/bash -c "echo Stopping Apache...;sleep 1;/usr/local/mapguideopensource-2.2.0/webserverextensions/apache2/bin/apachectl stop; echo Stopping MapGuide...; sleep 2; kill -SIGKILL $MGPID; sleep 3"
