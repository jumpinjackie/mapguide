var agent = navigator.userAgent.toLowerCase();
var msie = agent.indexOf("msie") != -1;
var firefox = agent.indexOf("firefox") != -1;

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
