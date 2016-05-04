MapGuide Test Suite Overview
============================

The MapGuide Test Suite contains the test data and applications that can be used to validate and verify a given installation of MapGuide

The test data in this suite is required for the MapGuide Server tests to run properly

Usage
=====

To run the test suite on Windows, run the command as follows:

run_tests.bat [-p=Platform]
              [-t=TestSuite]
              [-m=MapAgent]
              [-w=WebRoot]
              [-s=ServerPath]
              [-ws=WebServerName]
              [-wp=WebServerPort]
              [-iw]

To run the test suite on Linux, run the command as follows:

run_tests.sh [-p=Platform]
             [-t=TestSuite]
             [-m=MapAgent]
             [-w=WebRoot]
             [-s=ServerPath]

Where the parameters are:

Use Integrated PHP Web Server: -iw

Platform: -p[latform]=x86 (default)
                      x64

TestSuite: -t[est]=server (default)
                   php
                   phpagent
                   dotnet
                   java
                   all
                   
MapAgent:  -m[apagent]=<your mapagent url>

WebRoot:   -w[ebroot]=<path to your web root www>

ServerPath: -s[erverpath]=<path to your server bin directory>

WebServerName: -ws=Host name of web server

WebServerPort: -wp=Port number of web server

Requirements
============
 - An installed/pre-configured copy of MapGuide Open Source, this could be installed by the Windows installer, or configured via the InstantSetup utility
 - Java SDK (if running the Java test suite). It must be the same bitness as your MapGuide Server. If you have a 32-bit MapGuide installed, you must use a 32-bit JDK
 - Apache Ant (if running the Java test suite)
 - .net Framework 4.5 (if running the .net test suite)

General Notes (Windows):
========================

If your MapGuide Server is running as a service, it has to be stopped when running any test suite, as they will start the mgserver executable before
running their tests, and tear it down when finished.

You must have the following environment variables set:
 - JAVA_HOME points to an installation of the Java SDK of the same bitness as the installation of MapGuide you are testing against
 - PATH contains the path to your installation of Apache Ant

You must have a copy of the Sheboygan dataset in the UnitTest/TestData/Samples/Sheboygan directory. You can either download a copy and put it in this
directory, or if you have 7-zip in the PATH environment variable, you can run the build.bat file in this direcotry to build Sheboygan.mgp from the existing
data files.

If running on an installation of MapGuide installed with the Windows installer, you will need elevated privileges to run this batch file in order
to run the server test suite as part of preparation the batch file will have to copy all the required test data to a location under Program Files, an
area that needs elevated privileges to write files to.

Depending on your installation configuration you may not be able to run certain test suites. For example, if you did not enable Java for your MapGuide
installation, don't expect the Java test suite to run or pass. As a general rule, the following test suites can be run under the given conditions:

 * server: always (provided you have elevated privileges for a non-InstantSetup installation)
 * php: always (provided you supply the correct WebRoot path)
 * dotnet: You must supply the correct WebRoot path and mapviewernet\bin is assumed to exist under this path and contains the required .net assemblies and dlls
 * java: You must supply the correct WebRoot path and WEB-INF\lib is assumed to exist under this path and contains the required JAR files
 * all: All of the above conditions must be satisfied