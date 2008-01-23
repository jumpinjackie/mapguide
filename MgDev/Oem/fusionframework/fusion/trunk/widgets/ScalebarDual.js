/**
 * Fusion.Widget.ScalebarDual
 *
 * $Id: Scalebar.js 1084 2007-12-06 16:58:52Z madair $
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

 /********************************************************************
 * Class: Fusion.Widget.ScalebarDual
 *
 * A dynamically generated cartographic scalebar that looks like the Google scalebar
 *
 * **********************************************************************/

Fusion.Widget.ScalebarDual = Class.create();
Fusion.Widget.ScalebarDual.prototype = {
    nominalSize: 100,
    initialize : function(widgetTag) {
        //console.log('Scalebar.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);

        var json = widgetTag.extension;
        
        this.content = $(this.getName());
        
        var scaleDiv = document.createElement('div');
        Element.addClassName(scaleDiv, 'scaleDiv');
        Element.addClassName(scaleDiv, 'scaleMetric');
        this.content.appendChild(scaleDiv);
        
        this.metricDiv = document.createElement('div');
        this.metricDiv.style.backgroundColor = 'olive';//remove this when css is done
        Element.addClassName(this.metricDiv, 'scaleLabel');
        scaleDiv.appendChild(this.metricDiv);
        
        scaleDiv = document.createElement('div');
        Element.addClassName(scaleDiv, 'scaleDiv');
        Element.addClassName(scaleDiv, 'scaleImperial');
        this.content.appendChild(scaleDiv);
        
        this.imperialDiv = document.createElement('div');
        this.imperialDiv.style.backgroundColor = 'green';//remove this when css is done
        Element.addClassName(this.imperialDiv, 'scaleLabel');
        scaleDiv.appendChild(this.imperialDiv);
        
        Element.addClassName(this.domObj, 'dualScalebar');
        
        this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.extentsChangedCB.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.extentsChangedCB.bind(this));
    },

    extentsChangedCB : function() {
      var maxWidth = this.content.getWidth();
      
      //TODO: support projected data
      var map = this.getMap();
      var oExtent = map.getCurrentExtents();
      var res = map.getResolution();

      var kmPerPixel;
      var units = map.getUnits()
      if (units == 'degrees' || units == 'dd' ) {   //TODO: add case for units='ft'
        var cp = oExtent.getCenterLonLat();
        if (Math.abs(cp.lat) > 89.9) return;
        
        var ddPerPixel = res;
        var metersPerDD = 110570;
        //adjust for latitude
        var kmPerDD = 110.570 * Math.cos(cp.lat*Math.PI/180);
        kmPerPixel = kmPerDD * ddPerPixel;
      } else {
        kmPerPixel = res/1000.0;
      }
      var miPerPixel = kmPerPixel * 0.621371192;
      var ftPerPixel = kmPerPixel * 3280.8399;
      var kmBarLength = 0; //pixels
      var miBarLength = 0;
      var km = 0;
      var mi = 0;
      var ft = 0;
      var kmDone = miDone = false;
      while(!kmDone && !miDone) {
          if (kmBarLength < this.nominalSize) {
              if (km < 1)
                  km = (km * 10  + 1)/10 ;
              else if (km < 10)
                  km = km + 1;
              else if (km < 100)
                  km = km + 10;
              else if (km < 1000)
                  km = km + 100;
              else
                  km = km + 1000;
          } else {
            kmDone = true;
          }
          if (miBarLength < this.nominalSize) {
              if (mi < 1)
                  mi = (mi * 10  + 1)/10 ;
              else if (mi < 10)
                  mi = mi + 1;
              else if (mi < 100)
                  mi = mi + 10;
              else if (mi < 1000)
                  mi = mi + 100;
              else
                  mi = mi + 1000;
          } else {
            miDone = true;
          }
          
          kmBarLength = km / kmPerPixel;
          miBarLength = mi / miPerPixel;
      }
      //for small values, convert to feet and round
      if (mi < 0.4) {
          ft = (Math.floor(mi * 52.8) * 100);
          miBarLength = ft / ftPerPixel;
      }
      
      var sbMax = Math.max(kmBarLength, miBarLength) + 2;
      this.content.style.width = Math.round(sbMax) + 'px';
      //(document.getElementById(this.id)).style.width = sbMax + 'px';
      this.metricDiv.style.width = Math.round(kmBarLength - 0) + "px";  //0 was 24 why?
      //for small values, convert units
      if (km <= 0.5) {
          this.metricDiv.innerHTML = km * 1000 + " m";
      }else{
          this.metricDiv.innerHTML = km + " km";
      }
      this.imperialDiv.style.width = Math.round(miBarLength - 0) + "px";  //0 was 24 why?
      if (mi < 0.4) {
          this.imperialDiv.innerHTML = ft + " ft";
      }else{
          this.imperialDiv.innerHTML = mi + " mi";
      }
  }
};
