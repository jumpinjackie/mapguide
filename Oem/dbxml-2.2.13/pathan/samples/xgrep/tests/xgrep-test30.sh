echo child::*[self::chapter or self::appendix][position\(\)=last\(\)] selects the last chapter or appendix child of the context node
../xgrep "/xgrep-test/doc/child::*[self::chapter or self::appendix][position()=last()]" xgrep-test1.xml

