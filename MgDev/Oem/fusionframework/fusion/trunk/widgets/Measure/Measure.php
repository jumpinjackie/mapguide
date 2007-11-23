<?php
/**
 * Measure
 *
 * $Id: $
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
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
    "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
    <meta http-equiv="Content-type" content="text/html; charset=utf-8">
    <title>Measure</title>
    <style type="text/css" media="screen">
        @import url(Measure.css);
    </style>
    <script type="text/javascript" charset="utf-8">
        var Fusion;
        var measureWidgets;
        var currentWidget;
        window.onload = function() {
            Fusion = window.top.Fusion;
            measureWidgets = Fusion.getWidgetsByType('Measure');
            for (var i=0; i<measureWidgets.length; i++) {
              measureWidgets[i].registerForEvent(Fusion.Event.MEASURE_NEW_SEGMENT, measureNewSegment);
              measureWidgets[i].registerForEvent(Fusion.Event.MEASURE_CLEAR, measureClear);  
            }
        };
        
        function measureNewSegment(eventId, widget, marker) {
            marker.registerForEvent(Fusion.Event.MARKER_DISTANCE_CHANGED, measureSegmentUpdate)
            var tbody = document.getElementById('segmentTBody');
            var segmentId = tbody.childNodes.length + 1;
            var tr = document.createElement('tr');
            tr.marker = marker;
            var td = document.createElement('td');
            td.innerHTML = 'Segment '+segmentId;
            tr.appendChild(td);
            var td = document.createElement('td');
            td.innerHTML = '...';
            tr.appendChild(td);
            tbody.appendChild(tr);
        }
        
        function measureSegmentUpdate(eventId, marker) {
            var distanceText = 'calculating ...';
            var distanceValue = 0;
            var distance = marker.getDistanceLabel();
            if (distance) {
                distanceText = distance;
                distanceValue = parseFloat(distance);
            }
            var totalDistance = 0;
            var tbody = document.getElementById('segmentTBody');
            for (var i=0; i<tbody.childNodes.length; i++) {
                if (tbody.childNodes[i].marker == marker) {
                    tbody.childNodes[i].childNodes[1].innerHTML = distanceText;
                }
                totalDistance += parseFloat(tbody.childNodes[i].childNodes[1].innerHTML);
            }
            var tDist = document.getElementById('totalDistance');
            tDist.innerHTML = totalDistance + ' ' + marker.unitAbbr;
        }
        
        function measureClear() {
            var tbody = document.getElementById('segmentTBody');
            while(tbody.firstChild) {
                tbody.firstChild.marker.deregisterForEvent(Fusion.Event.MARKER_DISTANCE_CHANGED, measureSegmentUpdate);
                tbody.firstChild.marker = null;
                tbody.removeChild(tbody.firstChild);
            }
            var tDist = document.getElementById('totalDistance');
            tDist.innerHTML = '';
        }
    </script>
</head>
<body id="MeasurementWidgetResults">
    <h1>Measurement Results</h1>
    <table id="MeasurementWidgetResultsTable" border="0" cellspacing="5" cellpadding="5">
        <thead>
            <tr>
                <th>Segment</th>
                <th>Length</th>
            </tr>
        </thead>
        <tbody id="segmentTBody"></tbody>
        <tfoot>
            <tr>
                <th>Total</th>
                <td id="totalDistance"></td>
            </tr>
        </tfoot>
    </table>
</body>
</html>