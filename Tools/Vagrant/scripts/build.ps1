<#
.SYNOPSIS
    Builds the MapGuide Open Source codebase
.DESCRIPTION
    This PowerShell script will build the entire MapGuide Open Source codebase, which consists of
        - Oem components
        - Server tier
        - Web tier
        - Documentation
        - Windows Installer
        - mg-desktop
    This script requires PowerShell Community Extensions installed
.EXAMPLE
    C:\PS> .\build.ps1 -platform x64 -build_root 'D:\mg-trunk\MgDev' -java_home 'C:\Program Files\Java\jdk1.7.0_71' -output '.\Release64'
    Build the full source (located at D:\mg-trunk\MgDev) for 64-bit using the JDK at (C:\Program Files\Java\jdk1.7.0_71). The build output will be at (.\Release64)
.EXAMPLE
    C:\PS> .\build.ps1 -platform x86 -build_root 'D:\mg-trunk\MgDev' -java_home 'C:\Program Files (x86)\Java\jdk1.7.0_71' -output '.\Release'
    Build the full source (located at D:\mg-trunk\MgDev) for 32-bit using the JDK at (C:\Program Files (x86)\Java\jdk1.7.0_71). The build output will be at (.\Release64)
.LINK
    Project Home: http://mapguide.osgeo.org
    PowerShell Community Extensions: https://pscx.codeplex.com/
.NOTES
    Author: Jackie Ng
#>
param (
    [Parameter(HelpMessage = "The CPU platform to build for")]
    [ValidateSet("x86", "x64")]
    [Alias("p")]
    [string]
    $platform = "x86",
    [Parameter(HelpMessage = "The build configuration to build for")]
    [ValidateSet("Debug", "Release")]
    [Alias("c")]
    [string]
    $config = "Release",
    [Parameter(HelpMessage = "The action to perform")]
    [ValidateSet("build", "install", "buildinstall", "clean", "installprepare")]
    [Alias("a")]
    [string]
    $action = "buildinstall",
    [Parameter(HelpMessage = "The components to build")]
    [ValidateSet("all", "oem", "server", "web", "installer", "desktop", "doc")]
    [Alias("w")]
    [string]
    $component = "all",
    [Parameter(HelpMessage = "The directory where compiled binaries will be copied to. If relative, the path will be relative to the build root")]
    [Alias("o")]
    [string]
    $output = ".\Release",
    [Parameter(HelpMessage = "The directory where the MapGuide source resides in")]
    [string]
    $build_root = ".\",
    [Parameter(HelpMessage = "The directory where the MapGuide Installer source resides in")]
    [string]
    $install_root = "..\Installer",
    [Parameter(HelpMessage = "The path to the Java SDK required for building the Java API bindings. If not specified, the value of the JAVA_HOME environment variable will be used")]
    [Alias("j")]
    [string]
    $java_home = "",
    [string]
    $doxygen_path = "C:\Program Files\doxygen\bin",
    [string]
    $ant_path = "C:\apache-ant\bin",
    [string]
    $graphviz_path = "C:\Program Files (x86)\Graphviz2.30\bin",
    [string]
    $artifact_path = ".\artifacts",
    [int]
    $major = 3,
    [int]
    $minor = 0,
    [int]
    $build = 0,
    [int]
    $svnrev = 0,
    [string]
    $label = "Release"
)

Function EnvSetup([string]$plat, [string]$phpVer, [string]$apacheVer)
{
    Try
    {
        pushd .\Oem\LinuxApt
        # If any of these files don't exist, we unpack the httpd source
        $apacheParts = @(
            "",
            "include",
            "include\httpd.h",
            "include\http_config.h",
            "include\ap_config.h",
            "include\http_log.h",
            "include\http_protocol.h",
            "include\http_main.h",
            "include\util_script.h",
            "include\http_core.h",
            "include\apr_strings.h",
            "include\apr_tables.h"
        )
        # If any of these files don't exist, we unpack the php source
        $phpParts = @(
            "",
            "ext",
            "ext\standard",
            "ext\standard\info.h",
            "main",
            "main\php.h",
            "main\php_ini.h",
            "main\config.w32.h",
            "main\win95nt.h",
            "TSRM",
            "TSRM\readdir.h",
            "Zend",
            "Zend\zend.h",
            "Zend\zend_exceptions.h",
            "Zend\zend_API.h"
        )
        Write-Host "Checking if we need to unpack httpd source"
        foreach ($part in $apacheParts) {
            $path = [System.IO.Path]::Combine(".\httpd-$apacheVer", $part)
            if (-Not(Test-Path $path)) {
                if (-Not(Test-Path "httpd-$apacheVer.tar")) {
                    & sz x "httpd-$apacheVer.tar.bz2"
                }
                & sz x -y "httpd-$apacheVer.tar"
                break
            }
        }
        Write-Host "Checking if we need to unpack php source"
        foreach ($part in $phpParts) {
            $path = [System.IO.Path]::Combine(".\php-$phpVer", $part)
            if (-Not(Test-Path $path)) {
                if (-Not(Test-Path "php-$phpVer.tar")) {
                    & sz x "php-$phpVer.tar.bz2"
                }
                & sz x -y "php-$phpVer.tar"
                break
            }
        }
        Write-Host "Patching PHP headers for Windows"
        Copy-Item -Force -Path "..\php\main\*.h" "php-$phpVer\main"
        Copy-Item -Force -Path "..\php\TSRM\*.h" "php-$phpVer\TSRM"
        Write-Host "Patching HTTPD headers for Windows"
        Copy-Item -Force -Path "..\httpd\include\*.h" "httpd-$apacheVer\include"
    }
    Finally
    {
        popd
    }
}

Function CheckStatus([string]$action, [int]$status)
{
    Write-Host "Status for action ($action) is: $status"
    <#
    if (-Not $status) {
        $msg = "Action ($action) failed"
        throw $msg
    }
    #>
}

Function MsBuildAction([string]$comp, [string]$conf, [string]$plat, [string]$action)
{
    $uplat = $plat
    # x86 has to be coerced to Win32
    if ($plat.Equals("x86")) {
        $uplat = "Win32"
    }

    $solutions = @{
        "oem" = ".\Oem\oem.sln";
        "csmap" = ".\Oem\CsMap\VC110\OpenSource.sln";
        "server" = ".\Server\Server.sln";
        "web" = ".\Web\src\WebTier.sln";
        "desktop" = ".\Desktop\MgDesktopApi.sln";
        "desktop_viewer" = ".\Desktop\MgDesktopDotNet.sln";
    };
    #$msbParams = "/m /p:Configuration=$conf;Platform=$plat /v:n /t:$action"

    Write-Host "Action ($action) on $comp ($conf|$plat)"
    switch ($comp) {
        "oem" {
            msbuild /m /p:Configuration=$conf /p:Platform=$uplat /v:n /t:$action $solutions["oem"]
            CheckStatus "$action $comp" $LASTEXITCODE
            msbuild /m /p:Configuration=$conf /p:Platform=$uplat /v:n /t:$action $solutions["csmap"]
            CheckStatus "$action $comp" $LASTEXITCODE
        }
        "server" {
            msbuild /m /p:Configuration=$conf /p:Platform=$uplat /v:n /t:$action $solutions["server"]
            CheckStatus "$action $comp" $LASTEXITCODE
        }
        "web" {
            msbuild /m /p:Configuration=$conf /p:Platform=$uplat /v:n /t:$action $solutions["web"]
            CheckStatus "$action $comp" $LASTEXITCODE
        }
        "desktop" {
            msbuild /m /p:Configuration=$conf /p:Platform=$uplat /v:n /t:$action $solutions["desktop"]
            CheckStatus "$action $comp" $LASTEXITCODE
            msbuild /m /p:Configuration=$conf /p:Platform=$plat /v:n /t:$action $solutions["desktop_viewer"]
            CheckStatus "$action $comp" $LASTEXITCODE
        }
        "instantsetup" {
            Try {
                pushd $output
                Write-Host "Entering directory: $output"
                Write-Host "Making instant setup bundle"
                & sz a "$artifact_path\MapGuideOpenSource-$major.$minor.$build.$svnrev-InstantSetup-$plat.exe" -mmt -mx5 -sfx 7z.sfx CS-Map Server Web Setup | Out-Null
            } Finally {
                popd
            }
        }
        "installer" {
            $installerName="MapGuideOpenSource-$major.$minor.$build.$svnrev-$label-$plat";
            $installerCulture="en-US";
            $installerTitle="MapGuide Open Source $major.$minor.$build ($label - $plat)";
            $installerVersion="$major.$minor.$build";
            $installerRegKey="Software\OSGeo\MapGuide\$major.$minor.$build.$svnrev";
            $installerArcSde="no";
            if (Test-Path ("$output\Server\FDO\ArcSdeProvider.dll")) {
                $installerArcSde="yes";
            }

            Write-Host "============ Installer Summary ================="
            Write-Host "Name:    $installerName"
            Write-Host "Culture: $installerCulture"
            Write-Host "Title:   $installerTitle"
            Write-Host "Version: $installerVersion"
            Write-Host "Reg Key: $installerRegKey"
            Write-Host "ArcSDE?: $installerArcSde"
            Write-Host "================================================"

            Try {
                pushd $installerSrcRoot
                Write-Host "Entering $installerSrcRoot"
                $wix_server="$installerSrcRoot\Libraries\MapGuide Server\FileIncludes"
                $wix_web="$installerSrcRoot\Libraries\MapGuide Web Extensions\FileIncludes"
                $wix_csmap="$installerSrcRoot\Libraries\CS Map\FileIncludes"

                # These paraffin mappings are to be process flat (-norecurse)
                $flatMappings = @(
                    @{
                        "desc" = "Server - binaries";
                        "dirPart" = "Server\bin";
                        "custom" = "SRVBINFILES";
                        "dirref" = "SERVERROOTLOCATION";
                        "output" = "$wix_server\incBinFiles.wxs";
                    },
                    @{
                        "desc" = "Server - RepositoryAdmin";
                        "dirPart" = "Server\RepositoryAdmin";
                        "custom" = "SRVREPADMINFILES";
                        "dirref" = "SERVERROOTLOCATION";
                        "output" = "$wix_server\incRepositoryAdminFiles.wxs";
                    },
                    @{
                        "desc" = "Server - resources";
                        "dirPart" = "Server\Resources";
                        "custom" = "SRVRESOURCESFILES";
                        "dirref" = "SERVERROOTLOCATION";
                        "output" = "$wix_server\incResourcesFiles.wxs";
                    },
                    @{
                        "desc" = "Server - Schema";
                        "dirPart" = "Server\Schema";
                        "custom" = "SRVSCHEMAFILES";
                        "dirref" = "SERVERROOTLOCATION";
                        "output" = "$wix_server\incSchemaFiles.wxs";
                    },
                    @{
                        "desc" = "Server - wms";
                        "dirPart" = "Server\wms";
                        "custom" = "SRVWMSFILES";
                        "dirref" = "SERVERROOTLOCATION";
                        "output" = "$wix_server\incWmsFiles.wxs";
                    },
                    @{
                        "desc" = "Server - wfs";
                        "dirPart" = "Server\wfs";
                        "custom" = "SRVWFSFILES";
                        "dirref" = "SERVERROOTLOCATION";
                        "output" = "$wix_server\incWfsFiles.wxs";
                    },
                    @{
                        "desc" = "Web - wwwroot";
                        "dirPart" = "Web\www";
                        "custom" = "WEBROOTFILES";
                        "dirref" = "WEBEXTENSIONSLOCATION";
                        "output" = "$wix_web\incWebRootFiles.wxs";
                    }
                )
                # These paraffin mappings are to be processed recursively
                $recurseMappings = @(
                    @{
                        "desc" = "CS-Map - dictionaries";
                        "dirPart" = "CS-Map\Dictionaries";
                        "custom" = "CSMAPDICTFILES";
                        "dirref" = "CSMAPLOCATION";
                        "output" = "$wix_csmap\incCSMapDictionaryFiles.wxs";
                    },
                    @{
                        "desc" = "Web - Apache";
                        "dirPart" = "Web\Apache24";
                        "custom" = "APACHEFILES";
                        "dirref" = "WEBEXTENSIONSLOCATION";
                        "output" = "$wix_web\incApacheFiles.wxs";
                    },
                    @{
                        "desc" = "Web - Php TS";
                        "dirPart" = "Web\Php";
                        "custom" = "PHPFILES";
                        "dirref" = "WEBEXTENSIONSLOCATION";
                        "output" = "$wix_web\incPhpFiles.wxs";
                    },
                    @{
                        "desc" = "Web - Tomcat";
                        "dirPart" = "Web\Tomcat";
                        "custom" = "TOMCATFILES";
                        "dirref" = "WEBEXTENSIONSLOCATION";
                        "output" = "$wix_web\incTomcatFiles.wxs";
                    },
                    @{
                        "desc" = "Web - Help";
                        "dirPart" = "Web\www\help";
                        "custom" = "HELPFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incHelpFiles.wxs";
                    },
                    @{
                        "desc" = "Web - DevGuide Samples (PHP)";
                        "dirPart" = "Web\www\phpsamples";
                        "custom" = "PHPDEVGUIDEFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incPhpDevGuideFiles.wxs";
                    },
                    @{
                        "desc" = "Web - DevGuide Samples (Java)";
                        "dirPart" = "Web\www\javasamples";
                        "custom" = "JAVADEVGUIDEFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incJavaDevGuideFiles.wxs";
                    },
                    @{
                        "desc" = "Web - DevGuide Samples (.net)";
                        "dirPart" = "Web\www\dotnetsamples";
                        "custom" = "DOTNETDEVGUIDEFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incDotNetDevGuideFiles.wxs";
                    },
                    @{
                        "desc" = "Web - Viewer Samples (PHP)";
                        "dirPart" = "Web\www\phpviewersample";
                        "custom" = "PHPVIEWERSAMPLEFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incPhpViewerSampleFiles.wxs";
                    },
                    @{
                        "desc" = "Web - Viewer Samples (Java)";
                        "dirPart" = "Web\www\javaviewersample";
                        "custom" = "JAVAVIEWERSAMPLEFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incJavaViewerSampleFiles.wxs";
                    },
                    @{
                        "desc" = "Web - Viewer Samples (.net)";
                        "dirPart" = "Web\www\dotnetviewersample";
                        "custom" = "DOTNETVIEWERSAMPLEFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incDotNetViewerSampleFiles.wxs";
                    },
                    @{
                        "desc" = "Web - OpenLayers Samples";
                        "dirPart" = "Web\www\ol2samples";
                        "custom" = "OPENLAYERSSAMPLEFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incOpenLayersSampleFiles.wxs";
                    },
                    @{
                        "desc" = "Web - mapagent";
                        "dirPart" = "Web\www\mapagent";
                        "custom" = "MAPAGENTFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incMapAgentFiles.wxs";
                    },
                    @{
                        "desc" = "Web - mapviewernet";
                        "dirPart" = "Web\www\mapviewernet";
                        "custom" = "MAPVIEWERASPXFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incMapViewerAspxFiles.wxs";
                    },
                    @{
                        "desc" = "Web - mapviewerphp";
                        "dirPart" = "Web\www\mapviewerphp";
                        "custom" = "MAPVIEWERPHPFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incMapViewerPhpFiles.wxs";
                    },
                    @{
                        "desc" = "Web - mapviewerjava";
                        "dirPart" = "Web\www\mapviewerjava";
                        "custom" = "MAPVIEWERJSPFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incMapViewerJspFiles.wxs";
                    },
                    @{
                        "desc" = "Web - fusion";
                        "dirPart" = "Web\www\fusion";
                        "custom" = "FUSIONFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incFusionFiles.wxs";
                    },
                    @{
                        "desc" = "Web - viewerfiles";
                        "dirPart" = "Web\www\viewerfiles";
                        "custom" = "MAPVIEWERFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incMapViewerFiles.wxs";
                    },
                    @{
                        "desc" = "Web - stdicons";
                        "dirPart" = "Web\www\stdicons";
                        "custom" = "MAPVIEWERSTDICONFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incMapViewerStdiconFiles.wxs";
                    },
                    @{
                        "desc" = "Web - schemareport";
                        "dirPart" = "Web\www\schemareport";
                        "custom" = "MAPVIEWERSCHEMAREPORTFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incMapViewerSchemareportFiles.wxs";
                    },
                    @{
                        "desc" = "Web - mapadmin";
                        "dirPart" = "Web\www\mapadmin";
                        "custom" = "MAPVIEWERMAPADMINFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incMapViewerMapAdminFiles.wxs";
                    },
                    @{
                        "desc" = "Web - localized";
                        "dirPart" = "Web\www\localized";
                        "custom" = "MAPVIEWERLOCALIZEDFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incMapViewerLocalizedFiles.wxs";
                    },
                    @{
                        "desc" = "Web - WEB-INF";
                        "dirPart" = "Web\www\WEB-INF";
                        "custom" = "WEBINFFILES";
                        "dirref" = "WEBROOTLOCATION";
                        "output" = "$wix_web\incWebInfFiles.wxs";
                    }
                )

                if ($plat.Equals("x64")) {
                    foreach ($map in $flatMappings) {
                        Write-Host ("Generating: " + $map["desc"])
                        if (-Not (Test-Path ("$output\" + $map["dirPart"]))) {
                            throw ("No such directory " + ("$output\" + $map["dirPart"]))
                        }
                        .\Paraffin.exe -Win64 -guids -dir ("$output\" + $map["dirPart"]) -alias ('$(var.MgSource)\' + $map["dirPart"]) -custom $map["custom"] -dirref $map["dirref"] -norecurse ($map["output"])
                    }
                    foreach ($map in $recurseMappings) {
                        Write-Host ("Generating: " + $map["desc"])
                        if (-Not (Test-Path ("$output\" + $map["dirPart"]))) {
                            throw ("No such directory " + ("$output\" + $map["dirPart"]))
                        }
                        .\Paraffin.exe -Win64 -guids -dir ("$output\" + $map["dirPart"]) -alias ('$(var.MgSource)\' + $map["dirPart"]) -custom $map["custom"] -dirref $map["dirref"] ($map["output"])
                    }
                } else {
                    foreach ($map in $flatMappings) {
                        Write-Host ("Generating: " + $map["desc"])
                        if (-Not (Test-Path ("$output\" + $map["dirPart"]))) {
                            throw ("No such directory " + ("$output\" + $map["dirPart"]))
                        }
                        .\Paraffin.exe -guids -dir ("$output\" + $map["dirPart"]) -alias ('$(var.MgSource)\' + $map["dirPart"]) -custom $map["custom"] -dirref $map["dirref"] -norecurse ($map["output"])
                    }
                    foreach ($map in $recurseMappings) {
                        Write-Host ("Generating: " + $map["desc"])
                        if (-Not (Test-Path ("$output\" + $map["dirPart"]))) {
                            throw ("No such directory " + ("$output\" + $map["dirPart"]))
                        }
                        .\Paraffin.exe -guids -dir ("$output\" + $map["dirPart"]) -alias ('$(var.MgSource)\' + $map["dirPart"]) -custom $map["custom"] -dirref $map["dirref"] ($map["output"])
                    }
                }
                msbuild /m /p:Configuration=$conf /p:Platform=$plat /v:n /t:$action "/p:OutputName=$installerName;MgSource=$output;MgCulture=$installerCulture;MgTitle=$installerTitle;MgVersion=$installerVersion;MgRegKey=$installerRegKey;MgPlatform=$plat;Have_ArcSde=$installerArcSde" InstallerWix.sln
                CheckStatus "$action $comp" $LASTEXITCODE
                Copy-Item "Bootstrapper\vcredist_2012_$plat.exe" -Destination "Output\$installerCulture\vcredist_2012_$plat.exe" -Force
            } Finally {
                popd
            }

            Try {
                pushd $installerSrcRoot\Bootstrapper
                Write-Host "Entering $installerSrcRoot\Bootstrapper"
                makensis /DMAXCOMPRESSION /DCPU=$plat /DINSTALLER_ROOT=$installerSrcRoot "/DNSISDIR=$installerSrcRoot\Support\NSIS" /DOUTNAME=$installerName /DCULTURE=$installerCulture "/DMG_VERSION=$major.$minor.$build.$svnrev" Setup.nsi
            } Finally {
                popd
            }

            Try {
                pushd $installerSrcRoot\Output\en-US
                Move-Item -path "$installerName.exe" -destination "$artifact_path" -Force
            } Finally {
                popd
            }
        }
        "doc" {
            Try {
                pushd "Doc"
                MakeDirIfNotExists "MgOpensource_WebAPIReference"
                Copy-Item -Path "css\doxygen.css" -Destination "MgOpensource_WebAPIReference" -Force
                MakeDirIfNotExists "MgOpensource_WebAPIReference\xml_schemas"
                Copy-Item -Path "xml_schemas\*" -Destination "MgOpensource_WebAPIReference\xml_schemas" -Force -Recurse
                doxygen MgOpenSource_Doxyfile | Tee-Object MGOpenSource_run_log.txt
                perl -w PostProcessor/post_process.MgOpenSource.pl
            } Finally {
                popd
            }

            Try {
                pushd "Doc\devguide"
                Invoke-BatchFile make.bat html
            } Finally {
                popd
            }

            Try {
                pushd "UnitTest\TestData\Samples\Sheboygan"
                if (Test-Path "Sheboygan.mgp") {
                    Remove-Item -Force "Sheboygan.mgp"
                }
                Copy-Item -Path "..\..\FeatureService\SDF\Sheboygan_BuildingOutlines.sdf" -Destination "Library\Samples\Sheboygan\Data\BuildingOutlines.FeatureSource_DATA_BuildingOutlines.sdf" -Force
                Copy-Item -Path "..\..\FeatureService\SDF\Sheboygan_CityLimits.sdf" -Destination "Library\Samples\Sheboygan\Data\CityLimits.FeatureSource_DATA_CityLimits.sdf" -Force
                Copy-Item -Path "..\..\FeatureService\SDF\Sheboygan_Parcels.sdf" -Destination "Library\Samples\Sheboygan\Data\Parcels.FeatureSource_DATA_CityLimits.sdf" -Force
                Copy-Item -Path "..\..\FeatureService\SDF\Sheboygan_VotingDistricts.sdf" -Destination "Library\Samples\Sheboygan\Data\VotingDistricts.FeatureSource_DATA_CityLimits.sdf" -Force
                Copy-Item -Path "..\..\MappingService\UT_HydrographicPolygons.sdf" -Destination "Library\Samples\Sheboygan\Data\HydrographicPolygons.FeatureSource_DATA_CityLimits.sdf" -Force
                Copy-Item -Path "..\..\MappingService\UT_Rail.sdf" -Destination "Library\Samples\Sheboygan\Data\Rail.FeatureSource_DATA_CityLimits.sdf" -Force
                Copy-Item -Path "..\..\TileService\UT_RoadCenterLines.sdf" -Destination "Library\Samples\Sheboygan\Data\RoadCenterLines.FeatureSource_DATA_CityLimits.sdf" -Force
                & sz a -tzip "$artifact_path\Sheboygan.mgp" Library/ MgResourcePackageManifest.xml
            } Finally {
                popd
            }
        }
        "all" {
            foreach ($sln in $solutions.GetEnumerator())
            {
                if ($sln.Key -eq "desktop_viewer") {
                    msbuild /m /p:Configuration=$conf /p:Platform=$plat /v:n /t:$action $sln.Value
                } else {
                    msbuild /m /p:Configuration=$conf /p:Platform=$uplat /v:n /t:$action $sln.Value
                }
                CheckStatus ("$action " + $sln.Key) $LASTEXITCODE
                #$status = Invoke-MsBuild -Path $sln.Value -ShowBuildWindow -MsBuildParameters $msbParams
                #CheckStatus ("Building " + $sln.Key) $status
            }
        }
    }
}

Function BuildAction([string]$comp, [string]$conf, [string]$plat)
{
    MsBuildAction $comp $conf $plat "Build"
}

Function CleanAction([string]$comp, [string]$conf, [string]$plat)
{
    MsBuildAction $comp $conf $plat "Clean"
}

Function MakeDirIfNotExists([string]$dir) {
    if (-Not(Test-Path $dir)) {
        New-Item -ItemType directory -Path $dir
    }
}

Function InstallAction([string]$comp, [string]$conf, [string]$plat, [string]$outDir)
{
    Write-Host "Installing $comp ($conf|$plat) to $outDir"
    MakeDirIfNotExists $outDir

    $typebuild = $conf
    if ($plat.Equals("x64")) {
        $typebuild += "64"
    }

    $exclude = @(".svn\*", "_svn\*", "Logs", "Packages", "Repositories", "Temp", "*.c", "OSTN02.txt", "OSTN97.txt", "cs-map.*", "*.par", "*.mrt", "*.asc", "Compiler.mak", "Compiler.nmk", "UnitTestResults.xml")
    if ($conf.Equals("Release")) {
        $exclude += "*.pdb"
        $exclude += "*.exp"
        $exclude += "*.lib"
    }
    switch ($comp) {
        "server" {
            $mappings = @{
                "Server\bin\$typebuild\*" = "Server";
                "Server\RepositoryAdmin\*" = "Server\RepositoryAdmin";
                "Oem\CsMap\Dictionaries\*" = "CS-Map\Dictionaries";
            };
            if ($plat.Equals("x64")) {
                $mappings.Add("Oem\dbxml\bin64\$conf\*.exe", "Server\bin");
            } else {
                $mappings.Add("Oem\dbxml\bin\$conf\*.exe", "Server\bin");
            }
            foreach ($map in $mappings.GetEnumerator()) {
                $key = $map.Key
                $value = $map.Value
                if (-Not ($map.Value -is [array])) {
                    $value = @($map.Value)
                }
                foreach ($val in $value) {
                    $outPath = [System.IO.Path]::Combine($outDir, $val)
                    MakeDirIfNotExists $outPath
                    Copy-Item ($key) -Destination "$outPath" -Recurse -Force -Exclude $exclude
                }
            }

            $outPath = [System.IO.Path]::Combine($outDir, "Server\bin")
            $srcPath = [System.IO.Path]::Combine($outDir, "Server", "*.*")
            Move-Item -Path "$srcPath" -Destination "$outPath" -Force
        }
        "web" {
            $mappings = @{
                "Web\src\webconfig.ini" = "Web\www";
                "Common\MapGuideCommon\Resources\*" = "Web\www\mapagent\Resources";
                "Web\src\localized\*" = "Web\www\localized";
                "Web\src\mapadmin\*" = "Web\www\mapadmin";
                "Web\src\stdicons\*" = "Web\www\stdicons";
                "Web\src\viewerfiles\*" = "Web\www\viewerfiles";
                "Web\src\schemareport\*" = "Web\www\schemareport";
                "Web\src\mapviewerphp\*" = "Web\www\mapviewerphp";
                "Web\src\mapviewernet\*" = "Web\www\mapviewernet";
                "Web\src\mapagent\*" = "Web\www\mapagent";
                "Web\bin\$typebuild\OSGeo.MapGuide.*" = @(
                    "Web\www\mapviewernet\bin",
                    "Web\www\dotnetsamples\bin",
                    "Web\www\dotnetviewersample\bin"
                );
                "Web\bin\$typebuild\Mg*.dll" = @(
                    "Web\www\mapviewernet\bin",
                    "Web\www\mapagent",
                    "Web\Php",
                    "Web\Php\ext",
                    "Web\Tomcat\bin",
                    "Web\www\dotnetsamples\bin",
                    "Web\www\dotnetviewersample\bin"
                );
                "Web\bin\$typebuild\ACE.dll" = @(
                    "Web\www\mapviewernet\bin",
                    "Web\www\mapagent",
                    "Web\Php",
                    "Web\Tomcat\bin",
                    "Web\www\dotnetsamples\bin",
                    "Web\www\dotnetviewersample\bin"
                );
                "Web\bin\$typebuild\GEOS.dll" = @(
                    "Web\www\mapviewernet\bin",
                    "Web\www\mapagent",
                    "Web\Php",
                    "Web\Php\ext",
                    "Web\Tomcat\bin",
                    "Web\www\dotnetsamples\bin",
                    "Web\www\dotnetviewersample\bin"
                );
                "Web\bin\$typebuild\lib_json.dll" = @(
                    "Web\www\mapviewernet\bin",
                    "Web\www\mapagent",
                    "Web\Php",
                    "Web\Php\ext",
                    "Web\Tomcat\bin",
                    "Web\www\dotnetsamples\bin",
                    "Web\www\dotnetviewersample\bin"
                );
                "Web\bin\$typebuild\MapGuideJavaApi*" = "Web\Tomcat\bin";
                "Web\bin\$typebuild\*UnmanagedApi.dll" = "Web\www\mapviewernet\bin";
                "Web\bin\$typebuild\xerces*.dll" = @(
                    "Web\www\mapviewernet\bin",
                    "Web\www\mapagent",
                    "Web\Php",
                    "Web\Php\ext",
                    "Web\www\dotnetsamples\bin",
                    "Web\www\dotnetviewersample\bin"
                );
                "Oem\SQLite\bin\$typebuild\php_*.dll" = "Web\Php\ext";
                "Web\bin\$typebuild\php_*.dll" = "Web\Php\ext";
                "Web\src\mapviewerjava\*" = "Web\www\mapviewerjava";
                "Web\src\WEB-INF\*" = "Web\www\WEB-INF";
            };
            if ($plat.Equals("x64")) {
                $mappings.Add("Web\src\mapagent64\isapi_MapAgent*", "Web\www\mapagent");
                $mappings.Add("Web\src\mapagent64\mapagent*", "Web\www\mapagent");
                $mappings.Add("Web\src\mapagent64\mod_mgmapagent*", "Web\Apache24\modules");
            } else {
                $mappings.Add("Web\src\mapagent\isapi_MapAgent*", "Web\www\mapagent");
                $mappings.Add("Web\src\mapagent\mapagent*", "Web\www\mapagent");
                $mappings.Add("Web\src\mapagent\mod_mgmapagent*", "Web\Apache24\modules");
            }
            foreach ($map in $mappings.GetEnumerator()) {
                $key = $map.Key
                $value = $map.Value
                if (-Not ($map.Value -is [array])) {
                    $value = @($map.Value)
                }
                foreach ($val in $value) {
                    $outPath = [System.IO.Path]::Combine($outDir, $val)
                    MakeDirIfNotExists $outPath
                    Copy-Item ($key) -Destination "$outPath" -Recurse -Force -Exclude $exclude
                }
            }
            # Fusion
            MakeDirIfNotExists "$outDir\Web\www\fusion"
            Try {
                pushd "Oem\fusion"
                ant clean -f build.xml
                ant docs -f build.xml
                ant deploy -f build.xml "-Ddeploy.home=$outDir\Web\www\fusion"
                ant selfbuild -f build.xml "-Dselfbuild.home=$outDir\fusion-selfbuild"
            } Finally {
                popd
            }
            # Fusion templates
            Copy-Item "Oem\fusionMG\*" -Destination "$outDir\Web\www\fusion" -Recurse -Force -Exclude $exclude
        }
        "doc" {
            $mappings = @{
                "Doc\MgOpensource_WebAPIReference\*" = "Web\www\help\webapi";
                "Doc\devguide\build\html\*" = "Web\www\help\devguide";
                "Doc\samples\*" = "Web\www";
                "Doc\viewerapi\*" = "Web\www\help\viewerapi";
                "UnitTest\TestData\Samples\Sheboygan\Sheboygan.mgp" = "";
            };
            foreach ($map in $mappings.GetEnumerator()) {
                $key = $map.Key
                $value = $map.Value
                if (-Not ($map.Value -is [array])) {
                    $value = @($map.Value)
                }
                foreach ($val in $value) {
                    $outPath = [System.IO.Path]::Combine($outDir, $val)
                    MakeDirIfNotExists $outPath
                    Copy-Item ($key) -Destination "$outPath" -Recurse -Force -Exclude $exclude
                }
            }
        }
        "desktop" {
            $exclude += "Backup"
            $exclude += "MgMapGuideCommon*"
            $exclude += "OSGeo.MapGuide.Commmon*"
            $mappings = @{
                "Desktop\bin\$typebuild\*" = "mg-desktop\Desktop";
                "Desktop\Samples\*" = "mg-desktop\DesktopSamples";
            };
            foreach ($map in $mappings.GetEnumerator()) {
                $key = $map.Key
                $value = $map.Value
                if (-Not ($map.Value -is [array])) {
                    $value = @($map.Value)
                }
                foreach ($val in $value) {
                    $outPath = [System.IO.Path]::Combine($outDir, $val)
                    MakeDirIfNotExists $outPath
                    Copy-Item ($key) -Destination "$outPath" -Recurse -Force -Exclude $exclude
                }
            }
        }
    }
}

Function PrepareInstallerStagingArea([string]$output, [string]$conf, [string]$plat, [string]$installerSupport, [string]$buildRoot, [string]$phpDist, [string]$apacheDist, [string]$tomcatDist)
{
    $uplat = $plat
    # x86 has to be coerced to Win32
    if ($plat.Equals("x86")) {
        $uplat = "Win32"
    }
    $typebuild = $conf
    if ($plat.Equals("x64")) {
        $typebuild += "64"
    }

    Write-Host "Preparing staging area at: $output"

    # Put our Apache/Tomcat/PHP binaries into the output
    MakeDirIfNotExists $output
    MakeDirIfNotExists "$output\Web\Php"
    MakeDirIfNotExists "$output\Web\Tomcat"
    MakeDirIfNotExists "$output\Web\Apache24"
    & sz x $apacheDist -y "-o$output\Web"
    & sz x $phpDist -y "-o$output\Web\Php"
    Copy-Item "$installerSupport\mod_jk.so" -Destination "$output\Web\Apache24\modules" -Force
    Try {
        & sz x "$tomcatDist" -y "-o$output\Web"
        pushd $output\Web
        $tomcatName = [System.IO.Path]::GetFileNameWithoutExtension($tomcatDist);
        if (Test-Path $tomcatName) {
            & sz x $tomcatName -y
            Remove-Item $tomcatName -Force
            $dirToRename = Get-ChildItem 'apache-tomcat-*' | ForEach-Object {$_.FullName} | Select-Object -First 1
            $dirToRename = [System.IO.Path]::GetFullPath($dirToRename)
            Copy-Item -Path "$dirToRename\*" -Destination "Tomcat" -Force -Recurse
            Remove-Item $dirToRename -Force -Recurse
        } else {
            throw "Expected $tomcatName to exist"
        }
    } Finally {
        popd
    }
    Copy-Item "$installerSupport\configs\Tomcat\*" -Destination "$output\Web\Tomcat" -Recurse -Force
    Copy-Item "$installerSupport\configs\Php\*" -Destination "$output\Web\Php" -Recurse -Force
    Copy-Item "$installerSupport\configs\Apache2\*" -Destination "$output\Web\Apache24" -Recurse -Force
    Copy-Item "$installerSupport\configs\FDO\providers.xml" -Destination "$output\Server\FDO" -Force
    Copy-Item "$output\Web\Php\libpq.dll" -Destination "$output\Server\FDO" -Force
    Copy-Item "$output\Web\Php\libeay32.dll" -Destination "$output\Server\FDO" -Force
    Copy-Item "$output\Web\Php\ssleay32.dll" -Destination "$output\Server\FDO" -Force
    msbuild /m /p:Configuration=$conf /p:Platform=$uplat "$installerSrcRoot\InstallerPreReq.sln"
    Copy-Item "$installerSrcRoot\FdoRegUtil\$typebuild\FdoRegUtil.exe" -Destination "$output\Server\FDO" -Force
}

Function StampVersion([string]$buildRoot)
{
    Write-Host "Stamping version $major.$minor.$build.$svnrev to applicable files"
    set-alias st "$buildRoot\BuildTools\WebTools\SetAssemblyVersion\SetAssemblyVersion\bin\Release\SetAssemblyVersion.exe"
    if (-Not (Get-Command doxygen -errorAction SilentlyContinue)) {
        throw "SetAssemblyVersion.exe not found"
    }

    Try {
        pushd $buildRoot
        cscript updateversion.vbs "/major:$major" "/minor:$minor" "/point:$build" "/build:$svnrev"
    } Finally {
        popd
    }

    $paths = @(
        "$buildRoot\Web\src\DotNetApi",
        "$buildRoot\Web\src\DotNetApi\Foundation",
        "$buildRoot\Web\src\DotNetApi\Geometry",
        "$buildRoot\Web\src\DotNetApi\MapGuideCommon",
        "$buildRoot\Web\src\DotNetApi\PlatformBase",
        "$buildRoot\Web\src\DotNetApi\Web",
        "$buildRoot\Desktop\DesktopUnmanagedApi\DotNet\Partials",
        "$buildRoot\Desktop\MgAppLayout\Properties",
        "$buildRoot\Desktop\MapViewer\Properties",
        "$buildRoot\Desktop\MapViewer.Desktop\Properties"
    )
    foreach ($path in $paths) {
        Try {
            pushd $path
            st "-set:$major.$minor.$build.$svnrev" AssemblyInfo.cs
        } Finally {
            popd
        }
    }
}

Try
{
    Import-Module Pscx
    #Import-Module -Name ".\Invoke-MsBuild.psm1"

    $haveMsBuild = (Get-Command msbuild -errorAction SilentlyContinue)
    $haveNmake   = (Get-Command nmake -errorAction SilentlyContinue)

    If (-Not $haveMsBuild -or -Not $haveNmake) {
        Write-Host "Could not find msbuild or nmake. Running VS command prompt"
        $vscmd = "$env:VS110COMNTOOLS" + "VsDevCmd.bat"
        Write-Host "Running: $vscmd"
        Invoke-BatchFile $vscmd
        if (-Not $?) {
            throw "Failure invoking VsDevCmd.bat"
        }
    } Else {
        Write-Host "msbuild and nmake exist. Proceeding with build"
    }
    $cwd = (Get-Item -Path ".\" -Verbose).FullName

    MakeDirIfNotExists $artifact_path
    $artifact_path = (Get-Item -Path $artifact_path -Verbose).FullName
    $log_path = [System.IO.Path]::GetFullPath((Join-Path $artifact_path "\logs"));
    MakeDirIfNotExists $log_path

    StampVersion $build_root

    $cwd = (Get-Item -Path "$build_root" -Verbose).FullName
    pushd $cwd
    $phpVersion = "5.5.17"
    $apacheVersion = "2.4.10"
    $tomcatVersion = "7.0.56"
    if (-Not [System.IO.Path]::IsPathRooted($install_root)) {
        $installerSrcRoot = [System.IO.Path]::GetFullPath((Join-Path $cwd $install_root));
    } else {
        $installerSrcRoot = $install_root
    }
    $installerSupport = [System.IO.Path]::Combine($installerSrcRoot, "Support\Web\$platform")
    $phpDist = [System.IO.Path]::Combine($installerSupport, "php-$phpVersion-$platform-VC11.zip")
    $apacheDist = [System.IO.Path]::Combine($installerSupport, "httpd-$apacheVersion-$platform-VC11.zip")
    $tomcatDist = [System.IO.Path]::Combine($cwd, "Oem\LinuxApt", "apache-tomcat-$tomcatVersion.tar.gz")

    # Alias 7-zip for easy invocation
    if (!$env:PATH.Contains("$cwd\BuildTools\WebTools\7-Zip")) {
        $env:PATH += ";$cwd\BuildTools\WebTools\7-Zip"
        Write-Host "Appended 7zip to PATH"
    } else {
        Write-Host "7zip already in PATH"
    }

    set-alias sz "7z.exe"  
    if (-Not (Get-Command sz -errorAction SilentlyContinue)) {
        throw "7-Zip command not found"
    }

    # Check doxygen
    Write-Host "Checking for doxygen"
    if (-Not (Get-Command doxygen -errorAction SilentlyContinue)) {
        # Retry
        set-alias doxygen "$doxygen_path\doxygen.exe"
        if (-Not (Get-Command doxygen -errorAction SilentlyContinue)) {
            throw "Doxygen not found"
        }
    }

    # Check graphviz
    Write-Host "Checking for graphviz"
    if (-Not (Get-Command dot -errorAction SilentlyContinue)) {
        throw "dot not found. Ensure graphviz bin directory is in your PATH environment variable"
    }

    # Check perl
    Write-Host "Checking for perl"
    if (-Not (Get-Command perl -errorAction SilentlyContinue)) {
        throw "perl not found. Ensure perl bin directory is in your PATH environment variable"
    }

    # Check sphinx-build
    Write-Host "Checking for sphinx-build"
    if (-Not (Get-Command sphinx-build -errorAction SilentlyContinue)) {
        throw "sphinx-build not found. Ensure Python is installed and you have this sphinx package installed (http://sphinx.pocoo.org)"
    }

    # Check and unpack the PHP and HTTPD sources
    EnvSetup $platform $phpVersion $apacheVersion

    $env:PHP_SRC = "$cwd\Oem\LinuxApt\php-$phpVersion"
    $env:HTTPD_SRC = "$cwd\Oem\LinuxApt\httpd-$apacheVersion"
    if ($platform.Equals("x64")) {
        if ($java_home.Length > 0) {
            $env:JAVA_HOME = $java_home
        } else {
            $env:JAVA_HOME = "C:\Program Files\Java\jdk1.7.0_71"
        }
        $env:PHP_LIB = "$cwd\Oem\php\x64\Release_TS"
        $env:HTTPD_LIB = "$cwd\Oem\httpd\lib64"
    } else {
        if ($java_home.Length > 0) {
            $env:JAVA_HOME = $java_home
        } else {
            $env:JAVA_HOME = "C:\Program Files (x86)\Java\jdk1.7.0_71"
        }
        $env:PHP_LIB = "$cwd\Oem\php\Release_TS"
        $env:HTTPD_LIB = "$cwd\Oem\httpd\lib"
    }

    # JAVA_HOME must check out
    Write-Host "Checking for JAVA_HOME"
    if (-Not (Test-Path $env:JAVA_HOME)) {
        throw "JAVA_HOME ($env:JAVA_HOME) path does not exist"
    }

    # Check ant
    Write-Host "Checking for ant"
    if (-Not (Get-Command ant -errorAction SilentlyContinue)) {
        # Retry
        set-alias ant "$ant_path\ant.bat"
        if (-Not (Get-Command ant -errorAction SilentlyContinue)) {
            throw "ant not found. Please check that ant is in your PATH environment variable or that you passed in a valid path for -ant_path"
        }
    }

    # Check makensis
    Write-Host "Checking for makensis"
    if (-Not (Get-Command makensis -errorAction SilentlyContinue)) {
        # Retry
        set-alias makensis "$installerSrcRoot\Support\NSIS\makensis.exe"
        if (-Not (Get-Command ant -errorAction SilentlyContinue)) {
            throw "makensis not found. Please check that you provided a valid path for -install_root"
        }
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
    Write-Host "Artifact Root: $artifact_path"
    Write-Host "Version (major): $major"
    Write-Host "Version (minor): $minor"
    Write-Host "Version (build): $build"
    Write-Host "=== Environment Variable Summary ===="
    Write-Host "PHP_SRC: $env:PHP_SRC"
    Write-Host "PHP_LIB: $env:PHP_LIB"
    Write-Host "HTTPD_SRC: $env:HTTPD_SRC"
    Write-Host "HTTPD_LIB: $env:HTTPD_LIB"
    Write-Host "JAVA_HOME: $env:JAVA_HOME"
    Write-Host "====================================="

    switch($action) {
        "clean" {
            CleanAction $component $config $platform
        }
        "build" {
            BuildAction $component $config $platform
        }
        "buildinstall" {
            if ($component.Equals("all")) {
                BuildAction "oem" $config $platform
                InstallAction "oem" $config $platform $output

                BuildAction "server" $config $platform
                InstallAction "server" $config $platform $output

                BuildAction "web" $config $platform
                InstallAction "web" $config $platform $output

                BuildAction "doc" $config $platform
                InstallAction "doc" $config $platform $output

                PrepareInstallerStagingArea $output $config $platform $installerSupport $cwd $phpDist $apacheDist $tomcatDist
                BuildAction "instantsetup" $config $platform
                BuildAction "installer" $config $platform

                BuildAction "desktop" $config $platform
                InstallAction "desktop" $config $platform $output
            } else {
                BuildAction $component $config $platform
                InstallAction $component $config $platform $output
            }
        }
        "install" {
            if ($component.Equals("all")) {
                InstallAction "server" $config $platform $output
                InstallAction "web" $config $platform $output
                InstallAction "doc" $config $platform $output
                PrepareInstallerStagingArea $output $config $platform $installerSupport $cwd $phpDist $apacheDist $tomcatDist
                InstallAction "desktop" $config $platform $output
            } else {
                InstallAction $component $config $platform $output
            }
        }
        "installprepare" {
            PrepareInstallerStagingArea $output $config $platform $installerSupport $cwd $phpDist $apacheDist $tomcatDist
        }
    }
}
Finally
{
    popd
}