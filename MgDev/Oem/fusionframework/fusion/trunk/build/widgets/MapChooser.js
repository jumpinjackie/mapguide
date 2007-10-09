/********************************************************************** * 
 * @project Fusion
 * @revision $Id: MapChooser.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose MapChooser widget
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
 * MapChooser 
 *
 * To put a MapChooser control in your application, you first need to add a
 * widget to your WebLayout as follows:
 *
 * <Command xsi:type="MapChooserCommandType">
 *   <Name>MyMapChooser</Name>
 *   <Label>Legend</Label>
 *   <TargetViewer>All</TargetViewer>
 *   <Action>MapChooser</Action>
 * </Command>
 *
 * The important parts of this Command are:
 *
 * Name (string, mandatory) 
 * 
 * an element with an id that is the same as this name must be in
 * the application.  For instance:
 *
 * <div id="MyMapChooser"></div>
 *
 * The MapChooser will appear inside the element you provide.
 * **********************************************************************/

Fusion.Widget.MapChooser = Class.create();
Fusion.Widget.MapChooser.prototype = 
{
    currentNode: null,
    domObj: null,
    oRoot: null,
    initialize : function(widgetTag)
    {
        //console.log('MapChooser.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        
        this.defIcon = Fusion.getFusionURL() + 'images/tree_map.png';
        
        this.oTree = new Jx.Tree(this.domObj);
        
        var opt = {};
        opt.label = 'Maps';
        opt.data = null;
        opt.isOpen = true;
        this.oRoot = new Jx.TreeFolder(opt);
        this.oTree.append(this.oRoot);
        if (json.Group) {
            for (var i=0; i<json.Group.length; i++) {
                this.processGroupNode(json.Group[i], this.oRoot);
            }
        }
    },
    
    processGroupNode: function(json, oParent) {
        opt = {};
        opt.label = json.Name ? json.Name[0] : '';
        opt.isOpen = true;
        var folder = new Jx.TreeFolder(opt);
        oParent.append(folder);
    
        //there is a bug in DomNode that is not easily fixed
        //that has to do with searching for nested nodes
        if (json.Group) {
            for (var i=0; i<json.Group.length; i++) {
            this.processGroupNode(json.Group[i], folder);
            }
        }
        
        if (json.Map) {
            for (var i=0; i<json.Map.length; i++) {
                opt = {};
                opt.label = json.Name ? json.Name[0] : '';
                opt.data = json.ResourceId ? json.ResourceId[0] : '';
                opt.imgIcon = this.defIcon;
                var item = new Jx.TreeItem(opt);
                item.addSelectionListener(this);
                folder.append(item);
            }
        }
    },
    
    
    
    /**
     * remove the dom objects representing the legend layers and groups
     */
    clear: function() {
        while (this.oRoot.nodes.length > 0) {
            this.oRoot.remove(this.oRoot.nodes[0]);
        }
        this.mapGroups = [];
        this.maps = [];
    },
    selectionChanged: function(o) {
        if (this.currentNode) {
            Element.removeClassName(this.currentNode.domObj.childNodes[2], 'jxTreeSelectedNode');
        }
        this.currentNode = o;
        Element.addClassName(this.currentNode.domObj.childNodes[2], 'jxTreeSelectedNode');
        
        this.getMap().loadMap(this.currentNode.data);
    }
};