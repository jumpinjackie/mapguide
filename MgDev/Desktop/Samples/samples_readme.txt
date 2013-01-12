mg-desktop samples README
=========================

The following samples all require the Sheboygan sample dataset. For these samples to build, copy your mg-desktop files (and its subfolders) into the Lib directory, you must use the correct build of mg-desktop for the version of VS you are using to open this solution file:

 - MgDesktopSamples.sln: mg-desktop .net Framework 2.0, VC9 build
 - MgDesktopSamples_VS2010.sln: mg-desktop .net Framework 4.0, VC10 build

Basically, you can't use a .net 4.0 build of mg-desktop on a VS2008 solution

Note that in order to use mg-desktop, your main executable must have CPU architecture set to x86 (because these are .net wrappers to 32-bit unmanaged dlls). All the executable projects in this solution have their CPU architecture set to x86

BasicSample
===========

Demonstrates a basic Windows Forms Application that embeds the mg-desktop map viewer

AdvancedSample
==============

Demonstrates a Windows Forms Application that embeds the mg-desktop map viewer and shows how to do things like measuring and digitizing

FreeFormSample
==============

Demonstrates that the MgMapViewer control is like any other WinForms control, and that you can have your toolbar buttons on top of the viewer control itself, allowing for a minimal user interface.

SampleExtension
===============

The MapGuide Developer's Guide fully ported to mg-desktop in the form of an mg-desktop AppLayout extension module. This is the most comprehensive demonstration of the mg-desktop API

To run this particular sample extension, build it in Visual Studio, then run from the command-line of the project's output directory:

MgAppLayout.exe Sheboygan.AppLayout

The Sheboygan.AppLayout document will instruct MgAppLayout.exe to load your SampleExtension and invoke the components within.