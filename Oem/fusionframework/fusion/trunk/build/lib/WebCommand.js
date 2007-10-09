/*****************************************************************************
 *
 *
 * Purpose: Fusion
 *
 * Project: Fusion
 *
 * Author: DM Solutions Group Inc 
 *
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
 *
 *****************************************************************************/
/*
* $Id: WebCommand.js 667 2007-06-29 19:33:51Z pspencer $
*/

Fusion.Lib.WebCommand = Class.create();
Fusion.Lib.WebCommand.prototype = {
    sName : "",
    sLabel : "",
    sTooltip : "",
    sDescription : "",
    sImageurl : "",
    sDisabledImageURL : "",
    sTargetViewer : "",
    sType : 'CommandType',
    sLocation : 'widgets/',
    jsonNode : null,
    sAction : "",  
    sMapId : '',

    initialize : function(jsonNode) {
        this.jsonNode = Object.extend({},jsonNode);
        this.sType = jsonNode['@type'] ? jsonNode['@type'][0] : 'CommandType';
        if (jsonNode != null) {
            this.sName = jsonNode.Name ? jsonNode.Name[0] : '';
            this.sLabel = jsonNode.Label ? jsonNode.Label[0] : '';
            this.sTooltip = jsonNode.Tooltip ? jsonNode.Tooltip[0] : '';
            this.sDescription = jsonNode.Description ? jsonNode.Description[0] : '';
            this.sLocation = jsonNode.Location ? jsonNode.Location[0] : '';
            if (this.sLocation == '') {
                this.sLocation = 'widgets';
            }

            this.sTargetViewer = jsonNode.TargetViewer ? jsonNode.TargetViewer[0] : '';
            this.sAction = jsonNode.Action ? jsonNode.Action[0] : '';
            this.sMapId = jsonNode.MapId ? jsonNode.MapId[0] : '';

            var sImageurl = jsonNode.ImageURL ? jsonNode.ImageURL[0] : '';
            if (sImageurl.length > 4 && sImageurl.substring(0,4) == "http") {
                this.sImageurl = sImageurl;
            } else {
                if (this.sType == 'BasicCommandType') {
                    this.sImageurl = Fusion.getFusionURL() + sImageurl;
                } else {
                    this.sImageurl = sImageurl;
                }
            }

            var sDisabledImageURL = jsonNode.DisabledImageUrl ? jsonNode.DisabledImageUrl[0] : '';
            if (sImageurl.length > 4 && sImageurl.substring(0,4) == "http") {
                this.sDisabledImageURL = sDisabledImageURL;
            }
            else if (sDisabledImageURL == '') {
                sDisabledImageURL = sImageurl;
            } else {
                if (this.sType == 'BasicCommandType') {
                    this.sDisabledImageURL = Fusion.getFusionURL() + sDisabledImageURL;
                } else {
                    this.sDisabledImageURL = sDisabledImageURL;
                }
            }
        }
    },

    getName : function() {
        return this.sName;
    },

    setName : function(sName) {
        this.sName = sName;
    },

    getImageURL : function() {
        return this.sImageurl;
    },

    getDisabledImageURL : function() {
        return this.sDisabledImageURL;
    },

    getType : function() {
        return this.sType;
    },

    getAction : function() {
        return this.sAction;
    },

    getMap : function() {
        if (this.sMapId != '') {
            return Fusion.getMapById(this.sMapId);
        } else {
            //return first map if map id id not defined.
            return Fusion.getMapByIndice(0);
        }  
    }
};
