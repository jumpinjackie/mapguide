echo 'There are two nodes with Intensity >= 250 and Intensity <= 300'
../xgrep '//Cue/*[ . >= 250 and . <= 300 ]' xgrep-test1.xml
