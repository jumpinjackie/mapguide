Java Unit Test README
=====================

This is the test suite for the Java bindings to the MapGuide API.

This test suite exercises the enhanced Java wrapper (MapGuideApiEx.jar) and not the original (MapGuideApi.jar)

Requirements
============

 - You must have already built the Oem/Server/Web components of MapGuide
 - Apache Ant
 - ant-contrib 1.0b2 or higher

Running the tests
=================

 1. Start the MapGuide Server process
 2. Run the following ant command: ant check

Unit tests results will be logged to unittest.log

The default ant build.xml properties is configured for 64-bit release configuration of MapGuide. Where applicable, you should override these properties to match your specific build configuration.