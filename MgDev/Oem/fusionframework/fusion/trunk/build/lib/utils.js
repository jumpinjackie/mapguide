/**
 * Utils
 *
 * $Id: utils.js 967 2007-10-16 16:03:17Z madair $
 *
 * Copyright (c) 2007, DM Solutions Group Inc.
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

var DomNode = Class.create();

DomNode.prototype = {
    initialize: function( xmlNode /*, parent */ ) {
        this.textContent = '';
        this.nodeName = xmlNode?xmlNode.nodeName:'';
        this.parentNode = arguments[1]?arguments[1]:null;
        this.childNodes  = [];
        this.attributes = [];
        this.attributeNames = [];
        if (xmlNode) {
            if (xmlNode.attributes) {
                for (var i=0; i<xmlNode.attributes.length; i++) {
                    this.attributeNames.push(xmlNode.attributes[i].name);
                    this.attributes[xmlNode.attributes[i].name] = xmlNode.attributes[i].nodeValue;
                }
            }
            for (var i=0; i<xmlNode.childNodes.length; i++) {
              if (xmlNode.childNodes[i].nodeType != 3) {
                this.childNodes.push(new DomNode(xmlNode.childNodes[i], this));
              } else {
                this.textContent = this.textContent + xmlNode.childNodes[i].nodeValue;
              }
            }
        }
        this._currentNode = 0;
    },
    getAttribute: function(name) {
        return typeof(this.attributes[name]) != 'undefined' ? this.attributes[name] : null;
    },
    removeChild: function(child) {
        var result = null;
        for(var i=0; i<this.childNodes.length; i++) {
            if (this.childNodes[i] == child) {
                this._currentNode = 0;
                this.childNodes.splice(i,1);
                child.parentNode = null;
                result = child;
                break;
            }
        }
        return result;
    },
    appendChild: function(child) {
        if (child.parentNode) {
            child.parentNode.removeChild(child);
        }
        child.parentNode = this;
        this.childNodes.push(child);
    },
    insertBefore: function(newChild,refChild) {
        var bInserted = false;
        if (refChild) {
            for (var i=0; i<this.childNodes.length; i++) {
                if (this.childNodes[i] == refChild) {
                    if (newChild.parentNode) {
                        newChild.parentNode.removeChild(child);
                    }
                    newChild.parentNode = this;
                    this.childNodes.splice(i,0,newChild);
                    bInserted = true;
                    break;
                }
            }
        }
        if (!bInserted) {
            this.appendChild(newChild);
        }
    },
    toString: function(depth) {
        var s = '';
        var spacer = '';
        for (i=0; i<depth; i++) {
            spacer = spacer + '';
        }
        s = spacer + '&lt;' + this.nodeName;
        if (this.attributes.length > 0) {
            for (var name in this.attributes) {
                if (typeof(this.attributes[name]) == 'String' ) {
                    s = s + ' ' + name + '="' + this.attributes[name] + '"';
                }
            }
        }
        s = s + '&gt;';
        if (this.childNodes.length == 0) {
            s = s + this.textContent;
            spacer = '';
        } else {  
            s = s + '\n';
        }
        for (var i=0; i<this.childNodes.length; i++) {
            s = s + this.childNodes[i].toString( depth + 1 );
        }
        s = s + spacer + '&lt;/'+this.nodeName+'&gt;';
        return s;
    },
    toXML: function() {
        var s = this.parentNode?'':'<?xml version="1.0" encoding="UTF-8"?>\n';
        s = s+ '<' + this.nodeName;
        if (this.attributeNames.length > 0) {
            for (var i=0; i<this.attributeNames.length; i++) {
                var name = this.attributeNames[i];
                s = s + ' ' + name + '="' + this.attributes[name] + '"';
            }
        }
        s = s + '>';
        if (this.childNodes.length == 0) {
            var content = this.textContent + ''; //force string value if textContent was automatically made to a number
            content = content.replace('&','&amp;');
            content = content.replace(/</g, encodeURIComponent('&lt;'));
            content = content.replace(/>/g, encodeURIComponent('&gt;'));
            s = s + content;
            
        }
        for (var i=0; i<this.childNodes.length; i++) {
            s = s + this.childNodes[i].toXML();
        }
        s = s + '</'+this.nodeName+'>\n';
        return s;
    },
    getNodeText: function(name) {
        var s = '';
        var n = this.findFirstNode(name);
        if (n) {
            s = n.textContent;
        }
        return s;
    },
    setNodeText: function(name, value) {
        var n = this.findFirstNode(name);
        if (n) {
            n.setTextContent(value);
        }
    },
    setTextContent: function(value) {
        this.textContent = value;
    },
    setAttribute: function(name, value) {
        if (typeof this.attributes[name] == 'undefined') {
            this.attributeNames.push(name);
        }
        this.attributes[name] = value;
    },
    findFirstNode: function( name ) {
        this._currentNode = 0;
        if (this.nodeName == name) {
            return this;
        } else {
            for (var i=0; i<this.childNodes.length; i++) {
                var node = this.childNodes[i].findFirstNode(name);
                if (node) {
                    if (node.parentNode == this) {
                        this._currentNode = i + 1;
                    } else {
                        this._currentNode = i;          
                    }
                    return node;
                }
            }
            return false;
        }
    },
    findNextNode: function( name ) {
        if (this.nodeName == name) {
            return this;
        } else {
            for (var i=this._currentNode; i<this.childNodes.length; i++) {
                var node = this.childNodes[i].findNextNode(name);
                if (node) {
                    if (node.parentNode == this) {
                        this._currentNode = i + 1;
                    } else {
                        this._currentNode = i;          
                    }
                    return node;
                }
            }
            return false;
        } 
    }
};

var DomNodeFactory = {
    create: function( name, value ) {
        var node = new DomNode();
        node.nodeName = name;
        node.textContent = value || '';
        return node;
    }
};