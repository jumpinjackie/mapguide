<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
-->
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8">
    <title>Digitizing</title>
    
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <meta http-equiv="content-script-type" content="text/javascript">     
    <script  language="javascript" type="text/javascript">
      function DigitizePoint() {
          // ShowResults("");
          parent.parent.mapFrame.DigitizePoint(OnPointDigitized);
      }
      function OnPointDigitized(point) {
          ShowResults("X: " + point.X + ", Y: " + point.Y);
      }
      function DigitizeLine() {
          ShowResults("");
          parent.parent.mapFrame.DigitizeLine(OnLineDigitized);
      }
      function OnLineDigitized(line) {
          ShowResults(FormatLineResults(line));
      }
      function DigitizeMultiLine() {
          ShowResults("");
          parent.parent.mapFrame.DigitizeMultiLine(OnMultiLineDigitized);
      }     
      function DigitizeLineString() {
          ShowResults("");
          parent.parent.mapFrame.DigitizeLineString(OnDigitizeLineString);
      }
      function OnDigitizeLineString(line) {
          ShowResults(FormatLineResults(line));
      }
          
      function DigitizeCircle() {
          ShowResults("");
          parent.parent.mapFrame.DigitizeCircle(OnCircleDigitized);
      }
      function OnCircleDigitized(circle) {
          ShowResults("Center X: " + circle.Center.X + ", Y: " + circle.Center.Y + "\nRadius: " + circle.Radius);
      }
      function DigitizeRectangle() {
          ShowResults("");
          parent.parent.mapFrame.DigitizeRectangle(OnRectangleDigitized);
      }
      function OnRectangleDigitized(rect) {
          str = "X1: " + rect.Point1.X + ", Y1: " + rect.Point1.Y + "\n";
          str += "X2: " + rect.Point2.X + ", Y1: " + rect.Point2.Y + "\n";
          
          ShowResults(str);
      }
      function DigitizePolygon() {
          ShowResults("");
          parent.parent.mapFrame.DigitizePolygon(OnPolygonDigitized);
      }
      function OnPolygonDigitized(poly) {
          ShowResults(FormatLineResults(poly));
      }
      function FormatLineResults(line) {
          str = "Points: " + line.Count + "\n";
          for(var i = 0; i < line.Count; i++) {
              pt = line.Point(i);
              str += "X" + i + ": " + pt.X + ", Y" + i + ": " + pt.Y + "\n";
          }
          return str;
      }
      function ShowResults(res)  {
          document.getElementById("res").value = res;
      }
    </script>
  </head>
  <body class="AppFrame">   
    <h1 class="AppHeading">Digitizing</h1>  
    <p>
    Click button then click on map: 
    </p>
    <input type="button" value=" Point " onclick="DigitizePoint();">
    <br>
    <input type="button" value=" LineString " onclick="DigitizeLineString();">    
    <br>
    <input type="button" value=" Polygon " onclick="DigitizePolygon();">
    <hr>
    
    <p>Click button then drag on map: 
    </p>
    <input type="button" value=" Line " onclick="DigitizeLine();">
    <br>
    <input type="button" value=" Rectangle " onclick="DigitizeRectangle();">
    <br>
    <input type="button" value=" Circle " onclick="DigitizeCircle();">
    
    <hr>
    Result:<br>
    <textarea id="res" rows="10" cols="32" name="results">
    </textarea>
    
    <hr>
      
  </body>
</html>