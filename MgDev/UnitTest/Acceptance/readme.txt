MapGuide End-to-End Test Readme
===============================

This is a suite of end-to-end automated browser tests that will allow one to easily verify a given 
installation of MapGuide satisfies the minimum validation criteria for a functional build (in addition to passing unit tests)

Requirements
============

 - Node.js
 - Java SDK (1.7)
 - Google Chrome (latest)
 - Mozilla Firefox (latest)
 - Internet Explorer (latest)
 - An installation of MapGuide with the loaded Sheboygan dataset

First Time Setup
================

Run "npm install" to install the required node dependencies
    - Nightwatch.js
    - Nightwatch.js HTML reporter
    - Selenium
        - Standalone Driver
        - IE Driver
        - Chrome Driver

Ensure that MapGuide is running and the Sheboygan dataset has been loaded

Running the Tests
=================

"./node_modules/.bin/nightwatch" --env [environment] (--tag [tag1], --tag [tag2] ...)

    - environment: A comma-delimited list of environments, which can be:
        - default (using Google Chrome)
        - firefox
        - ie
        
    - tag: A series of tagged test suites (specify a --tag for each tag), which can be:
        - ajax (all suites)
            - navigation
            - ui
            - tools
        - fusion (all suites)

Parallel test execution
=======================

To enable parallel execution, set an environment variable named MG_PARALLEL_TEST=1 before running the test suite.

Parallel execution will farm out each individual test suite (.js file) to a separate worker, all results are collated at the end.

Parallel execution has no effect if running for multiple environments.