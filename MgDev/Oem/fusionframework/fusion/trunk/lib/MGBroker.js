/**
 * Fusion.Lib.MGBroker
 *
 * $Id: MGBroker.js 974 2007-10-16 20:55:51Z madair $
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
 
/****************************************************************************
 * Class: Fusion.Lib.MGBroker
 *
 * MGBroker is used to broker requests to the MapGuide Open Source
 * mapagent interface.  It is a very simple class that is configured
 * with a URL and credentials via the setSiteURL method and can
 * send requests to the server via the dispatchRequest method.
 */

Fusion.Lib.MGBroker = Class.create();
Fusion.Lib.MGBroker.prototype = {
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
     * default method for communicating with the server, if set
     * this overrides the individual request methods
     */
    method: null,
    
    /**
     * @class
     * MGBroker constructor
     *
     * @constructor
     * create a new MGBroker instance
     */
    initialize : function() { 
    },
    /**
     * send a request to the MapGuide Open Source server using
     * XMLHttpRequest and return the result to the specified
     * function.
     * @param r {Object} an MGRequest-subclass instance that
     *        defines the operation to request.
     * @param f {Function} a function object to invoke when the
     *        XMLHttpRequest call completes
     */
    dispatchRequest : function( r, f ) {
        var s = r.encode() + '&ts='+(new Date()).getTime();
        if (this.method) {
            r.options.method = this.method;
        }
        var a = new Ajax.Request( this.mapAgentURL, 
                 Object.extend({ parameters:r.parameters, onComplete:f }, r.options ) );
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
        this.mapAgentURL = url;
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

/****************************************************************************
 * Class: Fusion.Lib.MGRequest
 *
 * MGRequest is the base class for all broker-compatible requests.  A request
 * is a wrapper around an operation that is supported by the mapagent.
 */
Fusion.Lib.MGRequest = Class.create();
Fusion.Lib.MGRequest.prototype = {
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
     * initialize a new instance of MGRequest
     */
    initializeRequest : function() {
        this.options = { method:'post' };
        this.parameters = { version : '1.0.0', locale : Fusion.locale };
    },
    
    /**
     * set the parameters associated with this request.  Parameters are
     * dependent on the specific MGRequest subclass except for two
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

Fusion.Lib.MGRequest.MGEnumerateResources = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGEnumerateResources.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGEnumerateResources
 *
 * encapsulate a request to the server to enumerate resources in the library.
 */
Object.extend(  Fusion.Lib.MGRequest.MGEnumerateResources.prototype, {
    /**
     * @constructor
     * initialize a new instance of MGEnumerateResources
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
     * @return {Object} an instance of MGEnumerateResources
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

Fusion.Lib.MGRequest.MGGetResourceContent = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGGetResourceContent.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGGetResourceContent
 *
 * encapsulate a request to the server to get resource contents from the library.
 */
Object.extend(  Fusion.Lib.MGRequest.MGGetResourceContent.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetResourceContent
     *
     * @param resourceID {String} optional parameter indicating the resource
     * to enumerate.  If not set or null, it defaults to "Library://" which
     * is the root of the library.
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGGetResourceContent
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETRESOURCECONTENT',
            resourceid : (resourceID || "Library://")
        } );
    }
});

Fusion.Lib.MGRequest.MGGetResourceHeader = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGGetResourceHeader.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGGetResourceHeader
 *
 * encapsulate a request to the server to get resource header from the library.
 */
Object.extend(  Fusion.Lib.MGRequest.MGGetResourceHeader.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetResourceHeader
     *
     * @param resourceID {String} optional parameter indicating the resource
     * to enumerate.  If not set or null, it defaults to "Library://" which
     * is the root of the library.
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGGetResourceHeader
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETRESOURCEHEADER',
            resourceid : (resourceID || "Library://")
        } );
    }
});

Fusion.Lib.MGRequest.MGCreateSession = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGCreateSession.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGCreateSession
 *
 * encapsulate a request to the server to create a new session on the server.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGCreateSession.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGCreateSession
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGCreateSession
     */
    initialize : function( ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'CREATESESSION'
        } );
    }
});

Fusion.Lib.MGRequest.MGCopyResource = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGCopyResource.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGCopyResource
 *
 * encapsulate a request to the server to copy a resource.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGCopyResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGCopyResource
     *
     * @param sourceID {String} the Resource ID of the source
     * @param destinationID {String} the Resource ID of the destination
     * @param overwrite {Boolean} overwrite the destination if it exists
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGCopyResource
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

Fusion.Lib.MGRequest.MGDeleteResource = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGDeleteResource.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGDeleteResource
 *
 * encapsulate a request to the server to delete a resource.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGDeleteResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGDeleteResource
     *
     * @param resourceID {String} the id of the resource to delete
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGDeleteResource
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'DELETERESOURCE',
            resourceid : resourceID
        } );
    }
});

Fusion.Lib.MGRequest.MGMoveResource = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGMoveResource.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGMoveResource
 *
 * encapsulate a request to the server to move a resource in the repository.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGMoveResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGMoveResource
     *
     * @param sourceID {String} the Resource ID of the source
     * @param destinationID {String} the Resource ID of the destination
     * @param overwrite {Boolean} overwrite the destination if it exists
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGMoveResource
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

Fusion.Lib.MGRequest.MGMoveResource = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGMoveResource.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGMoveResource
 *
 * encapsulate a request to the server to set the content XML of a resource.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGMoveResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGMoveResource
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGMoveResource
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

Fusion.Lib.MGRequest.MGDescribeSchema = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGDescribeSchema.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGDescribeSchema
 *
 * encapsulate a request to the server to describe the schema of a FeatureSource.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGDescribeSchema.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGDescribeSchema
     *
     * @param resourceID {String} the id of the resource to describe the schema for
     * @param schema {String} what does this do?
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGDescribeSchema
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

Fusion.Lib.MGRequest.MGGetSpatialContexts = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGGetSpatialContexts.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGGetSpatialContexts
 *
 * encapsulate a request to the server to retrieve the spatial context of a resource.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGGetSpatialContexts.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetSpatialContexts
     *
     * @param resourceID {String} the id of the resource to retrieve the spatial context for
     * @param activeonly {Boolean} what does this do?
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGGetSpatialContexts
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

Fusion.Lib.MGRequest.MGEnumerateResourceReferences = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGEnumerateResourceReferences.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGEnumerateResourceReferences
 *
 * encapsulate a request to the server to enumerate the references to a resource id.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGEnumerateResourceReferences.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGEnumerateResourceReferences
     *
     * @param resourceID {String} the id of the resource to retrieve the spatial context for
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGEnumerateResourceReferences
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'ENUMERATERESOURCEREFERENCES',
            resourceid: resourceID
        } );
    }
});

Fusion.Lib.MGRequest.MGEnumerateResourceData = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGEnumerateResourceData.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGEnumerateResourceData
 *
 * encapsulate a request to the server to enumerate the data associated with
 * a FeatureSource
 * N.B. This does not enumerate resource data for 'unmanaged' FeatureSources
 *      (those referencing files or directories outside the respository)
 *      Fusion.Lib.MGRequest.MGDescribeSchema should be used for those sources.
 */
Object.extend(  Fusion.Lib.MGRequest.MGEnumerateResourceData.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGEnumerateResourceData
     *
     * @param resourceID {String} the id of the FeatureSource to retrieve data for
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGEnumerateResourceData
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'ENUMERATERESOURCEDATA',
            resourceid: resourceID
        } );
    }
});

Fusion.Lib.MGRequest.MGGetVisibleMapExtent = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGGetVisibleMapExtent.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGGetVisibleMapExtent
 *
 * encapsulate a request to the server to enumerate the data associated with
 * a FeatureSource
 * N.B. This does not enumerate resource data for 'unmanaged' FeatureSources
 *      (those referencing files or directories outside the respository)
 *      Fusion.Lib.MGRequest.MGDescribeSchema should be used for those sources.
 */
Object.extend(  Fusion.Lib.MGRequest.MGGetVisibleMapExtent.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetVisibleMapExtent
     *
     * @param sessionId {String} the id of the session to restore
     * @param mapName {String} the name of the map
     * @param viewCenterX {String} the horizontal center of the view
     * @param viewCenterY {String} the vertical center of the view
     * @param viewScale {String} the scale of the map
     * @param dataExtent {String} the extent of the data 
     * @param displayDpi {String} the DPI of the display
     * @param displayWidth {String} the width of the map
     * @param displayHeight {String} the height of the map
     * @param showLayers {String} a list of layer names to show
     * @param hideLayers {String} a list of layer names to hide
     * @param showGroups {String} a list of group names to show
     * @param hideGroups {String} a list of groupnames to hide
     * @param refreshLayers {String} a list of layers that need to be refreshed
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGGetVisibleMapExtent
     */
    initialize : function( sessionId, mapName, viewCenterX, viewCenterY,
                           viewScale, dataExtent, displayDpi, displayWidth, 
                           displayHeight, showLayers, hideLayers, 
                           showGroups, hideGroups, refreshLayers ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETVISIBLEMAPEXTENT',
            session: sessionId,
            mapname: mapName,
            setviewcenterx: viewCenterX,
            setviewcentery: viewCenterY,
            setviewscale: viewScale,
            setdataextent: dataExtent,
            setdisplaydpi: displayDpi,
            setdisplaywidth: displayWidth,
            setdisplayheight: displayHeight,
            showlayers: showLayers,
            hidelayers: hideLayers,
            showgroups: showGroups,
            hidegroups: hideGroups,
            refreshlayers: refreshLayers
        } );
    }
});


Fusion.Lib.MGRequest.MGQueryMapFeatures = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGQueryMapFeatures.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGQueryMapFeatures
 *
 * encapsulate a request to the server to query map features on 
 * selectable layers
 */
Object.extend(  Fusion.Lib.MGRequest.MGQueryMapFeatures.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGQueryMapFeatures
     *
     * @param sessionId {String} the id of the session to restore
     * @param mapName {String} the id of the session to restore
     * @param geometry (sting wkt} gemetry to use for selection.  Example : POLYGON(x1 y1, x2,y2)
     * @param maxFeatures {integer} number of maximum results (-1 to indicate no maximum)
     * @param selectionPersist {boolean} save the selection (valid values are 0 and 1) 
     * @param selectionVariant {String} indicates the spatial operation. Valid values are 'INTERSECTS', ...
     *@param layerNames {String} comma separated list of layer names to include in the query
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGQueryMapFeatures
     */
    initialize : function( sessionId, mapName, geometry, maxFeatures, persist, selectionVariant, layerNames ) 
    {
        this.initializeRequest();
        this.setParams( {
            operation : 'QUERYMAPFEATURES',
            session: sessionId,
            mapname: mapName,
            geometry: geometry,
            maxFeatures: maxFeatures,
            persist: persist,
            selectionVariant: selectionVariant,
            layerNames: layerNames
        } );
    }
});

Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope.prototype, Fusion.Lib.MGRequest.prototype);
/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope
 *
 * encapsulate a request to the server to query map features on 
 * selectable layers
 */
Object.extend(  Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope
     *
     * @param sessionId {String} the id of the session to restore
     * @param mapName {String} the id of the session to restore
     * @param features (String XML} a feature set selection XML
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope
     */
    initialize : function( sessionId, mapName, features ) 
    {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETFEATURESETENVELOPE',
            session: sessionId,
            mapname: mapName,
            featureSet: features
        } );
    }
});