$solutionDir = [System.IO.Path]::GetDirectoryName($dte.Solution.FullName) + "\"
$path = $installPath.Replace($solutionDir, "`$(SolutionDir)")
$contentDir = Join-Path $path "mg-desktop"
$files = $(Join-Path $contentDir "*.*")

$MgDesktopPostBuildCmd = "
xcopy /s /y `"$files`" `"`$(TargetDir)`""