param($installPath, $toolsPath, $package, $project)

. (Join-Path $toolsPath "GetMgApiBasePostBuildCmd.ps1")

# Get the current Post Build Event cmd
$currentPostBuildCmd = $project.Properties.Item("PostBuildEvent").Value

# Remove our post build command from it (if it's there)
$project.Properties.Item("PostBuildEvent").Value = $currentPostBuildCmd.Replace($MgApiBasePostBuildCmd, "")
Write-Host "Removed mapguide-api-base post-build event"