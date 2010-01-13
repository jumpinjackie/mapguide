// Common utility functions

function encodeComponent(str) {
    op = /\(/g; cp = /\)/g;
    return encodeURIComponent(str).replace(op, "%28").replace(cp, "%29");
}

function ParseLocalizedFloat(floatString) {
    if (thousandSeparator.length > 0) {
        floatString = floatString.replace(thousandSeparator, "");
    }
    if (decimalSeparator != "." && decimalSeparator.length > 0) {
        floatString = floatString.replace(decimalSeparator, ".");
    }
    return parseFloat(floatString);
}
