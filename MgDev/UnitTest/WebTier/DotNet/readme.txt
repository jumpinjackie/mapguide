This is the .net port of the PHP test runner for unit testing the language bindings to the MapGuide Web API.

This test runner differs from the PHP one in the following ways:

 - It does not support test update/generation. Use the existing PHP runner to do that.
 - Does not support the HTTP API tests. The PHP test runner already covers this and duplicating it in .net serves no real purpose.
 - It is strictly for running from the command-line and not within a web browser.