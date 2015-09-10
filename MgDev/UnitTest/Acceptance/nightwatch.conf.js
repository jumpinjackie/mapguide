module.exports = (function(settings) {
    if (process.env.MG_PARALLEL_TEST == "1") {
        settings.test_workers = {
            enabled: true,
            workers: "auto"
        }
    }
    if (process.platform == "win32") {
        settings.selenium.cli_args["webdriver.chrome.driver"] = "./node_modules/chromedriver/lib/chromedriver/chromedriver.exe";
        settings.selenium.cli_args["webdriver.ie.driver"] = "./node_modules/iedriver/lib/iedriver/IEDriverServer.exe";
    } else {
        throw "Testing on non-Windows platforms not supported yet";
    }
    return settings;
})(require('./nightwatch.json'));