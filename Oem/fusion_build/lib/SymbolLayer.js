Fusion.SymbolLayer = OpenLayers.Class({
    map: null,
    autoCenterWhenOutOfView: true,
    symbolLayer: null,
    symbolFeature: null,
    symbolStyle: null,
    symbolRadius: 16,
    showDirection: false,
    directionFeature: null,
    directionStyle: null,
    directionLength: 80,
    isDraggable: true,
    dragControl: null,
    position: null,
    listeners: null,
    
    initialize: function(name, options) {
        Fusion.Lib.EventMgr.prototype.initialize.apply(this);
        OpenLayers.Util.extend(this, options);
        
        if(!this.map) {
            return;
        }
        
        var styleMapInitObj = {
            select: { cursor: "pointer" }
        };
        
        if(options.symbolStyle != null) {
            styleMapInitObj.symbol = options.symbolStyle;
        }
        
        if(options.directionStyle != null) {
            styleMapInitObj.direction = options.directionStyle;
        }
        
        this.symbolLayer = new OpenLayers.Layer.Vector(name, {
            displayInLayerSwitcher: false,
            styleMap: new OpenLayers.StyleMap(styleMapInitObj)
        });
        
        var self = this;
        
        this.symbolLayer.getFeatureFromEvent = function(evt) {
            var feature = OpenLayers.Layer.Vector.prototype.getFeatureFromEvent.apply(this, arguments);
            if(!feature || !self.showDirection || !self.symbolFeature) return feature;
            // Use symbol feature if the cursor is too close to symbol
            var evtPixel = self.map.events.getMousePosition(evt);
            var symbolFeaturePixel = self.map.getPixelFromLonLat(new OpenLayers.LonLat(self.symbolFeature.geometry.x, self.symbolFeature.geometry.y));
            var distX = evtPixel.x - symbolFeaturePixel.x;
            var distY = evtPixel.y - symbolFeaturePixel.y;
            var sqrDist = distX*distX+distY*distY;
            if(feature == self.symbolFeature) {
                if(sqrDist>self.symbolRadius*self.symbolRadius) {
                    feature = self.directionFeature;
                }
            }
            else if(feature == self.directionFeature) {
                if(sqrDist <= self.symbolRadius*self.symbolRadius) {
                    feature = self.symbolFeature;
                }
            }
            return feature;
        };
        
        this.map.addLayer(this.symbolLayer);
        
        if(this.isDraggable) {
            this.dragControl = new OpenLayers.Control.DragFeature(this.symbolLayer, {
                doneDragging: function(pixel) {
                    var position = self.map.getLonLatFromPixel(pixel);
                    if(!self.symbolFeature || (self.showDirection && !self.directionFeature) || !self.position) return;
                    if(this.feature == self.symbolFeature) {
                        self.moveSymbolTo(position.lon, position.lat);
                    }
                    else if(self.showDirection && this.feature == self.directionFeature){
                        var source = self.map.getPixelFromLonLat(new OpenLayers.LonLat(self.position.x, self.position.y));
                        var diffX = source.x - pixel.x;
                        var diffY = source.y - pixel.y;
                        if(diffX*diffX + diffY*diffY > self.symbolRadius*self.symbolRadius) {
                            self.rotateSymbolTo(position.lon, position.lat);
                        }
                    }
                },
                moveFeature: function(pixel) {
                    var position = self.map.getLonLatFromPixel(pixel);
                    if(!self.symbolFeature || (self.showDirection && !self.directionFeature) || !self.position) return;
                    if(this.feature == self.symbolFeature) {
                        self.moveSymbolTo(position.lon, position.lat);
                    }
                    else if(self.showDirection && this.feature == self.directionFeature){
                        var source = self.map.getPixelFromLonLat(new OpenLayers.LonLat(self.position.x, self.position.y));
                        var diffX = source.x - pixel.x;
                        var diffY = source.y - pixel.y;
                        if(diffX*diffX + diffY*diffY > self.symbolRadius*self.symbolRadius) {
                            self.rotateSymbolTo(position.lon, position.lat);
                        }
                    }
                }
            });
            this.map.addControl(this.dragControl);
        }
        
        this.map.events.register("zoomend", this, this.refreshSymbol);
        if(this.autoCenterWhenOutOfView) {
            this.map.events.register("moveend", this, this.onMapExtentsChanged);
        }
    },
    
    isDragControlActive: function() {
        return this.isDraggable && this.dragControl.active;
    },
    
    activateDragControl: function() {
        if(this.isDraggable) {
            this.dragControl.activate();
        }
    },
    
    deactivateDragControl: function() {
        if(this.isDraggable) {
            this.dragControl.deactivate();
        }
    },
    
    registerListener: function(listener) {
        if(!listener || typeof(listener.onSymbolChanged) != "function") return;
        if(!this.listeners) this.listeners = [];
        //make sure the same listener will not be added twice
        for(var index = 0; index < this.listeners.length; index++) {
            if(this.listeners[index] == listener) {
                return;
            }
        }
        this.listeners[this.listeners.length] = listener;
    },
    
    deregisterListener: function(listener) {
        if(!this.listeners || !listener) return;
        for(var index = 0; index < this.listeners.length; index++) {
            if(this.listeners[index] == listener) {
                this.listeners.splice(index, 1);
                break;
            }
        }
        if(this.listeners.length == 0) {
            this.deactivateDragControl();
            this.clearSymbol();
        }
    },
    
    fireChangeEvent: function(exceptionListener) {
        if(!this.listeners) return;
        for(var index = 0; index < this.listeners.length; index++) {
            var listener = this.listeners[index];
            if(listener != exceptionListener) {
                listener.onSymbolChanged(this.position);
            }
        }
    },
    
    isSameDouble: function(one, other) {
        return Math.abs(one - other) < 1e-9;    // 1e-9 is tolerance
    },
    
    isZero: function(d) {
        return Math.abs(d) < 1e-9;    //1e-9 is tolerance
    },
    
    onMapExtentsChanged: function() {
		if(!this.position) return;
        var extent = this.map.getExtent();
        if(!extent.contains(this.position.x, this.position.y)) {
            var mapCenterLL = extent.getCenterLonLat();
            this.moveSymbolTo(mapCenterLL.lon, mapCenterLL.lat, this);
        }
    },
    
    refreshSymbol: function(source) {
        if(!source) source = this;
        this.updateSymbolPosition(this.position.x, this.position.y, this.position.z, this.position.targetX, this.position.targetY, this.position.targetZ, source);
    },
    
    moveSymbolTo: function(x, y, source) {
        if(!this.position) {
            return;
        }
        
        if(!source) {
            source = this;
        }
        
        var diffX = this.position.targetX - this.position.x;
        var diffY = this.position.targetY - this.position.y;
        this.updateNormalizedSymbolPosition(x, y, this.position.z, x+diffX, y+diffY, this.position.targetZ, source);
    },
    
    rotateSymbolTo: function(x, y, source) {
        if(!this.position) {
            return;
        }
        
        if(!source) {
            source = this;
        }
        
        var oriDiffX = this.position.targetX-this.position.x;
        var oriDiffY = this.position.targetY-this.position.y;
        var oriDiff = Math.sqrt(oriDiffX*oriDiffX+oriDiffY*oriDiffY);
        
        var diffX = x - this.position.x;
        var diffY = y - this.position.y;
        var diffXY = Math.sqrt(diffX*diffX + diffY*diffY);
        
        if(!this.isZero(diffXY)) {
            x = this.position.x + diffX * oriDiff/diffXY;
            y = this.position.y + diffY * oriDiff/diffXY;
        }
        
        this.updateNormalizedSymbolPosition(this.position.x, this.position.y, this.position.z, x, y, this.position.targetZ, source);
    },
    
    updateSymbolPosition: function(x, y, z, targetX, targetY, targetZ, source) {
        if(!source) {
            source = this;
        }
        
        var diffX = targetX-x;
        var diffY = targetY-y;
        var diffXY = Math.sqrt(diffX*diffX+diffY*diffY);
        var viewSourcePixel = this.map.getPixelFromLonLat(new OpenLayers.LonLat(x, y));
        
        var alignLonLat = this.map.getLonLatFromPixel(viewSourcePixel.add(this.directionLength, 0));
        var alignDiffX = alignLonLat.lon-x;
        var alignDiffY = alignLonLat.lat-y;
        var alignDiffXY = Math.sqrt(alignDiffX*alignDiffX+alignDiffY*alignDiffY);
        
        if(!this.isZero(diffXY)) {
            targetX = x+diffX*alignDiffXY/diffXY;
            targetY = y+diffY*alignDiffXY/diffXY;
            targetZ = z+(targetZ-z)*alignDiffXY/diffXY;
        }
        
        this.updateNormalizedSymbolPosition(x, y, z, targetX, targetY, targetZ, source);
    },
    
    updateNormalizedSymbolPosition: function(x, y, z, targetX, targetY, targetZ, source) {
        
        var changed = ((!this.position) || (!this.isSameDouble(this.position.x, x)) || (!this.isSameDouble(this.position.y, y)) || (!this.isSameDouble(this.position.z, z)) ||
                (!this.isSameDouble(this.position.targetX, targetX)) || (!this.isSameDouble(this.position.targetY, targetY)) || (!this.isSameDouble(this.position.targetZ, targetZ)) ||
                (!this.symbolFeature));
            
        if(!this.position) {
            this.position = {};
        }
        
        this.position.x = x;
        this.position.y = y;
        this.position.z = z;
        this.position.targetX = targetX;
        this.position.targetY = targetY;
        this.position.targetZ = targetZ;
        
        if(!changed) {
            return;
        }
        
        this.updateView();
        if(!source) source = this;
        this.fireChangeEvent(source);
    },
    
    updateView: function() {
        
        if((!this.listeners || this.listeners.length == 0) ||
            (this.isSameDouble(this.position.x, this.position.targetX) && this.isSameDouble(this.position.y, this.position.targetY))) {
            this.clearSymbol();
            return;
        }
        
        var viewSourcePixel = this.map.getPixelFromLonLat(new OpenLayers.LonLat(this.position.x, this.position.y));
        var viewTargetPixel = this.map.getPixelFromLonLat(new OpenLayers.LonLat(this.position.targetX, this.position.targetY));
        var rotation = 180.0*Math.atan2(viewTargetPixel.y-viewSourcePixel.y, viewTargetPixel.x-viewSourcePixel.x)/Math.PI;
        this.symbolLayer.styleMap.styles["symbol"].defaultStyle.rotation = rotation;
        
        if(!this.symbolFeature) {
            var startPoint = new OpenLayers.Geometry.Point(this.position.x, this.position.y);
            this.symbolFeature = new OpenLayers.Feature.Vector(startPoint);
            this.symbolFeature.renderIntent = "symbol";
            if(this.showDirection) {
                this.directionFeature = new OpenLayers.Feature.Vector(new OpenLayers.Geometry.LineString([startPoint, new OpenLayers.Geometry.Point(this.position.targetX, this.position.targetY)]));
                this.directionFeature.renderIntent = "direction";
                this.symbolLayer.addFeatures(this.directionFeature);
            }
            
            this.symbolLayer.addFeatures(this.symbolFeature);   
        }else {
            
            this.symbolFeature.geometry.move(this.position.x-this.symbolFeature.geometry.x, this.position.y-this.symbolFeature.geometry.y);
            if(this.showDirection) {
                var originalTarget = this.directionFeature.geometry.components[1];
                originalTarget.move(this.position.targetX-originalTarget.x, this.position.targetY-originalTarget.y);
                this.symbolLayer.drawFeature(this.directionFeature);
            }
            
            this.symbolLayer.drawFeature(this.symbolFeature);
        }
    },
    
    clearSymbol: function() {
        this.symbolLayer.destroyFeatures();
        this.symbolFeature = null;
        this.directionFeature = null;
    }
});

Fusion.SymbolLayer.CreateOrGet = function(name, options) {
    var map = options.map;
    if(!map) return null;
    var layers = map.getLayersByName(name);
    if(layers != null && layers.length != 0) {
        var layer = layers[0];
        if(layer != null) {
            return (!layer.extension) ? null : layer.extension.symbolWrapper;
        }
    }
    var toReturn = new Fusion.SymbolLayer(name, options);
    var symbolLayer = toReturn.symbolLayer;
    if(!symbolLayer.extension) symbolLayer.extension = {};
    symbolLayer.extension.symbolWrapper = toReturn;
    return toReturn;
}