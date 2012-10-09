param($installPath, $toolsPath, $package, $project)

. (Join-Path $toolsPath "GetCsMapDictPostBuildCmd.ps1")

# Get the current Post Build Event cmd
$currentPostBuildCmd = $project.Properties.Item("PostBuildEvent").Value

# Append our post build command if it's not already there
if (!$currentPostBuildCmd.Contains($CsMapDictPostBuildCmd)) {
    $project.Properties.Item("PostBuildEvent").Value += $CsMapDictPostBuildCmd
    Write-Host "Updated Post-Build event for project"
}

$project.ProjectItems | ForEach { if ($_.Name -eq "cs-map-dummy-C008508B-11C9-40D0-B0CB-4AA35B6EE010.txt") { $_.Remove() } }
$projectPath = Split-Path $project.FullName -Parent
Join-Path $projectPath "cs-map-dummy-C008508B-11C9-40D0-B0CB-4AA35B6EE010.txt" | Remove-Item