/*
 * Get the Xpath value
 */
Selenium.prototype.getXmlElement = function(xpath) {
    var currentDocument = selenium.browserbot.getCurrentWindow().document
    if (currentDocument instanceof XMLDocument) {
        var result = selenium.browserbot.locateElementByXPath(xpath,currentDocument);
        return result;
    }
};
