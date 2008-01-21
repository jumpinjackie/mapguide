/**
 * @project         MapGuide Open Source Web Studio
 * @revision        $Id: WSBroker.js,v 1.28 2006/06/13 04:21:15 zak Exp $
 * @fileoverview    this file contains classes for communicating
 *                  with a MapGuide MapAgent
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
 * WSBroker is used to broker requests to the MapGuide Open Source
 * mapagent interface.  It is a very simple class that is configured
 * with a URL and credentials via the setSiteURL method and can
 * send requests to the server via the dispatchRequest method.
 */
var WSBroker = Class.create();
WSBroker.prototype = {
    /**
     * the URL to a MapGuide Open Source installation.  Set this using
     * setSiteURL
     * @type String
     */
    mapGuideURL : '',
    /**
     * the agent URL for the MapGuide Open Source installation.  Set from
     * setSiteURL
     * @type String
     */
    mapAgentURL : '',
    /**
     * @class
     * WSBroker constructor
     *
     * @constructor
     * create a new WSBroker instance
     */
    initialize : function() { 
    },
    /**
     * send a request to the MapGuide Open Source server using
     * XMLHttpRequest and return the result to the specified
     * function.
     * @param r {Object} an WSRequest-subclass instance that
     *        defines the operation to request.
     * @param f {Function} a function object to invoke when the
     *        XMLHttpRequest call completes
     */
    dispatchRequest : function( r, f ) {
        var ts = (new Date()).getTime();
        var a = new Ajax.Request( this.mapAgentURL, 
                          Object.extend({ parameters:r.encode() + '&ts='+ts+'&username='+this.user+'&password='+this.pass, onComplete:f }, r.options ) );
        a.originalRequest = r;
    },
    /**
     * set up a connection to a MapGuide Open Source site.  This function
     * expects that url is in the form http(s)://<address>/path-to-mapguide.
     * Path-to-mapguide is should be the base URL to a MapGuide Open
     * Source install.  It is expected that the mapagent is
     * in the expected place (mapagent/mapagent.fcgi) under that URL.  If
     * (for some strange reason) its not, then you can include the full
     * path to mapagent.fcgi in the URL and this function won't try to
     * guess its location.
     * The user name and password are passed on using basic HTML
     * authentication (http://<user>:<pass>@<server>/path-to-mapguide).
     * @param url {String} a properly formatted universal reverse locator
     *        to a MapGuide Open Source installation.
     * @param user {String} a valid user name
     * @param pass {String} the password for the given user.
     */
    setSiteURL : function(url, user, pass) {
        //url = url.replace('://', '://'+user+':'+pass+'@');
        this.user = user;
        this.pass = pass;
        if (url.indexOf('mapagent.fcgi') == -1) {
            if (url.charAt(url.length - 1) != '/') {
                url = url + '/';
            }
            this.mapGuideURL = url;            
            url = url + 'mapagent/mapagent.fcgi';
        }
        this.mapAgentURL = 'redirect.php?s=' + url;
    },
    /**
     * remove all authentication information from the broker
     */
    clearSiteURL: function() {
        this.user = '';
        this.pass = '';
        this.mapGuideURL = '';
        this.mapAgentURL = '';
    }
};

/**
 * WSRequest is the base class for all broker-compatible requests.  A request
 * is a wrapper around an operation that is supported by the mapagent.
 */
var WSRequest = Class.create();
WSRequest.prototype = {
    /**
     * core options shared by all requests
     */
    options : null,
    
    /**
     * core parameters shared by all requests
     */
    parameters : null,
    
    /**
     * @constructor
     * initialize a new instance of WSRequest
     */
    initializeRequest : function() {
        this.options = { method:'post' };
        this.parameters = { version : '1.0.0', locale : 'en' };
    },
    
    /**
     * set the parameters associated with this request.  Parameters are
     * dependent on the specific WSRequest subclass except for two
     * mandatory parameters, version and locale, that are provided by
     * this base class.
     *
     * @param o {Object} an object that contains named key : value pairs 
     * representing parameters to a request
     */
    setParams : function( o ){ Object.extend( this.parameters, (o || {}) ); },

    /**
     * set the options associated with this request
     * @param o {Object} an object that contains named key : value pairs 
     * representing for a request
     */
    setOptions : function( o ){ Object.extend( this.options, (o || {}) ); },
    
    /**
     * returns a string containing all the request parameters in URL form suitable
     * for appending to a URL.
     * @return {String} the parameters in URL form.
     */
    encode : function() {
        var s = sep = '';
        for (var p in this.parameters) {
            if (this.parameters[p]) {
                s = s + sep + p + '=' + encodeURI(this.parameters[p]);
            }
            sep = '&';
        }
        return s;
    }
};

var WSEnumerateResources = Class.create();
Object.extend(WSEnumerateResources.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 * @class WSEnumerateResources
 * encapsulate a request to the server to enumerate resources in the library.
 */
Object.extend(  WSEnumerateResources.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSEnumerateResources
     *
     * @param resourceID {String} optional parameter indicating the resource
     * to enumerate.  If not set or null, it defaults to "Library://" which
     * is the root of the library.
     *
     * @param type {String} optional parameter indicating the type of resources
     * to enumerate.  If not set, it will default to an empty string which
     * indicates all types will be returned.
     *
     * @param depth {Integer} optional parameter that controls the depth of the
     * resource tree to enumerate.  If not set, it will default to -1 which
     * means the tree will be fully enumerated.
     *
     * @return {Object} an instance of WSEnumerateResources
     */
    initialize : function( resourceID, type, depth ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'ENUMERATERESOURCES',
            resourceid : (resourceID || "Library://"),
            type : (type || ""),
            depth : (typeof depth == 'undefined' ? -1 : depth) } );
    }
});

var WSGetResourceContent = Class.create();
Object.extend(WSGetResourceContent.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 *
 * encapsulate a request to the server to get resource contents from the library.
 */
Object.extend(  WSGetResourceContent.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSGetResourceContent
     *
     * @param resourceID {String} optional parameter indicating the resource
     * to enumerate.  If not set or null, it defaults to "Library://" which
     * is the root of the library.
     *
     * @return {Object} an instance of WSGetResourceContent
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETRESOURCECONTENT',
            resourceid : (resourceID || "Library://")
        } );
    }
});

var WSGetResourceHeader = Class.create();
Object.extend(WSGetResourceHeader.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 *
 * encapsulate a request to the server to get resource header from the library.
 */
Object.extend(  WSGetResourceHeader.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSGetResourceHeader
     *
     * @param resourceID {String} optional parameter indicating the resource
     * to enumerate.  If not set or null, it defaults to "Library://" which
     * is the root of the library.
     *
     * @return {Object} an instance of WSGetResourceHeader
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETRESOURCEHEADER',
            resourceid : (resourceID || "Library://")
        } );
    }
});

var WSCreateSession = Class.create();
Object.extend(WSCreateSession.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 *
 * encapsulate a request to the server to create a new session on the server.
 *
 */
Object.extend(  WSCreateSession.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSCreateSession
     *
     * @return {Object} an instance of WSCreateSession
     */
    initialize : function( ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'CREATESESSION'
        } );
    }
});

var WSCopyResource = Class.create();
Object.extend(WSCopyResource.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 *
 * encapsulate a request to the server to copy a resource.
 *
 */
Object.extend(  WSCopyResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSCopyResource
     *
     * @param sourceID {String} the Resource ID of the source
     * @param destinationID {String} the Resource ID of the destination
     * @param overwrite {Boolean} overwrite the destination if it exists
     *
     * @return {Object} an instance of WSCopyResource
     */
    initialize : function( sourceID, destinationID, overwrite ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'COPYRESOURCE',
            source : sourceID,
            destination: destinationID,
            overwrite : overwrite
        } );
    }
});

var WSDeleteResource = Class.create();
Object.extend(WSDeleteResource.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 *
 * encapsulate a request to the server to delete a resource.
 *
 */
Object.extend(  WSDeleteResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSDeleteResource
     *
     * @param resourceID {String} the id of the resource to delete
     *
     * @return {Object} an instance of WSDeleteResource
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'DELETERESOURCE',
            resourceid : resourceID
        } );
    }
});

var WSMoveResource = Class.create();
Object.extend(WSMoveResource.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 *
 * encapsulate a request to the server to move a resource in the repository.
 *
 */
Object.extend(  WSMoveResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSMoveResource
     *
     * @param sourceID {String} the Resource ID of the source
     * @param destinationID {String} the Resource ID of the destination
     * @param overwrite {Boolean} overwrite the destination if it exists
     *
     * @return {Object} an instance of WSMoveResource
     */
    initialize : function( sourceID, destinationID, overwrite ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'MOVERESOURCE',
            source : sourceID,
            destination : destinationID,
            overwrite : overwrite
        } );
    }
});

var WSSetResource = Class.create();
Object.extend(WSSetResource.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 *
 * encapsulate a request to the server to set the content XML of a resource.
 *
 */
Object.extend(  WSSetResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSSetResource
     *
     * @return {Object} an instance of WSSetResource
     */
    initialize : function( resourceID, content, header ) {
        this.initializeRequest();
        this.setParams( {
            method: 'post', /* SetContent requires post method */
            operation : 'SETRESOURCE',
            resourceid : resourceID,
            content : content,
            header : header
        } );
    }
});

WSDescribeSchema
var WSDescribeSchema = Class.create();
Object.extend(WSDescribeSchema.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 *
 * encapsulate a request to the server to describe the schema of a FeatureSource.
 *
 */
Object.extend(  WSDescribeSchema.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSDescribeSchema
     *
     * @param resourceID {String} the id of the resource to describe the schema for
     * @param schema {String} what does this do?
     *
     * @return {Object} an instance of WSDescribeSchema
     */
    initialize : function( resourceID, schema ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'DESCRIBEFEATURESCHEMA',
            resourceid : resourceID,
            schema : schema
        } );
    }
});

var WSGetSpatialContexts = Class.create();
Object.extend(WSGetSpatialContexts.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 *
 * encapsulate a request to the server to retrieve the spatial context of a resource.
 *
 */
Object.extend(  WSGetSpatialContexts.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSGetSpatialContexts
     *
     * @param resourceID {String} the id of the resource to retrieve the spatial context for
     * @param activeonly {Boolean} what does this do?
     *
     * @return {Object} an instance of WSGetSpatialContexts
     */
    initialize : function(resourceID, activeonly) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETSPATIALCONTEXTS',
            resourceid : resourceID,
            activeonly : activeonly?'1':'0'
        } );
    }
});

var WSEnumerateResourceReferences = Class.create();
Object.extend(WSEnumerateResourceReferences.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 *
 * encapsulate a request to the server to enumerate the references to a resource id.
 *
 */
Object.extend(  WSEnumerateResourceReferences.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSEnumerateResourceReferences
     *
     * @param resourceID {String} the id of the resource to retrieve the spatial context for
     *
     * @return {Object} an instance of WSEnumerateResourceReferences
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'ENUMERATERESOURCEREFERENCES',
            resourceid: resourceID
        } );
    }
});

var WSEnumerateResourceData = Class.create();
Object.extend(WSEnumerateResourceData.prototype, WSRequest.prototype);
/**
 * @extends WSRequest
 *
 * encapsulate a request to the server to enumerate the data associated with
 * a FeatureSource
 * N.B. This does not enumerate resource data for 'unmanaged' FeatureSources
 *      (those referencing files or directories outside the respository)
 *      WSDescribeSchema should be used for those sources.
 */
Object.extend(  WSEnumerateResourceData.prototype, {
    /**
     * @constructor
     * initialize a new instance of WSEnumerateResourceData
     *
     * @param resourceID {String} the id of the FeatureSource to retrieve data for
     *
     * @return {Object} an instance of WSEnumerateResourceData
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'ENUMERATERESOURCEDATA',
            resourceid: resourceID
        } );
    }
});
