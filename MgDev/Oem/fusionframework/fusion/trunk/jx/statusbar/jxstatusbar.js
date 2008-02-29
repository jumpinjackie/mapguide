/**
 * @project         Jx
 * @revision        $Id: jxstatusbar.js 324 2007-06-20 19:04:54Z pspencer $
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 */


/**
 *
 */
Jx.Statusbar = Class.create();
Jx.Statusbar.prototype = {
    items : null,
    domObj : null,
    initialize : function(domObj) {
        this.domObj = document.createElement('ul');
        this.domObj.className = 'jxStatusBar';
        $(domObj).appendChild(this.domObj);
    },
    add : function( ) {
        for (var i=0; i<arguments.length; i++) {
            arguments[i].statusbar = this;
            var li = document.createElement('li');
            li.className = 'jxTray';
            li.appendChild(arguments[i].domObj);
            this.domObj.appendChild(li);
        }
    }
};

/**
 *
 */
Jx.StatusbarItem = Class.create();
Jx.StatusbarItem.prototype = {
    domObj: null,
    statusbar: null,
    initialize : function( ) {
        this.domObj = document.createElement('div');
    },
    
    setContentHTML: function(s) {
        this.domObj.innerHTML = s;
    },
    
    setContentObj: function(o) {
        this.domObj.appendChild(o);
    }
};