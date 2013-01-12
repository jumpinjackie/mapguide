/* Copyright (c) 2006-2008 MetaCarta, Inc., published under the Clear BSD
 * license.  See http://svn.openlayers.org/trunk/openlayers/license.txt for the
 * full text of the license. */

/**
 * @requires OpenLayers/Lang/en.js
 */

/**
 * Namespace: OpenLayers.Lang["hu"]
 * Dictionary for Hungarian.  Keys for entries are used in calls to
 *     <OpenLayers.Lang.translate>.  Entry bodies are normal strings or
 *     strings formatted for use with <OpenLayers.String.format> calls.
 */
OpenLayers.Lang.hu = OpenLayers.Util.applyDefaults({

    'unhandledRequest': "Nem kezelt kérés visszatérése ${statusText}",

    'permalink': "Permalink",

    'overlays': "Rávetítések",

    'baseLayer': "Alapréteg",

    'sameProjection':
        "Az áttekintő térkép csak abban az esetben működik, ha ugyanazon a vetületen van mint a fő térkép.",

    'readNotImplemented': "Olvasás nincs végrehajtva.",

    'writeNotImplemented': "Írás nincs végrehajtva.",

    'noFID': "Nem frissíthető olyan jellemző, amely nem rendelkezik FID-del.",

    'errorLoadingGML': "Hiba GML-fájl betöltésekor ${url}",

    'browserNotSupported':
        "A böngészője nem támogatja a vektoros renderelést. A jelenleg támogatott renderelők:\n${renderers}",

    'componentShouldBe': "addFeatures : az összetevőnek ilyen típusúnak kell lennie: ${geomType}",

    // console message
    'getFeatureError':
        "A getFeatureFromEvent réteget hívott meg renderelő nélkül. Ez rendszerint azt jelenti, hogy " +
        "megsemmisített egy fóliát, de néhány ahhoz társított kezelőt nem.",

    // console message
    'minZoomLevelError':
        "A minZoomLevel tulajdonságot csak a következővel való használatra szánták: " +
        "a FixedZoomLevels-leszármazott fóliák. Ez azt jelenti, hogy " +
        "a minZoomLevel wfs fólia jelölőnégyzetei már " +
        "a múlté. Mi azonban nem távolíthatjuk el annak a veszélye nélkül, hogy " +
        "az esetlegesen ettől függő OL alapú alkalmazásokat tönkretennénk." +
        "Ezért ezt érvénytelenítjük -- a minZoomLevel " +
        "az alul levő jelölőnégyzet a 3.0-s verzióból el lesz távolítva. Kérjük, helyette " +
        "használja a  min/max felbontás beállítást, amelyről az alábbi helyen talál leírást: " +
        "http://trac.openlayers.org/wiki/SettingZoomLevels",

    'commitSuccess': "WFS tranzakció: SIKERES ${response}",

    'commitFailed': "WFS tranzakció: SIKERTELEN ${response}",

    'googleWarning':
        "A Google fólia betöltése sikertelen.<br><br>" +
        "Ahhoz, hogy ez az üzenet eltűnjön, válasszon egy új BaseLayer fóliát " +
        "a jobb felső sarokban található fóliakapcsoló segítségével.<br><br>" +
        "Nagy valószínűséggel ez azért van, mert a Google Maps könyvtár " +
        "parancsfájlja nem található, vagy nem tartalmazza " +
        "az Ön oldalához tartozó megfelelő API-kulcsot.<br><br>" +
        "Fejlesztőknek: A helyes működtetésre vonatkozó segítség az alábbi helyen érhető el, " +
        "<a href='http://trac.openlayers.org/wiki/Google' " +
        "target='_blank'>kattintson ide</a>",

    'getLayerWarning':
        "A ${layerType} fólia nem töltődött be helyesen.<br><br>" +
        "Ahhoz, hogy ez az üzenet eltűnjön, válasszon egy új BaseLayer fóliát " +
        "a jobb felső sarokban található fóliakapcsoló segítségével.<br><br>" +
        "Nagy valószínűséggel ez azért van, mert a ${layerLib} könyvtár " +
        "parancsfájlja helytelen.<br><br>" +
        "Fejlesztőknek: A helyes működtetésre vonatkozó segítség az alábbi helyen érhető el, " +
        "<a href='http://trac.openlayers.org/wiki/${layerLib}' " +
        "target='_blank'>kattintson ide</a>",

    'scale': "Lépték = 1 : ${scaleDenom}",

    // console message
    'layerAlreadyAdded':
        "Megpróbálta hozzáadni a ${layerName} fóliát a térképhez, de az már hozzá van adva",

    // console message
    'reprojectDeprecated':
        "Ön a 'reproject' beállítást használja " +
        "a ${layerName} fólián. Ez a beállítás érvénytelen: " +
        "használata az üzleti alaptérképek fölötti adatok megjelenítésének támogatására " + 
        "szolgált, de ezt a funkció ezentúl a " +
        "Gömbi Mercator használatával érhető el. További információ az alábbi helyen érhető el: " +
        "http://trac.openlayers.org/wiki/SphericalMercator.",

    // console message
    'methodDeprecated':
        "Ez a módszer érvénytelenítve lett és a 3.0-s verzióból el lesz távolítva. " +
        "Használja a ${newMethod} módszert helyette.",

    // console message
    'boundsAddError': "Az x és y értékeknek egyaránt meg kell felelnie, hogy a funkciót hozzáadhassa.",

    // console message
    'lonlatAddError': "A hossz. és szél. értékeknek egyaránt meg kell felelnie, hogy a funkciót hozzáadhassa.",

    // console message
    'pixelAddError': "Az x és y értékeknek egyaránt meg kell felelnie, hogy a funkciót hozzáadhassa.",

    // console message
    'unsupportedGeometryType': "Nem támogatott geometriatípus: ${geomType}",

    // console message
    'pagePositionFailed':
        "OpenLayers.Util.pagePosition failed: lehetséges, hogy az ${elemId} azonosítójú elem téves helyre került.",
                    
    // console message
    'filterEvaluateNotImplemented': "ennél a szűrőtípusnál kiértékelés nem hajtódik végre.",
	
    'end': ''
}, OpenLayers.Lang["en"]);
