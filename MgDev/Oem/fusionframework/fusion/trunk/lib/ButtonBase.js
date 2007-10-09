/********************************************************************** * 
 * @project Fusion
 * @revision $Id: ButtonBase.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose Base widget for button type widgets
 * @author yassefa@dmsolutions.ca
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
 * Utility base class for a button type widget.
 *
 * Configurations for the buttons come from :
 * - html area : corresponds to the web layout command name
 * - width,height : default are 20,20. Classes should redifine teh
 *   getButtonWidth and getButtonHeight to provide othe values
 * - imageurl : image that will be used when the button is active.
 *   Default value is read from the web layout command imageurl parameter.
 *   Classes can redifine getImageURL.
 * - imageurl : image that will be used when the button is active.
 *   Default value is read from the web layout command disabledimageurl 
 *   parameter.  Classes can redifine getDisabledImageURL.
 * 
 * Clases inheriting should redefine the function activateTool which is
 * called when the button is clicked
 * **********************************************************************/

 
Fusion.Tool.ButtonBase = Class.create();
Fusion.Tool.ButtonBase.prototype = {
    /**
     * constructor
     */
    initialize : function() {
        /* overload enable/disable.  Normal inheritance should
         * work but because we use inheritFrom, it doesn't overload
         * Widget's enable/disable functions.  We do it manually
         * here.
         */
        this.enable = Fusion.Tool.ButtonBase.prototype.enable;
        this.disable = Fusion.Tool.ButtonBase.prototype.disable;

        //console.log('Fusion.Tool.ButtonBase.initialize');

        this._oButton = new Fusion.Tool.Button(this.domObj, this.widgetTag);
        if (!this.isEnabled()) {
            this._oButton.disableTool();
        }
        this.clickWatcher = this.clickCB.bind(this);
        this._oButton.observeEvent('click', this.clickWatcher);
    },
    
    clickCB : function(e) {
        //console.log('Fusion.Tool.ButtonBase.clickCB');
        if (this.isEnabled()) {
            this.activateTool();
        }
        /* I put this in to prevent the context menu from activating tools twice but it doesn't seem to be needed now */
        /* Event.stop(e); */
        //remove the focus on the button to prevent a problem in IE with some
        //buttons retaining their background colour when they shouldn't
        this._oButton._oButton.domObj.blur();
    },

    activateTool :  function() {
        //console.log('Fusion.Tool.ButtonBase.activateTool');
        if (this.execute) {
            this.execute();
        }
        if (this.group) {
            this._oButton.activateTool();
            for (var i=0; i<this.groups[this.group].length; i++) {
                if (this.groups[this.group][i] != this) {
                    this.groups[this.group][i]._oButton.deactivateTool();
                }
            }
        }
    },
    
    enable: function() {
        //console.log('button base enable');
        Fusion.Widget.prototype.enable.apply(this,[]);
        this._oButton.enableTool();
    },
    
    disable: function() {
        //console.log('button base disable');
        Fusion.Widget.prototype.disable.apply(this,[]);
        this._oButton.disableTool();
    }
};
