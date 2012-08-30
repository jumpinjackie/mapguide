SampleExtension readme
======================

SampleExtension is an AppLayout extension that demonstrates usages of the mg-desktop API.

It is effectively the "MapGuide Developer's Guide" samples converted over for use inside mg-desktop. If you ever look at the code, you will see that it isn't really that much different (just ASP.net-isms replaced with WinForms-isms)

Building the SampleExtension
============================

In Visual Studio, open the SampleExtension.csproj if you downloaded the .net 2.0 binary distribution (or SampleExtension.Net40.csproj if you downloaded the .net 4.0 binary distribution)

Fix the broken references with the matching dlls from your mg-desktop binary distribution. This should allow you to build the SampleExtension project.

Once built, you will have a SampleExtension.dll in your output directory, the components defined in this dll can then be referenced from within the AppLayout engine. The Sheboygan.AppLayout already does this.