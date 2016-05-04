module.exports = {
    setDefaultBrowserSize: function(browser) {
        browser.resizeWindow(1280, 800);
    },
    screenshotName: function(browser, test) {
        return "./screenshots/" + process.platform + "_" + process.arch + "_" + browser.options.desiredCapabilities.browserName + "/" + test + ".png";
    }
};