document.newElement = function(tag, props) {
    var createdElement;
    var temp = tag.split(' ').shift();//error in IE if tag has extra attributes listed
                                      //just strip off the tag name, properties should
                                      //set through the props argument

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
            createdElement = this.createElement(temp);
        }
    }
    else {
        createdElement = this.createElement(tag);
    }

    return document.id(this.createElement(temp)).set(props);
};
