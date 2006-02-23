echo child::para[position\(\)=3][attribute::type="warning"] selects the third para child of the context node if that child has a type attribute with value warning
../xgrep "/xgrep-test/child::para[position()=3][attribute::type='warning']" xgrep-test1.xml
../xgrep "/xgrep-test/child::para[position()=1][attribute::type='warning']" xgrep-test1.xml

