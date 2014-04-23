<?php

function is_installed($feature)
{
    return file_exists(dirname(__FILE__)."/".$feature);
}

$hasDotNetDevGuide = is_installed("dotnetsamples");
$hasDotNetViewerSample = is_installed("dotnetviewersample");
$hasPhpDevGuide = is_installed("phpsamples");
$hasPhpViewerSample = is_installed("phpviewersample");
$hasJavaDevGuide = is_installed("javasamples");
$hasJavaViewerSample = is_installed("javaviewersample");
$hasOL2Samples = is_installed("ol2samples");

$hasDotNet = $hasDotNetDevGuide || $hasDotNetViewerSample;
$hasPhp = $hasPhpDevGuide || $hasPhpViewerSample;
$hasJava = $hasJavaDevGuide || $hasJavaViewerSample;
$hasNothing = !$hasDotNet && !$hasPhp && !$hasJava;

?>
<!DOCTYPE html>
<html>
    <head>
        <title>MapGuide Samples Landing Page</title>
        <style type="text/css">
        html { padding: 0; margin: 0; }
        body {
            margin: 0px 0px 0px 0px;
            color: #000000;
            font-family: arial,verdana,helvetica,sans-serif;
            background: #f0f0f0;
            height: 100%;
        }
        .banner {
            width: 100%;
            padding: 6px 2px 6px 2px;
            background: #004282;
            color: #ffffff;
            font-family: arial,verdana,helvetica,sans-serif;
            font-weight: bold;
            font-size: 20px;
        }
        .content-body {
            padding-left: 20px;
            font-size: 0.8em;
        }
        .sample-header {
            font-weight: bold;
            text-decoration: underline;
        }
        .error { color: red; }
        </style>
    </head>
    <body>
        <div class="banner">
            MapGuide Samples
        </div>
        <div class="content-body">
            <p>Here you will find an assorted list of samples using</p>
            <ul>
                <li>AJAX Viewer</li>
                <li>MapGuide Web Extensions API</li>
                <? if ($hasOL2Samples) {?>
                <li>OpenLayers integration</li>
                <? } ?>
            </ul>
            <p>For more code samples, visit the <a href="http://trac.osgeo.org/mapguide/wiki/CodeSamples">Community Code Samples</a> page on the MapGuide Open Source wiki</p>
            <p><strong>NOTE: You should download the <a href="http://download.osgeo.org/mapguide/releases/2.0.0/samples/Sheboygan.mgp">Sheboygan Dataset</a> and load this in via the <a href="mapadmin/login.php">MapGuide Site Administrator</a> before running any of these samples</strong></p>
            <p><strong>NOTE: Some samples may not be available because you did not choose to enable this feature from the Windows installer.</strong></p>
            <? if ($hasOL2Samples) { ?>
            <p><strong>NOTE: Some OpenLayers integration samples require some sample resources to be loaded. <a href="ol2samples/data/load.php">Click here</a> to load these resources</strong></p>
            <? } ?>
            <p>Click on a link below to go to that particular sample</p>
            <? if ($hasNothing) { ?>
                <strong class="error">No samples found on this MapGuide installation.</strong>
            <? } else { ?>
                <? if ($hasDotNet) { ?>
                <p class="sample-header">AJAX Viewer (.net)</p>
                <ul>
                    <? if ($hasDotNetDevGuide) { ?>
                    <li><a href="dotnetsamples/main.aspx">.net Developer's Guide samples</a></li>
                    <? } ?>
                    <? if ($hasDotNetViewerSample) { ?>
                    <li><a href="dotnetviewersample/ajaxviewersample.aspx">.net Viewer Sample</a></li>
                    <li><a href="dotnetviewersample/ajaxtiledviewersample.aspx">.net Tiled Viewer Sample</a></li>
                    <? } ?>
                </ul>
                <? } ?>
                <? if ($hasPhp) { ?>
                <p class="sample-header">AJAX Viewer (PHP)</p>
                <ul>
                    <? if ($hasPhpDevGuide) { ?>
                    <li><a href="phpsamples/main.php">PHP Developer's Guide samples</a></li>
                    <? } ?>
                    <? if ($hasPhpViewerSample) { ?>
                    <li><a href="phpviewersample/ajaxviewersample.php">PHP Viewer Sample</a></li>
                    <li><a href="phpviewersample/ajaxtiledviewersample.php">PHP Tiled Viewer Sample</a></li>
                    <? } ?>
                </ul>
                <? } ?>
                <? if ($hasJava) { ?>
                <p class="sample-header">AJAX Viewer (Java)</p>
                <ul>
                    <? if ($hasJavaDevGuide) { ?>
                    <li><a href="javasamples/main.jsp">Java Developer's Guide samples</a></li>
                    <? } ?>
                    <? if ($hasJavaViewerSample) { ?>
                    <li><a href="javaviewersample/ajaxviewersample.jsp">Java Viewer Sample</a></li>
                    <li><a href="javaviewersample/ajaxtiledviewersample.jsp">Java Tiled Viewer Sample</a></li>
                    <? } ?>
                </ul>
                <? } ?>
                <? if ($hasOL2Samples) { ?>
                <p class="sample-header">MapGuide with <a href="http://www.openlayers.org">OpenLayers</a></p>
                <ul>
                    <li><a href="ol2samples/untiled/index.html">Basic Sheboygan un-tiled map example with basic legend</a></li>
                    <li><a href="ol2samples/tiled/index.html">Basic Sheboygan tiled map example</a></li>
                    <li><a href="ol2samples/selection/index.html">Basic Sheboygan un-tiled map example with selection</a></li>
                    <li><a href="ol2samples/mixed/index.html">Basic Sheboygan mixed map (tiled and untiled) example</a></li>
                    <li><a href="ol2samples/commercial/index.html">Sheboygan map with Google/OSM layers</a></li>
                </ul>
                <? } ?>
            <? } ?>
        </div>
    </body>
</html>