CopyResources : ..\..\..\Common\MapGuideCommon\Resources\mapguide_en.res
    if NOT EXIST ..\mapagent\Resources\nul mkdir ..\mapagent\Resources
    xcopy /r /d /y "..\..\..\Common\MapGuideCommon\Resources\mapguide_en.res" ..\mapagent\Resources\

debug : CopyResources

release : CopyResources

cleandebug :

cleanrelease :

CopyResources64 : ..\..\..\Common\MapGuideCommon\Resources\mapguide_en.res
    if NOT EXIST ..\mapagent64\Resources\nul mkdir ..\mapagent64\Resources
    xcopy /r /d /y "..\..\..\Common\MapGuideCommon\Resources\mapguide_en.res" ..\mapagent64\Resources\

debug64 : CopyResources64

release64 : CopyResources64

cleandebug64 :

cleanrelease64 :
