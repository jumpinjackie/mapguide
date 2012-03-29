@echo off
echo [build]: Web Tier - fusion templates
%XCOPY% "%MG_OEM%\fusionMG" "%MG_OEM%\fusion_build" /EXCLUDE:svn_excludes.txt+%CONFIGURATION%_excludes.txt
echo [build]: Web Tier - fusion
call %ANT% deploy -Ddeploy.home="%MG_OEM%\fusion_build" -f "%MG_OEM%\fusion\build.xml"