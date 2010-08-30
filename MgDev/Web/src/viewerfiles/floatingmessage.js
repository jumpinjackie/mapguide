var FloatingMessage = function(parentNode)
{
    this.initialize(parentNode);
}

FloatingMessage.prototype = 
{
    parentNode : null,
    domObj : null,
    leadingIcon : null,
    textCell : null,
    message : "",
    
    infoIconName : "../stdicons/icon_info.gif",
    warningIconName : "../stdicons/icon_warning.gif",
    errorIconName : "../stdicons/icon_error.gif",
    
    containerCssText : "position:absolute; z-index:10000; padding:10px; border:solid 2px #ECECEC; background:#FFFFBB",
    iconCssText : "margin-right:10px; width:16px; height:16px",
    textCellCssText : "width:100%; vertical-align:top; font: 8pt Tahoma",

    opacity: 0.95,
    
    initialize : function(parentNode)
    {
        this.parentNode = $(parentNode);
        // Create the div container
        var container   = document.createElement("div");
        container.style.visibility = "hidden";
        this.container  = $(container);
        parentNode.appendChild(container);

        container.style.cssText = this.containerCssText;
        var offset = {left:10, top:10};
        container.style.left = offset.left + "px";
        container.style.top  = offset.top  + "px";
        
        // Create the inner table
        var table = document.createElement("table");
        container.appendChild(table);
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
    
    info : function(message)
    {
        this.message = message;
        this.leadingIcon.src = this.infoIconName;
        this.show();
    },
    
    warn : function(message)
    {
        this.message = message;
        this.leadingIcon.src = this.warningIconName;
        this.show();
    },
    
    error : function(message)
    {
        this.message = message;
        this.leadingIcon.src = this.errorIconName;
        this.show();
    },
    
    clear : function()
    {
        this.message = "";
        this.textCell.innerHTML = "";
        this.hide();
    },
    
    show : function()
    {
        this.textCell.innerHTML = this.message;
        this.container.fade(this.opacity);
    },
    
    hide : function()
    {
        this.container.fade(0);
    },
    
    refreshLayout: function()
    {
        // 90 = 2 * padding (10) + 2 * offset(10) + 2 * border (2) + 46 (the navigator control)
        this.container.style.width  = this.parentNode.offsetWidth - 90 + "px";
    },
    
    CLASS_NAME: "FloatingMessage"
};