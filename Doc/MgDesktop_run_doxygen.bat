@echo off

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
:: Copy the doxygen.css file

echo Copying doxygen.css ...

if not exist MgDesktop_WebAPIReference\		mkdir MgDesktop_WebAPIReference\
copy css\doxygen.css 							MgDesktop_WebAPIReference

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
:: Copy the XML schemas

if not exist MgDesktop_WebAPIReference\xml_schemas 	mkdir MgDesktop_WebAPIReference\xml_schemas\
xcopy /s /y "xml_schemas\*"								MgDesktop_WebAPIReference\xml_schemas\

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
:: Run Doxygen

echo Running Doxygen ...
doxygen MgDesktop_Doxyfile > MgDesktop_run_log.txt
echo Doxygen generation is complete. The error log is in 'MgDesktop_warn_log.txt'.

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
;: Post Process 

echo Running the post processor ...
perl -w PostProcessor/post_process.MgDesktop.pl

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
echo All done.

