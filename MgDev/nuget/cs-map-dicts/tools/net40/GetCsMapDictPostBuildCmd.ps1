$solutionDir = [System.IO.Path]::GetDirectoryName($dte.Solution.FullName) + "\"
$path = $installPath.Replace($solutionDir, "`$(SolutionDir)")
$contentDir = Join-Path $path "dictionaries"
$files = $(Join-Path $contentDir "*.*")

$CsMapDictPostBuildCmd = "
if not exist `"`$(TargetDir)Dictionaries`" md `"`$(TargetDir)Dictionaries`"
xcopy /s /y `"$files`" `"`$(TargetDir)Dictionaries`""