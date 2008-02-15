#!/usr/bin/perl -w
#############################################################
#  Copyright (C) 2008 by Autodesk, Inc.
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
################################################################### 
#	post_process.MgOpenSource.pl
#	============================
#
#	Purpose:
#		Does some post-processing of the HTML files produced
#		by Doxygen, to fix things that we cannot configure
#	    through Doxygen.
#		
#		1) Removes the "File List" and "File Members" nodes from the Table Of Contents.
#		2) Removes '__get', '__set', and '__inherit'. (These are SWIG commands
#		   but because they are in the comments they get picked up by Doxygen.)
#		3) Sets the title to be shown in the web browser.
#		4) Fixes the links in the top-of-page tabs on the first page (index.htm)
#		   the user sees.
#		   (If you have DISABLE_INDEX set to NO, the default, in the Doxygen
# 		    configuration file, then Doxygen puts tabs at the top of each page,
#			with links to "Main Page, "Modules", etc.  We don't want to show
#  		    the "Files" tab, so we set DISABLE_INDEX to YES, and add the desired tabs
#		    to our custom header (which we specify via HTML_HEADER).
#			We also have CREATE_SUBDIRS set to YES, so some pages are in
#			subdirectories, and some are at the root level.  The links in
#			our custom header are fine for pages in subdirectories, but need
#			to be fixed for pages in the root level.  (EG, index.htm.)
#	
#	Usage:	
#		1) Alter the file paths etc. below (in the Parameters section) if necessary.
#		2) Call the script like this:
#				perl -w "post_process.MgOpenSource.pl"  
#
#
#	History:
# 		v 1.0 	- Written by Philip Sharman, 2008.02.11
# 		v 1.1	- Removed $SCRIPTS_DIRECTORY.  Now use use File::Bin to determine where the
#				  script lives.  PHS, 2008.02.14
#############################################################
package PostProcessor;
use lib 'PostProcessor/PerlModules';
use PostProcessor;

use strict;
use warnings;
use English;
use FindBin;
use Perl6::Slurp;			# Available from http://search.cpan.org/dist/Perl6-Slurp/
use Fatal qw(open close);		# See Perl Best Practices, page 278.

# Flush output to screen after every "print" so we can see what it going on immediately
# See Perl Best Practices, page 226.
use IO::Handle;
*STDOUT->autoflush();


################################################################### 
###		Global Variables
################################################################### 
# These must be defined this way so that  'PostProcessor.pm' can use them.
use vars qw($DIRECTORY_SEPARATOR $INPUT_DIRECTORY $TITLE $TRUE $FALSE $VERBOSE);
$TRUE =  (1==1);
$FALSE = (1==0);
# (Sadly, there seems to be no way to use Readonly with variables we want to share with a module.)


################################################################### 
###		Get information about the script's environment
################################################################### 
# Set the directory separator appropriate for this platform
if ($OSNAME eq 'MSWin32') 
{	
	$DIRECTORY_SEPARATOR = '\\';
}
elsif ($OSNAME eq 'linux') 
{
	$DIRECTORY_SEPARATOR = '/';
}
else
{
	# We are confused
	die "Fatal Error.  This script does not know how to handle the case where \$OSNAME = '$OSNAME'.\n";
}


################################################################### 
###		Parameters
################################################################### 
# Change these as necessary

# This script will examine all the .htm files in this directory (and its subdirectories)
$INPUT_DIRECTORY = '..' . $DIRECTORY_SEPARATOR . 'MgOpensource_WebAPIReference';
#$INPUT_DIRECTORY = 'TEST_POST_PROCESSING_INPUT'; 	# For testing

# The title to be shown in the web browser
$TITLE = 'MapGuide Web API Reference';

# Set this to $TRUE if you want more verbose information about what the script is doing
#$VERBOSE = $TRUE;
$VERBOSE = $FALSE;


################################################################### 
# Do it
do_it();

################################################################### 
