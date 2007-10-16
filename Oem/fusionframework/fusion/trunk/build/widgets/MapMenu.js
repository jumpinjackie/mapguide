/********************************************************************** * 
 * @project Fusion
 * @revision $Id $
 * @purpose MapMenu widget
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
 * MapMenu 
 *
 * To put a MapMenu control in your application, you first need to add a
 * widget to your WebLayout as follows:
 *
 * <Command xsi:type="MapMenuCommandType">
 *   <Name>MyMapMenu</Name>
 *   <Label>Map Menu</Label>
 *   <Folder>Library://Samples/</Folder>
 *   <TargetViewer>All</TargetViewer>
 *   <Action>MapMenu</Action>
 * </Command>
 *
 * The important parts of this Command are:
 *
 * Name (string, mandatory) 
 * 
 * an element with an id that is the same as this name must be in
 * the application.  For instance:
 *
 * <div id="MyMapMenu"></div>
 *
 * The MapMenu will appear inside the element you provide.
 * 
 * Folder (string, optional)
 *
 * The repository folder to enumerate maps below. If this is not set,
 * the entire repository is enumerated.
 * **********************************************************************/


Fusion.Widget.MapMenu = Class.create();
Fusion.Widget.MapMenu.prototype = 
{
    domObj: null,
    oMenu: null,
    sRootFolder: '',
    aMenus : null,
    initialize : function(widgetTag)
    {
        //console.log('MapMenu.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.MenuBase.prototype, []);
        this.enable();
        
        var json = widgetTag.extension;
        var mapGroups = Fusion.applicationDefinition.mapGroups;
        for (var i=0; i<mapGroups.length; i++) {
            var mapGroup = mapGroups[i];
            var opt = {};
            opt.label = mapGroup.mapId;
            var data = mapGroup;
            var action = new Jx.Action(this.switchMap.bind(this, data));
            var menuItem = new Jx.MenuItem(action,opt);
            this.oMenu.add(menuItem);
        }
    },
    
    switchMap: function(data) {
        this.getMap().loadMapGroup(data);
    }
};