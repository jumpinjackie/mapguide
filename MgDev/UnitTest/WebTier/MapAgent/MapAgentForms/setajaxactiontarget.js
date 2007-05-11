function SetAjaxActionTarget()
{
    var splitUrl = document.URL.split('/');
    var url = splitUrl[0]+"//"+splitUrl[2];

    if (document.input.agent[0].checked)
    {
        document.input.action = url+"/mapguide2008/mapviewerphp/ajaxviewer.php";
    }
    else if (document.input.agent[1].checked)
    {
        document.input.action = url+"/mapguide2008/mapviewernet/ajaxviewer.aspx";
    }
    else if (document.input.agent[2].checked)
    {
        document.input.action = url+"/mapguide2008/mapviewerjava/ajaxviewer.jsp";
    }
}
