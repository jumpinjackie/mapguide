function SetActionTarget()
{
    var splitUrl = document.URL.split('/');
    document.input.action = splitUrl[0]+"//"+splitUrl[2]+"/mapguide2008/mapagent/mapagent.fcgi";
}
