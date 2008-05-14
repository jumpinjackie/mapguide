/******************************************************************************
 *  Prototype JavaScript framework, version 1.5.0
 *  (c) 2005-2007 Sam Stephenson
 *  Prototype is freely distributable under the terms of an MIT-style license.
 *  For details, see the Prototype web site: http://prototype.conio.net/
 ******************************************************************************
 * Jx UI Library, version 1.0
 * Copyright &copy; 2008, DM Solutions Group Inc.
 * Jx is freely distributable under the terms of an MIT-style license.
 *****************************************************************************/
/*  Prototype JavaScript framework, version 1.5.0
 *  (c) 2005-2007 Sam Stephenson
 *
 *  Prototype is freely distributable under the terms of an MIT-style license.
 *  For details, see the Prototype web site: http://prototype.conio.net/
 *
/*--------------------------------------------------------------------------*/

var Prototype = {
  Version: '1.5.0',
  BrowserFeatures: {
    XPath: !!document.evaluate
  },

  ScriptFragment: '(?:<script.*?>)((\n|\r|.)*?)(?:<\/script>)',
  emptyFunction: function() {},
  K: function(x) { return x }
}

var Class = {
  create: function() {
    return function() {
      this.initialize.apply(this, arguments);
    }
  }
}

var Abstract = new Object();

Object.extend = function(destination, source) {
  for (var property in source) {
    destination[property] = source[property];
  }
  return destination;
}

Object.extend(Object, {
  inspect: function(object) {
    try {
      if (object === undefined) return 'undefined';
      if (object === null) return 'null';
      return object.inspect ? object.inspect() : object.toString();
    } catch (e) {
      if (e instanceof RangeError) return '...';
      throw e;
    }
  },

  keys: function(object) {
    var keys = [];
    for (var property in object)
      keys.push(property);
    return keys;
  },

  values: function(object) {
    var values = [];
    for (var property in object)
      values.push(object[property]);
    return values;
  },

  clone: function(object) {
    return Object.extend({}, object);
  }
});

Function.prototype.bind = function() {
  var __method = this, args = $A(arguments), object = args.shift();
  return function() {
    return __method.apply(object, args.concat($A(arguments)));
  }
}

Function.prototype.bindAsEventListener = function(object) {
  var __method = this, args = $A(arguments), object = args.shift();
  return function(event) {
    return __method.apply(object, [( event || window.event)].concat(args).concat($A(arguments)));
  }
}

Object.extend(Number.prototype, {
  toColorPart: function() {
    var digits = this.toString(16);
    if (this < 16) return '0' + digits;
    return digits;
  },

  succ: function() {
    return this + 1;
  },

  times: function(iterator) {
    $R(0, this, true).each(iterator);
    return this;
  }
});

var Try = {
  these: function() {
    var returnValue;

    for (var i = 0, length = arguments.length; i < length; i++) {
      var lambda = arguments[i];
      try {
        returnValue = lambda();
        break;
      } catch (e) {}
    }

    return returnValue;
  }
}

/*--------------------------------------------------------------------------*/

var PeriodicalExecuter = Class.create();
PeriodicalExecuter.prototype = {
  initialize: function(callback, frequency) {
    this.callback = callback;
    this.frequency = frequency;
    this.currentlyExecuting = false;

    this.registerCallback();
  },

  registerCallback: function() {
    this.timer = setInterval(this.onTimerEvent.bind(this), this.frequency * 1000);
  },

  stop: function() {
    if (!this.timer) return;
    clearInterval(this.timer);
    this.timer = null;
  },

  onTimerEvent: function() {
    if (!this.currentlyExecuting) {
      try {
        this.currentlyExecuting = true;
        this.callback(this);
      } finally {
        this.currentlyExecuting = false;
      }
    }
  }
}
String.interpret = function(value){
  return value == null ? '' : String(value);
}

Object.extend(String.prototype, {
  gsub: function(pattern, replacement) {
    var result = '', source = this, match;
    replacement = arguments.callee.prepareReplacement(replacement);

    while (source.length > 0) {
      if (match = source.match(pattern)) {
        result += source.slice(0, match.index);
        result += String.interpret(replacement(match));
        source  = source.slice(match.index + match[0].length);
      } else {
        result += source, source = '';
      }
    }
    return result;
  },

  sub: function(pattern, replacement, count) {
    replacement = this.gsub.prepareReplacement(replacement);
    count = count === undefined ? 1 : count;

    return this.gsub(pattern, function(match) {
      if (--count < 0) return match[0];
      return replacement(match);
    });
  },

  scan: function(pattern, iterator) {
    this.gsub(pattern, iterator);
    return this;
  },

  truncate: function(length, truncation) {
    length = length || 30;
    truncation = truncation === undefined ? '...' : truncation;
    return this.length > length ?
      this.slice(0, length - truncation.length) + truncation : this;
  },

  strip: function() {
    return this.replace(/^\s+/, '').replace(/\s+$/, '');
  },

  stripTags: function() {
    return this.replace(/<\/?[^>]+>/gi, '');
  },

  stripScripts: function() {
    return this.replace(new RegExp(Prototype.ScriptFragment, 'img'), '');
  },

  extractScripts: function() {
    var matchAll = new RegExp(Prototype.ScriptFragment, 'img');
    var matchOne = new RegExp(Prototype.ScriptFragment, 'im');
    return (this.match(matchAll) || []).map(function(scriptTag) {
      return (scriptTag.match(matchOne) || ['', ''])[1];
    });
  },

  evalScripts: function() {
    return this.extractScripts().map(function(script) { return eval(script) });
  },

  escapeHTML: function() {
    var div = document.createElement('div');
    var text = document.createTextNode(this);
    div.appendChild(text);
    return div.innerHTML;
  },

  unescapeHTML: function() {
    var div = document.createElement('div');
    div.innerHTML = this.stripTags();
    return div.childNodes[0] ? (div.childNodes.length > 1 ?
      $A(div.childNodes).inject('',function(memo,node){ return memo+node.nodeValue }) :
      div.childNodes[0].nodeValue) : '';
  },

  toQueryParams: function(separator) {
    var match = this.strip().match(/([^?#]*)(#.*)?$/);
    if (!match) return {};

    return match[1].split(separator || '&').inject({}, function(hash, pair) {
      if ((pair = pair.split('='))[0]) {
        var name = decodeURIComponent(pair[0]);
        var value = pair[1] ? decodeURIComponent(pair[1]) : undefined;

        if (hash[name] !== undefined) {
          if (hash[name].constructor != Array)
            hash[name] = [hash[name]];
          if (value) hash[name].push(value);
        }
        else hash[name] = value;
      }
      return hash;
    });
  },

  toArray: function() {
    return this.split('');
  },

  succ: function() {
    return this.slice(0, this.length - 1) +
      String.fromCharCode(this.charCodeAt(this.length - 1) + 1);
  },

  camelize: function() {
    var parts = this.split('-'), len = parts.length;
    if (len == 1) return parts[0];

    var camelized = this.charAt(0) == '-'
      ? parts[0].charAt(0).toUpperCase() + parts[0].substring(1)
      : parts[0];

    for (var i = 1; i < len; i++)
      camelized += parts[i].charAt(0).toUpperCase() + parts[i].substring(1);

    return camelized;
  },

  capitalize: function(){
    return this.charAt(0).toUpperCase() + this.substring(1).toLowerCase();
  },

  underscore: function() {
    return this.gsub(/::/, '/').gsub(/([A-Z]+)([A-Z][a-z])/,'#{1}_#{2}').gsub(/([a-z\d])([A-Z])/,'#{1}_#{2}').gsub(/-/,'_').toLowerCase();
  },

  dasherize: function() {
    return this.gsub(/_/,'-');
  },

  inspect: function(useDoubleQuotes) {
    var escapedString = this.replace(/\\/g, '\\\\');
    if (useDoubleQuotes)
      return '"' + escapedString.replace(/"/g, '\\"') + '"';
    else
      return "'" + escapedString.replace(/'/g, '\\\'') + "'";
  }
});

String.prototype.gsub.prepareReplacement = function(replacement) {
  if (typeof replacement == 'function') return replacement;
  var template = new Template(replacement);
  return function(match) { return template.evaluate(match) };
}

String.prototype.parseQuery = String.prototype.toQueryParams;

var Template = Class.create();
Template.Pattern = /(^|.|\r|\n)(#\{(.*?)\})/;
Template.prototype = {
  initialize: function(template, pattern) {
    this.template = template.toString();
    this.pattern  = pattern || Template.Pattern;
  },

  evaluate: function(object) {
    return this.template.gsub(this.pattern, function(match) {
      var before = match[1];
      if (before == '\\') return match[2];
      return before + String.interpret(object[match[3]]);
    });
  }
}

var $break    = new Object();
var $continue = new Object();

var Enumerable = {
  each: function(iterator) {
    var index = 0;
    try {
      this._each(function(value) {
        try {
          iterator(value, index++);
        } catch (e) {
          if (e != $continue) throw e;
        }
      });
    } catch (e) {
      if (e != $break) throw e;
    }
    return this;
  },

  eachSlice: function(number, iterator) {
    var index = -number, slices = [], array = this.toArray();
    while ((index += number) < array.length)
      slices.push(array.slice(index, index+number));
    return slices.map(iterator);
  },

  all: function(iterator) {
    var result = true;
    this.each(function(value, index) {
      result = result && !!(iterator || Prototype.K)(value, index);
      if (!result) throw $break;
    });
    return result;
  },

  any: function(iterator) {
    var result = false;
    this.each(function(value, index) {
      if (result = !!(iterator || Prototype.K)(value, index))
        throw $break;
    });
    return result;
  },

  collect: function(iterator) {
    var results = [];
    this.each(function(value, index) {
      results.push((iterator || Prototype.K)(value, index));
    });
    return results;
  },

  detect: function(iterator) {
    var result;
    this.each(function(value, index) {
      if (iterator(value, index)) {
        result = value;
        throw $break;
      }
    });
    return result;
  },

  findAll: function(iterator) {
    var results = [];
    this.each(function(value, index) {
      if (iterator(value, index))
        results.push(value);
    });
    return results;
  },

  grep: function(pattern, iterator) {
    var results = [];
    this.each(function(value, index) {
      var stringValue = value.toString();
      if (stringValue.match(pattern))
        results.push((iterator || Prototype.K)(value, index));
    })
    return results;
  },

  include: function(object) {
    var found = false;
    this.each(function(value) {
      if (value == object) {
        found = true;
        throw $break;
      }
    });
    return found;
  },

  inGroupsOf: function(number, fillWith) {
    fillWith = fillWith === undefined ? null : fillWith;
    return this.eachSlice(number, function(slice) {
      while(slice.length < number) slice.push(fillWith);
      return slice;
    });
  },

  inject: function(memo, iterator) {
    this.each(function(value, index) {
      memo = iterator(memo, value, index);
    });
    return memo;
  },

  invoke: function(method) {
    var args = $A(arguments).slice(1);
    return this.map(function(value) {
      return value[method].apply(value, args);
    });
  },

  max: function(iterator) {
    var result;
    this.each(function(value, index) {
      value = (iterator || Prototype.K)(value, index);
      if (result == undefined || value >= result)
        result = value;
    });
    return result;
  },

  min: function(iterator) {
    var result;
    this.each(function(value, index) {
      value = (iterator || Prototype.K)(value, index);
      if (result == undefined || value < result)
        result = value;
    });
    return result;
  },

  partition: function(iterator) {
    var trues = [], falses = [];
    this.each(function(value, index) {
      ((iterator || Prototype.K)(value, index) ?
        trues : falses).push(value);
    });
    return [trues, falses];
  },

  pluck: function(property) {
    var results = [];
    this.each(function(value, index) {
      results.push(value[property]);
    });
    return results;
  },

  reject: function(iterator) {
    var results = [];
    this.each(function(value, index) {
      if (!iterator(value, index))
        results.push(value);
    });
    return results;
  },

  sortBy: function(iterator) {
    return this.map(function(value, index) {
      return {value: value, criteria: iterator(value, index)};
    }).sort(function(left, right) {
      var a = left.criteria, b = right.criteria;
      return a < b ? -1 : a > b ? 1 : 0;
    }).pluck('value');
  },

  toArray: function() {
    return this.map();
  },

  zip: function() {
    var iterator = Prototype.K, args = $A(arguments);
    if (typeof args.last() == 'function')
      iterator = args.pop();

    var collections = [this].concat(args).map($A);
    return this.map(function(value, index) {
      return iterator(collections.pluck(index));
    });
  },

  size: function() {
    return this.toArray().length;
  },

  inspect: function() {
    return '#<Enumerable:' + this.toArray().inspect() + '>';
  }
}

Object.extend(Enumerable, {
  map:     Enumerable.collect,
  find:    Enumerable.detect,
  select:  Enumerable.findAll,
  member:  Enumerable.include,
  entries: Enumerable.toArray
});
var $A = Array.from = function(iterable) {
  if (!iterable) return [];
  if (iterable.toArray) {
    return iterable.toArray();
  } else {
    var results = [];
    for (var i = 0, length = iterable.length; i < length; i++)
      results.push(iterable[i]);
    return results;
  }
}

Object.extend(Array.prototype, Enumerable);

if (!Array.prototype._reverse)
  Array.prototype._reverse = Array.prototype.reverse;

Object.extend(Array.prototype, {
  _each: function(iterator) {
    for (var i = 0, length = this.length; i < length; i++)
      iterator(this[i]);
  },

  clear: function() {
    this.length = 0;
    return this;
  },

  first: function() {
    return this[0];
  },

  last: function() {
    return this[this.length - 1];
  },

  compact: function() {
    return this.select(function(value) {
      return value != null;
    });
  },

  flatten: function() {
    return this.inject([], function(array, value) {
      return array.concat(value && value.constructor == Array ?
        value.flatten() : [value]);
    });
  },

  without: function() {
    var values = $A(arguments);
    return this.select(function(value) {
      return !values.include(value);
    });
  },

  indexOf: function(object) {
    for (var i = 0, length = this.length; i < length; i++)
      if (this[i] == object) return i;
    return -1;
  },

  reverse: function(inline) {
    return (inline !== false ? this : this.toArray())._reverse();
  },

  reduce: function() {
    return this.length > 1 ? this : this[0];
  },

  uniq: function() {
    return this.inject([], function(array, value) {
      return array.include(value) ? array : array.concat([value]);
    });
  },

  clone: function() {
    return [].concat(this);
  },

  size: function() {
    return this.length;
  },

  inspect: function() {
    return '[' + this.map(Object.inspect).join(', ') + ']';
  }
});

Array.prototype.toArray = Array.prototype.clone;

function $w(string){
  string = string.strip();
  return string ? string.split(/\s+/) : [];
}

if(window.opera){
  Array.prototype.concat = function(){
    var array = [];
    for(var i = 0, length = this.length; i < length; i++) array.push(this[i]);
    for(var i = 0, length = arguments.length; i < length; i++) {
      if(arguments[i].constructor == Array) {
        for(var j = 0, arrayLength = arguments[i].length; j < arrayLength; j++)
          array.push(arguments[i][j]);
      } else {
        array.push(arguments[i]);
      }
    }
    return array;
  }
}
var Hash = function(obj) {
  Object.extend(this, obj || {});
};

Object.extend(Hash, {
  toQueryString: function(obj) {
    var parts = [];

	  this.prototype._each.call(obj, function(pair) {
      if (!pair.key) return;

      if (pair.value && pair.value.constructor == Array) {
        var values = pair.value.compact();
        if (values.length < 2) pair.value = values.reduce();
        else {
        	key = encodeURIComponent(pair.key);
          values.each(function(value) {
            value = value != undefined ? encodeURIComponent(value) : '';
            parts.push(key + '=' + encodeURIComponent(value));
          });
          return;
        }
      }
      if (pair.value == undefined) pair[1] = '';
      parts.push(pair.map(encodeURIComponent).join('='));
	  });

    return parts.join('&');
  }
});

Object.extend(Hash.prototype, Enumerable);
Object.extend(Hash.prototype, {
  _each: function(iterator) {
    for (var key in this) {
      var value = this[key];
      if (value && value == Hash.prototype[key]) continue;

      var pair = [key, value];
      pair.key = key;
      pair.value = value;
      iterator(pair);
    }
  },

  keys: function() {
    return this.pluck('key');
  },

  values: function() {
    return this.pluck('value');
  },

  merge: function(hash) {
    return $H(hash).inject(this, function(mergedHash, pair) {
      mergedHash[pair.key] = pair.value;
      return mergedHash;
    });
  },

  remove: function() {
    var result;
    for(var i = 0, length = arguments.length; i < length; i++) {
      var value = this[arguments[i]];
      if (value !== undefined){
        if (result === undefined) result = value;
        else {
          if (result.constructor != Array) result = [result];
          result.push(value)
        }
      }
      delete this[arguments[i]];
    }
    return result;
  },

  toQueryString: function() {
    return Hash.toQueryString(this);
  },

  inspect: function() {
    return '#<Hash:{' + this.map(function(pair) {
      return pair.map(Object.inspect).join(': ');
    }).join(', ') + '}>';
  }
});

function $H(object) {
  if (object && object.constructor == Hash) return object;
  return new Hash(object);
};
ObjectRange = Class.create();
Object.extend(ObjectRange.prototype, Enumerable);
Object.extend(ObjectRange.prototype, {
  initialize: function(start, end, exclusive) {
    this.start = start;
    this.end = end;
    this.exclusive = exclusive;
  },

  _each: function(iterator) {
    var value = this.start;
    while (this.include(value)) {
      iterator(value);
      value = value.succ();
    }
  },

  include: function(value) {
    if (value < this.start)
      return false;
    if (this.exclusive)
      return value < this.end;
    return value <= this.end;
  }
});

var $R = function(start, end, exclusive) {
  return new ObjectRange(start, end, exclusive);
}

var Ajax = {
  getTransport: function() {
    return Try.these(
      function() {return new XMLHttpRequest()},
      function() {return new ActiveXObject('Msxml2.XMLHTTP')},
      function() {return new ActiveXObject('Microsoft.XMLHTTP')}
    ) || false;
  },

  activeRequestCount: 0
}

Ajax.Responders = {
  responders: [],

  _each: function(iterator) {
    this.responders._each(iterator);
  },

  register: function(responder) {
    if (!this.include(responder))
      this.responders.push(responder);
  },

  unregister: function(responder) {
    this.responders = this.responders.without(responder);
  },

  dispatch: function(callback, request, transport, json) {
    this.each(function(responder) {
      if (typeof responder[callback] == 'function') {
        try {
          responder[callback].apply(responder, [request, transport, json]);
        } catch (e) {}
      }
    });
  }
};

Object.extend(Ajax.Responders, Enumerable);

Ajax.Responders.register({
  onCreate: function() {
    Ajax.activeRequestCount++;
  },
  onComplete: function() {
    Ajax.activeRequestCount--;
  }
});

Ajax.Base = function() {};
Ajax.Base.prototype = {
  setOptions: function(options) {
    this.options = {
      method:       'post',
      asynchronous: true,
      contentType:  'application/x-www-form-urlencoded',
      encoding:     'UTF-8',
      parameters:   ''
    }
    Object.extend(this.options, options || {});

    this.options.method = this.options.method.toLowerCase();
    if (typeof this.options.parameters == 'string')
      this.options.parameters = this.options.parameters.toQueryParams();
  }
}

Ajax.Request = Class.create();
Ajax.Request.Events =
  ['Uninitialized', 'Loading', 'Loaded', 'Interactive', 'Complete'];

Ajax.Request.prototype = Object.extend(new Ajax.Base(), {
  _complete: false,

  initialize: function(url, options) {
    this.transport = Ajax.getTransport();
    this.setOptions(options);
    this.request(url);
  },

  request: function(url) {
    this.url = url;
    this.method = this.options.method;
    var params = this.options.parameters;

    if (!['get', 'post'].include(this.method)) {
      // simulate other verbs over post
      params['_method'] = this.method;
      this.method = 'post';
    }

    params = Hash.toQueryString(params);
    if (params && /Konqueror|Safari|KHTML/.test(navigator.userAgent)) params += '&_='

    // when GET, append parameters to URL
    if (this.method == 'get' && params)
      this.url += (this.url.indexOf('?') > -1 ? '&' : '?') + params;

    try {
      Ajax.Responders.dispatch('onCreate', this, this.transport);

      this.transport.open(this.method.toUpperCase(), this.url,
        this.options.asynchronous);

      if (this.options.asynchronous)
        setTimeout(function() { this.respondToReadyState(1) }.bind(this), 10);

      this.transport.onreadystatechange = this.onStateChange.bind(this);
      this.setRequestHeaders();

      var body = this.method == 'post' ? (this.options.postBody || params) : null;

      this.transport.send(body);

      /* Force Firefox to handle ready state 4 for synchronous requests */
      if (!this.options.asynchronous && this.transport.overrideMimeType)
        this.onStateChange();

    }
    catch (e) {
      this.dispatchException(e);
    }
  },

  onStateChange: function() {
    var readyState = this.transport.readyState;
    if (readyState > 1 && !((readyState == 4) && this._complete))
      this.respondToReadyState(this.transport.readyState);
  },

  setRequestHeaders: function() {
    var headers = {
      'X-Requested-With': 'XMLHttpRequest',
      'X-Prototype-Version': Prototype.Version,
      'Accept': 'text/javascript, text/html, application/xml, text/xml, */*'
    };

    if (this.method == 'post') {
      headers['Content-type'] = this.options.contentType +
        (this.options.encoding ? '; charset=' + this.options.encoding : '');

      /* Force "Connection: close" for older Mozilla browsers to work
       * around a bug where XMLHttpRequest sends an incorrect
       * Content-length header. See Mozilla Bugzilla #246651.
       */
      if (this.transport.overrideMimeType &&
          (navigator.userAgent.match(/Gecko\/(\d{4})/) || [0,2005])[1] < 2005)
            headers['Connection'] = 'close';
    }

    // user-defined headers
    if (typeof this.options.requestHeaders == 'object') {
      var extras = this.options.requestHeaders;

      if (typeof extras.push == 'function')
        for (var i = 0, length = extras.length; i < length; i += 2)
          headers[extras[i]] = extras[i+1];
      else
        $H(extras).each(function(pair) { headers[pair.key] = pair.value });
    }

    for (var name in headers)
      this.transport.setRequestHeader(name, headers[name]);
  },

  success: function() {
    return !this.transport.status
        || (this.transport.status >= 200 && this.transport.status < 300);
  },

  respondToReadyState: function(readyState) {
    var state = Ajax.Request.Events[readyState];
    var transport = this.transport, json = this.evalJSON();

    if (state == 'Complete') {
      try {
        this._complete = true;
        (this.options['on' + this.transport.status]
         || this.options['on' + (this.success() ? 'Success' : 'Failure')]
         || Prototype.emptyFunction)(transport, json);
      } catch (e) {
        this.dispatchException(e);
      }

      if ((this.getHeader('Content-type') || 'text/javascript').strip().
        match(/^(text|application)\/(x-)?(java|ecma)script(;.*)?$/i))
          this.evalResponse();
    }

    try {
      (this.options['on' + state] || Prototype.emptyFunction)(transport, json);
      Ajax.Responders.dispatch('on' + state, this, transport, json);
    } catch (e) {
      this.dispatchException(e);
    }

    if (state == 'Complete') {
      // avoid memory leak in MSIE: clean up
      this.transport.onreadystatechange = Prototype.emptyFunction;
    }
  },

  getHeader: function(name) {
    try {
      return this.transport.getResponseHeader(name);
    } catch (e) { return null }
  },

  evalJSON: function() {
    try {
      var json = this.getHeader('X-JSON');
      return json ? eval('(' + json + ')') : null;
    } catch (e) { return null }
  },

  evalResponse: function() {
    try {
      return eval(this.transport.responseText);
    } catch (e) {
      this.dispatchException(e);
    }
  },

  dispatchException: function(exception) {
    (this.options.onException || Prototype.emptyFunction)(this, exception);
    Ajax.Responders.dispatch('onException', this, exception);
  }
});

Ajax.Updater = Class.create();

Object.extend(Object.extend(Ajax.Updater.prototype, Ajax.Request.prototype), {
  initialize: function(container, url, options) {
    this.container = {
      success: (container.success || container),
      failure: (container.failure || (container.success ? null : container))
    }

    this.transport = Ajax.getTransport();
    this.setOptions(options);

    var onComplete = this.options.onComplete || Prototype.emptyFunction;
    this.options.onComplete = (function(transport, param) {
      this.updateContent();
      onComplete(transport, param);
    }).bind(this);

    this.request(url);
  },

  updateContent: function() {
    var receiver = this.container[this.success() ? 'success' : 'failure'];
    var response = this.transport.responseText;

    if (!this.options.evalScripts) response = response.stripScripts();

    if (receiver = $(receiver)) {
      if (this.options.insertion)
        new this.options.insertion(receiver, response);
      else
        receiver.update(response);
    }

    if (this.success()) {
      if (this.onComplete)
        setTimeout(this.onComplete.bind(this), 10);
    }
  }
});

Ajax.PeriodicalUpdater = Class.create();
Ajax.PeriodicalUpdater.prototype = Object.extend(new Ajax.Base(), {
  initialize: function(container, url, options) {
    this.setOptions(options);
    this.onComplete = this.options.onComplete;

    this.frequency = (this.options.frequency || 2);
    this.decay = (this.options.decay || 1);

    this.updater = {};
    this.container = container;
    this.url = url;

    this.start();
  },

  start: function() {
    this.options.onComplete = this.updateComplete.bind(this);
    this.onTimerEvent();
  },

  stop: function() {
    this.updater.options.onComplete = undefined;
    clearTimeout(this.timer);
    (this.onComplete || Prototype.emptyFunction).apply(this, arguments);
  },

  updateComplete: function(request) {
    if (this.options.decay) {
      this.decay = (request.responseText == this.lastText ?
        this.decay * this.options.decay : 1);

      this.lastText = request.responseText;
    }
    this.timer = setTimeout(this.onTimerEvent.bind(this),
      this.decay * this.frequency * 1000);
  },

  onTimerEvent: function() {
    this.updater = new Ajax.Updater(this.container, this.url, this.options);
  }
});
function $(element) {
  if (arguments.length > 1) {
    for (var i = 0, elements = [], length = arguments.length; i < length; i++)
      elements.push($(arguments[i]));
    return elements;
  }
  if (typeof element == 'string')
    element = document.getElementById(element);
  return Element.extend(element);
}

if (Prototype.BrowserFeatures.XPath) {
  document._getElementsByXPath = function(expression, parentElement) {
    var results = [];
    var query = document.evaluate(expression, $(parentElement) || document,
      null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
    for (var i = 0, length = query.snapshotLength; i < length; i++)
      results.push(query.snapshotItem(i));
    return results;
  };
}

document.getElementsByClassName = function(className, parentElement) {
  if (Prototype.BrowserFeatures.XPath) {
    var q = ".//*[contains(concat(' ', @class, ' '), ' " + className + " ')]";
    return document._getElementsByXPath(q, parentElement);
  } else {
    var children = ($(parentElement) || document.body).getElementsByTagName('*');
    var elements = [], child;
    for (var i = 0, length = children.length; i < length; i++) {
      child = children[i];
      if (Element.hasClassName(child, className))
        elements.push(Element.extend(child));
    }
    return elements;
  }
};

/*--------------------------------------------------------------------------*/

if (!window.Element)
  var Element = new Object();

Element.extend = function(element) {
  if (!element || _nativeExtensions || element.nodeType == 3) return element;

  if (!element._extended && element.tagName && element != window) {
    var methods = Object.clone(Element.Methods), cache = Element.extend.cache;

    if (element.tagName == 'FORM')
      Object.extend(methods, Form.Methods);
    if (['INPUT', 'TEXTAREA', 'SELECT'].include(element.tagName))
      Object.extend(methods, Form.Element.Methods);

    Object.extend(methods, Element.Methods.Simulated);

    for (var property in methods) {
      var value = methods[property];
      if (typeof value == 'function' && !(property in element))
        element[property] = cache.findOrStore(value);
    }
  }

  element._extended = true;
  return element;
};

Element.extend.cache = {
  findOrStore: function(value) {
    return this[value] = this[value] || function() {
      return value.apply(null, [this].concat($A(arguments)));
    }
  }
};

Element.Methods = {
  visible: function(element) {
    return $(element).style.display != 'none';
  },

  toggle: function(element) {
    element = $(element);
    Element[Element.visible(element) ? 'hide' : 'show'](element);
    return element;
  },

  hide: function(element) {
    $(element).style.display = 'none';
    return element;
  },

  show: function(element) {
    $(element).style.display = '';
    return element;
  },

  remove: function(element) {
    element = $(element);
    element.parentNode.removeChild(element);
    return element;
  },

  update: function(element, html) {
    html = typeof html == 'undefined' ? '' : html.toString();
    $(element).innerHTML = html.stripScripts();
    setTimeout(function() {html.evalScripts()}, 10);
    return element;
  },

  replace: function(element, html) {
    element = $(element);
    html = typeof html == 'undefined' ? '' : html.toString();
    if (element.outerHTML) {
      element.outerHTML = html.stripScripts();
    } else {
      var range = element.ownerDocument.createRange();
      range.selectNodeContents(element);
      element.parentNode.replaceChild(
        range.createContextualFragment(html.stripScripts()), element);
    }
    setTimeout(function() {html.evalScripts()}, 10);
    return element;
  },

  inspect: function(element) {
    element = $(element);
    var result = '<' + element.tagName.toLowerCase();
    $H({'id': 'id', 'className': 'class'}).each(function(pair) {
      var property = pair.first(), attribute = pair.last();
      var value = (element[property] || '').toString();
      if (value) result += ' ' + attribute + '=' + value.inspect(true);
    });
    return result + '>';
  },

  recursivelyCollect: function(element, property) {
    element = $(element);
    var elements = [];
    while (element = element[property])
      if (element.nodeType == 1)
        elements.push(Element.extend(element));
    return elements;
  },

  ancestors: function(element) {
    return $(element).recursivelyCollect('parentNode');
  },

  descendants: function(element) {
    return $A($(element).getElementsByTagName('*'));
  },

  immediateDescendants: function(element) {
    if (!(element = $(element).firstChild)) return [];
    while (element && element.nodeType != 1) element = element.nextSibling;
    if (element) return [element].concat($(element).nextSiblings());
    return [];
  },

  previousSiblings: function(element) {
    return $(element).recursivelyCollect('previousSibling');
  },

  nextSiblings: function(element) {
    return $(element).recursivelyCollect('nextSibling');
  },

  siblings: function(element) {
    element = $(element);
    return element.previousSiblings().reverse().concat(element.nextSiblings());
  },

  match: function(element, selector) {
    if (typeof selector == 'string')
      selector = new Selector(selector);
    return selector.match($(element));
  },

  up: function(element, expression, index) {
    return Selector.findElement($(element).ancestors(), expression, index);
  },

  down: function(element, expression, index) {
    return Selector.findElement($(element).descendants(), expression, index);
  },

  previous: function(element, expression, index) {
    return Selector.findElement($(element).previousSiblings(), expression, index);
  },

  next: function(element, expression, index) {
    return Selector.findElement($(element).nextSiblings(), expression, index);
  },

  getElementsBySelector: function() {
    var args = $A(arguments), element = $(args.shift());
    return Selector.findChildElements(element, args);
  },

  getElementsByClassName: function(element, className) {
    return document.getElementsByClassName(className, element);
  },

  readAttribute: function(element, name) {
    element = $(element);
    if (document.all && !window.opera) {
      var t = Element._attributeTranslations;
      if (t.values[name]) return t.values[name](element, name);
      if (t.names[name])  name = t.names[name];
      var attribute = element.attributes[name];
      if(attribute) return attribute.nodeValue;
    }
    return element.getAttribute(name);
  },

  getHeight: function(element) {
    return $(element).getDimensions().height;
  },

  getWidth: function(element) {
    return $(element).getDimensions().width;
  },

  classNames: function(element) {
    return new Element.ClassNames(element);
  },

  hasClassName: function(element, className) {
    if (!(element = $(element))) return;
    var elementClassName = element.className;
    if (elementClassName.length == 0) return false;
    if (elementClassName == className ||
        elementClassName.match(new RegExp("(^|\\s)" + className + "(\\s|$)")))
      return true;
    return false;
  },

  addClassName: function(element, className) {
    if (!(element = $(element))) return;
    Element.classNames(element).add(className);
    return element;
  },

  removeClassName: function(element, className) {
    if (!(element = $(element))) return;
    Element.classNames(element).remove(className);
    return element;
  },

  toggleClassName: function(element, className) {
    if (!(element = $(element))) return;
    Element.classNames(element)[element.hasClassName(className) ? 'remove' : 'add'](className);
    return element;
  },

  observe: function() {
    Event.observe.apply(Event, arguments);
    return $A(arguments).first();
  },

  stopObserving: function() {
    Event.stopObserving.apply(Event, arguments);
    return $A(arguments).first();
  },

  // removes whitespace-only text node children
  cleanWhitespace: function(element) {
    element = $(element);
    var node = element.firstChild;
    while (node) {
      var nextNode = node.nextSibling;
      if (node.nodeType == 3 && !/\S/.test(node.nodeValue))
        element.removeChild(node);
      node = nextNode;
    }
    return element;
  },

  empty: function(element) {
    return $(element).innerHTML.match(/^\s*$/);
  },

  descendantOf: function(element, ancestor) {
    element = $(element), ancestor = $(ancestor);
    while (element = element.parentNode)
      if (element == ancestor) return true;
    return false;
  },

  scrollTo: function(element) {
    element = $(element);
    var pos = Position.cumulativeOffset(element);
    window.scrollTo(pos[0], pos[1]);
    return element;
  },

  getStyle: function(element, style) {
    element = $(element);
    if (['float','cssFloat'].include(style))
      style = (typeof element.style.styleFloat != 'undefined' ? 'styleFloat' : 'cssFloat');
    style = style.camelize();
    var value = element.style[style];
    if (!value) {
      if (document.defaultView && document.defaultView.getComputedStyle) {
        var css = document.defaultView.getComputedStyle(element, null);
        value = css ? css[style] : null;
      } else if (element.currentStyle) {
        value = element.currentStyle[style];
      }
    }

    if((value == 'auto') && ['width','height'].include(style) && (element.getStyle('display') != 'none'))
      value = element['offset'+style.capitalize()] + 'px';

    if (window.opera && ['left', 'top', 'right', 'bottom'].include(style))
      if (Element.getStyle(element, 'position') == 'static') value = 'auto';
    if(style == 'opacity') {
      if(value) return parseFloat(value);
      if(value = (element.getStyle('filter') || '').match(/alpha\(opacity=(.*)\)/))
        if(value[1]) return parseFloat(value[1]) / 100;
      return 1.0;
    }
    return value == 'auto' ? null : value;
  },

  setStyle: function(element, style) {
    element = $(element);
    for (var name in style) {
      var value = style[name];
      if(name == 'opacity') {
        if (value == 1) {
          value = (/Gecko/.test(navigator.userAgent) &&
            !/Konqueror|Safari|KHTML/.test(navigator.userAgent)) ? 0.999999 : 1.0;
          if(/MSIE/.test(navigator.userAgent) && !window.opera)
            element.style.filter = element.getStyle('filter').replace(/alpha\([^\)]*\)/gi,'');
        } else if(value === '') {
          if(/MSIE/.test(navigator.userAgent) && !window.opera)
            element.style.filter = element.getStyle('filter').replace(/alpha\([^\)]*\)/gi,'');
        } else {
          if(value < 0.00001) value = 0;
          if(/MSIE/.test(navigator.userAgent) && !window.opera)
            element.style.filter = element.getStyle('filter').replace(/alpha\([^\)]*\)/gi,'') +
              'alpha(opacity='+value*100+')';
        }
      } else if(['float','cssFloat'].include(name)) name = (typeof element.style.styleFloat != 'undefined') ? 'styleFloat' : 'cssFloat';
      element.style[name.camelize()] = value;
    }
    return element;
  },

  getDimensions: function(element) {
    element = $(element);
    var display = $(element).getStyle('display');
    if (display != 'none' && display != null) // Safari bug
      return {width: element.offsetWidth, height: element.offsetHeight};

    // All *Width and *Height properties give 0 on elements with display none,
    // so enable the element temporarily
    var els = element.style;
    var originalVisibility = els.visibility;
    var originalPosition = els.position;
    var originalDisplay = els.display;
    els.visibility = 'hidden';
    els.position = 'absolute';
    els.display = 'block';
    var originalWidth = element.clientWidth;
    var originalHeight = element.clientHeight;
    els.display = originalDisplay;
    els.position = originalPosition;
    els.visibility = originalVisibility;
    return {width: originalWidth, height: originalHeight};
  },

  makePositioned: function(element) {
    element = $(element);
    var pos = Element.getStyle(element, 'position');
    if (pos == 'static' || !pos) {
      element._madePositioned = true;
      element.style.position = 'relative';
      // Opera returns the offset relative to the positioning context, when an
      // element is position relative but top and left have not been defined
      if (window.opera) {
        element.style.top = 0;
        element.style.left = 0;
      }
    }
    return element;
  },

  undoPositioned: function(element) {
    element = $(element);
    if (element._madePositioned) {
      element._madePositioned = undefined;
      element.style.position =
        element.style.top =
        element.style.left =
        element.style.bottom =
        element.style.right = '';
    }
    return element;
  },

  makeClipping: function(element) {
    element = $(element);
    if (element._overflow) return element;
    element._overflow = element.style.overflow || 'auto';
    if ((Element.getStyle(element, 'overflow') || 'visible') != 'hidden')
      element.style.overflow = 'hidden';
    return element;
  },

  undoClipping: function(element) {
    element = $(element);
    if (!element._overflow) return element;
    element.style.overflow = element._overflow == 'auto' ? '' : element._overflow;
    element._overflow = null;
    return element;
  }
};

Object.extend(Element.Methods, {childOf: Element.Methods.descendantOf});

Element._attributeTranslations = {};

Element._attributeTranslations.names = {
  colspan:   "colSpan",
  rowspan:   "rowSpan",
  valign:    "vAlign",
  datetime:  "dateTime",
  accesskey: "accessKey",
  tabindex:  "tabIndex",
  enctype:   "encType",
  maxlength: "maxLength",
  readonly:  "readOnly",
  longdesc:  "longDesc"
};

Element._attributeTranslations.values = {
  _getAttr: function(element, attribute) {
    return element.getAttribute(attribute, 2);
  },

  _flag: function(element, attribute) {
    return $(element).hasAttribute(attribute) ? attribute : null;
  },

  style: function(element) {
    return element.style.cssText.toLowerCase();
  },

  title: function(element) {
    var node = element.getAttributeNode('title');
    return node.specified ? node.nodeValue : null;
  }
};

Object.extend(Element._attributeTranslations.values, {
  href: Element._attributeTranslations.values._getAttr,
  src:  Element._attributeTranslations.values._getAttr,
  disabled: Element._attributeTranslations.values._flag,
  checked:  Element._attributeTranslations.values._flag,
  readonly: Element._attributeTranslations.values._flag,
  multiple: Element._attributeTranslations.values._flag
});

Element.Methods.Simulated = {
  hasAttribute: function(element, attribute) {
    var t = Element._attributeTranslations;
    attribute = t.names[attribute] || attribute;
    return $(element).getAttributeNode(attribute).specified;
  }
};

// IE is missing .innerHTML support for TABLE-related elements
if (document.all && !window.opera){
  Element.Methods.update = function(element, html) {
    element = $(element);
    html = typeof html == 'undefined' ? '' : html.toString();
    var tagName = element.tagName.toUpperCase();
    if (['THEAD','TBODY','TR','TD'].include(tagName)) {
      var div = document.createElement('div');
      switch (tagName) {
        case 'THEAD':
        case 'TBODY':
          div.innerHTML = '<table><tbody>' +  html.stripScripts() + '</tbody></table>';
          depth = 2;
          break;
        case 'TR':
          div.innerHTML = '<table><tbody><tr>' +  html.stripScripts() + '</tr></tbody></table>';
          depth = 3;
          break;
        case 'TD':
          div.innerHTML = '<table><tbody><tr><td>' +  html.stripScripts() + '</td></tr></tbody></table>';
          depth = 4;
      }
      $A(element.childNodes).each(function(node){
        element.removeChild(node)
      });
      depth.times(function(){ div = div.firstChild });

      $A(div.childNodes).each(
        function(node){ element.appendChild(node) });
    } else {
      element.innerHTML = html.stripScripts();
    }
    setTimeout(function() {html.evalScripts()}, 10);
    return element;
  }
};

Object.extend(Element, Element.Methods);

var _nativeExtensions = false;

if(/Konqueror|Safari|KHTML/.test(navigator.userAgent))
  ['', 'Form', 'Input', 'TextArea', 'Select'].each(function(tag) {
    var className = 'HTML' + tag + 'Element';
    if(window[className]) return;
    var klass = window[className] = {};
    klass.prototype = document.createElement(tag ? tag.toLowerCase() : 'div').__proto__;
  });

Element.addMethods = function(methods) {
  Object.extend(Element.Methods, methods || {});

  function copy(methods, destination, onlyIfAbsent) {
    onlyIfAbsent = onlyIfAbsent || false;
    var cache = Element.extend.cache;
    for (var property in methods) {
      var value = methods[property];
      if (!onlyIfAbsent || !(property in destination))
        destination[property] = cache.findOrStore(value);
    }
  }

  if (typeof HTMLElement != 'undefined') {
    copy(Element.Methods, HTMLElement.prototype);
    copy(Element.Methods.Simulated, HTMLElement.prototype, true);
    copy(Form.Methods, HTMLFormElement.prototype);
    [HTMLInputElement, HTMLTextAreaElement, HTMLSelectElement].each(function(klass) {
      copy(Form.Element.Methods, klass.prototype);
    });
    _nativeExtensions = true;
  }
}

var Toggle = new Object();
Toggle.display = Element.toggle;

/*--------------------------------------------------------------------------*/

Abstract.Insertion = function(adjacency) {
  this.adjacency = adjacency;
}

Abstract.Insertion.prototype = {
  initialize: function(element, content) {
    this.element = $(element);
    this.content = content.stripScripts();

    if (this.adjacency && this.element.insertAdjacentHTML) {
      try {
        this.element.insertAdjacentHTML(this.adjacency, this.content);
      } catch (e) {
        var tagName = this.element.tagName.toUpperCase();
        if (['TBODY', 'TR'].include(tagName)) {
          this.insertContent(this.contentFromAnonymousTable());
        } else {
          throw e;
        }
      }
    } else {
      this.range = this.element.ownerDocument.createRange();
      if (this.initializeRange) this.initializeRange();
      this.insertContent([this.range.createContextualFragment(this.content)]);
    }

    setTimeout(function() {content.evalScripts()}, 10);
  },

  contentFromAnonymousTable: function() {
    var div = document.createElement('div');
    div.innerHTML = '<table><tbody>' + this.content + '</tbody></table>';
    return $A(div.childNodes[0].childNodes[0].childNodes);
  }
}

var Insertion = new Object();

Insertion.Before = Class.create();
Insertion.Before.prototype = Object.extend(new Abstract.Insertion('beforeBegin'), {
  initializeRange: function() {
    this.range.setStartBefore(this.element);
  },

  insertContent: function(fragments) {
    fragments.each((function(fragment) {
      this.element.parentNode.insertBefore(fragment, this.element);
    }).bind(this));
  }
});

Insertion.Top = Class.create();
Insertion.Top.prototype = Object.extend(new Abstract.Insertion('afterBegin'), {
  initializeRange: function() {
    this.range.selectNodeContents(this.element);
    this.range.collapse(true);
  },

  insertContent: function(fragments) {
    fragments.reverse(false).each((function(fragment) {
      this.element.insertBefore(fragment, this.element.firstChild);
    }).bind(this));
  }
});

Insertion.Bottom = Class.create();
Insertion.Bottom.prototype = Object.extend(new Abstract.Insertion('beforeEnd'), {
  initializeRange: function() {
    this.range.selectNodeContents(this.element);
    this.range.collapse(this.element);
  },

  insertContent: function(fragments) {
    fragments.each((function(fragment) {
      this.element.appendChild(fragment);
    }).bind(this));
  }
});

Insertion.After = Class.create();
Insertion.After.prototype = Object.extend(new Abstract.Insertion('afterEnd'), {
  initializeRange: function() {
    this.range.setStartAfter(this.element);
  },

  insertContent: function(fragments) {
    fragments.each((function(fragment) {
      this.element.parentNode.insertBefore(fragment,
        this.element.nextSibling);
    }).bind(this));
  }
});

/*--------------------------------------------------------------------------*/

Element.ClassNames = Class.create();
Element.ClassNames.prototype = {
  initialize: function(element) {
    this.element = $(element);
  },

  _each: function(iterator) {
    this.element.className.split(/\s+/).select(function(name) {
      return name.length > 0;
    })._each(iterator);
  },

  set: function(className) {
    this.element.className = className;
  },

  add: function(classNameToAdd) {
    if (this.include(classNameToAdd)) return;
    this.set($A(this).concat(classNameToAdd).join(' '));
  },

  remove: function(classNameToRemove) {
    if (!this.include(classNameToRemove)) return;
    this.set($A(this).without(classNameToRemove).join(' '));
  },

  toString: function() {
    return $A(this).join(' ');
  }
};

Object.extend(Element.ClassNames.prototype, Enumerable);
var Selector = Class.create();
Selector.prototype = {
  initialize: function(expression) {
    this.params = {classNames: []};
    this.expression = expression.toString().strip();
    this.parseExpression();
    this.compileMatcher();
  },

  parseExpression: function() {
    function abort(message) { throw 'Parse error in selector: ' + message; }

    if (this.expression == '')  abort('empty expression');

    var params = this.params, expr = this.expression, match, modifier, clause, rest;
    while (match = expr.match(/^(.*)\[([a-z0-9_:-]+?)(?:([~\|!]?=)(?:"([^"]*)"|([^\]\s]*)))?\]$/i)) {
      params.attributes = params.attributes || [];
      params.attributes.push({name: match[2], operator: match[3], value: match[4] || match[5] || ''});
      expr = match[1];
    }

    if (expr == '*') return this.params.wildcard = true;

    while (match = expr.match(/^([^a-z0-9_-])?([a-z0-9_-]+)(.*)/i)) {
      modifier = match[1], clause = match[2], rest = match[3];
      switch (modifier) {
        case '#':       params.id = clause; break;
        case '.':       params.classNames.push(clause); break;
        case '':
        case undefined: params.tagName = clause.toUpperCase(); break;
        default:        abort(expr.inspect());
      }
      expr = rest;
    }

    if (expr.length > 0) abort(expr.inspect());
  },

  buildMatchExpression: function() {
    var params = this.params, conditions = [], clause;

    if (params.wildcard)
      conditions.push('true');
    if (clause = params.id)
      conditions.push('element.readAttribute("id") == ' + clause.inspect());
    if (clause = params.tagName)
      conditions.push('element.tagName.toUpperCase() == ' + clause.inspect());
    if ((clause = params.classNames).length > 0)
      for (var i = 0, length = clause.length; i < length; i++)
        conditions.push('element.hasClassName(' + clause[i].inspect() + ')');
    if (clause = params.attributes) {
      clause.each(function(attribute) {
        var value = 'element.readAttribute(' + attribute.name.inspect() + ')';
        var splitValueBy = function(delimiter) {
          return value + ' && ' + value + '.split(' + delimiter.inspect() + ')';
        }

        switch (attribute.operator) {
          case '=':       conditions.push(value + ' == ' + attribute.value.inspect()); break;
          case '~=':      conditions.push(splitValueBy(' ') + '.include(' + attribute.value.inspect() + ')'); break;
          case '|=':      conditions.push(
                            splitValueBy('-') + '.first().toUpperCase() == ' + attribute.value.toUpperCase().inspect()
                          ); break;
          case '!=':      conditions.push(value + ' != ' + attribute.value.inspect()); break;
          case '':
          case undefined: conditions.push('element.hasAttribute(' + attribute.name.inspect() + ')'); break;
          default:        throw 'Unknown operator ' + attribute.operator + ' in selector';
        }
      });
    }

    return conditions.join(' && ');
  },

  compileMatcher: function() {
    this.match = new Function('element', 'if (!element.tagName) return false; element = $(element); return ' + this.buildMatchExpression());
  },

  findElements: function(scope) {
    var element;

    if (element = $(this.params.id))
      if (this.match(element))
        if (!scope || Element.childOf(element, scope))
          return [element];

    scope = (scope || document).getElementsByTagName(this.params.tagName || '*');

    var results = [];
    for (var i = 0, length = scope.length; i < length; i++)
      if (this.match(element = scope[i]))
        results.push(Element.extend(element));

    return results;
  },

  toString: function() {
    return this.expression;
  }
}

Object.extend(Selector, {
  matchElements: function(elements, expression) {
    var selector = new Selector(expression);
    return elements.select(selector.match.bind(selector)).map(Element.extend);
  },

  findElement: function(elements, expression, index) {
    if (typeof expression == 'number') index = expression, expression = false;
    return Selector.matchElements(elements, expression || '*')[index || 0];
  },

  findChildElements: function(element, expressions) {
    return expressions.map(function(expression) {
      return expression.match(/[^\s"]+(?:"[^"]*"[^\s"]+)*/g).inject([null], function(results, expr) {
        var selector = new Selector(expr);
        return results.inject([], function(elements, result) {
          return elements.concat(selector.findElements(result || element));
        });
      });
    }).flatten();
  }
});

function $$() {
  return Selector.findChildElements(document, $A(arguments));
}
var Form = {
  reset: function(form) {
    $(form).reset();
    return form;
  },

  serializeElements: function(elements, getHash) {
    var data = elements.inject({}, function(result, element) {
      if (!element.disabled && element.name) {
        var key = element.name, value = $(element).getValue();
        if (value != undefined) {
          if (result[key]) {
            if (result[key].constructor != Array) result[key] = [result[key]];
            result[key].push(value);
          }
          else result[key] = value;
        }
      }
      return result;
    });

    return getHash ? data : Hash.toQueryString(data);
  }
};

Form.Methods = {
  serialize: function(form, getHash) {
    return Form.serializeElements(Form.getElements(form), getHash);
  },

  getElements: function(form) {
    return $A($(form).getElementsByTagName('*')).inject([],
      function(elements, child) {
        if (Form.Element.Serializers[child.tagName.toLowerCase()])
          elements.push(Element.extend(child));
        return elements;
      }
    );
  },

  getInputs: function(form, typeName, name) {
    form = $(form);
    var inputs = form.getElementsByTagName('input');

    if (!typeName && !name) return $A(inputs).map(Element.extend);

    for (var i = 0, matchingInputs = [], length = inputs.length; i < length; i++) {
      var input = inputs[i];
      if ((typeName && input.type != typeName) || (name && input.name != name))
        continue;
      matchingInputs.push(Element.extend(input));
    }

    return matchingInputs;
  },

  disable: function(form) {
    form = $(form);
    form.getElements().each(function(element) {
      element.blur();
      element.disabled = 'true';
    });
    return form;
  },

  enable: function(form) {
    form = $(form);
    form.getElements().each(function(element) {
      element.disabled = '';
    });
    return form;
  },

  findFirstElement: function(form) {
    return $(form).getElements().find(function(element) {
      return element.type != 'hidden' && !element.disabled &&
        ['input', 'select', 'textarea'].include(element.tagName.toLowerCase());
    });
  },

  focusFirstElement: function(form) {
    form = $(form);
    form.findFirstElement().activate();
    return form;
  }
}

Object.extend(Form, Form.Methods);

/*--------------------------------------------------------------------------*/

Form.Element = {
  focus: function(element) {
    $(element).focus();
    return element;
  },

  select: function(element) {
    $(element).select();
    return element;
  }
}

Form.Element.Methods = {
  serialize: function(element) {
    element = $(element);
    if (!element.disabled && element.name) {
      var value = element.getValue();
      if (value != undefined) {
        var pair = {};
        pair[element.name] = value;
        return Hash.toQueryString(pair);
      }
    }
    return '';
  },

  getValue: function(element) {
    element = $(element);
    var method = element.tagName.toLowerCase();
    return Form.Element.Serializers[method](element);
  },

  clear: function(element) {
    $(element).value = '';
    return element;
  },

  present: function(element) {
    return $(element).value != '';
  },

  activate: function(element) {
    element = $(element);
    element.focus();
    if (element.select && ( element.tagName.toLowerCase() != 'input' ||
      !['button', 'reset', 'submit'].include(element.type) ) )
      element.select();
    return element;
  },

  disable: function(element) {
    element = $(element);
    element.disabled = true;
    return element;
  },

  enable: function(element) {
    element = $(element);
    element.blur();
    element.disabled = false;
    return element;
  }
}

Object.extend(Form.Element, Form.Element.Methods);
var Field = Form.Element;
var $F = Form.Element.getValue;

/*--------------------------------------------------------------------------*/

Form.Element.Serializers = {
  input: function(element) {
    switch (element.type.toLowerCase()) {
      case 'checkbox':
      case 'radio':
        return Form.Element.Serializers.inputSelector(element);
      default:
        return Form.Element.Serializers.textarea(element);
    }
  },

  inputSelector: function(element) {
    return element.checked ? element.value : null;
  },

  textarea: function(element) {
    return element.value;
  },

  select: function(element) {
    return this[element.type == 'select-one' ?
      'selectOne' : 'selectMany'](element);
  },

  selectOne: function(element) {
    var index = element.selectedIndex;
    return index >= 0 ? this.optionValue(element.options[index]) : null;
  },

  selectMany: function(element) {
    var values, length = element.length;
    if (!length) return null;

    for (var i = 0, values = []; i < length; i++) {
      var opt = element.options[i];
      if (opt.selected) values.push(this.optionValue(opt));
    }
    return values;
  },

  optionValue: function(opt) {
    // extend element because hasAttribute may not be native
    return Element.extend(opt).hasAttribute('value') ? opt.value : opt.text;
  }
}

/*--------------------------------------------------------------------------*/

Abstract.TimedObserver = function() {}
Abstract.TimedObserver.prototype = {
  initialize: function(element, frequency, callback) {
    this.frequency = frequency;
    this.element   = $(element);
    this.callback  = callback;

    this.lastValue = this.getValue();
    this.registerCallback();
  },

  registerCallback: function() {
    setInterval(this.onTimerEvent.bind(this), this.frequency * 1000);
  },

  onTimerEvent: function() {
    var value = this.getValue();
    var changed = ('string' == typeof this.lastValue && 'string' == typeof value
      ? this.lastValue != value : String(this.lastValue) != String(value));
    if (changed) {
      this.callback(this.element, value);
      this.lastValue = value;
    }
  }
}

Form.Element.Observer = Class.create();
Form.Element.Observer.prototype = Object.extend(new Abstract.TimedObserver(), {
  getValue: function() {
    return Form.Element.getValue(this.element);
  }
});

Form.Observer = Class.create();
Form.Observer.prototype = Object.extend(new Abstract.TimedObserver(), {
  getValue: function() {
    return Form.serialize(this.element);
  }
});

/*--------------------------------------------------------------------------*/

Abstract.EventObserver = function() {}
Abstract.EventObserver.prototype = {
  initialize: function(element, callback) {
    this.element  = $(element);
    this.callback = callback;

    this.lastValue = this.getValue();
    if (this.element.tagName.toLowerCase() == 'form')
      this.registerFormCallbacks();
    else
      this.registerCallback(this.element);
  },

  onElementEvent: function() {
    var value = this.getValue();
    if (this.lastValue != value) {
      this.callback(this.element, value);
      this.lastValue = value;
    }
  },

  registerFormCallbacks: function() {
    Form.getElements(this.element).each(this.registerCallback.bind(this));
  },

  registerCallback: function(element) {
    if (element.type) {
      switch (element.type.toLowerCase()) {
        case 'checkbox':
        case 'radio':
          Event.observe(element, 'click', this.onElementEvent.bind(this));
          break;
        default:
          Event.observe(element, 'change', this.onElementEvent.bind(this));
          break;
      }
    }
  }
}

Form.Element.EventObserver = Class.create();
Form.Element.EventObserver.prototype = Object.extend(new Abstract.EventObserver(), {
  getValue: function() {
    return Form.Element.getValue(this.element);
  }
});

Form.EventObserver = Class.create();
Form.EventObserver.prototype = Object.extend(new Abstract.EventObserver(), {
  getValue: function() {
    return Form.serialize(this.element);
  }
});
if (!window.Event) {
  var Event = new Object();
}

Object.extend(Event, {
  KEY_BACKSPACE: 8,
  KEY_TAB:       9,
  KEY_RETURN:   13,
  KEY_ESC:      27,
  KEY_LEFT:     37,
  KEY_UP:       38,
  KEY_RIGHT:    39,
  KEY_DOWN:     40,
  KEY_DELETE:   46,
  KEY_HOME:     36,
  KEY_END:      35,
  KEY_PAGEUP:   33,
  KEY_PAGEDOWN: 34,

  element: function(event) {
    return event.target || event.srcElement;
  },

  isLeftClick: function(event) {
    return (((event.which) && (event.which == 1)) ||
            ((event.button) && (event.button == 1)));
  },

  pointerX: function(event) {
    return event.pageX || (event.clientX +
      (document.documentElement.scrollLeft || document.body.scrollLeft));
  },

  pointerY: function(event) {
    return event.pageY || (event.clientY +
      (document.documentElement.scrollTop || document.body.scrollTop));
  },

  stop: function(event) {
    if (event.preventDefault) {
      event.preventDefault();
      event.stopPropagation();
    } else {
      event.returnValue = false;
      event.cancelBubble = true;
    }
  },

  // find the first node with the given tagName, starting from the
  // node the event was triggered on; traverses the DOM upwards
  findElement: function(event, tagName) {
    var element = Event.element(event);
    while (element.parentNode && (!element.tagName ||
        (element.tagName.toUpperCase() != tagName.toUpperCase())))
      element = element.parentNode;
    return element;
  },

  observers: false,

  _observeAndCache: function(element, name, observer, useCapture) {
    if (!this.observers) this.observers = [];
    if (element.addEventListener) {
      this.observers.push([element, name, observer, useCapture]);
      element.addEventListener(name, observer, useCapture);
    } else if (element.attachEvent) {
      this.observers.push([element, name, observer, useCapture]);
      element.attachEvent('on' + name, observer);
    }
  },

  unloadCache: function() {
    if (!Event.observers) return;
    for (var i = 0, length = Event.observers.length; i < length; i++) {
      Event.stopObserving.apply(this, Event.observers[i]);
      Event.observers[i][0] = null;
    }
    Event.observers = false;
  },

  observe: function(element, name, observer, useCapture) {
    element = $(element);
    useCapture = useCapture || false;

    if (name == 'keypress' &&
        (navigator.appVersion.match(/Konqueror|Safari|KHTML/)
        || element.attachEvent))
      name = 'keydown';

    Event._observeAndCache(element, name, observer, useCapture);
  },

  stopObserving: function(element, name, observer, useCapture) {
    element = $(element);
    useCapture = useCapture || false;

    if (name == 'keypress' &&
        (navigator.appVersion.match(/Konqueror|Safari|KHTML/)
        || element.detachEvent))
      name = 'keydown';

    if (element.removeEventListener) {
      element.removeEventListener(name, observer, useCapture);
    } else if (element.detachEvent) {
      try {
        element.detachEvent('on' + name, observer);
      } catch (e) {}
    }
  }
});

/* prevent memory leaks in IE */
if (navigator.appVersion.match(/\bMSIE\b/))
  Event.observe(window, 'unload', Event.unloadCache, false);
var Position = {
  // set to true if needed, warning: firefox performance problems
  // NOT neeeded for page scrolling, only if draggable contained in
  // scrollable elements
  includeScrollOffsets: false,

  // must be called before calling withinIncludingScrolloffset, every time the
  // page is scrolled
  prepare: function() {
    this.deltaX =  window.pageXOffset
                || document.documentElement.scrollLeft
                || document.body.scrollLeft
                || 0;
    this.deltaY =  window.pageYOffset
                || document.documentElement.scrollTop
                || document.body.scrollTop
                || 0;
  },

  realOffset: function(element) {
    var valueT = 0, valueL = 0;
    do {
      valueT += element.scrollTop  || 0;
      valueL += element.scrollLeft || 0;
      element = element.parentNode;
    } while (element);
    return [valueL, valueT];
  },

  cumulativeOffset: function(element) {
    var valueT = 0, valueL = 0;
    do {
      valueT += element.offsetTop  || 0;
      valueL += element.offsetLeft || 0;
      element = element.offsetParent;
    } while (element);
    return [valueL, valueT];
  },

  positionedOffset: function(element) {
    var valueT = 0, valueL = 0;
    do {
      valueT += element.offsetTop  || 0;
      valueL += element.offsetLeft || 0;
      element = element.offsetParent;
      if (element) {
        if(element.tagName=='BODY') break;
        var p = Element.getStyle(element, 'position');
        if (p == 'relative' || p == 'absolute') break;
      }
    } while (element);
    return [valueL, valueT];
  },

  offsetParent: function(element) {
    if (element.offsetParent) return element.offsetParent;
    if (element == document.body) return element;

    while ((element = element.parentNode) && element != document.body)
      if (Element.getStyle(element, 'position') != 'static')
        return element;

    return document.body;
  },

  // caches x/y coordinate pair to use with overlap
  within: function(element, x, y) {
    if (this.includeScrollOffsets)
      return this.withinIncludingScrolloffsets(element, x, y);
    this.xcomp = x;
    this.ycomp = y;
    this.offset = this.cumulativeOffset(element);

    return (y >= this.offset[1] &&
            y <  this.offset[1] + element.offsetHeight &&
            x >= this.offset[0] &&
            x <  this.offset[0] + element.offsetWidth);
  },

  withinIncludingScrolloffsets: function(element, x, y) {
    var offsetcache = this.realOffset(element);

    this.xcomp = x + offsetcache[0] - this.deltaX;
    this.ycomp = y + offsetcache[1] - this.deltaY;
    this.offset = this.cumulativeOffset(element);

    return (this.ycomp >= this.offset[1] &&
            this.ycomp <  this.offset[1] + element.offsetHeight &&
            this.xcomp >= this.offset[0] &&
            this.xcomp <  this.offset[0] + element.offsetWidth);
  },

  // within must be called directly before
  overlap: function(mode, element) {
    if (!mode) return 0;
    if (mode == 'vertical')
      return ((this.offset[1] + element.offsetHeight) - this.ycomp) /
        element.offsetHeight;
    if (mode == 'horizontal')
      return ((this.offset[0] + element.offsetWidth) - this.xcomp) /
        element.offsetWidth;
  },

  page: function(forElement) {
    var valueT = 0, valueL = 0;

    var element = forElement;
    do {
      valueT += element.offsetTop  || 0;
      valueL += element.offsetLeft || 0;

      // Safari fix
      if (element.offsetParent==document.body)
        if (Element.getStyle(element,'position')=='absolute') break;

    } while (element = element.offsetParent);

    element = forElement;
    do {
      if (!window.opera || element.tagName=='BODY') {
        valueT -= element.scrollTop  || 0;
        valueL -= element.scrollLeft || 0;
      }
    } while (element = element.parentNode);

    return [valueL, valueT];
  },

  clone: function(source, target) {
    var options = Object.extend({
      setLeft:    true,
      setTop:     true,
      setWidth:   true,
      setHeight:  true,
      offsetTop:  0,
      offsetLeft: 0
    }, arguments[2] || {})

    // find page position of source
    source = $(source);
    var p = Position.page(source);

    // find coordinate system to use
    target = $(target);
    var delta = [0, 0];
    var parent = null;
    // delta [0,0] will do fine with position: fixed elements,
    // position:absolute needs offsetParent deltas
    if (Element.getStyle(target,'position') == 'absolute') {
      parent = Position.offsetParent(target);
      delta = Position.page(parent);
    }

    // correct by body offsets (fixes Safari)
    if (parent == document.body) {
      delta[0] -= document.body.offsetLeft;
      delta[1] -= document.body.offsetTop;
    }

    // set position
    if(options.setLeft)   target.style.left  = (p[0] - delta[0] + options.offsetLeft) + 'px';
    if(options.setTop)    target.style.top   = (p[1] - delta[1] + options.offsetTop) + 'px';
    if(options.setWidth)  target.style.width = source.offsetWidth + 'px';
    if(options.setHeight) target.style.height = source.offsetHeight + 'px';
  },

  absolutize: function(element) {
    element = $(element);
    if (element.style.position == 'absolute') return;
    Position.prepare();

    var offsets = Position.positionedOffset(element);
    var top     = offsets[1];
    var left    = offsets[0];
    var width   = element.clientWidth;
    var height  = element.clientHeight;

    element._originalLeft   = left - parseFloat(element.style.left  || 0);
    element._originalTop    = top  - parseFloat(element.style.top || 0);
    element._originalWidth  = element.style.width;
    element._originalHeight = element.style.height;

    element.style.position = 'absolute';
    element.style.top    = top + 'px';
    element.style.left   = left + 'px';
    element.style.width  = width + 'px';
    element.style.height = height + 'px';
  },

  relativize: function(element) {
    element = $(element);
    if (element.style.position == 'relative') return;
    Position.prepare();

    element.style.position = 'relative';
    var top  = parseFloat(element.style.top  || 0) - (element._originalTop || 0);
    var left = parseFloat(element.style.left || 0) - (element._originalLeft || 0);

    element.style.top    = top + 'px';
    element.style.left   = left + 'px';
    element.style.height = element._originalHeight;
    element.style.width  = element._originalWidth;
  }
}

// Safari returns margins on body which is incorrect if the child is absolutely
// positioned.  For performance reasons, redefine Position.cumulativeOffset for
// KHTML/WebKit only.
if (/Konqueror|Safari|KHTML/.test(navigator.userAgent)) {
  Position.cumulativeOffset = function(element) {
    var valueT = 0, valueL = 0;
    do {
      valueT += element.offsetTop  || 0;
      valueL += element.offsetLeft || 0;
      if (element.offsetParent == document.body)
        if (Element.getStyle(element, 'position') == 'absolute') break;

      element = element.offsetParent;
    } while (element);

    return [valueL, valueT];
  }
}

Element.addMethods();// script.aculo.us builder.js v1.7.0, Fri Jan 19 19:16:36 CET 2007

// Copyright (c) 2005, 2006 Thomas Fuchs (http://script.aculo.us, http://mir.aculo.us)
//
// script.aculo.us is freely distributable under the terms of an MIT-style license.
// For details, see the script.aculo.us web site: http://script.aculo.us/

var Builder = {
  NODEMAP: {
    AREA: 'map',
    CAPTION: 'table',
    COL: 'table',
    COLGROUP: 'table',
    LEGEND: 'fieldset',
    OPTGROUP: 'select',
    OPTION: 'select',
    PARAM: 'object',
    TBODY: 'table',
    TD: 'table',
    TFOOT: 'table',
    TH: 'table',
    THEAD: 'table',
    TR: 'table'
  },
  // note: For Firefox < 1.5, OPTION and OPTGROUP tags are currently broken,
  //       due to a Firefox bug
  node: function(elementName) {
    elementName = elementName.toUpperCase();
    
    // try innerHTML approach
    var parentTag = this.NODEMAP[elementName] || 'div';
    var parentElement = document.createElement(parentTag);
    try { // prevent IE "feature": http://dev.rubyonrails.org/ticket/2707
      parentElement.innerHTML = "<" + elementName + "></" + elementName + ">";
    } catch(e) {}
    var element = parentElement.firstChild || null;
      
    // see if browser added wrapping tags
    if(element && (element.tagName.toUpperCase() != elementName))
      element = element.getElementsByTagName(elementName)[0];
    
    // fallback to createElement approach
    if(!element) element = document.createElement(elementName);
    
    // abort if nothing could be created
    if(!element) return;

    // attributes (or text)
    if(arguments[1])
      if(this._isStringOrNumber(arguments[1]) ||
        (arguments[1] instanceof Array)) {
          this._children(element, arguments[1]);
        } else {
          var attrs = this._attributes(arguments[1]);
          if(attrs.length) {
            try { // prevent IE "feature": http://dev.rubyonrails.org/ticket/2707
              parentElement.innerHTML = "<" +elementName + " " +
                attrs + "></" + elementName + ">";
            } catch(e) {}
            element = parentElement.firstChild || null;
            // workaround firefox 1.0.X bug
            if(!element) {
              element = document.createElement(elementName);
              for(attr in arguments[1]) 
                element[attr == 'class' ? 'className' : attr] = arguments[1][attr];
            }
            if(element.tagName.toUpperCase() != elementName)
              element = parentElement.getElementsByTagName(elementName)[0];
            }
        } 

    // text, or array of children
    if(arguments[2])
      this._children(element, arguments[2]);

     return element;
  },
  _text: function(text) {
     return document.createTextNode(text);
  },

  ATTR_MAP: {
    'className': 'class',
    'htmlFor': 'for'
  },

  _attributes: function(attributes) {
    var attrs = [];
    for(attribute in attributes)
      attrs.push((attribute in this.ATTR_MAP ? this.ATTR_MAP[attribute] : attribute) +
          '="' + attributes[attribute].toString().escapeHTML() + '"');
    return attrs.join(" ");
  },
  _children: function(element, children) {
    if(typeof children=='object') { // array can hold nodes and text
      children.flatten().each( function(e) {
        if(typeof e=='object')
          element.appendChild(e)
        else
          if(Builder._isStringOrNumber(e))
            element.appendChild(Builder._text(e));
      });
    } else
      if(Builder._isStringOrNumber(children)) 
         element.appendChild(Builder._text(children));
  },
  _isStringOrNumber: function(param) {
    return(typeof param=='string' || typeof param=='number');
  },
  build: function(html) {
    var element = this.node('div');
    $(element).update(html.strip());
    return element.down();
  },
  dump: function(scope) { 
    if(typeof scope != 'object' && typeof scope != 'function') scope = window; //global scope 
  
    var tags = ("A ABBR ACRONYM ADDRESS APPLET AREA B BASE BASEFONT BDO BIG BLOCKQUOTE BODY " +
      "BR BUTTON CAPTION CENTER CITE CODE COL COLGROUP DD DEL DFN DIR DIV DL DT EM FIELDSET " +
      "FONT FORM FRAME FRAMESET H1 H2 H3 H4 H5 H6 HEAD HR HTML I IFRAME IMG INPUT INS ISINDEX "+
      "KBD LABEL LEGEND LI LINK MAP MENU META NOFRAMES NOSCRIPT OBJECT OL OPTGROUP OPTION P "+
      "PARAM PRE Q S SAMP SCRIPT SELECT SMALL SPAN STRIKE STRONG STYLE SUB SUP TABLE TBODY TD "+
      "TEXTAREA TFOOT TH THEAD TITLE TR TT U UL VAR").split(/\s+/);
  
    tags.each( function(tag){ 
      scope[tag] = function() { 
        return Builder.node.apply(Builder, [tag].concat($A(arguments)));  
      } 
    });
  }
}
// script.aculo.us effects.js v1.7.0, Fri Jan 19 19:16:36 CET 2007

// Copyright (c) 2005, 2006 Thomas Fuchs (http://script.aculo.us, http://mir.aculo.us)
// Contributors:
//  Justin Palmer (http://encytemedia.com/)
//  Mark Pilgrim (http://diveintomark.org/)
//  Martin Bialasinki
// 
// script.aculo.us is freely distributable under the terms of an MIT-style license.
// For details, see the script.aculo.us web site: http://script.aculo.us/ 

// converts rgb() and #xxx to #xxxxxx format,  
// returns self (or first argument) if not convertable  
String.prototype.parseColor = function() {  
  var color = '#';
  if(this.slice(0,4) == 'rgb(') {  
    var cols = this.slice(4,this.length-1).split(',');  
    var i=0; do { color += parseInt(cols[i]).toColorPart() } while (++i<3);  
  } else {  
    if(this.slice(0,1) == '#') {  
      if(this.length==4) for(var i=1;i<4;i++) color += (this.charAt(i) + this.charAt(i)).toLowerCase();  
      if(this.length==7) color = this.toLowerCase();  
    }  
  }  
  return(color.length==7 ? color : (arguments[0] || this));  
}

/*--------------------------------------------------------------------------*/

Element.collectTextNodes = function(element) {  
  return $A($(element).childNodes).collect( function(node) {
    return (node.nodeType==3 ? node.nodeValue : 
      (node.hasChildNodes() ? Element.collectTextNodes(node) : ''));
  }).flatten().join('');
}

Element.collectTextNodesIgnoreClass = function(element, className) {  
  return $A($(element).childNodes).collect( function(node) {
    return (node.nodeType==3 ? node.nodeValue : 
      ((node.hasChildNodes() && !Element.hasClassName(node,className)) ? 
        Element.collectTextNodesIgnoreClass(node, className) : ''));
  }).flatten().join('');
}

Element.setContentZoom = function(element, percent) {
  element = $(element);  
  element.setStyle({fontSize: (percent/100) + 'em'});   
  if(navigator.appVersion.indexOf('AppleWebKit')>0) window.scrollBy(0,0);
  return element;
}

Element.getOpacity = function(element){
  return $(element).getStyle('opacity');
}

Element.setOpacity = function(element, value){
  return $(element).setStyle({opacity:value});
}

Element.getInlineOpacity = function(element){
  return $(element).style.opacity || '';
}

Element.forceRerendering = function(element) {
  try {
    element = $(element);
    var n = document.createTextNode(' ');
    element.appendChild(n);
    element.removeChild(n);
  } catch(e) { }
};

/*--------------------------------------------------------------------------*/

Array.prototype.call = function() {
  var args = arguments;
  this.each(function(f){ f.apply(this, args) });
}

/*--------------------------------------------------------------------------*/

var Effect = {
  _elementDoesNotExistError: {
    name: 'ElementDoesNotExistError',
    message: 'The specified DOM element does not exist, but is required for this effect to operate'
  },
  tagifyText: function(element) {
    if(typeof Builder == 'undefined')
      throw("Effect.tagifyText requires including script.aculo.us' builder.js library");
      
    var tagifyStyle = 'position:relative';
    if(/MSIE/.test(navigator.userAgent) && !window.opera) tagifyStyle += ';zoom:1';
    
    element = $(element);
    $A(element.childNodes).each( function(child) {
      if(child.nodeType==3) {
        child.nodeValue.toArray().each( function(character) {
          element.insertBefore(
            Builder.node('span',{style: tagifyStyle},
              character == ' ' ? String.fromCharCode(160) : character), 
              child);
        });
        Element.remove(child);
      }
    });
  },
  multiple: function(element, effect) {
    var elements;
    if(((typeof element == 'object') || 
        (typeof element == 'function')) && 
       (element.length))
      elements = element;
    else
      elements = $(element).childNodes;
      
    var options = Object.extend({
      speed: 0.1,
      delay: 0.0
    }, arguments[2] || {});
    var masterDelay = options.delay;

    $A(elements).each( function(element, index) {
      new effect(element, Object.extend(options, { delay: index * options.speed + masterDelay }));
    });
  },
  PAIRS: {
    'slide':  ['SlideDown','SlideUp'],
    'blind':  ['BlindDown','BlindUp'],
    'appear': ['Appear','Fade']
  },
  toggle: function(element, effect) {
    element = $(element);
    effect = (effect || 'appear').toLowerCase();
    var options = Object.extend({
      queue: { position:'end', scope:(element.id || 'global'), limit: 1 }
    }, arguments[2] || {});
    Effect[element.visible() ? 
      Effect.PAIRS[effect][1] : Effect.PAIRS[effect][0]](element, options);
  }
};

var Effect2 = Effect; // deprecated

/* ------------- transitions ------------- */

Effect.Transitions = {
  linear: Prototype.K,
  sinoidal: function(pos) {
    return (-Math.cos(pos*Math.PI)/2) + 0.5;
  },
  reverse: function(pos) {
    return 1-pos;
  },
  flicker: function(pos) {
    return ((-Math.cos(pos*Math.PI)/4) + 0.75) + Math.random()/4;
  },
  wobble: function(pos) {
    return (-Math.cos(pos*Math.PI*(9*pos))/2) + 0.5;
  },
  pulse: function(pos, pulses) { 
    pulses = pulses || 5; 
    return (
      Math.round((pos % (1/pulses)) * pulses) == 0 ? 
            ((pos * pulses * 2) - Math.floor(pos * pulses * 2)) : 
        1 - ((pos * pulses * 2) - Math.floor(pos * pulses * 2))
      );
  },
  none: function(pos) {
    return 0;
  },
  full: function(pos) {
    return 1;
  }
};

/* ------------- core effects ------------- */

Effect.ScopedQueue = Class.create();
Object.extend(Object.extend(Effect.ScopedQueue.prototype, Enumerable), {
  initialize: function() {
    this.effects  = [];
    this.interval = null;
  },
  _each: function(iterator) {
    this.effects._each(iterator);
  },
  add: function(effect) {
    var timestamp = new Date().getTime();
    
    var position = (typeof effect.options.queue == 'string') ? 
      effect.options.queue : effect.options.queue.position;
    
    switch(position) {
      case 'front':
        // move unstarted effects after this effect  
        this.effects.findAll(function(e){ return e.state=='idle' }).each( function(e) {
            e.startOn  += effect.finishOn;
            e.finishOn += effect.finishOn;
          });
        break;
      case 'with-last':
        timestamp = this.effects.pluck('startOn').max() || timestamp;
        break;
      case 'end':
        // start effect after last queued effect has finished
        timestamp = this.effects.pluck('finishOn').max() || timestamp;
        break;
    }
    
    effect.startOn  += timestamp;
    effect.finishOn += timestamp;

    if(!effect.options.queue.limit || (this.effects.length < effect.options.queue.limit))
      this.effects.push(effect);
    
    if(!this.interval) 
      this.interval = setInterval(this.loop.bind(this), 15);
  },
  remove: function(effect) {
    this.effects = this.effects.reject(function(e) { return e==effect });
    if(this.effects.length == 0) {
      clearInterval(this.interval);
      this.interval = null;
    }
  },
  loop: function() {
    var timePos = new Date().getTime();
    for(var i=0, len=this.effects.length;i<len;i++) 
      if(this.effects[i]) this.effects[i].loop(timePos);
  }
});

Effect.Queues = {
  instances: $H(),
  get: function(queueName) {
    if(typeof queueName != 'string') return queueName;
    
    if(!this.instances[queueName])
      this.instances[queueName] = new Effect.ScopedQueue();
      
    return this.instances[queueName];
  }
}
Effect.Queue = Effect.Queues.get('global');

Effect.DefaultOptions = {
  transition: Effect.Transitions.sinoidal,
  duration:   1.0,   // seconds
  fps:        60.0,  // max. 60fps due to Effect.Queue implementation
  sync:       false, // true for combining
  from:       0.0,
  to:         1.0,
  delay:      0.0,
  queue:      'parallel'
}

Effect.Base = function() {};
Effect.Base.prototype = {
  position: null,
  start: function(options) {
    this.options      = Object.extend(Object.extend({},Effect.DefaultOptions), options || {});
    this.currentFrame = 0;
    this.state        = 'idle';
    this.startOn      = this.options.delay*1000;
    this.finishOn     = this.startOn + (this.options.duration*1000);
    this.event('beforeStart');
    if(!this.options.sync)
      Effect.Queues.get(typeof this.options.queue == 'string' ? 
        'global' : this.options.queue.scope).add(this);
  },
  loop: function(timePos) {
    if(timePos >= this.startOn) {
      if(timePos >= this.finishOn) {
        this.render(1.0);
        this.cancel();
        this.event('beforeFinish');
        if(this.finish) this.finish(); 
        this.event('afterFinish');
        return;  
      }
      var pos   = (timePos - this.startOn) / (this.finishOn - this.startOn);
      var frame = Math.round(pos * this.options.fps * this.options.duration);
      if(frame > this.currentFrame) {
        this.render(pos);
        this.currentFrame = frame;
      }
    }
  },
  render: function(pos) {
    if(this.state == 'idle') {
      this.state = 'running';
      this.event('beforeSetup');
      if(this.setup) this.setup();
      this.event('afterSetup');
    }
    if(this.state == 'running') {
      if(this.options.transition) pos = this.options.transition(pos);
      pos *= (this.options.to-this.options.from);
      pos += this.options.from;
      this.position = pos;
      this.event('beforeUpdate');
      if(this.update) this.update(pos);
      this.event('afterUpdate');
    }
  },
  cancel: function() {
    if(!this.options.sync)
      Effect.Queues.get(typeof this.options.queue == 'string' ? 
        'global' : this.options.queue.scope).remove(this);
    this.state = 'finished';
  },
  event: function(eventName) {
    if(this.options[eventName + 'Internal']) this.options[eventName + 'Internal'](this);
    if(this.options[eventName]) this.options[eventName](this);
  },
  inspect: function() {
    var data = $H();
    for(property in this)
      if(typeof this[property] != 'function') data[property] = this[property];
    return '#<Effect:' + data.inspect() + ',options:' + $H(this.options).inspect() + '>';
  }
}

Effect.Parallel = Class.create();
Object.extend(Object.extend(Effect.Parallel.prototype, Effect.Base.prototype), {
  initialize: function(effects) {
    this.effects = effects || [];
    this.start(arguments[1]);
  },
  update: function(position) {
    this.effects.invoke('render', position);
  },
  finish: function(position) {
    this.effects.each( function(effect) {
      effect.render(1.0);
      effect.cancel();
      effect.event('beforeFinish');
      if(effect.finish) effect.finish(position);
      effect.event('afterFinish');
    });
  }
});

Effect.Event = Class.create();
Object.extend(Object.extend(Effect.Event.prototype, Effect.Base.prototype), {
  initialize: function() {
    var options = Object.extend({
      duration: 0
    }, arguments[0] || {});
    this.start(options);
  },
  update: Prototype.emptyFunction
});

Effect.Opacity = Class.create();
Object.extend(Object.extend(Effect.Opacity.prototype, Effect.Base.prototype), {
  initialize: function(element) {
    this.element = $(element);
    if(!this.element) throw(Effect._elementDoesNotExistError);
    // make this work on IE on elements without 'layout'
    if(/MSIE/.test(navigator.userAgent) && !window.opera && (!this.element.currentStyle.hasLayout))
      this.element.setStyle({zoom: 1});
    var options = Object.extend({
      from: this.element.getOpacity() || 0.0,
      to:   1.0
    }, arguments[1] || {});
    this.start(options);
  },
  update: function(position) {
    this.element.setOpacity(position);
  }
});

Effect.Move = Class.create();
Object.extend(Object.extend(Effect.Move.prototype, Effect.Base.prototype), {
  initialize: function(element) {
    this.element = $(element);
    if(!this.element) throw(Effect._elementDoesNotExistError);
    var options = Object.extend({
      x:    0,
      y:    0,
      mode: 'relative'
    }, arguments[1] || {});
    this.start(options);
  },
  setup: function() {
    // Bug in Opera: Opera returns the "real" position of a static element or
    // relative element that does not have top/left explicitly set.
    // ==> Always set top and left for position relative elements in your stylesheets 
    // (to 0 if you do not need them) 
    this.element.makePositioned();
    this.originalLeft = parseFloat(this.element.getStyle('left') || '0');
    this.originalTop  = parseFloat(this.element.getStyle('top')  || '0');
    if(this.options.mode == 'absolute') {
      // absolute movement, so we need to calc deltaX and deltaY
      this.options.x = this.options.x - this.originalLeft;
      this.options.y = this.options.y - this.originalTop;
    }
  },
  update: function(position) {
    this.element.setStyle({
      left: Math.round(this.options.x  * position + this.originalLeft) + 'px',
      top:  Math.round(this.options.y  * position + this.originalTop)  + 'px'
    });
  }
});

// for backwards compatibility
Effect.MoveBy = function(element, toTop, toLeft) {
  return new Effect.Move(element, 
    Object.extend({ x: toLeft, y: toTop }, arguments[3] || {}));
};

Effect.Scale = Class.create();
Object.extend(Object.extend(Effect.Scale.prototype, Effect.Base.prototype), {
  initialize: function(element, percent) {
    this.element = $(element);
    if(!this.element) throw(Effect._elementDoesNotExistError);
    var options = Object.extend({
      scaleX: true,
      scaleY: true,
      scaleContent: true,
      scaleFromCenter: false,
      scaleMode: 'box',        // 'box' or 'contents' or {} with provided values
      scaleFrom: 100.0,
      scaleTo:   percent
    }, arguments[2] || {});
    this.start(options);
  },
  setup: function() {
    this.restoreAfterFinish = this.options.restoreAfterFinish || false;
    this.elementPositioning = this.element.getStyle('position');
    
    this.originalStyle = {};
    ['top','left','width','height','fontSize'].each( function(k) {
      this.originalStyle[k] = this.element.style[k];
    }.bind(this));
      
    this.originalTop  = this.element.offsetTop;
    this.originalLeft = this.element.offsetLeft;
    
    var fontSize = this.element.getStyle('font-size') || '100%';
    ['em','px','%','pt'].each( function(fontSizeType) {
      if(fontSize.indexOf(fontSizeType)>0) {
        this.fontSize     = parseFloat(fontSize);
        this.fontSizeType = fontSizeType;
      }
    }.bind(this));
    
    this.factor = (this.options.scaleTo - this.options.scaleFrom)/100;
    
    this.dims = null;
    if(this.options.scaleMode=='box')
      this.dims = [this.element.offsetHeight, this.element.offsetWidth];
    if(/^content/.test(this.options.scaleMode))
      this.dims = [this.element.scrollHeight, this.element.scrollWidth];
    if(!this.dims)
      this.dims = [this.options.scaleMode.originalHeight,
                   this.options.scaleMode.originalWidth];
  },
  update: function(position) {
    var currentScale = (this.options.scaleFrom/100.0) + (this.factor * position);
    if(this.options.scaleContent && this.fontSize)
      this.element.setStyle({fontSize: this.fontSize * currentScale + this.fontSizeType });
    this.setDimensions(this.dims[0] * currentScale, this.dims[1] * currentScale);
  },
  finish: function(position) {
    if(this.restoreAfterFinish) this.element.setStyle(this.originalStyle);
  },
  setDimensions: function(height, width) {
    var d = {};
    if(this.options.scaleX) d.width = Math.round(width) + 'px';
    if(this.options.scaleY) d.height = Math.round(height) + 'px';
    if(this.options.scaleFromCenter) {
      var topd  = (height - this.dims[0])/2;
      var leftd = (width  - this.dims[1])/2;
      if(this.elementPositioning == 'absolute') {
        if(this.options.scaleY) d.top = this.originalTop-topd + 'px';
        if(this.options.scaleX) d.left = this.originalLeft-leftd + 'px';
      } else {
        if(this.options.scaleY) d.top = -topd + 'px';
        if(this.options.scaleX) d.left = -leftd + 'px';
      }
    }
    this.element.setStyle(d);
  }
});

Effect.Highlight = Class.create();
Object.extend(Object.extend(Effect.Highlight.prototype, Effect.Base.prototype), {
  initialize: function(element) {
    this.element = $(element);
    if(!this.element) throw(Effect._elementDoesNotExistError);
    var options = Object.extend({ startcolor: '#ffff99' }, arguments[1] || {});
    this.start(options);
  },
  setup: function() {
    // Prevent executing on elements not in the layout flow
    if(this.element.getStyle('display')=='none') { this.cancel(); return; }
    // Disable background image during the effect
    this.oldStyle = {};
    if (!this.options.keepBackgroundImage) {
      this.oldStyle.backgroundImage = this.element.getStyle('background-image');
      this.element.setStyle({backgroundImage: 'none'});
    }
    if(!this.options.endcolor)
      this.options.endcolor = this.element.getStyle('background-color').parseColor('#ffffff');
    if(!this.options.restorecolor)
      this.options.restorecolor = this.element.getStyle('background-color');
    // init color calculations
    this._base  = $R(0,2).map(function(i){ return parseInt(this.options.startcolor.slice(i*2+1,i*2+3),16) }.bind(this));
    this._delta = $R(0,2).map(function(i){ return parseInt(this.options.endcolor.slice(i*2+1,i*2+3),16)-this._base[i] }.bind(this));
  },
  update: function(position) {
    this.element.setStyle({backgroundColor: $R(0,2).inject('#',function(m,v,i){
      return m+(Math.round(this._base[i]+(this._delta[i]*position)).toColorPart()); }.bind(this)) });
  },
  finish: function() {
    this.element.setStyle(Object.extend(this.oldStyle, {
      backgroundColor: this.options.restorecolor
    }));
  }
});

Effect.ScrollTo = Class.create();
Object.extend(Object.extend(Effect.ScrollTo.prototype, Effect.Base.prototype), {
  initialize: function(element) {
    this.element = $(element);
    this.start(arguments[1] || {});
  },
  setup: function() {
    Position.prepare();
    var offsets = Position.cumulativeOffset(this.element);
    if(this.options.offset) offsets[1] += this.options.offset;
    var max = window.innerHeight ? 
      window.height - window.innerHeight :
      document.body.scrollHeight - 
        (document.documentElement.clientHeight ? 
          document.documentElement.clientHeight : document.body.clientHeight);
    this.scrollStart = Position.deltaY;
    this.delta = (offsets[1] > max ? max : offsets[1]) - this.scrollStart;
  },
  update: function(position) {
    Position.prepare();
    window.scrollTo(Position.deltaX, 
      this.scrollStart + (position*this.delta));
  }
});

/* ------------- combination effects ------------- */

Effect.Fade = function(element) {
  element = $(element);
  var oldOpacity = element.getInlineOpacity();
  var options = Object.extend({
  from: element.getOpacity() || 1.0,
  to:   0.0,
  afterFinishInternal: function(effect) { 
    if(effect.options.to!=0) return;
    effect.element.hide().setStyle({opacity: oldOpacity}); 
  }}, arguments[1] || {});
  return new Effect.Opacity(element,options);
}

Effect.Appear = function(element) {
  element = $(element);
  var options = Object.extend({
  from: (element.getStyle('display') == 'none' ? 0.0 : element.getOpacity() || 0.0),
  to:   1.0,
  // force Safari to render floated elements properly
  afterFinishInternal: function(effect) {
    effect.element.forceRerendering();
  },
  beforeSetup: function(effect) {
    effect.element.setOpacity(effect.options.from).show(); 
  }}, arguments[1] || {});
  return new Effect.Opacity(element,options);
}

Effect.Puff = function(element) {
  element = $(element);
  var oldStyle = { 
    opacity: element.getInlineOpacity(), 
    position: element.getStyle('position'),
    top:  element.style.top,
    left: element.style.left,
    width: element.style.width,
    height: element.style.height
  };
  return new Effect.Parallel(
   [ new Effect.Scale(element, 200, 
      { sync: true, scaleFromCenter: true, scaleContent: true, restoreAfterFinish: true }), 
     new Effect.Opacity(element, { sync: true, to: 0.0 } ) ], 
     Object.extend({ duration: 1.0, 
      beforeSetupInternal: function(effect) {
        Position.absolutize(effect.effects[0].element)
      },
      afterFinishInternal: function(effect) {
         effect.effects[0].element.hide().setStyle(oldStyle); }
     }, arguments[1] || {})
   );
}

Effect.BlindUp = function(element) {
  element = $(element);
  element.makeClipping();
  return new Effect.Scale(element, 0,
    Object.extend({ scaleContent: false, 
      scaleX: false, 
      restoreAfterFinish: true,
      afterFinishInternal: function(effect) {
        effect.element.hide().undoClipping();
      } 
    }, arguments[1] || {})
  );
}

Effect.BlindDown = function(element) {
  element = $(element);
  var elementDimensions = element.getDimensions();
  return new Effect.Scale(element, 100, Object.extend({ 
    scaleContent: false, 
    scaleX: false,
    scaleFrom: 0,
    scaleMode: {originalHeight: elementDimensions.height, originalWidth: elementDimensions.width},
    restoreAfterFinish: true,
    afterSetup: function(effect) {
      effect.element.makeClipping().setStyle({height: '0px'}).show(); 
    },  
    afterFinishInternal: function(effect) {
      effect.element.undoClipping();
    }
  }, arguments[1] || {}));
}

Effect.SwitchOff = function(element) {
  element = $(element);
  var oldOpacity = element.getInlineOpacity();
  return new Effect.Appear(element, Object.extend({
    duration: 0.4,
    from: 0,
    transition: Effect.Transitions.flicker,
    afterFinishInternal: function(effect) {
      new Effect.Scale(effect.element, 1, { 
        duration: 0.3, scaleFromCenter: true,
        scaleX: false, scaleContent: false, restoreAfterFinish: true,
        beforeSetup: function(effect) { 
          effect.element.makePositioned().makeClipping();
        },
        afterFinishInternal: function(effect) {
          effect.element.hide().undoClipping().undoPositioned().setStyle({opacity: oldOpacity});
        }
      })
    }
  }, arguments[1] || {}));
}

Effect.DropOut = function(element) {
  element = $(element);
  var oldStyle = {
    top: element.getStyle('top'),
    left: element.getStyle('left'),
    opacity: element.getInlineOpacity() };
  return new Effect.Parallel(
    [ new Effect.Move(element, {x: 0, y: 100, sync: true }), 
      new Effect.Opacity(element, { sync: true, to: 0.0 }) ],
    Object.extend(
      { duration: 0.5,
        beforeSetup: function(effect) {
          effect.effects[0].element.makePositioned(); 
        },
        afterFinishInternal: function(effect) {
          effect.effects[0].element.hide().undoPositioned().setStyle(oldStyle);
        } 
      }, arguments[1] || {}));
}

Effect.Shake = function(element) {
  element = $(element);
  var oldStyle = {
    top: element.getStyle('top'),
    left: element.getStyle('left') };
    return new Effect.Move(element, 
      { x:  20, y: 0, duration: 0.05, afterFinishInternal: function(effect) {
    new Effect.Move(effect.element,
      { x: -40, y: 0, duration: 0.1,  afterFinishInternal: function(effect) {
    new Effect.Move(effect.element,
      { x:  40, y: 0, duration: 0.1,  afterFinishInternal: function(effect) {
    new Effect.Move(effect.element,
      { x: -40, y: 0, duration: 0.1,  afterFinishInternal: function(effect) {
    new Effect.Move(effect.element,
      { x:  40, y: 0, duration: 0.1,  afterFinishInternal: function(effect) {
    new Effect.Move(effect.element,
      { x: -20, y: 0, duration: 0.05, afterFinishInternal: function(effect) {
        effect.element.undoPositioned().setStyle(oldStyle);
  }}) }}) }}) }}) }}) }});
}

Effect.SlideDown = function(element) {
  element = $(element).cleanWhitespace();
  // SlideDown need to have the content of the element wrapped in a container element with fixed height!
  var oldInnerBottom = element.down().getStyle('bottom');
  var elementDimensions = element.getDimensions();
  return new Effect.Scale(element, 100, Object.extend({ 
    scaleContent: false, 
    scaleX: false, 
    scaleFrom: window.opera ? 0 : 1,
    scaleMode: {originalHeight: elementDimensions.height, originalWidth: elementDimensions.width},
    restoreAfterFinish: true,
    afterSetup: function(effect) {
      effect.element.makePositioned();
      effect.element.down().makePositioned();
      if(window.opera) effect.element.setStyle({top: ''});
      effect.element.makeClipping().setStyle({height: '0px'}).show(); 
    },
    afterUpdateInternal: function(effect) {
      effect.element.down().setStyle({bottom:
        (effect.dims[0] - effect.element.clientHeight) + 'px' }); 
    },
    afterFinishInternal: function(effect) {
      effect.element.undoClipping().undoPositioned();
      effect.element.down().undoPositioned().setStyle({bottom: oldInnerBottom}); }
    }, arguments[1] || {})
  );
}

Effect.SlideUp = function(element) {
  element = $(element).cleanWhitespace();
  var oldInnerBottom = element.down().getStyle('bottom');
  return new Effect.Scale(element, window.opera ? 0 : 1,
   Object.extend({ scaleContent: false, 
    scaleX: false, 
    scaleMode: 'box',
    scaleFrom: 100,
    restoreAfterFinish: true,
    beforeStartInternal: function(effect) {
      effect.element.makePositioned();
      effect.element.down().makePositioned();
      if(window.opera) effect.element.setStyle({top: ''});
      effect.element.makeClipping().show();
    },  
    afterUpdateInternal: function(effect) {
      effect.element.down().setStyle({bottom:
        (effect.dims[0] - effect.element.clientHeight) + 'px' });
    },
    afterFinishInternal: function(effect) {
      effect.element.hide().undoClipping().undoPositioned().setStyle({bottom: oldInnerBottom});
      effect.element.down().undoPositioned();
    }
   }, arguments[1] || {})
  );
}

// Bug in opera makes the TD containing this element expand for a instance after finish 
Effect.Squish = function(element) {
  return new Effect.Scale(element, window.opera ? 1 : 0, { 
    restoreAfterFinish: true,
    beforeSetup: function(effect) {
      effect.element.makeClipping(); 
    },  
    afterFinishInternal: function(effect) {
      effect.element.hide().undoClipping(); 
    }
  });
}

Effect.Grow = function(element) {
  element = $(element);
  var options = Object.extend({
    direction: 'center',
    moveTransition: Effect.Transitions.sinoidal,
    scaleTransition: Effect.Transitions.sinoidal,
    opacityTransition: Effect.Transitions.full
  }, arguments[1] || {});
  var oldStyle = {
    top: element.style.top,
    left: element.style.left,
    height: element.style.height,
    width: element.style.width,
    opacity: element.getInlineOpacity() };

  var dims = element.getDimensions();    
  var initialMoveX, initialMoveY;
  var moveX, moveY;
  
  switch (options.direction) {
    case 'top-left':
      initialMoveX = initialMoveY = moveX = moveY = 0; 
      break;
    case 'top-right':
      initialMoveX = dims.width;
      initialMoveY = moveY = 0;
      moveX = -dims.width;
      break;
    case 'bottom-left':
      initialMoveX = moveX = 0;
      initialMoveY = dims.height;
      moveY = -dims.height;
      break;
    case 'bottom-right':
      initialMoveX = dims.width;
      initialMoveY = dims.height;
      moveX = -dims.width;
      moveY = -dims.height;
      break;
    case 'center':
      initialMoveX = dims.width / 2;
      initialMoveY = dims.height / 2;
      moveX = -dims.width / 2;
      moveY = -dims.height / 2;
      break;
  }
  
  return new Effect.Move(element, {
    x: initialMoveX,
    y: initialMoveY,
    duration: 0.01, 
    beforeSetup: function(effect) {
      effect.element.hide().makeClipping().makePositioned();
    },
    afterFinishInternal: function(effect) {
      new Effect.Parallel(
        [ new Effect.Opacity(effect.element, { sync: true, to: 1.0, from: 0.0, transition: options.opacityTransition }),
          new Effect.Move(effect.element, { x: moveX, y: moveY, sync: true, transition: options.moveTransition }),
          new Effect.Scale(effect.element, 100, {
            scaleMode: { originalHeight: dims.height, originalWidth: dims.width }, 
            sync: true, scaleFrom: window.opera ? 1 : 0, transition: options.scaleTransition, restoreAfterFinish: true})
        ], Object.extend({
             beforeSetup: function(effect) {
               effect.effects[0].element.setStyle({height: '0px'}).show(); 
             },
             afterFinishInternal: function(effect) {
               effect.effects[0].element.undoClipping().undoPositioned().setStyle(oldStyle); 
             }
           }, options)
      )
    }
  });
}

Effect.Shrink = function(element) {
  element = $(element);
  var options = Object.extend({
    direction: 'center',
    moveTransition: Effect.Transitions.sinoidal,
    scaleTransition: Effect.Transitions.sinoidal,
    opacityTransition: Effect.Transitions.none
  }, arguments[1] || {});
  var oldStyle = {
    top: element.style.top,
    left: element.style.left,
    height: element.style.height,
    width: element.style.width,
    opacity: element.getInlineOpacity() };

  var dims = element.getDimensions();
  var moveX, moveY;
  
  switch (options.direction) {
    case 'top-left':
      moveX = moveY = 0;
      break;
    case 'top-right':
      moveX = dims.width;
      moveY = 0;
      break;
    case 'bottom-left':
      moveX = 0;
      moveY = dims.height;
      break;
    case 'bottom-right':
      moveX = dims.width;
      moveY = dims.height;
      break;
    case 'center':  
      moveX = dims.width / 2;
      moveY = dims.height / 2;
      break;
  }
  
  return new Effect.Parallel(
    [ new Effect.Opacity(element, { sync: true, to: 0.0, from: 1.0, transition: options.opacityTransition }),
      new Effect.Scale(element, window.opera ? 1 : 0, { sync: true, transition: options.scaleTransition, restoreAfterFinish: true}),
      new Effect.Move(element, { x: moveX, y: moveY, sync: true, transition: options.moveTransition })
    ], Object.extend({            
         beforeStartInternal: function(effect) {
           effect.effects[0].element.makePositioned().makeClipping(); 
         },
         afterFinishInternal: function(effect) {
           effect.effects[0].element.hide().undoClipping().undoPositioned().setStyle(oldStyle); }
       }, options)
  );
}

Effect.Pulsate = function(element) {
  element = $(element);
  var options    = arguments[1] || {};
  var oldOpacity = element.getInlineOpacity();
  var transition = options.transition || Effect.Transitions.sinoidal;
  var reverser   = function(pos){ return transition(1-Effect.Transitions.pulse(pos, options.pulses)) };
  reverser.bind(transition);
  return new Effect.Opacity(element, 
    Object.extend(Object.extend({  duration: 2.0, from: 0,
      afterFinishInternal: function(effect) { effect.element.setStyle({opacity: oldOpacity}); }
    }, options), {transition: reverser}));
}

Effect.Fold = function(element) {
  element = $(element);
  var oldStyle = {
    top: element.style.top,
    left: element.style.left,
    width: element.style.width,
    height: element.style.height };
  element.makeClipping();
  return new Effect.Scale(element, 5, Object.extend({   
    scaleContent: false,
    scaleX: false,
    afterFinishInternal: function(effect) {
    new Effect.Scale(element, 1, { 
      scaleContent: false, 
      scaleY: false,
      afterFinishInternal: function(effect) {
        effect.element.hide().undoClipping().setStyle(oldStyle);
      } });
  }}, arguments[1] || {}));
};

Effect.Morph = Class.create();
Object.extend(Object.extend(Effect.Morph.prototype, Effect.Base.prototype), {
  initialize: function(element) {
    this.element = $(element);
    if(!this.element) throw(Effect._elementDoesNotExistError);
    var options = Object.extend({
      style: {}
    }, arguments[1] || {});
    if (typeof options.style == 'string') {
      if(options.style.indexOf(':') == -1) {
        var cssText = '', selector = '.' + options.style;
        $A(document.styleSheets).reverse().each(function(styleSheet) {
          if (styleSheet.cssRules) cssRules = styleSheet.cssRules;
          else if (styleSheet.rules) cssRules = styleSheet.rules;
          $A(cssRules).reverse().each(function(rule) {
            if (selector == rule.selectorText) {
              cssText = rule.style.cssText;
              throw $break;
            }
          });
          if (cssText) throw $break;
        });
        this.style = cssText.parseStyle();
        options.afterFinishInternal = function(effect){
          effect.element.addClassName(effect.options.style);
          effect.transforms.each(function(transform) {
            if(transform.style != 'opacity')
              effect.element.style[transform.style.camelize()] = '';
          });
        }
      } else this.style = options.style.parseStyle();
    } else this.style = $H(options.style)
    this.start(options);
  },
  setup: function(){
    function parseColor(color){
      if(!color || ['rgba(0, 0, 0, 0)','transparent'].include(color)) color = '#ffffff';
      color = color.parseColor();
      return $R(0,2).map(function(i){
        return parseInt( color.slice(i*2+1,i*2+3), 16 ) 
      });
    }
    this.transforms = this.style.map(function(pair){
      var property = pair[0].underscore().dasherize(), value = pair[1], unit = null;

      if(value.parseColor('#zzzzzz') != '#zzzzzz') {
        value = value.parseColor();
        unit  = 'color';
      } else if(property == 'opacity') {
        value = parseFloat(value);
        if(/MSIE/.test(navigator.userAgent) && !window.opera && (!this.element.currentStyle.hasLayout))
          this.element.setStyle({zoom: 1});
      } else if(Element.CSS_LENGTH.test(value)) 
        var components = value.match(/^([\+\-]?[0-9\.]+)(.*)$/),
          value = parseFloat(components[1]), unit = (components.length == 3) ? components[2] : null;

      var originalValue = this.element.getStyle(property);
      return $H({ 
        style: property, 
        originalValue: unit=='color' ? parseColor(originalValue) : parseFloat(originalValue || 0), 
        targetValue: unit=='color' ? parseColor(value) : value,
        unit: unit
      });
    }.bind(this)).reject(function(transform){
      return (
        (transform.originalValue == transform.targetValue) ||
        (
          transform.unit != 'color' &&
          (isNaN(transform.originalValue) || isNaN(transform.targetValue))
        )
      )
    });
  },
  update: function(position) {
    var style = $H(), value = null;
    this.transforms.each(function(transform){
      value = transform.unit=='color' ?
        $R(0,2).inject('#',function(m,v,i){
          return m+(Math.round(transform.originalValue[i]+
            (transform.targetValue[i] - transform.originalValue[i])*position)).toColorPart() }) : 
        transform.originalValue + Math.round(
          ((transform.targetValue - transform.originalValue) * position) * 1000)/1000 + transform.unit;
      style[transform.style] = value;
    });
    this.element.setStyle(style);
  }
});

Effect.Transform = Class.create();
Object.extend(Effect.Transform.prototype, {
  initialize: function(tracks){
    this.tracks  = [];
    this.options = arguments[1] || {};
    this.addTracks(tracks);
  },
  addTracks: function(tracks){
    tracks.each(function(track){
      var data = $H(track).values().first();
      this.tracks.push($H({
        ids:     $H(track).keys().first(),
        effect:  Effect.Morph,
        options: { style: data }
      }));
    }.bind(this));
    return this;
  },
  play: function(){
    return new Effect.Parallel(
      this.tracks.map(function(track){
        var elements = [$(track.ids) || $$(track.ids)].flatten();
        return elements.map(function(e){ return new track.effect(e, Object.extend({ sync:true }, track.options)) });
      }).flatten(),
      this.options
    );
  }
});

Element.CSS_PROPERTIES = $w(
  'backgroundColor backgroundPosition borderBottomColor borderBottomStyle ' + 
  'borderBottomWidth borderLeftColor borderLeftStyle borderLeftWidth ' +
  'borderRightColor borderRightStyle borderRightWidth borderSpacing ' +
  'borderTopColor borderTopStyle borderTopWidth bottom clip color ' +
  'fontSize fontWeight height left letterSpacing lineHeight ' +
  'marginBottom marginLeft marginRight marginTop markerOffset maxHeight '+
  'maxWidth minHeight minWidth opacity outlineColor outlineOffset ' +
  'outlineWidth paddingBottom paddingLeft paddingRight paddingTop ' +
  'right textIndent top width wordSpacing zIndex');
  
Element.CSS_LENGTH = /^(([\+\-]?[0-9\.]+)(em|ex|px|in|cm|mm|pt|pc|\%))|0$/;

String.prototype.parseStyle = function(){
  var element = Element.extend(document.createElement('div'));
  element.innerHTML = '<div style="' + this + '"></div>';
  var style = element.down().style, styleRules = $H();
  
  Element.CSS_PROPERTIES.each(function(property){
    if(style[property]) styleRules[property] = style[property]; 
  });
  if(/MSIE/.test(navigator.userAgent) && !window.opera && this.indexOf('opacity') > -1) {
    styleRules.opacity = this.match(/opacity:\s*((?:0|1)?(?:\.\d*)?)/)[1];
  }
  return styleRules;
};

Element.morph = function(element, style) {
  new Effect.Morph(element, Object.extend({ style: style }, arguments[2] || {}));
  return element;
};

['setOpacity','getOpacity','getInlineOpacity','forceRerendering','setContentZoom',
 'collectTextNodes','collectTextNodesIgnoreClass','morph'].each( 
  function(f) { Element.Methods[f] = Element[f]; }
);

Element.Methods.visualEffect = function(element, effect, options) {
  s = effect.gsub(/_/, '-').camelize();
  effect_class = s.charAt(0).toUpperCase() + s.substring(1);
  new Effect[effect_class](element, options);
  return $(element);
};

Element.addMethods();// script.aculo.us dragdrop.js v1.7.0, Fri Jan 19 19:16:36 CET 2007

// Copyright (c) 2005, 2006 Thomas Fuchs (http://script.aculo.us, http://mir.aculo.us)
//           (c) 2005, 2006 Sammi Williams (http://www.oriontransfer.co.nz, sammi@oriontransfer.co.nz)
// 
// script.aculo.us is freely distributable under the terms of an MIT-style license.
// For details, see the script.aculo.us web site: http://script.aculo.us/

if(typeof Effect == 'undefined')
  throw("dragdrop.js requires including script.aculo.us' effects.js library");

var Droppables = {
  drops: [],

  remove: function(element) {
    this.drops = this.drops.reject(function(d) { return d.element==$(element) });
  },

  add: function(element) {
    element = $(element);
    var options = Object.extend({
      greedy:     true,
      hoverclass: null,
      tree:       false
    }, arguments[1] || {});

    // cache containers
    if(options.containment) {
      options._containers = [];
      var containment = options.containment;
      if((typeof containment == 'object') && 
        (containment.constructor == Array)) {
        containment.each( function(c) { options._containers.push($(c)) });
      } else {
        options._containers.push($(containment));
      }
    }
    
    if(options.accept) options.accept = [options.accept].flatten();

    Element.makePositioned(element); // fix IE
    options.element = element;

    this.drops.push(options);
  },
  
  findDeepestChild: function(drops) {
    deepest = drops[0];
      
    for (i = 1; i < drops.length; ++i)
      if (Element.isParent(drops[i].element, deepest.element))
        deepest = drops[i];
    
    return deepest;
  },

  isContained: function(element, drop) {
    var containmentNode;
    if(drop.tree) {
      containmentNode = element.treeNode; 
    } else {
      containmentNode = element.parentNode;
    }
    return drop._containers.detect(function(c) { return containmentNode == c });
  },
  
  isAffected: function(point, element, drop) {
    return (
      (drop.element!=element) &&
      ((!drop._containers) ||
        this.isContained(element, drop)) &&
      ((!drop.accept) ||
        (Element.classNames(element).detect( 
          function(v) { return drop.accept.include(v) } ) )) &&
      Position.within(drop.element, point[0], point[1]) );
  },

  deactivate: function(drop) {
    if(drop.hoverclass)
      Element.removeClassName(drop.element, drop.hoverclass);
    this.last_active = null;
  },

  activate: function(drop) {
    if(drop.hoverclass)
      Element.addClassName(drop.element, drop.hoverclass);
    this.last_active = drop;
  },

  show: function(point, element) {
    if(!this.drops.length) return;
    var affected = [];
    
    if(this.last_active) this.deactivate(this.last_active);
    this.drops.each( function(drop) {
      if(Droppables.isAffected(point, element, drop))
        affected.push(drop);
    });
        
    if(affected.length>0) {
      drop = Droppables.findDeepestChild(affected);
      Position.within(drop.element, point[0], point[1]);
      if(drop.onHover)
        drop.onHover(element, drop.element, Position.overlap(drop.overlap, drop.element));
      
      Droppables.activate(drop);
    }
  },

  fire: function(event, element) {
    if(!this.last_active) return;
    Position.prepare();

    if (this.isAffected([Event.pointerX(event), Event.pointerY(event)], element, this.last_active))
      if (this.last_active.onDrop) 
        this.last_active.onDrop(element, this.last_active.element, event);
  },

  reset: function() {
    if(this.last_active)
      this.deactivate(this.last_active);
  }
}

var Draggables = {
  drags: [],
  observers: [],
  
  register: function(draggable) {
    if(this.drags.length == 0) {
      this.eventMouseUp   = this.endDrag.bindAsEventListener(this);
      this.eventMouseMove = this.updateDrag.bindAsEventListener(this);
      this.eventKeypress  = this.keyPress.bindAsEventListener(this);
      
      Event.observe(document, "mouseup", this.eventMouseUp);
      Event.observe(document, "mousemove", this.eventMouseMove);
      Event.observe(document, "keypress", this.eventKeypress);
    }
    this.drags.push(draggable);
  },
  
  unregister: function(draggable) {
    this.drags = this.drags.reject(function(d) { return d==draggable });
    if(this.drags.length == 0) {
      Event.stopObserving(document, "mouseup", this.eventMouseUp);
      Event.stopObserving(document, "mousemove", this.eventMouseMove);
      Event.stopObserving(document, "keypress", this.eventKeypress);
    }
  },
  
  activate: function(draggable) {
    if(draggable.options.delay) { 
      this._timeout = setTimeout(function() { 
        Draggables._timeout = null; 
        window.focus(); 
        Draggables.activeDraggable = draggable; 
      }.bind(this), draggable.options.delay); 
    } else {
      window.focus(); // allows keypress events if window isn't currently focused, fails for Safari
      this.activeDraggable = draggable;
    }
  },
  
  deactivate: function() {
    this.activeDraggable = null;
  },
  
  updateDrag: function(event) {
    if(!this.activeDraggable) return;
    var pointer = [Event.pointerX(event), Event.pointerY(event)];
    // Mozilla-based browsers fire successive mousemove events with
    // the same coordinates, prevent needless redrawing (moz bug?)
    if(this._lastPointer && (this._lastPointer.inspect() == pointer.inspect())) return;
    this._lastPointer = pointer;
    
    this.activeDraggable.updateDrag(event, pointer);
  },
  
  endDrag: function(event) {
    if(this._timeout) { 
      clearTimeout(this._timeout); 
      this._timeout = null; 
    }
    if(!this.activeDraggable) return;
    this._lastPointer = null;
    this.activeDraggable.endDrag(event);
    this.activeDraggable = null;
  },
  
  keyPress: function(event) {
    if(this.activeDraggable)
      this.activeDraggable.keyPress(event);
  },
  
  addObserver: function(observer) {
    this.observers.push(observer);
    this._cacheObserverCallbacks();
  },
  
  removeObserver: function(element) {  // element instead of observer fixes mem leaks
    this.observers = this.observers.reject( function(o) { return o.element==element });
    this._cacheObserverCallbacks();
  },
  
  notify: function(eventName, draggable, event) {  // 'onStart', 'onEnd', 'onDrag'
    if(this[eventName+'Count'] > 0)
      this.observers.each( function(o) {
        if(o[eventName]) o[eventName](eventName, draggable, event);
      });
    if(draggable.options[eventName]) draggable.options[eventName](draggable, event);
  },
  
  _cacheObserverCallbacks: function() {
    ['onStart','onEnd','onDrag'].each( function(eventName) {
      Draggables[eventName+'Count'] = Draggables.observers.select(
        function(o) { return o[eventName]; }
      ).length;
    });
  }
}

/*--------------------------------------------------------------------------*/

var Draggable = Class.create();
Draggable._dragging    = {};

Draggable.prototype = {
  initialize: function(element) {
    var defaults = {
      handle: false,
      reverteffect: function(element, top_offset, left_offset) {
        var dur = Math.sqrt(Math.abs(top_offset^2)+Math.abs(left_offset^2))*0.02;
        new Effect.Move(element, { x: -left_offset, y: -top_offset, duration: dur,
          queue: {scope:'_draggable', position:'end'}
        });
      },
      endeffect: function(element) {
        var toOpacity = typeof element._opacity == 'number' ? element._opacity : 1.0;
        new Effect.Opacity(element, {duration:0.2, from:0.7, to:toOpacity, 
          queue: {scope:'_draggable', position:'end'},
          afterFinish: function(){ 
            Draggable._dragging[element] = false 
          }
        }); 
      },
      zindex: 1000,
      revert: false,
      scroll: false,
      scrollSensitivity: 20,
      scrollSpeed: 15,
      snap: false,  // false, or xy or [x,y] or function(x,y){ return [x,y] }
      delay: 0
    };
    
    if(!arguments[1] || typeof arguments[1].endeffect == 'undefined')
      Object.extend(defaults, {
        starteffect: function(element) {
          element._opacity = Element.getOpacity(element);
          Draggable._dragging[element] = true;
          new Effect.Opacity(element, {duration:0.2, from:element._opacity, to:0.7}); 
        }
      });
    
    var options = Object.extend(defaults, arguments[1] || {});

    this.element = $(element);
    
    if(options.handle && (typeof options.handle == 'string'))
      this.handle = this.element.down('.'+options.handle, 0);
    
    if(!this.handle) this.handle = $(options.handle);
    if(!this.handle) this.handle = this.element;
    
    if(options.scroll && !options.scroll.scrollTo && !options.scroll.outerHTML) {
      options.scroll = $(options.scroll);
      this._isScrollChild = Element.childOf(this.element, options.scroll);
    }

    Element.makePositioned(this.element); // fix IE    

    this.delta    = this.currentDelta();
    this.options  = options;
    this.dragging = false;   

    this.eventMouseDown = this.initDrag.bindAsEventListener(this);
    Event.observe(this.handle, "mousedown", this.eventMouseDown);
    
    Draggables.register(this);
  },
  
  destroy: function() {
    Event.stopObserving(this.handle, "mousedown", this.eventMouseDown);
    Draggables.unregister(this);
  },
  
  currentDelta: function() {
    return([
      parseInt(Element.getStyle(this.element,'left') || '0'),
      parseInt(Element.getStyle(this.element,'top') || '0')]);
  },
  
  initDrag: function(event) {
    if(typeof Draggable._dragging[this.element] != 'undefined' &&
      Draggable._dragging[this.element]) return;
    if(Event.isLeftClick(event)) {    
      // abort on form elements, fixes a Firefox issue
      var src = Event.element(event);
      if((tag_name = src.tagName.toUpperCase()) && (
        tag_name=='INPUT' ||
        tag_name=='SELECT' ||
        tag_name=='OPTION' ||
        tag_name=='BUTTON' ||
        tag_name=='TEXTAREA')) return;
        
      var pointer = [Event.pointerX(event), Event.pointerY(event)];
      var pos     = Position.cumulativeOffset(this.element);
      this.offset = [0,1].map( function(i) { return (pointer[i] - pos[i]) });
      
      Draggables.activate(this);
      Event.stop(event);
    }
  },
  
  startDrag: function(event) {
    this.dragging = true;
    
    if(this.options.zindex) {
      this.originalZ = parseInt(Element.getStyle(this.element,'z-index') || 0);
      this.element.style.zIndex = this.options.zindex;
    }
    
    if(this.options.ghosting) {
      this._clone = this.element.cloneNode(true);
      Position.absolutize(this.element);
      this.element.parentNode.insertBefore(this._clone, this.element);
    }
    
    if(this.options.scroll) {
      if (this.options.scroll == window) {
        var where = this._getWindowScroll(this.options.scroll);
        this.originalScrollLeft = where.left;
        this.originalScrollTop = where.top;
      } else {
        this.originalScrollLeft = this.options.scroll.scrollLeft;
        this.originalScrollTop = this.options.scroll.scrollTop;
      }
    }
    
    Draggables.notify('onStart', this, event);
        
    if(this.options.starteffect) this.options.starteffect(this.element);
  },
  
  updateDrag: function(event, pointer) {
    if(!this.dragging) this.startDrag(event);
    Position.prepare();
    Droppables.show(pointer, this.element);
    Draggables.notify('onDrag', this, event);
    
    this.draw(pointer);
    if(this.options.change) this.options.change(this);
    
    if(this.options.scroll) {
      this.stopScrolling();
      
      var p;
      if (this.options.scroll == window) {
        with(this._getWindowScroll(this.options.scroll)) { p = [ left, top, left+width, top+height ]; }
      } else {
        p = Position.page(this.options.scroll);
        p[0] += this.options.scroll.scrollLeft + Position.deltaX;
        p[1] += this.options.scroll.scrollTop + Position.deltaY;
        p.push(p[0]+this.options.scroll.offsetWidth);
        p.push(p[1]+this.options.scroll.offsetHeight);
      }
      var speed = [0,0];
      if(pointer[0] < (p[0]+this.options.scrollSensitivity)) speed[0] = pointer[0]-(p[0]+this.options.scrollSensitivity);
      if(pointer[1] < (p[1]+this.options.scrollSensitivity)) speed[1] = pointer[1]-(p[1]+this.options.scrollSensitivity);
      if(pointer[0] > (p[2]-this.options.scrollSensitivity)) speed[0] = pointer[0]-(p[2]-this.options.scrollSensitivity);
      if(pointer[1] > (p[3]-this.options.scrollSensitivity)) speed[1] = pointer[1]-(p[3]-this.options.scrollSensitivity);
      this.startScrolling(speed);
    }
    
    // fix AppleWebKit rendering
    if(navigator.appVersion.indexOf('AppleWebKit')>0) window.scrollBy(0,0);
    
    Event.stop(event);
  },
  
  finishDrag: function(event, success) {
    this.dragging = false;

    if(this.options.ghosting) {
      Position.relativize(this.element);
      Element.remove(this._clone);
      this._clone = null;
    }

    if(success) Droppables.fire(event, this.element);
    Draggables.notify('onEnd', this, event);

    var revert = this.options.revert;
    if(revert && typeof revert == 'function') revert = revert(this.element);
    
    var d = this.currentDelta();
    if(revert && this.options.reverteffect) {
      this.options.reverteffect(this.element, 
        d[1]-this.delta[1], d[0]-this.delta[0]);
    } else {
      this.delta = d;
    }

    if(this.options.zindex)
      this.element.style.zIndex = this.originalZ;

    if(this.options.endeffect) 
      this.options.endeffect(this.element);
      
    Draggables.deactivate(this);
    Droppables.reset();
  },
  
  keyPress: function(event) {
    if(event.keyCode!=Event.KEY_ESC) return;
    this.finishDrag(event, false);
    Event.stop(event);
  },
  
  endDrag: function(event) {
    if(!this.dragging) return;
    this.stopScrolling();
    this.finishDrag(event, true);
    Event.stop(event);
  },
  
  draw: function(point) {
    var pos = Position.cumulativeOffset(this.element);
    if(this.options.ghosting) {
      var r   = Position.realOffset(this.element);
      pos[0] += r[0] - Position.deltaX; pos[1] += r[1] - Position.deltaY;
    }
    
    var d = this.currentDelta();
    pos[0] -= d[0]; pos[1] -= d[1];
    
    if(this.options.scroll && (this.options.scroll != window && this._isScrollChild)) {
      pos[0] -= this.options.scroll.scrollLeft-this.originalScrollLeft;
      pos[1] -= this.options.scroll.scrollTop-this.originalScrollTop;
    }
    
    var p = [0,1].map(function(i){ 
      return (point[i]-pos[i]-this.offset[i]) 
    }.bind(this));
    
    if(this.options.snap) {
      if(typeof this.options.snap == 'function') {
        p = this.options.snap(p[0],p[1],this);
      } else {
      if(this.options.snap instanceof Array) {
        p = p.map( function(v, i) {
          return Math.round(v/this.options.snap[i])*this.options.snap[i] }.bind(this))
      } else {
        p = p.map( function(v) {
          return Math.round(v/this.options.snap)*this.options.snap }.bind(this))
      }
    }}
    
    var style = this.element.style;
    if((!this.options.constraint) || (this.options.constraint=='horizontal'))
      style.left = p[0] + "px";
    if((!this.options.constraint) || (this.options.constraint=='vertical'))
      style.top  = p[1] + "px";
    
    if(style.visibility=="hidden") style.visibility = ""; // fix gecko rendering
  },
  
  stopScrolling: function() {
    if(this.scrollInterval) {
      clearInterval(this.scrollInterval);
      this.scrollInterval = null;
      Draggables._lastScrollPointer = null;
    }
  },
  
  startScrolling: function(speed) {
    if(!(speed[0] || speed[1])) return;
    this.scrollSpeed = [speed[0]*this.options.scrollSpeed,speed[1]*this.options.scrollSpeed];
    this.lastScrolled = new Date();
    this.scrollInterval = setInterval(this.scroll.bind(this), 10);
  },
  
  scroll: function() {
    var current = new Date();
    var delta = current - this.lastScrolled;
    this.lastScrolled = current;
    if(this.options.scroll == window) {
      with (this._getWindowScroll(this.options.scroll)) {
        if (this.scrollSpeed[0] || this.scrollSpeed[1]) {
          var d = delta / 1000;
          this.options.scroll.scrollTo( left + d*this.scrollSpeed[0], top + d*this.scrollSpeed[1] );
        }
      }
    } else {
      this.options.scroll.scrollLeft += this.scrollSpeed[0] * delta / 1000;
      this.options.scroll.scrollTop  += this.scrollSpeed[1] * delta / 1000;
    }
    
    Position.prepare();
    Droppables.show(Draggables._lastPointer, this.element);
    Draggables.notify('onDrag', this);
    if (this._isScrollChild) {
      Draggables._lastScrollPointer = Draggables._lastScrollPointer || $A(Draggables._lastPointer);
      Draggables._lastScrollPointer[0] += this.scrollSpeed[0] * delta / 1000;
      Draggables._lastScrollPointer[1] += this.scrollSpeed[1] * delta / 1000;
      if (Draggables._lastScrollPointer[0] < 0)
        Draggables._lastScrollPointer[0] = 0;
      if (Draggables._lastScrollPointer[1] < 0)
        Draggables._lastScrollPointer[1] = 0;
      this.draw(Draggables._lastScrollPointer);
    }
    
    if(this.options.change) this.options.change(this);
  },
  
  _getWindowScroll: function(w) {
    var T, L, W, H;
    with (w.document) {
      if (w.document.documentElement && documentElement.scrollTop) {
        T = documentElement.scrollTop;
        L = documentElement.scrollLeft;
      } else if (w.document.body) {
        T = body.scrollTop;
        L = body.scrollLeft;
      }
      if (w.innerWidth) {
        W = w.innerWidth;
        H = w.innerHeight;
      } else if (w.document.documentElement && documentElement.clientWidth) {
        W = documentElement.clientWidth;
        H = documentElement.clientHeight;
      } else {
        W = body.offsetWidth;
        H = body.offsetHeight
      }
    }
    return { top: T, left: L, width: W, height: H };
  }
}

/*--------------------------------------------------------------------------*/

var SortableObserver = Class.create();
SortableObserver.prototype = {
  initialize: function(element, observer) {
    this.element   = $(element);
    this.observer  = observer;
    this.lastValue = Sortable.serialize(this.element);
  },
  
  onStart: function() {
    this.lastValue = Sortable.serialize(this.element);
  },
  
  onEnd: function() {
    Sortable.unmark();
    if(this.lastValue != Sortable.serialize(this.element))
      this.observer(this.element)
  }
}

var Sortable = {
  SERIALIZE_RULE: /^[^_\-](?:[A-Za-z0-9\-\_]*)[_](.*)$/,
  
  sortables: {},
  
  _findRootElement: function(element) {
    while (element.tagName.toUpperCase() != "BODY") {  
      if(element.id && Sortable.sortables[element.id]) return element;
      element = element.parentNode;
    }
  },

  options: function(element) {
    element = Sortable._findRootElement($(element));
    if(!element) return;
    return Sortable.sortables[element.id];
  },
  
  destroy: function(element){
    var s = Sortable.options(element);
    
    if(s) {
      Draggables.removeObserver(s.element);
      s.droppables.each(function(d){ Droppables.remove(d) });
      s.draggables.invoke('destroy');
      
      delete Sortable.sortables[s.element.id];
    }
  },

  create: function(element) {
    element = $(element);
    var options = Object.extend({ 
      element:     element,
      tag:         'li',       // assumes li children, override with tag: 'tagname'
      dropOnEmpty: false,
      tree:        false,
      treeTag:     'ul',
      overlap:     'vertical', // one of 'vertical', 'horizontal'
      constraint:  'vertical', // one of 'vertical', 'horizontal', false
      containment: element,    // also takes array of elements (or id's); or false
      handle:      false,      // or a CSS class
      only:        false,
      delay:       0,
      hoverclass:  null,
      ghosting:    false,
      scroll:      false,
      scrollSensitivity: 20,
      scrollSpeed: 15,
      format:      this.SERIALIZE_RULE,
      onChange:    Prototype.emptyFunction,
      onUpdate:    Prototype.emptyFunction
    }, arguments[1] || {});

    // clear any old sortable with same element
    this.destroy(element);

    // build options for the draggables
    var options_for_draggable = {
      revert:      true,
      scroll:      options.scroll,
      scrollSpeed: options.scrollSpeed,
      scrollSensitivity: options.scrollSensitivity,
      delay:       options.delay,
      ghosting:    options.ghosting,
      constraint:  options.constraint,
      handle:      options.handle };

    if(options.starteffect)
      options_for_draggable.starteffect = options.starteffect;

    if(options.reverteffect)
      options_for_draggable.reverteffect = options.reverteffect;
    else
      if(options.ghosting) options_for_draggable.reverteffect = function(element) {
        element.style.top  = 0;
        element.style.left = 0;
      };

    if(options.endeffect)
      options_for_draggable.endeffect = options.endeffect;

    if(options.zindex)
      options_for_draggable.zindex = options.zindex;

    // build options for the droppables  
    var options_for_droppable = {
      overlap:     options.overlap,
      containment: options.containment,
      tree:        options.tree,
      hoverclass:  options.hoverclass,
      onHover:     Sortable.onHover
    }
    
    var options_for_tree = {
      onHover:      Sortable.onEmptyHover,
      overlap:      options.overlap,
      containment:  options.containment,
      hoverclass:   options.hoverclass
    }

    // fix for gecko engine
    Element.cleanWhitespace(element); 

    options.draggables = [];
    options.droppables = [];

    // drop on empty handling
    if(options.dropOnEmpty || options.tree) {
      Droppables.add(element, options_for_tree);
      options.droppables.push(element);
    }

    (this.findElements(element, options) || []).each( function(e) {
      // handles are per-draggable
      var handle = options.handle ? 
        $(e).down('.'+options.handle,0) : e;    
      options.draggables.push(
        new Draggable(e, Object.extend(options_for_draggable, { handle: handle })));
      Droppables.add(e, options_for_droppable);
      if(options.tree) e.treeNode = element;
      options.droppables.push(e);      
    });
    
    if(options.tree) {
      (Sortable.findTreeElements(element, options) || []).each( function(e) {
        Droppables.add(e, options_for_tree);
        e.treeNode = element;
        options.droppables.push(e);
      });
    }

    // keep reference
    this.sortables[element.id] = options;

    // for onupdate
    Draggables.addObserver(new SortableObserver(element, options.onUpdate));

  },

  // return all suitable-for-sortable elements in a guaranteed order
  findElements: function(element, options) {
    return Element.findChildren(
      element, options.only, options.tree ? true : false, options.tag);
  },
  
  findTreeElements: function(element, options) {
    return Element.findChildren(
      element, options.only, options.tree ? true : false, options.treeTag);
  },

  onHover: function(element, dropon, overlap) {
    if(Element.isParent(dropon, element)) return;

    if(overlap > .33 && overlap < .66 && Sortable.options(dropon).tree) {
      return;
    } else if(overlap>0.5) {
      Sortable.mark(dropon, 'before');
      if(dropon.previousSibling != element) {
        var oldParentNode = element.parentNode;
        element.style.visibility = "hidden"; // fix gecko rendering
        dropon.parentNode.insertBefore(element, dropon);
        if(dropon.parentNode!=oldParentNode) 
          Sortable.options(oldParentNode).onChange(element);
        Sortable.options(dropon.parentNode).onChange(element);
      }
    } else {
      Sortable.mark(dropon, 'after');
      var nextElement = dropon.nextSibling || null;
      if(nextElement != element) {
        var oldParentNode = element.parentNode;
        element.style.visibility = "hidden"; // fix gecko rendering
        dropon.parentNode.insertBefore(element, nextElement);
        if(dropon.parentNode!=oldParentNode) 
          Sortable.options(oldParentNode).onChange(element);
        Sortable.options(dropon.parentNode).onChange(element);
      }
    }
  },
  
  onEmptyHover: function(element, dropon, overlap) {
    var oldParentNode = element.parentNode;
    var droponOptions = Sortable.options(dropon);
        
    if(!Element.isParent(dropon, element)) {
      var index;
      
      var children = Sortable.findElements(dropon, {tag: droponOptions.tag, only: droponOptions.only});
      var child = null;
            
      if(children) {
        var offset = Element.offsetSize(dropon, droponOptions.overlap) * (1.0 - overlap);
        
        for (index = 0; index < children.length; index += 1) {
          if (offset - Element.offsetSize (children[index], droponOptions.overlap) >= 0) {
            offset -= Element.offsetSize (children[index], droponOptions.overlap);
          } else if (offset - (Element.offsetSize (children[index], droponOptions.overlap) / 2) >= 0) {
            child = index + 1 < children.length ? children[index + 1] : null;
            break;
          } else {
            child = children[index];
            break;
          }
        }
      }
      
      dropon.insertBefore(element, child);
      
      Sortable.options(oldParentNode).onChange(element);
      droponOptions.onChange(element);
    }
  },

  unmark: function() {
    if(Sortable._marker) Sortable._marker.hide();
  },

  mark: function(dropon, position) {
    // mark on ghosting only
    var sortable = Sortable.options(dropon.parentNode);
    if(sortable && !sortable.ghosting) return; 

    if(!Sortable._marker) {
      Sortable._marker = 
        ($('dropmarker') || Element.extend(document.createElement('DIV'))).
          hide().addClassName('dropmarker').setStyle({position:'absolute'});
      document.getElementsByTagName("body").item(0).appendChild(Sortable._marker);
    }    
    var offsets = Position.cumulativeOffset(dropon);
    Sortable._marker.setStyle({left: offsets[0]+'px', top: offsets[1] + 'px'});
    
    if(position=='after')
      if(sortable.overlap == 'horizontal') 
        Sortable._marker.setStyle({left: (offsets[0]+dropon.clientWidth) + 'px'});
      else
        Sortable._marker.setStyle({top: (offsets[1]+dropon.clientHeight) + 'px'});
    
    Sortable._marker.show();
  },
  
  _tree: function(element, options, parent) {
    var children = Sortable.findElements(element, options) || [];
  
    for (var i = 0; i < children.length; ++i) {
      var match = children[i].id.match(options.format);

      if (!match) continue;
      
      var child = {
        id: encodeURIComponent(match ? match[1] : null),
        element: element,
        parent: parent,
        children: [],
        position: parent.children.length,
        container: $(children[i]).down(options.treeTag)
      }
      
      /* Get the element containing the children and recurse over it */
      if (child.container)
        this._tree(child.container, options, child)
      
      parent.children.push (child);
    }

    return parent; 
  },

  tree: function(element) {
    element = $(element);
    var sortableOptions = this.options(element);
    var options = Object.extend({
      tag: sortableOptions.tag,
      treeTag: sortableOptions.treeTag,
      only: sortableOptions.only,
      name: element.id,
      format: sortableOptions.format
    }, arguments[1] || {});
    
    var root = {
      id: null,
      parent: null,
      children: [],
      container: element,
      position: 0
    }
    
    return Sortable._tree(element, options, root);
  },

  /* Construct a [i] index for a particular node */
  _constructIndex: function(node) {
    var index = '';
    do {
      if (node.id) index = '[' + node.position + ']' + index;
    } while ((node = node.parent) != null);
    return index;
  },

  sequence: function(element) {
    element = $(element);
    var options = Object.extend(this.options(element), arguments[1] || {});
    
    return $(this.findElements(element, options) || []).map( function(item) {
      return item.id.match(options.format) ? item.id.match(options.format)[1] : '';
    });
  },

  setSequence: function(element, new_sequence) {
    element = $(element);
    var options = Object.extend(this.options(element), arguments[2] || {});
    
    var nodeMap = {};
    this.findElements(element, options).each( function(n) {
        if (n.id.match(options.format))
            nodeMap[n.id.match(options.format)[1]] = [n, n.parentNode];
        n.parentNode.removeChild(n);
    });
   
    new_sequence.each(function(ident) {
      var n = nodeMap[ident];
      if (n) {
        n[1].appendChild(n[0]);
        delete nodeMap[ident];
      }
    });
  },
  
  serialize: function(element) {
    element = $(element);
    var options = Object.extend(Sortable.options(element), arguments[1] || {});
    var name = encodeURIComponent(
      (arguments[1] && arguments[1].name) ? arguments[1].name : element.id);
    
    if (options.tree) {
      return Sortable.tree(element, arguments[1]).children.map( function (item) {
        return [name + Sortable._constructIndex(item) + "[id]=" + 
                encodeURIComponent(item.id)].concat(item.children.map(arguments.callee));
      }).flatten().join('&');
    } else {
      return Sortable.sequence(element, arguments[1]).map( function(item) {
        return name + "[]=" + encodeURIComponent(item);
      }).join('&');
    }
  }
}

// Returns true if child is contained within element
Element.isParent = function(child, element) {
  if (!child.parentNode || child == element) return false;
  if (child.parentNode == element) return true;
  return Element.isParent(child.parentNode, element);
}

Element.findChildren = function(element, only, recursive, tagName) {    
  if(!element.hasChildNodes()) return null;
  tagName = tagName.toUpperCase();
  if(only) only = [only].flatten();
  var elements = [];
  $A(element.childNodes).each( function(e) {
    if(e.tagName && e.tagName.toUpperCase()==tagName &&
      (!only || (Element.classNames(e).detect(function(v) { return only.include(v) }))))
        elements.push(e);
    if(recursive) {
      var grandchildren = Element.findChildren(e, only, recursive, tagName);
      if(grandchildren) elements.push(grandchildren);
    }
  });

  return (elements.length>0 ? elements.flatten() : []);
}

Element.offsetSize = function (element, type) {
  return element['offset' + ((type=='vertical' || type=='height') ? 'Height' : 'Width')];
}
// script.aculo.us controls.js v1.7.0, Fri Jan 19 19:16:36 CET 2007

// Copyright (c) 2005, 2006 Thomas Fuchs (http://script.aculo.us, http://mir.aculo.us)
//           (c) 2005, 2006 Ivan Krstic (http://blogs.law.harvard.edu/ivan)
//           (c) 2005, 2006 Jon Tirsen (http://www.tirsen.com)
// Contributors:
//  Richard Livsey
//  Rahul Bhargava
//  Rob Wills
// 
// script.aculo.us is freely distributable under the terms of an MIT-style license.
// For details, see the script.aculo.us web site: http://script.aculo.us/

// Autocompleter.Base handles all the autocompletion functionality 
// that's independent of the data source for autocompletion. This
// includes drawing the autocompletion menu, observing keyboard
// and mouse events, and similar.
//
// Specific autocompleters need to provide, at the very least, 
// a getUpdatedChoices function that will be invoked every time
// the text inside the monitored textbox changes. This method 
// should get the text for which to provide autocompletion by
// invoking this.getToken(), NOT by directly accessing
// this.element.value. This is to allow incremental tokenized
// autocompletion. Specific auto-completion logic (AJAX, etc)
// belongs in getUpdatedChoices.
//
// Tokenized incremental autocompletion is enabled automatically
// when an autocompleter is instantiated with the 'tokens' option
// in the options parameter, e.g.:
// new Ajax.Autocompleter('id','upd', '/url/', { tokens: ',' });
// will incrementally autocomplete with a comma as the token.
// Additionally, ',' in the above example can be replaced with
// a token array, e.g. { tokens: [',', '\n'] } which
// enables autocompletion on multiple tokens. This is most 
// useful when one of the tokens is \n (a newline), as it 
// allows smart autocompletion after linebreaks.

if(typeof Effect == 'undefined')
  throw("controls.js requires including script.aculo.us' effects.js library");

var Autocompleter = {}
Autocompleter.Base = function() {};
Autocompleter.Base.prototype = {
  baseInitialize: function(element, update, options) {
    this.element     = $(element); 
    this.update      = $(update);  
    this.hasFocus    = false; 
    this.changed     = false; 
    this.active      = false; 
    this.index       = 0;     
    this.entryCount  = 0;

    if(this.setOptions)
      this.setOptions(options);
    else
      this.options = options || {};

    this.options.paramName    = this.options.paramName || this.element.name;
    this.options.tokens       = this.options.tokens || [];
    this.options.frequency    = this.options.frequency || 0.4;
    this.options.minChars     = this.options.minChars || 1;
    this.options.onShow       = this.options.onShow || 
      function(element, update){ 
        if(!update.style.position || update.style.position=='absolute') {
          update.style.position = 'absolute';
          Position.clone(element, update, {
            setHeight: false, 
            offsetTop: element.offsetHeight
          });
        }
        Effect.Appear(update,{duration:0.15});
      };
    this.options.onHide = this.options.onHide || 
      function(element, update){ new Effect.Fade(update,{duration:0.15}) };

    if(typeof(this.options.tokens) == 'string') 
      this.options.tokens = new Array(this.options.tokens);

    this.observer = null;
    
    this.element.setAttribute('autocomplete','off');

    Element.hide(this.update);

    Event.observe(this.element, "blur", this.onBlur.bindAsEventListener(this));
    Event.observe(this.element, "keypress", this.onKeyPress.bindAsEventListener(this));
  },

  show: function() {
    if(Element.getStyle(this.update, 'display')=='none') this.options.onShow(this.element, this.update);
    if(!this.iefix && 
      (navigator.appVersion.indexOf('MSIE')>0) &&
      (navigator.userAgent.indexOf('Opera')<0) &&
      (Element.getStyle(this.update, 'position')=='absolute')) {
      new Insertion.After(this.update, 
       '<iframe id="' + this.update.id + '_iefix" '+
       'style="display:none;position:absolute;filter:progid:DXImageTransform.Microsoft.Alpha(opacity=0);" ' +
       'src="javascript:false;" frameborder="0" scrolling="no"></iframe>');
      this.iefix = $(this.update.id+'_iefix');
    }
    if(this.iefix) setTimeout(this.fixIEOverlapping.bind(this), 50);
  },
  
  fixIEOverlapping: function() {
    Position.clone(this.update, this.iefix, {setTop:(!this.update.style.height)});
    this.iefix.style.zIndex = 1;
    this.update.style.zIndex = 2;
    Element.show(this.iefix);
  },

  hide: function() {
    this.stopIndicator();
    if(Element.getStyle(this.update, 'display')!='none') this.options.onHide(this.element, this.update);
    if(this.iefix) Element.hide(this.iefix);
  },

  startIndicator: function() {
    if(this.options.indicator) Element.show(this.options.indicator);
  },

  stopIndicator: function() {
    if(this.options.indicator) Element.hide(this.options.indicator);
  },

  onKeyPress: function(event) {
    if(this.active)
      switch(event.keyCode) {
       case Event.KEY_TAB:
       case Event.KEY_RETURN:
         this.selectEntry();
         Event.stop(event);
       case Event.KEY_ESC:
         this.hide();
         this.active = false;
         Event.stop(event);
         return;
       case Event.KEY_LEFT:
       case Event.KEY_RIGHT:
         return;
       case Event.KEY_UP:
         this.markPrevious();
         this.render();
         if(navigator.appVersion.indexOf('AppleWebKit')>0) Event.stop(event);
         return;
       case Event.KEY_DOWN:
         this.markNext();
         this.render();
         if(navigator.appVersion.indexOf('AppleWebKit')>0) Event.stop(event);
         return;
      }
     else 
       if(event.keyCode==Event.KEY_TAB || event.keyCode==Event.KEY_RETURN || 
         (navigator.appVersion.indexOf('AppleWebKit') > 0 && event.keyCode == 0)) return;

    this.changed = true;
    this.hasFocus = true;

    if(this.observer) clearTimeout(this.observer);
      this.observer = 
        setTimeout(this.onObserverEvent.bind(this), this.options.frequency*1000);
  },

  activate: function() {
    this.changed = false;
    this.hasFocus = true;
    this.getUpdatedChoices();
  },

  onHover: function(event) {
    var element = Event.findElement(event, 'LI');
    if(this.index != element.autocompleteIndex) 
    {
        this.index = element.autocompleteIndex;
        this.render();
    }
    Event.stop(event);
  },
  
  onClick: function(event) {
    var element = Event.findElement(event, 'LI');
    this.index = element.autocompleteIndex;
    this.selectEntry();
    this.hide();
  },
  
  onBlur: function(event) {
    // needed to make click events working
    setTimeout(this.hide.bind(this), 250);
    this.hasFocus = false;
    this.active = false;     
  }, 
  
  render: function() {
    if(this.entryCount > 0) {
      for (var i = 0; i < this.entryCount; i++)
        this.index==i ? 
          Element.addClassName(this.getEntry(i),"selected") : 
          Element.removeClassName(this.getEntry(i),"selected");
        
      if(this.hasFocus) { 
        this.show();
        this.active = true;
      }
    } else {
      this.active = false;
      this.hide();
    }
  },
  
  markPrevious: function() {
    if(this.index > 0) this.index--
      else this.index = this.entryCount-1;
    this.getEntry(this.index).scrollIntoView(true);
  },
  
  markNext: function() {
    if(this.index < this.entryCount-1) this.index++
      else this.index = 0;
    this.getEntry(this.index).scrollIntoView(false);
  },
  
  getEntry: function(index) {
    return this.update.firstChild.childNodes[index];
  },
  
  getCurrentEntry: function() {
    return this.getEntry(this.index);
  },
  
  selectEntry: function() {
    this.active = false;
    this.updateElement(this.getCurrentEntry());
  },

  updateElement: function(selectedElement) {
    if (this.options.updateElement) {
      this.options.updateElement(selectedElement);
      return;
    }
    var value = '';
    if (this.options.select) {
      var nodes = document.getElementsByClassName(this.options.select, selectedElement) || [];
      if(nodes.length>0) value = Element.collectTextNodes(nodes[0], this.options.select);
    } else
      value = Element.collectTextNodesIgnoreClass(selectedElement, 'informal');
    
    var lastTokenPos = this.findLastToken();
    if (lastTokenPos != -1) {
      var newValue = this.element.value.substr(0, lastTokenPos + 1);
      var whitespace = this.element.value.substr(lastTokenPos + 1).match(/^\s+/);
      if (whitespace)
        newValue += whitespace[0];
      this.element.value = newValue + value;
    } else {
      this.element.value = value;
    }
    this.element.focus();
    
    if (this.options.afterUpdateElement)
      this.options.afterUpdateElement(this.element, selectedElement);
  },

  updateChoices: function(choices) {
    if(!this.changed && this.hasFocus) {
      this.update.innerHTML = choices;
      Element.cleanWhitespace(this.update);
      Element.cleanWhitespace(this.update.down());

      if(this.update.firstChild && this.update.down().childNodes) {
        this.entryCount = 
          this.update.down().childNodes.length;
        for (var i = 0; i < this.entryCount; i++) {
          var entry = this.getEntry(i);
          entry.autocompleteIndex = i;
          this.addObservers(entry);
        }
      } else { 
        this.entryCount = 0;
      }

      this.stopIndicator();
      this.index = 0;
      
      if(this.entryCount==1 && this.options.autoSelect) {
        this.selectEntry();
        this.hide();
      } else {
        this.render();
      }
    }
  },

  addObservers: function(element) {
    Event.observe(element, "mouseover", this.onHover.bindAsEventListener(this));
    Event.observe(element, "click", this.onClick.bindAsEventListener(this));
  },

  onObserverEvent: function() {
    this.changed = false;   
    if(this.getToken().length>=this.options.minChars) {
      this.startIndicator();
      this.getUpdatedChoices();
    } else {
      this.active = false;
      this.hide();
    }
  },

  getToken: function() {
    var tokenPos = this.findLastToken();
    if (tokenPos != -1)
      var ret = this.element.value.substr(tokenPos + 1).replace(/^\s+/,'').replace(/\s+$/,'');
    else
      var ret = this.element.value;

    return /\n/.test(ret) ? '' : ret;
  },

  findLastToken: function() {
    var lastTokenPos = -1;

    for (var i=0; i<this.options.tokens.length; i++) {
      var thisTokenPos = this.element.value.lastIndexOf(this.options.tokens[i]);
      if (thisTokenPos > lastTokenPos)
        lastTokenPos = thisTokenPos;
    }
    return lastTokenPos;
  }
}

Ajax.Autocompleter = Class.create();
Object.extend(Object.extend(Ajax.Autocompleter.prototype, Autocompleter.Base.prototype), {
  initialize: function(element, update, url, options) {
    this.baseInitialize(element, update, options);
    this.options.asynchronous  = true;
    this.options.onComplete    = this.onComplete.bind(this);
    this.options.defaultParams = this.options.parameters || null;
    this.url                   = url;
  },

  getUpdatedChoices: function() {
    entry = encodeURIComponent(this.options.paramName) + '=' + 
      encodeURIComponent(this.getToken());

    this.options.parameters = this.options.callback ?
      this.options.callback(this.element, entry) : entry;

    if(this.options.defaultParams) 
      this.options.parameters += '&' + this.options.defaultParams;

    new Ajax.Request(this.url, this.options);
  },

  onComplete: function(request) {
    this.updateChoices(request.responseText);
  }

});

// The local array autocompleter. Used when you'd prefer to
// inject an array of autocompletion options into the page, rather
// than sending out Ajax queries, which can be quite slow sometimes.
//
// The constructor takes four parameters. The first two are, as usual,
// the id of the monitored textbox, and id of the autocompletion menu.
// The third is the array you want to autocomplete from, and the fourth
// is the options block.
//
// Extra local autocompletion options:
// - choices - How many autocompletion choices to offer
//
// - partialSearch - If false, the autocompleter will match entered
//                    text only at the beginning of strings in the 
//                    autocomplete array. Defaults to true, which will
//                    match text at the beginning of any *word* in the
//                    strings in the autocomplete array. If you want to
//                    search anywhere in the string, additionally set
//                    the option fullSearch to true (default: off).
//
// - fullSsearch - Search anywhere in autocomplete array strings.
//
// - partialChars - How many characters to enter before triggering
//                   a partial match (unlike minChars, which defines
//                   how many characters are required to do any match
//                   at all). Defaults to 2.
//
// - ignoreCase - Whether to ignore case when autocompleting.
//                 Defaults to true.
//
// It's possible to pass in a custom function as the 'selector' 
// option, if you prefer to write your own autocompletion logic.
// In that case, the other options above will not apply unless
// you support them.

Autocompleter.Local = Class.create();
Autocompleter.Local.prototype = Object.extend(new Autocompleter.Base(), {
  initialize: function(element, update, array, options) {
    this.baseInitialize(element, update, options);
    this.options.array = array;
  },

  getUpdatedChoices: function() {
    this.updateChoices(this.options.selector(this));
  },

  setOptions: function(options) {
    this.options = Object.extend({
      choices: 10,
      partialSearch: true,
      partialChars: 2,
      ignoreCase: true,
      fullSearch: false,
      selector: function(instance) {
        var ret       = []; // Beginning matches
        var partial   = []; // Inside matches
        var entry     = instance.getToken();
        var count     = 0;

        for (var i = 0; i < instance.options.array.length &&  
          ret.length < instance.options.choices ; i++) { 

          var elem = instance.options.array[i];
          var foundPos = instance.options.ignoreCase ? 
            elem.toLowerCase().indexOf(entry.toLowerCase()) : 
            elem.indexOf(entry);

          while (foundPos != -1) {
            if (foundPos == 0 && elem.length != entry.length) { 
              ret.push("<li><strong>" + elem.substr(0, entry.length) + "</strong>" + 
                elem.substr(entry.length) + "</li>");
              break;
            } else if (entry.length >= instance.options.partialChars && 
              instance.options.partialSearch && foundPos != -1) {
              if (instance.options.fullSearch || /\s/.test(elem.substr(foundPos-1,1))) {
                partial.push("<li>" + elem.substr(0, foundPos) + "<strong>" +
                  elem.substr(foundPos, entry.length) + "</strong>" + elem.substr(
                  foundPos + entry.length) + "</li>");
                break;
              }
            }

            foundPos = instance.options.ignoreCase ? 
              elem.toLowerCase().indexOf(entry.toLowerCase(), foundPos + 1) : 
              elem.indexOf(entry, foundPos + 1);

          }
        }
        if (partial.length)
          ret = ret.concat(partial.slice(0, instance.options.choices - ret.length))
        return "<ul>" + ret.join('') + "</ul>";
      }
    }, options || {});
  }
});

// AJAX in-place editor
//
// see documentation on http://wiki.script.aculo.us/scriptaculous/show/Ajax.InPlaceEditor

// Use this if you notice weird scrolling problems on some browsers,
// the DOM might be a bit confused when this gets called so do this
// waits 1 ms (with setTimeout) until it does the activation
Field.scrollFreeActivate = function(field) {
  setTimeout(function() {
    Field.activate(field);
  }, 1);
}

Ajax.InPlaceEditor = Class.create();
Ajax.InPlaceEditor.defaultHighlightColor = "#FFFF99";
Ajax.InPlaceEditor.prototype = {
  initialize: function(element, url, options) {
    this.url = url;
    this.element = $(element);

    this.options = Object.extend({
      paramName: "value",
      okButton: true,
      okText: "ok",
      cancelLink: true,
      cancelText: "cancel",
      savingText: "Saving...",
      clickToEditText: "Click to edit",
      okText: "ok",
      rows: 1,
      onComplete: function(transport, element) {
        new Effect.Highlight(element, {startcolor: this.options.highlightcolor});
      },
      onFailure: function(transport) {
        alert("Error communicating with the server: " + transport.responseText.stripTags());
      },
      callback: function(form) {
        return Form.serialize(form);
      },
      handleLineBreaks: true,
      loadingText: 'Loading...',
      savingClassName: 'inplaceeditor-saving',
      loadingClassName: 'inplaceeditor-loading',
      formClassName: 'inplaceeditor-form',
      highlightcolor: Ajax.InPlaceEditor.defaultHighlightColor,
      highlightendcolor: "#FFFFFF",
      externalControl: null,
      submitOnBlur: false,
      ajaxOptions: {},
      evalScripts: false
    }, options || {});

    if(!this.options.formId && this.element.id) {
      this.options.formId = this.element.id + "-inplaceeditor";
      if ($(this.options.formId)) {
        // there's already a form with that name, don't specify an id
        this.options.formId = null;
      }
    }
    
    if (this.options.externalControl) {
      this.options.externalControl = $(this.options.externalControl);
    }
    
    this.originalBackground = Element.getStyle(this.element, 'background-color');
    if (!this.originalBackground) {
      this.originalBackground = "transparent";
    }
    
    this.element.title = this.options.clickToEditText;
    
    this.onclickListener = this.enterEditMode.bindAsEventListener(this);
    this.mouseoverListener = this.enterHover.bindAsEventListener(this);
    this.mouseoutListener = this.leaveHover.bindAsEventListener(this);
    Event.observe(this.element, 'click', this.onclickListener);
    Event.observe(this.element, 'mouseover', this.mouseoverListener);
    Event.observe(this.element, 'mouseout', this.mouseoutListener);
    if (this.options.externalControl) {
      Event.observe(this.options.externalControl, 'click', this.onclickListener);
      Event.observe(this.options.externalControl, 'mouseover', this.mouseoverListener);
      Event.observe(this.options.externalControl, 'mouseout', this.mouseoutListener);
    }
  },
  enterEditMode: function(evt) {
    if (this.saving) return;
    if (this.editing) return;
    this.editing = true;
    this.onEnterEditMode();
    if (this.options.externalControl) {
      Element.hide(this.options.externalControl);
    }
    Element.hide(this.element);
    this.createForm();
    this.element.parentNode.insertBefore(this.form, this.element);
    if (!this.options.loadTextURL) Field.scrollFreeActivate(this.editField);
    // stop the event to avoid a page refresh in Safari
    if (evt) {
      Event.stop(evt);
    }
    return false;
  },
  createForm: function() {
    this.form = document.createElement("form");
    this.form.id = this.options.formId;
    Element.addClassName(this.form, this.options.formClassName)
    this.form.onsubmit = this.onSubmit.bind(this);

    this.createEditField();

    if (this.options.textarea) {
      var br = document.createElement("br");
      this.form.appendChild(br);
    }

    if (this.options.okButton) {
      okButton = document.createElement("input");
      okButton.type = "submit";
      okButton.value = this.options.okText;
      okButton.className = 'editor_ok_button';
      this.form.appendChild(okButton);
    }

    if (this.options.cancelLink) {
      cancelLink = document.createElement("a");
      cancelLink.href = "javascript:void(0)";
      cancelLink.appendChild(document.createTextNode(this.options.cancelText));
      cancelLink.onclick = this.onclickCancel.bind(this);
      cancelLink.className = 'editor_cancel';      
      this.form.appendChild(cancelLink);
    }
  },
  hasHTMLLineBreaks: function(string) {
    if (!this.options.handleLineBreaks) return false;
    return string.match(/<br/i) || string.match(/<p>/i);
  },
  convertHTMLLineBreaks: function(string) {
    return string.replace(/<br>/gi, "\n").replace(/<br\/>/gi, "\n").replace(/<\/p>/gi, "\n").replace(/<p>/gi, "");
  },
  createEditField: function() {
    var text;
    if(this.options.loadTextURL) {
      text = this.options.loadingText;
    } else {
      text = this.getText();
    }

    var obj = this;
    
    if (this.options.rows == 1 && !this.hasHTMLLineBreaks(text)) {
      this.options.textarea = false;
      var textField = document.createElement("input");
      textField.obj = this;
      textField.type = "text";
      textField.name = this.options.paramName;
      textField.value = text;
      textField.style.backgroundColor = this.options.highlightcolor;
      textField.className = 'editor_field';
      var size = this.options.size || this.options.cols || 0;
      if (size != 0) textField.size = size;
      if (this.options.submitOnBlur)
        textField.onblur = this.onSubmit.bind(this);
      this.editField = textField;
    } else {
      this.options.textarea = true;
      var textArea = document.createElement("textarea");
      textArea.obj = this;
      textArea.name = this.options.paramName;
      textArea.value = this.convertHTMLLineBreaks(text);
      textArea.rows = this.options.rows;
      textArea.cols = this.options.cols || 40;
      textArea.className = 'editor_field';      
      if (this.options.submitOnBlur)
        textArea.onblur = this.onSubmit.bind(this);
      this.editField = textArea;
    }
    
    if(this.options.loadTextURL) {
      this.loadExternalText();
    }
    this.form.appendChild(this.editField);
  },
  getText: function() {
    return this.element.innerHTML;
  },
  loadExternalText: function() {
    Element.addClassName(this.form, this.options.loadingClassName);
    this.editField.disabled = true;
    new Ajax.Request(
      this.options.loadTextURL,
      Object.extend({
        asynchronous: true,
        onComplete: this.onLoadedExternalText.bind(this)
      }, this.options.ajaxOptions)
    );
  },
  onLoadedExternalText: function(transport) {
    Element.removeClassName(this.form, this.options.loadingClassName);
    this.editField.disabled = false;
    this.editField.value = transport.responseText.stripTags();
    Field.scrollFreeActivate(this.editField);
  },
  onclickCancel: function() {
    this.onComplete();
    this.leaveEditMode();
    return false;
  },
  onFailure: function(transport) {
    this.options.onFailure(transport);
    if (this.oldInnerHTML) {
      this.element.innerHTML = this.oldInnerHTML;
      this.oldInnerHTML = null;
    }
    return false;
  },
  onSubmit: function() {
    // onLoading resets these so we need to save them away for the Ajax call
    var form = this.form;
    var value = this.editField.value;
    
    // do this first, sometimes the ajax call returns before we get a chance to switch on Saving...
    // which means this will actually switch on Saving... *after* we've left edit mode causing Saving...
    // to be displayed indefinitely
    this.onLoading();
    
    if (this.options.evalScripts) {
      new Ajax.Request(
        this.url, Object.extend({
          parameters: this.options.callback(form, value),
          onComplete: this.onComplete.bind(this),
          onFailure: this.onFailure.bind(this),
          asynchronous:true, 
          evalScripts:true
        }, this.options.ajaxOptions));
    } else  {
      new Ajax.Updater(
        { success: this.element,
          // don't update on failure (this could be an option)
          failure: null }, 
        this.url, Object.extend({
          parameters: this.options.callback(form, value),
          onComplete: this.onComplete.bind(this),
          onFailure: this.onFailure.bind(this)
        }, this.options.ajaxOptions));
    }
    // stop the event to avoid a page refresh in Safari
    if (arguments.length > 1) {
      Event.stop(arguments[0]);
    }
    return false;
  },
  onLoading: function() {
    this.saving = true;
    this.removeForm();
    this.leaveHover();
    this.showSaving();
  },
  showSaving: function() {
    this.oldInnerHTML = this.element.innerHTML;
    this.element.innerHTML = this.options.savingText;
    Element.addClassName(this.element, this.options.savingClassName);
    this.element.style.backgroundColor = this.originalBackground;
    Element.show(this.element);
  },
  removeForm: function() {
    if(this.form) {
      if (this.form.parentNode) Element.remove(this.form);
      this.form = null;
    }
  },
  enterHover: function() {
    if (this.saving) return;
    this.element.style.backgroundColor = this.options.highlightcolor;
    if (this.effect) {
      this.effect.cancel();
    }
    Element.addClassName(this.element, this.options.hoverClassName)
  },
  leaveHover: function() {
    if (this.options.backgroundColor) {
      this.element.style.backgroundColor = this.oldBackground;
    }
    Element.removeClassName(this.element, this.options.hoverClassName)
    if (this.saving) return;
    this.effect = new Effect.Highlight(this.element, {
      startcolor: this.options.highlightcolor,
      endcolor: this.options.highlightendcolor,
      restorecolor: this.originalBackground
    });
  },
  leaveEditMode: function() {
    Element.removeClassName(this.element, this.options.savingClassName);
    this.removeForm();
    this.leaveHover();
    this.element.style.backgroundColor = this.originalBackground;
    Element.show(this.element);
    if (this.options.externalControl) {
      Element.show(this.options.externalControl);
    }
    this.editing = false;
    this.saving = false;
    this.oldInnerHTML = null;
    this.onLeaveEditMode();
  },
  onComplete: function(transport) {
    this.leaveEditMode();
    this.options.onComplete.bind(this)(transport, this.element);
  },
  onEnterEditMode: function() {},
  onLeaveEditMode: function() {},
  dispose: function() {
    if (this.oldInnerHTML) {
      this.element.innerHTML = this.oldInnerHTML;
    }
    this.leaveEditMode();
    Event.stopObserving(this.element, 'click', this.onclickListener);
    Event.stopObserving(this.element, 'mouseover', this.mouseoverListener);
    Event.stopObserving(this.element, 'mouseout', this.mouseoutListener);
    if (this.options.externalControl) {
      Event.stopObserving(this.options.externalControl, 'click', this.onclickListener);
      Event.stopObserving(this.options.externalControl, 'mouseover', this.mouseoverListener);
      Event.stopObserving(this.options.externalControl, 'mouseout', this.mouseoutListener);
    }
  }
};

Ajax.InPlaceCollectionEditor = Class.create();
Object.extend(Ajax.InPlaceCollectionEditor.prototype, Ajax.InPlaceEditor.prototype);
Object.extend(Ajax.InPlaceCollectionEditor.prototype, {
  createEditField: function() {
    if (!this.cached_selectTag) {
      var selectTag = document.createElement("select");
      var collection = this.options.collection || [];
      var optionTag;
      collection.each(function(e,i) {
        optionTag = document.createElement("option");
        optionTag.value = (e instanceof Array) ? e[0] : e;
        if((typeof this.options.value == 'undefined') && 
          ((e instanceof Array) ? this.element.innerHTML == e[1] : e == optionTag.value)) optionTag.selected = true;
        if(this.options.value==optionTag.value) optionTag.selected = true;
        optionTag.appendChild(document.createTextNode((e instanceof Array) ? e[1] : e));
        selectTag.appendChild(optionTag);
      }.bind(this));
      this.cached_selectTag = selectTag;
    }

    this.editField = this.cached_selectTag;
    if(this.options.loadTextURL) this.loadExternalText();
    this.form.appendChild(this.editField);
    this.options.callback = function(form, value) {
      return "value=" + encodeURIComponent(value);
    }
  }
});

// Delayed observer, like Form.Element.Observer, 
// but waits for delay after last key input
// Ideal for live-search fields

Form.Element.DelayedObserver = Class.create();
Form.Element.DelayedObserver.prototype = {
  initialize: function(element, delay, callback) {
    this.delay     = delay || 0.5;
    this.element   = $(element);
    this.callback  = callback;
    this.timer     = null;
    this.lastValue = $F(this.element); 
    Event.observe(this.element,'keyup',this.delayedListener.bindAsEventListener(this));
  },
  delayedListener: function(event) {
    if(this.lastValue == $F(this.element)) return;
    if(this.timer) clearTimeout(this.timer);
    this.timer = setTimeout(this.onTimerEvent.bind(this), this.delay * 1000);
    this.lastValue = $F(this.element);
  },
  onTimerEvent: function() {
    this.timer = null;
    this.callback(this.element, $F(this.element));
  }
};
// script.aculo.us slider.js v1.7.0, Fri Jan 19 19:16:36 CET 2007

// Copyright (c) 2005, 2006 Marty Haught, Thomas Fuchs 
//
// script.aculo.us is freely distributable under the terms of an MIT-style license.
// For details, see the script.aculo.us web site: http://script.aculo.us/

if(!Control) var Control = {};
Control.Slider = Class.create();

// options:
//  axis: 'vertical', or 'horizontal' (default)
//
// callbacks:
//  onChange(value)
//  onSlide(value)
Control.Slider.prototype = {
  initialize: function(handle, track, options) {
    var slider = this;
    
    if(handle instanceof Array) {
      this.handles = handle.collect( function(e) { return $(e) });
    } else {
      this.handles = [$(handle)];
    }
    
    this.track   = $(track);
    this.options = options || {};

    this.axis      = this.options.axis || 'horizontal';
    this.increment = this.options.increment || 1;
    this.step      = parseInt(this.options.step || '1');
    this.range     = this.options.range || $R(0,1);
    
    this.value     = 0; // assure backwards compat
    this.values    = this.handles.map( function() { return 0 });
    this.spans     = this.options.spans ? this.options.spans.map(function(s){ return $(s) }) : false;
    this.options.startSpan = $(this.options.startSpan || null);
    this.options.endSpan   = $(this.options.endSpan || null);

    this.restricted = this.options.restricted || false;

    this.maximum   = this.options.maximum || this.range.end;
    this.minimum   = this.options.minimum || this.range.start;

    // Will be used to align the handle onto the track, if necessary
    this.alignX = parseInt(this.options.alignX || '0');
    this.alignY = parseInt(this.options.alignY || '0');
    
    this.trackLength = this.maximumOffset() - this.minimumOffset();

    this.handleLength = this.isVertical() ? 
      (this.handles[0].offsetHeight != 0 ? 
        this.handles[0].offsetHeight : this.handles[0].style.height.replace(/px$/,"")) : 
      (this.handles[0].offsetWidth != 0 ? this.handles[0].offsetWidth : 
        this.handles[0].style.width.replace(/px$/,""));

    this.active   = false;
    this.dragging = false;
    this.disabled = false;

    if(this.options.disabled) this.setDisabled();

    // Allowed values array
    this.allowedValues = this.options.values ? this.options.values.sortBy(Prototype.K) : false;
    if(this.allowedValues) {
      this.minimum = this.allowedValues.min();
      this.maximum = this.allowedValues.max();
    }

    this.eventMouseDown = this.startDrag.bindAsEventListener(this);
    this.eventMouseUp   = this.endDrag.bindAsEventListener(this);
    this.eventMouseMove = this.update.bindAsEventListener(this);

    // Initialize handles in reverse (make sure first handle is active)
    this.handles.each( function(h,i) {
      i = slider.handles.length-1-i;
      slider.setValue(parseFloat(
        (slider.options.sliderValue instanceof Array ? 
          slider.options.sliderValue[i] : slider.options.sliderValue) || 
         slider.range.start), i);
      Element.makePositioned(h); // fix IE
      Event.observe(h, "mousedown", slider.eventMouseDown);
    });
    
    Event.observe(this.track, "mousedown", this.eventMouseDown);
    Event.observe(document, "mouseup", this.eventMouseUp);
    Event.observe(document, "mousemove", this.eventMouseMove);
    
    this.initialized = true;
  },
  dispose: function() {
    var slider = this;    
    Event.stopObserving(this.track, "mousedown", this.eventMouseDown);
    Event.stopObserving(document, "mouseup", this.eventMouseUp);
    Event.stopObserving(document, "mousemove", this.eventMouseMove);
    this.handles.each( function(h) {
      Event.stopObserving(h, "mousedown", slider.eventMouseDown);
    });
  },
  setDisabled: function(){
    this.disabled = true;
  },
  setEnabled: function(){
    this.disabled = false;
  },  
  getNearestValue: function(value){
    if(this.allowedValues){
      if(value >= this.allowedValues.max()) return(this.allowedValues.max());
      if(value <= this.allowedValues.min()) return(this.allowedValues.min());
      
      var offset = Math.abs(this.allowedValues[0] - value);
      var newValue = this.allowedValues[0];
      this.allowedValues.each( function(v) {
        var currentOffset = Math.abs(v - value);
        if(currentOffset <= offset){
          newValue = v;
          offset = currentOffset;
        } 
      });
      return newValue;
    }
    if(value > this.range.end) return this.range.end;
    if(value < this.range.start) return this.range.start;
    return value;
  },
  setValue: function(sliderValue, handleIdx){
    if(!this.active) {
      this.activeHandleIdx = handleIdx || 0;
      this.activeHandle    = this.handles[this.activeHandleIdx];
      this.updateStyles();
    }
    handleIdx = handleIdx || this.activeHandleIdx || 0;
    if(this.initialized && this.restricted) {
      if((handleIdx>0) && (sliderValue<this.values[handleIdx-1]))
        sliderValue = this.values[handleIdx-1];
      if((handleIdx < (this.handles.length-1)) && (sliderValue>this.values[handleIdx+1]))
        sliderValue = this.values[handleIdx+1];
    }
    sliderValue = this.getNearestValue(sliderValue);
    this.values[handleIdx] = sliderValue;
    this.value = this.values[0]; // assure backwards compat
    
    this.handles[handleIdx].style[this.isVertical() ? 'top' : 'left'] = 
      this.translateToPx(sliderValue);
    
    this.drawSpans();
    if(!this.dragging || !this.event) this.updateFinished();
  },
  setValueBy: function(delta, handleIdx) {
    this.setValue(this.values[handleIdx || this.activeHandleIdx || 0] + delta, 
      handleIdx || this.activeHandleIdx || 0);
  },
  translateToPx: function(value) {
    return Math.round(
      ((this.trackLength-this.handleLength)/(this.range.end-this.range.start)) * 
      (value - this.range.start)) + "px";
  },
  translateToValue: function(offset) {
    return ((offset/(this.trackLength-this.handleLength) * 
      (this.range.end-this.range.start)) + this.range.start);
  },
  getRange: function(range) {
    var v = this.values.sortBy(Prototype.K); 
    range = range || 0;
    return $R(v[range],v[range+1]);
  },
  minimumOffset: function(){
    return(this.isVertical() ? this.alignY : this.alignX);
  },
  maximumOffset: function(){
    return(this.isVertical() ? 
      (this.track.offsetHeight != 0 ? this.track.offsetHeight :
        this.track.style.height.replace(/px$/,"")) - this.alignY : 
      (this.track.offsetWidth != 0 ? this.track.offsetWidth : 
        this.track.style.width.replace(/px$/,"")) - this.alignY);
  },  
  isVertical:  function(){
    return (this.axis == 'vertical');
  },
  drawSpans: function() {
    var slider = this;
    if(this.spans)
      $R(0, this.spans.length-1).each(function(r) { slider.setSpan(slider.spans[r], slider.getRange(r)) });
    if(this.options.startSpan)
      this.setSpan(this.options.startSpan,
        $R(0, this.values.length>1 ? this.getRange(0).min() : this.value ));
    if(this.options.endSpan)
      this.setSpan(this.options.endSpan, 
        $R(this.values.length>1 ? this.getRange(this.spans.length-1).max() : this.value, this.maximum));
  },
  setSpan: function(span, range) {
    if(this.isVertical()) {
      span.style.top = this.translateToPx(range.start);
      span.style.height = this.translateToPx(range.end - range.start + this.range.start);
    } else {
      span.style.left = this.translateToPx(range.start);
      span.style.width = this.translateToPx(range.end - range.start + this.range.start);
    }
  },
  updateStyles: function() {
    this.handles.each( function(h){ Element.removeClassName(h, 'selected') });
    Element.addClassName(this.activeHandle, 'selected');
  },
  startDrag: function(event) {
    if(Event.isLeftClick(event)) {
      if(!this.disabled){
        this.active = true;
        
        var handle = Event.element(event);
        var pointer  = [Event.pointerX(event), Event.pointerY(event)];
        var track = handle;
        if(track==this.track) {
          var offsets  = Position.cumulativeOffset(this.track); 
          this.event = event;
          this.setValue(this.translateToValue( 
           (this.isVertical() ? pointer[1]-offsets[1] : pointer[0]-offsets[0])-(this.handleLength/2)
          ));
          var offsets  = Position.cumulativeOffset(this.activeHandle);
          this.offsetX = (pointer[0] - offsets[0]);
          this.offsetY = (pointer[1] - offsets[1]);
        } else {
          // find the handle (prevents issues with Safari)
          while((this.handles.indexOf(handle) == -1) && handle.parentNode) 
            handle = handle.parentNode;
            
          if(this.handles.indexOf(handle)!=-1) {
            this.activeHandle    = handle;
            this.activeHandleIdx = this.handles.indexOf(this.activeHandle);
            this.updateStyles();
            
            var offsets  = Position.cumulativeOffset(this.activeHandle);
            this.offsetX = (pointer[0] - offsets[0]);
            this.offsetY = (pointer[1] - offsets[1]);
          }
        }
      }
      Event.stop(event);
    }
  },
  update: function(event) {
   if(this.active) {
      if(!this.dragging) this.dragging = true;
      this.draw(event);
      // fix AppleWebKit rendering
      if(navigator.appVersion.indexOf('AppleWebKit')>0) window.scrollBy(0,0);
      Event.stop(event);
   }
  },
  draw: function(event) {
    var pointer = [Event.pointerX(event), Event.pointerY(event)];
    var offsets = Position.cumulativeOffset(this.track);
    pointer[0] -= this.offsetX + offsets[0];
    pointer[1] -= this.offsetY + offsets[1];
    this.event = event;
    this.setValue(this.translateToValue( this.isVertical() ? pointer[1] : pointer[0] ));
    if(this.initialized && this.options.onSlide)
      this.options.onSlide(this.values.length>1 ? this.values : this.value, this);
  },
  endDrag: function(event) {
    if(this.active && this.dragging) {
      this.finishDrag(event, true);
      Event.stop(event);
    }
    this.active = false;
    this.dragging = false;
  },  
  finishDrag: function(event, success) {
    this.active = false;
    this.dragging = false;
    this.updateFinished();
  },
  updateFinished: function() {
    if(this.initialized && this.options.onChange) 
      this.options.onChange(this.values.length>1 ? this.values : this.value, this);
    this.event = null;
  }
}/**
 * $Id: jxcore.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx Core
 *
 * Purpose: 
 * Implementation of core and base classes for Jx components.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/ 
/**
 * Class: Jx
 * Jx is a global singleton object that contains the entire Jx library
 * within it.  All Jx functions, attributes and classes are accessed
 * through the global Jx object.  Jx should not create any other
 * global variables, if you discover that it does then please report
 * it as a bug
 */
 
/* LEAVE THIS SEMI-COLON, IT PREVENTS PROBLEMS WITH COMPRESSION */
;

/* firebug console supressor for IE/Safari/Opera */
if (!("console" in window) || !("firebug" in console)) {
    var names = ["log", "debug", "info", "warn", "error", "assert", "dir", "dirxml",
    "group", "groupEnd", "time", "timeEnd", "count", "trace", "profile", "profileEnd"];

    window.console = {};
    for (var i = 0; i < names.length; ++i) {
        window.console[names[i]] = function() {};
    }
}
/* inspired by extjs, removes css image flicker and related problems in IE 6 */
var ua = navigator.userAgent.toLowerCase();
var isIE = ua.indexOf("msie") > -1,
    isIE7 = ua.indexOf("msie 7") > -1;
if(isIE && !isIE7) {
    try {
        document.execCommand("BackgroundImageCache", false, true);
    } catch(e) {}
}

/* Setup global namespace
 * If jxcore is loaded by jx.js, then the namespace and baseURL are
 * already established
 */
if (typeof Jx == 'undefined') {
    var Jx = {};
    /**
     * Property: {Boolean} COMBINED_CSS
     * controls whether Jx uses a single, combined CSS file or
     * individual ones.  The combined version is used automatically
     * if the combined or compressed version of Jx is used,
     * otherwise separate ones are used.
     */ 
    Jx.COMBINED_CSS = true;
    var aScripts = document.getElementsByTagName('SCRIPT');
    for (var i=0; i<aScripts.length; i++) {
        var s = aScripts[i].src;
        //only check for lib/jx because we were loaded by 
        var n = s.indexOf('lib/jx');
        if (n != -1) {
            /**
             * Property: {String} baseURL
             * This is the URL that Jx was loaded from, it is 
             * automatically calculated from the script tag
             * src property that included Jx.
             */ 
            Jx.baseURL = s.substring(0,n);
            break;
        }
    }
} 

/**
 * Property: {Object} importRules
 *
 * an object containing a list of CSS files to be included
 * to make the loaded Jx components function correctly
 */
Jx.importRules = {};
/**
 * Property: {Object} importRules
 *
 * an object containing a list of CSS files to be included
 * to make the loaded Jx components function correctly
 */
Jx.importRulesIE = {};

/**
 * Method: addStyleSheet
 *
 * Individual components of Jx call this function to get their
 * style sheets imported at run time.
 *
 * Parameters:
 *
 *   styleSheet {String} the relative path to the CSS file (relative
 *              to <Jx.baseURL>).
 *
 *   ieOnly {Boolean} if true, then the style sheet is only loaded 
 *          if the browser is Internet Explorer.
 */
Jx.addStyleSheet = function(styleSheet, ieOnly) { 
    if (ieOnly) {
        this.importRulesIE[styleSheet] = styleSheet; 
    } else {
        this.importRules[styleSheet] = styleSheet; 
    }
};

/* everything needs reset.css */
Jx.addStyleSheet('reset.css');

/**
 * Method: applyPNGFilter
 *
 * Static method that applies the PNG Filter Hack for IE browsers
 * when showing 24bit PNG's.  Used automatically for img tags with
 * a class of png24.
 *
 * The filter is applied using a nifty feature of IE that allows javascript to
 * be executed as part of a CSS style rule - this ensures that the hack only
 * gets applied on IE browsers.
 *
 * Parameters:
 *
 *   object {Object} the object (img) to which the filter needs to be applied.
 */
Jx.applyPNGFilter = function(o)  {
   var t=Jx.baseURL + "images/a_pixel.png";
   if( o.src != t ) {
       var s=o.src;
       o.src = t;
       o.runtimeStyle.filter = "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='"+s+"',sizingMethod='scale')";
   }
};

Jx.imgQueue = [];   //The queue of images to be loaded
Jx.imgLoaded = {};  //a hash table of images that have been loaded and cached
Jx.imagesLoading = 0; //counter for number of concurrent image loads 

/**
 * Method: addToImgQueue
 *
 * request that an image be set to a DOM IMG element src attribute.  This puts 
 * the image into a queue and there are private methods to manage that queue
 * and limit image loading to 2 at a time.
 */
Jx.addToImgQueue = function(obj) {
    if (Jx.imgLoaded[obj.src]) {
        //if this image was already requested (i.e. it's in cache) just set it directly
        obj.domElement.src = obj.src;
    } else {
        //otherwise stick it in te queue
        Jx.imgQueue.push(obj);
        Jx.imgLoaded[obj.src] = true;
    }
    //start the queue management process
    Jx.checkImgQueue();
};

/**
 * Method: checkImgQueue
 *
 * An internal method that ensures no more than 2 images are loading at a time.
 */
Jx.checkImgQueue = function() {
    //console.log(this.imgQueue.length+":"+this.imagesLoading);
    while (Jx.imagesLoading < 2 && Jx.imgQueue.length > 0) {
        Jx.loadNextImg();
    }
};

/**
 * Method: loadNextImg
 *
 * An internal method actually populate the DOM element with the image source.
 */
Jx.loadNextImg = function() {
    var obj = Jx.imgQueue.shift();
    if (obj) {
        ++Jx.imagesLoading;
        obj.domElement.onload = function(){--Jx.imagesLoading; Jx.checkImgQueue();};
        obj.domElement.onerror = function(){--Jx.imagesLoading; Jx.checkImgQueue();};
        obj.domElement.src = obj.src;
    }
}; 


/**
  * Class: Jx.Listener
 *
 * Jx.Listener is a mix-in class that performs common listener functions
 * for objects that support listeners.  It is intended to be added to
 * existing classes using the following syntax:
 *
 * (code)
 * Object.extends( MyClass.prototype, Jx.Listener.prototype)
 * (end)
 *
 * The Jx.Listener class functions provide support for managing a list of
 * listeners (add, remove) and dispatching events to listeners (processEvent).
 */
Jx.Listener = Class.create();
Jx.Listener.prototype = {
    /**
     * Method: addListener
     *
     * add a listener to the provided list.
     *
     * Parameters:
     *
     *   list {Array} the array of listeners to add the listener to.
     *
     *   object {Object} the object to add as a listener.
     */
    addListener: function (list,obj) {list.push(obj);},
    /**
     * Method: removeListener
     *
     * remove a listener from the provided list.
     *
     * Parameters:
     *
     * list {Array} the array of listeners to remove the listener from.
     *
     * obj {Object} the object to remove as a listener.
     */
    removeListener: function(list,obj) {
        for(var i=0;i<list.length;i++) {if (list[i] == obj) {list.splice(i,1); break;}}},
    /**
     * Method: processEvent
     *
     * call each listener with a given method and event.
     *
     * Parameters:
     *
     *   list {Array} the array of listeners to call.
     *
     *   fnName {String} the name of a function to call on each listener.
     *
     *   obj {Object} an object to pass to each listener.
     */
    processEvent: function(list,fnName,obj){list.each(function(o){if (o[fnName]) {o[fnName](obj);}});}
};

/**
 * Class: Jx.UniqueId
 *
 * Jx.UniqueId is used to assign unique ids to selected elements
 * This is used to solve a problem where multiple external html
 * fragments are loaded into the DOM via ajax at runtime.  It is
 * not always possible to ensure that every element has a unique
 * id.  This is not a problem if you are using id for CSS styling
 * but if you are using it to access elements using $() then
 * you may get unexpected results.
 *
 * Jx.UniqueId is a mix-in class.  Extend an existing class to
 * enable it to handle unique ids.  Register the ids that you
 * want to be unique and then get a reference to those objects
 * through the interface exposed by this class.
 *
 * The class retrieves the elements by id by walking a dom object
 * and retains references to each of the actual DOM objects
 * you have registered.
 */
Jx.UniqueId = Class.create();
Jx.UniqueId.prototype = {
    /**
     * Property: {Array} uniqueIdRefs
     *
     * an array of references obtained from by registering ids
     */
    uniqueIdRefs: null,
    /**
     * Method: initUniqueId
     * 
     * initialize the UniqueId object.  This must be called prior to
     * calling the <registerIds> function.  Typically, it is called
     * in the constructor of an object that includes Jx.UniqueId.
     */
    initUniqueId: function() { 
        this.uniqueIdRefs = [];
    },
    /**
     * Method: deregisterIds
     *
     * removes all registered ids
     */
    deregisterIds: function() {
        this.uniqueIdRefs.length = 0;
    },
    /**
     * Method: registerIds
     *
     * searches the domObj for each of the ids passed in and
     * obtains a unique reference to them so that subsequent
     * calls to <getObj> will return the right object.
     *
     * Parameters:
     *
     * aIds {Array} an array of strings containing ids of DOM elements
     *      to register.
     *
     * domObj {Object} an HTML element reference to search for unique
     *        ids within
     */
    registerIds: function (aIds, domObj) {
        if (aIds.indexOf(domObj.id) != -1) {
            this.uniqueIdRefs[domObj.id] = domObj;
        }
        for (var i=0; i<domObj.childNodes.length; i++) {
            this.registerIds(aIds, domObj.childNodes[i]);
        }
    },
    /**
     * Method: getObj
     *
     * return an object by id if it was previously registered
     *
     * Parameters:
     * 
     * id {String} the original registered id to get the DOM object for
     *
     * Returns:
     *
     * {Object} returns an object or null if the requested id did not
     * exist in the original DOM object or if the id was not registered.
     */
    getObj: function(id) {
        return this.uniqueIdRefs[id] || null;
    }
};

/**
 * Class: Jx.Action
 *
 * Jx.Action is a utility class that provides a mechanism to separate
 * the user interface code from the implementation code for a particular
 * piece of functionality.  A Jx.Action is used primarily as the basis for
 * clickable UI elements such as Jx.Button and Jx.MenuItem that need to
 * execute a particular function when the user clicks them.  The Jx.Action
 * includes a mechanism for controlling the state of the action, allowing
 * an application to enable or disable an action at any time.  A single
 * Jx.Action may be used with multiple buttons and menu items, allowing 
 * the application to easily keep various user interface elements
 * synchronized without having to explicitly maintain all of them.
 *
 * A new instance of Jx.Action is created by passing a function object
 * to the constructor.  The function object may be a function name or
 * the result of using the Prototype bind() function.
 *
 * For example:
 *
 * (code)
 * //Example 1:
 * //use a function reference directly
 * function myFunction() { alert('my function'); }
 * var action = new Jx.Action(myFunction);
 *
 * //Example 2:
 * //use a function bound to an object through bind()
 * var myClass = Class.create();
 * myClass.prototype = { 
 *   initialize: function() {this.x = 1;}, 
 *   getX: function() {alert(this.x); }
 * };
 *
 * var myInstance = new myClass();
 * var action = new Jx.Action(myInstance.getX.bind(myInstance));
 *
 * (end)
 *
 * To enable or disable a Jx.Action (and consequently update any associated
 * buttons or menu items), use the setEnabled([true|false]) method.
 *
 * For example:
 *
 * (code)
 * //disable an action
 * action.setEnabled(false);
 * (end)
 */
Jx.Action = Class.create();
Jx.Action.prototype = {
    /**
     * Property: {Array} pcl
     * an array of property change listeners attached to this action
     */
    pcl: null,
    /**
     * Property: {Boolean} enabled
     *
     * whether the action (and its associated interface object) is
     * currently enabled or not.  This is controlled through the
     * setEnabled function
     */
    enabled : null,
    /**
     * Constructor: Jx.Action
     * 
     * construct a new instance of Jx.Action that invokes a function
     * when activated
     *
     * Parameters: 
     *
     * f - {Function} the function that this action triggers
     */
    initialize: function(f) {
        this.pcl = [];
        this.enabled = true;
        this.actionPerformed = f;
    },
    /**
     * Method: addPropertyChangeListener
     *
     * add a property change listener to this action.  When the enabled
     * state of the action changes, all property change listeners are
     * notified through their propertyChanged method.
     *
     * Parameters: 
     *
     * obj - {Object} the object to notify of property changes
     */
    addPropertyChangeListener: function(obj){this.addListener(this.pcl, obj);},
    /**
     * Method: removePropertyChangeListener
     *
     * remove a property change listener from this action.
     *
     * Parameters: 
     *
     * obj - {Object} the property change listener to remove.
     */
    removePropertyChangeListener: function(obj) {
        this.removeListener(this.pcl, obj);
    },
    /**
     * Method: isEnabled
     * 
     * return whether the action is currently enabled or not.
     *
     * Return: {Boolean}
     */
    isEnabled: function() {return this.enabled;},
    /**
     * Method: setEnabled
     *
     * set the state of this action.
     *
     * Parameters: 
     *
     * b - {Boolean} a boolean value to set enabled state of this action to.
     */
    setEnabled: function(b){
        /* prevent unnecessary propogation of propertyChanged */
        if (this.enabled == b) {
            return;
        }
        this.enabled = b;
        this.processEvent(this.pcl,'propertyChanged',this);
    },
    /**
     * Method: bindTo
     *
     * convenience function to bind an item to this action.  This
     * adds the item as a property change listener to the action
     * and adds the action as an ActionListener to the item.
     *
     * Parameters: 
     *
     * item - {Object} the object to bind the action to.
     */
    bindTo : function( item ) {
        this.addPropertyChangeListener(item);
        item.addActionListener(this);
    },
    /**
     * Method: unbindFrom
     *
     * convenience function to undo a binding between an object and
     * this action.
     *
     * Parameters: 
     *
     * item - {Object} the object to unbind from this action.
     */
    unbindFrom: function(item) {
        this.removePropertyChangeListener(item);
        item.removeActionListener(this);
    },
    /**
     * Method: actionPerformed
     *
     * placeholder function to conform to the ActionListener interface.
     *
     * Parameters: 
     *
     * obj - {Object} the object that performed the action.
     */
    actionPerformed : function(obj) { alert('actionPerformed'); }
};
Object.extend(Jx.Action.prototype, Jx.Listener.prototype);

/**
 * Class: Element
 *
 * Element is a global object provided by the prototype.js library.  The
 * functions documented here are extensions to the Element object provided
 * by Jx to make cross-browser compatibility easier to achieve.
 */
Object.extend( Element, {
    /**
     * Method: getBoxSizing
     *
     * return the box sizing of an element, one of 'content-box' or 
     *'border-box'.
     *
     * Parameters: 
     *
     * elem - {Object} the element to get the box sizing of.
     *
     * Return: {String} the box sizing of the element.
     */
    getBoxSizing : function(elem) {
      var result = 'content-box';
      elem = $(elem);
      if (elem.currentStyle || window.opera) { 
          var cm = document["compatMode"];
          if (cm == "BackCompat" || cm == "QuirksMode") { 
              result = 'border-box'; 
          } else {
              result = 'content-box'; 
        }
      } else {
          if (arguments.length == 0) {
              node = document.documentElement; 
          }
          var sizing = Element.getStyle(elem, "-moz-box-sizing");
          if (!sizing) { 
              sizing = Element.getStyle(elem, "box-sizing"); 
          }
          result = (sizing ? sizing : 'content-box');
      }
      return result;
    },
    /**
     * Method: getContentBoxSize
     *
     * return the size of the content area of an element.  This is the size of
     * the element less margins, padding, and borders.
     *
     * Parameters: 
     *
     * elem - {Object} the element to get the content size of.
     *
     * Return: {Object} an object with two properties, width and height, that
     * are the size of the content area of the measured element.
     */
    getContentBoxSize : function(elem) {
      elem = $(elem);
      Element.toggleMeasurable(elem);
      var w = elem.offsetWidth;
      var h = elem.offsetHeight;
      var padding = Element.getPaddingSize(elem);
      var border = Element.getBorderSize(elem);
      Element.toggleMeasurable(elem);
      w = w - padding.left - padding.right - border.left - border.right;
      h = h - padding.bottom - padding.top - border.bottom - border.top;
      return {width: w, height: h};
    },
    /**
     * Method: getBorderBoxSize
     *
     * return the size of the border area of an element.  This is the size of
     * the element less margins.
     *
     * Parameters: 
     *
     * elem - {Object} the element to get the border sizing of.
     *
     * Return: {Object} an object with two properties, width and height, that
     * are the size of the border area of the measured element.
     */
    getBorderBoxSize: function(elem) {
      elem = $(elem);
      Element.toggleMeasurable(elem);
      var w = elem.offsetWidth;
      var h = elem.offsetHeight;
      Element.toggleMeasurable(elem);
      return {width: w, height: h}; 
    },
    /**
     * Method: setContentBoxSize
     *
     * set either or both of the width and height of an element to
     * the provided size.  This function ensures that the content
     * area of the element is the requested size and the resulting
     * size of the element may be larger depending on padding and
     * borders.
     *
     * Parameters: 
     *
     * elem - {Object} the element to set the content area of.
     *
     * size - {Object} an object with a width and/or height property that is the size to set
     * the content area of the element to.
     */
    setContentBoxSize : function(elem, size) {
        elem = $(elem);
        if (Element.getBoxSizing(elem) == 'border-box') {
            var padding = Element.getPaddingSize(elem);
            var border = Element.getBorderSize(elem);
            if (typeof size.width != 'undefined') {
                var width = (size.width + padding.left + padding.right + border.left + border.right);
                if (width < 0) {
                    width = 0;
                }
                elem.style.width = width + 'px';
            }
            if (typeof size.height != 'undefined') {
                var height = (size.height + padding.top + padding.bottom + border.top + border.bottom);
                if (height < 0) {
                    height = 0;
                }
                elem.style.height = height + 'px';
            }
        } else {
            if (typeof size.width != 'undefined') {
                elem.style.width = size.width + 'px';
            }
            if (typeof size.height != 'undefined') {
                elem.style.height = size.height + 'px';
            }
        }
    },
    /**
     * Method: setBorderBoxSize
     *
     * set either or both of the width and height of an element to
     * the provided size.  This function ensures that the border
     * size of the element is the requested size and the resulting
     * content areaof the element may be larger depending on padding and
     * borders.
     *
     * Parameters: 
     *
     * elem - {Object} the element to set the border size of.
     *
     * size - {Object} an object with a width and/or height property that is the size to set
     * the content area of the element to.
     */
    setBorderBoxSize : function(elem, size) {
      elem = $(elem);
      if (Element.getBoxSizing(elem) == 'content-box') {
        var padding = Element.getPaddingSize(elem);
        var border = Element.getBorderSize(elem);
        var margin = Element.getMarginSize(elem);
        if (typeof size.width != 'undefined') {
          var width = (size.width - padding.left - padding.right - border.left - border.right - margin.left - margin.right);
          if (width < 0) {
            width = 0;
          }
          elem.style.width = width + 'px';
        }
        if (typeof size.height != 'undefined') {
          var height = (size.height - padding.top - padding.bottom - border.top - border.bottom - margin.top - margin.bottom);
          if (height < 0) {
            height = 0;
          }
          elem.style.height = height + 'px';
        }
      } else {
        if (typeof size.width != 'undefined' && size.width >= 0) {
          elem.style.width = size.width + 'px';
        }
        if (typeof size.height != 'undefined' && size.height >= 0) {
          elem.style.height = size.height + 'px';
        }
      }
    },
    /**
     * Method: getPaddingSize
     *
     * returns the padding for each edge of an element
     *
     * Parameters: 
     *
     * elem - {Object} The element to get the padding for.
     *
     * Return: {Object} an object with properties left, top, right and bottom
     * that contain the associated padding values.
     */
    getPaddingSize : function (elem) {
      elem = $(elem);
      Element.toggleMeasurable(elem);
      var l = Element.getNumber(Element.getStyle(elem, 'padding-left'));
      var t = Element.getNumber(Element.getStyle(elem, 'padding-top'));
      var r = Element.getNumber(Element.getStyle(elem, 'padding-right'));
      var b = Element.getNumber(Element.getStyle(elem, 'padding-bottom'));
      Element.toggleMeasurable(elem);
      return {left:l, top:t, right: r, bottom: b};
    },
    /**
     * Method: getBorderSize
     *
     * returns the border size for each edge of an element
     *
     * Parameters: 
     *
     * elem - {Object} The element to get the borders for.
     *
     * Return: {Object} an object with properties left, top, right and bottom
     * that contain the associated border values.
     */
    getBorderSize : function(elem) {
      elem = $(elem);
      Element.toggleMeasurable(elem);
      var l = Element.getNumber(Element.getStyle(elem, 'border-left-width'));
      var t = Element.getNumber(Element.getStyle(elem, 'border-top-width'));
      var r = Element.getNumber(Element.getStyle(elem, 'border-right-width'));
      var b = Element.getNumber(Element.getStyle(elem, 'border-bottom-width'));
      Element.toggleMeasurable(elem);
      return {left:l, top:t, right: r, bottom: b};
    },
    /**
     * Method: getMarginSize
     *
     * returns the margin size for each edge of an element
     *
     * Parameters: 
     *
     * elem - {Object} The element to get the margins for.
     *
     * Return: {Object} an object with properties left, top, right and bottom
     * that contain the associated margin values.
     */
    getMarginSize : function(elem) {
      elem = $(elem);
      Element.toggleMeasurable(elem);
      var l = Element.getNumber(Element.getStyle(elem, 'margin-left'));
      var t = Element.getNumber(Element.getStyle(elem, 'margin-top'));
      var r = Element.getNumber(Element.getStyle(elem, 'margin-right'));
      var b = Element.getNumber(Element.getStyle(elem, 'margin-bottom'));
      Element.toggleMeasurable(elem);
      return {left:l, top:t, right: r, bottom: b};
    },
    /**
     * Method: getNumber
     *
     * safely parse a number and return its integer value.  A NaN value 
     * returns 0.  CSS size values are also parsed correctly.
     *
     * Parameters: 
     *
     * n - {Mixed} the string or object to parse.
     *
     * Return: {Integer} the integer value that the parameter represents
     */
    getNumber: function(n) {
      var result = n==null||isNaN(parseInt(n))?0:parseInt(n);
      return result;
    },
    /**
     * Method: getPageDimensions
     *
     * return the dimensions of the browser client area.
     *
     * Return: {Object} an object containing a width and height property 
     * that represent the width and height of the browser client area.
     */
    getPageDimensions: function() {
        return {width: Element.getInsideWindowWidth(), height: Element.getInsideWindowHeight()};
    },
    /**
     * Method: getInsideWindowWidth
     *
     * returns the width of the browser client area
     *
     * Return: {Integer} the width of the browser client area
     */
    getInsideWindowWidth: function() {
        if (window.innerWidth) {
            return window.innerWidth;
        } else if (document.compatMode && document.compatMode.indexOf("CSS1") >= 0) {
            // measure the html element's clientWidth
            return document.body.parentElement.clientWidth;
        } else if (document.body && document.body.clientWidth) {
            return document.body.clientWidth;
        }
        return 0; 
    },
    /**
     * Method: getInsideWindowHeight
     *
     * returns the height of the browser client area
     *
     * Return: {Integer} the height of the browser client area
     */
    getInsideWindowHeight: function() {
        if (window.innerHeight) {
            return window.innerHeight;
        } else if (document.compatMode && document.compatMode.indexOf("CSS1") >= 0) {
            // measure the html element's clientHeight
            return document.body.parentElement.clientHeight;
        } else if (document.body && document.body.clientHeight) {
            return document.body.clientHeight;
        }
        return 0; 
    },
    /**
     * Method: toggleMeasurable
     *
     * toggles an element's display style property so it can be
     * measured.  If the element has display: none, it is
     * changed to display: block and made temporarily visible
     * so that it can be measured.  Calling this function
     * a second time with the same element will revert the
     * changes.  This allows an element to be measured in
     * various ways.
     *
     * Parameters: 
     *
     * elem - {Object} the element to measure.
     */
    toggleMeasurable: function(elem) {
        if (Element.getStyle(elem, 'display') == 'none') {
            elem.old = {};
            elem.old.display = elem.style.display;
            elem.old.visibility = elem.style.visibility;
            elem.old.position = elem.style.position;
            elem.style.position = 'absolute';
            elem.style.visibility = 'hidden';
            elem.style.display = 'block';
        } else {
            if (elem.old) {
                elem.style.display = elem.old.display;
                elem.style.visibility = elem.old.visibility;
                elem.style.position = elem.old.position;
                elem.old = null;
            }
        }
    }
} );

/**
 * Class: Jx.ContentLoader
 * 
 * ContentLoader is a mix-in class that provides a consistent
 * mechanism for other Jx controls to load content in one of
 * four different ways:
 *
 * o using an existing element, by id
 *
 * o using an existing element, by object reference
 *
 * o using an HTML string
 *
 * o using a URL to get the content remotely
 */
Jx.ContentLoader = Class.create();
Jx.ContentLoader.prototype = {
    /**
     * Property: bContentLoaded
     *
     * tracks the load state of the content, specifically useful
     * in the case of remote content.
     */ 
    bContentLoaded: false,
    /**
     * Method: contentLoaded
     * 
     * callback function that handles remote content
     *
     * Parameters: 
     *
     * element - {Object} the element to put the content into
     *
     * options - {Object} the options that were passed to loadContent originally, only
     * used to get the optional onContentLoaded callback function.
     *
     * r - {XmlHttpRequest} the XmlHttpRequest object that has the content.
     */
    contentLoaded: function(element, options, r) { 
        element.innerHTML = r.responseText;
        this.bContentLoaded = true;
        if (options.onContentLoaded) {
            options.onContentLoaded();
        }
    },
    /**
     * Method: contentLoadFailed
     * 
     * callback function that handles failure to load remote content
     *
     * Parameters: 
     *
     * options - {Object} the options that were passed to loadContent originally, only
     * used to get the optional onContentLoadedFailed callback function.
     *
     * r - {XmlHttpRequest} the XmlHttpRequest object that has the failure code
     */
    contentLoadFailed: function(options, r) {
        this.bContentLoaded = false;
        if (options.onContentLoadFailed) {
            options.onContentLoadFailed();
        }
    },
    /**
     * Method: loadContent
     *
     * triggers loading of content based on parameters passed in the
     * options parameter.  The options parameter can have the following
     * attributes:
     *
     * Parameters: 
     * element - {Object} the element to insert the content into
     * options - {Object} an object containing the attributes indicating what 
     * content to load.
     *
     * Options:
     * content - {HTMLElement} an HTML element that becomes the content
     * contentID - {String} the id of an element to use as the content
     * contentURL - {String} URL to get the content remotely
     * contentHTML - {String} some HTML to use as the innerHTML of the content
     *      area.
     * onContentLoaded - {Function} a function to call when the content is 
     *      loaded.
     * onContentLoadFailed - a function object that is called if the content
     *      fails to load, primarily useful when using the contentURL method of
     *      loading content.
     */     
    loadContent: function(element, options) {
        options = options || {};
        element = $(element);
        if (options.content) {
            element.appendChild(options.content);
            this.bContentLoaded = true;
        } else if (options.contentID) {
            var c = $(options.contentID);
            if (c) {
                element.appendChild(c);
                this.bContentLoaded = true;                
            }
        } else if (options.contentURL) {
            this.bContentLoaded = false;
            var ts = '';//'ts=' + (new Date()).getTime();
            var a = new Ajax.Request( options.contentURL, 
                Object.extend({method:'get',
                               onSuccess:this.contentLoaded.bind(this, element, options), 
                               onFailure: this.contentLoadFailed.bind(this, options),
                               requestHeaders: ['If-Modified-Since', 'Sat, 1 Jan 2000 00:00:00 GMT'],
                               parameters: ts}));
        } else if (options.contentHTML) {
            element.innerHTML = options.contentHTML;
            this.bContentLoaded = true;
        } else {
            this.bContentLoaded = true;
        }
        if (this.bContentLoaded && options.onContentLoaded) {
            options.onContentLoaded();
        }
    }
};/**
 * $Id: jxbutton.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx.Button
 *
 * Purpose: 
 * Implementation of a generic button widget and several
 * useful subclasses.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
/**
 * Class: Jx.Button
 *
 * Jx.Button creates a clickable element in the application that is hooked
 * to an instance of Jx.Action.  When the button is clicked, it activates
 * its associated Jx.Action instance.  When the Jx.Action associated with
 * the button is enabled or disabled, the Jx.Button updates its visual
 * appearance to reflect the current state of the Jx.Action.
 *
 * Visually, a Jx.Button consists of an <A> tag that may contain either
 * an image, a label, or both (the label appears to the right of the button
 * if both are present).  The default styles for Jx.Button expect the
 * image to be 16 x 16 pixels, with a padding of 4px and a border of 1px
 * which results in an element that is 26 pixels high.  The width of the
 * button automatically accomodates the image and label as required.
 *
 * When you construct a new instance of Jx.Button, the button does not
 * automatically get inserted into the application.  Typically a button
 * is used as part of building another capability such as a Jx.Toolbar.
 * However, if you want to manually insert the button into your
 * application, you may use the domObj property of the Jx.Button instance.
 * In this case, you will use one of the DOM manipulation functions such
 * as appendChild, and pass button.domObj.  For example:
 *
 * (code)
 * var button = new Jx.Button(myAction, options);
 * var li = $('myListItem'); //obtain a reference to a DOM element
 * li.appendChild(button.domObj);
 * (end)
 *
 * To use a Jx.Button in an application, you must first create an instance
 * of Jx.Action and then pass it as the first parameter to the Jx.Button
 * constructor.  The second argument to a Jx.Button constructor is an options 
 * object allows configuring the button.
 *
 * (code)
 * Example:
 * function myFunc() { alert('you clicked me'); }
 * var action = new Jx.Action(myFunc);
 * var options = {
 *     imgPath: 'images/mybutton.png',
 *     tooltip: 'click me!',
 *     label: 'click me'
 * }
 * var button = new Jx.Button(action, options);
 * (end)
 */
 
Jx.addStyleSheet('button/button.css');

Jx.Button = Class.create();
Object.extend(Jx.Button.prototype, Jx.Listener.prototype);
Object.extend(Jx.Button.prototype, {
    /**
     * Property: {Array} al
     * an array of action listeners that are attached to the button.
     */
    al: null,
    /**
     * Property: {Object} domObj
     * the HTML element that is inserted into the DOM for this button
     */
    domObj: null,
    /**
     * Property: {Boolean} enabled
     * whether the button is enabled or not.  This is controlled by
     * the Action object that the button is associated with.
     */
    enabled:null,
    /**
     * Constructor: Jx.Button
     * create a new button.
     *
     * Parameters:
     * action - {Object} the action to trigger for this button.
     * options - {Object} an object containing optional properties for this
     * button as below.
     *
     * Options:
     * image - optional.  A string value that is the url to load the image to
     *      display in this button.  The default styles size this image to 16 x 16.
     *      If not provided, then the button will have no icon.
     * tooltip - optional.  A string value to use as the alt/title attribute of
     *      the <A> tag that wraps the button, resulting in a tooltip that appears 
     *      when the user hovers the mouse over a button in most browsers.  If 
     *      not provided, the button will have no tooltip.
     * label - optional.  A string value that is used as a label on the button.
     * disabledClass - optional.  A string value that is the name of a CSS class
     *      to put on the <A> tag if the button is in a disabled state. If not 
     *      provided, the default class jxDisabled is used. You may provide 
     *      your own class or override jxDisabled.
     * imgClass - optional.  A string value that is the name of a CSS class to
     *      put on the <IMG> element.  The default value is jxButtonIcon.
     */
    initialize : function( action, options ) {
        options = options || {};
        this.al = [];
        this.action = action; //TODO move action into options
        
        var imgPath = (options.imgPath || options.image) || '';
        var tooltip = options.tooltip || '';
        var label = options.label || '';
        this.disabledClass = options.disabledClass || 'jxDisabled';
        
        this.domA = document.createElement('a');
        this.domA.className = 'jxButton';
        this.domA.href='javascript:void(0)'; //make hover effects work in IE
        this.domA.onclick = this.onclick.bindAsEventListener(this);
        this.domA.title = tooltip;
        this.domA.alt = tooltip;
        
        var span = document.createElement('span');
        span.className = 'jxButtonSpan';
        this.domA.appendChild(span);
        
        
        // if (imgPath != '') {
        //             this.domImg = document.createElement('span');
        //             this.domImg.className = 'jxButtonIcon';
        //             if (options.imageClass && options.imageClass != '') {
        //                 Element.addClassName(this.domImg, options.imageClass);
        //             }
        //             this.domImg.style.backgroundImage = "url("+imgPath+")";
        //             this.domImg.innerHTML = '&nbsp;&nbsp;';
        //             span.appendChild(this.domImg);
        //         }
        
        this.domLabel = document.createElement('span');
        this.domLabel.className = 'jxButtonContent';
        //this.domLabel.className = label != '' ? 'jxButtonLabel' : ''; //jxButtonLabel jxButtonEmptyLabel';
        this.domLabel.innerHTML = label != '' ? label : '&nbsp;';
        span.appendChild(this.domLabel);

        if (label != '') {
            Element.addClassName(this.domLabel, 'jxButtonLabel');
        }
        
        if (imgPath != '') {
            Element.addClassName(this.domLabel, 'jxButtonIcon');
            this.domLabel.style.backgroundImage = "url("+imgPath+")";
        }
        
        /* if this is an action button, then hook to the action */
        if (this.action) {
            this.action.bindTo(this);
            this.propertyChanged(this.action);            
        }
        
        this.domObj = document.createElement('div');
        this.domObj.className = 'jxButtonContainer';
        this.domObj.appendChild(this.domA);
        
        if (options.halign && options.halign == 'left') {
                Element.addClassName(this.domObj, 'jxButtonContentLeft');                
        }
        if (options.valign && options.valign == 'top') {
            Element.addClassName(this.domObj, 'jxButtonContentTop');
        }
    },
    /**
     * Method: onclick
     * triggered when the user clicks the button, processes the
     * actionPerformed event
     */
    onclick : function() {
        if (this.enabled) {
            this.processEvent(this.al, 'actionPerformed', this);
        }
        return false;
    },
    /**
     * Method: addActionListener
     * add an action listener to the button
     *
     * Parameters: 
     * obj - {Object} the object to add as an action listener
     */
    addActionListener: function(obj) { 
        this.addListener(this.al,obj); 
    },
    /**
     * Method: removeActionListener
     * remove an action listener from the button
     *
     * Parameters: 
     * obj - {Object} the object to remove.
     */
    removeActionListener : function(obj) { 
        this.removeListener(this.al, obj);
    },
    /**
     * Method: propertyChanged
     * implements the PropertyChangeListener interface
     * for handling the enabled state changing on the action
     * associated with this button
     *
     * Parameters:
     * obj - {Object} the action that is changing state
     */
    propertyChanged: function(obj) {
        this.enabled = obj.isEnabled();
        if (this.enabled) {
            Element.removeClassName( this.domObj, this.disabledClass );
        } else {
            Element.addClassName( this.domObj, this.disabledClass );
        }
    },
    /**
     * Method: setImage
     * set the image of this button to a new image URL
     *
     * Parameters:
     * path - {String} the new url to use as the image for this button
     */
    setImage: function(path) {
        if (this.domImg) {
            this.domImg.src = path;
        }
    },
    /**
     * Method: setLabel
     * 
     * sets the text of the button.  Only works if a label was supplied
     * when the button was constructed
     *
     * Parametera: 
     *
     * label - {String} the new label for the button
     */
    setLabel: function(label) {
        if (this.domLabel) {
            this.domLabel.innerHTML = label;
        }
    },
    /**
     * Method: setTooltip
     * sets the tooltip displayed by the button
     *
     * Parameters: 
     * tooltip - {String} the new tooltip
     */
    setTooltip: function(tooltip) {
        if (this.domImg) {
            this.domImg.title = tooltip;
            this.domImg.alt = tooltip;
        }
    }
} );

/**
 * Class: Jx.Button.Flyout
 *
 * Flyout buttons expose a panel when the user clicks the button.  The
 * panel can have arbitrary content.
 *
 * The button itself does not trigger an external action.  You must provide
 * any necessary code to hook up elements in the panel to your application.
 */
Jx.Button.Flyout = Class.create();
Object.extend(Jx.Button.Flyout.prototype, Jx.Button.prototype);
Object.extend(Jx.Button.Flyout.prototype, Jx.ContentLoader.prototype);
Object.extend(Jx.Button.Flyout.prototype, {
    /**
     * Property: content
     * {HTMLElement} a reference to the DOM node that contains the flyout 
     * content
     */
    content: null,
    /**
     * Constructor: Jx.Button.Flyout
     * construct a new instance of a flyout button.  The single options
     * argument takes the same parameters as <Jx.Button.initialize> plus
     * content loading options as per <Jx.ContentLoader>.
     *
     * Parameters: 
     * options - {Object} an options object used to initialize the button.
     * See <Jx.Button::Jx.Button>
     */
    initialize: function(options) {
        options = options || {};
        var a = new Jx.Action(this.show.bind(this));
        Jx.Button.prototype.initialize.apply(this, [a, options]);
        Element.addClassName(this.domA, 'jxButtonFlyout');
        
        /* iframe shim to prevent scrollbars and 
           inputs from showing through the menu */
        this.iframe = document.createElement('iframe');
        this.iframe.className = 'jxMenuShim';
        this.iframe.scrolling = 'no';
        this.iframe.frameborder = 0;
        
        this.content = document.createElement('div');
        Element.addClassName(this.content, 'jxFlyout');
        this.loadContent(this.content, options);
        this.domObj.appendChild(this.content);
        this.content.style.display = 'none';
        
        this.keypressWatcher = this.keypressHandler.bindAsEventListener(this);
        this.hideWatcher = this.clickHandler.bindAsEventListener(this);
    },
    
    /**
     * Method: show
     * Show the flyout content
     */
    show: function() {
        if (!window.opera && !this.iframe.parentNode) {
            this.content.appendChild(this.iframe);
        }
        this.content.style.display = 'block';
        Event.observe(window, 'keypress', this.keypressWatcher);
        Event.observe(document, 'click', this.hideWatcher);
    },
    
    /**
     * Method: hide
     * Hide the flyout content
     */
    hide: function() {
        this.content.style.display = 'none';
        Event.stopObserving(window, 'keypress', this.keypressWatcher);    
        Event.stopObserving(document, 'click', this.hideWatcher);
    },
    /**
     * Method: clickHandler
     * hide flyout if the user clicks outside of the flyout 
     */
    clickHandler: function(e) {
        var elm = Event.element(e);
        if (!Element.descendantOf(elm, this.domObj.parentNode)) {
            this.hide();
        }
    },
    /**
     * Method: keypressHandler
     * hide flyout if the user presses the ESC key 
     */
    keypressHandler: function(e) {
        var charCode=(e.charCode)?e.charCode:e.keyCode;
        if (charCode == Event.KEY_ESC) {
            this.hide();
        }
    }
});

/**
 * Class: Jx.Button.Multi
 * 
 * Multi buttons are used to contain multiple buttons in a drop down list
 * where only one button is actually visible and clickable in the interface.
 * 
 * When the user clicks the active button, it performs its normal action.
 * The user may also click a drop-down arrow to the right of the button and
 * access the full list of buttons.  Clicking a button in the list causes
 * that button to replace the active button in the toolbar and performs
 * the button's regular action.
 *
 * Other buttons can be added to the Multi button using the <add> function.
 */
Jx.Button.Multi = Class.create();
Jx.Button.Multi.prototype = {
    /**
     * Property: {<Jx.Button>} activeButton
     * the currently selected button
     */
    activeButton: null,
    /**
     * Property: {Array} buttons
     * an array of all available buttons
     */
    buttons: null,
    /**
     * Constructor: Jx.Button.Multi
     * construct a new instance of Jx.Button.Multi.
     */
    initialize: function() {
        this.buttons = [];
        this.content = document.createElement('div');
        this.tb = new Jx.Toolbar(this.content, 'right');
        this.flyout = new Jx.Button.Flyout({content: this.content, label: '&nbsp;'});
        Element.addClassName(this.flyout.domObj.firstChild, 'jxButtonMulti');
        
        this.domObj = document.createElement('div');
        //this.domObj.className = 'jxButtonMulti';
        this.buttonContainer = document.createElement('div');
        this.buttonContainer.className = 'jxButtonMultiContainer';
        this.domObj.appendChild(this.buttonContainer);
        this.domObj.appendChild(this.flyout.domObj);
    },
    /**
     * Method: add
     * adds one or more buttons to the Multi button.  The first button
     * added becomes the active button initialize.  This function 
     * takes a variable number of arguments, each of which is expected
     * to be an instance of <Jx.Button>.
     *
     * Parameters: 
     * button - {<Jx.Button>} a <Jx.Button> instance, may be repeated in the parameter list
     */
    add: function() {
        for (var i=0; i<arguments.length; i++) {
            var theButton = arguments[i];
            var action = new Jx.Action(this.setButton.bind(this, theButton));
            var button = new Jx.Button(action, {});
            var click = button.domA.onclick;
            button.domObj = theButton.domObj.cloneNode(true);
            button.domObj.onclick = click;
            this.tb.add(button);
            if (!this.activeButton) {
                this.buttonContainer.appendChild(theButton.domObj);
                this.activeButton = theButton;
            }
        }
    },
    /**
     * Method: setActiveButton
     * update the menu item to be the requested button.
     *
     * Parameters: 
     * button - {<Jx.Button>} a <Jx.Button> instance that was added to this multi button.
     */
    setActiveButton: function(button) {
        while(this.buttonContainer.childNodes.length > 0) {
            this.buttonContainer.removeChild(this.buttonContainer.firstChild);
        }
        this.buttonContainer.appendChild(button.domObj);
    },
    /**
     * Method: setButton
     * update the active button in the menu item, trigger the button's action
     * and hide the flyout that contains the buttons.
     *
     * Parameters: 
     * button - {<Jx.Button>} The button to set as the active button
     */
    setButton: function(button) {
        this.setActiveButton(button);
        button.onclick();
        this.flyout.hide();
    }
};

/**
 * Class: Jx.Button.Picker
 * 
 * A Picker button allows the user to choose an item from a list of items.
 * The items can be any HTML element.  The chosen item is placed into the 
 * menu item.
 */
Jx.Button.Picker = Class.create();
Object.extend(Jx.Button.Picker.prototype, Jx.Button.Flyout.prototype);
Object.extend(Jx.Button.Picker.prototype, {
    /**
     * Property: {HTMLElement} ul
     * the UL element that contains the items to pick from
     */
    ul: null,
    /**
     * Property {HTMLElement} selectedItem
     * the currently selected item
     */
    selectedItem: null,
    /**
     * Constructor: Jx.Button.Picker
     * construct a new instance of <Jx.Button.Picker>
     */
    initialize: function() {
        Jx.Button.Flyout.prototype.initialize.apply(this, arguments);
        this.ul = document.createElement('ul');
        this.content.appendChild(this.ul);
        Element.removeClassName(this.domLabel, 'jxButtonEmptyLabel');
    },
    /**
     * Method: add
     * adds one or more items to the picker, passed as separate arguments.
     *
     * Parameters: 
     * item - {Object} can be present one or more times in the argument list.  The item
     * to be added can be a Jx object with a domObj property, a string,
     * or an HTML element reference.
     */
    add: function() {
        for (var i=0; i<arguments.length; i++) {
            var thing = arguments[i];
            if (thing.domObj) {
                thing = thing.domObj;
            }
            var li = document.createElement('li');
            var a = document.createElement('a');
            
            li.appendChild(a);
            if (typeof(thing) == 'string') {
                a.innerHTML = thing;
            } else {
                a.appendChild(thing);
            }
            this.ul.appendChild(li);
            
            if (!this.selectedItem) {
                this.updateButton(a);
            }
        }
    },
    /**
     * Method: clickHandler
     * handle the user selecting an item in the list
     *
     * Parameters: 
     * e - {Event} the event object associated with the click event
     */
    clickHandler: function(e) {
        var elm = Event.element(e);
        if (!Element.descendantOf(elm, this.domObj.parentNode)) {
            this.hide();
            return;
        }
        var a = Event.findElement(e, 'A');
        if (a && Element.descendantOf(a, this.ul)) {
            this.updateButton(a);
            this.hide();
        }
    },
    /**
     * Method: updateButton
     * updates the button to contain the picked item
     *
     * Parameters: 
     * a - {HTMLElement} the A tag that the user clicked on
     */
    updateButton: function(a) {
        while(this.domLabel.childNodes.length > 0) {
            this.domLabel.removeChild(this.domLabel.firstChild);
        }
        this.domLabel.appendChild(a.firstChild.cloneNode(true));
        this.selectedItem = a.firstChild;
    }
    
});/**
 * $Id: jxcolor.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx.Color
 *
 * Purpose: 
 * Implementation of a color selection panel.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
 
Jx.addStyleSheet('color/color.css');

/**
 * Class: Jx.ColorPanel
 *
 * A Jx.ColorPanel presents a user interface for selecting colors.  Currently,
 * the user can either enter a HEX colour value or select from a palette of
 * web-safe colours.  The user can also enter an opacity value.
 *
 * A Jx.ColorPanel can be embedded anywhere in a web page by appending its
 * <Jx.ColorPanel.domObj> property to an HTML element.  However, a
 * a Jx.Button subclass  is provided ( <Jx.Button.Color> ) that embeds a
 * colour panel inside a button for easy use in toolbars.
 *
 * Colour changes are propogated via a colorChanged listener pattern.  To
 * be notified of changes in a Jx.ColorPanel, register a color change
 * listener that implements a colorChanged method.  For instance:
 *
 * (code)
 * var colorChangeListener = {
 * colorChanged: function(colorPanel) {
 *     alert('the color changed to: ' + colorPanel.color);
 *   }
 * }
 *
 * var colorPanel = new Jx.ColorPanel();
 * colorPanel.addColorChangeListener(colorChangeListener);
 *
 * document.getElementsByTagName('BODY')[0].appendChild(colorPanel.domObj);
 * (end)
 */
Jx.ColorPanel = Class.create();

Jx.ColorPanel.prototype = {
    /**
     * Property: {HTMLElement} domObj
     * the HTML element representing the color panel
     */
    domObj: null,
    /**
     * Property {String} color
     * the currently selected colour, in hex format
     */
    color: null,
    /**
     * Property: {Float} alpha
     * the current alpha value, between 0 (transparent) and 1 (opaque)
     */
    alpha: null,
    /**
     * Property: {Array} ccl
     * color change property listeners
     */
    ccl: null,
    /**
     * Property: {Array} hexColors
     * an array of valid hex values that are used to build a web-safe
     * palette
     */
    hexColors: ['00', '33', '66', '99', 'CC', 'FF'],
    /**
     * Constructor: Jx.ColorPanel
     * initialize a new instance of Jx.ColorPanel
     *
     * Parameters: 
     * options - {Object} an object containing a variable list of optional initialization
     * parameters.
     *
     * Options:
     * color - a colour to initialize the panel with, defaults to #000000
     *         (black) if not specified.
     * alpha - an alpha value to initialize the panel with, defaults to 1
     *         (opaque) if not specified.
     */
    initialize: function(options) {
        
        options = options || {};
        this.keypressWatcher = this.keypressHandler.bind(this);
        
        this.ccl = [];
        
        this.color = options.color || '#000000';
        this.alpha = options.alpha || 1;
        
        this.domObj = document.createElement('div');
        this.domObj.className = 'jxColorPanel';
        
        var top = document.createElement('div');
        top.className = 'jxColorBar';
        
        var d = document.createElement('div');
        d.className = 'jxColorPreview';
        
        // var img = document.createElement('img');
        // img.src = Jx.baseURL + '/color/grid.png';        
        // d.appendChild(img);
        
        this.selectedSwatch = document.createElement('div');
        d.appendChild(this.selectedSwatch);
        
        top.appendChild(d);
        
        this.colorInputLabel = document.createElement('label');
        this.colorInputLabel.className = 'jxColorLabel';
        this.colorInputLabel.innerHTML = '#';
        top.appendChild(this.colorInputLabel);
        
        this.colorInput = document.createElement('input');
        this.colorInput.className = 'jxHexInput';
        this.colorInput.type = 'text';
        this.colorInput.maxLength = 6;
        Event.observe(this.colorInput, 'keyup', this.colorChanged.bind(this));
        Event.observe(this.colorInput, 'blur', this.colorChanged.bind(this));
        Event.observe(this.colorInput, 'change', this.colorChanged.bind(this));
        top.appendChild(this.colorInput);
        
        this.alphaLabel = document.createElement('label');
        this.alphaLabel.className = 'jxAlphaLabel';
        this.alphaLabel.innerHTML = 'alpha (%)';
        top.appendChild(this.alphaLabel);
        
        this.alphaInput = document.createElement('input');
        this.alphaInput.className = 'jxAlphaInput';
        this.alphaInput.type = 'text';
        this.alphaInput.maxLength = 3;
        Event.observe(this.alphaInput, 'keyup', this.alphaChanged.bind(this));
        top.appendChild(this.alphaInput);
        
        this.domObj.appendChild(top);
        
        var a = document.createElement('a');
        a.href="javascript:void(0)";
        a.className = 'jxColorClose';
        a.alt = 'Close';
        a.title = 'Close';
        Event.observe(a, 'click', this.hide.bind(this));
        
        img = document.createElement('img');
        img.className = 'png24';
        img.src = Jx.baseURL + 'images/close.png';
        a.appendChild(img);
        
        this.domObj.appendChild(a);
        
        d = document.createElement('div');
        d.className = 'jxClearer';
        this.domObj.appendChild(d);
        
        var swatchClick = this.swatchClick.bindAsEventListener(this);
        var swatchOver = this.swatchOver.bindAsEventListener(this);
        
        var table = document.createElement('table');
        table.className = 'jxColorGrid';
        var tbody = document.createElement('tbody');
        table.appendChild(tbody);
        for (var i=0; i<12; i++) {
            var tr = document.createElement('tr');
            for (var j=-3; j<18; j++) {
                var bSkip = false;
                var r, g, b;
                /* hacky approach to building first three columns
                 * because I couldn't find a good way to do it
                 * programmatically
                 */
                
                if (j < 0) {
                    if (j == -3 || j == -1) {
                        r = g = b = 0;
                        bSkip = true;
                    } else {
                        if (i<6) {
                            r = g = b = i;
                        } else {
                            if (i == 6) {
                                r = 5; g = 0; b = 0;
                            } else if (i == 7) {
                                r = 0; g = 5; b = 0;
                            } else if (i == 8) {
                                r = 0; g = 0; b = 5;
                            } else if (i == 9) {
                                r = 5; g = 5; b = 0;
                            } else if (i == 10) {
                                r = 0; g = 5; b = 5;
                            } else if (i == 11) {
                                r = 5; g = 0; b = 5;
                            }
                        }
                    }
                } else {
                    /* remainder of the columns are built
                     * based on the current row/column
                     */
                    r = parseInt(i/6)*3 + parseInt(j/6);
                    g = j%6;
                    b = i%6;
                }
                var bgColor = '#'+this.hexColors[r]+this.hexColors[g]+this.hexColors[b];
                
                var td = document.createElement('td');
                if (!bSkip) {
                    td.style.backgroundColor = bgColor;

                    var a = document.createElement('a');
                    if ((r > 2 && g > 2) || (r > 2 && b > 2) || (g > 2 && b > 2)) {
                        a.className = 'colorSwatch borderBlack';
                    } else {
                        a.className = 'colorSwatch borderWhite';
                    }
                    a.href = '#';
                    a.title = bgColor;
                    a.alt = bgColor;
                    a.swatchColor = bgColor;
                    
                    a.onmouseover = swatchOver;
                    a.onclick = swatchClick;

                    td.appendChild(a);
                } else {
                    var span = document.createElement('span');
                    td.className = 'emptyCell';
                    td.appendChild(span);
                }
                tr.appendChild(td);
            }
            tbody.appendChild(tr);
        }

        this.domObj.appendChild(table);
        
        var d = document.createElement('div');
        d.className = 'jxColorPreview';
        this.previewSwatch = document.createElement('div');
        d.appendChild(this.previewSwatch);
        this.previewSwatch.style.backgroundColor = this.color;
        this.domObj.appendChild(d);
        
        this.previewLabel = document.createElement('label');
        this.previewLabel.className = 'jxColorLabel';
        this.previewLabel.innerHTML = this.color;
        this.domObj.appendChild(this.previewLabel);
        
        d = document.createElement('div');
        d.className = 'jxClearer';
        this.domObj.appendChild(d);
        
        this.updateSelected();
    },
    
    /**
     * Method: swatchOver
     * handle the mouse moving over a colour swatch by updating the preview
     *
     * Parameters: 
     * e - {Event} the mousemove event object
     */
    swatchOver: function(e) {
        var a = Event.element(e);
        
        this.previewSwatch.style.backgroundColor = a.swatchColor;
        this.previewLabel.innerHTML = a.swatchColor;
    },
    
    /**
     * Method: swatchClick
     * handle mouse click on a swatch by updating the color and hiding the
     * panel.
     *
     * Parameters: 
     * e - {Event} the mouseclick event object
     */
    swatchClick: function(e) {
        var a = Event.element(e);
        
        this.color = a.swatchColor;
        this.updateSelected();
        this.hide();
    },
    
    /**
     * Method: colorChanged
     * handle the user entering a new colour value manually by updating the
     * selected colour if the entered value is valid HEX.
     */
    colorChanged: function() {
        var color = this.colorInput.value;
        if (color.substring(0,1) == '#') {
            color = color.substring(1);
        }
        if (color.toLowerCase().match(/^[0-9a-f]{6}$/)) {
            this.color = '#' +color.toUpperCase();
            this.updateSelected();
        }
    },
    
    /**
     * Method: alphaChanged
     * handle the user entering a new alpha value manually by updating the
     * selected alpha if the entered value is valid alpha (0-100).
     */
    alphaChanged: function() {
        var alpha = this.alphaInput.value;
        if (alpha.match(/^[0-9]{1,3}$/)) {
            this.alpha = parseFloat(alpha/100);
            this.updateSelected();
        }
    },
    
    /**
     * Method: setColor
     * set the colour represented by this colour panel
     *
     * Parameters: 
     * color - {String} the new hex color value
     */
    setColor: function( color ) {
        this.colorInput.value = color;
        this.colorChanged();
    },
    
    /**
     * Method: setAlpha
     * set the alpha represented by this colour panel
     *
     * Parameters: 
     * alpha - {Integer} the new alpha value (between 0 and 100)
     */
    setAlpha: function( alpha ) {
        this.alphaInput.value = alpha;
        this.alphaChanged();
    },
    
    /**
     * Method: updateSelected
     * update the colour panel user interface based on the current
     * colour and alpha values
     */
    updateSelected: function() {
        this.selectedSwatch.style.backgroundColor = this.color;
        this.colorInput.value = this.color.substring(1);
        
        this.alphaInput.value = parseInt(this.alpha*100);
        if (this.alpha < 1) {
            this.selectedSwatch.style.opacity = this.alpha;
            this.selectedSwatch.style.filter = 'Alpha(opacity='+(this.alpha*100)+')';
        } else {
            this.selectedSwatch.style.opacity = '';
            this.selectedSwatch.style.filter = '';
        }
        this.processEvent(this.ccl,'colorChanged',this);
    },
    
    /**
     * Method: show
     * show the panel
     */
    show: function() {
        this.domObj.style.display = 'block';
        Event.observe(window, 'keypress', this.keypressWatcher);
    },
    
    /**
     * Method: hide
     * hide the panel
     */
    hide: function() {
        this.domObj.style.display = 'none';
        Event.stopObserving(window, 'keypress', this.keypressWatcher);
        
    },
    
    /**
     * Method: keypressHandler
     * handle the user pressing a key.  If the key is the ESC key, then
     * hide the color panel
     *
     * Parameters: 
     * e - {Event} The keypress event
     */
    keypressHandler: function(e) {
        var charCode=(e.charCode)?e.charCode:e.keyCode;
        if (charCode == Event.KEY_ESC) {
            this.hide();
        }
    },
    /**
     * Method: addColorChangeListener
     * add a colour change listener, an object that has a colorChanged
     * function.
     *
     * Parameters: 
     * obj - {Object} The colour change listener to call when the colour changes.
     */
    addColorChangeListener: function(obj){this.addListener(this.ccl, obj);},
    /**
     * Method: removeColorChangeListener
     * remove a previously added colour change listener
     *
     * Parameters: 
     * obj - {Object} The colour change listener to remove
     */
    removeColorChangeListener: function(obj) {
        this.removeListener(this.ccl, obj);
    }
};
Object.extend(Jx.ColorPanel.prototype, Jx.Listener.prototype);

/**
 * Class: Jx.Button.Color
 *
 * A <Jx.ColorPanel> wrapped up in a Jx.Button.  The button includes a
 * preview of the currently selected color.  Clicking the button opens
 * the color panel.
 *
 * Inherits From:
 * <Jx.Button.Flyout>
 */
Jx.Button.Color = Class.create();
Object.extend(Jx.Button.Color.prototype, Jx.Button.Flyout.prototype);
Object.extend(Jx.Button.Color.prototype, {
    /**
     * Property: { Array } colorPanel
     * By declaring this property as an array in the prototype, the
     * array is used for all instances making it a Class property.
     * A <Jx.ColorPanel> instance used by all color buttons is made the
     * first element so that only one color panel is ever created.
     */
    colorPanel: [],
    /**
     * Property: {HTMLElement} swatch
     * a div used to represent the current colour in the button.
     */
    swatch: null,
    /**
     * Property: {String} color
     * the current colour of this button in hex.  We have to maintain
     * it here because we share a colour panel with all other colour
     * buttons
     */
    color: null,
    /**
     * Property: {Integer} alpha
     * the current alpha of this button.  We have to maintain
     * it here because we share a colour panel with all other colour
     * buttons
     */
    alpha: null,
    /**
     * Property: {Array} ccl
     * color change property listeners
     */
    ccl: null,
    /**
     * Constructor: Jx.Button.Color
     * initialize a new colour button.
     *
     * Parameters: 
     * options - {Object} an object containing a variable list of optional initialization
     * parameters.
     *
     * Options:
     * color - a colour to initialize the panel with, defaults to #000000
     *      (black) if not specified.
     * alpha - an alpha value to initialize the panel with, defaults to 1
     *      (opaque) if not specified.
     */
     
    initialize: function(options) {
        options = options || {};
        options.label = options.label || '&nbsp;';
        this.color = options.color || '#000000';
        this.alpha = options.alpha || 100;
        this.ccl = [];
        
        if (this.colorPanel.length == 0) {
            this.colorPanel[0] = new Jx.ColorPanel();
        }
        var d = document.createElement('div');
        d.className = 'jxColorButtonPreview';
        
        this.selectedSwatch = document.createElement('div');
        d.appendChild(this.selectedSwatch);
        
        /* create a button with a label so it doesn't get an empty label class
           and then replace the label text with the swatch */
        Jx.Button.Flyout.prototype.initialize.apply(this, [options]);
        Element.addClassName(this.domObj.firstChild, 'jxButtonColor');
        this.domObj.firstChild.firstChild.insertBefore(d, this.domObj.firstChild.firstChild.firstChild);
        this.updateSwatch();
    },
    
    /**
     * Method: show
     * show the color panel when the user clicks the button
     */
    show: function() {
        if (this.colorPanel[0].currentButton) {
            this.colorPanel[0].currentButton.hide();
        }
        this.colorPanel[0].currentButton = this;
        this.colorPanel[0].addColorChangeListener(this);
        this.content.appendChild(this.colorPanel[0].domObj);
        this.colorPanel[0].domObj.style.display = 'block';
        Jx.Button.Flyout.prototype.show.apply(this, arguments);
        /* setting these before causes an update problem when clicking on
         * a second color button when another one is open - the color
         * wasn't updating properly
         */
        this.colorPanel[0].setColor(this.color);
        this.colorPanel[0].setAlpha(this.alpha);
    },
    
    /**
     * Method: hide
     * hide the colour panel
     */    
    hide: function() {
        this.colorPanel[0].removeColorChangeListener(this);
        Jx.Button.Flyout.prototype.hide.apply(this, arguments);
        this.colorPanel[0].currentButton = null;
    },
    
    /**
     * Method: setColor
     * set the colour represented by this colour panel
     *
     * Parameters: 
     * color - {String} the new hex color value
     */
    setColor: function(color) {
        this.color = color;
        this.updateSwatch();
    },
    
    /**
     * Method: setAlpha
     * set the alpha represented by this colour panel
     *
     * Parameters: 
     * alpha - {Integer} the new alpha value (between 0 and 100)
     */
    setAlpha: function(alpha) {
        this.alpha = alpha;
        this.updateSwatch();
    },
    
    /**
     * Method: colorChanged
     * colorChangeListener callback function when the user changes
     * the colour in the panel (just update the preview).
     */
    colorChanged: function(panel) {
        this.color = panel.color;
        this.alpha = panel.alpha * 100;
        this.updateSwatch();
        this.processEvent(this.ccl,'colorChanged',this);
    },
    
    /**
     * Method: updateSwatch
     * Update the swatch color for the current color
     */
    updateSwatch: function() {
        this.selectedSwatch.style.backgroundColor = this.color;
        if (this.alpha < 100) {
            this.selectedSwatch.style.opacity = this.alpha/100;
            this.selectedSwatch.style.filter = 'Alpha(opacity='+(this.alpha)+')';    
        } else {
            this.selectedSwatch.style.opacity = '';
            this.selectedSwatch.style.filter = '';    
        }
    },
    /**
     * Method: addColorChangeListener
     * add a colour change listener, an object that has a colorChanged
     * function.
     *
     * Parameters: 
     * obj - {Object} The colour change listener to call when the colour changes.
     */
    addColorChangeListener: function(obj){this.addListener(this.ccl, obj);},
    /**
     * Method: removeColorChangeListener
     * remove a previously added colour change listener
     *
     * Parameters: 
     * obj - {Object} The colour change listener to remove
     */
    removeColorChangeListener: function(obj) {
        this.removeListener(this.ccl, obj);
    }
});
/**
 * $Id: jxdialog.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx.Dialog
 *
 * Purpose: 
 * Implementation of a javascript Dialog for Jx.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

Jx.addStyleSheet('dialog/dialog.css');

/**
 * Class: Jx.Dialog
 *
 * A Jx.Dialog implements a floating dialog.  Dialogs represent a useful way
 * to present users with certain information or application controls.
 * Jx.Dialog is designed to provide the same types of features as traditional
 * operating system dialog boxes, including:
 *
 * - dialogs may be modal (user must dismiss the dialog to continue) or 
 * non-modal
 *
 * - dialogs are moveable (user can drag the title bar to move the dialog
 * around)
 *
 * - dialogs may be a fixed size or allow user resizing.
 *
 * - dialogs may have associated help content that is available as a slide-out
 * help panel on the right-hand side of the dialog.
 *
 * Jx.Dialog uses <Jx.ContentLoader> to load content into the content area
 * of the dialog.  Refer to the <Jx.ContentLoader> documentation for details
 * on content options.  Jx.Dialog also uses <Jx.ContentLoader> to load
 * help content, using non-standard options prefixed by 'help' instead of
 * 'content' (see options in initialize below).
 *
 * (code)
 * var dialog = new Jx.Dialog({});
 * (end)
 *
 * Inherits From:
 * <Jx.ContentLoader>, <Jx.UniqueId>
 */
Jx.Dialog = Class.create();
Jx.Dialog.prototype = {
    /**
     * Property: {Function} onClose
     * if specified by the caller, this is a function to invoke
     * when the dialog closes.
     */
    onClose : null,
    /**
     * Property: {Function} onOpen
     * if specified by the caller, this is a function to invoke
     * when the dialog opens.
     */
    onOpen : null,
    /**
     * Property: {Function} onChange
     * if specified by the caller, this is a function to invoke
     * when any input element in the dialog is changed.
     */
    onChange : null,
    /**
     * Property: {String} title
     * the text to display in the title bar of the dialog.
     */
    title : null,
    /**
     * Property: {HTMLElement} content
     * the content area element of the dialog, used to load user-
     * specified content into.
     */
    content : null,
    /**
     * Property: {HTMLElement} action
     * the action area element of the dialog, used for buttons.
     */
    action : null,
    /**
     * Property: {Object} values
     * when dialog content is loaded, the content is inspected for input
     * elements and a record of them is keep in this object for later
     * reference.
     */
    values : null,
    /**
     * Property: {Object} actions
     * when dialog content is loaded, the content is inspected for
     * input elements of type 'button' and a record of them is
     * kept in this object for later reference.
     */
    actions : null,
    /**
     * Property: {Function} handler
     * if specified by the caller, this is a function to invoke
     * when an input of type button is clicked by the user.
     */
    handler : null,
    /**
     * TODO: Jx.Dialog.bContentLoaded
     * remove this property, it is provided by Jx.ContentLoader.
     */
    bContentLoaded : null,
    /**
     * Property: {Array} zIndex
     * class variable containing the last assigned z-index so that
     * dialogs stack correctly.
     */
    zIndex: [101],
    /**
     * Property: {Array} stack
     * class variable containing the list of visible dialogs used to
     * manage the z-index of non-modal dialogs.  This allows the user
     * to click a dialog and have it appear in front of other dialogs.
     */
    stack: [],
    /**
     * Property: {HTMLElement} blanket
     * modal dialogs prevent interaction with the rest of the application
     * while they are open, this element is displayed just under the
     * dialog to prevent the user from clicking anything.
     */
    blanket: null,
    
    /**
     * Property: firstShow
     * {Boolean} true if the dialog has not been made visible yet.  This
     * facilitates resizing of <Jx.Layout> managed elements in the dialog
     */
    firstShow: true,
    
    /**
     * Property: {Boolean} modal
     * whether or not the dialog is modal.
     */
    modal: true,
    /**
     * Constructor: Jx.Dialog
     * Construct a new instance of Jx.Dialog
     *
     * Parameters: 
     * options - {Object} an object containing options for the dialog.
     *
     * Options:
     * onChange - (optional) {Function} a function to call if the user changes
     *      the value of any input in the dialog
     * onClose - (optional) {Function} a function to call when the user closes
     *      the dialog
     * onOpen - (optional) {Function} a function to call when the user opens
     *      the dialog
     * onContentLoaded - (optional) {Function} a function to call when the
     *      content of the dialog has been loaded, used primarily with
     *      asynchronous content
     * handler - (optional) {Function} a function to call when the user clicks
     *      on any input of type 'button' in the dialog
     * modal - (optional) {Boolean} controls whether the dialog will be modal
     *      or not.  The default is to create modal dialogs.
     * top - (optional) {Integer} the distance in pixels to initially place
     *      the dialog from the top edge of its container.  Default is 0 unless
     *      bottom is specified, in which case the top value is not used.
     * bottom - (optional) {Integer} the distance in pixels to initially place
     *      the dialog from the bottom edge of its container.  Default is null, which
     *      means that the dialog is not positioned relative to the bottom of its
     *      container.  If top and bottom are specified, top is used.
     * left - (optional) {Integer} the distance in pixels to initially place
     *      the dialog from the left edge of its container.
     * right - (optional) {Integer} the distance in pixels to initially place
     *      the dialog from the right edge of its container.  Default is null, which
     *      means that the dialog is not positioned relative to the right of its
     *      container.  If left and right are specified, left is used.
     * width - (optional) {Integer} the initial width in pixels of the dialog.
     *      The default value is 250 if not specified.
     * height - (optional) {Integer} the initial height in pixels of the 
     *      dialog. The default value is 250 if not specified.
     * title - (optional) {String} the title of the dialog box.  "New Dialog"
     *      is the default value.
     * titleHeight - (optional) {Integer} the height, in pixels, of the
     *      dialog's title bar.  The default value is 22, and works with the default
     *      dialog styles.  Only change this value is you are overriding the
     *      default style of the dialog's title bar
     * closeImg - (optional) {String} URL to an image to use for the close
     *      button in the dialog title bar area.  The default value is to use
     *      the close image provided by Jx.  Only override this value if you are
     *      changing the default style of the dialog's title bar.
     * helpID - (optional) {String} passed to <Jx.ContentLoader> for loading
     *      help content.
     * helpURL - (optional) {String} passed to <Jx.ContentLoader> for loading
     *      help content.
     * helpHTML - (optional) {String} passed to <Jx.ContentLoader> for loading
     *      help content.
     * help - (optional) {Object} passed to <Jx.ContentLoader> for loading
     *      help content.
     * contentID - (optional) {String} passed to <Jx.ContentLoader> for loading
     *      dialog content.
     * contentURL - (optional) {String} passed to <Jx.ContentLoader> for loading
     *      dialog content.
     * contentHTML - (optional) {String} passed to <Jx.ContentLoader> for loading
     *      dialog content.
     * content - (optional) {Object} passed to <Jx.ContentLoader> for loading
     *      dialog content.
     * buttons - (optional) {Array} an array of strings that contain the labels
     *      for buttons to create in the Action area of the dialog.
     * id - (optional) {String} an HTML ID to assign to the dialog, primarily
     *      used for applying CSS styles to specific dialogs
     * parentObj - (optional) {HTMLElement} a reference to an HTML element that
     *      the dialog is to be contained by.  The default value is for the dialog
     *      to be contained by the body element.
     * resizeable - (optional) {Boolean} determines whether the dialog is
     *      resizeable by the user or not.  Default is false.
     * imageBaseUrl - (optional) {String} the path to the various dialog edge
     *      images.  If not specified, then Jx.baseURL is used.
     */
    initialize: function(options) {
        this.initUniqueId();
        this.values = {};
        this.actions = {};
        this.handler = options.handler?options.handler:null;
        if (options.onChange) {this.onChange = options.onChange;}
        if (options.onClose) {this.onClose = options.onClose;}
        if (options.onOpen) {this.onOpen = options.onOpen;}
        if (options.onContentLoaded) {this.onContentLoaded = options.onContentLoaded;}
        
        this.imageBaseUrl = options.imageBaseUrl || Jx.baseURL + "/images/";
        if (this.imageBaseUrl.slice(-1) != '/') {
            this.imageBaseUrl += '/';
        }
        this.modal = typeof options.modal == 'undefined' ? true : options.modal;
        
        var w = options.width || 250;
        var h = options.height || 250;
        var b = (typeof options.bottom != 'undefined') ? options.bottom : null;
        var r = (typeof options.right != 'undefined') ? options.right : null;
        var t = (typeof options.top != 'undefined') ? options.top : (b != null ? null : 0);
        var l = (typeof options.left != 'undefined') ? options.left : (r != null ? null : 0);
        
        this.blanket = document.createElement('div');
        this.blanket.className = 'jxDialogModal';
        this.blanket.style.display = 'none';
        
        if (!window.opera && this.modal) {
            var iframe = document.createElement('iframe');
            iframe.className = 'jxDialogShim';
            iframe.scrolling = 'no';
            iframe.frameborder = 0;
            this.blanket.appendChild(iframe);
            if (options.parentObj) {
                $(options.parentObj).appendChild(this.blanket);
            } else {
                document.body.appendChild(this.blanket);
                var temp = new Jx.Layout(this.blanket);
                temp.resize();
            }        
        }

        /* dragHandle is the title bar */
        this.dragHandle = document.createElement('div');
        this.dragHandle.innerHTML = options.title || '&nbsp;';
        this.dragHandle.style.width = '100%';
        
        var domObj = document.createElement('div');
        domObj.className = 'jxDialogContainer';
        this.domObj = domObj;
        /* set the id if passed in through options */
        if (options.id) {
            domObj.id = options.id;
        }

        var dialogObj = document.createElement('div');
        dialogObj.className = 'jxDialog';
        this.innerDialogObj = dialogObj;
        
        var titleObj = document.createElement('div');
        titleObj.className = 'jxDialogTitle';
        this.title = titleObj;
        titleObj.appendChild(this.dragHandle);
        
        /* element must be in the page to be measured */
        titleObj.style.visibility = 'hidden';
        document.getElementsByTagName('BODY')[0].appendChild(titleObj);
        this.titleHeight = Element.getBorderBoxSize(titleObj).height;
        document.getElementsByTagName('BODY')[0].removeChild(titleObj);
        titleObj.style.visibility = '';
        
        var contentObj = document.createElement('div');
        contentObj.className = 'jxDialogContent';
        this.content = contentObj;
        
        domObj.appendChild(dialogObj);
        dialogObj.appendChild(titleObj);
        dialogObj.appendChild(contentObj);
                
        new Jx.Layout(dialogObj, {width: w, height: h});
        new Jx.Layout(titleObj, {top: 0, left: 0, right: 0, bottom: null, height: this.titleHeight, width: null});

        /* optional 'action' area for buttons if necessary */
        this.actionHeight = 0;
        if (options.buttons) {
            var actionObj = document.createElement('div');
            actionObj.className = 'jxDialogAction';
            actionObj.style.visibility = 'hidden';
            document.getElementsByTagName('BODY')[0].appendChild(actionObj);
            this.actionHeight = parseInt(Element.getStyle(actionObj, 'height'));
            document.getElementsByTagName('BODY')[0].removeChild(actionObj);
            actionObj.style.visibility = '';
            dialogObj.appendChild(actionObj);
            new Jx.Layout(actionObj, {top: null, left: 0, right: 0, bottom: 0, height: this.actionHeight, width: null});
            this.action = actionObj;
            this.setButtons(options.buttons);
        }

        new Jx.Layout(contentObj, {top: this.titleHeight, left: 0, right: 0, bottom: this.actionHeight});
        
        
        /* build the decorations for the title bar */
        var atag = document.createElement('a');
        atag.href = 'javascript:void(0)';
        atag.className = 'jxDialogCloseButton';
        atag.onclick = this.close.bindAsEventListener(this);
        
        var close = document.createElement('img');
        if(options.closeImg) {
            close.src = options.closeImg;
        }
        else {
            close.src = this.imageBaseUrl + 'icon_close.png';
        }
        close.alt = 'Close Dialog';
        close.title = 'Close Dialog';
        atag.appendChild(close);
        titleObj.appendChild(atag);
        
        if (options.helpID || options.helpHTML || options.helpURL || options.help) {
            /* put a button in the title bar to open help */
            var atag2 = document.createElement('a');
            atag2.href = 'javascript:void(0)';
            atag2.className = 'jxDialogHelpButton';
            atag2.onclick = this.toggleHelp.bindAsEventListener(this);
            var help = document.createElement('img');
            help.src = this.imageBaseUrl + 'icon_quickhelp.png';
            help.alt = 'Help';
            help.title = 'Help';
            atag2.appendChild(help);
            titleObj.appendChild(atag2);

            /* make the help area */
            this.help = document.createElement('div');
            this.help.className = 'jxDialogHelp';
            this.help.style.display = 'none';
            this.help.isVisible = false;
            var helpOpts = {};
            helpOpts.contentID = options.helpID;
            helpOpts.content = options.help;
            helpOpts.contentURL = options.helpURL;
            helpOpts.contentHTML = options.helpHTML;
            helpOpts.onContentLoaded = this.onHelpContentLoaded.bind(this);
            this.loadContent(this.help, helpOpts);
            dialogObj.appendChild(this.help);
        }
        
        var contentOpts = {};
        contentOpts.contentID = options.contentID;
        contentOpts.content = options.content;
        contentOpts.contentURL = options.contentURL;
        contentOpts.contentHTML = options.contentHTML;
        contentOpts.onContentLoaded = this.onDialogContentLoaded.bind(this);
        this.loadContent(contentObj, contentOpts);
                
        /* element must be in the page to be measured */
        dialogObj.style.visibility = 'hidden';
        document.body.appendChild(dialogObj);
        //force an initial size calculation
        dialogObj.resize();
        
        /* the outer box needs to be sized to the border box size plus margins
         * of the inner box.
         */
         
        this.dialogBoxSize = Element.getBorderBoxSize(dialogObj);
        this.dialogBoxMargins = Element.getMarginSize(dialogObj);
        
        var containerSize = {width: this.dialogBoxSize.width, height: this.dialogBoxSize.height};
        containerSize.width += this.dialogBoxMargins.left + this.dialogBoxMargins.right; 
        containerSize.height += this.dialogBoxMargins.top + this.dialogBoxMargins.bottom;
        
        document.body.removeChild(dialogObj);
        dialogObj.style.visibility = '';
        domObj.appendChild(dialogObj);
        
        /* now create overall container with the correct size */
        domObj.style.display = "none";
        
        Element.setBorderBoxSize(domObj, {width:(containerSize.width), height:(containerSize.height)});
        
        domObj.style.position = 'absolute';
        if (t != null) {
            domObj.style.top = (t) + 'px';
        } else {
            domObj.style.bottom = (b) + 'px';
        }
        if (l != null) {
            domObj.style.left = (l) + 'px';
        } else {
            domObj.style.right = (r) + 'px';
        }

        if (options.parentObj) {
            $(options.parentObj).appendChild(domObj);
        } else {
            document.body.appendChild(domObj);            
        }

        /* Background */
        this.DTOffset = 0;
        this.DBOffset = 0;
        this.DROffset = 0;
        this.DLOffset = 0;
        this.decorationOffsets = {top:0, right: 0, bottom: 0, left: 0};
        
        /* top left */
        
        var imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgTL';
        var img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_tl.png';
        img.className = 'png24'; /* apply png hack for IE */
        imgContainer.appendChild(img);
        domObj.appendChild(imgContainer);
        this.decorationOffsets.top += parseInt(Element.getStyle(img,'width'));
        this.decorationOffsets.left += parseInt(Element.getStyle(img,'height'));

        /* top right */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgTR';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_tr.png';
        img.className = 'png24'; /* apply png hack for IE */
        imgContainer.appendChild(img);
        domObj.appendChild(imgContainer);
        this.decorationOffsets.top += parseInt(Element.getStyle(img,'width'));
        this.decorationOffsets.right += parseInt(Element.getStyle(img,'height'));

        /* bottom right */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgBR';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_br.png';
        img.className = 'png24'; /* apply png hack for IE */
        imgContainer.appendChild(img);
        domObj.appendChild(imgContainer);
        this.decorationOffsets.bottom += parseInt(Element.getStyle(img,'width'));
        this.decorationOffsets.right += parseInt(Element.getStyle(img,'height'));

        /* bottom left */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgBL';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_bl.png';
        img.className = 'png24'; /* apply png hack for IE */
        imgContainer.appendChild(img);
        domObj.appendChild(imgContainer);
        this.decorationOffsets.bottom += parseInt(Element.getStyle(img,'width'));
        this.decorationOffsets.left += parseInt(Element.getStyle(img,'height'));

        /* top */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgT';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_t.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.style.width = containerSize.width-this.decorationOffsets.top + 'px';
        imgContainer.appendChild(img);
        domObj.appendChild(imgContainer);
        this.topImg = img;

        /* bottom */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgB';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_b.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.style.width = containerSize.width-this.decorationOffsets.bottom + 'px';
        imgContainer.appendChild(img);
        domObj.appendChild(imgContainer);
        this.bottomImg = img;

        /* left */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgL';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_l.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.style.height = containerSize.height-this.decorationOffsets.left + 'px';
        imgContainer.appendChild(img);
        domObj.appendChild(imgContainer);
        this.leftImg = img;

        /* right */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgR';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_r.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.style.height = containerSize.height-this.decorationOffsets.right + 'px';
        imgContainer.appendChild(img);
        domObj.appendChild(imgContainer);
        this.rightImg = img;

        
        Event.observe(domObj, 'mousedown', this.mouseDown.bind(this));
        Event.observe(this.title, 'mousedown', this.mouseDown.bind(this));
                
        if (options.resizeable) {
            this.resizeHandle = document.createElement('div');
            this.resizeHandle.className = 'jxDialogResize';
            this.resizeHandle.style.position = 'absolute'; //required for Draggable
            this.domObj.appendChild(this.resizeHandle);

            this.resizeHandleSize = {width:parseInt(Element.getStyle(this.resizeHandle,'width')),
                                     height:parseInt(Element.getStyle(this.resizeHandle,'height'))};

            this.resizeHandle.style.top = (containerSize.height-this.resizeHandleSize.height) + 'px';
            this.resizeHandle.style.left = (containerSize.width-this.resizeHandleSize.width) + 'px';
            new Draggable(this.resizeHandle, {starteffect: false, endeffect: false,change:this.ondrag.bind(this), zindex: 0});
        }
        
        this.bOpen = false;
    },
    /**
     * Method: mouseDown
     * handle the user clicking the title area, promote the dialog to the
     * top of the stack so it is fully visible to the user.
     */
    mouseDown: function() {
        for (var i=0; i<this.stack.length; i++) {
            if (this.stack[i] == this) {
                this.stack.splice(i, 1);
                this.stack.push(this);
            }
        }
        for (var i=0; i<this.stack.length; i++) {
            this.stack[i].domObj.style.zIndex = (101 + i);
        }
    },
    /** 
     * Method: ondrag
     * callback function for user-resizing of the dialog
     *
     * Parameters: 
     * obj - {Object} a drag object that indicates what happened.
     */
    ondrag: function(obj) {
        this.mouseDown();
        
        var delta = obj.currentDelta();
        //delta is top/left of resize image.  Bottom/right of the dialog needs to be
        //adjusted for size of image (20x20).
        var deltaX = delta[0] + this.resizeHandleSize.width;
        var deltaY = delta[1] + this.resizeHandleSize.height;
        this.resize({width: deltaX, height: deltaY});
    },
    /**
     * Method: resize
     * programmatically resize the dialog in either or both dimensions
     *
     * Parameters: 
     * newSize - {Object} an object containing either or both 'width' and 'height' properties
     * that hold the new width or height in pixels
     */
    resize: function(newSize) {

        /* let jxLayout manage the internal stuff */
        this.innerDialogObj.resize(newSize);
                
        /* sizing logic for container
         *
         * we need to set the size of this.innerDialogObj to the requested size
         * then size the title, content and action area within it and expand
         * this.domObj to contain it properly.  Then we need to position the
         * images appropriately too.
         */
        if (newSize.width) {
            var outerWidth = newSize.width;
            Element.setBorderBoxSize(this.domObj, {width: outerWidth});
            Element.setBorderBoxSize(this.topImg, {width:outerWidth-this.decorationOffsets.top});
            Element.setBorderBoxSize(this.bottomImg, {width:outerWidth-this.decorationOffsets.bottom});
        }
        if (newSize.height) {
            var outerHeight = newSize.height;
            Element.setBorderBoxSize(this.domObj, {height: outerHeight});
            Element.setBorderBoxSize(this.leftImg, {height:outerHeight-this.decorationOffsets.left});
            Element.setBorderBoxSize(this.rightImg, {height:outerHeight-this.decorationOffsets.right});
            if (this.help) {
                Element.setBorderBoxSize(this.help, {height:newSize.height - this.titleHeight});
            }
        }
    },
    /**
     * Method: setTitle
     * set the text of the dialog title.
     *
     * Parameters: 
     * title - {String} the new title
     */
    setTitle: function( title ) {
        this.title.childNodes[0].innerHTML = title;
    },
    /**
     * Method: setButtons
     * create buttons in the action area of the dialog
     *
     * Parameters: 
     * buttons - {Array} an array of strings containing the labels to place on the
     * buttons.  One button is created for each label.
     */
    setButtons: function(buttons) {
        this.action.innerHTML = '';
        for (var i=0; i<buttons.length;i++) {
            var button = document.createElement('input');
            button.id = buttons[i];
            button.type = 'button';
            button.className = 'normalButton';
            button.name = buttons[i];
            button.value = buttons[i];
            button.onclick = this.buttonHandler.bind(this, button);
            this.action.appendChild(button);
            this.uniqueIdRefs[button.id] = button;
        }
    },
    /**
     * Method: processInputs
     * look through the DOM obj for nodes of type INPUT and register
     * for events on them.
     *
     * Parameters: 
     * obj - {HTMLElement} the DOM obj to process
     */
    processInputs : function(obj) {
        for (var i=0;i<obj.childNodes.length; i++) {
            var node = obj.childNodes[i];
            if (node.tagName == 'INPUT' || node.tagName == 'SELECT' || node.tagName == 'TEXTAREA') {
                if (node.type == 'button') {
                    this.actions[node.id] = node;
                    node.onclick = this.buttonHandler.bind(this, node);
                } else {
                    this.values[node.id] = node;
                    if (this.onChange) {
                        node.onchange = this.onChangeHandler.bind(this, node);
                    }
                }
            } else {
                if (node.childNodes) {
                    this.processInputs(node);
                }
            }
        }
    },
    /**
     * Method: buttonHandler
     * handle button events, dispatching to user handler if required.
     *
     * Parameters: 
     * input - {HTMLElement} the input element that triggered a click event
     * event - {Event} the event object
     */
    buttonHandler : function(input, event) {
        if (this.handler) {
            this.handler(input.value, this);
        }
    },
    /**
     * Method: onChangeHandler
     * handle onchange events, dispatching to user handler if required.
     *
     * Parameters: 
     * input - {HTMLElement} the input element that triggered an onchange event
     * event - {Event} the event object
     */
    onChangeHandler: function(input, event) {
        if (this.onChange) {
            this.onChange(input, this);
        }
    },
    /**
     * Method: getValue
     * retrieve the value of an input element in the dialog.  This is done
     * safely so depending on the type of the input.
     *
     * Parameters: 
     * name - {String} the id of the element to get the value of
     *
     * Returns: {String} the value or an empty string.
     */
    getValue : function( name ) {
        var result = '';
        var input = this.values[name];
        if (input) {
            switch (input.tagName) {
                case 'INPUT': 
                    result = input.value;
                    break;
                case 'SELECT':
                    result = input.options[input.selectedIndex].value;
            }
        }
        return result;
    },
    /**
     * Method: setValue
     * set the value of an input element in the dialog.  This is done
     * safely  depending on the type of the input.
     *
     * Parameters: 
     * name - {String} the id of the element to set the value of
     * value - {String} the value to set
     */
    setValue : function( name, value ) {
        if (typeof this.values[name] != 'undefined') {
            if (this.values[name].type == 'text' || this.values[name].type == 'hidden') {
                this.values[name].value = value;
            }
        }
    },
    /**
     * Method: show
     * show the dialog
     */
    show : function( ) {
        this.stack.push(this);
        if (this.modal) {
            this.blanket.style.zIndex = this.zIndex[0]++;
            this.blanket.style.visibility = 'visible';
            this.blanket.style.display = 'block';
        }
        this.domObj.style.zIndex = this.zIndex[0]++;
        Effect.Appear(this.domObj, {duration: 0.1});
        this.domObj.style.display = 'block';
        new Draggable(this.domObj, {handle:this.dragHandle, starteffect: false, endeffect: false});
        this.content.resize({forceResize: this.firstShow});
        this.firstShow = false;
    },
    /**
     * Method: hide
     * hide the dialog
     */
    hide : function() {
        for (var i=0; i<this.stack.length; i++) {
            if (this.stack[i] == this) {
                this.stack.splice(i,1);
            }
        }
        this.zIndex[0]--;
        Effect.Fade(this.domObj, {duration: 0.3});
        //this.domObj.style.display = 'none';
        if (this.modal) {
            this.blanket.style.visibility = 'hidden';
            this.zIndex[0]--;
        }
        
    },
    /**
     * Method: open
     * open the dialog.  This may be delayed depending on the 
     * asynchronous loading of dialog content.  The onOpen
     * callback function is called when the dialog actually
     * opens
     */
    open: function() {
        if (!this.bOpen) {
            this.bOpen = true;
        }
        if (this.bContentLoaded) {
            this.show();
            if (this.onOpen) this.onOpen();
        }
    },
    /**
     * Method: close
     * close the dialog and trigger the onClose callback function
     * if necessary
     */
    close: function() {
        this.bOpen = false;
        this.hide();
        if (this.onClose) this.onClose();
    },
    /**
     * Method: onDialogContentLoaded
     * handle the dialog content being loaded.  This triggers
     * processing of inputs and the onContentLoaded callback
     * function (if necessary).  Also, if the dialog was previously
     * requested to be opened, this will actually open it.
     */
    onDialogContentLoaded : function() {
        this.processInputs(this.content);
        if (this.onContentLoaded) {
            this.onContentLoaded(this);
        }
        if (this.bOpen) {
            //may need to do this?
            //window.setTimeout(this.open.bind(this),1);
            this.open();
            this.bOpen = false;
        }
    },
    /**
     * Method: onHelpContentLoaded
     * handle the help content being loaded.  
     */
    onHelpContentLoaded : function() {
        var img = document.createElement('img');
        img.className = 'jxDialogHelpCloseButton png24';
        img.src = this.imageBaseUrl + 'help_close.png';
        img.onclick = this.toggleHelp.bind(this);
        img.alt = 'Close Help';
        img.title = 'Close Help';
        this.help.appendChild(img);
    },
    /**
     * Method: toggleHelp
     * show or hide the help panel.
     */
    toggleHelp: function() { 
        if (this.help.isVisible) {
            Effect.Fade(this.help, {duration: 0.3});
        } else {
            var actionSize = this.action ? Element.getBorderBoxSize(this.action) : {width: 0, height: 0};
            var contentSize = Element.getBorderBoxSize(this.content);
            Element.setBorderBoxSize(this.help, {height:contentSize.height+actionSize.height});
            Effect.Appear(this.help, {duration: 0.3});
        }
        this.help.isVisible = !this.help.isVisible;
    }
};
Object.extend(Jx.Dialog.prototype, Jx.UniqueId.prototype);
Object.extend(Jx.Dialog.prototype, Jx.ContentLoader.prototype);/**
 * $Id: jxgrid.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx.Grid
 *
 * Purpose: 
 * Implementation of a javascript Grid control for Jx.
 *
 * Jx.Grid is a tabular control with convenient controls for resizing columns,
 * sorting, and inline editing.  It is created inside another element, typically a
 * div.  If the div is resizable (for instance it fills the page or there is a
 * user control allowing it to be resized), you must call the resize() method
 * of the grid to let it know that its container has been resized.
 *
 * When creating a new Jx.Grid, you can specify a number of options for the grid
 * that control its appearance and functionality.
 *
 * Jx.Grid renders data that comes from an external source.  This external 
 * source, called the model, must implement the following interface.
 *
 * *Mandatory Functions*
 *
 * addGridListener(l) - mandatory
 * mandatory.  This function accepts one argument, l, which is the listener
 * to add.  The model can then call the gridChanged() method on the grid
 * listener object when something in the model changes.
 * 
 * removeGridListener(l) - mandatory
 * mandatory.  This function accepts one argument, l, which is the listener
 * to remove.  The listener should have been previously added using
 * addGridListener.
 * 
 * getColumnCount() - mandatory
 * mandatory.  This function returns the number of columns of data in the 
 * model as an integer value.
 * 
 * getColumnHeaderHTML(column) - mandatory
 * mandatory. This function returns an HTML string to be placed in the
 * column header for the given column index.
 * 
 * getColumnHeaderHeight() - mandatory
 * mandatory.  This function returns an integer which is the height of the
 * column header row in pixels.
 * 
 * getColumnWidth(column) - mandatory
 * mandatory.  This function returns an integer which is the width of the
 * given column in pixels.
 * 
 * getRowHeaderHTML(row) - mandatory
 * mandatory.  This function returns an HTML string to be placed in the row
 * header for the given row index
 * 
 * getRowHeaderWidth() - mandatory
 * mandatory.  This function returns an integer which is the width of the row
 * header column in pixels.
 * 
 * getRowHeight(row) - mandatory
 * mandatory.  This function returns an integer which is the height of the
 * given row in pixels.
 * 
 * getRowCount() - mandatory
 * mandatory.  This function returns the number of rows of data in the model
 * as an integer value.
 * 
 * getValueAt(row, column) - mandatory
 * mandatory.  This function returns an HTML string which is the text to place
 * in the cell at the given row and column.
 * 
 * isCellEditable(row, column) - mandatory
 * mandatory.  This function returns a boolean value to indicate if a given
 * cell is editable by the user.
 *
 * *Optional Functions*
 *  
 * setColumnWidth(column, width) - optional
 * optional.  This function is called with a column index and width in pixels
 * when a column is resized.  This function is only required if the grid
 * allows resizeable columns.
 * 
 * setValueAt(row, column, value) - optional
 * optional.  This function is called with the row and column of a cell and a
 * new value for the cell.  It is mandatory to provide this function if any of
 * the cells in the model are editable.
 * 
 * rowSelected(row) - optional
 * optional.  This function is called by the grid to indicate that the user
 * has selected a row by clicking on the row header.
 * 
 * columnSelected(column) - optional
 * optional.  This function is called by the grid to indicate that the user
 * has selected a column by clicking on the column header.
 * 
 * cellSelected(row, column) - optional
 * optional.  This function is called by the grid to indicate that the user
 * has selected a cell by clicking on the cell in the grid.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

/* our default css styles */
Jx.addStyleSheet('grid/grid.css');

/**
 * Class: Jx.Grid
 * A tabular control that has fixed scrolling headers on the rows and columns
 * like a spreadsheet.
 */
Jx.Grid = Class.create();
Jx.Grid.prototype = {
    domObj : null,
    model : null,
    /**
     * Constructor: Jx.Grid
     * construct a new instance of Jx.Grid within the domObj
     *
     * Parameters:
     * domObj - {HTMLElement} the HTML element to create the grid inside.
     *          The grid will resize to fill the domObj.
     * options - you can specify some options as attributes of a
     * generic object.
     *
     * Options:
     * alternateRowColors - defaults to false.  If set to true, then
     *      alternating CSS classes are used for rows
     * rowHeaders - defaults to false.  If set to true, then a column
     *      of row header cells are displayed.
     * columnHeaders - defaults to false.  If set to true, then a column
     *      of row header cells are displayed.
     * rowSelection - defaults to false.  If set to true, allow the
     *      user to select rows.
     * cellSelection - defaults to false.  If set to true, allow the
     *      user to select cells.
     */
    initialize : function( domObj, options ) {
        this.domObj = $(domObj);
        
        /* if this grid is the content of a Jx Panel, this will
           allow the grid to resize when the panel resizes
         */
        if (!this.domObj.jxLayout) {
            new Jx.Layout(this.domObj);
        }
        this.domObj.jxLayout.addSizeChangeListener(this);
        
        options = options || {};
        
        this.rowColObj = document.createElement('div');
        this.rowColObj.className = 'jxGridContainer';
        
        this.colObj = document.createElement('div');
        this.colObj.className = 'jxGridContainer';
        this.colTable = document.createElement('table');
        this.colTable.className = 'jxGridTable';
        this.colTableHead = document.createElement('thead');
        this.colTable.appendChild(this.colTableHead);
        this.colTableBody = document.createElement('tbody');
        this.colTable.appendChild(this.colTableBody);
        this.colObj.appendChild(this.colTable);
        
        this.rowObj = document.createElement('div');
        this.rowObj.className = 'jxGridContainer';
        this.rowTable = document.createElement('table');
        this.rowTable.className = 'jxGridTable';
        this.rowTableHead = document.createElement('thead');
        this.rowTable.appendChild(this.rowTableHead);
        this.rowObj.appendChild(this.rowTable);
        
        this.gridObj = document.createElement('div');
        this.gridObj.className = 'jxGridContainer';
        this.gridObj.style.overflow = 'scroll';
        this.gridTable = document.createElement('table');
        this.gridTable.className = 'jxGridTable';
        this.gridTableBody = document.createElement('tbody');
        this.gridTable.appendChild(this.gridTableBody);
        this.gridObj.appendChild(this.gridTable);
        
        this.domObj.appendChild(this.rowColObj);
        this.domObj.appendChild(this.rowObj);
        this.domObj.appendChild(this.colObj);
        this.domObj.appendChild(this.gridObj);
        
        this.bAlternateRowColors = options.alternateRowColors || false;
        this.showRowHeader = options.rowHeaders || false;
        this.showColumnHeader = options.columnHeaders || false;
        this.rowSelection = options.rowSelection || false;
        this.cellSelection = options.cellSelection || false;
                
        Event.observe(this.gridObj, 'scroll', this.onScroll.bind(this));
        Event.observe(this.gridObj, 'click', this.onClickGrid.bindAsEventListener(this));
        Event.observe(this.rowObj, 'click', this.onClickRowHeader.bindAsEventListener(this));
        Event.observe(this.colObj, 'click', this.onClickColumnHeader.bindAsEventListener(this));
        Event.observe(this.gridObj, 'mousemove', this.onMouseMoveGrid.bindAsEventListener(this));
        Event.observe(this.rowObj, 'mousemove', this.onMouseMoveRowHeader.bindAsEventListener(this));
        Event.observe(this.colObj, 'mousemove', this.onMouseMoveColumnHeader.bindAsEventListener(this));
    },
    
    /**
     * Method: onScroll
     * handle the grid scrolling by updating the position of the headers
     */
    onScroll: function() {
        this.colObj.scrollLeft = this.gridObj.scrollLeft;
        this.rowObj.scrollTop = this.gridObj.scrollTop;        
    },
    
    /**
     * Method: sizeChanged
     * Handle the container of the grid resizing
     */
    sizeChanged: function() {
        this.resize();
    },
    
    /**
     * Method: resize
     * resize the grid to fit inside its container.  This involves knowing something
     * about the model it is displaying (the height of the column header and the
     * width of the row header) so nothing happens if no model is set
     */
    resize: function() {
        if (!this.model) {
            return;
        }
        
        /* TODO: Jx.Grid.resize
         * if not showing column or row, should we handle the resize differently
         */
        var colHeight = this.showColumnHeader ? this.model.getColumnHeaderHeight() : 1;
        var rowWidth = this.showRowHeader ? this.model.getRowHeaderWidth() : 1;
        
        var size = Element.getContentBoxSize(this.domObj);
        
        /* -1 because of the right/bottom borders */
        this.rowColObj.style.width = (rowWidth - 1) + 'px';
        this.rowColObj.style.height = (colHeight - 1) + 'px';
        
        this.rowObj.style.top = (colHeight) + 'px';
        this.rowObj.style.left = '0px';
        this.rowObj.style.width = (rowWidth - 1) + 'px';
        this.rowObj.style.height = (size.height - colHeight - 1) + 'px';

        this.colObj.style.top = '0px';
        this.colObj.style.left = (rowWidth) + 'px';
        this.colObj.style.width = (size.width - rowWidth - 1) + 'px';
        this.colObj.style.height = (colHeight - 1) + 'px';

        this.gridObj.style.top = (colHeight) + 'px';
        this.gridObj.style.left = (rowWidth) + 'px';
        this.gridObj.style.width = (size.width - rowWidth - 1) + 'px';
        this.gridObj.style.height = (size.height - colHeight - 1) + 'px';
    },
    
    /**
     * Method: setModel
     * set the model for the grid to display.  If a model is attached to the grid
     * it is removed and the new model is displayed.
     * 
     * Parameters:
     * model - {Object} the model to use for this grid
     */
    setModel: function(model) {
        if (this.model) {
            this.model.removeGridListener(this);
        }
        this.model = model;
        if (this.model) {
            this.domObj.jxLayout.resize();
            this.model.addGridListener(this);
            this.createGrid();
            this.resize();
        } else {
            this.destroyGrid();
        }
    },
    
    /**
     * Method: destroyGrid
     * destroy the contents of the grid safely
     */
    destroyGrid: function() {
        var n = this.colTableHead.cloneNode(false);
        this.colTable.replaceChild(n, this.colTableHead);
        this.colTableHead = n;
        
        n = this.colTableBody.cloneNode(false);
        this.colTable.replaceChild(n, this.colTableBody);
        this.colTableBody = n;
        
        n = this.rowTableHead.cloneNode(false);
        this.rowTable.replaceChild(n, this.rowTableHead);
        this.rowTableHead = n;
        
        n = this.gridTableBody.cloneNode(false);
        this.gridTable.replaceChild(n, this.gridTableBody);
        this.gridTableBody = n;
        
    },
    
    /**
     * Method: createGrid
     * create the grid for the current model
     */
    createGrid: function() {
        this.destroyGrid();
        if (this.model) {
            var model = this.model;
            var nColumns = model.getColumnCount();
            var nRows = model.getRowCount();
            
            /* create header if necessary */
            if (this.showColumnHeader) {
                var colHeight = model.getColumnHeaderHeight();
                var trHead = document.createElement('tr');
                this.colTableHead.appendChild(trHead);
                var trBody = document.createElement('tr');
                this.colTableBody.appendChild(trBody);
                
                var th = document.createElement('th');
                th.style.width = '0px';
                th.style.height = '0px';
                trHead.appendChild(th);
                th = th.cloneNode(true);
                th.style.height = (colHeight) + 'px';
                trBody.appendChild(th);
                for (var i=0; i<nColumns; i++) {
                    var colWidth = model.getColumnWidth(i);
                    th = document.createElement('th');
                    th.className = 'jxGridColHeadHide';
                    th.style.width = (colWidth) + 'px';
                    var p = document.createElement('p');
                    p.style.height = '0px';
                    p.style.width = (colWidth) + 'px';
                    th.appendChild(p);
                    trHead.appendChild(th);
                    
                    th = document.createElement('th');
                    th.className = 'jxGridColHead';
                    th.innerHTML = model.getColumnHeaderHTML(i);
                    trBody.appendChild(th);
                }
                /* one extra column at the end for filler */
                var th = document.createElement('th');
                th.style.width = '1000px';
                th.style.height = '0px';
                trHead.appendChild(th);
                th = th.cloneNode(true);
                th.style.height = (colHeight - 1) + 'px';
                th.className = 'jxGridColHead';
                trBody.appendChild(th);
                
            }
            
            if (this.showRowHeader) {
                var rowWidth = model.getRowHeaderWidth();
                var tr = document.createElement('tr');
                var td = document.createElement('td');
                td.style.width = '0px';
                td.style.height = '0px';
                tr.appendChild(td);
                var th = document.createElement('th');
                th.style.width = (rowWidth) + 'px';
                th.style.height = '0px';
                tr.appendChild(th);
                this.rowTableHead.appendChild(tr);
                for (var i=0; i<nRows; i++) {
                    var rowHeight = model.getRowHeight(i);
                    var tr = document.createElement('tr');
                    var td = document.createElement('td');
                    td.className = 'jxGridRowHeadHide';
                    td.style.width = '0px';
                    td.style.height = (rowHeight)+'px';
                    var p = document.createElement('p');
                    p.style.width = '0px';
                    p.style.height = (rowHeight)+'px';
                    td.appendChild(p);
                    tr.appendChild(td);
                    var th = document.createElement('th');
                    th.className = 'jxGridRowHead';
                    th.innerHTML = model.getRowHeaderHTML(i);
                    tr.appendChild(th);
                    this.rowTableHead.appendChild(tr);
                }
                /* one extra row at the end for filler */
                var tr = document.createElement('tr');
                var td = document.createElement('td');
                td.style.width = '0px';
                td.style.height = '1000px';
                tr.appendChild(td);
                var th = document.createElement('th');
                th.style.width = (rowWidth) + 'px';
                th.style.height = '1000px';
                th.className = 'jxGridRowHead';
                tr.appendChild(th);
                this.rowTableHead.appendChild(tr);
            }
            
            var colHeight = model.getColumnHeaderHeight();
            var trBody = document.createElement('tr');
            this.gridTableBody.appendChild(trBody);
            
            var td = document.createElement('td');
            td.style.width = '0px';
            td.style.height = '0px';
            trBody.appendChild(td);
            for (var i=0; i<nColumns; i++) {
                var colWidth = model.getColumnWidth(i);
                td = document.createElement('td');
                td.className = 'jxGridColHeadHide';
                td.style.width = (colWidth) + 'px';
                var p = document.createElement('p');
                p.style.height = '0px';
                p.style.width = (colWidth) + 'px';
                td.appendChild(p);
                trBody.appendChild(td);
            }
            
            for (var j=0; j<nRows; j++) {
                var rowHeight = model.getRowHeight(j);
                var actualRowHeight = rowHeight;
                var tr = document.createElement('tr');
                this.gridTableBody.appendChild(tr);
                
                var td = document.createElement('td');
                td.className = 'jxGridRowHeadHide';
                td.style.width = '0px';
                td.style.height = (rowHeight) + 'px';
                var p = document.createElement('p');
                p.style.height = (rowHeight) + 'px';
                td.appendChild(p);
                tr.appendChild(td);
                for (var i=0; i<nColumns; i++) {
                    var colWidth = model.getColumnWidth(i);
                    td = document.createElement('td');
                    td.className = 'jxGridCell';
                    td.innerHTML = model.getValueAt(j,i);
                    tr.appendChild(td);
                    var tdSize = Element.getDimensions(td);
                    if (tdSize.height > actualRowHeight) {
                        actualRowHeight = tdSize.height;
                    }
                }
                /* some notes about row sizing
                 * In Safari, the height of a TR is always returned as 0
                 * In Safari, the height of any given TD is the height it would
                 * render at, not the actual height of the row
                 * In IE, the height is returned 1px bigger than any other browser
                 * Firefox just works
                 *
                 * So, for Safari, we have to measure every TD and take the highest one
                 * and if its IE, we subtract 1 from the overall height, making all
                 * browsers identical
                 *
                 * Using document.all is not a good hack for this
                 */
                if (document.all) {
                    actualRowHeight -= 1;
                }
                if (this.showRowHeader) {
                    this.setRowHeaderHeight(j, actualRowHeight);                    
                }
                /* if we apply the class before adding content, it
                 * causes a rendering error in IE (off by 1) that is 'fixed'
                 * when another class is applied to the row, causing dynamic
                 * shifting of the row heights
                 */
                if (this.bAlternateRowColors) {
                    tr.className = (j%2) ? 'jxGridRowOdd' : 'jxGridRowEven';
                } else {
                    tr.className = 'jxGridRowAll';
                }
            }
            
        }
    },
    
    /**
     * Method: setRowHeaderHeight
     * set the height of a row.  This is used internally to adjust the height of
     * the row header when cell contents wrap.  A limitation of the table structure
     * is that overflow: hidden on a td will work horizontally but not vertically
     *
     * Parameters:
     * row - {Integer} the row to set the height for
     * height - {Integer} the height to set the row (in pixels)
     */
    setRowHeaderHeight: function(row, height) {
        //this.rowTableHead.childNodes[row+1].childNodes[0].style.height = (height) + 'px';
        this.rowTableHead.childNodes[row+1].childNodes[0].childNodes[0].style.height = (height) + 'px';
    },
    
    /**
     * Method: gridChanged
     * called through the grid listener interface when data has changed in the
     * underlying model
     *
     * Parameters:
     * model - {Object} the model that changed
     * row - {Integer} the row that changed
     * col - {Integer} the column that changed
     * value - {Mixed} the new value
     */
    gridChanged: function(model, row, col, value) {
        if (this.model == model) {
            this.gridObj.childNodes[row].childNodes[col].innerHTML = value;
        }
    },
    
    /** 
     * Method: prelightRowHeader
     * apply the jxGridRowHeaderPrelight style to the header cell of a row.
     * This removes the style from the previously pre-lit row header.
     * 
     * Parameters:
     * row - {Integer} the row to pre-light the header cell of
     */
    prelightRowHeader: function(row) {
        var cell = (row >= 0 && row < this.rowTableHead.rows.length-1) ? this.rowTableHead.rows[row+1].cells[1] : null;
        if (this.prelitRowHeader != cell) {
            if (this.prelitRowHeader) {
                Element.removeClassName(this.prelitRowHeader, 'jxGridRowHeaderPrelight');
            }
            this.prelitRowHeader = cell;
            if (this.prelitRowHeader) {
                Element.addClassName(this.prelitRowHeader, 'jxGridRowHeaderPrelight');
            }
        }
    },
    
    /** 
     * Method: prelightColumnHeader
     * apply the jxGridColumnHeaderPrelight style to the header cell of a column.
     * This removes the style from the previously pre-lit column header.
     * 
     * Parameters:
     * col - {Integer} the column to pre-light the header cell of
     */
    prelightColumnHeader: function(col) {
        if (this.colTableBody.rows.length == 0) {
            return;
        }
        var cell = (col >= 0 && col < this.colTableBody.rows[0].cells.length-1) ? this.colTableBody.rows[0].cells[col+1] : null;
        if (this.prelitColumnHeader != cell) {
            if (this.prelitColumnHeader) {
                Element.removeClassName(this.prelitColumnHeader, 'jxGridColumnHeaderPrelight');
            }
            this.prelitColumnHeader = cell;
            if (this.prelitColumnHeader) {
                Element.addClassName(this.prelitColumnHeader, 'jxGridColumnHeaderPrelight');
            }
        }
    },
    
    /** 
     * Method: prelightRow
     * apply the jxGridRowPrelight style to row.
     * This removes the style from the previously pre-lit row.
     * 
     * Parameters:
     * row - {Integer} the row to pre-light
     */
    prelightRow: function(row) {
        var tr = (row >= 0 && row < this.gridTableBody.rows.length-1) ? this.gridTableBody.rows[row+1] : null;
        
        if (this.prelitRow != row) {
            if (this.prelitRow) {
                Element.removeClassName(this.prelitRow, 'jxGridRowPrelight');
            }
            this.prelitRow = tr;
            if (this.prelitRow && !Element.hasClassName(this.prelitRow, 'jxGridRowSelected')) {
                this.prelightRowHeader(row);
                Element.addClassName(this.prelitRow, 'jxGridRowPrelight');
            }
        }
    },
    
    /** 
     * Method: prelightColumn
     * apply the jxGridColumnPrelight style to a column.
     * This removes the style from the previously pre-lit column.
     * 
     * Parameters:
     * col - {Integer} the column to pre-light
     *
     * TODO: Jx.Grid.prelightColumn
     * Not Yet Implemented.
     */
    prelightColumn: function(col) {
        /* TODO: Jx.Grid.prelightColumn
         * implement column prelighting (possibly) 
         */
        this.prelightColumnHeader(col);
    },
    
    /** 
     * Method: prelightCell
     * apply the jxGridCellPrelight style to a cell.
     * This removes the style from the previously pre-lit cell.
     *
     * Parameters:
     * row - {Integer} the row of the cell to pre-light
     * col - {Integer} the column of the cell to pre-light
     */
    prelightCell: function(row, col) {
         var td = (row >=0 && col >=0 && row < this.gridTableBody.rows.length - 1 && col < this.gridTableBody.rows[row+1].cells.length - 1) ? this.gridTableBody.rows[row+1].cells[col+1] : null;
        if (this.prelitCell != td) {
            if (this.prelitCell) {
                Element.removeClassName(this.prelitCell, 'jxGridCellPrelight');
            }
            this.prelitCell = td;
            if (this.prelitCell) {
                Element.addClassName(this.prelitCell, 'jxGridCellPrelight');
                this.prelightRow(row);
                this.prelightColumn(col);
            }
        }    
    },
    
    /** 
     * Method: selectCell
     * Select a cell and apply the jxGridCellSelected style to it.
     * This deselects a previously selected cell.
     *
     * If the model supports cell selection, it should implement
     * a cellSelected function to receive notification of the selection.
     *
     * Parameters:
     * row - {Integer} the row of the cell to select
     * col - {Integer} the column of the cell to select
     */
    selectCell: function(row, col) {
         var td = (row >=0 && col >=0 && row < this.gridTableBody.rows.length - 1 && col < this.gridTableBody.rows[row+1].cells.length - 1) ? this.gridTableBody.rows[row+1].cells[col+1] : null;
         if (!td) {
             return;
         }
         
         if (this.selectedCell) {
             Element.addClassName(this.selectedCell, 'jxGridCellSelected');
         } else {
             Element.removeClassName(this.selectedCell, 'jxGridCellSelected');
         }
    },
    
    /** 
     * Method: selectRowHeader
     * Apply the jxGridRowHeaderSelected style to the row header cell of a
     * selected row.
     *
     * Parameters:
     * row - {Integer} the row header to select
     * selected - {Boolean} the new state of the row header
     */
    selectRowHeader: function(row, selected) {
        var cell = (row >= 0 && row < this.rowTableHead.rows.length-1) ? this.rowTableHead.rows[row+1].cells[1] : null;
        if (!cell) {
            return;
        }
        if (selected) {
            Element.addClassName(cell, 'jxGridRowHeaderSelected');
        } else {
            Element.removeClassName(cell, 'jxGridRowHeaderSelected');
        }
    },
    
    /** 
     * Method: selectRow
     * Select a row and apply the jxGridRowSelected style to it.
     *
     * If the model supports row selection, it should implement
     * a rowSelected function to receive notification of the selection.
     *
     * Parameters:
     * row - {Integer} the row to select
     * selected - {Boolean} the new state of the row
     */
    selectRow: function(row, selected) {
        var tr = (row >= 0 && row < this.gridTableBody.rows.length - 1) ? this.gridTableBody.rows[row+1] : null;
        if (tr) {
            if (selected) {
                Element.addClassName(tr, 'jxGridRowSelected');
            } else {
                Element.removeClassName(tr, 'jxGridRowSelected');
            }
        }
        this.selectRowHeader(row, selected);
    },
    
    /** 
     * method: selectColumnHeader
     * Apply the jxGridColumnHeaderSelected style to the column header cell of a
     * selected column.
     *
     * Parameters:
     * col - {Integer} the column header to select
     * selected - {Boolean} the new state of the column header
     */
    selectColumnHeader: function(col, selected) {
        if (this.colTableBody.rows.length == 0) {
            return;
        }
        var cell = (col >= 0 && col < this.colTableBody.rows[0].cells.length-1) ? this.colTableBody.rows[0].cells[col+1] : null;
        if (cell == null) { 
            return; 
        }
        
        if (selected) {
            Element.addClassName(cell, 'jxGridColumnHeaderSelected');
        } else {
            Element.removeClassName(cell, 'jxGridColumnHeaderSelected');
        }
    },
    
    /** 
     * Method: selectColumn
     * Select a column.
     * This deselects a previously selected column.
     *
     * Parameters:
     * col - {Integer} the column to select
     * selected - {Boolean} the new state of the column
     */
    selectColumn: function(col, selected) {
        /* todo: implement column selection */
        if (col >= 0 && col < this.gridTable.rows[0].cells.length) {
            if (selected) {
                for (var i=0; i<this.gridTable.rows.length; i++) {
                    Element.removeClassName(this.gridTable.rows[i].cells[this.selectedColumn + 1], 'jxGridColumnSelected');
                }
            } else {
                for (var i=0; i<this.gridTable.rows.length; i++) {
                    Element.addClassName(this.gridTable.rows[i].cells[col+1], 'jxGridColumnSelected');
                }
                
            }
        }
        this.selectColumnHeader(col, selected);
    },
    
    /**
     * Method: onMouseMoveGrid
     * handle the mouse moving over the main grid.  This pre-lights the cell,
     * and subsquently the row and column (and headers).
     *
     * Parameters:
     * e - {Event} the browser event object
     */
    onMouseMoveGrid: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        this.prelightCell(rc.row, rc.column);
    },
    
    /**
     * Method: onMouseMoveRowHeader
     * handle the mouse moving over the row header cells.  This pre-lights
     * the row and subsequently the row header.
     *
     * Parameters:
     * e - {Event} the browser event object
     */
    onMouseMoveRowHeader: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        this.prelightRow(rc.row);
    },

    /**
     * Method: onMouseMoveColumnHeader
     * handle the mouse moving over the column header cells.  This pre-lights
     * the column and subsequently the column header.
     *
     * Parameters:
     * e - {Event} the browser event object
     */
    onMouseMoveColumnHeader: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        this.prelightColumn(rc.column);
    },
    
    /**
     * Method: onClickGrid
     * handle the user clicking on the grid.  This triggers an
     * event to the model (if a cellSelected function is provided).
     *
     * The following is an example of a function in the model that selects
     * a row when the cellSelected function is called and deselects any rows
     * that are currently selected.
     *
     * (code)
     * cellSelected: function(grid, row,col) { 
     *    if (this.selectedRow != null) {
     *        grid.selectRow(this.selectedRow, false);
     *    }
     *    this.selectedRow = row;
     *    grid.selectRow(row, true);
     * }
     *
     * Parameters:
     * e - {Event} the browser event object
     */
    onClickGrid: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        //this.selectCell(rc.row, rc.column);
        
        if (this.model.cellSelected) {
            this.model.cellSelected(this, rc.row, rc.column);
        }
    },
    
    /**
     * Method: onClickRowHeader
     * handle the user clicking on the row header.  This triggers an
     * event to the model (if a rowSelected function is provided) which
     * can then select the row if desired.  
     *
     * The following is an example of a function in the model that selects
     * a row when the rowSelected function is called and deselects any rows
     * that are currently selected.  More complex code could be written to 
     * allow the user to select multiple rows.
     *
     * (code)
     * rowSelected: function(grid, row) {
     *    if (this.selectedRow != null) {
     *        grid.selectRow(this.selectedRow, false);
     *    }
     *    this.selectedRow = row;
     *    grid.selectRow(row, true);
     * }
     * (end)
     *
     * Parameters:
     * e - {Event} the browser event object
     */
    onClickRowHeader: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        
        if (this.model.rowSelected) {
            this.model.rowSelected(this, rc.row);
        }
    },
    
    /**
     * Method: onClickColumnHeader
     * handle the user clicking on the column header.  This triggers column
     * selection and column (and header) styling changes and an
     * event to the model (if a columnSelected function is provided)
     *
     * The following is an example of a function in the model that selects
     * a column when the columnSelected function is called and deselects any 
     * columns that are currently selected.  More complex code could be written
     * to allow the user to select multiple columns.
     *
     * (code)
     * colSelected: function(grid, col) {
     *    if (this.selectedColumn != null) {
     *        grid.selectColumn(this.selectedColumn, false);
     *    }
     *    this.selectedColumn = col;
     *    grid.selectColumn(col, true);
     * }
     * (end)
     *
     * Parameters:
     * e - {Event} the browser event object
     */
    onClickColumnHeader: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        
        if (this.model.columnSelected) {
            this.model.columnSelected(this, rc.column);
        }
    },
    
    /**
     * method: getRowColumnFromEvent
     * retrieve the row and column indexes from an event click.
     * This function is used by the grid, row header and column
     * header to safely get these numbers.
     *
     * If the event isn't valid (i.e. it wasn't on a TD or TH) then
     * the returned values will be -1, -1
     *
     * Parameters:
     * e - {Event} the browser event object
     *
     * @return Object an object with two properties, row and column,
     *         that contain the row and column that was clicked
     */
    getRowColumnFromEvent: function(e) {
        var td = Event.element(e);
        if (td.tagName != 'TD' && td.tagName != 'TH') {
            return {row:-1,column:-1};
        }
        var tr = td.parentNode;
        var col = td.cellIndex - 1; /* because of hidden spacer column */
        var row = tr.rowIndex - 1; /* because of hidden spacer row */
        
        if (col == -1) { 
            /* bug in safari returns 0 for cellIndex - only choice seems
             * to be to loop through the row
             */
            for (var i=0; i<tr.childNodes.length; i++) {
                if (tr.childNodes[i] == td) {
                    col = i - 1;
                    break;
                }
            }
        }
        return {row:row,column:col};
    }
};/**
 * $Id: jxlayout.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx.Layout
 *
 * Purpose: 
 * Implementation of a javascript layout engine for Jx.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

/**
 * Class: Jx.Layout
 *
 * Jx.Layout is used to provide more flexible layout options for applications
 *
 * Jx.Layout wraps an existing DOM element (typically a div) and provides
 * extra functionality for sizing that element within its parent and sizing
 * elements contained within it that have a 'resize' function attached to them.
 *
 * To create a Jx.Layout, pass the element or id plus an options object to
 * the constructor:
 *
 * (code)
 * var myContainer = new Jx.Layout('myDiv', options);
 * (end)
 *
 * Inherits From:
 * <Jx.Listener>
 */
 
Jx.Layout = Class.create();
Jx.Layout.prototype = {
    scl: null,
    /**
     * Constructor: Jx.Layout
     * Create a new instance of Jx.Layout.
     *
     * Parameters:
     * domObj - {HTMLElement} element or id to apply the layout to
     * options - {Object} options can be passed to the Jx.Layout as an object
     * with some, all, or none of the options below.
     *
     * Options:
     * position - how to position the element, either 'absolute' or 'relative'.
     *    The default (if not passed) is 'absolute'.  When using
     *    'absolute' positioning, both the width and height are
     *    controlled by Jx.Layout.  If 'relative' positioning is used
     *    then only the width is controlled, allowing the height to
     *    be controlled by its content.
     * left - the distance (in pixels) to maintain the left edge of the element
     *    from its parent element.  The default value is 0.  If this is set
     *    to 'null', then the left edge can be any distance from its parent
     *    based on other parameters.
     * right - the distance (in pixels) to maintain the right edge of the element
     *    from its parent element.  The default value is 0.  If this is set
     *    to 'null', then the right edge can be any distance from its parent
     *    based on other parameters.
     * top - the distance (in pixels) to maintain the top edge of the element
     *    from its parent element.  The default value is 0.  If this is set
     *    to 'null', then the top edge can be any distance from its parent
     *    based on other parameters.
     * bottom - the distance (in pixels) to maintain the bottom edge of the element
     *    from its parent element.  The default value is 0.  If this is set
     *    to 'null', then the bottom edge can be any distance from its parent
     *    based on other parameters.
     * width - the width (in pixels) of the element.  The default value is null.
     *    If this is set to 'null', then the width can be any value based on
     *    other parameters.
     * height - the height (in pixels) of the element.  The default value is null.
     *    If this is set to 'null', then the height can be any value based on
     *    other parameters.
     * minWidth - the minimum width that the element can be sized to.  The default
     *    value is 0.
     * minHeight - the minimum height that the element can be sized to.  The
     *    default value is 0.
     * maxWidth - the maximum width that the element can be sized to.  The default
     *    value is -1, which means no maximum.
     * maxHeight - the maximum height that the element can be sized to.  The
     *    default value is -1, which means no maximum.
     */
    initialize: function(domObj, options) {
        options = options || {};
        this.options = new Jx.Constraint(options);
        this.domObj = $(domObj);
        this.domObj.resize = this.resize.bind(this);
        this.domObj.style.position = this.options.position;
        this.domObj.jxLayout = this;

        if (this.domObj.parentNode && this.domObj.parentNode.tagName == 'BODY') {
            Event.observe(window, 'resize', this.windowResize.bind(this));
        }
        
        this.scl = [];
    },
    
    /**
     * Method: windowResize
     * when the window is resized, any Jx.Layout controlled elements that are
     * direct children of the BODY element are resized
     */
     windowResize: function() {
        if (this.resizeTimer) {
            window.clearTimeout(this.resizeTimer);
            this.resizeTimer = null;
        }
        this.resizeTimer = window.setTimeout(this.resize.bind(this), 250);
    },
    
    /**
     * Method: resize
     * resize the element controled by this Jx.Layout object.
     *
     * Parameters:
     * options - new options to apply, see <Jx.Layout::Jx.Layout>
     */
     resize: function(options) {
        this.resizeTimer = null;
        var needsResize = false;
        if (options) {
            for (var i in options) {
                if (this.options[i] != options[i]) {
                    needsResize = true;
                    this.options[i] = options[i];
                }
            }
            if (options.forceResize) {
                needsResize = true;
            }
        }
        
        
        var parentSize;
        if (this.domObj.parentNode.tagName == 'BODY') {
            parentSize = Element.getPageDimensions();
        } else {
            parentSize = Element.getContentBoxSize(this.domObj.parentNode);
        }
        
        if (this.lastParentSize && !needsResize) {
            needsResize = (this.lastParentSize.width != parentSize.width || 
                          this.lastParentSize.height != parentSize.height);
        } else {
            needsResize = true;
        }
        this.lastParentSize = parentSize;
        
        if (!needsResize) {
            return;
        }
        
        var l, t, w, h;
        
        /* calculate left and width */
        if (this.options.left != null) {
            /* fixed left */
            l = this.options.left;
            if (this.options.right == null) {
                /* variable right */
                if (this.options.width == null) {
                    //console.log( 'fixed left, variable right, variable width' );
                    /* variable right and width
                     * set right to min, stretch width */
                    w = parentSize.width - l;
                    if (w < this.options.minWidth ) {
                        w = this.options.minWidth;
                    }
                    if (this.options.maxWidth >= 0 && w > this.options.maxWidth) {
                        w = this.options.maxWidth;
                    }
                } else {
                    //console.log( 'fixed left, variable right, fixed width' );
                    /* variable right, fixed width
                     * use width
                     */
                    w = this.options.width;
                }
            } else {
                /* fixed right */
                if (this.options.width == null) {
                    //console.log( 'fixed left, fixed right, variable width' );
                    
                    /* fixed right, variable width
                     * stretch width
                     */
                    w = parentSize.width - l - this.options.right;
                    if (w < this.options.minWidth) {
                        w = this.options.minWidth;
                    }
                    if (this.options.maxWidth >= 0 && w > this.options.maxWidth) {
                        w = this.options.maxWidth;
                    }
                } else {
                    //console.log( 'fixed left, fixed right, fixed width' );
                    
                    /* fixed right, fixed width
                     * respect left and width, allow right to stretch
                     */
                    w = this.options.width;
                }
            }
            
        } else {
            if (this.options.right == null) {
                if (this.options.width == null) {
                    //console.log( 'variable left, variable right, variable width' );
                    
                    /* variable left, width and right
                     * set left, right to min, stretch width
                     */
                     l = 0;
                     w = parentSize.width;
                     if (this.options.maxWidth >= 0 && w > this.options.maxWidth) {
                         l = l + parseInt(w - this.options.maxWidth)/2;
                         w = this.options.maxWidth;
                     }
                } else {
                    //console.log( 'variable left, variable right, fixed width' );
                    
                    /* variable left, fixed width, variable right
                     * distribute space between left and right
                     */
                    w = this.options.width;
                    l = parseInt((parentSize.width - w)/2);
                    if (l < 0) {
                        l = 0;
                    }
                }
            } else {
                if (this.options.width != null) {
                    //console.log( 'variable left, fixed right, fixed width' );
                    
                    /* variable left, fixed width, fixed right
                     * left is calculated directly
                     */
                    w = this.options.width;
                    l = parentSize.width - w - this.options.right;
                    if (l < 0) {
                        l = 0;
                    }
                } else {
                    //console.log( 'variable left, fixed right, variable width' );
                    
                    /* variable left and width, fixed right
                     * set left to min value and stretch width
                     */
                    l = 0;
                    w = parentSize.width - this.options.right;
                    if (w < this.options.minWidth) {
                        w = this.options.minWidth;
                    }
                    if (this.options.maxWidth >= 0 && w > this.options.maxWidth) {
                        l = w - this.options.maxWidth - this.options.right;
                        w = this.options.maxWidth;                        
                    }
                }
            }
        }
        
        /* calculate the top and height */
        if (this.options.top != null) {
            /* fixed top */
            t = this.options.top;
            if (this.options.bottom == null) {
                /* variable bottom */
                if (this.options.height == null) {
                    //console.log( 'fixed top, variable bottom, variable height' );
                    
                    /* variable bottom and height
                     * set bottom to min, stretch height */
                    h = parentSize.height - t;
                    if (h < this.options.minHeight) {
                        h = this.options.minHeight;
                    }
                    if (this.options.maxHeight >= 0 && h > this.options.maxHeight) {
                        h = this.options.maxHeight;
                    }
                } else {
                    //console.log( 'fixed top, variable bottom, fixed height' );
                    
                    /* variable bottom, fixed height
                     * stretch height
                     */
                    h = this.options.height;
                    if (this.options.maxHeight >= 0 && h > this.options.maxHeight) {
                        t = h - this.options.maxHeight;
                        h = this.options.maxHeight;
                    }
                }
            } else {
                /* fixed bottom */
                if (this.options.height == null) {
                    //console.log( 'fixed top, fixed bottom, variable height' );
                    
                    /* fixed bottom, variable height
                     * stretch height
                     */
                    h = parentSize.height - t - this.options.bottom;
                    if (h < this.options.minHeight) {
                        h = this.options.minHeight;
                    }
                    if (this.options.maxHeight >= 0 && h > this.options.maxHeight) {
                        h = this.options.maxHeight;
                    }                
                } else {
                    //console.log( 'fixed top, fixed bottom, fixed height' );
                    
                    /* fixed bottom, fixed height
                     * respect top and height, allow bottom to stretch
                     */
                    h = this.options.height;
                }
            }
            
        } else {
            if (this.options.bottom == null) {
                if (this.options.height == null) {
                    //console.log( 'variable top, variable bottom, variable height' );
                    
                    /* variable top, height and bottom
                     * set top, bottom to min, stretch height
                     */
                     t = 0;
                     h = parentSize.height;
                     if (h < this.options.minHeight) {
                         h = this.options.minHeight;
                     }
                     if (this.options.maxHeight >= 0 && h > this.options.maxHeight) {
                         t = parseInt((parentSize.height - this.options.maxHeight)/2);
                         h = this.options.maxHeight;
                     }
                } else {
                    //console.log( 'variable top, variable bottom, fixed height' );
                    
                    /* variable top, fixed height, variable bottom
                     * distribute space between top and bottom
                     */
                    h = this.options.height;
                    t = parseInt((parentSize.height - h)/2);
                    if (t < 0) {
                        t = 0;
                    }
                }
            } else {
                if (this.options.height != null) {
                    //console.log( 'variable top, fixed bottom, fixed height' );
                    
                    /* variable top, fixed height, fixed bottom
                     * top is calculated directly
                     */
                    h = this.options.height;
                    t = parentSize.height - h - this.options.bottom;
                    if (t < 0) {
                        t = 0;
                    }
                } else {
                    //console.log( 'variable top, fixed bottom, variable height' );
                    
                    /* variable top and height, fixed bottom
                     * set top to min value and stretch height
                     */
                    t = 0;
                    h = parentSize.height - this.options.bottom;
                    if (h < this.options.minHeight) {
                        h = this.options.minHeight;
                    }
                    if (this.options.maxHeight >= 0 && h > this.options.maxHeight) {
                        t = parentSize.height - this.options.maxHeight - this.options.bottom;
                        h = this.options.maxHeight;
                    }
                }
            }
        }
                
        this.domObj.style.position = this.options.position;
        if (this.options.position == 'absolute') {
            var padding = Element.getPaddingSize(this.domObj.parentNode);
        
            this.domObj.style.left = (l+padding.left) + 'px';
            this.domObj.style.top = (t+padding.top) + 'px';
            Element.setBorderBoxSize(this.domObj, {width: w, height: h} );
        } else {
            var sizeOpts = {width: w};
            if (this.options.height) {
                sizeOpts.height = this.options.height;
            }
            Element.setBorderBoxSize(this.domObj, sizeOpts);
        }
        
        var o = {forceResize: options ? options.forceResize : false};
        for (var i=0; i<this.domObj.childNodes.length; i++) {
            var c = this.domObj.childNodes[i];
            if (c.resize) {
                c.resize(o);
            }
        }
        this.processEvent(this.scl,'sizeChanged',this);
    },
    /**
     * Method: addSizeChangeListener
     * add a size change listener to be notified when the size of the
     * element changes.
     *
     * Parameters:
     * obj - {Object} a size change listener
     */
     addSizeChangeListener: function(obj){this.addListener(this.scl, obj);},
    /**
     * Method: removeSizeChangeListener
     * remove a size change listener 
     *
     * Parameters:
     * obj - {Object} a size change listener
     */
     removeSizeChangeListener: function(o) {
        this.removeListener(this.scl, o);
    }
};
Object.extend(Jx.Layout.prototype, Jx.Listener.prototype);

/**
 * Class: Jx.Constraint
 * A utility class used with <Jx.Layout> to represent the current
 * set of constraints on an element.  This object is not intended
 * to be instantiated directly by a user
 */
Jx.Constraint = Class.create();
Jx.Constraint.prototype = {
    position: 'absolute',
    left: 0,
    right: 0,
    top: 0,
    bottom: 0,
    width: null,
    height: null,
    minWidth: 0,
    minHeight: 0,
    maxWidth: -1,
    maxHeight: -1,
    initialize: function(o) {
        for( var i in o ) {
            this[i] = o[i];
        }
    }
};/**
 * $Id: jxmenu.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx.Menu
 *
 * Purpose: 
 * Implementation of a javascript menuing system for Jx.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/


 Jx.addStyleSheet('menu/menu.css');
 Jx.addStyleSheet('button/button.css');

/**
 * Class: Jx.MenuItem
 * A menu item is a single entry in a menu.  It is typically composed of
 * a label and an optional icon.  Selecting the menu item triggers an
 * action.
 */
Jx.MenuItem = Class.create();
Object.extend(Jx.MenuItem.prototype, Jx.Listener.prototype);
Object.extend(Jx.MenuItem.prototype, {
    /**
     * Property: al
     * {Array} action listeners
     */
    al: null,
    /**
     * Property: domObj
     * {HTMLElement} the HTML element for this menu item.
     */
    domObj: null,
    /**
     * Property: parent
     * {<Jx.SubMenu> or <Jx.Menu>} the menu that contains the menu item.
     */
    parent: null,
    /**
     * Property: enabled
     * {Boolean} whether the menu item is enabled or not
     */
    enabled: false,
    /**
     * Property: button
     * {<Jx.Button>} the clickable part of the menu item is managed as a
     * <Jx.Button>
     */
    button: null,
    /**
     * Constructor: Jx.MenuItem
     * Create a new instance of Jx.MenuItem
     *
     * Parameters:
     * action - {<Jx.Action>} the action to invoke when the menu item is
     *          invoked
     * options - {Object} an object containing options as below.
     * 
     * Options:
     * image - {String} URL to an image to use as an icon on this menu item
     * label - {String} the label for the menu item.
     */
    initialize: function(action, options) {
        this.initializeItem(options);
        action.bindTo(this);
        this.propertyChanged(action);
    },
    /**
     * Method: initializeItem
     * Internal method to initalize the button for this menu item.
     *
     * Parameters:
     * options - {Object} an object containing options as below.
     *
     * Options:
     * image - {String} URL to an image to use as an icon on this menu item
     * label - {String} the label for the menu item.     */
    initializeItem: function(options) { 
        if (!options.image) {
            options.image = Jx.baseURL + 'images/a_pixel.png';
        }
        if (!options.label) {
            options.label = '&nbsp;';
        }
        this.label = options.label || '&nbsp;';
        this.image = options.image || null;
        
        this.al = [];
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxMenuItem';
        
        /* menu items are buttons without the jxButton class */
        var action = new Jx.Action(this.processActionEvent.bindAsEventListener(this))
        this.button = new Jx.Button(action, options);
        //Element.removeClassName(this.button.domA, 'jxButton');
        
        Event.observe(this.button.domObj, 'mouseover', this.onmouseover.bindAsEventListener(this), true);
        
        this.domObj.appendChild(this.button.domObj);
    },
    /**
     * Method: setParent
     * Set the parent of this menu item
     *
     * Parameters:
     * obj - {Object} the new parent
     */
    setParent: function(obj) {
        this.parent = obj;
    },
    /**
     * Method: hide
     * Hide the menu item.
     */
    hide: function() {},
    /**
     * Method: show
     * Show the menu item
     */
    show: function() {},
    /**
     * Method: addActionListener
     * Add an action listener to this menu item
     *
     * Parameters:
     * obj - {Object} the action listener
     */
    addActionListener: function(obj) { this.addListener(this.al,obj); },
    /**
     * Method: removeActionListener
     * Remove an action listener from this menu item
     *
     * Parameters:
     * obj - {Object} the action listener
     */
    removeActionListener : function(obj) { this.removeListener(this.al, obj); },
    /**
     * Method: processActionEvent
     * Process an action event by informing any action listeners that the action
     * associated with this menu item and hiding the menu.
     *
     * Parameters:
     * e - {Event} the event associated with the user clicking on the menu item
     */
    processActionEvent: function(e) { 
        if (this.enabled) {
            this.processEvent(this.al, 'actionPerformed', this);
            if (this.parent && this.parent.deactivate) {
                this.parent.deactivate(e);
            }
        }
    },
    /**
     * Method: propertyChanged
     * Track when a property of the action changes and update the state of the
     * menu item accordingly.
     *
     * Parameters:
     * obj - {<Jx.Action>} the action object
     */
    propertyChanged: function(obj) {
        this.enabled = obj.isEnabled();
        if (this.enabled) {
            Element.removeClassName( this.domObj.childNodes[0].childNodes[0], 'jxDisabled' );
        } else {
            Element.addClassName( this.domObj.childNodes[0].childNodes[0], 'jxDisabled' );
        }
    },
    /**
     * Method: onmouseover
     * handle the mouse moving over the menu item
     *
     * Parameters:
     * e - {Event} the mousemove event
     */
    onmouseover: function(e) {
        var target = Event.element(e);
        if (this.parent && this.parent.setVisibleItem) {
            this.parent.setVisibleItem(this);
        }
        this.show();
    }
});

/**
 * Class: Jx.MenuSeparator
 * A convenience class to create a visual separator in a menu.
 */
Jx.MenuSeparator = Class.create();
Object.extend(Jx.MenuSeparator.prototype, {
    /**
     * Property: domObj
     * {HTMLElement} the HTML element that the separator is contained
     * within
     */
    domObj: null,
    /**
     * Property: parent
     * {<Jx.Menu>, <Jx.SubMenu>} the menu that the separator is in.
     */
    parent: null,
    /**
     * Constructor: Jx.MenuSeparator
     * Create a new instance of a menu separator
     */
    initialize: function() {
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxMenuItem';
        var span = document.createElement('span');
        span.className = 'jxMenuSeparator';
        span.innerHTML = '&nbsp;';
        this.domObj.appendChild(span);
    },
    /**
     * Method: setParent
     * Set the parent of this menu item
     *
     * Parameters:
     * obj - {Object} the new parent
     */
    setParent: function(obj) {
        this.parent = obj;
    },
    /**
     * Method: hide
     * Hide the menu item.
     */
    hide: function() {},
    /**
     * Method: show
     * Show the menu item
     */
    show: function() {}
});

/**
 * Class: Jx.SubMenu
 * A sub menu contains menu items within a main menu or another
 * sub menu.
 *
 * Inherits From:
 * <Jx.MenuItem>
 */
Jx.SubMenu = Class.create();
Object.extend(Jx.SubMenu.prototype, Jx.MenuItem.prototype);
Object.extend(Jx.SubMenu.prototype, {
    /**
     * Property: subDomObj
     * {HTMLElement} the HTML container for the sub menu.
     */
    subDomObj: null,
    /**
     * Property: parent
     * {<Jx.Menu> or <Jx.SubMenu>} the menu or sub menu that this sub menu
     * belongs
     */
    parent: null,
    /**
     * Property: visibleItem
     * {<Jx.MenuItem>} the visible item within the menu
     */
    visibleItem: null,
    /**
     * Property: items
     * {Array} the menu items that are in this sub menu.
     */
    items: null,
    /**
     * Constructor: Jx.SubMenu
     * Create a new instance of Jx.SubMenu
     *
     * Parameters:
     * options - see <Jx.MenuItem::Jx.MenuItem>
     */
    initialize: function(options) { 
        this.open = false;
        this.items = [];
        this.initializeItem(options);
        Element.addClassName(this.domObj.childNodes[0].childNodes[0], 'jxButtonSubMenu');
        
        this.iframe = document.createElement('iframe');
        this.iframe.className = 'jxMenuShim';
        this.iframe.scrolling = 'no';
        this.iframe.frameborder = 0;
        
        this.subDomObj = document.createElement('ul');
        this.subDomObj.className = 'jxSubMenu';
        this.subDomObj.style.display = 'none';
        
        // this.button.domObj.appendChild(this.subDomObj);
        this.domObj.appendChild(this.subDomObj);
    },
    /**
     * Method: setParent
     * Set the parent of this sub menu
     *
     * Parameters:
     * obj - {Object} the parent
     */
    setParent: function(obj) {
        this.parent = obj;
    },
    /**
     * Method: show
     * Show the sub menu
     */
    show: function() {
        if (this.open || this.items.length == 0) {
            return;
        }

        this.open = true;
        this.subDomObj.style.display = 'block';
        if (!window.opera) {
            this.subDomObj.childNodes[0].appendChild(this.iframe);
            var size = Element.getBorderBoxSize(this.subDomObj);
            this.iframe.style.width = size.width + "px";
            this.iframe.style.height = size.height + "px";
            
        }
        this.setActive(true);
    },
    /**
     * Method: hide
     * Hide the sub menu
     */
    hide: function() {
        if (!this.open) {
            return;
        }
        this.open = false;
        for (var i=0; i<this.items.length; i++) {
            this.items[i].hide();
        }
        this.subDomObj.style.display = 'none';
        if (!window.opera && this.iframe.parentNode) {
            this.subDomObj.childNodes[0].removeChild(this.iframe);
        }
        this.visibleItem = null;
    },
    /**
     * Method: add
     * Add menu items to the sub menu.
     *
     * Parameters:
     * item - {<Jx.MenuItem>} the menu item to add.  Multiple menu items
     * can be added by passing multiple arguments to this function.
     */
    add : function() { /* menu */
        for (var i=0; i<arguments.length; i++) {
            var item = arguments[i];
            this.items.push(item);
            item.setParent(this);
            this.subDomObj.appendChild(item.domObj);
        }
    },
    /**
     * Method: insertBefore
     * Insert a menu item before another menu item.
     *
     * Parameters:
     * newItem - {<Jx.MenuItem>} the menu item to insert
     * targetItem - {<Jx.MenuItem>} the menu item to insert before
     */
    insertBefore: function(newItem, targetItem) {
        var bInserted = false;
        for (var i=0; i<this.items.length; i++) {
            if (this.items[i] == targetItem) {
                this.items.splice(i, 0, newItem);
                this.subDomObj.insertBefore(newItem.domObj, targetItem.domObj);
                bInserted = true;
                break;
            }
        }
        if (!bInserted) {
            this.add(newItem);
        }
    },
    /**
     * Method: remove
     * Remove a single menu item from the menu.
     *
     * Parameters:
     * item - {<Jx.MenuItem} the menu item to remove.
     */
    remove: function(item) {
        for (var i=0; i<this.items.length; i++) {
            if (this.items[i] == item) {
                this.items.splice(i,1);
                this.subDomObj.removeChild(item.domObj);
                break;
            }
        }
    },
    /**
     * Method: processActionEvent
     * Process an action event coming from the menu item that
     * represents the sub menu in its parent by showing or hiding
     * the sub menu.
     */
    processActionEvent: function(e) { 
        if (this.open) { 
            this.hide(); 
        } else { 
            this.show();
        }
        return Event.stop(e);
    },
    /**
     * Method: deactivate
     * Deactivate the sub menu
     *
     * Parameters:
     * e - {Event} the event that triggered the menu being
     * deactivated.
     */
    deactivate: function(e) {
        if (this.parent) {
            this.parent.deactivate(e);            
        }
    },
    /**
     * Method: isActive
     * Indicate if this sub menu is active
     *
     * Returns:
     * {Boolean} true if the <Jx.Menu> that ultimately contains
     * this sub menu is active, false otherwise.
     */
    isActive: function() { 
        if (this.parent) {
            return this.parent.isActive();
        } else {
            return false;
        }
    },
    /**
     * Method: setActive
     * Set the active state of the <Jx.Menu> that contains this sub menu
     *
     * Parameters:
     * isActive - {Boolean} the new active state
     */
    setActive: function(isActive) { 
        if (this.parent && this.parent.setActive) {
            this.parent.setActive(isActive);
        }
    },
    /**
     * Method: setVisibleItem
     * Set a sub menu of this menu to be visible and hide the previously
     * visible one.
     *
     * Parameters: 
     * obj - {<Jx.SubMenu>} the sub menu that should be visible
     */
    setVisibleItem: function(obj) {
        if (this.visibleItem != obj) {
            if (this.visibleItem && this.visibleItem.hide) {
                this.visibleItem.hide();
            }
            this.visibleItem = obj;
            this.visibleItem.show();
        }
    }
});

/**
 * Class: Jx.Menu
 * A main menu as opposed to a sub menu that lives inside the menu.
 *
 * TODO: Jx.Menu
 * revisit this to see if Jx.Menu and Jx.SubMenu can be merged into
 * a single implementation.
 */
Jx.Menu = Class.create();
Jx.Menu.prototype = {
    /**
     * Property: domObj
     * {HTMLElement} The HTML element containing the menu.
     */
    domObj : null,
    /**
     * Property: buttonObj
     * {<Jx.Button>} The button that represents this menu in a toolbar and
     * opens the menu.
     */
    buttonObj : null,
    /**
     * Property: subDomObj
     * {HTMLElement} the HTML element that contains the menu items
     * within the menu.
     */
    subDomObj : null,
    /**
     * Property: items
     * {Array} the items in this menu
     */
    items : null,
    /**
     * Property: menus
     * {Array} Class variable that contains a reference to all
     * menus shared by all menu instances.
     */
    menus : [],
    /**
     * Constructor: Jx.Menu
     * Create a new instance of Jx.Menu.
     *
     * Parameters:
     * options - {Object} an options object passed directly to the
     * <Jx.Button> when creating the button that triggers this menu.
     * If options is not passed, then no button is created.
     */
    initialize : function(options) {
        /* stores menu items and sub menus */
        this.items = [];
        
        /* iframe shim to prevent scrollbars and 
           inputs from showing through the menu */
        this.iframe = document.createElement('iframe');
        this.iframe.className = 'jxMenuShim';
        this.iframe.scrolling = 'no';
        this.iframe.frameborder = 0;
        
        /* the DOM element that holds the actual menu */
        this.subDomObj = document.createElement('ul');
        this.subDomObj.className = 'jxMenu';
        this.subDomObj.style.display = 'none';
        
        /* if options are passed, make a button inside an LI so the
           menu can be embedded inside a toolbar */
        if (options) {
            this.domObj = document.createElement('li');
            
            var action = new Jx.Action(this.show.bind(this));
            this.button = new Jx.Button(action, options);
            Element.addClassName(this.button.domObj.firstChild, 'jxButtonMenu');
            this.domObj.appendChild(this.button.domObj);
        
            Event.observe(this.domObj, 'mouseover', this.onMouseOver.bindAsEventListener(this));
            //this.button.domObj.appendChild(this.subDomObj);
            this.domObj.appendChild(this.subDomObj);
        }
        
        /* pre-bind the hide function for efficiency */
        this.hideWatcher = this.hide.bindAsEventListener(this);
    },
    /**
     * Method: add
     * Add menu items to the sub menu.
     *
     * Parameters:
     * item - {<Jx.MenuItem>} the menu item to add.  Multiple menu items
     * can be added by passing multiple arguments to this function.
     */
    add : function() {
        for (var i=0; i<arguments.length; i++) {
            var item = arguments[i];
            this.items.push(item);
            item.setParent(this);
            this.subDomObj.appendChild(item.domObj);
        }
    },
    /**
     * Method: deactivate
     * Deactivate the menu by hiding it.
     */
    deactivate: function() { this.hide(); },
    /**
     * Method: actionPerformed
     * Any action performed in the menu ultimately calls this
     * method to hide the menu.
     */
    actionPerformed : function() {this.hide();},
    /**
     * Method: onMouseOver
     * Handle the user moving the mouse over the button for this menu
     * by showing this menu and hiding the other menu.
     *
     * Parameters:
     * e - {Event} the mouse event
     */
    onMouseOver: function(e) {
        if (this.menus[0] && this.menus[0] != this) {
            this.show(e);
        }
    },
    /**
     * Method: hide
     * Hide the menu.
     *
     * Parameters:
     * e - {Event} the mouse event
     */
    hide: function(e) {
        if (e) {
            var root = Event.findElement(e, 'LI');
            if (root == this.domObj) {
                return;
            }
        }
        if (this.menus[0] && this.menus[0] == this) {
            this.menus[0] = null;
        }
        for (var i=0; i<this.items.length; i++) {
            this.items[i].hide(e);
        }
        Event.stopObserving(document, 'click', this.hideWatcher, true);
        this.subDomObj.style.display = 'none';  
    },
    /**
     * Method: show
     * Show the menu
     *
     * Parameters:
     * e - {Event} the mouse event
     */
    show : function(e) {
        if (this.menus[0] && this.menus[0] != this) {
            this.menus[0].hide(e);
        }
        if (this.items.length ==0) {
            return;
        }
        this.menus[0] = this;
        this.subDomObj.style.display = 'block';
        this.subDomObj.style.visibility = 'visible';
        
        if (!window.opera) {
            this.subDomObj.childNodes[0].appendChild(this.iframe);
            var size = Element.getContentBoxSize(this.subDomObj);
            this.iframe.style.width = size.width + "px";
            this.iframe.style.height = size.height + "px";
        }
        Event.stop(e);
        /* fix bug in IE that closes the menu as it opens because of bubbling */
        Event.observe(document, 'click', this.hideWatcher, true);
    },
    /**
     * Method: setVisibleItem
     * Set the sub menu that is currently open
     *
     * Parameters:
     * obj- {<Jx.SubMenu>} the sub menu that just became visible
     */
    setVisibleItem: function(obj) {
        if (this.visibleItem != obj) {
            if (this.visibleItem && this.visibleItem.hide) {
                this.visibleItem.hide();
            }
            this.visibleItem = obj;
            this.visibleItem.show();
        }
    }
};

/**
 * Class: Jx.ContextMenu
 * A <Jx.Menu> that has no button but can be opened at a specific 
 * browser location to implement context menus (for instance).
 *
 * Inherits From:
 * <Jx.Menu>
 */
Jx.ContextMenu = Class.create();
Object.extend(Jx.ContextMenu.prototype, Jx.Menu.prototype);
Object.extend(Jx.ContextMenu.prototype, {
    /**
     * Constructor: Jx.ContextMenu
     * create a new context menu
     *
     * Parameters:
     * id - {HTMLElement} element or id to make this the context menu
     * for.  The menu hooks the oncontextmenu event of the element
     * and shows itself at the mouse position where the right-click
     * happened.
     */
    initialize : function(id) {
        Jx.Menu.prototype.initialize.apply(this, []);
        document.getElementsByTagName('BODY')[0].appendChild(this.subDomObj);
        if ($(id)) {
            $(id).oncontextmenu = this.show.bindAsEventListener(this);;
        }
    },
    /**
     * Method: show
     * Show the context menu at the location of the mouse click
     *
     * Parameters:
     * e - {Event} the mouse event
     */
    show : function(e) {
        this.subDomObj.style.left = Event.pointerX(e) + "px";
        this.subDomObj.style.top = Event.pointerY(e) + "px";
        Jx.Menu.prototype.show.apply(this, [e]);
    }
});/**
 * $Id: jxpanel.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx.Panel
 *
 * Purpose: 
 * Implementation of a javascript panels for Jx.  Panels are vertically 
 * oriented areas that can be resized by the user dragging the panel title
 * bar.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

Jx.addStyleSheet('panel/panel.css');

/**
 * Class: Jx.PanelManager
 * A panel manager manages a set of panels within a DOM element.
 */
Jx.PanelManager = Class.create();
Jx.PanelManager.prototype = {
    /**
     * Property: panels
     * {Array} the panels being managed by the manager
     */
    panels: null,
    /**
     * Property: height
     * {Integer} the height of the container, cached for speed
     */
    height: null,
    /**
     * Property: firstLayout
     * {Boolean} true until the panel manager has first been resized
     */
    firstLayout: true,
    /**
     * Constructor: Jx.PanelManager
     * Create a new instance of Jx.PanelManager.
     *
     * Parameters:
     * domObj - {HTMLElement} the HTML element that will contain the panels
     * panels - {Array} the panels to go into the PanelManager
     *
     * TODO: Jx.PanelManager.initialize
     * Remove the panels parameter in favour of an add method.
     */
    initialize: function(domObj, panels) {
        this.domObj = $(domObj);
        this.panels = panels;
        var d = document.createElement('div');
        d.style.position = 'absolute';
        new Jx.Layout(d, {minHeight:0,maxHeight:0,height:0});
        var elements = [d];
        for (var i=0; i<this.panels.length; i++) {
            elements.push(this.panels[i].domObj);
        }
        this.splitter = new Jx.Splitter(this.domObj, {splitInto: panels.length+1,
                                                     layout: 'vertical',
                                                     elements: elements });
        /* redirect splitter sizeChanged so we can capture the first time the
         * splitter is layed out in the client in a way that we can measure
         * the title bar heights
         */
        this.splitter.sizeChanged = this.sizeChanged.bind(this);
        /* create the bars for the panel manager */
        for (var i=0; i<this.panels.length; i++) {
            var panel = this.panels[i];
            this.splitter.bars[i].appendChild(panel.title);
            this.splitter.bars[i].style.height = Element.getBorderBoxSize(panel.title).height + 'px';
            Element.removeClassName(this.splitter.bars[i], 'jxSplitterBar');
            Element.addClassName(this.splitter.bars[i], 'jxPanelBar');
            panel.manager = this;
        }
    },
    /**
     * Method: sizeChanged
     * called when the size of the container of the splitter changes.  This tracks the
     * first time the panel titles have a non-zero height (i.e. they are put in the
     * DOM and are visible) and recalculates the panel title height then.
     */ 
    sizeChanged: function() {
        if (this.firstLayout) {
            if (Element.getBorderBoxSize(this.panels[0].title).height != 0) {
                for (var i=0; i<this.splitter.bars.length; i++) {
                    var panel = this.panels[i];
                    this.splitter.bars[i].style.height = Element.getBorderBoxSize(panel.title).height + 'px';
                    this.splitter.bars[i].size = null;
                }
                this.firstLayout = false;
                Jx.Splitter.prototype.sizeChanged.apply(this.splitter, []);            
            }
        } else {
            Jx.Splitter.prototype.sizeChanged.apply(this.splitter, []);            
        }
    },
    
    /**
     * Method: maximizePanel
     * Maximize the panel, taking up all available space (taking into
     * consideration any minimum or maximum values)
     */
    maximizePanel: function(panel) {
        var h = Element.getContentBoxSize(this.domObj).height;
        
        var t = 0;
        for (var i=1; i<this.splitter.elements.length; i++) {
            var p = this.splitter.elements[i];
            t += Element.getBorderBoxSize(p.leftBar).height;
            if (p !== panel.domObj) {
                p.jxLayout.resize({top: t, height: p.jxLayout.options.minHeight, bottom: null});                    
                t += p.jxLayout.options.minHeight;
                p.rightBar.style.top = t + 'px';
            } else {
                break;
            }
        }
        
        b = h;
        for (var i=this.splitter.elements.length - 1; i > 0; i--) {
            p = this.splitter.elements[i];
            if (p !== panel.domObj) {
                b -= p.jxLayout.options.minHeight;
                p.jxLayout.resize({top: b, height: p.jxLayout.options.minHeight, bottom: null});
                b -= Element.getBorderBoxSize(p.leftBar).height;
                p.leftBar.style.top = b + 'px';
                
            } else {
                break;
            }
        }
        panel.domObj.jxLayout.resize({top: t, height:b - t, bottom: null});
    }
};

/**
 * Class: Jx.Panel
 * A panel that can be displayed inside a panel manager.
 */
Jx.Panel = Class.create();
Jx.Panel.prototype = {
    /** 
     * Property: labelObj
     * the DOM object that holds the label in the title bar. 
     */
    labelObj : null,
    /**
     * Property: state
     * the state of this panel 
     */
    state : 'open',
    /**
     * Property: busyCount
     * track the busy state of this panel - used to control a 'loading' image 
     */
    busyCount : null,
    /**
     * Property: bContentReady
     * {Boolean} is the content ready to be displayed?
     */
    bContentReady : null,
    /**
     * Property: onContentReady
     * {Function} a function to call when the content is ready
     */
    onContentReady : null,
    
    /** 
     * Constructor: Jx.Panel
     * Initialize a new Jx.Panel instance
     *
     * Options:
     *
     * label - String, the title of the Jx Panel
     * toolbar - element to use as the toolbar
     * menubar - element to use as the menu
     * content - element to use as the content. A content area is created
     *           if none is provided.  Otherwise, the content element is moved
     *           in the DOM
     * statusbar - element to use as the statusbar
     * helpCallback - function to call when the user clicks the contextual help button
     * state - initial state of the panel (open or closed)
     *
     * Inherits From:
     * <Jx.UniqueId>, <Jx.ContentLoader>
     */
    initialize : function(options){
        //console.log("Jx.Panel::initialize('"+options.label+"')");
        this.initUniqueId();
        
        /* set up the title object */
        this.title = document.createElement('div');
        this.title.className = "jxPanelTitle";
        //TODO: Jx.Panel.initialize
        //Opera is broken because it doesn't report the height of the
        //title bars at all unless set through javascript
        //this is a hack until we can figure out from css what the height is
        this.title.style.height = '22px';
        
        this.labelObj = document.createElement('span');
        this.labelObj.className = 'jxPanelLabel';
        this.labelObj.innerHTML = options.label?options.label:'empty';
    
        this.imageBaseUrl = options.imageBaseUrl || Jx.baseURL + 'images/';
        
        var a, img;
        if (options.helpCallback) {
            a = document.createElement('a');
            a.className = 'jxPanelHelp';
            a.href = 'javascript:void(0)';
            Event.observe(a, 'click', options.helpCallback);
            img = document.createElement('img');
            img.src = this.imageBaseUrl + "help.png";
            img.alt = 'Help on this panel';
            img.title = 'Help on this panel';
            a.appendChild(img);
            this.title.appendChild(a);
        }
    
        a = document.createElement('a');
        a.className = 'jxPanelMaximize';
        a.href = 'javascript:void(0)';
        Event.observe(a, 'click', this.maximize.bindAsEventListener(this));
        img = document.createElement('img');
        img.src = this.imageBaseUrl + "maximize.png";
        img.alt = 'Maximize Panel';
        img.title = 'Maximize Panel';
        a.appendChild(img);
        this.title.appendChild(a);
    
        a = document.createElement('a');
        a.className = 'jxPanelLoading';
        a.href = 'javascript:void(0)';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + "loading.gif";
        img.alt = 'Loading Panel';
        img.title = 'Loading Panel';
        a.appendChild(img);
        this.loadingObj = {};
        this.loadingObj.link = a;
        this.loadingObj.img = img;
        this.title.appendChild(this.loadingObj.link);
        this.title.appendChild(this.labelObj);
        
        Event.observe(this.title, 'dblclick', this.maximize.bindAsEventListener(this));
        
        this.domObj = document.createElement('div');
        this.domObj.className = 'jxPanel';
        
        this.jxLayout = new Jx.Layout(this.domObj, options.constraint || {});
        this.jxLayout.addSizeChangeListener(this);
        
        var top = 0;
        var bottom = 0;
        if (options.menubar) {
            this.menubar = options.menubar;
            this.domObj.appendChild(options.menubar);
            var h = Element.getBorderBoxSize(options.menubar).height;
            new Jx.Layout(this.menubar, {top: top, height:h});
            top += h;
        }
        if (options.toolbar) {
            this.toolbar = options.toolbar;
            this.domObj.appendChild(options.toolbar);
            var h = Element.getBorderBoxSize(options.toolbar).height;
            new Jx.Layout(this.toolbar, {top:top, height: h});
        }
        
        if (options.statusbar) {
            this.statusbar = options.statusbar;
            this.domObj.appendChild(options.statusbar);
            var h = Element.getBorderBoxSize(options.statusbar).height;
            new Jx.Layout(this.statusbar, {bottom: bottom, height: h, top: null});
            bottom += h;
        }
        this.content = document.createElement('div');
        Element.addClassName(this.content, 'jxPanelContent');
        new Jx.Layout(this.content, {top: top, bottom: bottom});
        this.domObj.appendChild(this.content);
        this.loadContent(this.content, options);
        
        this.busyCount = 0;
        this.bContentReady = false;
    },
    /**
     * Method: setLabel
     * Set the label in the title bar of this panel
     *
     * Parameters:
     * s - {String} the new label
     */
    setLabel: function(s) {
        this.labelObj.innerHTML = s;
    },
    /**
     * Method: getLabel
     * Get the label of the title bar of this panel
     *
     * Returns: 
     * {String} the label
     */
    getLabel: function() {
        return this.labelObj.innerHTML;
    },
    /**
     * Method: finalize
     * Clean up the panel
     */
    finalize: function() {
        this.domObj = null;
        this.deregisterIds();
    },
    /**
     * Method: maximize
     * Maximize this panel
     */
    maximize: function() {
        if (this.manager) {
            this.manager.maximizePanel(this);
        }
    },
    /**
     * Method: setContent
     * set the content of this panel to some HTML
     *
     * Parameters:
     * html - {String} the new HTML to go in the panel
     */
    setContent : function (html) {
        //console.log('Jx.Panel::setContent()');
        this.content.innerHTML = html;
        this.bContentReady = true;
    },
    /**
     * Method: setContentURL
     * Set the content of this panel to come from some URL.
     *
     * Parameters:
     * url - {String} URL to some HTML content for this panel
     */
    setContentURL : function (url) {
        this.bContentReady = false;
        this.setBusy(true);
        if (arguments[1]) {
            this.onContentReady = arguments[1];
        }
        if (url.indexOf('?') == -1) {
            url = url + '?';
        }
        //var ts = (new Date()).getTime();
        //url = url + 'ts='+ts;
        var opts = { method: 'get',
                     onComplete:this.panelContentLoaded.bind(this),
                     requestHeaders: ['If-Modified-Since', 'Sat, 1 Jan 2000 00:00:00 GMT']};
        var a = new Ajax.Request( url, opts);
    },
    /**
     * Method: panelContentLoaded
     * When the content of the panel is loaded from a remote URL, this 
     * method is called when the ajax request returns.
     *
     * Parameters:
     * r - {XmlHttpRequest} the XmlHttpRequest object
     */
    panelContentLoaded: function(r) {
        this.content.innerHTML = r.responseText;
        this.bContentReady = true;
        this.setBusy(false);
        if (this.onContentReady) {
            window.setTimeout(this.onContentReady.bind(this),1);
        }
    },
    /**
     * Method: setBusy
     * Set the panel as busy or not busy, which displays a loading image
     * in the title bar.
     *
     * Parameters:
     * isBusy - {Boolean} the busy state
     */
    setBusy : function(isBusy) {
        this.busyCount += isBusy?1:-1;
        this.loadingObj.img.style.visibility = (this.busyCount>0)?'visible':'hidden';
    },
    /**
     * Method: sizeChanged
     * handle the size of the container changing by resizing the various
     * elements of the panel.
     */
    sizeChanged: function() {
        var top = 0;
        var bottom = 0;
        if (this.menubar) {
            this.menubar.style.height = '';
            var size = Element.getBorderBoxSize(this.menubar);
            this.menubar.resize({height:size.height});
            top += size.height;
        }
        if (this.toolbar) {
            this.toolbar.style.height = '';
            var size = Element.getBorderBoxSize(this.toolbar);
            this.toolbar.resize({height:size.height});
            top += size.height;
        }
        if (this.statusbar) {
            this.statusbar.style.height = '';
            var size = Element.getBorderBoxSize(this.statusbar);
            this.statusbar.resize({height:size.height});
            bottom += size.height;
        }
        if (top || bottom) {
            this.content.resize({bottom: bottom, top: top});
        }               
    }
};
Object.extend(Jx.Panel.prototype, Jx.UniqueId.prototype);
Object.extend(Jx.Panel.prototype, Jx.ContentLoader.prototype);/**
 * $Id: jxpicker.js 516 2008-03-10 23:21:04Z pspencer $
 *
 * Title: Jx.Picker
 *
 * Purpose: 
 * Implementation of a javascript Pick List for Jx.  This is analagous to an 
 * HTML select except that it allows other things to be put in the list.  The
 * selected item from the list can also be editable.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

Jx.addStyleSheet('picker/picker.css');

/**
 * Class: Jx.Picker
 * A drop down list of selectable items that can be any HTML.
 *
 * Inherits From:
 * <Jx.Listener>
 */
Jx.Picker = Class.create();
Jx.Picker.prototype = {
    /** 
     * Property: sl
     * {Array} selection listeners 
     */
    sl : null,
    
    /** 
     * Property: domObj
     * {HTMLElement} the div that contains the control, 
     * used to show/hide the control 
     */
    domObj : null,
    /** 
     * Property: ul
     * {HTMLElement} the ul that contains the selectable items 
     */
    ul : null,
    /**
     * Property: currentSelection
     * {Object} current selection in the list 
     */
    currentSelection : null,
    
    /**
     * Property: isEditable
     * {Boolean} is the selected item editable? 
     **/
    isEditable: false,
    /** 
     * Constructor: Jx.Picker
     * create a new instance of Jx.Picker
     *
     * Options:
     * editable - {Boolean} defaults to false.  If true, then the selected item
     * is editable.
     */
    initialize: function(options) {
        options = options || {};
        
        this.domObj = document.createElement('div');
        this.domObj.className = 'jxPicker';
        this.isEditable = options.editable || false;
        
        if (this.isEditable) {
            this.domInput = document.createElement('input');
            this.domInput.type = 'text';
            Event.observe(this.domInput, 'change', this.valueChanged.bind(this));
            Event.observe(this.domInput, 'keydown', this.onKeyPress.bind(this));
        } else {
            this.domInput = document.createElement('span');
        }
        this.domInput.className = 'jxPickerInput';
        
        this.domObj.appendChild(this.domInput);
        
        this.domA = document.createElement('a');
        this.domA.href = 'javascript:void(0)';
        this.domA.className = 'jxPickerDiscloser';
        Event.observe(this.domA, 'click', this.toggle.bind(this));
        
        this.domButton = document.createElement('img');
        this.domButton.src = Jx.baseURL + 'images/disclose2.png';
        Element.addClassName(this.domButton, 'png24');
        this.domA.appendChild(this.domButton);
        this.domObj.appendChild(this.domA);
        
        if (!window.opera) {
            //insert iframeShim for IE to cover other controls in the page.
            var iframe = document.createElement('iframe');
            iframe.className = 'jxDialogShim';
            iframe.scrolling = 'no';
            iframe.frameborder = 0;
            this.domObj.appendChild(iframe);
        }
        
        this.domListDiv = document.createElement('div');
        this.domListDiv.className = 'jxPickerOptions';
        this.domListDiv.style.display = 'none';
        this.domObj.appendChild(this.domListDiv);
        this.domList = document.createElement('ul');
        this.domListDiv.appendChild(this.domList);
        this.sl = [];
    },
    
    /**
     * Method: onKeyPress
     * Handle the user pressing a key by looking for an ENTER key to set the
     * value.
     *
     * Parameters:
     * e - {Event} the keypress event
     */
    onKeyPress: function(e) {
        var charCode = (e.charCode) ? e.charCode : ((e.keyCode) ? e.keyCode : e.which);
        if (charCode == Event.KEY_RETURN) {
            this.valueChanged();
        }
    },
    
    /**
     * Method: valueChanged
     * When the value is changed, propogate the change to the selection 
     * listeners
     */
    valueChanged: function() {
        this.processEvent(this.sl, 'selectionChanged', this);
    },
    
    /**
     * Method: add
     * add a new item to the pick list
     *
     * Parameters:
     * domObj - {HTMLElement} the element to add
     * idx - {Integer} the index to add the element add
     */
    add: function(domObj, idx) {
        var li = document.createElement('li');
        var a = document.createElement('a');
        a.href="javascript:void(0)";
        
        if (typeof(domObj) == 'string') {
            a.innerHTML = domObj;
        } else {
            a.appendChild(domObj);
        }
        Event.observe(a, 'click', this.pick.bindAsEventListener(this));
        li.appendChild(a);

        if (arguments.length > 1 && this.domList.childNodes.length > idx) {
            this.domList.insertBefore(li, this.domList.childNodes[idx]);
        } else {
            this.domList.appendChild(li);
        }
        if (this.getValue() == '') {
            this.setValue(a.childNodes[0]);
        }
    },
    
    /**
     * Method: remove
     * Remove the item at the given index
     *
     * Parameters:
     * idx - {Integer} the item to remove.
     */
    remove: function(idx) {
        if (idx > 0 && idx < this.domList.childNodes.length) {
            this.domList.removeChild(this.domList.childNodes[idx]);
        }
    },
    
    /**
     * Method: pick
     * user has clicked something in the list, select it
     *
     * Parameters:
     * e - {Event} the mouse event from the user's click
     */
    pick: function(e) {
        var target = Event.element(e);
        if (target.tagName == 'A') {
            this.currentSelection = target;
            this.setValue(this.currentSelection.childNodes[0]);
            this.valueChanged();
        }
        this.close();
    },
    /**
     * Method: setValue
     * set the value of the picker
     *
     * Parameters:
     * value - {Object} the new value.  May be a string, a text node, or
     * another DOM element.
     */
    setValue: function(value) {
        if (this.isEditable) {
            if (typeof(value) == 'string') {
                this.domInput.value = value;
            } else if (value.nodeType && value.nodeType == 3) {
                this.domInput.value = value.nodeValue;
            } else {
                this.domInput.value = value.innerHTML;
            }
        } else if (!this.isEditable) {
            if (typeof(value) == 'string') {
                this.domInput.innerHTML = value;
            } else if (value.nodeType && value.nodeType == 3) {
                this.domInput.innerHTML = value.nodeValue;
            } else {
                this.domInput.appendChild(value);
            }
        }
    },
    
    /**
     * Method: getValue
     * Return the current value
     *
     * Returns:
     * {Object} returns the currently selected item
     */
    getValue: function() {
        value = '';
        if (this.isEditable) {
            value = this.domInput.value;
        } else if (this.domInput.childNodes.length > 0) {
            if (this.domInput.childNodes[0].nodeType == 3) {
                value = this.domInput.innerHTML;
            } else {
                value = this.domInput.childNodes[0];
            }
        }
        return value;
    },
    
    /**
     * Method: toggle
     * Toggle the display of the list associated with the picker
     */
    toggle: function() {
        if (this.domListDiv.style.display == 'block') {
            this.close();
        } else {
            this.open();
        }
    },
    
    /**
     * Method: open
     * Open the pick list
     */
    open: function() {
        if (!this.keypressListener) {
            this.keypressListener = this.keypress.bindAsEventListener(this);
        }
        this.domListDiv.style.display = 'block';
        Event.observe(document, 'keypress', this.keypressListener);
        //Effect.SlideDown(this.domObj, 0.2);
    },
    /**
     * Method: keypress
     * Handle a key press event when the list is open so we can close it if the
     * user hits the ESC key.
     *
     * Parameters:
     * e - {Event} the keypress event
     */
    keypress: function(e) {
        var charCode=(e.charCode)?e.charCode:e.keyCode;
        if (charCode == Event.KEY_ESC) {
            this.close();
        }
    },
    /**
     * Method: close
     * Close the picker.
     */
    close: function() {
        this.domListDiv.style.display = 'none';
        Event.stopObserving(document, 'keypress', this.keypressListener);
        //Effect.SlideUp(this.domObj, 0.2);
    },
    /**
     * Method: addSelectionListener
     * Add a selection listener to the picker
     *
     * Parameters:
     * obj - {Object} the selection listener
     */
    addSelectionListener: function(obj) { this.addListener(this.sl, obj); },
    /**
     * Method: removeSelectionListener
     * Remove a selection listener to the picker
     *
     * Parameters:
     * obj - {Object} the selection listener
     */
    removeSelectionListener: function(obj) { this.removeListener(this.sl, obj); },
    /**
     * Method: getSelection
     * return the current selection
     *
     * Returns:
     * {Object} the current selection or an empty string.
     */
    getSelection: function() {
        return this.currentSelection ? this.currentSelection.name : '';
    }
};
Object.extend(Jx.Picker.prototype, Jx.Listener.prototype);/**
 * $Id: jxsplitter.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx.Splitter
 *
 * Purpose: 
 * Implementation of a javascript Splitter for Jx.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
  
/**
 * Class: Jx.Splitter
 * a Jx.Splitter creates two or more containers within a parent container
 * and provides user control over the size of the containers.  The split
 * can be made horizontally or vertically.
 * 
 * A horizontal split creates containers that divide the space horizontally
 * with vertical bars between the containers.  A vertical split divides
 * the space vertically and creates horizontal bars between the containers.
 */
 
Jx.Splitter = Class.create();
Jx.Splitter.prototype = {
    /**
     * Property: domObj
     * {HTMLElement} the element being split
     */
    domObj: null,
    /**
     * Property: elements
     * {Array} an array of elements that are displayed in each of the split 
     * areas
     */
    elements: null,
    /**
     * Property: bars
     * {Array} an array of the bars between each of the elements used to
     * resize the split areas.
     */
    bars: null,
    /**
     * Property: firstUpdate
     * {Boolean} track the first resize event so that unexposed Jx things
     * can be forced to calculate their size the first time they are exposed.
     */
    firstUpdate: true,
    /**
     * Constructor: Jx.Splitter
     * Create a new instance of Jx.Splitter
     *
     * Parameters:
     * domObj - {HTMLElement} the element or id of the element to split
     * options - {Object} optional arguments specified as properties of
     * this object are as below.
     *
     * Options:
     * elements - {Array} an array of elements to put into the split areas.
     *      If splitInto is not set, then it is calculated from the length of
     *      this array.
     * splitInto - {Integer} the number of elements to split the domObj into.
     *      If not set, then the length of the elements option is used, or 2 if
     *      elements is not specified.  If splitInto is specified and elements
     *      is specified, then splitInto is used.  If there are more elements than
     *      splitInto specifies, then the extras are ignored.  If there are less
     *      elements than splitInto specifies, then extras are created.
     * containerOptions - {Array} an array of objects that provide options
     *      for the <Jx.Layout> constraints on each element.
     * layout - {String} either 'horizontal' or 'vertical', indicating the
     *      direction in which the domObj is to be split.
     * snappers - {Array} an array of objects which can be used to snap
     *      elements open or closed.
     */
    initialize: function(domObj, options) {
        options = options || {};  
        
        this.domObj = $(domObj);
        this.domObj.style.overflow = 'hidden';
        if (this.domObj.jxLayout) {
            this.domObj.jxLayout.addSizeChangeListener(this);
        }
       
        this.elements = [];
        this.bars = [];
        
        var nSplits = options.splitInto || (options.elements ? options.elements.length : 2);
        
        var aContainerOpts = options.containerOptions || [];

        for (var i=0; i<nSplits; i++) {
            this.elements[i] = options.elements ? options.elements[i] : this.prepareElement();
            this.domObj.appendChild(this.elements[i]);
            if (!this.elements[i].jxLayout) {
                new Jx.Layout(this.elements[i], aContainerOpts[i]);
            }
        }
        
        for (var i=1; i<nSplits; i++) {
            this.bars[i-1] = this.prepareBar();
            this.bars[i-1].leftSide = this.elements[i-1];
            this.bars[i-1].rightSide = this.elements[i];
            this.elements[i-1].rightBar = this.bars[i-1];
            this.elements[i].leftBar = this.bars[i-1];
            this.domObj.appendChild(this.bars[i-1]);
        }

        this.layout = options.layout || 'horizontal';
        this.establishConstraints();
        
        if (options.snappers) {
            for (var i=0; i<options.snappers.length; i++) {
                if (options.snappers[i]) {
                    new Jx.Splitter.Snapper(options.snappers[i], this.elements[i], this);
                }
            }
        }
    },
    /**
     * Method: prepareElement
     * Prepare a new, empty element to go into a split area.
     *
     * Returns:
     * {HTMLElement} an HTMLElement that goes into a split area.
     */
    prepareElement: function(){
        var o = document.createElement('div');
        o.style.position = 'absolute';
        o.leftBar = null;
        o.rightBar = null;
        return o;
    },
    
    /**
     * Method: prepareBar
     * Prepare a new, empty bar to go into between split areas.
     *
     * Returns:
     * {HTMLElement} an HTMLElement that becomes a bar.
     */
    prepareBar: function() {
        var o = document.createElement('div');
        o.className = 'jxSplitterBar';
        o.style.position = 'absolute';
        o.title = 'drag this bar to resize';
        o.style.lineHeight = '1px'; // for IE, horizontal bars
        o.splitterObj = this;
        return o;
    },
    
    /**
     * Method: establishConstraints
     * Setup the initial set of constraints that set the behaviour of the
     * bars between the elements in the split area.
     */
    establishConstraints: function() {
        if (this.layout == 'horizontal') {
            for (var i=0; i< this.bars.length; i++) {
                this.bars[i].style.top = '0px';
                this.bars[i].style.height = '100%';
                new Draggable(this.bars[i], { constraint: 'horizontal' , 
                                     starteffect : function(element) { 
                                            element.style.backgroundColor = '#eee';
                                            },
                                     endeffect : function(element) {
                                         element.style.backgroundColor = '';
                                     }
                });
            }
        } else {
            for (var i=0; i< this.bars.length; i++) {
                this.bars[i].style.left = '0px';
                this.bars[i].style.width = '100%';
                new Draggable(this.bars[i], { constraint: 'vertical' , 
                                     starteffect : function(element) { 
                                            element.style.backgroundColor = '#eee';
                                            },
                                     endeffect : function(element) {
                                         element.style.backgroundColor = '';
                                     }
                });
            }
        }
        Draggables.addObserver(this);
    },
    
    /**
     * Method: onEnd
     * Handle the end of a drag event on a bar.
     *
     * Parameters:
     * eventName - {String} the name associated with the drag event
     * obj - {HTMLElement} the bar that was dragged
     * event - {Event} the event object associated with the drag
     */
    onEnd: function(eventName, obj, event) {
        if (obj.element.splitterObj != this) {
            return;
        }
        if (this.layout == 'horizontal') {
            this.dragHorizontal(eventName, obj, event);
        } else {
            this.dragVertical(eventName, obj, event);
        }
    },
    
    /**
     * Method: dragHorizontal
     * In a horizontally split container, handle a bar being dragged left or
     * right by resizing the elements on either side of the bar.
     *
     * Parameters:
     * eventName - {String} the name associated with the drag event
     * obj - {HTMLElement} the bar that was dragged
     * event - {Event} the event object associated with the drag
     */
    dragHorizontal: function(eventName, obj, event) {
        var leftEdge = parseInt(obj.element.style.left);
        var leftSide = obj.element.leftSide;
        var rightSide = obj.element.rightSide;
        
        /* process right side first */
        var rsLeft, rsWidth, rsRight;
        
        if (!obj.element.size) {
            obj.element.size = Element.getBorderBoxSize(obj.element);
        }
        var barSize = obj.element.size;
        rsLeft = leftEdge + barSize.width;
        
        var parentSize = Element.getContentBoxSize(this.domObj);
        
        if (rightSide.jxLayout.options.width != null) {
            rsWidth = rightSide.jxLayout.options.width + rightSide.jxLayout.options.left - rsLeft;
            rsRight = parentSize.width - rsLeft - rsWidth;
        } else {
            rsWidth = parentSize.width - rightSide.jxLayout.options.right - rsLeft;
            rsRight = rightSide.jxLayout.options.right;
        }
        
        /* enforce constraints on right side */
        if (rsWidth < 0) {
            rsWidth = 0;
        }
        
        if (rsWidth < rightSide.jxLayout.options.minWidth) {
            rsWidth = rightSide.jxLayout.options.minWidth;
        }
        if (rightSide.jxLayout.options.maxWidth >= 0 && rsWidth > rightSide.jxLayout.options.maxWidth) {
            rsWidth = rightSide.jxLayout.options.maxWidth;
        }
        
        rsLeft = parentSize.width - rsRight - rsWidth;
        leftEdge = rsLeft - barSize.width;
        
        /* process left side */
        var lsLeft, lsWidth;
        lsLeft = leftSide.jxLayout.options.left;
        lsWidth = leftEdge - lsLeft;
        
        /* enforce constraints on left */
        if (lsWidth < 0) {
            lsWidth = 0;
        }
        if (lsWidth < leftSide.jxLayout.options.minWidth) {
            lsWidth = leftSide.jxLayout.options.minWidth;
        }
        if (leftSide.jxLayout.options.maxWidth >= 0 && 
            lsWidth > leftSide.jxLayout.options.maxWidth) {
            lsWidth = leftSide.jxLayout.options.maxWidth;
        }
        
        /* update the leftEdge to accomodate constraints */
        if (lsLeft + lsWidth != leftEdge) {
            /* need to update right side, ignoring constraints because left side
               constraints take precedence (arbitrary decision)
             */
            leftEdge = lsLeft + lsWidth;
            var delta = leftEdge + barSize.width - rsLeft;
            rsLeft += delta;
            rsWidth -= delta; 
        }
        
        /* put bar in its final location based on constraints */
        obj.element.style.left = leftEdge + 'px';
        
        /* update leftSide positions */
        if (leftSide.jxLayout.options.width == null) {
            var parentSize = Element.getContentBoxSize(this.domObj);
            leftSide.jxLayout.resize({right: parentSize.width - lsLeft-lsWidth});
        } else {
            leftSide.jxLayout.resize({width: lsWidth});
        }
        
        /* update rightSide position */
        if (rightSide.jxLayout.options.width == null) {
            rightSide.jxLayout.resize({left:rsLeft});
        } else {
            rightSide.jxLayout.resize({left: rsLeft, width: rsWidth});
        }
    },
    
    /**
     * Method: dragVertical
     * In a vertically split container, handle a bar being dragged up or
     * down by resizing the elements on either side of the bar.
     *
     * Parameters:
     * eventName - {String} the name associated with the drag event
     * obj - {HTMLElement} the bar that was dragged
     * event - {Event} the event object associated with the drag
     */
    dragVertical: function(eventName, obj, event) {
        /* top edge of the bar */
        var topEdge = parseInt(obj.element.style.top);
        
        /* the containers on either side of the bar */
        var topSide = obj.element.leftSide;
        var bottomSide = obj.element.rightSide;
        
        /* measure the bar and parent container for later use */
        if (!obj.element.size) {
            obj.element.size = Element.getBorderBoxSize(obj.element);
        }
        var barSize = obj.element.size;
        var parentSize = Element.getContentBoxSize(this.domObj);

        /* process top side first */
        var bsTop, bsHeight, bsBottom;
        
        /* top edge of bottom side is the top edge of bar plus the height of the bar */
        bsTop = topEdge + barSize.height;
        
        if (bottomSide.jxLayout.options.height != null) {
            /* bottom side height is fixed */
            bsHeight = bottomSide.jxLayout.options.height + bottomSide.jxLayout.options.top - bsTop;
            bsBottom = parentSize.height - bsTop - bsHeight;
        } else {
            /* bottom side height is not fixed. */
            bsHeight = parentSize.height - bottomSide.jxLayout.options.bottom - bsTop;
            bsBottom = bottomSide.jxLayout.options.bottom;
        }
        
        /* enforce constraints on bottom side */
        if (bsHeight < 0) {
            bsHeight = 0;
        }
        
        if (bsHeight < bottomSide.jxLayout.options.minHeight) {
            bsHeight = bottomSide.jxLayout.options.minHeight;
        }
        
        if (bottomSide.jxLayout.options.maxHeight >= 0 && bsHeight > bottomSide.jxLayout.options.maxHeight) {
            bsHeight = bottomSide.jxLayout.options.maxHeight;
        }
        
        /* recalculate the top of the bottom side in case it changed
           due to a constraint.  The bar may have moved also.
         */
        bsTop = parentSize.height - bsBottom - bsHeight;
        topEdge = bsTop - barSize.height;
                
        /* process left side */
        var tsTop, tsHeight;
        tsTop = topSide.jxLayout.options.top;
        tsHeight = topEdge - tsTop;
                        
        /* enforce constraints on left */
        if (tsHeight < 0) {
            tsHeight = 0;
        }
        if (tsHeight < topSide.jxLayout.options.minHeight) {
            tsHeight = topSide.jxLayout.options.minHeight;
        }
        if (topSide.jxLayout.options.maxHeight >= 0 && 
            tsHeight > topSide.jxLayout.options.maxHeight) {
            tsHeight = topSide.jxLayout.options.maxHeight;
        }
        
        /* update the topEdge to accomodate constraints */
        if (tsTop + tsHeight != topEdge) {
            /* need to update right side, ignoring constraints because left side
               constraints take precedence (arbitrary decision)
             */
            topEdge = tsTop + tsHeight;
            var delta = topEdge + barSize.height - bsTop;
            bsTop += delta;
            bsHeight -= delta; 
        }
        
        /* put bar in its final location based on constraints */
        obj.element.style.top = topEdge + 'px';
        
        /* update topSide positions */
        if (topSide.jxLayout.options.height == null) {
            topSide.jxLayout.resize({bottom: parentSize.height - tsTop-tsHeight});
        } else {
            topSide.jxLayout.resize({height: tsHeight});
        }
        
        /* update bottomSide position */
        if (bottomSide.jxLayout.options.height == null) {
            bottomSide.jxLayout.resize({top:bsTop});
        } else {
            bottomSide.jxLayout.resize({top: bsTop, height: bsHeight});
        }
    },
    
    /**
     * Method: sizeChanged
     * handle the size of the container being changed.
     */
    sizeChanged: function() {
        if (this.layout == 'horizontal') {
            this.horizontalResize();
        } else {
            this.verticalResize();
        }
    },
    
    /**
     * Method: horizontalResize
     * Resize a horizontally layed-out container
     */
    horizontalResize: function() {
        var availableSpace = Element.getContentBoxSize(this.domObj).width;
        var overallWidth = availableSpace;

        for (var i=0; i<this.bars.length; i++) {
            var bar = this.bars[i];
            if (!bar.size) {
                bar.size = Element.getBorderBoxSize(bar);
            }
            availableSpace -= bar.size.width;
        }

        var nVariable = 0;
        var jxo;
        for (var i=0; i<this.elements.length; i++) {
            var e = this.elements[i];
            jxo = e.jxLayout.options;
            if (jxo.width != null) {
                availableSpace -= parseInt(jxo.width);
            } else {
                var w = 0;
                if (jxo.right != 0 || 
                    jxo.left != 0) {
                    w = Element.getBorderBoxSize(e).width;
                }
                
                availableSpace -= w;
                nVariable++;
            }
        }

        if (nVariable == 0) { /* all fixed */
            /* stick all available space in the last one */
            availableSpace += jxo.width;
            jxo.width = null;
            nVariable = 1;
        }

        var amount = parseInt(availableSpace / nVariable);
        /* account for rounding errors */
        var remainder = availableSpace % nVariable;

        var currentPosition = 0;

        for (var i=0; i<this.elements.length; i++) {
             var e = this.elements[i];
             var jxl = e.jxLayout;
             var jxo = jxl.options;
             if (jxo.width != null) {
                 jxl.resize({left: currentPosition});
                 currentPosition += jxo.width;
             } else {
                 var a = amount;
                 if (nVariable == 1) {
                     a += remainder;
                 }
                 nVariable--;
                 
                 var w = 0;
                 if (jxo.right != 0 || jxo.left != 0) {
                     w = Element.getBorderBoxSize(e).width + a;
                 } else {
                     w = a;
                 }
                 
                 if (w < 0) {
                     if (nVariable > 0) {
                         amount = amount + w/nVariable;
                     }
                     w = 0;
                 }
                 if (w < jxo.minWidth) {
                     if (nVariable > 0) {
                         amount = amount + (w - jxo.minWidth)/nVariable;
                     }
                     w = jxo.minWidth;
                 }
                 if (jxo.maxWidth >= 0 && w > jxo.maxWidth) {
                     if (nVariable > 0) {
                         amount = amount + (w - jxo.maxWidth)/nVariable;
                     }
                     w = e.options.maxWidth;
                 }
                 
                 var r = overallWidth - currentPosition - w;
                 jxl.resize({left: currentPosition, right: r});
                 currentPosition += w;
             }
             if (e.rightBar) {
                 e.rightBar.style.left = currentPosition + 'px';
                 currentPosition += e.rightBar.size.width;
             }
         }
    },
    
    /**
     * Method: verticalResize
     * Resize a vertically layed out container.
     */
    verticalResize: function() { 
        var availableSpace = Element.getContentBoxSize(this.domObj).height;
        var overallHeight = availableSpace;

        for (var i=0; i<this.bars.length; i++) {
            var bar = this.bars[i];
            if (!bar.size) {
                bar.size = Element.getBorderBoxSize(bar);
            }
            availableSpace -= bar.size.height;
        }

        var nVariable = 0;
        
        var jxo;
        for (var i=0; i<this.elements.length; i++) {
            var e = this.elements[i];
            jxo = e.jxLayout.options;
            if (jxo.height != null) {
                availableSpace -= parseInt(jxo.height);
            } else {
                var h = 0;
                if (jxo.bottom != 0 || jxo.top != 0) {
                    h = Element.getBorderBoxSize(e).height;
                }
                
                availableSpace -= h;
                nVariable++;
            }
        }

        if (nVariable == 0) { /* all fixed */
            /* stick all available space in the last one */
            availableSpace += jxo.height;
            jxo.height = null;
            nVariable = 1;
        }

        var amount = parseInt(availableSpace / nVariable);
        /* account for rounding errors */
        var remainder = availableSpace % nVariable;

        var currentPosition = 0;

        for (var i=0; i<this.elements.length; i++) {
             var e = this.elements[i];
             var jxl = e.jxLayout;
             var jxo = jxl.options;
             if (jxo.height != null) {
                 jxl.resize({top: currentPosition});
                 currentPosition += jxo.height;
             } else {
                 var a = amount;
                 if (nVariable == 1) {
                     a += remainder;
                 }
                 nVariable--;
                 
                 var h = 0;
                 if (jxo.bottom != 0 || 
                     jxo.top != 0) {
                     h = Element.getBorderBoxSize(e).height + a;
                 } else {
                     h = a;
                 }
                 
                 if (h < 0) {
                     if (nVariable > 0) {
                         amount = amount + h/nVariable;
                     }
                     h = 0;
                 }
                 if (h < jxo.minHeight) {
                     if (nVariable > 0) {
                         amount = amount + (h - jxo.minHeight)/nVariable;
                     }
                     h = jxo.minHeight;
                 }
                 if (jxo.maxHeight >= 0 && h > jxo.maxHeight) {
                     if (nVariable > 0) {
                         amount = amount + (h - jxo.maxHeight)/nVariable;
                     }
                     h = jxo.maxHeight;
                 }
                 
                 var r = overallHeight - currentPosition - h;
                 jxl.resize({top: currentPosition, bottom: r});
                 currentPosition += h;
             }
             if (e.rightBar) {
                 e.rightBar.style.top = currentPosition + 'px';
                 currentPosition += e.rightBar.size.height;
             }
         }
    }
};

/**
 * Class: Jx.Splitter.Snapper
 * A helper class to create an element that can snap a split panel open or
 * closed.
 */
Jx.Splitter.Snapper = Class.create();
Jx.Splitter.Snapper.prototype = {
    /**
     * Property: snapper
     * {HTMLElement} the DOM element of the snapper (the thing that gets
     * clicked).
     */
    snapper: null,
    /**
     * Property: element
     * {HTMLElement} An element of the <Jx.Splitter> that gets controlled
     * by this snapper
     */
    element: null,
    /**
     * Property: splitter
     * {<Jx.Splitter>} the splitter that this snapper is associated with.
     */
    splitter: null,
    /**
     * Property: layout
     * {String} track the layout of the splitter for convenience.
     */
    layout: 'vertical',
    /**
     * Constructor: Jx.Splitter.Snapper
     * Create a new Jx.Splitter.Snapper
     *
     * Parameters:
     * snapper - {HTMLElement} the clickable thing that snaps the element
     *           open and closed
     * element - {HTMLElement} the element that gets controlled by the snapper
     * splitter - {<Jx.Splitter>} the splitter that this all happens inside of.
     */
    initialize: function(snapper, element, splitter) {
        this.snapper = snapper;
        this.element = element;
        element.jxLayout.addSizeChangeListener(this);
        this.splitter = splitter;
        this.layout = splitter.layout; 
        var jxo = this.element.jxLayout.options;
        var size = Element.getBorderBoxSize(this.element);
        if (this.layout == 'vertical') {
            this.originalSize = size.height;
            this.minimumSize = jxo.minHeight ? jxo.minHeight : 0;
        } else {
            this.originalSize = size.width;
            this.minimumSize = jxo.minWidth ? jxo.minWidth : 0;
        }
        
        Event.observe(snapper, 'click', this.toggleElement.bind(this));
    },
    
    /**
     * Method: toggleElement
     * Snap the element open or closed.
     */
    toggleElement: function() {
        var size = Element.getBorderBoxSize(this.element);
        var newSize = {};
        if (this.layout == 'vertical') {
            if (size.height == this.minimumSize) {
                newSize.height = this.originalSize;
            } else {
                this.originalSize = size.height;
                newSize.height = this.minimumSize;
            }
        } else {
            if (size.width == this.minimumSize) {
                newSize.width = this.originalSize;
            } else {
                this.originalSize = size.width;
                newSize.width = this.minimumSize;
            }
        }
        this.element.jxLayout.resize(newSize);
        this.splitter.sizeChanged();
    },
    
    /**
     * Method: sizeChanged
     * Handle the size of the element changing to see if the
     * toggle state has changed.
     */
    sizeChanged: function() {
        var size = Element.getBorderBoxSize(this.element);
        if (this.layout == 'vertical') {
            if (size.height == this.minimumSize) {
                Element.addClassName(this.snapper, 'jxSnapClosed');
                Element.removeClassName(this.snapper, 'jxSnapOpened');
            } else {
                Element.addClassName(this.snapper, 'jxSnapOpened');
                Element.removeClassName(this.snapper, 'jxSnapClosed');
            }
        } else {
            if (size.width == this.minimumSize) {
                Element.addClassName(this.snapper, 'jxSnapClosed');
                Element.removeClassName(this.snapper, 'jxSnapOpened');
            } else {
                Element.addClassName(this.snapper, 'jxSnapOpened');
                Element.removeClassName(this.snapper, 'jxSnapClosed');
            }
        }
    }
};/**
 * $Id: jxtab.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx.Tab
 *
 * Purpose: 
 * Implementation of a javascript tabbed panel capability for Jx.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
Jx.addStyleSheet('tab/tabs.css');
Jx.addStyleSheet('tabs/tabs_ie.css', true);

/**
 * Class: Jx.TabSet
 * A TabSet manages a set of <Jx.Tab> content areas by ensuring that only one
 * of the content areas is visible (i.e. the active tab).  TabSet does not
 * manage the actual tabs.  The instances of <Jx.Tab> that are to be managed
 * as a set have to be added to both a TabSet and a <Jx.Toolbar>.  The content
 * areas of the <Jx.Tab>s are sized to fit the content area that the TabSet
 * is managing.
 *
 * (code)
 * var tabBar = new Jx.Toolbar('tabBar');
 * var tabSet = new Jx.TabSet('tabArea');
 * 
 * var tab1 = new Jx.Tab('tab 1', {contentID: 'content1'});
 * var tab2 = new Jx.Tab('tab 2', {contentID: 'content2'});
 * var tab3 = new Jx.Tab('tab 3', {contentID: 'content3'});
 * var tab4 = new Jx.Tab('tab 4', {contentURL: 'test_content.html'});
 * 
 * tabSet.add(t1, t2, t3, t4);
 * tabBar.add(t1, t2, t3, t4);
 * (end)
 *
 * Inherits From:
 * <Jx.Listener>
 */
Jx.TabSet = Class.create();
Jx.TabSet.prototype = {
    /**
     * Property: domObj
     * {HTMLElement} The HTML element that represents this tab set in the DOM.
     * The content areas of each tab are sized to fill the domObj.
     */
    domObj : null,
    /**
     * Property: sl
     * {Array} array of selection listeners.
     */
    sl: null,
    /**
     * Constructor: Jx.TabSet
     * Create a new instance of <Jx.TabSet> within a specific element of
     * the DOM.
     *
     * Parameters:
     * domObj - {HTMLElement} an element or id of an element to put the
     * content of the tabs into.
     */
    initialize : function(domObj) {
        this.domObj = $(domObj);
        if (!Element.hasClassName(this.domObj, 'jxTabSetContainer')) {
            Element.addClassName(this.domObj, 'jxTabSetContainer');
        }
        this.sl = [];
    },
    /**
     * Method: sizeChanged
     * Respond to the size of the container changing.
     *
     * TODO: is this needed?
     */
    sizeChanged: function() { 
      this.resizeTabBox(); 
    },
    /**
     * Method: resizeTabBox
     * Resize the tab set content area and propogate the changes to
     * each of the tabs managed by the tab set.
     */
    resizeTabBox: function() {
    
        var parentSize = Element.getContentBoxSize(this.domObj.parentNode);
        Element.setBorderBoxSize(this.domObj, {width: parentSize.width, height: parentSize.height});
        // this is a bullshit hack for IE.  We need to set the tab content height
        // for IE when the tabs are in a snap panel, otherwise the tab content
        // doesn't collapse with the panel and no scrollbars appear.  This only
        // affects the height.  In fact, setting the width breaks tab placement
        
        for (var i=0; i<this.domObj.childNodes.length; i++) {
            // don't try to set the height on a text node 
            if (this.domObj.childNodes[i].nodeType == 3) {
                 continue;
             }
            Element.setBorderBoxSize(this.domObj.childNodes[i], {height: parentSize.height});
            if (this.domObj.childNodes[i].resize) {
                this.domObj.childNodes[i].resize({forceResize: true});
            }
        }
    },
    
    /**
     * Method: add
     * Add one or more <Jx.Tab>s to the TabSet.
     *
     * Parameters:
     * tab - {<Jx.Tab>} an instance of <Jx.Tab> to add to the tab set.  More
     * than one tab can be added by passing extra parameters to this method.
     */
    add : function() {
        for (var i=0; i<arguments.length; i++) {
            var tab = arguments[i];
            tab.addSelectionListener(this);
            this.domObj.appendChild(tab.content);
            if (!this.activeTab) {
                this.setActiveTab(tab);
            }
        }
    },
    /**
     * Method: remove
     * Remove a tab from the TabSet.
     *
     * Parameters:
     * tab - {<Jx.Tab>} the tab to remove.
     *
     * TODO: Implement remove for Jx.TabSet
     */
    remove : function(tab) {},
    /**
     * Method: setActiveTab
     * Set the active tab to the one passed to this method
     *
     * Parameters:
     * tab - {<Jx.Tab>} the tab to make active.
     */
    setActiveTab: function(tab) {
        if (this.activeTab) {
            Element.removeClassName(this.activeTab.domObj, 'tabActive');
            Element.removeClassName(this.activeTab.content, 'tabContentActive');
        }
        this.activeTab = tab;
        Element.addClassName(this.activeTab.domObj, 'tabActive');
        Element.addClassName(this.activeTab.content, 'tabContentActive');
        if (this.activeTab.content.resize) {
          this.activeTab.content.resize({forceResize: true});
        }
    },
    /**
     * Method: selectionChanged
     * Handle selection changing on the tabs themselves and activate the
     * appropriate tab in response.
     *
     * Parameters:
     * tab - {<Jx.Tab>} the tab to make active.
     */
    selectionChanged: function(tab) {
        this.setActiveTab(tab);
        this.processEvent(this.sl, 'selectionChanged', tab);
    },
    /**
     * Method: addSelectionListener
     * Add a selection listener
     *
     * Parameters:
     * obj - {Object} the selection listener to add
     */
    addSelectionListener: function(obj) { this.addListener(this.sl, obj); },
    /**
     * Method: removeSelectionListener
     * Remove a selection listener
     *
     * Parameters:
     * obj - {Object} the selection listener to remove
     */
    removeSelectionListener: function(obj) { this.removeListener(this.sl, obj); }
};
Object.extend(Jx.TabSet.prototype, Jx.Listener.prototype);

/**
 * Class: Jx.Tab
 * A single tab in a tab set.  A tab has a label (displayed in the tab) and a
 * content area that is displayed when the tab is active.  A tab has to be
 * added to both a <Jx.TabSet> (for the content) and <Jx.Toolbar> (for the
 * actual tab itself) in order to be useful.  Alternately, you can use
 * a <Jx.TabBox> which combines both into a single control at the cost of
 * some flexibility in layout options.
 *
 * A tab is a <Jx.ContentLoader> and you can specify the initial content of
 * the tab using any of the methods supported by 
 * <Jx.ContentLoader::loadContent>.  You can acccess the actual DOM element
 * that contains the content (if you want to dynamically insert content
 * for instance) via the <Jx.Tab::content> property.
 *
 * (code)
 * var tab1 = new Jx.Tab('tab 1', {contentID: 'content1'});
 * (end)
 *
 * Inherits From:
 * <Jx.Listener>, <Jx.ContentLoader>
 */
Jx.Tab = Class.create();
Jx.Tab.prototype = {
    /**
     * Property: domObj
     * {HTMLElement} The button that is used to activate the tab.
     */
    domObj: null,
    /**
     * Property: content
     * {HTMLElement} The content area that is displayed when the tab is active.
     */
    content: null,
    /**
     * Property: name
     * {String} the name of the tab is also the label.
     */
    name: null,
    /**
     * Property: sl
     * {Array} an array of selection listeners.
     */
    sl: null,
    /**
     * Constructor: Jx.Tab
     * Create a new instance of Jx.Tab.
     *
     * Parameters:
     * name - {String} the name (and label) of the tab.
     * options - {Object} an object containing options that are used
     * to control the appearance of the tab.  See 
     * <Jx.ContentLoader::loadContent> and <Jx.Layout::Jx.Layout> for
     * valid options.
     */
    initialize : function(name, options) {
        this.sl = [];
        options = options || {};
        this.name = name;
        this.content = document.createElement('div');
        this.content.className = 'tabContent';
        this.loadContent(this.content, options);
        var a = new Jx.Action(this.clicked.bind(this));
        var b = new Jx.Button(a, {label: name});
        this.domObj = b.domA;
        // rename the element from jxButton to jxTab
        // Element.removeClassName(this.domObj, 'jxButton');
        Element.addClassName(this.domObj, 'jxTab');
        new Jx.Layout(this.content, options);
        //this.content.resize = this.resize.bind(this);
    },
    /**
     * Method: clicked
     * Handle the tab being clicked by generating a selectionChanged
     * event.
     */
    clicked: function() {
        this.processEvent(this.sl, 'selectionChanged', this);
        this.domObj.childNodes[0].blur();
    },
    /**
     * Method: addSelectionListener
     * Add a selection listener
     *
     * Parameters:
     * obj - {Object} the selection listener to add
     */
    addSelectionListener: function(obj) { this.addListener(this.sl, obj); },
    /**
     * Method: removeSelectionListener
     * Remove a selection listener
     *
     * Parameters:
     * obj - {Object} the selection listener to remove
     */
    removeSelectionListener: function(obj) { this.removeListener(this.sl, obj); }
};
Object.extend(Jx.Tab.prototype, Jx.Listener.prototype);
Object.extend(Jx.Tab.prototype, Jx.ContentLoader.prototype);

/**
 * Class: Jx.TabBox
 * A convenience class to handle the common case of a single toolbar
 * directly attached to the content area of the tabs.  It manages both a
 * <Jx.Toolbar> and a <Jx.TabSet> so that you don't have to.  If you are using
 * a TabBox, then tabs only have to be added to the TabBox rather than to
 * both a <Jx.TabSet> and a <Jx.Toolbar>.
 *
 * (code)
 * var tabBox = new Jx.TabBox('subTabArea', 'top');
 * 
 * var tab1 = new Jx.Tab('Tab 1', {contentID: 'content4'});
 * var tab2 = new Jx.Tab('Tab 2', {contentID: 'content5'});
 * 
 * tabBox.add(tab1, tab2);
 * (end)
 *
 * Inherits From:
 * <Jx.Listener>
 */
Jx.TabBox = Class.create();
Jx.TabBox.prototype = {
    /**
     * Property: tabBar
     * {<Jx.Toolbar>} the toolbar for this tab box.
     */
    tabBar: null,
    /**
     * Property: tabSet
     * {<Jx.TabSet>} the tab set for this tab box.
     */
    tabSet: null,
    /**
     * Constructor: Jx.TabBox
     */
    initialize : function(domObj, position) {
        var parent = $(domObj);
        position = position || 'top';
        var tabBarDiv = document.createElement('div');
        parent.appendChild(tabBarDiv);
        this.tabBar = new Jx.Toolbar(tabBarDiv, position);
        this.tabSet = new Jx.TabSet(parent);
        switch (position) {
            case 'top':
                Element.addClassName(parent, 'jxTabBoxTop');
                break;
            case 'bottom':
                Element.addClassName(parent, 'jxTabBoxBottom');
                break;
            case 'left':
                Element.addClassName(parent, 'jxTabBoxLeft');
                Element.addClassName(tabBarDiv, 'verticalToolbar');
                break;
            case 'right':
                Element.addClassName(parent, 'jxTabBoxRight');
                Element.addClassName(tabBarDiv, 'verticalToolbar');
                break;
        }
        this.sl = [];
    },
    /**
     * Method: sizeChanged
     * Handle the size of the content area of the tab box changing.
     *
     * TODO: is this needed?
     */
    sizeChanged: function() { this.tabSet.sizeChanged(); },
    
    /**
     * Method: add
     * Add one or more <Jx.Tab>s to the TabBox.
     *
     * Parameters:
     * tab - {<Jx.Tab>} an instance of <Jx.Tab> to add to the tab box.  More
     * than one tab can be added by passing extra parameters to this method.
     * Unlike <Jx.TabSet>, tabs do not have to be added to a separate 
     * <Jx.Toolbar>.
     */
    add : function() { 
        this.tabBar.add.apply(this.tabBar, arguments); 
        this.tabSet.add.apply(this.tabSet, arguments); 
    },
    /**
     * Method: remove
     * Remove a tab from the TabSet.
     *
     * Parameters:
     * tab - {<Jx.Tab>} the tab to remove.
     *
     * TODO: implement remove for Jx.TabBox.
     */
    remove : function(tab) { /* TODO */ }
};
Object.extend(Jx.TabBox.prototype, Jx.Listener.prototype);
/**
 * $Id: jxtoolbar.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx.Toolbar
 *
 * Purpose: 
 * Implementation of a javascript Toolbar for Jx.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
/**
 * import stylesheets
 */
Jx.addStyleSheet('toolbar/toolbar.css');
Jx.addStyleSheet('button/button.css');

/**
 * Class: Jx.Toolbar
 *
 * A toolbar is a container object that contains other objects such as
 * buttons.  The toolbar organizes the objects it contains automatically,
 * wrapping them as necessary.  Multiple toolbars may be placed within
 * the same containing object.
 *
 * Jx.Toolbar includes CSS classes for styling the appearance of a
 * toolbar to be similar to traditional desktop application toolbars.
 *
 * There is one special object, Jx.ToolbarSeparator, that provides
 * a visual separation between objects in a toolbar.
 *
 * The following example shows how to create a Jx.Toolbar instance and place two objects in it.
 * (code)
 * //myToolbarContainer is the id of a <div> in the HTML page.
 * function myFunction() {}
 * var myToolbar = new Jx.Toolbar('myToolbarContainer');
 * 
 * var myAction = new Jx.Action(myFunction):
 * var myButton = new Jx.Button(myAction);
 *
 * var myElement = document.createElement('select');
 *
 * myToolbar.add(myButton, new Jx.ToolbarSeparator(), myElement);
 * (end)
 *
 * While a toolbar is generally a *dumb* container, it serves a special purpose
 * for menus by providing some infrastructure so that menus can behave
 * properly.
 *
 * In general, almost anything can be placed in a Toolbar, and mixed with 
 * anything else.
 */
Jx.Toolbar = Class.create();
Jx.Toolbar.prototype = {
    /**
     * Property: items
     * {Array} an array of the things in the toolbar.
     */
    items : null,
    /**
     * Property: domObj
     * {HTMLElement} the HTML element that the toolbar lives in
     */
    domObj : null,
    /**
     * Property: isActive
     * When a toolbar contains <Jx.Menu> instances, they want to know
     * if any menu in the toolbar is active and this is how they
     * find out.
     */
    isActive : false,
    /**
     * Constructor: Jx.Toolbar
     * Create a new instance of Jx.Toolbar.
     *
     * Parameters:
     * domObj - {HTMLElement} object reference or id to place the toolbar in.
     * position - one of 'top', 'right', 'bottom', or 'left', indicates how
     * the toolbar is being placed in the page and may influence the behaviour
     * of items in the toolbar that open sub panels, they will tend to open
     * them towards the center of the page.  Default is top.
     */
    initialize : function(domObj, position) {
        var parent = $(domObj);
        this.domObj = document.createElement('ul');
        Element.addClassName(this.domObj,'jxToolbar');
        
        if (!Element.hasClassName(parent, 'jxToolbarContainer')) {
            Element.addClassName(parent, 'jxToolbarContainer');
            parent.appendChild(this.domObj);
            var clearer = document.createElement('div');
            clearer.className = 'jxClearer';
            parent.appendChild(clearer);                    
        } else {
            parent.insertBefore(this.domObj, parent.lastChild);
        }
        switch (position) {
            case 'top':
                Element.addClassName(parent, 'jxBarTop');
                break;
            case 'right':
                Element.addClassName(parent, 'jxBarRight');
                break;
            case 'bottom':
                Element.addClassName(parent, 'jxBarBottom');
                break;
            case 'left':
                Element.addClassName(parent, 'jxBarLeft');
                break;
            default:
                Element.addClassName(parent, 'jxBarTop');
        }
        this.deactivateWatcher = this.deactivate.bindAsEventListener(this);
        
    },
    /**
     * Method: add
     * Add an item to the toolbar.  If the item being added is a Jx component
     * with a domObj property, the domObj is added.  If the item being added
     * is an LI element, then it is given a CSS class of *jxToolItem*.
     * Otherwise, the thing is wrapped in a <Jx.ToolbarItem>.
     *
     * Parameters:
     * thing - {Object} the thing to add.  More than one thing can be added
     * by passing multiple arguments.
     */
    add : function( ) {
        for (var i=0; i<arguments.length; i++) {
            var thing = arguments[i];
            thing.toolbar = this;
            if (thing.domObj) {
                thing = thing.domObj;
            }
            if (thing.tagName == 'LI') {
                if (!Element.hasClassName(thing, 'jxToolItem')) {
                    Element.addClassName(thing, 'jxToolItem');
                }
                this.domObj.appendChild(thing);
            } else {
                var item = new Jx.ToolbarItem(thing);
                this.domObj.appendChild(item.domObj);
            }
        }
    },
    /**
     * Method: deactivate
     * Deactivate the Toolbar (when it is acting as a menu bar).
     */
    deactivate: function() {
        for (var i=0; i<this.items.length; i++) {
            this.items[i].hide();
        }
        this.setActive(false);
    },
    /**
     * Method: isActive
     * Indicate if the toolbar is currently active (as a menu bar)
     *
     * Returns:
     * {Boolean}
     */
    isActive: function() { 
        return this.isActive; 
    },
    /**
     * Method: setActive
     * Set the active state of the toolbar (for menus)
     *
     * Parameters: 
     * b - {Boolean} the new state
     */
    setActive: function(b) { 
        this.isActive = b;
        if (this.isActive) {
            Event.observe(document, 'click', this.deactivateWatcher);
        } else {
            Event.stopObserving(document, 'click', this.deactivateWatcher);
        }
    },
    /**
     * Method: setVisibleItem
     * For menus, they want to know which menu is currently open.
     *
     * Parameters:
     * obj - {<Jx.Menu>} the menu that just opened.
     */
    setVisibleItem: function(obj) {
        if (this.visibleItem && this.visibleItem.hide && this.visibleItem != obj) {
            this.visibleItem.hide();
        }
        this.visibleItem = obj;
        if (this.isActive()) {
            this.visibleItem.show();
        }
    }
};

/**
 * Class: Jx.ToolbarItem
 * A helper class to provide a container for something to go into 
 * a <Jx.Toolbar>.
 */
Jx.ToolbarItem = Class.create();
Jx.ToolbarItem.prototype = {
    /**
     * Property: domObj
     * {HTMLElement} an element to contain the thing to be placed in the
     * toolbar.
     */
    domObj: null,
    /**
     * Constructor: Jx.ToolbarItem
     * Create a new instance of Jx.ToolbarItem.
     *
     * Parameters:
     * jxThing - {Object} the thing to be contained.
     */
    initialize : function( jxThing ) {
        this.al = [];
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxToolItem';
        if (jxThing) {
            if (jxThing.domObj) {
                this.domObj.appendChild(jxThing.domObj);
                if (jxThing instanceof Jx.Tab) {
                    // this.domObj.className = 'jxTabItem';
                    Element.addClassName(this.domObj, 'jxTabItem');
                }
            } else {
                this.domObj.appendChild(jxThing);
                if (Element.hasClassName(jxThing, 'jxTab')) {
                    // this.domObj.className = 'jxTabItem';
                    Element.addClassName(this.domObj, 'jxTabItem');
                }
            }
        }
    }
};

/**
 * Class: Jx.ToolbarSeparator
 * A helper class that represents a visual separator in a <Jx.Toolbar>
 */
Jx.ToolbarSeparator = Class.create();
Jx.ToolbarSeparator.prototype = {
    /**
     * Property: domObj
     * {HTMLElement} The DOM element that goes in the <Jx.Toolbar>
     */
    domObj: null,
    /**
     * Constructor: Jx.ToolbarSeparator
     * Create a new Jx.ToolbarSeparator
     */
    initialize: function() {
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxToolItem';
        this.domSpan = document.createElement('span');
        this.domSpan.className = 'separator';
        this.domObj.appendChild(this.domSpan);
    }
};

/**
 * $Id: jxtree.js 512 2008-03-07 21:15:45Z pspencer $
 *
 * Title: Jx.Tree
 *
 * Purpose: 
 * Implementation of a javascript Tree for Jx.
 *
 * Author: 
 * Paul Spencer (pspencer@dmsolutions.ca)
 *
 * Copyright:
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

Jx.addStyleSheet('tree/tree.css');
Jx.addStyleSheet('tree/tree_ie.css', true);

/**
 * Class: Jx.TreeItem 
 * An item in a tree.  An item is a leaf node that has no children.
 *
 * Jx.TreeItem supports selection listeners.  When an item in the tree
 * is selected by the user, listeners are called by invoking the
 * selectionChanged method on the listener and passing the tree item
 * object that was selected.  The application is responsible for
 * changing the style of the selected item in the tree and for
 * tracking selection if that is important.
 * 
 * When the selection changes, the mouse event that triggered the
 * selection change is passed to the listener as a lastEvent
 * member of the tree item object.  You can use this to determine
 * if the item was clicked, double-clicked, right clicked etc.
 *
 * Inherits From:
 * <Jx.Listener>
 */
Jx.TreeItem = Class.create();
Jx.TreeItem.prototype = {
    /**
     * Property: data
     * {Object} arbitrary data associated with the TreeItem
     */
    data : null,
    /**
     * Property: domObj
     * {HTMLElement} a reference to the HTML element that is the TreeItem
     * in the DOM
     */
    domObj : null,
    /**
     * Property: parent
     * {Object} the folder or tree that this item belongs to
     */
    parent : null,
    /**
     * Property: currentClassName
     * {String} the current CSS class name for this TreeItem.
     *
     * TODO: Property currentClassName
     * This property does not appear to be used?
     */
    currentClassName : null,
    /**
     * Property: onClick
     * {Function} callback function to invoke with the TreeItem is clicked
     */
    onClick : null,
    /**
     * Property: sl
     * {Array} an array of selection listeners
     */
    sl : null,
    /**
     * Property: enabled
     * {Boolean} is the TreeItem enabled or not?
     */
    enabled : null,
    /**
     * Property: contextMenu
     *
     * {<Jx.ContextMenu>} an optional context menu for the TreeItem
     */
    contextMenu : null,
    /**
     * Constructor: Jx.TreeItem
     * Create a new instance of Jx.TreeItem with the associated options
     *
     * Parameters:
     * options - {Object} an object containing the below optional
     * attributes that control how the TreeItem functions.
     *
     * Options:
     * label - {String} the label to display for the TreeItem
     * data - {Object} any arbitrary data to be associated with the TreeItem
     * contextMenu - {<Jx.ContextMenu>} the context menu to trigger if there
     *      is a right click on the node
     * imgNode - {String} URL to an image to use for the node of this 
     *      TreeItem.  Not sure if this should ever be set.
     * imgIcon - {String} URL to an image to use as the icon next to the
     *      label of this TreeItem
     * enabled - {Boolean} the initial state of the TreeItem.  If the 
     *      TreeItem is not enabled, it cannot be clicked.
     */
    initialize : function( options ) {
        this.initializeItem(options);
    },
    /**
     * Method: initializeItem
     * internal method to initialize the TreeItem.
     *
     * Parameters: 
     * options - See <Jx.TreeItem::Jx.TreeItem>
     */
    initializeItem: function(options) {
        options = options || {};
        this.sl = [];
        var label = options.label || 'new node';
        this.data = options.data || null;
        this.contextMenu = options.contextMenu || null;
        
        this.imgNode = options.imgNode || Jx.baseURL + 'images/tree_none.png';
        this.imgIcon = options.imgIcon || Jx.baseURL + 'images/tree_page.png';
                
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxTreeNode';
        this.currentClassName = 'jxTreeNode';
      
        var elm = document.createElement('img');
        elm.className = 'jxTreeNodeImage';
        //elm.src = this.imgNode;
        Jx.addToImgQueue({domElement: elm, src: this.imgNode});
        this.domObj.appendChild(elm);
        
        elm = document.createElement('img');
        elm.className = 'jxTreeNodeIcon';
        //elm.src = this.imgIcon;
        Jx.addToImgQueue({domElement: elm, src: this.imgIcon});
        this.domObj.appendChild(elm);
      
        elm = document.createElement('a');
        elm.href = 'javascript:void(0)';
        elm.onclick = this.selected.bindAsEventListener(this);
        elm.ondblclick = this.selected.bindAsEventListener(this);
        elm.oncontextmenu = this.showMenu.bindAsEventListener(this);
        elm.innerHTML = label;
        this.domObj.appendChild(elm);
        
        //this.update(false);
        //TODO: this could cause a memory leak in remove
        this.domObj.jxTreeItem = this;
        this.domObj.childNodes[2].jxTreeItem = this;
        
        this.onClick = options.onClick || null;
        this.enabled = typeof options.enabled != 'undefined' ? options.enabled : true;
        if (this.enabled) {
            Element.removeClassName(this.domObj, 'jxDisabled');
        } else {
            Element.addClassName(this.domObj, 'jxDisabled');
        }
        var clearer = document.createElement('span');
        clearer.className = 'jxClearer';
        this.domObj.appendChild(clearer);
    },
    /**
     * Method: finalize
     * Clean up the TreeItem and remove all DOM references
     */
    finalize: function() { this.finalizeItem(); },
    /**
     * Method: finalizeItem
     * Clean up the TreeItem and remove all DOM references
     */
    finalizeItem: function() {  
        if (!this.domObj) {
            return;
        }
        this.domObj.childNodes[2].onclick = null;
        this.domObj.childNodes[2].ondblclick = null;
        this.domObj.childNodes[2].oncontextmenu = null;
        this.contextMenu = null;
        this.onClick = null;
        for (var i=this.sl.length; i>=0; i--) {
            this.removeSelectionListener(this.sl[i]);
        }
        this.parent = null;
        this.domObj.jxTreeItem = null;
        this.domObj = null;
    },
    /**
     * Method: clone
     * Create a clone of the TreeItem
     * 
     * Returns: 
     * {<Jx.TreeItem>} a copy of the TreeItem
     */
    clone : function() {
        var options = { label : this.domObj.childNodes[2].innerHTML, 
                        data : this.data,
                        onClick : this.onClick,
                        imgNode : this.imgNode,
                        imgIcon : this.imgIcon
                        };
        var item = new Jx.TreeItem(options);
        return item;
    },
    /**
     * Method: update
     * Update the CSS of the TreeItem's DOM element in case it has changed
     * position
     *
     * Parameters:
     * shouldDescend - {Boolean} propagate changes to child nodes?
     */
    update : function(shouldDescend) {
        var isLast = (arguments.length > 1) ? arguments[1] : 
                     (this.parent && this.parent.isLastNode(this));
        if (isLast) {
            Element.removeClassName(this.domObj, 'jxTreeNode');
            Element.addClassName(this.domObj, 'jxTreeNodeLast');
        } else {
            Element.removeClassName(this.domObj, 'jxTreeNodeLast');
            Element.addClassName(this.domObj, 'jxTreeNode');
        }
    },
    /**
     * Method: selected
     * Called when the DOM element for the TreeItem is clicked, the
     * node is selected.
     *
     * Parameters:
     * e - {Event} the DOM event
     */
    selected : function(e) {
        this.lastEvent = e?e:event;
        this.processEvent(this.sl,'selectionChanged',this);
    },
    /**
     * Method: showMenu
     * Called when the DOM element for the TreeItem is right-clicked.  The
     * node is selected and the context menu displayed (if there is one).
     *
     * Parameters:
     * e - {Event} the DOM event
     */
    showMenu: function(e) {
        this.lastEvent = e?e:event;
        this.processEvent(this.sl,'selectionChanged',this);
        if (this.contextMenu) {
            this.contextMenu.show(this.lastEvent);
        }
        Event.stop(e);
    },
    /**
     * Method: addSelectionListener
     * Add a selection listener.
     *
     * Parameters:
     * obj - {Object} the object to add as a selection listener
     */
    addSelectionListener: function(obj){this.addListener(this.sl, obj);},
    /**
     * Method: removeSelectionListener
     * Remove a selection listener.
     *
     * Parameters:
     * obj - {Object} the object to remove as a selection listener
     */
    removeSelectionListener: function(obj) {this.removeListener(this.sl, obj);},
    /**
     * Method: getName
     * Get the label associated with a TreeItem
     *
     * Returns: 
     * {String} the name
     */
    getName : function() { return this.domObj.childNodes[2].innerHTML; },
    /**
     * Method: setName
     * Set the label of a TreeItem
     *
     * Parameters:
     * name - {String} the new label
     */
    setName : function(name) { this.domObj.childNodes[2].innerHTML = name; },
    /**
     * Method: propertyChanged
     * A property of an object has changed, synchronize the state of the 
     * TreeItem with the state of the object
     *
     * Parameters:
     * obj - {Object} the object whose state has changed
     */
    propertyChanged : function(obj) {
        this.enabled = obj.isEnabled();
        if (this.enabled) {
            Element.removeClassName(this.domObj, 'jxDisabled');
        } else {
            Element.addClassName(this.domObj, 'jxDisabled');
        }
    }
};
Object.extend(Jx.TreeItem.prototype, Jx.Listener.prototype);

/**
 * Class: Jx.TreeFolder
 *
 * A Jx.TreeFolder is an item in a tree that can contain other items.  It is
 * expandable and collapsible.
 *
 * Inherits From:
 * <Jx.TreeItem>
 */
Jx.TreeFolder = Class.create();
Object.extend(Jx.TreeFolder.prototype, Jx.TreeItem.prototype);
Object.extend(Jx.TreeFolder.prototype, {
    /**
     * Property: subDomObj
     * {HTMLElement} an HTML container for the things inside the folder
     */
    subDomObj : null,
    /**
     * Property: nodes
     * {Array} an array of references to the javascript objects that are
     * children of this folder
     */
    nodes : null,
    /**
     * Property: isOpen
     * {Boolean} is the folder open or not?
     */
    isOpen : false,
    /**
     * Property: dl
     * {Array} disclosure listeners
     */
    dl : null,
    /**
     * Constructor: Jx.TreeFolder
     * Create a new instance of Jx.TreeFolder
     *
     * Parameters:
     * options - {Object} an object containing any of the options of a
     * <Jx.TreeItem> (see <Jx.TreeItem::Jx.TreeItem>) plus the following
     * optional attributes that control how the TreeFolder functions.
     *
     * Options:
     * imgTreePlus - {String} a URL to an image for opening the folder
     * imgTreeMinus - {String} a URL to an image for closing the folder
     * imgTreeFolder - {String} a URL to an image to represent the folder
     *      when it is closed
     * imgTreeFolderOpen - {String} a URL to an image to represent the folder
     *      when it is open
     */
    initialize : function( options ) {
        this.initializeFolder(options);
    },
    /**
     * Method: initializeFolder
     * Internal method for initializing a folder.
     *
     * Parameters:
     * options - {Object} see <Jx.TreeFolder::Jx.TreeFolder> for options.
     */
    initializeFolder : function(options) {
        options = options || {};
        this.initializeItem(options);
        
        this.imgTreePlus = options.imgTreePlus || Jx.baseURL + 'images/tree_plus.png';
        this.imgTreeMinus = options.imgTreeMinus || Jx.baseURL + 'images/tree_minus.png';
        
        this.imgTreeFolder = options.imgTreeFolder || Jx.baseURL + 'images/tree_folder.png';
        this.imgTreeFolderOpen = options.imgTreeFolderOpen || Jx.baseURL + 'images/tree_folder_open.png';
        
        //console.log('imgTreePlus is ' + this.imgTreePlus);
        //this.domObj.childNodes[0].src = this.imgTreePlus;
        //this.domObj.childNodes[1].src = this.imgTreeFolder;
        Jx.addToImgQueue({domElement: this.domObj.childNodes[0], src: this.imgTreePlus});
        Jx.addToImgQueue({domElement: this.domObj.childNodes[1], src: this.imgTreeFolder});
        
        this.domObj.childNodes[0].onclick = this.clicked.bindAsEventListener(this);
                
        this.nodes = [];
        this.subDomObj = document.createElement('ul');
        this.domObj.appendChild(this.subDomObj);
        this.subDomObj.className = 'jxTree';
        this.isOpen = options.isOpen || false;
        this.dl = [];
        if (this.isOpen) {
            this.expand();
        } else {
            this.collapse();
        }
    //this.makeDroppable();
    },
    /**
     * Method: finalize
     * Clean up a TreeFolder.
     */
    finalize: function() {
        this.finalizeFolder();
        this.finalizeItem();
        this.subDomObj = null;
    },
    /**
     * Method: finalizeFolder
     * Internal method to clean up folder-related stuff.
     */
    finalizeFolder: function() {
        this.domObj.childNodes[0].onclick = null;
        for (var i=this.nodes.length-1; i>=0; i--) {
            this.nodes[i].finalize();
            if (this.nodes[i].domObj) this.subDomObj.removeChild(this.nodes[i].domObj);
            this.nodes.pop();
        }
        for (var i=this.dl.length-1; i>=0; i--) {
            this.removeDisclosureListener(this.dl[i]);
        }
    },
    /*
    makeDroppable : function() {
        if (!document.Jx.TreeCurrentTimeout) document.Jx.TreeCurrentTimeout = null;
        Droppables.add(this.domObj.childNodes[2], {
            greedy : true,
            onHover : function(draggable, droppable, overlap) { 
                var dropped = droppable.parentNode.jxTreeItem;
                if (document.Jx.TreeCurrentTimeout && 
                    document.currentDroppable != droppable) {
                    window.clearTimeout(document.Jx.TreeCurrentTimeout);
                    document.Jx.TreeCurrentTimeout = null;
                }
                if (!document.Jx.TreeCurrentTimeout && 
                    dropped.nodes && 
                    !dropped.isOpen)
                {
                    document.Jx.TreeCurrentTimeout = window.setTimeout(dropped.expand.bind(dropped), 700);
                }
                
                if (!document.currentHighlight != droppable) {
                    if (droppable == draggable)
                    {
                        Droppables.remove(droppable);
                    }
                    if (document.currentDroppable && 
                        document.currentDroppable != droppable) {
                        Element.removeClassName(document.currentDroppable, 
                                                'jxTreeDropzone');
                    }
                    
                    Element.addClassName(droppable, 'jxTreeDropzone');
                    document.currentDroppable = droppable;
                }
            },
            onDrop : function(draggable, droppable) {
                var dragged = draggable.jxTreeItem;
                var dropped = droppable.parentNode.jxTreeItem;

                Element.removeClassName(droppable, 
                                            'jxTreeDropzone');
                if (dropped.nodes) {
                    dragged.parent.remove(dragged);
                    dropped.append(dragged);
                    dropped.expand();
                } else if (dropped.parent != null) {
                    dragged.parent.remove(dragged);
                    dropped.parent.insert(dragged,dropped);
                }
            }
        });
    },
    */
    /**
     * Method: clone
     * Create a clone of the TreeFolder
     * 
     * Returns: 
     * {<Jx.TreeFolder>} a copy of the TreeFolder
     */
    clone : function() {
        var options = { label : this.domObj.childNodes[2].innerHTML, 
                        data : this.data,
                        onClick : this.onClick,
                        imgTreePlus : this.imgTreePlus,
                        imgTreeMinus : this.imgTreeMinus,
                        imgTreeFolder : this.imgTreeFolder,
                        imgTreeFolderOpen : this.imgTreeFolderOpen
                        };
        var node = new Jx.TreeFolder(options);
        for (var i=0;i<this.nodes.length;i++) {
            node.append(this.nodes[i].clone());
        }
        return node;
    },
    /**
     * Method: isLastNode
     * Indicates if a node is the last thing in the folder.
     *
     * Parameters:
     * node - {Jx.TreeItem} the node to check
     *
     * Returns:
     *
     * {Boolean}
     */
    isLastNode : function(node) {
        if (this.nodes.length == 0) {
            return false;
        } else {
            return this.nodes[this.nodes.length-1] == node;
        }
    },
    /**
     * Method: update
     * Update the CSS of the TreeFolder's DOM element in case it has changed
     * position.
     *
     * Parameters:
     * shouldDescend - {Boolean} propagate changes to child nodes?
     */
    update : function(bDescend) {
        /* avoid update if not attached to tree yet */
        if (!this.parent) return;
        var bLast = false;
        if (arguments.length > 1) {
            bLast = arguments[1];
        } else {
            bLast = (this.parent && this.parent.isLastNode(this));
        }
        
        if (bLast) {
            this.domObj.className = 'jxTreeNodeLast';
            this.subDomObj.className = 'jxTree';
        } else {
            this.domObj.className = 'jxTreeNode';
            this.subDomObj.className = 'jxTree jxTreeNest';
        }
        
        if (this.isOpen) {
            //this.domObj.childNodes[0].src = this.imgTreeMinus;
            //this.domObj.childNodes[1].src = this.imgTreeFolderOpen;
            Jx.addToImgQueue({domElement: this.domObj.childNodes[0], src: this.imgTreeMinus});
            Jx.addToImgQueue({domElement: this.domObj.childNodes[1], src: this.imgTreeFolderOpen});
        } else {
            //this.domObj.childNodes[0].src = this.imgTreePlus;
            //this.domObj.childNodes[1].src = this.imgTreeFolder;
            Jx.addToImgQueue({domElement: this.domObj.childNodes[0], src: this.imgTreePlus});
            Jx.addToImgQueue({domElement: this.domObj.childNodes[1], src: this.imgTreeFolder});
        }
        
        if (this.nodes && bDescend) {
            //this.nodes.each(function(n){n.update(true);});
            for(var i=0; i<this.nodes.length; i++) {
                this.nodes[i].update(false, i==this.nodes.length-1);
            }
        }
    },
    /**
     * Method: append
     * append a node at the end of the sub-tree
     *
     * Parameters:
     * node - {Object} the node to append.
     */
    append : function( node ) {
        node.parent = this;
        this.nodes.push(node);
        this.subDomObj.appendChild( node.domObj );
        this.update(true);
    },
    /**
     * Method: insert
     * insert a node after refNode.  If refNode is null, insert at beginning
     *
     * Parameters:
     * node - {Object} the node to insert
     * refNode - {Object} the node to insert before
     */
    insert : function( node, refNode ) {
        node.parent = this;
        //if refNode is not supplied, insert at the beginning.
        if (!refNode) {
            this.nodes.unshift(node);
            //sanity check to make sure there is actually something there
            if (this.subDomObj.childNodes.length ==0) {
                this.subDomObj.appendChild(node.domObj);
            } else {
                this.subDomObj.insertBefore(node.domObj, this.subDomObj.childNodes[0]);                
            }
        } else {
            //walk all nodes looking for the ref node.  Track if it actually
            //happens so we can append if it fails.
            var b = false;
            for(var i=0;i<this.nodes.length;i++) {
                if (this.nodes[i] == refNode) {
                    //increment to append after ref node.  If this pushes us
                    //past the end, it'll get appended below anyway
                    i = i + 1;
                    if (i < this.nodes.length) {
                        this.nodes.splice(i, 0, node);
                        this.subDomObj.insertBefore(node.domObj, this.subDomObj.childNodes[i]);
                        b = true;
                        break;
                    }
                }
            }
            //if the node wasn't inserted, it is because refNode didn't exist
            //and so the fallback is to just append the node.
            if (!b) {
                this.nodes.push(node); 
                this.subDomObj.appendChild(node.domObj); 
            }
        }
        this.update(true);
        //if (this.parent)
        //    this.parent.update(true);        
    },
    /**
     * Method: remove
     * remove the specified node from the tree
     *
     * Parameters:
     * node - {Object} the node to remove
     */
    remove : function(node) {
        node.parent = null;
        for(var i=0;i<this.nodes.length;i++) {
            if (this.nodes[i] == node) {
                this.nodes.splice(i, 1);
                this.subDomObj.removeChild(this.subDomObj.childNodes[i]);
                break;
            }
        }
        this.update(true);
        //if (this.parent)
        //    this.parent.update(true);        
    },
    /**
     * Method: replace
     * Replace a node with another node
     *
     * Parameters:
     * newNode - {Object} the node to put into the tree
     * refNode - {Object} the node to replace
     *
     * Returns:
     * {Boolean} true if the replacement was successful.
     */
    replace: function( newNode, refNode ) {
        //walk all nodes looking for the ref node. 
        var b = false;
        for(var i=0;i<this.nodes.length;i++) {
            if (this.nodes[i] == refNode) {
                if (i < this.nodes.length) {
                    newNode.parent = this;
                    this.nodes.splice(i, 1, newNode);
                    this.subDomObj.replaceChild(newNode.domObj, refNode.domObj);
                    return true;
                }
            }
        }
        return false;
    },
    
    /**
     * Method: clicked
     * handle the user clicking on this folder by expanding or
     * collapsing it.
     *
     * Parameters: 
     * e - {Event} the event object
     */
    clicked : function(e) {
        e = e?e:event;
        if (this.isOpen) {
            this.collapse();
        } else {
            this.expand();
        }
    },
    /**
     * Method: expand
     * Expands the folder
     */
    expand : function() {
        this.isOpen = true;
        this.subDomObj.style.display = 'block';
        this.update(true);
        this.processEvent(this.dl, 'disclosed', this);    
    },
    /**
     * Method: collapse
     * Collapses the folder
     */
    collapse : function() {
        this.isOpen = false;
        this.subDomObj.style.display = 'none';
        this.update(true);
        this.processEvent(this.dl, 'disclosed', this);
    },
    /**
     * Method: addDisclosureListener
     * Add a disclosure (when the folder is opened) listener
     *
     * Parameters:
     * obj - {Object} a discloser listener to add
     */
    addDisclosureListener: function(obj){this.addListener(this.dl, obj);},
    /**
     * Method: removeDisclosureListener
     * Remove a disclosure listener
     *
     * Parameters:
     * obj - {Object} a discloser listener to remove
     */
    removeDisclosureListener: function(obj) {this.removeListener(this.dl, obj);},
    /**
     * Method: findChild
     * Get a reference to a child node by recursively searching the tree
     * 
     * Parameters:
     * path - {Array} an array of labels of nodes to search for
     *
     * Returns:
     * {Object} the node or null if the path was not found
     */
    findChild : function(path) {
        //path is empty - we are asking for this node
        if (path.length == 0)
            return this;
        
        //path has only one thing in it - looking for something in this folder
        if (path.length == 1)
        {
            for (var i=0; i<this.nodes.length; i++)
            {
                if (this.nodes[i].getName() == path[0])
                    return this.nodes[i];
            }
            return null;
        }
        //path has more than one thing in it, find a folder and descend into it    
        var childName = path.shift();
        for (var i=0; i<this.nodes.length; i++)
        {
            if (this.nodes[i].getName() == childName && this.nodes[i].findChild)
                return this.nodes[i].findChild(path);
        }
        return null;
    }
});

/**
 * Class: Jx.Tree
 * Jx.Tree displays hierarchical data in a tree structure of folders and nodes.
 *
 * Inherits From:
 * <Jx.TreeFolder>
 */
Jx.Tree = Class.create();
Object.extend( Jx.Tree.prototype, Jx.TreeFolder.prototype );
Object.extend( Jx.Tree.prototype, {
    /**
     * Constructor: Jx.Tree
     * Create a new instance of Jx.Tree
     *
     * Parameters:
     * id - {String} the id of the DOM element to create the tree inside.
     */
    initialize : function( id ) {
        this.subDomObj = document.createElement('ul');
        this.subDomObj.className = 'jxTreeRoot';
        $(id).appendChild(this.subDomObj);
        this.nodes = [];
        this.dl = [];
        this.isOpen = true;
    },
    /**
     * Method: finalize
     * Clean up a Jx.Tree instance
     */
    finalize: function() { 
        this.clear(); 
        this.subDomObj.parentNode.removeChild(this.subDomObj); 
    },
    /**
     * Method: clear
     * Clear the tree of all child nodes
     */
    clear: function() {
        for (var i=this.nodes.length-1; i>=0; i--) {
            this.subDomObj.removeChild(this.nodes[i].domObj);
            this.nodes[i].finalize();
            this.nodes.pop();
        }
    },
    /**
     * Method: update
     * Update the CSS of the Tree's DOM element in case it has changed
     * position
     *
     * Parameters:
     * shouldDescend - {Boolean} propagate changes to child nodes?
     */
    update: function(shouldDescend) {
        var bLast = true;
        if (this.subDomObj)
        {
            if (bLast) {
                Element.removeClassName(this.subDomObj, 'jxTreeNest');
            } else {
                Element.addClassName(this.subDomObj, 'jxTreeNest');
            }
        }
        if (this.nodes && shouldDescend) {
            this.nodes.each(function(n){n.update(false);});
        }
    },
    /**
     * Method: append
     * Append a node at the end of the sub-tree
     * 
     * Parameters:
     * node - {Object} the node to append.
     */
    append: function( node ) {
        node.parent = this;
        this.nodes.push(node);
        this.subDomObj.appendChild( node.domObj );
        this.update(true);        
    }
});/**
 * this file is to be loaded/included after all other Jx files 
 *
 * Copyright &copy; 2008, DM Solutions Group Inc.
 */
/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/ 

if (Jx.COMBINED_CSS) {
    //console.log('Jx.COMBINED_CSS');
    document.write('<link type="text/css" rel="stylesheet" href="'+Jx.baseURL+'lib/jx_combined.css" />\n');
} else {
    //console.log('no Jx.COMBINED_CSS');
    for (var styleSheet in Jx.importRules) {
        var url = Jx.baseURL+styleSheet;
        document.write('<link type="text/css" rel="stylesheet" href="'+url+'" />\n');
    }
    document.write('<!--[if IE]>');
    for (var styleSheet in Jx.importRulesIE) {
        var url = Jx.baseURL+styleSheet;
        document.write('<link type="text/css" rel="stylesheet" href="'+url+'" />\n');
    }
    document.write('<![endif]-->\n');
}