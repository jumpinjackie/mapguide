@echo off
echo [build]: Web Tier - fusion templates
%XCOPY% "%MG_OEM%\fusionMG" "%MG_OEM%\fusion_build" /EXCLUDE:svn_excludes.txt+%CONFIGURATION%_excludes.txt
echo [build]: Web Tier - Clean fusion
call %ANT% clean -f "%MG_OEM%\fusion\build.xml"
echo [build]: Web Tier - Build Fusion API docs
call %ANT% docs -f "%MG_OEM%\fusion\build.xml"
echo [build]: Web Tier - fusion
call %ANT% deploy -Ddeploy.home="%MG_OEM%\fusion_build" -f "%MG_OEM%\fusion\build.xml"
echo [build]: Fusion - build pack
call %ANT% selfbuild -Dselfbuild.home="%MG_OUTPUT%\fusion-selfbuild" -f "%MG_OEM%\fusion\build.xml"