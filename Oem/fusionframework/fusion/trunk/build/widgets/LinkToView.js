/*****************************************************************************
 * $Id: $
 * 
 * The LinkToView widget displays a link to the current view of the application
 *
 * Copyright (c) 2007 DM Solutions Group Inc.
 *****************************************************************************
 * This code shall not be copied or used without the expressed written consent
 * of DM Solutions Group Inc.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 ********************************************************************
 *
 * To put a LinkToView widget in your application, you first need to add
 * a widget to your ApplicationDefinition as follows:
 *
 *
 * **********************************************************************/

Fusion.Widget.LinkToView = Class.create();
Fusion.Widget.LinkToView.prototype = {

    initialize : function(widgetTag) {
        //console.log('LinkToView.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);

        var json = widgetTag.extension;
        this.baseUrl = window.location.protocol + '//' + window.location.host + window.location.pathname + '?';

        //remove any existing extent param
        var queryParams = Fusion.parseQueryString();
        var join = '';
        for (var param in queryParams) {
          if (typeof queryParams[param] == 'function') continue;
          if (param == 'extent') continue;
          this.baseUrl += join + param + '=' + queryParams[param];
          join = '&';
        }

        this.anchorLabel = this.domObj.innerHTML;

        this.anchor = document.createElement('a');
        this.anchor.className = 'anchorLinkToView';
        this.anchor.href = this.baseUrl;
        this.anchor.innerHTML = this.anchorLabel;
        this.anchor.title = 
        this.domObj.innerHTML = '';
        this.domObj.appendChild(this.anchor);

        this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.updateLink.bind(this));
        this.enable();                   
    },
    
    updateLink : function() {
        var sBbox = this.getMap().getCurrentExtents().toBBOX();
        var join = (this.baseUrl.indexOf('?')==this.baseUrl.length-1)?'':'&';
        this.anchor.href = this.baseUrl + join +'extent=' + sBbox;
    }
};
