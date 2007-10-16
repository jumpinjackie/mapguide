/********************************************************************** * 
 * @project Fusion
 * @revision $Id $
 * @purpose ViewOptions widget
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
 * ViewOptions 
 *
 * To put a ViewOptions control in your application, you first need to add a
 * widget to your ApplicationDefinition as follows:
 *
 *
 * The important parts of this Command are:
 *
 * Name (string, mandatory) 
 * 
 * an element with an id that is the same as this name must be in
 * the application.  For instance:
 *
 * <div id="MyViewOptions"></div>
 *
 * The ViewOptions will appear inside the element you provide.
 * 
 * **********************************************************************/


Fusion.Widget.ViewOptions = Class.create();
Fusion.Widget.ViewOptions.prototype = 
{
    displayUnits: false,
    options : {
        'Imperial': 'Miles', 
        'Metric': 'Meters',
        'Degrees': 'Degrees'
    },

    initialize : function(widgetTag) {
        //console.log('ViewOptions.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.MenuBase.prototype, []);
        //this.enable();
        
        var json = widgetTag.extension;
        
        //set up the root menu
        
        for (var key in this.options) {
          var action = new Jx.Action(this.setViewOptions.bind(this, this.options[key]));
          var menuItem = new Jx.MenuItem(action, {label: key} );
          this.oMenu.add(menuItem);
        }

        this.displayUnits = json.DisplayUnits ? json.DisplayUnits[0] : false;
        if (!this.displayUnits) {
            this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.setMapUnits.bind(this));
        }
    },
    
    //action to perform when the button is clicked
    activateTool: function(e) {
        this.oMenu.show(e);
    },

    setMapUnits: function() {
      this.setViewOptions(this.getMap().getUnits());
    },
    
    setViewOptions: function(data) {
      for (var i=0; i<Fusion.applicationDefinition.widgetSets.length; ++i) {
        var widgetSet = Fusion.applicationDefinition.widgetSets[i];
        for (var j=0; j<widgetSet.widgetInstances.length; ++j) {
          var widget = widgetSet.widgetInstances[j];
          for (var k=0; k<widget.paramRegister.length; ++k) {
            if (widget.paramRegister[k] == 'Units') {
              widget.setParameter('Units', data);
            }
          }
        }
      }
    }
};