/********************************************************************** * 
 * @project Fusion
 * @revision $Id: $
 * @purpose Base widget for menu type widgets
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
 * **********************************************************************/

 
Fusion.Tool.MenuBase = Class.create();
Fusion.Tool.MenuBase.prototype = {
    /**
     * constructor
     */
    initialize : function() {
        /* overload enable/disable.  Normal inheritance should
         * work but because we use inheritFrom, it doesn't overload
         * Widget's enable/disable functions.  We do it manually
         * here.
         */
        this.enable = Fusion.Tool.MenuBase.prototype.enable;
        this.disable = Fusion.Tool.MenuBase.prototype.disable;

        //console.log('Fusion.Tool.MenuBase.initialize');
            var options = {};
            options.imgPath = this.widgetTag.imageUrl;
            options.imgClass = this.widgetTag.imageClass;
            options.tooltip = this.widgetTag.tooltip;
            options.label = this.widgetTag.label;

        this.oMenu = new Jx.Menu(options);
$(this.widgetTag.name).appendChild(this.oMenu.domObj);
    },
    
    enable: function() {
        //TODO: figure out how to enable and disable menu widgets
        Fusion.Widget.prototype.enable.apply(this,[]);
    },
    
    disable: function() {
        Fusion.Widget.prototype.disable.apply(this,[]);
    }
};
