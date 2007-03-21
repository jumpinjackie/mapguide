--TEST--
show information about extension
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`$php -n --re unknown`);
var_dump(`$php -n --re ""`);
var_dump(`$php -n --re date`);

echo "Done\n";
?>
--EXPECTF--	
string(44) "Exception: Extension unknown does not exist
"
string(37) "Exception: Extension  does not exist
"
string(%d) "Extension [ <persistent> extension #%d date version %s ] {

  - Dependencies {
    Dependency [ session (Optional) ]
  }

  - INI {
    Entry [ date.timezone <ALL> ]
      Current = ''
    }
    Entry [ date.default_latitude <ALL> ]
      Current = '%s'
    }
    Entry [ date.default_longitude <ALL> ]
      Current = '%s'
    }
    Entry [ date.sunset_zenith <ALL> ]
      Current = '%s'
    }
    Entry [ date.sunrise_zenith <ALL> ]
      Current = '%s'
    }
  }

  - Constants [14] {
    Constant [ string DATE_ATOM ] { Y-m-d\TH:i:sP }
    Constant [ string DATE_COOKIE ] { l, d-M-y H:i:s T }
    Constant [ string DATE_ISO8601 ] { Y-m-d\TH:i:sO }
    Constant [ string DATE_RFC822 ] { D, d M y H:i:s O }
    Constant [ string DATE_RFC850 ] { l, d-M-y H:i:s T }
    Constant [ string DATE_RFC1036 ] { D, d M y H:i:s O }
    Constant [ string DATE_RFC1123 ] { D, d M Y H:i:s O }
    Constant [ string DATE_RFC2822 ] { D, d M Y H:i:s O }
    Constant [ string DATE_RFC3339 ] { Y-m-d\TH:i:sP }
    Constant [ string DATE_RSS ] { D, d M Y H:i:s O }
    Constant [ string DATE_W3C ] { Y-m-d\TH:i:sP }
    Constant [ integer SUNFUNCS_RET_TIMESTAMP ] { 0 }
    Constant [ integer SUNFUNCS_RET_STRING ] { 1 }
    Constant [ integer SUNFUNCS_RET_DOUBLE ] { 2 }
  }

  - Functions {
    Function [ <internal> public function strtotime ] {

      - Parameters [2] {
        Parameter #0 [ <required> $time ]
        Parameter #1 [ <optional> $now ]
      }
    }
    Function [ <internal> public function date ] {

      - Parameters [2] {
        Parameter #0 [ <required> $format ]
        Parameter #1 [ <optional> $timestamp ]
      }
    }
    Function [ <internal> public function idate ] {

      - Parameters [2] {
        Parameter #0 [ <required> $format ]
        Parameter #1 [ <optional> $timestamp ]
      }
    }
    Function [ <internal> public function gmdate ] {

      - Parameters [2] {
        Parameter #0 [ <required> $format ]
        Parameter #1 [ <optional> $timestamp ]
      }
    }
    Function [ <internal> public function mktime ] {

      - Parameters [6] {
        Parameter #0 [ <optional> $hour ]
        Parameter #1 [ <optional> $min ]
        Parameter #2 [ <optional> $sec ]
        Parameter #3 [ <optional> $mon ]
        Parameter #4 [ <optional> $day ]
        Parameter #5 [ <optional> $year ]
      }
    }
    Function [ <internal> public function gmmktime ] {

      - Parameters [6] {
        Parameter #0 [ <optional> $hour ]
        Parameter #1 [ <optional> $min ]
        Parameter #2 [ <optional> $sec ]
        Parameter #3 [ <optional> $mon ]
        Parameter #4 [ <optional> $day ]
        Parameter #5 [ <optional> $year ]
      }
    }
    Function [ <internal> public function checkdate ] {

      - Parameters [3] {
        Parameter #0 [ <required> $month ]
        Parameter #1 [ <required> $day ]
        Parameter #2 [ <required> $year ]
      }
    }
    Function [ <internal> public function strftime ] {

      - Parameters [2] {
        Parameter #0 [ <required> $format ]
        Parameter #1 [ <optional> $timestamp ]
      }
    }
    Function [ <internal> public function gmstrftime ] {

      - Parameters [2] {
        Parameter #0 [ <required> $format ]
        Parameter #1 [ <optional> $timestamp ]
      }
    }
    Function [ <internal> public function time ] {

      - Parameters [0] {
      }
    }
    Function [ <internal> public function localtime ] {

      - Parameters [2] {
        Parameter #0 [ <optional> $timestamp ]
        Parameter #1 [ <optional> $associative_array ]
      }
    }
    Function [ <internal> public function getdate ] {

      - Parameters [1] {
        Parameter #0 [ <optional> $timestamp ]
      }
    }
    Function [ <internal> public function date_create ] {
    }
    Function [ <internal> public function date_parse ] {
    }
    Function [ <internal> public function date_format ] {
    }
    Function [ <internal> public function date_modify ] {
    }
    Function [ <internal> public function date_timezone_get ] {
    }
    Function [ <internal> public function date_timezone_set ] {
    }
    Function [ <internal> public function date_offset_get ] {
    }
    Function [ <internal> public function date_time_set ] {
    }
    Function [ <internal> public function date_date_set ] {
    }
    Function [ <internal> public function date_isodate_set ] {
    }
    Function [ <internal> public function timezone_open ] {
    }
    Function [ <internal> public function timezone_name_get ] {
    }
    Function [ <internal> public function timezone_name_from_abbr ] {
    }
    Function [ <internal> public function timezone_offset_get ] {
    }
    Function [ <internal> public function timezone_transitions_get ] {
    }
    Function [ <internal> public function timezone_identifiers_list ] {
    }
    Function [ <internal> public function timezone_abbreviations_list ] {
    }
    Function [ <internal> public function date_default_timezone_set ] {

      - Parameters [1] {
        Parameter #0 [ <required> $timezone_identifier ]
      }
    }
    Function [ <internal> public function date_default_timezone_get ] {

      - Parameters [0] {
      }
    }
    Function [ <internal> public function date_sunrise ] {

      - Parameters [6] {
        Parameter #0 [ <required> $time ]
        Parameter #1 [ <optional> $format ]
        Parameter #2 [ <optional> $latitude ]
        Parameter #3 [ <optional> $longitude ]
        Parameter #4 [ <optional> $zenith ]
        Parameter #5 [ <optional> $gmt_offset ]
      }
    }
    Function [ <internal> public function date_sunset ] {

      - Parameters [6] {
        Parameter #0 [ <required> $time ]
        Parameter #1 [ <optional> $format ]
        Parameter #2 [ <optional> $latitude ]
        Parameter #3 [ <optional> $longitude ]
        Parameter #4 [ <optional> $zenith ]
        Parameter #5 [ <optional> $gmt_offset ]
      }
    }
    Function [ <internal> public function date_sun_info ] {

      - Parameters [3] {
        Parameter #0 [ <required> $time ]
        Parameter #1 [ <required> $latitude ]
        Parameter #2 [ <required> $longitude ]
      }
    }
  }

  - Classes [2] {
    Class [ <internal:date> class DateTime ] {

      - Constants [11] {
        Constant [ string ATOM ] { Y-m-d\TH:i:sP }
        Constant [ string COOKIE ] { l, d-M-y H:i:s T }
        Constant [ string ISO8601 ] { Y-m-d\TH:i:sO }
        Constant [ string RFC822 ] { D, d M y H:i:s O }
        Constant [ string RFC850 ] { l, d-M-y H:i:s T }
        Constant [ string RFC1036 ] { D, d M y H:i:s O }
        Constant [ string RFC1123 ] { D, d M Y H:i:s O }
        Constant [ string RFC2822 ] { D, d M Y H:i:s O }
        Constant [ string RFC3339 ] { Y-m-d\TH:i:sP }
        Constant [ string RSS ] { D, d M Y H:i:s O }
        Constant [ string W3C ] { Y-m-d\TH:i:sP }
      }

      - Static properties [0] {
      }

      - Static methods [0] {
      }

      - Properties [0] {
      }

      - Methods [9] {
        Method [ <internal, ctor> public method __construct ] {
        }

        Method [ <internal> public method format ] {
        }

        Method [ <internal> public method modify ] {
        }

        Method [ <internal> public method getTimezone ] {
        }

        Method [ <internal> public method setTimezone ] {
        }

        Method [ <internal> public method getOffset ] {
        }

        Method [ <internal> public method setTime ] {
        }

        Method [ <internal> public method setDate ] {
        }

        Method [ <internal> public method setISODate ] {
        }
      }
    }

    Class [ <internal:date> class DateTimeZone ] {

      - Constants [0] {
      }

      - Static properties [0] {
      }

      - Static methods [2] {
        Method [ <internal> static public method listAbbreviations ] {
        }

        Method [ <internal> static public method listIdentifiers ] {
        }
      }

      - Properties [0] {
      }

      - Methods [4] {
        Method [ <internal, ctor> public method __construct ] {
        }

        Method [ <internal> public method getName ] {
        }

        Method [ <internal> public method getOffset ] {
        }

        Method [ <internal> public method getTransitions ] {
        }
      }
    }
  }
}

"
Done
