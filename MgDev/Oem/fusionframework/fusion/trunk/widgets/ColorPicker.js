/********************************************************************** * 
 * @project Fusion
 * @revision $Id: ColorPicker.js 864 2007-10-06 19:06:56Z madair $
 * @purpose Select a color for some purpose
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
 * The user can pick from a palette of web-safe colors or enter a hex value
 *
 * **********************************************************************/


Fusion.Widget.ColorPicker = Class.create();
Fusion.Widget.ColorPicker.prototype = 
{
    /* HTML input element that is used to store both the initial
       value for this widget and receives the color value as the
       color changes */
    colorInput: null,

    /* Int (0-100) containing the alpha chosen by the user */
    alpha: 100,
    
    /* String containing the HEX value of the color chosen by the
       user, in RRGGBB format */
    color: '#000000',
    
    colorButton: null,
    
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);        
        
        var json = widgetTag.extension;
        if (json.ColorInputId) {
            this.colorInput = $(json.ColorInputId[0]);
        }
        
        if (this.colorInput) {
            this.alpha = 100 * parseInt('0x'+this.colorInput.value.substring(0,2))/255;
            this.color = '#'+this.colorInput.value.substring(2);
            this.colorInput.widget = this;
        }
        
        this.colorButton = new Jx.Button.Color({color: this.color, alpha: this.alpha, label: widgetTag.label, tooltip: widgetTag.tooltip});
        this.colorButton.addColorChangeListener(this);
        
        if (this.domObj) {
            this.domObj.appendChild(this.colorButton.domObj);
        }
    },
    
    colorChanged: function(button) {
        var a = parseInt(this.colorButton.alpha*255/100).toString(16);
        var c = a + this.colorButton.color.substring(1);
        //console.log('colorChanged: '+c);
        if (this.colorInput) {
            this.colorInput.value = c;
        }
    }
};