echo 'Select a repeating node with a value from the XML document (should succeed using // axis)'
../xgrep '//integerElement[number(//n)]' xgrep-test56.xml
