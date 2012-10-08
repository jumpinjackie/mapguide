param($installPath, $toolsPath, $package, $project)

. (Join-Path $toolsPath "GetCsMapDictPostBuildCmd.ps1")

# Get the current Post Build Event cmd
$currentPostBuildCmd = $project.Properties.Item("PostBuildEvent").Value

# Append our post build command if it's not already there
if (!$currentPostBuildCmd.Contains($CsMapDictPostBuildCmd)) {
    $project.Properties.Item("PostBuildEvent").Value += $CsMapDictPostBuildCmd
    Write-Host "Updated Post-Build event for project"
}