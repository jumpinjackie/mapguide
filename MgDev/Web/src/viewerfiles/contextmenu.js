var scrollingMenu;
var iconCtxMenu = "../stdicons/icon_ctxarrow.gif"; //TODO will be overridable. should treat scrolling icons the same way

function MenuData(menuName, arrowHeight, scrollInc, scrollDelay, minSize, iconScrollUp, iconScrollUpDisabled, iconScrollDown, iconScrollDownDisabled, owner, withIFrame, bkColor)
{
    this.menuName = menuName;
    this.arrowHeight = arrowHeight;
    this.scrollDelay = scrollDelay;
    this.scrollInc = scrollInc;
    this.minSize = minSize;
    this.iconScrollUp = iconScrollUp;
    this.iconScrollUpDisabled = iconScrollUpDisabled;
    this.iconScrollDown = iconScrollDown;
    this.iconScrollDownDisabled = iconScrollDownDisabled;
    this.owner = owner;
    this.withIFrame = withIFrame;
    this.bkColor = bkColor;
    resetContent(this);
}

function resetContent(menuObj)
{
    menuObj.menuDiv = null;
    menuObj.listDiv = null;
    menuObj.innerListDiv = null;
    menuObj.scrollAmount = 0
    menuObj.scrolled = false;
    menuObj.scrolling = false;
    menuObj.scrollTimer = null;
    menuObj.scrollUpArea = null;
    menuObj.scrollDownArea = null;
    menuObj.scrollUpArrow = null;
    menuObj.scrollDownArrow = null;
    menuObj.content = "";
    menuObj.optionCount = 0;
    menuObj.subMenus = new Array();
    menuObj.parentMenu = null;
    menuObj.currChildMenu = null;
    menuObj.nesting = 0;
    menuObj.openRight = true;
    menuObj.mouseY = 0;
    menuObj.left = 0;
    menuObj.width = 0;
    menuObj.top = 0;
    menuObj.contentHeight = 0;
}

function initMenu()
{
    menuData = new MenuData("Main", 20, 21, 50, 250);
}

function enableArrow(menuObj, enabled, up)
{
    if(up)
        menuObj.scrollUpArrow.src = enabled? menuObj.iconScrollUp: menuObj.iconScrollUpDisabled;
    else
        menuObj.scrollDownArrow.src = enabled? menuObj.iconScrollDown: menuObj.iconScrollDownDisabled;
}

function onEnterOption(e)
{
    var bgcolor = "#dcdcdc";
    var color = "black";

    var menuObj = this.menuObj;
    if(menuObj != null)
    {
        var name = getAttributeNodeValue(this,"name");
        if(name.indexOf("Sub:") == 0)
        {
            var subMenu = menuObj.subMenus[name.substr(4)];
            if(menuObj.currChildMenu)
            {
                if(subMenu.menuName == menuObj.currChildMenu.menuName)
                {
                    this.style.backgroundColor = bgcolor;
                    this.style.color = color;
                    return true;
                }
                else
                {
                    hidePopup(menuObj.currChildMenu);
                    menuObj.currChildMenu = null;
                }
            }
            menuObj.mouseY = (msie? event.clientY: e.clientY);
            menuObj.currChildMenu = subMenu;
            showPopup(subMenu, true, -1, -1);
        }
        else
        {
            if(menuObj.currChildMenu)
            {
                hidePopup(menuObj.currChildMenu);
                menuObj.currChildMenu = null;
            }

            if(menuObj.owner)
            {
                try
                {
                    menuObj.owner.OnCommandSelected(name);
                }
                catch(e) {}
            }

            var enabled = getAttributeNodeValue(this,"state");
            if(!enabled || enabled == "false")
                return true;
        }
    }

    this.style.backgroundColor = bgcolor;
    this.style.color = color;
    return true;
}

function onLeaveOption(e)
{
    var bgcolor = "white";
    var color = "black";

    var menuObj = this.menuObj;
    if(menuObj != null)
    {
        bgcolor = menuObj.bkColor;
        if(menuObj.owner)
        {
            try
            {
                menuObj.owner.OnCommandSelected(-1);
            }
            catch(e) {}
        }
        var name = getAttributeNodeValue(this,"name");
        if(name.indexOf("Sub:") != 0)
        {
            var enabled = getAttributeNodeValue(this,"state");
            if(!enabled || enabled == "false")
                return true;
        }
    }

    this.style.backgroundColor = bgcolor;
    this.style.color = color;
    return true;
}

function scrollUp(menuObj)
{
    if(scrollingMenu == null)
    {
        alert("No Menu object !!!");
        return;
    }

    if(scrollingMenu.scrolled == 0)
        return;
    scrollingMenu.scrolled -= scrollingMenu.scrollInc;
    if(scrollingMenu.scrolled < 0)
        scrollingMenu.scrolled = 0;
    scrollingMenu.innerListDiv.style.top = -scrollingMenu.scrolled +"px";
    enableArrow(scrollingMenu, true, false);

    if(scrollingMenu.scrolled == 0)
    {
        clearInterval(scrollingMenu.scrollTimer);
        scrollingMenu.scrolling = false;
        enableArrow(scrollingMenu, false, true);
    }
}

function scrollDown(menuObj)
{
    if(scrollingMenu == null)
    {
        alert("No Menu object !!!");
        return;
    }

    if(scrollingMenu.scrolled >= scrollingMenu.scrollAmount)
        return;
    scrollingMenu.scrolled += scrollingMenu.scrollInc;
    if(scrollingMenu.scrolled > scrollingMenu.scrollAmount)
        scrollingMenu.scrolled = scrollingMenu.scrollAmount;
    scrollingMenu.innerListDiv.style.top = -scrollingMenu.scrolled +"px";
    enableArrow(scrollingMenu, true, true);

    if(scrollingMenu.scrolled == scrollingMenu.scrollAmount)
    {
        clearInterval(scrollingMenu.scrollTimer);
        scrollingMenu.scrolling = false;
        enableArrow(scrollingMenu, false, false);
    }
}

function onStartScrollingUp(e)
{
    var ev = (msie? event: e);
    var tgt = (msie? ev.srcElement: e.target);

    var menuObj = tgt.menuObj;
    if(menuObj == null)
        return false;

    scrollingMenu = menuObj;
    if(!menuObj.scrolling)
    {
        menuObj.scrollTimer = setInterval(scrollUp, menuObj.scrollDelay);
        menuObj.scrolling = true;
    }
    return false;
}

function onStopScrolling(e)
{
    var ev = (msie? event: e);
    var tgt = (msie? ev.srcElement: e.target);

    var menuObj = tgt.menuObj;
    if(menuObj == null)
        return false;

    scrollingMenu = null;
    if(menuObj.scrolling)
    {
        clearInterval(menuObj.scrollTimer);
        menuObj.scrolling = false;
    }
    return false;
}

function onStartScrollingDown(e)
{
    var ev = (msie? event: e);
    var tgt = (msie? ev.srcElement: e.target);

    var menuObj = tgt.menuObj;
    if(menuObj == null)
        return false;

    scrollingMenu = menuObj;
    if(!menuObj.scrolling)
    {
        menuObj.scrollTimer = setInterval(scrollDown, menuObj.scrollDelay);
        menuObj.scrolling = true;
    }
    return false;
}

function executeOption(menuObj, option)
{
    var menu = menuObj;
    do
    {
        if(menu.parentMenu == null)
            break;
        menu = menu.parentMenu;
    }
    while(true);

    hidePopup(menu);
    if(menu.owner)
    {
        try
        {
            menu.owner.ExecuteCommand(option);
        }
        catch(e) {}
    }
}

function PopupMouseDown(e)
{
    var ev = (msie? (e == null? event: e): e);
    var tgt = getOptionElement(msie? ev.srcElement: e.target);

    var menuObj = tgt == null? null: tgt.menuObj;
    if(menuObj != null)
    {
        var state = getAttributeNodeValue(tgt,"state");
       if(!(!state || state == "false"))
           executeOption(menuObj, getAttributeNodeValue(tgt,"name"));
        return true;
    }
    else
        return false;
}

function getOptionElement(elt)
{
    if(elt == null)
        return null;
    while(elt && elt.nodeName != "TR")
        elt = elt.parentNode;
    return elt;
}

function getMaxOptionLen(menuObj)
{
    var len = 0;
    var metrics = document.getElementById("Metrics");
    for(var i = 1; ; i++)
    {
        var text = document.getElementById(menuObj.menuName + "T" + i);
        if(text == null)
            break;
        metrics.innerHTML = text.innerHTML;
        if(metrics.offsetWidth > len)
            len = metrics.offsetWidth;
    }
    return len;
}

function startBuildMenuContent(menuObj)
{
    resetContent(menuObj);
    menuObj.content = "<div class=\"PopupScrollUpArea\" id=\"" + menuObj.menuName + "ScrollUpArea\" align=center style=\"height: 12px;\"><img id=\"" + menuObj.menuName + "ArrowUp\" src=\"" + menuObj.iconScrollUp + "\" width=16 height=16></div>" +
        "<div class=\"PopupList\" id=\"" + menuObj.menuName + "List\" style=\"background-color: " + menuObj.bkColor + "\">\n" +
        "<div class=\"PopupInnerList\" id=\"" + menuObj.menuName + "InnerList\" style=\"background-color: " + menuObj.bkColor + "\">\n" +
        "<table class=\"PopupTableContent\" id=\"Tbl" + menuObj.menuName + "\" width=100% cellspacing=0 cellpadding=0 style=\"background-color: " + menuObj.bkColor + "\">\n";
}

function endBuildMenuContent(menuObj)
{
    menuObj.content +=
        "</table>\n" +
        "</div>\n" +
        "</div>\n" +
        "<div class=\"PopupScrollDownArea\" id=\"" + menuObj.menuName + "ScrollDownArea\" align=center><img src=\"" + menuObj.iconScrollDown + "\" id=\"" + menuObj.menuName + "ArrowDown\" width=16 height=16></div>\n";
}

function addMenuOption(menuObj, text, param, type, icon, iconDisabled, enabled)
{
    if(type != 2)
    {
        var imgTag;
        if(icon == "")
            imgTag = "";
        else
            imgTag = "<img width=16 height=16 id=\"PMI" + param + "\" src=\"" + icon + "\" style=\"position: relative; top: 2px; left: 0px;\">";
        var optNum = menuObj.optionCount++ + 1;

        if(type == 1)
            menuObj.content += "<tr height=21 id=\"" + (menuObj.menuName + optNum) + "\" state=\"" + (enabled? "true": "false") + "\" icon=\"" + icon + "\" icond=\"" + iconDisabled +  "\" name=\"" + param + "\"><td width=24 align=center><div style=\"width: 100%; height: 21px;\">" + imgTag + "</td><td><span id=\"" + (menuObj.menuName + "T" + optNum) + "\">" + text + "</span></td><td width=8></div></td></tr>\n";
        else
            menuObj.content += "<tr height=21 id=\"" + (menuObj.menuName + optNum) + "\" name=\"" + param + "\"><td width=24 align=center><div style=\"width: 100%; height: 21px;\">" + imgTag + "</td><td><span id=\"" + (menuObj.menuName + "T" + optNum) + "\">" + text + "</span></td><td width=8><img src=\"" + iconCtxMenu + "\"></div></td></tr>\n";

        menuObj.contentHeight += 21;
    }
    else
    {
        menuObj.content += "<tr height=3><td colspan=3 align=center valign=center><div style=\"width: 95%; height: 2px; border-bottom: #a0a0a0 1px solid;\"><span></span></div></td></tr>\n";
        menuObj.contentHeight += 3;
    }
}

function showPopup(menuObj, optimizeWidth, x, y)
{
    menuObj.menuDiv = document.getElementById(menuObj.menuName);
    menuObj.menuDiv.innerHTML = menuObj.content;

    menuObj.listDiv = document.getElementById(menuObj.menuName + "List");
    menuObj.innerListDiv = document.getElementById(menuObj.menuName + "InnerList");
    menuObj.scrollUpArea = document.getElementById(menuObj.menuName + "ScrollUpArea");
    menuObj.scrollDownArea = document.getElementById(menuObj.menuName + "ScrollDownArea");
    menuObj.scrollUpArrow = document.getElementById(menuObj.menuName + "ArrowUp");
    menuObj.scrollDownArrow = document.getElementById(menuObj.menuName + "ArrowDown");

    menuObj.scrollUpArea.menuObj = menuObj;
    menuObj.scrollDownArea.menuObj = menuObj;

    for(var i=1; ; i++)
    {
        var obj = document.getElementById(menuObj.menuName + i);
        if(obj == null)
            break;

        var name = getAttributeNodeValue(obj,"name");
        if(name.indexOf("Sub:") != 0)
        {
            var state = getAttributeNodeValue(obj,"state");
            var enabled = true;
            if(menuObj.owner)
            {
                try
                {
                    enabled = menuObj.owner.OnEnableCommand(name);
                }
                catch(e) {}
            }
            if(state != enabled)
            {
                if(firefox)
                    obj.attributes["state"].nodeValue = enabled;
                else
                    obj.state = enabled;
                obj.style.color = enabled? "black": "#b4b4b4";
                var iconElt = document.getElementById("PMI" + name);
                if(iconElt)
                    iconElt.src = enabled ? (getAttributeNodeValue(obj,"icon")) : (getAttributeNodeValue(obj,"icond"));
            }
         }

        obj.onmouseover = onEnterOption;
        obj.onmouseout = onLeaveOption;
        obj.menuObj = menuObj;
    }

    menuObj.scrolled = 0;
    menuObj.scrolling = false;

    menuObj.scrollUpArea.onmousemove = onStartScrollingUp
    menuObj.scrollUpArea.onmouseout = onStopScrolling
    menuObj.scrollDownArea.onmousemove = onStartScrollingDown
    menuObj.scrollDownArea.onmouseout = onStopScrolling

    var w, h;
    if(msie)
    {
        w = document.body.clientWidth;
        h = document.body.clientHeight;
    }
    else
    {
        w = window.innerWidth - 2;
        h = window.innerHeight - 2;
    }

    //calculate menu width and x position
    var mw = menuObj.minSize;
    if(optimizeWidth)
    {
        var lenMax = getMaxOptionLen(menuObj) + 35;
        if(lenMax > mw)
            mw = lenMax;
    }

    menuObj.width = mw;
    var xPos;

    var mh = h - 5;

    if(menuObj.contentHeight > mh)
    {
        var lph = mh - 2 * menuObj.arrowHeight;
        menuObj.scrollAmount = menuObj.contentHeight - lph;
        menuObj.listDiv.style.top = (menuObj.arrowHeight + 1) + "px";
        menuObj.listDiv.style.height = lph + "px";
        menuObj.menuDiv.style.height = mh + "px";
        menuObj.scrollUpArea.style.top = "0px";
        menuObj.scrollUpArea.style.height = menuObj.arrowHeight + "px";
        menuObj.scrollDownArea.style.top = (mh - menuObj.arrowHeight - 1) + "px";
        menuObj.scrollDownArea.style.height = menuObj.arrowHeight + "px";
        menuObj.listDiv.style.clip = "rect(0px " + mw + "px " + lph + "px 0px)";
        menuObj.innerListDiv.style.position = "absolute";
        menuObj.innerListDiv.style.width = mw + "px";
        enableArrow(menuObj, false, true);
        enableArrow(menuObj, true, false);
    }
    else
    {
       menuObj.scrollAmount = 0;
       mh = menuObj.contentHeight;
       menuObj.scrollUpArea.style.visibility = "hidden";
       menuObj.scrollDownArea.style.visibility = "hidden";
       menuObj.scrollUpArrow.style.visibility = "hidden";
       menuObj.scrollDownArrow.style.visibility = "hidden";
       menuObj.menuDiv.style.height = mh + "px";
    }

    if(menuObj.nesting > 0)
    {
        var parentMenu = menuObj.parentMenu;
        if(menuObj.openRight = parentMenu.openRight)
        {
            x = parentMenu.left + parentMenu.width - 4;
            if(x + mw > w)
            {
                menuObj.openRight = false;
                x = parentMenu.left - mw + 4;
            }
        }
        else
        {
            x = parentMenu.left - mw + 4;
            if(x < 0)
            {
                menuObj.openRight = true;
                x = parentMenu.left + parentMenu.width - 4;
            }
        }
        y = parentMenu.mouseY - 10;
    }

    if(x == -1)
        xPos = w - mw;
    else
    {
        xPos = x;
        if(xPos + mw > w)
            xPos = w - mw;
        else
            if(xPos < 0)
                xPos = 0;
    }

    menuObj.left = xPos;
    menuObj.menuDiv.style.width = mw + "px";

    var yPos = y + mh >= h? h - mh - 1: y;
    menuObj.top = yPos;
    menuObj.menuDiv.style.left = xPos + "px";
    menuObj.menuDiv.style.visibility = "visible";
    menuObj.menuDiv.style.top = yPos + "px";

    if(menuObj.withIFrame)
    {
        var iframe = document.getElementById("IFr" + menuObj.menuName);
        iframe.style.left = menuObj.menuDiv.style.left;
        iframe.style.top = yPos + "px";
        iframe.style.width = menuObj.menuDiv.style.width;
        iframe.style.height = (msie? mh: mh+1) + "px";
        iframe.style.zIndex = menuObj.nesting * 2 + 100;
        iframe.style.visibility = "visible";
    }

    menuObj.menuDiv.style.zIndex = menuObj.nesting * 2 + 101;

   if(menuObj.nesting == 0 && menuObj.owner)
   {
        try
        {
            menuObj.owner.OnMenuShown(true);
        }
        catch(e) {}
   }
}

function hidePopup(menuObj)
{
    if(menuObj == null)
        return;

    if(menuObj.currChildMenu)
    {
        hidePopup(menuObj.currChildMenu);
        menuObj.currChildMenu = null;
    }
    if(menuObj.menuDiv != null)
        menuObj.menuDiv.style.visibility = "hidden";
    if(menuObj.withIFrame)
        document.getElementById("IFr" + menuObj.menuName).style.visibility = "hidden";
    clearInterval(menuObj.scrollTimer);
    if(menuObj.nesting == 0 && menuObj.owner)
    {
        try
        {
            menuObj.owner.OnMenuShown(false);
        }
        catch(e) {}
    }
}

function isPopupOpen(menuObj)
{
    return menuObj && menuObj.menuDiv && menuObj.menuDiv.style.visibility == "visible";
}
