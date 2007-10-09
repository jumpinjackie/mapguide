/********************************************************************** * 
 * @project Fusion
 * @revision $Id: InvokeURL.js 665 2007-06-29 14:49:06Z pspencer $
 * @purpose InvokeURL widget
 * @author madair@dmsolutions.ca
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
 * To put a InvokeURL control in your application, you first need to add
 * a widget to your WebLayout as follows:
 *
 * <Command xsi:type="">
 *   <Name>InvokeURL</Name>
 *   <Label>Keep Session Alive/Label>
 *   <TargetViewer>All</TargetViewer>
 *   <Action>KeepSessionAlive</Action>
 * </Command>
 *
 * The important parts of this Command are:
 *
 * Name (string, mandatory) 
 * 
 * If the Target property points to TaskPane widget, the task will be listed in
 * the menu list of the TaskPane and loaded there.
 * Otherwise if the target is an existing IFrame in the page it will be loaded 
 * there, otherwise it will open a new window with that name.
 *
 *
 * **********************************************************************/

Fusion.Widget.InvokeURL = Class.create();
Fusion.Widget.InvokeURL.prototype = {
    sFeatures : 'menubar=no,location=no,resizable=no,status=no',

    initialize : function(widgetTag) {
        //console.log('InvokeURL.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);

        var json = widgetTag.extension;
        this.sTarget = json.Target ? json.Target[0] : "InvokeUrlWindow";
        this.sBaseUrl = json.Url[0];  //must be supplied
        
        this.bSelectionOnly = (json.DisableIfSelectionEmpty &&
                           (json.DisableIfSelectionEmpty[0] == 'true' ||
                            json.DisableIfSelectionEmpty[0] == '1')) ? true : false;
                            
        this.additionalParameters = [];
        if (json.AdditionalParameter) {
            for (var i=0; i<json.AdditionalParameter.length; i++) {
                var p = json.AdditionalParameter[i];
                var k = p.Key[0];
                var v = p.Value[0];
                this.additionalParameters.push(k+'='+encodeURIComponent(v));
            }
        }
        
        this.enable = Fusion.Widget.InvokeURL.prototype.enable;
        this.getMap().registerForEvent(Fusion.Event.MAP_SELECTION_ON, this.enable.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_SELECTION_OFF, this.enable.bind(this));
        this.disable();
    },

    enable: function() {
        var map = this.getMap();
        if (this.bSelectionOnly || !map) {
            if (map && map.hasSelection()) {
                if (this.action) {
                    this.action.setEnabled(true);
                } else {
                    Fusion.Tool.ButtonBase.prototype.enable.apply(this,[]);
                }
            } else {
                if (this.action) {
                    this.action.setEnabled(false);
                } else {
                    this.disable();
                }
            }
        } else {
            if (this.action) {
                this.action.setEnabled(true);
            } else {
                Fusion.Tool.ButtonBase.prototype.enable.apply(this,[]);
            }
        }
    },
    
    execute : function() {
        var url = this.sBaseUrl;
        //add in other parameters to the url here
        
        var map = this.getMap();
        var params = [];
        params.push('locale=en');
        params.push('session='+map.getSessionID());
        params.push('mapname='+map.getMapName());
        params = params.concat(this.additionalParameters);
        if (url.indexOf('?') < 0) {
            url += '?';
        } else if (url.slice(-1) != '&') {
            url += '&';
        }
        url += params.join('&');
        var taskPaneTarget = Fusion.getWidgetById(this.sTarget);
        if ( taskPaneTarget ) {
            taskPaneTarget.setContent(url);
        } else {
            var pageElement = $(this.sTarget);
            if ( pageElement ) {
                pageElement.src = url;
            } else {
                window.open(url, this.sTarget, this.sWinFeatures);
            }
        }
    }
};
