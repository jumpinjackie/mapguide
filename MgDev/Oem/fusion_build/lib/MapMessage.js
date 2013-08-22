/*****************************************************************************
 * Class: Fusion.MapMessage
 *
 * It is to show a floating message bar over the main map viewer
 */
Fusion.MapMessage = OpenLayers.Class({
    parentNode : null,
    domObj : null,
    leadingIcon : null,
    textCell : null,
    message : "",
    
    infoIconName : "images/icons/info.png",
    warningIconName : "images/icons/warning.png",
    errorIconName : "images/icons/error.png",
    
    containerCssText : "position:absolute; z-index:10000; padding:10px; border:solid 2px #ECECEC; background:#FFFFBB",
    iconCssText : "margin-right:10px",
    textCellCssText : "width:100%; vertical-align:top; font: 8pt Tahoma",

    opacity: 0.95,
    
    initialize : function(parentNode)
    {
        this.parentNode = $(parentNode);
    },
    
    createBar : function()
    {
        if (!this.container)
        {
            // Create the div container
            var container   = document.createElement("div");
            container.style.visibility = "hidden";
            this.container  = $(container);
            this.parentNode.appendChild(container);
        }

        this.container.style.cssText = this.containerCssText;
        var offset = {left:10, top:10};
        this.container.style.left = offset.left + "px";
        this.container.style.top  = offset.top  + "px";
        
        this.container.innerHTML = "";
        
        // Create the inner table
        var table = document.createElement("table");
        this.container.appendChild(table);
        table.style.width = "100%";
        table.cellSpacing = "0";
        table.cellPadding = "0";
        table.border      = "0";
        // Create the table row
        var row   = table.insertRow(0);
        // The icon cell
        var cell  = row.insertCell(0);
        // Add the info icon by default
        var icon  = document.createElement("img");
        icon.src  = this.infoIconName;
        cell.appendChild(icon);
        icon.style.cssText = this.iconCssText;
        this.leadingIcon   = icon;
        // Create the text cell
        cell      = row.insertCell(1);
        cell.style.cssText = this.textCellCssText;
        this.textCell = $(cell);
        this.textCell.innerHTML = this.message;
        
        this.refreshLayout();
        // Hide message bar by default
        this.container.setOpacity(0);
        this.container.style.visibility = "visible";
    },
    
    removeBar : function()
    {
        if (typeof (this.container) != "undefined" && this.container != null)
        {
            this.container.fade(0);
            window.setTimeout((function()
            {
                if (typeof (this.container) != "undefined" && this.container != null)
                {
                    this.container.parentNode.removeChild(this.container);
                    this.container = null;
                }
                
            }).bind(this), 500);
        }
    },
    
    /**
     * Function: info
     * 
     * Displays an informational message on the Map Message notification bar
     * 
     * Parameters:
     * 
     *   message - The message to display
     */
    info : function(message)
    {
        this.message = message;
        this.show();
        this.leadingIcon.src = this.infoIconName;
    },
    
    /**
     * Function: warn
     * 
     * Displays a warning message on the Map Message notification bar
     * 
     * Parameters:
     * 
     *   message - The message to display
     */
    warn : function(message)
    {
        this.message = message;
        this.show();
        this.leadingIcon.src = this.warningIconName;
    },
    
    /**
     * Function: error
     * 
     * Displays a error message on the Map Message notification bar
     * 
     * Parameters:
     * 
     *   message - The message to display
     */
    error : function(message)
    {
        this.message = message;
        this.show();
        this.leadingIcon.src = this.errorIconName;
    },
    
    /**
     * Function: clear
     * 
     * Hides the notification bar and clears all content
     */
    clear : function()
    {
        this.message = "";
        this.textCell.innerHTML = "";
        this.hide();
    },
    
    /**
     * Function: show
     * 
     * Displays the notification bar
     */
    show : function()
    {
        this.createBar();
        this.textCell.innerHTML = this.message;
        this.container.fade(this.opacity);
    },
    
    /**
     * Function: hide
     * 
     * Hides the notification bar
     */
    hide : function()
    {
        this.removeBar();
    },
    
    hideDesignatedMessage: function(message)
    {
        if(message == this.message)
        {
            this.removeBar();
        }
    },
    
    refreshLayout: function()
    {
        if (typeof (this.container) != "undefined" && this.container != null)
        {
            // 44 = 2 * padding (10) + 2 * offset(10) + 2 * border (2)
            var newWidth = this.parentNode.offsetWidth - 44;
            if (newWidth >= 0)
                this.container.style.width  = this.parentNode.offsetWidth - 44 + "px";
        }
    },
    
    CLASS_NAME: "Fusion.MapMessage"
});