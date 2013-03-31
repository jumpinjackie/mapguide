/**
 * Fusion.Lib.MGBroker
 *
 * $Id: MGBroker.js 2585 2012-09-07 14:01:25Z jng $
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
 * with a URL and credentials via the <Fusion.Lib.MGBroker.setSiteURL> method and can
 * send requests to the server via the <Fusion.Lib.MGBroker.dispatchRequest> method.
 */

Fusion.Lib.MGBroker = OpenLayers.Class({
    /**
     * the URL to a MapGuide Open Source installation.  Set this using
     * <Fusion.Lib.MGBroker.setSiteURL>
     * @type String
     */
    mapGuideURL : '',
    /**
     * the agent URL for the MapGuide Open Source installation.  Set from
     * <Fusion.Lib.MGBroker.setSiteURL>
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
     * Method: dispatchRequest
     * 
     * send a request to the MapGuide Open Source server using
     * XMLHttpRequest and return the result to the specified
     * function.
     * 
     * Parameters:
     * r - {Object} an <Fusion.Lib.MGRequest>-subclass instance that
     *        defines the operation to request.
     * f - {Function} a function object to invoke when the
     *        XMLHttpRequest call completes
     */
    dispatchRequest : function(r, f) {
        var s = r.encode() + '&ts='+(new Date()).getTime();
        if (this.method) {
            r.options.method = this.method;
        }
        var a = new OpenLayers.Ajax.Request( this.mapAgentURL, 
            OpenLayers.Util.extend({parameters:r.parameters, onComplete:f }, r.options));
        a.originalRequest = r;
    },
    /**
     * Method: setSiteURL
     * 
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
     * 
     * Parameters:
     * url - {String} a properly formatted universal reverse locator
     *        to a MapGuide Open Source installation.
     * user - {String} a valid user name
     * pass - {String} the password for the given user.
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
     * Method: clearSiteURL
     * 
     * remove all authentication information from the broker
     */
    clearSiteURL: function() {
        this.user = '';
        this.pass = '';
        this.mapGuideURL = '';
        this.mapAgentURL = '';
    }
});

/****************************************************************************
 * Class: Fusion.Lib.MGRequest
 *
 * MGRequest is the base class for all broker-compatible requests.  A request
 * is a wrapper around an operation that is supported by the mapagent.
 */
Fusion.Lib.MGRequest = OpenLayers.Class({
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
        this.parameters = { version : '1.0.0', locale : Fusion.locale, clientagent : 'Fusion Viewer' };
    },
    
    /**
     * Method: setParams
     * 
     * set the parameters associated with this request.  Parameters are
     * dependent on the specific MGRequest subclass except for two
     * mandatory parameters, version and locale, that are provided by
     * this base class.
     *
     * Parameters:
     * o - {Object} an object that contains named key : value pairs representing parameters to a request
     */
    setParams : function( o ){ OpenLayers.Util.extend( this.parameters, (o || {}) ); },

    /**
     * Method: setOptions
     * 
     * set the options associated with this request
     * 
     * Parameters:
     * o - {Object} an object that contains named key : value pairs representing for a request
     */
    setOptions : function( o ){ OpenLayers.Util.extend( this.options, (o || {}) ); },
    
    /**
     * Method: encode
     * 
     * returns a string containing all the request parameters in URL form suitable
     * for appending to a URL.
     * 
     * Return:
     * {String} the parameters in URL form.
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
});

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGEnumerateResources
 *
 * encapsulate a request to the server to enumerate resources in the library.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGEnumerateResources = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGEnumerateResources
     * 
     * initialize a new instance of MGEnumerateResources
     *
     * Parameters:
     * resourceID - {String} optional parameter indicating the resource
     *              to enumerate.  If not set or null, it defaults to "Library://" which
     *              is the root of the library.
     *
     * type - {String} optional parameter indicating the type of resources
     *        to enumerate.  If not set, it will default to an empty string which
     *        indicates all types will be returned.
     *
     * depth - {Integer} optional parameter that controls the depth of the
     *         resource tree to enumerate.  If not set, it will default to -1 which
     *         means the tree will be fully enumerated.
     *
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

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGGetResourceContent
 *
 * encapsulate a request to the server to get resource contents from the library.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGGetResourceContent = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGGetResourceContent
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetResourceContent
     *
     * Parameters:
     * resourceID - {String} optional parameter indicating the resource
     *              to enumerate.  If not set or null, it defaults to "Library://" which
     *              is the root of the library.
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETRESOURCECONTENT',
            resourceid : (resourceID || "Library://")
        } );
    }
});

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGGetResourceHeader
 *
 * encapsulate a request to the server to get resource header from the library.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGGetResourceHeader = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGGetResourceHeader
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetResourceHeader
     *
     * Parameters:
     * resourceID - {String} optional parameter indicating the resource
     *              to enumerate.  If not set or null, it defaults to "Library://" which
     *              is the root of the library.
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETRESOURCEHEADER',
            resourceid : (resourceID || "Library://")
        } );
    }
});

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGCreateSession
 *
 * encapsulate a request to the server to create a new session on the server.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGCreateSession = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGCreateSession
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGCreateSession
     */
    initialize : function( ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'CREATESESSION'
        } );
    }
});

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGCopyResource
 *
 * encapsulate a request to the server to copy a resource.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGCopyResource = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGCopyResource
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGCopyResource
     *
     * Parameters:
     * sourceID - {String} the Resource ID of the source
     * destinationID - {String} the Resource ID of the destination
     * overwrite - {Boolean} overwrite the destination if it exists
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

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGDeleteResource
 *
 * encapsulate a request to the server to delete a resource.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGDeleteResource = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGDeleteResource
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGDeleteResource
     *
     * Parameters:
     * resourceID - {String} the id of the resource to delete
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'DELETERESOURCE',
            resourceid : resourceID
        } );
    }
});

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGMoveResource
 *
 * encapsulate a request to the server to move a resource in the repository.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGMoveResource = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGMoveResource
     * initialize a new instance of Fusion.Lib.MGRequest.MGMoveResource
     *
     * Parameters:
     * sourceID - {String} the Resource ID of the source
     * destinationID - {String} the Resource ID of the destination
     * overwrite - {Boolean} overwrite the destination if it exists
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

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGMoveResource
 *
 * encapsulate a request to the server to set the content XML of a resource.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGMoveResource = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGMoveResource
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGMoveResource
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

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGDescribeSchema
 *
 * encapsulate a request to the server to describe the schema of a FeatureSource.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGDescribeSchema = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGDescribeSchema
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGDescribeSchema
     *
     * Parameters:
     * resourceID - {String} the id of the resource to describe the schema for
     * schema - {String} what does this do?
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

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGGetSpatialContexts
 *
 * encapsulate a request to the server to retrieve the spatial context of a resource.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGGetSpatialContexts = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGGetSpatialContexts
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetSpatialContexts
     *
     * Parameters:
     * resourceID - {String} the id of the resource to retrieve the spatial context for
     * activeonly - {Boolean} what does this do?
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

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGEnumerateResourceReferences
 *
 * encapsulate a request to the server to enumerate the references to a resource id.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGEnumerateResourceReferences = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGEnumerateResourceReferences
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGEnumerateResourceReferences
     *
     * Parameters:
     * resourceID - {String} the id of the resource to retrieve the spatial context for
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'ENUMERATERESOURCEREFERENCES',
            resourceid: resourceID
        } );
    }
});

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGEnumerateResourceData
 *
 * encapsulate a request to the server to enumerate the data associated with
 * a FeatureSource
 * N.B. This does not enumerate resource data for 'unmanaged' FeatureSources
 *      (those referencing files or directories outside the respository)
 *      <Fusion.Lib.MGRequest.MGDescribeSchema> should be used for those sources.
 *
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGEnumerateResourceData = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGEnumerateResourceData
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGEnumerateResourceData
     *
     * Parameters:
     * resourceID - {String} the id of the FeatureSource to retrieve data for
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'ENUMERATERESOURCEDATA',
            resourceid: resourceID
        } );
    }
});

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGGetVisibleMapExtent
 *
 * Encapsulate a request to the server to get the visible map extent
 *      
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGGetVisibleMapExtent = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGGetVisibleMapExtent
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetVisibleMapExtent
     *
     * Parameters:
     * sessionId - {String} the id of the session to restore
     * mapName - {String} the name of the map
     * viewCenterX - {String} the horizontal center of the view
     * viewCenterY - {String} the vertical center of the view
     * viewScale - {String} the scale of the map
     * dataExtent - {String} the extent of the data 
     * displayDpi - {String} the DPI of the display
     * displayWidth - {String} the width of the map
     * displayHeight - {String} the height of the map
     * showLayers - {String} a list of layer names to show
     * hideLayers - {String} a list of layer names to hide
     * showGroups - {String} a list of group names to show
     * hideGroups - {String} a list of groupnames to hide
     * refreshLayers - {String} a list of layers that need to be refreshed
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


/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGQueryMapFeatures
 *
 * encapsulate a request to the server to query map features on 
 * selectable layers
 * 
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGQueryMapFeatures = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGQueryMapFeatures
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGQueryMapFeatures
     *
     * Parameters:
     * sessionId - {String} the id of the session to restore
     * mapName - {String} the id of the session to restore
     * geometry (sting wkt} gemetry to use for selection.  Example : POLYGON(x1 y1, x2,y2)
     * maxFeatures - {integer} number of maximum results (-1 to indicate no maximum)
     * selectionPersist - {boolean} save the selection (valid values are 0 and 1) 
     * selectionVariant - {String} indicates the spatial operation. Valid values are 'INTERSECTS', ...
     * featureFilter - {String} filter crieteria to be applied for selection.
     * layerNames - {String} comma separated list of layer names to include in the query
     * layerAttributeFilter - {integer} bitmask determining layer selection behaviour (1=visible layers,
     *          2=selectable layers, 4=layers with tooltips)
     */
    initialize : function( sessionId, mapName, geometry, maxFeatures, persist, selectionVariant, featureFilter, layerNames, layerAttributeFilter ) 
    {
        this.initializeRequest();
        this.setParams( {
            operation : 'QUERYMAPFEATURES',
            format: "application/json",
            session: sessionId,
            mapname: mapName,
            geometry: geometry,
            maxFeatures: maxFeatures,
            persist: persist,
            selectionVariant: selectionVariant,
            featureFilter: featureFilter,
            layerNames: layerNames,
            layerAttributeFilter: layerAttributeFilter
        } );
    }
});

/****************************************************************************
 * Class: Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope
 *
 * encapsulate a request to the server to query map features on 
 * selectable layers
 * 
 * Inherits from:
 *  - <Fusion.Lib.MGRequest>
 */
Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope = OpenLayers.Class(Fusion.Lib.MGRequest, {
    /**
     * Constructor: Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope
     * 
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope
     *
     * Parameters:
     * sessionId - {String} the id of the session to restore
     * mapName - {String} the id of the session to restore
     * features - (String XML} a feature set selection XML
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
