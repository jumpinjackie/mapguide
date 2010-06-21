use strict;
use warnings;
use Sleepycat::DbXml 'simple';


sub usage()
{
    warn  <<EOM;
This program modifies documents found in a DBXML container. You should
pre-load the container using loadExamplesData.[sh|cmd] before running this
example. You are only required to pass this command the path location of the
database environment that you specified when you pre-loaded the examples
data:

    -h <dbenv directory>
EOM
        exit( -1 );
}

#  print out the string values of matching documents
sub dumpDocuments($$)
{
    my $mgr = shift ;
    my $results = shift ;

    # //////////////////////////////////////////////////////////////////////
    # ////  Updates a document stored in a DBXML container.          ///////
    # //////////////////////////////////////////////////////////////////////
	
    # /// some defensive code eliminated for clarity //
	
	eval {
        $results->reset();
		print "Found " . $results->size() . " documents.\n\n";
		my $doc = $mgr->createDocument;
		while( $results->next($doc) ) {
			print "Document content: $doc\n";
		}
	};
    if (my $e = catch std::exception)
    {
        warn "Document query failed.\n";
        warn $e->what() . "\n";
        exit( -1 );
    }
    elsif ($@)
    {
        warn "Document query failed.\n";
        warn $@;
        exit( -1 );
    }
}

my $path2DbEnv;
my $theContainer = "namespaceExampleData.dbxml";
while (@ARGV && $ARGV[0] =~ /^-/ )
{
    my $opt = shift @ARGV ;

    if ($opt =~ s/-h//)
      { $path2DbEnv = $opt || shift }
    else
      { usage() }
}

usage()
  if ! $path2DbEnv ;

# open a container in the db environment
my $env = new DbEnv(0);
$env->set_cachesize(0, 64 * 1024, 1);
$env->open($path2DbEnv,
            Db::DB_INIT_MPOOL|Db::DB_CREATE|Db::DB_INIT_LOCK|Db::DB_INIT_LOG|Db::DB_INIT_TXN);
my $theMgr = new XmlManager($env);

my $containerTxn = $theMgr->createTransaction();
my $container = $theMgr->openContainer($containerTxn, $theContainer);
$containerTxn->commit();


# create a context and declare the namespaces
my $context = $theMgr->createQueryContext(XmlQueryContext::LiveValues, XmlQueryContext::Eager);
$context->setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
$context->setNamespace( "vegetables", "http://groceryItem.dbxml/vegetables");
$context->setNamespace( "desserts", "http://groceryItem.dbxml/desserts");
my $query = "collection('" . $container->getName() . "')/fruits:item/product[text() = 'Zapote Blanco']";
eval {
        # myDbEnv and myXmlContainer open with transactions. All subsequent
        # writes to them must also be performed inside a transaction.
        # Get a transaction
		my $txn = $theMgr->createTransaction();
		
		my $expression = $theMgr->prepare($txn, $query, $context);
		
		print "Updating document for the expression: '$query'\n";
		print "Return to continue: ";
		<>;
		
        #  print the document(s) to be updated -- those that describe Zapote Blanco (a fruit)
        #  The document strings are only being printed to show before/after results.
        #  Most modification programs would not perform the additional queries.
		my $results = $expression->execute($txn, $context);
		dumpDocuments($theMgr, $results);
		
		print "About to update the document(s) above.\nLook for a new element after the target element, named 'description' ...\n";
		print "Return to continue: ";
		<>;
		
        #  the modification is a new element in the target node, called "descripton,
        #  which goes immediately after the <product> element.
        #  if this program is run more than once, and committed, additional
        #  identical elements are added.  It is easy to modify this program
        #  to change the modification.
		my $modify = $theMgr->createModify();
		$modify->addInsertAfterStep($theMgr->prepare($txn, ".", $context), XmlModify::Element, "description", "very nice");
		
		my $uc = $theMgr->createUpdateContext();
		$results->reset();
		my $noOfModifications = $modify->execute($txn, $results, $context, $uc);
		print "$noOfModifications modifications made.\n";
		
        #  print the modified documents
		dumpDocuments($theMgr, $results);
		
		print "If committed, this program will add a new element each time it is run.\n";
		print "Press 'c' to commit this change:";
		my $cmt = <> ;
		if ($cmt =~ /^c/i) {
			$txn->commit();
		}
		else {
			$txn->abort();
		}
};
if (my $e = catch std::exception)
{
    warn "Document modification failed.\n";
    warn $e->what() . "\n";
    exit( -1 );
}
elsif ($@)
{
    warn "Document modification failed.\n";
    warn $@;
    exit( -1 );
}
