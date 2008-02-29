/* this file is to be loaded/included after all other Jx files */

if (Jx.COMBINED_CSS) {
    //console.log('Jx.COMBINED_CSS');
    document.write('<style type="text/css">@import url('+Jx.baseURL+'lib/jx_combined.css);</style>');
} else {
    //console.log('no Jx.COMBINED_CSS');
    document.write('<style type="text/css">');
    for (var styleSheet in Jx.importRules) {
        var url = Jx.baseURL+styleSheet;
        document.write('@import url('+url+');');
    }
    document.write('</style>');
    document.write('<!--[if IE]><style type="text/css">');
    for (var styleSheet in Jx.importRulesIE) {
        var url = Jx.baseURL+styleSheet;
        document.write('@import url('+url+');');
    }
    document.write('</style><![endif]-->\n');
}