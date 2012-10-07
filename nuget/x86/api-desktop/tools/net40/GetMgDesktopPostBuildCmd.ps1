$solutionDir = [System.IO.Path]::GetDirectoryName($dte.Solution.FullName) + "\"
$path = $installPath.Replace($solutionDir, "`$(SolutionDir)")

$MgDesktopPostBuildCmd = "
if not exist `"`$(TargetDir)FDO`" md `"`$(TargetDir)FDO`"
xcopy /s /y `"`$(ProjectDir)mg-desktop\FDO`" `"`$(TargetDir)FDO`"
if not exist `"`$(TargetDir)Resources`" md `"`$(TargetDir)Resources`"
xcopy /s /y `"`$(ProjectDir)mg-desktop\Resources`" `"`$(TargetDir)Resources`"
if not exist `"`$(TargetDir)Schema`" md `"`$(TargetDir)Schema`"
xcopy /s /y `"`$(ProjectDir)mg-desktop\Schema`" `"`$(TargetDir)Schema`"
copy /y `"`$(ProjectDir)mg-desktop\*.dll`" `"`$(TargetDir)`"
copy /y `"`$(ProjectDir)mg-desktop\Platform.ini`" `"`$(TargetDir)`""