function SetAjaxActionTarget()
{
    var splitUrl = document.URL.split('/');
    var url = splitUrl[0]+"//"+splitUrl[2];

    if (document.input.agent[0].checked)
    {
        document.input.action = url+"/mapguide/mapviewerphp/ajaxviewer.php";
    }
    else if (document.input.agent[1].checked)
    {
        document.input.action = url+"/mapguide/mapviewernet/ajaxviewer.aspx";
    }
    else if (document.input.agent[2].checked)
    {
        document.input.action = url+"/mapguide/mapviewerjava/ajaxviewer.jsp";
    }
}
