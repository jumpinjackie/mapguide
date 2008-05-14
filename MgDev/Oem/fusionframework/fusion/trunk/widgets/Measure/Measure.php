<?php
/**
 * Measure
 *
 * $Id: Measure.php 1326 2008-03-05 23:28:14Z pspencer $
 *
 * Copyright (c) 2007, DM Solutions Group Inc.
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
 
  $fusionMGpath = '../../MapGuide/php/';
  include $fusionMGpath . 'Common.php';
  SetLocalizedFilesPath(GetLocalizationPath());
  if(isset($_REQUEST['locale'])) {
    $locale = $_REQUEST['locale'];
  } else {
    $locale = GetDefaultLocale();
  }

  $title = GetLocalizedString( "MEASURETITLE", $locale );
  $total = GetLocalizedString( "TOTAL", $locale );
  $segment = GetLocalizedString( "SEGMENT", $locale );
  $length = GetLocalizedString( "LENGTH", $locale );
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
    "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
    <meta http-equiv="Content-type" content="text/html; charset=utf-8">
    <title><?php echo $title ?></title>
    <style type="text/css" media="screen">
        @import url(Measure.css);
    </style>
</head>
<body id="MeasurementWidgetResults">
    <h1><?php echo $title ?></h1>
    <table id="MeasurementWidgetResultsTable" border="0" cellspacing="5" cellpadding="5">
        <thead>
            <tr>
                <th><?php echo $segment ?></th>
                <th><?php echo $length ?></th>
            </tr>
        </thead>
        <tbody id="segmentTBody"></tbody>
        <tfoot>
            <tr>
                <th><?php echo $total ?></th>
                <td id="totalDistance"></td>
            </tr>
        </tfoot>
    </table>
</body>
</html>
