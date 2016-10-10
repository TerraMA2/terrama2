(function() {
  'use strict';

  // Dependencies
  var AbstractClass = require("./AbstractData");
  var Utils = require("./../Utils");

  /**
   * @param {Object} params
   * @param {number} params.id
   * @param {string} params.workspace
   * @param {Sequelize.Model[]} params.layers
   */
  function RegisteredView(params) {
    AbstractClass.call(this, {'class': 'RegisteredView'});
    /**
     * Registered View identifier
     * @type {number}
     */
    this.id = params.id;
    /**
     * It defines a maps server workspace. (GeoServer)
     * @type {string} 
     */
    this.workspace = params.workspace;
    /**
     * It defines a base URI of layers retrieved from maps server handler
     * @type {string}
     */
    this.uri = params.uri;
    /**
     * It defines a list of layers registered
     * @type {string[]}
     */
    this.layers = [];
    /**
     * If defines a view data series dependency. It must be injected using RegisteredView#setDataSeries
     * @type {DataSeries}
     */
    this.dataSeries = null;

    // setting layers
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
   * It sets a data series used in views link.
   * 
   * @param {DataSeries} dataSeries - TerraMA² View Data Series
   */
  RegisteredView.prototype.setDataSeries = function(dataSeries) {
    this.dataSeries = dataSeries;
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
      layers: this.layers,
      dataSeries: this.dataSeries
    });
  };

  module.exports = RegisteredView;
} ());