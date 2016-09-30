(function() {
  var AbstractClass = require("./AbstractData");
  var Utils = require("./../Utils");

  function RegisteredView(params) {
    AbstractClass.call(this, {'class': 'RegisteredView'});
    
    this.id = params.id;
    this.workspace = params.workspace;
    this.uri = params.uri;
    this.layers = [];

    this.setLayers(params.Layers || params.layers);
  }

  RegisteredView.prototype = Object.create(AbstractClass.prototype);
  RegisteredView.prototype.constructor = RegisteredView;

  RegisteredView.prototype.setLayers = function(layers) {
    var output = [];
    layers.forEach(function(layer) {
      var obj = layer;
      if (Utils.isFunction(layer.get)) {
        obj = layer.get();
      }
      output.push(obj);
    });
    this.layers = output;
  };

  RegisteredView.prototype.rawObject = function() {
    return this.toObject();
  }

  RegisteredView.prototype.toObject = function() {
    return Object.assign(AbstractClass.prototype.toObject.call(this), {
      id: this.id,
      workspace: this.workspace,
      uri: this.uri,
      layers: this.layers
    });
  };

  module.exports = RegisteredView;
} ());