built on a modified fastcgi 2.2.2 library

------------------------------------------------------------------------
Minimal Configuration to get working
------------------------------------------------------------------------

See the registry section below for details.  Also see the IIS or NSAPI configuration
information that must be done in addition to this.  To configure an interpreter
such as PHP, use the following registry entries in HKLM/SOFTWARE/:

FASTCGI/.php/
	AppPath c:\php\phpfcgi.exe
	BindPath php-fcgi

To get Perl, or some other fastcgi processor (TCL, etc.) working, use:

FASTCGI/.fpl/
	AppPath c:\perl\bin\perl.exe
FASTCGI/.ftcl/
	AppPath c:\tcl\tcsh.exe

To get a compiled executable working, such as the echo example in the
fastcgi distribution, use:

FASTCGI/echo.fcgi/
	AppPath c:\path\to\echo.exe

Under IIS, we cannot map the exe extension (IIS prevents that), so we
have to use some other extension (such as .fcgi) and map it internally
to .exe.


------------------------------------------------------------------------
configuration for IIS
------------------------------------------------------------------------
add to the application mappings extensions you want to be sent to the fastcgi dll.
For authentication to work properly in all instances, you MUST check the 
'Check that file exists' checkbox, this has the unfortunate
side effect of breaking path_info values.  To provide proper IIS security,
you should also utilize the impersonation features described below and at 
www.caraveo.com/fastcgi/


------------------------------------------------------------------------
configuration for NSAPI (iPlanet, Netscape)
------------------------------------------------------------------------
add to mime.types:
type=magnus-internal/x-fastcgi    exts=fphp

add to obj.conf
Init fn="load-modules" shlib="c:/path/to/nsapi_fcgi.dll" funcs="FCGIServiceHandler" shlib_flags="(global|now)"
--to the < Object name="default" >--
Service fn="FCGIServiceHandler" type="magnus-internal/x-fastcgi"
-- and finally --
<Object name="x-fastcgi">
ObjectType fn="force-type" type="magnus-internal/x-fastcgi"
Service fn=FCGIServiceHandler
</Object>


------------------------------------------------------------------------
configuring the process manager
------------------------------------------------------------------------
Most keys have defaults, and do not need to be set in the registry.  Requird keys
will be marked so.

create the registry keys:
HKEY_LOCAL_MACHINE\Software\FASTCGI
MaxPostData REG_DWORD 0
	(Byte limit for pre-reading the post data)
CustomVars REG_BINARY 0
	(newline deliminated, null terminated list of custom environment names,
	can be used to specify additional environment names that the web interface
	should look for in addition to the defaults)
ThreadPoolSize REG_DWORD 10 
	(IIS ONLY, 10 is default if not specified)
BypassAuth REG_DWORD 0 
	IIS ONLY, if 1 and IIS is configured to use isapi_fcgi.dll
	as a filter, and IIS is configured to use BASIC authentication,
	this will force all authentication requests to use the IIS anonymous
	user.  This in effect allows scripts to implement their own authentication
	mechanisms.
Impersonate REG_DWORD 1
	IIS ONLY, if 1, IIS security tokens will be used to impersonate the IIS
	authenticated (or anonymous) users.  This is on by default.  setting to
	zero dissables this feature.  Don't dissable it unless you are not worried
	about security.
StartServers REG_DWORD 5 
	Global value, used if no server specific setting
IncrementServers REG_DWORD 2
	Global value, used if no server specific setting
MaxServers REG_DWORD 25
	Global value, used if no server specific setting
Timeout REG_DWORD 600
	Global value, used if no server specific setting.  How long (seconds)
	extra servers (number above StartServers) are kept alive before 
	being terminated

create a key for each extension:
HKEY_LOCAL_MACHINE\Software\FASTCGI\.php    REQUIRED

inside that key, add the following values:

BindPath REG_SZ "php-fcgi" REQUIRED
AppPath REG_SZ "c:\php\phpfcgi.exe"
Args REG_SZ ""
StartServers REG_DWORD 5
IncrementServers REG_DWORD 2
MaxServers REG_DWORD 25
Environment REG_BINARY 
	(newline deliminated, null terminated, for adding additional
	environment variables ie. PHP_FCGI_MAX_REQUESTS=25)
Filter REG_DWORD 0
	If set to 1, this fastcgi server will be treated as a FastCGI FILTER.
	A Filter server receives the file to be filtered from the web server.
	for more information, see documentation at fastcgi.com
Timeout REG_DWORD 600
	How long extra servers (number above StartServers) are	kept alive 
	before being terminated

If the fast cgi application is started by an external process manager you
only need to use the BindPath.

You can set BindPath to a ip address/port if you need to use an interpreter 
on a different machine.

Multiple extensions can have the same BindPath, for instance, mapping 
.pl and .cgi to perl.exe.

If IncrementServers is not defined, it defaults to 1/2 StartServers.
If MaxServers is not defined, it defaults to 25.


How the process manager works
------------------------------------------------------------------------
It's fairly simple and needs to be further fleshed out.
At startup, it starts "StartServers" number of executables for each binding.
When running, if none are available, it starts "IncrementServers" more
executables, up to "MaxServers".

------------------------------------------------------------------------
building
------------------------------------------------------------------------
Download the devkit from www.fastcgi.com, and expand into the fastcgi directory
build the fastcgi development kit

------------------------------------------------------------------------
testing
------------------------------------------------------------------------
You can use the stress test application to test the isapi_fcgi.dll against the php test scripts.
stresstest -m /path/to/isapi_fcgi.dll -d /path/to/php4source -t 10

IIS debugging info at:
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vcmfc98/html/_mfcnotes_tn063.asp


