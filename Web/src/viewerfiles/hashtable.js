function Hashtable()
{
    this.length = 0;
    this.items = new Array();
    for (var i = 0; i < arguments.length; i += 2) {
        if (typeof(arguments[i + 1]) != 'undefined') {
            this.items[arguments[i]] = arguments[i + 1];
            this.length++;
        }
    }
   
    this.removeItem = function(in_key)
    {
        var tmp_value;
        if (typeof(this.items[in_key]) != 'undefined') {
            this.length--;
            var tmp_value = this.items[in_key];
            delete this.items[in_key];
        }
       
        return tmp_value;
    }

    this.getItem = function(in_key) {
        return this.items[in_key];
    }

    this.setItem = function(in_key, in_value)
    {
        if (typeof(in_value) != 'undefined') {
            if (typeof(this.items[in_key]) == 'undefined') {
                this.length++;
            }

            this.items[in_key] = in_value;
        }
       
        return in_value;
    }

    this.hasItem = function(in_key)
    {
        return typeof(this.items[in_key]) != 'undefined';
    }
}
