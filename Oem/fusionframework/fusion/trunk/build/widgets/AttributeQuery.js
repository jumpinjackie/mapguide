/********************************************************************** * 
 * @project Fusion
 * @revision $Id: AttributeQuery.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose Legend widget
 * @author pspencer@dmsolutions.ca
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
 * in all copies or substantial portions of the Software. ********************************************************************
* **********************************************************************/


Fusion.Event.SELECTION_STARTED = Fusion.Event.lastEventId++;
Fusion.Event.SELECTION_COMPLETE = Fusion.Event.lastEventId++;

Fusion.Widget.AttributeQuery = Class.create();
Fusion.Widget.AttributeQuery.prototype = {
    spatialFilter: null,
    filters: null,
    layerName: null,
    initialize: function(widgetTag) {
        //console.log('AttributeQuery.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);

        var json = widgetTag.extension;
        
        this.layerName = json.LayerName ? json.LayerName[0] : '';
        
        //result limiter defaults to 0 which returns all results.
        this.resultLimit = json.ResultLimit ? json.ResultLimit[0] : 0;

        this.maxDimension = json.MaximumZoomDimension ? json.MaximumZoomDimension[0] : -1;
        this.zoomFactor = json.ZoomFactor ? json.ZoomFactor[0] : 2;

        this.searchCategory = json.SearchCategory[0];

        var oSpatialFilter = json.SpatialFilter ? json.SpatialFilter[0] : '';
        if (oSpatialFilter) {
            this.spatialFilter = new Fusion.Widget.AttributeQuery.SpatialFilter(oSpatialFilter);
        }
        
        this.errorId = json.ErrorId ? json.ErrorId[0] : '';
        
        this.filters = [];
        if (json.Filter instanceof Array) {
            for (var i=0; i<json.Filter.length; i++) {
                this.filters.push(new Fusion.Widget.AttributeQuery.Filter(json.Filter[i]));
            }
        }
        this.fpQueryComplete = this.queryComplete.bind(this);
        
        this.fpMapSelectionChanged = this.mapSelectionChanged.bind(this);
        this.getMap().registerForEvent(Fusion.Event.MAP_SELECTION_ON, this.fpMapSelectionChanged);
        
        Event.observe(this.domObj, 'click', this.submitQuery.bind(this));
        
        this.registerEventID(Fusion.Event.SELECTION_STARTED);
        this.registerEventID(Fusion.Event.SELECTION_COMPLETE);
    },
    
    submitQuery: function() {
        var sd = Fusion.getSearchDefinitions();
        if (sd[this.searchCategory]) {
            var searchParams = {};
            for (var i=0; i<this.filters.length; i++) {
                var filter = this.filters[i];
                var text = filter.getFilterText();
                if (text != '') {
                    if (filter.validate()) {
                        searchParams[filter.parameter] = text;
                    } else {
                        return;
                    }
                }
            }
            
            var defn = sd[this.searchCategory];
            this.currentSearchDefinition = defn;
            var layer = '&layer=' + defn.category.layer;
            var filter = defn.getFilterUrl(searchParams);
            var limit = '&limit='+this.resultLimit;
            //TODO create a "meta" filter type to handle overall constraints
            if (filter == '&filter=()') {
                if ($(this.errorId)) {
                    $(this.errorId).style.display = 'block';
                }
                return;
            }
            var join = defn.getJoinUrl();
            
            var s = this.getMap().arch + '/' + Fusion.getScriptLanguage() + "/AttributeQuery." + Fusion.getScriptLanguage() ;
            var params = {};
            params.parameters = 'session='+this.getMap().getSessionID()+'&mapname='+ this.getMap().getMapName()+layer+filter+join+limit; 
            params.onComplete = this.fpQueryComplete;
            Fusion.ajaxRequest(s, params);
            this.triggerEvent(Fusion.Event.SELECTION_STARTED);
            
        }
    },
    
    queryComplete: function(r) {
        var result = '';
        try {
            eval ('result='+r.responseText);
        } catch (error){
            result = {};
        }
        this.lastResult = result;
        this.triggerEvent(Fusion.Event.SELECTION_COMPLETE);
    },
    
    getProperties: function() {
        var properties = null;
        if (this.lastResult && this.lastResult.properties) {
            properties = this.lastResult.properties;
        }
        return properties;
    },
    getPropertyKeys: function() {
        var propertyKeys = null;
        if (this.lastResult && this.lastResult.property_keys) {
            propertyKeys = this.lastResult.property_keys;
        }
        return propertyKeys;
    },
    getNumberOfProperties: function() {
        var n = 0;
        if (this.lastResult && this.lastResult.properties) {
            n = this.lastResult.properties.length;
        }
        return n;
    },
    getProperty: function(n) {
        var property = '';
        if (this.lastResult && this.lastResult.properties) {
            property = this.lastResult.properties[n];
        }
        return property;
    },
    getNumberOfResults: function() {
        result = 0;
        if (this.lastResult && this.lastResult.values) {
            result = this.lastResult.values.length;
        }
        return result;
    },
    /*this is the number of results found assuming no limit was imposed*/
    getTotalNumberOfResults: function() {
        result = 0;
        if (this.lastResult && this.lastResult.total_count) {
            result = this.lastResult.total_count;
        }
        return result;
    },
    getResult: function(idx) {
        return this.lastResult.values[idx];
    },
    
    resultHasGeometry: function(idx) {
        return this.lastResult.geometries[idx];
    },
    
    resultJoinValue: function(idx) {
        return this.lastResult.join_values[idx];
    },
    
    zoomToResult: function(condition) {
        //console.log('zoomTo ' + filter);
        var filter = '&filter='+condition;
        var sd = Fusion.getSearchDefinitions();
        var defn = sd[this.searchCategory];
        var joinLayer = defn.join.layer;
        var layerName = this.getMap().layerRoot.findLayerByAttribute('resourceId', joinLayer).layerName;
        var layer = '&layers=' + layerName;
        
        var s = this.getMap().arch + '/' + Fusion.getScriptLanguage() + "/Query." + Fusion.getScriptLanguage() ;
        var params = {};
        params.onComplete = this.selectComplete.bind(this);
        params.method = 'post';
        params.postBody = 'session='+this.getMap().getSessionID()+'&mapname='+ this.getMap().getMapName()+
                         layer+filter;
        Fusion.ajaxRequest(s, params);
    },
    selectComplete: function(r) {
        var node = new DomNode(r.responseXML);
        var success = node.getNodeText('Selection');
        if (success == 'true') {
            this.getMap().deregisterForEvent(Fusion.Event.MAP_SELECTION_ON, this.fpMapSelectionChanged);
            this.getMap().newSelection();
            this.getMap().getSelection(this.zoomToSelection.bind(this));
            this.getMap().registerForEvent(Fusion.Event.MAP_SELECTION_ON, this.fpMapSelectionChanged);
        } else {
            this.getMap().clearSelection();
        }    
    },
    /**
     * set the extents of the map based on the pixel coordinates
     * passed
     * 
     * @param selection the active selection, or null if there is none
     */
    zoomToSelection : function(selection) {
        var ll = selection.getLowerLeftCoord();
        var ur = selection.getUpperRightCoord();
        var zoom_size = Math.min( this.maxDimension, this.zoomFactor * Math.max( Math.abs(ur.x - ll.x), Math.abs(ur.y - ll.y))) / 2;
        var cX = (ur.x + ll.x)/2;
        var cY = (ur.y + ll.y)/2;
        ll.x = cX - zoom_size;
        ur.x = cX + zoom_size;
        ll.y = cY - zoom_size;
        ur.y = cY + zoom_size;
        this.getMap().setExtents(new OpenLayers.Bounds(ll.x,ll.y,ur.x,ur.y));
    },
    mapSelectionChanged: function() {
        // console.log('map selection changed');
        this.getMap().getSelection(this.fetchMapSelection.bind(this));
    },
    fetchMapSelection: function(oSelection) {
        var sd = Fusion.getSearchDefinitions();
        var defn = sd[this.searchCategory];
        if (!defn) { return; }
        var map = this.getMap();
        var oLayer = map.layerRoot.findLayerByAttribute('resourceId', defn.join.layer);
        var layer = null;
        if (oLayer) {
            layer = oSelection.getLayerByName(oLayer.layerName);
        }
        if (layer) {
            
            var a = [];
            var n = layer.getNumElements();
            
            var p = layer.getPropertyNames();
            var pIdx = p.indexOf(defn.join.foreignKey);
            for (var i=0; i<n; i++) {
                var s = layer.getElementValue(i, pIdx);
                a.push('('+defn.join.primaryKey+' = ' + s + ')');
            }
            if (a.length > 0) {
                var layer = '&layer=' + defn.category.layer;
                var filter = '&filter='+(a.join(' OR '));
                var limit = '&limit='+this.resultLimit;
                var join = defn.getJoinUrl();

                var s = this.getMap().arch + '/' + Fusion.getScriptLanguage() + "/AttributeQuery." + Fusion.getScriptLanguage() ;
                var params = {};
                params.onComplete = this.fpQueryComplete;
                params.method = 'post';
                params.postBody = 'session='+this.getMap().getSessionID()+'&mapname='+ this.getMap().getMapName()+layer+filter+join+limit;
                Fusion.ajaxRequest(s, params);
                this.triggerEvent(Fusion.Event.SELECTION_STARTED);
            }
        }
    }
};

Fusion.Widget.AttributeQuery.FilterBase = Class.create();
Fusion.Widget.AttributeQuery.FilterBase.prototype = {
    valueId: null,
    valueInput: null,
    parameter: null,
    validators: null,
    initialize: function(json) {
        this.valueId = json.ValueId ? json.ValueId[0] : '';
        this.valueInput = $(this.valueId);
        this.parameter = json.Parameter ? json.Parameter[0] : '';
        
        this.validators = [];
        if (json.Validator) {
            for (var i=0; i<json.Validator.length; i++) {
                this.validators.push(new Fusion.Widget.AttributeQuery.Validator(json.Validator[i]));
            }
        }
    },
    getValue: function( idOrObj, defaultValue ) {
        var result = defaultValue;
        var obj = $(idOrObj);
        
        if (obj) {
            if (obj.tagName == 'INPUT') {
                result = obj.value;
            } else if (obj.tagName == 'SELECT') {
                result = obj.options[obj.selectedIndex].value;
            }
        }
        return result;
    },
    setValue: function( idOrObj, value) {
        var obj = $(idOrObj);
        if (obj) {
            if (obj.tagName == 'INPUT') {
                obj.value = value;
            } else if (obj.tagName = 'SELECT') {
                for (var i=0; i<obj.options.length; i++) {
                    if (obj.options[i].value == value) {
                        obj.options[i].selected = true;
                    }
                }
            }
        }
    },
    validate: function() {
        for (var i=0; i<this.validators.length; i++) {
            if (!(this.validators[i].validate(this))) {
                return false;
            }
        }
        return true;
    }
};

Fusion.Widget.AttributeQuery.Validator = Class.create();
Fusion.Widget.AttributeQuery.Validator.prototype = {
    domNode: null,
    message: 'validation failed',
    type: '',
    initialize: function(json) {
        this.message = json.Message ? json.Message[0] : this.message;
        this.type = json.Type ? json.Type[0] : this.type;
        this.className = json.ClassName ? json.ClassName[0] : '';
        this.messageId = json.MessageId ? json.MessageId[0] : '';
        this.max = json.Max ? json.Max[0] : '';
        this.min = json.Min ? json.Min[0] : '';
        this.regex = json.Regex ? new RegExp(json.Regex[0]) : null;
        
    },
    validate: function(filter) {
        if ($(this.messageId)) {
            $(this.messageId).innerHTML = '';
        }
        var value = filter.getValue(filter.valueInput);
        if (value != '' || filter.allowEmptyValue) {
            switch(this.type) {
                case 'regex':
                    if (!this.regex.test(value)) {
                        return this.fail(filter);
                    }
                    break;
                case 'range':
                    var min = this.min;
                    if (min != '') {
                        if (min == '[YEAR]') {
                            var d = new Date();
                            min = d.getFullYear();
                        }
                        min = parseFloat(min);
                        if (value < min) {
                            return this.fail(filter);
                        }
                    }
                    var max = this.max;
                    if (max != '') {
                        if (max == '[YEAR]') {
                            var d = new Date();
                            max = d.getFullYear();
                        }
                        max = parseFloat(max);
                        if (value > max) {
                            return this.fail(filter);
                        }
                    }
                    break;
                    
                default:
                    return true;
            }
        }
        Element.removeClassName($(filter.valueInput), this.className);
        return true;
    },
    fail: function(filter) {
        Element.addClassName($(filter.valueInput), this.className);
        if ($(this.messageId)) {
            $(this.messageId).innerHTML = this.message;
        }
        return false;
    }
};

Fusion.Widget.AttributeQuery.Filter = Class.create();
Fusion.Widget.AttributeQuery.Filter.prototype = {
    unaryNot: null,
    validOperators: ['=', '<>', '<=', '<', '>=', '>', 'LIKE'],
    initialize: function( json ) {
        Object.inheritFrom(this, Fusion.Widget.AttributeQuery.FilterBase.prototype, [json]);
        
        var b  = json.AllowEmptyValue ? json.AllowEmptyValue[0] : 'false';
        this.allowEmptyValue = (b == '1' || b == 'true') ? true : false ;
        b = json.Not ? json.Not[0] : 'false';
        this.unaryNot = (b == '1' || b == 'true') ? true : false;
                         
    },
    getFilterText: function() {
        return this.getValue(this.valueInput, '');
    }

};

Fusion.Widget.AttributeQuery.SpatialFilter = Class.create();
Fusion.Widget.AttributeQuery.SpatialFilter.prototype = {
    validOperators: ['CONTAINS', 'CROSSES', 'DISJOINT', 'EQUALS',
                     'INSIDE', 'INTERSECTS', 'OVERLAPS', 'TOUCHES',
                     'WITHIN', 'COVEREDBY'],

    initialize: function(json) {
        Object.inheritFrom(this, Fusion.Widget.AttributeQuery.FilterBase.prototype, [json]);
    },
    getFilterText: function() {
        var result = '';
        var value = this.getValue(this.valueInput, '');
        if (value == '') {
            return result;
        }
        
        switch(this.valueType) {
            case 'POINT':
                var aValue = value.split(',');
                if (aValue.length == 2) {
                    result = 'POINT(';
                    result += aValue[0] + ' ' + aValue[1];
                    result += ')';
                }
                break;
            case 'LINE':
                var aValue = value.split(',');
                if (aValue.length > 2 && aValue.length % 2 == 0) {
                    result = 'LINE(';
                    var sep = '';
                    for (var i=0; i<aValue.length; i+=2) {
                        result += sep + aValue[i] + ' ' + aValue[i+1];
                        sep = ',';
                    }
                    result += ')';
                }
                break;
            case 'BBOX':
                var aValue = value.split(',');
                if (aValue.length == 4) {
                    result = 'POLYGON((';
                    result += aValue[0] + ' ' + aValue[1];
                    result += ',' + aValue[0] + ' ' + aValue[3];
                    result += ',' + aValue[2] + ' ' + aValue[3];
                    result += ',' + aValue[2] + ' ' + aValue[1];
                    result += ',' + aValue[0] + ' ' + aValue[1];
                    result += '))';
                }
                break;
            case 'WKT':
                result = value;
        }
        return result;
    }
};