#!/bin/bash

# smoke_test.sh
#
# Performs basic smoke testing of a Linux installation of MapGuide
#
#  1. Checks that the mgserver process is running
#  2. Checks that the httpd process is running
#  3. Checks that the mapagent is reachable and records the result of a GETSITEINFO request made with curl
#  4. Checks that the a fair number of FDO providers are functional with a GETFEATUREPROVIDERS request made with curl
#  5. Checks that the Sheboygan dataset can be loaded via the mapagent with the APPLYRESOURCEPACKAGE request made with curl
#  6. Checks that the Sheboygan data is indeed loaded by fetching for various resource contents with curl
#  7. Checks that we can walk schemas by issuing a series of DESCRIBESCHEMA calls
#  8. Checks that the AJAX viewer URL returns HTTP 200
#  9. Checks that the Fusion viewer URL with the 5 templates all return HTTP 200

pass=0
fail=0
http_port=8008
url_root="http://localhost:$http_port/mapguide"
mapagent_url="$url_root/mapagent/mapagent.fcgi"
ajaxviewer_url="$url_root/mapviewerajax/"
fusion_root="$url_root/fusion/templates/mapguide"
package_path=/tmp/Sheboygan.mgp

fsids=("Data/BuildingOutlines.FeatureSource" "Data/CityLimits.FeatureSource" "Data/HydrographicLines.FeatureSource" "Data/HydrographicPolygons.FeatureSource" "Data/Islands.FeatureSource" "Data/LandUse.FeatureSource" "Data/Parcels.FeatureSource" "Data/Rail.FeatureSource" "Data/RoadCenterLines.FeatureSource" "Data/Soils.FeatureSource" "Data/Trees.FeatureSource" "Data/VotingDistricts.FeatureSource")
basic_layout="Library://Samples/Sheboygan/Layouts/SheboyganPhp.WebLayout"
flex_layout="Library://Samples/Sheboygan/Layouts/Slate.ApplicationDefinition"

check_mgserver()
{
	echo "[check]: mgserver is running"
	if [ "$(pidof mgserver)" ]
	then
		pass=$((pass + 1))
		echo "    [check - PASS]: mgserver found"
	else
		fail=$((fail + 1))
		echo "    [check - FAIL]: mgserver not found"
	fi
}

check_httpd()
{
	echo "[check]: httpd is running"
	if [ "$(pidof httpd)" ]
	then
		pass=$((pass + 1))
		echo "    [check - PASS]: httpd found"
	else
		fail=$((fail + 1))
		echo "    [check - FAIL]: httpd not found"
	fi
}

assert_request()
{
	request_method=$1
	request_url=$2
	test_name=$3
	output_file=$4
	expected_status=$5
	username=$6
	password=$7
	if [ -z "$username" ]
	then
		#echo "[curl] request without credentials"
		curl -s -i -X $request_method "$request_url" -o $output_file
	else
		#echo "[curl] request with credentials"
		curl -u $username:$password -s -i -X $request_method "$request_url" -o $output_file
	fi
	status=`cat $output_file | grep HTTP/1.1 | awk {'print $2'}`
	if [ "$status" != "$expected_status" ]
	then
		fail=$((fail + 1))
		echo "    [check - FAIL]: Expected HTTP status of $expected_status. Check $output_file for more information"
	else
		pass=$((pass + 1))
		echo "    [check - PASS]: $test_name"
	fi
}

assert_package_load()
{
	request_method="POST"
	request_url="$mapagent_url"
	expected_status=$1
	test_name=$2
	output_file=$3
	username=$4
	password=$5
	if [ -z "$username" ]
	then
		#echo "[curl] request without credentials"
		curl -s -i -X $request_method -d 'MAX_FILE_SIZE=100000000' -d 'OPERATION=APPLYRESOURCEPACKAGE' -d 'VERSION=1.0.0' -d 'LOCALE=en' -d PACKAGE=$package_path "$request_url" -o $output_file
	else
		#echo "[curl] request with credentials"
		curl -u $username:$password -s -i -X $request_method -d 'MAX_FILE_SIZE=100000000' -d 'OPERATION=APPLYRESOURCEPACKAGE' -d 'VERSION=1.0.0' -d 'LOCALE=en' -d PACKAGE=$package_path "$request_url" -o $output_file
	fi	
	status=`cat $output_file | grep HTTP/1.1 | awk {'print $2'}`
	if [ "$status" != "$expected_status" ]
	then
		fail=$((fail + 1))
		echo "    [check - FAIL]: Expected HTTP status of $expected_status. Check $output_file for more information"
	else
		pass=$((pass + 1))
		echo "    [check - PASS]: $test_name"
	fi
}

check_createsession()
{
	echo "[check]: Create Session"
	assert_request "POST" "$mapagent_url?OPERATION=CREATESESSION&VERSION=1.0.0&LOCALE=en" "CREATESESSION un-authenticated" "unauthenticated.txt" "401"
	assert_request "POST" "$mapagent_url?OPERATION=CREATESESSION&VERSION=1.0.0&LOCALE=en" "CREATESESSION admin" "CREATESESSION_admin.txt" "200" "Administrator" "admin"
	assert_request "POST" "$mapagent_url?OPERATION=CREATESESSION&VERSION=1.0.0&LOCALE=en" "CREATESESSION anon" "CREATESESSION_anon.txt" "200" "Anonymous"
}

check_siteversion()
{
	echo "[check]: Get MapGuide version from mapagent"
	assert_request "GET" "$mapagent_url?OPERATION=GETSITEINFO&VERSION=1.0.0&LOCALE=en" "GETSITEINFO admin" "GETSITEINFO_admin.txt" "200" "Administrator" "admin"
	assert_request "GET" "$mapagent_url?OPERATION=GETSITEINFO&VERSION=1.0.0&LOCALE=en" "GETSITEINFO anon" "GETSITEINFO_anon.txt" "200" "Anonymous"
	assert_request "GET" "$mapagent_url?OPERATION=GETSITEINFO&VERSION=1.0.0&LOCALE=en" "GETSITEINFO un-authenticated" "unauthenticated.txt" "401"
	assert_request "GET" "$mapagent_url?OPERATION=GETSITEINFO&VERSION=1.0.0&LOCALE=en" "GETSITEINFO bad credentials" "unauthenticated.txt" "401" "foo" "bar"
}

check_featureproviders()
{
	echo "[check]: Get FDO Feature Provider List"
	assert_request "GET" "$mapagent_url?OPERATION=GETFEATUREPROVIDERS&VERSION=1.0.0&LOCALE=en" "GETFEATUREPROVIDERS admin" "GETFEATUREPROVIDERS_admin.txt" "200" "Administrator" "admin"
	assert_request "GET" "$mapagent_url?OPERATION=GETFEATUREPROVIDERS&VERSION=1.0.0&LOCALE=en" "GETFEATUREPROVIDERS anon" "GETFEATUREPROVIDERS_anon.txt" "200" "Anonymous"
	assert_request "GET" "$mapagent_url?OPERATION=GETFEATUREPROVIDERS&VERSION=1.0.0&LOCALE=en" "GETFEATUREPROVIDERS un-authenticated" "unauthenticated.txt" "401"
	assert_request "GET" "$mapagent_url?OPERATION=GETFEATUREPROVIDERS&VERSION=1.0.0&LOCALE=en" "GETFEATUREPROVIDERS bad credentials" "unauthenticated.txt" "401" "foo" "bar"
}

check_loadpackage()
{
	echo "[check]: Load Package"
	assert_package_load "401" "APPLYRESOURCEPACKAGE un-authenticated" "unauthenticated.txt"
	assert_package_load "401" "APPLYRESOURCEPACKAGE anon un-authenticated" "APPLYRESOURCEPACKAGE_anon.txt" "Anonymous"
	assert_package_load "200" "APPLYRESOURCEPACKAGE admin" "APPLYRESOURCEPACKAGE_admin.txt" "Administrator" "admin"
}

check_postpackageload()
{
	assert_request "GET" "$mapagent_url?OPERATION=ENUMERATERESOURCES&VERSION=1.0.0&LOCALE=en&RESOURCEID=Library://&DEPTH=-1" "ENUMERATERESOURCES un-authenticated" "unauthenticated.txt" "401"
	assert_request "GET" "$mapagent_url?OPERATION=ENUMERATERESOURCES&VERSION=1.0.0&LOCALE=en&RESOURCEID=Library://&DEPTH=-1" "ENUMERATERESOURCES admin" "ENUMERATERESOURCES_admin.txt" "200" "Administrator" "admin"
	assert_request "GET" "$mapagent_url?OPERATION=ENUMERATERESOURCES&VERSION=1.0.0&LOCALE=en&RESOURCEID=Library://&DEPTH=-1" "ENUMERATERESOURCES anon" "ENUMERATERESOURCES_anon.txt" "200" "Anonymous"

	echo "[check]: Verify some resources exist after loading package"
	for r in "${fsids[@]}"
	do
		resid="Library://Samples/Sheboygan/$r"
		assert_request "GET" "$mapagent_url?OPERATION=GETRESOURCECONTENT&VERSION=1.0.0&LOCALE=en&RESOURCEID=$resid" "GETRESOURCECONTENT admin $resid" "GETRESOURCE_admin.txt" "200" "Administrator" "admin"
		assert_request "GET" "$mapagent_url?OPERATION=GETRESOURCECONTENT&VERSION=1.0.0&LOCALE=en&RESOURCEID=$resid" "GETRESOURCECONTENT anon $resid" "GETRESOURCE_anon.txt" "200" "Anonymous"
	done
}

check_schemawalk()
{
	echo "[check]: Check we can walk some feature sources"
	for r in "${fsids[@]}"
	do
		resid="Library://Samples/Sheboygan/$r"
		assert_request "GET" "$mapagent_url?OPERATION=DESCRIBEFEATURESCHEMA&VERSION=1.0.0&LOCALE=en&RESOURCEID=$resid" "DESCRIBEFEATURESCHEMA admin $resid" "DESCRIBEFEATURESCHEMA_admin.txt" "200" "Administrator" "admin"
		assert_request "GET" "$mapagent_url?OPERATION=DESCRIBEFEATURESCHEMA&VERSION=1.0.0&LOCALE=en&RESOURCEID=$resid" "DESCRIBEFEATURESCHEMA anon $resid" "DESCRIBEFEATURESCHEMA_anon.txt" "200" "Anonymous"
	done
}

check_ajaxviewer()
{
	echo "[check]: AJAX viewer"
	assert_request "GET" "$ajaxviewer_url?WEBLAYOUT=$basic_layout&USERNAME=Anonymous" "Load AJAX Viewer" "AJAXVIEWER_anon.txt" "200"
}

check_fusion()
{
	templates=("slate" "limegold" "aqua" "turquoiseyellow" "maroon")
	for template in "${templates[@]}"
	do
		echo "[check]: Fusion viewer $template"
		assert_request "GET" "$fusion_root/$template/index.html?ApplicationDefinition=$flex_layout" "Load Fusion template $template" "Fusion_$template.txt" "200"
	done
}

check_mgserver
check_httpd
check_createsession
check_siteversion
check_featureproviders
check_loadpackage
check_postpackageload
check_schemawalk
check_ajaxviewer
check_fusion

echo "Pass: $pass, Fail: $fail"
