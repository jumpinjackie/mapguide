@echo off

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
:: Copy the doxygen.css file

echo Copying doxygen.css ...

if not exist MgOpensource_WebAPIReference\		mkdir MgOpensource_WebAPIReference\
copy css\doxygen.css 							MgOpensource_WebAPIReference

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
:: Copy the XML schemas

if not exist MgOpensource_WebAPIReference\xml_schemas 	mkdir MgOpensource_WebAPIReference\xml_schemas\
xcopy /s /y "xml_schemas\*"								MgOpensource_WebAPIReference\xml_schemas\

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
:: Run Doxygen

doxygen MgOpenSource_Doxyfile > MGOpenSource_run_log.txt
echo Doxygen generation is complete. The error log is in 'MgOpenSource_warn_log.txt'.

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
