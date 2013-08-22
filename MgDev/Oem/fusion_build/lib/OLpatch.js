/** remove String.prototype.contains **/
String.prototype.contains = null;

/**
 * Method: isValidLonLat
 * 
 * Parameters:
 * lonlat - {<OpenLayers.LonLat>}
 * 
 * Returns:
 * {Boolean} Whether or not the lonlat passed in is non-null and within
 *           the maxExtent bounds
 */
OpenLayers.Map.prototype.isValidLonLat = function(lonlat) {
    var valid = false;
    if (lonlat != null) {
        var maxExtent = this.getMaxExtent();
        valid = this.baseLayer.displayOutsideMaxExtent || maxExtent.containsLonLat(lonlat);        
    }
    return valid;
}

// Add swiss projection
Proj4js.wktProjections["Swiss_Oblique_Cylindrical"] = "somerc";
