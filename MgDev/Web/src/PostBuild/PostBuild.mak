CopyResources : ..\..\..\Common\MapGuideCommon\Resources\mapguide_en.res
    if NOT EXIST ..\mapagent\Resources\nul mkdir ..\mapagent\Resources
    xcopy /r /d /y "..\..\..\Common\MapGuideCommon\Resources\mapguide_en.res" ..\mapagent\Resources\

debug : CopyResources

release : CopyResources

cleandebug :

cleanrelease :
