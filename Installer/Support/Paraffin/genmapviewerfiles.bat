paraffin -dir D:\OSGeo\Installer\Support\Web\mapviewer -custom MAPVIEWERDATAFILES -alias ..\..\Support\Web\mapviewer -dirref WEBROOTLOCATION -guids -multiple -direXclude .svn incMapViewerDataFiles.wxs 
          
paraffin -dir D:\OSGeo\mapguide\Web\src\viewerfiles -custom MAPVIEWERFILES -alias ..\Web\src\viewerfiles -dirref WEBROOTLOCATION -guids -multiple -direXclude .svn incMapViewerFiles.wxs 

paraffin -dir D:\OSGeo\mapguide\Web\src\stdicons -custom MAPVIEWERSTDICONFILES -alias ..\Web\stdicons -dirref WEBROOTLOCATION -guids -multiple -direXclude .svn incMapViewerStdiconFiles.wxs 

paraffin -dir D:\OSGeo\mapguide\Web\src\schemareport -custom MAPVIEWERSCHEMAREPORTFILES -alias ..\Web\schemareport -dirref WEBROOTLOCATION -guids -multiple -direXclude .svn incMapViewerSchemareportFiles.wxs 

paraffin -dir D:\OSGeo\mapguide\Web\src\mapadmin -custom MAPVIEWERMAPADMINFILES -alias ..\Web\mapadmin -dirref WEBROOTLOCATION -guids -multiple -direXclude .svn incMapViewerMapAdminFiles.wxs 

paraffin -dir D:\OSGeo\mapguide\Web\src\localized -custom MAPVIEWERLOCALIZEDFILES -alias ..\Web\localized -dirref WEBROOTLOCATION -guids -multiple -direXclude .svn incMapViewerLocalizedFiles.wxs 
