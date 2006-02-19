
use strict;
use warnings;

use Test::More tests => 28 ;
BEGIN { use_ok('Sleepycat::DbXml') };

{
    my $manager = new XmlManager() ;
    my $value = new XmlValue() ;
    ok $manager, "created XmlManager ok";

    my $results = $manager->createResults();

    ok $results, "created XmlResults" ;

    # Empty

    ok ! $results->hasNext(), "hasNext is false";
    ok ! $results->hasPrevious(), "hasPrevious is false";
    is   $results->size(), 0, "size is 0";

    ok ! $results->next($value), "next is false";
    ok ! $results->previous($value), "previous is false";

    # 1 value in set

    #my $v1 = new XmlValue("abc");
    #$results->add($v1);
    $results->add("xyz");

    ok   $results->hasNext(), "hasNext is true";
    ok ! $results->hasPrevious(), "hasPrevious is false";
    is   $results->size(), 1, "size is 1";

    $results->reset();
    ok $results->peek($value), "peek is true";
    is $value, 'xyz'; 
    ok $results->next($value), "next is true";
    is $value, 'xyz'; 
    ok $results->previous($value), "previous is true";

    $results->reset();
    ok ! $results->hasPrevious(), "hasPrevious is false";
    $results->add(new XmlValue("abc"));

    ok   $results->hasNext(), "hasNext is false";
    ok   ! $results->hasPrevious(), "hasPrevious is false";
    is   $results->size(), 2, "size is 2";

    ok $results->peek($value), "peek is true";
    is $value, 'xyz'; 
    ok $results->next($value), "next is true";
    is $value, 'xyz'; 
    ok $results->next($value), "next is true";
    is $value, 'abc'; 
    ok $results->previous($value), "previous is true";
    is $value, 'xyz'; 
}

