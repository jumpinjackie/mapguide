@echo off

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
:: Copy the doxygen.css file

if exist "C:\Program Files\doxygen\html\doxygen.css" goto CopyCSS
	echo ERROR: Could not find 'doxygen.css' in 'C:\Program Files\doxygen\html'.
	echo Either copy it manually to 'MgOpenSource_HTML', or change this batch file to the correct path for your machine.
	echo (The API Reference has not been built.)
	exit

:CopyCSS
echo Copying doxygen.css ...
if not exist MgOpensource_WebAPIReference			mkdir MgOpensource_WebAPIReference\
copy "C:\Program Files\doxygen\html\doxygen.css" 	MgOpensource_WebAPIReference

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
:: Copy the XML schemas

if not exist MgOpensource_WebAPIReference\xml_schemas 	mkdir MgOpensource_WebAPIReference\xml_schemas\
xcopy /s /y "xml_schemas\*"								MgOpensource_WebAPIReference\xml_schemas\

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
:: Run Doxygen
doxygen MgOpenSource_Doxyfile
echo Doxygen generation is complete.

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
