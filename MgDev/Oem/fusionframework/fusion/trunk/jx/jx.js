/**********************************************************************
 *
 * $Id: jx.js 445 2007-10-26 22:06:17Z fwarnock $
 *
 * purpose: general purpose GUI components based on Prototype and 
 *          scriptaculous.
 *
 * author: Paul Spencer (pspencer@dmsolutions.ca)
 *
 **********************************************************************
 *
 * Copyright (c) 2005, DM Solutions Group Inc.
 *
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
 *
 **********************************************************************/ 


var Jx = {
    baseURL: '',
  Version: '1.0.0',
  scriptaculous : ['builder','effects','dragdrop','controls','slider'],
  jx : ['layout','splitter','dialog','panel','grid','tree','toolbar','statusbar',
        'button','color','menu','picker','tab'],
  
  require: function(libraryName) {
      // inserting via DOM fails in Safari 2.0, so brute force approach
      document.write('<script type="text/javascript" src="'+libraryName+'"></script>');
  },
  load: function() {
      if((typeof Prototype=='undefined') || 
         (typeof Element == 'undefined') || 
         (typeof Element.Methods=='undefined') ||
         parseFloat(Prototype.Version.split(".")[0] + "." +
         Prototype.Version.split(".")[1]) < 1.5) {
          throw("jx requires the Prototype JavaScript framework >= 1.5.0");
      }
    
        this.scriptaculous.each(
            function(include) { 
                Jx.require(Jx.baseURL+'scriptaculous/'+include+'.js');
            }
        );
        Jx.require(Jx.baseURL+'jxcore.js');
        this.jx.each(
            function(include) { 
                Jx.require(Jx.baseURL+include+'/jx'+include+'.js');
            }
        );
        Jx.require(Jx.baseURL+'jxstyles.js');
    },
    
    bootstrap: function() {
        var aScripts = document.getElementsByTagName('SCRIPT');
        for (var i=0; i<aScripts.length; i++) {
            var s = aScripts[i].src;
            var n = s.indexOf('jx.js');
            if (n != -1) {
                Jx.baseURL = s.substring(0,n);
                Jx.load();
                break;
            }
        }
    }
};

Jx.bootstrap();