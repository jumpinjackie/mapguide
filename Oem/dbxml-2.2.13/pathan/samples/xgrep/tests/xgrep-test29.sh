echo child::*[self::chapter or self::appendix] selects the chapter and appendix children of the context node
../xgrep "/xgrep-test/doc/child::*[self::chapter or self::appendix]" xgrep-test1.xml

