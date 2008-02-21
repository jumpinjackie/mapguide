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
#############################################################
#	PostProcessor.pm
#	=================
#
#	Called by "post_process.MgOpenSource.pl".
#
#
#	History:
# 		v 1.0 	- Written by Philip Sharman, 2008.02.12
# 		v 1.1 	- Now uses Find::Bin to determine the script's location.  
#				- Also uses File::Spec->canonpath to clean up any mixture of
#				  slashes and backslashes in file paths. 
#				- Moved the main part ('do_it') to here instead of in the calling script.
#				  PHS, 2008.02.14
#		v 1.2	- Changed to not use any external Perl modules, to make it easier to
#				  distribute.  PHS, 2008.02.20
#############################################################
package PostProcessor;
use File::Find;


## Uncomment for testing and debugging ...
#	my $DIRECTORY_SEPARATOR = '\\';
#	print "DIRECTORY_SEPARATOR = $DIRECTORY_SEPARATOR \n"; 
#	
##	remove_SWIG_commands_from_directory('..\MgEnterprise_HTML');
#	$VERBOSE = 1;
#	$TRUE = 1;
#	$FALSE = not $TRUE;
#	remove_SWIG_commands_from_directory('C:\OSGeo+Perforce\mapguide\MgDev\Doc\API\HTML\PostProcessor\TEST_POST_PROCESSING_INPUT');


return 1; # Included files must return this.

################################################################### 
###		Subroutines
################################################################### 
# Deletes the 'File List' and 'File Members' from the TOC.
sub delete_file_list
{
	my $toc_file = "$INPUT_DIRECTORY$DIRECTORY_SEPARATOR" . 'tree.htm';  
	$toc_file = change_relative_path_to_absolute($toc_file);		
	
	if (not -e $toc_file)
	{
		print "*** ERROR in delete_file_list: We could not find the TOC file: '$toc_file'. ***\n"; 
		return;
	}
	
	# Slurp the input file into a variable	
	my $text = slurp($toc_file);		
	
	
	# 1) Delete 'File List'
	my $okay1 = ( $text =~ s/<p><img[^>]*><img[^>]*><a class=\"el\" href=\"files\.htm\"[^>]*>File List<\/a>.*?<\/p>\s*<div[^>]*>.*?<\/div>//s );
	if ($okay1)
	{
		print "Removed File List from '$toc_file'.\n"; 
	}
	else
	{
		print "*** ERROR in delete_file_list: We could not find the File List section in '$toc_file'. ***\n"; 
	}

	# 2) Delete 'File Members'
	my $okay2 = ( $text =~ s/<p><img[^>]*><img[^>]*><a class=\"el\" href=\"globals\.htm\"[^>]*>File Members<\/a><\/p>// );
	if ($okay1)
	{
		print "Removed File Members from '$toc_file'.\n"; 
	}
	else
	{
		print "*** ERROR in delete_file_list: We could not find the File Members section in '$toc_file'. ***\n"; 
	}

	# Write to the output file. 
	if ($okay1 and $okay2)
	{
		open (OUTPUT, ">", $toc_file) or die ("Cannot write too '$toc_file'. ($!) "); 
		print OUTPUT "$text\n";
		close OUTPUT;
		print "Saved: '$toc_file'.\n"; 
		print "--------------------------------\n"; 
	}
}

################################################################### 
# Change the tile in index.htm.
sub change_title
{
	my $index_file = "$INPUT_DIRECTORY$DIRECTORY_SEPARATOR" . 'index.htm';
	my $index_file_new = $index_file;	
	
	if (not -e $index_file)
	{
		print "*** ERROR in change_title: We could not find the index file: '$index_file'. ***\n"; 
		return;
	}
	
	# Slurp the input file into a variable
	my $text =slurp($index_file);		
	
	# Change it
	my $okay = ( $text =~ s/<title>Doxygen Documentation<\/title>/<title>$TITLE<\/title>/s );
	if ($okay)
	{
		print "Changed the title in '$index_file'.\n"; 
	}
	else
	{
		print "*** ERROR in change_title: We could not change the title in '$index_file'. ***\n"; 
	}

	# Write to the output file. 
	if ($okay)
	{
		open (OUTPUT, ">", $index_file) or die ("Cannot write to '$index_file'. ($!) "); 
		print OUTPUT "$text\n";
		close OUTPUT;
		print "Saved: '$index_file'.\n"; 
		print "--------------------------------\n"; 
	}
}

#######################################################
sub remove_SWIG_commands_from_directory #	($directory)
# Calls remove_SWIG_commands() with every file in the directory.
# Does so recursively.
#
# This uses Recipe 9.7 from _Perl Cookbook_.
{
	my $directory = shift;
	$directory = change_relative_path_to_absolute($directory); # Relative paths make working with File::Find difficult.
	print "Removing SWIG_commands from directory: '$directory'. (This can take several minutes.)\n"; 
	
	# Check the directory exists
	if (! -e $directory)
	{
		die "ERORR in remove_SWIG_commands_from_directory: Cannot find the directory '$directory'. \n";
	}

	# Check that it is a directory
	if (! -d $directory)
	{
		die "ERORR in remove_SWIG_commands_from_directory: This is not a directory: '$directory'. \n";
	}

	my @directories_to_search;
	push(@directories_to_search, $directory);
	find(\&remove_SWIG_commands_from_file, @directories_to_search );
	print "\n"; 
}

################################################################### 
# Remove  __get, __set, __inherited etc. 
sub remove_SWIG_commands_from_file 
{
	my $directory = $File::Find::dir;
    my $name = $_;
    my $fullPath = $File::Find::name;
    
    # Change all forward and backward slashes to $DIRECTORY_SEPARATOR  
   	$directory = File::Spec->canonpath($directory);
   	$fullPath = File::Spec->canonpath($fullPath);
   	
    # For debugging:
#    print "parent directory = '$directory'\n"; 
#    print "name =             '$name'\n"; 
#    print "fullPath =         '$fullPath", -d && "\\", "'\n"; # Add a backslash if it is a directory
        
	if ($VERBOSE) { print "Removing SWIG commands from file '$fullPath'.\n"; }
	
	# Ignore directories and binary files
	if (-d $fullPath  or  -B $fullPath)
	{
#		if ($VERBOSE) { print "Ignoring '$fullPath'\n"; }
		return;
	}
	
	my $FILE_HAS_BEEN_CHANGED = $FALSE;
	
	if ( $fullPath =~ /.htm\s*$/ )
   	{
   		if (not -e $fullPath)
		{
			print "*** ERROR in remove_SWIG_commands_from_file(): We could not find the file: '$fullPath'. ***\n"; 
			return;
		}
	
#		# This takes a long time, so give a progress indicator
#		$n++;
#		if ($n > 100)
#		{	
#			$n = 0;
#			print ".";
#		}
		
		# Slurp the input file into a variable
		my $text = slurp($fullPath);		
		
		($FILE_HAS_BEEN_CHANGED, $text) = remove_SWIG_commands_from_text($text, $fullPath);
		
#		# If the file has been changed, save it
		# Write to the output file. 
		if ($FILE_HAS_BEEN_CHANGED)
		{
			open (OUTPUT, ">", $fullPath) or die ("Cannot write to '$fullPath'. ($!) "); 
			print OUTPUT "$text\n";
			close OUTPUT;
			print "Saved: '$fullPath'.\n"; 
			print "--------------------------------\n"; 
		}
   	}
}

################################################################### 
# Remove  __get, __set, __inherited etc. 
# This is called by 'remove_SWIG_commands_from_file'.
# (Splitting this off into a separate subroutine makes it easier to write 
# unit tests for it.)
# Returns ($FILE_HAS_BEEN_CHANGED, $text)
sub remove_SWIG_commands_from_text #	($text, $path)
{
	my ($text, $path) = @_;	# Note: The text has been obtained by slurping in an entire file, so it will contain newlines
	my $FILE_HAS_BEEN_CHANGED = $FALSE;
	
	# --------------------------------------------------- #
	# 1) Remove titles that refer to __get etc.  (The titles are never seen because they all appear in framesets anywyay, but it is nice to be clean.)
	#    Change 'title="__get"' to ''.
	# 1a) Show what we're about to change.
	my $TITLE_REGEX = qr{
										(									# $1 (captured just so we can print it below)		
											title=\"							# title="
												(									# one or more:
													(\s*,?\s*)							# optionally, ", "
													(__get|__set|__inherited)			# __get, __set, or __inherited
												)+									#
												\s*									# optional whitespace
												\"									# closing "
										)									#
									}x;

	if ($VERBOSE)
	{
		while( $text =~  m/$TITLE_REGEX/g )
		{
			print "1) Fixing: '$1'.\n"; 
		}
	}
			
	# 1b) Change it
	if ($text =~ /$TITLE_REGEX/) { $FILE_HAS_BEEN_CHANGED = $TRUE; }
	$text =~  s/$TITLE_REGEX//gsx;


	# --------------------------------------------------- #
	# 2) Deal with paragraphs
	# 2a) Show what we're about to change
	my $PARAGRAPH_REGEX = qr{
											(									# $1 (captured just so we can print it below)		
												<p>\s*								# <p>
												(										# one or more:
													(\s*,?\s*)								# optionally, ", "
													(__get|__set|__inherited)				# __get, __set, or __inherited
												)+										#
												\s*(<\/p>)?							# Doxygen doesn't typically put in the closing marker
											)									#
										}x;
				
	if ($VERBOSE)
	{
		while ( $text =~  m/$PARAGRAPH_REGEX/g)
		{
			print "2) Fixing: '$1'.\n"; 
		}
	}
	
	# 2b) Change it
	if ($text =~ /$PARAGRAPH_REGEX/) { $FILE_HAS_BEEN_CHANGED = $TRUE; }
	$text =~  	s/$PARAGRAPH_REGEX//g;		
	
	
	# --------------------------------------------------- #
	# 3) Deal with other text (e.g., ">__get, __set<" or ">__get, __set Gets the widget")
	# 3a) Show what we're about to change
	my $OTHER_REGEX = qr{
									(										# $1 (captured just so we can print it below)	
										>									# >
										(										# $2 (the part to replace)
											(										# one or more:
												(\s*,?\s*)								# optionally, ", "
												(__get|__set|__inherited)				# __get, __set, or __inherited
											)+										#
											\s*
										)										#
										\s*									# 
									)										#			
								 }x;
								 
	if ($VERBOSE)
	{
		while ( $text =~  m/$OTHER_REGEX/g )
		{
			print "3) Fixing: '$2' in '$1'.\n"; 
		}
	}
	
	# 3b) Change it
	if ($text =~ /$OTHER_REGEX/) { $FILE_HAS_BEEN_CHANGED = $TRUE; }
	$text =~  	s/$OTHER_REGEX/>/g;	
	

	# --------------------------------------------------- #
	# Check that we got them all
	if ($text =~ /(.{0,20}(__get|__set|__inerited).{0,20})/)
	{
		print "*** WARNING in remove_SWIG_commands_from_text: The file '$path'\nstill needs replacing because of this:\n\t'$1'. *** \n"
	}
		
	return ($FILE_HAS_BEEN_CHANGED, $text);
	
} # end of 'remove_SWIG_commands_from_text'

###################################################################  
sub fix_links_in_directory #	($directory)
# Calls fix_links_in_file() with every file in the directory.
# Does NOT do so recursively; it only examines file in this directory.
{
	my $directory = $_[0];
#	print "processing directory: '$directory'\n"; 
	
	# Check the directory exists
	if (! -e $directory)
	{
		die "ERORR: Cannot find the directory '$directory'. \n";
	}

	my $file;
	opendir DIR, $directory;
	foreach my $item (sort readdir(DIR))		# $item is the short name, e.g. "file.htm"
	{
		if (-f "$directory$DIRECTORY_SEPARATOR$item")			
		{
			# Process the file
			fix_links_in_file( "$directory$DIRECTORY_SEPARATOR$item" );
		}
	}
	closedir DIR;
}

#######################################################
# We use a custom header to insert tabs at the top of each page.
# Need to fix the links in pages at the root level.
sub fix_links_in_file	# ($path)
{
	my $path = $_[0];	# EG, "MgEnterprise_HTML\d0\d02\group___mg_drawing_service.htm"
#	print "processing file: '$path'\n"; 
	
	my $FILE_HAS_BEEN_CHANGED = $FALSE;
	
	if ( $path =~ /\.htm\s*$/ )
   	{
   		if (not -e $path)
		{
			print "*** ERROR in fix_links_in_file(): We could not find the file: '$path'. ***\n"; 
			return;
		}
	
		# Slurp the input file into a variable
		my $text = slurp($path);			
	
		($FILE_HAS_BEEN_CHANGED, $text) = fix_links_in_text($text, $path);
		
		# If the file has been changed, save it
		if ($FILE_HAS_BEEN_CHANGED) 
		{ 			
			# Save the file
			open (OUTPUT, ">", $path) or die ("*** FATAL ERROR in xxx: Cannot write to '$path'. ($!) "); 
			print OUTPUT "$text\n";
			print "Saved: '$path'.\n"; 
			close OUTPUT; 
			
			print "--------------------------------\n"; 
		}
	}
} # end of fix_links_in_file

################################################################### 
# This is called by 'fix_links_in_file'.
# (Splitting this off into a separate subroutine makes it easier to write 
# unit tests for it.)
# Returns ($FILE_HAS_BEEN_CHANGED, $text)
sub fix_links_in_text	# ($text, $path)
{
	my ($text, $path) = @_;
	my $FILE_HAS_BEEN_CHANGED = $FALSE;
	
	# Change it	
	my $MAIN_PAGE_REGEX = '<li><a href="\.\./\.\./main\.htm"><span>Main&nbsp;Page<\/span></a></li>';
	if ($text =~ /$MAIN_PAGE_REGEX/)
	{
		if ($VERBOSE) { print "Changing Main Page link in '$path'.\n"; }
		$text =~  	s@$MAIN_PAGE_REGEX@<li><a href=\"main.htm\"><span>Main&nbsp;Page</span></a></li>@;	
		$FILE_HAS_BEEN_CHANGED = $TRUE;
	}
	
	my $MODULES_REGEX = '<li><a href="\.\./\.\./modules\.htm"><span>Modules</span></a></li>';
	if ($text =~ /$MODULES_REGEX/)
	{
		if ($VERBOSE) { print "Changing Modules link in '$path'.\n"; }
		$text =~  	s@$MODULES_REGEX@<li><a href="modules.htm"><span>Modules</span></a></li>@;	
		$FILE_HAS_BEEN_CHANGED = $TRUE;
	}
	
	my $NAMESPACES_REGEX = '<li><a href="\.\./\.\./namespaces\.htm"><span>Namespaces</span></a></li>';
	if ($text =~ /$NAMESPACES_REGEX/)
	{
		if ($VERBOSE) { print "Changing Namespaces link in '$path'.\n"; }
		$text =~  	s@$NAMESPACES_REGEX@<li><a href="namespaces.htm"><span>Namespaces</span></a></li>@;	
		$FILE_HAS_BEEN_CHANGED = $TRUE;
	}
		
	my $CLASSES_REGEX = '<li><a href="\.\./\.\./classes\.htm"><span>Classes</span></a></li>';
	if ($text =~ /$CLASSES_REGEX/)
	{
		if ($VERBOSE) { print "Changing Classes link in '$path'.\n"; }
		$text =~  	s@$CLASSES_REGEX@<li><a href="classes.htm"><span>Classes</span></a></li>@;	
		$FILE_HAS_BEEN_CHANGED = $TRUE;	
	}	
		
	my $FILES_REGEX = '<li><a href="\.\./\.\./files\.htm"><span>Files</span></a></li>';
	if ($text =~ /$FILES_REGEX/)
	{
		if ($VERBOSE) { print "Removed Files link in '$path'.\n"; }
		$text =~  	s@$FILES_REGEX@@;	# Remove this link.
		$FILE_HAS_BEEN_CHANGED = $TRUE;	
	}
	
	# Check that we got them all
	if ($text =~ /(.{0,10}<a href="\.\..{0,30})/) # "
	{
		print "*** WARNING in fix_links_in_file: The file '$path'\nstill needs links fixed because of this:\n\t'$1'. *** \n";
	}
		
	return ($FILE_HAS_BEEN_CHANGED, $text);
}
		
################################################################### 
sub show_global_variables
{
	print "DIRECTORY_SEPARATOR = '$DIRECTORY_SEPARATOR' \n"; 
	print "INPUT_DIRECTORY =     '$INPUT_DIRECTORY' \n"; 
	print "TITLE =               '$TITLE' \n"; 
	print "TRUE =                '$TRUE' \n"; 
	print "FALSE =               '$FALSE' \n"; 
	print "VERBOSE =             '$VERBOSE' \n"; 
	print "\n"; 
}

################################################################### 
sub change_relative_path_to_absolute # ($path)
# Given a path that is relative to this script's location, returns an absolute path.
{
	my $path = shift;
		
	if (File::Spec->file_name_is_absolute( $path ) )
	{
		# It is already absolute
		return $path;
	}
	else
	{
		my $location_of_this_script = $FindBin::Bin;
		my $absolute_path = File::Spec->rel2abs( $path, $location_of_this_script );
		
		return File::Spec->canonpath( $absolute_path ); 
	}
}

################################################################### 
sub convert_seconds_to_hms_string	# ($seconds)
{
	my $seconds = $_[0];
	
	my $hours   = int ($seconds / (60*60));
	
	my $seconds_to_convert_to_minutes = $seconds -($hours * (60*60) );
	
	my $minutes = int ($seconds_to_convert_to_minutes / 60);
	# Format number with up to 2 leading zeroes
    my $min = sprintf("%02d", $minutes);
	
	my $remaining_seconds = $seconds % 60;
	# Format number with up to 2 leading zeroes
    my $sec = sprintf("%02d", $remaining_seconds);
	
	if ($hours > 0)
	{
		return "$hours hours $min minutes $sec seconds"; 
	}
	else
	{
		return "$min minutes $sec seconds"; 
	}

#	return "$hours:$min:$sec"; 
}

###################################################################
# Slurps the entire contents of a file into a scalar vaiable.
# Returns the text.
#
# See http://www.perl.com/pub/a/2003/11/21/slurp.html
sub slurp 	# ($file)
{
	my $file = shift;
	my $file_handle = 'INPUT';
	
	local( $/ ) ;
	open( $file_handle, $file ) or die "*** FATAL ERROR: Could not read from '$file_handle'.\n";
	my $text = <$file_handle>;
	close $file_handle;
	
	return $text;
}

################################################################### 