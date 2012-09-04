$solutionDir = [System.IO.Path]::GetDirectoryName($dte.Solution.FullName) + "\"
$path = $installPath.Replace($solutionDir, "`$(SolutionDir)")

$MgApiBasePostBuildCmd = "
if not exist `"`$(TargetDir)Dictionaries`" md `"`$(TargetDir)Dictionaries`"
xcopy /s /y `"`$(ProjectDir)mapguide-api-base\Dictionaries`" `"`$(TargetDir)Dictionaries`"
copy /y `"`$(ProjectDir)mapguide-api-base\*.dll`" `"`$(TargetDir)`""