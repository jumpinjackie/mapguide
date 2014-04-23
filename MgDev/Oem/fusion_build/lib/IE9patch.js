document.newElement = function(tag, props) {
    var createdElement;
    if (Browser.Engine.trident && props){
        ['name', 'type', 'checked'].each(function(attribute){
            if (!props[attribute]) return;
            tag += ' ' + attribute + '="' + props[attribute] + '"';
            if (attribute != 'checked') delete props[attribute];
        });
  
        var replaceTag = '<' + tag +'>';

        try {
            createdElement = this.createElement(replaceTag);
        }
        catch (e) {
            createdElement = this.createElement(tag);
        }
    }
    else {
        createdElement = this.createElement(tag);
    }

    return document.id(this.createElement(tag)).set(props);
};