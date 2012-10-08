$solutionDir = [System.IO.Path]::GetDirectoryName($dte.Solution.FullName) + "\"
$path = $installPath.Replace($solutionDir, "`$(SolutionDir)")
$contentDir = Join-Path $path "mapguide-api-web"
$files = $(Join-Path $contentDir "*.*")

$MgWebApiPostBuildCmd = "
xcopy /s /y `"$files`" `"`$(TargetDir)`""