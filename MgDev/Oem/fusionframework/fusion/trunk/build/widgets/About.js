/********************************************************************** * 
 * @project Fusion
 * @revision $Id: About.js 665 2007-06-29 14:49:06Z pspencer $
 * @purpose About widget
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
 * Class: About
 *
 * About widget to display a pop-up window about the application.  Configure in
 * WebLayout as:
 *
 * (start code)
 *       <Command>
 *         <Action>About</Action>
 *         <Name>About</Name>
 *         <Label>About</Label>
 *         <Tooltip>About</Tooltip>
 *         <Description>Display information about this application</Description>
 *         <AboutURL>/mapguide/mapadmin/about.php</AboutURL>
 *         <ImageURL />
 *         <DisabledImageURL />
 *         <TargetViewer>All</TargetViewer>
 *       </Command>
 * (end)
 *
 *  All properties are optional except <Action> and <Name>.
 * 
 * 
 * **********************************************************************/

Fusion.Widget.About = Class.create();
Fusion.Widget.About.prototype = 
{
    _nWidth : 500,
    _nHeight : 400,
    _sDefaultUrl : '/mapguide/mapadmin/about.php',  //TBD we need a Fusion specific About page

/*
 * Constructor: 
 *
 * Parameters:
 *
 * widgetTag - JSON formatted node for this widget from the WebLayout
 *
 */
    initialize : function(widgetTag) {
        //console.log('About.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        var json = widgetTag.extension;
        this._sAboutUrl = (json.AboutURL) ? 
                json.AboutURL[0] : this._sDefaultUrl;
        this.enable();
    },

    /**
     * Function: execute
     *
     * opens a pop-up window with the about information when invoked
     * 
     */
    execute : function() {
      //console.log('About.execute');

      var sFeatures = 'menubar=no,location=no,resizable=no,status=no';
      sFeatures += ',width=' + this._nWidth;
      sFeatures += ',height=' + this._nHeight;
      window.open(this._sAboutUrl, 'AboutPopup', sFeatures);
    }
};
