/********************************************************************** * 
 * @project Fusion
 * @revision $Id: $
 * @purpose Search widget
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
 * To put a Search control in your application, you first need to add
 * a widget to your WebLayout as follows:
 *
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

Fusion.Widget.Search = Class.create();
Fusion.Widget.Search.prototype = {
    sFeatures : 'menubar=no,location=no,status=no,scrollbars=yes',

    initialize : function(widgetTag) {
        //console.log('Search.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);

        var json = widgetTag.extension;
        this.sTarget = json.Target ? json.Target[0] : "SearchWindow";
        this.sBaseUrl = Fusion.getFusionURL() + 'widgets/Search/SearchPrompt.php';
        this.prompt = json.Prompt ? json.Prompt[0] : "";
        this.layer = json.Layer ? json.Layer[0] : "";
        this.filter = json.Filter ? json.Filter[0] : "";
        this.limit = json.MatchLimit ? json.MatchLimit[0] : 100;
        this.resultColumns = json.ResultColumns ? json.ResultColumns[0].Column : [];

    },

    execute : function() {
        var url = this.sBaseUrl;
        //add in other parameters to the url here
        
        var map = this.getMap();
        var mapLayers = map.getAllMaps();
        var taskPaneTarget = Fusion.getWidgetById(this.sTarget);
        var pageElement = $(this.sTarget);

        var params = [];
        params.push('locale='+Fusion.locale);
        params.push('session='+mapLayers[0].getSessionID());
        params.push('mapname='+mapLayers[0].getMapName());
        if (taskPaneTarget || pageElement) {
          params.push('popup=false');
        } else {
          params.push('popup=true');
        }
        params.push('widgetname='+this.id);  

        if (url.indexOf('?') < 0) {
            url += '?';
        } else if (url.slice(-1) != '&') {
            url += '&';
        }
        url += params.join('&');
        if ( taskPaneTarget ) {
            taskPaneTarget.setContent(url);
        } else {
            if ( pageElement ) {
                pageElement.src = url;
            } else {
                window.open(url, this.sTarget, this.sWinFeatures);
            }
        }
    }
};
