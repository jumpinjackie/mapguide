--TEST--
date_sunrise() and date_sunset() functions
--INI--
precision=13
--FILE--
<?php

putenv ("TZ=Asia/Jerusalem");

for($a=1;$a<=12;$a++){
	echo date_sunrise(mktime(1,1,1,$a,1,2003),SUNFUNCS_RET_TIMESTAMP,31.76670,35.23330,90.83,2)." ";
	echo date_sunrise(mktime(1,1,1,$a,1,2003),SUNFUNCS_RET_STRING,31.76670,35.23330,90.83,2)." ";
	echo date_sunrise(mktime(1,1,1,$a,1,2003),SUNFUNCS_RET_DOUBLE,31.76670,35.23330,90.83,2)."\n";
	
	echo date_sunset(mktime(1,1,1,$a,1,2003),SUNFUNCS_RET_TIMESTAMP,31.76670,35.23330,90.83,2)." ";
	echo date_sunset(mktime(1,1,1,$a,1,2003),SUNFUNCS_RET_STRING,31.76670,35.23330,90.83,2)." ";
	echo date_sunset(mktime(1,1,1,$a,1,2003),SUNFUNCS_RET_DOUBLE,31.76670,35.23330,90.83,2)."\n";
}
?>
--EXPECTF--
1041395864 06:37 6.6290131458%d
1041432452 16:47 16.792451114%d
1044073855 06:30 6.5154089279%d
1044112463 17:14 17.239870289%d
1046491495 06:04 6.0822145033%d
1046533075 17:37 17.632011035%d
1049167581 05:26 5.4394438111%d
1049212774 17:59 17.993035729%d
1051757532 04:52 4.8701934126%d
1051806007 18:20 18.335390508%d
1054434776 04:32 4.5489827182%d
1054485647 18:40 18.679812949%d
1057026949 04:35 4.5971956372%d
1057078197 18:49 18.832563396%d
1059706409 04:53 4.8916575089%d
1059755837 18:37 18.621440704%d
1062385999 05:13 5.2220951121%d
1062432291 18:04 18.080957168%d
1064979098 05:31 5.5273199215%d
1065021952 17:25 17.431339135%d
1067658845 05:54 5.9016292870%d
1067698274 16:51 16.853902453%d
1070252387 06:19 6.3299242689%d
1070289382 16:36 16.606312600%d
