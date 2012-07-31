/**
 * OpenLayersGMapsPatch.js
 *
 * This is a TOS-compliant patch to remove the "Map Data" popup that gets displayed on each map move.
 *
 * Source: http://trac.osgeo.org/openlayers/wiki/Release/2.11/GoogleMaps37
 * TODO: When we move to OpenLayers 2.12, the patch is built in and this file will no longer be needed
 */

OpenLayers.Layer.Google.v3.repositionMapElements = function() {
    // This is the first time any Google layer in this mapObject has been
    // made visible.  The mapObject needs to know the container size.
    google.maps.event.trigger(this.mapObject, "resize");
    
    var div = this.mapObject.getDiv().firstChild;
    if (!div || div.childNodes.length < 3) {
        this.repositionTimer = window.setTimeout(
            OpenLayers.Function.bind(this.repositionMapElements, this),
            250
        );
        return false;
    }

    var cache = OpenLayers.Layer.Google.cache[this.map.id];
    var container = this.map.viewPortDiv;
    
    // move the ToS and branding stuff up to the container div
    // depends on value of zIndex, which is not robust
    for (var i=div.children.length-1; i>=0; --i) {
        if (div.children[i].style.zIndex == 1000001) {
            var termsOfUse = div.children[i];
            container.appendChild(termsOfUse);
            termsOfUse.style.zIndex = "1100";
            termsOfUse.style.bottom = "";
            termsOfUse.className = "olLayerGoogleCopyright olLayerGoogleV3";
            termsOfUse.style.display = "";
            cache.termsOfUse = termsOfUse;
        }
        if (div.children[i].style.zIndex == 1000000) {
            var poweredBy = div.children[i];
            container.appendChild(poweredBy);
            poweredBy.style.zIndex = "1100";
            poweredBy.style.bottom = "";
            poweredBy.className = "olLayerGooglePoweredBy olLayerGoogleV3 gmnoprint";
            poweredBy.style.display = "";
            cache.poweredBy = poweredBy;
        }
        if (div.children[i].style.zIndex == 10000002) {
            container.appendChild(div.children[i]);
        }
    }

    this.setGMapVisibility(this.visibility);
};