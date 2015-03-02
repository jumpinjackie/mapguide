<#
.SYNOPSIS
    Builds the FDO codebase
.DESCRIPTION
    This PowerShell script will build the entire FDO codebase, which consists of
        - Oem components
        - Server tier
        - Web tier
        - Documentation
        - Windows Installer
        - mg-desktop
    This script requires PowerShell Community Extensions installed
.EXAMPLE
    C:\PS> .\build.ps1 -platform x64 -build_root 'D:\fdo-trunk' -output '.\Release64'
    Build the full source (located at D:\fdo-trunk) for 64-bit. The build output will be at (.\Release64)
.EXAMPLE
    C:\PS> .\build.ps1 -platform x86 -build_root 'D:\fdo-trunk' -output '.\Release'
    Build the full source (located at D:\fdo-trunk) for 32-bit. The build output will be at (.\Release64)
.LINK
    Project Home: http://mapguide.osgeo.org
    PowerShell Community Extensions: https://pscx.codeplex.com/
.NOTES
    Author: Jackie Ng
#>
param(
    [Parameter(HelpMessage = "The CPU platform to build for")]
    [ValidateSet("Win32", "x64")]
    [Alias("p")]
    $platform = "Win32",
    [Parameter(HelpMessage = "The build configuration to build for")]
    [ValidateSet("debug", "release")]
    [Alias("c")]
    [string]
    $config = "release",
    [Parameter(HelpMessage = "The action to perform")]
    [ValidateSet("build", "install", "buildinstall", "clean")]
    [Alias("a")]
    [string]
    $action = "buildinstall",
    [Parameter(HelpMessage = "The components to build")]
    [ValidateSet("thirdparty", "fdo", "all")]
    [Alias("w")]
    [string]
    $component = "all",
    [Parameter(HelpMessage = "The directory where compiled binaries will be copied to. If relative, the path will be relative to the build root")]
    [Alias("o")]
    [string]
    $output = ".\Release",
    [Parameter(HelpMessage = "The directory where the FDO source resides in")]
    [string]
    $build_root = ".\",
    [Parameter(HelpMessage = "The directory where the MySQL SDK resides in")]
    [string]
    $fdo_mysql = "",
    [Parameter(HelpMessage = "The directory where the PostgreSQL SDK resides in")]
    [string]
    $fdo_postgresql = "",
    [Parameter(HelpMessage = "The directory where the Oracle Instant Client SDK resides in")]
    [string]
    $fdo_oracle = ""
)

Function MakeDirIfNotExists([string]$dir) {
    if (-Not(Test-Path $dir)) {
        New-Item -ItemType directory -Path $dir
    }
}

Try
{
    Import-Module Pscx

    $env:FDOMYSQL = $fdo_mysql
    $env:FDOORACLE = $fdo_oracle
    $env:FDOPOSTGRESQL = $fdo_postgresql

    $cwd = (Get-Item -Path "$build_root" -Verbose).FullName
    pushd $cwd
    Write-Host "Entering $cwd"

    If ($platform.Equals("x64")) {
        Write-Host "Setting FDO build environment for x64"
        Invoke-BatchFile setenvironment.bat x86_amd64
    } Else {
        Write-Host "Setting FDO build environment for x86"
        Invoke-BatchFile setenvironment.bat x86
    }
    if (-Not $?) {
        throw "Failure invoking setenvironment.bat"
    }

    # Check perl
    Write-Host "Checking for perl"
    if (-Not (Get-Command perl -errorAction SilentlyContinue)) {
        throw "perl not found. Ensure perl bin directory is in your PATH environment variable"
    }

    # Normalize the output path
    if (-Not [System.IO.Path]::IsPathRooted($output)) {
        $output = [System.IO.Path]::GetFullPath((Join-Path $cwd $output));
    }

    Write-Host "========= Action Summary ============"
    Write-Host "Platform: $platform"
    Write-Host "Configuration: $config"
    Write-Host "Action: $action"
    Write-Host "Component: $component"
    Write-Host "Output Dir: $output"
    Write-Host "Build Root: $build_root"
    Write-Host "======== Env var summary ============"
    Write-Host "FDOMYSQL: $env:FDOMYSQL"
    Write-Host "FDOPOSTGRESQL: $env:FDOPOSTGRESQL"
    Write-Host "FDOORACLE: $env:FDOORACLE"
    Write-Host "====================================="

    switch ($component) {
        "thirdparty" {
            & .\build_thirdparty.bat "-a=$action" "-o=$output" "-p=$platform" "-c=$config"
        }
        "fdo" {
            & .\build.bat "-a=$action" "-o=$output" "-p=$platform" "-c=$config"
        }
        "all" {
            & .\build_thirdparty.bat "-a=$action" "-o=$output" "-p=$platform" "-c=$config"
            & .\build.bat "-a=$action" "-o=$output" "-p=$platform" "-c=$config"
        }
    }
}
Finally
{
    popd
}