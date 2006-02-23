function SetDwfActionTarget()
{
    var splitUrl = document.URL.split('/');
    var url = splitUrl[0]+"//"+splitUrl[2];

    if (document.input.agent[0].checked)
    {
        document.input.action = url+"/mapguide/mapviewerphp/dwfviewer.php";
    }
    else if (document.input.agent[1].checked)
    {
        document.input.action = url+"/mapguide/mapviewernet/dwfviewer.aspx";
    }
    else if (document.input.agent[2].checked)
    {
        document.input.action = url+"/mapguide/mapviewerjava/dwfviewer.jsp";
    }
}
