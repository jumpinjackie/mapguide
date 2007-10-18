/**
 * Fusion.Widget.About
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

/********************************************************************
* Class: Fusion.Widget.About
*
* About widget to display a pop-up window about the application.  The contents
* of the page are come from an HTML page set as the AboutUrl extension property.
*
* **********************************************************************/

Fusion.Widget.About = Class.create();
Fusion.Widget.About.prototype = 
{
    _nWidth : 500,
    _nHeight : 400,
    _sDefaultUrl : '/mapguide/mapadmin/about.php',  //TBD we need a Fusion specific About page

/*
 * Constructor: About
 *
 * Parameters:
 *
 * widgetTag - JSON node for this widget from the Application definition
 *
 */
    initialize : function(widgetTag) {
        //console.log('About.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        var json = widgetTag.extension;
        this._sAboutUrl = (json.AboutURL) ? 
                json.AboutURL[0] : this._sDefaultUrl;
        this.enable();
    },

    /**
     * Function: execute
     *
     * opens a pop-up window with the about information when invoked
     * 
     */
    execute : function() {
      //console.log('About.execute');

      var sFeatures = 'menubar=no,location=no,resizable=no,status=no';
      sFeatures += ',width=' + this._nWidth;
      sFeatures += ',height=' + this._nHeight;
      window.open(this._sAboutUrl, 'AboutPopup', sFeatures);
    }
};
