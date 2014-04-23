param($installPath, $toolsPath, $package, $project)

# Set Copy Local to false for all mg-desktop references
foreach ($reference in $project.Object.References)
{
    if ($reference.Name -eq "OSGeo.MapGuide.Foundation" -or 
        $reference.Name -eq "OSGeo.MapGuide.Geometry" -or
        $reference.Name -eq "OSGeo.MapGuide.PlatformBase" -or
        $reference.Name -eq "OSGeo.MapGuide.Desktop" -or
        $reference.Name -eq "OSGeo.MapGuide.Viewer" -or
        $reference.Name -eq "OSGeo.MapGuide.Viewer.Desktop")
    {
        $reference.CopyLocal = $true
    }
}
Write-Host "Set <Copy Local> = true for all mg-desktop references"

. (Join-Path $toolsPath "GetMgDesktopPostBuildCmd.ps1")

# Get the current Post Build Event cmd
$currentPostBuildCmd = $project.Properties.Item("PostBuildEvent").Value

# Append our post build command if it's not already there
if (!$currentPostBuildCmd.Contains($MgDesktopPostBuildCmd)) {
    $project.Properties.Item("PostBuildEvent").Value += $MgDesktopPostBuildCmd
    Write-Host "Updated Post-Build event for project"
}