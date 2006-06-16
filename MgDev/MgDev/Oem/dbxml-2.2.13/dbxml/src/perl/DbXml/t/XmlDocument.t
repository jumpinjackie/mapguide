
use strict;
use warnings;

use Test::More tests => 36;

BEGIN { use_ok('Sleepycat::DbXml', 'simple') };
BEGIN { use_ok('File::Path') };

sub dumpException
{
    if (my $e = catch std::exception)
    {
        warn "caught exception ";
        warn $e->what() . "\n";
        exit( -1 );
    }
    elsif ($@)
    {
        warn "failed\n";
        warn $@;
        exit( -1 );
    }    
}


my $mgr = new XmlManager();
ok $mgr, "Created Manager";

{
    # setContent/getContent

    my $doc ;

    $doc = $mgr->createDocument(); 

    ok ! $@, "Creating document doesn't throw an exception" ;
    ok defined $doc, "Document object created ok" ;

    my $content = '<def />';
    $doc->setContent($content);

    my $cont = $doc->getContent();
    ok $cont eq $content, "getContent returned correct content" ;

    $doc->fetchAllData();
}

my $doc ;

eval { $doc = $mgr->createDocument(); };

ok ! $@, "Creating document doesn't throw an exception" ;
ok defined $doc, "Document object created ok" ;

my $content = '<abc />';
$doc->setContent($content);

my $cont = $doc->getContent();

ok $cont eq $content, "getContent returned correct content" ;

is $doc->getName(), '', "setName/getName works ok" ; 

$doc->setName("fred");

is $doc->getName(), 'fred', "setName/getName works ok" ; 

eval
{ 
    # getMetaData/setMetaData/removeMetaData

    #my $value1 = new XmlValue(XmlValue::DECIMAL, 42) ;
    my $value1 = new XmlValue(XmlValue::STRING, "Fred") ;
    #my $value2 = new XmlValue(XmlValue::DECIMAL, 43) ;
    my $value2 = 43 ;
    my $valueb = new XmlValue(XmlValue::BOOLEAN, 1) ;

    my $uri = 'http://fred.com/';
    my $name1 = 'universe' ;
    my $name2 = 'everything' ;
    my $name3 = 'else' ;

    my %names = map {$_ => 1 } $name1, $name2, $name3 ;

    $doc->setMetaData($uri, $name1, $value1) ;
    $doc->setMetaData($uri, $name2, $value2) ;
    $doc->setMetaData($uri, $name3, $valueb) ;

    {
        my $Uri;
        my $name;
        my $value = new XmlValue() ;
        #my $value ;
        my $iter = $doc->getMetaDataIterator();
        while ($iter->next($Uri, $name, $value))
        {
            print "# $Uri $name\n" ;
		}
        $iter->reset();
        while ($iter->next($Uri, $name, $value))
        {
            next if $name eq 'name';
            my $str = $value->isNull();
            #print "# $Uri $name $value $str\n" ;

            is $Uri, $uri, "URI is $uri";
            ok $names{$name}, "Matched '$name'"
        }

        $iter->reset();
    }

    my $typed = new XmlValue() ;
    my $untyped ;

    # try to fetch an unknown bit of meta data
    ok !$doc->getMetaData($uri."x", "xx who", $typed), "getMetaData returns FALSE" ;

    ok $doc->getMetaData($uri, $name1, $typed), "getMetaData into XmlValue returns TRUE" ;
    ok $typed->isString(), "getMetaData returned a number";
    my $got = $typed->asString();
    is $got, 'Fred', "getMeta returns 'Fred'";

    ok $doc->getMetaData($uri, $name1, $untyped), "getMetaData into Perl scalar returns TRUE" ;
    is $untyped, "Fred\0", "getMeta returns 'Fred'";
    #print "AOK\n" if $untyped == 42 ;

    ok $doc->getMetaData($uri, $name2, $untyped), "getMetaData into Perl scalar returns TRUE" ;
    is $untyped, "43\0", "getMeta returns 43";


    ok $doc->getMetaData($uri, $name2, $typed), "getMetaData into XmlValue returns TRUE" ;
    $got = $typed->asString();
    is $got, 43, "getMeta returns 43";

    ok $doc->getMetaData($uri, $name3, $typed), "getMetaData into XmlValue returns TRUE" ;
    ok $typed->isBoolean(), "getMetaData returned a Boolean";
    $got = $typed->asBoolean();
    ok $got, "getMeta returns 'true'";

    ok $doc->getMetaData($uri, $name3, $untyped), "getMetaData into Perl scalar returns TRUE" ;
    is $untyped, "true\0", "getMeta returns 'true'";

    # now remove one of the bits of meta-data

    $doc->removeMetaData($uri, $name2);
    ok ! $doc->getMetaData($uri, $name2, $typed), "getMetaData returns FALSE" ;
}
or dumpException();


{ 
    # getMetaData with NULL XmlValue should not throw exception

    my $uri = 'http://fred.com/';
    my $name = 'hi' ;
    my $value1 = new XmlValue() ;
    eval { $doc->getMetaData($uri, $name, $value1) ; };
    ok !$@, "getMetaData with NULL XmlValue should not throw exception";

}

{ 
    # setMetaData with NULL XmlValue should throw exception

    my $value1 = new XmlValue() ;

    my $uri = 'http://fred.com/';
    my $name1 = 'blank' ;

    eval { $doc->setMetaData($uri, $name1, $value1); } ;
    ok $@, "setMetaData with NULL XmlValue should throw exception";

}

{ 
    # setMetaData with NODE XmlValue should throw exception

    my $doc2 = $mgr->createDocument(); 
    my $value1 = new XmlValue(XmlValue::NODE, $doc2) ;

    my $uri = 'http://fred.com/';
    my $name1 = 'foo' ;

    eval { $doc->setMetaData($uri, $name1, $value1) } ;
    ok $@, "setMetaData with NODE XmlValue should throw exception";
}

__END__

{ 
    my $context = new XmlQueryContext() ;

    my $value1 = new XmlValue(XmlValue::DOCUMENT, "<x />") ;
    my $value2 = new XmlValue(XmlValue::DOCUMENT, "<y />") ;

    my $uri = 'http://fred.com/';
    my $name1 = 'foo' ;
    my $name2 = 'bar' ;

    $doc->setMetaData($uri, $prefix, $name1, $value1) ;
    $doc->setMetaData($uri, $prefix, $name2, $value2) ;

    #my $value1 = new XmlValue() ;
    my $value3 = new XmlValue(XmlValue::DOCUMENT, "<z />") ;

    ok $doc->getMetaData($uri, $name1, $value3), "getMetaData returns TRUE" ;
    ok $value3->isDocument($context), "getMetaData returned a Document";
    is $value3->asDocument()->getContentAsString(), "<x />";

    eval { $value3->asNumber($context); };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";


    ok $doc->getMetaData($uri, $name2, $value3), "getMetaData returns TRUE" ;
    ok $value3->isDocument($context), "getMetaData returned a Document";
    is $value3->asDocument()->getContentAsString(), "<y />";



}

{
    # XmlDocument::queryWithXPath

    my $doc = new XmlDocument(); 
    my $title = '<title>Knowledge Discovery in Databases.</title>';
    my $content = "<book>$title</book>";
    $doc->setContent($content);

    my $results ;
    eval { $results = $doc->queryWithXPath('/book/title') } ;
    ok !$@, "queryWithXPath returned 0";

    my $value = new XmlValue ;
    ok $results->next($value), "results.next returns true" ;
    ok !$value->isNull(), "results->next not null";
    ok !$value->isDocument(), "results->next is not document";
    ok !$value->isString(), "results->next is not string";
    ok $value->isNode(), "results->next is node";
    is $value->asString(), $title, "nodelist contents ok";

    ok ! $results->next($value), "results.next returns false";
    ok $value->isNull(), "results->next is null";

}

{
    # XmlDocument::queryWithXPath using XmlQueryExpression

    my $tmp_dir = "tmp" ;

    if (-d $tmp_dir)
    {
        rmtree($tmp_dir);
    }

    mkdir $tmp_dir, 0777 ;


    my $container_name = "$tmp_dir/fred1" ;
    my $x = new XmlContainer($container_name); 
    ok $x, "new XmlContainer returned object" ;

    eval { $x->open(Db::DB_CREATE); } ;
    if (my $E = XmlException::catch())
    {
        ok 0, "open should not throw an exception";
        print "# " . $E->what() . "\n" ;
        exit;
    }
    else
    {
        ok 1, "open should not throw an exception";
    }

    my $context = new XmlQueryContext(XmlQueryContext::ResultValues) ;
    my $path = '/book/title';
    my $expression;
    eval { $expression = $x->parseXPathExpression($path, $context) } ;
    ok !$@, "parseXPathExpression ok" ;
    print "$@\n";


    my $doc = new XmlDocument(); 
    my $title = '<title>Knowledge Discovery in Databases.</title>';
    my $content = "<book>$title</book>";
    $doc->setContent($content);

    my $results ;
    eval { $results = $doc->queryWithXPath('/book/title') } ;
    ok !$@, "queryWithXPath returned 0";

    my $value = new XmlValue ;
    ok $results->next($value), "results.next returns true" ;
    ok !$value->isNull(), "results->next not null";
    ok !$value->isDocument(), "results->next is not document";
    ok !$value->isString(), "results->next is not string";
    ok $value->isNode(), "results->next is node";
    is $value->asString(), $title, "nodelist contents ok";

    $x->close(); 
    $x->remove();     ok ! $results->next($value), "results.next returns false";
    ok $value->isNull(), "results->next is null";


}

{
    # XmlDocument::queryWithXPath - destroy document before calling results.next

    my $doc = new XmlDocument(); 
    my $title = '<title>Knowledge Discovery in Databases.</title>';
    my $content = "<book>$title</book>";
    $doc->setContent($content);

    my $results ;
    eval { $results = $doc->queryWithXPath('/book/title') } ;
    ok !$@, "queryWithXPath returned 0";

    undef $doc ;

    my $value = new XmlValue ;
    ok $results->next($value), "results.next returns true" ;
    ok !$value->isNull(), "results->next not null";
    ok !$value->isDocument(), "results->next is not document";
    ok !$value->isString(), "results->next is not string";
    ok $value->isNode(), "results->next is node";
    is $value->asString(), $title, "nodelist contents ok";

    ok ! $results->next($value), "results.next returns false";
    ok $value->isNull(), "results->next is null";

}

{
    # XmlDocument::queryWithXPath -- ResultDocumentsAndValues

    my $doc = new XmlDocument(); 
    my $title = '<title>Knowledge Discovery in Databases.</title>';
    my $content = "<book>$title</book>";
    $doc->setContent($content);

    my $context = new XmlQueryContext() ;
    $context->setReturnType(XmlQueryContext::ResultDocumentsAndValues);

    my $results ;
    eval { $results = $doc->queryWithXPath('/book/title', $context) } ;
    ok !$@, "queryWithXPath returned 0";

    my $doc1 = new XmlDocument ;
    my $value = new XmlValue ;
    ok $results->next($doc1, $value), "results.next returns true" ;

    is $doc->getID(), $doc1->getID(), "returned document matches";

    ok !$value->isNull(), "results->next not null";
    ok !$value->isDocument(), "results->next is not document";
    ok !$value->isString(), "results->next is not string";
    ok $value->isNode(), "results->next is node";
    is $value->asString(), $title, "nodelist contents ok";

    my $doc2 = new XmlDocument ;
    ok ! $results->next($doc2, $value), "results.next returns false";
    is $doc->getID(), $doc2->getID(), "returned document matches";
    ok $value->isNull(), "results->next is null";

}

{
    # XmlDocument::queryWithXPath -- ResultDocuments

    my $doc = new XmlDocument(); 
    my $title = '<title>Knowledge Discovery in Databases.</title>';
    my $content = "<book>$title</book>";
    $doc->setContent($content);

    my $context = new XmlQueryContext() ;
    $context->setReturnType(XmlQueryContext::ResultDocuments);

    my $results ;
    eval { $results = $doc->queryWithXPath('/book/title', $context) } ;
    ok !$@, "queryWithXPath returned 0";

    my $doc1 = new XmlDocument ;
    ok $results->next($doc1), "results.next returns true" ;

    is $doc->getID(), $doc1->getID(), "returned document matches";

    my $doc2 = new XmlDocument ;
    ok ! $results->next($doc2), "results.next returns false";
    is $doc->getID(), $doc2->getID(), "returned document matches";

}


{
    # XmlDocument::queryWithXPath -- ResultDocuments, but use next XMLValue

    my $doc = new XmlDocument(); 
    my $title = '<title>Knowledge Discovery in Databases.</title>';
    my $content = "<book>$title</book>";
    $doc->setContent($content);

    my $context = new XmlQueryContext() ;
    $context->setReturnType(XmlQueryContext::ResultDocuments);

    my $results ;
    eval { $results = $doc->queryWithXPath('/book/title', $context) } ;
    ok !$@, "queryWithXPath returned 0";

    my $value = new XmlValue ;
    ok $results->next($value), "results.next returns true" ;
    ok !$value->isNull(), "results->next not null";
    ok $value->isDocument(), "results->next is document";
    ok !$value->isString(), "results->next is not string";
    ok !$value->isNode(), "results->next is not node";
    is $value->asString(), $content, "document contents ok";

}


{
    # XmlDocument::queryWithXPath -- ResultValues, but use next XMLDocument

    my $doc = new XmlDocument(); 
    my $title = '<title>Knowledge Discovery in Databases.</title>';
    my $content = "<book>$title</book>";
    $doc->setContent($content);

    my $context = new XmlQueryContext() ;
    $context->setReturnType(XmlQueryContext::ResultValues);

    my $results ;
    eval { $results = $doc->queryWithXPath('/book/title', $context) } ;
    ok !$@, "queryWithXPath returned 0";

    my $doc1 = new XmlDocument ;
    eval { $results->next($doc1) } ;
    ok $@, "next failed";
    is $@->getExceptionCode(), XmlException::INVALID_VALUE, "exception is INVALID_VALUE";

}



{
    # XmlDocument::queryWithXPath -- ResultValues, 
    # but use next XMLDocument, XmlValue

    my $doc = new XmlDocument(); 
    my $title = '<title>Knowledge Discovery in Databases.</title>';
    my $content = "<book>$title</book>";
    $doc->setContent($content);

    my $context = new XmlQueryContext() ;
    $context->setReturnType(XmlQueryContext::ResultValues);

    my $results ;
    eval { $results = $doc->queryWithXPath('/book/title', $context) } ;
    ok !$@, "queryWithXPath returned 0";

    my $doc1 = new XmlDocument ;
    my $value = new XmlValue ;
    eval { $results->next($doc1, $value) } ;
    ok $@, "next failed";
    is $@->getExceptionCode(), XmlException::INVALID_VALUE, "exception is INVALID_VALUE";



}



{
    # XmlDocument::queryWithXPath -- ResultDocuments, 
    # but use next XMLDocument, XmlValue

    my $doc = new XmlDocument(); 
    my $title = '<title>Knowledge Discovery in Databases.</title>';
    my $content = "<book>$title</book>";
    $doc->setContent($content);

    my $context = new XmlQueryContext() ;
    $context->setReturnType(XmlQueryContext::ResultDocuments);

    my $results ;
    eval { $results = $doc->queryWithXPath('/book/title', $context) } ;
    ok !$@, "queryWithXPath returned 0";

    my $doc1 = new XmlDocument ;
    my $value = new XmlValue ;
    ok $results->next($doc1, $value), "results.next returns true" ;

    ok !$value->isNull(), "results->next not null";
    ok $value->isDocument(), "results->next is document";
    ok !$value->isString(), "results->next is not string";
    ok !$value->isNode(), "results->next is not node";
    is $value->asString(), $content, "document contents ok";

    ok ! $results->next($doc1, $value), "results.next returns false" ;
}


