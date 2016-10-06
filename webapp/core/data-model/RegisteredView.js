(function() {
  'use strict';

  // Dependencies
  var AbstractClass = require("./AbstractData");
  var Utils = require("./../Utils");

  /**
   * @param {Object} params
   * @param {number} params.id
   * @param {string} params.workspace
   * @param {string[]} params.layers
   */
  function RegisteredView(params) {
    AbstractClass.call(this, {'class': 'RegisteredView'});
    
    this.id = params.id;
    this.workspace = params.workspace;
    this.uri = params.uri;
    this.layers = [];

    this.setLayers(params.Layers || params.layers);
  }
  // Javascript Object Inheritance Way
  RegisteredView.prototype = Object.create(AbstractClass.prototype);
  RegisteredView.prototype.constructor = RegisteredView;

  /**
   * It sets layers to cache. 
   * @todo Improve this doc
   * @param {Sequelize.Model[]|Object[]}
   */
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
  /**
   * Get real representation of RegisteredView
   * 
   * @returns {Object}
   */
  RegisteredView.prototype.rawObject = function() {
    return this.toObject();
  }
  /**
   * Retrieves a standard representation used in TCP communication
   * 
   * @returns {Object}
   */
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