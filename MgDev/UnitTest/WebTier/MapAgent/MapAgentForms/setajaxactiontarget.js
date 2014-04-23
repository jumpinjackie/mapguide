function SetAjaxActionTarget()
{
    var splitUrl = document.URL.split('/');
    var url = splitUrl[0]+"//"+splitUrl[2]+"/"+splitUrl[3];

    if (document.input.agent[0].checked)
    {
        document.input.action = url+"/mapviewerphp/ajaxviewer.php";
    }
    else if (document.input.agent[1].checked)
    {
        document.input.action = url+"/mapviewernet/ajaxviewer.aspx";
    }
    else if (document.input.agent[2].checked)
    {
        document.input.action = url+"/mapviewerjava/ajaxviewer.jsp";
    }
}
