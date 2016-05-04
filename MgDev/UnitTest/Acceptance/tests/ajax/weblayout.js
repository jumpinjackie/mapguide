//This is the command layout of Library://Samples/Sheboygan/Layouts/SheboyganPhp.WebLayout

module.exports = {
    toolbar: {
        BUFFER: 21,
        MEASURE: 24,
        ZOOM_RECT: 8,
        ZOOM_IN: 6,
        ZOOM_OUT: 7,
        SELECT: 14,
        PAN: 0
    },
    getToolbarCommand: function(cmdCode) {
        return "//span[contains(@onclick, 'ExecuteCommand(" + cmdCode + "')]";
    }
};