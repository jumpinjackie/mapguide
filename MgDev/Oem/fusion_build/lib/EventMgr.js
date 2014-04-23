/**
 * Class: Fusion.Lib.EventMgr
 *
 * an internal class for managing generic events.  Classes that wish to
 * publish and trigger events that other objects can listen for need to
 * inherit from Fusion.Lib.EventMgr.
 *
 * To publish an event, call registerEventID with some unique numeric or
 * string value.  Other objects can then call registerForEvent with the
 * eventID and a function to call when the event is triggered.
 *
 * To trigger an event, call triggerEvent with the eventID and any additional
 * arguments that should be passed to listeners.
 */
Fusion.Lib.EventMgr = OpenLayers.Class({
    /* an array of eventIDs and associated listener functions */
    events : null,
    
    initialize: function() { if (!this.events) {this.events = []; }},

    /**
     * Method: destroy
     *
     */
    destroy: function() {
       this.events = []; 
    },
    
    /**
     * Method: registerEventID
     * 
     * register an event ID so that others can use it.  This should really
     * only be called by 'this' object.
     *
     * Parameters:
     * eventID - the event ID to register
     */
    registerEventID : function( eventID ) {
        if (!this.events) {this.events = []; }
        if (!eventID) {
            Fusion.reportError(new Fusion.Error(Fusion.Error.WARNING, 
                          OpenLayers.i18n('registerEventError')));
        }
        var ev = new String(eventID);
        if (!this.events[eventID]) {
            this.events[eventID] = [];
        }
    },

    /**
     * Method: registerForEvent
     * 
     * register for receiving a callback when an event happens. If you
     * want the callback to be a method on an instance of some object, 
     * use the {<OpenLayers.Function.bind>} function as in:
     *
     * otherObj.registerForEvent(SOME_EVENT, OpenLayers.Function.bind(this.callback,this));
     *
     * Parameters:
     * eventID - the event ID to register for
     * f - the function to call when the event happens.  
     */
    registerForEvent : function(eventID, f) {
        var ev = new String(eventID);
        this.events[eventID].push(f);
    },

    /**
     * Method: deregisterForEvent
     * 
     * deregister a callback function when you no longer want to
     * recieve it.  Note that if you used {<OpenLayers.Function.bind>} when registering,
     * you need to pass EXACTLY THE SAME FUNCTION when
     * deregistering.  Typically, this means you need to assign the
     * result of {<OpenLayers.Function.bind>} to an instance variable and pass that instance
     * variable to both {<Fusion.Lib.EventMgr.registerForEvent>} and {<Fusion.Lib.EventMgr.deregisterForEvent>}.
     *
     * For instance:
     *
     * this.callbackFn = OpenLayers.Function.bind(this.callback, this);
     * otherObj.registerForEvent(SOME_EVENT, this.callbackFn);
     * otherObj.deregisterForEvent(SOME_EVENT, this.callbackFn);
     *
     * Parameters:
     * eventID - the event ID to deregister
     * f - the function that used when registering.
     */
    deregisterForEvent : function(eventID, f) {
        var ev = new String(eventID);
        var bResult = false;
        if (!this.events[eventID]){
            return false;
        }

        for (var i=0;i<this.events[eventID].length;i++) {
            if (this.events[eventID][i]== f) {
                this.events[eventID].splice(i,1);
                bResult = true;
            }
        }
        return bResult;
    },       

    /**
     * Method: triggerEvent
     * 
     * trigger an event and call all registered listener functions.
     * This is intended to be called by 'this'.  The eventID param
     * is mandatory.  Any additional arguments will be passed to the
     * listener function.
     *
     * Parameters:
     * eventID - the event ID to trigger
     */
    triggerEvent : function( eventID ) {
        var ev = new String(eventID);
        if (!this.events || !this.events[eventID]) {
            return false;
        }

        for (var i=0; i<this.events[eventID].length; i++) {
            this.events[eventID][i].apply(null, arguments);
        }
        return true;
    }
});
        
//window.Fusion = OpenLayers.Class(Fusion.Lib.EventMgr, Fusion.prototype);
//OpenLayers.Util.extend(Fusion, Fusion.Lib.EventMgr.prototype);
Fusion.events = [];
Fusion.registerEventID = Fusion.Lib.EventMgr.prototype.registerEventID;
Fusion.registerForEvent = Fusion.Lib.EventMgr.prototype.registerForEvent;
Fusion.triggerEvent = Fusion.Lib.EventMgr.prototype.triggerEvent;

/**
 * Constant: Fusion.Event.FUSION_INITIALIZED
 */
Fusion.Event.FUSION_INITIALIZED = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.FUSION_ERROR
 */
Fusion.Event.FUSION_ERROR = Fusion.Event.lastEventId++;
Fusion.registerEventID(Fusion.Event.FUSION_INITIALIZED);
Fusion.registerEventID(Fusion.Event.FUSION_ERROR);
