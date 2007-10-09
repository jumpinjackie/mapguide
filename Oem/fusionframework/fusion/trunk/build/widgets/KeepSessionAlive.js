/********************************************************************** * 
 * @project Fusion
 * @revision $Id: KeepSessionAlive.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose KeepSessionAlive widget
 * @author pspencer@dmsolutions.ca
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
 * Keep the current session active by 'pinging' the server periodically
 
 * To put a KeepSessionAlive control in your application, you first need to add
 * a widget to your WebLayout as follows:
 *
 * <Command xsi:type="KeepSessionAliveCommandType">
 *   <Name>KeepAlive</Name>
 *   <Label>Keep Session Alive/Label>
 *   <TargetViewer>All</TargetViewer>
 *   <Action>KeepSessionAlive</Action>
 * </Command>
 *
 * The important parts of this Command are:
 *
 * Name (string, mandatory) 
 * 
 * an element with an id that is the same as this name must be in
 * the application.  For instance:
 *
 * <div id="KeepAlive"></div>
 *
 * A button that activates this widget will appear inside the
 * element you provide.
 * **********************************************************************/

Fusion.Widget.KeepSessionAlive = Class.create();
Fusion.Widget.KeepSessionAlive.prototype = {
    initialize : function(widgetTag) {
        //console.log('KeepSessionAlive.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        var json = widgetTag.extension;
        
        this.delay = parseInt(json.RefreshInterval ? json.RefreshInterval[0] : 300);
        
        window.setInterval(this.pingServer.bind(this), this.delay * 1000);
        
        this.domObj.style.display = 'none';
    },
    
    pingServer: function() {
        var s = this.getMap().arch + '/' + Fusion.getScriptLanguage() + "/Common." + Fusion.getScriptLanguage() ;
        var params = {};
        params.parameters = 'session='+this.getMap().getSessionID();
        Fusion.ajaxRequest(s, params);
    }
};
