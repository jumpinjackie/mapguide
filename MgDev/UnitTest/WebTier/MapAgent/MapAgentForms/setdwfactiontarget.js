function SetDwfActionTarget()
{
    var splitUrl = document.URL.split('/');
    var url = splitUrl[0]+"//"+splitUrl[2]+"/"+splitUrl[3];

    if (document.input.agent[0].checked)
    {
        document.input.action = url+"/mapviewerphp/dwfviewer.php";
    }
    else if (document.input.agent[1].checked)
    {
        document.input.action = url+"/mapviewernet/dwfviewer.aspx";
    }
    else if (document.input.agent[2].checked)
    {
        document.input.action = url+"/mapviewerjava/dwfviewer.jsp";
    }
}
