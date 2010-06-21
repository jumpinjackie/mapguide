function LegendItem(type, code)
{
    this.type = type;
    this.code = code;
    this.id = 0;
}

function GroupItem(legend, expanded, parent, visible, displayInLegend, objectId, name, basegroup)
{
    this.LegendItemInfo = LegendItem;
    this.LegendItemInfo(0, 'G');
    this.legend = legend;
    this.expanded = expanded;
    this.parent = parent;
    this.visible = visible;
    this.displayInLegend = displayInLegend;
    this.objectId = objectId;
    this.name = name;
    this.base = basegroup;
    this.children = null;
    this.hasVisibleLayers = false;
    this.isDisplayed = false;
}

function LayerItem(legend, name, expanded, parent, visible, displayInLegend, selectable, resId, objectId, baselayer)
{
    this.LegendItemInfo = LegendItem;
    this.LegendItemInfo(1, 'L');
    this.legend = legend;
    this.expanded = expanded;
    this.parent = parent;
    this.visible = visible;
    this.displayInLegend = displayInLegend;
    this.selectable = selectable;
    this.resId = resId;
    this.objectId = objectId;
    this.layerRes = resId;
    this.name = name;
    this.base = baselayer;
    this.children = new Array();
    this.lyrtype = 0;
    this.curScale = -1;
    this.isDisplayed = false;
    this.iconWasRequested = false;
}

function ScaleRangeItem(min, max, parent)
{
    this.LegendItemInfo = LegendItem;
    this.LegendItemInfo(2, 'R');
    this.min = min;
    this.max = max;
    this.parent = parent;
    this.showlongtheme = false;
    this.children = new Array();
    this.iconWasRequested = false;
}

function StyleItem(legend, filter, geomtype, catindex)
{
    this.LegendItemInfo = LegendItem;
    this.LegendItemInfo(3, 'S');
    this.legend = legend;
    this.filter = filter;
    this.geomtype = geomtype;
    this.catindex = catindex;
    this.iconWasRequested = false;
}

function GroupSummary(name, objectid, parent)
{
    this.name = name;
    this.objectid = objectid;
    this.isGroup = true;
    this.parent = parent;
    this.children = null;
}

function LayerSummary(name, objectId, layerRes)
{
    this.name = name;
    this.objectId = objectId;
    this.layerRes = layerRes;
    this.isGroup = false;
    this.type = 1;
}

function Layer(legend, name, objectId)
{
    this.legend = legend;
    this.name = name;
    this.objectId = objectId;
}
