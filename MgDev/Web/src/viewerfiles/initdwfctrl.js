function CreateDWFControl()
{
    var d = document.getElementById("DWF");
    d.innerHTML = '<object id="DWFViewer"\nclassid="clsid:A662DA7E-CCB7-4743-B71A-D817F6D575DF"\nborder=0\nwidth=100%\nheight=100%\nstyle="position: absolute; left: -1px; top: 0px;" >\n' +
                  '  <param name="Src" value="' + Src + '">\n' +
                  '  <param name="ToolbarVisible" value="False">\n' +
                  '  <param name="NavpaneWidth" value="' + NavpaneWidth + '">\n' +
                  '  <param name="NavpaneVisible" value="' + NavpaneVisible + '">\n' +
                  '  <param name="LayersBandVisible" value="' + LayersBandVisible + '">\n' +
                  '  <param name="PropertiesBandVisible" value="' + PropertiesBandVisible + '">\n' +
                  '  <param name="HyperlinkTarget" value="' + HyperlinkTarget + '">\n' +
                  '  <param name="ViewerParams" value="?PreferredDefaultInterface=\'{69BD6A6D-F788-4A34-BAE9-76AB5F69EF9D}">\n' +
                  '</object>';
    InitDocument();
}
