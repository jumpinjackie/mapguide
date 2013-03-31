/**
 * Fusion
 *
 * $Id: fusion.js 2629 2012-12-20 02:38:35Z liuar $
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

/**
 * declare global namespace object for Jx library to use
 */
var Jx = {};

/**
 * reverse inheritance logic to allow for delayed loading of dependencies.
 * Under normal circumstances, Object.extend from Prototype would be used,
 * but in Fusion, widget code is loaded before base class code and the
 * extend function won't work until all the base class code is available.
 * DEPRECATED
 */
Object.inheritFrom = function(destination, source, args) {
    var parent;
    if(typeof source == "function") {
        // get the prototype of the superclass
        parent = source.prototype;
    } else {
        // in this case we're extending with the prototype
        parent = source;
    }
    for (property in parent) {
        if (typeof destination[property] == 'undefined') {
            destination[property] = parent[property];
        }
    }
    if (parent.initialize) {
        parent.initialize.apply(destination, args);
    }
};

(function() {
    /**
     * Before creating the OpenLayers namespace, check to see if
     * OpenLayers.singleFile is true.  This occurs if the
     * OpenLayers/SingleFile.js script is included before this one - as is the
     * case with single file builds.
     */
    var singleFile = (typeof Fusion == "object" && Fusion.singleFile);

    /**
     * Namespace: Fusion
     * This is the top-level namespace of the Fusion library
     */
    window.Fusion = {
        
        /***************************************************************************
         * A set of utility classes that provide common tools for applications
         ***************************************************************************/
        Tool: {},

        /***************************************************************************
         * A set of classes that implement the functionlity required for applications
         ***************************************************************************/
        Widget: {},

        /***************************************************************************
         * A class to provide for event handling at application runtime
         ***************************************************************************/
        Event: {lastEventId: 1},

        /***************************************************************************
         * A set of classes to provide core funtionality
         ***************************************************************************/
        Lib: {},

        /***************************************************************************
         * A set of classes that implement layer functionality for various server 
         * technologies.
         ***************************************************************************/
        Layers: {},

        /***************************************************************************
         * Application scope constants
         ***************************************************************************/
        Constant: {},
    
        /**
         * Pointer to the Application Definition, parsed into all the Fusion 
         * objects: MapGroups, WidgetSets
         */
        applicationDefinition: null,
    
        /**
         * URL of a proxy script if the map server is on a different domain/port
         * that the application template to get around the same-origin policy
         */
        sRedirectScript: "",  
        
        /**
         * A flag to force XHR calls through the proxy, defaults to false
         */
        bForceRedirect: false,
        
        /**
         * the script programming language for server-side programming.  
         * Typically set to 'php' but other server languages may be used. 
         */
        sScriptLang: "php",
        
        /**
         * the locale setting for the browser and for localization of Fusion.
         * This is the same as the locale setting in OpenLayers and is used as 
         * the file name for localization files in the /text subdirectory 
         */
        locale: 'en',
        
        /**
         * the hash of strings used for localization of Fusion.  Strings are
         * indexed by a key which is used in calls to OpenLayers.i18n()
         */
        Strings: {},    //locale specific strings
    
        /** URL to the directory from which fusion.js was loaded */
        fusionURL: null,
    
        /** 
         * configuration object that holds the server configuration as
         * loaded from fusion/config.json
         */
        configuration: null,
    
        /**
        * broker instance for communicating with the mapagent, typically only
        * used with MapGuide
        */
        oBroker: null,
    
        /** An array of scripts that are queued to be loaded */
        aScripts: [],
        /** An array of scripts that are currently being loaded */
        aLoadingScripts: [],
        
        /** 
        * The current state during initialization of the application.  Valid 
        * states are enumerated below 
        */
        loadState: null,
 
        /** API loading has not begun */
        UNLOADED: 0,
        /** Load the configuration file for the application */
        LOAD_CONFIG: 1,
        /** Load the widget files required by the application */
        LOAD_WIDGETS: 2,
        /** Loading is complete */
        LOAD_COMPLETE: 3,
    
        /** 
        * unit related stuff - the following values are the keys into the 
        * unit related arrays below: aUnitPerMeter, aMeterPerUnit, aUnitNames
        * and aUnitAbbr
        */

        /**
         * Constant: UNKNOWN
         * 
         * An unknown unit
         */
        UNKNOWN: 0,
        /**
         * Constant: INCHES
         * 
         * Inch unit
         */
        INCHES: 1,
        /**
         * Constant: FEET
         * 
         * Feet unit
         */
        FEET: 2,
        /**
         * Constant: YARDS
         * 
         * Yard unit
         */
        YARDS: 3,
        /**
         * Constant: MILES
         * 
         * Mile unit
         */
        MILES: 4,
        /**
         * Constant: NAUTICALMILES
         * 
         * Nautical Mile unit
         */
        NAUTICALMILES: 5,
        /**
         * Constant: MILLIMETERS
         * 
         * Millimeter unit
         */
        MILLIMETERS: 6,
        /**
         * Constant: CENTIMETERS
         * 
         * Centimeter unit
         */
        CENTIMETERS: 7,
        /**
         * Constant: METERS
         * 
         * Meter unit
         */
        METERS: 8,
        /**
         * Constant: KILOMETERS
         * 
         * Kilometer unit
         */
        KILOMETERS: 9,
        /**
         * Constant: DEGREES
         * 
         * Degree unit
         */
        DEGREES: 10,
        /**
         * Constant: DECIMALDEGREES
         * 
         * Decimal Degree unit
         */
        DECIMALDEGREES: 11,
        /**
         * Constant: DMS
         * 
         * DMS unit
         */
        DMS: 12,
        /**
         * Constant: PIXELS
         * 
         * Pixel unit
         */
        PIXELS: 13,
        /** 
        * conversion factors to go from the specified units to meters
        */
        aUnitPerMeter: [1.0, /* 0 - UNKNOWN */
                        39.37, /* 1 - INCHES */
                        3.2808, /* 2 - FEET */
                        1.0936133, /* 3 - YARDS */
                        0.00062137, /* 4 - MILES */
                        0.000539956803, /* 5 - NAUTICAL MILES */
                        1000.0, /* 6 - MILLIMETERS */
                        100.0, /* 7 - CENTIMETERS */
                        1.0, /* 8- METERS */
                        0.001, /* 9 - KILOMETERS */
                        0.000009044, /* 10 - DEGREES */
                        0.000009044, /* 11 - DECIMALDEGREES */
                        0.000009044, /* 12 - DMS */
                        1.0 /* 13 - PIXELS */
                        ],
        /** 
        * conversion factors to go from meters to the specified units
        */
        aMeterPerUnit: [1.0, /* 0 - UNKNOWN */
                        0.0254, /* 1 - INCHES */
                        0.3048, /* 2 - FEET */
                        0.9144, /* 3 - YARDS */
                        1609.344, /* 4 - MILES */
                        1852, /* 5 - NAUTICAL MILES */
                        0.001, /* 6 - MILLIMETERS */
                        0.01, /* 7 - CENTIMETERS */
                        1.0, /* 8- METERS */
                        1000.0, /* 9 - KILOMETERS */
                        111061.75033, /* 10 - DEGREES */
                        111061.75033, /* 11 - DECIMALDEGREES */
                        111061.75033, /* 12 - DMS */
                        1.0 /* 13 - PIXELS */],
                        
        /** 
        * the units names spelled out fully
        */
        aUnitNames: ['Unknown','Inches', 'Feet', 'Yards', 'Miles', 'Nautical Miles',
                     'Millimeters', 'Centimeters', 'Meters', 'Kilometers', 
                     'Degrees', 'Decimal Degrees', 'Degrees Minutes Seconds', 'Pixels'],
                     
        /** 
        * unit names abbreviations
        */
        aUnitAbbr: ['unk', 'in', 'ft', 'yd', 'mi', 'nm', 
                    'mm', 'cm', 'm', 'km', 
                    '&deg;', '&deg;', '&deg;', 'px'],
        
        /**
         * Function: initialize
         *
         * Initialize the Fusion application.  This method must be called from
         * the template's onload method once all script files have been loaded.
         * Applications can register for the event {<Fusion.Event.FUSION_INITIALIZED>}
         * to carry out further application initialization when all the Fusion
         * objects have been created and are available.
         *
         * Valid optional parameters that can be passed include:
         * 
         * {String} applicationDefinitionURL: a URL or resource ID 
         *              for an ApplicationDefinition file
         *          
         * {String} sessionId: a session id to start the application
         *              with.  Normally, applications are started without
         *              a session id and create one.  However, sometimes
         *              it is desirable to use an existing session.
         *
         * Parameters:
         * options - {Object} Optional parameters
         *
         *           
         */
        initialize: function(options) {
            options = options || {};
            var sessionIdParam = this.getQueryParam('Session');
            // Override the pre-created sessionId by user specified sessionId(if exist)
            this.sessionId = sessionIdParam || options.sessionId || this.sessionId;
        
            if (options.applicationDefinitionURL) {
                this.applicationDefinitionURL = options.applicationDefinitionURL;            
            } else {
                var queryAppDef = this.getQueryParam('ApplicationDefinition');
                if (queryAppDef) {
                    this.applicationDefinitionURL = queryAppDef.split('+').join(' ');
                    this.appDefJson = null;   //wipe out any preloaded AppDef in a single file build
                } else {
                    this.applicationDefinitionURL = 'ApplicationDefinition.xml';
                }
            }
            if (Fusion._singleFile) {
                //override this method since OL is loaded in the fusion file
                OpenLayers._getScriptLocation = function() {
                    return Fusion.fusionURL + 'lib/OpenLayers/';
                };
            }
            this.initializeLocale();

            /*
             * if the application has been loaded from the same host as
             * fusion is installed in, then technically we don't need to
             * use the redirect script because we conform to the 
             * Same Origin Policy for XmlHttpRequest to work.
             */
            var test = window.location.protocol+'//'+window.location.host;
            var configUrl = 'config.json';
            //if (this.fusionURL.indexOf(test,0) == 0) {
            if ( ((this.fusionURL.indexOf("http://") < 0) || (this.fusionURL.indexOf(test,0) == 0)) && !(this.bForceRedirect)) {
                this.sRedirectScript = '';
            } else {
                this.sRedirectScript = 'redirect.php';
                configUrl += '&method=get';
            }
        
            if (Fusion.configuration) {
                //config.json loaded via single file build
                this.serverSet();
            } else {
                var options = {
                    onSuccess: OpenLayers.Function.bind(this.getConfigCB, this),
                    onFailure: OpenLayers.Function.bind(this.serverFailed, this),
                    method: 'get'
                };
                this.ajaxRequest(configUrl, options);
            }
        },
    
        /**
         * Function: initializeLocale
         *
         * Initialize the Strings hash with the requested locale.  This method
         * simply extends the OpenLayers strings hash with strings specific
         * to Fusion.  Add strings to the files in fusion/text/[locale].json.
         * This method can be called before <Fusion.initialize> if localized
         * strings are required in the template initialization.
         *
         * Parameters:
         * locale - {String} the locale value e.g. 'en-CA' or simply 'en' for English
         *
         */
        initializeLocale: function(locale) {
          OpenLayers.Lang.setCode(locale ? locale : window._FusionLocale);
          this.locale = OpenLayers.Lang.code;
      
          //check if strings are defined for specified locale, if not, set them to the default locale
          if (!OpenLayers.Lang[this.locale]) {
            OpenLayers.Lang[this.locale] = OpenLayers.Lang[OpenLayers.Lang.defaultCode];
          }
          if (!Fusion.Strings[this.locale]) {
            Fusion.Strings[this.locale] = Fusion.Strings[OpenLayers.Lang.defaultCode];
          }
          OpenLayers.Lang[this.locale] = OpenLayers.Util.extend(OpenLayers.Lang[this.locale],Fusion.Strings[this.locale]);
        },
    
        /**
         * Function: setLoadState
         *
         * set the current initialization state of the application.
         * Depending on the state, this will cause various scripts
         * to be loaded.
         *
         * Parameters:
         * state - {Integer} the new loading state
         */
        setLoadState: function(state) {
            //console.log('setLoadState: ' + state);
            this.loadState = state;
            switch(state) {
                case this.LOAD_CONFIG:
                    //console.log('load config');
                    this.loadConfig();
                    break;
                case this.LOAD_WIDGETS:
                    //console.log('load widgets');
                    this.loadQueuedScripts();
                    break;
                case this.LOAD_COMPLETE:
                    //console.log('load complete');
                    if (this.applicationDefinition) {
                      this.applicationDefinition.create();
                      this.triggerEvent(Fusion.Event.FUSION_INITIALIZED);
                    } else {
                      Fusion.reportError(new Fusion.Error(Fusion.Error.FATAL, 
                                          'failed to create AppDef object'));
                    }
                    break;
            }
        },
        
        /**
         * Function: loadQueuedScripts
         *
         * load any scripts that have been queued for loading.  As the
         * scripts load, they are removed.  When all queued scripts have
         * been loaded, the load state is advanced.  If any of the loaded
         * scripts require other scripts (for inheritance), they will be
         * queued and loaded before the load state is advanced.
         */
        loadQueuedScripts: function() {
            //with the compressed version, all scripts are already laoded so just 
            //increment the load state
            if (this.aScripts.length == 0) {
                this.setLoadState(this.LOAD_COMPLETE);
                return;
            }

            this.aLoadingScripts = [];
            //make a separate array of what is actually being loaded to keep track
            //of them (require adds to aScripts so we can't use that array
            //directly).
            for (var i=0; i<this.aScripts.length; i++) {
                this.aLoadingScripts[i] = this.aScripts[i];
            }
            this.aScripts = [];
            
            //Ted Yang fix: this is because script loaded will change aLoadingScripts. So we have to create a temp collection to store the loading script.
            //See function scriptLoaded
            //Now IE9beta, Safari, Firefox and Chrome all support onload event,
            //but the IE9beta will triggered the event synchronously, so the aLoadingScripts will be changed before the "for sentence" is finished,
            //this caused the needed scripts will not be loaded (nearly half of them) 
            var loadingScriptsTemp = [];
            for (var i = 0; i < this.aLoadingScripts.length; i++) {
                loadingScriptsTemp[i] = this.aLoadingScripts[i];
            }
            //add the script elements to the DOM to cause them to load.
            for (var i = 0; i < loadingScriptsTemp.length; i++) {
                document.getElementsByTagName('head')[0].appendChild(loadingScriptsTemp[i]);
            }

            //if IE or Safari
            //Safari has fixed this problem already, and IE9beta also seems to support the onload event, so this code is only for the old IEs.
            this.checkLoadInterval = window.setInterval(
                    OpenLayers.Function.bind(this.checkLoadingScripts, this), 500);
        },
    
        /**
         * Function: queueScript
         *
         * Insert a new script into the loading queue.  The URL should be relative
         * to the Fusion base url.  The script will not actually be loaded until
         * <Fusion.loadQueuedScripts> is called.
         *
         * Parameters: 
         * url - {String} The url of the script.
         */
        queueScript: function(url) {
            if(!document.getElementById(url) && !this.aScripts[url]) {
                var script = document.createElement('script');
                script.defer = false;
                script.type = "text/javascript";
                //TODO: test url to see if it needs to come from fusion
                script.id = url;
                script.src = this.getFusionURL() + url;
                script.onload = OpenLayers.Function.bind(this.scriptLoaded, this, url);
                script.onerror = OpenLayers.Function.bind(this.scriptFailed, this, script.src);
                this.aScripts[url] = script;
                this.aScripts.push(script);
            }
        },
        
        /**
         * Function: scriptFailed
         *
         * Called when a script fails to load for some reason.
         *
         * Parameters: 
         * url - {String} the url that failed to load
         *
         * TODO: the application probably won't work if a script fails to load
         * so we need to decide how to inform the user and fail gracefully.
         */
        scriptFailed: function(url) {
            Fusion.reportError(new Fusion.Error(Fusion.Error.FATAL, 
                              OpenLayers.i18n('scriptFailed',{'script':url})));
        },
    
        /**
         * Function: scriptLoaded
         *
         * a script has loaded.  It is removed from the various tracking
         * arrays.  When all requested scripts have been loaded, we check
         * to see if any scripts where required by the ones just loaded.
         * If yes, then we <Fusion.loadQueuedScripts> again, otherwise we advance
         * the load state.
         *
         * Parameters: 
         * url - {String} the url of the script that was loaded.
         */
        scriptLoaded: function(url) {
            for (var i=0; i<this.aLoadingScripts.length;i++) {
                if (this.aLoadingScripts[i].id == url) {
                    this.aLoadingScripts.splice(i,1);
                }
            }
            if (this.aLoadingScripts.length == 0) {
                window.clearInterval(this.checkLoadInterval);
                if (this.aScripts.length > 0) {
                    this.loadQueuedScripts();
                } else {
                    this.setLoadState(this.LOAD_COMPLETE);
                }
            }
        },
        
        /**
         * Function: checkLoadingScripts
         *
         * check if scripts have loaded.  In IE, scripts don't seem to fire the
         * onload event.  Safari also seems to have some problems.
         */
        checkLoadingScripts: function() {
            //In IE9beta, when one script is loaded and the this.aLoadingScripts.length is 0 (for example, there is only 1 item in the aLoadingScripts) , 
            //the onload event will be triggered synchronously,
            //but the "this.checkLoadInterval" is not created yet, so the clearInterval will failed to work in function scriptLoaded,
            //so I add this code to fix this problem
            //if IE9beta fix the problem, and we don't need to support the old IEs, maybe we can remove the checkLoadingScripts
            if (this.aLoadingScripts.length == 0) {
                window.clearInterval(this.checkLoadInterval);
                return;
            }

            var agt=navigator.userAgent.toLowerCase();
            for (var i=this.aLoadingScripts.length-1; i>=0; i--) {
                var s = this.aLoadingScripts[i];
                if (agt.indexOf('safari') != -1) {
                    var widgetName = s.id.substring(s.id.lastIndexOf('/')+1, s.id.indexOf('.js'));
                    var b;
                    eval ('b = typeof Fusion.Widget.'+widgetName+' == "function";');
                    if (b) {
                        this.scriptLoaded(s.id);
                    }
                } else if (s.readyState == 'loaded' ||
                    s.readyState == 'complete') {
                    this.scriptLoaded(s.id);
                }
            }
        },
        
        /**
         * Function: loadConfig
         *
         * asynchronously load the application definition.  For MapGuide, also
         * set up the broker object.  Once the AppDef is loaded/created advance
         * the load state.
         */
        loadConfig: function() {
            var mapAgentUrl = this.getConfigurationItem('mapguide', 'mapAgentUrl');
            if (mapAgentUrl) {
                this.oBroker = new Fusion.Lib.MGBroker();
                var url = mapAgentUrl;
                if (this.sRedirectScript) {
                    url = this.sRedirectScript + '?s=' + url;
                    this.oBroker.method = 'post';
                } else {
                    this.oBroker.method = 'get';
                }
                this.oBroker.setSiteURL(url, "Anonymous", "");
            }
            this.applicationDefinition = new Fusion.Lib.ApplicationDefinition(this.sessionId);
            if (Fusion.appDefJson) {
                Fusion.setLoadState(Fusion.LOAD_WIDGETS);
            }
        },
    
        /**
         * Function: getConfigCB
         *
         * the server has returned the application configuration file that
         * contains enough information to bootstrap the application.
         *
         * Parameters:
         * r - {XMLHttpRequest} an XMLHttpRequest object
         */
        getConfigCB: function(r) {
            if (r.responseText) {  
                eval("this.configuration="+r.responseText);
                this.serverSet();
            } else {
                //console.log('Error parsing configuration file, it is not valid somehow?');
                alert(OpenLayers.i18n('configParseError'));
            }
        },
    
        /**
         * Function: serverSet
         *
         * the server has returned the application configuration file that
         * contains enough information to bootstrap the application.
         *
         * Parameters:
         * r - {XMLHttpRequest} an XMLHttpRequest object
         */
        serverSet: function() {
            var s = this.configuration.mapguide.webTierUrl;
            /* if it is set, use it ... otherwise assume fusion is installed in
             * the default location and compute the web tier url from that
             */
            if (s) {
                var nLength = s.length;
                var slastChar =  s.charAt((nLength-1));
                if (slastChar != '/') {
                    s = s + "/";
                }
            } else {
                var idx = this.fusionURL.lastIndexOf('fusion');
                if (idx == -1) {
                  s = this.fusionURL + "../";   //loaded relatively from within fusion directory
                } else {
                  s = this.fusionURL.substring(0, idx);
                }
            }
            this.configuration.mapguide.webTierUrl = s;
            this.configuration.mapguide.mapAgentUrl = s + 'mapagent/mapagent.fcgi';
        
            //trigger loading stuff ...
            this.setLoadState(this.LOAD_CONFIG);
        },
    
        /**
         * Function: serverFailed
         *
         * the application failed to load the application configuration file.
         * Not much point in continuing, but we can inform the user why this
         * happened.
         *
         * Parameters: 
         * r - {XMLHttpRequest} the XMLHttpRequest object
         *
         * TODO: do something more useful in here?
         */
        serverFailed: function(r) {
            //console.log('error loading server configuration file');
            alert(OpenLayers.i18n('configLoadError')); 
        },
    
        /**
         * Function: ajaxRequest
         *
         * convenience function to issue an {XmlHttpRequest} to the server relative
         * to the Fusion installation.
         *
         * Parameters: 
         * scriptURL - {String} the URL (relative to Fusion) to request
         * options - {Object} optional parameters to send with the request, passed directly to the
         *            <OpenLayers Ajax.Request> function
         */
        ajaxRequest: function(scriptURL, options) {
            //console.log('options.parameters='+options.parameters);
            var r = this.getRedirectScript();
            if (r != '') {
                r = r + '?s=';
            }
            var url = r + this.getFusionURL() + scriptURL;
            if (!options.onException) {
                options.onException = OpenLayers.Function.bind(this.ajaxException, this);
            }
            if (!options.contentType) {
              options.contentType = 'application/x-www-form-urlencoded';
            }
            if (options.parameters && typeof options.parameters == 'string') {
              if (options.parameters.indexOf('?') < 0) {
                  options.parameters = '?' + options.parameters;
              }
            }
            var temp = new OpenLayers.Ajax.Request( url, options);
        },
    
        /**
         * Function: ajaxException
         *
         * trap exceptions produced in handling an ajax response.
         *
         * Parameters: 
         * r - {XmlHttpRequest}
         * e - {Exception}
         */
        ajaxException: function(r, e) {
            this.reportError(new Fusion.Error(Fusion.Error.WARNING, 
                OpenLayers.i18n('ajaxError', {'exception':e.message, 
                                              'filename':e.fileName, 
                                              'line':e.lineNumber,
                                              'response': r.transport.responseText
                                              })));
        },
    
         /**
         * Function: getXmlAsJson
         *
         * Convert XML to JSON using a server-side script
         * for requests that aren't available in JSON.
         *
         * Parameters: 
         * url - {String} the URL of the XML object to be converted;
         *       this can be any URL and is not prepended with the Fusion URL
         * callback - {Function} a callback function to be called if 
         *            the request is successful
         *
         * boolean indicator if the content is JSON or not.
        */
        getXmlAsJson: function(url, callback) {
            var options = {
                method: 'get',
                onSuccess: OpenLayers.Function.bind(this.xml2json, this, callback),
                onFailure: OpenLayers.Function.bind(this.ajaxException, this)
            };
            var temp = new OpenLayers.Ajax.Request(url, options);
        },
    
        /**
         * Function: parseJSON
         *
         * Parses the given JSON string to a javascript object. This is safer than using eval() as there is no possibility of
         * arbitrary code execution
         *
         * Parameters: 
         * str - {String} The JSON string to parse
         *
        */
        parseJSON: function(str) {
            var o;
            if (typeof(JSON) != 'undefined' && JSON.parse != null) {
                o = JSON.parse(str);
            } else {
                eval('o='+str); //TODO: Still evil for now, but the evil is localized to this one spot. Replace with a JSON.parse shim
            }
            return o;
        },
    
         /**
         * Function: xml2json
         *
         * Callback method to convert the XNR response into an actual json object
         * and triggers the call to the callback method
         *
         * Parameters: 
         * callback - {Function} callback method to be executed on success and will be passed a parsed json object
         * r - {XmlHttpRequest} the XmlHttpRequest object
         * json - {Boolean} boolean indicator if the content is JSON or not (set by OpenLayers)
         *
        */
        xml2json: function(callback, r, json) {
          if (json) {
            var o;
            eval("o="+r.responseText);
            callback(o);
          } else {
            if (r.status >= 400) {
                var msg = r.transport.responseText;
                if (msg == "") {
                    msg = OpenLayers.i18n('serverNotAvailable');
                }
                Fusion.reportError(new Fusion.Error(Fusion.Error.FATAL,
                  OpenLayers.i18n('invalidXMLDocument', {'msg':msg, 'url':r.request.url})));
                return;
            }

            var options = {
              onSuccess: callback,
              method: 'post',
              parameters: {'xml': encodeURIComponent(r.responseText)}
            };
            var sl = Fusion.getScriptLanguage();
            Fusion.ajaxRequest('common/'+sl+'/Xml2JSON.'+sl, options);
          }
        },
    
        /**
         * Function: getMapByName
         *
         * return a map widget with the given name
         *
         * Parameters: 
         * name - {String} The map name to return
         *
         * Return: 
         * {<Fusion.Widget.Map>} a map object or null if not found.
         */
        getMapByName: function(name) {
            var map = null;
            if (this.applicationDefinition) {
                map = this.applicationDefinition.getMapByName(name);
            }
            return map;
        },
    
        /**
         * Function: getMapById
         *
         * return a map widget that is associated with the given DOM element
         * by id.
         *
         * Parameters: 
         * id - {String} The map id to return
         *
         * Return: 
         * {<Fusion.Widget.Map>} a map object or null if not found.
         */
        getMapById: function(id) {
            var map = null;
            if (this.applicationDefinition) {
                map = this.applicationDefinition.getMapById(id);
            }
            return map;
        },
    
        /**
         * Function: getMapByIndice
         *
         * return the map widget at the given index if there are more than one
         * map elements specified in the AppDef
         *
         * Parameters: 
         * indice - {String} The map indice to return
         *
         * Return: 
         * {<Fusion.Widget.Map>} a map object or null if not found.
         */
        getMapByIndice: function(indice) {
            var map = null;
            if (this.applicationDefinition) {
                map = this.applicationDefinition.getMapByIndice(indice);
            }
            return map;
        },
    
        /**
         * Function: getWidgetById
         *
         * return a reference to a widget by the id of the DOM
         * Element that it was created in.
         *
         * Parameters: 
         * id - {String} the DOM id to get the widget for
         *
         * Return: 
         * {<Fusion.Widget>} the widget or null
         */
        getWidgetById: function(id) {
            /*
            var widget = null;
            if (this.applicationDefinition) {
                widget = this.applicationDefinition.getWidgetById(id);
            }
            return widget;
            */
            var d = $(id);
            if (d && d.widget) {
                return d.widget;
            } else {
                return null;
            }
        },
    
        /**
         * Function getWidgetsByType
         *
         * returns an array of widgets by type.
         *
         * Parameters: 
         * type - {String} the type of widget to get references to
         *
         * Return: 
         * {Array} an array of widgets, which may be empty
         */
        getWidgetsByType: function(type) {
            var widgets = [];
            if (this.applicationDefinition) {
                widgets = this.applicationDefinition.getWidgetsByType(type);
            }
            return widgets;
        },
    
        /**
         * Function getSearchDefinitions
         *
         * returns the SearchDefinition objects as defined in the AppDef.
         *
         * Return: 
         * {Array} an array of searchDefinitions, which may be empty
         */
        getSearchDefinitions: function() {
            if (this.applicationDefinition) {
                return this.applicationDefinition.searchDefinitions;
            } else {
                return {};
            }
        },
    
        /**
         * Function getSearchCategories
         *
         * returns the SearchCategory objects as defined in the AppDef.
         *
         * Return:
         * {Array} an array of searchCategories, which may be empty
         */
        getSearchCategories: function() {
            if (this.applicationDefinition) {
                return this.applicationDefinition.searchCategories;
            } else {
                return {};
            }
        },
    
        /**
         * Function getApplicationDefinitionURL
         *
         * returns the URL of the Application Definition
         *
         * Return: 
         * {String} a URL
         */
        getApplicationDefinitionURL: function() { return this.applicationDefinitionURL; },

        /**
         * Function: getApplicationURL
         *
         * returns the applicaiton's absolute URL minus the filename.html part
         *
         * Return: 
         * {String} a URL
         */
        getApplicationURL: function() { 
          var path = window.location.pathname;
          var idx = path.lastIndexOf('/');
          if (idx>=0) {
            path = path.substr(0,idx+1);
          }
          return window.location.protocol + "//" + window.location.host + path;
        },
    
        /**
         * Function: getFusionURL
         *
         * returns the application's absolute URL minus the filename.html part
         *
         * Return: 
         * {String} a URL
         */
        getFusionURL: function() {return this.fusionURL;},
    
        /**
         * Function: getConfigurationItem
         *
         * returns individual elements out of the Fusion config.json file
         *
         * Parameters: 
         * arch - {String} the server architecture (mapguide or mapserver)
         * key - {String} the config item to be returned
         *
         * Return: 
         * {String} a the value for the key of null if not found
         */
        getConfigurationItem: function(arch, key) { 
            if (this.configuration[arch] && (this.configuration[arch][key]!='undefined')) { 
                return this.configuration[arch][key]; 
            } 
            return null; 
        },
    
        /**
         * Function: getScriptLanguage
         *
         * Accessor to return the server-side script language
         *
         * Return: 
         * {String} the script language e.g. 'php'
         */
        getScriptLanguage: function() { return this.configuration.general.scriptLanguage; },
    
        /**
         * Function: getRedirectScript
         *
         * Accessor to return the URL of the redirect script
         *
         * Return: 
         * {String} a URL
         */
        getRedirectScript: function() { return this.sRedirectScript; },
    
        /**
         * Function: getBroker
         *
         * Accessor to return the broker object (for MapGuide only).  The broker
         * is used to prepare various queries to the MapGuide server.
         *
         * Return: 
         * {<Fusion.Lib.MGBroker>} the broker object
         */
        getBroker: function() { return this.oBroker; },
    
        /**
         * Function: require
         *
         * Marks a JavaScript file as required which puts it into the queue for
         * loading during Fusion initialization.  Widget code is included
         * through this mecahnism
         *
         * Parameters: 
         * url - {String} the URL of the JS file to be loaded
         *
         */
        require: function(url) { this.queueScript(url); },
    
        /**
         * Function: reportError
         *
         * Triggers the <Fusion.Event.FUSION_ERROR> and passes along the error
         * object to the callback functions registered for this event.  Widgets
         * call MFusion.reportError> to inform the system of errors.
         * Applications will typically register an event listener for the error
         * event and do something to report the error to the user.  
         * By default, errors are not reported since there is no listener
         *
         * Parameters:
         * o - {Object} the error object which is typically a string
         */
        reportError: function(o) { this.triggerEvent(Fusion.Event.FUSION_ERROR, o); },
    
        /**
         * Function: unitFromName
         *
         * returns index into the units array for the given unit name or 
         * abbreviation
         *
         * Parameters: 
         * unit - {String} the units name to look up
         *
         * Return: 
         * {Integer} index into the units array
         */
        unitFromName: function(unit) {
            switch(unit.toLowerCase()) {
                case 'unknown':
                    return Fusion.UNKNOWN;
                case 'inches':
                case 'inch':
                case 'in':
                    return Fusion.INCHES;
                case 'feet':
                case 'ft':
                    return Fusion.FEET;
                case 'yards':
                case 'yard':
                case 'yd':
                    return Fusion.YARDS;
                case 'miles':
                case 'mile':
                case 'mi':
                    return Fusion.MILES;
                case 'nautical miles':
                case 'nautical mile':
                case 'nm':
                    return Fusion.NAUTICALMILES;
                case 'millimeters':
                case 'millimeter':
                case 'mm':
                    return Fusion.MILLIMETERS;
                case 'centimeters':
                case 'centimeter':
                case 'cm':
                    return Fusion.CENTIMETERS;
                case 'meters':
                case 'meter':
                case 'm':
                    return Fusion.METERS;
                case 'kilometers':
                case 'kilometer':
                case 'km':
                    return Fusion.KILOMETERS;
                case 'degrees':
                case 'degree':
                case 'deg':
                    return Fusion.DEGREES;
                case 'decimal degrees':
                case 'dd':
                    return Fusion.DECIMALDEGREES;
                case 'degrees minutes seconds':
                case 'dms':
                    return Fusion.DMS;
                case 'pixels':
                case 'pixel':
                case 'px':
                    return Fusion.PIXELS;
                default:
                    return Fusion.UNKNOWN;
            }
        },
        
        /**
         * Function: unitSystem
         *
         * Given a unit, this method returns if the units system is one of:
         * imperial, metric, degrees or device units
         *
         * Parameters: 
         * unit - {Integer} the units array index
         *
         * Return: 
         * {String} the units system
         */
        unitSystem: function(unit) {
            switch(unit) {

                case Fusion.INCHES:
                case Fusion.FEET:
                case Fusion.YARDS:
                case Fusion.MILES:
                case Fusion.NAUTICALMILES:
                    return 'imperial';
                case Fusion.MILLIMETERS:
                case Fusion.CENTIMETERS:
                case Fusion.METERS:
                case Fusion.KILOMETERS:
                    return 'metric';
                case Fusion.DEGREES:
                case Fusion.DECIMALDEGREES:
                case Fusion.DMS:
                    return 'deg';
                case Fusion.UNKNOWN:
                case Fusion.PIXELS:
                default:
                    return 'device';
            }
        },
    
        /**
         * Function: unitName
         *
         * Given a unit, this method returns the units name
         *
         * Parameters: 
         * unit - {Integer} the units array index
         *
         * Return: 
         * {String} the units name
         */
        unitName: function(unit) {
            if (unit >= Fusion.UNKNOWN && unit <= Fusion.PIXELS) {
              return (Fusion.aUnitNames[unit]);
            }
            return 'Unknown';
        },
        
        /**
         * Function: unitAbbr
         *
         * Given a unit, this method returns the units abbreviation
         *
         * Parameters:
         * unit - {Integer} the units array index
         *
         * Return: 
         * {String} the units abbreviation
         */
        unitAbbr: function(unit) {
            if (unit >= Fusion.UNKNOWN && unit <= Fusion.PIXELS) {
              return (Fusion.aUnitAbbr[unit]);
            }
            return 'Unk';
        },

        /**
         * Function: toMeter
         *
         * Converts a length value from native units into meters.  This is the
         * identity transform if the input units are meters
         *
         * Parameters:
         * unit - {Integer} the units array index
         * value - {Float} the value to be converted
         *
         * Return: 
         * {Float} the value in meters
         */
        toMeter: function(unit, value) {
            if (unit == Fusion.UNKNOWN) {
                return value;
            }
            if (unit > Fusion.UNKNOWN && unit < Fusion.PIXELS) {
              return (Fusion.aMeterPerUnit[unit] * value);
            }
            return false;
        },

        /**
         * Function: fromMeter
         *
         * Converts a length value from meters into native units.  This is the
         * identity transform if the native units are meters
         *
         * Parameters:
         * unit - {Integer} the units array index
         * value - {Float} the value to be converted
         *
         * Return: 
         * {Float} the value in native units
         */
        fromMeter: function(unit, value) {
            if (unit == Fusion.UNKNOWN) {
                return value;
            }
            if (unit > Fusion.UNKNOWN && unit < Fusion.PIXELS) {
                return (Fusion.aUnitPerMeter[unit] * value);
            }
            return false;
        },

        /**
         * Function: convert
         *
         * Converts a length value from one unit system into another.
         *
         * Parameters:
         * unitsIn - {Integer} the units array index of the input
         * unitsOut - {Integer} the units array index of the output
         * value - {Float} the value to be converted
         *
         * Return: 
         * {Float} the value in output units
         */
        convert: function(unitsIn, unitsOut, value) {
            if (unitsIn >= Fusion.UNKNOWN && unitsIn < Fusion.PIXELS && 
                unitsOut >= Fusion.UNKNOWN && unitsOut < Fusion.PIXELS) {
                return Fusion.fromMeter(unitsOut, Fusion.toMeter(unitsIn, value));
            }
            return false;
        },
    
      /**
         * Function: initUnits
         *
         * initializes the meters per unit values when a new map is loaded.  
         * Some systems make different assumptions for the conversion of degrees
         * to meters so this makes sure both Fusion and OpenLayers are using 
         * the same value.
         *
         * Parameters: 
         * metersPerUnit - {Float} the value returned by LoadMap.php
         *                 for meters per unit
         */
        initUnits: function(metersPerUnit) {
            var eps = 1000;
            if (Math.abs(metersPerUnit-Fusion.aMeterPerUnit[Fusion.DEGREES]) < eps){
                Fusion.aMeterPerUnit[Fusion.DEGREES] = metersPerUnit;
                Fusion.aMeterPerUnit[Fusion.DECIMALDEGREES] = metersPerUnit;
                Fusion.aMeterPerUnit[Fusion.DMX] = metersPerUnit;
                var inverse = 1.0/metersPerUnit;
                Fusion.aUnitPerMeter[Fusion.DEGREES] = inverse;
                Fusion.aUnitPerMeter[Fusion.DECIMALDEGREES] = inverse;
                Fusion.aUnitPerMeter[Fusion.DMX] = inverse;
            
                var inPerUnit = OpenLayers.INCHES_PER_UNIT.m * metersPerUnit;
                OpenLayers.INCHES_PER_UNIT["dd"] = inPerUnit;
                OpenLayers.INCHES_PER_UNIT["degrees"] = inPerUnit;
                OpenLayers.INCHES_PER_UNIT["Degree"] = inPerUnit;
            }
        },
    
        /**
        * Function: getClosestUnits
        *
        * find the OpenLayers units identifier given the Fusion metersPerUnit value
        *
        * Parameters:
        * metersPerUnit - {Float} the value returned by LoadMap.php for meters per unit
        */
        getClosestUnits: function(metersPerUnit) {
            var units = "degrees";
            var minDiff = 100000000;
            for (var key in OpenLayers.INCHES_PER_UNIT) {
                var newDiff = Math.abs((metersPerUnit * 39.3701) - OpenLayers.INCHES_PER_UNIT[key]);
                if(newDiff < minDiff)
                {
                    minDiff = newDiff;
                    units = key;
                }
            }
            return units;
        },

        /**
         * Function: addWidgetStyleSheet
         *
         * Dynamically load a CSS stylesheet.  The url will be prepended with
         * the Fusion URL.
         *
         * Parameters: 
         * url - {String} the URL of the CSS file to be loaded
         */
        addWidgetStyleSheet: function(url) {
            var lnk = document.createElement('link');
            var hd = document.getElementsByTagName('HEAD')[0];
            hd.insertBefore(lnk, Fusion._scriptObject);
            lnk.type = 'text/css';
            lnk.rel='stylesheet';
            lnk.href = Fusion.getFusionURL()+url;
        },

        /**
         * Function: parseQueryString
         *
         * An initialization time function to parse the application URL parameters
         * and stores them in an array.  They can be retrieved using 
         * {<Fusion.getQueryParam>}
         *
         * Return: 
         * {Array} an array of the query params from when the page was loaded
         */
        parseQueryString: function() {
            this.queryParams = [];
            var s=window.location.search;
            if (s!='') {
                s=s.substring( 1 );
                var p=s.split('&');
                for (var i=0;i<p.length;i++) {
                    var q=p[i].split('=');
                    this.queryParams[q[0].toLowerCase()]=decodeURIComponent(q[1]);
                }
            }
            return this.queryParams;
        },

        /**
         * Function: getQueryParam
         *
         * Returns the query parameter value for a given parameter name
         *
         * Parameters: 
         * p - {String} the parameter to lookup
         *
         * Return: 
         * parameter value or the empty string '' if not found
         */
        getQueryParam: function(p) {
            if (!this.queryParams) {
                this.parseQueryString();
            }
            p = p.toLowerCase();
            if (this.queryParams[p] && typeof this.queryParams[p] == 'string') {
                return this.queryParams[p];
            } else {
                return '';
            }
        },
    
        /**
         * Property: _scriptName
         * {String} Relative path of this script.
         */
        _scriptName: "lib/fusion",

        /**
         * Function: _getScriptLocation
         * Return the path to this script for bootstrapping Fusion.
         *
         * Return:
         * {String} Path to this script
         */
        _getScriptLocation: function () {
            Fusion.fusionURL = null;
            var scriptName = Fusion._scriptName;

            var scripts = document.getElementsByTagName('script');
            for (var i = 0; i < scripts.length; i++) {
                var src = scripts[i].getAttribute('src');
                if (src) {
                    var index = src.lastIndexOf(scriptName); 
                    if (index > -1) {
                        Fusion._scriptObject = scripts[i];
                        var img = document.createElement('img');
                        img.src = src.slice(0, index)+'lib/a_pixel.png';
                        var s = img.src;
                        var n = s.lastIndexOf('lib/a_pixel.png');
                        Fusion.fusionURL = s.slice(0, n);
                        Fusion.aPixel = img;
                        break;
                    }
                }
            }
            if (!this.fusionURL) {
                alert('failed to determine fusionURL using script name '+Fusion._scriptName+'.  Initialization aborted');
                return;
            }
            return Fusion.fusionURL;
        }
    };

    /*********************************************************************************/
    /* actual bootstrap execution code follows */
    
    Fusion._singleFile = singleFile;
    var host = Fusion._getScriptLocation();
    Jx.aPixel = Fusion.aPixel;

    var getXmlHttpRequest = function(){
        try { return new XMLHttpRequest(); }
        catch (e) { try { return new ActiveXObject('Msxml2.XMLHTTP'); } 
        catch (e) { try { return new ActiveXObject('Microsoft.XMLHTTP'); }
        catch (e) { return  null; }}}
    }
    
    //determine the language to use and add resource bundles to be loaded to the core scripts
    var locale = navigator.language ?
                  navigator.language.substring(0,2):    //e.g. en-CA becomes just en                  
                  navigator.userLanguage.substring(0,2);//only use the prefix part for now, 
    var s = window.location.search;
    var idx = s.indexOf('locale=');
    if (idx>0) {
      var temp = s.substring(idx+7);
      var idx2 = temp.indexOf('&');
      locale = temp.substring(0,idx2<0?temp.length:idx2);
    }
    else if(navigator.userAgent.indexOf("MSIE")>0) {
        //This is because IE will always returns OS language instead of browser language. So we need the server side's help.
        var xhr = getXmlHttpRequest();
        xhr.open("GET", Fusion.fusionURL+"common/php/GetLocale.php",false);
        xhr.send(null);
        if(xhr.status == 200) {
            var returnLocale = xhr.responseText;
            if(returnLocale.length >= 2) {
                locale = returnLocale.substring(0, 2);
            }
        }      
    }
    window._FusionLocale = locale;
    
    if (!Fusion._singleFile) {
        var coreScripts = ['lib/Proj4js/proj4js-compressed.js',
                            'lib/OpenLayers/OpenLayers.js',
                            'lib/OpenLayers/OpenLayersGMapsPatch.js', /* Remove when we base off of OL 2.12 */
                            'lib/OLpatch.js',
                            'lib/jxLib/jxlib.uncompressed.js',
                            'lib/IE9patch.js',
                            'lib/EventMgr.js',
                            'lib/Error.js',
                            'lib/ApplicationDefinition.js',
                            'lib/MGBroker.js',
                            'lib/Widget.js',
                            'lib/Map.js',
                            'lib/MapMessage.js',
                            'layers/Layers.js',
                            'lib/Search.js',
                            'text/en.json'];
        if (locale != 'en') {
            coreScripts.push('lib/OpenLayers/Lang/'+locale+'.js');
            coreScripts.push('text/'+locale+'.json');
        }
        
        var allScriptTags = new Array(coreScripts.length);
        for (var i = 0; i < coreScripts.length; i++) {
            allScriptTags[i] = "<script src='" + host + coreScripts[i] +
                                   "'></script>"; 
        }
        document.write(allScriptTags.join(""));
    }
    
    /*********************************************************************************/
    /* Load commercial layer API scripts */
    
    /*This function is extracted from Fusion to get the MapAgentUrl */ 
    var getAgentUrl = function() { 
        /* 
        * if the application has been loaded from the same host as 
        * fusion is installed in, then technically we don't need to 
        * use the redirect script because we conform to the  
        * Same Origin Policy for XmlHttpRequest to work. 
        */ 
        var fusionURL = Fusion._getScriptLocation(); 
        var testUrl = window.location.protocol + '//' + window.location.host; 
        var configUrl = 'config.json'; 
    
        if (((Fusion.fusionURL.indexOf("http://") < 0) || (Fusion.fusionURL.indexOf(testUrl, 0) == 0)) && !(Fusion.bForceRedirect)) { 
            Fusion.sRedirectScript = ''; 
        } 
        else { 
            Fusion.sRedirectScript = 'redirect.php'; 
            configUrl += '&method=get'; 
        } 
    
        var r = Fusion.getRedirectScript(); 
        if (r != '') { 
            r = r + '?s='; 
        } 
    
        var fusionURL = r + Fusion.getFusionURL() + configUrl; 
    
        var xhr = getXmlHttpRequest(); 
        xhr.open("GET", fusionURL, false); 
        xhr.send(null); 
    
        eval("Fusion.configuration=" + xhr.responseText); 
        var s = Fusion.configuration.mapguide.webTierUrl; 
        /* if it is set, use it ... otherwise assume fusion is installed in 
        * the default location and compute the web tier url from that 
        */ 
        if (s) { 
            var nLength = s.length; 
            var slastChar = s.charAt((nLength - 1)); 
            if (slastChar != '/') { 
                s = s + "/"; 
            } 
        } 
        else { 
            var idx = Fusion.fusionURL.lastIndexOf('fusion'); 
            if (idx == -1) { 
                s = Fusion.fusionURL + "../";   //loaded relatively from within fusion directory 
            } 
            else { 
                s = Fusion.fusionURL.substring(0, idx); 
            } 
        } 
        Fusion.configuration.mapguide.mapAgentUrl = s + 'mapagent/mapagent.fcgi'; 
        return Fusion.configuration.mapguide.mapAgentUrl; 
    }
    
    var addElement = function(element) { 
        if (!element) { 
            return; 
        } 
        var src = element.textContent; 
    
        // For IE Browser 
        if (!src) { 
            src = element.text; 
        } 
    
        document.writeln('<script src="' + src + '"><\/script>'); 
    }

    /**
     * Function: createSessionId
     *
     * Pre-create a session to avoid the 401 Unauthorized dialog
     *
     */
    var createSessionId = function(user, pass){
        var xhr = getXmlHttpRequest();
        var url = Fusion.fusionURL+"layers/MapGuide/php/CreateSession.php";
        if (user != null && user != "")
        {
            url += "?username=" + user;
            if (pass != null && pass != "")
                url += "&password=" + pass;
        }
        xhr.open("GET", url, false);
        xhr.send(null); 
        var o;
        eval('o='+xhr.responseText);
        Fusion.sessionId = o.sessionId;
    }

    var appDefUrl = Fusion.getQueryParam('ApplicationDefinition'); 
    if(appDefUrl && (
        appDefUrl.indexOf('Library') == 0 ||
        appDefUrl.indexOf('Session') == 0)){
        var passedSessionId = Fusion.getQueryParam('Session'); 
        if(passedSessionId == null || passedSessionId == "" ){
            createSessionId(Fusion.getQueryParam("Username"), Fusion.getQueryParam("Password"));
        }else{
            Fusion.sessionId = passedSessionId;
        }
        var xhr = new getXmlHttpRequest(); 
        var mapAgentUrl = getAgentUrl(); 
        xhr.open("GET", mapAgentUrl + "?OPERATION=GETRESOURCECONTENT&VERSION=1.0.0&LOCALE=en&CLIENTAGENT=MapGuide+Developer&RESOURCEID=" + appDefUrl + "&FORMAT=text%2Fxml&SESSION="+ Fusion.sessionId, false); 
        xhr.send(null); 
        var appDefXML = xhr.responseXML.documentElement; 

        if(appDefXML){
            var googleElement = appDefXML.getElementsByTagName("GoogleScript")[0]; 
            var yahooElement = appDefXML.getElementsByTagName("YahooScript")[0]; 
            var veElement = appDefXML.getElementsByTagName("VirtualEarthScript")[0];
            var osmElement = appDefXML.getElementsByTagName("OpenStreetMapScript")[0];

            addElement(googleElement); 
            addElement(yahooElement); 
            addElement(veElement);
            addElement(osmElement);
        }
    }
    
})();

