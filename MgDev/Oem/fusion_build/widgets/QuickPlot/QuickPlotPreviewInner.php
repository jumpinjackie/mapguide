<?php

    $fusionMGpath = '../../layers/MapGuide/php/';
    include $fusionMGpath . 'Common.php';
    
    $locale = GetDefaultLocale();
    $scaleDenominator;
    $annotations;
    
    $args = GetRequestMethod();
    GetParameters($args);

    $templ = file_get_contents("QuickPlotPreviewInner.templ");
    SetLocalizedFilesPath(GetLocalizationPath());
    
    // Localize the page
    $templ = Localize($templ, $locale, GetClientOS());
    
    // Set some annotation labels in the page by replacing the placeholders in the html code
    $templ = str_replace(array_keys($annotations), array_values($annotations), $templ);
    // Set the date annotation according to its format mask
    $pattern = "/\{date:(.+?)\}/";
    $matches = array();
    if (preg_match($pattern, $templ, $matches))
    {
        $mask  = $matches[1];
        $date  = date($mask);
        $templ = preg_replace($pattern, $date, $templ);
    }
    
    $jsPath = "";
    print sprintf($templ, $jsPath);
?>

<?php
function GetParameters($params)
{
    global $scaleDenominator, $annotations;
    
    $scaleDenominator = intval($params["scale_denominator"]);
    $annotations = array();
    
    // The parameters whose name matches this pattern will be treated as annotation
    $pattern = "/^\{field:.+\}$/i";
    foreach ($params as $key => $value)
    {
        if (preg_match($pattern, $key) == 1)
        {
            $annotations[$key] = htmlspecialchars(urlDecode($value), ENT_QUOTES);
        }
    }
    
    // The scale annotation
    $annotations["{scale}"] = "1 : " . $scaleDenominator;
}

function GetRequestMethod()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
    {
        return $_POST;
    }
    else
    {
        return $_GET;
    }
}

?>
