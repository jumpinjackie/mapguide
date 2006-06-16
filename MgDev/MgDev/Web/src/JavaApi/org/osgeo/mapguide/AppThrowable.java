package org.osgeo.mapguide;

class AppThrowable extends Exception
{
    public AppThrowable()
    {
    }

    public AppThrowable(long cPtr, boolean cMemoryOwn)
    {
    }

    public String getMessage()
    {
        try
        {
            return ((MgException)this).GetMessage();
        }
        catch (MgException e)
        {
            return "";
        }
    }

    public void printStackTrace()
    {
        try
        {
            System.out.println(((MgException)this).GetStackTrace());
        }
        catch (MgException e)
        {
        }
    }

    public void delete()
    {
        //implemented by derived classes
    }
}
