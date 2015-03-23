.. index:
   single: mapagent
   
The Map Agent
=============

The MapGuide API is only accessible from the web tier, and only communicates with the server tier. Client applications like the AJAX viewer do not use the MapGuide API to communicate with the MapGuide Server, instead they communicate with the MapGuide Server through a subset of the MapGuide API exposed by the `mapagent` over the standard http protocol.

When the AJAX viewer renders a map, it is not using the :ref:`rendering-service` of the MapGuide API to do this. It is issuing a basic HTTP request to the mapagent to do this.

The mapagent, exposes a pretty large set of the MapGuide API over http, that is available for client consumption. The only pieces of the MapGuide API that are not exposed are:

 * Feature Service API that involve updating/inserting data and transactions
    * UpdateFeatures
    * ApplySchema
    * BeginTransaction

Because this functionality is exposed over http, you can programmatically communicate with the mapagent by making http requests to the mapagent url.

.. note::

    The mapagent URL is generally `http://yourservername:port/mapguide/mapagent/mapagent.fcgi`. On a default Apache installation, this port number is usually 8008
    
Unlike the MapGuide API, you can use any language that supports making http requests. In this respects, using the mapagent is a more flexible approach.

For a strongly-typed client service interface to the mapagent, you can use the :ref:`maestroapi`.