#!/usr/bin/perl -w
################################################################### 
#	post_process.MgOpenSource.pl
#	============================
#
#	Purpose:
#		Does some post-processing of the HTML files produced
#		by Doxygen, to fix things that we cannot configure
#	    through Doxygen.
#		See PostProcess.pm for details.
#	
#	Usage:	
#		1) Alter the file paths etc. below (in the Parameters section) if necessary.
#		2) Call the script like this (from the HTML directory):
#				perl -w "PostProcessor\post_process.MgOpenSource.pl"  
#
#
#	History:
# 		v 1.0 	- Written by Philip Sharman, 2008.02.11
# 		v 1.1	- Removed $SCRIPTS_DIRECTORY.  Now use use File::Bin to determine where the
#				  script lives.  PHS, 2008.02.14
#		v 1.2	- Changed to not use any external Perl modules, to make it easier to
#				  distribute.  Also moved the main calling routine back to here and out
#				  of PostProcessor.pm.  PHS, 2008.02.20
#		v 1.3	- Added ROOT, call to 'change_root', and call to 'change_namespace()'.
#                 PHS, 2008.02.24
#############################################################
package PostProcessor;
use lib 'PostProcessor'; # Look in the PostProcessor directory for PostProcessor.pm (assuming we are called from the HTML parent directory).
use PostProcessor;

use strict;
use warnings;
use English;
use FindBin;
use Fatal qw(open close);		# See Perl Best Practices, page 278.

# Flush output to screen after every "print" so we can see what it going on immediately
# See Perl Best Practices, page 226.
use IO::Handle;
*STDOUT->autoflush();


################################################################### 
###		Global Variables
################################################################### 
# These must be defined this way so that  'PostProcessor.pm' can use them.
use vars qw($DIRECTORY_SEPARATOR $INPUT_DIRECTORY $TITLE $ROOT $TRUE $FALSE $VERBOSE);
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
$INPUT_DIRECTORY = '..' . $DIRECTORY_SEPARATOR . 'MgDesktop_WebAPIReference';
#$INPUT_DIRECTORY = 'TEST_POST_PROCESSING_INPUT'; 	# For testing

# The title to be shown in the web browser
$TITLE = 'MapGuide Web API Reference';

# The name for the top node in the TOC
$ROOT = 'MapGuide Web API Reference';

# Set this to $TRUE if you want more verbose information about what the script is doing
#$VERBOSE = $TRUE;
$VERBOSE = $FALSE;


################################################################### 
# Do it all
my $start_time = time();
$INPUT_DIRECTORY = change_relative_path_to_absolute($INPUT_DIRECTORY);
print "Post processing '$INPUT_DIRECTORY' ... \n\n"; 

# Do it
show_global_variables(); # (for debugging)
#delete_file_list();
remove_SWIG_commands_from_directory($INPUT_DIRECTORY);
#change_title();
fix_links_in_directory($INPUT_DIRECTORY);
#change_root();
#remove_namespace();

# All done
print "\nPost processing is complete. (Time taken = ", convert_seconds_to_hms_string((time() - $start_time)) , ".)\n"; 

################################################################### 
