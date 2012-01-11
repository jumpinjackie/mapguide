var agent = navigator.userAgent.toLowerCase();
var firefox = agent.indexOf("firefox") != -1;
var opera = agent.indexOf("opera") != -1;

var safariIndex = agent.indexOf("safari");
var safari1or2 = false;
var safari3plus = false;
var safari = false;
if(safariIndex != -1)
{
    safari = true;
    var safariVersion = agent.substr(safariIndex + 7, 3);
    if(parseFloat(safariVersion) >= 500)
    {
        safari3plus = true;
    }
    else
    {
        safari1or2 = true;
    }
}

var msieIndex = agent.indexOf("msie");
var msie = false;
var msie7plus = false;
var msie6minus = false;
if(msieIndex != -1)
{
    msie = true;
    var msieVersion = agent.substr(msieIndex + 5, 1);
    if(parseFloat(msieVersion) >= 7)
    {
        msie7plus = true;
    }
    else
    {
        msie6minus = true;
    }
}
