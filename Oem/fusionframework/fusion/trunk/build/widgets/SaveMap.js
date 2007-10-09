/********************************************************************** * 
 * @project Fusion
 * @revision $Id: SaveMap.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose Save map widget
 * @author zjames@dmsolutions.ca
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
 * save the current map image on the client's computer
 *
 * usage:
 * DWF format support requires a structure like this in the weblayout:
 * scales
 * <Command xsi:type="FusionCommandType">
 *    <Name>SaveDWF</Name>
 *    <Label>Save as DWF</Label>
 *    <Tooltip>Click to save the current map as a DWF document</Tooltip>
 *    <TargetViewer>All</TargetViewer>
 *    <PrintLayout>
 *        <Name>My Layout</Name>
 *        <ResourceId>Library://PrintLayouts/first.PrintLayout</ResourceId>
 *        <Scale>25000</Scale>
 *        <Scale>10000</Scale>
 *    </PrintLayout>
 *    <PrintLayout>
 *        <Name>My Other Layout</Name>
 *        <ResourceId>Library://PrintLayouts/second.PrintLayout</ResourceId>
 *    </PrintLayout>
 *    <Action>SaveMap</Action>
 *    <Format>DWF</Format>
 * </Command>
 *

 * **********************************************************************/

Fusion.Widget.SaveMap = Class.create();
Fusion.Widget.SaveMap.prototype = {
    iframe : null,
    printLayout : null,
    printScale : null,
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);

        var json = widgetTag.extension;
        this.format = (json.Format && json.Format[0] != '')?
                       json.Format[0] : 'png';
        
        //for DWF, parse printLayouts and build menu
        if (this.format == 'DWF' && json.PrintLayout.length) {
            
            this.printLayout = json.PrintLayout[0];
            this.printScale =  this.printLayout.Scale[0];
        }

        this.enable = Fusion.Widget.SaveMap.prototype.enable;
    },
    
    enable: function() {
        Fusion.Tool.ButtonBase.prototype.enable.apply(this, []);
    },
    
    /**
     * called when the button is clicked by the Fusion.Tool.ButtonBase widget
     * prompts user to save the map.
     */
    activateTool : function()
    {
        if (!this.iframe) {
            this.iframe = document.createElement('iframe');
            this.iframe.id = 'w';
            this.iframe.style.visibility = 'hidden';
            document.body.appendChild(this.iframe);
        }
        var szLayout = '';
        var szScale = '';
        if (this.format === 'DWF') {
            szLayout = '&layout=' + this.printLayout;
            szScale = '&scale=' + this.printScale;
        }
        //TODO: revisit Fusion.getWebAgentURL
        if(navigator.appVersion.match(/\bMSIE\b/)) {
            //var url = Fusion.getWebAgentURL() + "OPERATION=GETDYNAMICMAPOVERLAYIMAGE&FORMAT=PNG&VERSION=1.0.0&SESSION=" + this.getMap().getSessionID() + "&MAPNAME=" + this.getMap().getMapName() + "&SEQ=" + Math.random();
            
            var url = Fusion.fusionURL + '/' + this.getMap().arch + '/' + Fusion.getScriptLanguage() + "/SaveMapFrame." + Fusion.getScriptLanguage() + '?session='+this.getMap().getSessionID() + '&mapname=' + this.getMap().getMapName() + '&format=' + this.format + szLayout;
            //this.iframe.src = url;
            w = open(url, "Save", 'menubar=no,height=200,width=300');
        } else {
            var s = Fusion.fusionURL + '/' + this.getMap().arch + '/' + Fusion.getScriptLanguage() + "/SaveMap." + Fusion.getScriptLanguage() + '?session='+this.getMap().getSessionID() + '&mapname=' + this.getMap().getMapName() + '&format=' + this.format + szLayout;
            //console.log(s);
            
            this.iframe.src = s;
        }
    }
};
