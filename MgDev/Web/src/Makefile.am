AUTOMAKE_OPTIONS = foreign
SUBDIRS = WebApp HttpHandler FastCgiAgent PhpApi JavaApi
DIST_SUBDIRS = WebApp HttpHandler FastCgiAgent PhpApi JavaApi
EXTRA_DIST = MapGuideApi DotNetApi DotNetUnmanagedApi mapviewernet mapviewerjava mapviewerphp mapadmin mapagent stdicons viewerfiles WebSupport WebTier.sln webconfig.ini WEB-INF localized schemareport webstudio PostBuild

prefix=@prefix@/webserverextensions/www

mapagentdir = ${prefix}/mapagent
mapadmindir = ${prefix}/mapadmin
mapviewerphpdir = ${prefix}/mapviewerphp
mapviewerjavadir = ${prefix}/mapviewerjava
stdiconsdir = ${prefix}/stdicons
localizeddir = ${prefix}/localized
schemareportdir = ${prefix}/schemareport
viewerfilesdir = ${prefix}/viewerfiles
resourcesdir = ${prefix}/Resources
tempdir = ${prefix}/TempDir
webinfdir = ${prefix}/WEB-INF

install-data-local:
	test -d $(mapagentdir) || $(mkinstalldirs) $(mapagentdir)
	test -d $(mapadmindir) || $(mkinstalldirs) $(mapadmindir)
	test -d $(mapviewerphpdir) || $(mkinstalldirs) $(mapviewerphpdir)
	test -d $(mapviewerjavadir) || $(mkinstalldirs) $(mapviewerjavadir)
	test -d $(resourcesdir) || $(mkinstalldirs) $(resourcesdir)
	test -d $(tempdir) || $(mkinstalldirs) $(tempdir)
	test -d $(stdiconsdir) || $(mkinstalldirs) $(stdiconsdir)
	test -d $(localizeddir) || $(mkinstalldirs) $(localizeddir)
	test -d $(schemareportdir) || $(mkinstalldirs) $(schemareportdir)
	test -d $(viewerfilesdir) || $(mkinstalldirs) $(viewerfilesdir)
	test -d $(webinfdir) || $(mkinstalldirs) $(webinfdir)
	@cp -rfp mapadmin/* $(mapadmindir)
	@cp -fp PhpApi/constants.php $(mapadmindir)
	@cp -fp ../../Common/MapGuideCommon/Resources/mapguide_en.res $(resourcesdir)
	@cp -rfp mapviewerphp/* $(mapviewerphpdir)
	@cp -fp PhpApi/constants.php $(mapviewerphpdir)
	@find mapviewerjava -maxdepth 1 -type f -exec cp -fp {} $(mapviewerjavadir) \;
	@cp -rfp localized/* $(localizeddir)
	@cp -fp stdicons/* $(stdiconsdir)
	@cp -rfp schemareport/* $(schemareportdir)
	@cp -fp viewerfiles/* $(viewerfilesdir)
	@cp -rfp WEB-INF/* $(webinfdir)
	@pushd $(mapagentdir); ln -s ../../bin/mapagent mapagent; popd;
	@cp -fp ../../UnitTest/WebTier/MapAgent/MapAgentForms/* $(mapagentdir)
	@chmod 777 $(tempdir)

install-data-hook:
	@sed -e 's#^ResourcesPath.*=.*#ResourcesPath                               = ${prefix}/Resources/#' \
             -e 's#^TempPath.*=.*#TempPath                                    = ${prefix}/TempDir/#' \
           webconfig.ini > ${prefix}/webconfig.ini
