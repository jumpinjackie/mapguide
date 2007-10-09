/*****************************************************************************
 * $Id: Error.js 737 2007-09-04 14:55:50Z pspencer $
 * Purpose: Fusion.Error - general error class for managing error information
 * Project: Fusion
 * Author: DM Solutions Group Inc 
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
 *****************************************************************************/
/**
 * Class: Fusion.Error
 * General purpose Error object
 */
Fusion.Error = Class.create();

Fusion.Error.FATAL = 0;
Fusion.Error.WARNING = 1;
Fusion.Error.NOTICE = 2;

Fusion.Error.prototype = {
    type: null,
    message: null,
    initialize: function(type, message) {
        this.type = type;
        this.message = message;
    },
    
    alert: function() {
        var type = this.typeToString(this.type);
        alert('Fusion Error: ' + type + '\n' + this.message);
    },
    
    toString: function() {
        var type = this.typeToString(this.type);
        return type + ": " + this.message;
    },
    
    typeToString: function(type) {
        switch (type) {
            case Fusion.Error.FATAL:
                return 'FATAL';
            case Fusion.Error.WARNING:
                return 'WARNING';
            case Fusion.Error.NOTICE:
                return 'NOTICE';
            default:
                return 'UNKNOWN ('+type+')';
        }
    }
};