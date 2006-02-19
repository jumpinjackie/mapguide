echo self::para selects the context node if it is a para element, and otherwise selects nothing
../xgrep "/xgrep-test/para/self::para" xgrep-test1.xml
../xgrep "/xgrep-test/self::para" xgrep-test1.xml

