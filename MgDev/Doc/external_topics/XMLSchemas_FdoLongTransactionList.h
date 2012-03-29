/**
\addtogroup FdoLongTransactionList_schema

<p>
This schema governs the output of
MgFeatureService::GetLongTransactions(). What follows is a
representation of the element hierarchy in
FdoLongTransactionList-1.0.0.xsd. Frequency of element
occurrence is indicated by adding a quantifier symbol (* for
0 or more, + for 1 or more, ? for 0 or 1 and nothing for 1)
to the end of the element identifier.
</p>
<pre>
FdoLongTransactionList
   ProviderName
   LongTransactiopn+ isActive=&lt;T|F&gt; isFrozen=&lt;T|F&gt;
      Name
      Description
      Owner
      CreationDate
</pre>

<a href="../../xml_schemas/FdoLongTransactionList/FdoLongTransactionList.html">Details</a>

**/