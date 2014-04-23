function SetActionTarget()
{
    var splitUrl = document.URL.split('/');
    document.input.action = splitUrl[0]+"//"+splitUrl[2]+"/"+splitUrl[3]+"/mapagent/mapagent.fcgi";
}
