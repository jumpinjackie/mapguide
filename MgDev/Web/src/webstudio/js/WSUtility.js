/**
 * @project         MapGuide Open Source Web Studio
 * @revision        $Id: WSUtility.js,v 1.12 2006/06/16 20:48:18 zak Exp $
 * @fileoverview    this file contains utility classes for Web Studio
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @author          Zak James (zjames@dmsolutions.ca)
 * @author          Fred Warnock (fwarnock@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 2.1 of the GNU Lesser
 * General Public License as published by the Free Software Foundation.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
/**
 * WSResourceId provides resource id parsing services to the application.
 *
 * TODO: this isn't used everywhere yet, but it should be.
 */
var WSResourceId = Class.create();
WSResourceId.prototype = {
    /** {string} copy of the original resource id */
    resourceid: null,
    /** {string} the path (folders) up to the resource (but not including it) */
    path: null,
    /** {string} the name of the resource or an empty string if it is the root node */
    name: null,
    /** {string} the type of the resource or an empty string if it is a folder */
    type: null,
    /** {array} an array of folders in the path */
    aPath: null,
    /**
     * construct a new instance of WSResourceId.
     *
     * @param resourceId {String} the resource id that this class represents.
     */
    initialize: function( resourceId ) {
        this.resourceid = resourceId;
        //parse out the components of the path
        this.aPath = ['Library://'];
        if (resourceId.length > 10) {
            resourceId = resourceId.substr(10);
            this.aPath = this.aPath.concat(resourceId.split('/'));
            //check to see if the last thing was a / in which case we have an empty string
            if (this.aPath[this.aPath.length-1] == '') {
                this.aPath.pop();
            }
        }
        this.name = '';
        this.type = '';
        if (this.aPath.length > 1) {
            this.name = this.aPath.pop();
            idx = this.name.lastIndexOf('.');
            if (idx >= 0) {
                this.type = this.name.slice(idx+1);
                this.name = this.name.slice(0,idx);
            }
        }
    },
    /**
     * return the full resource id
     */
    getResourceId: function() {
        return this.resourceid;
    },
    /**
     * return the name of the resource without its path or type
     */
    getName: function() {
        return this.name;
    },
    /**
     * set the name of the resource
     */
    setName: function(name) {
        this.name = name;
    },
    /**
     * return the type of this resource
     */
    getType: function() {
        return this.type;
    },
    /**
     * return the path of this resource in the repository
     */
    getPath: function() {
        if (!this.path) {
            var sep = '';
            this.path = '';
            for (var i=0; i<this.aPath.length;i++) {
                this.path = this.path + sep + this.aPath[i];
                if (i>0) { sep = '/'; }
            }
        }
        return this.path;
    },
    /**
     * return the path of this resource in the repository as
     * an array of components
     */
    getPathAsArray: function() {
        return this.aPath;
    },
    /**
     * return the rid of this resource in the repository as
     * a session rid (for temporary storage)
     */
    getSessionResourceId: function(szSession) {
        var szPath =  this.getPath().replace(/Library:/,'Session:' + szSession);
        if (szPath[szPath.length-1] != '/') {
            szPath += '/';
        }
        szPath += this.getName();
        return (this.getType() != '')?szPath + '.' + this.getType(): szPath;
    }
    

};

/**
 * WSInPlaceEditor is a subclass of the scriptaculous Ajax.InPlaceEditor designed
 * to be used by the rename capability in the tree view. Functions for appearance
 * and submit handling have been overridden. Submission does not rely on server
 * processing since the broker and MapAgent are used to commit the rename to the
 * repository.
 */
var WSInPlaceEditor = Class.create();
Object.extend(WSInPlaceEditor.prototype, Ajax.InPlaceEditor.prototype);
Object.extend(WSInPlaceEditor.prototype, {
    onSubmit: function() {
        this.leaveEditMode();
        this.options.onComplete(this.element, this.editField.value);
    },
    onclickCancel: function() {
      this.options.onComplete(this.element, null);
      this.leaveEditMode();
      return false;
    },
    createForm: function() {
      this.form = document.createElement("form");
      this.form.id = this.options.formId;
      //Element.addClassName(this.form, this.options.formClassName)
      this.form.onsubmit = this.onSubmit.bind(this);

      this.createEditField();

      if (this.options.textarea) {
        var br = document.createElement("br");
        this.form.appendChild(br);
      }

      if (this.options.okButton) {
        okButton = document.createElement("input");
        okButton.type = "submit";
        okButton.value = this.options.okText;
        okButton.className = 'editor_ok_button';
        this.form.appendChild(okButton);
      }

      if (this.options.cancelLink) {
        cancelLink = document.createElement("a");
        cancelLink.href = "#";
        var cancelImg = document.createElement('img');
        cancelImg.className = 'png24'; /* apply png hack for IE */
        cancelImg.src = 'images/tree_close.png';
        cancelImg.alt = 'Cancel';
        cancelImg.title = 'Cancel';
        cancelLink.appendChild(cancelImg);
        cancelLink.onclick = this.onclickCancel.bind(this);
        cancelLink.className = 'editor_cancel';     
        this.form.appendChild(cancelLink);
      }
    },
    leaveEditMode: function() {
      Element.removeClassName(this.element, this.options.savingClassName);
      Element.removeClassName(this.element, this.options.hoverClassName);
      this.removeForm();
      //this.leaveHover();
      Element.show(this.element);
      Element.addClassName(this.element, 'selected');
      this.editing = false;
      this.saving = false;
      this.oldInnerHTML = null;
      this.onLeaveEditMode();
    }
    
});