param($installPath, $toolsPath, $package, $project)

# Set Copy Local to false for all mg-desktop references
foreach ($reference in $project.Object.References)
{
    if ($reference.Name -eq "OSGeo.MapGuide.Foundation" -or 
        $reference.Name -eq "OSGeo.MapGuide.Geometry" -or
        $reference.Name -eq "OSGeo.MapGuide.PlatformBase")
    {
        $reference.CopyLocal = $true
    }
}
Write-Host "Set <Copy Local> = true for all mapguide-api-base references"

. (Join-Path $toolsPath "GetMgApiBasePostBuildCmd.ps1")

# Get the current Post Build Event cmd
$currentPostBuildCmd = $project.Properties.Item("PostBuildEvent").Value

# Append our post build command if it's not already there
if (!$currentPostBuildCmd.Contains($MgApiBasePostBuildCmd)) {
    $project.Properties.Item("PostBuildEvent").Value += $MgApiBasePostBuildCmd
    Write-Host "Updated mapguide-api-base Post-Build event for project"
}