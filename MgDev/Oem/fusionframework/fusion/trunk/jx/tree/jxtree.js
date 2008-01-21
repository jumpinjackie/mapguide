/**
 * @project         Jx
 * @revision        $Id: jxtree.js 435 2007-10-16 20:23:34Z pspencer $
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 */

Jx.addStyleSheet('tree/tree.css');
Jx.addStyleSheet('tree/tree_ie.css', true);

/**
 * Jx.TreeItem is an item in a tree.  An item is a leaf node that has
 * no children.
 *
 * Jx.TreeItem supports selection listeners.  When an item in the tree
 * is selected by the user, listeners are called by invoking the
 * selectionChanged method on the listener and passing the tree item
 * object that was selected.  The application is responsible for
 * changing the style of the selected item in the tree and for
 * tracking selection if that is important.
 * 
 * When the selection changes, the mouse event that triggered the
 * selection change is passed to the listener as a lastEvent
 * member of the tree item object.  You can use this to determine
 * if the item was clicked, double-clicked, right clicked etc.
 */
Jx.TreeItem = Class.create();
Jx.TreeItem.prototype = {
    data : null,
    domObj : null,
    parent : null,
    currentClassName : null,
    onClick : null,
    sl : null,
    enabled : null,
    contextMenu : null,
    initialize : function( options ) {
        this.initializeItem(options);
    },
    initializeItem: function(options) {
        options = options || {};
        this.sl = [];
        var label = options.label || 'new node';
        this.data = options.data || null;
        this.contextMenu = options.contextMenu || null;
        
        this.imgNode = options.imgNode || Jx.baseURL + 'images/tree_none.png';
        this.imgIcon = options.imgIcon || Jx.baseURL + 'images/tree_page.png';
                
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxTreeNode';
        this.currentClassName = 'jxTreeNode';
      
        var elm = document.createElement('img');
        elm.className = 'jxTreeNodeImage';
        //elm.src = this.imgNode;
        Jx.addToImgQueue({domElement: elm, src: this.imgNode});
        this.domObj.appendChild(elm);
        
        elm = document.createElement('img');
        elm.className = 'jxTreeNodeIcon';
        //elm.src = this.imgIcon;
        Jx.addToImgQueue({domElement: elm, src: this.imgIcon});
        this.domObj.appendChild(elm);
      
        elm = document.createElement('a');
        elm.href = 'javascript:void(0)';
        elm.onclick = this.selected.bindAsEventListener(this);
        elm.ondblclick = this.selected.bindAsEventListener(this);
        elm.oncontextmenu = this.showMenu.bindAsEventListener(this);
        elm.innerHTML = label;
        this.domObj.appendChild(elm);
        
        //this.update(false);
        //TODO: this could cause a memory leak in remove
        this.domObj.jxTreeItem = this;
        this.domObj.childNodes[2].jxTreeItem = this;
        
        this.onClick = options.onClick || null;
        this.enabled = typeof options.enabled != 'undefined' ? options.enabled : true;
        if (this.enabled) {
            Element.removeClassName(this.domObj, 'jxDisabled');
        } else {
            Element.addClassName(this.domObj, 'jxDisabled');
        }
        var clearer = document.createElement('span');
        clearer.className = 'jxClearer';
        this.domObj.appendChild(clearer);
    },
    finalize: function() { this.finalizeItem(); },
    finalizeItem: function() {  
        if (!this.domObj) {
            return;
        }
        this.domObj.childNodes[2].onclick = null;
        this.domObj.childNodes[2].ondblclick = null;
        this.domObj.childNodes[2].oncontextmenu = null;
        this.contextMenu = null;
        this.onClick = null;
        for (var i=this.sl.length; i>=0; i--) {
            this.removeSelectionListener(this.sl[i]);
        }
        this.parent = null;
        this.domObj.jxTreeItem = null;
        this.domObj = null;
    },
    clone : function(node) {
        var options = { label : this.domObj.childNodes[2].innerHTML, 
                        data : this.data,
                        onClick : this.onClick,
                        imgNode : this.imgNode,
                        imgIcon : this.imgIcon
                        };
        var item = new Jx.TreeItem(options);
        return item;
    },
    update : function(bDescend) {
        var bLast
        if (arguments.length > 1) {
            bLast = arguments[1];
        } else {
            bLast = (this.parent && this.parent.isLastNode(this));
        }
        if (bLast) {
            Element.removeClassName(this.domObj, 'jxTreeNode');
            Element.addClassName(this.domObj, 'jxTreeNodeLast');
        } else {
            Element.removeClassName(this.domObj, 'jxTreeNodeLast');
            Element.addClassName(this.domObj, 'jxTreeNode');
        }
    },
    selected : function(e) {
        this.lastEvent = e?e:event;
        this.processEvent(this.sl,'selectionChanged',this);
    },
    showMenu: function(e) {
        this.lastEvent = e?e:event;
        this.processEvent(this.sl,'selectionChanged',this);
        if (this.contextMenu) {
            this.contextMenu.show(this.lastEvent);
        }
        Event.stop(e);
    },
    addSelectionListener: function(o){this.addListener(this.sl, o);},
    removeSelectionListener: function(o) {this.removeListener(this.sl, o);},
    getName : function() { return this.domObj.childNodes[2].innerHTML; },
    setName : function(name) { this.domObj.childNodes[2].innerHTML = name; },
    propertyChanged : function(o) {
        this.enabled = o.isEnabled();
        if (this.enabled) {
            Element.removeClassName(this.domObj, 'jxDisabled');
        } else {
            Element.addClassName(this.domObj, 'jxDisabled');
        }
    }
};
Object.extend(Jx.TreeItem.prototype, Jx.Listener.prototype);

Jx.TreeFolder = Class.create();
Object.extend(Jx.TreeFolder.prototype, Jx.TreeItem.prototype);
Object.extend(Jx.TreeFolder.prototype, {
    subDomObj : null,
    nodes : null,
    isOpen : false,
    dl : null,
    initialize : function( options ) {
        this.initializeFolder(options);
    },
    initializeFolder : function(options) {
        options = options || {};
        this.initializeItem(options);
        
        this.imgTreePlus = options.imgTreePlus || Jx.baseURL + 'images/tree_plus.png';
        this.imgTreeMinus = options.imgTreeMinus || Jx.baseURL + 'images/tree_minus.png';
        
        this.imgTreeFolder = options.imgTreeFolder || Jx.baseURL + 'images/tree_folder.png';
        this.imgTreeFolderOpen = options.imgTreeFolderOpen || Jx.baseURL + 'images/tree_folder_open.png';
        
        //console.log('imgTreePlus is ' + this.imgTreePlus);
        //this.domObj.childNodes[0].src = this.imgTreePlus;
        //this.domObj.childNodes[1].src = this.imgTreeFolder;
        Jx.addToImgQueue({domElement: this.domObj.childNodes[0], src: this.imgTreePlus});
        Jx.addToImgQueue({domElement: this.domObj.childNodes[1], src: this.imgTreeFolder});
        
        this.domObj.childNodes[0].onclick = this.clicked.bindAsEventListener(this);
                
        this.nodes = [];
        this.subDomObj = document.createElement('ul');
        this.domObj.appendChild(this.subDomObj);
        this.subDomObj.className = 'jxTree';
        this.isOpen = options.isOpen || false;
        this.dl = [];
        if (this.isOpen) {
            this.expand();
        } else {
            this.collapse();
        }
    //this.makeDroppable();
    },
    finalize: function() {
        this.finalizeFolder();
        this.finalizeItem();
        this.subDomObj = null;
    },
    finalizeFolder: function() {
        this.domObj.childNodes[0].onclick = null;
        for (var i=this.nodes.length-1; i>=0; i--) {
            this.nodes[i].finalize();
            if (this.nodes[i].domObj) this.subDomObj.removeChild(this.nodes[i].domObj);
            this.nodes.pop();
        }
        for (var i=this.dl.length-1; i>=0; i--) {
            this.removeDisclosureListener(this.dl[i]);
        }
    },
    /*
    makeDroppable : function() {
        if (!document.Jx.TreeCurrentTimeout) document.Jx.TreeCurrentTimeout = null;
        Droppables.add(this.domObj.childNodes[2], {
            greedy : true,
            onHover : function(draggable, droppable, overlap) { 
                var dropped = droppable.parentNode.jxTreeItem;
                if (document.Jx.TreeCurrentTimeout && 
                    document.currentDroppable != droppable) {
                    window.clearTimeout(document.Jx.TreeCurrentTimeout);
                    document.Jx.TreeCurrentTimeout = null;
                }
                if (!document.Jx.TreeCurrentTimeout && 
                    dropped.nodes && 
                    !dropped.isOpen)
                {
                    document.Jx.TreeCurrentTimeout = window.setTimeout(dropped.expand.bind(dropped), 700);
                }
                
                if (!document.currentHighlight != droppable) {
                    if (droppable == draggable)
                    {
                        Droppables.remove(droppable);
                    }
                    if (document.currentDroppable && 
                        document.currentDroppable != droppable) {
                        Element.removeClassName(document.currentDroppable, 
                                                'jxTreeDropzone');
                    }
                    
                    Element.addClassName(droppable, 'jxTreeDropzone');
                    document.currentDroppable = droppable;
                }
            },
            onDrop : function(draggable, droppable) {
                var dragged = draggable.jxTreeItem;
                var dropped = droppable.parentNode.jxTreeItem;

                Element.removeClassName(droppable, 
                                            'jxTreeDropzone');
                if (dropped.nodes) {
                    dragged.parent.remove(dragged);
                    dropped.append(dragged);
                    dropped.expand();
                } else if (dropped.parent != null) {
                    dragged.parent.remove(dragged);
                    dropped.parent.insert(dragged,dropped);
                }
            }
        });
    },
    */
    clone : function(node) {
        var options = { label : this.domObj.childNodes[2].innerHTML, 
                        data : this.data,
                        onClick : this.onClick,
                        imgTreePlus : this.imgTreePlus,
                        imgTreeMinus : this.imgTreeMinus,
                        imgTreeFolder : this.imgTreeFolder,
                        imgTreeFolderOpen : this.imgTreeFolderOpen
                        };
        var node = new Jx.TreeFolder(options);
        for (var i=0;i<this.nodes.length;i++) {
            node.append(this.nodes[i].clone());
        }
        return node;
    },
    isLastNode : function(node) {
        if (this.nodes.length == 0) {
            return false;
        } else {
            return this.nodes[this.nodes.length-1] == node;
        }
    },
    update : function(bDescend) {
        /* avoid update if not attached to tree yet */
        if (!this.parent) return;
        var bLast = false;
        if (arguments.length > 1) {
            bLast = arguments[1];
        } else {
            bLast = (this.parent && this.parent.isLastNode(this));
        }
        
        if (bLast) {
            this.domObj.className = 'jxTreeNodeLast';
            this.subDomObj.className = 'jxTree';
        } else {
            this.domObj.className = 'jxTreeNode';
            this.subDomObj.className = 'jxTree jxTreeNest';
        }
        
        if (this.isOpen) {
            //this.domObj.childNodes[0].src = this.imgTreeMinus;
            //this.domObj.childNodes[1].src = this.imgTreeFolderOpen;
            Jx.addToImgQueue({domElement: this.domObj.childNodes[0], src: this.imgTreeMinus});
            Jx.addToImgQueue({domElement: this.domObj.childNodes[1], src: this.imgTreeFolderOpen});
        } else {
            //this.domObj.childNodes[0].src = this.imgTreePlus;
            //this.domObj.childNodes[1].src = this.imgTreeFolder;
            Jx.addToImgQueue({domElement: this.domObj.childNodes[0], src: this.imgTreePlus});
            Jx.addToImgQueue({domElement: this.domObj.childNodes[1], src: this.imgTreeFolder});
        }
        
        if (this.nodes && bDescend) {
            //this.nodes.each(function(n){n.update(true);});
            for(var i=0; i<this.nodes.length; i++) {
                this.nodes[i].update(false, i==this.nodes.length-1);
            }
        }
    },
    /**
     * append a node at the end of the sub-tree
     * @param node {Object} the node to append.
     */
    append : function( node ) {
        node.parent = this;
        this.nodes.push(node);
        this.subDomObj.appendChild( node.domObj );
        this.update(true);
    },
    /**
     * insert a node after refNode.  If refNode is null, insert at beginning
     * @param node {Object} the node to insert
     * @param refNode {Object} the node to insert before
     */
    insert : function( node, refNode ) {
        node.parent = this;
        //if refNode is not supplied, insert at the beginning.
        if (!refNode) {
            this.nodes.unshift(node);
            //sanity check to make sure there is actually something there
            if (this.subDomObj.childNodes.length ==0) {
                this.subDomObj.appendChild(node.domObj);
            } else {
                this.subDomObj.insertBefore(node.domObj, this.subDomObj.childNodes[0]);                
            }
        } else {
            //walk all nodes looking for the ref node.  Track if it actually
            //happens so we can append if it fails.
            var b = false;
            for(var i=0;i<this.nodes.length;i++) {
                if (this.nodes[i] == refNode) {
                    //increment to append after ref node.  If this pushes us
                    //past the end, it'll get appended below anyway
                    i = i + 1;
                    if (i < this.nodes.length) {
                        this.nodes.splice(i, 0, node);
                        this.subDomObj.insertBefore(node.domObj, this.subDomObj.childNodes[i]);
                        b = true;
                        break;
                    }
                }
            }
            //if the node wasn't inserted, it is because refNode didn't exist
            //and so the fallback is to just append the node.
            if (!b) {
                this.nodes.push(node); 
                this.subDomObj.appendChild(node.domObj); 
            }
        }
        this.update(true);
        //if (this.parent)
        //    this.parent.update(true);        
    },
    /**
     * remove the specified node from the tree
     * @param node {Object} the node to remove
     */
    remove : function(node) {
        node.parent = null;
        for(var i=0;i<this.nodes.length;i++) {
            if (this.nodes[i] == node) {
                this.nodes.splice(i, 1);
                this.subDomObj.removeChild(this.subDomObj.childNodes[i]);
                break;
            }
        }
        this.update(true);
        //if (this.parent)
        //    this.parent.update(true);        
    },
    replace: function( newNode, refNode ) {
        //walk all nodes looking for the ref node. 
        var b = false;
        for(var i=0;i<this.nodes.length;i++) {
            if (this.nodes[i] == refNode) {
                if (i < this.nodes.length) {
                    newNode.parent = this;
                    this.nodes.splice(i, 1, newNode);
                    this.subDomObj.replaceChild(newNode.domObj, refNode.domObj);
                    return true;
                }
            }
        }
        return false;
    },
    
    /**
     * handle the user clicking on this folder by expanding or
     * collapsing it.
     * @param e {Event} the event object
     */
    clicked : function(e) {
        e = e?e:event;
        if (this.isOpen) {
            this.collapse();
        } else {
            this.expand();
        }
    },
    expand : function() {
        this.isOpen = true;
        this.subDomObj.style.display = 'block';
        this.update(true);
        this.processEvent(this.dl, 'disclosed', this);    
    },
    collapse : function() {
        this.isOpen = false;
        this.subDomObj.style.display = 'none';
        this.update(true);
        this.processEvent(this.dl, 'disclosed', this);
    },
    addDisclosureListener: function(o){this.addListener(this.dl, o);},
    removeDisclosureListener: function(o) {this.removeListener(this.dl, o);},
    findChild : function(path) {
        //path is empty - we are asking for this node
        if (path.length == 0)
            return this;
        
        //path has only one thing in it - looking for something in this folder
        if (path.length == 1)
        {
            for (var i=0; i<this.nodes.length; i++)
            {
                if (this.nodes[i].getName() == path[0])
                    return this.nodes[i];
            }
            return null;
        }
        //path has more than one thing in it, find a folder and descend into it    
        var childName = path.shift();
        for (var i=0; i<this.nodes.length; i++)
        {
            if (this.nodes[i].getName() == childName && this.nodes[i].findChild)
                return this.nodes[i].findChild(path);
        }
        return null;
    }
});

Jx.Tree = Class.create();
Object.extend( Jx.Tree.prototype, Jx.TreeFolder.prototype );
Object.extend( Jx.Tree.prototype, {
    initialize : function( id ) {
        this.subDomObj = document.createElement('ul');
        this.subDomObj.className = 'jxTreeRoot';
        $(id).appendChild(this.subDomObj);
        this.nodes = [];
        this.dl = [];
        this.isOpen = true;
    },
    finalize: function() { 
        this.clear(); 
        this.subDomObj.parentNode.removeChild(this.subDomObj); 
    },
    clear: function() {
        for (var i=this.nodes.length-1; i>=0; i--) {
            this.subDomObj.removeChild(this.nodes[i].domObj);
            this.nodes[i].finalize();
            this.nodes.pop();
        }
    },
    update : function(bDescend) {
        var bLast = true;
        if (this.subDomObj)
        {
            if (bLast) {
                Element.removeClassName(this.subDomObj, 'jxTreeNest');
            } else {
                Element.addClassName(this.subDomObj, 'jxTreeNest');
            }
        }
        if (this.nodes && bDescend)
            this.nodes.each(function(n){n.update(false);});
    },
    /**
     * append a node at the end of the sub-tree
     * @param node {Object} the node to append.
     */
    append : function( node ) {
        node.parent = this;
        this.nodes.push(node);
        this.subDomObj.appendChild( node.domObj );
        this.update(true);        
    }
});