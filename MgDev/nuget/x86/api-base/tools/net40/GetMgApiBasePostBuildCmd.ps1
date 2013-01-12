$solutionDir = [System.IO.Path]::GetDirectoryName($dte.Solution.FullName) + "\"
$path = $installPath.Replace($solutionDir, "`$(SolutionDir)")
$contentDir = Join-Path $path "mapguide-api-base"
$files = $(Join-Path $contentDir "*.*")

$MgApiBasePostBuildCmd = "
xcopy /s /y `"$files`" `"`$(TargetDir)`""