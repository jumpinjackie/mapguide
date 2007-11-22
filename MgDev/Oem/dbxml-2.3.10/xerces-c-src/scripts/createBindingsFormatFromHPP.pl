#
# Copyright 1999-2002,2004 The Apache Software Foundation.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#      http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#

my $filename = $ARGV[0];
open(IN,"< $filename") || die("Couldn't open input file: $filename");

while (<IN>) {
  if ($_=~/^\s*class\s+[A-Z0-9_]+_EXPORT\s+([^\s]+)/) {
    $classname = $1;
    last;
  }
}

open(OUT,">out.html") || die("Could not write to out.html");

print OUT <<"END1";
<h3>
<font color="#000000">$filename:</font></h3>

<div id="$classname"><font face="Courier New,Courier"><font size=-1>class
$classname</font></font>
<table>
END1

while (<IN>) {
  if ($_=~/^\s*public\s*:/) {
    print OUT <<"END5";

<tr ALIGN=LEFT VALIGN=TOP>
<td><font face="Courier New,Courier"><font size=-1>{</font></font></td>

<td></td>

<td></td>

<td></td>

</tr>

<tr ALIGN=LEFT VALIGN=TOP>
<td><font face="Courier New,Courier"><font size=-1>public:</font></font></td>

<td></td>

<td></td>

<td></td>

</tr>
END5
    last;
  }
}

while (<IN>) {
  if ($_=~/^\s*(virtual)\s+([^\s\(][^\(]*[^\s\(])\s+([^\s\(]+\(.*)/) {
    $a=$1;
    $b=$2;
    $c=$3;
    while ($c!~/\)/) {
      $c.=<IN>;
    }
    print OUT <<"END";

<tr ALIGN=LEFT VALIGN=TOP>
<td></td>

<td><font face="Courier New,Courier"><font size=-1>$a</font></font></td>

<td><font face="Courier New,Courier"><font size=-1>$b</font></font></td>

<td><font face="Courier New,Courier"><font size=-1>$c</font></font></td>
</tr>
END
  } elsif ($_=~/^\s*(enum)\s+([^\s]+)\s*{/) {
    print OUT <<"END2";

<tr ALIGN=LEFT VALIGN=TOP>
<td></td>

<td><font face="Courier New,Courier"><font size=-1>$1</font></font></td>

<td><font face="Courier New,Courier"><font size=-1>$2 {</font></font></td>

<td></td>
</tr>
END2

    while (<IN>) {
      if ($_=~/([^\s]+)\s*=\s*([^\s,]+),?(\s|$)/) {
        print OUT <<"END3";
<tr ALIGN=LEFT VALIGN=TOP>
<td></td>

<td></td>

<td><font face="Courier New,Courier"><font size=-1>$1</font></font></td>

<td><font face="Courier New,Courier"><font size=-1>= $2,</font></font></td>
</tr>
END3
      }
      if ($_=~/}\s*;/) {
        print OUT <<"END4";
<tr ALIGN=LEFT VALIGN=TOP>
<td><font face="Courier New,Courier"><font size=-1>};</font></font></td>

<td></td>

<td></td>

<td></td>
</tr>
END4
        last;
      }
    }
  }

#  enum ExceptionCode {
#    INVALID_EXPRESSION_ERR = 51,
#    TYPE_ERR = 52,
#  };
}

print OUT <<"END6";
<tr ALIGN=LEFT VALIGN=TOP>
<td><font face="Courier New,Courier"><font size=-1>};</font></font></td>

<td></td>

<td></td>

<td></td>
</tr>
</table>
END6

close(OUT);
close(IN);
