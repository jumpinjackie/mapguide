#!/usr/bin/python
# Generates test.xml and base files for long string testing
# Argument is the number of nested nodes in test.xml
# Try it with arg > 16000 or so.
from sys import argv
test=open("test.xml", "w")
base=open("base", "w")
if len(argv) < 2 :
  max = 5
else : max = int(argv[1])
for i in range(0,max) :
  base.write("/e")
  base.write(str(i))
  test.write("<e")
  test.write(str(i))
  test.write(">")
for i in range(max-1,-1,-1) :
  test.write("</e")
  test.write(str(i))
  test.write(">")
base.write("\n<e")
base.write(str(max-1))
base.write("/>\n%\n")
