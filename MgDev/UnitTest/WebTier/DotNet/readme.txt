About
=====

This is the .net port of the PHP test runner for unit testing the language bindings to the MapGuide Web API.

This test runner differs from the PHP one in the following ways:

 - It does not support test update/generation. Use the existing PHP runner to do that.
 - Does not support the HTTP API tests. The PHP test runner already covers this and duplicating it in .net serves no real purpose.
 - It is strictly for running from the command-line and not within a web browser.

Running the test suite
======================

1. Run the MapGuide Server
2. Build the DotNet.sln
3. Run the MgTestRunner like so:

 MgTestRunner.exe "../../../web/src/webconfig.ini" "../../../Oem/CsMap/Dictionaries"
 
The paths in those arguments are relative to the directory containing the MgTestRunner.exe executable.

Test results (and failures) are written to UnitTest.log. The return code of MgTestRunner.exe is the number of failed tests. A return code of 0 means all tests have passed.