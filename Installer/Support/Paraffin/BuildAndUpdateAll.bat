cmd /c .\genapachefiles.bat
cmd /c .\genbinfiles.bat
cmd /c gencsmapdictfiles.bat
cmd /c genfdofiles.bat
cmd /c genmaestrobinfiles.bat
cmd /c genmapagentfiles.bat
cmd /c genmapvieweraspxfiles.bat
cmd /c genmapviewerfiles.bat
cmd /c genmapviewerjspfiles.bat
cmd /c genmapviewerphpfiles.bat
cmd /c genphpfiles.bat
cmd /c genrepadminfiles.bat
cmd /c genresourcesfiles.bat
cmd /c genschemafiles.bat
cmd /c gentomcatfiles.bat
cmd /c genwfsfiles.bat
cmd /c genwmsfiles.bat

copy /Y  incBinFiles.wxs "..\..\Libraries\MapGuide Server\FileIncludes\"
copy /Y  incRepositoryAdminFiles.wxs "..\..\Libraries\MapGuide Server\FileIncludes\"
copy /Y  incResourcesFiles.wxs "..\..\Libraries\MapGuide Server\FileIncludes\"
copy /Y  incSchemaFiles.wxs "..\..\Libraries\MapGuide Server\FileIncludes\"
copy /Y  incWfsFiles.wxs "..\..\Libraries\MapGuide Server\FileIncludes\"
copy /Y  incWmsFiles.wxs "..\..\Libraries\MapGuide Server\FileIncludes\"

copy /Y  incCSMapDictionaryFiles.wxs "..\..\Libraries\CS Map\FileIncludes\"

copy /Y  incFdoFiles.wxs "..\..\Libraries\FDO\FileIncludes\"

copy /Y  incMaestroBinFiles.wxs "..\..\Libraries\Maestro\FileIncludes\"

copy /Y  incApacheFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incMapAgentFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incMapViewerAspxFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incMapViewerDataFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incMapViewerFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incMapViewerJspFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incMapViewerLocalizedFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incMapViewerMapAdminFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incMapViewerPhpFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incMapViewerSchemareportFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incMapViewerStdiconFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incPhpFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"
copy /Y  incTomcatFiles.wxs "..\..\Libraries\MapGuide Web Extensions\FileIncludes\"

pause