/********************************************************************** * 
 * @project Fusion
 * @revision $Id: Print.js 933 2007-10-15 19:42:06Z madair $
 * @purpose ZoomToSelection widget
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
 * in all copies or substantial portions of the Software.
 ********************************************************************
 *
 * Print the current map.
 *
 * To put a Print control in your application, you first need to add
 * a widget to your WebLayout as follows:
 *
 * <Command xsi:type="FusionCommandType">
 *   <Name>MyPrint</Name>
 *   <Label>Print/Label>
 *   <TargetViewer>All</TargetViewer>
 *   <Action>Print</Action>
 *   <ImageURL>images/icon_print.png</ImageURL>
 *   <DisabledImageURL>images/icon_print.png</DisabledImageURL>
 *   <Width>24</Width>
 *   <Height>24</Height>
 *   <ShowPrintUI>false</ShowPrintUI>
 *   <PageTitle>Nanaimo Cemetery</PageTitle>
 *   <ShowLegend>false</ShowLegend>
 *   <ShowNorthArrow>false</ShowNorthArrow>
 * </Command>
 *
 * The important parts of this Command are:
 *
 * Name (string, mandatory) 
 * 
 * an element with an id that is the same as this name must be in
 * the application.  For instance:
 *
 * <div id="MyPrint"></div>
 *
 * A button that activates this widget will appear inside the
 * element you provide.
 * **********************************************************************/

Fusion.Widget.Print = Class.create();
Fusion.Widget.Print.prototype = {
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        
        var json = widgetTag.extension;
        
        var showPrintUI = json.ShowPrintUI ? json.ShowPrintUI[0] : 'false';
        this.showPrintUI = (showPrintUI.toLowerCase() == 'true' || showPrintUI == '1');
        
        var showTitle = json.ShowTitle ? json.ShowTitle[0] : 'false';
        this.showTitle = (showTitle.toLowerCase() == 'true' || showTitle == '1');

        this.pageTitle = json.PageTitle ? json.PageTitle[0] : '';
        
        this.resultsLayer = json.ResultsLayer ? json.ResultsLayer[0] : null;

        var showLegend = json.ShowLegend ? json.ShowLegend[0] : 'false';
        this.showLegend = (showLegend.toLowerCase() == 'true' || showLegend == '1');
        
        var showNorthArrow =json.ShowNorthArrow ? json.ShowNorthArrow[0] : 'false';
        this.showNorthArrow = (showNorthArrow.toLowerCase() == 'true' || showNorthArrow == '1');
        
        //this.dialogContentURL = Fusion.getRedirectScript() + '?s=' + Fusion.getFusionURL() + widgetTag.location + 'html/Print.html';
        this.dialogContentURL = Fusion.getFusionURL() + widgetTag.location + 'html/Print.html';
        
        /*
         * TODO: this is bad, why did we do this?
         this.getMap().registerForEvent(Fusion.Event.SELECTION_COMPLETE, this.getSelection.bind(this));
         */
        
    },
    /**
     * load an interface that builds a printable version of
     * the current map view
     */
    execute : function() {
        if (this.showPrintUI) {
            this.openPrintUI();
        } else if (this.resultsLayer){
            this.createResultLayer();  
        } else {
            this.openPrintable();
        }
    },
    
    openPrintUI: function() {
        if (!this.dialog) {

            var size = Element.getPageDimensions();
            var o = {
                title: 'Printable Page ',
                id: 'printablePage',
                contentURL : this.dialogContentURL,
                onContentLoaded: this.contentLoaded.bind(this),
                width: 320,
                height: 200,
                top: (size.height-200)/2,
                left: (size.width-320)/2,
                buttons: ['generate', 'cancel'],
                handler: this.handler.bind(this)
            };
            this.dialog = new Jx.Dialog(o);
            
        }
        this.dialog.open();
    },
    
    setParameter: function(param, value) {
        switch (param) {
            case 'Print_ShowTitle':
            this.showTitle = value;
            break;
            case 'Print_Title':
            this.pageTitle = value;
            break;
            case 'Print_ShowLegend':
            this.showLegend = value;
            break;
            case 'Print_ShowNorthArrow':
            this.showNorthArrow = value;
            break;
        }
    },
    
    contentLoaded: function() {
        this.dialog.registerIds(['dialog.print.showtitle', 
                                 'dialog.print.title',
                                 'dialog.print.showlegend',
                                 'dialog.print.shownortharrow'], this.dialog.content);
        this.dialog.getObj('dialog.print.showtitle').checked = this.showTitle;
        this.dialog.getObj('dialog.print.title').value = this.pageTitle;
        this.dialog.getObj('dialog.print.title').disabled = !this.showTitle;
        this.dialog.getObj('dialog.print.showlegend').checked = this.showLegend;
        this.dialog.getObj('dialog.print.shownortharrow').checked = this.showNorthArrow;
        
        Event.observe(this.dialog.getObj('dialog.print.showtitle'), 'click', this.controlTitle.bind(this));
    },
    
    controlTitle: function() {
        this.dialog.getObj('dialog.print.title').disabled = !this.dialog.getObj('dialog.print.showtitle').checked;
        
    },
    
    handler: function(button) {
        if (button == 'generate') {
            this.showTitle = this.dialog.getObj('dialog.print.showtitle').checked;
            this.pageTitle = this.dialog.getObj('dialog.print.title').value;
            this.showLegend = this.dialog.getObj('dialog.print.showlegend').checked;
            this.showNorthArrow = this.dialog.getObj('dialog.print.shownortharrow').checked;
            this.openPrintable();
            
        }
        this.dialog.close();
    },
    
    /* retrieve the results from the attributeQuery widget */
    /* triggered by a Fusion.Event.SELECTION_COMPLETE event */
    /*
     TODO: this is bad, we are directly dependent on the AttributeQuery widget
    getSelection: function() {
        var widget = Fusion.getWidgetById('AttributeQuery');
        var count = widget.getNumberOfResults();
        this.selectionString = '';
        var sep = '';
        for (var i=0; i < count; i++) {
            this.selectionString += '(' + widget.getResult(i) + ')';
            sep = ' AND ';
        };
        
    },
    */
    /* call the server to create a layer with search results if needed*/
    /* the layer to use is specified in the weblayout */ 
    createResultLayer: function() {
        if (!this.resultsLayer) {
            return;
        }
        
        var maps = this.getMap().getAllMaps();
        var printFeaturesUrl = 'ext/nanaimo/' + this.getMap().arch + '/' + Fusion.getScriptLanguage() +
                      '/PrintFeatures.' + Fusion.getScriptLanguage();
        var session = 'session='+maps[0].getSessionID();
        var mapname = '&mapname='+this.getMap().getMapName();
        var layer = '&layer='+ this.resultsLayer;
        var selection = '&selection=' + this.selectionString;
        var params = {};
        params.parameters = session + mapname + layer + selection;
        params.onComplete = this.resultLayerCB.bind(this);
        Fusion.ajaxRequest(printFeaturesUrl, params);
        
    },
    
    resultLayerCB: function( r, json) {
        if (json) {
            var o;
            eval('o=' + r.responseText);
            //TODO: test result
            this.openPrintable();
        }
    },
    
    openPrintable: function() {
        var mainMap = this.getMap();
        var url = Fusion.getConfigurationItem('mapguide', 'webTierUrl') + 'mapviewerphp/printablepage.php?';
        var extents = mainMap.getCurrentExtents();
        var centerX = (extents.left + extents.right)/ 2;
        var centerY = (extents.top + extents.bottom)/ 2;
        var dpi = mainMap._nDpi;
        var scale = mainMap.getScale();
        var maps = mainMap.getAllMaps();
        url = url + 'MAPNAME=' + mainMap.getMapName();
        url = url + '&SESSION=' + maps[0].getSessionID();
        url = url + '&CENTERX='+centerX;
        url = url + '&CENTERY='+centerY;
        url = url + '&DPI='+dpi;
        url = url + '&SCALE='+scale;
        url = url + '&ISTITLE=' + (this.showTitle != '' ? '1' : '0');
        url = url + '&ISLEGEND=' + (this.showLegend ? '1' : '0');
        url = url + '&ISARROW=' + (this.showNorthArrow ? '1' : '0');
        if (this.pageTitle != '') {
            url = url + '&TITLE='+this.pageTitle;
        }
        
        window.open(url, 'printablepage', '');
        
    }
};
