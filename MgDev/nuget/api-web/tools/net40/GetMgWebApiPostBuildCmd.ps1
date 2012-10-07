$solutionDir = [System.IO.Path]::GetDirectoryName($dte.Solution.FullName) + "\"
$path = $installPath.Replace($solutionDir, "`$(SolutionDir)")

$MgWebApiPostBuildCmd = "
copy /y `"`$(ProjectDir)mapguide-api-web\*.dll`" `"`$(TargetDir)`""