#!/usr/bin/perl -w
#
# makeUnicodeXerString.pl:
#
# Copyright (c) 2003 DecisionSoft Ltd.
# Stephen White (swhite) Tue Jan 14 15:41:46 2003
#
# Id: $Id: makeUnicodeXerString.pl,v 1.1 2004/07/21 09:23:02 jpcs Exp $

use vars qw($opt_h);
use Getopt::Std;

my %charMapping = ( ':', 'chColon',
                    '\\', 'chBackSlash',
                    '[', 'chOpenSquare',
                    ']', 'chCloseSquare',
                    '{', 'chOpenCurly',
                    '}', 'chCloseCurly',
                    '?', 'chQuestion',
                    '(', 'chOpenParen',
                    ')', 'chCloseParen',
                    '/', 'chForwardSlash',
                    '*', 'chAsterisk',
                    '+', 'chPlus',
                    '.', 'chPeriod',
                    '-', 'chDash',
                    '|', 'chPipe',
                    '_', 'chUnderscore',
                    ',', 'chComma',
                    '&', 'chAmpersand',
                    '0', 'chDigit_0', '1', 'chDigit_1', '2', 'chDigit_2',
                    '3', 'chDigit_3', '4', 'chDigit_4', '5', 'chDigit_5',
                    '6', 'chDigit_6', '7', 'chDigit_7', '8', 'chDigit_8',
                    '9', 'chDigit_9');

#
# usage: display usage message
#
sub usage() {
    print<<EOF;
usage: $0 [ options ] word

Takes a word and produces a static XMLCh * definition for it.

Options:
    -h Displays this help message

EOF
    exit(1);
}

#
# main:
#

getopts("h");

if ($opt_h or @ARGV == 0) {
    usage();
}

my $word = $ARGV[0];

print "{ ";

while ($word=~s/^(.)//) {
  if (defined($charMapping{$1})) {
    $ch = $charMapping{$1};
  } else {
    $ch = $1;
    if ($ch=~/[A-Za-z]/) {
      $ch = "chLatin_$ch";
    } else {
      $ch = "UNKNOWN_CHAR_$ch";
    }
  }
  print "XERCES_CPP_NAMESPACE_QUALIFIER $ch, ";
}

print "XERCES_CPP_NAMESPACE_QUALIFIER chNull };\n";
