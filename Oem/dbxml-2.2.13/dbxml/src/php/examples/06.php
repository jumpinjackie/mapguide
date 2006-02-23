    mgr = XmlManager()
    uc = mgr.createUpdateContext()
    container = mgr.createContainer("test.dbxml")
    container.addIndex("", "title", "node-element-equality-string", uc)
    container.putDocument(book_name, book_content, uc)
    qc = mgr.createQueryContext()
    results = mgr.query("collection('test.dbxml')//*[title='Knowledge Discovery in Databases.']", qc)
    qc.setVariableValue("title", XmlValue("Knowledge Discovery in Databases."))
    results = mgr.query("collection('test.dbxml')//*[title=$title]", qc)
    for value in results:
        document = value.asDocument()
        print document.getName(), "=", value.asString()

<?php
$book_name = 'book1';
$book_content = "<book><title>Knowledge Discovery in Databases.</title></book>";
$container_name = 'test.dbxml';

@unlink($container_name);
$mgr = new XmlManager(null);
$con = $mgr->createContainer("test.dbxml");
$con->addIndex("", "title", "node-element-equality-string");
$con->putDocument($book_name, $book_content);
$qc = $mgr->createQueryContext();
$qc->setVariableValue("title", "Knowledge Discovery in Databases.");
$results = $mgr->query("collection('test.dbxml')//*[title=\$title]", $qc);

$results->reset();
while($results->hasNext()) 
{
    $val = $results->next();
    $doc = $val->asDocument();
    print $doc->getName()." = ".$val->asString()."\n";
}
